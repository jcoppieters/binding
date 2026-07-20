# UI TODO — Central Reference Document

> **This is the single source of truth. Keep it up to date with every session.**

**Last updated:** 2026-07-20

**Recent completions (July 2026):**
- ✅ Multi-button switch system (DTBS-ES/B families) with per-button binding support
- ✅ Visual binding editor with wire drawing, device ports, and SVG rendering
- ✅ Temperature sensor type detection fix (hardware discovery patching)
- ✅ Image storage in .img companion files (clean JSON separation)
- ✅ Floor plan crop modal with 8 resize handles
- ✅ Device visibility improvements (white backgrounds, faded floor plans)
- ✅ Binding panel device persistence (manually added devices stay until binding created)
- ✅ Per-button wire filtering (show only active button's wires)

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

- [x] **P2-5** Floor plan import (image overlay, toggle) — **DONE** ✅
  - ✅ Per-room background image upload via room ... menu (🗺️ Grondplan toevoegen)
  - ✅ File input → FileReader → save as data URL to room.backgroundImage
  - ✅ Crop modal with drag and 8 resize handles (extracted to crop-modal.js)
  - ✅ Separate background image layer with independent opacity control (25% faded)
  - ✅ Background color layer at 85% opacity for visibility
  - ✅ "Verwijder grondplan" menu item appears when room.backgroundImage is set
  - ✅ Images stored in .img companion files (separate from .duo for clean JSON)
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

- [ ] **P2-11** Enhance module detail modal with Units tab
  - **Current state**: Modal shows general info (channelGroups, node type, firmware)
  - **Enhancement**: Add tab navigation — "Info" (current) + "Units" (new)
  - **Units tab content**:
    * List all units (channels) for this module instance
    * Per unit: name/label, type (relay/dimmer/input), unit #, status (gebruikt/beschikbaar)
    * For outputs: current value (ON/OFF, dimmer %, motor position) — live if connected
    * Group by channel type (e.g., all relays, then all dimmers)
    * Click unit → highlight in Home View (if assigned to room)
  - **Benefits**:
    * Verify unit names/labels match expectations before assigning to rooms
    * See which units are already used and where
    * Troubleshoot: "Why can't I see Unit 3?" → check if it exists in module
    * Bridge gap between Rail View hardware and Home View devices
  - **Implementation notes**:
    * Reuse `generateUnitsForModule()` from `unit-helpers.js`
    * Add `analyzeUnitUsage()` to mark used units with room info
    * Tab UI: two buttons at top, swap content div on click
    * Live status: fetch from `/api/master/node/:address/units` (if connected)

- [ ] **P2-12** Improve unit picker UX flow
  - **Current state**: Filter tabs (All/Uitgang/Ingang) + search box → flat list
  - **Desired flow** (after filter selection):
    1. **Filter by I/O direction**: All / Uitgang (outputs) / Ingang (inputs)
    2. **Then show device TYPE groups**: Lamp 💡 | Relais ⚡ | Dimmer 🎚️ | Motor 🪟 | etc.
    3. **Within each type**: List units with **specifications + status**:
       - For dimmers: type (fase aansnijding / fase afsnijding / PWM / DC)
       - For relays: type (NO / NC / SSR)
       - For motors: type (up/down / polar)
       - For all: **Beschikbaar** / **Gebruikt in: Keuken**
    4. User clicks a unit → name prompt → device added to room
  - **Benefits**:
    * Easier to find "I need a dimmer" without knowing exact module names
    * Specifications visible before selection (avoid wrong dimmer type)
    * Clear visual grouping by function (all lamps together)
  - **Implementation**:
    * After filter, compute type counts: `{lamp: 12, relay: 8, dimmer: 4, ...}`
    * Show type buttons with counts: "💡 Lamp (12)" "⚡ Relais (8)"
    * Click type → filter _filteredUnits by that device type
    * Add specs to unit card (read from channelType + moduleDef)
  - **Alternative**: Keep current flat list, but add type badges + specs inline
    * Pros: simpler, less clicks
    * Cons: longer scroll, harder to scan
  - **Decision**: User feedback needed — test current UI first

### Phase 3 — Visual wiring diagram (Binding View)

**Goal:** Visual node-graph editor where devices show input/output ports and you draw wires between them.

**Current Status:**
- [x] Basic room device system with 7 device types (💡 Lamp, 🔘 Schakelaar, ⚡ Relais, 🎚️ Dimmer, 🪟 Motor, 🌡️ Sensor, 🔳 Drukknop)
- [x] Device cards (100×100px) with icons, colors, positioning (white backgrounds with colored borders)
- [x] Drag-and-drop positioning within rooms
- [x] Device menu (move/delete)
- [x] Floor plan upload per room with crop modal
- [x] Room resizing
- [x] Binding panel with device display and port visualization
- [x] Multi-button switch support with per-button binding filtering
- [x] Wire drawing and rendering between device ports
- [x] Wire deletion
- [x] Binding state management (ADD_BINDING/REMOVE_BINDING actions)

**Port System:**
- **Controllers** (physical inputs that generate events): outputs on RIGHT
  * Smartbox digital inputs (input_digital): "Druk" (press event)
  * Smartbox analog inputs (input_analog): "Waarde" (value change)
  * Switches/buttons in rooms: "Kort", "Lang", "Druk", "Dubbel"
  * Sensors: "Trigger", "Waarde"
  * **Important**: Hardware "inputs" are software "outputs" — they OUTPUT events/signals
- **Controllables** (devices that receive commands): inputs on LEFT
  * Lamps/relays: "Aan", "Uit", "Schakel"
  * Dimmers: "Aan", "Uit", "Dim+", "Dim-", "Stel waarde"
  * Motors: "Op", "Neer", "Stop"
- **Binding panel layout**: Controllers left column, controllables right column, SVG wires between
- **State**: `project.bindings[]` with `{id, from: {deviceId, portId}, to: {deviceId, portId}, color}`

**Tasks:**

- [x] **P3-1** Define DEVICE_PORTS constant with ports per device type — **DONE** ✅
  - Map channelType to port definitions:
    * `input_digital` → outputs: ["Kort", "Lang"]
    * `input_analog` → outputs: ["Waarde"]
    * `relay_*` → inputs: ["Aan", "Uit", "Schakel"]
    * `dimmer_*` → inputs: ["Aan", "Uit", "Schakel", "Dim+", "Dim-", "PIR", "Stel waarde"]
    * `motor_*` → inputs: ["Op", "Neer", "Stop"]
    * `temperature` → outputs: ["Temp +", "Temp -", "Koelen", "Verwarmen"]
  - Room device types (switch/button/sensor) also get output ports
  - Multi-button switches: per-button channel filtering with button selector UI
- [x] **P3-2** Device card port visualization (colored dots on card edges) — **DONE** ✅
- [x] **P3-3** Binding panel: show selected device with ports — **DONE** ✅
  - Controllers on left, controllables on right
  - Button selector for multi-button switches (2-column grid)
  - Per-button/sensor binding filtering with channelRef tracking
- [x] **P3-4** Add devices to binding canvas for wiring — **DONE** ✅
  - Drag from room → binding panel to add devices
  - Manually added devices persist until binding created
  - Auto-load connected devices based on existing bindings
- [x] **P3-5** Wire drawing (drag output → input, SVG path rendering) — **DONE** ✅
  - Bezier curves from output ports (right) to input ports (left)
  - Color-coded by port type
  - Live wire preview during drawing
- [x] **P3-6** Wire state management (ADD_BINDING/REMOVE_BINDING actions) — **DONE** ✅
  - Bindings stored with channelRef for multi-button switches
  - State subscription for real-time updates
- [x] **P3-7** Delete wire (click + confirm) — **DONE** ✅
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
- [ ] **P3-6** Moods as inputs for logic blocks (AND/OR/XOR triggers)
  - **Context**: Moods (type 7, virtual units on master node 0xFC) currently have outputs only
  - **Goal**: Allow moods to be activated by logic block outputs (e.g., AND of multiple sensors)
  - **Use cases**:
    * "Night mode" mood activated when (Dark outside AND After 22h)
    * "All off" mood triggered by (Help button OR Alarm sensor)
    * "Party mode" activated by (Weekend AND Evening AND Motion sensor)
  - **Implementation**:
    * Add input ports to mood device type: `{ id: 'activate', label: 'Activeer', color: '#10b981' }`
    * Moods can receive wires from logic block outputs OR direct controller outputs
    * When mood input receives trigger → activate mood (same as manual mood selection)
  - **State management**:
    * Binding: `{from: {logicBlockId, output}, to: {deviceId: moodId, portId: 'activate'}}`
    * Or direct: `{from: {deviceId: switchId, portId: 'kort'}, to: {deviceId: moodId, portId: 'activate'}}`
  - **Serialization**: Maps to binding with mood unit as target (type 7 unit)
  - **UI TODO**: Research how Duotecno hardware handles mood activation via bindings
    * Check if moods can be bound as targets in existing binding software
    * Verify protocol support for mood activation triggers
    * Document any limitations or special cases
  - **Priority**: P3 (after basic logic blocks working) — defer to later phase
- [ ] **P3-7** Serialize wiring diagram → structured bindings in project file
- [ ] **P3-8** Deserialize structured bindings → wiring diagram (for existing `.duo` files)
- [ ] **P3-9** Show current state (when connected) of all output-devices (dimmers, relais, motor, ...)
- [ ] **P3-10** Allow controlling the possibility of the inputs (generate short, long press, ...)

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
- [x] **P6-5** Smartbox plugin slot configurator (in module detail modal) — **DONE** ✅
  - **Status**: Fully implemented and tested
  - **UI**: Plugin slots section appears in module detail modal for all `isSmartboxBase` modules (DT0B-01, DT0B-LITE, DT0B-MINI)
  - **Slot configuration**:
    * Shows `slotCount` dropdown selects (4 for DT0B-01)
    * Plugin options per slot: DT0B-LE/TE/PW/DC/R/SSR or "— Leeg —"
    * Changes saved instantly via UPDATE_MODULE_SLOTS action
    * Fixed bug: Each slot change now fetches fresh state (prevents overwriting previous changes)
  - **Unit generation**: `generateUnitsForModule()` processes base channelGroups + plugin channelGroups
  - **Result**: Configured plugins now appear as units in unit picker (e.g., 2× DT0B-TE → 2 dimmers in Uitgang filter)
  - **Testing**: ✅ Multi-slot configuration works (Slot 1-3 all retained correctly)
  - **Note**: DT0B-ULTRA slot count unknown — verify with hardware docs before implementing
- [ ] **P6-6** Module database editor (internal maintenance tool)
  - CRUD UI for `reader/modules/_index.json` entries
  - Fields: model, name, category, uiCategory, channelGroups, powerW, dinUnits, imageFile, variants
  - Import/export JSON; scrape product page URL to pre-fill fields
  - Useful as long as the product range evolves and scraper can't auto-populate everything

### Phase 7 — Save & Upload

- [x] **P7-1** "Save" button → serialize project to `.duo` JSON → `POST /api/project/save` — **DONE** ✅
  - `src/api/projectAPI.ts` handles save + list + load
  - Frontend in `main.js → saveProject()`
  - Images stored in separate .img companion files (clean JSON + image data separation)
  - 50MB body parser limit for large images
- [x] **P7-2** "Open project" → load `.duo` file — **DONE** ✅
  - Lists `.duo` files from `reader/projects/` directory
  - Frontend in `main.js → openProject()`
  - Auto-merges .img files back into project on load
- [ ] **P7-4** Upload bindings to hardware
  - Serialize structured bindings → `bind*.txt` format strings
  - Default: `/upload/changed` (XOR checksum diff, skip unchanged)
  - Option: upload all (force), upload single node
  - Show per-node result: uploaded / skipped / failed
- [ ] **P7-5** Upload moods (per dirty LCD node)
- [ ] **P7-6** Upload schedules (per dirty node)

### Phase 7b — Import existing installation from hardware

**Goal:** Read binding files from hardware and convert to visual wiring diagram

**Backend parsers available:**
- ✅ `BindingFileParser` (src/parsers/bindingFile.ts) - parses bind*.txt files
- ✅ `generateBindingString()` / `parseBindingString()` (src/api/bindingEditorAPI.ts) - high-level binding conversion
- ✅ `BindingManager` (src/services/BindingManager.ts) - in-memory CRUD for binding strings
- ✅ TCP download protocol (src/communication/) - fetch bind*.txt from hardware

**Tasks:**

- [ ] **P7b-1** Download all binding files from connected hardware **⚠️ NEEDS RESEARCH**
  - **Current status**: TCP protocol supports **uploading** bindings (✅ `BindingWriterFSM`, `BindingUploader`)
  - **Missing**: No download/read protocol implementation found in codebase
  - **requestFileInfo exists**: Can fetch binding count + XOR checksum from node (for change detection)
  - **Question for engineers**: Does the Duotecno TCP protocol support downloading bind*.txt from hardware?
    * If YES: implement download FSM (reverse of BindingWriterFSM)
    * If NO: **Skip P7b-1** → use P7b-1a alternative below
  - Show progress: "Downloading bindings from node 0x03... (1/15)"
  - Handle errors: missing files, connection issues, timeout
  
- [ ] **P7b-1a** Import binding files from local Config folder (ALTERNATIVE to P7b-1) **✅ READY NOW**
  - **Simpler approach**: Load bind*.txt files from disk instead of hardware
  - User workflow:
    1. User clicks "Import bindings" in header dropdown
    2. File picker: select Config folder (contains bind*.txt files)
    3. Read all bind*.txt files from selected folder
  - **Advantages**:
    * No TCP download protocol needed
    * Works offline (no hardware connection required)
    * Uses existing `BindingFileParser` (already implemented ✅)
    * Most projects already have bind*.txt files saved
  - **Implementation**: 
    * Native file picker via `<input type="file" webkitdirectory>` or Electron/Tauri file API
    * Read files via FileReader API (browser) or fs module (Electron/Tauri)
    * Pass to existing `BindingFileParser` backend
  
- [ ] **P7b-2** Parse binding strings into structured format
  - Use `BindingFileParser` to parse each bind*.txt file
  - Extract binding entries: type, flags, input/output addresses, events, actions
  - Handle all binding types: I (Immediate), N (Normal), C (Conditional), Td/Tf/Tr/Ti/To (Timers), G (Group)
  - Collect parse errors for user review
  
- [ ] **P7b-3** Match bindings to project devices
  - Map node addresses to modules in Rail View (use discovery data)
  - Map unit addresses to specific channels (relay 1, dimmer 2, etc.)
  - Create device entries in Home View for bound units
  - All unassigned devices land in default "House" room
  
- [ ] **P7b-4** Reconstruct visual bindings for simple cases
  - **Simple bindings** (Type I/N): direct controller → controllable wire
    * Input: switch button → Output: lamp relay
    * Convert to visual wire in binding panel
  - **Conditional bindings** (Type C): logic blocks (NOT/AND/OR/XOR)
    * Parse operator and operands
    * Create logic block visual element
    * Connect inputs and output wires
  - **Timer bindings** (Td/Tf/Tr/Ti/To): timer blocks
    * Parse timer parameters (delay, flash rate, repeat count)
    * Create timer block visual element on wire
  
- [ ] **P7b-5** Handle complex bindings as "legacy"
  - **Complex bindings** that can't be visualized:
    * Multiple-hop bindings (relay chains)
    * Property overrides (Type P)
    * Custom protocols not in standard types
  - Store as "legacy binding" objects: `{type: 'legacy', rawString: '...', description: '...'}`
  - Show in separate "Legacy Bindings" section in binding panel
  - Display raw binding string with explanation
  - Mark as "not visually editable" - can view/delete but not edit graphically
  - User can delete legacy binding and re-create visually
  
- [ ] **P7b-6** Import UI flow
  - "Import from hardware" button in header dropdown
  - Modal: "This will download existing bindings and create devices. Continue?"
  - Progress indicator during download and parsing
  - Summary dialog:
    * X bindings imported
    * Y devices created in Home View
    * Z legacy bindings (view details)
  - Option to review legacy bindings before accepting
  - Option to cancel if unexpected results
  
- [ ] **P7b-7** Conflict resolution
  - If project already has devices/bindings:
    * Ask user: "Replace all" / "Merge" / "Cancel"
    * Replace: clear existing, import fresh
    * Merge: keep existing, add new (may create duplicates)
  - Handle node address conflicts (same address, different module type)
  - Handle binding conflicts (same input/output, different parameters)
  
- [ ] **P7b-8** Validation and warnings
  - Verify all node addresses found in Rail View
  - Warn about bindings to unknown nodes: "Binding references node 0x05, but no module with this address exists"
  - Warn about bindings to unknown unit types: "Cannot determine unit type for node 0x0A unit 3"
  - Offer to create "Unknown" module placeholders for missing nodes
  - Allow user to assign correct module types before finalizing import

### Phase 7c — Simple binding import (MVP - No logic blocks, no timers) **🚀 READY TO START**

**Goal:** Import simple bindings from bind*.txt files on disk (Type I/N only)

**Why start here:**
- ✅ No TCP download protocol needed (works offline)
- ✅ `BindingFileParser` already exists and works
- ✅ Most projects have bind*.txt files saved locally
- ✅ Can deliver value immediately without waiting for P3-8/P3-10 (logic blocks/timers)
- ✅ Foundation for full import once visual elements are ready

**Scope limitations (MVP):**
- ✅ Type I (Immediate) and Type N (Normal) bindings only
- ❌ No Type C (Conditional/logic blocks) - requires P3-8 first
- ❌ No Type Td/Tf/Tr/Ti/To (Timers) - requires P3-10 first
- ❌ No Type G (Group/Moods) - requires P4 first
- ❌ No Type P (Property) - deferred
- ⏳ Complex bindings → marked as "legacy" (stored but not visualized)

**Implementation plan:**

- [ ] **P7c-1** File picker UI for Config folder
  - "Import bindings" button in Project dropdown menu
  - File picker: `<input type="file" webkitdirectory multiple>` to select Config folder
  - Display selected folder path in modal
  - List found bind*.txt files with node addresses
  - Continue button to proceed with import

- [ ] **P7c-2** Backend API for parsing binding files
  - `POST /api/bindings/import` endpoint
  - Accept array of `{fileName, content}` objects
  - Use existing `BindingFileParser` to parse each file
  - Return structured data: `{bindings: [...], errors: [...]}`
  - Group bindings by type: `{simple: [...], conditional: [...], timer: [...], legacy: [...]}`

- [ ] **P7c-3** Match bindings to existing Rail View devices
  - For each binding entry, extract node addresses and unit addresses
  - Look up node address in `project.cabinets[].modules[]` and `project.woningDevices[]`
  - Match unit addresses to channel indexes (relay 1 = unit 1, dimmer 2 = unit 2, etc.)
  - Create mapping: `{nodeAddress, unitAddress} → {cabinetId?, moduleInstanceId?, channelIndex}`
  - Warn about unmatched node addresses

- [ ] **P7c-4** Create Home View devices for bound units (Type I/N only)
  - For each simple binding (Type I/N):
    * Extract input unit (controller) and output unit (controllable)
    * Check if units already exist as Home View devices
    * If not, create new devices in default "Imported" room
    * Set device.channelRef to link back to Rail View unit
  - Name devices based on unit type: "Lamp 1", "Schakelaar 2", "Dimmer 3"
  - Store original binding entry in device metadata for later visualization

- [ ] **P7c-5** Convert simple bindings to visual wires
  - For each Type I/N binding:
    * Map input event to port (E01 "Kort" → portId: "kort")
    * Map output action to port (F10 "Toggle" → portId: "schakel")
    * Create binding object: `{id, from: {deviceId, portId}, to: {deviceId, portId}, color}`
    * Add to `project.bindings[]`
  - Result: Visual wires appear in binding panel for imported bindings

- [ ] **P7c-6** Store complex bindings as "legacy" (no visualization)
  - Types C/Td/Tf/Tr/Ti/To/G/P → stored as legacy
  - Create legacy binding objects: `{type: 'legacy', subtype: 'C'|'Td'|..., rawString: '...', description: 'Conditional binding: ...'}`
  - Add to `project.legacyBindings[]` array
  - Display count in import summary: "14 simple bindings imported, 3 legacy bindings stored"

- [ ] **P7c-7** Import summary dialog
  - Show results after import:
    * "✅ 14 simple bindings imported and visualized"
    * "⏳ 3 conditional bindings stored as legacy (requires logic blocks)"
    * "⏳ 2 timer bindings stored as legacy (requires timer blocks)"
    * "⚠️ 1 binding to unknown node 0x05 (not in Rail View)"
  - Option to review legacy bindings
  - Option to create missing devices as "Unknown" placeholders
  - Accept/Cancel buttons

- [ ] **P7c-8** Legacy bindings view (in binding panel)
  - When viewing a device with legacy bindings, show separate section:
    * "Legacy Bindings (3)" collapsible section
    * Per legacy binding: raw string, parsed description, "Delete" button
    * Tooltip: "This binding uses features not yet supported in visual editor"
  - User can delete legacy binding and recreate visually

**Value proposition:**
- Installer can import 80% of bindings immediately (most are simple Type I/N)
- Visualizes existing work in new UI without manual recreation
- Provides clear path forward: "3 bindings need logic blocks - coming soon"
- Foundation for full import once P3-8/P3-10 are complete

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

*(See Phase 7b for full import plan, Phase 7c for MVP simple import)*

| Binding type | Wiring diagram element | Import status | Phase |
|-------------|----------------------|---------------|-------|
| `I` Immediate | Simple wire | ✅ **Ready NOW** | **P7c (MVP)** |
| `N` Normal | Simple wire | ✅ **Ready NOW** | **P7c (MVP)** |
| `C` Conditional | Wire + NOT/AND/OR/XOR logic block | ⏳ TODO (needs P3-8) | P7b-4 |
| `Td` Timer Delayed | Wire + timer block | ⏳ TODO (needs P3-10) | P7b-4 |
| `Tf` Timer Flashing | Wire + timer block | ⏳ TODO (needs P3-10) | P7b-4 |
| `Tr` Timer Repeat | Wire + timer block | ⏳ TODO (needs P3-10) | P7b-4 |
| `Ti` Timer Interval | Wire + timer block | ⏳ TODO (needs P3-10) | P7b-4 |
| `To` Timer Oneshot | Wire + timer block | ⏳ TODO (needs P3-10) | P7b-4 |
| `G` Group | Mood (separate Mood API, not wiring diagram) | ⏳ TODO (needs P4) | P7b-4 |
| `P` Property | Property override — dedicated UI element (deferred) | ⏭️ Later | — |
| *Complex* | Legacy binding (view/delete only) | ✅ Ready NOW | **P7c-6** |

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

| Stub | Phase | Status |
|------|-------|--------|
| `+ Kast toevoegen` | P1-3 | ✅ DONE |
| `+ Rail toevoegen` | P1-3 | ✅ DONE |
| `+ Schakelaar` / `+ LCD` | P1-4 | ✅ DONE |
| `+ Kamer toevoegen` | P2-2 | ✅ DONE |
| `selectRoom()` | P2-3 | ⏭️ Skipped (no zoom needed) |
| `addDeviceToRoom()` | P2-4 | ✅ DONE (basic) |
| `openModuleDetail()` | P6-1 | ✅ DONE |
| Binding wizard (`openBW` / `saveBW`) | P3 | ✅ DONE (replaced with visual wiring) |
| Save button | P7-1 | ✅ DONE |
| Upload button | P7-4 | ⏳ TODO |
| Export Excel / PDF | P9-2, P9-3 | ⏳ TODO |
| Connection status indicator | P8-2 | ✅ DONE |

---

## Open questions *(add new ones here; none blocking Phase 0)*

- **Q-1** ~~Reconstruct wiring graph from `bind*.txt`~~ → **RESOLVED**: See Phase 7b for detailed import plan. Simple bindings (Type I/N) convert to visual wires, conditional (Type C) to logic blocks, timers (Td/Tf/Tr/Ti/To) to timer blocks. Complex bindings stored as "legacy bindings" (view/delete but not visually editable).
- **Q-2** Auth API uses HTTPS with device self-signed certs. Node.js `https` client will likely need `rejectUnauthorized: false`. Verify during P4-1.
- **Q-3** Moods live on LCD/Touchscreen nodes (virtual units). If no LCD is configured yet, can moods live on the master node? Clarify during P4 implementation.
- **Q-4** Scheduling API: spec uses `/schedulescfg/tag` (extra 's') in one place and `/schedulecfg/tag` elsewhere. Confirm correct URL against lab during P5.

---

## Architecture discussions *(open items for team review)*

### A-1: Client vs Server logic split

**Current state**: `unit-helpers.js` (client-side JavaScript) generates units from module channelGroups and tracks usage.

**Concern**: Where should business logic live?
- **Client (*.js, *.html)**: UI rendering, user interactions, visual state
- **Server (TypeScript)**: Data transformations, project validation, business rules

**Specific questions**:
1. Should unit generation (`getAllUnitsWithUsage`) be a server API endpoint?
   - **Pros**: Centralized logic, testable, reusable for future tools
   - **Cons**: Extra HTTP round-trip, server must cache project state
   - **Decision**: Keep in client for now (performance), revisit if logic becomes complex

2. Does the server currently have all needed state?
   - Server has: project file structure, module database, binding strings
   - Server may NOT have: unit usage tracking, live discovery results, UI selections
   - **TODO**: Audit server state completeness — add section below if gaps found

3. What belongs on server vs client?
   - **Clear server**: Project CRUD, file parsing, TCP/UDP communication, binding serialization
   - **Clear client**: DOM manipulation, drag-and-drop, modal dialogs, animations
   - **Gray area**: Unit generation, usage tracking, binding validation, name sync
   - **Guideline**: Move to server when:
     * Logic needed by multiple clients (mobile app, CLI tool)
     * Data transformation is complex/testable
     * Business rules enforce consistency

**Action items**:
- [ ] Create `A-2` section: "Server state completeness audit" — list what server has/needs
- [ ] Add server endpoints for unit management if needed (e.g., `GET /api/units`, `POST /api/units/:id/assign`)
- [ ] Consider TypeScript types for unit-helpers functions (gradual migration)

### A-2: Server state completeness audit

**Question**: Does the server maintain all needed project state for UI logic to work?

**Known server state** (from `src/models/project.ts`):
- ✅ Project metadata (name, master IP, password)
- ✅ Rail View: cabinets, modules, woning devices with positions
- ✅ Home View: floors, rooms, devices with channelRefs
- ✅ Bindings: structured JSON format
- ✅ Module database: channelGroups, specs, pricing

**Potentially missing**:
- ❓ Unit usage tracking (which units assigned to which rooms)
  - Currently computed client-side in `analyzeUnitUsage()`
  - Should this be cached server-side for performance?
- ❓ Live discovery results (discovered nodes, unit capabilities)
  - `MasterConnectionService` has this, but is it persisted?
  - Should discovery results be stored in `.duo` file?
- ❓ Module → unit label mappings
  - Default labels ("Dimmer 1", "Relais 2") generated client-side
  - Custom labels (renamed by user) — where do these live?
  - Should unit labels be part of ModuleInstance in project file?
- ❓ Device → unit bidirectional link
  - Room devices have `channelRef: {nodeAddress, unitAddress, moduleInstanceId}`
  - Do units also have `room: {floorId, roomId}` back-reference?
  - If not, usage must be computed every time → performance concern

**Recommendation**:
- Add `units[]` array to `ModuleInstance` in project file schema
- Each unit has: `{unitAddress, label, channelType, room?: {floorId, roomId}}`
- Server `GET /api/project/units` endpoint returns all units with usage
- Client calls this for unit picker, avoids recomputing usage

**Follow-up**: Discuss with team, decide on server state design before P2-4a merge

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
