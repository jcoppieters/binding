# Solution: MESSAGE_ERROR_REFUSED_RUNNING_STATE (0x00)

## Problem Summary
Your TCP/IP binding writer was getting **MESSAGE_ERROR_REFUSED_RUNNING_STATE (error 0x00)** when trying to open/close/erase binding files, even with Installer access level (2) properly set.

## Root Cause  
**Two separate configuration steps are required:**

1. ✅ **FC_ACCESSLEVELSET (0xDE)** - Password-based authentication *(you already had this)*
2. ❌ **FC_NODECONFIG (0x94)** - Node state configuration with **APPL_UNLOCK flag** *(MISSING!)*

Without the `APPL_UNLOCK` flag (0x02) set in `bDLLAccessControl`, the node remains in "running state" and **refuses all binding file operations** with error code 0x00.

---

## What Was Missing

### From C++ Source Analysis (src/DTBindingSoftware)

The C++ Configuration Software calls **two commands** before binding operations:

```cpp
// 1. Set access level (you have this)
setAccessLevel("2222");  // Installer level

// 2. Set node config with APPL_UNLOCK (YOU WERE MISSING THIS!)
SetNodeConfig(nodeAddress, bNodeFlags, bDLLAccessControl | APPL_UNLOCK);
```

**Key Constants from C++ source:**
- `FC_NODECONFIG = 0x94` (148 decimal) - from `nodemess.h`
- `APPL_UNLOCK = 0x02` - from `nodegenestructs.h`
- `APPL_SERIALIZE = 0x01` - optional serialization flag
- `MESSAGE_ERROR_REFUSED_RUNNING_STATE = 0x00` - from `nodemess.h`

**Message Format:**
```
FC_NODECONFIG command format (from TXNodeMessage.cpp line 141):
  MessageTransfer(nodeAddress, BROADCAST_UNIT, FC_NODECONFIG, 2, [bNodeFlags, bDLLAccessControl])
  
Where:
  - nodeAddress: target node (e.g., 0xFC for gateway)
  - BROADCAST_UNIT: 0xFF
  - bNodeFlags: node configuration byte (typically 0x00)
  - bDLLAccessControl: MUST include APPL_UNLOCK (0x02) for programming
```

---

## Solution Implemented

### 1. Added FC_NODECONFIG Constants to TcpConnection.ts

```typescript
// Node configuration (from nodemess.h)
private static readonly FC_NODECONFIG = 0x94; // 148 decimal
private static readonly EV_NODECONFIG = 0x0C; // 12 decimal

// DLL Access Control flags (from nodegenestructs.h)
private static readonly APPL_SERIALIZE = 0x01;
private static readonly APPL_UNLOCK = 0x02;
```

### 2. Added setNodeConfig() Method to TcpConnection.ts

```typescript
/**
 * Set node configuration flags (CRITICAL: Must be called before binding operations!)
 * This enables "programming mode" by setting the APPL_UNLOCK flag
 */
async setNodeConfig(
  nodeAddress: number,
  nodeConfig: number = 0x00,
  enableProgramming: boolean = true
): Promise<void>
```

### 3. Added enableProgrammingMode() to BindingWriterFSM.ts

```typescript
/**
 * Enable programming mode on the node by setting APPL_UNLOCK flag
 * CRITICAL: This must be called BEFORE any binding file operations!
 * Without this, the node returns MESSAGE_ERROR_REFUSED_RUNNING_STATE (0x00)
 */
async enableProgrammingMode(): Promise<void> {
  await this.connection.setNodeConfig(
    this.currentNodeAddress,
    0x00,  // nodeConfig flags
    true   // enableProgramming = set APPL_UNLOCK
  );
}
```

### 4. Updated writeBindingsToNode() Call Sequence

**OLD (failing) sequence:**
```
1. setAccessLevel()
2. openFile()  ❌ ERROR: REFUSED_RUNNING_STATE
```

**NEW (working) sequence:**
```
1. setAccessLevel()
2. enableProgrammingMode()  ✅ Sets APPL_UNLOCK
3. openFile()  ✅ Works!
4. eraseFile()
5. writeBindings()
6. closeFile()
7. disableProgrammingMode()  ✅ Clears APPL_UNLOCK (returns node to normal state)
```

### 5. Added disableProgrammingMode() to Clean Up After Upload

**CRITICAL:** After uploading bindings, the node must be returned to normal operating state by clearing the APPL_UNLOCK flag. Without this cleanup step, nodes remain in programming mode and may exhibit abnormal behavior (such as excessive cmd=122 messages on the CAN bus).

```typescript
/**
 * Disable programming mode on the node by clearing APPL_UNLOCK flag
 * CRITICAL: This should be called AFTER binding file operations complete!
 * This returns the node to normal operating state
 */
async disableProgrammingMode(): Promise<void> {
  await this.connection.setNodeConfig(
    this.currentNodeAddress,
    0x00,  // nodeConfig flags
    false  // enableProgramming = clear APPL_UNLOCK
  );
}
```

