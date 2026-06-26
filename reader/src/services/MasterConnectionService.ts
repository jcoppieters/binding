/**
 * Singleton Master Connection Service
 * Manages the ONE server-side TCP connection to the Duotecno master
 * Implements discovery process and in-memory node/unit database
 */

import { EventEmitter } from 'events';
import { Socket } from 'net';
import { DuotecnoProtocol, Rec, Cmd, CommRecord, Message } from '../communication/DuotecnoProtocol.js';
import { UnitType } from '../models/binding.js';

/**
 * Connection status
 */
export enum ConnectionStatus {
  Disconnected = 'disconnected',
  Connecting = 'connecting',
  LoggedIn = 'logged-in',
  Discovering = 'discovering',
  Ready = 'ready',
  Error = 'error'
}

/**
 * Login result
 */
export type LoginResult = 
  | { success: true; status: 'logged-in' }
  | { success: false; status: 'not-found' | 'wrong-password' | 'timeout' | 'error'; message: string };

/**
 * Discovered node in database
 */
export interface DiscoveredNode {
  nodeAddress: number;
  name: string;
  physicalAddress: number;
  type: number;
  nrUnits: number;
  flags: number;
  units: DiscoveredUnit[];
}

/**
 * Discovered unit in database
 */
export interface DiscoveredUnit {
  nodeAddress: number;
  unitAddress: number;
  name: string;
  type: UnitType;
  status?: number;
  value?: number;
  preset?: number;
  temperature?: {
    temp: number;
    preset: number;
    sun: number;
    hsun: number;
    moon: number;
    hmoon: number;
  };
}

/**
 * Singleton Master Connection Service
 */
export class MasterConnectionService extends EventEmitter {
  private static instance: MasterConnectionService;
  
  private socket: Socket | null = null;
  private status: ConnectionStatus = ConnectionStatus.Disconnected;
  private buffer: string = '';
  
  // Connection config (stored for reference, may be used for reconnection logic later)
  // private _host: string = '';
  // private _port: number = 0;
  private password: string = '';
  
  // In-memory database
  private nodes: Map<number, DiscoveredNode> = new Map();
  
  // Discovery state
  private expectedNodes: number = 0;
  private discoveredNodeCount: number = 0;
  private pendingUnitCount: number = 0;
  private currentNodeIndex: number = 0;
  private currentUnitIndex: number = 0;
  private currentNodeAddress: number = 0;
  private currentNodeUnits: number = 0;
  
  // Command queue and response tracking
  private commandQueue: Message[] = [];
  private waitingForResponse: boolean = false;
  private responseTimeout: NodeJS.Timeout | null = null;
  private readonly RESPONSE_TIMEOUT_MS = 5000; // 5 seconds
  
  // Heartbeat
  private heartbeatInterval: NodeJS.Timeout | null = null;
  private readonly HEARTBEAT_INTERVAL_MS = 30000; // 30 seconds
  
  // Pending command responses
  private pendingCommands: Map<string, {
    resolve: (status: DiscoveredUnit) => void;
    reject: (error: Error) => void;
    timeout: NodeJS.Timeout;
    expectedValue?: number;
    expectedStatus?: number;
  }> = new Map();
  
  private constructor() {
    super();
  }
  
  /**
   * Get singleton instance
   */
  static getInstance(): MasterConnectionService {
    if (!MasterConnectionService.instance) {
      MasterConnectionService.instance = new MasterConnectionService();
    }
    return MasterConnectionService.instance;
  }
  
  /**
   * Get connection status
   */
  getStatus(): ConnectionStatus {
    return this.status;
  }
  
  /**
   * Check if connected and ready
   */
  isConnected(): boolean {
    return this.status === ConnectionStatus.Ready || this.status === ConnectionStatus.Discovering;
  }
  
  /**
   * Get all discovered nodes
   */
  getNodes(): DiscoveredNode[] {
    return Array.from(this.nodes.values());
  }
  
  /**
   * Get specific node
   */
  getNode(nodeAddress: number): DiscoveredNode | undefined {
    return this.nodes.get(nodeAddress);
  }
  
  /**
   * Get all units for a node
   */
  getNodeUnits(nodeAddress: number): DiscoveredUnit[] {
    const node = this.nodes.get(nodeAddress);
    return node ? node.units : [];
  }
  
