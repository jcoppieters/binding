/**
 * Master API - Master connection management endpoints
 */

import { Router } from 'express';
import { MasterConnectionService } from '../services/MasterConnectionService.js';

export interface MasterAPIDependencies {
  masterService: MasterConnectionService;
}

/**
 * Create master API router
 */
export function createMasterAPI(deps: MasterAPIDependencies): Router {
  const router = Router();

  /**
   * API: Connect and login to master
   * Body: { host: string, port: number, password?: string, fullScan?: boolean }
   */
  router.post('/master/connect', async (req, res) => {
    try {
      const { host, port, password = '', fullScan = false } = req.body;
      
      if (!host || !port) {
        return res.status(400).json({ error: 'host and port required' });
      }
      
      // Connect and login (quick mode skips unit details for faster discovery)
      const result = await deps.masterService.connect(host, port, password, !fullScan);
      
      if (!result.success) {
        return res.status(401).json({ 
          error: result.status,
          message: result.message 
        });
      }
      
      return res.json({
        success: true,
        status: result.status,
        nodes: deps.masterService.getNodes().length,
        units: deps.masterService.getNodes().reduce((sum, n) => sum + n.units.length, 0)
      });
    } catch (error: any) {
      console.error('Failed to connect to master:', error);
      return res.status(500).json({ error: error.message || 'Failed to connect to master' });
    }
  });

  /**
   * API: Disconnect from master
   */
  router.post('/master/disconnect', async (_req, res) => {
    try {
      await deps.masterService.disconnect();
      res.json({ success: true, status: 'disconnected' });
    } catch (error) {
      console.error('Failed to disconnect from master:', error);
      res.status(500).json({ error: 'Failed to disconnect from master' });
    }
  });

  /**
   * API: Get master connection status
   */
  router.get('/master/status', (_req, res) => {
    const status = deps.masterService.getStatus();
    const connected = deps.masterService.isConnected();
    
    res.json({ 
      connected,
      status,
      nodes: connected ? deps.masterService.getNodes().length : 0,
      units: connected ? deps.masterService.getNodes().reduce((sum, n) => sum + n.units.length, 0) : 0
    });
  });

  /**
   * API: Get discovered nodes with full unit details
   */
  router.get('/master/nodes', (_req, res) => {
    res.json(deps.masterService.getNodes());
  });

  /**
   * API: Fetch units for a specific node
   * Params: nodeAddress (hex, e.g. '05' or decimal)
   */
  router.get('/master/node/:nodeAddress/units', async (req, res) => {
    try {
      const { nodeAddress } = req.params;
      const addr = parseInt(nodeAddress, nodeAddress.startsWith('0x') ? 16 : 10);
      
      if (isNaN(addr) || addr < 0 || addr > 255) {
        return res.status(400).json({ error: 'Invalid node address' });
      }

      if (!deps.masterService.isConnected()) {
        return res.status(503).json({ error: 'Not connected to master', success: false });
      }

      // Check if node exists in discovered nodes
      const existingNode = deps.masterService.getNodes().find(n => n.nodeAddress === addr);
      if (!existingNode) {
        return res.status(404).json({ 
          error: `Node 0x${addr.toString(16).padStart(2, '0')} not found. Run "Scan alles" first to discover all nodes.`,
          success: false 
        });
      }

      // Fetch units for this specific node
      const success = await deps.masterService.fetchNodeUnits(addr);
      
      if (!success) {
        return res.status(500).json({ 
          error: `Failed to fetch units for node 0x${addr.toString(16).padStart(2, '0')}. The node may be offline or not responding.`,
          success: false 
        });
      }

      // Return updated node data
      const node = deps.masterService.getNodes().find(n => n.nodeAddress === addr);
      
      return res.json({ 
        success: true, 
        nodeAddress: addr,
        units: node?.units.length ?? 0,
        node
      });
    } catch (error: any) {
      console.error('Failed to fetch node units:', error);
      return res.status(500).json({ error: error.message || 'Failed to fetch node units', success: false });
    }
  });

  return router;
}
