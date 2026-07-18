# Duotecno Configuration Software – UI Specification

> Status: draft – living document, freely extendable

---

## Current Status (2026-07-08)

### ✅ Completed

**Rail View – core**
- Cabinet cards with configurable width (12/18/24/36/54/82M), add/rename/delete
- Rails auto-calculated from cabinet width; modules auto-reflowed across rails
- Module cards: product image, model name, DIN-unit count, node address badge
- CAN bus snake (orange) connects rails with terminator blocks (60Ω) at each end
- Add / change / delete / reorder modules per rail
- Module picker: searchable, filterable by category, shows product images

**Woning panel**
- Field devices (schakelaars + LCD panels) on their own CAN panel below the cabinet
- Product images loaded from module DB; UNKNOWN cards with amber pulse animation
- Add switcher / LCD buttons; multi-row snake layout for many devices
- Devices + add-buttons vertically aligned to card center on the CAN line

**Discovery (TCP connect)**
- Connect modal: "Verbinden" (quick 3 polls) + "🔍 Scan alles" (full 20 polls)
- Discovered nodes matched to existing project modules / woning devices
- Unmatched nodes auto-added as UNKNOWN cards (cabinet or woning)
- Classification uses **node.type** first: type 32 (kGUINode) → woning; type 8 (kModemNode) → cabinet; others use unit-type heuristic (any DIM/SWITCH/MOTOR/AUDIO → cabinet)
- UNKNOWN module modal: hardware info, unit type breakdown, auto-suggestions
- UNKNOWN woning modal: auto-suggestions, "🗄 Naar kast" escape button
- When assigning a woning-type model (switch/lcd) to a cabinet UNKNOWN → auto-moves to woning panel
- Node address shown as `0xFC` (hex), node type shown as `0x20 (GUI (LCD))`
- Badges: ✓ (matched), ! (UNKNOWN), ? (not yet scanned)

**Project file**
- Save / load `.duo` JSON project files
- Project name in header; modified indicator

**Module DB**
- 115 modules, 17 families + 98 standalone entries
- scraped from duotecno.com with images, DIN widths, wattages
- `reader/modules/_index.json`

### ❌ Pending

| ID | Feature | Notes |
|----|---------|-------|
| P1-5 | Resources sidebar | Sidebar panel listing all modules/families |
| P2 | Home View | Rooms, floors, floor-plan, device placement |
| P9 | Materiaallijst | Auto-generated BOM from Rail View |
| P8-5 | Rename node via TCP | Write new name back to hardware |
| P8-6 | Node-type → model auto-lookup | Engineer-provided table (firmware name → model) |
| P10 | Upload naar hardware | Generate & upload binding files via TCP |
| P11 | Undo / redo | Command stack (Ctrl+Z / Ctrl+Y) |
| — | Ethernet port counting | Switch sizing from Rail View |
| — | Multi-cabinet support | Multiple kast cards per project |
| — | PDF/Excel export | Rail View + Materiaallijst print-ready |

---

## Overview

The new Configuration Software has **three views** that together cover the full project workflow:

| View | Purpose |
|------|---------|
| **Rail View** | Physical hardware layout – what goes in the cabinet + woning devices |
| **Home View** | Logical/functional layout – what does what, where |
| **Materiaallijst** | Bill of materials, auto-generated from Rail View |

All views share the same underlying project data model.

---

## 1. Rail View

### 1.1 Concept

Mimics the way a DIN-rail cabinet is physically organised. The installer sees exactly what is mounted in the cabinet, on which rail, in which order. Below the cabinet a **Woning** panel shows the field devices (switches, LCD) on CAN Segment 2.

### 1.2 CAN Bus Topology

```
60Ω ── [Module …] ── [Module …] ──┐   (Rail 1, Segment 1, left→right)
                                   │ vertical snake
60Ω ── [PSU+CAN] ── [Module …] ──┘   (Rail 2, Segment 1, right→left)
          │
          │ Segment 2 drop
          ▼
  ┌────────────────────────────────────────────┐
  │ 60Ω ── [SW] ── [SW] ── [SW] ── [LCD] ── 60Ω │  (Woning, Segment 2)
  └────────────────────────────────────────────┘
```

