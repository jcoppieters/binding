# UI TODO — Central Reference Document

> **This is the single source of truth. Keep it up to date with every session.**

Analysed sources: `UI.md`, `specs/new-binding-software-ui.md`, `specs/binding-files-format.md`, `specs/api - Authorization API - v1.05.pdf`, `specs/api - moodscfg - v1.15.pdf`, `specs/api - schedulingcfg - v1.12.pdf`, `specs/udp - config protocol v3.01.pdf`, `public/demo1–5.html`, `src/api/`, `src/services/`, `src/communication/`, `src/models/`, `duotecnopro/` mobile app.

---

## Protocol summary (four distinct channels)

| Protocol | Port | Transport | Purpose |
|----------|------|-----------|---------|
| TCP binding protocol | 5001 | TCP | Upload/download binding files, live control |
| UDP config protocol | 5002 | UDP broadcast | **Network discovery** — find all Duotecno devices on LAN |
| HTTP config API | 8080 | HTTP + Bearer token | Moods, scheduling, node config |
| Auth API | 8081 | **HTTPS** | Get/refresh access tokens for HTTP API |

### Lab systems
| System | IP | Port | Password | Notes |
|--------|-----|------|----------|-------|
| Test install (can mess up) | 192.168.0.97 | 5001 | GMTecno | Free to test |
| Real house | 192.168.0.5 | 5001 | duotecno | **Read-only!** |

UDP discovery: broadcast `[184,0,0]` to port 5002. Response per device: name, MAC, IP, netmask, gateway.

---

## Architecture decisions (all settled)

| Topic | Decision |
|-------|---------|
| New app shell | demo5-style: header + three tabs. Replaces `index.html`. |
| demo1–5.html | Reference prototypes only — do not maintain |
| Old node/unit viewer | Moves into per-module config modals |
| Component structure | Split into small files from the start — demo5 monolith is unmaintainable |
| Module JSON database | Local JSON files under `reader/modules/` — URL-configurable for future remote |
| Rail View | Design-time offline; compared to live discovery when connected |
| Discovery | UDP broadcast port 5002 — not CAN addresses |
| Binding wizard | **Replaced** entirely by the visual wiring diagram |
| Wiring diagram scope | Per-device: click one device → see all its wires |
| Save vs Upload | Two separate actions: **Save to .duo** and **Upload to hardware** |
| Moods | Sent to hardware only on "Upload to hardware", not live |
| Scheduling | Same as moods — explicit upload only |
| Timers | In binding files (Td/Tf/Tr types). Moods handle sequencing via entry delays. |
| Logic blocks | NOT/AND/OR → `C` (Conditional) binding type; Timers → timer binding types |
| Project bindings storage | **Structured JSON** (our own format) — serialized to `bind*.txt` only at upload |
| Import without .duo | All devices land in one "House" room; move to real rooms manually |
| Auth for HTTP APIs | Bearer token via `HTTPS :8081/auth/create|refresh`; used in `:8080` calls |

---

## What the backend already has ✅

| Piece | Location | Status |
|-------|----------|--------|
| Load installation from files | `src/api/installationAPI.ts` | ✅ |
| `BindingManager` — in-memory CRUD | `src/services/BindingManager.ts` | ✅ |
| High-level binding creation | `src/api/bindingEditorAPI.ts` | ✅ |
| Raw bindings CRUD | `src/api/bindingsAPI.ts` | ✅ |
| Upload all / changed (checksum) / modified | `src/api/uploadAPI.ts` | ✅ |
| TCP FSM writer | `src/communication/BindingWriterFSM.ts` | ✅ |
| Per-node file-info (count + XOR checksum) | `BindingUploader.requestFileInfo()` | ✅ |
| Master TCP connection service | `src/services/MasterConnectionService.ts` | ✅ |
| Two-phase discovery (quick/full modes) | `MasterConnectionService.connect(quickMode)` | ✅ |
| Per-node unit fetching | `POST /api/master/node/:address/units` | ✅ |
| Unit live state (frontend) | `public/unit-control.js` | ✅ |

**Gap:** project data model (rooms/devices/rail layout), UI→binding-string generation, UDP discovery, HTTP API clients for moods/scheduling/auth.

---

## Definitive TODO list

### Phase 0 — Foundation *(start here)*

- [x] **P0-1** Define `.duo` project file schema — **DONE** (`src/models/project.ts`)
- [x] **P0-2** Module JSON database — **DONE** (reader/modules/, fully populated)
- [x] **P0-3** New app shell — **DONE** (index.html + state.js + router.js + main.js)
  - Header with logo, project name, connection status, dropdown menu, upload button
  - Three view tabs: Rail View, Home View, Materials View
  - **Fixed header button alignment**: All buttons (connection status, Project dropdown, Verstuur bindings) now same height
    * Connection status had margin-top:1rem from styles.css → added margin:0 override
    * Added border:1px solid transparent to match other buttons (box model parity)
    * Font size, padding, border-radius all matched: 13px, 6px 14px, 6px
