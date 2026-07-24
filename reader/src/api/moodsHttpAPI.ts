/**
 * Moods HTTP API — bridges the frontend to a Duotecno node's live moods
 * config API (HTTPS auth :apiPort + HTTP config :8080). See `MoodHttpService`.
 *
 * All endpoints take `host`, `apiPort`, `password` and the persisted
 * `clientId`/`refreshToken` (if any) in the request body/query, and return
 * the (possibly renewed) credentials alongside the result so the frontend
 * can persist them in `project.meta`.
 */

import { Router } from 'express';
import { MoodHttpService } from '../services/MoodHttpService.js';

export interface MoodsHttpAPIDependencies {
  moodHttpService: MoodHttpService;
}

export function createMoodsHttpAPI(deps: MoodsHttpAPIDependencies): Router {
  const router = Router();
  const { moodHttpService } = deps;

  function credentialsFrom(body: any) {
    return body.clientId && body.refreshToken ? { clientId: body.clientId, refreshToken: body.refreshToken } : undefined;
  }

  /** Authenticate (create or refresh client) — call once before the other endpoints. */
  router.post('/moods-http/connect', async (req, res) => {
    try {
      const { host, apiPort = 8081, password = '' } = req.body;
      if (!host) return res.status(400).json({ error: 'host required' });
      const credentials = await moodHttpService.authenticate(host, apiPort, password, credentialsFrom(req.body));
      return res.json({ success: true, ...credentials });
    } catch (error: any) {
      return res.status(401).json({ error: error.message || 'Authenticatie mislukt' });
    }
  });

  /** GET summary of all moods on the node. */
  router.post('/moods-http/info', async (req, res) => {
    try {
      const { host, apiPort = 8081, password = '' } = req.body;
      const credentials = credentialsFrom(req.body);
      if (!host || !credentials) return res.status(400).json({ error: 'host and credentials required' });
      const info = await moodHttpService.getMoodsInfo(host, apiPort, credentials, password);
      return res.json(info);
    } catch (error: any) {
      return res.status(502).json({ error: error.message || 'Ophalen mislukt' });
    }
  });

  /** GET a single mood ("Ophalen"). */
  router.post('/moods-http/:id/fetch', async (req, res) => {
    try {
      const { host, apiPort = 8081, password = '' } = req.body;
      const credentials = credentialsFrom(req.body);
      if (!host || !credentials) return res.status(400).json({ error: 'host and credentials required' });
      const mood = await moodHttpService.getMood(host, apiPort, credentials, password, Number(req.params.id));
      return res.json(mood);
    } catch (error: any) {
      return res.status(502).json({ error: error.message || 'Ophalen mislukt' });
    }
  });

  /** POST (create/edit) a single mood ("Versturen"). */
  router.post('/moods-http/:id/send', async (req, res) => {
    try {
      const { host, apiPort = 8081, password = '', mood } = req.body;
      const credentials = credentialsFrom(req.body);
      if (!host || !credentials || !mood) return res.status(400).json({ error: 'host, credentials and mood required' });
      const result = await moodHttpService.saveMood(host, apiPort, credentials, password, mood);
      return res.json(result);
    } catch (error: any) {
      return res.status(502).json({ error: error.message || 'Versturen mislukt' });
    }
  });

  return router;
}
