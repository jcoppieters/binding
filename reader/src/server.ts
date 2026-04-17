/**
 * Web server for Duotecno configuration viewer
 */

import express from 'express';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { InstallationReader } from './readers/installation.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = 3000;

// Serve static files from public directory
app.use(express.static(join(__dirname, '../public')));

// Global installation cache
let cachedInstallation: any = null;
let configPath = join(__dirname, '../configGM');

/**
 * Load installation from config directory
 */
async function loadInstallation() {
  if (!cachedInstallation) {
    console.log('Loading installation configuration...');
    const installation = await InstallationReader.read(configPath);
    cachedInstallation = InstallationReader.toJSON(installation);
    console.log('✅ Installation loaded');
  }
  return cachedInstallation;
}

/**
 * API: Get complete installation data
 */
app.get('/api/installation', async (req, res) => {
  try {
    const data = await loadInstallation();
    res.json(data);
  } catch (error) {
    console.error('Error loading installation:', error);
    res.status(500).json({ error: 'Failed to load installation' });
  }
});

/**
 * API: Get specific node data
 */
app.get('/api/nodes/:nodeAddress', async (req, res) => {
  try {
    const data = await loadInstallation();
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const node = data.nodes.find((n: any) => n.nodeAddress === nodeAddress);
    
    if (!node) {
      return res.status(404).json({ error: 'Node not found' });
    }
    
    res.json(node);
  } catch (error) {
    console.error('Error getting node:', error);
    res.status(500).json({ error: 'Failed to get node' });
  }
});

/**
 * API: Get specific unit data with all bindings
 */
app.get('/api/nodes/:nodeAddress/units/:unitAddress', async (req, res) => {
  try {
    const data = await loadInstallation();
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const unitAddress = parseInt(req.params.unitAddress, 16);
    
    const node = data.nodes.find((n: any) => n.nodeAddress === nodeAddress);
    if (!node) {
      return res.status(404).json({ error: 'Node not found' });
    }
    
    const unit = node.units.find((u: any) => u.address[1] === unitAddress);
    if (!unit) {
      return res.status(404).json({ error: 'Unit not found' });
    }
    
    res.json({
      node: {
        nodeAddress: node.nodeAddress,
        name: node.name,
      },
      unit,
    });
  } catch (error) {
    console.error('Error getting unit:', error);
    res.status(500).json({ error: 'Failed to get unit' });
  }
});

/**
 * API: Reload configuration
 */
app.post('/api/reload', async (req, res) => {
  try {
    cachedInstallation = null;
    const data = await loadInstallation();
    res.json({ success: true, summary: data.summary });
  } catch (error) {
    console.error('Error reloading installation:', error);
    res.status(500).json({ error: 'Failed to reload installation' });
  }
});

// Start server
app.listen(PORT, () => {
  console.log(`\n🌐 Duotecno Configuration Viewer`);
  console.log(`   http://localhost:${PORT}`);
  console.log(`   Config: ${configPath}\n`);
  
  // Pre-load installation
  loadInstallation().catch(console.error);
});
