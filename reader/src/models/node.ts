/**
 * Node and unit information models
 * Based on nodedatabase.txt and nodeconfiginfo.txt formats
 */

/**
 * Unit information from nodedatabase.txt
 */
export interface Unit {
  index: number;
  name: string;
  address: number[];       // [nodeAddr, unitAddr]
  unitType: string;        // e.g., "Audio Matrix", "Control", "Switch"
  config: number;          // Config byte (hex)
}

/**
 * Device node information from nodedatabase.txt
 */
export interface DeviceNode {
  index: number;
  name: string;
  physicalAddress: number; // Physical address (hex)
  nodeAddress: number;     // Logical node address (hex)
  nodeType: string;        // e.g., "Std. Node", "Gateway"
  numberOfUnits: number;
  flags: number;           // Node flags (hex)
  softwareVersion: string; // e.g., "V90"
  units: Unit[];
}

/**
 * Node configuration from nodeconfiginfo.txt
 */
export interface NodeConfig {
  index: number;
  nodeAddress: number;
  productID: number;           // Product ID (hex)
  configFlags: number;         // Configuration flags (hex)
  configUnitCreate: number;    // Unit creation flags (hex)
  configUnitCreateType: number;
  configMMtype: number;
  configBitmaptype: number;
  configMemorytype: number;
  configScreenmaptype: number;
  configScheduletype: number;
}

/**
 * Node database - collection of all nodes
 */
export interface NodeDatabase {
  nodes: DeviceNode[];
  nodeConfigs: NodeConfig[];
}
