/**
 * Runtime unit state models with type-specific properties
 * Based on doc/protocol.ts Unit class
 */

import { UnitType } from './binding.js';

/**
 * Temperature presets
 */
export enum TempPreset {
  Off = -1,
  Sun = 0,
  HalfSun = 1,
  Moon = 2,
  HalfMoon = 3,
}

/**
 * Motor/blind states
 */
export enum MotorState {
  Stopped = 0,
  StoppedDown = 1,
  StoppedUp = 2,
  BusyDown = 3,
  BusyUp = 4,
  Opening = 3,  // Alias for BusyUp
  Closing = 4,  // Alias for BusyDown
  Open = 2,     // Alias for StoppedUp
  Closed = 1,   // Alias for StoppedDown
}

/**
 * Sensor/HVAC states
 */
export enum SensorState {
  Idle = 0,
  Heating = 1,
  Cooling = 2,
}

/**
 * Audio destination with source selection
 */
export interface AudioDestination {
  id: number;
  nodeAddress: number;
  unitAddress: number;
  name: string;
  source: number;
  volume: number;
  sources: string[];
  // Basic audio specific fields
  basictype?: number;
  disc?: number;
  track?: number;
  preset?: number;
  frequency?: string;
}

/**
 * Base runtime state for all units
 */
export interface UnitRuntimeState {
  // Identity
  nodeAddress: number;
  unitAddress: number;
  name: string;
  displayName: string;
  type: UnitType;
  active: boolean;
  
  // Current state
  value: number | boolean;      // Current value (dimmer %, boolean state, temperature, etc.)
  status: number | boolean;     // Status (on/off state, motor position, sensor state)
  
  // Buffering for UI updates
  bufferValues?: boolean;
  bufferedValue?: number | boolean;
  bufferedStatus?: number | boolean;
  
  // Last request timestamp (for rate limiting)
  lastRequested?: number;
  
  // Type-specific properties
  dimmer?: DimmerState;
  temperature?: TemperatureState;
  motor?: MotorUnitState;
  audio?: AudioState;
  camera?: CameraState;
  register?: RegisterState;
}

/**
 * Dimmer-specific state
 */
export interface DimmerState {
  percentage: number;  // 0-99
  isOn: boolean;
}

/**
 * Temperature sensor state
 */
export interface TemperatureState {
  // Current temperature (x10 degrees Celsius)
  temp: number;
  
  // Preset configuration
  preset: TempPreset;
  sun: number;      // x10 degrees
  hsun: number;     // x10 degrees (half sun)
  moon: number;     // x10 degrees
  hmoon: number;    // x10 degrees (half moon)
  
  // HVAC extended properties
  workingmode?: number;
  fanspeed?: number;
  swingangle?: number;
  swingmode?: number;
  
  // Available choices for extended HVAC
  workingmodes?: { [key: string]: number };
  fanspeeds?: { [key: string]: number };
  swingangles?: { [key: string]: number };
  swingmodes?: { [key: string]: number };
}

/**
 * Motor/blind state
 */
export interface MotorUnitState {
  position: MotorState;
  isMoving: boolean;
}

/**
 * Audio unit state
 */
export interface AudioState {
  destinations: AudioDestination[];
  destinationId: number;       // Current destination index
  destinationNr: number;       // Current destination number
  destination?: AudioDestination;  // Current destination object
}

/**
 * Camera unit state
 */
export interface CameraState {
  url: string;
  user: string;
  pwd: string;
}

/**
 * Register unit state
 */
export interface RegisterState {
  decimals: number;
  divider: number;
  multiplier: number;
  icon: number;
  measure: string;
  register: number;
}

/**
 * Helper to create default runtime state for a unit
 */
export function createUnitRuntimeState(
  nodeAddress: number,
  unitAddress: number,
  name: string,
  type: UnitType
): UnitRuntimeState {
  const displayName = name.replace(/\$|\*|!/g, '') || `N${nodeAddress}-U${unitAddress}`;
  
  return {
    nodeAddress,
    unitAddress,
    name,
    displayName,
    type,
    active: true,
    value: false,
    status: false,
    lastRequested: 0,
  };
}

/**
 * Helper to get readable state string for a unit
 */
export function getUnitDisplayState(state: UnitRuntimeState): string {
  switch (state.type) {
    case UnitType.DIM:
      return state.status ? `on (${state.value}%)` : 'off';
      
    case UnitType.SWITCH:
    case UnitType.CONTROL:
      return state.status ? 'on' : 'off';
      
    case UnitType.SENS:
      if (state.temperature) {
        const tempC = state.temperature.temp / 10;
        return `${tempC.toFixed(1)}°C - preset: ${TempPreset[state.temperature.preset] || 'off'}`;
      }
      return 'unknown';
      
    case UnitType.DUOSWITCH:
      if (typeof state.status === 'number') {
        const motorState = state.status as MotorState;
        if (motorState === MotorState.Opening) return 'opening';
        if (motorState === MotorState.Closing) return 'closing';
        if (motorState === MotorState.Open) return 'open';
        if (motorState === MotorState.Closed) return 'closed';
        if (motorState === MotorState.Stopped) return 'stopped';
      }
      return 'unknown';
      
    case UnitType.AUDIO:
    case UnitType.AUDIO_HC4:
      if (state.audio?.destination) {
        return state.status ? `on (vol: ${state.audio.destination.volume})` : 'off';
      }
      return state.status ? 'on' : 'off';
      
    default:
      return typeof state.status !== 'undefined' ? state.status.toString() : 'unknown';
  }
}
