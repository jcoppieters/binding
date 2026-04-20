/**
 * CAN Bus Communication Service
 * 
 * Handles low-level communication with Duotecno hardware nodes over CAN bus.
 * Based on legacy C++ MessageTransfer implementation.
 */

import EventEmitter from 'events';

// Function codes from nodemess.h
export enum FunctionCode {
  // Node management
  FC_NODESETNAME = 0x8C,
  FC_NODEREQUESTNAME = 0x8E,
  FC_NODEREQUESTSTATUS = 0x8F,
  FC_NODEGETRESOURCES = 0x9D,
  FC_NODERESET = 0x9B,
  FC_NODECONFIG = 0x94,
  
  // Unit management
  FC_UNITSETNAME = 0x8B,
  FC_UNITREQUESTNAME = 0x8D,
  FC_UNITREQUESTCONFIG = 0x84,
  FC_UNITREQUESTCONTROLSTATUS = 0x87,
}

// Event codes from nodemess.h
export enum EventCode {
  EV_NODENAME = 0x0A,
  EV_UNITNAME = 0x09,
  EV_UNITCONFIG = 0x08,
  EV_NODECONFIG = 0x0C,      // Config/unlock acknowledgment
  EV_NODERESOURCES = 0x0D,
  EV_NODESTATUS = 0x0E,
  EV_MESSAGEERROR = 0x11,
}

// Error codes from nodemess.h
export enum MessageError {
  REFUSED_RUNNING_STATE = 0x00,
  INVALID = 0x01,
  BUFFER_FULL = 0x02,
  TIMEOUT = 0x06,
}

export interface CANFrame {
  nodeAddress: number;
  unitAddress: number;
  functionCode: number;
  dataLength: number;
  data: Buffer;
  checksum?: number;
}

export interface CANResponse {
  eventCode: EventCode;
  nodeAddress: number;
  unitAddress: number;
  data: Buffer;
}

export interface CANBusConfig {
  port?: string;           // Serial port (e.g., '/dev/tty.usbserial')
  baudRate?: number;       // Default: 115200
  timeout?: number;        // Default: 6000ms
  maxRetries?: number;     // Default: 3
}

/**
 * Abstract CAN Bus Service Interface
 * 
 * Allows for mock implementation during development and real hardware later.
 */
export abstract class CANBusService extends EventEmitter {
  protected config: Required<CANBusConfig>;
  
  constructor(config: CANBusConfig = {}) {
    super();
    this.config = {
      port: config.port || '/dev/tty.usbserial',
      baudRate: config.baudRate || 115200,
      timeout: config.timeout || 6000,
      maxRetries: config.maxRetries || 3,
    };
  }
  
  /**
   * Connect to CAN bus
   */
  abstract connect(): Promise<void>;
  
  /**
   * Disconnect from CAN bus
   */
  abstract disconnect(): Promise<void>;
  
  /**
   * Check if connected
   */
  abstract isConnected(): boolean;
  
  /**
   * Send a message to a node/unit
   * Implements the MessageTransfer() equivalent from C++
   */
  abstract sendMessage(message: CANFrame): Promise<void>;
  
  /**
   * Subscribe to CAN bus events
   * Default implementation does nothing (for direct CAN bus)
   * Override in TCP implementation
   */
  async subscribe(): Promise<void> {
    // Default no-op for direct CAN bus
  }
  
  /**
   * Unsubscribe from CAN bus events
   * Default implementation does nothing (for direct CAN bus)
   * Override in TCP implementation
   */
  async unsubscribe(): Promise<void> {
    // Default no-op for direct CAN bus
  }
  
  /**
   * Wait for a specific event response
   */
  async waitForEvent(
    eventCode: EventCode,
    nodeAddress: number,
    timeout?: number
  ): Promise<CANResponse> {
    return new Promise((resolve, reject) => {
      const timeoutMs = timeout || this.config.timeout;
      const timer = setTimeout(() => {
        this.removeListener('event', handler);
        reject(new Error(`Timeout waiting for event 0x${eventCode.toString(16).toUpperCase()}`));
      }, timeoutMs);
      
      const handler = (response: CANResponse) => {
        if (
          response.eventCode === eventCode &&
          response.nodeAddress === nodeAddress
        ) {
          clearTimeout(timer);
          this.removeListener('event', handler);
          resolve(response);
        }
      };
      
      this.on('event', handler);
    });
  }
  
