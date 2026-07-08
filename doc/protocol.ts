import {Master} from './master';
import {temp} from '../pipes/temp.pipe';
import {CommRecord, DBInfo, LogFunction, Message, NodeInfo, NodeType, UnitState, WriteError} from "./types";
import {NodeConfig} from "./nodes";
import {Sanitizers} from "./sanitizers";
import {UnitInfo, UnitType} from "./unit";
import {hex, two} from "./helper_functions";
import {BasicAudioSources, Destination, LimitBasicAudioType} from "../models/audio";
import {kEmptyMasterMood, MasterMood, masterUnitDefaults, MoodUnit, UnitToMoodType} from "../models/mood";
import { TemperatureControlMode } from '../models/temperature';

// Duotecno master IP protocol implementation
// Johan Coppieters
//
// Dec 2018 - v1 - first version based on 1 smartbox
// Mar 2019 - v2 - rewrite to support multiple masters
//
// Dec 2019 - v3 - for app side only
// May-Nov 2021 - v4 - changes for pro-app
// Nov 2022 - Register and merge with gateway


///////////////////////////
// Commands + attributes //
///////////////////////////
enum Cmd {
  SetSensorValue = 7,
  Internal = 9,     // smartsystem defined (not Duotecno)

  SetBasicAudio = 159,
  SetExtendedAudio = 208,
  SetAVMatrix = 202,

  SetSwitch = 163,
  SetDimmer = 162,
  SetControl = 168,
  SetMotor = 182,
  SetSensor = 136,

  SetDateTime = 170,

  Login = 214,
  Heartbeat = 215,
  DatabaseInfo = 209,
  SetSchedule = 217,
  Register = 224
}

// for temperature - SetSensor
export enum setSensorMethods  {
  workingmode = 16,
  fanspeed = 17,
  swingangle = 18,
  swingmode = 19
}

// for Set Switch/Dimmer/Control
const reqDim = 3;
const reqOff = 9;
const reqOn = 10;

// for Login
const reqDisconnect = 0;
const reqConnect = 3;

// for DatabaseInfo = 209;
const reqDBInfo = 0;
const reqNodeInfo = 1;
const reqUnitInfo = 2;
const reqUnitStatus = 3;
const reqSensUnitStatus = 137;

// for Schedule
const reqSchedule = 218;

// for versions et al
const reqNodeManagement = 220;

export enum reqNodeAttributes {
  masterSupported = 0,
  isMaster = 4,
  nodeInfo = 5,
  nodeVersion = 6,
  nodeProtocol = 7
}


export function cmdName(cmd: number | string) {
  return Cmd[cmd] || "cmd" + cmd;
}

type CmdSet = {
  cmd: number,
  method: number,
  value?: number | boolean,
  data?: number[],
  message?: Message,
  reqStatus?: boolean
}

export enum TempPreset {
  off = -1,
  sun = 0,
  halfsun = 1,
  hsun = 1,
  moon = 2,
  halfmoon = 3,
  hmoon = 3
}

//////////////////////
// Received results //
//////////////////////
export enum Rec {
  // return info from cmdDatabaseInfo
  DBInfo = 0,
  NodeInfo = 1,
  UnitInfo = 2,

  Internal = 9,     // smartsystem defined (not Duotecno)

  ErrorMessage = 17,
  ConnectStatus = 67,
  AudioStatus = 23,
  AutoStatus = 48,
  AudioExtendedStatus = 70,
  TimeDateStatus = 71,
  HeartbeatStatus = 72,
  ScheduleStatus = 73,
  NodeMgtInfo = 74,
  Register = 77,

  // return info from recDBInfo
  Info = 64,

  // return info from reqUnitStatus
  Mood = 4,
  Dimmer = 5,
  Switch = 6,
  Sensor = 7,
  Motor = 38,
  Macro = 69
}

export function recName(rec: number) {
  return Rec[rec] || "rec" + rec;
}


/////////////////////////
// Node in the network //
/////////////////////////
export class Node {
  master: Master;
  active: boolean;
  name: string;
  index: number;
  nodeAddress: number;
  physicalAddress: number;
  type: NodeType;
  flags: number;
  nrUnits: number;
  units: Array<Unit>;
  serverAddress?: string;
  serverPort?: number;
  serverPassword?: string;

  constructor(master: Master, params: NodeConfig) {
    this.master = master;
    Sanitizers.nodeConfig(params, this);
    this.units = [];

    // remove | in names
    this.name = this.name.replace("\|", " ");
    // let separ = this.name.indexOf('|');
    // this.name = (separ < 0) ? this.name : this.name.substring(0, separ) + ' ' + (this.name.substring(separ + 1));
  }

  asNodeConfig(units?: Array<UnitInfo>): NodeConfig {
    return {
      name: this.name,
      nodeAddress: this.nodeAddress,
      physicalAddress: this.physicalAddress,
      type: this.type,
      serverAddress: this.serverAddress,
      serverPort: this.serverPort,
      serverPassword: this.serverPassword,
      units: units || [], nrUnits: units?.length || 0
    };
  }

  isNode(node: Node): boolean;
  isNode(master: string, port: number, nodeAddress: number): boolean;
  isNode(master: string | Node, port?: number, nodeAddress?: number): boolean {
    if (master instanceof Node) {
      const node = master;
      port = node.master.getPort();
      master = node.master.getAddress();
      nodeAddress = node.nodeAddress;
    }
    return (this.master.same(master, port)) &&
      (this.nodeAddress == nodeAddress);
  }

  getURL(protocol = "http") {
    return protocol + "://" + this.serverAddress + ":" + (this.serverPort || 8081);
  }

  hasSettings(): boolean {
    return !!this.serverAddress;
  }

  inMultiNode() {
    return this.master.inMultiNode();
  }