- [x] **P0-4** Remote connection via noports proxy via getMasterURL() — **DONE**
  - Supports tunnel ID format: `.tcp` for TCP connections, `.http` for HTTP API
  - Proxy server hardcoded: `abc123.tcp:5001` → `masters.duotecno.eu:5098/abc123.tcp:5001`

### Phase 1 — Rail View

- [x] **P1-1** Render Rail View from project data — **DONE**
- [x] **P1-2** Add module: picker → choose model → save to project — **DONE**
- [x] **P1-3** Add cabinet — **DONE**
- [x] **P1-4** Add switch / LCD to woning panel — **DONE**
- [x] **P1-5** Resources panel in sidebar — **DONE**
- [x] **P1-6** DIN rail space bar (M-units used vs. cabinet width) — **DONE**
- [x] **P1-7** Delete module from Rail View — **DONE**
- [x] **P1-8** Reorder modules within cabinet — **DONE**
- [x] **P1-9** Module detail modal with hardware info — **DONE**
  - Shows channel groups, discovery status, node type, firmware name
  - Auto-suggestions for UNKNOWN modules based on discovered capabilities
  - "Fetch units" button for per-node refresh (incremental discovery)
  - `public/components/rail-view.js::openModuleDetail()`
- [x] **P1-10** Materials View (Materiaallijst tab) — **DONE**
  - `public/components/mat-view.js`
  - Bill of materials with quantities, DIN space, power consumption, pricing
  - Pricing data merged from `modules/product-codes.csv` into `_index.json`
  - Export to CSV/Excel functionality
  - Totals row with project-wide sums

### Phase 2 — Home View

- [x] **P2-1** Render rooms from project data — **DONE**
- [x] **P2-2** Add / rename / delete room and floor — **DONE**
  - Add room prompt with name, icon, floor selection
  - Floor menu with rename/delete options
  - Room menu with rename/delete options
  - State actions: ADD_ROOM, UPDATE_ROOM, REMOVE_ROOM, ADD_FLOOR, UPDATE_FLOOR, REMOVE_FLOOR

- [x] **P2-4** Add device to room — **DONE** (basic UI structure)
  - "+ apparaat" button in room header opens device picker dialog
  - 7 device type buttons with visual selection (switch, button, sensor, lamp, relay, dimmer, motor)
  - Name input with validation
  - Dispatches ADD_DEVICE_TO_ROOM action to add device to room
  - **STATUS**: Basic demo version complete — adds mock devices with icons/colors
  - **NEXT**: Link to real units from material list (see P2-4a below)

- [ ] **P2-4a** Link room devices to real material units (MAJOR TASK)
  - **Goal**: Room devices must be actual units from the materiaallijst, not mock objects
  - **Device picker must show**:
    * Available units grouped by type (relay, lamp, dimmer, motor, switch, sensor, button)
    * Module origin for each unit (e.g., "DT02 Relay Module - Rail 1, Cabinet A")
    * Ongebruikt vs. Gebruikt status badge for each unit
    * Unit name as default (e.g., "Relais 1", "Dimmer kanaal 2") — editable in the picker
  - **Rules**:
    * Controllers (switch, sensor, button): can only be used ONCE — cannot appear in multiple rooms
    * Controllables (lamp, relay, dimmer, motor): CAN be used multiple times (multiple lamps on 1 circuit)
    * For now: only show UNUSED units in picker (multi-use for lamps = future enhancement)
  - **Data model**:
    * Room device gets `channelRef: {cabinetId, moduleId, unitAddress}` or `{woningDeviceId, unitAddress}`
    * This links the room device to the physical unit in Rail View
    * Unit itself gets `room: {floorId, roomId}` assignment (bidirectional link)
  - **Name editing**:
    * Room device name can be changed in Home View (rename in device context menu)
    * Name change must update the underlying unit's label
    * Later: name change must be uploaded to the node (via TCP binding protocol)
  - **State actions needed**:
    * ADD_DEVICE_TO_ROOM: link device to unit, mark unit as used, set unit.room
    * UPDATE_DEVICE_NAME: rename in both room device + linked unit
    * REMOVE_DEVICE_FROM_ROOM: unlink, mark unit as unused, clear unit.room
  - **UI changes needed**:
    * Replace device type picker with unit picker (filterable list with search)
    * Show unit availability + module origin in picker
    * Add "Naam" field pre-filled with unit's current label (editable)
    * Device card in room shows unit icon + custom name
  - **Backend support**:
    * May need `GET /api/units/available` endpoint to list unused units
    * Or compute availability in frontend from project.cabinets + project.woningDevices
  - **Future enhancements** (not now):
    * Allow adding already-used lamps to multiple rooms (same circuit)
    * Show "Gebruikt in: Keuken, Badkamer" badge for multi-use units
    * Separate "Add existing device" vs "Add new device" flows

