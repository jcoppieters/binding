/**
 * Complete installation model
 * Combines all configuration files into a single view
 */

import { BindingFile } from './binding.js';
import { NodeDatabase } from './node.js';
import { BindingConfigDatabase } from './bindingConfig.js';

/**
 * Complete Duotecno installation configuration
 */
export interface Installation {
  // Core configuration
  nodeDatabase: NodeDatabase;
  bindingConfigInfo: BindingConfigDatabase;
  bindingFiles: Map<number, BindingFile>;  // Map: nodeAddress -> BindingFile
  
  // Additional configurations
  moodConfigs?: Map<number, any>;
  scheduleConfigs?: Map<number, any>;
  audioConfigs?: Map<number, any>;
  unitConfigs?: Map<number, any>;
  
  // Metadata
  configPath: string;
  loadedAt: Date;
}

/**
 * Installation summary for display
 */
export interface InstallationSummary {
  totalNodes: number;
  totalUnits: number;
  totalBindings: number;
  nodesByType: Record<string, number>;
  unitsByType: Record<string, number>;
  bindingsByType: Record<string, number>;
  errors: string[];
}
