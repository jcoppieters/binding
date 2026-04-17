import { TcpConnection, CanBusMessage } from './TcpConnection';
import { NodeBindingFile } from '../readers/BindingFileReader';

/**
 * Write binding files to Duotecno nodes via TCP/IP
 * Based on src/DTBindingSoftware/src/BindingFileFSM.cpp
 */

export class BindingWriter {
  private connection: TcpConnection;
  
  // Message codes (from src/DTBindingSoftware/src/nodemess.h)
  private static readonly FC_NODEBINDINGSFILECONTROL = 0x98;  // 152 decimal
  private static readonly FC_NODEBINDINGENTRY = 0x99;           // 153 decimal
  private static readonly BROADCAST_UNIT = 0xFF;
  
  // File control commands
  private static readonly FILE_OPEN = 0x01;
  private static readonly FILE_CLOSE = 0x02;
  private static readonly FILE_ERASE = 0x03;
  // private static readonly FILE_INFO = 0x04;  // Reserved for future use
  
  // Binding entry commands
  private static readonly WRITE_HEADER = 0x01;
  private static readonly WRITE_DATA = 0x02;
  private static readonly WRITE_DONE = 0x03;
  // private static readonly READ_HEADER = 0x04;  // Reserved for future use
  // private static readonly READ_DATA = 0x05;    // Reserved for future use
  
  constructor(host: string, port: number, password: string = '') {
    this.connection = new TcpConnection(host, port, password);
    
    // Log received messages for debugging
    this.connection.on('message', (bytes: number[]) => {
      console.log('RX:', this.connection.formatMessage(bytes));
    });
    
    // Log disconnections
    this.connection.on('disconnected', () => {
      console.log('WARNING: Master node disconnected');
    });
  }
  
  /**
   * Connect to the master node
   */
  async connect(): Promise<void> {
    return this.connection.connect();
  }
  
  /**
   * Disconnect from the master node
   */
  disconnect(): void {
    this.connection.disconnect();
  }
  
  /**
   * Open the binding file on a node
   */
  private async openBindingFile(nodeAddress: number): Promise<void> {
    console.log(`Opening binding file on node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    
    const message: CanBusMessage = {
      nodeAddress,
      unitAddress: BindingWriter.BROADCAST_UNIT,
      messageCode: BindingWriter.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriter.FILE_OPEN]
    };
    
    this.connection.sendCanBusMessage(message);
    
    // Wait for the node to process
    await this.delay(500);
  }
  
  /**
   * Close the binding file on a node
   */
  private async closeBindingFile(nodeAddress: number): Promise<void> {
    console.log(`Closing binding file on node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    
    const message: CanBusMessage = {
      nodeAddress,
      unitAddress: BindingWriter.BROADCAST_UNIT,
      messageCode: BindingWriter.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriter.FILE_CLOSE]
    };
    
    this.connection.sendCanBusMessage(message);
    
    // Wait for the node to process
    await this.delay(500);
  }
  
