/**
 * Unit action definitions
 * Maps unit types to their available binding actions with function codes
 * Based on the Duotecno binding software UI specifications
 */

/**
 * Function codes for different unit types
 * These are the message codes sent in bindings (F[code] in binding strings)
 */
export enum FunctionCode {
  // Switch/Relay functions
  SWITCH_ON_OFF_STATUS = 0x00,      // On/Off (Status) - toggle based on current state
  SWITCH_PIR_ON = 0x01,              // Switch Pir On - activate via PIR sensor
  SWITCH_ON_OFF = 0x02,              // Switch On/Off - direct on/off choice
  SWITCH_OFF = 0x03,                 // Switch Off - always turn off
  SWITCH_ON = 0x04,                  // Switch On - always turn on
  
  // Dimmer functions
  DIM_ON_OFF_STATUS = 0x10,          // On/Off (Status) - toggle dimmer
  DIM_VALUE_OFF = 0x11,              // Dim Value/Off - set value or turn off
  DIM_PIR_ON = 0x12,                 // Dim Pir On - activate dimmer via PIR
  DIM_UP_DOWN = 0x13,                // Dim Up/Down - continuous dimming
  DIM_UP_DN_ON_OFF = 0x14,           // Up/Dn/On/Off - combined control
  DIM_UP = 0x15,                     // Dim Up - only brighten
  DIM_DOWN = 0x16,                   // Dim Down - only darken
  DIM_ON_OFF = 0x17,                 // Dim On/Off - simple on/off
  SET_DIM_OFF = 0x18,                // Set Dim Off - turn off
  SET_DIM_ON = 0x19,                 // Set Dim On - turn on
  SET_DIM_VALUE = 0x1A,              // Set Dim Value - set to specific level
  
  // Shutter/Rollerladen (Rolluik) functions
  SHUTTER_UP_DOWN = 0x20,            // Up/Down - toggle direction
  SHUTTER_STOP = 0x21,               // Stop - halt movement
  SHUTTER_UP = 0x22,                 // Up - move up
  SHUTTER_DOWN = 0x23,               // Down - move down
  SHUTTER_UP_STOP = 0x24,            // Up/Stop - up then stop
  SHUTTER_DOWN_STOP = 0x25,          // Down/Stop - down then stop
  
  // Temperature sensor functions
  TEMP_HEATING = 0x30,               // Heating - activate heating mode
  TEMP_COOLING = 0x31,               // Cooling - activate cooling mode
  
  // Generic/Other
  GENERIC_TRIGGER = 0xFF,            // Generic trigger
}

/**
 * Represents a single action that can be bound to a unit
 */
export interface UnitAction {
  code: FunctionCode;
  label: string;                     // Display name for UI
  description: string;               // Detailed description (Dutch from manual)
  requiresData: boolean;             // Does this action need additional data?
  dataType?: 'value' | 'time' | 'none';  // Type of data if required
  dataRange?: [number, number];      // Min/max values for data
}

/**
 * Input event types from control units
 */
export enum InputEventType {
  TOGGLE = 0x01,                     // EV_UNITCONTROLTOGGLE
  PULSE_TOGGLE = 0x02,               // EV_UNITCONTROLPULSTOGGLE
  PULSE = 0x03,                      // EV_UNITCONTROLPULS
  STATUS = 0x04,                     // EV_UNITCONTROLSTATUS
  DIM_STATUS = 0x05,                 // EV_UNITDIMSTATUS
  SWITCH_STATUS = 0x06,              // EV_UNITSWITCHSTATUS
  SENSOR_STATUS = 0x07,              // EV_UNITSENSORSTATUS
}

/**
 * Input event action (what type of event triggers the binding)
 */
export interface InputEvent {
  code: InputEventType;
  label: string;
  description: string;
}

/**
 * Available input events for control units
 */
export const INPUT_EVENTS: InputEvent[] = [
  {
    code: InputEventType.PULSE,
    label: 'Event Short Pulse',
    description: 'Korte puls op de ingang',
  },
  {
    code: InputEventType.TOGGLE,
    label: 'Event Toggle',
    description: 'Toggle event bij statusverandering',
  },
  {
    code: InputEventType.PULSE_TOGGLE,
    label: 'Event Pulse Toggle',
    description: 'Puls en toggle gecombineerd',
  },
];

/**
 * RELAY/SWITCH ACTIONS
 * Based on section 6.6.1 from the manual
 */
