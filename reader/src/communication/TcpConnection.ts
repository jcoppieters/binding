import { Socket } from 'net';
import { EventEmitter } from 'events';

/**
 * Message format for TCP/IP communication with Duotecno master node
 * Format: [value1,value2,value3,...]\n
 * 
 * Based on src/DTBindingSoftware/src/communication/tcpsocket/
 */

export interface TcpMessage {
  bytes: number[];
}

export interface CanBusMessage {
  nodeAddress: number;
  unitAddress: number;
  messageCode: number;
  data: number[];
}

export interface CanBusResponse {
  nodeAddress: number;
  unitAddress: number;
  messageCode: number;
  data: number[];
}

export class TcpConnection extends EventEmitter {
  private socket: Socket | null = null;
  private host: string;
  private port: number;
  private password: string;
  private connected: boolean = false;
  private buffer: string = '';
  
  // Protocol constants (from tcpsocketmessages_def.h)
  private static readonly FC_PROTOCOLMSG = 245; // 0xF5 (TX - outgoing messages)
  private static readonly EV_PROTOCOLMSG = 122; // 0x7A (RX - incoming messages)
  private static readonly OBJECT_CANBUS_MSG = 0x01;
  private static readonly FC_CLIENTCONNECTSET = 214;
  private static readonly OBJECT_PASSWORD = 3;
  private static readonly ATTR_SUBSCRIBE_CANBUS_MSG = 0x02;
  private static readonly FC_APPLICATIONTASKSET = 212;
  private static readonly FC_ACCESSLEVELSET = 222;
  private static readonly EV_ACCESSLEVELINFO = 75;
  
  // Node configuration (from nodemess.h)
  private static readonly FC_NODECONFIG = 0x94; // 148 decimal
  private static readonly EV_NODECONFIG = 0x0C; // 12 decimal
  
  // DLL Access Control flags (from nodegenestructs.h)
  private static readonly APPL_SERIALIZE = 0x01;
  private static readonly APPL_UNLOCK = 0x02;
  
  // Access level methods
  private static readonly ATTR_CODE = 0;
  private static readonly OBJECT_CHANGECODE = 1;
  private static readonly OBJECT_RESET = 2;
  
  // Task types (from tcpsockethomeclientcmn.h)
  private static readonly TASK_BROWSE = 0;
  private static readonly TASK_BROWSE_CLEAR = 1;
  private static readonly TASK_PNP = 2;
  private static readonly TASK_UPDATE_DBASE = 3;
  
  // APDU Header constants (from apdu_def.h)
  private static readonly APDU_BASIC_PACKET = 0x40;
  private static readonly APDU_EXPLICIT_INVOKE = 0x28;
  private static readonly APDU_IMPLICIT_INVOKE = 0x20;
  
  // Event message codes (from nodemess.h)
  private static readonly EV_NODEBINDINGSFILECONTROL = 0x13;
  private static readonly EV_NODEBINDINGENTRY = 0x10;
  private static readonly EV_MESSAGEERROR = 0x11;
  
  constructor(host: string, port: number, password: string = '') {
    super();
    this.host = host;
    this.port = port;
    this.password = password;
  }
  
  /**
   * Format a message according to the Duotecno TCP protocol
   * Format: [byte1,byte2,byte3,...]\n
   */
  public formatMessage(bytes: number[]): string {
    return `[${bytes.join(',')}]\n`;
  }
  
  /**
   * Parse a received message
   * Format: [byte1,byte2,byte3,...]\n
   */
  private parseMessage(data: string): number[] | null {
    const match = data.match(/\[([^\]]+)\]/);
    if (!match) return null;
    
