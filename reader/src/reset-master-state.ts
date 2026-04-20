/**
 * Reset master node to normal operating state
 * Use this to clear APPL_UNLOCK and stop excessive CAN bus messages
 * 
 * This performs a SOFT RESET which clears runtime state without erasing configuration
 */

import { TcpConnection } from './communication/TcpConnection';

const MASTER_NODE_IP = '192.168.0.97';
const MASTER_NODE_PORT = 5001;
const PASSWORD = 'GMTecno';

async function resetMasterState() {
  console.log('='.repeat(60));
  console.log('Resetting Master Node to Normal State');
  console.log('='.repeat(60));
  console.log('This will perform a SOFT RESET to clear stuck states');
  console.log('Configuration and bindings will NOT be erased');
  console.log('='.repeat(60));
  
  const connection = new TcpConnection(MASTER_NODE_IP, MASTER_NODE_PORT, PASSWORD);
  
  try {
    // Connect
    console.log('\nConnecting to master node...');
    await connection.connect();
    console.log('✓ Connected\n');
    
    // Give it a moment to stabilize
    await new Promise(resolve => setTimeout(resolve, 500));
    
    // Perform soft reset on master node (0xFC)
    console.log('Performing soft reset on master node (0xFC)...');
    await connection.resetNode(0xFC);
    console.log('✓ Master node reset complete\n');
    
    // Give master time to restart properly
    console.log('Waiting for master to stabilize (3 seconds)...');
    await new Promise(resolve => setTimeout(resolve, 3000));
    
    console.log('✓ State reset complete');
    console.log('='.repeat(60));
    console.log('Check your proxy logs - cmd=122 messages should stop now');
    console.log('='.repeat(60));
    
  } catch (error) {
    console.error('Error:', error);
  } finally {
    // Disconnect cleanly
    console.log('\nDisconnecting...');
    connection.disconnect();
    
    // Give disconnect time to complete
    await new Promise(resolve => setTimeout(resolve, 500));
    process.exit(0);
  }
}

resetMasterState().catch(console.error);
