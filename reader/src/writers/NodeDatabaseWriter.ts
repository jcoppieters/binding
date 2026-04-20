/**
 * Writer for nodedatabase.txt
 * Handles updating unit and node names
 */

import { readFileSync, writeFileSync } from 'fs';

export class NodeDatabaseWriter {
  /**
   * Update a unit's name in nodedatabase.txt
   */
  static updateUnitName(
    filePath: string,
    nodeAddress: number,
    unitAddress: number,
    newName: string
  ): void {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    
    let currentNodeAddress: number | null = null;
    let currentUnitAddress: number | null = null;
    let inTargetUnit = false;
    
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i].trim();
      
      // Track current node address
      if (trimmed.startsWith('bNodeAddress=')) {
        const value = trimmed.split('=')[1].replace(/;$/, '');
        currentNodeAddress = parseInt(value, 16);
      }
      
      // Track current unit - parse bAddress which is "XX;YY;" format
      if (trimmed.startsWith('bAddress=')) {
        const value = trimmed.split('=')[1].replace(/;$/, '');
        const addresses = value.split(';').filter(s => s.trim());
        if (addresses.length >= 2) {
          currentUnitAddress = parseInt(addresses[1], 16);
          inTargetUnit = 
            currentNodeAddress === nodeAddress && 
            currentUnitAddress === unitAddress;
        }
      }
      
      // Update name if we're in the target unit
      if (inTargetUnit && trimmed.startsWith('szName=')) {
        lines[i] = lines[i].replace(/szName=.*$/, `szName=${newName};`);
        inTargetUnit = false; // Only update first occurrence in this unit
        break;
      }
    }
    
    writeFileSync(filePath, lines.join('\n'), 'utf-8');
  }
  
  /**
   * Update a node's name in nodedatabase.txt
   */
  static updateNodeName(
    filePath: string,
    nodeAddress: number,
    newName: string
  ): void {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    
    let currentNodeAddress: number | null = null;
    let inTargetNode = false;
    let foundNodeAddress = false;
    
    for (let i = 0; i < lines.length; i++) {
      const trimmed = lines[i].trim();
      
      // Track current node address
      if (trimmed.startsWith('bNodeAddress=')) {
        const value = trimmed.split('=')[1].replace(/;$/, '');
        currentNodeAddress = parseInt(value, 16);
        foundNodeAddress = true;
        inTargetNode = currentNodeAddress === nodeAddress;
      }
      
      // Reset when we hit a new NODE= section
      if (trimmed.startsWith('NODE=')) {
        foundNodeAddress = false;
        inTargetNode = false;
      }
      
      // Update name if we're in the target node (before any units)
      if (inTargetNode && foundNodeAddress && trimmed.startsWith('szName=') && !trimmed.startsWith('UNIT=')) {
        lines[i] = lines[i].replace(/szName=.*$/, `szName=${newName};`);
        break;
      }
    }
    
    writeFileSync(filePath, lines.join('\n'), 'utf-8');
  }
}
