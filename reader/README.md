# Duotecno Configuration Reader/Writer

TypeScript project for reading, editing, and uploading Duotecno binding configuration files with real-time hardware control.

## Features

✅ **Configuration Management**
- Parse all Duotecno config files (`bind*.txt`, `nodedatabase.txt`, etc.)
- Full-featured binding editor with visual UI
- Real-time validation and syntax checking

✅ **Hardware Integration**
- TCP/IP connection to master node
- Upload bindings to hardware
- Real-time unit control (lights, switches, dimmers, motors)
- Live status monitoring with polling

✅ **REST API Server**
- Complete REST API for all operations
- Web-based binding editor
- Modular architecture with separate API modules

---

## Quick Start

### Install Dependencies

```bash
npm install
```

### Start Server

```bash
npm run server
```

Server starts at: **http://localhost:3000**

### Connect to Hardware

```bash
curl -X POST http://localhost:3000/api/master/connect \
  -H "Content-Type: application/json" \
  -d '{"host":"192.168.1.100","port":5001,"password":""}'
```

### Upload Bindings

```bash
# Upload only changed nodes (recommended)
curl -X POST http://localhost:3000/api/upload/changed

# Upload specific node
curl -X POST http://localhost:3000/api/upload/node/03
```

---

## Documentation

### 📚 Core Documentation

| Document | Description |
|----------|-------------|
| **[API-REFERENCE.md](API-REFERENCE.md)** | Complete REST API with HTTP examples |
| **[BINDING-FILES.md](BINDING-FILES.md)** | Binding file format and syntax |
| **[HARDWARE-UPLOAD.md](HARDWARE-UPLOAD.md)** | Hardware connection and upload guide |

### 🔧 Development

| Document | Description |
|----------|-------------|
| [DEVELOPMENT.md](DEVELOPMENT.md) | Development setup and workflow |
| [IMPLEMENTATION-SUMMARY.md](IMPLEMENTATION-SUMMARY.md) | Implementation history |

---

## Project Structure

```
reader/
├── src/
│   ├── server.ts                    # Main server with mounted API routers
│   ├── api/                         # Modular API endpoints
│   │   ├── installationAPI.ts       # Installation & node management
│   │   ├── bindingsAPI.ts           # Binding CRUD operations
│   │   ├── uploadAPI.ts             # Hardware upload
│   │   ├── masterAPI.ts             # Master connection
│   │   ├── unitsAPI.ts              # Unit control & status
│   │   └── statsAPI.ts              # Statistics
│   ├── communication/               # TCP protocol & hardware communication
│   │   ├── TcpConnection.ts         # TCP socket wrapper
│   │   ├── DuotecnoProtocol.ts      # Protocol message builders
│   │   └── BindingUploader.ts       # Binding upload logic
│   ├── services/                    # Core services
│   │   ├── MasterConnectionService.ts  # Singleton TCP connection
│   │   └── BindingManager.ts        # Binding file management
│   ├── readers/                     # Config file parsers
│   │   ├── installation.ts          # Main installation reader
│   │   ├── BindingFileReader.ts     # Binding file parser
│   │   └── ...
│   ├── writers/                     # Config file writers
│   │   └── NodeDatabaseWriter.ts    # Write node/unit names
│   ├── models/                      # TypeScript types
│   └── utils/                       # Helper functions
├── public/                          # Web UI
│   ├── index.html                   # Main UI
│   ├── app.js                       # Frontend logic
│   ├── unit-control.js              # Unit control & polling
│   └── styles.css                   # Styling
├── API-REFERENCE.md                 # 📖 API documentation
├── BINDING-FILES.md                 # 📖 Binding format reference
├── HARDWARE-UPLOAD.md               # 📖 Hardware upload guide
└── package.json
```

---

## Key Concepts

### Nodes

Physical Duotecno controller modules on the CAN bus.
- Address: Hex (e.g., `0x03`, `0xFC`)
- Types: Master, slave nodes
- Contains: Multiple units

### Units

Individual controllable devices within a node.
- Types: Switch, Dimmer, Motor, Sensor, Control
- Address: Hex (e.g., `0x01`, `0x15`)
- State: Status, value, temperature

### Bindings

