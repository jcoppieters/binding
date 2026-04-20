/**
 * TCP-based CAN Bus Service
 * Connects to Duotecno smartbox via TCP socket and sends/receives CAN bus messages
 * 
 * Based on master.ts from the Ionic/Angular app
 */

import * as net from 'net';
import { DuotecnoProtocol, Rec, CommRecord, Message, Cmd } from './DuotecnoProtocol';
import { CANBusService, CANFrame, EventCode } from './CANBusService';

export interface TCPConnectionConfig {
  host: string;
  port: number;
  password: string;
}

export class TCPCANBusService extends CANBusService {
  private socket: net.Socket | null = null;
  private buffer: string = '';
  private tcpConfig: TCPConnectionConfig | null = null;
  private _isConnected: boolean = false;
  private isLoggedIn: boolean = false;
  private isSubscribedToCANBus: boolean = false;
  private heartbeatInterval: NodeJS.Timeout | null = null;
  private loginResolve: ((success: boolean) => void) | null = null;

  constructor() {
    super();
  }

  /**
   * Set TCP connection configuration
   */
  setConfig(config: TCPConnectionConfig): void {
    this.tcpConfig = config;
  }

  /**
   * Connect to smartbox via TCP
   */
  async connect(): Promise<void> {
    if (!this.tcpConfig) {
      throw new Error('TCP config not set. Call setConfig() first.');
    }

    if (this.socket) {
      await this.disconnect();
    }

    const config = this.tcpConfig;

    return new Promise((resolve, reject) => {
      console.log(`[TCP] Connecting to ${config.host}:${config.port}...`);

      this.socket = new net.Socket();

      // Set up event handlers
      this.socket.on('data', (data: Buffer) => {
        this.handleData(data.toString());
      });

      this.socket.on('error', (err: Error) => {
        console.error('[TCP] Socket error:', err.message);
        reject(err);
      });

      this.socket.on('close', () => {
        console.log('[TCP] Socket closed');
        this._isConnected = false;
        this.isLoggedIn = false;
        this.stopHeartbeat();
      });

      this.socket.on('connect', () => {
        console.log('[TCP] Socket connected');
        this._isConnected = true;
        resolve();
        this.startHeartbeat();
      });

      // Connect to smartbox
      this.socket.connect(config.port, config.host);
    });
  }

  /**
   * Login to master with password
   */
  async login(): Promise<boolean> {
    if (!this._isConnected) {
      throw new Error('Not connected to smartbox');
    }

    return new Promise((resolve, reject) => {
      this.loginResolve = resolve;

      try {
        const message = DuotecnoProtocol.buildLogin(this.tcpConfig!.password);
        this.writeMessage(message);
      } catch (err) {
        this.loginResolve = null;
        reject(err);
      }
    });
  }

  /**
   * Disconnect from master
   */
  async disconnect(): Promise<void> {
    this.stopHeartbeat();

    if (this._isConnected && this.socket) {
      // Unsubscribe from CAN bus events before disconnecting
      if (this.isSubscribedToCANBus) {
        const unsubscribeMsg = DuotecnoProtocol.buildCANBusSubscribe(false);
        this.writeMessage(unsubscribeMsg);
        console.log('[TCP] Unsubscribing from CAN bus events');
      }

      const message = DuotecnoProtocol.buildDisconnect();
      this.writeMessage(message);
      
      return new Promise((resolve) => {
        setTimeout(() => {
          this.socket?.destroy();
          this.socket = null;
          this._isConnected = false;
          this.isLoggedIn = false;
          this.isSubscribedToCANBus = false;
          resolve();
        }, 100);
      });
    }
  }

  /**
   * Send CAN bus message over TCP using FC_PROTOCOLMSG wrapper
   */
  async sendMessage(frame: CANFrame): Promise<void> {
    if (!this.isLoggedIn) {
      throw new Error('Not logged in to smartbox');
    }

    // Build wrapped CAN bus message using DuotecnoProtocol helper
    // The protocol handles wrapping in FC_PROTOCOLMSG with proper APDU headers
    const unitAddress = frame.unitAddress ?? 0xFF;
    const dataBytes = Array.from(frame.data);
    
    // Build CAN bus message wrapper
    const message = DuotecnoProtocol.buildCANBusMessage(
      frame.nodeAddress,
      unitAddress,
      frame.functionCode,
      dataBytes
    );

    console.log(`[TCP] TX CAN: Func=${frame.functionCode.toString(16).toUpperCase().padStart(2, '0')} Node=${frame.nodeAddress.toString(16).toUpperCase().padStart(2, '0')} Unit=${unitAddress.toString(16).toUpperCase().padStart(2, '0')} Data=[${dataBytes.map(b => b.toString(16).toUpperCase().padStart(2, '0')).join(' ')}]`);
    console.log(`[TCP] TX Message: [${message.map(b => b.toString(16).toUpperCase().padStart(2, '0')).join(', ')}]`);

    this.writeMessage(message);
  }

  /**
   * Check connection status
   */
  isConnected(): boolean {
    return this._isConnected && this.isLoggedIn;
  }

  /**
   * Internal: Write message to socket
   */
  private writeMessage(message: Message): void {
    if (!this.socket) {
      throw new Error('Socket not available');
    }

    const formatted = DuotecnoProtocol.formatMessage(message);
    this.socket.write(formatted);
  }

  /**
   * Internal: Handle incoming data
   */
  private handleData(data: string): void {
    this.buffer += data;

    // Process all complete messages in buffer
    while (this.buffer.length > 0) {
      const next = DuotecnoProtocol.nextMessage(this.buffer);
      this.buffer = next.rest;

      if (!next.cmd) {
        break;
      }

      this.processMessage(next);
    }
  }

