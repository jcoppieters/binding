/**
 * Duotecno type maps — unit types, node types, module categories.
 *
 * UnitType values come from the Duotecno IP protocol (doc/protocol.ts).
 * Icons are UTF-8 emoji — sufficient for tool UI, no SVG needed.
 *
 * This is the client-side canonical copy. The server keeps a mirror in
 * `reader/src/models/unitTypes.ts` (plain ES modules here can't import a
 * TypeScript module without a bundler) — keep the two in sync.
 */

// ─── UnitType enum ────────────────────────────────────────────────────────────
// Matches kDimmer=1, kSwitch=2, ... kGUI=32 from the Duotecno protocol.
export const UnitType = Object.freeze({
  kNoType:        0,
  kDimmer:        1,
  kSwitch:        2,   // relay output
  kInput:         3,   // digital/analog input (button, sensor)
  kTemperature:   4,
  kExtendedAudio: 5,
  kMood:          7,   // virtual channel (mood / scene)
  kSwitchingMotor:8,
  kAudio:         10,
  kAV:            11,
  kIRTX:          12,
  kVideo:         14,
  kCamera:        21,
  kLightbulb:     22,
  kRegister:      24,
});

// ─── NodeType enum ────────────────────────────────────────────────────────────
export const NodeType = Object.freeze({
  kStandardNode: 1,
  kGatewayNode:  4,   // TCP server / master
  kModemNode:    8,   // config modem (never on CAN bus)
  kGUINode:      32,  // LCD / touchscreen field device → always house
});

// ─── Unit type info ───────────────────────────────────────────────────────────
// Used in rail-view discovery cards, module detail modals, and home-view binding.
// icon: UTF-8 emoji (no SVG required for this tool)
// label: Dutch short name shown in the UI
// category: DeviceType used for room devices / binding ports (null = not placeable)
// channelTypes: binding channel type strings (module channel-group matching)
// cabinetHardware: true → presence of this type means the node is cabinet hardware
export const UNIT_TYPE_INFO = Object.freeze({
  [UnitType.kDimmer]:         { label: 'Dimmer',           icon: '💡', category: 'dimmer', cabinetHardware: true,  channelTypes: ['dimmer_le','dimmer_te','dimmer_pwm','dimmer_dc'] },
  [UnitType.kSwitch]:         { label: 'Relais',           icon: '⚡', category: 'relay',  cabinetHardware: true,  channelTypes: ['relay_no','relay_nc','relay_ssr'] },
  [UnitType.kInput]:          { label: 'Input',            icon: '🔲', category: 'input',  cabinetHardware: false, channelTypes: ['input_digital','input_analog'] },
  [UnitType.kTemperature]:    { label: 'Sensor',           icon: '🌡', category: 'sensor', cabinetHardware: false, channelTypes: ['temperature'] },
  [UnitType.kExtendedAudio]:  { label: 'Audio ext.',       icon: '🔊', category: 'audio',  cabinetHardware: true,  channelTypes: ['audio'] },
  [UnitType.kMood]:           { label: 'Virtueel (moods)', icon: '🌙', category: 'mood',   cabinetHardware: false, channelTypes: [] },
  [UnitType.kSwitchingMotor]: { label: 'Motor',            icon: '🔄', category: 'motor',  cabinetHardware: true,  channelTypes: ['motor_updown','motor_polar'] },
  [UnitType.kAudio]:          { label: 'Audio',            icon: '🎵', category: 'audio',  cabinetHardware: true,  channelTypes: ['audio'] },
  [UnitType.kAV]:             { label: 'AV Matrix',        icon: '📺', category: null,     cabinetHardware: true,  channelTypes: [] },
  [UnitType.kIRTX]:           { label: 'IR TX',            icon: '📡', category: null,     cabinetHardware: false, channelTypes: [] },
  [UnitType.kVideo]:          { label: 'Video',            icon: '📹', category: null,     cabinetHardware: true,  channelTypes: [] },
  18:                         { label: 'Alarm',            icon: '🚨', category: null,     cabinetHardware: false, channelTypes: [] },
});

// ─── NodeType names ───────────────────────────────────────────────────────────
export const NODE_TYPE_NAMES = Object.freeze({
  [NodeType.kStandardNode]: 'Standard',
  [NodeType.kGatewayNode]:  'Gateway (master)',
  [NodeType.kModemNode]:    'Modem',
  [NodeType.kGUINode]:      'GUI (LCD)',
});

// ─── Module category icons ────────────────────────────────────────────────────
// Matches the 'category' field in modules/_index.json.
export const CATEGORY_ICON = Object.freeze({
  dimmer:  '💡',
  relay:   '⚡',
  input:   '🔲',
  motor:   '🔄',
  audio:   '🎵',
  av:      '📺',
  dali:    '🌐',
  psu:     '🔌',
  switch:  '🔘',  // house wall switch
  lcd:     '🖥',  // house LCD panel
});

// ─── Helpers ──────────────────────────────────────────────────────────────────

/** Returns the UNIT_TYPE_INFO entry for a unit type number, or a fallback. */
export function unitTypeInfo(type) {
  return UNIT_TYPE_INFO[type] ?? { label: `Type ${type}`, icon: '❓', category: null, cabinetHardware: false, channelTypes: [] };
}

/**
 * Returns true when a node's unit composition indicates cabinet hardware
 * (dimmer / relay / motor / audio outputs — never found on a wall switch).
 */
export function hasCabinetUnits(units = []) {
  return units.some(u => UNIT_TYPE_INFO[u.type]?.cabinetHardware === true);
}