  /**
   * Get specific unit
   */
  getUnit(nodeAddress: number, unitAddress: number): DiscoveredUnit | undefined {
    const node = this.nodes.get(nodeAddress);
    return node?.units.find(u => u.unitAddress === unitAddress);
  }
  
  /**
   * Get all units (flattened from all nodes)
   */
  getAllUnits(): DiscoveredUnit[] {
    const allUnits: DiscoveredUnit[] = [];
    for (const node of this.nodes.values()) {
      allUnits.push(...node.units);
    }
    return allUnits;
  }
  
  /**
   * Connect and login to master
   */
  async connect(host: string, port: number, password: string = ''): Promise<LoginResult> {
    if (this.socket) {
      await this.disconnect();
    }
    
    // this._host = host;
    // this._port = port;
    this.password = password;
    this.status = ConnectionStatus.Connecting;
    
    return new Promise((resolve) => {
      const connectionTimeout = setTimeout(() => {
        if (this.status === ConnectionStatus.Connecting) {
          this.disconnect();
          resolve({ success: false, status: 'timeout', message: 'Connection timeout' });
        }
      }, 10000);
      
      this.socket = new Socket();
      
      this.socket.on('connect', () => {
        console.log(`✓ Connected to master ${host}:${port}`);
        clearTimeout(connectionTimeout);

        // Set up temporary data handler for login response
        const onLoginData = (data: Buffer) => {
          clearTimeout(loginTimeout);
          if (DuotecnoProtocol.loginOK(data)) {
            console.log('✓ Login successful');
            this.status = ConnectionStatus.LoggedIn;
            
            // Remove temporary handler and set up permanent one
            this.socket!.removeListener('data', onLoginData);

            this.socket!.on('data', (data: Buffer) => {
              this.handleData(data);
            });
                
            // Start heartbeat
            this.startHeartbeat();

            this.startDiscovery();
            resolve({ success: true, status: 'logged-in' });
            
          } else {
            this.disconnect();
            resolve({ success: false, status: 'wrong-password', message: 'Login failed' });
          }
        }
        
        // Attach temporary handler before sending login
        this.socket!.on('data', onLoginData);
        
        // Send login message
        const loginMsg = DuotecnoProtocol.buildLogin(password);
        this.send(loginMsg);
        
        // Wait 5secs for login response
        const loginTimeout = setTimeout(() => {
          clearTimeout(connectionTimeout);
          resolve({ success: false, status: 'timeout', message: 'Login timeout' });
        }, 5000);
      });
      
      this.socket.on('error', (err: Error) => {
        clearTimeout(connectionTimeout);
        console.error('Socket error:', err);
        this.status = ConnectionStatus.Error;
        resolve({ success: false, status: 'error', message: err.message });
      });
      
      this.socket.on('close', () => {
        console.log('Connection closed');
        this.handleDisconnect();
      });
      
      this.socket.connect(port, host);
    });
  }
  
  /**
   * Disconnect from master
   */
  async disconnect(): Promise<void> {
    if (this.socket) {
      // Send logout
      if (this.password && this.status !== ConnectionStatus.Disconnected) {
        const logoutMsg = DuotecnoProtocol.buildDisconnect();
        this.send(logoutMsg);
      }
      
      this.socket.destroy();
      this.socket = null;
    }
    
    this.handleDisconnect();
  }
  
  /**
   * Handle disconnection cleanup
   */
  private handleDisconnect(): void {
    this.stopHeartbeat();
    this.status = ConnectionStatus.Disconnected;
    this.buffer = '';
    this.commandQueue = [];
    this.waitingForResponse = false;
    
    if (this.responseTimeout) {
      clearTimeout(this.responseTimeout);
      this.responseTimeout = null;
    }
    
    // Reject all pending commands
    for (const [_key, pending] of this.pendingCommands.entries()) {
      clearTimeout(pending.timeout);
      pending.reject(new Error('Connection closed'));
    }
    this.pendingCommands.clear();
    
    this.emit('disconnected');
  }
  