  /**
   * Internal: Process a complete message
   */
  private processMessage(record: CommRecord): void {
    if (!record.message) return;

    const msg = record.message;

    console.log(`[TCP] RX: cmd=${record.cmd} (0x${record.cmd.toString(16).toUpperCase()}) msg=`, msg);

    switch (record.cmd) {
      case Rec.ConnectStatus:
        this.handleLoginResponse(msg);
        break;

      case Rec.HeartbeatStatus:
        // Heartbeat acknowledgement
        break;

      case Rec.ErrorMessage:
        console.error(`[TCP] Error message: (${msg[1]})`, msg);
        break;

      case Rec.EV_PROTOCOLMSG:
        // CAN bus event wrapped in protocol message
        // Format: [122, objectType, nodeAddress, unitAddress, srcUnit, srcNode, APDUheader, eventCode, dataLength, ...data]
        this.handleProtocolCANEvent(msg);
        break;

      default:
        if (record.isStatus) {
          console.log('[TCP] Status:', msg);
        }
        break;
    }
  }

  /**
   * Subscribe to CAN bus events
   */
  async subscribe(): Promise<void> {
    if (!this.isLoggedIn) {
      throw new Error('Not logged in to smartbox');
    }

    if (this.isSubscribedToCANBus) {
      console.log('[TCP] Already subscribed to CAN bus');
      return;
    }

    const subscribeMsg = DuotecnoProtocol.buildCANBusSubscribe(true);
    this.writeMessage(subscribeMsg);
    console.log(`[TCP] Subscribing to CAN bus events: [${subscribeMsg.join(', ')}]`);
    
    // Wait a bit for subscription to complete
    await new Promise(resolve => setTimeout(resolve, 100));
  }

  /**
   * Unsubscribe from CAN bus events
   */
  async unsubscribe(): Promise<void> {
    if (!this.isSubscribedToCANBus) {
      console.log('[TCP] Not subscribed to CAN bus');
      return;
    }

    const unsubscribeMsg = DuotecnoProtocol.buildCANBusSubscribe(false);
    this.writeMessage(unsubscribeMsg);
    console.log(`[TCP] Unsubscribing from CAN bus events: [${unsubscribeMsg.join(', ')}]`);
    
    // Wait a bit for unsubscription to complete
    await new Promise(resolve => setTimeout(resolve, 100));
  }

  /**
   * Internal: Handle login response
   */
  private handleLoginResponse(msg: Message): void {
    this.isLoggedIn = (msg[2] === 1);
    console.log(`[TCP] Login ${this.isLoggedIn ? 'SUCCESS' : 'FAILED'}`);

    if (this.loginResolve) {
      this.loginResolve(this.isLoggedIn);
      this.loginResolve = null;
    }
  }

  /**
   * Internal: Handle CAN bus event wrapped in protocol message
   * Format: [EV_PROTOCOLMSG, OBJECT_CANBUS_MSG, nodeAddr, unitAddr, srcUnit, srcNode, APDUheader, eventCode, dataLength, ...data]
   */
  private handleProtocolCANEvent(msg: Message): void {
    if (msg.length < 3) {
      console.error('[TCP] Invalid protocol event - too short:', msg);
      return;
    }

    const objectType = msg[1];
    
    // Handle subscription response
    if (objectType === Cmd.ATTR_SUBSCRIBE_CANBUS_MSG) {
      const status = (msg[2] === 1);
      this.isSubscribedToCANBus = status;
      console.log(`[TCP] CAN bus subscription ${status ? 'enabled' : 'disabled'}`);
      return;
    }
    
    // Handle CAN bus message event
    if (objectType === Cmd.OBJECT_CANBUS_MSG) {
      if (msg.length < 9) {
        console.error('[TCP] Invalid CAN bus message - too short:', msg);
        return;
      }

      const nodeAddress = msg[2];
      const unitAddress = msg[3];
      // msg[4] = source unit
      // msg[5] = source node
      // msg[6] = APDU header
      const eventCode = msg[7] as EventCode;
      const dataLength = msg[8];
      const data = msg.slice(9, 9 + dataLength);

      console.log(`[TCP] RX CAN Event: ${eventCode.toString(16).toUpperCase().padStart(2, '0')} Node=${nodeAddress.toString(16).toUpperCase().padStart(2, '0')} Unit=${unitAddress.toString(16).toUpperCase().padStart(2, '0')} [${data.map(b => b.toString(16).toUpperCase().padStart(2, '0')).join(' ')}]`);

      // Emit event for listeners (must be 'event', not 'response')
      this.emit('event', {
        nodeAddress,
        eventCode,
        data: Buffer.from(data),
        timestamp: Date.now()
      });
    }
  }

  /**
   * Internal: Start heartbeat timer
   */
  private startHeartbeat(): void {
    this.stopHeartbeat();

    this.heartbeatInterval = setInterval(() => {
      if (this._isConnected && this.socket) {
        const message = DuotecnoProtocol.buildHeartbeat();
        this.writeMessage(message);
      }
    }, 15000); // Every 15 seconds

    console.log('[TCP] Heartbeat started');
  }

  /**
   * Internal: Stop heartbeat timer
   */
  private stopHeartbeat(): void {
    if (this.heartbeatInterval) {
      clearInterval(this.heartbeatInterval);
      this.heartbeatInterval = null;
      console.log('[TCP] Heartbeat stopped');
    }
  }
}