- Each **rail** is part of **CAN Segment 1**.
- CAN runs left→right on odd rails, then snakes down and right→left on the next rail.
- The **PSU + 2-channel CAN Switch** (DT-PSU-CAN) terminates Segment 1 and starts **Segment 2**.
- Every rail starts with a **60 Ω termination resistor**; Rail 1 also ends on its own left terminator (segment start).
- **Segment 2** runs through all woning field devices (switches, LCD panels), terminated at both ends with 60 Ω.
- Orange dots mark each module's CAN connection on Segment 1; blue dots on Segment 2.

### 1.3 Visual Module Design

Modules are rendered as realistic DIN-rail cards:
- **Top & bottom grey terminal rows** (screw terminals, width-proportional)
- **Black face plate** with model name, type label, channel colour dots, CAN port indicators, address badge
- **Uniform height**; width proportional to DIN-unit count (full / half for DALI)
- CAN connection dots (left & right) sit on the bus line — orange for Seg 1, blue for Seg 2

### 1.4 Module Categories & Types

| Category | Model | Description | Width | I/O | W |
|----------|-------|-------------|-------|-----|---|
| Dimmer | DT05-06TE | Trailing Edge 6×30 W | full | 6 dim | 18 |
| Dimmer | DT06-02 | Leading Edge 8×130 W | full | 8 dim | 22 |
| Relay | DT02-08 | 8 relay outputs | full | 8 relay | 15 |
| Input | DT03-16 | 16 digital inputs | full | 16 in | 7 |
| Motor | DT04-04 | 4× up/down motor | full | 4 motor | 18 |
| DALI | DT13-DAL | DALI bus interface | **half** | 64 DALI | 8 |
| PSU+CAN | DT-PSU-CAN | Power supply + 2-ch CAN switch | full | – | – |

*Woning field devices (Segment 2):*

| Type | Model | Description |
|------|-------|-------------|
| Schakelaar 4-gang | DT-SW-04 | Aluminium 4-button switch with centre display (temp, icons) |
| Schakelaar 2-gang | DT-SW-02 | Aluminium 2-button switch |
| LCD Touch Panel | DT-LCD-01 | Large touch display: clock, room name, temperature, icon bar |

### 1.5 Automatic Calculations

- **Total power consumption** of all modules → number of PSUs needed (1 PSU = 60 W).
- The PSU+CAN Switch counts as 1 PSU; the summary shows how many extra PSUs are required.
- Shown in a banner at the top of each cabinet card.

### 1.6 Interactions

| Action | How |
|--------|-----|
| Add module | Click **+** at end of rail → category → model → confirm |
| Add rail | Button below cabinet |
| Edit module | Click module → toast / sidebar |
| Add cabinet | Sidebar → + Kast toevoegen |

---

## 2. Home View

### 2.1 Concept

A functional / spatial view of the installation. Rooms and floors give the installer a navigational structure. Devices are placed on an optional floor-plan background image. Bindings link inputs to outputs.

### 2.2 Structure

```
Project
 └── Floor 1
 │    └── Room A  (e.g. Living room)
 │    └── Room B  (e.g. Kitchen)
 └── Floor 2
      └── Room C  (e.g. Master bedroom)
```

- Floors and rooms are added manually (floor plan import is optional).
- Without a floor plan, rooms are shown as draggable labelled rectangles.

### 2.3 Device Types

| Type | Binds to |
|------|----------|
| Lamp / Dimmer | Dimmer or Relay output |
| Roller blind / Motor | Motor output (up/down) |
| Switch / Button | Any input channel |
| Sensor | Sensor input |
| Audio zone | Audio output |
| DALI group | DALI group |

### 2.4 Bindings

- Rendered as dashed curved lines between device icons.
- Click to see/edit parameters (dim level, long-press action, …).
- Binding types correspond to the existing binding file format (see `specs/binding-files-format.md`).

