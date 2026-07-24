/**
 * .duo project file — TypeScript types
 *
 * The project file stores all design-time configuration for a Duotecno installation.
 * It is independent of the hardware connection and can be edited offline.
 *
 * At upload time:
 *   bindings[]  →  serialised to bind*.txt strings → uploaded via TCP to nodes
 *   moods       →  uploaded via HTTP to LCD nodes
 *   schedules   →  uploaded via HTTP to LCD nodes
 */

// ─── Identifiers ─────────────────────────────────────────────────────────────

/** Short random string, project-local only. Not related to node/unit addresses. */
export type LocalId = string;

// ─── Meta ─────────────────────────────────────────────────────────────────────

export interface ProjectMeta {
  name: string;
  created: string;   // ISO 8601
  modified: string;  // ISO 8601
  version: '1';
  masterIp?: string;       // last used master IP address
  masterPassword?: string; // last used master password (stored for installer convenience)
  masterPort?: number;     // TCP binding-protocol port, default 5001
  masterApiPort?: number;  // HTTPS auth-API port for moods/scheduling, default 8081
  /**
   * Auth client credentials for the HTTPS :masterApiPort auth API (see
   * `MoodHttpService`). Stored so the app reuses the same client + refresh
   * token across sessions instead of creating a new client every time
   * (the server only allows a limited number of clients).
   */
  moodsApiClientId?: string;
  moodsApiRefreshToken?: string;
}

// ─── Nodes — single source of truth for hardware ──────────────────────────────

/**
 * Temporary node addresses (for modules/house devices added before hardware
 * exists / before discovery) start here. Real hardware addresses are always
 * <= 255 (1 byte on the CAN bus), so this range can never collide.
 */
export const TEMP_NODE_ADDRESS_START = 1024;

/** A single channel on a node. */
export interface NodeUnit {
  unitAddress: number;
  type: number;    // UnitType enum value (see models/unitTypes.ts)
  name?: string;   // discovered or user-given name
}

/**
 * A physical (or planned) hardware node — module, field device, or master.
 * Keyed by `nodeAddress`, which is the single primary key used everywhere
 * else in the project (cabinets, house, channelRef, bindings).
 *
 * Discovery is the source of truth for `units`/`name`/`type`/`physicalAddress`:
 * on connect, nodes are upserted here by `nodeAddress` (added if new, blank
 * fields filled in — never overwriting a user-given name).
 */
export interface DuoNode {
  nodeAddress: number;        // real (<=255) or temp planning address (>= TEMP_NODE_ADDRESS_START)
  physicalAddress?: number;   // physical hardware address, filled in by discovery
  model?: string;             // product code, once known
  name?: string;
  type?: number;               // NodeType enum value
  units: NodeUnit[];
}

// ─── Rail View ─────────────────────────────────────────────────────────────────
// Cabinets/house are thin position lists — all node/unit data lives in `nodes[]`.

/** A module placed on a DIN rail — just a position + reference to its node. */
export interface RailModuleRef {
  nodeAddress: number;         // key into DuoProject.nodes
  position: number;            // slot index within the rail (0-indexed)
  /**
   * Smartbox slot configuration — only present when the node's model is a
   * Smartbox base. Each entry is a plugin model code or null (empty slot).
   */
  slots?: (string | null)[];
}

/** Physical cabinet. Modules are stored flat; rails are computed from widthUnits at render time. */
export interface Cabinet {
  id: LocalId;
  name: string;               // e.g. "Hoofdkast"
  location?: string;          // free text, e.g. "Technische ruimte"
  widthUnits: number;         // cabinet width in DIN M-units (1M ≈ 18mm), default 12
  modules: RailModuleRef[];   // ordered flat list — reflowed into rails at render time
}

/** House/field device on CAN Segment 2 (switches, LCD panels, sensors) — position only. */
export interface HouseDeviceRef {
  nodeAddress: number;        // key into DuoProject.nodes
  position: number;           // order in house panel (left → right)
}