  /**
   * Send message and wait for response with retry logic
   */
  async sendMessageWithRetry(
    message: CANFrame,
    expectedEvent: EventCode
  ): Promise<CANResponse> {
    let lastError: Error | undefined;
    
    for (let attempt = 0; attempt < this.config.maxRetries; attempt++) {
      try {
        // Send message
        await this.sendMessage(message);
        
        // Wait for response
        const response = await this.waitForEvent(
          expectedEvent,
          message.nodeAddress,
          this.config.timeout
        );
        
        return response;
      } catch (error) {
        lastError = error as Error;
        console.warn(
          `Attempt ${attempt + 1}/${this.config.maxRetries} failed:`,
          error
        );
      }
    }
    
    throw lastError || new Error('Failed after retries');
  }
}

/**
 * Mock CAN Bus Service for development/testing
 * 
 * Simulates hardware responses without actual CAN bus connection.
 */
export class MockCANBusService extends CANBusService {
  private connected = false;
  private mockDelay = 100; // Simulate network latency
  
  async connect(): Promise<void> {
    console.log('[MOCK CAN] Connecting to mock CAN bus...');
    await new Promise(resolve => setTimeout(resolve, this.mockDelay));
    this.connected = true;
    console.log('[MOCK CAN] Connected successfully');
  }
  
  async disconnect(): Promise<void> {
    console.log('[MOCK CAN] Disconnecting...');
    this.connected = false;
  }
  
  isConnected(): boolean {
    return this.connected;
  }
  
  async sendMessage(message: CANFrame): Promise<void> {
    if (!this.connected) {
      throw new Error('CAN bus not connected');
    }
    
    console.log('[MOCK CAN] Sending message:', {
      node: `0x${message.nodeAddress.toString(16).toUpperCase()}`,
      unit: `0x${message.unitAddress.toString(16).toUpperCase()}`,
      fc: `0x${message.functionCode.toString(16).toUpperCase()}`,
      dataLength: message.dataLength,
      data: message.data.toString('hex'),
    });
    
    // Simulate async hardware response
    setTimeout(() => {
      // Generate appropriate mock response based on function code
      let eventCode: EventCode;
      
      switch (message.functionCode) {
        case FunctionCode.FC_NODESETNAME:
          eventCode = EventCode.EV_NODENAME;
          break;
        case FunctionCode.FC_UNITSETNAME:
          eventCode = EventCode.EV_UNITNAME;
          break;
        case FunctionCode.FC_NODEREQUESTSTATUS:
          eventCode = EventCode.EV_NODESTATUS;
          break;
        default:
          return; // No response for unknown commands
      }
      
      const response: CANResponse = {
        eventCode,
        nodeAddress: message.nodeAddress,
        unitAddress: message.unitAddress,
        data: Buffer.from([0x00]), // 0x00 = success
      };
      
      console.log('[MOCK CAN] Received response:', {
        event: `0x${eventCode.toString(16).toUpperCase()}`,
        node: `0x${response.nodeAddress.toString(16).toUpperCase()}`,
        success: true,
      });
      
      this.emit('event', response);
    }, this.mockDelay);
  }
}

/**
 * Real CAN Bus Service via Serial Port
 * 
 * TODO: Implement actual serial communication with CAN adapter
 * Requires: serialport package
 */
export class SerialCANBusService extends CANBusService {
  async connect(): Promise<void> {
    throw new Error('Serial CAN bus not yet implemented - use MockCANBusService for now');
  }
  
  async disconnect(): Promise<void> {
    throw new Error('Not implemented');
  }
  
  isConnected(): boolean {
    return false;
  }
  
  async sendMessage(_message: CANFrame): Promise<void> {
    throw new Error('Not implemented');
  }
}
