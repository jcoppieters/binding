// Johan Coppieters.
//
// v1 - server version Apr 2019
// v2 - app version
// v3.1 - brought in line with server version
// v4 - for the pro app - removed scenes, added config of moods
//    - changed all "logicalAddresses" to nodeAddress and unitAddress
//    - removed extended types
// v5 - Jul 2021: added ip master API types and conversion

import {buildNumber, version} from "src/version";

export const kDebug = false;
export const kVersion = version;
export const kBuild = buildNumber;

// for Active=Y/N or New=X
export type YNX = 'Y' | 'N' | 'X';
export type YN = 'Y' | 'N';

// Add missing Timer type
export type Timer = ReturnType<typeof setTimeout>;

// coming from app or from unit tests
export type LogFunction = (message: any, ...optionalParams: any[]) => void;

// Node types
export enum NodeType { kNoNode = 0, kStandardNode = 1, kGatewayNode = 4, kModemNode = 8, kGUINode = 32 }

// Unit types and states
export enum UnitState { kOpening = 4, kClosing = 3, kOpen = 2, kClosed = 1, kStopped = 0 }

export enum UnitMotorCmd { kClose = 5, kOpen = 4, kStop = 3}

export const kVirtualNodeAddress = 255;

export enum ScreenType {
  kLight = 0, kSwitch = 1, kMood = 2, kTemperature = 3, kUpDown = 4, kAudio = 5, kRegister = 6
}

// kLightbulb  == kSwitch with no "*" or "$" in the name
// kDoor == kSwitchingMotor with "*" in the name
// kGarageDoor == kSwitchingMotor with "$" in the name
// kCondition  == kMood with "*" in the name
// kLock == kMood with $ in the name
// kUnlocker = kMood with $ in the name


////////////////////////////////////////////////////
// from Protocol//Hardware to DB, Nodes and Units //
////////////////////////////////////////////////////
export interface DBInfo {
  nrNodes: number;
}

export interface NodeInfo {
  name?: string;
  physicalAddress: number;
  nodeAddress: number;
  nrUnits: number;
  type: NodeType;
  flags?: number;
}


// handlers from incoming and outgoing messages
export type Message = Array<number>;

export enum WriteError { writeFatal = -1, writeError = 0, writeOK = 1 }

export interface CommRecord {
  isStatus: boolean;
  cmd: number;
  message: Message;
  rest: string;
  raw: string;
}

export const kEmptyCommRecord = { status: false, cmd: -1, message: [-1, 0, 0], rest: ''};
