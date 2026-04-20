/**
 * Installation reader - combines all configuration files
 */

import { readdirSync } from 'fs';
import { join } from 'path';
import { Installation, InstallationSummary } from '../models/installation.js';
import { BindingFileParser } from '../parsers/bindingFile.js';
import { NodeDatabaseParser, NodeConfigParser } from '../parsers/nodeDatabase.js';
import { BindingConfigParser } from '../parsers/bindingConfig.js';
import { BindingFile } from '../models/binding.js';
import { formatBindingConnection } from '../utils/resolver.js';

export interface InstallationJSON {
  summary: InstallationSummary;
  nodes: any[];
  bindingFiles: any[];
  configPath: string;
  loadedAt: string;
}

export class InstallationReader {
  /**
   * Read complete installation from a config directory
   */
  static async read(configPath: string): Promise<Installation> {
    console.log(`\n📂 Reading Duotecno configuration from: ${configPath}\n`);
    
    // Parse node database
    console.log('📊 Parsing node database...');
    const nodeDatabasePath = join(configPath, 'Config', 'nodedatabase.txt');
    const nodes = NodeDatabaseParser.parse(nodeDatabasePath);
    
    console.log('⚙️  Parsing node configuration...');
    const nodeConfigPath = join(configPath, 'Config', 'nodeconfiginfo.txt');
    const nodeConfigs = NodeConfigParser.parse(nodeConfigPath);
    
    // Parse binding config info (human-readable)
    console.log('📋 Parsing binding configuration info...');
    const bindingConfigPath = join(configPath, 'Config', 'bindingconfiginfo.txt');
    const bindingConfigInfo = BindingConfigParser.parse(bindingConfigPath);
    
    // Parse all binding files
    console.log('🔗 Parsing binding files...');
    const bindingFiles = new Map<number, BindingFile>();
    const files = readdirSync(configPath);
    
    for (const file of files) {
      if (file.match(/^bind[0-9a-fA-F]{2}\.txt$/)) {
        const filePath = join(configPath, file);
        console.log(`  - ${file}`);
        const bindingFile = BindingFileParser.parse(filePath);
        bindingFiles.set(bindingFile.nodeAddress, bindingFile);
      }
    }
    
    const installation: Installation = {
      nodeDatabase: { nodes, nodeConfigs },
      bindingConfigInfo,
      bindingFiles,
      configPath,
      loadedAt: new Date(),
    };
    
    console.log('\n✅ Configuration loaded successfully!\n');
    
    return installation;
  }
  
  /**
   * Generate a summary of the installation
   */
  static getSummary(installation: Installation): InstallationSummary {
    const { nodeDatabase, bindingConfigInfo, bindingFiles } = installation;
    
    // Count totals
    const totalNodes = nodeDatabase.nodes.length;
    const totalUnits = nodeDatabase.nodes.reduce((sum, node) => sum + node.units.length, 0);
    const totalBindings = bindingConfigInfo.bindings.length;
    
    // Count by type
    const nodesByType: Record<string, number> = {};
    for (const node of nodeDatabase.nodes) {
      nodesByType[node.nodeType] = (nodesByType[node.nodeType] || 0) + 1;
    }
    
    const unitsByType: Record<string, number> = {};
    for (const node of nodeDatabase.nodes) {
      for (const unit of node.units) {
        unitsByType[unit.unitType] = (unitsByType[unit.unitType] || 0) + 1;
      }
    }
    
    const bindingsByType: Record<string, number> = {};
    for (const [_, file] of bindingFiles) {
      for (const entry of file.entries) {
        bindingsByType[entry.bindingType] = (bindingsByType[entry.bindingType] || 0) + 1;
      }
    }
    
    // Collect errors
    const errors: string[] = [];
    for (const [_, file] of bindingFiles) {
      if (file.errorCount > 0) {
        errors.push(`${file.fileName}: ${file.errorCount} errors`);
        errors.push(...file.errors.map(e => `  ${e}`));
      }
    }
    
    return {
      totalNodes,
      totalUnits,
      totalBindings,
      nodesByType,
      unitsByType,
      bindingsByType,
      errors,
    };
  }
  
