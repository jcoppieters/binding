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
import { BindingUploader } from './communication/BindingUploader.js';
import { NodeDatabaseWriter } from './writers/NodeDatabaseWriter.js';
import { CANBusService, MockCANBusService } from './communication/CANBusService.js';
import { TCPCANBusService } from './communication/TCPCANBusService.js';
import { NodeNamingService } from './services/NodeNamingService.js';
import bindingEditorAPI from './api/bindingEditorAPI.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = dirname(__filename);

const app = express();
const PORT = 3000;

// Enable JSON body parsing
app.use(express.json());

// Serve static files from public directory
app.use(express.static(join(__dirname, '../public')));

// Request logging middleware
app.use((req, _res, next) => {
  console.log(`[${req.method}] ${req.url}`);
  next();
});

// Global state
let cachedInstallation: any = null;
let configPath = join(__dirname, '../configValies');
const bindingManager = new BindingManager();
let uploader: BindingUploader | null = null;

// Hardware communication services (can be Mock or TCP)
let canBus: CANBusService = new MockCANBusService();
let nodeNaming = new NodeNamingService(canBus);
let connectionType: 'mock' | 'tcp' = 'mock';
let hardwareConnected = false;

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

// ==================== Installation APIs ====================

/**
 * API: Get complete installation data
 */
app.get('/api/installation', async (_req, res) => {
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
    
    return res.json(node);
  } catch (error) {
    console.error('Error getting node:', error);
    return res.status(500).json({ error: 'Failed to get node' });
  }
});

/**
 * API: Reload installation configuration
 */
app.post('/api/reload', async (_req, res) => {
  try {
    invalidateInstallationCache();
    const data = await loadInstallation();
    // Reload binding files into manager
    await loadBindingFiles();
    res.json({ 
      success: true, 
      summary: data.summary,
      bindings: bindingManager.getStatistics()
    });
  } catch (error) {
    console.error('Error reloading:', error);
    res.status(500).json({ error: 'Failed to reload configuration' });
  }
});

/**
 * API: Rename a node
 */
app.post('/api/nodes/:nodeAddress/rename', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const { name, pushToHardware = false } = req.body;
    
    if (!name || typeof name !== 'string') {
      return res.status(400).json({ error: 'Name required' });
    }
    
    // Update nodedatabase.txt
    const nodeDatabasePath = join(configPath, 'Config', 'nodedatabase.txt');
    NodeDatabaseWriter.updateNodeName(nodeDatabasePath, nodeAddress, name);
    
    // Optionally push to hardware
    let hardwareResult = null;
    if (pushToHardware) {
      try {
        if (!hardwareConnected) {
          await canBus.connect();
          hardwareConnected = true;
        }
        hardwareResult = await nodeNaming.uploadNodeName(nodeAddress, name);
      } catch (error) {
        console.error('Hardware push failed:', error);
        hardwareResult = {
          success: false,
          error: error instanceof Error ? error.message : String(error),
        };
      }
    }
    
    // Invalidate cache so UI shows updated name
    invalidateInstallationCache();
    
    return res.json({
      success: true,
      message: 'Node renamed successfully',
      hardwarePush: hardwareResult,
    });
  } catch (error) {
    console.error('Error renaming node:', error);
    return res.status(500).json({ error: 'Failed to rename node' });
  }
});

/**
 * API: Rename a unit
 */