  /**
   * Start discovery process
   */
  private async startDiscovery(): Promise<void> {
    console.log('🔍 Starting discovery process');
    this.status = ConnectionStatus.Discovering;
    this.nodes.clear();
    this.expectedNodes = 0;
    this.discoveredNodeCount = 0;
    this.pendingUnitCount = 0;
    this.currentNodeIndex = 0;
    this.currentUnitIndex = 0;
    this.currentNodeAddress = 0;
    this.currentNodeUnits = 0;

    // Request database info (first step in sequential discovery)
    const dbInfoMsg = DuotecnoProtocol.buildDBInfo();
    this.sendQueued(dbInfoMsg);
    
    this.emit('discovery-started');
  }
  
  /**
   * Handle incoming data
   */
  private handleData(data: Buffer): void {
    this.buffer += data.toString();
    
    // Process all complete messages in buffer
    while (this.buffer.length > 0) {
      const record = DuotecnoProtocol.nextMessage(this.buffer);
      
      if (!record.message) {
        // No complete message, wait for more data
        this.buffer = record.rest;
        break;
      }
      
      this.buffer = record.rest;
      
      // Handle the message
      this.handleMessage(record);
      this.emit('message', record);
    }
  }
  
  /**
   * Handle incoming message
   */
  private handleMessage(record: CommRecord): void {
    if (!record.message) return;
    
    const msg = record.message;
    const method = record.cmd === Rec.Info && msg.length > 1 ? msg[1] : undefined;
    
    console.debug(`Received: cmd=${record.cmd}, isStatus=${record.isStatus}${method !== undefined ? `, method=${method}` : ''}, data=[${msg.join(',')}]`);
    
    // Mark response received for queue processing
    // Don't clear queue flag for heartbeat responses (they bypass the queue)
    if (record.cmd !== Rec.HeartbeatStatus) {
      this.responseReceived();
    }
    
    // Handle status messages (unit-specific state updates)
    if (record.isStatus) {
      this.handleStatusUpdate(record);
      
    // Handle non-status messages
    } else if (record.cmd === Rec.ConnectStatus) {
      // Login status (handled in connect method, but could be logged here)
      console.debug('Login status:', msg);
      
    } else if (record.cmd === Rec.Info) {  // cmd = 64, method in msg[1]
      if (record.method === Rec.DBInfo) {
        this.handleDBInfo(msg);

      } else if (record.method === Rec.NodeInfo) {
        this.handleNodeInfo(msg);

      } else if (record.method === Rec.UnitInfo) {
        this.handleUnitInfo(msg);
      }
      
    } else if (record.cmd === Rec.TimeDateStatus) {
      // Time/date status update
      console.debug('Time/Date status:', msg);
      
    } else if (record.cmd === Rec.ScheduleStatus) {
      // Schedule status
      console.debug('Schedule status:', msg);
      
    } else if (record.cmd === Rec.HeartbeatStatus) {
      // Heartbeat response
      console.debug('Heartbeat status:', msg);
      
    } else if (record.cmd === Rec.NodeMgtInfo) {
      // Node management info
      console.debug('Node management info:', msg);
      
    } else if (record.cmd === Rec.Register) {
      // Register info
      console.debug('Register info:', msg);
      
    } else if (record.cmd === Rec.Internal) {
      // Internal message
      console.log('Received an internal message!');
      
    } else if (record.cmd === Rec.AutoStatus) {
      // Auto status (IR?)
      console.debug('Auto status:', msg);
      
    } else if (record.cmd === Rec.AudioStatus || record.cmd === Rec.AudioExtendedStatus) {
      // Audio status
      console.debug('Audio status:', msg);
      
    } else if (record.cmd === 75) { // EV_ACCESSLEVELINFO
      this.emit('access-level', msg);
      
    } else if (record.cmd === Rec.EV_PROTOCOLMSG) { // 122 - CAN bus events
      this.handleCANBusEvent(msg);
      
    } else if (record.cmd === Rec.ErrorMessage) {
      // Error message from hardware
      const method = msg[1];
      const nodeAddr = msg[2];
      const unitAddr = msg[3];
      const errorData = msg[4];
      console.warn(`⚠️ Hardware Error: cmd=${record.cmd}, method=${method}, node=0x${nodeAddr?.toString(16)}, unit=0x${unitAddr?.toString(16)}, error=0x${errorData?.toString(16)}`);
      console.warn('   Command rejected - this method may not be supported by this device');
      
    } else {
      console.log('Unhandled message type:', record.cmd, msg);
    }
  }
  
