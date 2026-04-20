import { TcpConnection, CanBusMessage, CanBusResponse } from './TcpConnection';
import { NodeBindingFile } from '../readers/BindingFileReader';

/**
 * State-machine based binding file writer
 * Based on src/DTBindingSoftware/src/BindingFileFSM.cpp
 */

// Event codes (from nodemess.h)
const FILE_EVENT_OPENED = 0x01;
const FILE_EVENT_CLOSED = 0x00;
const FILE_EVENT_ERASED = 0x03;

const BINDING_EVENT_HEADER_WRITTEN = 0x01;
const BINDING_EVENT_DATA_WRITTEN = 0x03;
const BINDING_EVENT_WRITE_DONE = 0x05;

// State machine states
enum State {
  IDLE,
  ERASING_FILE,
  OPENING_FILE,
  WRITING_HEADER,
  WRITING_DATA,
  WRITING_DONE,
  CLOSING_FILE,
  COMPLETE,
  ERROR
}

export class BindingWriterFSM {
  private connection: TcpConnection;
  
  // Message codes (from nodemess.h)
  private static readonly FC_NODEBINDINGSFILECONTROL = 0x98;  // 152 decimal
  private static readonly FC_NODEBINDINGENTRY = 0x99;           // 153 decimal
  private static readonly BROADCAST_UNIT = 0xFF;
  
  // File control commands (FC_NODEBINDINGSFILECONTROL)
  private static readonly FILE_CLOSE = 0x00;
  private static readonly FILE_OPEN = 0x01;
  // private static readonly FILE_INFO = 0x02;  // Unused - kept for reference
  private static readonly FILE_ERASE = 0x03;
  
  // Binding entry commands (FC_NODEBINDINGENTRY)
  private static readonly WRITE_HEADER = 0x01;
  private static readonly WRITE_DATA = 0x03;    // Not 0x02!
  private static readonly WRITE_DONE = 0x05;    // Not 0x03!
  
  // State machine variables
  private state: State = State.IDLE;
  private currentNodeAddress: number = 0;
  private currentBindings: string[] = [];
  private maxCharsPerChunk: number = 5;
  
  // Promise resolvers for async/await on responses
  private responseResolver: ((value: any) => void) | null = null;
  private responseRejecter: ((reason: any) => void) | null = null;
  private responseTimeout: NodeJS.Timeout | null = null;
  private readonly RESPONSE_TIMEOUT_MS = 10000; // Increased to 10 seconds for testing
  private taskPassword: string;
  private accessPassword: string;

  constructor(host: string, port: number, password: string = '', taskPassword: string = '1234', accessPassword: string = '2222') {
    this.connection = new TcpConnection(host, port, password);
    this.taskPassword = taskPassword;
    this.accessPassword = accessPassword;
    
    // Listen for file control responses
    this.connection.on('file-control-response', (response: CanBusResponse) => {
      if (response.nodeAddress === this.currentNodeAddress) {
        this.handleFileControlResponse(response);
      }
    });
    
    // Listen for binding entry responses
    this.connection.on('binding-entry-response', (response: CanBusResponse) => {
      if (response.nodeAddress === this.currentNodeAddress) {
        this.handleBindingEntryResponse(response);
      }
    });
    
    // Listen for binding entry errors (error codes 0x80+)
    this.connection.on('binding-entry-error', (error: any) => {
      if (error.nodeAddress === this.currentNodeAddress) {
        console.error(`❌ Node 0x${error.nodeAddress.toString(16).padStart(2, '0')} binding entry error: ${error.errorName}`);
        // Reject the current operation
        if (this.responseRejecter) {
          this.rejectPendingResponse(new Error(`Binding entry error: ${error.errorName} (0x${error.errorCode.toString(16)})`));
        }
      }
    });
    
    // Listen for node errors
    this.connection.on('node-error', (error: any) => {
      if (error.nodeAddress === this.currentNodeAddress) {
        console.error(`Node 0x${error.nodeAddress.toString(16).padStart(2, '0')} error: ${error.errorName}`);
        if (error.errorCode === 0x00) {
          console.error('⚠️  Node is refusing operation - may need to close existing file first or use different sequence');
        }
        // Don't reject here - let timeout handle it, but log for debugging
      }
    });
    
    // Handle disconnections
    this.connection.on('disconnected', () => {
      console.log('WARNING: Master node disconnected');
      this.rejectPendingResponse(new Error('Connection lost'));
    });
  }
  
  /**
   * Connect to the master node
   */
  async connect(): Promise<void> {
    return this.connection.connect();
  }
  