const RELAY_ACTIONS: UnitAction[] = [
  {
    code: FunctionCode.SWITCH_ON_OFF_STATUS,
    label: 'On/Off (Status)',
    description: 'De relais wordt geactiveerd of gedeactiveerd naargelang de vorige status.',
    requiresData: false,
  },
  {
    code: FunctionCode.SWITCH_PIR_ON,
    label: 'Switch Pir On',
    description: 'De relais wordt geactiveerd door een PIR. Hier kan de tijd ingesteld worden waarna de relais terug moet deactiveren.',
    requiresData: true,
    dataType: 'time',
    dataRange: [0, 255], // Time in seconds
  },
  {
    code: FunctionCode.SWITCH_ON_OFF,
    label: 'Switch On/Off',
    description: 'Hierbij kan On of Off gekozen worden. De keuze die men instelt zal na een druk op de toets altijd eerst uitgevoerd worden.',
    requiresData: false,
  },
  {
    code: FunctionCode.SWITCH_OFF,
    label: 'Switch Off',
    description: 'De relais zal altijd uitgeschakeld worden!',
    requiresData: false,
  },
  {
    code: FunctionCode.SWITCH_ON,
    label: 'Switch On',
    description: 'De relais zal altijd ingeschakeld worden!',
    requiresData: false,
  },
];

/**
 * DIMMER ACTIONS
 * Based on section 6.6.2 from the manual
 */
const DIMMER_ACTIONS: UnitAction[] = [
  {
    code: FunctionCode.DIM_ON_OFF_STATUS,
    label: 'On/Off (Status)',
    description: 'De dimmer wordt geactiveerd of gedeactiveerd naargelang de vorige status. Ook de laatste dim waarde zal hersteld worden bij het activeren.',
    requiresData: false,
  },
  {
    code: FunctionCode.DIM_VALUE_OFF,
    label: 'Dim Value/Off',
    description: 'De dimmer wordt geactiveerd op de hierin vooraf ingestelde waarde of gedeactiveerd.',
    requiresData: true,
    dataType: 'value',
    dataRange: [0, 100], // Percentage
  },
  {
    code: FunctionCode.DIM_PIR_ON,
    label: 'Dim Pir On',
    description: 'Bij een PIR actie gaat de dimmer aan op de vooraf ingestelde waarde voor een vooraf ingestelde tijd.',
    requiresData: true,
    dataType: 'value',
    dataRange: [0, 100],
  },
  {
    code: FunctionCode.DIM_UP_DOWN,
    label: 'Dim Up/Down',
    description: 'De dimmer zal opdimmen of neerdimmen. (alleen bij lang drukken)',
    requiresData: false,
  },
  {
    code: FunctionCode.DIM_UP_DN_ON_OFF,
    label: 'Up/Dn/On/Off',
    description: 'De dimmer zal het omgekeerde doen van de laatste bewerking.',
    requiresData: false,
  },
  {
    code: FunctionCode.DIM_UP,
    label: 'Dim Up',
    description: 'De dimmer zal altijd opdimmen',
    requiresData: false,
  },
  {
    code: FunctionCode.DIM_DOWN,
    label: 'Dim Down',
    description: 'De dimmer zal altijd neerdimmen',
    requiresData: false,
  },
  {
    code: FunctionCode.DIM_ON_OFF,
    label: 'Dim On/Off',
    description: 'Hierbij kan On/Off gekozen worden. De keuze die men instelt zal na een druk op de toets altijd eerst uitgevoerd worden.',
    requiresData: false,
  },
  {
    code: FunctionCode.SET_DIM_OFF,
    label: 'Set Dim Off',
    description: 'De dimmer zal altijd uitgeschakeld worden.',
    requiresData: false,
  },
  {
    code: FunctionCode.SET_DIM_ON,
    label: 'Set Dim On',
    description: 'De dimmer zal altijd ingeschakeld worden.',
    requiresData: false,
  },
  {
    code: FunctionCode.SET_DIM_VALUE,
    label: 'Set Dim Value',
    description: 'De dimmer zal altijd gestart worden op de vooraf ingestelde waarde.',
    requiresData: true,
    dataType: 'value',
    dataRange: [0, 100],
  },
];

/**
 * SHUTTER/ROLLUIK ACTIONS
 * Based on section 6.6.4 from the manual
 */