export interface RailView {
  cabinets: Cabinet[];
  house: HouseDeviceRef[];
}

// ─── Channel reference ────────────────────────────────────────────────────────

/** Points to a specific channel on a specific hardware node. */
export interface ChannelRef {
  nodeAddress: number;        // key into DuoProject.nodes
  unitAddress: number;        // unit/channel address on that node
}

// ─── Home View ────────────────────────────────────────────────────────────────

/** Functional type of a device in the home view. */
export type DeviceType =
  | 'dimmer'        // dimmer output
  | 'relay'         // relay output (on/off)
  | 'motor'         // up/down motor output
  | 'input'         // button/switch input
  | 'sensor'        // sensor (PIR, temp, ...)
  | 'audio'         // audio zone
  | 'mood'          // virtual mood/scene channel
  | 'dali_group';   // DALI group

/**
 * One button/sensor on a multi-unit room device (e.g. an 8-button DTBS switch).
 * Only what can't be derived from `nodes[]` is stored: the unit address and an
 * optional user label. Icon/channelType/etc. are looked up live via `nodeAddress`
 * (inherited from the parent RoomDevice) + `unitAddress`.
 */
export interface RoomDeviceUnit {
  unitAddress: number;
  label?: string;    // user override; falls back to the node unit's own name
}

/**
 * A logical device placed in a room — linked to a physical channel.
 * Single-unit devices (lamp, relay, motor, sensor) use `channelRef`.
 * Multi-unit devices (a switch with several buttons ± a temp sensor) use
 * `buttons`/`sensors` instead — each entry shares this device's `nodeAddress`.
 */
export interface RoomDevice {
  id: LocalId;
  type: DeviceType;
  name: string;
  channelRef?: ChannelRef;    // set once assigned to a physical channel

  /** Set when this device represents multiple units on one node (e.g. a button switch). */
  isMultiUnit?: boolean;
  nodeAddress?: number;       // shared node for buttons/sensors below (multi-unit only)
  buttons?: RoomDeviceUnit[];
  sensors?: RoomDeviceUnit[];
  activeButton?: number;      // index into buttons[] currently shown in the binding panel
  activeSensor?: boolean;     // true = showing sensors[0] instead of buttons[activeButton]

  canvasX?: number;           // position within room canvas (optional, for future visual placement)
  canvasY?: number;
}

/** A room on a floor. Shown as a draggable rectangle on the Home View canvas. */
export interface Room {
  id: LocalId;
  name: string;
  icon?: string;              // emoji icon shown in sidebar
  floorId: LocalId;
  canvasX: number;
  canvasY: number;
  canvasWidth: number;
  canvasHeight: number;
  floorplanImage?: string;    // data URL or path to uploaded floor plan image
  devices: RoomDevice[];
}

/** A floor (verdieping) grouping rooms. */
export interface Floor {
  id: LocalId;
  name: string;
  label?: string;
}

export interface HomeView {
  floors: Floor[];
  rooms: Room[];
}

// ─── Bindings (structured, serialised to bind*.txt at upload) ─────────────────

/** A logic block inserted inline on a wire. Maps to `C` (Conditional) binding type. */
export interface LogicBlock {
  type: 'NOT' | 'AND' | 'OR';
}

/** A timer block inserted inline on a wire. Maps to Td/Tf/Tr/Ti/To binding types. */
export interface TimerBlock {
  type: 'delayed' | 'flashing' | 'repeat' | 'oneshot' | 'interval';
  durationMs: number;
}

/**
 * One end of a binding wire. `channelRef` is the source of truth (nodeAddress +
 * unitAddress + port); `deviceId` is an optional cache of which RoomDevice.id
 * currently represents that channel, kept only for fast UI lookups.
 */
export interface BindingEndpoint {
  channelRef: ChannelRef;
  portId: string;             // e.g. "kort", "lang", "schakel", "op"
  deviceId?: LocalId;         // RoomDevice.id, if placed in a room
}

