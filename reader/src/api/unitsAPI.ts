/**
 * Units API - Unit control and status endpoints
 */

import { Router } from 'express';
import { MasterConnectionService } from '../services/MasterConnectionService.js';

export interface UnitsAPIDependencies {
  masterService: MasterConnectionService;
}

/**
 * Create units API router
 */
export function createUnitsAPI(deps: UnitsAPIDependencies): Router {
  const router = Router();

  /**
   * API: Get all unit states
   */
  router.get('/units', (_req, res) => {
    if (!deps.masterService.isConnected()) {
      return res.status(400).json({ error: 'Not connected to master' });
    }
    
    const nodes = deps.masterService.getNodes();
    const units = nodes.flatMap(node => node.units);
    return res.json({ units });
  });

  /**
   * API: Get unit state
   */
  router.get('/units/:nodeAddress/:unitAddress', (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      
      const unit = deps.masterService.getUnit(nodeAddress, unitAddress);
      
      if (!unit) {
        return res.status(404).json({ error: 'Unit not found' });
      }
      
      return res.json(unit);
    } catch (error) {
      console.error('Error getting unit state:', error);
      return res.status(500).json({ error: 'Failed to get unit state' });
    }
  });

  /**
   * API: Set unit state
   * Body: { value: number | boolean, status?: number, type?: number }
   */
  router.post('/units/:nodeAddress/:unitAddress/set', async (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      const { value, status, type } = req.body;
      
      // Either value or status must be provided
      if (value == null && status == null) {
        return res.status(400).json({ error: 'Either value or status required' });
      }
      
      // Get current unit to determine type if not provided
      const currentUnit = deps.masterService.getUnit(nodeAddress, unitAddress);
      if (!currentUnit) {
        return res.status(404).json({ error: 'Unit not found' });
      }
      
      const unitType = type !== undefined ? type : currentUnit.type;
      const unitStatus = status !== undefined ? status : 0;
      
      // Send command and wait for status response
      const updatedUnit = await deps.masterService.sendUnitCommand(
        nodeAddress,
        unitAddress,
        unitType,
        unitStatus,
        value
      );
      
      return res.json({ success: true, unit: updatedUnit });
    } catch (error: any) {
      console.error('Error setting unit state:', error);
      return res.status(500).json({ error: error.message || 'Failed to set unit state' });
    }
  });

  /**
   * API: Send dimmer method directly
   * Body: { method: number } - Raw dimmer method number (5=UP, 6=DOWN, 9=OFF, 10=ON, etc.)
   */
  router.post('/units/:nodeAddress/:unitAddress/dimmer-method', async (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      const { method } = req.body;
      
      if (method == null) {
        return res.status(400).json({ error: 'Method number required' });
      }
      
      // Build and send dimmer command with raw method number
      const message = [162, method, nodeAddress, unitAddress]; // Cmd.SetDimmer = 162
      deps.masterService.sendRawMessage(message);
      
      // Wait briefly for hardware to process
      await new Promise(resolve => setTimeout(resolve, 200));
      
      // Hardware bug workaround: Request status explicitly for dimmers
      // Some dimmers don't send automatic status updates after commands
      deps.masterService.requestUnitStatus(nodeAddress, unitAddress, 1); // 1 = UnitType.DIM
      
      // Wait for status response
      await new Promise(resolve => setTimeout(resolve, 200));
      
      // Get updated unit state
      const updatedUnit = deps.masterService.getUnit(nodeAddress, unitAddress);
      if (updatedUnit) {
        return res.json({ success: true, unit: updatedUnit });
      } else {
        return res.json({ success: true });
      }
    } catch (error: any) {
      console.error('Error sending dimmer method:', error);
      return res.status(500).json({ error: error.message || 'Failed to send dimmer method' });
    }
  });

  /**
   * API: Request unit status refresh
   */
  router.post('/units/:nodeAddress/:unitAddress/refresh', async (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      
      // Get unit to determine its type
      const unit = deps.masterService.getUnit(nodeAddress, unitAddress);
      if (!unit) {
        return res.status(404).json({ error: 'Unit not found' });
      }
      
      // Request status update
      deps.masterService.requestUnitStatus(nodeAddress, unitAddress, unit.type);
      
      return res.json({ success: true });
    } catch (error: any) {
      console.error('Error requesting unit status:', error);
      return res.status(500).json({ error: error.message || 'Failed to request unit status' });
    }
  });

  /**
   * API: Refresh all unit states
   */
  router.post('/units/refresh-all', async (_req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      // Request status for all discovered units
      const nodes = deps.masterService.getNodes();
      for (const node of nodes) {
        for (const unit of node.units) {
          deps.masterService.requestUnitStatus(node.nodeAddress, unit.unitAddress, unit.type);
        }
      }
      
      return res.json({ success: true });
    } catch (error: any) {
      console.error('Error refreshing all units:', error);
      return res.status(500).json({ error: error.message || 'Failed to refresh all units' });
    }
  });

  /**
   * API: Get all units for a specific node
   */
  router.get('/nodes/:nodeAddress/units-state', (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const units = deps.masterService.getNodeUnits(nodeAddress);
      
      return res.json({ units });
    } catch (error) {
      console.error('Error getting node units:', error);
      return res.status(500).json({ error: 'Failed to get node units' });
    }
  });

  /**
   * API: Send sensor control command (preset selection or temp adjust)
   * Body: { preset?: number, action?: 'inc' | 'dec' }
   */
  router.post('/units/:nodeAddress/:unitAddress/sensor-control', async (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      const { preset, action } = req.body;
      
      let message: number[];
      
      if (preset !== undefined) {
        // Set preset (method 13)
        message = [136, 13, nodeAddress, unitAddress, preset]; // Cmd.SetSensor=136, method=13
        console.log(`📤 Setting sensor preset: [${message.join(',')}]`);
      } else if (action === 'inc' || action === 'dec') {
        // Inc/Dec temperature (method 5=inc, 6=dec)
        const method = action === 'inc' ? 5 : 6;
        message = [136, method, nodeAddress, unitAddress]; // Cmd.SetSensor=136
        console.log(`📤 ${action === 'inc' ? 'Incrementing' : 'Decrementing'} sensor temp: [${message.join(',')}]`);
      } else {
        return res.status(400).json({ error: 'Either preset or action (inc/dec) required' });
      }
      
      // Send command
      deps.masterService.sendRawMessage(message);
      
      // Wait for hardware to process
      await new Promise(resolve => setTimeout(resolve, 250));
      
      // Request status update (multiple times for sensors as they send extended status)
      deps.masterService.requestUnitStatus(nodeAddress, unitAddress, 7); // 7 = UnitType.SENS
      
      // Wait for status response
      await new Promise(resolve => setTimeout(resolve, 400));
      
      // Request again to ensure we have latest setpoint values
      deps.masterService.requestUnitStatus(nodeAddress, unitAddress, 7);
      
      // Wait for second status response
      await new Promise(resolve => setTimeout(resolve, 400));
      
      // Get updated unit state
      const updatedUnit = deps.masterService.getUnit(nodeAddress, unitAddress);
      if (updatedUnit) {
        return res.json({ success: true, unit: updatedUnit });
      } else {
        return res.json({ success: true });
      }
    } catch (error: any) {
      console.error('Error sending sensor control:', error);
      return res.status(500).json({ error: error.message || 'Failed to send sensor control' });
    }
  });

  return router;
}