app.post('/api/nodes/:nodeAddress/units/:unitAddress/rename', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const unitAddress = parseInt(req.params.unitAddress, 16);
    const { name, pushToHardware = false } = req.body;
    
    if (!name || typeof name !== 'string') {
      return res.status(400).json({ error: 'Name required' });
    }
    
    // Update nodedatabase.txt
    const nodeDatabasePath = join(configPath, 'Config', 'nodedatabase.txt');
    NodeDatabaseWriter.updateUnitName(nodeDatabasePath, nodeAddress, unitAddress, name);
    
    // Optionally push to hardware
    let hardwareResult = null;
    if (pushToHardware) {
      try {
        if (!hardwareConnected) {
          await canBus.connect();
          hardwareConnected = true;
        }
        hardwareResult = await nodeNaming.uploadUnitName(nodeAddress, unitAddress, name);
      } catch (error) {
        console.error('Hardware push failed:', error);
        hardwareResult = {
          success: false,
          error: error instanceof Error ? error.message : String(error),
        };
      }
    }
    
    // Invalidate cache so UI shows updated name
    invalidateInstallationCache();
    
    return res.json({
      success: true,
      message: 'Unit renamed successfully',
      hardwarePush: hardwareResult,
    });
  } catch (error) {
    console.error('Error renaming unit:', error);
    return res.status(500).json({ error: 'Failed to rename unit' });
  }
});

/**
 * API: Push node name to hardware
 */
app.post('/api/nodes/:nodeAddress/push-name', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const installation = await loadInstallation();
    const node = installation.nodes.find((n: any) => n.address === nodeAddress);
    
    if (!node) {
      return res.status(404).json({ error: 'Node not found' });
    }
    
    if (!hardwareConnected) {
      await canBus.connect();
      hardwareConnected = true;
    }
    
    const result = await nodeNaming.uploadNodeName(nodeAddress, node.name);
    
    return res.json(result);
  } catch (error) {
    console.error('Error pushing node name:', error);
    return res.status(500).json({ error: 'Failed to push name to hardware' });
  }
});

/**
 * API: Push unit name to hardware
 */
app.post('/api/nodes/:nodeAddress/units/:unitAddress/push-name', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const unitAddress = parseInt(req.params.unitAddress, 16);
    const installation = await loadInstallation();
    const node = installation.nodes.find((n: any) => n.address === nodeAddress);
    
    if (!node) {
      return res.status(404).json({ error: 'Node not found' });
    }
    
    const unit = node.units.find((u: any) => u.address === unitAddress);
    if (!unit) {
      return res.status(404).json({ error: 'Unit not found' });
    }
    
    if (!hardwareConnected) {
      await canBus.connect();
      hardwareConnected = true;
    }
    
    const result = await nodeNaming.uploadUnitName(nodeAddress, unitAddress, unit.name);
    
    return res.json(result);
  } catch (error) {
    console.error('Error pushing unit name:', error);
    return res.status(500).json({ error: 'Failed to push name to hardware' });
  }
});

/**
 * API: Push all names to hardware
 */
app.post('/api/hardware/push-all-names', async (_req, res) => {
  try {
    const installation = await loadInstallation();
    
    if (!hardwareConnected) {
      await canBus.connect();
      hardwareConnected = true;
    }
    
    // Convert installation format to what NodeNamingService expects
    const nodes = installation.nodes.map((node: any) => ({
      address: node.address,
      name: node.name,
      units: node.units.map((unit: any) => ({
        address: unit.address,
        name: unit.name,
      })),
    }));
    
    const result = await nodeNaming.uploadAllNames(nodes);
    
    return res.json({
      success: result.errors.length === 0,
      ...result,
    });
  } catch (error) {
    console.error('Error pushing all names:', error);
    return res.status(500).json({ error: 'Failed to push names to hardware' });
  }
});

/**
 * API: Get hardware connection status
 */
app.get('/api/hardware/status', (_req, res) => {
  res.json({
    connected: hardwareConnected,
    type: connectionType,
    canBusConnected: (canBus as any).isConnectedAndLoggedIn ? (canBus as any).isConnectedAndLoggedIn() : canBus.isConnected(),
  });
});

/**
 * API: Connect to hardware (TCP or Mock)
 * Body: { type: 'mock' | 'tcp', host?: string, port?: number, password?: string }
 */
