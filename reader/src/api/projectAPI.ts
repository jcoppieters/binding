import { Router } from 'express';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { readdir, readFile, writeFile, mkdir } from 'fs/promises';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const PROJECTS_DIR = join(__dirname, '../../projects');

/** Same sanitization used for the .duo/.img filenames, so the .data folder always matches. */
function sanitizeName(name: string): string {
  return name
    .replace(/[^a-z0-9_\- ]/gi, '_')
    .replace(/\s+/g, '_')
    .trim() || 'project';
}

/** Companion folder holding legacy bind*.txt / MoodConfig / etc. files for a project. */
function dataDirFor(projectName: string): string {
  return join(PROJECTS_DIR, `${sanitizeName(projectName)}.data`);
}

/** Recursively list files under `dir` matching bind*.txt, as paths relative to `dir`. */
async function listBindFiles(dir: string, relBase = ''): Promise<string[]> {
  let entries;
  try {
    entries = await readdir(join(dir, relBase), { withFileTypes: true });
  } catch {
    return [];
  }
  const results: string[] = [];
  for (const entry of entries) {
    const rel = relBase ? `${relBase}/${entry.name}` : entry.name;
    if (entry.isDirectory()) {
      results.push(...await listBindFiles(dir, rel));
    } else if (/^bind[0-9a-f]{2}\.txt$/i.test(entry.name)) {
      results.push(rel);
    }
  }
  return results;
}

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
      const safeName = sanitizeName(project.meta.name);
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

  /**
   * POST /api/project/data/upload — copy legacy install files (bind*.txt, MoodConfig, ...)
   * into the project's companion `<name>.data/` folder for later (re)import/upload.
   * body: { projectName, files: [{ path, contentBase64 }] }
   */
  router.post('/project/data/upload', async (req, res) => {
    try {
      const { projectName, files } = req.body;
      if (!projectName || !Array.isArray(files)) {
        res.status(400).json({ error: 'projectName and files[] required' });
        return;
      }

      const dataDir = dataDirFor(projectName);
      await mkdir(dataDir, { recursive: true });

      let written = 0;
      for (const file of files) {
        const relPath = String(file.path || '').replace(/^[/\\]+/, '');
        if (!relPath || /\.\./.test(relPath)) continue; // path-traversal guard
        const dest = join(dataDir, relPath);
        await mkdir(dirname(dest), { recursive: true });
        await writeFile(dest, Buffer.from(file.contentBase64 || '', 'base64'));
        written++;
      }

      res.json({ ok: true, written, folder: `${sanitizeName(projectName)}.data` });
    } catch (e: any) {
      console.error('[projectAPI] data upload error:', e);
      res.status(500).json({ error: e.message });
    }
  });

  /** GET /api/project/data/list?projectName=xxx — bind*.txt files already stored for this project */
  router.get('/project/data/list', async (req, res) => {
    const projectName = String(req.query.projectName ?? '');
    if (!projectName) {
      res.json({ files: [] });
      return;
    }
    const files = await listBindFiles(dataDirFor(projectName));
    res.json({ files });
  });

  /** GET /api/project/data/file?projectName=xxx&path=bind10.txt — read one stored file as text */
  router.get('/project/data/file', async (req, res) => {
    try {
      const projectName = String(req.query.projectName ?? '');
      const relPath = String(req.query.path ?? '');
      if (!projectName || !relPath || /\.\./.test(relPath)) {
        res.status(400).json({ error: 'Invalid request' });
        return;
      }
      const content = await readFile(join(dataDirFor(projectName), relPath), 'utf-8');
      res.json({ content });
    } catch (e: any) {
      res.status(404).json({ error: 'File niet gevonden' });
    }
  });

  return router;
}
