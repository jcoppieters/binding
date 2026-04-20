/**
 * Node and Unit Naming Service
 * 
 * Handles uploading node/unit names to hardware via CAN bus.
 * Based on RenameNodeUnitsEngine.cpp from legacy C++ code.
 */

import {
  CANBusService,
  FunctionCode,
  EventCode,
  CANFrame,
} from '../communication/CANBusService';

const BROADCAST_UNIT = 0xFF;
const MAX_NAME_LENGTH = 32;
const CHUNK_SIZE = 5; // Max data per CAN frame (7 bytes total - 2 for index/length)

// Node config flags (from nodegenestructs.h)
const APPL_UNLOCK = 0x02; // Enable programming mode

export interface NameUploadResult {
  success: boolean;
  nodeAddress: number;
  unitAddress?: number;
  name: string;
  error?: string;
}

export class NodeNamingService {
  constructor(private canBus: CANBusService) {}
  
  /**
   * Unlock node for programming (set APPL_UNLOCK flag)
   * Must be called before uploading names or bindings
   */
  async unlockNode(nodeAddress: number): Promise<void> {
    console.log(`[NodeNaming] Unlocking node 0x${nodeAddress.toString(16)} for programming...`);

    const message: CANFrame = {
      nodeAddress,
      unitAddress: BROADCAST_UNIT,
      functionCode: FunctionCode.FC_NODECONFIG,
      dataLength: 2,
      data: Buffer.from([
        0x00,         // nodeConfig (keep current config)
        APPL_UNLOCK,  // dllAccessControl (enable programming)
      ]),
    };

    const response = await this.canBus.sendMessageWithRetry(
      message,
      EventCode.EV_NODECONFIG
    );

    // Check if unlock succeeded (response.data[3] should have APPL_UNLOCK flag)
    const dllAccess = response.data[3];
    if ((dllAccess & APPL_UNLOCK) === 0) {
      throw new Error('Failed to unlock node for programming');
    }

    console.log(`[NodeNaming] ✅ Node unlocked (DllAccess=0x${dllAccess.toString(16).padStart(2, '0')})`);
  }

  /**
   * Lock node after programming (clear APPL_UNLOCK flag)
   */
  async lockNode(nodeAddress: number): Promise<void> {
    console.log(`[NodeNaming] Locking node 0x${nodeAddress.toString(16)}...`);

    const message: CANFrame = {
      nodeAddress,
      unitAddress: BROADCAST_UNIT,
      functionCode: FunctionCode.FC_NODECONFIG,
      dataLength: 2,
      data: Buffer.from([
        0x00,  // nodeConfig (keep current config)
        0x00,  // dllAccessControl (disable programming)
      ]),
    };

    await this.canBus.sendMessageWithRetry(
      message,
      EventCode.EV_NODECONFIG
    );

    console.log(`[NodeNaming] ✅ Node locked`);
  }

  /**
   * Upload a node name to hardware
   * 
   * Implements the SendNodeName() FSM from RenameNodeUnitsEngine.cpp
   */
  async uploadNodeName(
    nodeAddress: number,
    name: string
  ): Promise<NameUploadResult> {
    let lastError: Error | string | null = null;
    
    try {
      // Subscribe to CAN bus to receive responses
      await this.canBus.subscribe();
      
      // NOTE: Unlock might not be needed over TCP (only for direct CAN bus)
      // Try without unlock first - if it fails, we can add it back
      // await this.unlockNode(nodeAddress);

      // Validate and format name
      const formattedName = this.formatName(name);
      const data = Buffer.from(formattedName, 'ascii');
      
      console.log(`[NodeNaming] Uploading node name to 0x${nodeAddress.toString(16)}: "${name}"`);
      
      // Send name in chunks (max 5 bytes per CAN frame)
      for (let position = 0; position < data.length; position += CHUNK_SIZE) {
        const chunk = data.slice(position, position + CHUNK_SIZE);
        const chunkLength = chunk.length;
        
        // Build CAN message
        // Format: [index][raw_ascii_bytes...]
        // Note: C++ converts to hex string then back to bytes - we skip that intermediate step
        const messageData = Buffer.concat([
          Buffer.from([position]), // Index in name string
          chunk,                   // Raw ASCII bytes
        ]);
        
        const message: CANFrame = {
          nodeAddress,
          unitAddress: BROADCAST_UNIT,
          functionCode: FunctionCode.FC_NODESETNAME,
          dataLength: chunkLength + 1, // +1 for index byte
          data: messageData,
        };
        
        console.log(
          `[NodeNaming] Sending chunk ${position}/${data.length}: "${chunk.toString('ascii')}"`
        );
        
        // Send and wait for acknowledgement
        const response = await this.canBus.sendMessageWithRetry(
          message,
          EventCode.EV_NODENAME
        );
        
        // Check response status
        if (response.data[0] !== 0x00) {
          throw new Error(`Hardware rejected name (error code: ${response.data[0]})`);
        }
      }
      
      console.log(`[NodeNaming] ✅ Node name uploaded successfully`);
      
      // Lock node after successful upload (if we unlocked it)
      // await this.lockNode(nodeAddress);

      return {
        success: true,
        nodeAddress,
        name,
      };
    } catch (error) {
      console.error('[NodeNaming] ❌ Upload failed:', error);
      lastError = error instanceof Error ? error.message : String(error);
    } finally {
      // Always unsubscribe after operation
      await this.canBus.unsubscribe();
    }

    // Error path (after unsubscribe in finally)
    return {
      success: false,
      nodeAddress,
      name,
      error: lastError || 'Unknown error',
    };
  }
  
