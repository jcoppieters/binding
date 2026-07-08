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
}

// ─── Rail View ───────────────────────────────────────────────────────────────

/**
 * A single hardware module placed on a DIN rail.
 * `model` references the product database (reader/modules/).
 */
export interface ModuleInstance {
  id: LocalId;
  model: string;              // e.g. "DT05-06TE" or 'UNKNOWN'
  nodeAddress?: number;       // IP node address (hex), assigned after discovery
  physicalAddress?: number;   // Physical hardware address from discovery (hex)
  name?: string;              // optional label shown in config modal
  position: number;           // slot index within the rail (0-indexed)

  /**
   * Smartbox slot configuration — only present when model is a Smartbox base.
   * Array length equals the base's slotCount.
   * Each entry is a plugin model code or null (empty slot).
   * e.g. ["DT0B-TE", "DT0B-R", "DT0B-LE", null]
   */
  slots?: (string | null)[];
}

/** Physical cabinet. Modules are stored flat; rails are computed from widthUnits at render time. */
export interface Cabinet {
  id: LocalId;
  name: string;               // e.g. "Hoofdkast"
  location?: string;          // free text, e.g. "Technische ruimte"
  widthUnits: number;         // cabinet width in DIN M-units (1M ≈ 18mm), default 12
  modules: ModuleInstance[];  // ordered flat list — reflowed into rails at render time
}

/**
 * Woning/field device on CAN Segment 2 (switches, LCD panels, sensors).
 * `model` is the specific variant code (e.g. "DTBS-B4FA" — includes finish).
 */
export interface WoningDevice {
  id: LocalId;
  model: string;              // specific variant code (with finish)
  nodeAddress?: number;
  name?: string;
  position: number;           // order in woning panel (left → right)
}

export interface RailView {
  cabinets: Cabinet[];
  woningDevices: WoningDevice[];
}

// ─── Channel reference ────────────────────────────────────────────────────────

/**
 * Points to a specific channel on a specific hardware node.
 * Used to link a logical device (room device) to a physical channel (module).
 */
export interface ChannelRef {
  nodeAddress: number;        // node address (hex)
  unitAddress: number;        // unit/channel address on that node
  moduleInstanceId: LocalId;  // back-reference to the ModuleInstance in railView
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
  | 'dali_group';   // DALI group

/** A logical device placed in a room — linked to a physical channel. */
export interface RoomDevice {
  id: LocalId;
  type: DeviceType;
  name: string;
  channelRef?: ChannelRef;    // set once assigned to a physical channel
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
 * One wire in the wiring diagram.
 * Connects an output port on a source device to an input port on a target device.
 * Serialised to one or more binding strings at upload time.
 */
export interface BindingWire {
  id: LocalId;
  sourceDeviceId: LocalId;    // RoomDevice.id
  sourcePort: string;         // e.g. "short_press", "long_press"
  targetDeviceId: LocalId;    // RoomDevice.id
  targetPort: string;         // e.g. "toggle", "on", "dim_up"
  logicBlock?: LogicBlock;
  timerBlock?: TimerBlock;
  /** Raw binding strings for wires that couldn't be reconstructed visually (legacy import). */
  legacyRaw?: string[];
}

// ─── Top-level project ────────────────────────────────────────────────────────

export interface DuoProject {
  meta: ProjectMeta;
  railView: RailView;
  homeView: HomeView;
  /** All wiring connections. Each wire → one or more binding strings at upload. */
  bindings: BindingWire[];
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

export function createEmptyProject(name: string): DuoProject {
  const now = new Date().toISOString();
  return {
    meta: { name, created: now, modified: now, version: '1' },
    railView: { cabinets: [], woningDevices: [] },
    homeView: {
      floors: [{ id: 'floor-0', name: 'Gelijkvloers' }],
      rooms: [],
    },
    bindings: [],
  };
}

export function makeId(): LocalId {
  return Math.random().toString(36).slice(2, 10);
}
