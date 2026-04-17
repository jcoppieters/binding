#!/usr/bin/env node
/**
 * Duotecno Configuration Reader
 * Main entry point
 */

import { join } from 'path';
import { InstallationReader } from './readers/installation.js';

async function main() {
  const args = process.argv.slice(2);
  const configPath = args[0] || join(process.cwd(), 'configGM');
  
  try {
    // Read the installation
    const installation = await InstallationReader.read(configPath);
    
    // Print the report
    InstallationReader.printReport(installation);
    
    // Optionally export to JSON
    if (args.includes('--json')) {
      const jsonOutput = JSON.stringify(installation, null, 2);
      console.log('\n\n📤 JSON Export:');
      console.log(jsonOutput);
    }
    
  } catch (error) {
    console.error('\n❌ Error reading configuration:', error);
    process.exit(1);
  }
}

main();
