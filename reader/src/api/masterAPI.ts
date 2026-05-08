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
   * Body: { host: string, port: number, password?: string }
   */
  router.post('/master/connect', async (req, res) => {
    try {
      const { host, port, password = '' } = req.body;
      
      if (!host || !port) {
        return res.status(400).json({ error: 'host and port required' });
      }
      
      // Connect and login (will auto-discover nodes/units)
      const result = await deps.masterService.connect(host, port, password);
      
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

  return router;
}
