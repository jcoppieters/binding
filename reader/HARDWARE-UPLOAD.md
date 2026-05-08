# Duotecno Hardware Upload Guide

Complete guide for uploading configuration changes to Duotecno hardware via TCP/IP connection.

## Overview

This guide explains how to connect to Duotecno master hardware and upload binding configurations, including:
- Establishing TCP connection
- Uploading binding files
- Verification and troubleshooting
- Best practices

---

## Quick Start

### 1. Connect to Master

```bash
curl -X POST http://localhost:3000/api/master/connect \
  -H "Content-Type: application/json" \
  -d '{
    "host": "192.168.1.100",
    "port": 5001,
    "password": ""
  }'
```

### 2. Upload Bindings

```bash
# Upload single node
curl -X POST http://localhost:3000/api/upload/node/03

# Upload all changed nodes
curl -X POST http://localhost:3000/api/upload/changed
```

### 3. Verify

```bash
# Check upload status
curl http://localhost:3000/api/master/status
```

---

## Connection Setup

### Prerequisites

- Duotecno master node on network
- IP address of master (default gateway IP)
- TCP port (default: 5001)
- Password (if configured)

### Network Requirements

**Required:**
- Master node connected to LAN
- PC/Mac on same network
- No firewall blocking port 5001

**Optional:**
- Static IP for master (recommended)
- Port forwarding for remote access

### Finding Master IP

1. **From Duotecno Gateway:**
   - Check gateway web interface
   - Usually displayed on main page

2. **From Your Computer:**
   ```bash
   # Scan local network
   nmap -p 5001 192.168.1.0/24
   ```

3. **Default Gateway:**
   - Often uses gateway IP (e.g., `192.168.1.1`)

---

## Connection API

### Connect to Master

Establish TCP connection and authenticate.

```http
POST /api/master/connect
Content-Type: application/json

{
  "host": "192.168.1.100",
  "port": 5001,
  "password": ""
}
```

**Response (Success):**
```json
{
  "success": true,
  "status": "connected",
  "nodes": 12,
  "units": 45
}
```

**Response (Failed):**
```json
{
  "error": "auth-failed",
  "message": "Authentication failed - wrong password"
}
```

### Check Connection Status

```http
GET /api/master/status
```

**Response:**
```json
{
  "connected": true,
  "status": "connected",
  "nodes": 12,
  "units": 45
}
```

### Disconnect

```http
POST /api/master/disconnect
```

---

## Upload Methods

### Method 1: Upload Single Node

Upload bindings for one specific node.

**Use when:**
- Testing changes to one node
- Making targeted updates
- Troubleshooting specific node

```http
POST /api/upload/node/:nodeAddress
```

**Example:**
```bash
# Upload node 3 (0x03)
curl -X POST http://localhost:3000/api/upload/node/03
```

**Response:**
```json
{
  "success": true,
  "nodeAddress": 3,
  "totalBindings": 45,
  "uploadedBindings": 45,
  "skipped": 0,
  "errors": [],
  "duration": 2345
}
```

---

### Method 2: Upload All Nodes (Force)

Force upload of ALL nodes regardless of changes.

**Use when:**
- Initial hardware setup
- After hardware reset
- Restoring from backup
- When unsure what's on hardware

```http
POST /api/upload/all
```

**Response:**
```json
{
  "total": 12,
  "successful": 12,
  "failed": 0,
  "results": [
    {
      "nodeAddress": 3,
      "success": true,
      "uploadedBindings": 45,
      "duration": 2100
    },
    {
      "nodeAddress": 5,
      "success": true,
      "uploadedBindings": 23,
      "duration": 1500
    }
  ]
}
```

**⚠️ Warning:** This can take several minutes for large installations.

---

### Method 3: Upload Changed Nodes (Smart)

**Recommended method** - Only uploads nodes that differ from hardware.

**Use when:**
- Daily development work
- After editing bindings
- Minimizing upload time