  typeName(): string {
    switch (this.type) {
      case NodeType.kStandardNode:
        return "Standard";
      case NodeType.kGatewayNode:
        return "Gateway";
      case NodeType.kModemNode:
        return "Modem";
      case NodeType.kGUINode:
        return "GUI";
      default:
        return "Unknown node type (" + this.type + ")";
    }
  }

  isMaster(): boolean {
    return (this.type === NodeType.kGatewayNode) || (this.type === NodeType.kGUINode);
  }

  getName(): string {
    return this.name;
  }

  getSort(): string {
    return this.getName().toLowerCase();
  }

  getNumber(): string {
    return hex(this.nodeAddress);
  }

  getDescription(): string {
    return this.getName() + ", active: " + this.active + ", type: " + this.typeName() + ", node: " + this.getName();
  }

  findUnit(unitAddress: number): Unit {
    return this.units.find(u => u && (u.unitAddress === unitAddress));
  }

  findUnitIndex(unitAddress: number): number {
    return this.units.findIndex(u => u && (u.unitAddress === unitAddress));
  }


  allActiveUnits(unitType?: UnitType | Array<UnitType>): Array<Unit> {
    if (!unitType)
      return this.units;

    else if (unitType instanceof Array)
      return this.units.filter(u => (u.active && (<Array<UnitType>>unitType).includes(u.type)));

    else
      return this.units.filter(u => (u.active && (u.type == unitType)));
  }
}


/////////////////////////
// Unit within a Node  //
/////////////////////////
export class Unit {
  node: Node;
  active: boolean = false;
  name: string;
  selected: boolean = false;
  index: number;
  nodeAddress: number;
  unitAddress: number;
  type: UnitType;
  flags: number;
  displayName: string;
  value: number | boolean;
  bufferedValue: number | boolean;
  status: number;
  bufferedStatus: number;

  lastRequested: number;
  bufferValues = false;

  // mood config stuff
  delay?: number;
  msg?: string;

  // Temperature stuff (values in x10 degree Celcius)
  preset: number;  // 0=sun, 1=half sun, 2=moon, 3=half moon, -1=off
  sun: number;
  hsun: number;
  moon: number;
  hmoon: number;
  temp: number;

  // Temperature - HVAC Extra's
  //  values
  workingmode: number;
  fanspeed: number;
  swingangle: number;
  swingmode: number;
  //  choices
  workingmodes?: {[key: string]: number};
  fanspeeds?: {[key: string]: number};
  swingangles?: {[key: string]: number};
  swingmodes?: {[key: string]: number};


  // Audio Extra's
  destinations: Destination[]; // max 4, each with sources
  destinationId: number;       // current
  destinationNr: number;       // current
  destination: Destination;    // destinations[current]

  // Camera Extra's
  url: string;
  user: string;
  pwd: string;

  // Register Extra's
  decimals: number;
  divider: number;
  multiplier: number;
  icon: number;
  measure: string;
  register: number;


  constructor(node, params: UnitInfo) {
    this.node = node;
    Sanitizers.unitInfo(params, this);
    this.lastRequested = 0;

    // remove the |
    this.name = this.name.replace(/\|/g, " ");

    // delete all type modifiers ( $, * and ! )
    // if the display name is empty make a N[nodeAdr]-U[unitAdr] name.
    this.displayName = this.name.replace(/\$|\*|\!/g, '') || this.getSerialNr();
  }

  turnOnBuffering() {
    this.bufferValues = true;
    this.bufferedValue = this.value;
    this.bufferedStatus = this.status;
    setTimeout(() => {
      this.bufferValues = false;
      this.value = this.bufferedValue;
      this.status = this.bufferedStatus
    }, 3000 + ((this.node.master.nrActiveUnits() > 200) ? 2000 : 0));
  }

  receive(value: number | boolean, status: number | boolean) {
    this.bufferedValue = value;
    this.bufferedStatus = status as number;
    // console.log("receive: " + this.bufferValues + " -> " + value + "/" + status);
    if (!this.bufferValues) {
      this.value = this.bufferedValue;
      this.status = this.bufferedStatus;
    }
  }

  asUnitInfo(): UnitInfo {
    return {
      name: this.name, type: this.type, active: this.active,
      url: this.url, user: this.user, pwd: this.pwd,
      register: this.register, measure: this.measure, icon: this.icon,
      decimals: this.decimals, multiplier: this.multiplier, divider: this.divider,
      masterAddress: this.node.master.getAddress(), masterPort: this.node.master.getPort(),
      nodeAddress: this.node.nodeAddress, unitAddress: this.unitAddress
    };
  }

  asMoodConfig(): MasterMood {
    return {...kEmptyMasterMood, name: this.name, id: this.unitAddress};
  }

  asMoodUnit(): MoodUnit {
    const mu: MoodUnit = {
      nodeAddress: this.nodeAddress, unitAddress: this.unitAddress, msg: "", unitType: UnitType.kNoType,
      code: UnitToMoodType[this.type]
    };
    masterUnitDefaults(mu);
    mu.name = this.name;
    return mu;
  }

  isUnitOf(master: Master, unit: MoodUnit): boolean {
    return master.same(this.node.master) &&
      (this.nodeAddress == unit.nodeAddress) &&
      (this.unitAddress == unit.unitAddress);
  }


  hasSpecials(): boolean {
    let special = this.name.indexOf("|20");
    if (special < 0) special = this.name.indexOf("|50");
    if (special < 0) special = this.name.indexOf("|90");
    if (special < 0) special = this.name.indexOf("|OFF");
    return special >= 0;
  }


  isUnit(UnitInfo: UnitInfo): boolean;
  isUnit(unit: Unit): boolean;
  isUnit(master: string, port: number, nodeAddress: number, unitAddress: number): boolean;