app.post('/api/hardware/connect', async (req, res) => {
  try {
    const { type = 'mock', host, port, password } = req.body;

    // Disconnect from current service if connected
    if (hardwareConnected) {
      await canBus.disconnect();
      hardwareConnected = false;
    }

    // Switch CAN bus service type
    if (type === 'tcp') {
      if (!host || !port) {
        return res.status(400).json({ error: 'TCP connection requires host and port' });
      }

      console.log(`[Server] Switching to TCP CAN bus: ${host}:${port}`);
      const tcpService = new TCPCANBusService();
      tcpService.setConfig({ host, port, password: password || '' });
      
      // Connect and login to smartbox
      await tcpService.connect();
      const loginSuccess = await tcpService.login();
      
      if (!loginSuccess) {
        throw new Error('Login failed');
      }

      // Switch to TCP service
      canBus = tcpService;
      nodeNaming = new NodeNamingService(canBus);
      connectionType = 'tcp';
      hardwareConnected = true;

      return res.json({ 
        success: true, 
        connected: true, 
        type: 'tcp',
        host,
        port
      });

    } else {
      // Mock mode
      console.log('[Server] Switching to Mock CAN bus');
      canBus = new MockCANBusService();
      nodeNaming = new NodeNamingService(canBus);
      connectionType = 'mock';

      await canBus.connect();
      hardwareConnected = true;

      return res.json({ 
        success: true, 
        connected: true, 
        type: 'mock'
      });
    }
  } catch (error: any) {
    console.error('Failed to connect to hardware:', error);
    hardwareConnected = false;
    return res.status(500).json({ error: error.message || 'Failed to connect to hardware' });
  }
});

/**
 * API: Disconnect from hardware
 */
app.post('/api/hardware/disconnect', async (_req, res) => {
  try {
    if (hardwareConnected) {
      await canBus.disconnect();
      hardwareConnected = false;
    }
    res.json({ success: true, connected: false });
  } catch (error) {
    console.error('Failed to disconnect from hardware:', error);
    res.status(500).json({ error: 'Failed to disconnect from hardware' });
  }
});

/**
 * API: Upload a single node name
 * Body: { nodeAddress: number, name: string }
 */
app.post('/api/hardware/node-name', async (req, res) => {
  try {
    if (!canBus.isConnected()) {
      return res.status(400).json({ error: 'Not connected to hardware' });
    }

    const { nodeAddress, name } = req.body;
    
    if (!nodeAddress || !name) {
      return res.status(400).json({ error: 'Missing required fields: nodeAddress, name' });
    }

    const result = await nodeNaming.uploadNodeName(nodeAddress, name);
    
    return res.json({
      success: result.success,
      message: result.success ? 'Node name uploaded successfully' : 'Failed to upload node name',
      error: result.error,
    });
  } catch (error) {
    console.error('Error uploading node name:', error);
    return res.status(500).json({ error: 'Failed to upload node name' });
  }
});

/**
 * API: Upload a single unit name
 * Body: { nodeAddress: number, unitAddress: number, name: string }
 */
app.post('/api/hardware/unit-name', async (req, res) => {
  try {
    if (!canBus.isConnected()) {
      return res.status(400).json({ error: 'Not connected to hardware' });
    }

    const { nodeAddress, unitAddress, name } = req.body;
    
    if (!nodeAddress || !unitAddress || !name) {
      return res.status(400).json({ error: 'Missing required fields: nodeAddress, unitAddress, name' });
    }

    const result = await nodeNaming.uploadUnitName(nodeAddress, unitAddress, name);
    
    return res.json({
      success: result.success,
      message: result.success ? 'Unit name uploaded successfully' : 'Failed to upload unit name',
      error: result.error,
    });
  } catch (error) {
    console.error('Error uploading unit name:', error);
    return res.status(500).json({ error: 'Failed to upload unit name' });
  }
});

// ==================== Binding Editor APIs ====================

/**
 * API: Get modified nodes list
 * NOTE: This MUST come before /api/bindings/:nodeAddress to avoid matching "modified" as a nodeAddress
 */
app.get('/api/bindings/modified', (_req, res) => {
  try {
    const modified = bindingManager.getModifiedNodes();
    res.json({ modified });
  } catch (error) {
    res.status(500).json({ error: 'Failed to get modified nodes' });
  }
});

