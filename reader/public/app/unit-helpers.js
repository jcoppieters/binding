/**
 * Unit management helpers
 * 
 * Generates virtual "units" (channels) from module definitions and tracks their usage.
 * Each unit represents a single controllable/controller channel (relay, dimmer, input, etc.).
 */

import { UnitType } from './unit-types.js';

/**
 * Generate all units from a module's channelGroups
 * @param {object} moduleInstance - ModuleInstance from project.railView.cabinets[].modules[]
 * @param {object} moduleDef - Module definition from modules database
 * @param {object} cabinet - Cabinet containing this module
 * @param {object} modules - Full modules database (needed for Smartbox plugins)
 * @returns {Array} Array of unit objects
 */
export function generateUnitsForModule(moduleInstance, moduleDef, cabinet, modules) {
  if (!moduleDef?.channelGroups) return [];
  
  const units = [];
  let unitAddress = 0; // Unit address increments across all channel groups
  
  // Base channel groups from module definition
  for (const group of moduleDef.channelGroups) {
    for (let i = 0; i < group.count; i++) {
      const unit = {
        // Identity
        unitAddress,
        moduleInstanceId: moduleInstance.id,
        cabinetId: cabinet.id,
        
        // Hardware reference
        nodeAddress: moduleInstance.nodeAddress,
        
        // Type and capabilities
        channelType: group.type, // e.g. 'dimmer_te', 'relay_no', 'motor_updown'
        unitType: channelTypeToUnitType(group.type),
        
        // Display
        label: generateDefaultLabel(group.type, i + 1),
        icon: getIconForChannelType(group.type),
        
        // Module context (for display in picker)
        moduleName: moduleDef.name || moduleDef.model,
        moduleModel: moduleDef.model,
        cabinetName: cabinet.name,
        
        // Usage tracking (filled in by analyzeUnitUsage)
        usage: null, // null = unused, or { roomId, floorId, deviceId, deviceName }
      };
      
      units.push(unit);
      unitAddress++;
    }
  }
  
  // Smartbox+ plugin slots: each slot adds its own channel groups
  if (moduleDef.isSmartboxBase && moduleInstance.slots && modules) {
    for (let slotIdx = 0; slotIdx < moduleInstance.slots.length; slotIdx++) {
      const pluginModel = moduleInstance.slots[slotIdx];
      if (!pluginModel) continue; // Empty slot
      
      const pluginDef = modules[pluginModel];
      if (!pluginDef?.channelGroups) continue;
      
      // Add units from this plugin
      for (const group of pluginDef.channelGroups) {
        for (let i = 0; i < group.count; i++) {
          const unit = {
            unitAddress,
            moduleInstanceId: moduleInstance.id,
            cabinetId: cabinet.id,
            nodeAddress: moduleInstance.nodeAddress,
            channelType: group.type,
            unitType: channelTypeToUnitType(group.type),
            label: `${generateDefaultLabel(group.type, slotIdx + 1)} (Slot ${slotIdx + 1})`,
            icon: getIconForChannelType(group.type),
            moduleName: `${moduleDef.name} + ${pluginDef.name}`,
            moduleModel: `${moduleDef.model} (${pluginModel})`,
            cabinetName: cabinet.name,
            usage: null,
          };
          
          units.push(unit);
          unitAddress++;
        }
      }
    }
  }
  
  return units;
}

/**
 * Generate units for woning devices (switches, LCD panels)
 */
export function generateUnitsForWoningDevice(device, deviceDef) {
  if (!deviceDef?.channelGroups) return [];
  
  const units = [];
  let unitAddress = 0;
  
  for (const group of deviceDef.channelGroups) {
    for (let i = 0; i < group.count; i++) {
      const unit = {
        unitAddress,
        woningDeviceId: device.id,
        nodeAddress: device.nodeAddress,
        channelType: group.type,
        unitType: channelTypeToUnitType(group.type),
        label: generateDefaultLabel(group.type, i + 1),
        icon: getIconForChannelType(group.type),
        moduleName: deviceDef.name || deviceDef.model,
        moduleModel: deviceDef.model,
        usage: null,
      };
      
      units.push(unit);
      unitAddress++;
    }
  }
  
  return units;
}

/**
 * Get all units from entire project with usage information
 * @param {object} project - DuoProject
 * @param {object} modules - Module database
 * @returns {Array} All units with usage tracking
 */