  isUnit(master: Unit | string | UnitInfo, port?: number, nodeAddress?: number, unitAddress?: number): boolean {
    if (master instanceof Unit) {
      const unit = master;
      return ((this.node.master.same(unit.node.master.getAddress(), unit.node.master.getPort())) &&
        (this.node.nodeAddress == unit.node.nodeAddress) &&
        (this.unitAddress == unit.unitAddress));

    } else if (typeof master === "string") {
      return ((this.node.master.same(master, port)) &&
        (this.node.nodeAddress == nodeAddress) &&
        (this.unitAddress == unitAddress));

    } else { /* if (typeof master === "UnitInfo") */
      const uc = master;
      return ((this.node.master.same(uc.masterAddress, uc.masterPort)) &&
        (this.node.nodeAddress == uc.nodeAddress) &&
        (this.unitAddress == uc.unitAddress));
    }
  }

  included(units: Array<UnitInfo>): boolean {
    return (!!units) && (!!units.find(uc => this.isUnit(uc)));
  }


  sameValue(value): boolean {
    if (this.type === UnitType.kSwitchingMotor) {
      return (((+this.value == UnitState.kOpening) && (value == 4)) ||
        ((+this.value == UnitState.kClosing) && (value == 5)) ||
        ((+this.value <= UnitState.kOpen) && (value == 3)));
    } else {
      return (this.value == value);
    }
  }

  typeName(): string {
    switch (this.getType()) {
      case UnitType.kDimmer:
        return 'Dimmer';
      case UnitType.kSwitch:
        return 'Switch/Relay';
      case UnitType.kLightbulb:
        return 'Lightbulb';
      case UnitType.kInput:
        return 'Control input';
      case UnitType.kTemperature:
        return 'Temperature sensor';
      case UnitType.kRegister:
        return 'Register value';
      case UnitType.kExtendedAudio:
        return 'Extended audio';
      case UnitType.kMood:
        return 'Virtual mood';
      // case UnitType.kCondition: return 'Condition';
      case UnitType.kSwitchingMotor:
        return 'Switch motor';
      // case UnitType.kGarageDoor: return 'Garagedoor';
      // case UnitType.kDoor: return 'Door';
      // case UnitType.kLock: return 'Lock';
      // case UnitType.kUnlocker: return 'Unlocker';
      case UnitType.kAudio:
        return 'Basic audio';
      case UnitType.kAV:
        return 'AV Matrix';
      case UnitType.kIRTX:
        return 'IRTX';
      case UnitType.kVideo:
        return 'Video multiplexer';
      case UnitType.kCamera:
        return 'Camera';
      default:
        return 'Unknown unit type (' + this.type + ')';
    }
  }

  getName(): string {
    return this.displayName;
  }

  getDisplayName(): string {
    if (this.inMultiNode) {
      return this.displayName + ' on ' + this.node.getName();
    } else {
      return this.displayName;
    }
  }

  getNumber(): string {
    return this.node.getNumber() + ';' + hex(this.unitAddress);
  }

  getSort(): string {
    const name = this.getName().toLowerCase();
    switch (this.type) {
      case UnitType.kTemperature:
        return "01|" + name;
      case UnitType.kSwitchingMotor:
        return "02|" + name;
      case UnitType.kDimmer:
        return "03|" + name;
      case UnitType.kSwitch:
        return "04|" + name;
      case UnitType.kMood:
        return "09|" + name;
      case UnitType.kInput:
        return "10|" + name;
      case UnitType.kRegister:
        return "11|" + name;
      case UnitType.kExtendedAudio:
        "12|" + name;
      case UnitType.kAudio:
        return "13|" + name;
      case UnitType.kAV:
        return "14|" + name;
      case UnitType.kVideo:
        return "15|" + name;
      case UnitType.kIRTX:
        return "19|" + name;
      default:
        return "99|" + name;
    }
  }

  getType(): UnitType {
    return this.type;
  }


  getSerialNr(): string {
    if (this.inMultiNode) {
      return this.node.getName() + '-N' + this.nodeAddress + '-U' + this.unitAddress;
    } else {
      return 'N' + this.nodeAddress + '-U' + this.unitAddress;
    }
  }

  getModelName(): string {
    return this.typeName() + " " + hex(this.node.nodeAddress) + ";" + hex(this.unitAddress);
  }

  registerValue(): string {
    if ((!this.isRegister()) || (typeof this.value === "boolean")) return "-";
    return (this.value / this.divider * this.multiplier).toFixed(this.decimals) + " " + this.measure;
  }


  isCtrl(): boolean {
    return this.isSwitch() || this.isLight();
  }

  isSwitch(): boolean {
    return (this.type === UnitType.kSwitch);
  }

  isLight(): boolean {
    return this.isDimmer() || (this.type === UnitType.kLightbulb);
  }

  isMood(): boolean {
    return (this.type === UnitType.kMood);
  }

  isInput(): boolean {
    return (this.type === UnitType.kInput);
  }

  isRegister(): boolean {
    return (this.type === UnitType.kRegister);
  }

  isTemperature(): boolean {
    return (this.type === UnitType.kTemperature);
  }

  isExtendedTemperature(): boolean {
    return !!(this.swingmodes || this.swingangles || this.workingmodes || this.fanspeeds);
  }

  isDimmer(): boolean {
    return (this.type === UnitType.kDimmer);
  }

  isUpDown(): boolean {
    return (this.type === UnitType.kSwitchingMotor);
  }

  isMedia(): boolean {
    return (this.type == UnitType.kAudio) || (this.type == UnitType.kExtendedAudio) ||
      (this.type == UnitType.kVideo) || (this.type == UnitType.kAV);
  }

  isCamera(): boolean {
    return (this.type === UnitType.kCamera);
  }

  async setPreset(preset: number, temp: number) {
    await this.node.master.setPreset(this, preset, temp);
  }

  async selectPreset(preset: number) {
    await this.node.master.selectPreset(this, preset);
  }

  async sensorOnOff(on: boolean) {
    await this.node.master.setTempOnOff(this, on);
  }