/**
 * API: Get all bindings for a node
 */
app.get('/api/bindings/:nodeAddress', (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const state = bindingManager.getState(nodeAddress);
    
    if (!state) {
      return res.status(404).json({ error: 'Node bindings not loaded' });
    }
    
    return res.json(state);
  } catch (error) {
    return res.status(500).json({ error: 'Failed to get bindings' });
  }
});

/**
 * API: Add a new binding to a node
 * POST /api/bindings/:nodeAddress
 * Body: { bindingString: "..." }
 */
app.post('/api/bindings/:nodeAddress', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const { bindingString } = req.body;
    
    if (!bindingString) {
      return res.status(400).json({ error: 'bindingString required' });
    }
    
    const success = bindingManager.addBinding(nodeAddress, bindingString);
    
    if (!success) {
      return res.status(404).json({ error: 'Node not loaded' });
    }
    
    // Write to disk and invalidate cache so UI updates correctly
    await bindingManager.writeToFile(nodeAddress);
    invalidateInstallationCache();
    
    const state = bindingManager.getState(nodeAddress);
    return res.json({ success: true, state });
  } catch (error) {
    return res.status(500).json({ error: 'Failed to add binding' });
  }
});

/**
 * API: Update an existing binding
 * PUT /api/bindings/:nodeAddress/:bindingIndex
 * Body: { bindingString: "..." }
 */
app.put('/api/bindings/:nodeAddress/:bindingIndex', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const bindingIndex = parseInt(req.params.bindingIndex);
    const { bindingString } = req.body;
    
    if (!bindingString) {
      return res.status(400).json({ error: 'bindingString required' });
    }
    
    const success = bindingManager.updateBinding(nodeAddress, bindingIndex, bindingString);
    
    if (!success) {
      return res.status(404).json({ error: 'Node or binding index not found' });
    }
    
    // Write to disk and invalidate cache so UI updates correctly
    await bindingManager.writeToFile(nodeAddress);
    invalidateInstallationCache();
    
    const state = bindingManager.getState(nodeAddress);
    return res.json({ success: true, state });
  } catch (error) {
    return res.status(500).json({ error: 'Failed to update binding' });
  }
});

/**
 * API: Delete a binding
 * DELETE /api/bindings/:nodeAddress/:bindingIndex
 */
app.delete('/api/bindings/:nodeAddress/:bindingIndex', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const bindingIndex = parseInt(req.params.bindingIndex);
    
    const success = bindingManager.deleteBinding(nodeAddress, bindingIndex);
    
    if (!success) {
      return res.status(404).json({ error: 'Node or binding index not found' });
    }
    
    // Write to disk and invalidate cache so UI updates correctly
    await bindingManager.writeToFile(nodeAddress);
    invalidateInstallationCache();
    
    const state = bindingManager.getState(nodeAddress);
    return res.json({ success: true, state });
  } catch (error) {
    return res.status(500).json({ error: 'Failed to delete binding' });
  }
});

/**
 * API: Save bindings to filesystem
 * POST /api/bindings/:nodeAddress/save
 */
app.post('/api/bindings/:nodeAddress/save', async (req, res) => {
  try {
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    await bindingManager.writeToFile(nodeAddress);
    // Explicitly invalidate cache after save
    invalidateInstallationCache();
    
    const state = bindingManager.getState(nodeAddress);
    return res.json({ success: true, state });
  } catch (error) {
    return res.status(500).json({ 
      error: error instanceof Error ? error.message : 'Failed to save bindings' 
    });
  }
});

/**
 * API: Save all modified bindings to filesystem
 * POST /api/bindings/save-all
 */
app.post('/api/bindings/save-all', async (_req, res) => {
  try {
    const saved = await bindingManager.writeAllModified();
    return res.json({ success: true, savedNodes: saved });
  } catch (error) {
    return res.status(500).json({ 
      error: error instanceof Error ? error.message : 'Failed to save bindings' 
    });
  }
});

// ==================== Binding Upload APIs ====================