```http
POST /api/upload/changed
```

**How it works:**
1. Reads bindings from hardware
2. Compares with local files
3. Only uploads differences
4. Skips unchanged nodes

**Response:**
```json
{
  "total": 12,
  "uploaded": 3,
  "skipped": 9,
  "failed": 0,
  "results": [
    {
      "nodeAddress": 3,
      "success": true,
      "skipped": false,
      "uploadedBindings": 45,
      "reason": "Bindings differ from hardware"
    },
    {
      "nodeAddress": 5,
      "success": true,
      "skipped": true,
      "reason": "No changes detected"
    }
  ]
}
```

**Benefits:**
- ✅ Fast - skips unchanged nodes
- ✅ Safe - verifies before upload
- ✅ Efficient - minimal network traffic

---

### Method 4: Upload Modified Nodes

Upload only nodes with **unsaved modifications** (in-memory changes).

**Use when:**
- Testing editor changes
- Before saving to disk
- Quick preview on hardware

```http
POST /api/upload/modified
```

**Response:**
```json
{
  "total": 2,
  "successful": 2,
  "failed": 0,
  "results": [
    {
      "nodeAddress": 3,
      "success": true,
      "uploadedBindings": 45,
      "modified": true
    }
  ]
}
```

---

## Upload Process

### What Happens During Upload

1. **Connection Verification**
   - Check TCP connection active
   - Verify master responding

2. **Node Preparation**
   - Read local binding file
   - Parse all binding entries
   - Validate syntax

3. **Hardware Communication**
   - Send `DBInfo` command (request database info)
   - Wait for `FileControl` response
   - Begin file transfer

4. **Binding Transfer**
   - Send each binding entry
   - Wait for acknowledgement
   - Handle errors/retries

5. **Completion**
   - Send file end marker
   - Verify upload success
   - Update node state

### Timeline Example

**Uploading node with 45 bindings:**
```
00:00 - Connect verification
00:01 - Read bind03.txt (45 entries)
00:02 - Send DBInfo request
00:03 - Hardware responds OK
00:04 - Upload binding 1/45
00:05 - Upload binding 2/45
...
00:50 - Upload binding 45/45
00:51 - Send completion
00:52 - Verify success
```

**Average time:** ~30-60 seconds per node

---

## Error Handling

### Common Errors

#### 1. Not Connected

**Error:**
```json
{
  "error": "Not connected to master node. Use /api/master/connect first."
}
```

**Solution:**
```bash
# Connect first
curl -X POST http://localhost:3000/api/master/connect \
  -H "Content-Type: application/json" \
  -d '{"host":"192.168.1.100","port":5001}'
```

---

#### 2. Connection Timeout

**Error:**
```json
{
  "error": "Connection timeout",
  "message": "Master did not respond within 10 seconds"
}
```

**Causes:**
- Master offline
- Wrong IP address
- Network issues
- Firewall blocking

**Solutions:**
1. Verify master IP: `ping 192.168.1.100`
2. Check port: `telnet 192.168.1.100 5001`
3. Restart master node
4. Check firewall settings

---

#### 3. Authentication Failed

**Error:**
```json
{
  "error": "auth-failed",
  "message": "Authentication failed - wrong password"
}
```

**Solutions:**
1. Verify password in master settings
2. Try empty password: `"password":""`
3. Reset master password (hardware reset)

---

#### 4. Upload Timeout

**Error:**
```json
{
  "nodeAddress": 3,
  "success": false,
  "error": "Upload timeout - hardware did not acknowledge"
}
```

**Causes:**
- Node offline
- CAN bus issues
- Hardware busy

**Solutions:**
1. Verify node powered on
2. Check node address correct
3. Retry upload
4. Check CAN bus connections

---

#### 5. Invalid Binding Syntax

**Error:**
```json
{
  "error": "Invalid binding syntax at index 12",
  "binding": "00003_00_000C_NA..."
}
```

