/**
 * Binding Converter API
 * 
 * Converts parsed legacy bind*.txt bindings to visual binding format
 */

import { Router } from 'express';
import { BindingType } from '../models/binding.js';

const router = Router();

/**
 * Event code to port mapping (for input devices - controllers)
 */
const EVENT_TO_PORT: Record<number, string> = {
  0x01: 'kort',   // Short press
  0x02: 'lang',   // Long press
  0x03: 'puls',   // Pulse/toggle (used for simple toggle bindings)
  0x04: 'status'  // Status request
};

/**
 * Function code to port mapping (for output devices - controllables)
 * Based on bindingEditorAPI.ts and hardware specs
 */
const FUNCTION_TO_PORT: Record<number, string> = {
  0xFA6: 'schakel',  // Toggle
  0xFA2: 'aan',      // Turn on
  0xFA4: 'uit',      // Turn off
  0xFB6: 'op',       // Motor up / dim up
  0xFB4: 'neer',     // Motor down / dim down
  0xF9F: 'trigger',  // Scene/Mood trigger
};

/**
 * POST /convert
 * 
 * Convert parsed bindings to visual binding format
 * 
 * Body: {
 *   bindings: Array<ParsedBinding>,
 *   project: Project (with railView for device lookup)
 * }
 * 
 * Response: {
 *   visualBindings: Array<VisualBinding>,
 *   devicesToCreate: Array<DeviceDefinition>,
 *   warnings: Array<string>
 * }
 */
router.post('/convert', async (req, res) => {
  try {
    const { bindings, project } = req.body;

    if (!bindings || !Array.isArray(bindings)) {
      return res.status(400).json({ error: 'Invalid request: bindings array required' });
    }

    if (!project) {
      return res.status(400).json({ error: 'Invalid request: project required' });
    }

    const visualBindings = [];
    const devicesToCreate = [];
    const moodsToCreate = [];
    const warnings = [];

    // Build lookup maps
    const railDeviceMap = buildRailDeviceMap(project);
    const homeDeviceMap = buildHomeDeviceMap(project);

    console.log(`[converter] Converting ${bindings.length} bindings...`);
    console.log(`[converter] Rail devices: ${railDeviceMap.size}, Home devices: ${homeDeviceMap.size}`);

    // Convert each simple binding
    for (const binding of bindings) {
      try {
        // Only process Type I (Immediate) and Type N (Normal)
        if (binding.bindingType !== BindingType.IMMEDIATE && binding.bindingType !== BindingType.NORMAL) {
          continue; // Skip complex bindings
        }

        if (!binding.inputUnits || binding.inputUnits.length === 0) {
          warnings.push(`Binding ${binding.bindingNumber}: No input unit defined`);
          continue;
        }

        if (!binding.outputUnits || binding.outputUnits.length === 0) {
          warnings.push(`Binding ${binding.bindingNumber}: No output unit defined`);
          continue;
        }

        const input = binding.inputUnits[0];
        const output = binding.outputUnits[0];

        // Find or create input device (controller)
        const inputKey = `${input.nodeAddress}-${input.unitAddress}`;
        let inputDevice = homeDeviceMap.get(inputKey);
        
        if (!inputDevice) {
          const railDevice = railDeviceMap.get(inputKey);
          if (!railDevice) {
            warnings.push(`Binding ${binding.bindingNumber}: Input device not found (node 0x${input.nodeAddress.toString(16)}, unit ${input.unitAddress})`);
            continue;
          }
          
          // Create Home View device for this input
          inputDevice = createDeviceFromRail(railDevice, 'input');
          
          // Separate moods from regular devices
          if (inputDevice.type === 'mood') {
            moodsToCreate.push(inputDevice);
          } else {
            devicesToCreate.push(inputDevice);
          }
          
          homeDeviceMap.set(inputKey, inputDevice);
        }

        // Find or create output device (controllable)
        const outputKey = `${output.nodeAddress}-${output.unitAddress}`;
        let outputDevice = homeDeviceMap.get(outputKey);
        
        if (!outputDevice) {
          const railDevice = railDeviceMap.get(outputKey);
          if (!railDevice) {
            warnings.push(`Binding ${binding.bindingNumber}: Output device not found (node 0x${output.nodeAddress.toString(16)}, unit ${output.unitAddress})`);
            continue;
          }
          
          // Create Home View device for this output
          outputDevice = createDeviceFromRail(railDevice, 'output');
          
          // Separate moods from regular devices
          if (outputDevice.type === 'mood') {
            moodsToCreate.push(outputDevice);
          } else {
            devicesToCreate.push(outputDevice);
          }
          
          homeDeviceMap.set(outputKey, outputDevice);
        }

        // Map event code to port
        const inputPortId = EVENT_TO_PORT[input.event || 0x03] || 'puls';
        
        // Map function code to port (extract from output content)
        const functionCode = extractFunctionCode(output, binding.content);
        const outputPortId = FUNCTION_TO_PORT[functionCode] || 'schakel';

        // Build from/to with channelRef for multi-button switches
        const fromBinding: any = {
          deviceId: inputDevice.id,
          portId: inputPortId
        };
        
        // If input device is a multi-button switch, add channelRef to specify which button
        if (inputDevice.matchedButton || inputDevice.matchedSensor) {
          fromBinding.channelRef = {
            nodeAddress: input.nodeAddress,
            unitAddress: input.unitAddress,
            moduleInstanceId: inputDevice.channelRef?.moduleInstanceId || inputDevice.id
          };
        }
        
        const toBinding: any = {
          deviceId: outputDevice.id,
          portId: outputPortId
        };
        
        // If output device is multi-channel, add channelRef
        if (outputDevice.matchedButton || outputDevice.matchedSensor) {
          toBinding.channelRef = {
            nodeAddress: output.nodeAddress,
            unitAddress: output.unitAddress,
            moduleInstanceId: outputDevice.channelRef?.moduleInstanceId || outputDevice.id
          };
        }

        // Create visual binding
        const visualBinding = {
          id: `imported-${binding.nodeAddress}-${binding.bindingNumber}`,
          from: fromBinding,
          to: toBinding,
          color: '#3b82f6', // Default blue
          imported: true,
          sourceFile: binding.fileName,
          bindingNumber: binding.bindingNumber
        };

        visualBindings.push(visualBinding);

      } catch (err) {
        const error = err instanceof Error ? err : new Error(String(err));
        warnings.push(`Binding ${binding.bindingNumber}: ${error.message}`);
      }
    }

    console.log(`[converter] Converted ${visualBindings.length} bindings, ${devicesToCreate.length} devices to create, ${moodsToCreate.length} moods to create, ${warnings.length} warnings`);

    res.json({
      visualBindings,
      devicesToCreate,
      moodsToCreate,
      warnings
    });

  } catch (err) {
    console.error('[converter] Error:', err);
    const error = err instanceof Error ? err : new Error(String(err));
    res.status(500).json({ error: error.message || 'Conversion failed' });
  }
});

