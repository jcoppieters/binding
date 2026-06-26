/**
 * Emergency script to restore master node configuration
 * 
 * The master LCD node (0xFC) requires NODE_MASTER_LCD (0x08) flag
 * to function correctly. This script restores the correct configuration.
 */

import { TcpConnection } from './communication/TcpConnection';

const MASTER_NODE_IP = '192.168.0.97';
const MASTER_NODE_PORT = 5001;
const PASSWORD = 'GMTecno';
const ACCESS_PASSWORD = '2222'; // Installer level

// Node configuration flags (from nodegenestructs.h)
const NODE_MASTER_LCD = 0x08;

async function fixMasterNode() {
  console.log('============================================================');
  console.log('Master Node Recovery Tool');
  console.log('============================================================');
  console.log(`Master node: ${MASTER_NODE_IP}:${MASTER_NODE_PORT}`);
  console.log('');

  const connection = new TcpConnection(MASTER_NODE_IP, MASTER_NODE_PORT, PASSWORD);

  try {
    // Connect
    console.log('Connecting to master node...');
    await connection.connect();
    console.log('Connected!\n');

    // Subscribe to CAN bus to receive responses
    console.log('Subscribing to CAN bus messages...');
    connection.subscribeToCanBus();
    await new Promise(resolve => setTimeout(resolve, 500));

    // Set access level to Installer
    console.log('Setting access level (Installer)...');
    const level = await connection.setAccessLevel(ACCESS_PASSWORD);
    const levelNames = ['No authority', 'User', 'Installer', 'Engineer'];
    console.log(`✓ Access level: ${levelNames[level]}\n`);

    // Wait a moment
    await new Promise(resolve => setTimeout(resolve, 200));

    // Restore master node configuration
    console.log('Restoring master node (0xFC) configuration...');
    console.log(`Setting NODE_MASTER_LCD flag (0x${NODE_MASTER_LCD.toString(16).toUpperCase()})...`);
    
    await connection.setNodeConfig(
      0xFC,              // Master node address
      NODE_MASTER_LCD,   // Node config: 0x08 = NODE_MASTER_LCD flag
      false              // Clear APPL_UNLOCK (return to normal operation)
    );
    
    console.log('✓ Master node configuration restored!\n');

    // Verify by requesting config again (optional)
    console.log('Verifying configuration...');
    await new Promise(resolve => setTimeout(resolve, 500));
    
    console.log('\n============================================================');
    console.log('✓ Master node recovery complete!');
    console.log('============================================================');
    console.log('');
    console.log('The master LCD should now function correctly.');
    console.log('Please verify by checking if the LCD responds normally.');
    console.log('');

  } catch (error) {
    console.error('\n✗ Error during recovery:', error);
    throw error;
  } finally {
    // Cleanup
    setTimeout(() => {
      process.exit(0);
    }, 1000);
  }
}

// Run the recovery
fixMasterNode().catch(err => {
  console.error('Fatal error:', err);
  process.exit(1);
});