  /**
   * Handle CAN bus event (EV_PROTOCOLMSG wrapper)
   */
  private handleCANBusEvent(msg: Message): void {
    if (msg.length < 3) return;
    
    const objectType = msg[1];
    
    // Subscription confirmation
    if (objectType === 2) { // ATTR_SUBSCRIBE_CANBUS_MSG
      const subscribed = msg[2] === 1;
      console.log(`✓ CAN bus ${subscribed ? 'subscribed' : 'unsubscribed'}`);
      this.emit('can-subscription', subscribed);
      return;
    }
    
    // CAN bus message
    if (objectType === 1 && msg.length >= 9) { // OBJECT_CANBUS_MSG
      const nodeAddress = msg[2];
      const unitAddress = msg[3];
      const eventCode = msg[7];
      const dataLength = msg[8];
      const data = msg.slice(9, 9 + dataLength);
      
      const response = { nodeAddress, unitAddress, messageCode: eventCode, data };
      
      // Emit specific events based on message code
      if (eventCode === 0x11) { // EV_MESSAGEERROR
        this.emit('node-error', { nodeAddress, errorCode: data[0], data });
      } else if (eventCode === 0x0C) { // EV_NODECONFIG
        this.emit('node-config-response', response);
      } else if (eventCode === 0x10) { // EV_NODEBINDINGENTRY
        if (data[0] >= 0x80) {
          this.emit('binding-entry-error', { nodeAddress, errorCode: data[0] });
        } else {
          this.emit('binding-entry-response', response);
        }
      } else if (eventCode === 0x1B) { // EV_NODEBINDINGSFILECONTROL (0x9B response)
        this.emit('file-control-response', response);
      }
      
      // Generic CAN bus response
      this.emit('canbus-response', response);
    }
  }
  
  /**
   * Handle DBInfo response
   */
  private handleDBInfo(msg: Message): void {
    // Format: [64, 0, nrNodes, ...]
    this.expectedNodes = msg[2];
    console.log(`📊 Database Info: Expecting ${this.expectedNodes} nodes`);
    
    // Start sequential discovery: request first node
    this.currentNodeIndex = 0;
    if (this.expectedNodes > 0) {
      const nodeInfoMsg = DuotecnoProtocol.buildNodeInfo(this.currentNodeIndex);
      this.sendQueued(nodeInfoMsg);
    } else {
      this.discoveryComplete();
    }
  }
  
  /**
   * Handle NodeInfo response
   */
  private handleNodeInfo(msg: Message): void {
    // Format: [64, 1, nodeIdx, nodeAddr, physAddr(4 bytes), stringLength, ...name bytes..., type, nrUnits, flags]
    // const nodeIdx = msg[2]; // Index in discovery sequence
    const nodeAddr = msg[3];
    
    // Physical address (4 bytes at position 4-7)
    const physAddr = DuotecnoProtocol.makeLong(msg, 4);
    
    // Extract name - length-prefixed string starting at position 8
    const name = DuotecnoProtocol.bytesToString(msg, 8);
    const stringBytesUsed = 1 + name.length; // 1 byte for length + string chars
    
    // Data after the string
    const afterStringPos = 8 + stringBytesUsed; // = 9 + name.length
    const nrUnits = msg[afterStringPos];
    const type = msg[afterStringPos + 1];
    const flags = msg[afterStringPos + 2];
    
    const node: DiscoveredNode = {
      nodeAddress: nodeAddr,
      name: name || `Node ${nodeAddr.toString(16).toUpperCase()}`,
      physicalAddress: physAddr,
      type,
      nrUnits,
      flags,
      units: []
    };
    
    this.nodes.set(nodeAddr, node);
    this.discoveredNodeCount++;
    this.pendingUnitCount += nrUnits;
    
    console.log(`📍 Node ${nodeAddr.toString(16).toUpperCase()}: ${node.name} (${nrUnits} units)`);
    
    // Store current node info for sequential unit discovery
    this.currentNodeAddress = nodeAddr;
    this.currentNodeUnits = nrUnits;
    this.currentUnitIndex = 0;
    
    // Start sequential unit discovery: request first unit if any
    if (nrUnits > 0) {
      const unitInfoMsg = DuotecnoProtocol.buildUnitInfo(nodeAddr, this.currentUnitIndex);
      this.sendQueued(unitInfoMsg);
    } else {
      // No units, move to next node
      this.moveToNextNode();
    }
  }
  
