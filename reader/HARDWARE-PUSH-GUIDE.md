# Hardware Push Feature - User Guide

## Overview

This feature allows you to push node and unit names from the binding editor directly to the Duotecno hardware via CAN bus communication.

## Features

### 1. Rename with Optional Hardware Push

When renaming a node or unit, you'll be prompted to choose:
- **✅ Yes** - Save to `nodedatabase.txt` AND push to hardware
- **❌ No** - Save to `nodedatabase.txt` only

### 2. Dedicated Hardware Push Buttons

Each node and unit detail view has a **📤 Push to Hardware** button that:
- Reads the current name from `nodedatabase.txt`
- Uploads it to the physical hardware device
- Confirms success or displays error

### 3. Bulk Operation

The **📤 Push All Names** button in the header:
- Uploads ALL node and unit names to hardware
- Displays progress (successful/failed counts)
- Takes several minutes for large installations

## API Endpoints

### Rename with Hardware Push

```http
POST /api/nodes/:nodeAddress/rename
Content-Type: application/json

{
  "name": "New Name",
  "pushToHardware": true  // Optional, defaults to false
}
```

**Response:**
```json
{
  "success": true,
  "hardwarePush": {
    "success": true,
    "message": "Name pushed to hardware successfully"
  }
}
```

### Push Individual Name

```http
POST /api/nodes/:nodeAddress/push-name
```

```http
POST /api/nodes/:nodeAddress/units/:unitAddress/push-name
```

**Response:**
```json
{
  "success": true,
  "message": "Name \"Living Room\" pushed to hardware successfully"
}
```

### Push All Names

```http
POST /api/hardware/push-all-names
```

**Response:**
```json
{
  "totalNodes": 12,
  "successfulNodes": 12,
  "totalUnits": 45,
  "successfulUnits": 43,
  "errors": [
    "Failed to upload unit name for unit 0x05 on node 0x03: Timeout waiting for acknowledgement"
  ]
}
```

### Hardware Status

```http
GET /api/hardware/status
```

**Response:**
```json
{
  "connected": true,
  "type": "mock",  // or "serial"
  "port": null,
  "operationsCount": 24
}
```

## CAN Bus Protocol

### Node Name Upload (FC_NODESETNAME = 0x8C)

**CAN Frame Structure:**
```
Byte 0: 0x8C          (Function Code)
Byte 1: Node Address
Byte 2: Segment Index (0-based)
Byte 3-7: 5 bytes of hex-encoded name data
```

**Process:**
1. Name is formatted to max 32 characters, null-terminated
2. ASCII bytes are converted to hex (e.g., 'A' = 0x41)
3. Data is split into 5-byte segments
4. Each segment is sent with increasing index
5. Hardware responds with EV_NODENAME (0x0A) event

### Unit Name Upload (FC_UNITSETNAME = 0x8B)

**CAN Frame Structure:**
```
Byte 0: 0x8B          (Function Code)
Byte 1: Node Address
Byte 2: Unit Address
Byte 3: Segment Index (0-based)
Byte 4-7: 4 bytes of hex-encoded name data
```

**Note:** Unit names have one less byte per segment because the unit address occupies byte 2.

### Retry Logic

- **Max Attempts:** 3
- **Timeout per Attempt:** 6 seconds
- **Expected Acknowledgement:** EV_NODENAME (0x0A) or EV_UNITNAME (0x09)

## Mock vs Real Hardware

### Mock CAN Bus (Default)

The server starts with a `MockCANBusService` that:
- ✅ Simulates hardware responses with 100ms delay
- ✅ Logs all CAN traffic to console
- ✅ Always returns successful acknowledgements
- ✅ Perfect for development and testing

### Serial CAN Bus (Phase 1 - Not Yet Implemented)

To connect to real hardware:
```http
POST /api/hardware/connect
Content-Type: application/json

{
  "port": "/dev/ttyUSB0",  // or "COM3" on Windows
  "baudRate": 115200
}
```

## Error Handling

