# Binding File Upload to Duotecno Master Node

This directory contains the implementation for uploading binding files to Duotecno nodes via TCP/IP.

## How It Works

The system connects to a **master node** (HomeServer or Touchscreen) over TCP/IP, which then forwards CAN-bus messages to individual nodes.

### Architecture

```
[TypeScript Client] <-- TCP/IP --> [Master Node (192.168.0.97:5001)]
                                            |
                                            | CAN-bus
                                            v
                        [Node 03] [Node 04] [Node 05] [Node FC]
```

### Protocol

The TCP/IP protocol wraps CAN-bus messages in a simple text format:
```
[byte1,byte2,byte3,...]\n
```

Messages are sent as:
1. **Protocol wrapper** (FC_PROTOCOLMSG = 245, OBJECT_CANBUS_MSG = 1)
2. **CAN message** (node address, unit address, message code, data)

## Files

- **`TcpConnection.ts`** - Low-level TCP/IP socket connection to master node
- **`BindingWriter.ts`** - High-level binding upload logic
- **`BindingFileReader.ts`** - Reader for bind*.txt files
- **`test-upload.ts`** - Test script to upload bindings

## Upload Process

For each node:
1. **Erase** existing binding file
2. **Open** binding file
3. **Write** each binding (header + data)
4. **Close** binding file

Based on:
- `src/DTBindingSoftware/src/BindingFileFSM.cpp`
- `src/DTBindingSoftware/src/BindingsReadWriteFSM.cpp`
- `src/DTBindingSoftware/src/communication/tcpsocket/`

## Configuration

Edit `test-upload.ts` to configure:
- **Master node IP**: `MASTER_NODE_IP` (default: 192.168.0.97)
- **Port**: `MASTER_NODE_PORT` (default: 5001)
- **Password**: `PASSWORD` (if needed)
- **Config path**: Path to binding files
- **Nodes**: Array of node addresses and their binding files

## Usage

```bash
# Run the upload test
npm run test:upload
```

Or directly with tsx:
```bash
tsx src/test-upload.ts
```

## Message Codes

From `src/DTBindingSoftware/src/nodemess.h`:

- **FC_NODEBINDINGSFILECONTROL** (0x51) - File control operations
  - FILE_OPEN (0x01)
  - FILE_CLOSE (0x02)
  - FILE_ERASE (0x03)
  
- **FC_NODEBINDINGENTRY** (0x52) - Binding entry operations
  - WRITE_HEADER (0x01)
  - WRITE_DATA (0x02)
  - WRITE_DONE (0x03)

## Example Output

```
============================================================
Duotecno Binding Upload Test
============================================================
Master node: 192.168.0.97:5001
Config path: /Users/johan/.../Config - sb1+switch
============================================================

Connecting to master node...
Connected!

============================================================
Node 0xFC: bindfc.txt
============================================================
Reading binding file: .../bindfc.txt
Found 45 bindings

Writing bindings to node 0xfc
Total bindings: 45

Erasing binding file on node 0xfc
Opening binding file on node 0xfc

Writing binding 1/45
Writing binding header: #0, flags=0x00
...

Successfully wrote 45 bindings to node 0xfc
✓ Successfully uploaded to node 0xFC

============================================================
Upload complete!
============================================================
```

## Notes

- The master node must be configured as a **CAN-bus master** to forward messages
- Each binding write includes delays to allow the node to process
- Erase operation can take up to 2 seconds
- Messages are logged to console for debugging