  /**
   * Handle UnitInfo response
   */
  private handleUnitInfo(msg: Message): void {
    // Format: [64, 2, ?, unitIdx, nodeAddr, unitAddr, stringLength, ...name bytes..., type, flags]
    // const unitIdx = msg[3]; // Index in node's unit list
    const nodeAddr = msg[4];
    const unitAddr = msg[5];
    
    // Extract name - length-prefixed string starting at position 6
    const name = DuotecnoProtocol.bytesToString(msg, 6);
    const stringBytesUsed = 1 + name.length; // 1 byte for length + string chars
    
    // Data after the string
    const afterStringPos = 6 + stringBytesUsed; // = 7 + name.length
    const type = msg[afterStringPos] as UnitType;
    
    const unit: DiscoveredUnit = {
      nodeAddress: nodeAddr,
      unitAddress: unitAddr,
      name: name || `Unit ${unitAddr.toString(16).toUpperCase()}`,
      type
    };
    
    // Add to node
    const node = this.nodes.get(nodeAddr);
    if (node) {
      node.units.push(unit);
    }
    
    this.pendingUnitCount--;
    
    // Move to next unit in current node
    this.currentUnitIndex++;
    
    if (this.currentUnitIndex < this.currentNodeUnits) {
      // Request next unit in this node
      const unitInfoMsg = DuotecnoProtocol.buildUnitInfo(this.currentNodeAddress, this.currentUnitIndex);
      this.sendQueued(unitInfoMsg);
    } else {
      // All units for current node received, move to next node
      this.moveToNextNode();
    }
  }
  
  /**
   * Move to next node in sequential discovery
   */
  private moveToNextNode(): void {
    this.currentNodeIndex++;
    
    if (this.currentNodeIndex < this.expectedNodes) {
      // Request next node info
      const nodeInfoMsg = DuotecnoProtocol.buildNodeInfo(this.currentNodeIndex);
      this.sendQueued(nodeInfoMsg);
    } else {
      // All nodes discovered
      this.discoveryComplete();
    }
  }
  
  /**
   * Discovery complete
   */
  private discoveryComplete(): void {
    this.status = ConnectionStatus.Ready;
    const totalUnits = this.getAllUnits().length;
    console.log(`✅ Discovery complete: ${this.nodes.size} nodes, ${totalUnits} units`);
    
    // Request status for all discovered units (using queued sending)
    console.log('📊 Requesting status for all units...');
    for (const node of this.nodes.values()) {
      for (const unit of node.units) {
        const statusMsg = DuotecnoProtocol.buildUnitStatus(unit.nodeAddress, unit.unitAddress, unit.type);
        this.sendQueued(statusMsg);
      }
    }
    
    this.emit('discovery-complete', {
      nodes: Array.from(this.nodes.values()),
      units: this.getAllUnits()
    });
  }
  
  /**
   * Check if a command is an action command (doesn't need to wait for response)
   */
  private isActionCommand(msg: Message): boolean {
    const cmd = msg[0];
    return cmd === Cmd.SetDimmer || 
           cmd === Cmd.SetSwitch || 
           cmd === Cmd.SetControl || 
           cmd === Cmd.SetMotor || 
           cmd === Cmd.SetSensor || 
           cmd === Cmd.SetBasicAudio || 
           cmd === Cmd.SetExtendedAudio || 
           cmd === Cmd.SetAVMatrix ||
           cmd === Cmd.Register;
  }
  
  /**
   * Send message through queue system (for non-action commands)
   */
  private sendQueued(msg: Message): void {
    this.commandQueue.push(msg);
    this.processQueue();
  }
  
  /**
   * Process command queue
   */
  private processQueue(): void {
    if (this.waitingForResponse || this.commandQueue.length === 0) {
      return;
    }
    
    const msg = this.commandQueue.shift();
    if (!msg) return;
    
    // Only wait for response if it's not an action command
    if (!this.isActionCommand(msg)) {
      this.waitingForResponse = true;
      
      // Add timeout protection to prevent getting stuck
      this.responseTimeout = setTimeout(() => {
        console.warn('⚠️ Response timeout for queued command, continuing queue');
        this.responseReceived();
      }, this.RESPONSE_TIMEOUT_MS);
    }
    
    this.send(msg);
  }
  
