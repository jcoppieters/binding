# TCP Protocol Wrapper Fix

## Problem

Node name uploads were failing with error `[17, 12, 140]` (INVALID_MESSAGE_CODE for FC_NODESETNAME).

## Root Cause

**The TCP protocol requires ALL CAN bus messages to be wrapped in a `FC_PROTOCOLMSG` (245) packet.**

We were sending:
```
[140, nodeAddress, index, ...nameBytes]  // Direct FC_NODESETNAME - WRONG!
```

But the TCP server expects:
```
[245, 1, nodeAddress, 0xFF, 0xFF, 0xFF, 0x68, 140, dataLength, index, ...nameBytes]
```

This is documented in the C++ code: `CTCPSocketCommObject::MessageTransfer` → `sendCanBusMsg` → `CSocketProtocolMessage_pr2::setCanBusMessage`

## Solution Implemented

### 1. Added FC_PROTOCOLMSG Wrapper (DuotecnoProtocol.ts)

```typescript
enum Cmd {
  FC_PROTOCOLMSG = 245,     // 0xF5 - Wrapper for CAN messages
  OBJECT_CANBUS_MSG = 1,     // Object type identifier
  FC_NODESETNAME = 140,      // 0x8C
  FC_UNITSETNAME = 139,      // 0x8B
  FC_NODECONFIG = 148,       // 0x94
}

// New wrapper function
buildCANBusMessage(nodeAddress, unitAddress, functionCode, data): Message {
  const APDU_BASIC_PACKET = 0x40;      // For messages <= 6 bytes
  const APDU_EXPLICIT_INVOKE = 0x28;   // Requires acknowledgment
  
  const apduHeader = (data.length <= 6 ? APDU_BASIC_PACKET : 0) + APDU_EXPLICIT_INVOKE;
  
  return [
    Cmd.FC_PROTOCOLMSG,       // 245
    Cmd.OBJECT_CANBUS_MSG,    // 1
    nodeAddress,              // Destination node
    unitAddress,              // Destination unit (0xFF for node-level)
    0xFF,                     // Source unit (always 0xFF)
    0xFF,                     // Source node (always 0xFF)
    apduHeader,               // 0x68 for basic packets
    functionCode,             // FC_NODESETNAME, FC_UNITSETNAME, etc.
    data.length,              // Data length
    ...data                   // Actual data
  ];
}
```

### 2. Updated Message Builders

All FC command builders now use the wrapper:

```typescript
// Node name upload: wraps [segmentIndex, ...nameBytes]
buildNodeNameUpload(nodeAddress, segmentIndex, nameData): Message {
  const dataBytes = Array.from(nameData);
  return this.buildCANBusMessage(nodeAddress, 0xFF, Cmd.FC_NODESETNAME, [segmentIndex, ...dataBytes]);
}

// Unit name upload: wraps [segmentIndex, ...nameBytes]
buildUnitNameUpload(nodeAddress, unitAddress, segmentIndex, nameData): Message {
  const dataBytes = Array.from(nameData);
  return this.buildCANBusMessage(nodeAddress, unitAddress, Cmd.FC_UNITSETNAME, [segmentIndex, ...dataBytes]);
}

// Node config (unlock): wraps [nodeConfig, dllAccessControl]
buildNodeConfig(nodeAddress, nodeConfig, dllAccessControl): Message {
  return this.buildCANBusMessage(nodeAddress, 0xFF, Cmd.FC_NODECONFIG, [nodeConfig, dllAccessControl]);
}
```

### 3. Updated Send Method (TCPCANBusService.ts)

```typescript
async sendMessage(frame: CANFrame): Promise<void> {
  const unitAddress = frame.unitAddress ?? 0xFF;
  const dataBytes = Array.from(frame.data);
  
  // Build wrapped CAN message using protocol helper
  const message = DuotecnoProtocol.buildCANBusMessage(
    frame.nodeAddress,
    unitAddress,
    frame.functionCode,
    dataBytes
  );
  
  this.writeMessage(message);
}
```

### 4. Updated Response Handling

Responses also come wrapped in `EV_PROTOCOLMSG` (122):

```typescript
enum Rec {
  EV_PROTOCOLMSG = 122,  // 0x7A - Wrapper for CAN event responses
}

processMessage(record: CommRecord): void {
  switch (record.cmd) {
    case Rec.EV_PROTOCOLMSG:
      // Format: [122, 1, nodeAddr, unitAddr, srcUnit, srcNode, APDUheader, eventCode, dataLength, ...data]
      this.handleProtocolCANEvent(msg);
      break;
  }
}

handleProtocolCANEvent(msg: Message): void {
  const objectType = msg[1];      // Should be 1 (OBJECT_CANBUS_MSG)
  const nodeAddress = msg[2];
  const unitAddress = msg[3];
  const eventCode = msg[7];       // EV_NODENAME, EV_NODECONFIG, etc.
  const dataLength = msg[8];
  const data = msg.slice(9, 9 + dataLength);
  
  this.emit('response', { nodeAddress, eventCode, data, timestamp: Date.now() });
}
```