export function getAllUnitsWithUsage(project, modules) {
  const allUnits = [];
  
  // Generate units from cabinets
  for (const cabinet of project.railView.cabinets) {
    for (const module of cabinet.modules) {
      const moduleDef = modules[module.model];
      if (!moduleDef) continue;
      
      const units = generateUnitsForModule(module, moduleDef, cabinet, modules);
      allUnits.push(...units);
    }
  }
  
  // Generate units from woning devices
  for (const device of project.railView.woningDevices) {
    const deviceDef = modules[device.model];
    if (!deviceDef) continue;
    
    const units = generateUnitsForWoningDevice(device, deviceDef);
    allUnits.push(...units);
  }
  
  // Add discovered units that aren't generated from module definitions
  // (e.g., temperature sensors on Smartbox that are only visible via discovery)
  if (project.discoveredNodes) {
    for (const node of project.discoveredNodes) {
      if (!node.units || node.units.length === 0) continue;
      
      for (const discoveredUnit of node.units) {
        // Skip if this unit already exists (generated from module definition)
        const alreadyExists = allUnits.some(u => 
          u.nodeAddress === discoveredUnit.nodeAddress && 
          u.unitAddress === discoveredUnit.unitAddress
        );
        if (alreadyExists) continue;
        
        // Map unitType to channelType
        const channelType = unitTypeToChannelType(discoveredUnit.type);
        if (!channelType) continue; // Skip unknown types
        
        // Find module/woning device for this node
        let moduleContext = null;
        for (const cabinet of project.railView.cabinets) {
          const module = cabinet.modules.find(m => m.nodeAddress === node.nodeAddress);
          if (module) {
            const moduleDef = modules[module.model];
            moduleContext = {
              moduleInstanceId: module.id,
              cabinetId: cabinet.id,
              moduleName: moduleDef?.name || module.name || node.name,
              moduleModel: module.model,
              cabinetName: cabinet.name,
            };
            break;
          }
        }
        
        if (!moduleContext) {
          // Check woning devices
          const device = project.railView.woningDevices.find(d => d.nodeAddress === node.nodeAddress);
          if (device) {
            const deviceDef = modules[device.model];
            moduleContext = {
              woningDeviceId: device.id,
              moduleName: deviceDef?.name || device.name || node.name,
              moduleModel: device.model,
            };
          }
        }
        
        // Create unit from discovered data
        const unit = {
          unitAddress: discoveredUnit.unitAddress,
          nodeAddress: discoveredUnit.nodeAddress,
          channelType,
          unitType: discoveredUnit.type,
          label: discoveredUnit.name || `Unit ${discoveredUnit.unitAddress}`,
          icon: getIconForChannelType(channelType),
          usage: null,
          ...(moduleContext || {}),
        };
        
        allUnits.push(unit);
      }
    }
  }
  
  // Analyze usage from homeView rooms
  analyzeUnitUsage(allUnits, project);
  
  return allUnits;
}

/**
 * Analyze which units are used in rooms and mark them
 * @param {Array} units - Array of units to analyze
 * @param {object} project - DuoProject
 */
function analyzeUnitUsage(units, project) {
  // Build lookup maps for faster matching
  const unitsByRef = new Map();
  for (const unit of units) {
    const key = makeRefKey(unit);
    if (key) unitsByRef.set(key, unit);
  }
  
  // Scan all room devices
  for (const room of project.homeView.rooms) {
    const floor = project.homeView.floors.find(f => f.id === room.floorId);
    
    for (const device of room.devices) {
      if (!device.channelRef) continue; // Not linked to hardware
      
      const key = `${device.channelRef.nodeAddress}:${device.channelRef.unitAddress}`;
      const unit = unitsByRef.get(key);
      
      if (unit) {
        unit.usage = {
          roomId: room.id,
          roomName: room.name,
          floorId: room.floorId,
          floorName: floor?.name || 'Onbekend',
          deviceId: device.id,
          deviceName: device.name,
        };
      }
    }
  }
}

/**
 * Create a lookup key for a unit's hardware reference
 */
function makeRefKey(unit) {
  if (!unit.nodeAddress) return null;
  return `${unit.nodeAddress}:${unit.unitAddress}`;
}

/**
 * Map UnitType (protocol type) to channelType (binding system type)
 */
function unitTypeToChannelType(unitType) {
  const map = {
    [UnitType.kDimmer]: 'dimmer_le',  // Default to LE, will be refined by module def
    [UnitType.kSwitch]: 'relay_no',   // Default to NO
    [UnitType.kInput]: 'input_digital',
    [UnitType.kTemperature]: 'temperature',
    [UnitType.kSwitchingMotor]: 'motor_updown',
    [UnitType.kAudio]: 'audio',
    [UnitType.kExtendedAudio]: 'audio',
  };
  return map[unitType] || null;
}