  /**
   * Erase the binding file on a node
   */
  private async eraseBindingFile(nodeAddress: number): Promise<void> {
    console.log(`Erasing binding file on node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    
    const message: CanBusMessage = {
      nodeAddress,
      unitAddress: BindingWriter.BROADCAST_UNIT,
      messageCode: BindingWriter.FC_NODEBINDINGSFILECONTROL,
      data: [BindingWriter.FILE_ERASE]
    };
    
    this.connection.sendCanBusMessage(message);
    
    // Wait for the erase operation (can take a few seconds)
    await this.delay(2000);
  }
  
  /**
   * Write a binding header (binding number and flags)
   */
  private async writeBindingHeader(nodeAddress: number, bindingNumber: number, flags: number): Promise<void> {
    console.log(`Writing binding header: #${bindingNumber}, flags=0x${flags.toString(16).padStart(2, '0')}`);
    
    const message: CanBusMessage = {
      nodeAddress,
      unitAddress: BindingWriter.BROADCAST_UNIT,
      messageCode: BindingWriter.FC_NODEBINDINGENTRY,
      data: [
        BindingWriter.WRITE_HEADER,
        (bindingNumber >> 8) & 0xFF,  // MSB of binding number
        bindingNumber & 0xFF,          // LSB of binding number
        flags
      ]
    };
    
    this.connection.sendCanBusMessage(message);
    
    // Wait for the node to process
    await this.delay(300);
  }
  
  /**
   * Write binding data (the actual binding string)
   * Converts each character to hex (based on WriteData in TXNodeMessage.cpp)
   */
  private async writeBindingData(nodeAddress: number, bindingString: string): Promise<void> {
    console.log(`Writing ${bindingString.length} characters`);
    
    // C++ code chunks the string into 5-character pieces (maxPacketLength-1 where maxPacketLength=6)
    // Then converts each character to its ASCII byte value
    const maxCharsPerChunk = 5;
    
    for (let offset = 0; offset < bindingString.length; offset += maxCharsPerChunk) {
      // Get chunk of original string
      const endOffset = Math.min(offset + maxCharsPerChunk, bindingString.length);
      const stringChunk = bindingString.substring(offset, endOffset);
      
      // Convert each character in this chunk to its ASCII byte value
      const bytes: number[] = [];
      for (let i = 0; i < stringChunk.length; i++) {
        bytes.push(stringChunk.charCodeAt(i));
      }
      
      const message: CanBusMessage = {
        nodeAddress,
        unitAddress: BindingWriter.BROADCAST_UNIT,
        messageCode: BindingWriter.FC_NODEBINDINGENTRY,
        data: [BindingWriter.WRITE_DATA, ...bytes]
      };
      
      this.connection.sendCanBusMessage(message);
      
      // Wait between chunks
      await this.delay(200);
    }
  }
  
  /**
   * Signal that binding write is done
   */
  private async writeBindingDone(nodeAddress: number): Promise<void> {
    const message: CanBusMessage = {
      nodeAddress,
      unitAddress: BindingWriter.BROADCAST_UNIT,
      messageCode: BindingWriter.FC_NODEBINDINGENTRY,
      data: [BindingWriter.WRITE_DONE]
    };
    
    this.connection.sendCanBusMessage(message);
    
    // Wait for the node to process
    await this.delay(200);
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
   * Write a complete binding entry
   */
  private async writeBinding(nodeAddress: number, bindingNumber: number, bindingString: string): Promise<void> {
    // Normalize the binding string first
    const normalized = this.normalizeBindingString(bindingString);
    
    console.log(`Normalized binding: ${normalized.substring(0, 50)}...`);
    
    // Parse binding string to extract flags
    // Format: NODEADDR_FLAGS_BINDNUM_TYPE...
    // Position 7-8: flags (2 hex chars)
    const flagsStr = normalized.substring(7, 9);
    const flags = parseInt(flagsStr, 16);
    
    console.log(`Flags: 0x${flagsStr} (${flags})`);
    
    // Write header
    await this.writeBindingHeader(nodeAddress, bindingNumber, flags);
    
    // Write data (from position 14 onwards, like the C++ code does)
    // The first 14 characters (LLLLLL_NN_BBBB) are already in the header
    const dataToSend = normalized.substring(14);
    console.log(`Data portion: ${dataToSend.substring(0, 30)}...`);
    await this.writeBindingData(nodeAddress, dataToSend);
    
    // Signal done
    await this.writeBindingDone(nodeAddress);
  }
  
  /**
   * Write all bindings from a binding file to a node
   */
  async writeBindingsToNode(nodeAddress: number, bindingFile: NodeBindingFile): Promise<void> {
    console.log(`\nWriting bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    console.log(`Total bindings: ${bindingFile.bindings.length}`);
    
    try {
      // 1. Erase the existing binding file
      await this.eraseBindingFile(nodeAddress);
      
      // 2. Open the binding file
      await this.openBindingFile(nodeAddress);
      
      // 3. Write each binding
      for (let i = 0; i < bindingFile.bindings.length; i++) {
        const binding = bindingFile.bindings[i];
        console.log(`\nWriting binding ${i + 1}/${bindingFile.bindings.length}`);
        await this.writeBinding(nodeAddress, i, binding);
      }
      
      // 4. Close the binding file
      await this.closeBindingFile(nodeAddress);
      
      console.log(`\nSuccessfully wrote ${bindingFile.bindings.length} bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
    } catch (error) {
      console.error(`Error writing bindings to node 0x${nodeAddress.toString(16).padStart(2, '0')}:`, error);
      throw error;
    }
  }
  
  /**
   * Helper to delay execution
   */
  private delay(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
}
