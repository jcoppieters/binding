/**
 * Utility functions for resolving addresses to human-readable names
 */

import { DeviceNode } from '../models/node.js';
import { UnitReference } from '../models/binding.js';

/**
 * Find a node by its address
 */
export function findNodeByAddress(
  nodes: DeviceNode[],
  nodeAddress: number
): DeviceNode | undefined {
  return nodes.find(node => node.nodeAddress === nodeAddress);
}

/**
 * Find a unit within a node by unit address
 */
export function findUnitInNode(
  node: DeviceNode,
  unitAddress: number
): { name: string; type: string } | undefined {
  const unit = node.units.find(u => {
    if (u.address.length >= 2) {
      return u.address[1] === unitAddress;
    }
    return false;
  });
  
  if (unit) {
    return {
      name: unit.name,
      type: unit.unitType,
    };
  }
  
  return undefined;
}

/**
 * Resolve a unit reference to human-readable format
 */
export function resolveUnitReference(
  nodes: DeviceNode[],
  unitRef: UnitReference
): string {
  const node = findNodeByAddress(nodes, unitRef.nodeAddress);
  
  if (!node) {
    return `Unknown Node 0x${unitRef.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}:${unitRef.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`;
  }
  
  const unit = findUnitInNode(node, unitRef.unitAddress);
  
  if (!unit) {
    return `${node.name} -> Unknown Unit 0x${unitRef.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`;
  }
  
  let result = `${node.name} -> ${unit.name}`;
  
  if (unitRef.event !== undefined) {
    result += ` [Event: 0x${unitRef.event.toString(16).toUpperCase().padStart(2, '0')}]`;
  }
  
  return result;
}

/**
 * Get event name from event code
 */
export function getEventName(eventCode: number): string {
  const eventNames: Record<number, string> = {
    0x01: 'Toggle',
    0x02: 'Pulse Toggle',
    0x03: 'Pulse',
    0x04: 'Status',
    0x05: 'Dim Status',
    0x06: 'Switch Status',
    0x07: 'Sens Status',
  };
  
  return eventNames[eventCode] || `Event 0x${eventCode.toString(16).toUpperCase()}`;
}

/**
 * Format a binding connection in human-readable format
 */
export function formatBindingConnection(
  nodes: DeviceNode[],
  inputs: UnitReference[],
  outputs: UnitReference[],
  operators?: string[]
): string {
  const inputStrings = inputs.map(input => {
    const resolved = resolveUnitReference(nodes, input);
    const eventName = input.event !== undefined ? getEventName(input.event) : '';
    return eventName ? `${resolved} (${eventName})` : resolved;
  });
  
  const outputStrings = outputs.map(output => resolveUnitReference(nodes, output));
  
  let connectionStr = inputStrings.join(operators?.length ? ` ${operators[0]} ` : ' & ');
  connectionStr += ' => ' + outputStrings.join(', ');
  
  return connectionStr;
}