    const parts = match[1].split(',');
    return parts.map(part => parseInt(part.trim(), 10));
  }
  
  /**
   * Connect to the master node
   */
  async connect(): Promise<void> {
    return new Promise((resolve, reject) => {
      this.socket = new Socket();
      
      // Set a connection timeout
      const connectionTimeout = setTimeout(() => {
        if (!this.connected) {
          reject(new Error('Connection timeout'));
        }
      }, 10000);
      
      this.socket.on('connect', () => {
        console.log(`Connected to ${this.host}:${this.port}`);
        
        // Send password if provided
        if (this.password) {
          this.sendPassword(this.password);
        } else {
          // No password needed, we're connected
          clearTimeout(connectionTimeout);
          this.connected = true;
          
          // Subscribe to CAN bus messages
          console.log('Subscribing to CAN bus messages...');
          this.subscribeToCanBus();
          
          this.emit('connected');
          resolve();
        }
      });
      
      this.socket.on('data', (data) => {
        this.handleData(data);
      });
      
      this.socket.on('error', (err) => {
        clearTimeout(connectionTimeout);
        console.error('Socket error:', err);
        this.emit('error', err);
        if (!this.connected) {
          reject(err);
        }
      });
      
      this.socket.on('close', (hadError) => {
        console.log(`Connection closed${hadError ? ' with error' : ''}`);
        this.connected = false;
        this.emit('disconnected');
      });
      
      // Handle password authentication response
      this.once('authenticated', () => {
        clearTimeout(connectionTimeout);
        this.connected = true;
        
        // Subscribe to CAN bus messages
        console.log('Subscribing to CAN bus messages...');
        this.subscribeToCanBus();
        
        this.emit('connected');
        resolve();
      });
      
      // Handle authentication failure
      this.once('auth-failed', (err) => {
        clearTimeout(connectionTimeout);
        reject(err);
      });
      
      this.socket.connect(this.port, this.host);
    });
  }
  
  /**
   * Send password for authentication
   */
  private sendPassword(password: string): void {
    // Convert password to ASCII bytes
    const passwordBytes: number[] = [];
    for (let i = 0; i < password.length; i++) {
      passwordBytes.push(password.charCodeAt(i));
    }
    
    // Message format: [FC_CLIENTCONNECTSET, OBJECT_PASSWORD, length, ...password_chars...]
    const message = [
      TcpConnection.FC_CLIENTCONNECTSET,
      TcpConnection.OBJECT_PASSWORD,
      password.length,  // CRITICAL: Must include password length!
      ...passwordBytes
    ];
    
    console.log('TX Login:', this.formatMessage(message));
    this.sendRaw(message);
  }
  
  /**
   * Subscribe to CAN bus messages
   * Required to receive responses from nodes
   */
  private subscribeToCanBus(): void {
    const message = [
      TcpConnection.FC_PROTOCOLMSG,
      TcpConnection.ATTR_SUBSCRIBE_CANBUS_MSG,
      1  // 1 = subscribe, 0 = unsubscribe
    ];
    
    console.log('TX Subscribe:', message);
    this.sendRaw(message);
  }
  
  /**
   * Start a task on the master node (e.g., UPDATE_DBASE for configuration mode)
   * @param taskPassword - 4-digit numeric password for task authorization
   * @param taskType - Task type (default: TASK_UPDATE_DBASE)
   */
  startTask(taskPassword: string = '1234', taskType: number = TcpConnection.TASK_UPDATE_DBASE): void {
    if (taskPassword.length !== 4 || !/^\d{4}$/.test(taskPassword)) {
      throw new Error('Task password must be exactly 4 digits');
    }
    
    // Convert password digits to ASCII bytes
    const passwordBytes = [
      taskPassword.charCodeAt(0),
      taskPassword.charCodeAt(1),
      taskPassword.charCodeAt(2),
      taskPassword.charCodeAt(3)
    ];
    
    const message = [
      TcpConnection.FC_APPLICATIONTASKSET,
      taskType,
      ...passwordBytes
    ];
    
    console.log(`TX Start Task ${taskType}: [${message.join(',')}]`);
    this.sendRaw(message);
  }
  
  /**
   * Set access level by sending password
   * @param accessPassword - 4-digit password ('1111' for User, '2222' for Installer)
   * @returns Promise that resolves when access level is granted
   */
  async setAccessLevel(accessPassword: string = '2222'): Promise<number> {
    if (accessPassword.length !== 4 || !/^\d{4}$/.test(accessPassword)) {
      throw new Error('Access password must be exactly 4 digits');
    }
    
    return new Promise((resolve, reject) => {
      // Listen for access level response
      const timeout = setTimeout(() => {
        this.off('access-level-granted', handleResponse);
        reject(new Error('Access level request timeout'));
      }, 5000);
      
      const handleResponse = (accessLevel: number) => {
        clearTimeout(timeout);
        resolve(accessLevel);
      };
      
      this.once('access-level-granted', handleResponse);
      
      // Convert password digits to ASCII bytes
      const passwordBytes = [
        accessPassword.charCodeAt(0),
        accessPassword.charCodeAt(1),
        accessPassword.charCodeAt(2),
        accessPassword.charCodeAt(3)
      ];
      
      const message = [
        TcpConnection.FC_ACCESSLEVELSET,
        TcpConnection.ATTR_CODE,  // Method: check access level
        ...passwordBytes
      ];
      
      console.log(`TX Set Access Level (${accessPassword}): [${message.join(',')}]`);
      this.sendRaw(message);
    });
  }
  
  /**
   * Set node configuration flags (CRITICAL: Must be called before binding operations!)
   * This enables "programming mode" by setting the APPL_UNLOCK flag
   * 
   * @param nodeAddress - Node logical address (e.g., 0xFC for gateway)
   * @param nodeConfig - Node configuration flags (usually current bNodeFlags, e.g., 0x00)
   * @param enableProgramming - If true, sets APPL_UNLOCK to allow binding operations
   * @returns Promise that resolves when node config is set
   * 
   * From C++: SetNodeConfig(bNodeAddress, bNodeConfig, bDllAccessControl)
   * - FC_NODECONFIG = 0x94 (148)
   * - APPL_UNLOCK = 0x02 (enables programming/binding mode)
   * - APPL_SERIALIZE = 0x01 (optional serialization flag)
   */
  async setNodeConfig(
    nodeAddress: number,
    nodeConfig: number = 0x00,
    enableProgramming: boolean = true
  ): Promise<void> {
    return new Promise((resolve, reject) => {
      // Build access control flags
      let dllAccessControl = 0x00;
      if (enableProgramming) {
        dllAccessControl |= TcpConnection.APPL_UNLOCK;
      }
      
      console.log(`TX SetNodeConfig: Node 0x${nodeAddress.toString(16).toUpperCase().padStart(2, '0')}, ` +
                  `Config=0x${nodeConfig.toString(16).toUpperCase().padStart(2, '0')}, ` +
                  `DllAccess=0x${dllAccessControl.toString(16).toUpperCase().padStart(2, '0')} ` +
                  `(UNLOCK=${enableProgramming})`);
      
      // Set up response handler
      const timeout = setTimeout(() => {
        this.removeListener('node-config-response', handleResponse);
        this.removeListener('node-error', handleError);
        console.warn('⚠️  FC_NODECONFIG: No response after 5s (might be OK if node is busy)');
        resolve(); // Still resolve - non-critical
      }, 5000);
      
      const handleResponse = (response: CanBusResponse) => {
        if (response.nodeAddress === nodeAddress) {
          clearTimeout(timeout);
          this.removeListener('node-error', handleError);
          console.log(`✓ Node 0x${nodeAddress.toString(16).toUpperCase().padStart(2, '0')} config acknowledged`);
          resolve();
        }
      };
      
      const handleError = (error: any) => {
        if (error.nodeAddress === nodeAddress || error.nodeAddress === 0xFC) {
          clearTimeout(timeout);
          this.removeListener('node-config-response', handleResponse);
          console.error(`❌ FC_NODECONFIG failed: ${error.errorName}`);
          reject(new Error(`FC_NODECONFIG failed: ${error.errorName}`));
        }
      };
      
      this.once('node-config-response', handleResponse);
      this.on('node-error', handleError);
      
      // FC_NODECONFIG requires explicit invoke via CAN bus message
      // Message format from C++:
      // MessageTransfer(nodeAddress, BROADCAST_UNIT, FC_NODECONFIG, 2, [nodeConfig, dllAccessControl])
      this.sendCanBusMessage({
        nodeAddress: nodeAddress,
        unitAddress: 0xFF,  // BROADCAST_UNIT
        messageCode: TcpConnection.FC_NODECONFIG,
        data: [nodeConfig, dllAccessControl]
      });
    });
  }
  
  /**
   * Handle incoming data
   */
  private handleData(data: Buffer): void {
    this.buffer += data.toString();
    
    // Process complete messages (terminated with ]\n)
    let endIndex: number;
    while ((endIndex = this.buffer.indexOf(']\n')) !== -1) {
      const message = this.buffer.substring(0, endIndex + 2);
      this.buffer = this.buffer.substring(endIndex + 2);
      
      const bytes = this.parseMessage(message);
      if (bytes) {
        this.handleMessage(bytes);
      }
    }
  }
  
  /**
   * Handle a parsed message
   */
  private handleMessage(bytes: number[]): void {
    console.log('RX:', this.formatMessage(bytes));
    
    // Check for authentication response (EV_CLIENTCONNECTSTATUS = 67)
    if (bytes.length > 0 && bytes[0] === 67) {
      // Response format: [67, object_type, status, ...]
      // object_type: 3 = OBJECT_PASSWORD
      // status: 1 = success, 0 = failure
      const success = bytes.length >= 3 && bytes[2] === 1;
      
      if (success) {
        console.log('Authentication successful!');
        this.emit('authenticated');
      } else {
        console.error('Authentication failed - wrong password');
        this.emit('auth-failed', new Error('Authentication failed - wrong password'));
      }
      return;
    }
    
    // Check for access level response (EV_ACCESSLEVELINFO = 75)
    if (bytes.length > 0 && bytes[0] === TcpConnection.EV_ACCESSLEVELINFO) {
      // Response format: [75, method, access_level, ...password_echo]
      // access_level: 0=No authority, 1=User, 2=Installer, 3=Engineer
      const method = bytes[1];
      const accessLevel = bytes[2];
      
      const accessLevelNames = ['No authority', 'User', 'Installer', 'Engineer'];
      console.log(`Access level granted: ${accessLevel} (${accessLevelNames[accessLevel] || 'Unknown'})`);
      
      this.emit('access-level-granted', accessLevel);
      return;
    }
    
    // Parse CAN bus message responses
    // Format: [PROTOCOLMSG, OBJECT_CANBUS_MSG, nodeAddr, unitAddr, srcUnit, reserved, apduHeader, msgCode, length, ...data]
    // Note: Received messages use EV_PROTOCOLMSG (122), sent messages use FC_PROTOCOLMSG (245)
    if (bytes.length >= 9 && 
        (bytes[0] === TcpConnection.EV_PROTOCOLMSG || bytes[0] === TcpConnection.FC_PROTOCOLMSG) &&
        bytes[1] === TcpConnection.OBJECT_CANBUS_MSG) {
      
      const response: CanBusResponse = {
        nodeAddress: bytes[2],
        unitAddress: bytes[3],
        messageCode: bytes[7],
        data: bytes.slice(9, 9 + bytes[8])
      };
      
      console.log(`CAN Response: Node=0x${response.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}, MsgCode=0x${response.messageCode.toString(16).toUpperCase().padStart(2, '0')}, Data=[${response.data.join(',')}]`);
      
      // Emit specific events based on message code
      if (response.messageCode === TcpConnection.EV_MESSAGEERROR) {
        // Error message format: [error_code, node_status]
        const errorCode = response.data[0] || 0;
        const errorNames: {[key: number]: string} = {
          0x00: 'REFUSED_RUNNING_STATE',
          0x01: 'INVALID',
          0x02: 'BUFFER_FULL',
          0x03: 'BINDING_NOT_FOUND',
          0x04: 'REFERENCE_NOT_FOUND',
          0x05: 'BINDING_TYPE_INVALID',
          0x06: 'MESSAGE_TRANSFER_TIMEOUT',
          0x07: 'DLL_ACK_TIMEOUT',
          0x08: 'TRANSCEIVER_CHIP',
          0x09: 'UNIT_NR_INVALID',
          0x0A: 'BINDING_CONFIG_REFUSED',
          0x0B: 'WRONG_OBJECT_MEMBER',
          0x0C: 'INVALID_MESSAGE_CODE',
          0x0D: 'WRONG_DIRECT_CALL',
          0x0E: 'REFUSED_WRONG_DATA',
          0x0F: 'REFUSED_BINDING_STATE',
          0x10: 'CAN_RX_PROTOCOL_TIMEOUT',
          0x11: 'CAN_TX_PROTOCOL_TIMEOUT',
          0x12: 'ONLY_FOR_MASTER_LCD',
          0x13: 'NO_FREE_MTC_MAX_RETRIES',
          0x14: 'BINDING_SYNTAX_ERROR'
        };
        const errorName = errorNames[errorCode] || `UNKNOWN(${errorCode})`;
        console.error(`❌ Node error: ${errorName}`);
        this.emit('node-error', { nodeAddress: response.nodeAddress, errorCode, errorName });
      } else if (response.messageCode === TcpConnection.EV_NODECONFIG) {
        // EV_NODECONFIG response: [nodeType, numUnits, nodeFlags, dllAccessControl]
        console.log(`✓ Node config response: Type=${response.data[0]}, Units=${response.data[1]}, Flags=0x${response.data[2]?.toString(16) || '0'}, DllAccess=0x${response.data[3]?.toString(16) || '0'}`);
        this.emit('node-config-response', response);
      } else if (response.messageCode === TcpConnection.EV_NODEBINDINGSFILECONTROL) {
        this.emit('file-control-response', response);
      } else if (response.messageCode === TcpConnection.EV_NODEBINDINGENTRY) {
        // Binding entry responses - also used for errors!
        // Error codes 0x80+ are binding entry errors (e.g., 0x88 = BINDING_CONFIG_WRONG_STATE)
        if (response.data.length > 0 && response.data[0] >= 0x80) {
          const errorCode = response.data[0];
          const bindingErrorNames: {[key: number]: string} = {
            0x80: 'WRONG_BINDING_ENTRY_NR',
            0x81: 'WRITE_ERROR_WRONG_STATE',
            0x82: 'READ_ERROR_WRONG_STATE',
            0x83: 'BINDING_FILE_FULL',
            0x84: 'BINDING_ENTRY_ERROR',
            0x85: 'BINDING_FILE_CLOSED',
            0x86: 'BINDING_FILE_EMPTY',
            0x87: 'BINDING_FILE_CORRUPT',
            0x88: 'BINDING_CONFIG_WRONG_STATE',
            0x89: 'BINDING_TEMP_BUFFER_FULL'
          };
          const errorName = bindingErrorNames[errorCode] || `UNKNOWN(0x${errorCode.toString(16)})`;
          console.error(`❌ Binding entry error: ${errorName}`);
          this.emit('binding-entry-error', { nodeAddress: response.nodeAddress, errorCode, errorName });
        } else {
          this.emit('binding-entry-response', response);
        }
      }
      
      // Also emit generic response event
      this.emit('canbus-response', response);
      return;
    }
    
    // Check for subscribe confirmation (EV_PROTOCOLMSG with ATTR_SUBSCRIBE_CANBUS_MSG)
    // Format: [EV_PROTOCOLMSG, ATTR_SUBSCRIBE_CANBUS_MSG, status]
    if (bytes.length >= 3 && 
        bytes[0] === TcpConnection.EV_PROTOCOLMSG &&
        bytes[1] === TcpConnection.ATTR_SUBSCRIBE_CANBUS_MSG) {
      const status = bytes[2] === 1;
      console.log(`CAN bus subscription ${status ? 'confirmed' : 'failed'}`);
      this.emit('canbus-subscribed', status);
      return;
    }
    
    // Log unhandled message types for debugging
    console.log(`Unhandled message type: ${bytes[0]} (0x${bytes[0]?.toString(16)}), Length: ${bytes.length}, Data: [${bytes.slice(0, 10).join(',')}${bytes.length > 10 ? '...' : ''}]`);
    this.emit('message', bytes);
  }
  
  /**
   * Send a raw message (array of bytes)
   */
  private sendRaw(bytes: number[]): void {
    if (!this.socket) {
      throw new Error('Not connected');
    }
    
    const message = this.formatMessage(bytes);
    console.log('TX:', message.trim());
    this.socket.write(message);
  }
  
  /**
   * Send a CAN bus message through the TCP/IP gateway
   * This wraps a CAN message in the TCP protocol
   */
  sendCanBusMessage(msg: CanBusMessage): void {
    if (!this.connected) {
      throw new Error('Not connected');
    }
    
    // Calculate APDU header based on message characteristics
    // (from getADPUHeader in tcpsocketmsgapduheader.cpp)
    let apduHeader = 0;
    
    // Add BASIC_PACKET flag if length <= 6
    if (msg.data.length <= 6) {
      apduHeader += TcpConnection.APDU_BASIC_PACKET;
    }
    
    // Add EXPLICIT_INVOKE for addressed messages, IMPLICIT_INVOKE for broadcast
    if (msg.nodeAddress === 0xFF) {
      apduHeader += TcpConnection.APDU_IMPLICIT_INVOKE;
    } else {
      apduHeader += TcpConnection.APDU_EXPLICIT_INVOKE;
    }
    
    const message = [
      TcpConnection.FC_PROTOCOLMSG,      // 245 (0xF5)
      TcpConnection.OBJECT_CANBUS_MSG,   // 0x01
      msg.nodeAddress,                    // Target node address
      msg.unitAddress,                    // Unit address (0xFF for broadcast)
      0xFF,                               // Source unit (always 0xFF)
      0xFF,                               // Reserved
      apduHeader,                         // APDU Header (calculated above)
      msg.messageCode,                    // CAN message code
      msg.data.length,                    // Data length
      ...msg.data                         // Data bytes
    ];
    
    this.sendRaw(message);
  }
  
  /**
   * Close the connection
   */
  disconnect(): void {
    if (this.socket) {
      this.socket.destroy();
      this.socket = null;
      this.connected = false;
    }
  }
  
  /**
   * Check if connected
   */
  isConnected(): boolean {
    return this.connected;
  }
}