  /**
   * Upload a unit name to hardware
   * 
   * Implements the SendUnitName() FSM from RenameNodeUnitsEngine.cpp
   */
  async uploadUnitName(
    nodeAddress: number,
    unitAddress: number,
    name: string
  ): Promise<NameUploadResult> {
    let lastError: Error | string | null = null;
    
    try {
      if (unitAddress === BROADCAST_UNIT) {
        throw new Error('Cannot rename virtual unit address 0xFF');
      }

      // Subscribe to CAN bus to receive responses
      await this.canBus.subscribe();
      
      // NOTE: Unlock not needed over TCP (only for direct CAN bus)
      // await this.unlockNode(nodeAddress);

      const formattedName = this.formatName(name);
      const data = Buffer.from(formattedName, 'ascii');
      
      console.log(
        `[NodeNaming] Uploading unit name to 0x${nodeAddress.toString(16)}/0x${unitAddress.toString(16)}: "${name}"`
      );
      
      // Send name in chunks
      for (let position = 0; position < data.length; position += CHUNK_SIZE) {
        const chunk = data.slice(position, position + CHUNK_SIZE);
        const chunkLength = chunk.length;
        
        // Build CAN message
        // Format: [index][raw_ascii_bytes...]
        // Note: C++ converts to hex string then back to bytes - we skip that intermediate step
        const messageData = Buffer.concat([
          Buffer.from([position]),     // Index in name string
          chunk,                       // Raw ASCII bytes
        ]);
        
        const message: CANFrame = {
          nodeAddress,
          unitAddress,
          functionCode: FunctionCode.FC_UNITSETNAME,
          dataLength: chunkLength + 1,
          data: messageData,
        };
        
        console.log(
          `[NodeNaming] Sending chunk ${position}/${data.length}: "${chunk.toString('ascii')}"`
        );
        
        const response = await this.canBus.sendMessageWithRetry(
          message,
          EventCode.EV_UNITNAME
        );
        
        if (response.data[0] !== 0x00) {
          throw new Error(`Hardware rejected name (error code: ${response.data[0]})`);
        }
      }
      
      console.log(`[NodeNaming] ✅ Unit name uploaded successfully`);
      
      // NOTE: Lock not needed over TCP (only for direct CAN bus)
      // await this.lockNode(nodeAddress);

      return {
        success: true,
        nodeAddress,
        unitAddress,
        name,
      };
    } catch (error) {
      console.error('[NodeNaming] ❌ Upload failed:', error);
      lastError = error instanceof Error ? error.message : String(error);
    } finally {
      // Always unsubscribe after operation
      await this.canBus.unsubscribe();
    }

    // Error path (after unsubscribe in finally)
    return {
      success: false,
      nodeAddress,
      unitAddress,
      name,
      error: lastError || 'Unknown error',
    };
  }
  
  /**
   * Upload all node and unit names from a database
   */
  async uploadAllNames(
    nodes: Array<{
      address: number;
      name: string;
      units: Array<{ address: number; name: string }>;
    }>
  ): Promise<{
    totalNodes: number;
    totalUnits: number;
    successfulNodes: number;
    successfulUnits: number;
    errors: string[];
  }> {
    const errors: string[] = [];
    let successfulNodes = 0;
    let successfulUnits = 0;
    
    console.log(`[NodeNaming] Starting bulk upload for ${nodes.length} nodes...`);
    
    for (const node of nodes) {
      // Upload node name
      const nodeResult = await this.uploadNodeName(node.address, node.name);
      if (nodeResult.success) {
        successfulNodes++;
      } else {
        errors.push(
          `Node 0x${node.address.toString(16)}: ${nodeResult.error}`
        );
      }
      
      // Upload unit names
      for (const unit of node.units) {
        if (unit.address === BROADCAST_UNIT) continue;
        
        const unitResult = await this.uploadUnitName(
          node.address,
          unit.address,
          unit.name
        );
        
        if (unitResult.success) {
          successfulUnits++;
        } else {
          errors.push(
            `Unit 0x${node.address.toString(16)}/0x${unit.address.toString(16)}: ${unitResult.error}`
          );
        }
      }
    }
    
    const totalUnits = nodes.reduce((sum, n) => sum + n.units.length, 0);
    
    console.log(
      `[NodeNaming] Bulk upload complete: ${successfulNodes}/${nodes.length} nodes, ${successfulUnits}/${totalUnits} units`
    );
    
    return {
      totalNodes: nodes.length,
      totalUnits,
      successfulNodes,
      successfulUnits,
      errors,
    };
  }
  
  /**
   * Format name according to Duotecno constraints
   * 
   * Based on FormatData() from RenameNodeUnitsEngine.cpp
   */
  private formatName(name: string): string {
    // Trim and limit length
    let formatted = name.trim();
    
    if (formatted.length === 0) {
      // If empty, use space (as per C++ implementation)
      return ' ';
    }
    
    if (formatted.length > MAX_NAME_LENGTH - 1) {
      formatted = formatted.substring(0, MAX_NAME_LENGTH - 1);
    }
    
    // Add null terminator
    return formatted + '\0';
  }
}
