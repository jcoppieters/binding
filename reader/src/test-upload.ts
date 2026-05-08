/**
 * Test script to upload binding files to a Duotecno master node
 * 
 * IMPORTANT PROTOCOL REQUIREMENTS:
 * 1. Connect with TCP/IP password (if required)
 * 2. Set access level (Installer = '2222')
 * 3. Enable APPL_UNLOCK flag on EACH node individually before writing bindings
 *    - This is done per-node, NOT broadcasted to all nodes
 *    - Node will refuse operations with MESSAGE_ERROR_REFUSED_RUNNING_STATE (0x00) without this
 * 4. Write binding file (erase, open, write entries, close)
 * 5. Repeat steps 3-4 for each additional node
 * 
 * Based on C++ analysis:
 * - FC_NODECONFIG (0x94) with APPL_UNLOCK (0x02) must be sent to each node
 * - BROADCAST_NODE (0x00) is only for PnP operations, not for FC_NODECONFIG
 * - See src/DTBindingSoftware/src/TXNodeMessage.cpp::SetNodeConfig()
 * 
 * Usage:
 *   tsx src/test-upload.ts
 */

import { BindingWriterFSM } from './communication/BindingWriterFSM';
import { BindingFileReader } from './readers/BindingFileReader';
import * as path from 'path';
import * as os from 'os';

// Configuration
const MASTER_NODE_IP = '192.168.0.97';
const MASTER_NODE_PORT = 5001;
const PASSWORD = 'GMTecno'; // TCP/IP login password (found via testing)
const TASK_PASSWORD = '1234'; // 4-digit task password (try: 1234, 0000, 1111)
const ACCESS_PASSWORD = '2222'; // Access level password: '1111'=User, '2222'=Installer

// Path to binding files
const CONFIG_PATH = path.join(
  os.homedir(),
  'Library/Mobile Documents/com~apple~CloudDocs/Projects/Duotecno/Config - sb1+switch'
);

// Node addresses and their binding files
// NOTE: APPL_UNLOCK will be sent to each node individually before uploading
const NODES = [
  { address: 0x03, file: 'bind03.txt' },  // Node 03 - has 12 bindings
  // { address: 0x04, file: 'bind04.txt' },  // Node 04
  // { address: 0x05, file: 'bind05.txt' },  // Node 05
  // { address: 0xFC, file: 'bindfc.txt' },  // Node FC (master LCD)
];

async function main() {
  console.log('='.repeat(60));
  console.log('Duotecno Binding Upload Test');
  console.log('='.repeat(60));
  console.log(`Master node: ${MASTER_NODE_IP}:${MASTER_NODE_PORT}`);
  console.log(`Config path: ${CONFIG_PATH}`);
  console.log('='.repeat(60));
  
  const writer = new BindingWriterFSM(MASTER_NODE_IP, MASTER_NODE_PORT, PASSWORD, TASK_PASSWORD, ACCESS_PASSWORD);
  
  try {
    // Connect to master node
    console.log('\nConnecting to master node...');
    await writer.connect();
    console.log('Connected!');
    
    // Upload bindings to each node
    for (const node of NODES) {
      const bindingFilePath = path.join(CONFIG_PATH, node.file);
      
      console.log('\n' + '='.repeat(60));
      console.log(`Node 0x${node.address.toString(16).toUpperCase()}: ${node.file}`);
      console.log('='.repeat(60));
      
      try {
        // Read the binding file
        console.log(`Reading binding file: ${bindingFilePath}`);
        const bindingFile = await BindingFileReader.readFile(bindingFilePath);
        
        console.log(`Found ${bindingFile.bindings.length} bindings`);
        
        // CRITICAL: Enable programming mode for this specific node
        // This sets APPL_UNLOCK flag which is required before any binding operations
        console.log(`Enabling programming mode for node 0x${node.address.toString(16).toUpperCase()}...`);
        await writer.getConnection().setNodeConfig(node.address, 0x00, 1);
        
        // Upload to node
        await writer.writeBindingsToNode(node.address, bindingFile);
        
        console.log(`✓ Successfully uploaded to node 0x${node.address.toString(16).toUpperCase()}`);
      } catch (error) {
        console.error(`✗ Failed to upload to node 0x${node.address.toString(16).toUpperCase()}:`, error);
        // Continue with next node
      }
    }
    
    console.log('\n' + '='.repeat(60));
    console.log('Upload complete!');
    console.log('='.repeat(60));
    
  } catch (error) {
    console.error('Error:', error);
  } finally {
    // Disconnect
    console.log('\nDisconnecting...');
    writer.disconnect();
  }
}

// Run the test
main().catch(console.error);
