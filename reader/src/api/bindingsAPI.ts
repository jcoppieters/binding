/**
 * Bindings API - Binding management endpoints
 */

import { Router } from 'express';
import { BindingManager } from '../services/BindingManager.js';

export interface BindingsAPIDependencies {
  bindingManager: BindingManager;
  invalidateInstallationCache: () => void;
}

/**
 * Create bindings API router
 */
export function createBindingsAPI(deps: BindingsAPIDependencies): Router {
  const router = Router();

  /**
   * API: Get modified nodes list
   * NOTE: This MUST come before /bindings/:nodeAddress to avoid matching "modified" as a nodeAddress
   */
  router.get('/bindings/modified', (_req, res) => {
    try {
      const modified = deps.bindingManager.getModifiedNodes();
      res.json({ modified });
    } catch (error) {
      res.status(500).json({ error: 'Failed to get modified nodes' });
    }
  });

  /**
   * API: Get all bindings for a node
   */
  router.get('/bindings/:nodeAddress', (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const state = deps.bindingManager.getState(nodeAddress);
      
      if (!state) {
        return res.status(404).json({ error: 'Node bindings not loaded' });
      }
      
      return res.json(state);
    } catch (error) {
      return res.status(500).json({ error: 'Failed to get bindings' });
    }
  });

  /**
   * API: Add a new binding to a node
   * POST /bindings/:nodeAddress
   * Body: { bindingString: "..." }
   */
  router.post('/bindings/:nodeAddress', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const { bindingString } = req.body;
      
      if (!bindingString) {
        return res.status(400).json({ error: 'bindingString required' });
      }
      
      const success = deps.bindingManager.addBinding(nodeAddress, bindingString);
      
      if (!success) {
        return res.status(404).json({ error: 'Node not loaded' });
      }
      
      // Write to disk and invalidate cache so UI updates correctly
      await deps.bindingManager.writeToFile(nodeAddress);
      deps.invalidateInstallationCache();
      
      const state = deps.bindingManager.getState(nodeAddress);
      return res.json({ success: true, state });
    } catch (error) {
      return res.status(500).json({ error: 'Failed to add binding' });
    }
  });

  /**
   * API: Update an existing binding
   * PUT /bindings/:nodeAddress/:bindingIndex
   * Body: { bindingString: "..." }
   */
  router.put('/bindings/:nodeAddress/:bindingIndex', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const bindingIndex = parseInt(req.params.bindingIndex);
      const { bindingString } = req.body;
      
      if (!bindingString) {
        return res.status(400).json({ error: 'bindingString required' });
      }
      
      const success = deps.bindingManager.updateBinding(nodeAddress, bindingIndex, bindingString);
      
      if (!success) {
        return res.status(404).json({ error: 'Node or binding index not found' });
      }
      
      // Write to disk and invalidate cache so UI updates correctly
      await deps.bindingManager.writeToFile(nodeAddress);
      deps.invalidateInstallationCache();
      
      const state = deps.bindingManager.getState(nodeAddress);
      return res.json({ success: true, state });
    } catch (error) {
      return res.status(500).json({ error: 'Failed to update binding' });
    }
  });

  /**
   * API: Delete a binding
   * DELETE /bindings/:nodeAddress/:bindingIndex
   */
  router.delete('/bindings/:nodeAddress/:bindingIndex', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      const bindingIndex = parseInt(req.params.bindingIndex);
      
      const success = deps.bindingManager.deleteBinding(nodeAddress, bindingIndex);
      
      if (!success) {
        return res.status(404).json({ error: 'Node or binding index not found' });
      }
      
      // Write to disk and invalidate cache so UI updates correctly
      await deps.bindingManager.writeToFile(nodeAddress);
      deps.invalidateInstallationCache();
      
      const state = deps.bindingManager.getState(nodeAddress);
      return res.json({ success: true, state });
    } catch (error) {
      return res.status(500).json({ error: 'Failed to delete binding' });
    }
  });

  /**
   * API: Save bindings to filesystem
   * POST /bindings/:nodeAddress/save
   */
  router.post('/bindings/:nodeAddress/save', async (req, res) => {
    try {
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      await deps.bindingManager.writeToFile(nodeAddress);
      // Explicitly invalidate cache after save
      deps.invalidateInstallationCache();
      
      const state = deps.bindingManager.getState(nodeAddress);
      return res.json({ success: true, state });
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Failed to save bindings' 
      });
    }
  });

  /**
   * API: Save all modified bindings to filesystem
   * POST /bindings/save-all
   */
  router.post('/bindings/save-all', async (_req, res) => {
    try {
      const saved = await deps.bindingManager.writeAllModified();
      return res.json({ success: true, savedNodes: saved });
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Failed to save bindings' 
      });
    }
  });

  return router;
}