  /**
   * Print a detailed installation report
   */
  static printReport(installation: Installation): void {
    const summary = this.getSummary(installation);
    
    console.log('='.repeat(80));
    console.log('DUOTECNO INSTALLATION REPORT');
    console.log('='.repeat(80));
    console.log(`Configuration Path: ${installation.configPath}`);
    console.log(`Loaded At: ${installation.loadedAt.toISOString()}`);
    console.log('='.repeat(80));
    
    // Summary
    console.log('\n📊 SUMMARY');
    console.log('-'.repeat(80));
    console.log(`Total Nodes:    ${summary.totalNodes}`);
    console.log(`Total Units:    ${summary.totalUnits}`);
    console.log(`Total Bindings: ${summary.totalBindings}`);
    
    // Nodes
    console.log('\n🖥️  NODES');
    console.log('-'.repeat(80));
    
    // Sort nodes by address for consistent display
    const sortedNodes = [...installation.nodeDatabase.nodes].sort((a, b) => a.nodeAddress - b.nodeAddress);
    
    for (const node of sortedNodes) {
      console.log(`\n[${node.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}] ${node.name}`);
      console.log(`    Type: ${node.nodeType}`);
      console.log(`    Physical Address: 0x${node.physicalAddress.toString(16).toUpperCase()}`);
      console.log(`    Software Version: ${node.softwareVersion}`);
      console.log(`    Units: ${node.numberOfUnits}`);
      
      if (node.units.length > 0) {
        console.log(`    Unit Details:`);
        for (const unit of node.units) {
          const addr = unit.address.map(a => a.toString(16).toUpperCase().padStart(2, '0')).join(':');
          console.log(`      [${addr}] ${unit.name} (${unit.unitType})`);
        }
      }
    }
    
    // Bindings
    console.log('\n🔗 BINDINGS');
    console.log('-'.repeat(80));
    console.log(`Binding types:`);
    for (const [type, count] of Object.entries(summary.bindingsByType)) {
      console.log(`  ${type}: ${count}`);
    }
    
    console.log(`\nBinding Details:`);
    for (const binding of installation.bindingConfigInfo.bindings) {
      console.log(`\n[0x${binding.bindingNumber.toString(16).toUpperCase().padStart(4, '0')}] ${binding.name}`);
      console.log(`  Expression: ${binding.evaluationString}`);
      console.log(`  Flags: 0x${binding.bindingFlags.toString(16).toUpperCase().padStart(2, '0')}`);
      
      if (binding.inputs.length > 0) {
        console.log(`  Inputs:`);
        for (const input of binding.inputs) {
          const addr = input.address.map(a => a.toString(16).toUpperCase().padStart(2, '0')).join(':');
          console.log(`    - ${input.name} [${addr}] Event: 0x${input.event.toString(16).toUpperCase()}`);
        }
      }
      
      if (binding.outputs.length > 0) {
        console.log(`  Outputs:`);
        for (const output of binding.outputs) {
          const addr = output.address.map(a => a.toString(16).toUpperCase().padStart(2, '0')).join(':');
          console.log(`    - ${output.name} [${addr}] Data: ${output.data}`);
        }
      }
      
      if (binding.timers && binding.timers.length > 0) {
        console.log(`  Timers:`);
        for (const timer of binding.timers) {
          console.log(`    - ${timer.name} (${timer.type}) Data: ${timer.data}`);
        }
      }
    }
    
    // Binding Connections (human-readable)
    console.log('\n🔀 BINDING CONNECTIONS');
    console.log('-'.repeat(80));
    const nodes = installation.nodeDatabase.nodes;
    
    // Sort binding files by node address
    const sortedBindingFiles = Array.from(installation.bindingFiles.entries())
      .sort((a, b) => a[0] - b[0]);
    
    for (const [nodeAddr, file] of sortedBindingFiles) {
      const nodeName = nodes.find(n => n.nodeAddress === nodeAddr)?.name || `Node 0x${nodeAddr.toString(16).toUpperCase()}`;
      console.log(`\n${nodeName} (${file.fileName}):`);
      
      for (const entry of file.entries) {
        if (entry.inputUnits && entry.inputUnits.length > 0 && 
            entry.outputUnits && entry.outputUnits.length > 0) {
          
          // Find matching binding config info for the name
          const configInfo = installation.bindingConfigInfo.bindings.find(
            b => b.bindingNumber === entry.bindingNumber
          );
          
          const bindingName = configInfo?.name || `Binding 0x${entry.bindingNumber.toString(16).toUpperCase()}`;
          const connection = formatBindingConnection(
            nodes,
            entry.inputUnits,
            entry.outputUnits,
            entry.operators?.map(op => op.toString())
          );
          
          console.log(`  [${entry.bindingType}] ${bindingName}`);
          console.log(`      ${connection}`);
        }
      }
    }
    
    // Binding Files (raw format)
    console.log('\n📄 BINDING FILES (Raw Protocol)');
    console.log('-'.repeat(80));
    
    // Sort binding files by node address
    const sortedBindingFilesRaw = Array.from(installation.bindingFiles.entries())
      .sort((a, b) => a[0] - b[0]);
    
    for (const [nodeAddr, file] of sortedBindingFilesRaw) {
      console.log(`\n${file.fileName} (Node 0x${nodeAddr.toString(16).toUpperCase().padStart(2, '0')})`);
      console.log(`  Entries: ${file.entries.length}`);
      if (file.errorCount > 0) {
        console.log(`  ⚠️  Errors: ${file.errorCount}`);
      }
      
      for (const entry of file.entries.slice(0, 5)) { // Show first 5
        console.log(`    0x${entry.bindingNumber.toString(16).toUpperCase().padStart(4, '0')} [${entry.bindingType}] ${entry.content.substring(0, 60)}${entry.content.length > 60 ? '...' : ''}`);
      }
      
      if (file.entries.length > 5) {
        console.log(`    ... and ${file.entries.length - 5} more`);
      }
    }
    
    // Errors
    if (summary.errors.length > 0) {
      console.log('\n⚠️  ERRORS');
      console.log('-'.repeat(80));
      for (const error of summary.errors) {
        console.log(error);
      }
    }
    
    console.log('\n' + '='.repeat(80));
  }