/**
 * Build a map of Rail View devices by node-unit key
 */
function buildRailDeviceMap(project: any): Map<string, any> {
  const map = new Map();

  // Add cabinet modules
  for (const cabinet of (project.railView?.cabinets || [])) {
    for (const module of (cabinet.modules || [])) {
      if (module.nodeAddress != null && module.channelGroups) {
        // For each channel group, create entries for individual units
        for (const group of module.channelGroups) {
          const count = group.count || 1;
          for (let i = 0; i < count; i++) {
            const unitAddress = (group.startUnit || 0) + i;
            const key = `${module.nodeAddress}-${unitAddress}`;
            map.set(key, {
              ...module,
              cabinetId: cabinet.id,
              channelType: group.type,
              channelIndex: i,
              unitAddress,
              location: 'cabinet'
            });
          }
        }
      }
    }
  }

  // Add woning devices
  for (const device of (project.railView?.woningDevices || [])) {
    if (device.nodeAddress != null && device.channelGroups) {
      for (const group of device.channelGroups) {
        const count = group.count || 1;
        for (let i = 0; i < count; i++) {
          const unitAddress = (group.startUnit || 0) + i;
          const key = `${device.nodeAddress}-${unitAddress}`;
          map.set(key, {
            ...device,
            channelType: group.type,
            channelIndex: i,
            unitAddress,
            location: 'woning'
          });
        }
      }
    }
  }

  // Fallback: Use discoveredNodes if channelGroups not yet configured
  // This happens when modules exist in Rail View but channels aren't defined yet
  for (const node of (project.discoveredNodes || [])) {
    if (node.units) {
      for (const unit of node.units) {
        const key = `${node.nodeAddress}-${unit.unitAddress}`;
        // Only add if not already in map (channelGroups take precedence)
        if (!map.has(key)) {
          // Map unit type to channel type
          // Type 3 = input_digital, Type 1/2 = relay/dimmer, Type 7 = mood, Type 8 = motor, Type 4 = temperature
          let channelType = 'unknown';
          if (unit.type === 3) channelType = 'input_digital';
          else if (unit.type === 1) channelType = 'dimmer_1ch';
          else if (unit.type === 2) channelType = 'relay_16a';
          else if (unit.type === 7) channelType = 'mood';
          else if (unit.type === 8) channelType = 'motor_updown';
          else if (unit.type === 4) channelType = 'temperature';
          
          map.set(key, {
            id: `discovered-${node.nodeAddress}-${unit.unitAddress}`,
            model: node.name || 'UNKNOWN',
            nodeAddress: node.nodeAddress,
            unitAddress: unit.unitAddress,
            name: unit.name || `Unit ${unit.unitAddress}`,
            channelType,
            unitType: unit.type,
            location: 'discovered'
          });
        }
      }
    }
  }

  return map;
}

