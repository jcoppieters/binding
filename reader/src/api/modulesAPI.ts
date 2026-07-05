/**
 * Modules API — serves the product database from reader/modules/
 */

import { Router } from 'express';
import { readFile } from 'fs/promises';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));
const MODULES_DIR = join(__dirname, '../../modules');

export function createModulesAPI(): Router {
  const router = Router();

  /** GET /api/modules — full index (115 entries: families + standalone) */
  router.get('/modules', async (_req, res) => {
    try {
      const raw = await readFile(join(MODULES_DIR, '_index.json'), 'utf-8');
      res.setHeader('Content-Type', 'application/json');
      res.send(raw); // send pre-serialised — no re-parse/stringify
    } catch {
      res.status(500).json({ error: 'Module database not found. Run: npx tsx src/scripts/scrape-products.ts && npx tsx src/scripts/consolidate-modules.ts' });
    }
  });

  /** GET /api/modules/:slug — single module/family file */
  router.get('/modules/:slug', async (req, res) => {
    try {
      const raw = await readFile(join(MODULES_DIR, `${req.params.slug}.json`), 'utf-8');
      res.setHeader('Content-Type', 'application/json');
      res.send(raw);
    } catch {
      res.status(404).json({ error: `Module '${req.params.slug}' not found` });
    }
  });

  return router;
}
