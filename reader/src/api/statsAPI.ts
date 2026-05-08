/**
 * Stats API - Statistics endpoints
 */

import { Router } from 'express';
import { BindingManager } from '../services/BindingManager.js';

export interface StatsAPIDependencies {
  bindingManager: BindingManager;
}

/**
 * Create stats API router
 */
export function createStatsAPI(deps: StatsAPIDependencies): Router {
  const router = Router();

  /**
   * API: Get binding manager statistics
   */
  router.get('/stats', (_req, res) => {
    try {
      const stats = deps.bindingManager.getStatistics();
      res.json(stats);
    } catch (error) {
      res.status(500).json({ error: 'Failed to get statistics' });
    }
  });

  return router;
}
