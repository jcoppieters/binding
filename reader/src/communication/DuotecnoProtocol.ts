/**
 * Duotecno Protocol Implementation
 * Based on protocol.ts from the Ionic/Angular app
 * 
 * Message Format: [cmd, ...params]\n
 * - Messages are arrays of numbers
 * - Converted to comma-separated strings wrapped in [...]
 * - Terminated with line feed (\n)
 */

import { UnitType } from '../models/binding';

export type Message = number[];

export enum Cmd {
  SetSensorValue = 7,
  Internal = 9,

  SetBasicAudio = 159,
  SetExtendedAudio = 208,
  SetAVMatrix = 202,

  SetSwitch = 163,
  SetDimmer = 162,
  SetControl = 168,
  SetMotor = 182,
  SetSensor = 136,

  SetDateTime = 170,

  Login = 214,
  Heartbeat = 215,
  DatabaseInfo = 209,
  SetSchedule = 217,
  Register = 224,
  
  // TCP Protocol Message Wrapper (FC_PROTOCOLMSG)
  FC_PROTOCOLMSG = 245,   // 0xF5 - Wrapper for CAN bus messages over TCP
  OBJECT_CANBUS_MSG = 1,   // Object type for CAN bus messages
  ATTR_SUBSCRIBE_CANBUS_MSG = 2,  // Attribute to subscribe/unsubscribe to CAN bus events
  
  // CAN bus function codes (sent wrapped in FC_PROTOCOLMSG)
  FC_NODECONFIG = 148,   // 0x94 - Set node config / unlock for programming
  FC_NODESETNAME = 140,  // 0x8C - Set node name
  FC_UNITSETNAME = 139,  // 0x8B - Set unit name
}

export enum Rec {
  DBInfo = 0,
  NodeInfo = 1,
  UnitInfo = 2,

  Internal = 9,

  ErrorMessage = 17,
  ConnectStatus = 67,
  AudioStatus = 23,
  AutoStatus = 48,
  AudioExtendedStatus = 70,
  TimeDateStatus = 71,
  HeartbeatStatus = 72,
  ScheduleStatus = 73,
  NodeMgtInfo = 74,
  Register = 77,

  Info = 64,

  Mood = 4,
  Dimmer = 5,
  Switch = 6,
  Sensor = 7,
  Motor = 38,
  Macro = 69,
  
  // Protocol message wrapper (EV_PROTOCOLMSG)
  EV_PROTOCOLMSG = 122,  // 0x7A - Wrapper for CAN bus event responses
}

export interface CommRecord {
  rest: string;
  isStatus: boolean;
  message: Message | null;
  cmd: number;
  method: number;
  raw: string;
}