**Solutions:**
1. Validate binding format
2. Check node addresses
3. Run syntax validator
4. See [BINDING-FILES.md](BINDING-FILES.md)

---

## Best Practices

### 1. Always Connect First

```bash
# ✅ Correct
curl -X POST /api/master/connect -d '{"host":"192.168.1.100",...}'
curl -X POST /api/upload/changed

# ❌ Wrong
curl -X POST /api/upload/changed  # Fails: not connected
```

### 2. Use Smart Upload

```bash
# ✅ Recommended - only uploads changes
curl -X POST /api/upload/changed

# ⚠️ Avoid unless necessary - uploads everything
curl -X POST /api/upload/all
```

### 3. Save Before Upload

```bash
# ✅ Correct order
curl -X POST /api/bindings/03/save        # Save to disk
curl -X POST /api/upload/node/03          # Upload to hardware

# ⚠️ Risky - modified data not saved
curl -X POST /api/upload/modified         # Uploads unsaved changes
```

### 4. Verify After Upload

```bash
# Upload
curl -X POST /api/upload/node/03

# Verify
curl http://localhost:3000/api/bindings/03
# Check "modified": false
```

### 5. Test Individual Nodes

```bash
# ✅ Test one node first
curl -X POST /api/upload/node/03

# Then upload all
curl -X POST /api/upload/changed
```

### 6. Use Timeouts

```bash
# Set reasonable timeout for large uploads
curl -X POST /api/upload/all --max-time 300
```

---

## Complete Workflow

### Typical Development Workflow

1. **Edit Bindings**
   ```bash
   # Via web UI or API
   curl -X POST /api/bindings/03 \
     -d '{"bindingString":"..."}'
   ```

2. **Save to Disk**
   ```bash
   curl -X POST /api/bindings/03/save
   ```

3. **Connect to Master**
   ```bash
   curl -X POST /api/master/connect \
     -d '{"host":"192.168.1.100","port":5001}'
   ```

4. **Upload Changes**
   ```bash
   # Smart upload (recommended)
   curl -X POST /api/upload/changed
   ```

5. **Test on Hardware**
   - Press buttons
   - Verify lights respond
   - Check expected behavior

6. **If Issues:**
   ```bash
   # Revert changes
   curl -X POST /api/reload
   
   # Re-upload
   curl -X POST /api/upload/node/03
   ```

7. **Disconnect**
   ```bash
   curl -X POST /api/master/disconnect
   ```

---

## Bulk Operations

### Upload All Nodes Script

```bash
#!/bin/bash

HOST="192.168.1.100"
PORT=5001
API="http://localhost:3000"

# 1. Connect
echo "Connecting to master..."
curl -X POST $API/api/master/connect \
  -H "Content-Type: application/json" \
  -d "{\"host\":\"$HOST\",\"port\":$PORT,\"password\":\"\"}"

# 2. Upload all changes
echo "Uploading changed nodes..."
curl -X POST $API/api/upload/changed

# 3. Check status
echo "Checking status..."
curl $API/api/master/status

# 4. Disconnect
echo "Disconnecting..."
curl -X POST $API/api/master/disconnect

echo "Done!"
```

### Node-by-Node Upload

```bash
#!/bin/bash

API="http://localhost:3000"

# List of nodes
NODES=(03 05 0A 0F 25 FC)

for NODE in "${NODES[@]}"; do
  echo "Uploading node $NODE..."
  
  curl -X POST $API/api/upload/node/$NODE
  
  if [ $? -eq 0 ]; then
    echo "✅ Node $NODE uploaded successfully"
  else
    echo "❌ Node $NODE upload failed"
  fi
  
  # Wait between uploads
  sleep 2
done
```

---

## Monitoring Upload Progress

### Real-time Status

Monitor upload in real-time using server logs:

```bash
# Start server with logging
npm run server | tee upload.log

# In another terminal, watch logs
tail -f upload.log | grep -i upload
```

