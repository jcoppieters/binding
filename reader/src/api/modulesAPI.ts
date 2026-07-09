/**
 * Modules API — serves the product database from reader/modules/
 */

import { Router } from 'express';
import { readFile } from 'fs/promises';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import express from 'express';

const __dirname = dirname(fileURLToPath(import.meta.url));
const MODULES_DIR = join(__dirname, '../../modules');

export function createModulesAPI(): Router {
  const router = Router();

  /** Serve product images from modules/images/ */
  router.use('/modules/images', express.static(join(MODULES_DIR, 'images')));

  /** GET /api/modules — full index */
  router.get('/modules', async (_req, res) => {
    try {
      const raw = await readFile(join(MODULES_DIR, '_index.json'), 'utf-8');
      res.setHeader('Content-Type', 'application/json');
      res.send(raw);
    } catch {
      res.status(500).json({ error: 'Module database not found. Run: npx tsx src/scripts/scrape-products.ts && npx tsx src/scripts/consolidate-modules.ts' });
    }
  });

  return router;
}
