/**
 * Duotecno type maps — unit types, node types, module categories.
 *
 * UnitType values come from the Duotecno IP protocol (doc/protocol.ts).
 * Icons are UTF-8 emoji — sufficient for tool UI, no SVG needed.
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
  kGUINode:      32,  // LCD / touchscreen field device → always woning
});

// ─── Unit type info ───────────────────────────────────────────────────────────
// Used in rail-view discovery cards and module detail modals.
// icon: UTF-8 emoji (no SVG required for this tool)
// label: Dutch short name shown in the UI
// channelTypes: binding channel type strings (for future Home View bindings)
// cabinetHardware: true → presence of this type means the node is cabinet hardware
export const UNIT_TYPE_INFO = Object.freeze({
  [UnitType.kDimmer]:         { label: 'Dimmer',           icon: '💡', cabinetHardware: true,  channelTypes: ['dimmer_le','dimmer_te','dimmer_pwm','dimmer_dc'] },
  [UnitType.kSwitch]:         { label: 'Relais',           icon: '⚡', cabinetHardware: true,  channelTypes: ['relay_no','relay_nc','relay_ssr'] },
  [UnitType.kInput]:          { label: 'Input',            icon: '🔲', cabinetHardware: false, channelTypes: ['input_digital','input_analog'] },
  [UnitType.kTemperature]:    { label: 'Sensor',           icon: '🌡', cabinetHardware: false, channelTypes: ['temperature'] },
  [UnitType.kExtendedAudio]:  { label: 'Audio ext.',       icon: '🔊', cabinetHardware: true,  channelTypes: ['audio'] },
  [UnitType.kMood]:           { label: 'Virtueel (moods)', icon: '🌙', cabinetHardware: false, channelTypes: [] },
  [UnitType.kSwitchingMotor]: { label: 'Motor',            icon: '🔄', cabinetHardware: true,  channelTypes: ['motor_updown','motor_polar'] },
  [UnitType.kAudio]:          { label: 'Audio',            icon: '🎵', cabinetHardware: true,  channelTypes: ['audio'] },
  [UnitType.kAV]:             { label: 'AV Matrix',        icon: '📺', cabinetHardware: true,  channelTypes: [] },
  [UnitType.kIRTX]:           { label: 'IR TX',            icon: '📡', cabinetHardware: false, channelTypes: [] },
  [UnitType.kVideo]:          { label: 'Video',            icon: '📹', cabinetHardware: true,  channelTypes: [] },
  18:                         { label: 'Alarm',            icon: '🚨', cabinetHardware: false, channelTypes: [] },
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
  switch:  '🔘',  // woning wall switch
  lcd:     '🖥',  // woning LCD panel
});

// ─── Helpers ──────────────────────────────────────────────────────────────────

/** Returns the UNIT_TYPE_INFO entry for a unit type number, or a fallback. */
export function unitTypeInfo(type) {
  return UNIT_TYPE_INFO[type] ?? { label: `Type ${type}`, icon: '❓', cabinetHardware: false, channelTypes: [] };
}

/**
 * Returns true when a node's unit composition indicates cabinet hardware
 * (dimmer / relay / motor / audio outputs — never found on a wall switch).
 */
export function hasCabinetUnits(units = []) {
  return units.some(u => UNIT_TYPE_INFO[u.type]?.cabinetHardware === true);
}