  /**
   * Export installation as JSON for web interface
   */
  static toJSON(installation: Installation): InstallationJSON {
    const summary = this.getSummary(installation);
    const nodes = installation.nodeDatabase.nodes;
    
    // Convert nodes to JSON-friendly format with binding info
    const nodesJSON = nodes.map(node => {
      const bindingFile = installation.bindingFiles.get(node.nodeAddress);
      const units = node.units.map(unit => {
        const unitAddress = unit.address;
        
        // Find bindings where this unit is an input
        const bindingsAsInput = bindingFile?.entries.filter(entry => 
          entry.inputUnits?.some(input => 
            input.nodeAddress === unitAddress[0] && 
            input.unitAddress === unitAddress[1]
          )
        ) || [];
        
        // Find bindings where this unit is an output
        const bindingsAsOutput = bindingFile?.entries.filter(entry => 
          entry.outputUnits?.some(output => 
            output.nodeAddress === unitAddress[0] && 
            output.unitAddress === unitAddress[1]
          )
        ) || [];
        
        // Find bindings from other nodes that reference this unit
        const bindingsFromOtherNodes: any[] = [];
        for (const [otherNodeAddr, otherFile] of installation.bindingFiles) {
          if (otherNodeAddr !== node.nodeAddress) {
            const entries = otherFile.entries.filter(entry =>
              entry.inputUnits?.some(input => 
                input.nodeAddress === unitAddress[0] && 
                input.unitAddress === unitAddress[1]
              ) ||
              entry.outputUnits?.some(output => 
                output.nodeAddress === unitAddress[0] && 
                output.unitAddress === unitAddress[1]
              )
            );
            
            for (const entry of entries) {
              bindingsFromOtherNodes.push({
                nodeAddress: otherNodeAddr,
                bindingNumber: entry.bindingNumber,
                bindingType: entry.bindingType,
                inputUnits: entry.inputUnits,
                outputUnits: entry.outputUnits,
                operators: entry.operators,
              });
            }
          }
        }
        
        return {
          address: unitAddress,
          name: unit.name,
          unitType: unit.unitType,
          hasBindings: bindingsAsInput.length > 0 || bindingsAsOutput.length > 0 || bindingsFromOtherNodes.length > 0,
          bindingsAsInput: bindingsAsInput.map(e => ({
            bindingNumber: e.bindingNumber,
            bindingType: e.bindingType,
            inputUnits: e.inputUnits,
            outputUnits: e.outputUnits,
            operators: e.operators,
          })),
          bindingsAsOutput: bindingsAsOutput.map(e => ({
            bindingNumber: e.bindingNumber,
            bindingType: e.bindingType,
            inputUnits: e.inputUnits,
            outputUnits: e.outputUnits,
            operators: e.operators,
          })),
          bindingsFromOtherNodes,
        };
      });
      
      return {
        nodeAddress: node.nodeAddress,
        name: node.name,
        nodeType: node.nodeType,
        physicalAddress: node.physicalAddress,
        softwareVersion: node.softwareVersion,
        numberOfUnits: node.numberOfUnits,
        units,
      };
    });
    
    // Sort nodes by node address
    nodesJSON.sort((a, b) => a.nodeAddress - b.nodeAddress);
    
    // Convert binding files to JSON
    const bindingFilesJSON = Array.from(installation.bindingFiles.entries()).map(([nodeAddr, file]) => ({
      nodeAddress: nodeAddr,
      fileName: file.fileName,
      entries: file.entries.map(entry => ({
        nodeAddress: entry.nodeAddress,
        bindingNumber: entry.bindingNumber,
        bindingType: entry.bindingType,
        bindingFlags: entry.bindingFlags,
        content: entry.content,
        inputUnits: entry.inputUnits,
        outputUnits: entry.outputUnits,
        operators: entry.operators,
        expression: entry.expression,
      })),
    }));
    
    // Sort binding files by node address
    bindingFilesJSON.sort((a, b) => a.nodeAddress - b.nodeAddress);
    
    return {
      summary,
      nodes: nodesJSON,
      bindingFiles: bindingFilesJSON,
      configPath: installation.configPath,
      loadedAt: installation.loadedAt.toISOString(),
    };
  }
}