- [x] **P2-5** Floor plan import (image overlay, toggle) — **PARTIALLY DONE**
  - ✅ Per-room background image upload via room ... menu (🗺️ Grondplan toevoegen)
  - ✅ File input → FileReader → save as data URL to room.backgroundImage
  - ✅ CSS background-size:cover + background-position:center applied to room card
  - ✅ "Verwijder grondplan" menu item appears when room.backgroundImage is set
  - ⏳ TODO: Add crop/select step after upload — user selects portion of drawing for this room
- [x] **P2-6** Bigger room cards with horizontal scroll — **DONE**
  - Room cards occupy ~80% of available canvas width (not small fixed-size cards)
  - Multiple rooms flow horizontally → horizontal scroll to navigate between rooms
  - Rooms take full height above divider with flex:2 layout
  - Device cards remain 100×100px fixed size
  - Allows room interiors to be visible at a useful scale for device placement
- [x] **P2-8** Drag-and-drop device positioning in rooms — **DONE** ✅
  - Single unified HTML5 drag system for all drag operations
  - **Drop zone detection** determines action based on where device is dropped:
    * Drop in SAME ROOM → repositions device at cursor location (UPDATE_DEVICE_POSITION)
    * Drop in DIFFERENT ROOM → moves device to that room at cursor location (MOVE_DEVICE_TO_ROOM with x,y)
    * Drop in BINDING PANEL → adds device to binding visualization (copy)
    * Drop OUTSIDE all zones → nothing happens (canceled)
  - Visual feedback during drag:
    * Room border highlights with blue dashed outline when dragging over it
    * Binding panel highlights with blue background when dragging over it
    * dropEffect cursor shows 'move' for room, 'copy' for binding panel
  - Drop position calculated from cursor coordinates, centered on device (subtract 50px)
  - Position constrained to room bounds (0 to container width/height - 100px)
  - Position stored in device.x and device.y coordinates and persists across reloads
  - **No keyboard modifiers needed** - intuitive single drag like Figma/modern design tools
  - Simplified code: removed ~100 lines of mousedown/mousemove logic
  - **Fixed**: effectAllowed='copyMove' allows both room moves AND binding panel copies
  - Essential for accurate positioning on ground plan backgrounds
- [x] **P2-9** Remove/move device between rooms — **DONE**
  - "..." menu button in top-right corner of device card
  - Menu options:
    * "Verplaats naar..." → submenu with list of all rooms
    * "Verwijder" → removes device from room (with confirmation)
  - Bindings remain intact when moving devices between rooms
  - Allows reorganization of devices without losing wiring work
  - Menu styling: dropdown positioned below "..." button, closes on outside click
- [x] **P2-10** Resize room width and height — **DONE**
  - Blue resize handle in bottom-right corner of room card (appears on hover)
  - Drag handle diagonally to adjust both width AND height
  - Other rooms shift horizontally to accommodate width changes
  - Width and height stored per room in project file (room.width, room.height)
  - Defaults: 80% width, 400px height; minimums: 600px width, 300px height
  - Allows different room sizes for better floor plan visualization and device layout

### Phase 3 — Visual wiring diagram (Binding View)

**Goal:** Visual node-graph editor where devices show input/output ports and you draw wires between them.

**Current Status:**
- [x] Basic room device system with 7 device types (💡 Lamp, 🔘 Schakelaar, ⚡ Relais, 🎚️ Dimmer, 🪟 Motor, 🌡️ Sensor, 🔳 Drukknop)
- [x] Device cards (100×100px) with icons, colors, positioning
- [x] Drag-and-drop positioning within rooms
- [x] Device menu (move/delete)
- [x] Floor plan upload per room
- [x] Room resizing
- [x] Binding panel placeholder (click device triggers showDeviceBindings)

**Port System:**
- **Controllers** (switch, sensor, button): outputs on RIGHT (kort/lang/druk/trigger)
- **Controllables** (lamp, relay, dimmer, motor): inputs on LEFT (aan/uit/schakel/dim+/dim-/op/neer/stop/PIR/stel waarde)
- **Binding panel layout**: Controllers left column, controllables right column, SVG wires between
- **State**: `project.bindings[]` with `{id, from: {deviceId, portId}, to: {deviceId, portId}, color}`

**Tasks:**