/**
 * API: Connect to master node
 * POST /api/upload/connect
 * Body: { host: "192.168.0.97", port: 5001, password?: "" }
 */
app.post('/api/upload/connect', async (req, res) => {
  try {
    const { host, port, password } = req.body;
    
    if (!host || !port) {
      return res.status(400).json({ error: 'host and port required' });
    }
    
    if (uploader) {
      uploader.disconnect();
    }
    
    uploader = new BindingUploader(host, port, password || '');
    await uploader.connect();
    
    return res.json({ success: true, connected: true });
  } catch (error) {
    return res.status(500).json({ 
      error: error instanceof Error ? error.message : 'Connection failed' 
    });
  }
});

/**
 * API: Disconnect from master node
 * POST /api/upload/disconnect
 */
app.post('/api/upload/disconnect', (_req, res) => {
  try {
    if (uploader) {
      uploader.disconnect();
      uploader = null;
    }
    res.json({ success: true, connected: false });
  } catch (error) {
    res.status(500).json({ error: 'Disconnect failed' });
  }
});

/**
 * API: Upload bindings to a single node
 * POST /api/upload/node/:nodeAddress
 */
app.post('/api/upload/node/:nodeAddress', async (req, res) => {
  try {
    if (!uploader) {
      return res.status(400).json({ error: 'Not connected to master node' });
    }
    
    const nodeAddress = parseInt(req.params.nodeAddress, 16);
    const result = await uploader.uploadToNode(nodeAddress, bindingManager);
    
    return res.json(result);
  } catch (error) {
    return res.status(500).json({ 
      error: error instanceof Error ? error.message : 'Upload failed' 
    });
  }
});

/**
 * API: Upload all bindings (force)
 * POST /api/upload/all
 */
app.post('/api/upload/all', async (_req, res) => {
  try {
    if (!uploader) {
      return res.status(400).json({ error: 'Not connected to master node' });
    }
    
    const results = await uploader.uploadAll(bindingManager);
    
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
 * POST /api/upload/changed
 */
app.post('/api/upload/changed', async (_req, res) => {
  try {
    if (!uploader) {
      return res.status(400).json({ error: 'Not connected to master node' });
    }
    
    const results = await uploader.uploadChanged(bindingManager);
    
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
 * POST /api/upload/modified
 */
app.post('/api/upload/modified', async (_req, res) => {
  try {
    if (!uploader) {
      return res.status(400).json({ error: 'Not connected to master node' });
    }
    
    const results = await uploader.uploadModified(bindingManager);
    
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

// ==================== Binding Editor API ====================

// Mount the binding editor API routes
app.use('/api/editor', bindingEditorAPI);

// ==================== Statistics API ====================

/**
 * API: Get binding manager statistics
 */
app.get('/api/stats', (_req, res) => {
  try {
    const stats = bindingManager.getStatistics();
    res.json(stats);
  } catch (error) {
    res.status(500).json({ error: 'Failed to get statistics' });
  }
});

// ==================== Server Startup ====================

// Prevent process from exiting unexpectedly
process.on('exit', (code) => {
  console.log(`\n⚠️  Process exiting with code: ${code}`);
});

process.on('SIGINT', () => {
  console.log('\n👋 Received SIGINT, shutting down gracefully...');
  process.exit(0);
});

process.on('uncaughtException', (error) => {
  console.error('\n❌ Uncaught Exception:', error);
});

process.on('unhandledRejection', (reason, promise) => {
  console.error('\n❌ Unhandled Rejection at:', promise, 'reason:', reason);
});

// Start server
const server = app.listen(PORT, () => {
  console.log(`\n🌐 Duotecno Configuration Server`);
  console.log(`   http://localhost:${PORT}`);
  console.log(`   Config: ${configPath}\n`);
});

// Pre-load installation and bindings after server starts
server.on('listening', async () => {
  try {
    await loadInstallation();
    await loadBindingFiles();
    console.log('\n✅ Server ready and listening for requests\n');
  } catch (error) {
    console.error('❌ Error during startup:', error);
  }
});