  async setHVAC(mode: TemperatureControlMode, value: number) {
    await this.node.master.doHVAC(this, mode, value);
  }

  async doIncDecPreset(inc: boolean) {
    await this.node.master.doIncDecPreset(this, inc);
  }

  async audioSrcDest(source: number, destination: number) {
    await this.node.master.setAudioSrcDest(this, source, destination);
  }

  async audioAction(action: string, fnc?: string) {
    await this.node.master.sendAudioAction(this, action, fnc);
  }

  inMultiNode() {
    return this.node.inMultiNode();
  }

  async reqState() {
    const req = new Date().getTime();
    if (req - this.lastRequested > 250) {
      // console.log("+++ allowed reqState: " + (req - this.lastRequested));
      this.lastRequested = req
      await this.node.master.requestUnitStatus(this);
    } else {
      //console.log("--- blocked reqState: " + (req - this.lastRequested))
    }
  }

  async setState(value) {
    await this.node.master.setUnitStatus(this, value);
  }


  getDispayState(): string {
    switch (this.getType()) {
      case UnitType.kDimmer:
        return ((this.status) ? 'on' : 'off') + ' (' + this.value + '%)';

      case UnitType.kSwitch:
      case UnitType.kLightbulb:
        return (this.status) ? 'on' : 'off';

      case UnitType.kInput:
        return (this.status) ? 'on' : 'off';

      case UnitType.kRegister:
        return this.registerValue();

      case UnitType.kTemperature:
        return temp(this.value) + 'C - preset: ' + this.preset;

      //  case UnitType.kCondition:
      case UnitType.kMood:
        return (this.status) ? 'on' : 'off';

      //  case UnitType.kLock:
      //    return (this.status) ? 'locked' : 'unlocked';

      //  case UnitType.kUnlocker:
      //    return (this.status) ? 'unlocking' : 'locked';

      case UnitType.kCamera:
        return 'on';

      //  case UnitType.kGarageDoor:
      //  case UnitType.kDoor:
      case UnitType.kSwitchingMotor:
        if (this.status === UnitState.kOpening) return 'opening';
        if (this.status === UnitState.kClosing) return 'closing';
        if (this.status === UnitState.kOpen) return 'open';
        if (this.status === UnitState.kClosed) return 'closed';
        if (this.status === UnitState.kStopped) return 'stopped';

    }
    return (typeof this.status != "undefined") ? this.status.toString() : 'unknown';
  }

  isOn(): boolean {
    switch (this.getType()) {
      case UnitType.kDimmer:
      case UnitType.kSwitch:
      case UnitType.kLightbulb:
        return !!this.status;

      case UnitType.kTemperature:
      case UnitType.kCamera:
      case UnitType.kInput:
      case UnitType.kMood:
        return false;

      case UnitType.kAudio:
      case UnitType.kExtendedAudio:
      case UnitType.kAV:
      case UnitType.kVideo:
        return !!this.status;

      case UnitType.kSwitchingMotor:
        return (this.status === UnitState.kOpen);
    }
    return false;
  }

  getDescription() {
    return this.getDisplayName() + ', active: ' + this.active + ', type: ' + this.typeName() + ', status: ' + this.status + ', value: ' + this.value;
  }

}


/////////////////////////////////////
// IP node protocol implementation //
/////////////////////////////////////
type DeliverStatus = (unit: Unit) => void;
type ValueSubscribtion = { deliver: DeliverStatus, unit: Unit };
// callbacks, waiting to be called when a status for them arrives
const subscribers: Array<ValueSubscribtion> = [];