- [ ] **P3-1** Define DEVICE_PORTS constant with ports per device type
- [ ] **P3-2** Device card port visualization (colored dots on card edges)
- [ ] **P3-3** Binding panel: show selected device with ports
- [ ] **P3-4** Add devices to binding canvas for wiring
- [ ] **P3-5** Wire drawing (drag output → input, SVG path rendering)
- [ ] **P3-6** Wire state management (ADD_BINDING/REMOVE_BINDING actions)
- [ ] **P3-7** Delete wire (click + Delete key)
- [ ] **P3-8** Logic blocks (NOT/AND/OR/XOR) as visual elements on wires
- [ ] **P3-9** Device controls + live state in binding panel (reuse unit-control.js)
- [ ] **P3-10** Timer blocks on wires (Td/Tf/Tr binding types)
    * Output port on RIGHT side (colored dot)
    * Label shows logic type name ("NOT", "AND", "OR", "XOR")
    * Color-coded: NOT=purple, AND=blue, OR=green, XOR=orange
  - **Placement in binding canvas**:
    * Drag logic block from toolbar/palette into binding canvas
    * Position between controller output and controllable input
    * Wires connect: controller → logic block input(s) → logic block output → controllable
  - **Wire connections**:
    * Input wires: multiple controller outputs can connect to logic block inputs
    * Output wire: logic block output connects to one or more controllable inputs
    * Wire colors reflect source (controller output color)
  - **Configuration**:
    * Click block → opens config panel
    * AND/OR: configure number of inputs (2-8)
    * Set inversion per input (NOT modifier per input)
    * Advanced: debounce time, trigger edge (rising/falling/both)
  - **State management**:
    * Logic blocks stored in `project.logicBlocks[]` with `{id, type, inputs[], output, config}`
    * Bindings reference logic block ID: `{from: {deviceId, portId}, to: {logicBlockId, inputIndex}}`
    * Output binding: `{from: {logicBlockId, output}, to: {deviceId, portId}}`
  - **Serialization to `bind*.txt`**:
    * Maps to `C` (Conditional) binding type
    * Duotecno protocol supports AND/OR/NOT/XOR in binding strings
    * Complex logic may require multiple binding entries or intermediate virtual units
  - **UI workflow**:
    1. User drags "AND" block from palette to canvas
    2. Draws wire from Switch 1 kort → AND input 1
    3. Draws wire from Switch 2 kort → AND input 2
    4. Draws wire from AND output → Lamp input
    5. Result: Lamp only turns on when BOTH switches pressed
  - **Future enhancements**:
    * Compound blocks: combine multiple gates (e.g., NAND = NOT + AND)
    * Visual simulation: show block state in real-time (inputs/output active/inactive)
    * Copy/paste blocks with connected wires
    * Block templates library
- [ ] **P3-5** Insert timer blocks on wire (delayed/flashing/repeat) → `Td`/`Tf`/`Tr` binding types
  - **TODO LATER**: Add timer/delay blocks to bindings
  - Timer blocks appear as visual elements on wire paths
  - Configurable delay time, flash rate, repeat count
  - Maps to `Td`, `Tf`, `Tr` binding types with time parameters
- [ ] **P3-6** Serialize wiring diagram → structured bindings in project file
- [ ] **P3-7** Deserialize structured bindings → wiring diagram (for existing `.duo` files)

### Phase 4 — Moods editor

- [ ] **P4-1** Auth client: `HTTPS :8081/auth/create|refresh` → store client_id + tokens; auto-refresh before expiry
  - Note: devices use self-signed certs → `rejectUnauthorized: false` may be needed *(see Q-2)*
- [ ] **P4-2** Mood list modal per LCD node: `GET :8080/moodscfg/info`
- [ ] **P4-3** Edit mood: name, description, userAdjustable; entries `{nodeAddress, unitAddress, msg, delay}`
  - `msg` format = data message string (see appendix in `api - moodscfg` spec, per unit type)
- [ ] **P4-4** Store moods in project file (dirty flag)
- [ ] **P4-5** Upload moods: `POST :8080/config/upload/moods.json` (triggered by Upload to hardware)

### Phase 5 — Scheduling editor

- [ ] **P5-1** Schedule list modal: `GET :8080/schedulecfg/info`
- [ ] **P5-2** Edit schedule item: weeks (week1/2/3/holiday), days, time, action (sensor: sun/moon/…; virtual: puls/ton/toff/…)
- [ ] **P5-3** Alarm clock config: `GET/POST/DELETE :8080/schedulecfg/clockalarm`
- [ ] **P5-4** Store schedules in project file (dirty flag)
- [ ] **P5-5** Upload schedules: `POST :8080/config/upload/scheduling.json`

### Phase 6 — Per-module config modals

- [x] **P6-1** Module detail modal (click module in Rail View) — **DONE**
  - Product image, channel groups with colour-coded dots + counts, dinUnits + powerW
  - Rename (label), node address (hex) input
  - Reorder: ← Links / Rechts → buttons (triggers `MOVE_MODULE`)
  - Delete button with confirm (triggers `REMOVE_MODULE`)
  - Hardware discovery info section with ✓/✗ status badges
  - Auto-suggestions for UNKNOWN modules based on discovered unit capabilities
  - "Fetch units" button for per-node incremental discovery (refresh without full rescan)
- [x] **P6-2** Woning device detail modal (click switch/LCD in woning panel) — **DONE**
  - Same pattern: image, rename, address, delete (`REMOVE_WONING_DEVICE`)
  - Hardware discovery info + fetch units button (parity with cabinet modules)
  - "Move to cabinet" button for UNKNOWN woning devices (misclassified smartboxes)
  - **Code consolidation**: Unified `openDeviceDetailModal()` handles both cabinet + woning devices
    * Three shared sub-functions: `buildDiscoverySection()`, `buildCapabilitiesSection()`, `buildChannelGroupsSection()`
    * Context object distinguishes device type: `{type:'module', cabinetId}` vs `{type:'woning'}`
    * Same-size displays (150×120px) for all device types
    * Generic state actions (UPDATE_MODULE / UPDATE_WONING_DEVICE) dispatched based on context
    * Reduced code: 1687 → 1374 lines (-313 lines, ~18% reduction)