### 2.5 Interactions

| Action | How |
|--------|-----|
| Add floor | Sidebar → **+ Verdieping** |
| Add room | Select floor → **+ Kamer** |
| Add device | Drag from palette onto canvas |
| Create binding | Click source → drag to target |
| Import floor plan | Room settings → upload image |

---

## 3. Materiaallijst (Bill of Materials)

Auto-generated from the Rail View. Contains all modules and field devices with:

| Column | Description |
|--------|-------------|
| Model | Hardware model code |
| Omschrijving | Description |
| Type | Category badge |
| DIN | DIN-unit width (cabinet modules only) |
| W/st. | Power consumption per unit |
| Qty | Count |
| Totaal W | Total watts |
| CAN Segment | Seg 1 / Seg 2 / Seg 1+2 |
| Opmerking | Notes |

**Footer row** shows: total wattage, PSUs needed (⌈W/60⌉), and how many extra PSUs beyond the built-in PSU+CAN Switch.

Export: Excel, PDF (planned).

---

## 4. Shared Concerns

### 4.1 Project File

- Single file (`.duo` / JSON) contains rail + home configuration.
- Compatible with existing bind-file export for hardware upload.

### 4.2 Navigation

- Three top-level tabs: **Rail View · Home View · Materiaallijst**
- Project name and save status always visible in the header.

### 4.3 Undo / Redo

- All mutations go through a command stack (Ctrl+Z / Ctrl+Y).

---

## 5. Open Questions / Future Work

- [ ] Multi-cabinet projects
- [ ] Automatic CAN address assignment
- [ ] Firmware version tracking per module
- [ ] PDF / print export of Rail View (for electrician)
- [ ] Scenario / mood programming in Home View
- [ ] Schedule editor integration
- [ ] Remote / cloud sync of project files
- [ ] Drag-and-drop reorder of modules within a rail
- [ ] Ethernet port counting / switch sizing


---

## 1. Rail View

### 1.1 Concept

Mimics the way a DIN-rail cabinet is physically organised. The installer sees exactly what is mounted in the cabinet, on which rail, in which order.

### 1.2 Structure

```
Cabinet
 └── Rail 1 ─── [60Ω] ── [Module A] ── [Module B] ── … ── [60Ω]
 └── Rail 2 ─── [60Ω] ── [Module C] ── … ── [60Ω]
 └── …
```

- Each **rail** is a CAN-bus segment.
- Every rail starts and ends with a **60 Ω termination resistor** (rendered visually).
- Modules sit between the terminators in insertion order.
- Multiple rails can exist in one cabinet (stacked DIN rails).

### 1.3 Module Properties

Each module has at minimum:

| Property | Type | Description |
|----------|------|-------------|
| `type` | string | Hardware model identifier (e.g. `DT05-06TE`) |
| `label` | string | User-given name |
| `widthUnits` | number | DIN-unit width (e.g. 4, 6, 8 …) |
| `powerConsumption` | number | Watts consumed from the PSU rail |
| `needsEthernet` | boolean | Whether an Ethernet connection is required |
| `canAddress` | number | Node address on the CAN bus |
| `functions` | Function[] | List of I/O functions exposed by the module |

### 1.4 Module Categories & Types (initial list, extendable)

| Category | Model | Description | Width (DIN) | I/O |
|----------|-------|-------------|-------------|-----|
| Dimmer | DT05-06TE | Trailing Edge 6×30 W | 6 | 6 dimmer outputs |
| Dimmer | DT06-02 | Leading Edge 8×130 W | 8 | 8 dimmer outputs |
| Relay | DT02-08 | 8 relay outputs | 8 | 8 relay outputs |
| Input | DT03-xx | Input interface | 4 | 8/16 digital inputs |
| Motor | DT04-xx | Motor/blind controller | 6 | 4 × up/down |
| Audio | DT08-xx | Audio zone controller | 6 | multi-zone audio |
| DALI | DT13-DAL | DALI bus interface | 4 | 64 DALI devices |
| Switch | DT07-xx | Touch switch / display | 2 | local UI |
| Sensor | DT02-08 | Analog/digital sensor | 4 | 8 sensor inputs |