### Common Errors

1. **Timeout waiting for acknowledgement**
   - Hardware not connected or not responding
   - CAN bus communication issue
   - Wrong node/unit address

2. **Name too long**
   - Max 32 characters for nodes
   - Max 32 characters for units
   - Automatically truncated if longer

3. **Invalid address**
   - Node address must be 0x00-0xFF
   - Unit address must be 0x00-0xFF

### Debugging

Enable CAN bus logging in the browser console:
```javascript
// All CAN messages are logged by MockCANBusService
// Look for lines like:
Mock CAN: TX 8C 03 00 48 65 6C 6C 6F  // Sending "Hello" segment 0
Mock CAN: RX 0A 03 00 00 00 00 00 00  // Received acknowledgement
```

## Implementation Details

### Files

- **Backend:**
  - `src/communication/CANBusService.ts` - Abstract CAN bus layer
  - `src/services/NodeNamingService.ts` - High-level name upload service
  - `src/server-enhanced.ts` - REST API endpoints

- **Frontend:**
  - `public/app.js` - UI functions (pushNodeNameToHardware, pushUnitNameToHardware, pushAllNamesToHardware)
  - `public/index.html` - "Push All Names" button
  - `public/styles.css` - Hardware button styling

### Reference Implementation

The TypeScript implementation is a direct port of the C++ code:
- `src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp` - FSM state machine
- `src/DTBindingSoftware/src/TXNodeMessage.cpp` - SetNodeName() function
- `src/DTBindingSoftware/src/TXUnitMessage.cpp` - SetUnitName() function

### Testing

1. Start the server:
   ```bash
   cd reader
   npm run dev
   ```

2. Open browser: `http://localhost:3000`

3. Test rename with hardware push:
   - Navigate to a node detail view
   - Click **✏️ Rename**
   - Enter new name
   - Click **✅ Yes** when prompted for hardware push
   - Check console for CAN traffic logs

4. Test dedicated push button:
   - Click **📤 Push to Hardware** in node detail view
   - Confirm the operation
   - Check console for success message

5. Test bulk operation:
   - Click **📤 Push All Names** in header
   - Confirm the operation (may take several minutes)
   - Review the summary report

## Testing through the API:

### Connect to TCP Master

```http
POST http://localhost:3000/api/hardware/connect
Content-Type: application/json

{"type":"tcp","host":"192.168.0.97","port":5001,"password":"GMTecno"}
```

### Upload Node Name

```http
POST http://localhost:3000/api/hardware/node-name
Content-Type: application/json

{"nodeAddress":3, "name":"Test node"}
```

### Upload Unit Name

```http
POST http://localhost:3000/api/hardware/unit-name
Content-Type: application/json

{"nodeAddress":3, "unitAddress":35, "name":"Bulbje"}
```
## Future Enhancements (Phase 1)

- [ ] Real serial port communication (replace MockCANBusService)
- [ ] Progress bar for bulk operations
- [ ] Automatic retry with exponential backoff
- [ ] Queue system for multiple concurrent operations
- [ ] Connection status indicator (connected/disconnected)
- [ ] Hardware discovery (scan for connected nodes)

## Related Documentation

- [HARDWARE-COMMUNICATION-OVERVIEW.md](./HARDWARE-COMMUNICATION-OVERVIEW.md) - Complete protocol reference
- [README-BINDING-EDITOR.md](./README-BINDING-EDITOR.md) - Binding editor feature overview
- [WEB-BINDING-EDITOR-GUIDE.md](./WEB-BINDING-EDITOR-GUIDE.md) - Web editor user guide

## Known Limitations

1. Mock hardware always succeeds (no error simulation yet)
2. No progress feedback during bulk operations
3. Serial port connection not implemented
4. No CAN bus monitoring/debugging UI
5. No transaction log or audit trail

## Support

For issues or questions:
1. Check browser console for error messages
2. Verify node/unit addresses are correct
3. Ensure hardware is connected and powered on
4. Review CAN bus logs for communication errors
