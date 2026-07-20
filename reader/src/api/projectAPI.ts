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
      const duoFilename = `${safeName}.duo`;
      const imgFilename = `${safeName}.img`;

      await mkdir(PROJECTS_DIR, { recursive: true });
      
      // Extract images from project
      const images: Record<string, string> = {};
      const cleanProject = { ...project };
      
      // Extract room background images
      if (cleanProject.homeView?.rooms) {
        cleanProject.homeView.rooms = cleanProject.homeView.rooms.map((room: any) => {
          if (room.backgroundImage && room.backgroundImage.startsWith('data:')) {
            images[`room-${room.id}`] = room.backgroundImage;
            return { ...room, backgroundImageRef: `room-${room.id}`, backgroundImage: undefined };
          }
          return room;
        });
      }
      
      // Save .duo file (clean, without base64 images)
      await writeFile(
        join(PROJECTS_DIR, duoFilename),
        JSON.stringify({ ...cleanProject, meta: { ...cleanProject.meta, modified: new Date().toISOString() } }, null, 2),
        'utf-8'
      );
      
      // Save .img file if there are images
      if (Object.keys(images).length > 0) {
        await writeFile(
          join(PROJECTS_DIR, imgFilename),
          JSON.stringify({ images }, null, 2),
          'utf-8'
        );
      }
      
      res.json({ ok: true, filename: duoFilename });
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
      const project = JSON.parse(raw);
      
      // Try to load companion .img file
      const imgFilename = filename.replace('.duo', '.img');
      try {
        const imgRaw = await readFile(join(PROJECTS_DIR, imgFilename), 'utf-8');
        const imgData = JSON.parse(imgRaw);
        
        // Merge images back into project
        if (imgData.images && project.homeView?.rooms) {
          project.homeView.rooms = project.homeView.rooms.map((room: any) => {
            if (room.backgroundImageRef && imgData.images[room.backgroundImageRef]) {
              return { ...room, backgroundImage: imgData.images[room.backgroundImageRef], backgroundImageRef: undefined };
            }
            return room;
          });
        }
      } catch (imgErr) {
        // No .img file or error reading it - that's okay, continue without images
        console.log(`[projectAPI] No .img file found for ${filename}`);
      }
      
      res.json(project);
    } catch (e: any) {
      res.status(404).json({ error: 'Project niet gevonden' });
    }
  });

  return router;
}