  /**
   * Set access level for configuration operations
   * Must be called before writing bindings
   */
  async setAccessLevel(): Promise<number> {
    console.log(`Setting access level (${this.accessPassword})...`);
    const level = await this.connection.setAccessLevel(this.accessPassword);
    const levelNames = ['No authority', 'User', 'Installer', 'Engineer'];
    console.log(`✓ Access level: ${levelNames[level] || 'Unknown'}`);
    return level;
  }
  
  /**
   * Enable programming mode on the node by setting APPL_UNLOCK flag
   * CRITICAL: This must be called BEFORE any binding file operations!
   * Without this, the node returns MESSAGE_ERROR_REFUSED_RUNNING_STATE (0x00)
   */
  async enableProgrammingMode(): Promise<void> {
    console.log(`Enabling programming mode on node 0x${this.currentNodeAddress.toString(16).toUpperCase().padStart(2, '0')}...`);
    
    // When using TCP/IP gateway, try setting APPL_UNLOCK on master node (0xFC) first
    try {
      console.log('Setting APPL_UNLOCK on master node (0xFC) first...');
      await this.connection.setNodeConfig(
        0xFC,  // master node
        0x00,  // nodeConfig flags
        true   // enableProgramming = set APPL_UNLOCK flag
      );
      console.log('✓ Master node APPL_UNLOCK set');
    } catch (err) {
      console.warn('⚠️  Master node config warning (may be normal):', err);
    }
    
    // Small delay
    await new Promise(resolve => setTimeout(resolve, 200));
    
    // Now set APPL_UNLOCK on target node
    await this.connection.setNodeConfig(
      this.currentNodeAddress,
      0x00,  // nodeConfig flags (use 0x00 if unknown)
      true   // enableProgramming = set APPL_UNLOCK flag
    );
    console.log('✓ Programming mode enabled (APPL_UNLOCK set)');
  }
  
  /**
   * Disable programming mode on the node by clearing APPL_UNLOCK flag
   * CRITICAL: This should be called AFTER binding file operations complete!
   * This returns the node to normal operating state
   */
  async disableProgrammingMode(): Promise<void> {
    console.log(`Disabling programming mode on node 0x${this.currentNodeAddress.toString(16).toUpperCase().padStart(2, '0')}...`);
    
    // Clear APPL_UNLOCK on target node
    try {
      await this.connection.setNodeConfig(
        this.currentNodeAddress,
        0x00,  // nodeConfig flags
        false  // enableProgramming = clear APPL_UNLOCK flag
      );
      console.log('✓ Programming mode disabled (APPL_UNLOCK cleared)');
    } catch (err) {
      console.warn('⚠️  Failed to disable programming mode:', err);
    }
    
    // Small delay
    await new Promise(resolve => setTimeout(resolve, 200));
    
    // Clear APPL_UNLOCK on master node (0xFC) as well
    try {
      console.log('Clearing APPL_UNLOCK on master node (0xFC)...');
      await this.connection.setNodeConfig(
        0xFC,  // master node
        0x00,  // nodeConfig flags
        false  // enableProgramming = clear APPL_UNLOCK flag
      );
      console.log('✓ Master node APPL_UNLOCK cleared');
    } catch (err) {
      console.warn('⚠️  Master node config warning (may be normal):', err);
    }
  }
  
  /**
   * Start configuration task on master node
   * Must be called before writing bindings
   */
  startConfigurationTask(): void {
    console.log('Starting UPDATE_DBASE task on master node...');
    this.connection.startTask(this.taskPassword);
  }
  
  /**
   * Disconnect from the master node
   */
  disconnect(): void {
    this.connection.disconnect();
  }
  
  /**
   * Get the underlying TCP connection for direct access
   * Useful for calling connection methods like setNodeConfig() before operations
   */
  getConnection(): TcpConnection {
    return this.connection;
  }
  