export const Protocol = {
  // set to a different value if needed.
  logger: console.log,

  setLogger(logger: LogFunction) {
    this.logger = logger;
  },

  ////////////////////
  // Helper methods //
  ////////////////////

  getStr: function (arr: Array<number>, at: number) {
    return arr.slice(at + 1, at + arr[at] + 1)
      .map(val => String.fromCharCode(val))
      .join('');
  },

  makeWord: function (arr: Array<number>, at: number) {
    return arr[at + 0] * 256 + arr[at + 1];
  },
  makeSignedWord: function (arr: Array<number>, at: number) {
    if (arr[at + 0] > 127)
      return (arr[at + 0] - 255) * 256 +
        (arr[at + 1] - 255) - 1;
    else
      return arr[at + 0] * 256 + arr[at + 1];
  },
  makeLong: function (arr: Array<number>, at: number) {
    return arr[at + 0] * 256 * 256 * 256 +
      arr[at + 1] * 256 * 256 +
      arr[at + 2] * 256 +
      arr[at + 3];
  },
  makeSigned: function (arr: Array<number>, at: number) {
    if (arr[at + 0] > 127)
      return (arr[at + 0] - 255) * 256 * 256 * 256 +
        (arr[at + 1] - 255) * 256 * 256 +
        (arr[at + 2] - 255) * 256 +
        (arr[at + 3] - 255) - 1;
    else
      return arr[at + 0] * 256 * 256 * 256 +
        arr[at + 1] * 256 * 256 +
        arr[at + 2] * 256 +
        arr[at + 3];
  },
  signedToArray: function (value: number) {
    if (value < 0)
      return [256 + Math.floor(value / 256 / 256 / 256),
        256 + Math.floor(value / 256 / 256) % 256,
        256 + Math.floor(value / 256) % 256,
        256 + Math.floor(value) % 256]
    else
      return [Math.floor(value / 256 / 256 / 256),
        Math.floor(value / 256 / 256) % 256,
        Math.floor(value / 256) % 256,
        Math.floor(value) % 256];
  },
  signedWordToArray: function (value: number) {
    if (value < 0)
      return [256 + Math.floor(value / 256) % 256, 256 + Math.floor(value) % 256];
    else
      return [Math.floor(value / 256) % 256, Math.floor(value) % 256];
  },
  wordToArray: function (value: number) {
    return [Math.floor(value / 256) % 256, Math.floor(value) % 256];
  },

  isStatus: function (cmd: number): boolean {
    return (cmd === Rec.Mood) ||
      (cmd === Rec.Dimmer) ||
      (cmd === Rec.Switch) ||
      (cmd === Rec.Sensor) ||
      (cmd === Rec.Motor) ||
      (cmd === Rec.Macro);
  },


  //////////////////////////
  // Code to String stuff //
  //////////////////////////

  translateError: function (err: Message): string {
    if (err[0] != Rec.ErrorMessage) return "received unexpected data: " + err;

    switch (err[1]) {
      case 11:
        return "Wrong object method received for " + err[2] + "/" + err[3];
      case 12:
        return "Wrong Message Code received: " + err[2];
      case 18:
        return "This function can only be executed when this node is the master";
      case 128:
        return "The node database is not ready";
      case 129:
        return "Node " + err[2] + " could not be found in the database";
      case 130:
        return "Wrong node index: " + err[2];
      case 131:
        return "Unit " + err[2] + " with address " + err[3] + " could not be found in the database";
      case 132:
        return "Wrong unit index " + err[3] + " for this node " + err[2];
      case 133:
        return "Unit " + err[3] + " of node " + err[2] + " is of a different type";
      case 140:
        return "The requested operation is not allowed";
      case 141:
        return "The requested operation is not allowed because a wrong access code is used";
      case 142:
        return "The requested operation is not implemented in this software version";
      default:
        return "Unknown error";
    }
  },


  ////////////////////
  // Socket methods //
  ////////////////////

  write: function (socket: WebSocket /* | Socket */, data: Message | string): WriteError {
    if (!data) {
      this.logger("protocol", "sending: empty command??");
      return;
    } 

    let cmd: string | number = parseInt(<string>data[0]);
    if (isNaN(cmd)) cmd = <string>data[0];

    if (data instanceof Array) {
      data = data.join(",");
    }

    if (typeof data === "string") {
      // if no enclosing "[...]", add them
      if (data[0] != "[") data = "[" + data + "]";
      // this.logger("protocol", "sending: " + cmdName(cmd) + " - " + data);

      try {
        // append a LF char and send
        socket.send(data + String.fromCharCode(10));
        return WriteError.writeOK;

      } catch (err) {
        this.logger('error sending through socket ' + err.message);
        return WriteError.writeFatal;
      }
    } else {
      throw (new Error("wrong data type for sending"));
    }
  },

  //////////////////////////////
  // Handle incoming data     //
  //  strip []                //
  //  convert to array        //
  //  convert chars to ints   //
  //////////////////////////////
  nextMessage: function (buffer: string): CommRecord {
    // pre return result
    const nextRec: CommRecord = {rest: buffer, isStatus: false, message: null, cmd: 0, raw: ""};

    // no "start-of-data" -> discard buffer || else -> trim buffer
    const begin = buffer.indexOf('[');
    if (begin < 0) {
      nextRec.rest = "";
    } else if (begin != 0) {
      nextRec.rest = buffer.substring(begin);
    }

    // we either have valid start data or nothing
    if (nextRec.rest.length > 0) {
      // do we have an "end-of-data" in our buffer
      let end = nextRec.rest.indexOf(']');

      // if no end-of-data was found:
      // leave it in the buffer and hope more data will arrive soon
      //TODO: set up a timer that clears the buffer if nothing comes through

      if (end >= 0) {
        // fetch the first available message (discard the [ and ])
        nextRec.raw = nextRec.rest.substring(1, end);

        // delete the used message from the input buffer
        // if there, also delete the trailing LF (0x0A)
        if ((end <= nextRec.rest.length) && (nextRec.rest.charCodeAt(end + 1) === 0x0A)) end++;
        nextRec.rest = nextRec.rest.substring(end + 1);

        // convert to array and turn strings into numbers
        nextRec.message = nextRec.raw.split(',').map(c => parseInt(c));

        // get the first byte to see what kind of incoming data
        nextRec.cmd = <number>nextRec.message[0];
        nextRec.isStatus = this.isStatus(nextRec.cmd);

        // this.logger("protocol", (nextRec.isStatus ? "status -> " : "") + nextRec.message);
      }

    }
    return nextRec;
  },


  buildLogin: function (password: string): Message {
    password = password || "";
    return [Cmd.Login, reqConnect, password.length,
      ...password.split('').map(c => c.charCodeAt(0))];
  },

  buildDisconnect: function (): Message {
    return [Cmd.Login, reqDisconnect];
  },

  buildHeartbeat: function (): Message {
    return [Cmd.Heartbeat, 1];
  },


  /////////////////////////////////////
  // Collect info of all found nodes //
  /////////////////////////////////////

  buildDBInfo: function (): Message {
    return [Cmd.DatabaseInfo, reqDBInfo];
  },

  buildNodeInfo: function (nodeInx: number): Message {
    return [Cmd.DatabaseInfo, reqNodeInfo, nodeInx];
  },

  buildUnitInfo: function (node: Node, unitInx: number) {
    return [Cmd.DatabaseInfo, reqUnitInfo, node.nodeAddress, unitInx];
  },

  buildRequestUnitStatus: function (node: Node, unit: Unit): Message {
    return [Cmd.DatabaseInfo, reqUnitStatus, node.nodeAddress, unit.unitAddress, unit.type];
  },

  buildRequestSensStatus: function (node: Node, unit: Unit, method: setSensorMethods): Message {
    return [reqSensUnitStatus, method, node.nodeAddress, unit.unitAddress];
  },

  buildRequestSchedule(): Message {
    return [reqSchedule, 0];
  },
  buildRequestNodeMgt(method = reqNodeAttributes.nodeProtocol): Message {
    if (method == reqNodeAttributes.nodeVersion) {
      console.log("*** ERROR IN IP PROTOCOL: don't ask for nodeVersion");
      method = reqNodeAttributes.nodeProtocol;
    }
    return [reqNodeManagement, method];
  },

  getCmdAndMethod: function (unit: Unit, value: number | boolean): CmdSet {
    const updown = (bump, value): number => {
      if ((bump == -1) && (value > 0)) return value - 1;
      else if ((bump == -2) && (value < 100)) return value + 1;
      else return value;
    };

    switch (unit.type) {
      case UnitType.kDimmer:
        if (typeof value === "boolean")
          return {cmd: Cmd.SetDimmer, method: (value) ? reqOn : reqOff};
        else if (value <= 0)
          return {cmd: Cmd.SetDimmer, method: reqOff};
        else
          return {cmd: Cmd.SetDimmer, method: reqDim, value: Math.max(Math.min(value, 99), 1)};

      case UnitType.kSwitch:
        return {cmd: Cmd.SetSwitch, method: (value) ? 3 : 2};

      case UnitType.kInput:
      case UnitType.kMood:
        if (+value < 0)
          return {cmd: Cmd.SetControl, method: 2}; // short pulse
        else
          return {cmd: Cmd.SetControl, method: 3, value: (value) ? 1 : 0};  // long event + 0/1

      case UnitType.kSwitchingMotor:
        return {cmd: Cmd.SetMotor, method: <number>value};  // 5 close, 4 open, 3 is stop

      case UnitType.kTemperature:
        return {cmd: Cmd.SetSensor, method: 13 /* select preset */, value};

      case UnitType.kAudio:
        if (typeof value === "boolean")
          return {cmd: Cmd.SetBasicAudio, method: value ? 4 : 5};
        else
          return {cmd: Cmd.SetBasicAudio, method: (value == -1) ? 9 : 8};

      case UnitType.kExtendedAudio:
        if (typeof value === "boolean")
          return {
            cmd: Cmd.SetExtendedAudio, method: value ? 4 : 5,
            data: [unit.destinationId]
          };
        else if (value < 0)  // -1 = down, -2 = up
          return {
            cmd: Cmd.SetExtendedAudio, method: 12,
            data: [unit.destinationId, updown(value, unit.value)]
          };
        else
          return {
            cmd: Cmd.SetExtendedAudio, method: 12,
            data: [unit.destinationId, <number>unit.value]
          };

      case UnitType.kAV:
        const source = (unit.destination) ? unit.destination.source : 255;
        if (typeof value === "boolean")
          return {cmd: Cmd.SetAVMatrix, method: value ? 2 : 3, data: [unit.destinationId, source]};  // connect - disconnect
        else if (value < 0)   // -1 = down, -2 = up
          return {cmd: Cmd.SetAVMatrix, method: (value == -1) ? 8 : 7, data: [unit.destinationId, 1]};  // volume inc/dec
        else
          return {cmd: Cmd.SetAVMatrix, method: 4, data: [unit.destinationId, value]}; // volume

      case UnitType.kIRTX:
      case UnitType.kVideo:

      default: // "Unknown unit type (" + unit.type + ")";
        // this.logger('setting ' + unit.type + ' not yet implemented');
        return {cmd: 0, method: 0, value: 0};
    }
  },

  buildSetCmd: function (node: Node, unit: Unit, value: number | boolean): CmdSet {
    let params = this.getCmdAndMethod(unit, value);
    if (params.cmd) {
      params.message = [params.cmd, params.method, node.nodeAddress, unit.unitAddress];
      if (typeof params.value != 'undefined') {
        params.message.push(params.value);
      }
      if (typeof params.data != 'undefined') {
        params.message.push(...params.data);
      }

      // some need a requestStatus afterwards
      params.reqStatus = ((params.cmd === Cmd.SetDimmer) && (params.method === reqDim));
    }
    return params;
  },

  /* Temperature / Presets */
  buildSelectPreset(node: Node, unit: Unit, preset: number): Message {
    // console.log("buildSelectPreset " + unit.getDisplayName() + " to " + preset);
    let params = this.getCmdAndMethod(unit, preset);
    return [params.cmd, 13, node.nodeAddress, unit.unitAddress, preset];
  },
  buildSetSensor(node: Node, unit: Unit, value: number): Message {
    return [Cmd.SetSensorValue, 10, node.nodeAddress, unit.unitAddress, UnitType.kTemperature, ...this.wordToArray(value)];
  },
  buildSetPreset(node: Node, unit: Unit, preset: number, value: number): Message {
    return [Cmd.SetSensor, 1, node.nodeAddress, unit.unitAddress, preset, Math.floor(value / 256), Math.floor(value) % 256];
  },
  buildIncDecPreset(node: Node, unit: Unit, inc: boolean): Message {
    return [Cmd.SetSensor, (inc) ? 5 : 6, node.nodeAddress, unit.unitAddress];
  },
  buildSensorOnOff(node: Node, unit: Unit, on: boolean): Message {
    return [Cmd.SetSensor, 3, node.nodeAddress, unit.unitAddress, (on) ? 1 : 0];
  },
  buildHVAC(node: Node, unit: Unit, mode: TemperatureControlMode, value: number): Message {
    if (value < 0) value = 0; // -1 means for swingmodes: off (not changeable)

    if (mode == "swingangle")
      return [Cmd.SetSensor, setSensorMethods.swingmode, node.nodeAddress, unit.unitAddress, Math.floor(value / 256), value % 256];
    else
      return [Cmd.SetSensor, setSensorMethods[mode], node.nodeAddress, unit.unitAddress, value];
  },

  /* Schedule commands */
  buildSendSchedule(schedule: number): Message {
    return [Cmd.SetSchedule, 0, Math.max(0, Math.min(3, schedule))];
  },

  buildSendDateTime(date: Date): Message {
    const year = date.getFullYear();
    return [Cmd.SetDateTime, 0, 49, 49, 49, 49,
      date.getHours(), date.getMinutes(), date.getSeconds(), 0,
      date.getDate(), date.getMonth() + 1, Math.floor(year / 100), year % 100];
  },

  /* Audio */
  getAudioIds(unit: Unit, source: number, destination: number) {
    const destNr = Math.max(0, Math.min(unit.destinations.length - 1, destination));
    const srcNr = Math.max(0, Math.min(unit.destinations[destNr].sources.length, source));

    const unitAdr = unit.destinations[destNr].unitAddress;
    const nodeAdr = unit.destinations[destNr].nodeAddress;
    return {destNr, srcNr, unitAdr, nodeAdr};
  },

  buildAudioSrcDest(node: Node, unit: Unit, source: number, destination: number): Message {
    if (unit.type === UnitType.kAudio) {
      return [Cmd.SetBasicAudio, 1, node.nodeAddress, unit.unitAddress,
        Math.max(0, Math.min(7, source))];

    } else if (unit.type === UnitType.kExtendedAudio) {
      if ((!unit.destinations) || (!unit.destinations.length)) {
        // this.logger('set SrcDest ' + unit.displayName + ' has no destinations');
        return null;
      }
      const {destNr, srcNr, unitAdr, nodeAdr} = this.getAudioIds(unit, source, destination);
      return [Cmd.SetExtendedAudio, 1, nodeAdr, unitAdr, destNr, srcNr];

    } else {
      // this.logger('set SrcDest ' + unit.type + ' not yet implemented');
      return null;
    }
  },

  actions: {
    basic: {
      'back': 38,
      'skipback': 36,
      'play': 32,
      'pause': 34,
      'stop': 33,
      'skipforward': 35,
      'forward': 37,
      'function': 23,
      'functions': {
        '1': 0, '2': 1, '3': 2, '4': 3, '5': 4, '6': 5, '7': 6, '8': 7, '9': 8, '0': 9,
        'up': 10, 'down': 11, 'left': 12, 'right': 13, 'enter': 14, '-': 15,
        'menu': 16, 'guide': 17, 'info': 18, 'exit': 19
      }

    },
    extended: {
      'back': 21,
      'skipback': 19,
      'play': 15,
      'pause': 17,
      'stop': 16,
      'skipforward': 18,
      'forward': 20,
      'function': 24,
      'functions': {
        '1': 0, '2': 1, '3': 2, '4': 3, '5': 4, '6': 5, '7': 6, '8': 7, '9': 8, '0': 9,
        'up': 10, 'down': 11, 'left': 12, 'right': 13, 'enter': 14, '-': 15,
        'menu': 16, 'guide': 17, 'info': 18, 'exit': 19
      }
    }
  },
  buildAudioAction(node: Node, unit: Unit, action: string, fnc?: string): Message {
    let msg = null;
    if (unit.type === UnitType.kAudio) {
      msg = [Cmd.SetBasicAudio, this.actions.basic[action], node.nodeAddress, unit.unitAddress];
      if (fnc) {
        msg.push(this.actions.basic.functions[fnc]);
      }

    } else if (unit.type === UnitType.kExtendedAudio) {
      const {destNr, unitAdr, nodeAdr} = this.getAudioIds(unit, 0, unit.destinationId);

      msg = [Cmd.SetExtendedAudio, this.actions.extended[action], nodeAdr, unitAdr, destNr];
      if (fnc) {
        msg.push(this.actions.extended.functions[fnc]);
      }
    } else {
      // this.logger('send Action to ' + unit.typeName() + ' not yet implemented');
    }
    return msg;
  },

  buildRegister(register: number, value?: number) {
    if (typeof value != "undefined") {
      // set register
      return [Cmd.Register, 1, ...this.wordToArray(register), ...this.signedToArray(value)];
    } else {
      // get register
      return [Cmd.Register, 2, ...this.wordToArray(register)];
    }
  },

  ///////////////////
  // Received info //
  ///////////////////
  receiveStatus: function (next: CommRecord, unit: Unit): boolean {
    // unit.status 
    //   0=off,1=on,2=pir-on
    //   0=stopped, 1/2=stopped-down/up, 3/4=busy-down/up 
    //   0=idle, 1=heating, 2=cooling
    // after: 136, swingmode: [19],node/unit: [11,0], [0],[1]
    // example: 7,1,11,0,4,0,1,1,1,0,0,0,220,0,225,0,215,0,220,0,0, [0,0],0,0,0
    if (next.cmd === Rec.Sensor) {
      // sensor -> value
      unit.value = next.message[9] * 256 + next.message[10];   // 10x current temperature
      unit.status = next.message[7];                            // 0=idle, 1=heating, 2=cooling
      unit.preset = (next.message[6]) ? next.message[8] : -1;   // 0=sun, 1=half sun, 2=moon, 3=half moon, -1 = off
      unit.sun = next.message[11] * 256 + next.message[12];   // 10x temperature
      unit.hsun = next.message[13] * 256 + next.message[14];   // 10x temperature
      unit.moon = next.message[15] * 256 + next.message[16];   // 10x temperature
      unit.hmoon = next.message[17] * 256 + next.message[18];   // 10x temperature
      // console.log('received temperature = ' + temp(unit.value) + ", preset = " + unit.preset);

      if (next.message[1] === 1) {
        // extended sensor status
        unit.swingangle = next.message[21] * 256 + next.message[22]; 
        unit.workingmode = next.message[23];
        unit.fanspeed = next.message[24];
        unit.swingmode = next.message[25];
        // console.log('received extended sensor status for ' + unit.getDisplayName() + 
        //   ' with swingmode = ' + unit.swingangle + ', workingmode = ' + unit.workingmode +
        //   ', fanspeed = ' + unit.fanspeed + ', swingmode = ' + unit.swingmode);
      }
      return true;


      // Dimmers, switches and moods have
      //  - status (0=off,1=on,2=pir-on)
      //  = value (true/false for switches and moods, 1-99 for dimmers)
    } else if (next.cmd === Rec.Switch) {
      // switch -> boolean
      unit.receive(next.message[6] > 0, next.message[6]);
      return true;

    } else if (next.cmd === Rec.Dimmer) {
      // dimmer -> 0 .. 99
      unit.receive(next.message[7], next.message[6]);
      return true;

    } else if (next.cmd === Rec.Mood) {
      // control -> boolean
      unit.status = next.message[6];
      unit.value = (next.message[6] != 0);
      return true;

    } else if (next.cmd === Rec.Motor) {
      // motor -> boolean/status
      unit.status = next.message[6];       // 0 = stopped, 1 stopped/down, 2 = stopped/up, busy/down, busy/up
      unit.value = next.message[6];        // (unit.status == 1);    // true=closed
      return true;

    } /* don't react to macro's for the app -- Tom @ 12/04/2021
    else if (next.cmd = Rec.Macro) {
      // = EV_UNITMACROCOMMANDO
      // example: On 50%: [69,0,NodeAddress,UnitAddress,6,1,0,50]
      //          Off   : [69,0,NodeAddress,UnitAddress,6,0,0,0]    -> don't touch dimmer value
      // [4] = event, [5] = state

      // STATUS
      if (((next.message[4] == 7) || (next.message[4] == 5)) && (next.message[5] == 1)) {
        // PIR ON -- override status value with "2" (our PIR ON)
        unit.status = 2;

      } else if ((next.message[4] < 8) || (next.message[4] == 10)) {
        // ON/OFF messages, including sensor types (off/heating/cooling)
        unit.status = next.message[5];
      }

      // VALUE
      // only change dim value when state = 1 (ON, PIR ON, DIM STOP) for Dimmers and IRTX + for any setpoint event
      if ((((next.message[4] === 1) || (next.message[4] === 6) || (next.message[4] === 7)) && (next.message[5] === 1)) || 
          (next.message[4] === 11)) {
        unit.value = next.message[6]*256 + next.message[7];
      }
      return true;
    } */
    return false;
  },

  receiveAudioStatus: function (unit: Unit, next: CommRecord) {
    // extended
    // 70,0=status,252=node,102=unit=,type audio=5,[5]reserved=0,[6]dest=0,
    //  [7]src of 0=255,[8]src of 1=255,[9]src of 2=255,[10]src of 3=255,
    //  [11]vol of 0=0,[12]vol of 1=14,[13]vol of 2=12,[14]vol of 3=0
    // 70,0,252,102,5,0,1,0,0,255,255,11,12,0,0
    // 70,0,252,101,5,0,0,255,255,255,255,10,0,0,0 -> off
    //
    // Basic
    // 23,0=status,61=node,0=unit,10=rs232,3=type,255=src,28=vol,
    //    255=disc,[9]255=track,[10]2=preset,[11]102=freq,[12]70=freq

    // basic audio only has 1 destination
    unit.destinationId = (next.cmd === Rec.AudioExtendedStatus) ? next.message[6] : 0;

    if ((unit.destinations) && (next.cmd === Rec.AudioExtendedStatus)) {
      // select destination (if found)
      unit.destination = unit.destinations.find(d => d.id == unit.destinationId);
      // desperate attempt
      if (!unit.destination && unit.destinations && unit.destinations.length) {
        unit.destination = unit.destinations[0];
        unit.destinationId = 0;
      }

      unit.destinations.forEach(d => {
        d.volume = next.message[d.id + 11];
        d.source = next.message[d.id + 7];
        // this.logger("received source " + next.message[d.id+7] + " for destination " + d.id + " with volume " + d.volume);
      });
      // this.logger("current destination = " + unit.destinationId);
    }

    // basic audio
    if ((unit.destinations) && (next.cmd === Rec.AudioStatus)) {
      unit.destination = unit.destinations[0];
      unit.destination.volume = next.message[7];
      unit.destination.source = next.message[6];
      unit.destination.disc = (next.message[8] != 255) ? next.message[8] + 1 : 0;
      unit.destination.track = (next.message[9] != 255) ? next.message[9] + 1 : 0;
      unit.destination.preset = (next.message[10] != 255) ? next.message[10] + 1 : 0;
      unit.destination.frequency = (next.message[11] != 255) ? next.message[11] + "." + two(next.message[12]) : "";
      if (unit.destination.basictype != next.message[5]) {
        // first status reception -> adjust type
        unit.destination.basictype = LimitBasicAudioType(next.message[5]);
        unit.destination.sources = BasicAudioSources[unit.destination.basictype];
        this.logger("**** changed audio type " + next.message[5] + " - " + unit.name + " = " + unit.getNumber());
      }
    }

    // reflect volume and state of current destination in the value of the unit
    if (unit.destination) {
      unit.value = unit.destination.volume;
      unit.status = (unit.destination.source == 255) ? 0 : 1;
    }

    // this.logger((next.cmd === Rec.AudioExtendedStatus) ? "receiveAudioExtendedStatus: " : "receiveAudioStatus: " + " - " +  unit.name + " = " + unit.getNumber());
  },

  makeDBInfo(res: Message): DBInfo {
    return {nrNodes: res[2]};
  },

  getNodeInfo(res: Message): NodeInfo {
    let name = this.getStr(res, 8);
    let offset = name.length;
    return {
      name: name,
      // index: res[2],               // should be == nodeInx
      nodeAddress: res[3],
      physicalAddress: this.makeLong(res, 4),
      nrUnits: res[offset + 9],
      type: res[offset + 10],
      // : res[offset + 11]
    }
  },

  getUnitInfo(res: Message): UnitInfo {
    let name = this.getStr(res, 6);
    let offset = name.length;

    return {
      name,
      // index: res[3],        // should be == unitInx
      nodeAddress: res[4],     // difference with res[2] ?
      unitAddress: res[5],
      type: res[offset + 7],
      // flags: res[offset + 8]
    };
  }
};