This is called in a `finally` block to ensure it runs even if an error occurs during upload.

### 6. Added unsubscribeFromCanBus() to Stop CAN Bus Traffic

**CRITICAL:** When connecting via TCP/IP, the client subscribes to ALL CAN bus messages. Without unsubscribing before disconnect, the system continues receiving CAN bus traffic (cmd=122 messages) even after operations complete.

```typescript
/**
 * Unsubscribe from CAN bus messages
 * Should be called before disconnecting to stop receiving CAN bus traffic
 */
private unsubscribeFromCanBus(): void {
  const message = [
    FC_PROTOCOLMSG,
    ATTR_SUBSCRIBE_CANBUS_MSG,
    0  // 0 = unsubscribe (was 1 for subscribe)
  ];
  this.sendRaw(message);
}
```

Updated `disconnect()` method now unsubscribes before closing the socket:
```typescript
disconnect(): void {
  if (this.socket) {
    // Unsubscribe from CAN bus before closing
    this.unsubscribeFromCanBus();
    
    // Give the unsubscribe message time to be sent
    setTimeout(() => {
      if (this.socket) {
        this.socket.destroy();
      }
    }, 100);
    
    this.connected = false;
  }
}
```

---

## Testing

```bash
cd reader
npm run build
npm run test:upload
```

Expected output:
```
✓ Access level: Installer
TX SetNodeConfig: Node 0xFC, Config=0x00, DllAccess=0x02 (UNLOCK=true)
✓ Programming mode enabled (APPL_UNLOCK set)
TX OpenFile: Node 0xFC
✓ File opened successfully
...
✓ CAN bus unsubscribed successfully
```

## Recovery: If Nodes Get Stuck (cmd=122 flood)

**Symptom:** After a failed upload, continuous cmd=122 messages flood the CAN bus from node 0xFC.

**Cause:** The master node was left in programming/debug mode (APPL_UNLOCK set) when upload failed.

**Solution:** Perform a soft reset to clear the stuck state:

```bash
npm run reset
```

This sends **FC_NODERESET (0x9b)** with parameter 0, which:
- Clears runtime state (including APPL_UNLOCK)
- Does NOT erase configuration or bindings
- Triggers node rediscovery
- Returns system to normal operation

**Verified:** This successfully cleared the cmd=122 flood and restored normal operation with all nodes rediscovered properly.

---

## Key Findings from C++ Source

### Files Analyzed:
- `TXNodeMessage.cpp` (line 141-150): SetNodeConfig implementation
- `nodegenestructs.h` (line 136-137): APPL_UNLOCK/SERIALIZE definitions  
- `nodemess.h` (line 120, 238): FC_NODECONFIG and error codes
- `BindingFileFSM.cpp` (line 379, 486): Usage patterns
- `PropertyPageNodeControl.cpp` (line 237): UI control example
- `MessageError.cpp` (line 106): Error handling

### Important Notes:
1. **The APPL_UNLOCK checkbox is DISABLED in the C++ GUI** (line 97) - it's programmatically controlled
2. **No explicit response expected** from FC_NODECONFIG - the C++ code sends it and continues
3. **Access level alone is NOT enough** - you need both password authentication AND node state unlock
4. **Error 0x00 is specifically for "running state"** - node needs to be in programming mode

---

## Why This Was Hard to Find

1. **Two separate authentication mechanisms:** Password-based access level vs. state-based node unlock
2. **No explicit error message:** Error code 0x00 just says "REFUSED RUNNING STATE" without mentioning APPL_UNLOCK
3. **Disabled in GUI:** The unlock checkbox is disabled, suggesting it might be automatic
4. **No direct FSM call:** The C++ code doesn't call SetNodeConfig in the binding FSM - it's done at a higher level

---

## References

**C++ Source Locations:**
- Command definition: `src/DTBindingSoftware/src/nodemess.h:120`
- Flag definitions: `src/DTBindingSoftware/src/nodegenestructs.h:136-137`  
- Implementation: `src/DTBindingSoftware/src/TXNodeMessage.cpp:141-150`
- Error definition: `src/DTBindingSoftware/src/nodemess.h:238`
- Usage example: `src/DTBindingSoftware/src/views/nodeprops/PropertyPageNodeControl.cpp:237`

**Protocol Flow:**
```
TCP/IP → FC_ACCESSLEVELSET (0xDE) → EV_ACCESSLEVELINFO (75)
       ↓
TCP/IP → FC_NODECONFIG (0x94) [nodeFlags, DllAccess|APPL_UNLOCK]
       ↓
TCP/IP → FC_NODEBINDINGSFILECONTROL (0x98) [FILE_OPEN]
       ↓
✅ Success!
```
