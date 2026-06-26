/**
 * Binding Manager - Manages binding files in memory with edit capabilities
 */

import { NodeBindingFile } from '../readers/BindingFileReader';
import { writeFile, mkdir } from 'fs/promises';
import { dirname } from 'path';

export interface BindingEdit {
  nodeAddress: number;
  bindingIndex: number;
  oldValue: string;
  newValue: string;
  timestamp: Date;
}

export interface NodeBindingState {
  nodeAddress: number;
  filename: string;
  filepath: string;
  bindings: string[];
  modified: boolean;
  modifiedTimestamp?: Date;
  edits: BindingEdit[];
  // For change detection
  uploadedChecksum?: number;
  uploadedCount?: number;
}

export class BindingManager {
  private bindingStates: Map<number, NodeBindingState> = new Map();

  /**
   * Load bindings from a NodeBindingFile
   */
  load(nodeAddress: number, file: NodeBindingFile, filepath: string): void {
    this.bindingStates.set(nodeAddress, {
      nodeAddress,
      filename: file.fileName,
      filepath,
      bindings: [...file.bindings], // clone array
      modified: false,
      edits: [],
    });
  }

  /**
   * Get all loaded nodes
   */
  getLoadedNodes(): number[] {
    return Array.from(this.bindingStates.keys());
  }

  /**
   * Get bindings for a specific node
   */
  getBindings(nodeAddress: number): string[] | null {
    const state = this.bindingStates.get(nodeAddress);
    return state ? [...state.bindings] : null;
  }

  /**
   * Get full state for a specific node
   */
  getState(nodeAddress: number): NodeBindingState | null {
    const state = this.bindingStates.get(nodeAddress);
    return state ? { ...state, bindings: [...state.bindings] } : null;
  }

  /**
   * Get all modified nodes
   */
  getModifiedNodes(): number[] {
    return Array.from(this.bindingStates.values())
      .filter(state => state.modified)
      .map(state => state.nodeAddress);
  }

  /**
   * Check if a node has modifications
   */
  isModified(nodeAddress: number): boolean {
    return this.bindingStates.get(nodeAddress)?.modified ?? false;
  }

  /**
   * Add a new binding to a node
   */
  addBinding(nodeAddress: number, bindingString: string): boolean {
    const state = this.bindingStates.get(nodeAddress);
    if (!state) return false;

    state.bindings.push(bindingString);
    state.modified = true;
    state.modifiedTimestamp = new Date();
    state.edits.push({
      nodeAddress,
      bindingIndex: state.bindings.length - 1,
      oldValue: '',
      newValue: bindingString,
      timestamp: new Date(),
    });

    return true;
  }

  /**
   * Update an existing binding
   */
  updateBinding(nodeAddress: number, bindingIndex: number, newBindingString: string): boolean {
    const state = this.bindingStates.get(nodeAddress);
    if (!state || bindingIndex < 0 || bindingIndex >= state.bindings.length) {
      return false;
    }

    const oldValue = state.bindings[bindingIndex];
    if (oldValue === newBindingString) {
      return true; // No change
    }

    state.bindings[bindingIndex] = newBindingString;
    state.modified = true;
    state.modifiedTimestamp = new Date();
    state.edits.push({
      nodeAddress,
      bindingIndex,
      oldValue,
      newValue: newBindingString,
      timestamp: new Date(),
    });

    return true;
  }

  /**
   * Delete a binding
   */
  deleteBinding(nodeAddress: number, bindingIndex: number): boolean {
    const state = this.bindingStates.get(nodeAddress);
    if (!state || bindingIndex < 0 || bindingIndex >= state.bindings.length) {
      return false;
    }

    const oldValue = state.bindings[bindingIndex];
    state.bindings.splice(bindingIndex, 1);
    state.modified = true;
    state.modifiedTimestamp = new Date();
    state.edits.push({
      nodeAddress,
      bindingIndex,
      oldValue,
      newValue: '',
      timestamp: new Date(),
    });

    return true;
  }

  /**
   * Reorder bindings (move a binding from one index to another)
   */
  reorderBinding(nodeAddress: number, fromIndex: number, toIndex: number): boolean {
    const state = this.bindingStates.get(nodeAddress);
    if (!state || fromIndex < 0 || fromIndex >= state.bindings.length ||
        toIndex < 0 || toIndex >= state.bindings.length) {
      return false;
    }

    const [binding] = state.bindings.splice(fromIndex, 1);
    state.bindings.splice(toIndex, 0, binding);
    state.modified = true;
    state.modifiedTimestamp = new Date();

    return true;
  }

  /**
   * Clear all modifications for a node (revert to last saved)
   */
  revertNode(nodeAddress: number): boolean {
    // This would require storing original state - for now just mark as unmodified
    const state = this.bindingStates.get(nodeAddress);
    if (!state) return false;

    state.modified = false;
    state.edits = [];
    return true;
  }

  /**
   * Write bindings to filesystem
   */
  async writeToFile(nodeAddress: number, outputPath?: string): Promise<void> {
    const state = this.bindingStates.get(nodeAddress);
    if (!state) {
      throw new Error(`Node 0x${nodeAddress.toString(16)} not loaded`);
    }

    const filepath = outputPath || state.filepath;
    
    // Ensure directory exists
    await mkdir(dirname(filepath), { recursive: true });

    // Write bindings (one per line)
    const content = state.bindings.join('\n') + '\n';
    await writeFile(filepath, content, 'utf-8');

    // Mark as saved
    state.modified = false;
    state.filepath = filepath;
  }

  /**
   * Write all modified bindings to filesystem
   */
  async writeAllModified(): Promise<number[]> {
    const modified = this.getModifiedNodes();
    for (const nodeAddress of modified) {
      await this.writeToFile(nodeAddress);
    }
    return modified;
  }

  /**
   * Calculate XOR checksum for a node's bindings
   * Based on C++ implementation
   */
  calculateChecksum(nodeAddress: number): number | null {
    const state = this.bindingStates.get(nodeAddress);
    if (!state) return null;

    let xor = 0;
    for (const binding of state.bindings) {
      for (let i = 0; i < binding.length; i++) {
        xor ^= binding.charCodeAt(i);
      }
    }
    return xor & 0xFF;
  }

  /**
   * Check if node bindings need uploading (compare with node's current state)
   */
  needsUpload(nodeAddress: number, nodeChecksum: number, nodeCount: number): boolean {
    const state = this.bindingStates.get(nodeAddress);
    if (!state) return false;

    // If counts differ, definitely needs upload
    if (state.bindings.length !== nodeCount) {
      return true;
    }

    // If checksums differ, needs upload
    const localChecksum = this.calculateChecksum(nodeAddress);
    if (localChecksum !== nodeChecksum) {
      return true;
    }

    return false;
  }

  /**
   * Mark a node as uploaded with specific checksum/count
   */
  markUploaded(nodeAddress: number, checksum: number, count: number): void {
    const state = this.bindingStates.get(nodeAddress);
    if (state) {
      state.uploadedChecksum = checksum;
      state.uploadedCount = count;
    }
  }

  /**
   * Get statistics
   */
  getStatistics() {
    const stats = {
      totalNodes: this.bindingStates.size,
      modifiedNodes: 0,
      totalBindings: 0,
      totalEdits: 0,
    };

    for (const state of this.bindingStates.values()) {
      if (state.modified) stats.modifiedNodes++;
      stats.totalBindings += state.bindings.length;
      stats.totalEdits += state.edits.length;
    }

    return stats;
  }
}
