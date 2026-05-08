# Duotecno API Reference

Complete REST API documentation for the Duotecno Configuration Server.

## Base URL

```
http://localhost:3000
```

## Table of Contents

1. [Installation & Configuration](#installation--configuration)
2. [Bindings Management](#bindings-management)
3. [Hardware Upload](#hardware-upload)
4. [Master Connection](#master-connection)
5. [Unit Control](#unit-control)
6. [Statistics](#statistics)

---

## Installation & Configuration

### Get Installation Data

Get complete installation configuration including all nodes, units, and bindings.

```http
GET /api/installation
```

**Response:**
```json
{
  "summary": {
    "totalNodes": 12,
    "totalUnits": 45,
    "totalBindings": 234
  },
  "nodes": [
    {
      "nodeAddress": 3,
      "name": "Living Room",
      "units": [
        {
          "address": [3, 1],
          "unitAddress": 1,
          "type": 1,
          "typeName": "DIM",
          "name": "Main Light"
        }
      ]
    }
  ]
}
```

### Get Node Data

Get detailed information about a specific node.

```http
GET /api/nodes/:nodeAddress
```

**Parameters:**
- `nodeAddress` - Hexadecimal node address (e.g., `03` or `FC`)

**Example:**
```http
GET /api/nodes/03
```

**Response:**
```json
{
  "nodeAddress": 3,
  "name": "Living Room",
  "units": [...]
}
```

### Reload Configuration

Reload installation configuration from disk.

```http
POST /api/reload
```

**Response:**
```json
{
  "success": true,
  "summary": {
    "totalNodes": 12,
    "totalUnits": 45
  },
  "bindings": {
    "totalNodes": 12,
    "totalBindings": 234
  }
}
```

### Rename Node

Rename a node in `nodedatabase.txt`.

```http
POST /api/nodes/:nodeAddress/rename
Content-Type: application/json

{
  "name": "New Node Name"
}
```

**Response:**
```json
{
  "success": true,
  "message": "Node renamed successfully"
}
```

### Rename Unit

Rename a unit in `nodedatabase.txt`.

```http
POST /api/nodes/:nodeAddress/units/:unitAddress/rename
Content-Type: application/json

{
  "name": "New Unit Name"
}
```

**Response:**
```json
{
  "success": true,
  "message": "Unit renamed successfully"
}
```

---

## Bindings Management

### Get Modified Nodes

Get list of nodes with unsaved binding changes.

```http
GET /api/bindings/modified
```

**Response:**
```json
{
  "modified": [3, 5, 252]
}
```

### Get Node Bindings

Get all bindings for a specific node.

```http
GET /api/bindings/:nodeAddress
```

**Example:**
```http
GET /api/bindings/03
```

**Response:**
```json
{
  "nodeAddress": 3,
  "filepath": "/path/to/bind03.txt",
  "modified": false,
  "totalBindings": 45,
  "bindings": [
    {
      "index": 0,
      "raw": "000003_00_0001_C(A000003U01E01)=U02FB6D0103S",
      "nodeAddress": 3,
      "flags": 0,
      "bindingNumber": 1,
      "type": "C"
    }
  ]
}
```

### Add Binding

Add a new binding to a node's binding file.

```http
POST /api/bindings/:nodeAddress
Content-Type: application/json

{
  "bindingString": "000003_00_0999_C(A000003U01E01)=U02FB6D0103S"
}
```

**Response:**
```json
{
  "success": true,
  "state": {
    "nodeAddress": 3,
    "totalBindings": 46,
    "modified": true
  }
}
```

### Update Binding

Update an existing binding by index.

```http
PUT /api/bindings/:nodeAddress/:bindingIndex
Content-Type: application/json

{
  "bindingString": "000003_00_0001_C(A000003U02E01)=U02FB6D0103S"
}
```

**Parameters:**
- `nodeAddress` - Hex node address
- `bindingIndex` - Zero-based index in binding file

**Response:**
```json
{
  "success": true,
  "state": {
    "modified": true,
    "totalBindings": 45
  }
}
```

### Delete Binding

Delete a binding by index.

```http
DELETE /api/bindings/:nodeAddress/:bindingIndex
```

**Response:**
```json
{
  "success": true,
  "state": {
    "totalBindings": 44,
    "modified": true
  }
}
```

### Save Node Bindings

Save a node's bindings to disk (write to `bind[xx].txt`).

```http
POST /api/bindings/:nodeAddress/save
```

**Example:**
```http
POST /api/bindings/03/save
```

**Response:**
```json
{
  "success": true,
  "state": {
    "modified": false,
    "totalBindings": 45
  }
}
```

### Save All Modified Bindings

Save all nodes with unsaved changes.

```http
POST /api/bindings/save-all
```

**Response:**
```json
{
  "success": true,
  "savedNodes": [3, 5, 252]
}
```

---

## Hardware Upload

Upload binding files to physical Duotecno hardware via TCP connection.

**Note:** Must be connected to master first (see [Master Connection](#master-connection)).

### Upload Single Node

Upload bindings for one node to hardware.

```http
POST /api/upload/node/:nodeAddress
```

**Example:**
```http
POST /api/upload/node/03
```

**Response:**
```json
{
  "success": true,
  "nodeAddress": 3,
  "totalBindings": 45,
  "uploadedBindings": 45,
  "skipped": 0,
  "errors": []
}
```

### Upload All Nodes

Force upload of ALL nodes (regardless of changes).

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
      "uploadedBindings": 45
    }
  ]
}
```

### Upload Changed Nodes

Smart upload - only uploads nodes that differ from hardware.

```http
POST /api/upload/changed
```

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
      "uploadedBindings": 45
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

### Upload Modified Nodes

Upload only nodes with unsaved modifications.

```http
POST /api/upload/modified
```

**Response:**
```json
{
  "total": 2,
  "successful": 2,
  "failed": 0,
  "results": [...]
}
```

---

## Master Connection

Manage TCP connection to Duotecno master for hardware communication and unit control.

### Connect to Master

Establish TCP connection and login.

```http
POST /api/master/connect
Content-Type: application/json

{
  "host": "192.168.1.100",
  "port": 5001,
  "password": ""
}
```

**Parameters:**
- `host` - IP address of master
- `port` - TCP port (default: 5001)
- `password` - Optional password (default: empty string)

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

### Disconnect from Master

Close TCP connection.

```http
POST /api/master/disconnect
```

**Response:**
```json
{
  "success": true,
  "status": "disconnected"
}
```

### Get Connection Status

Check current connection status.

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

---

## Unit Control

Control and monitor unit states in real-time. Requires active master connection.

### Get All Units

Get state of all discovered units.

```http
GET /api/units
```

**Response:**
```json
{
  "units": [
    {
      "nodeAddress": 3,
      "unitAddress": 1,
      "type": 1,
      "typeName": "DIM",
      "name": "Main Light",
      "status": 1,
      "value": 75
    }
  ]
}
```

### Get Unit State

Get state of a specific unit.

```http
GET /api/units/:nodeAddress/:unitAddress
```

**Example:**
```http
GET /api/units/03/01
```

**Response:**
```json
{
  "nodeAddress": 3,
  "unitAddress": 1,
  "type": 1,
  "typeName": "DIM",
  "name": "Main Light",
  "status": 1,
  "value": 75
}
```

### Set Unit State

Control a unit (turn on/off, set dimmer value, etc.).

```http
POST /api/units/:nodeAddress/:unitAddress/set
Content-Type: application/json

{
  "value": 50
}
```

**Parameters:**
- `value` - Number (dimmer: 0-99, switch: 0/1) or boolean
- `status` - Optional status override
- `type` - Optional type override

**Examples:**

**Turn dimmer to 75%:**
```json
{
  "value": 75
}
```

**Turn switch ON:**
```json
{
  "value": 1
}
```

**Turn switch OFF:**
```json
{
  "value": 0
}
```

**Response:**
```json
{
  "success": true,
  "unit": {
    "nodeAddress": 3,
    "unitAddress": 1,
    "status": 1,
    "value": 75
  }
}
```

### Refresh Unit Status

Request fresh status from hardware.

```http
POST /api/units/:nodeAddress/:unitAddress/refresh
```

**Response:**
```json
{
  "success": true
}
```

### Refresh All Units

Request status update for all units.

```http
POST /api/units/refresh-all
```

**Response:**
```json
{
  "success": true
}
```

### Get Node Units

Get all units for a specific node.

```http
GET /api/nodes/:nodeAddress/units-state
```

**Response:**
```json
{
  "units": [
    {
      "nodeAddress": 3,
      "unitAddress": 1,
      "type": 1,
      "status": 1,
      "value": 75
    }
  ]
}
```

---

## Statistics

### Get Binding Statistics

Get binding manager statistics.

```http
GET /api/stats
```

**Response:**
```json
{
  "totalNodes": 12,
  "totalBindings": 234,
  "modifiedNodes": 2,
  "nodeStats": {
    "3": {
      "nodeAddress": 3,
      "totalBindings": 45,
      "modified": true
    }
  }
}
```

---

## Error Responses

All endpoints return standard error responses:

```json
{
  "error": "Error message description"
}
```

**Common HTTP Status Codes:**
- `200` - Success
- `400` - Bad Request (missing parameters, not connected, etc.)
- `401` - Unauthorized (authentication failed)
- `404` - Not Found (node/unit/binding not found)
- `500` - Internal Server Error

---

## Testing with curl

### Connect and Control Example

```bash
# 1. Connect to master
curl -X POST http://localhost:3000/api/master/connect \
  -H "Content-Type: application/json" \
  -d '{"host":"192.168.1.100","port":5001,"password":""}'

# 2. Get unit state
curl http://localhost:3000/api/units/03/01

# 3. Turn dimmer to 75%
curl -X POST http://localhost:3000/api/units/03/01/set \
  -H "Content-Type: application/json" \
  -d '{"value":75}'

# 4. Disconnect
curl -X POST http://localhost:3000/api/master/disconnect
```

### Binding Management Example

```bash
# 1. Get node bindings
curl http://localhost:3000/api/bindings/03

# 2. Add new binding
curl -X POST http://localhost:3000/api/bindings/03 \
  -H "Content-Type: application/json" \
  -d '{"bindingString":"000003_00_0999_C(A000003U01E01)=U02FB6D0103S"}'

# 3. Save to disk
curl -X POST http://localhost:3000/api/bindings/03/save

# 4. Upload to hardware (requires master connection)
curl -X POST http://localhost:3000/api/upload/node/03
```