- [ ] **P6-3** LCD / Touchscreen config modal
  - Multi-pane (one pane per screen page); link to mood/schedule editors for this node
- [ ] **P6-4** Switch config modal: button labels, room assignment
- [ ] **P6-5** Smartbox plugin slot configurator (in module detail modal)
  - Show `slotCount` slots for all `isSmartboxBase` modules
  - Each slot: dropdown to select plugin model (DT0B-LE/DC/PW/TE/R/SSR/DAL12/DAL64) or empty
  - Selecting a plugin updates `ModuleInstance.slots[]` and recalculates total channelGroups for resource panel
  - DT0B-01/ULTRA support dimmer plugins; DT0B-LITE supports relay-only plugins
  - Derived total channels = fixed `channelGroups` + channels contributed by each slot's plugin
  - Note: DT0B-ULTRA slot count unknown — verify with hardware docs before implementing
- [ ] **P6-6** Module database editor (internal maintenance tool)
  - CRUD UI for `reader/modules/_index.json` entries
  - Fields: model, name, category, uiCategory, channelGroups, powerW, dinUnits, imageFile, variants
  - Import/export JSON; scrape product page URL to pre-fill fields
  - Useful as long as the product range evolves and scraper can't auto-populate everything

### Phase 7 — Save & Upload

- [x] **P7-1** "Save" button → serialize project to `.duo` JSON → `POST /api/project/save`
  - `src/api/projectAPI.ts` handles save + list + load
  - Frontend in `main.js → saveProject()`
- [x] **P7-2** "Open project" → load `.duo` file
  - Lists `.duo` files from `reader/projects/` directory
  - Frontend in `main.js → openProject()`
- [ ] **P7-3** Import existing installation (no `.duo`, only `bind*.txt` + node DB)
  - Parse binding strings → reconstruct what's possible into wiring diagram
  - Unknown/complex bindings kept as "legacy bindings" visible but not visually editable until re-wired
  - All devices land in "House" room
- [ ] **P7-4** Upload bindings to hardware
  - Serialize structured bindings → `bind*.txt` format strings
  - Default: `/upload/changed` (XOR checksum diff, skip unchanged)
  - Option: upload all (force), upload single node
  - Show per-node result: uploaded / skipped / failed
- [ ] **P7-5** Upload moods (per dirty LCD node)
- [ ] **P7-6** Upload schedules (per dirty node)

### Phase 8 — UDP Discovery & connection status

- [ ] **P8-0** Subnet scan: TCP-probe port 5001 on all 254 addresses of the local subnet
  - Send a small test packet to x.x.x.1…x.x.x.254 port 5001 with short timeout (~300ms)
  - Return list of responding IPs for the user to select
  - Useful when the installer doesn't know the master IP
- [x] **P8-1** Connect modal (click connection status in header) — **DONE**
  - IP address + password inputs (pre-filled from project.meta.masterIp / masterPassword)
  - Two-phase discovery modes:
    * **Quick connect** (default): Scans nodes only (3-5 seconds) — gets node list, addresses, types, names
    * **Full scan** (optional): Scans nodes + units (30+ seconds) — also fetches all unit details per node
  - Calls `POST /api/master/connect?fullScan=true|false` → polls `GET /api/master/status` until 'ready'
  - Fetches `GET /api/master/nodes`, dispatches SET_CONNECTION with nodes
  - Summary toast: X nodes found, Y matched, Z not in project
  - masterIp + masterPassword saved in project meta via SET_MASTER_CONFIG
  - **Per-node incremental fetch**: "Fetch units" button in device detail modals for on-demand unit loading
- [x] **P8-2** Map discovered nodes to project modules — **DONE**
  - `state.discoveredNodes[]` holds runtime DiscoveredNode[] (not persisted)
  - Module card shows ✓ badge (green) if node found, ? badge (orange) if no nodeAddress assigned after scan, ! badge (red) if nodeAddress set but not found
  - Three clear badge states: ✓ found (green), ? orphan (orange), ! missing (red — **no pulse animation**, symbols are clear enough)
  - Woning devices: badges display inline after address: `0xFC ✓` (no overlap with device name)
  - Header connection-status shows IP even when disconnected (last used)
  - Hardware info section in device detail modals shows discovery status + unit counts
  - Gateway nodes (NodeType 4) explicitly classified to cabinet — fixes issue where TCP server/master nodes were misclassified as woning devices