/**
 * One wire in the wiring diagram.
 * Connects an output port on a source device to an input port on a target device.
 * Serialised to one or more binding strings at upload time.
 */
export interface BindingWire {
  id: LocalId;
  from: BindingEndpoint;
  to: BindingEndpoint;
  color?: string;
  logicBlock?: LogicBlock;
  timerBlock?: TimerBlock;
  /** Raw binding strings for wires that couldn't be reconstructed visually (legacy import). */
  legacyRaw?: string[];
  imported?: boolean;
  sourceFile?: string;
  bindingNumber?: number;
}

/**
 * A complex legacy binding (Conditional/Group/Property/Timer types) that we
 * can parse enough to categorize, but not yet safely reconstruct/re-upload
 * (the C/G/P/Timer wire formats — and their function-code data payloads —
 * aren't fully mapped out yet, see TODO.md "Writing bindings to hardware").
 * Kept verbatim (byte-for-byte) so nothing is lost on import; NOT rendered
 * as a wire in the binding panel and NOT touched by any upload path.
 */
export interface LegacyBinding {
  nodeAddress: number;
  bindingNumber: number;
  bindingType: string;    // 'C' | 'G' | 'P' | 'Te' | 'Ti' | 'To' | 'Td' | 'Tf' | 'Tr'
  raw: string;             // exact original line from bind*.txt
  sourceFile?: string;
}

// ─── Moods (virtual scene channels) ────────────────────────────────────────────

/**
 * One step of a mood: send `msg` to a channel, optionally after waiting `delay`
 * seconds. `msg` is the hex-encoded action (see `mood-types.js`/`toMsg()`),
 * mirroring the format used by the Master HTTP API (`/moodscfg/`,
 * `/config/download|upload/moods.json`) and legacy `MoodConfig_XX.json` files.
 */
export interface MoodAction {
  channelRef: ChannelRef;
  msg: string;
  delay?: number;
}

/**
 * A mood/scene definition — the content of one Type-7 "mood" channel.
 * `channelRef` points at the mood's own virtual channel (its existence as a
 * device is derived live from `nodes[]`; this is where its *actions* live).
 */
export interface MoodDefinition {
  channelRef: ChannelRef;
  name: string;
  description?: string;
  userAdjustable: boolean;   // whether the end-user's own App may adjust this mood
  actions: MoodAction[];
}

// ─── Top-level project ────────────────────────────────────────────────────────

export interface DuoProject {
  meta: ProjectMeta;
  /** Single source of truth for all hardware — nodes and their units. */
  nodes: DuoNode[];
  railView: RailView;
  homeView: HomeView;
  /** All wiring connections. Each wire → one or more binding strings at upload. */
  bindings: BindingWire[];
  /** Complex legacy bindings preserved verbatim on import — see LegacyBinding. */
  legacyBindings?: LegacyBinding[];
  /** Mood/scene action lists, keyed by the mood's own channelRef. */
  moods?: MoodDefinition[];
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

export function createEmptyProject(name: string): DuoProject {
  const now = new Date().toISOString();
  return {
    meta: { name, created: now, modified: now, version: '1' },
    nodes: [],
    railView: { cabinets: [{ id: makeId(), name: 'Hoofdkast', widthUnits: 36, modules: [] }], house: [] },
    homeView: {
      floors: [{ id: 'floor-0', name: 'Gelijkvloers' }],
      rooms: [],
    },
    bindings: [],
  };
}

/** Allocate the next temporary node address (>= TEMP_NODE_ADDRESS_START) not already used in `nodes`. */
export function allocateTempNodeAddress(nodes: DuoNode[]): number {
  const used = new Set(nodes.map(n => n.nodeAddress));
  let addr = TEMP_NODE_ADDRESS_START;
  while (used.has(addr)) addr++;
  return addr;
}

export function makeId(): LocalId {
  return Math.random().toString(36).slice(2, 10);
}