/**
 * Build a map of Home View devices by node-unit key
 */
function buildHomeDeviceMap(project: any): Map<string, any> {
  const map = new Map();

  for (const room of (project.homeView?.rooms || [])) {
    for (const device of (room.devices || [])) {
      // Check primary channelRef
      if (device.channelRef) {
        const key = `${device.channelRef.nodeAddress}-${device.channelRef.unitAddress}`;
        map.set(key, device);
      }
      
      // Check multi-button switches (buttons array)
      if (device.buttons && Array.isArray(device.buttons)) {
        for (const button of device.buttons) {
          if (button.ref || button.channelRef) {
            const ref = button.ref || button.channelRef;
            const key = `${ref.nodeAddress}-${ref.unitAddress}`;
            // Store the device, but also store which button was matched
            map.set(key, {
              ...device,
              matchedButton: button,
              matchedButtonIndex: device.buttons.indexOf(button)
            });
          }
        }
      }
      
      // Check sensors array (for multi-button switches with temp sensors)
      if (device.sensors && Array.isArray(device.sensors)) {
        for (const sensor of device.sensors) {
          if (sensor.ref || sensor.channelRef) {
            const ref = sensor.ref || sensor.channelRef;
            const key = `${ref.nodeAddress}-${ref.unitAddress}`;
            map.set(key, {
              ...device,
              matchedSensor: sensor
            });
          }
        }
      }
    }
  }
  
  // Also check moods from homeView.moods array
  for (const mood of (project.homeView?.moods || [])) {
    if (mood.channelRef) {
      const key = `${mood.channelRef.nodeAddress}-${mood.channelRef.unitAddress}`;
      map.set(key, mood);
    }
  }

  return map;
}

/**
 * Create a Home View device definition from a Rail View device
 */
function createDeviceFromRail(railDevice: any, role: 'input' | 'output'): any {
  const deviceId = `imported-${railDevice.nodeAddress}-${railDevice.unitAddress}-${Date.now()}`;
  
  // Determine device type based on channel type and role
  let deviceType = 'lamp';
  let icon = '💡';
  let color = '#fbbf24';
  
  if (railDevice.channelType === 'input_digital') {
    deviceType = 'input';
    icon = '🔘';
    color = '#a78bfa';
  } else if (railDevice.channelType === 'input_analog' || railDevice.channelType === 'temperature') {
    deviceType = 'sensor';
    icon = '🌡️';
    color = '#f59e0b';
  } else if (railDevice.channelType?.startsWith('dimmer_')) {
    deviceType = 'dimmer';
    icon = '💡';
    color = '#fbbf24';
  } else if (railDevice.channelType?.startsWith('relay_')) {
    deviceType = 'relay';
    icon = '⚡';
    color = '#60a5fa';
  } else if (railDevice.channelType === 'motor_updown' || railDevice.channelType === 'motor_polar') {
    deviceType = 'motor';
    icon = '🪟';
    color = '#34d399';
  } else if (railDevice.channelType === 'mood') {
    deviceType = 'mood';
    icon = '🎭';
    color = '#ec4899';
  }

  // For discoveredNodes, use the unit name; otherwise use model + unit
  const deviceName = railDevice.name || `${railDevice.model || 'Device'} U${railDevice.unitAddress}`;

  return {
    id: deviceId,
    type: deviceType,
    name: deviceName,
    icon,
    color,
    x: 0, // Will be positioned by frontend
    y: 0,
    channelRef: {
      nodeAddress: railDevice.nodeAddress,
      unitAddress: railDevice.unitAddress,
      moduleInstanceId: railDevice.id,
      channelIndex: railDevice.channelIndex
    },
    imported: true
  };
}

/**
 * Extract function code from output unit
 * Format: A0000XXUxxFxxxDxxxxS where Fxxx is the function code
 */
function extractFunctionCode(output: any, content: string): number {
  // Try to parse from content string
  const match = content.match(/F([0-9A-F]{2,3})/i);
  if (match) {
    return parseInt(match[1], 16);
  }
  
  // Default to toggle
  return 0xFA6;
}

export default router;
