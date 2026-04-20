# Duotecno Binding Editor & Uploader

Complete server solution for managing Duotecno binding files with smart upload capabilities.

## Features

✅ **Read** binding files from filesystem into memory  
✅ **Edit** bindings via REST API (add, update, delete, reorder)  
✅ **Write** bindings back to filesystem  
✅ **Upload** to nodes with three modes:
- **All**: Force upload everything
- **Changed**: Smart upload (checks FILE_INFO, only uploads if different)
- **Modified**: Upload only files edited in the session

## Architecture

### Components

1. **BindingManager** (`src/services/BindingManager.ts`)
   - In-memory binding storage
   - Edit tracking (add, update, delete, reorder)
   - Change detection via XOR checksum calculation
   - Filesystem write operations

2. **BindingUploader** (`src/communication/BindingUploader.ts`)
   - Orchestrates node uploads
   - FILE_INFO requests for change detection
   - Three upload strategies (all/changed/modified)

3. **BindingWriterFSM** (`src/communication/BindingWriterFSM.ts`)
   - Low-level protocol state machine
   - Handles file operations (close→erase→open→write→close)

4. **Enhanced Server** (`src/server-enhanced.ts`)
   - REST API for all operations
   - Serves static UI files

## Quick Start

```bash
# Start the enhanced server
npm run server:enhanced

# In another terminal, test upload
curl http://localhost:3000/api/stats
```

## API Reference

### Installation APIs

```http
GET  /api/installation              # Get all installation data
GET  /api/nodes/:nodeAddress        # Get specific node

POST /api/reload                    # Reload config from filesystem
```

### Binding Editor APIs

```http
# Get bindings
GET  /api/bindings/:nodeAddress                    # Get all bindings for node
GET  /api/bindings/modified                        # List modified nodes

# Edit bindings
POST   /api/bindings/:nodeAddress                  # Add new binding
       Body: { "bindingString": "..." }

PUT    /api/bindings/:nodeAddress/:index           # Update binding
       Body: { "bindingString": "..." }

DELETE /api/bindings/:nodeAddress/:index           # Delete binding

# Save to filesystem
POST /api/bindings/:nodeAddress/save               # Save one node
POST /api/bindings/save-all                        # Save all modified
```

### Upload APIs

```http
# Connection
POST /api/upload/connect                           # Connect to master
     Body: { "host": "192.168.0.97", "port": 5001 }

POST /api/upload/disconnect                        # Disconnect

# Upload operations
POST /api/upload/node/:nodeAddress                 # Upload single node
POST /api/upload/all                               # Upload all (force)
POST /api/upload/changed                           # Upload changed only ⭐
POST /api/upload/modified                          # Upload edited only

# Statistics
GET  /api/stats                                    # Get stats
```

## Upload Strategies

### 1. Upload All (Force)
```bash
curl -X POST http://localhost:3000/api/upload/all
```
Uploads every loaded binding file regardless of state.

### 2. Upload Changed (Smart) ⭐ RECOMMENDED
```bash
curl -X POST http://localhost:3000/api/upload/changed
```
1. Requests FILE_INFO from each node (count + checksum)
2. Calculates local checksums
3. Only uploads if different
4. Skips unchanged files

**Use case**: Daily sync, deployment workflow

### 3. Upload Modified (Session)
```bash
curl -X POST http://localhost:3000/api/upload/modified
```
Only uploads files edited during this session.

**Use case**: Quick fixes, testing single changes

## Change Detection Details

The protocol includes **FILE_INFO** request that returns:
- Number of binding entries (1-2 bytes)
- XOR checksum (1 byte)

### XOR Checksum Calculation

```typescript
let xor = 0;
for (const binding of bindings) {
  for (const char of binding) {
    xor ^= char.charCodeAt(0);
  }
}
checksum = xor & 0xFF;
```

This matches the C++ implementation in `ComApplication.cpp`.

## Example Workflow

### 1. Start Server
```bash
npm run server:enhanced
```

### 2. Edit Bindings
```bash
# Get current bindings for node 0x03
curl http://localhost:3000/api/bindings/03

# Add a new binding
curl -X POST http://localhost:3000/api/bindings/03 \
  -H "Content-Type: application/json" \
  -d '{"bindingString": "000003_01_0001_GA..."}'

# Update binding at index 5
curl -X PUT http://localhost:3000/api/bindings/03/5 \
  -H "Content-Type: application/json" \
  -d '{"bindingString": "000003_01_0001_GA..."}'

# Delete binding at index 10
curl -X DELETE http://localhost:3000/api/bindings/03/10
```

### 3. Save to Filesystem
```bash
# Save single node
curl -X POST http://localhost:3000/api/bindings/03/save

# Or save all modified
curl -X POST http://localhost:3000/api/bindings/save-all
```

### 4. Upload to Nodes
```bash
# Connect to master
curl -X POST http://localhost:3000/api/upload/connect \
  -H "Content-Type: application/json" \
  -d '{"host": "192.168.0.97", "port": 5001}'

# Smart upload (changed only)
curl -X POST http://localhost:3000/api/upload/changed

# Response:
# {
#   "total": 10,
#   "uploaded": 2,
#   "skipped": 7,
#   "failed": 1,
#   "results": [...]
# }
```

## Error Handling

All upload operations return detailed results:

```json
{
  "nodeAddress": 3,
  "success": true,
  "skipped": false,
  "bindingsWritten": 12
}
```

Or on error:
```json
{
  "nodeAddress": 3,
  "success": false,
  "error": "SHARE_VIOLATION"
}
```

## Protocol Sequence

When uploading to a node:

1. Set access level (2222 = Installer)
2. Enable APPL_UNLOCK on master (0xFC)
3. Enable APPL_UNLOCK on target node
4. Close file (if already open)
5. Erase file ✅ Critical!
6. Open file
7. Write each binding (header → data chunks → done)
8. Close file

Missing any step results in protocol errors (0x82 SHARE_VIOLATION, 0x88 WRONG_STATE, etc.)

## Performance

- **Upload speed**: ~2-3 bindings/second per node
- **Change detection**: <100ms per node (FILE_INFO request)
- **Checksum calculation**: Instant (pure JavaScript)

For 100 nodes:
- Full upload: ~20-30 minutes
- Smart upload (10% changed): ~2-3 minutes
- Modified upload (1 node): ~10-20 seconds

## Next Steps

- [ ] Build web UI for visual binding editor
- [ ] Add binding validation (syntax checking)
- [ ] Implement undo/redo for edits
- [ ] Add binding search/filter
- [ ] Export/import binding sets
- [ ] Scheduled automatic sync