Automation rules connecting inputs to outputs.
- Format: ASCII text strings
- Types: Normal, Immediate, Conditional, Timer
- Storage: `bind[xx].txt` files per node

---

## Common Tasks

### Edit Bindings via Web UI

1. Open http://localhost:3000
2. Navigate to node
3. Click "Edit Bindings"
4. Add/edit/delete entries
5. Save changes

### Edit Bindings via API

```bash
# Get current bindings
curl http://localhost:3000/api/bindings/03

# Add new binding
curl -X POST http://localhost:3000/api/bindings/03 \
  -H "Content-Type: application/json" \
  -d '{"bindingString":"000003_00_0999_NA000003U01E01=U02F0400S"}'

# Save to disk
curl -X POST http://localhost:3000/api/bindings/03/save
```

### Control Units

```bash
# Connect to master first
curl -X POST http://localhost:3000/api/master/connect \
  -d '{"host":"192.168.1.100","port":5001}'

# Turn dimmer to 75%
curl -X POST http://localhost:3000/api/units/03/02/set \
  -d '{"value":75}'

# Turn switch ON
curl -X POST http://localhost:3000/api/units/03/01/set \
  -d '{"value":1}'

# Get unit state
curl http://localhost:3000/api/units/03/02
```

---

## Development

### Build

```bash
npm run build
```

### Run Development Server

```bash
npm run server
```

### Watch Mode

```bash
npm run watch
```

### Project Configuration

- **TypeScript:** ESM modules, target ES2020
- **Config directory:** `configValies/` (change in `server.ts`)
- **Port:** 3000 (change in `server.ts`)

---

## Architecture Highlights

### Singleton Master Connection

Only one TCP connection to master at a time:
```typescript
const masterService = MasterConnectionService.getInstance();
```

### Modular API Design

Each API domain has its own module:
```typescript
app.use('/api', createInstallationAPI({ ... }));
app.use('/api', createBindingsAPI({ ... }));
app.use('/api', createUploadAPI({ ... }));
```

### Real-time Status Polling

Frontend polls unit state every 1.5 seconds when viewing unit detail.

### Protocol Message Builders

All hardware commands use centralized builders:
```typescript
DuotecnoProtocol.buildSetSwitch(nodeAddr, unitAddr, on);
DuotecnoProtocol.buildSetDimmer(nodeAddr, unitAddr, value);
```

---

## Examples

### Complete Upload Workflow

```bash
# 1. Connect to master
curl -X POST http://localhost:3000/api/master/connect \
  -d '{"host":"192.168.1.100","port":5001,"password":""}'

# 2. Edit binding via API
curl -X POST http://localhost:3000/api/bindings/03 \
  -d '{"bindingString":"000003_00_0042_NA000003U01E01=U02F03D0132S"}'

# 3. Save to disk
curl -X POST http://localhost:3000/api/bindings/03/save

# 4. Upload to hardware
curl -X POST http://localhost:3000/api/upload/node/03

# 5. Verify on hardware
curl -X POST http://localhost:3000/api/units/03/01/set -d '{"value":1}'

# 6. Disconnect
curl -X POST http://localhost:3000/api/master/disconnect
```

---

## Troubleshooting

### Cannot Connect to Master

1. Verify IP address: `ping 192.168.1.100`
2. Check port open: `telnet 192.168.1.100 5001`
3. Verify password (if required)
4. Check firewall settings

### Bindings Not Working After Upload

1. Verify upload completed: Check `modified: false` in binding state
2. Check binding syntax: Use [BINDING-FILES.md](BINDING-FILES.md)
3. Test unit directly: Use `/api/units/:node/:unit/set`
4. Re-upload with force: `POST /api/upload/node/:nodeAddress`

### Unit Control Not Responding

1. Check connection: `GET /api/master/status`
2. Verify unit exists: `GET /api/units`
3. Request status refresh: `POST /api/units/:node/:unit/refresh`
4. Check hardware: Power, CAN bus connections

---

## See Also

- [API Reference](API-REFERENCE.md) - Complete API documentation
- [Binding Files](BINDING-FILES.md) - Format specification
- [Hardware Upload](HARDWARE-UPLOAD.md) - Upload procedures
- [Duotecno Protocol Spec](../specs/binding-files-format.md) - Low-level details

---

## License

Internal Duotecno project - All rights reserved
