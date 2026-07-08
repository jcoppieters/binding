export enum UnitType {
  kNoType = 0, kDimmer = 1, kSwitch = 2, kInput = 3,
  kTemperature = 4, kExtendedAudio = 5, kMood = 7, kSwitchingMotor = 8,
  kAudio = 10, kAV = 11, kIRTX = 12, kVideo = 14,

  kLightbulb = 22,  // not standard, but let's dream to fix this
  kCamera = 21,     // custom type
  kRegister = 24,      // custom type: for power and other metering

  kRGB = 30,        // virtual RGB Dimmer (3 dimmers)
  kRGBA = 31,       // virtual RGB Dimmer with alpha channel (4 dimmers)
  kHSL = 32,        // virtual HSL Dimmer (3 dimmers)
  kHSLA = 33,       // virtual HSL Dimmer with alpha channel (4 dimmers)

  kVirtualAudio = 133
}

export interface UnitInfo {
  // from hardware
  name: string;
  nodeAddress: number;
  unitAddress: number;
  type: UnitType;
  flags?: number;

  // config info
  active?: boolean;
  masterAddress?: string;      // address of master node
  masterPort?: number;         // port of master node

  // temperature stuff
  swingmodes?: { [key: string]: number };
  workingmodes?: { [key: string]: number };
  fanspeeds?: { [key: string]: number };
  swingangles?: { [key: string]: number };

  // camera stuff
  url?: string;
  user?: string;
  pwd?: string;

  // register stuff
  measure?: string;
  divider?: number;
  multiplier?: number;
  decimals?: number;
  register?: number;
  icon?: number;
}

export const kEmptyUnit: UnitInfo =
  {
    masterAddress: "0.0.0.0", masterPort: 5002,
    name: "unit", nodeAddress: 0, unitAddress: 0,
    type: UnitType.kNoType, active: false
  };

export function isLight(uc: UnitInfo) {
  return (uc.type === UnitType.kDimmer) || (uc.type === UnitType.kSwitch);
}

export function isTemperature(t: UnitInfo) {
  return (t.type == UnitType.kTemperature) && (t.name[0] != "<") && (t.name[0] != ">")
}

export function isRegister(t: UnitInfo) {
  return (t.type === UnitType.kRegister) || ((t.type == UnitType.kTemperature) && ((t.name[0] === "<") || (t.name[0] === ">")))
}

export function sameUnitInfo(a: UnitInfo, b: UnitInfo): boolean {
  return (a.masterAddress === b.masterAddress) &&
    (a.masterPort === b.masterPort) &&
    (a.nodeAddress === b.nodeAddress) &&
    (a.unitAddress === b.unitAddress);
}