  /**
   * Mark response received and continue processing queue
   */
  private responseReceived(): void {
    if (this.responseTimeout) {
      clearTimeout(this.responseTimeout);
      this.responseTimeout = null;
    }
    this.waitingForResponse = false;
    this.processQueue();
  }
  
  /**
   * Handle status update
   */
  private handleStatusUpdate(record: CommRecord): void {
    if (!record.message || record.message.length < 4) return;
    
    const msg = record.message;
    const nodeAddr = msg[2];
    const unitAddr = msg[3];
    const key = `${nodeAddr}:${unitAddr}`;
    
    const unit = this.getUnit(nodeAddr, unitAddr);
    if (!unit) {
      console.warn(`⚠️ Status for unknown unit ${nodeAddr.toString(16)}:${unitAddr.toString(16)}`);
      return;
    }
    
    // Update unit status based on message type
    switch (record.cmd) {
      case Rec.Switch:
        unit.status = msg[6];  // Status at position 6 (same as dimmer format)
        console.debug(`💡 Switch ${unit.name}: ${unit.status ? 'ON' : 'OFF'}`);
        break;
      case Rec.Dimmer:
        unit.status = msg[6];  // Status: 0=OFF, 1=ON, 2=PIRTIMED
        unit.value = msg[7];    // Dim value: 1-99
        // Display as OFF if value is 0, regardless of status byte (better UX during transitions)
        const displayStatus = (unit.status && unit.value > 0) ? 'ON' : 'OFF';
        console.debug(`🔆 Dimmer ${unit.name}: ${displayStatus}, value=${unit.value}%`);
        break;
      case Rec.Sensor:
        unit.status = msg[7];
        if (!unit.temperature) {
          unit.temperature = { temp: 0, preset: -1, sun: 0, hsun: 0, moon: 0, hmoon: 0 };
        }
        unit.temperature.temp = msg[9] * 256 + msg[10];
        unit.temperature.preset = msg[6] ? msg[8] : -1;
        unit.temperature.sun = msg[11] * 256 + msg[12];
        unit.temperature.hsun = msg[13] * 256 + msg[14];
        unit.temperature.moon = msg[15] * 256 + msg[16];
        unit.temperature.hmoon = msg[17] * 256 + msg[18];
        unit.value = unit.temperature.temp;
        console.debug(`🌡️ Sensor ${unit.name}: ${(unit.temperature.temp / 10).toFixed(1)}°C, preset=${unit.temperature.preset}, ` +
                     `setpoints: sun=${(unit.temperature.sun / 10).toFixed(1)}°C, hsun=${(unit.temperature.hsun / 10).toFixed(1)}°C, ` +
                     `moon=${(unit.temperature.moon / 10).toFixed(1)}°C, hmoon=${(unit.temperature.hmoon / 10).toFixed(1)}°C`);
        break;
      case Rec.Motor:
        unit.status = msg[6];  // Status at position 6 (consistent with other unit types)
        console.debug(`🪟 Motor ${unit.name}: status=${unit.status}`);
        break;
      case Rec.Mood:
        unit.status = msg[6];  // Status at position 6
        unit.value = msg[7];   // Value at position 7 (if applicable)
        console.debug(`🎭 Mood ${unit.name}: status=${unit.status}, value=${unit.value}`);
        break;
    }
    
    this.emit('unit-status-changed', { nodeAddress: nodeAddr, unitAddress: unitAddr, unit });
    
    // Resolve any pending commands for this unit
    const pending = this.pendingCommands.get(key);
    if (pending) {
      // Check if status matches expected values (if provided)
      const statusMatches = pending.expectedStatus === undefined || unit.status === pending.expectedStatus;
      const valueMatches = pending.expectedValue === undefined || unit.value === pending.expectedValue;
      
      if (statusMatches && valueMatches) {
        console.log(`Status matches expected (status=${pending.expectedStatus}, value=${pending.expectedValue}), resolving`);
        clearTimeout(pending.timeout);
        this.pendingCommands.delete(key);
        pending.resolve(unit);
      } else {
        console.log(`Status doesn't match expected yet (got status=${unit.status}, value=${unit.value}, expected status=${pending.expectedStatus}, value=${pending.expectedValue}), waiting...`);
      }
    }
  }
  