  /**
   * Handle file control response
   */
  private handleFileControlResponse(response: CanBusResponse): void {
    if (response.data.length === 0) return;
    
    const event = response.data[0];
    console.log(`File control event: 0x${event.toString(16).padStart(2, '0')}`);
    
    switch (this.state) {
      case State.ERASING_FILE:
        if (event === FILE_EVENT_ERASED) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`File erase failed: event=0x${event.toString(16)}`));
        }
        break;
        
      case State.OPENING_FILE:
        if (event === FILE_EVENT_OPENED) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`File open failed: event=0x${event.toString(16)}`));
        }
        break;
        
      case State.CLOSING_FILE:
        if (event === FILE_EVENT_CLOSED) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`File close failed: event=0x${event.toString(16)}`));
        }
        break;
    }
  }
  
  /**
   * Handle binding entry response
   */
  private handleBindingEntryResponse(response: CanBusResponse): void {
    if (response.data.length === 0) return;
    
    const event = response.data[0];
    console.log(`Binding entry event: 0x${event.toString(16).padStart(2, '0')}`);
    
    switch (this.state) {
      case State.WRITING_HEADER:
        if (event === BINDING_EVENT_HEADER_WRITTEN) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`Header write failed: event=0x${event.toString(16)}`));
        }
        break;
        
      case State.WRITING_DATA:
        if (event === BINDING_EVENT_DATA_WRITTEN) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`Data write failed: event=0x${event.toString(16)}`));
        }
        break;
        
      case State.WRITING_DONE:
        if (event === BINDING_EVENT_WRITE_DONE) {
          this.resolvePendingResponse(true);
        } else {
          this.rejectPendingResponse(new Error(`Write done failed: event=0x${event.toString(16)}`));
        }
        break;
    }
  }
  
  /**
   * Wait for a response with timeout
   */
  private waitForResponse(): Promise<any> {
    return new Promise((resolve, reject) => {
      this.responseResolver = resolve;
      this.responseRejecter = reject;
      
      this.responseTimeout = setTimeout(() => {
        this.responseResolver = null;
        this.responseRejecter = null;
        reject(new Error(`Response timeout in state ${State[this.state]}`));
      }, this.RESPONSE_TIMEOUT_MS);
    });
  }
  
  /**
   * Resolve pending response
   */
  private resolvePendingResponse(value: any): void {
    if (this.responseTimeout) {
      clearTimeout(this.responseTimeout);
      this.responseTimeout = null;
    }
    
    if (this.responseResolver) {
      this.responseResolver(value);
      this.responseResolver = null;
      this.responseRejecter = null;
    }
  }
  
  /**
   * Reject pending response
   */
  private rejectPendingResponse(reason: any): void {
    if (this.responseTimeout) {
      clearTimeout(this.responseTimeout);
      this.responseTimeout = null;
    }
    
    if (this.responseRejecter) {
      this.responseRejecter(reason);
      this.responseResolver = null;
      this.responseRejecter = null;
    }
  }
  
  /**
   * Send a message and wait for response
   */
  private async sendAndWait(message: CanBusMessage, newState: State): Promise<void> {
    this.state = newState;
    this.connection.sendCanBusMessage(message);
    await this.waitForResponse();
  }
  
  /**
   * Erase the binding file on the node
   * CRITICAL: This MUST be called before opening/writing bindings!
   * Without this, the node will reject with BINDING_CONFIG_WRONG_STATE (0x88)
   */
  private async eraseFile(): Promise<void> {
    console.log(`Erasing binding file on node 0x${this.currentNodeAddress.toString(16).padStart(2, '0')}`);
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriterFSM.FILE_ERASE]
    }, State.ERASING_FILE);
  }
  
  /**   * Open the binding file
   */
  private async openFile(): Promise<void> {
    console.log(`Opening binding file on node 0x${this.currentNodeAddress.toString(16).padStart(2, '0')}`);
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriterFSM.FILE_OPEN]
    }, State.OPENING_FILE);
  }
  
  /**
   * Close the binding file
   */
  private async closeFile(): Promise<void> {
    console.log(`Closing binding file on node 0x${this.currentNodeAddress.toString(16).padStart(2, '0')}`);
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriterFSM.FILE_CLOSE]
    }, State.CLOSING_FILE);
  }
  
  /**
   * Normalize binding string (based on BindingFileEntryOperations::normalize)
   */
  private normalizeBindingString(input: string): string {
    // Convert to uppercase
    let normalized = input.toUpperCase();
    
    // Fix timer types - must have uppercase 'T' and lowercase type letter
    normalized = normalized.replace(/TE/g, 'Te');
    normalized = normalized.replace(/TI/g, 'Ti');
    normalized = normalized.replace(/TO/g, 'To');
    normalized = normalized.replace(/TD/g, 'Td');
    normalized = normalized.replace(/TF/g, 'Tf');
    normalized = normalized.replace(/TR/g, 'Tr');
    
    // Ensure underscore at position 14 (between binding number and type)
    if (normalized.length > 14 && normalized[14] !== '_') {
      normalized = normalized.substring(0, 14) + '_' + normalized.substring(14);
    }
    
    return normalized;
  }
  
  /**
   * Write a binding header
   */
  private async writeHeader(bindingNumber: number, bindingString: string): Promise<void> {
    // Parse flags from binding string (position 7-8)
    const flagsStr = bindingString.substring(7, 9);
    const flags = parseInt(flagsStr, 16);
    
    console.log(`Writing header: binding #${bindingNumber}, flags=0x${flags.toString(16).padStart(2, '0')}`);
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGENTRY,
      data: [
        BindingWriterFSM.WRITE_HEADER,
        (bindingNumber >> 8) & 0xFF,  // MSB
        bindingNumber & 0xFF,          // LSB
        flags
      ]
    }, State.WRITING_HEADER);
  }
  
  /**
   * Write a chunk of binding data
   */
  private async writeDataChunk(data: string): Promise<void> {
    const chunk = data.substring(0, this.maxCharsPerChunk);
    
    // Convert each character to its ASCII byte value
    const bytes: number[] = [];
    for (let i = 0; i < chunk.length; i++) {
      bytes.push(chunk.charCodeAt(i));
    }
    
    console.log(`Writing ${bytes.length} bytes of data (chars: "${chunk}")`);
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGENTRY,
      data: [BindingWriterFSM.WRITE_DATA, ...bytes]
    }, State.WRITING_DATA);
    
    return;
  }
  
  /**
   * Signal that binding write is complete
   */
  private async writeDone(): Promise<void> {
    console.log('Signaling write done');
    
    await this.sendAndWait({
      nodeAddress: this.currentNodeAddress,
      unitAddress: BindingWriterFSM.BROADCAST_UNIT,
      messageCode: BindingWriterFSM.FC_NODEBINDINGENTRY,
      data: [BindingWriterFSM.WRITE_DONE]
    }, State.WRITING_DONE);
  }
  
  /**
   * Write a complete binding entry
   */
  private async writeBinding(bindingNumber: number, bindingString: string): Promise<void> {
    // Normalize the binding string
    const normalized = this.normalizeBindingString(bindingString);
    
    console.log(`\nWriting binding ${bindingNumber + 1}/${this.currentBindings.length}`);
    console.log(`Binding string: ${normalized.substring(0, 50)}...`);
    
    // Write header
    await this.writeHeader(bindingNumber, normalized);
    
    // Write data in chunks (starting from position 14)
    const dataToSend = normalized.substring(14);
    let pos = 0;
    
    while (pos < dataToSend.length) {
      const chunk = dataToSend.substring(pos, pos + this.maxCharsPerChunk);
      await this.writeDataChunk(chunk);
      pos += chunk.length;
    }
    
    // Signal done
    await this.writeDone();
  }
  
  /**
   * Write all bindings to a node
   */
  async writeBindingsToNode(nodeAddress: number, bindingFile: NodeBindingFile): Promise<void> {
    console.log(`\n${'='.repeat(60)}`);
    console.log(`Writing ${bindingFile.bindings.length} bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    console.log('='.repeat(60));
    
    this.currentNodeAddress = nodeAddress;
    this.currentBindings = bindingFile.bindings;
    
    try {
      this.state = State.IDLE;
      
      // 1. Set access level (required for configuration operations)
      await this.setAccessLevel();
      
      // Wait a moment for access level to be fully applied
      await new Promise(resolve => setTimeout(resolve, 200));
      
      // 2. Enable programming mode (set APPL_UNLOCK flag)
      // CRITICAL: Without this, you get MESSAGE_ERROR_REFUSED_RUNNING_STATE (0x00)
      await this.enableProgrammingMode();
      
      // Wait a moment for programming mode to be fully applied
      await new Promise(resolve => setTimeout(resolve, 200));
      
      // 3. CLOSE file first (if it's already open from previous operation)
      // Ignore errors - file might already be closed
      try {
        await this.closeFile();
        await new Promise(resolve => setTimeout(resolve, 100));
      } catch (err) {
        console.log('Note: File was not open (this is normal)');
      }
      
      // 4. ERASE the binding file
      // CRITICAL: C++ software ALWAYS erases before writing (see BindingsReadWriteFSM.cpp)
      // Without this, node rejects with BINDING_CONFIG_WRONG_STATE (0x88)
      // File must be CLOSED before erase or we get SHARE_VIOLATION (0x82)
      await this.eraseFile();
      
      // 5. Open the file for writing
      await this.openFile();
      
      // 6. Write each binding
      for (let i = 0; i < this.currentBindings.length; i++) {
        await this.writeBinding(i, this.currentBindings[i]);
      }
      
      // 7. Close the file
      await this.closeFile();
      
      this.state = State.COMPLETE;
      console.log(`\n✓ Successfully wrote ${this.currentBindings.length} bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
      
    } catch (error) {
      this.state = State.ERROR;
      console.error(`\n✗ Error writing bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}:`, error);
      throw error;
    } finally {
      // 8. Always disable programming mode (clear APPL_UNLOCK)
      // This returns the node to normal operating state
      try {
        await this.disableProgrammingMode();
      } catch (err) {
        console.warn('⚠️  Warning: Could not disable programming mode cleanly:', err);
      }
    }
  }
}