const SHUTTER_ACTIONS: UnitAction[] = [
  {
    code: FunctionCode.SHUTTER_UP_DOWN,
    label: 'Up / Down',
    description: 'Het rolluik zal op of neergaan naargelang de laatste actie.',
    requiresData: false,
  },
  {
    code: FunctionCode.SHUTTER_STOP,
    label: 'Stop',
    description: 'Het rolluik zal stoppen',
    requiresData: false,
  },
  {
    code: FunctionCode.SHUTTER_UP,
    label: 'Up',
    description: 'Het rolluik zal opgaan',
    requiresData: false,
  },
  {
    code: FunctionCode.SHUTTER_DOWN,
    label: 'Down',
    description: 'Het rolluik zal neergaan',
    requiresData: false,
  },
  {
    code: FunctionCode.SHUTTER_UP_STOP,
    label: 'Up / Stop',
    description: 'Hier kan men kiezen wat eerst moet gebeuren: stoppen of omhoog gaan. Deze keuze zal altijd eerst gebeuren, daarna de andere.',
    requiresData: false,
  },
  {
    code: FunctionCode.SHUTTER_DOWN_STOP,
    label: 'Down / Stop',
    description: 'Hier kan men kiezen wat eerst moet gebeuren: stoppen of omhoog gaan. Deze keuze zal altijd eerst gebeuren, daarna de andere.',
    requiresData: false,
  },
];

/**
 * TEMPERATURE SENSOR ACTIONS
 * Based on section 6.6.5 from the manual
 */
const TEMPERATURE_ACTIONS: UnitAction[] = [
  {
    code: FunctionCode.TEMP_HEATING,
    label: 'Heating',
    description: 'Elke sensor bevat deze functie. Wanneer op een bepaald ogenblik de temperatuur onder de gewenste waarde valt, zal deze sensor de verbindingen met de "Start Heating" functie activeren.',
    requiresData: false,
  },
  {
    code: FunctionCode.TEMP_COOLING,
    label: 'Cooling',
    description: 'Hier geldt dezelfde logica maar voor gemeten waarden die Hoger zijn dan de gewenste waarde. (rekening houdend met de ingestelde Hysteresis).',
    requiresData: false,
  },
];

/**
 * Map unit types to their available actions
 */
export const UNIT_ACTIONS_MAP: Record<string, UnitAction[]> = {
  // Relay/Switch units
  'Switch': RELAY_ACTIONS,
  'Relais': RELAY_ACTIONS,
  'SWITCH': RELAY_ACTIONS,
  
  // Dimmer units
  'Dimmer': DIMMER_ACTIONS,
  'DIM': DIMMER_ACTIONS,
  'DIMMER': DIMMER_ACTIONS,
  
  // Shutter/Blind units
  'DuoSwitch': SHUTTER_ACTIONS,
  'Rolluik': SHUTTER_ACTIONS,
  'Shutter': SHUTTER_ACTIONS,
  'Blind': SHUTTER_ACTIONS,
  'DUOSWITCH': SHUTTER_ACTIONS,
  
  // Temperature sensor units
  'Temperature': TEMPERATURE_ACTIONS,
  'Temp': TEMPERATURE_ACTIONS,
  'Sensor': TEMPERATURE_ACTIONS,
  'SENS': TEMPERATURE_ACTIONS,
};

/**
 * Get available actions for a specific unit type
 */
export function getActionsForUnitType(unitType: string): UnitAction[] {
  // Normalize unit type string
  const normalizedType = unitType.trim();
  
  // Direct lookup
  if (UNIT_ACTIONS_MAP[normalizedType]) {
    return UNIT_ACTIONS_MAP[normalizedType];
  }
  
  // Case-insensitive lookup
  const key = Object.keys(UNIT_ACTIONS_MAP).find(
    k => k.toLowerCase() === normalizedType.toLowerCase()
  );
  
  if (key) {
    return UNIT_ACTIONS_MAP[key];
  }
  
  // Default: return empty array
  console.warn(`No actions defined for unit type: ${unitType}`);
  return [];
}

/**
 * Get action by function code
 */
export function getActionByCode(code: FunctionCode): UnitAction | undefined {
  const allActions = [
    ...RELAY_ACTIONS,
    ...DIMMER_ACTIONS,
    ...SHUTTER_ACTIONS,
    ...TEMPERATURE_ACTIONS,
  ];
  
  return allActions.find(action => action.code === code);
}

/**
 * Check if a unit type supports bindings as an output
 */
export function canBeBindingOutput(unitType: string): boolean {
  return getActionsForUnitType(unitType).length > 0;
}

/**
 * Check if a unit type supports bindings as an input (control unit)
 */
export function canBeBindingInput(unitType: string): boolean {
  const inputTypes = ['Control', 'Input', 'CONTROL', 'Switch', 'Button'];
  return inputTypes.some(type => 
    unitType.toLowerCase().includes(type.toLowerCase())
  );
}