*Module list is data-driven; new modules can be added without code changes.*

### 1.5 Automatic Calculations (planned)

- **Total cabinet width** used vs. available.
- **Total power consumption** per PSU group → whether an extra PSU is needed.
- **Ethernet ports** needed → switch sizing.
- **CAN bus segment warnings** (too many nodes, missing terminator, …).

### 1.6 Interactions

| Action | How |
|--------|-----|
| Add module | Click **+** → pick category → pick model → confirm label & address |
| Reorder modules | Drag & drop within rail |
| Move to other rail | Drag to another rail |
| Delete module | Context menu or delete key |
| Add rail | Button at bottom of cabinet |
| Edit module properties | Click module → sidebar panel |

---

## 2. Home View

### 2.1 Concept

A functional / spatial view of the installation. Rooms and floors give the installer a navigational structure. Devices are placed on a (optional) floor-plan background image. Bindings link inputs to outputs.

### 2.2 Structure

```
Project
 └── Floor 1
 │    └── Room A  (e.g. Living room)
 │    └── Room B  (e.g. Kitchen)
 └── Floor 2
      └── Room C  (e.g. Master bedroom)
```

- Floors and rooms are added manually (floor plan import is optional).
- If no floor plan image is available, rooms are shown as labelled rectangles that the installer can resize and position freely.

### 2.3 Device Types placeable in rooms

| Type | Icon | Binds to |
|------|------|----------|
| Lamp / Dimmer | Lamp symbol | Dimmer or Relay output |
| Roller blind / Motor | Blind symbol | Motor output (up/down) |
| Switch / Button | Square | Any input channel |
| Sensor | Circle | Sensor input |
| Audio zone | Speaker | Audio output |
| DALI group | Bulb cluster | DALI group |

### 2.4 Bindings

A **binding** is a directional (or bidirectional) link between one or more inputs and one or more outputs.

- Rendered as a line between device icons.
- Click a binding to see/edit its parameters (e.g. dim level on button press, long-press action, …).
- Binding types correspond to the existing binding file format (see `specs/binding-files-format.md`).

### 2.5 Interactions

| Action | How |
|--------|-----|
| Add floor | Sidebar → **+ Add floor** |
| Add room | Select floor → **+ Add room** |
| Add device | Drag from device palette onto floor plan |
| Create binding | Click source device → drag to target device |
| Edit binding | Click the binding line → parameter panel |
| Import floor plan image | Room settings → upload background image |
| Select room | Click room in sidebar or directly on canvas |

### 2.6 Filter tabs (top of canvas)

Tabs to toggle visibility per function type: **Dimmer · Relay · Input · Switch · Motor · Audio**

---

## 3. Shared / Cross-cutting Concerns

### 3.1 Project File

- Single project file (`.duo` or JSON) contains both rail configuration and home configuration.
- Compatible with existing bind-file export for upload to hardware (see `BINDING-FILES.md`).

### 3.2 Upload to Hardware

- From either view, the **Upload** button serialises the project to the hardware binary binding format and transfers it via the existing communication stack.
- Progress and result are shown in an overlay (as in the current viewer).

### 3.3 Navigation

- Top-level tab or split-screen switch between **Rail View** and **Home View**.
- Project name and save status shown in the header at all times.

### 3.4 Undo / Redo

- All mutations go through a command stack.
- Ctrl+Z / Ctrl+Y for undo/redo.

---

## 4. Open Questions / Future Work

- [ ] Multi-cabinet projects (large installations with multiple physical cabinets)
- [ ] Automatic address assignment on CAN bus
- [ ] Firmware version tracking per module
- [ ] PDF / print export of Rail View (for electrician)
- [ ] Scenario / mood programming in Home View
- [ ] Schedule editor integration
- [ ] Remote / cloud sync of project files
