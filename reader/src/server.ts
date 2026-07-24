/**
 * Enhanced Web server for Duotecno configuration viewer and binding editor
 */

import express from 'express';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { readdir } from 'fs/promises';
import { InstallationReader } from './readers/installation.js';
import { BindingFileReader } from './readers/BindingFileReader.js';
import { BindingManager } from './services/BindingManager.js';
import { MasterConnectionService } from './services/MasterConnectionService.js';
import { MoodHttpService } from './services/MoodHttpService.js';

// Import API modules
import bindingEditorAPI from './api/bindingEditorAPI.js';
import bindingImportAPI from './api/bindingImportAPI.js';
import bindingConverterAPI from './api/bindingConverterAPI.js';
import { createInstallationAPI } from './api/installationAPI.js';
import { createBindingsAPI } from './api/bindingsAPI.js';
import { createUploadAPI } from './api/uploadAPI.js';
import { createMasterAPI } from './api/masterAPI.js';
import { createUnitsAPI } from './api/unitsAPI.js';
import { createStatsAPI } from './api/statsAPI.js';
import { createModulesAPI } from './api/modulesAPI.js';
import { createProjectAPI } from './api/projectAPI.js';
import { createMoodsHttpAPI } from './api/moodsHttpAPI.js';
import { attachLiveWebSocket } from './api/liveWs.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = 3000;

// Enable JSON body parsing with 50MB limit (for large images)
app.use(express.json({ limit: '50mb' }));
app.use(express.urlencoded({ limit: '50mb', extended: true }));

// Serve static files from public directory
app.use(express.static(join(__dirname, '../public')));

// Serve module images under /modules/images/ path
app.use('/modules/images', express.static(join(__dirname, '../modules/images')));

// Request logging middleware
app.use((req, _res, next) => {
  console.log(`[${req.method}] ${req.url}`);
  next();
});

// Global state
let cachedInstallation: any = null;
let configPath = join(__dirname, '../configValies');
const bindingManager = new BindingManager();

// Master connection for unit control and status monitoring (singleton)
const masterService = MasterConnectionService.getInstance();

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
 * Invalidate installation cache (call after modifying bindings)
 */
function invalidateInstallationCache() {
  cachedInstallation = null;
}

/**
 * Load all binding files into manager
 */
async function loadBindingFiles() {
  console.log('Loading binding files...');
  const files = await readdir(configPath);
  const bindFiles = files.filter(f => f.match(/^bind[0-9a-f]{2}\.txt$/i));
  
  for (const filename of bindFiles) {
    const filepath = join(configPath, filename);
    const bindingFile = await BindingFileReader.readFile(filepath);
    const nodeAddress = parseInt(filename.substring(4, 6), 16);
    bindingManager.load(nodeAddress, bindingFile, filepath);
  }
  
  const stats = bindingManager.getStatistics();
  console.log(`✅ Loaded ${stats.totalNodes} binding files (${stats.totalBindings} total bindings)`);
}

// ==================== Mount API Routes ====================

// Create and mount API routers
app.use('/api', createInstallationAPI({
  loadInstallation,
  invalidateInstallationCache,
  loadBindingFiles,
  bindingManager,
  configPath
}));

app.use('/api', createBindingsAPI({
  bindingManager,
  invalidateInstallationCache
}));

app.use('/api', createUploadAPI({
  bindingManager,
  masterService
}));

app.use('/api', createMasterAPI({
  masterService
}));

app.use('/api', createMoodsHttpAPI({
  moodHttpService: new MoodHttpService()
}));

app.use('/api', createUnitsAPI({
  masterService
}));

app.use('/api', createStatsAPI({
  bindingManager
}));

app.use('/api/editor', bindingEditorAPI);

// Binding import (from disk) - mount before regular bindings API to avoid conflicts
app.use('/api/import', bindingImportAPI);

// Binding converter (legacy → visual format)
app.use('/api/convert', bindingConverterAPI);

// Module database (product catalogue)
app.use('/api', createModulesAPI());

// Project save / load
app.use('/api', createProjectAPI());

// ==================== End API Routes ====================

process.on('SIGINT', () => {
  console.log('\n👋 Received SIGINT, shutting down gracefully...');
  
  // Disconnect from master if connected
  if (masterService.isConnected()) {
    masterService.disconnect().catch(console.error);
  }
  
  process.exit(0);
});

process.on('uncaughtException', (error) => {
  console.error('\n❌ Uncaught Exception:', error);
});

process.on('unhandledRejection', (reason, promise) => {
  console.error('\n❌ Unhandled Rejection at:', promise, 'reason:', reason);
});

// Start server
// Note: the legacy installation/bindings config (configPath, only used by the
// old standalone viewer at /viewer-legacy.html) is intentionally NOT pre-loaded
// here — it's loaded lazily on first request to /api/installation or /api/reload.
const httpServer = app.listen(PORT, () => {
  console.log(`\n🌐 Duotecno Configuration Server`);
  console.log(`   http://localhost:${PORT}`);
  console.log(`   🔌 Live updates: ws://localhost:${PORT}/ws/live\n`);
});

// Live status push (Home View) — see reader/TODO.md "Live hardware view"
attachLiveWebSocket(httpServer, masterService);