- [x] **P8-3** Header dropdown menu for file operations — **DONE**
  - Consolidated three buttons into `📁 Project ▾` dropdown: Nieuw Project, Openen, Opslaan, Hernoem
  - Changed "Upload naar hardware" → "Verstuur bindings"
  - Added `newProject()` function with confirmation dialog
  - Added `renameProject()` function — replaces double-click on project name (removed to reduce code)
  - Dropdown closes on outside click, styled to match header theme
  - Frees up header space for additional functions
- [ ] **P8-4** Compare discovery results vs. project: full diff panel
  - Show matched / missing in design / extra on network
- [ ] **P8-4** Auto-populate Rail View from discovery (unmatched nodes → add as "Unknown" module placeholder)
- [ ] **P8-5** Update node firmware name via TCP hardware command
  - Module/woning device modals show "Firmware naam" from discovery (read-only for now)
  - Future: allow editing → send new name to hardware via TCP protocol
  - Needed: TCP command for writing node name string (ask engineers)
- [ ] **P8-6** Node type code → module model lookup table (from Duotecno engineers)
  - `DiscoveredNode.type` byte identifies hardware type (e.g. 0x12 = DT05-06, etc.)
  - With this table: auto-assign correct module model without unit-composition guessing
  - Also resolves the CONTROL-type ambiguity (wall switch vs. input module in cabinet)

### Phase 9 — Materiaallijst

- [ ] **P9-1** Generate BOM from project data (not hardcoded)
- [ ] **P9-2** Excel export
- [ ] **P9-3** PDF / print view for electrician

---

## Binding → wiring diagram type mapping

| Binding type | Wiring diagram element |
|-------------|----------------------|
| `N` Normal | Simple wire |
| `C` Conditional | Wire + NOT/AND/OR logic block |
| `Td` Timer Delayed | Wire + timer block |
| `Tf` Timer Flashing | Wire + timer block |
| `Tr` Timer Repeat | Wire + timer block |
| `Ti` Timer Interval | Wire + timer block |
| `To` Timer Oneshot | Wire + timer block |
| `G` Group | Mood (separate Mood API, not wiring diagram) |
| `P` Property | Property override — dedicated UI element (deferred) |
| `I` Immediate | Simple wire variant |

---

## HTTP API endpoints (all `:8080` with Bearer auth from `:8081`)

