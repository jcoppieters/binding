# Duotecno Hardware Communication Overview

**Document Purpose**: Complete reference for all communication between binding software and Duotecno hardware/nodes

**Date**: April 2026  
**Status**: Based on legacy C++ codebase analysis

---

## Table of Contents

1. [Node/Unit Naming - Answering Your Question](#nodeunit-naming)
2. [Communication Architecture](#communication-architecture)
3. [CAN Bus Protocol](#can-bus-protocol)
4. [Function Codes Reference](#function-codes-reference)
5. [HTTP/HTTPS Remote API](#httphttps-remote-api)
6. [File Transfer Protocols](#file-transfer-protocols)
7. [Implementation Roadmap](#implementation-roadmap)

---

## Node/Unit Naming

### How Names Are Pushed to Hardware

**Your Question**: You stated that names are saved to `nodedatabase.txt` but not uploaded to hardware. However, we should have the possibility to push these changes to the hardware.

**Answer**: ✅ **YES, names CAN be pushed to hardware!** The legacy C++ code fully supports this.

### Implementation Details

#### 1. Node Name Upload

**Function Code**: `FC_NODESETNAME` (0x8c)

**Implementation**: [RenameNodeUnitsEngine.cpp](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp)

```cpp
// Pseudo-code flow:
void SendNodeName(BYTE nodeAddress) {
    // 1. Get name from nodedatabase.txt
    CString name = database->GetNodeName(nodeAddress);
    
    // 2. Format name (max 32 chars, null-terminated)
    BYTE data[UNIT_NAME_LENGTH];
    FormatData(name, data);
    
    // 3. Send in 5-byte chunks (CAN bus limitation)
    for (position = 0; position < length; position += 5) {
        BYTE chunk[5];
        ConvertToHex(data, chunk, 5);
        
        // MessageTransfer sends FC_NODESETNAME command
        CTXNodeMessage::SetNodeName(
            nodeAddress,
            position,      // index in name
            chunkLength,   // 5 bytes
            chunk          // hex-encoded data
        );
        
        // Wait for EV_NODENAME acknowledgement
        WaitForAck(timeout: 6 seconds);
    }
}
```

**Key Files**:
- [TXNodeMessage.cpp](../src/DTBindingSoftware/src/TXNodeMessage.cpp#L80-L92) - `SetNodeName()` 
- [RenameNodeUnitsEngine.cpp](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp#L1031-L1045) - FSM orchestration
- [nodemess.h](../src/DTBindingSoftware/src/nodemess.h#L113) - FC_NODESETNAME definition

#### 2. Unit Name Upload

**Function Code**: `FC_UNITSETNAME` (0x8b)

**Implementation**: [TXUnitMessage.cpp](../src/DTBindingSoftware/src/TXUnitMessage.cpp)

```cpp
void SendUnitName(BYTE nodeAddress, BYTE unitAddress) {
    // Similar to node names, but per unit
    CString name = database->GetUnitName(nodeAddress, unitAddress);
    
    // Send in chunks
    CTXUnitMessage::SetUnitName(
        nodeAddress,
        unitAddress,
        index,
        length,
        hexData
    );
    
    // Wait for EV_UNITNAME acknowledgement
}
```

**Key Files**:
- [TXUnitMessage.cpp](../src/DTBindingSoftware/src/TXUnitMessage.cpp#L75-L107) - `SetUnitName()`
- [RenameNodeUnitsEngine.cpp](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp#L820-L845) - `WriteUnitName()`

#### 3. Complete Workflow

The C++ software has a **Rename Engine State Machine** that:

1. **Reads** names from `nodedatabase.txt`
2. **Converts** ASCII to hex-encoded format
3. **Chunks** data (max 5 bytes per CAN frame due to 7-byte payload limit)
4. **Sends** via `FC_NODESETNAME` or `FC_UNITSETNAME`
5. **Waits** for acknowledgement: `EV_NODENAME` or `EV_UNITNAME`
6. **Retries** on timeout (max 3 attempts)
7. **Displays** progress messages to user

**See**: [RenameNodeUnitsEngine.h](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.h#L105) - `SendNodeName()` declaration

### What Gets Stored Where

| Data | nodedatabase.txt | Hardware Node Flash Memory |
|------|------------------|---------------------------|
| Node Name | ✅ Saved | ✅ Uploaded via FC_NODESETNAME |
| Unit Name | ✅ Saved | ✅ Uploaded via FC_UNITSETNAME |
| Node Address | ✅ Saved | ✅ Inherent (node's logical address) |
| Unit Type | ✅ Saved | ✅ Detected during browse |
| Binding Config | ✅ Saved | ✅ Uploaded via binding files |

### TypeScript Implementation TODO

To implement node/unit name upload in the TypeScript binding editor:

```typescript
// reader/src/services/NodeNamingService.ts (NEW FILE NEEDED)

export class NodeNamingService {
  async uploadNodeName(nodeAddress: number, name: string): Promise<void> {
    // 1. Format name (max 32 chars)
    const formatted = this.formatName(name);
    
    // 2. Send in 5-byte chunks
    for (let pos = 0; pos < formatted.length; pos += 5) {
      const chunk = formatted.slice(pos, pos + 5);
      
      // 3. Send CAN message
      await this.communication.sendMessage({
        nodeAddress,
        unitAddress: 0xFF, // broadcast unit
        functionCode: 0x8C, // FC_NODESETNAME
        length: chunk.length + 1,
        data: [pos, ...chunk] // index + data
      });
      
      // 4. Wait for acknowledgement
      await this.waitForEvent('EV_NODENAME', 6000);
    }
  }
  
  async uploadUnitName(
    nodeAddress: number, 
    unitAddress: number, 
    name: string
  ): Promise<void> {
    // Similar implementation with FC_UNITSETNAME (0x8B)
  }
  
  async uploadAllNames(): Promise<void> {
    // Iterate through all nodes & units from nodedatabase.txt
  }
}
```

**Dependencies Needed**:
1. Serial/CAN communication layer (not yet implemented)
2. Message framing (`MessageTransfer` equivalent)
3. Event acknowledgement system
4. Timeout/retry logic

---

## Communication Architecture

### 5 Communication Channels

```
┌─────────────────────────────────────────────────────────────┐
│                 Binding Software (PC/Mac)                    │
├─────────────────────────────────────────────────────────────┤
│  1. CAN Bus      │  Direct control of nodes/units           │
│  2. TCP/IP       │  Alternative LAN access (optional)        │
│  3. HTTP/HTTPS   │  Cloud server & remote configuration      │
│  4. FTP over CAN │  File transfers (bindings, firmware)      │
│  5. Serial Port  │  Physical CAN adapter/modem connection    │
└─────────────────────────────────────────────────────────────┘
           │                │              │
           ▼                ▼              ▼
    ┌──────────┐    ┌────────────┐   ┌──────────────┐
    │  Master  │◄──►│  Gateway   │◄─►│ Cloud Server │
    │   Node   │    │  (WiFi/IP) │   │ (HTTPS API)  │
    │ (0x03)   │    │            │   └──────────────┘
    └──────────┘    └────────────┘
         │
         ├──► CAN Bus Network
         │
    ┌────┴────┬──────────┬──────────┬──────────┐
    │         │          │          │          │
  Node      Node       Node       Node       Node
  (0x04)    (0x05)     (0x06)     (0x10)     (0xFC)
    │         │          │          │          │
  ┌─┴─┐     ┌─┴─┐      ┌─┴─┐      ┌─┴─┐      ┌─┴─┐
  Unit      Unit       Unit       Unit       Unit
  Unit      Unit       Unit       Unit       Unit
```

### Core Communication Class

**File**: [ComApplication.cpp](../src/DTBindingSoftware/src/ComApplication.cpp)

The `CComApplication` class is the **central hub** for all hardware communication:

```cpp
class CComApplication {
  // Primary method for sending messages to nodes
  BOOL MessageTransfer(
    BYTE nodeAddress,
    BYTE unitAddress,
    BYTE functionCode,
    BYTE dataLength = 0,
    CString data = ""
  );
  
  // Message handlers (60+ registered)
  CMessageHandler handlers[256];
  
  // Event dispatcher
  void ProcessReceivedMessage(CANFrame frame);
}
```

---

## CAN Bus Protocol

### Frame Format

```
┌──────────┬──────────┬──────────┬──────────┬──────────┬─────────┬──────────┐
│  Prefix  │   Node   │   Unit   │  FC Code │  Length  │  Data   │ Checksum │
│ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │ (1 byte) │(0-7 b)  │ (1 byte) │
└──────────┴──────────┴──────────┴──────────┴──────────┴─────────┴──────────┘

Example: Set dimmer to 50%
  [0x02][0x06][0x01][0xA2][0x03][0x03][0x32][0x00][0xXX]
   └─┬──┘└─┬──┘└─┬──┘└─┬──┘└─┬──┘└──────┬──────┘└─┬──┘
     │     │     │     │     │          │         └─── Checksum
     │     │     │     │     │          └───────────── Data (member, value MSB, LSB)
     │     │     │     │     └──────────────────────── Length = 3
     │     │     │     └────────────────────────────── FC_UNITDIMSET (0xA2)
     │     │     └──────────────────────────────────── Unit address 0x01
     │     └────────────────────────────────────────── Node address 0x06
     └──────────────────────────────────────────────── Prefix (protocol marker)
```

### Message Segmentation

**Problem**: CAN frames limited to 7 data bytes  
**Solution**: Multi-segment transfer for large data

Example: Node name "Living Room Master" (19 chars)
```
Segment 1: [index=0x00][chars 0-4]  → "Livin"
Segment 2: [index=0x05][chars 5-9]  → "g Roo"
Segment 3: [index=0x0A][chars 10-14] → "m Mas"
Segment 4: [index=0x0F][chars 15-18+\0] → "ter\0"
```

### Acknowledgement Protocol

Every command gets a response:
- ✅ **Success**: `EV_NODENAME` / `EV_UNITNAME` with `OBJECT_NAME_WRITE_OK`
- ❌ **Error**: `EV_MESSAGEERROR` with error code
- ⏱️ **Timeout**: 6 seconds → retry (max 3 times)

**Error Codes** ([nodemess.h](../src/DTBindingSoftware/src/nodemess.h#L238-L257)):
- `MESSAGE_ERROR_REFUSED_RUNNING_STATE` (0x00) - Node busy
- `MESSAGE_ERROR_INVALID` (0x01) - Malformed command
- `MESSAGE_ERROR_BUFFER_FULL` (0x02) - Node queue full
- ... (20+ error types)

---

## Function Codes Reference

### Complete FC_ Command List

**File**: [nodemess.h](../src/DTBindingSoftware/src/nodemess.h#L98-L189)

#### Node Management (0x80-0x9F)

| Code | Name | Purpose | Implementation |
|------|------|---------|----------------|
| 0x8A | FC_NODEREQUESTCONFIG | Get node configuration | [TXNodeMessage.cpp](../src/DTBindingSoftware/src/TXNodeMessage.cpp) |
| 0x8C | **FC_NODESETNAME** | **Upload node name** | [TXNodeMessage.cpp](../src/DTBindingSoftware/src/TXNodeMessage.cpp#L80) |
| 0x8E | FC_NODEREQUESTNAME | Download node name | [CTXBrowsePnpMessages](../src/DTBindingSoftware/src/TXNodeMessage.cpp) |
| 0x8F | FC_NODEREQUESTSTATUS | Get node status | Used for health checks |
| 0x90 | FC_BROWSEFORNODES | Discover nodes (browse) | PnP system |
| 0x91 | FC_BROWSEFORNEWNODES | Discover new nodes only | PnP incremental |
| 0x94 | FC_NODECONFIG | Configure node settings | Node setup |
| 0x9B | FC_NODERESET | Soft reset node | Restart |
| 0x9D | FC_NODEGETRESOURCES | Get capabilities | Resource negotiation |

#### Unit Control (0x83-0x89, 0xA2-0xA8)

| Code | Name | Purpose | Used For |
|------|------|---------|----------|
| 0x83 | FC_UNITREQUESTDIMSTATUS | Get dimmer status | Status polling |
| 0x84 | FC_UNITREQUESTCONFIG | Get unit config | Configuration |
| 0x86 | FC_UNITREQUESTSWITCHSTATUS | Get switch status | Status polling |
| 0x87 | FC_UNITREQUESTCONTROLSTATUS | Get control status | Status polling |
| 0x88 | FC_UNITSENSSET | Set sensor value | Climate control |
| 0x89 | FC_UNITREQUESTSENSSTATUS | Get sensor status | Climate monitoring |
| 0x8B | **FC_UNITSETNAME** | **Upload unit name** | [TXUnitMessage.cpp](../src/DTBindingSoftware/src/TXUnitMessage.cpp#L75) |
| 0x8D | FC_UNITREQUESTNAME | Download unit name | Name retrieval |
| 0x9A | FC_UNITRESET | Reset unit | Unit restart |
| 0xA2 | FC_UNITDIMSET | Set dimmer value | Light control |
| 0xA3 | FC_UNITSWITCHSET | Set switch on/off | Switch control |
| 0xA8 | FC_UNITCONTROLSET | Set control state | Button control |

#### Binding File Management (0x98-0x99)

| Code | Name | Purpose | Notes |
|------|------|---------|-------|
| 0x98 | FC_NODEBINDINGSFILECONTROL | Open/close/erase binding file | [CTXNodeBindingFileMessage](../src/DTBindingSoftware/src/TXNodeMessage.h#L59) |
| 0x99 | FC_NODEBINDINGENTRY | Read/write binding entries | Entry-by-entry transfer |

Commands:
- `FILE_OPEN` (0x01) - Open for write
- `FILE_ERASE` (0x03) - Erase existing bindings
- `WRITE_HEADER` (0x01) - Write binding header
- `WRITE_DATA` (0x03) - Write binding data
- `WRITE_DONE` (0x05) - Finalize transfer

#### FTP over CAN (0xAE-0xAF)

| Code | Name | Purpose | Since |
|------|------|---------|-------|
| 0xAE | FC_FTPFILECONTROL | Transfer files over CAN | v0561 |
| 0xAF | FC_REQUESTFTPFILECONTROL | Request file status | v0561 |

**Commands**: ([TXNodeMessage.h](../src/DTBindingSoftware/src/TXNodeMessage.h#L180-L237))
- `SendWriteOpen()` - Start upload
- `SendAppendData()` - Send data chunks
- `SendReadOpen()` - Start download
- `SendReadData()` - Request data chunks
- `SendClose()` - Finalize transfer
- `SendAbort()` - Cancel transfer

**Implementation**: [CTXNodeFTPFileControl](../src/DTBindingSoftware/src/TXNodeMessage.cpp)

#### Database Management (0xB5)

| Code | Name | Purpose | Since |
|------|------|---------|-------|
| 0xB5 | FC_DATABASEMANAGEMENT | Manage node databases | v0562 |

**Operations**: ([TXNodeMessage.h](../src/DTBindingSoftware/src/TXNodeMessage.h#L239-L268))
- `SendReload()` - Reload database from flash
- `SendErase()` - Erase database
- `SendSave()` - Save RAM → Flash
- `SendBackup()` - Create backup

Database IDs:
- Bindings
- IR Codes
- Screenmaps
- Mood scenes
- Schedule config

#### Memory Operations (0xFC-0xFF)

| Code | Name | Purpose | Use Case |
|------|------|---------|----------|
| 0xFC | FC_GETMEMORYDATA | Read raw memory | Debugging, diagnostics |
| 0xFD | FC_WRITEMEMORYDATA | Write raw memory | Direct flash/RAM access |
| 0xFE | FC_NODEEEPROMREAD | Read EEPROM | Legacy nodes |
| 0xFF | FC_NODEEEPROMWRITE | Write EEPROM | Legacy nodes |

**Warning**: Dangerous - can brick nodes if misused!

#### Specialized Units

**Audio** (0x9F, 0xA5):
- `FC_UNITAUDIOSET` - Control audio zones
- `FC_UNITREQUESTAUDIOSTATUS` - Get audio status

**Alarm** (0xCC, 0xCD):
- `FC_UNITALARMSET` - Arm/disarm alarm
- `FC_UNITREQUESTALARMSTATUS` - Get alarm status

**AV Matrix** (0xCA, 0xCB):
- `FC_UNITAVMATRIXSET` - Route audio/video
- `FC_UNITREQUESTAVMATRIXSTATUS` - Get matrix status

**IR Transmitter** (0xAD, 0xBF):
- `FC_UNITIRTXSET` - Send IR code
- `FC_UNITREQUESTIRTXSTATUS` - Get IR TX status

**See**: [nodemess.h](../src/DTBindingSoftware/src/nodemess.h) for complete list (60+ codes)

---

## HTTP/HTTPS Remote API

### Remote Configuration Server

**Base URL**: `https://connect.duotecno.eu:5099`

**Purpose**: 
- Remote binding file upload (via internet)
- Cloud-based node management
- Firmware downloads
- Authentication/authorization

### API Endpoints

**File**: [httprcfg_api.h](../src/DEPENDS/AppRemoteCfg_v2/src/dtapprcfgmfclib/httprcfg/httprcfg_api.h)

#### Authentication

```cpp
// Create client session
httprcfg_sendAuthCreateClient(
  server, port,
  clientID,      // UUID
  password,      // User password
  &credentials   // Returns access token
);

// Refresh token
httprcfg_sendAuthRefreshAccessCode(
  server, port,
  clientID,
  refreshToken,
  &credentials
);
```

**Endpoints**:
- `POST /auth/login`
- `POST /auth/refresh`

#### Node Information

```cpp
// Get node details
httprcfg_sendGetNodeInfo(
  server, port,
  &nodeInfo  // Response: address, type, version, etc.
);

// HTTPS version with auth
httpsrcfg_sendGetNodeInfo(
  server, port,
  clientID, accessToken,
  &nodeInfo
);
```

**Endpoint**: `GET /nodes/{nodeAddress}`

#### Binding File Upload

```cpp
// Upload binding file to specific node
httprcfg_sendUploadBindFile(
  server, port,
  nodeAddress,
  localFilePath,
  &result  // Success/error response
);

// HTTPS with auth
httpsrcfg_sendUploadBindFile(
  server, port,
  clientID, accessToken,
  nodeAddress,
  localFilePath,
  &result
);
```

**Endpoint**: `POST /bindings/upload/{nodeAddress}`

**Flow**:
1. Upload `.txt` binding file to cloud server
2. Server queues file for gateway
3. Gateway downloads and pushes to target node (via CAN)
4. Progress tracking via separate endpoint

#### Binding Actions

```cpp
// Start binding process
httprcfg_sendStartBindingsAction(
  server, port,
  bindAction,  // write/read/erase + node list
  &result
);

// Commands:
bindAction.action = "write";  // or "read", "erase"
bindAction.nodes = [0x03, 0x04, 0x05];
```

**Endpoints**:
- `POST /bindings/start`
- `GET /bindings/progress` - Real-time status
- `GET /bindings/log` - Operation log

#### Node Name Upload (NEW!)

```cpp
// Upload node/unit names to hardware
httprcfg_sendStartSendNames(
  server, port,
  actionList,  // List of name changes
  &result
);

// Action list format:
actionList.add({
  nodeAddress: 0x06,
  nodeName: "Living Room",
  units: [
    { address: 0x00, name: "Main Dimmer" },
    { address: 0x01, name: "Wall Switch" }
  ]
});

// Monitor progress
httprcfg_sendNamesGetProgressInfo(
  server, port,
  &progress  // % complete, current node, errors
);

// Get log
httprcfg_sendNamesGetLogInfo(
  server, port,
  &log  // Success/error messages
);
```

**Endpoints**:
- `POST /names/start` - Begin name upload
- `POST /names/cancel` - Stop operation
- `GET /names/progress` - Progress tracking
- `GET /names/log` - Operation log
- `GET /names/errors` - Error log

**Result**: Names are sent to gateway, which then pushes them to nodes via `FC_NODESETNAME` / `FC_UNITSETNAME` over CAN bus!

#### File Server

```cpp
// Upload arbitrary file
httprcfg_sendUploadFile(
  server, port,
  localFile,
  remotePath,  // e.g., "/dt/backup/bind/"
  &result
);

// List directory
httprcfg_sendRequestDirInfo(
  server, port,
  path,
  &dirInfo  // File list with metadata
);

// Delete file
httprcfg_sendDeleteFile(
  server, port,
  filePath,
  &result
);
```

**Use Cases**:
- Backup/restore binding files
- Firmware file management
- Configuration archival

#### Firmware Management

```cpp
// Get available firmware versions
httprcfg_sendGetCloudFirmwareVersions(
  server, port,
  &versionInfo  // List of available firmware
);

// Download firmware binary
httprcfg_sendDownloadFile(
  url,       // Firmware download URL
  destFile   // Local save path
);
```

**Endpoint**: `GET /firmware/versions`

### Request/Response Format

**All API calls use JSON**:

```json
// Request (POST /bindings/upload/6)
{
  "nodeAddress": 6,
  "fileName": "bind06.txt",
  "fileContent": "base64-encoded-data"
}

// Response (Success)
{
  "result": "success",
  "message": "Binding file uploaded"
}

// Response (Error)
{
  "result": "error",
  "errorCode": 1001,
  "message": "Node not reachable"
}
```

**Parsers**: [httprcfg_json.h](../src/DEPENDS/AppRemoteCfg_v2/src/dtapprcfgmfclib/httprcfg/httprcfg_json.h)

### Authentication Flow

```
1. User enters credentials
   └─► httprcfg_sendAuthCreateClient()

2. Server validates & returns tokens
   ├─► accessToken (short-lived, 1 hour)
   └─► refreshToken (long-lived, 30 days)

3. All API calls include accessToken
   └─► Authorization: Bearer {accessToken}

4. When accessToken expires
   └─► httprcfg_sendAuthRefreshAccessCode()
```

### Error Handling

**HTTP Status Codes** → `HttpRCfgResult_t`:

| HTTP Code | Result Enum | Meaning |
|-----------|-------------|---------|
| 200 | HTTPRCFG_OK | Success |
| 401 | HTTPRCFG_UNAUTHORIZED | Invalid auth |
| 404 | HTTPRCFG_NOT_FOUND | Resource missing |
| 500 | HTTPRCFG_SERVER_ERROR | Server failure |
| Timeout | HTTPRCFG_TIMEOUT | No response |
| Network | HTTPRCFG_CONNECTION_ERROR | Can't connect |

**Converter**: [httprcfg_cmn.cpp](../src/DEPENDS/AppRemoteCfg_v2/src/dtapprcfgmfclib/httprcfg/httprcfg_cmn.cpp#L88) - `convertHTTPStatusCode()`

---

## File Transfer Protocols

### 1. Binding File Upload (CAN Bus)

**Protocol**: Custom FTP-like over CAN  
**Implementation**: [BindingFileFSM.cpp](../src/DTBindingSoftware/src/BindingFileFSM.cpp)

#### State Machine Flow

```
1. OPEN FILE (FC_NODEBINDINGSFILECONTROL)
   ├─► Command: FILE_OPEN
   └─► Wait: EV_NODEBINDINGSFILECONTROL (FILE_OPENED)

2. ERASE EXISTING (optional)
   ├─► Command: FILE_ERASE
   └─► Wait: EV_NODEBINDINGSFILECONTROL (FILE_ERASED)

3. WRITE ENTRIES (loop for each binding)
   ├─► WRITE_HEADER (FC_NODEBINDINGENTRY)
   │   ├─► Data: [entry_nr_MSB][entry_nr_LSB][flags]
   │   └─► Wait: EV_NODEBINDINGENTRY (HEADER_WRITTEN)
   │
   ├─► WRITE_DATA (FC_NODEBINDINGENTRY)
   │   ├─► Data: [binding_condition][operators][...]
   │   └─► Wait: EV_NODEBINDINGENTRY (DATA_WRITTEN)
   │
   └─► Repeat for all entries

4. FINALIZE (FC_NODEBINDINGENTRY)
   ├─► Command: WRITE_DONE
   └─► Wait: EV_NODEBINDINGENTRY (WRITE_DONE)

5. CLOSE FILE (FC_NODEBINDINGSFILECONTROL)
   ├─► Command: FILE_CLOSE
   └─► Wait: EV_NODEBINDINGSFILECONTROL (FILE_CLOSED)
```

**Error Recovery**:
- Timeout → Retry current step (max 3 times)
- `SHARE_VIOLATION` → Wait & retry
- `BUFFER_FULL` → Slow down transmission
- Fatal error → Abort & close file

### 2. FTP over CAN (Generic Files)

**Since**: Firmware v0561  
**Used For**: Config files, mood scenes, screenmaps, IR codes

**Implementation**: [CTXNodeFTPFileControl](../src/DTBindingSoftware/src/TXNodeMessage.cpp)

#### Upload Flow

```cpp
// 1. Open for write
SendWriteOpen(nodeAddress, "moodconfig_06.txt");
// Response: EV_FTPFILECONTROLSTATUS (OPEN_OK)

// 2. Send data in chunks (max 5 bytes/chunk)
for (chunk in file) {
  SendAppendData(nodeAddress, position, data, length);
  // Response: EV_FTPFILECONTROLDATA (chunk acknowledged)
}

// 3. Close file
SendClose(nodeAddress);
// Response: EV_FTPFILECONTROLSTATUS (CLOSE_OK)
```

#### Download Flow

```cpp
// 1. Open for read
SendReadOpen(nodeAddress, "scheduleconfig_06.txt");
// Response: EV_FTPFILECONTROLSTATUS (file size)

// 2. Request data chunks
for (position = 0; position < fileSize; position += 5) {
  SendReadData(nodeAddress, position, 5);
  // Response: EV_FTPFILECONTROLDATA with data
}

// 3. Close
SendClose(nodeAddress);
```

**File Naming Convention**:
- Binding files: `bind{NodeHex}.txt` (e.g., `bind06.txt`)
- Mood config: `MoodConfig_{NodeHex}.txt`
- Schedule: `scheduleconfig_{NodeHex}.txt`
- IR codes: `irrxconfig_{NodeHex}.txt`

**Storage Path on Node**: `/dt/config/` (embedded Linux filesystem)

### 3. HTTP/HTTPS File Upload

**For remote operation via cloud**:

```
User PC                Cloud Server           Gateway           Target Node
  │                         │                    │                   │
  ├─ POST /bindings/upload ─►│                   │                   │
  │  (bind06.txt)            │                   │                   │
  │                          ├─ Store in queue ──►│                   │
  │                          │                   │                   │
  │◄─ 202 Accepted ──────────┤                   │                   │
  │                          │                   ├─ FTP over CAN ───►│
  │                          │                   │   (FC_FTPFILECONTROL)
  │                          │                   │                   ├─ Save
  │                          │                   │◄─ ACK ───────────┤
  │                          │◄─ Success ────────┤                   │
  │                          │                   │                   │
```

**Advantages**:
- Works over internet (no LAN required)
- Asynchronous (no need to stay connected)
- Multiple nodes in parallel
- Progress tracking

**Disadvantages**:
- Requires gateway with internet
- Slower than direct CAN
- Dependent on cloud availability

### 4. Firmware Update

**Protocol**: Two-stage (download + flash)

```
1. Download firmware from cloud
   └─► httprcfg_sendDownloadFile(firmwareURL, localFile)

2. Upload to node via CAN
   ├─► Open: SendWriteOpen(node, "/dt/firmware/update.bin")
   ├─► Transfer: SendAppendData() in 5-byte chunks
   └─► Close: SendClose()

3. Trigger update (@deprecated)
   └─► FC_NODE_FIRMWARE_UPDATE (custom code)
   
4. Node reboots and installs firmware
   └─► ~5-10 minutes for full update
```

**Safety**: Checksum verification, fallback to previous version on failure

---

## Implementation Roadmap

### Phase 1: CAN Bus Foundation ⭐ **PRIORITY**

**Goal**: Direct node/unit control over CAN bus

**Components**:
1. **Serial Communication Layer**
   - Port detection (macOS/Windows)
   - Baud rate configuration (typically 115200)
   - Frame encoding/decoding
   - Checksum validation

2. **MessageTransfer API**
   ```typescript
   class CANBusService {
     async sendCommand(
       nodeAddress: number,
       unitAddress: number,
       functionCode: number,
       data: Buffer
     ): Promise<CANResponse>;
     
     on(event: string, callback: Function): void;
     
     waitForEvent(
       eventType: number,
       timeout: number
     ): Promise<CANEvent>;
   }
   ```

3. **Basic Commands** (implement first):
   - `FC_NODEREQUESTSTATUS` (0x8F) - Node health check
   - `FC_NODEGETRESOURCES` (0x9D) - Get capabilities
   - `FC_UNITREQUESTCONFIG` (0x84) - Unit info
   - `FC_UNITREQUESTCONTROLSTATUS` (0x87) - Button status
   - **`FC_NODESETNAME` (0x8C)** - ✅ Upload node name
   - **`FC_UNITSETNAME` (0x8B)** - ✅ Upload unit name

4. **Event Handlers**:
   - `EV_NODESTATUS` - Node response
   - `EV_NODERESOURCES` - Capability data
   - `EV_UNITCONFIG` - Unit configuration
   - **`EV_NODENAME`** - Name upload ACK
   - **`EV_UNITNAME`** - Name upload ACK
   - `EV_MESSAGEERROR` - Error handling

**Deliverable**: Live status monitoring + name upload working

**Estimated Effort**: 2-3 weeks

---

### Phase 2: Node & Unit Naming ⭐ **YOUR REQUEST**

**Goal**: Complete name upload/download functionality

**Tasks**:
1. ✅ Read code (DONE - see above)
2. Implement `NodeNamingService`:
   ```typescript
   class NodeNamingService {
     async uploadNodeName(address: number, name: string);
     async uploadUnitName(node: number, unit: number, name: string);
     async downloadNodeName(address: number): Promise<string>;
     async downloadUnitName(node: number, unit: number): Promise<string>;
     async uploadAllNames(database: NodeDatabase);
   }
   ```

3. Add UI controls:
   - "Push Names to Hardware" button
   - Progress bar during upload
   - Error display
   - Retry logic

4. Update API:
   ```typescript
   // New endpoints
   POST /api/nodes/:address/name/upload
   POST /api/nodes/:address/units/:unit/name/upload
   GET /api/nodes/:address/name/download
   ```

5. Persistence:
   - Save to `nodedatabase.txt` (already working)
   - Push to hardware nodes (NEW)
   - Sync status indicator (local ≠ hardware)

**Success Criteria**:
- ✅ Rename node in UI → saves to file
- ✅ Click "Upload to Node" → sends via CAN
- ✅ Node stores name in flash
- ✅ After node reboot → name persists
- ✅ Download from hardware → populates UI

**Estimated Effort**: 1 week (after Phase 1 complete)

---

### Phase 3: Unit Control & Status

**Goal**: Control lights, switches, dimmers via CAN

**Commands**:
- `FC_UNITDIMSET` (0xA2) - Set dimmer level
- `FC_UNITSWITCHSET` (0xA3) - Switch on/off
- `FC_UNITSENSSET` (0x88) - Set thermostat

**UI**:
- Live dimmer slider
- Switch toggle
- Temperature control
- Real-time status updates

**Estimated Effort**: 2 weeks

---

### Phase 4: Binding File Upload

**Goal**: Push binding configuration to nodes

**Components**:
1. Binding File FSM (state machine)
2. CAN chunking (7-byte limit)
3. Progress tracking
4. Error recovery

**Flow**: Local `.txt` → CAN bus → node flash

**Estimated Effort**: 3 weeks (complex!)

---

### Phase 5: Node Discovery (PnP)

**Goal**: Auto-detect nodes on CAN bus

**Commands**:
- `FC_BROWSEFORNODES` (0x90)
- `FC_BROWSEFORNEWNODES` (0x91)
- `FC_PNPASSIGNLOGICALADDRESS` (0xB9)

**UI**: "Scan Network" button → auto-populates node list

**Estimated Effort**: 2 weeks

---

### Phase 6: HTTP Remote API

**Goal**: Cloud-based remote configuration

**Components**:
1. Authentication client
2. File upload API
3. Progress monitoring
4. Name upload via cloud (uses our FC_NODESETNAME backend!)

**Use Case**: Configure customer system from office

**Estimated Effort**: 2 weeks

---

### Phase 7: Advanced Features

**Optional enhancements**:
- Firmware updates
- Mood scene programming
- Scheduler configuration
- IR code learning
- DALI integration
- Gateway management

**Estimated Effort**: 4-8 weeks total

---

## Summary for Next Phases

### Immediate Next Steps (Your Question)

To implement **node/unit name upload to hardware**:

1. **Phase 1 Required First**:
   - ✅ CAN bus serial communication
   - ✅ `MessageTransfer()` equivalent
   - ✅ Event acknowledgement system

2. **Then Implement Phase 2**:
   - ✅ `FC_NODESETNAME` message builder
   - ✅ `FC_UNITSETNAME` message builder
   - ✅ Name chunking (5 bytes/message)
   - ✅ Retry logic on timeout
   - ✅ UI "Upload Names" button

3. **TypeScript Architecture**:
   ```
   reader/src/
     services/
       CanBusService.ts       ← Phase 1
       NodeNamingService.ts   ← Phase 2 (uses CanBusService)
     api/
       hardware-names.ts      ← REST endpoints
     models/
       HardwareNameSync.ts    ← Track upload status
   ```

### Key Takeaway

✅ **YES, names CAN be pushed to hardware!**  
✅ The C++ code fully implements `FC_NODESETNAME` and `FC_UNITSETNAME`  
✅ TypeScript implementation is feasible - just needs CAN bus layer first  
✅ See [RenameNodeUnitsEngine.cpp](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp) for reference

---

## Reference Files

### Core Communication
- [ComApplication.cpp](../src/DTBindingSoftware/src/ComApplication.cpp) - Central message hub
- [nodemess.h](../src/DTBindingSoftware/src/nodemess.h) - All FC_/EV_ codes
- [TXNodeMessage.cpp](../src/DTBindingSoftware/src/TXNodeMessage.cpp) - Node commands
- [TXUnitMessage.cpp](../src/DTBindingSoftware/src/TXUnitMessage.cpp) - Unit commands

### Node/Unit Naming
- [RenameNodeUnitsEngine.cpp](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.cpp) - Complete implementation
- [RenameNodeUnitsEngine.h](../src/DTBindingSoftware/src/RenameNodeUnitsEngine.h) - FSM interface

### File Transfer
- [BindingFileFSM.cpp](../src/DTBindingSoftware/src/BindingFileFSM.cpp) - Binding upload FSM
- [TXNodeMessage.h](../src/DTBindingSoftware/src/TXNodeMessage.h#L180) - FTP over CAN

### HTTP/HTTPS API
- [httprcfg_api.h](../src/DEPENDS/AppRemoteCfg_v2/src/dtapprcfgmfclib/httprcfg/httprcfg_api.h) - All endpoints
- [apprcfgtasks.cpp](../src/DEPENDS/AppRemoteCfg_v2/src/dtapprcfgmfclib/apprcfg/apprcfgtasks.cpp) - Task orchestration

### Protocol Definitions
- [nodemess.h](../src/DTBindingSoftware/src/nodemess.h) - **THE** master reference

---

**Document Version**: 1.0  
**Last Updated**: April 2026  
**Author**: Generated from C++ codebase analysis  
**Next Review**: After Phase 1 implementation
