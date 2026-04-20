/**
 * Example: Using the Binding Editor & Uploader programmatically
 */

import { BindingFileReader } from './readers/BindingFileReader.js';
import { BindingManager } from './services/BindingManager.js';
import { BindingUploader } from './communication/BindingUploader.js';

async function main() {
  console.log('🔧 Duotecno Binding Editor Example\n');

  // ==================== 1. Load bindings into manager ====================
  console.log('1️⃣  Loading binding files...');
  const manager = new BindingManager();

  // Load binding file for node 0x03
  const bindingFile = await BindingFileReader.readFile('./configGM/bind03.txt');
  manager.load(0x03, bindingFile, './configGM/bind03.txt');

  console.log(`   ✓ Loaded ${bindingFile.bindings.length} bindings for node 0x03\n`);

  // ==================== 2. Edit bindings ====================
  console.log('2️⃣  Editing bindings...');

  // Get current bindings
  const bindings = manager.getBindings(0x03);
  console.log(`   Original binding count: ${bindings?.length}`);

  // Add a new binding
  const newBinding = '000003_01_0099_GA000003U99E02=U09FA6D0102S';
  manager.addBinding(0x03, newBinding);
  console.log(`   ✓ Added new binding`);

  // Update an existing binding
  if (bindings && bindings.length > 0) {
    const updated = bindings[0].replace('0009', '0010');
    manager.updateBinding(0x03, 0, updated);
    console.log(`   ✓ Updated binding #0`);
  }

  // Check if modified
  console.log(`   Modified: ${manager.isModified(0x03)}`);
  console.log(`   New binding count: ${manager.getBindings(0x03)?.length}\n`);

  // ==================== 3. Calculate and verify checksum ====================
  console.log('3️⃣  Calculating checksum...');
  const checksum = manager.calculateChecksum(0x03);
  console.log(`   XOR Checksum: 0x${checksum?.toString(16).padStart(2, '0')}\n`);

  // ==================== 4. Save to filesystem ====================
  console.log('4️⃣  Saving to filesystem...');
  await manager.writeToFile(0x03, './configGM/bind03_modified.txt');
  console.log(`   ✓ Saved to bind03_modified.txt\n`);

  // ==================== 5. Upload to nodes ====================
  console.log('5️⃣  Uploading to nodes...');

  // Connect to master node
  const uploader = new BindingUploader('192.168.0.97', 5001);
  
  try {
    console.log('   Connecting to 192.168.0.97:5001...');
    await uploader.connect();
    console.log('   ✓ Connected\n');

    // Request current file info from node
    console.log('   Checking current state on node...');
    const fileInfo = await uploader.requestFileInfo(0x03);
    console.log(`   Node has: ${fileInfo.entryCount} bindings, checksum 0x${fileInfo.xorChecksum.toString(16)}`);
    console.log(`   Local has: ${manager.getBindings(0x03)?.length} bindings, checksum 0x${checksum?.toString(16)}\n`);

    // Check if upload needed
    const needsUpload = manager.needsUpload(0x03, fileInfo.xorChecksum, fileInfo.entryCount);
    console.log(`   Needs upload: ${needsUpload}\n`);

    if (needsUpload) {
      console.log('   Uploading bindings...');
      const result = await uploader.uploadToNode(0x03, manager);
      
      if (result.success) {
        console.log(`   ✓ Successfully uploaded ${result.bindingsWritten} bindings`);
      } else {
        console.error(`   ✗ Upload failed: ${result.error}`);
      }
    } else {
      console.log('   ✓ Node is already up to date, skipping upload');
    }

  } catch (error) {
    console.error(`   ✗ Error: ${error instanceof Error ? error.message : String(error)}`);
  } finally {
    uploader.disconnect();
    console.log('\n   Disconnected');
  }

  // ==================== 6. Show statistics ====================
  console.log('\n6️⃣  Final statistics:');
  const stats = manager.getStatistics();
  console.log(`   Total nodes: ${stats.totalNodes}`);
  console.log(`   Modified nodes: ${stats.modifiedNodes}`);
  console.log(`   Total bindings: ${stats.totalBindings}`);
  console.log(`   Total edits: ${stats.totalEdits}`);

  console.log('\n✅ Done!\n');
}

// Run if executed directly
if (import.meta.url === `file://${process.argv[1]}`) {
  main().catch(console.error);
}

export { main };
