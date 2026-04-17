/**
 * Binding file format models
 * Based on specs/binding-files-format.md
 */

/**
 * Binding type indicators
 */
export enum BindingType {
  NORMAL = 'N',           // Normal binding
  IMMEDIATE = 'I',        // Immediate binding
  CONDITIONAL = 'C',      // Conditional binding
  GROUP = 'G',            // Group binding
  PROPERTY = 'P',         // Property binding
  BLOCK = 'B',            // Block binding
  TIMER_EXTENDED = 'Te',  // Timer types
  TIMER_INTERVAL = 'Ti',
  TIMER_ONESHOT = 'To',
  TIMER_DELAYED = 'Td',
  TIMER_FLASHING = 'Tf',
  TIMER_REPEAT = 'Tr',
}

/**
 * Event codes from control units
 */
export enum EventCode {
  UNIT_CONTROL_TOGGLE = 0x01,
  UNIT_CONTROL_PULS_TOGGLE = 0x02,
  UNIT_CONTROL_PULS = 0x03,
  UNIT_CONTROL_STATUS = 0x04,
  UNIT_DIM_STATUS = 0x05,
  UNIT_SWITCH_STATUS = 0x06,
  UNIT_SENS_STATUS = 0x07,
}

/**
 * Unit types (BINDING_UNIT_TYPE_*)
 */
export enum UnitType {
  DIM = 0x01,              // Dimmer unit
  SWITCH = 0x02,           // Switch/relay unit
  CONTROL = 0x03,          // Control unit (buttons/inputs)
  SENS = 0x04,             // Sensor unit
  AUDIO = 0x05,            // Audio unit
  LCD = 0x06,              // LCD (not used)
  LCD_VIRTUAL = 0x07,      // LCD virtual unit
  DUOSWITCH = 0x08,        // Dual switch (blinds/shutters)
  SELECTOR = 0x09,         // Selector (not used)
  BOSERS = 0x0A,           // Bose RS232
  AVMATRIX = 0x0B,         // AV Matrix
  IRTX = 0x0C,             // IR Transmitter
  RC5RX = 0x0D,            // RC5 Receiver
  VIDEOMUX = 0x0E,         // Video Multiplexer
  ALARM_GALAXY = 0x12,     // Galaxy Alarm
  AUDIO_HC4 = 0x85,        // HC4 Audio specific
  BOSELF48 = 0x90,         // Bose LF48 specific
  BOSEV35 = 0x91,          // Bose V35 specific
}

/**
 * Conditional operators
 */
export enum BindingOperator {
  OR = '+',
  AND = '*',
  XOR = '^',
  NOT = '!',
}

/**
 * Unit reference in a binding
 * Format: A[NodeAddress]U[UnitAddress]E[Event]
 */
export interface UnitReference {
  nodeAddress: number;     // Node address (hex)
  unitAddress: number;     // Unit address (hex)
  event?: number;          // Event code (hex)
  eventChar?: string;      // Additional event character
}

/**
 * Parsed binding entry from bind*.txt file
 * Format: [NodeAddr]_[Flags]_[BindingNr]_[Type][Content]
 */
export interface BindingEntry {
  nodeAddress: number;     // Target node address (6 hex chars, last 2 significant)
  bindingFlags: number;    // Binding flags (2 hex chars)
  bindingNumber: number;   // Binding number (4 hex chars)
  bindingType: BindingType; // Type of binding
  content: string;         // Raw content after type indicator
  
  // Parsed content (depends on binding type)
  inputUnits?: UnitReference[];
  outputUnits?: UnitReference[];
  operators?: BindingOperator[];
  expression?: string;
  functionCode?: number;
  functionData?: number[];
}

/**
 * Complete binding file for a node
 */
export interface BindingFile {
  nodeAddress: number;
  fileName: string;
  entries: BindingEntry[];
  errorCount: number;
  errors: string[];
}
