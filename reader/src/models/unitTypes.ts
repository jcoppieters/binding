/**
 * Duotecno type maps — unit types, node types, and legacy binding-import port mapping.
 *
 * This is the server-side mirror of `reader/public/app/unit-types.js`.
 * Keep the two files in sync — they intentionally duplicate the same literal
 * data because the browser client (plain ES modules, no bundler) cannot import
 * a TypeScript module directly.
 */

// ─── UnitType enum ────────────────────────────────────────────────────────────
// Matches kDimmer=1, kSwitch=2, ... kGUI=32 from the Duotecno protocol.
export const UnitType = Object.freeze({
  kNoType: 0,
  kDimmer: 1,
  kSwitch: 2, // relay output
  kInput: 3, // digital/analog input (button, sensor)
  kTemperature: 4,
  kExtendedAudio: 5,
  kMood: 7, // virtual channel (mood / scene)
  kSwitchingMotor: 8,
  kAudio: 10,
  kAV: 11,
  kIRTX: 12,
  kVideo: 14,
  kCamera: 21,
  kLightbulb: 22,
  kRegister: 24,
});

// ─── NodeType enum ────────────────────────────────────────────────────────────
export const NodeType = Object.freeze({
  kStandardNode: 1,
  kGatewayNode: 4, // TCP server / master
  kModemNode: 8, // config modem (never on CAN bus)
  kGUINode: 32, // LCD / touchscreen field device → always house
});

/** Functional category of a room device — matches DeviceType in project.ts. */
export type UnitCategory = 'dimmer' | 'relay' | 'motor' | 'input' | 'sensor' | 'audio' | 'mood' | 'dali_group';

export const UNIT_TYPE_INFO: Record<number, { label: string; icon: string; category: UnitCategory | null; cabinetHardware: boolean }> = Object.freeze({
  [UnitType.kDimmer]: { label: 'Dimmer', icon: '💡', category: 'dimmer' as UnitCategory, cabinetHardware: true },
  [UnitType.kSwitch]: { label: 'Relais', icon: '⚡', category: 'relay' as UnitCategory, cabinetHardware: true },
  [UnitType.kInput]: { label: 'Input', icon: '🔲', category: 'input' as UnitCategory, cabinetHardware: false },
  [UnitType.kTemperature]: { label: 'Sensor', icon: '🌡', category: 'sensor' as UnitCategory, cabinetHardware: false },
  [UnitType.kExtendedAudio]: { label: 'Audio ext.', icon: '🔊', category: 'audio' as UnitCategory, cabinetHardware: true },
  [UnitType.kMood]: { label: 'Virtueel (moods)', icon: '🌙', category: 'mood' as UnitCategory, cabinetHardware: false },
  [UnitType.kSwitchingMotor]: { label: 'Motor', icon: '🔄', category: 'motor' as UnitCategory, cabinetHardware: true },
  [UnitType.kAudio]: { label: 'Audio', icon: '🎵', category: 'audio' as UnitCategory, cabinetHardware: true },
  [UnitType.kAV]: { label: 'AV Matrix', icon: '📺', category: null, cabinetHardware: true },
  [UnitType.kIRTX]: { label: 'IR TX', icon: '📡', category: null, cabinetHardware: false },
  [UnitType.kVideo]: { label: 'Video', icon: '📹', category: null, cabinetHardware: true },
});

export function unitTypeInfo(type: number) {
  return UNIT_TYPE_INFO[type] ?? { label: `Type ${type}`, icon: '❓', category: null, cabinetHardware: false };
}

// ─── Legacy bind*.txt import — event/function code → binding-panel port id ────
// Used only by bindingConverterAPI.ts when converting old binding files to
// visual bindings. Centralised here (was duplicated as 4 separate consts).

/** Event code (controller side) → port id. Same for every device category. */
export const LEGACY_EVENT_TO_PORT: Record<number, string> = {
  0x01: 'lang', // Event Long (L)
  0x02: 'status', // Event State (switches/dimmers only)
  0x03: 'kort', // Event Short Pulse (P)
  0x04: 'status', // Status request
};

/** Function code → port id, per controllable category. 'default' covers switch/relay/dimmer. */
export const LEGACY_FUNCTION_TO_PORT: Record<UnitCategory | 'default', Record<number, string>> = {
  default: {
    0xfa6: 'schakel', // Toggle
    0xfa2: 'aan', // Turn on
    0xfa4: 'uit', // Turn off
    0xfb6: 'op', // Dim up
    0xfb4: 'neer', // Dim down
    0xf9f: 'trigger', // Scene/Mood trigger
  },
  mood: {
    0xfa6: 'kort', // Short Pulse → Kort input
    0xfa2: 'status', // Short Pulse On/Off → Status input
    0xfa4: 'lang', // Long On/Off → Lang input
  },
  motor: {
    0xb6d: 'op', // Up/Down toggle
    0xfb6: 'op', // Up
    0xfb4: 'neer', // Down
    0xfb3: 'stop', // Stop
  },
  dimmer: {}, relay: {}, input: {}, sensor: {}, audio: {}, dali_group: {},
};

/** Default port returned when a function code has no explicit mapping for this category. */
const LEGACY_FUNCTION_FALLBACK: Record<string, string> = {
  mood: 'kort',
  motor: 'op',
  default: 'schakel',
};

/** Get the correct legacy import port id for a function code, based on device category. */
export function getLegacyFunctionPort(functionCode: number, category: string): string {
  const table = LEGACY_FUNCTION_TO_PORT[category as UnitCategory] ?? {};
  return table[functionCode] ?? LEGACY_FUNCTION_TO_PORT.default[functionCode] ?? LEGACY_FUNCTION_FALLBACK[category] ?? LEGACY_FUNCTION_FALLBACK.default;
}