### Server Output Example

```
[POST] /api/master/connect
✅ Connected to master at 192.168.1.100:5001
🔍 Discovered 12 nodes, 45 units

[POST] /api/upload/changed
📤 Uploading node 0x03...
   ▸ Sending binding 1/45
   ▸ Sending binding 2/45
   ...
   ▸ Sending binding 45/45
✅ Node 0x03 uploaded (45 bindings, 2.3s)

📤 Uploading node 0x05...
⏭️  Node 0x05 skipped (no changes)

✅ Upload complete: 1 uploaded, 11 skipped
```

---

## Troubleshooting Guide

### Upload Too Slow

**Symptoms:**
- Upload takes > 5 minutes per node
- Frequent timeouts

**Solutions:**
1. Reduce network latency
2. Use wired connection (not WiFi)
3. Increase timeout in code
4. Check CAN bus load

---

### Bindings Not Working After Upload

**Symptoms:**
- Upload succeeds
- Hardware doesn't respond
- Buttons do nothing

**Checks:**
1. **Verify upload completed:**
   ```bash
   curl http://localhost:3000/api/bindings/03
   # Check "modified": false
   ```

2. **Check binding syntax:**
   - Validate addresses
   - Check unit types
   - Verify events

3. **Test individual unit:**
   ```bash
   curl -X POST /api/units/03/02/set -d '{"value":1}'
   ```

4. **Re-upload with force:**
   ```bash
   curl -X POST /api/upload/node/03
   ```

---

### Partial Upload Failure

**Symptoms:**
- Some nodes upload, others fail
- Inconsistent results

**Solutions:**
1. **Upload failed nodes individually:**
   ```bash
   curl -X POST /api/upload/node/05
   curl -X POST /api/upload/node/0A
   ```

2. **Check which nodes failed:**
   ```bash
   curl -X POST /api/upload/all
   # Review "results" array
   ```

3. **Verify node connectivity:**
   - Check CAN bus
   - Power cycle node
   - Verify address

---

## Advanced Topics

### Custom Upload Strategy

Implement custom upload logic:

```typescript
import { BindingUploader } from './communication/BindingUploader.js';
import { BindingManager } from './services/BindingManager.js';

async function customUpload(nodeList: number[]) {
  const uploader = new BindingUploader('192.168.1.100', 5001, '');
  const manager = new BindingManager();
  
  for (const nodeAddress of nodeList) {
    console.log(`Uploading node ${nodeAddress}...`);
    
    try {
      const result = await uploader.uploadToNode(nodeAddress, manager);
      
      if (result.success) {
        console.log(`✅ Success: ${result.uploadedBindings} bindings`);
      } else {
        console.log(`❌ Failed: ${result.error}`);
      }
    } catch (err) {
      console.log(`❌ Error: ${err.message}`);
    }
  }
}

// Upload specific nodes
customUpload([3, 5, 10, 252]);
```

---

## Protocol Details

### TCP Connection

- **Port:** 5001 (default)
- **Protocol:** Raw TCP socket
- **Timeout:** 10 seconds connection, 30 seconds per operation
- **Encoding:** Binary protocol with ASCII binding strings

### Message Format

```
[STX][Length][Command][Data...][Checksum][ETX]
```

### Upload Sequence

```
Client → Master: DBInfo Request (get database info)
Master → Client: DBInfo Response (node ready)

Client → Master: FileControl Start (begin transfer)
Master → Client: FileControl Ack

Client → Master: Binding Entry 1
Master → Client: Ack

Client → Master: Binding Entry 2
Master → Client: Ack

...

Client → Master: FileControl End (transfer complete)
Master → Client: Final Ack
```

---

## See Also

- [API Reference](API-REFERENCE.md) - Complete REST API documentation
- [Binding Files Format](BINDING-FILES.md) - Binding syntax reference
- Server logs for detailed upload debugging
