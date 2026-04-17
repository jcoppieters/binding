/**
 * Parser for nodedatabase.txt
 * Contains node and unit information
 */

import { readFileSync } from 'fs';
import { DeviceNode, Unit, NodeConfig } from '../models/node.js';

export class NodeDatabaseParser {
  /**
   * Parse nodedatabase.txt file
   */
  static parse(filePath: string): DeviceNode[] {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    
    const nodes: DeviceNode[] = [];
    let currentNode: Partial<DeviceNode> | null = null;
    let currentUnit: Partial<Unit> | null = null;
    
    for (const line of lines) {
      const trimmed = line.trim();
      
      if (!trimmed || trimmed.startsWith(';')) {
        continue; // Skip empty lines and comments
      }
      
      if (trimmed.startsWith('NODE=')) {
        // Save previous node if exists
        if (currentNode) {
          nodes.push(this.finalizeNode(currentNode));
        }
        
        currentNode = {
          index: parseInt(trimmed.split('=')[1]),
          units: [],
        };
        currentUnit = null;
      } else if (trimmed.startsWith('UNIT=')) {
        // Save previous unit if exists
        if (currentUnit && currentNode) {
          currentNode.units!.push(this.finalizeUnit(currentUnit));
        }
        
        currentUnit = {
          index: parseInt(trimmed.split('=')[1]),
        };
      } else if (currentNode && !currentUnit) {
        // Parse node properties
        this.parseNodeProperty(currentNode, trimmed);
      } else if (currentUnit) {
        // Parse unit properties
        this.parseUnitProperty(currentUnit, trimmed);
      }
    }
    
    // Save last node
    if (currentUnit && currentNode) {
      currentNode.units!.push(this.finalizeUnit(currentUnit));
    }
    if (currentNode) {
      nodes.push(this.finalizeNode(currentNode));
    }
    
    return nodes;
  }
  
  static parseNodeProperty(node: Partial<DeviceNode>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'szName':
        node.name = value.replace(/;$/, '');
        break;
      case 'ulPhysical':
        node.physicalAddress = parseInt(value, 16);
        break;
      case 'bNodeAddress':
        node.nodeAddress = parseInt(value, 16);
        break;
      case 'szNodeType':
        node.nodeType = value.replace(/;$/, '');
        break;
      case 'bNrOfUnits':
        node.numberOfUnits = parseInt(value);
        break;
      case 'bFlags':
        node.flags = parseInt(value, 16);
        break;
      case 'szSoftwareVersion':
        node.softwareVersion = value.replace(/;$/, '');
        break;
    }
  }
  
  static parseUnitProperty(unit: Partial<Unit>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'szName':
        unit.name = value.replace(/;$/, '');
        break;
      case 'bAddress':
        unit.address = value.replace(/;$/, '').split(';').map(s => parseInt(s, 16));
        break;
      case 'szUnitType':
        unit.unitType = value.replace(/;$/, '');
        break;
      case 'bConfig':
        unit.config = parseInt(value, 16);
        break;
    }
  }
  
  static finalizeNode(node: Partial<DeviceNode>): DeviceNode {
    return {
      index: node.index!,
      name: node.name || '',
      physicalAddress: node.physicalAddress || 0,
      nodeAddress: node.nodeAddress || 0,
      nodeType: node.nodeType || '',
      numberOfUnits: node.numberOfUnits || 0,
      flags: node.flags || 0,
      softwareVersion: node.softwareVersion || '',
      units: node.units || [],
    };
  }
  
  static finalizeUnit(unit: Partial<Unit>): Unit {
    return {
      index: unit.index!,
      name: unit.name || '',
      address: unit.address || [],
      unitType: unit.unitType || '',
      config: unit.config || 0,
    };
  }
}

export class NodeConfigParser {
  /**
   * Parse nodeconfiginfo.txt file
   */
  static parse(filePath: string): NodeConfig[] {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    
    const configs: NodeConfig[] = [];
    let currentConfig: Partial<NodeConfig> | null = null;
    
    for (const line of lines) {
      const trimmed = line.trim();
      
      if (!trimmed || trimmed.startsWith(';')) {
        continue;
      }
      
      if (trimmed.startsWith('NODECONFIG=')) {
        if (currentConfig) {
          configs.push(this.finalizeConfig(currentConfig));
        }
        
        currentConfig = {
          index: parseInt(trimmed.split('=')[1]),
        };
      } else if (currentConfig) {
        this.parseConfigProperty(currentConfig, trimmed);
      }
    }
    
    if (currentConfig) {
      configs.push(this.finalizeConfig(currentConfig));
    }
    
    return configs;
  }
  
  static parseConfigProperty(config: Partial<NodeConfig>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'bNodeAddress':
        config.nodeAddress = parseInt(value, 16);
        break;
      case 'bProductID':
        config.productID = parseInt(value, 16);
        break;
      case 'ulConfigFlags':
        config.configFlags = parseInt(value, 16);
        break;
      case 'ulConfigUnitCreate':
        config.configUnitCreate = parseInt(value, 16);
        break;
      case 'usConfigUnitCreateType':
        config.configUnitCreateType = parseInt(value, 16);
        break;
      case 'usConfigMMtype':
        config.configMMtype = parseInt(value, 16);
        break;
      case 'usConfigBitmaptype':
        config.configBitmaptype = parseInt(value, 16);
        break;
      case 'usConfigMemorytype':
        config.configMemorytype = parseInt(value, 16);
        break;
      case 'usConfigScreenmaptype':
        config.configScreenmaptype = parseInt(value, 16);
        break;
      case 'usConfigScheduletype':
        config.configScheduletype = parseInt(value, 16);
        break;
    }
  }
  
  static finalizeConfig(config: Partial<NodeConfig>): NodeConfig {
    return {
      index: config.index!,
      nodeAddress: config.nodeAddress || 0,
      productID: config.productID || 0,
      configFlags: config.configFlags || 0,
      configUnitCreate: config.configUnitCreate || 0,
      configUnitCreateType: config.configUnitCreateType || 0,
      configMMtype: config.configMMtype || 0,
      configBitmaptype: config.configBitmaptype || 0,
      configMemorytype: config.configMemorytype || 0,
      configScreenmaptype: config.configScreenmaptype || 0,
      configScheduletype: config.configScheduletype || 0,
    };
  }
}
