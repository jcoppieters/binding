/**
 * Unit management helpers
 *
 * Derives the list of "units" (channels) for each hardware node and tracks
 * their usage in the home view. `project.nodes[]` is the source of truth:
 * a node's `units[]` (filled in by discovery) always wins. Before discovery
 * has run, channels are instead derived from the module database
 * (channelGroups) so bindings can be planned ahead of real hardware.
 */

import { UnitType, unitTypeInfo } from './unit-types.js';

/**
 * List of channels for one node: discovered units if present, otherwise a
 * planned list derived from the module definition's channelGroups.
 * @param {object} node - DuoNode from project.nodes[]
 * @param {object} moduleDef - Module definition from the modules database (by node.model)
 * @param {Array<string|null>} slots - Smartbox+ plugin slots (cabinet placement only)
 * @param {object} modules - Full modules database (needed for Smartbox plugins)
 */
function unitsForNode(node, moduleDef, slots, modules) {
  if (node.units && node.units.length > 0) {
    // Moods (virtual channels) are never placeable as a room device — they're
    // surfaced directly from project.nodes[] in the binding panel instead.
    return node.units.filter(u => u.type !== UnitType.kMood).map(u => ({
      unitAddress: u.unitAddress,
      nodeAddress: node.nodeAddress,
      channelType: unitTypeToChannelType(u.type),
      unitType: u.type,
      label: u.name || generateDefaultLabel(unitTypeToChannelType(u.type), u.unitAddress + 1),
      icon: unitTypeInfo(u.type).icon,
    }));
  }

  if (!moduleDef?.channelGroups) return [];

  const units = [];
  let unitAddress = 0;
  for (const group of moduleDef.channelGroups) {
    for (let i = 0; i < group.count; i++) {
      units.push({
        unitAddress,
        nodeAddress: node.nodeAddress,
        channelType: group.type,
        unitType: channelTypeToUnitType(group.type),
        label: generateDefaultLabel(group.type, i + 1),
        icon: getIconForChannelType(group.type),
      });
      unitAddress++;
    }
  }

  // Smartbox+ plugin slots: each slot adds its own channel groups
  if (moduleDef.isSmartboxBase && slots && modules) {
    for (let slotIdx = 0; slotIdx < slots.length; slotIdx++) {
      const pluginModel = slots[slotIdx];
      if (!pluginModel) continue; // Empty slot
      const pluginDef = modules[pluginModel];
      if (!pluginDef?.channelGroups) continue;

      for (const group of pluginDef.channelGroups) {
        for (let i = 0; i < group.count; i++) {
          units.push({
            unitAddress,
            nodeAddress: node.nodeAddress,
            channelType: group.type,
            unitType: channelTypeToUnitType(group.type),
            label: `${generateDefaultLabel(group.type, slotIdx + 1)} (Slot ${slotIdx + 1})`,
            icon: getIconForChannelType(group.type),
          });
          unitAddress++;
        }
      }
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
  const nodesByAddress = new Map(project.nodes.map(n => [n.nodeAddress, n]));

  for (const cabinet of project.railView.cabinets) {
    for (const ref of cabinet.modules) {
      const node = nodesByAddress.get(ref.nodeAddress);
      if (!node) continue;
      const moduleDef = modules[node.model];
      const units = unitsForNode(node, moduleDef, ref.slots, modules).map(u => ({
        ...u,
        usage: null,
        moduleName: moduleDef?.name || node.model || node.name,
        moduleModel: node.model,
        cabinetName: cabinet.name,
      }));
      allUnits.push(...units);
    }
  }

  for (const ref of project.railView.house) {
    const node = nodesByAddress.get(ref.nodeAddress);
    if (!node) continue;
    const deviceDef = modules[node.model];
    const units = unitsForNode(node, deviceDef, null, modules).map(u => ({
      ...u,
      usage: null,
      moduleName: deviceDef?.name || node.model || node.name,
      moduleModel: node.model,
    }));
    allUnits.push(...units);
  }

  analyzeUnitUsage(allUnits, project);

  return allUnits;
}

/**
 * Analyze which units are used in rooms and mark them
 * @param {Array} units - Array of units to analyze
 * @param {object} project - DuoProject
 */
function analyzeUnitUsage(units, project) {
  const unitsByRef = new Map();
  for (const unit of units) {
    unitsByRef.set(`${unit.nodeAddress}:${unit.unitAddress}`, unit);
  }

  for (const room of project.homeView.rooms) {
    const floor = project.homeView.floors.find(f => f.id === room.floorId);

    for (const device of room.devices) {
      // Single-unit device: channelRef. Multi-unit device: buttons[]/sensors[] share device.nodeAddress.
      const refs = device.channelRef
        ? [device.channelRef]
        : [...(device.buttons || []), ...(device.sensors || [])].map(u => ({ nodeAddress: device.nodeAddress, unitAddress: u.unitAddress }));

      for (const ref of refs) {
        const unit = unitsByRef.get(`${ref.nodeAddress}:${ref.unitAddress}`);
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
  return { nodeAddress: unit.nodeAddress, unitAddress: unit.unitAddress };
}

/**
 * Group units into device groups for multi-button switches.
 * Returns array of device groups where each group represents a physical device (switch).
 * A node is always exactly one physical device, so `nodeAddress` alone identifies the group.
 * @param {Array} units - Array of units
 * @param {object} modules - Module database
 * @returns {Array} Device groups
 */
export function groupUnitsIntoDevices(units, modules) {
  const deviceGroups = [];
  const processedUnits = new Set();

  for (const unit of units) {
    if (processedUnits.has(unit)) continue;

    // Group any field switch (module category 'switch', e.g. DTBS-* family) into
    // a single physical device — a node is always exactly one such device, whether
    // it has 1 button (DTBS-ES1) or 8 + a temp sensor (DTBS-8/DTBS-ES4/...).
    const isMultiButtonSwitch = modules?.[unit.moduleModel]?.category === 'switch';

    if (isMultiButtonSwitch) {
      // Find all units from this node (a node is always one physical device) —
      // sourced from the FULL unit list so a partially-used device is still
      // recognised as one node and can be skipped as a whole below, rather
      // than showing up with only the still-unused buttons.
      const nodeUnits = units.filter(u => u.nodeAddress === unit.nodeAddress && !processedUnits.has(u));

      const buttons = nodeUnits.filter(u => u.channelType === 'input_digital');
      const sensors = nodeUnits.filter(u => u.channelType === 'temperature' || u.channelType === 'input_analog');

      // Skip the whole group if any of its units are already used
      if (nodeUnits.some(u => u.usage)) {
        nodeUnits.forEach(u => processedUnits.add(u));
        continue;
      }

      deviceGroups.push({
        type: 'multi-button-switch',
        moduleModel: unit.moduleModel,
        moduleName: unit.moduleName,
        nodeAddress: unit.nodeAddress,
        cabinetName: unit.cabinetName,
        buttonCount: buttons.length,
        hasTempSensor: sensors.length > 0,
        buttons,
        sensors,
        units: nodeUnits,
        // Display properties
        label: `${unit.moduleName} (${buttons.length} knoppen${sensors.length ? ' + sensor' : ''})`,
        icon: '🔳',
      });

      nodeUnits.forEach(u => processedUnits.add(u));
    } else {
      processedUnits.add(unit);
      if (unit.usage) continue; // Already placed in a room

      // Single button or other type - add as individual unit
      deviceGroups.push({
        type: 'single',
        unit,
        units: [unit],
      });
    }
  }

  return deviceGroups;
}
