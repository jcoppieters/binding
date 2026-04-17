/**
 * Reader for node binding files (bind*.txt)
 * Reads raw binding strings from files
 */

import { readFileSync } from 'fs';
import { readFile } from 'fs/promises';

/**
 * Simple binding file structure
 */
export interface NodeBindingFile {
  nodeAddress: number;
  fileName: string;
  bindings: string[];  // Raw hex strings
}

export class BindingFileReader {
  /**
   * Read a binding file synchronously
   */
  static readFileSync(filePath: string): NodeBindingFile {
    const content = readFileSync(filePath, 'utf-8');
    return this.parseContent(filePath, content);
  }
  
  /**
   * Read a binding file asynchronously
   */
  static async readFile(filePath: string): Promise<NodeBindingFile> {
    const content = await readFile(filePath, 'utf-8');
    return this.parseContent(filePath, content);
  }
  
  /**
   * Parse file content
   */
  private static parseContent(filePath: string, content: string): NodeBindingFile {
    const fileName = filePath.split('/').pop() || '';
    const nodeAddress = this.extractNodeAddressFromFileName(fileName);
    
    // Split into lines and filter out empty ones
    const bindings = content
      .split('\n')
      .map(line => line.trim())
      .filter(line => line.length > 0);
    
    return {
      nodeAddress,
      fileName,
      bindings,
    };
  }
  
  /**
   * Extract node address from filename (e.g., "bindfc.txt" -> 0xFC)
   */
  private static extractNodeAddressFromFileName(fileName: string): number {
    const match = fileName.match(/bind([0-9a-fA-F]{2})\.txt/i);
    if (!match) {
      throw new Error(`Invalid binding filename: ${fileName}`);
    }
    return parseInt(match[1], 16);
  }
}
