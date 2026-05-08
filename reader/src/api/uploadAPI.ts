/**
 * Upload API - Hardware binding upload endpoints
 */

import { Router } from 'express';
import { BindingManager } from '../services/BindingManager.js';
import { BindingUploader } from '../communication/BindingUploader.js';
import { MasterConnectionService } from '../services/MasterConnectionService.js';

export interface UploadAPIDependencies {
  bindingManager: BindingManager;
  masterService: MasterConnectionService;
}

/**
 * Create upload API router
 */
export function createUploadAPI(deps: UploadAPIDependencies): Router {
  const router = Router();

  /**
   * API: Upload bindings to a single node
   * POST /upload/node/:nodeAddress
   */
  router.post('/upload/node/:nodeAddress', async (req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master node. Use /api/master/connect first.' });
      }
      
      const nodeAddress = parseInt(req.params.nodeAddress, 16);
      
      // Create uploader on-demand (uses singleton connection)
      const uploader = new BindingUploader('', 0, ''); // Dummy values, uses singleton
      const result = await uploader.uploadToNode(nodeAddress, deps.bindingManager);
      
      return res.json(result);
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Upload failed' 
      });
    }
  });

  /**
   * API: Upload all bindings (force)
   * POST /upload/all
   */
  router.post('/upload/all', async (_req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master node. Use /api/master/connect first.' });
      }
      
      // Create uploader on-demand (uses singleton connection)
      const uploader = new BindingUploader('', 0, '');
      const results = await uploader.uploadAll(deps.bindingManager);
      
      const summary = {
        total: results.length,
        successful: results.filter(r => r.success).length,
        failed: results.filter(r => !r.success).length,
        results
      };
      
      return res.json(summary);
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Upload failed' 
      });
    }
  });

  /**
   * API: Upload only changed bindings (smart upload)
   * POST /upload/changed
   */
  router.post('/upload/changed', async (_req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master node. Use /api/master/connect first.' });
      }
      
      // Create uploader on-demand (uses singleton connection)
      const uploader = new BindingUploader('', 0, '');
      const results = await uploader.uploadChanged(deps.bindingManager);
      
      const summary = {
        total: results.length,
        uploaded: results.filter(r => r.success && !r.skipped).length,
        skipped: results.filter(r => r.skipped).length,
        failed: results.filter(r => !r.success).length,
        results
      };
      
      return res.json(summary);
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Upload failed' 
      });
    }
  });

  /**
   * API: Upload only modified bindings
   * POST /upload/modified
   */
  router.post('/upload/modified', async (_req, res) => {
    try {
      if (!deps.masterService.isConnected()) {
        return res.status(400).json({ error: 'Not connected to master node. Use /api/master/connect first.' });
      }
      
      // Create uploader on-demand (uses singleton connection)
      const uploader = new BindingUploader('', 0, '');
      const results = await uploader.uploadModified(deps.bindingManager);
      
      const summary = {
        total: results.length,
        successful: results.filter(r => r.success).length,
        failed: results.filter(r => !r.success).length,
        results
      };
      
      return res.json(summary);
    } catch (error) {
      return res.status(500).json({ 
        error: error instanceof Error ? error.message : 'Upload failed' 
      });
    }
  });

  return router;
}