  /**
   * Request unit status
   */
  public requestUnitStatus(nodeAddr: number, unitAddr: number, type: UnitType): void {
    const msg = DuotecnoProtocol.buildUnitStatus(nodeAddr, unitAddr, type);
    this.send(msg);
  }
  
  /**
   * Send unit command and wait for status response
   */
  async sendUnitCommand(nodeAddr: number, unitAddr: number, type: UnitType, status: number, value?: number): Promise<DiscoveredUnit> {
    if (!this.isConnected()) {
      throw new Error('Not connected to master');
    }
    
    // Get current unit state before sending command
    const currentUnit = this.getUnit(nodeAddr, unitAddr);
    if (!currentUnit) {
      throw new Error('Unit not found');
    }
    
    let msg: Message;
    let expectedValue: number | undefined;
    let expectedStatus: number | undefined;
    
    switch (type) {
      case UnitType.SWITCH:
        console.log(`Building switch command: nodeAddr=${nodeAddr}, unitAddr=${unitAddr}, status=${status}, status>0=${status > 0}`);
        msg = DuotecnoProtocol.buildSetSwitch(nodeAddr, unitAddr, status > 0);
        console.log(`Switch command built: [${msg.join(',')}]`);
        expectedStatus = status > 0 ? 1 : 0;
        break;
      case UnitType.CONTROL:
      case UnitType.LCD_VIRTUAL:
        // Use value parameter: if < 0 = pulse, else = toggle with 0/1
        msg = DuotecnoProtocol.buildSetControl(nodeAddr, unitAddr, value != null ? value : status);
        // Control units don't have predictable state changes (pulse events)
        break;
      case UnitType.DIM:
        // If value provided (not null/undefined): use it. Else use status: >0 = -1 (ON/restore), 0 = 0 (OFF)
        const dimValue = value != null ? value : (status > 0 ? -1 : 0);
        
        // Hardware limitation: Some dimmers can't set to specific value when OFF
        // Solution: If dimmer is OFF and we want a specific value, turn ON first
        if (dimValue > 0 && currentUnit.status === 0) {
          console.log(`Dimmer is OFF, turning ON before setting to ${dimValue}%`);
          // First turn ON (will restore to previous value, e.g., 75%)
          const onMsg = DuotecnoProtocol.buildSetDimmer(nodeAddr, unitAddr, -1);
          this.send(onMsg);
          // Small delay to let dimmer turn on
          await new Promise(resolve => setTimeout(resolve, 100));
          // Now set to desired value
          msg = DuotecnoProtocol.buildSetDimmer(nodeAddr, unitAddr, dimValue);
        } else {
          msg = DuotecnoProtocol.buildSetDimmer(nodeAddr, unitAddr, dimValue);
        }
        
        // Only set expected value if we know the exact target (not for restore command -1)
        if (dimValue >= 0) {
          expectedValue = dimValue;
          expectedStatus = dimValue === 0 ? 0 : 1;
        }
        break;
      case UnitType.SENS:
        msg = DuotecnoProtocol.buildSetSensor(nodeAddr, unitAddr, value != null ? value : (status > 0 ? 0 : -1));
        break;
      case UnitType.DUOSWITCH:
        msg = DuotecnoProtocol.buildSetMotor(nodeAddr, unitAddr, status);
        expectedStatus = status;
        break;
      default:
        throw new Error(`Unsupported unit type: ${type}`);
    }
    
    // Check if unit is already at the expected state (for dimmer/switch)
    // If so, hardware might not send a status update
    const alreadyAtTarget = 
      (expectedStatus !== undefined && currentUnit.status === expectedStatus) &&
      (expectedValue === undefined || currentUnit.value === expectedValue);
    
    if (alreadyAtTarget) {
      console.log(`Unit already at target state (status=${expectedStatus}, value=${expectedValue}), returning current state`);
      this.send(msg); // Still send the command (hardware might do something)
      // Brief delay to allow command processing
      await new Promise(resolve => setTimeout(resolve, 100));
      return currentUnit;
    }
    
    // Send command (action commands don't wait for response)
    this.send(msg);
    
    // Explicitly request status for dimmers because:
    // 1. Hardware bug: Some dimmers don't send automatic status updates after commands
    // 2. ON command (dimValue=-1) restores previous value, which we need to fetch
    if (type === UnitType.DIM) {
      const statusMsg = DuotecnoProtocol.buildUnitStatus(nodeAddr, unitAddr, type);
      this.sendQueued(statusMsg);
    }
    
    // Wait for status response
    return new Promise((resolve, reject) => {
      const key = `${nodeAddr}:${unitAddr}`;
      const timeout = setTimeout(() => {
        this.pendingCommands.delete(key);
        
        // Request status explicitly
        this.requestUnitStatus(nodeAddr, unitAddr, type);
        
        // Give it one more chance
        setTimeout(() => {
          const unit = this.getUnit(nodeAddr, unitAddr);
          if (unit) {
            resolve(unit);
          } else {
            reject(new Error('Command timeout - no status response'));
          }
        }, 1000);
      }, 3000);
      
      this.pendingCommands.set(key, { resolve, reject, timeout, expectedValue, expectedStatus });
    });
  }
  
