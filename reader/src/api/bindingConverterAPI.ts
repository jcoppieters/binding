/**
 * Binding Converter API
 * 
 * Converts parsed legacy bind*.txt bindings to visual binding format
 */

import { Router } from 'express';
import { readFile } from 'fs/promises';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { BindingType } from '../models/binding.js';
import { LEGACY_EVENT_TO_PORT, getLegacyFunctionPort, unitTypeInfo } from '../models/unitTypes.js';

const router = Router();

const __dirname = dirname(fileURLToPath(import.meta.url));
const MODULES_DIR = join(__dirname, '../../modules');

/**
 * Models belonging to a field-switch family (module category 'switch', e.g.
 * DTBS-*) — their buttons/sensor must be grouped into ONE Home View device
 * per node, not one device per button. Cached after first read.
 */
let _switchModelsCache: Set<string> | null = null;
async function loadSwitchModels(): Promise<Set<string>> {
  if (_switchModelsCache) return _switchModelsCache;
  const models = new Set<string>();
  try {
    const raw = await readFile(join(MODULES_DIR, '_index.json'), 'utf-8');
    for (const family of JSON.parse(raw)) {
      if (family.category !== 'switch') continue;
      if (Array.isArray(family.variants)) {
        for (const v of family.variants) models.add(v.model);
      } else if (family.model) {
        models.add(family.model);
      }
    }
  } catch (err) {
    console.error('[converter] Failed to load module database for switch grouping:', err);
  }
  _switchModelsCache = models;
  return models;
}

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
router.post('/convert', async (req, res): Promise<void> => {
  try {
    const { bindings, project } = req.body;

    if (!bindings || !Array.isArray(bindings)) {
      res.status(400).json({ error: 'Invalid request: bindings array required' });
      return;
    }

    if (!project) {
      res.status(400).json({ error: 'Invalid request: project required' });
      return;
    }

    const visualBindings: any[] = [];
    const devicesToCreate: any[] = [];
    const moodsToCreate: any[] = [];
    const warnings: string[] = [];

    // Build lookup maps
    const railDeviceMap = buildRailDeviceMap(project);
    const homeDeviceMap = buildHomeDeviceMap(project);

    // Group field-switch (DTBS-*) units by node — one Home View device per
    // physical switch, not one per button.
    const switchModels = await loadSwitchModels();
    const switchNodeUnits = new Map<number, any[]>();
    for (const railUnit of railDeviceMap.values()) {
      if (!switchModels.has(railUnit.model)) continue;
      if (!switchNodeUnits.has(railUnit.nodeAddress)) switchNodeUnits.set(railUnit.nodeAddress, []);
      switchNodeUnits.get(railUnit.nodeAddress)!.push(railUnit);
    }
    const switchDeviceCache = new Map<number, any>();

    console.log(`[converter] Converting ${bindings.length} bindings...`);
    console.log(`[converter] Rail devices: ${railDeviceMap.size}, Home devices: ${homeDeviceMap.size}, Switch nodes: ${switchNodeUnits.size}`);

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

        // Special handling for mood trigger bindings (function 0xF9F)
        // The output references LCD unit, but targetMoodUnit contains the actual mood
        let actualOutput = output;
        if ((output as any).targetMoodUnit !== undefined) {
          const targetMood = (output as any).targetMoodUnit;
          console.log(`[converter] Mood trigger detected: LCD ${output.nodeAddress}-${output.unitAddress} -> actual mood ${output.nodeAddress}-${targetMood}`);
          actualOutput = {
            nodeAddress: output.nodeAddress,
            unitAddress: targetMood,
            event: 0x03, // Use 'puls' event for mood triggers
          };
        }

        // Find or create input device (controller)
        const inputKey = `${input.nodeAddress}-${input.unitAddress}`;
        const inputDevice = resolveOrCreateDevice(
          inputKey, input.nodeAddress, 'input', binding, warnings,
          homeDeviceMap, railDeviceMap, switchNodeUnits, switchDeviceCache, devicesToCreate, moodsToCreate
        );
        if (!inputDevice) continue;

        // Find or create output device (controllable)
        const outputKey = `${actualOutput.nodeAddress}-${actualOutput.unitAddress}`;
        const outputDevice = resolveOrCreateDevice(
          outputKey, actualOutput.nodeAddress, 'output', binding, warnings,
          homeDeviceMap, railDeviceMap, switchNodeUnits, switchDeviceCache, devicesToCreate, moodsToCreate
        );
        if (!outputDevice) continue;

        // Map event code to port
        const inputPortId = LEGACY_EVENT_TO_PORT[input.event || 0x03] || 'kort';

        // Map function code to port (extract from output content)
        // Use device-category-aware mapping for moods vs switches/dimmers/motors
        const functionCode = extractFunctionCode(binding.content);
        const outputPortId = getLegacyFunctionPort(functionCode, outputDevice.type);

        // from/to are always addressed by channelRef (nodeAddress+unitAddress) —
        // deviceId is just a cache of which RoomDevice currently represents it.
        const fromBinding = {
          channelRef: { nodeAddress: input.nodeAddress, unitAddress: input.unitAddress },
          portId: inputPortId,
          deviceId: inputDevice.id,
        };

        const toBinding = {
          channelRef: { nodeAddress: actualOutput.nodeAddress, unitAddress: actualOutput.unitAddress },
          portId: outputPortId,
          deviceId: outputDevice.id,
        };

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
 * Build a map of hardware channels by node-unit key, sourced entirely from
 * project.nodes[] (the single source of truth for node/unit data).
 */
function buildRailDeviceMap(project: any): Map<string, any> {
  const map = new Map();

  for (const node of (project.nodes || [])) {
    for (const unit of (node.units || [])) {
      const key = `${node.nodeAddress}-${unit.unitAddress}`;
      const info = unitTypeInfo(unit.type);
      map.set(key, {
        nodeAddress: node.nodeAddress,
        unitAddress: unit.unitAddress,
        name: unit.name || node.name,
        model: node.model,
        unitType: unit.type,
        category: info.category, // 'dimmer' | 'relay' | 'motor' | 'input' | 'sensor' | 'audio' | 'mood' | null
      });
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
      if (device.channelRef) {
        const key = `${device.channelRef.nodeAddress}-${device.channelRef.unitAddress}`;
        map.set(key, device);
      }

      // Multi-unit devices: each button/sensor shares the device's nodeAddress
      for (const button of (device.buttons || [])) {
        const key = `${device.nodeAddress}-${button.unitAddress}`;
        map.set(key, { ...device, matchedButton: button });
      }
      for (const sensor of (device.sensors || [])) {
        const key = `${device.nodeAddress}-${sensor.unitAddress}`;
        map.set(key, { ...device, matchedSensor: sensor });
      }
    }
  }

  // All moods (unit type 7) are available as binding targets even before
  // being placed in a room — sourced directly from project.nodes[].
  for (const node of (project.nodes || [])) {
    for (const unit of (node.units || [])) {
      if (unitTypeInfo(unit.type).category !== 'mood') continue;
      const key = `${node.nodeAddress}-${unit.unitAddress}`;
      if (map.has(key)) continue; // room-placed mood takes precedence
      map.set(key, {
        id: `mood-${node.nodeAddress}-${unit.unitAddress}`,
        name: unit.name || `Mood ${unit.unitAddress}`,
        icon: '🎭',
        color: '#ec4899',
        type: 'mood',
        channelRef: { nodeAddress: node.nodeAddress, unitAddress: unit.unitAddress },
      });
    }
  }

  return map;
}

/**
 * Resolve the Home View device for a node-unit key, creating it if needed.
 * Field-switch units (DTBS-* family) are grouped into ONE multi-button
 * device per node instead of one device per button/sensor.
 */
function resolveOrCreateDevice(
  key: string,
  nodeAddress: number,
  role: 'input' | 'output',
  binding: any,
  warnings: string[],
  homeDeviceMap: Map<string, any>,
  railDeviceMap: Map<string, any>,
  switchNodeUnits: Map<number, any[]>,
  switchDeviceCache: Map<number, any>,
  devicesToCreate: any[],
  moodsToCreate: any[]
): any | null {
  const existing = homeDeviceMap.get(key);
  if (existing) return existing;

  const switchUnits = switchNodeUnits.get(nodeAddress);
  if (switchUnits) {
    let switchDevice = switchDeviceCache.get(nodeAddress);
    if (!switchDevice) {
      switchDevice = createSwitchDeviceFromUnits(nodeAddress, switchUnits);
      switchDeviceCache.set(nodeAddress, switchDevice);
      devicesToCreate.push(switchDevice);
      // Register this one device for every unit key of the node so later
      // bindings referencing other buttons/sensors reuse the same device.
      for (const u of switchUnits) {
        homeDeviceMap.set(`${nodeAddress}-${u.unitAddress}`, switchDevice);
      }
    }
    return switchDevice;
  }

  const railDevice = railDeviceMap.get(key);
  if (!railDevice) {
    const unitAddress = key.split('-')[1];
    warnings.push(`Binding ${binding.bindingNumber}: ${role === 'input' ? 'Input' : 'Output'} device not found (node 0x${nodeAddress.toString(16)}, unit ${unitAddress})`);
    return null;
  }

  const device = createDeviceFromRail(railDevice);
  if (device.type === 'mood') {
    moodsToCreate.push(device);
  } else {
    devicesToCreate.push(device);
  }
  homeDeviceMap.set(key, device);
  return device;
}

/**
 * Create a grouped multi-button Home View device from all units of one
 * field-switch node (buttons + optional temp sensor).
 */
function createSwitchDeviceFromUnits(nodeAddress: number, units: any[]): any {
  const buttons = units.filter(u => u.category === 'input').map(u => ({ unitAddress: u.unitAddress, label: u.name }));
  const sensors = units.filter(u => u.category === 'sensor').map(u => ({ unitAddress: u.unitAddress, label: u.name }));

  return {
    id: `imported-switch-${nodeAddress}-${Date.now()}`,
    type: 'input',
    name: units[0]?.model || `Node 0x${nodeAddress.toString(16).toUpperCase()}`,
    isMultiUnit: true,
    nodeAddress,
    buttonCount: buttons.length,
    hasTempSensor: sensors.length > 0,
    buttons,
    sensors,
    activeButton: 0,
    activeSensor: false,
    icon: sensors.length ? '🔳 🌡️' : '🔳',
    color: '#a78bfa',
    x: 0,
    y: 0,
    imported: true,
  };
}

/**
 * Create a Home View device definition from a hardware channel (project.nodes[] entry)
 */
function createDeviceFromRail(railDevice: any): any {
  const deviceId = `imported-${railDevice.nodeAddress}-${railDevice.unitAddress}-${Date.now()}`;

  const category = railDevice.category ?? 'relay';
  const ICONS: Record<string, string> = {
    dimmer: '💡', relay: '⚡', motor: '🪟', input: '🔘',
    sensor: '🌡️', audio: '🔊', mood: '🎭',
  };
  const COLORS: Record<string, string> = {
    dimmer: '#fbbf24', relay: '#60a5fa', motor: '#34d399', input: '#a78bfa',
    sensor: '#f59e0b', audio: '#06b6d4', mood: '#ec4899',
  };
  const deviceType = category;
  const icon = ICONS[category] || '💡';
  const color = COLORS[category] || '#fbbf24';

  const deviceName = railDevice.name || `${railDevice.model || 'Device'} U${railDevice.unitAddress}`;

  return {
    id: deviceId,
    type: deviceType,
    name: deviceName,
    icon,
    color,
    x: 0, // Will be positioned by frontend
    y: 0,
    channelRef: { nodeAddress: railDevice.nodeAddress, unitAddress: railDevice.unitAddress },
    imported: true
  };
}

/**
 * Extract function code from output unit
 * Format: A0000XXUxxFxxxDxxxxS where Fxxx is the function code
 */
function extractFunctionCode(content: string): number {
  // Try to parse from content string
  const match = content.match(/F([0-9A-F]{2,3})/i);
  if (match) {
    return parseInt(match[1], 16);
  }
  
  // Default to toggle
  return 0xFA6;
}

export default router;