## APDU Header Calculation

The APDU header indicates packet type and service:

- **APDU_BASIC_PACKET (0x40)**: Used when data.length <= 6 bytes
- **APDU_EXPLICIT_INVOKE (0x28)**: Command requires acknowledgment
- **Result**: For name uploads (typically 5 bytes), header = 0x40 + 0x28 = **0x68**

## Message Format Examples

### Node Name Upload (segment 0, "Test ")
```
[
  245,      // FC_PROTOCOLMSG
  1,        // OBJECT_CANBUS_MSG
  3,        // nodeAddress = 0x03
  255,      // unitAddress = 0xFF (node-level)
  255,      // srcUnit = 0xFF
  255,      // srcNode = 0xFF
  104,      // APDUheader = 0x68
  140,      // FC_NODESETNAME = 0x8C
  6,        // dataLength = 6
  0,        // segmentIndex = 0
  84,       // 'T'
  101,      // 'e'
  115,      // 's'
  116,      // 't'
  32        // ' '
]
```

### Expected Response
```
[
  122,      // EV_PROTOCOLMSG
  1,        // OBJECT_CANBUS_MSG
  3,        // nodeAddress = 0x03
  255,      // unitAddress = 0xFF
  3,        // srcUnit = 0x03
  0,        // srcNode = 0x00
  16,       // APDUheader (APDU_RESULT)
  10,       // EV_NODENAME = 0x0A
  1,        // dataLength = 1
  0         // status = 0 (success)
]
```

## Testing

Test the implementation:

```bash
# Start server (if not running)
npm run dev

# Test connection
```http
POST http://localhost:3000/api/hardware/connect
Content-Type: application/json

{"type":"tcp","host":"192.168.0.97","port":5001,"password":"GMTecno"}
```

# Test name upload
```http
POST http://localhost:3000/api/hardware/node-name
Content-Type: application/json

{"nodeAddress":3,"name":"Test Node"}
```

# Test unit name upload
```http
POST http://localhost:3000/api/hardware/unit-name
Content-Type: application/json

{"nodeAddress":3,"unitAddress":35,"name":"Bulbje"}
```

Expected console output:
```
[TCP] TX CAN: FC=8C Node=03 Unit=FF Data=[00 54 65 73 74 20]
[TCP] RX CAN Event: 0A Node=03 Unit=FF [00]  ← Success!
```

## C++ References

Key files from binding software that document this protocol:

1. `/src/DTBindingSoftware/src/communication/tcpsocket/tcpsocketbase/tcpsocketmessages.cpp` (line 549-573)
   - `CSocketProtocolMessage_pr2::setCanBusMessage()` - Shows wrapper format

2. `/src/DTBindingSoftware/src/communication/tcpsocket/tcpsocketbase/tcpsocketmessages_def.h` (line 38-52)
   - Defines `ProtocolMsg::FC_PROTOCOLMSG` (245) and `OBJECT_CANBUS_MSG` (1)

3. `/src/DTBindingSoftware/src/communication/tcpsocket/tcpsocketmsgapduheader.cpp` (line 16-200)
   - `getADPUHeader()` - Calculates APDU header based on function code and data length

4. `/src/DTBindingSoftware/src/apdu_def.h`
   - Defines APDU_BASIC_PACKET (0x40), APDU_EXPLICIT_INVOKE (0x28), etc.

## Key Insights

1. **TCP != Direct CAN**: The TCP protocol is NOT a direct CAN bus interface - it's a wrapper protocol
2. **All FC commands**: ANY CAN function code must be wrapped in FC_PROTOCOLMSG when sent over TCP
3. **All EV responses**: Likewise, all event codes come back wrapped in EV_PROTOCOLMSG
4. **APDU headers**: Critical for proper message routing and acknowledgment handling
5. **Master units**: The "master" TCP server handles the wrapping/unwrapping and routes to CAN bus

## Next Steps

With this fix in place, you should be able to:

✅ Upload node names successfully
✅ Upload unit names successfully  
✅ Send node config (APPL_UNLOCK) if needed
✅ Receive proper event responses (EV_NODENAME, EV_NODECONFIG, etc.)

The protocol is now correctly implemented according to the C++ binding software specification.