export const DuotecnoProtocol = {
  /**
   * Helper: Convert string to byte array
   */
  stringToBytes(str: string): number[] {
    return str.split('').map(c => c.charCodeAt(0));
  },

  /**
   * Helper: Convert byte array to string
   */
  bytesToString(bytes: number[], offset: number): string {
    const length = bytes[offset];
    return bytes.slice(offset + 1, offset + length + 1)
      .map(val => String.fromCharCode(val))
      .join('');
  },

  /**
   * Helper: Make 16-bit word from two bytes
   */
  makeWord(arr: number[], at: number): number {
    return arr[at + 0] * 256 + arr[at + 1];
  },

  /**
   * Helper: Make 32-bit long from four bytes
   */
  makeLong(arr: number[], at: number): number {
    return arr[at + 0] * 256 * 256 * 256 +
           arr[at + 1] * 256 * 256 +
           arr[at + 2] * 256 +
           arr[at + 3];
  },

  /**
   * Check if a command is a status response
   */

  isStatus: function(cmd: number): boolean {
    return (cmd === Rec.Mood)   ||
           (cmd === Rec.Dimmer) ||
           (cmd === Rec.Switch) ||
           (cmd === Rec.Sensor) ||
           (cmd === Rec.Motor)  ||
           (cmd === Rec.Macro);
  },

  /**
   * Parse next message from buffer
   */
  nextMessage(buffer: string): CommRecord {
    const nextRec: CommRecord = { 
      rest: buffer, 
      isStatus: false, 
      message: null, 
      cmd: 0, method: 0,
      raw: "" 
    };

    // Find start of message [
    const begin = buffer.indexOf('[');
    if (begin < 0) {
      nextRec.rest = "";
      return nextRec;
    } else if (begin !== 0) {
      nextRec.rest = buffer.substring(begin);
    }

    if (nextRec.rest.length > 0) {
      // Find end of message ]
      let end = nextRec.rest.indexOf(']');

      if (end >= 0) {
        // Extract message (discard [ and ])
        nextRec.raw = nextRec.rest.substring(1, end);

        // Delete used message from buffer (including trailing LF if present)
        if ((end < nextRec.rest.length) && (nextRec.rest.charCodeAt(end + 1) === 0x0A)) {
          end++;
        }
        nextRec.rest = nextRec.rest.substring(end + 1);

        // Convert to array of numbers
        nextRec.message = nextRec.raw.split(',').map(c => parseInt(c));

        // Get command byte
        nextRec.cmd = nextRec.message[0];
        nextRec.method = nextRec.message[1];
        nextRec.isStatus = this.isStatus(nextRec.cmd);
      }
    }

    return nextRec;
  },

  /**
   * Format message for sending
   */
  formatMessage(data: Message | string): string {
    if (data instanceof Array) {
      data = data.join(",");
    }

    if (typeof data === "string") {
      // Add enclosing [...] if not present
      if (data[0] !== "[") {
        data = "[" + data + "]";
      }
      // Append line feed
      return data + String.fromCharCode(10);
    }

    throw new Error("Invalid message format");
  },

  /**
   * Build login message
   */
  buildLogin(password: string): Message {
    password = password || "";
    return [Cmd.Login, 3, password.length, ...this.stringToBytes(password)];
  },

  loginOK(data: Buffer): boolean {
    // [67,3,1] = login successful, [67,3,0] = login failed
    return ((data[0] === 91) &&     // [
            (data[1] === 54) && (data[2] === 55) && // "67"
            (data[3] === 44) &&   // ","
            (data[4] === 51) &&   // "3"
            (data[5] === 44) &&   // ","
            (data[6] === 49) &&   // login OK = "1", login failed = "0" 
            (data[7] === 93));    // ]
  },

  /**
   * Build disconnect message
   */
  buildDisconnect(): Message {
    return [Cmd.Login, 0];
  },

  /**
   * Build heartbeat message
   */
  buildHeartbeat(): Message {
    return [Cmd.Heartbeat, 1];
  },

  /**
   * Subscribe/unsubscribe to CAN bus events over TCP
   * Required to receive event responses (like EV_NODENAME) wrapped in EV_PROTOCOLMSG
   * 
   * @param subscribe - true to subscribe, false to unsubscribe
   * @returns Message in format [FC_PROTOCOLMSG, ATTR_SUBSCRIBE_CANBUS_MSG, state]
   */
  buildCANBusSubscribe(subscribe: boolean = true): Message {
    return [Cmd.FC_PROTOCOLMSG, Cmd.ATTR_SUBSCRIBE_CANBUS_MSG, subscribe ? 1 : 0];
  },

  /**
   * Build database info request
   */
  buildDBInfo(): Message {
    return [Cmd.DatabaseInfo, 0];
  },

  /**
   * Build node info request
   */
  buildNodeInfo(nodeIndex: number): Message {
    return [Cmd.DatabaseInfo, 1, nodeIndex];
  },

  /**
   * Build unit info request
   */
  buildUnitInfo(nodeAddress: number, unitIndex: number): Message {
    return [Cmd.DatabaseInfo, 2, nodeAddress, unitIndex];
  },

  /**
   * Build unit status request
   * Format: [209, 3, nodeAddr, unitAddr, unitType]
   */
  buildUnitStatus(nodeAddress: number, unitAddress: number, unitType: number): Message {
    return [Cmd.DatabaseInfo, 3, nodeAddress, unitAddress, unitType];
  },

  /**
   * Build switch control message
   */
  buildSetSwitch(nodeAddress: number, unitAddress: number, on: boolean): Message {
    return [Cmd.SetSwitch, on ? 3 : 2, nodeAddress, unitAddress]; // 3=on, 2=off
  },

  /**
   * Build control/virtual unit message (buttons, inputs, moods)
   * @param value - if < 0: pulse event (method 2), else: long toggle with data 0/1 (method 3)
   */
  buildSetControl(nodeAddress: number, unitAddress: number, value: number): Message {
    if (value < 0) {
      return [Cmd.SetControl, 2, nodeAddress, unitAddress]; // 2=PULS (short pulse)
    } else {
      return [Cmd.SetControl, 3, nodeAddress, unitAddress, value ? 1 : 0]; // 3=LONG (toggle event)
    }
  },

  /**
   * Build dimmer control message
   * @param value - <0 = ON/restore last value (method 10), 0 = OFF (method 9), 1-99 = dim to value (method 3)
   */
  buildSetDimmer(nodeAddress: number, unitAddress: number, value: number): Message {
    if (value < 0) {
      return [Cmd.SetDimmer, 10, nodeAddress, unitAddress]; // 10=ON (restore last value)
    } else if (value === 0) {
      return [Cmd.SetDimmer, 9, nodeAddress, unitAddress]; // 9=OFF
    } else {
      // Clamp value between 1-99
      const clampedValue = Math.max(1, Math.min(99, value));
      return [Cmd.SetDimmer, 3, nodeAddress, unitAddress, clampedValue]; // 3=ATTR_VALUE (dim to value)
    }
  },

  /**
   * Build sensor control message (basic - select preset)
   */
  buildSetSensor(nodeAddress: number, unitAddress: number, preset: number): Message {
    return [Cmd.SetSensor, 13, nodeAddress, unitAddress, preset]; // Method=13 (select preset)
  },

  /**
   * Build sensor temperature setpoint message
   * @param value - 16-bit temperature value (temp * 10, as signed 16-bit)
   */
  buildSetSensorValue(nodeAddress: number, unitAddress: number, value: number): Message {
    // SetSensorValue uses different command (7) vs SetSensor (136)
    const msb = Math.floor(value / 256);
    const lsb = value % 256;
    return [Cmd.SetSensorValue, 10, nodeAddress, unitAddress, UnitType.SENS, msb, lsb];
  },

  /**
   * Build sensor preset setpoint message (set temperature for a specific preset)
   * @param preset - preset number (0-3)
   * @param value - 16-bit temperature value
   */
  buildSetPreset(nodeAddress: number, unitAddress: number, preset: number, value: number): Message {
    const msb = Math.floor(value / 256);
    const lsb = value % 256;
    return [Cmd.SetSensor, 1, nodeAddress, unitAddress, preset, msb, lsb];
  },

  /**
   * Build sensor inc/dec preset setpoint message
   */
  buildIncDecPreset(nodeAddress: number, unitAddress: number, inc: boolean): Message {
    return [Cmd.SetSensor, inc ? 5 : 6, nodeAddress, unitAddress]; // 5=inc, 6=dec
  },

  /**
   * Build sensor on/off message
   */
  buildSensorOnOff(nodeAddress: number, unitAddress: number, on: boolean): Message {
    return [Cmd.SetSensor, 3, nodeAddress, unitAddress, on ? 1 : 0];
  },

  /**
   * Build HVAC control message (temperature sensor advanced control)
   * @param mode - 'workingmode' | 'fanspeed' | 'swingangle' | 'swingmode'
   * @param value - mode-specific value
   */
  buildHVAC(nodeAddress: number, unitAddress: number, mode: string, value: number): Message {
    // Map mode to method number
    const sensorMethods: { [key: string]: number } = {
      workingmode: 16,
      fanspeed: 17,
      swingangle: 18,
      swingmode: 19
    };
    
    const method = sensorMethods[mode];
    if (!method) {
      throw new Error(`Unknown HVAC mode: ${mode}`);
    }

    if (value < 0) value = 0; // -1 means off (not changeable)

    if (mode === 'swingangle' || mode === 'swingmode') {
      // These need 16-bit value
      const msb = Math.floor(value / 256);
      const lsb = value % 256;
      return [Cmd.SetSensor, method, nodeAddress, unitAddress, msb, lsb];
    } else {
      return [Cmd.SetSensor, method, nodeAddress, unitAddress, value];
    }
  },

  /**
   * Build motor control message
   */
  buildSetMotor(nodeAddress: number, unitAddress: number, action: number): Message {
    return [Cmd.SetMotor, action, nodeAddress, unitAddress]; // Method=action: 3=stop, 4=up, 5=down
  },

  /**
   * Build access level set message
   */
  buildSetAccessLevel(password: string): Message {
    if (password.length !== 4 || !/^\d{4}$/.test(password)) {
      throw new Error('Access password must be exactly 4 digits');
    }
    const passwordBytes = password.split('').map(c => c.charCodeAt(0));
    return [222, 3, ...passwordBytes]; // FC_ACCESSLEVELSET, ATTR_CODE
  },

  /**
   * Build start task message
   */
  buildStartTask(password: string, taskType: number = 0x09): Message {
    if (password.length !== 4 || !/^\d{4}$/.test(password)) {
      throw new Error('Task password must be exactly 4 digits');
    }
    const passwordBytes = password.split('').map(c => c.charCodeAt(0));
    return [212, taskType, 1, ...passwordBytes]; // FC_APPLICATIONTASKSET
  },



  /**
   * Build CAN bus message wrapper for TCP protocol
   * 
   * C++ reference: CSocketProtocolMessage_pr2::setCanBusMessage
   * Format: [FC_PROTOCOLMSG, OBJECT_CANBUS_MSG, nodeAddr, unitAddr, srcUnit, srcNode, APDUHeader, functionCode, dataLength, ...data]
   * 
   * APDU Header calculation:
   * - APDU_BASIC_PACKET (0x40) if dataLength <= 6
   * - APDU_EXPLICIT_INVOKE (0x28) for commands requiring response
   * - APDU_IMPLICIT_INVOKE (0x20) for commands not requiring response
   */
  buildCANBusMessage(nodeAddress: number, unitAddress: number, functionCode: number, data: number[]): Message {
    const APDU_BASIC_PACKET = 0x40;     // Basic (non-segmented) packet
    const APDU_EXPLICIT_INVOKE = 0x28;  // Requires response
    
    // Calculate APDU header: basic packet + explicit invoke for commands requiring acknowledgment
    const apduHeader = (data.length <= 6 ? APDU_BASIC_PACKET : 0) + APDU_EXPLICIT_INVOKE;
    
    return [
      Cmd.FC_PROTOCOLMSG,      // 245 - Protocol message wrapper
      Cmd.OBJECT_CANBUS_MSG,   // 1 - CAN bus object type
      nodeAddress,             // Destination node address
      unitAddress,             // Destination unit address (0xFF for node-level commands)
      0xFF,                    // Source unit (always 0xFF for client)
      0xFF,                    // Source node (always 0xFF for client)
      apduHeader,              // APDU header
      functionCode,            // CAN function code
      data.length,             // Data length
      ...data                  // Data bytes
    ];
  },

  /**
   * Build node config message (FC_NODECONFIG = 0x94 = 148)
   * Used to set APPL_UNLOCK flag for programming mode
   * Format: Wrapped in FC_PROTOCOLMSG with [nodeConfig, dllAccessControl] as data
   */
  buildNodeConfig(nodeAddress: number, nodeConfig: number, dllAccessControl: number): Message {
    return this.buildCANBusMessage(nodeAddress, 0xFF, Cmd.FC_NODECONFIG, [nodeConfig, dllAccessControl]);
  },

  /**
   * Build node name upload message (FC_NODESETNAME = 0x8C = 140)
   * Format: Wrapped in FC_PROTOCOLMSG with [segmentIndex, ...nameBytes] as data
   */
  buildNodeNameUpload(nodeAddress: number, segmentIndex: number, nameData: Buffer): Message {
    const dataBytes = Array.from(nameData);
    return this.buildCANBusMessage(nodeAddress, 0xFF, Cmd.FC_NODESETNAME, [segmentIndex, ...dataBytes]);
  },

  /**
   * Build unit name upload message (FC_UNITSETNAME = 0x8B = 139)
   * Format: Wrapped in FC_PROTOCOLMSG with [segmentIndex, ...nameBytes] as data
   */
  buildUnitNameUpload(nodeAddress: number, unitAddress: number, segmentIndex: number, nameData: Buffer): Message {
    const dataBytes = Array.from(nameData);
    return this.buildCANBusMessage(nodeAddress, unitAddress, Cmd.FC_UNITSETNAME, [segmentIndex, ...dataBytes]);
  },
};