  /**
   * Start heartbeat keepalive
   */
  private startHeartbeat(): void {
    this.stopHeartbeat();
    this.heartbeatInterval = setInterval(() => {
      if (this.isConnected()) {
        const heartbeatMsg = DuotecnoProtocol.buildHeartbeat();
        this.send(heartbeatMsg);
      }
    }, this.HEARTBEAT_INTERVAL_MS);
  }
  
  /**
   * Stop heartbeat
   */
  private stopHeartbeat(): void {
    if (this.heartbeatInterval) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
    }
  }
  
  /**
   * Send message to master
   */
  private send(msg: Message): void {
    if (!this.socket) {
      throw new Error('Not connected');
    }
    
    const formatted = `[${msg.join(',')}]\n`;
    console.log('📤 Sending:', formatted);
    this.socket.write(formatted);
  }
  
  /**
   * Subscribe to CAN bus events (for binding uploads)
   */
  public subscribeToCanBus(): void {
    const msg = DuotecnoProtocol.buildCANBusSubscribe(true);
    console.log('📤 Subscribing to CAN bus');
    this.send(msg);
  }
  
  /**
   * Unsubscribe from CAN bus events
   */
  public unsubscribeFromCanBus(): void {
    const msg = DuotecnoProtocol.buildCANBusSubscribe(false);
    console.log('📤 Unsubscribing from CAN bus');
    this.send(msg);
  }
  
  /**
   * Send raw CAN bus message (for binding operations)
   * Format: [FC_PROTOCOLMSG, OBJECT_CANBUS_MSG, nodeAddr, unitAddr, srcUnit, srcNode, apduHeader, funcCode, dataLen, ...data]
   */
  public sendCanBusMessage(nodeAddr: number, unitAddr: number, functionCode: number, data: number[]): void {
    const msg = DuotecnoProtocol.buildCANBusMessage(nodeAddr, unitAddr, functionCode, data);
    this.send(msg);
  }
  
  /**
   * Send raw message (for custom commands)
   * Useful for sending protocol messages that don't have a specific builder method
   */
  public sendRawMessage(msg: Message): void {
    if (!this.isConnected()) {
      throw new Error('Not connected to master');
    }
    this.send(msg);
  }
  
  /**
   * Set access level (for binding operations)
   */
  public async setAccessLevel(password: string): Promise<number> {
    return new Promise((resolve) => {
      const handler = (msg: number[]) => {
        // Access level response: [75, method, accessLevel, ...echo]
        const accessLevel = msg[2];
        resolve(accessLevel);
      };
      
      this.once('access-level', handler);
      
      const msg = DuotecnoProtocol.buildSetAccessLevel(password);
      this.send(msg);
    });
  }
  
  /**
   * Set node configuration flags (for binding operations)
   */
  public setNodeConfig(nodeAddr: number, nodeConfig: number, dllAccessControl: number): void {
    const msg = DuotecnoProtocol.buildNodeConfig(nodeAddr, nodeConfig, dllAccessControl);
    this.send(msg);
  }
  
  /**
   * Start task on master (for binding operations)
   */
  public startTask(password: string, taskType: number = 0x09): void {
    const msg = DuotecnoProtocol.buildStartTask(password, taskType);
    this.send(msg);
  }
}