/**
 * Get icon for channel type
 */
function getIconForChannelType(channelType) {
  const icons = {
    'dimmer_le': '💡',
    'dimmer_te': '💡',
    'dimmer_pwm': '💡',
    'dimmer_dc': '💡',
    'relay_no': '⚡',
    'relay_nc': '⚡',
    'relay_ssr': '⚡',
    'motor_updown': '🪟',
    'motor_polar': '🪟',
    'input_digital': '🔘',
    'input_analog': '🌡️',
    'temperature': '🌡️',  // Temperature sensors
    'dali': '✨',
    'dmx': '✨',
    'audio': '🔊',
  };
  
  return icons[channelType] || '📟';
}

/**
 * Map channel type to UnitType enum
 */
function channelTypeToUnitType(channelType) {
  const map = {
    'dimmer_le': UnitType.kDimmer,
    'dimmer_te': UnitType.kDimmer,
    'dimmer_pwm': UnitType.kDimmer,
    'dimmer_dc': UnitType.kDimmer,
    'relay_no': UnitType.kSwitch,
    'relay_nc': UnitType.kSwitch,
    'relay_ssr': UnitType.kSwitch,
    'motor_updown': UnitType.kSwitchingMotor,
    'motor_polar': UnitType.kSwitchingMotor,
    'input_digital': UnitType.kInput,
    'input_analog': UnitType.kInput,
    'temperature': UnitType.kTemperature,  // Temperature sensors
    'dali': UnitType.kNoType, // Virtual group
    'dmx': UnitType.kNoType,
    'audio': UnitType.kAudio,
  };
  
  return map[channelType] ?? UnitType.kNoType;
}

/**
 * Generate default label for a unit
 */
function generateDefaultLabel(channelType, index) {
  const labels = {
    'dimmer_le': 'Dimmer',
    'dimmer_te': 'Dimmer',
    'dimmer_pwm': 'Dimmer',
    'dimmer_dc': 'Dimmer',
    'relay_no': 'Relais',
    'relay_nc': 'Relais',
    'relay_ssr': 'Relais',
    'motor_updown': 'Motor',
    'motor_polar': 'Motor',
    'input_digital': 'Input',
    'input_analog': 'Sensor',
    'dali': 'DALI groep',
    'dmx': 'DMX kanaal',
    'audio': 'Audio zone',
  };
  
  return `${labels[channelType] || 'Kanaal'} ${index}`;
}

/**
 * Get icon for channel type
 */
function getIconForChannelType(channelType) {
  const icons = {
    'dimmer_le': '💡',
    'dimmer_te': '💡',
    'dimmer_pwm': '💡',
    'dimmer_dc': '💡',
    'relay_no': '⚡',
    'relay_nc': '⚡',
    'relay_ssr': '⚡',
    'motor_updown': '🪟',
    'motor_polar': '🪟',
    'input_digital': '🔘',
    'input_analog': '🌡️',
    'dali': '✨',
    'dmx': '✨',
    'audio': '🔊',
  };
  
  return icons[channelType] || '📟';
}

/**
 * Get available (unused) units filtered by type
 * @param {Array} allUnits - All units from getAllUnitsWithUsage()
 * @param {string} filterType - 'controller' or 'controllable' or null (all)
 * @returns {Array} Filtered unused units
 */
export function getAvailableUnits(allUnits, filterType = null) {
  let filtered = allUnits.filter(u => !u.usage); // Only unused
  
  if (filterType === 'controller') {
    // Controllers: inputs (buttons, switches, sensors)
    filtered = filtered.filter(u => 
      u.channelType.startsWith('input_') || 
      u.unitType === UnitType.kInput ||
      u.unitType === UnitType.kTemperature
    );
  } else if (filterType === 'controllable') {
    // Controllables: outputs (dimmers, relays, motors)
    filtered = filtered.filter(u => 
      u.unitType === UnitType.kDimmer ||
      u.unitType === UnitType.kSwitch ||
      u.unitType === UnitType.kSwitchingMotor ||
      u.channelType.startsWith('dimmer_') ||
      u.channelType.startsWith('relay_') ||
      u.channelType.startsWith('motor_')
    );
  }
  
  return filtered;
}

/**
 * Create a ChannelRef from a unit
 */
export function createChannelRef(unit) {
  return {
    nodeAddress: unit.nodeAddress,
    unitAddress: unit.unitAddress,
    moduleInstanceId: unit.moduleInstanceId || unit.woningDeviceId,
  };
}