### Moods
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/moodscfg/tag` | Check for changes |
| GET | `/moodscfg/info` | Summary of all moods |
| GET | `/moodscfg/[id]` | Single mood |
| POST | `/moodscfg/` | Create / edit mood |
| DELETE | `/moodscfg/[id]` | Delete mood |
| GET | `/config/download/moods.json` | Full download |
| POST | `/config/upload/moods.json` | Full upload |

### Scheduling
| Method | Endpoint | Description |
|--------|----------|-------------|
| GET | `/schedulecfg/tag` | Check for changes |
| GET | `/schedulecfg/info` | Summary |
| GET | `/schedulecfg/[node]/[unit]` | Single item |
| POST | `/schedulecfg` | Create / edit item |
| DELETE | `/schedulecfg/[node]/[unit]` | Delete item |
| GET/POST/DELETE | `/schedulecfg/clockalarm` | Alarm clock |
| GET | `/config/download/scheduling.json` | Full download |
| POST | `/config/upload/scheduling.json` | Full upload |

---

## Stubs in demo5 → phase that replaces them

| Stub | Phase |
|------|-------|
| `+ Kast toevoegen` | P1-3 |
| `+ Rail toevoegen` | P1-3 |
| `+ Schakelaar` / `+ LCD` | P1-4 |
| `+ Kamer toevoegen` | P2-2 |
| `selectRoom()` | P2-3 |
| `addDeviceToRoom()` | P2-4 |
| `openModuleDetail()` | P6-1 |
| Binding wizard (`openBW` / `saveBW`) | P3 |
| Save button | P7-1 |
| Upload button | P7-4 |
| Export Excel / PDF | P9-2, P9-3 |
| Connection status indicator | P8-2 |

---

## Open questions *(add new ones here; none blocking Phase 0)*

- **Q-1** Reconstruct wiring graph from `bind*.txt`: research during P7-3. Some bindings may only be "legacy" strings initially — show them in the UI but not visually editable until manually re-wired.
- **Q-2** Auth API uses HTTPS with device self-signed certs. Node.js `https` client will likely need `rejectUnauthorized: false`. Verify during P4-1.
- **Q-3** Moods live on LCD/Touchscreen nodes (virtual units). If no LCD is configured yet, can moods live on the master node? Clarify during P4 implementation.
- **Q-4** Scheduling API: spec uses `/schedulescfg/tag` (extra 's') in one place and `/schedulecfg/tag` elsewhere. Confirm correct URL against lab during P5.

---

## Module database design (settled)

### 1 — Switch cosmetic variants

Many switches (Room-E, Serenity, Easy Touch, etc.) are functionally identical — same channels, same CAN behaviour — but sold in 20+ finishes. The database has one entry per finish (needed for BOM/ordering), but the Configuration Software only cares about the functional type.

**Solution: `functionalModel` + `finish` fields**

```
DT1E-4Ab  → functionalModel: "DT1E-4",  finish: "Aluminium Zwart"
DT1E-4Br  → functionalModel: "DT1E-4",  finish: "Brons"
DT1E-4BCR → functionalModel: "DT1E-4",  finish: "Brushed Copper Rose"
```

- **Rail/Home View**: groups by `functionalModel`; finish is a cosmetic picker
- **Channel definitions**: defined once on `functionalModel`, shared by all variants
- **Bindings**: reference `functionalModel` (behaviour is the same)
- **BOM/Materiaallijst**: uses specific `model` code (needed for ordering)
- **Scraper**: add `functionalModel` (strip trailing color code) and `finish` to all switch/lcd/sensor products

Functional model groups in the current database:
| functionalModel | Count | Type |
|----------------|-------|------|
| DT0C-10 | 8 | 10" Touchscreen |
| DT0C-7 | 4 | 7" Touchscreen |
| DTBS-ES1 | 2 | Essence 1-button |
| DTBS-ES2 | 2 | Essence 2-button |
| DTBS-ES4 | 2 | Essence 4-button |
| DTBS-B1 | 4 | Serenity 1-button |
| DTBS-B2 | 4 | Serenity 2-button |
| DTBS-B4 | 4 | Serenity 4-button |
| DTBS-H1 | 4 | Velocity 1-lever |
| DTBS-H2 | 4 | Velocity 2-lever |
| DTBS-H4 | 4 | Velocity 4-lever |
| DTBS-4 | 12 | Touchbutton 4-ch |
| DT1ET-4 | 12 | Easy Touch |
| DT1E-4 | 13 | Room-E |
| DT40 | 11 | Motion detector |
| DT30 | 9 | Temperature sensor |

### 2 — Smartbox+ plug-in configuration

The Smartbox+ (DT0B-01/LITE/MINI) is a modular base unit with plug-in slots. Each slot accepts one plug-in board that defines the channel type. The Configuration Software must configure each slot before channels become available.

**Slot counts:**
| Base model | Slots |
|-----------|-------|
| DT0B-01 | 4 |
| DT0B-LITE | 4 |
| DT0B-MINI | 3 |

**Plugin types** (each occupies one slot, provides 1 channel):
| Plugin | `model` | Channel type |
|--------|---------|-------------|
| LE dimmer (fase aansnijding) | DT0B-LE | `dimmer_le` |
| TE dimmer (fase afsnijding) | DT0B-TE | `dimmer_te` |
| 0-10V / 1-10V dimmer | DT0B-DC | `dimmer_dc` |
| LED PWM dimmer | DT0B-PW | `dimmer_pwm` |
| Relay | DT0B-R | `relay_no` |
| Solid state relay | DT0B-SSR | `relay_ssr` |

**Module DB fields for Smartbox:**
```json
{ "isSmartboxBase": true,  "slotCount": 4 }   ← on DT0B-01/LITE/MINI
{ "isSmartboxPlugin": true, "channelGroups": [{ "type": "dimmer_te", "count": 1 }] }  ← on DT0B-TE
```

**In the project file:** a Smartbox instance stores its slot configuration:
```json
{
  "model": "DT0B-01",
  "nodeAddress": 3,
  "slots": ["DT0B-TE", "DT0B-TE", "DT0B-R", "DT0B-PW"]
}
```
Its channel list is derived at runtime from the slot array.

**UI: Smartbox config modal** (P6-1 extension) — when clicking a DT0B in Rail View, show a slot configurator letting the installer pick a plugin per slot. Channel list updates live as slots are configured.

### 3 — Channel definitions (`channelGroups` replaces `channels`)

Replace the scalar `channels` count with a structured `channelGroups` array on every module.

**`ChannelType` enum:**
```
relay_no     relay_nc     relay_ssr
dimmer_le    dimmer_te    dimmer_pwm    dimmer_dc
motor_updown motor_polar
input_digital input_analog
dali         dmx          audio
can_switch
```

**`ChannelGroup` shape:**
```json
{ "type": "dimmer_te", "count": 6, "maxLoadW": 500, "label": "Trailing edge dimmer" }
{ "type": "input_digital", "count": 16 }
{ "type": "input_analog", "count": 2 }
```

Examples of how existing modules map:
| Module | channelGroups |
|--------|--------------|
| DT05-06TE | `[{ type: dimmer_te, count: 6, maxLoadW: 500 }]` |
| DT05-06 | `[{ type: dimmer_le, count: 6, maxLoadW: 500 }]` |
| DT03-06 | `[{ type: relay_no, count: 6, maxCurrentA: 16 }]` |
| DT03-06NC | `[{ type: relay_nc, count: 6, maxCurrentA: 10 }]` |
| DT04-03 | `[{ type: motor_updown, count: 3, maxCurrentA: 16 }]` |
| DT04-P3 | `[{ type: motor_polar, count: 3, maxCurrentA: 10 }]` |
| DT07-16 | `[{ type: input_digital, count: 16 }, { type: input_analog, count: 2 }]` |
| DT13-DAL | `[{ type: dali, count: 64 }]` |
| DT0B-01 configured | derived from slots |
| DT1E-4 (switch) | `[{ type: input_digital, count: 4 }]` — 4 buttons |

**Todo items for this:**
- [x] **P0-2a** Update module JSON schema: add `channelGroups`, `functionalModel`, `finish`, `isSmartboxBase`, `isSmartboxPlugin`, `slotCount` fields to `DuotecnoProduct` type
- [x] **P0-2b** Update scraper to extract `functionalModel`/`finish` from switch/sensor names and `channelGroups` from descriptions
- [x] **P0-2c** Manually define `channelGroups` for all DIN modules and Smartbox plugin `channelGroups` — **DONE** (all families have channelGroups now)
- [x] **P0-2d** Add Smartbox `isSmartboxBase`/`isSmartboxPlugin`/`slotCount` to relevant entries — **DONE**
- [ ] **P0-2e** Fill in missing metadata for new entries: sourceUrl, pdfUrl, images for DT1C-4, DT43, DT13-ARI, DT13-BO, DMX-PWM, DT0D-*, DT20-LIC, DT00-DAL, DT07-DAL, DT0B-DAL*, DT00-12

---

## Next Steps — Implementation Plan

### Immediate priorities (Demo-ready features)

**Branch strategy**: Keep `main` branch demo-ready. Create `feature/material-units` branch for P2-4a.

**Recommended implementation order:**

1. **P2-8** Drag-and-drop device positioning in rooms
   - Essential for floor plan-based layouts
   - Quick win: device cards already exist, just add drag handlers
   - Store x/y coordinates in room device model
   - Optional: snap-to-grid for alignment
   - **Estimated effort**: 2-4 hours

2. **P2-9** Remove/move device between rooms  
   - Completes device management workflow
   - Context menu on device card: "Verwijder" / "Verplaats naar..."
   - Bindings should persist through moves
   - **Estimated effort**: 2-3 hours

3. **P3-4** Logic blocks (NOT, AND, OR, XOR) in binding view
   - Visual logic gates as draggable blocks
   - Input/output ports for wire connections
   - Foundation for advanced binding logic
   - Maps to `C` (Conditional) binding type
   - **Estimated effort**: 6-8 hours
   - **Why before P2-4a**: Demonstrates binding capabilities without changing data model

4. **P2-4a** Link room devices to real material units (MAJOR — separate branch)
   - ⚠️ **Create branch**: `git checkout -b feature/material-units`
   - Replace mock device picker with real unit picker
   - Show available units from Rail View with module origin
   - Implement bidirectional link: room device ↔ physical unit
   - Mark units as used/unused
   - Allow name editing with sync to underlying unit
   - **Test thoroughly before merge** — this changes data model significantly
   - **Estimated effort**: 12-16 hours

### Medium-term features (Live control + wiring)

4. **P3-1e** Device controls and live state in binding view
   - Study ProApp rendering code: `../duotecnopro/src/app/rendering`
   - Extract control layouts per device type
   - Implement modal/panel for device controls
   - Connect to existing `unit-control.js` for TCP communication
   - Add state subscription for real-time updates

5. **P3-1b** Device port visualization in Home View
   - Show colored dots on device cards (inputs left, outputs right)
   - Foundation for wire drawing

6. **P3-2** Wire drawing between devices
   - Drag from output dot to input dot
   - SVG path rendering with curves
   - Create binding entries in project

### Lower priority (Deferred)

- **P2-5** (remainder): Floor plan crop/select after upload
- **P3-4/P3-5**: Logic blocks and timers on wires
- **P4**: Moods editor
- **P5**: Scheduling editor
- **P6-5**: Smartbox plugin configurator

---

## Ideas / Future Enhancements

These are deferred indefinitely — may be useful for v2.0 but not essential for launch:

- **P2-3** Click room in sidebar → zoom canvas to show only that room full-width
  - Nice-to-have: sidebar navigation to specific room
  - Can be done with simple scroll-to behavior instead of zoom
  
- **P2-7** Zoom in/out controls for Home View canvas
  - Mouse wheel or toolbar buttons for zoom
  - Useful for large installations with many rooms
  - Complex interaction with drag-drop and wiring — defer until those are stable

- **Visual feedback on device icons** (mentioned in P3-1e)
  - Lamp icons glow when ON
  - Motor icons show animated arrows during movement
  - Dimmer brightness proportional to value
  - Requires real-time state subscription — implement after P3-1e controls are working

- **Multi-room device usage** (mentioned in P2-4a future enhancements)
  - Allow same lamp to appear in multiple rooms (same circuit)
  - Show "Gebruikt in: Keuken, Badkamer" badge
  - Requires careful UX to avoid confusion

- **Undo/redo for all actions**
  - Command pattern on state.dispatch()
  - Useful but adds complexity — defer until core features are stable
