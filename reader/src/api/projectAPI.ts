import { Router } from 'express';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { readdir, readFile, writeFile, mkdir } from 'fs/promises';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const PROJECTS_DIR = join(__dirname, '../../projects');

export function createProjectAPI() {
  const router = Router();

  /** POST /api/project/save — body: full project JSON */
  router.post('/project/save', async (req, res) => {
    try {
      const project = req.body;
      if (!project?.meta?.name) {
        res.status(400).json({ error: 'Missing project.meta.name' });
        return;
      }
      // Sanitize filename: keep alphanumeric, space, dash, underscore
      const safeName = project.meta.name
        .replace(/[^a-z0-9_\- ]/gi, '_')
        .replace(/\s+/g, '_')
        .trim() || 'project';
      const filename = `${safeName}.duo`;

      await mkdir(PROJECTS_DIR, { recursive: true });
      await writeFile(
        join(PROJECTS_DIR, filename),
        JSON.stringify({ ...project, meta: { ...project.meta, modified: new Date().toISOString() } }, null, 2),
        'utf-8'
      );
      res.json({ ok: true, filename });
    } catch (e: any) {
      console.error('[projectAPI] save error:', e);
      res.status(500).json({ error: e.message });
    }
  });

  /** GET /api/project/list — returns array of .duo filenames */
  router.get('/project/list', async (_req, res) => {
    try {
      await mkdir(PROJECTS_DIR, { recursive: true });
      const files = await readdir(PROJECTS_DIR);
      res.json(files.filter(f => f.endsWith('.duo')).sort());
    } catch (e: any) {
      res.status(500).json({ error: e.message });
    }
  });

  /** GET /api/project/load?file=xxx.duo — returns project JSON */
  router.get('/project/load', async (req, res) => {
    try {
      const filename = String(req.query.file ?? '');
      // Basic path-traversal guard
      if (!filename.endsWith('.duo') || /[/\\]|\.\./.test(filename)) {
        res.status(400).json({ error: 'Invalid filename' });
        return;
      }
      const raw = await readFile(join(PROJECTS_DIR, filename), 'utf-8');
      res.json(JSON.parse(raw));
    } catch (e: any) {
      res.status(404).json({ error: 'Project niet gevonden' });
    }
  });

  return router;
}
