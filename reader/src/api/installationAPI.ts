/**
 * Installation API - Installation and node management endpoints
 */

import { Router } from 'express';
import { join } from 'path';
import { NodeDatabaseWriter } from '../writers/NodeDatabaseWriter.js';
import { BindingManager } from '../services/BindingManager.js';

export interface InstallationAPIDependencies {
  loadInstallation: () => Promise<any>;
  invalidateInstallationCache: () => void;
  loadBindingFiles: () => Promise<void>;
  bindingManager: BindingManager;
  configPath: string;
}

/**
 * Create installation API router
 */
export function createInstallationAPI(deps: InstallationAPIDependencies): Router {
  const router = Router();

  /**
   * API: Get complete installation data
   */
  router.get('/installation', async (_req, res) => {
    try {
      const data = await deps.loadInstallation();
      res.json(data);
    } catch (error) {
      console.error('Error loading installation:', error);
      res.status(500).json({ error: 'Failed to load installation' });
    }
  });

  /**
   * API: Get specific node data
   */
  router.get('/nodes/:nodeAddress', async (req, res) => {
    try {
      const data = await deps.loadInstallation();
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const node = data.nodes.find((n: any) => n.nodeAddress === nodeAddress);
      
      if (!node) {
        return res.status(404).json({ error: 'Node not found' });
      }
      
      return res.json(node);
    } catch (error) {
      console.error('Error getting node:', error);
      return res.status(500).json({ error: 'Failed to get node' });
    }
  });

  /**
   * API: Reload installation configuration
   */
  router.post('/reload', async (_req, res) => {
    try {
      deps.invalidateInstallationCache();
      const data = await deps.loadInstallation();
      // Reload binding files into manager
      await deps.loadBindingFiles();
      res.json({ 
        success: true, 
        summary: data.summary,
        bindings: deps.bindingManager.getStatistics()
      });
    } catch (error) {
      console.error('Error reloading:', error);
      res.status(500).json({ error: 'Failed to reload configuration' });
    }
  });

  /**
   * API: Rename a node
   */
  router.post('/nodes/:nodeAddress/rename', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const { name } = req.body;
      
      if (!name || typeof name !== 'string') {
        return res.status(400).json({ error: 'Name required' });
      }
      
      // Update nodedatabase.txt
      const nodeDatabasePath = join(deps.configPath, 'Config', 'nodedatabase.txt');
      NodeDatabaseWriter.updateNodeName(nodeDatabasePath, nodeAddress, name);
      
      // Invalidate cache so UI shows updated name
      deps.invalidateInstallationCache();
      
      return res.json({
        success: true,
        message: 'Node renamed successfully',
      });
    } catch (error) {
      console.error('Error renaming node:', error);
      return res.status(500).json({ error: 'Failed to rename node' });
    }
  });

  /**
   * API: Rename a unit
   */
  router.post('/nodes/:nodeAddress/units/:unitAddress/rename', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const unitAddress = parseInt(req.params.unitAddress, 16);
      const { name } = req.body;
      
      if (!name || typeof name !== 'string') {
        return res.status(400).json({ error: 'Name required' });
      }
      
      // Update nodedatabase.txt
      const nodeDatabasePath = join(deps.configPath, 'Config', 'nodedatabase.txt');
      NodeDatabaseWriter.updateUnitName(nodeDatabasePath, nodeAddress, unitAddress, name);
      
      // Invalidate cache so UI shows updated name
      deps.invalidateInstallationCache();
      
      return res.json({
        success: true,
        message: 'Unit renamed successfully',
      });
    } catch (error) {
      console.error('Error renaming unit:', error);
      return res.status(500).json({ error: 'Failed to rename unit' });
    }
  });

  return router;
}
