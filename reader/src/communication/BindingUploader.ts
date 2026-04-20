/**
 * Binding Uploader - Orchestrates uploading bindings to nodes with change detection
 */

import { BindingWriterFSM } from './BindingWriterFSM';
import { BindingManager } from '../services/BindingManager';
import { TcpConnection } from './TcpConnection';

export interface FileInfo {
  nodeAddress: number;
  entryCount: number;
  xorChecksum: number;
}

export interface UploadResult {
  nodeAddress: number;
  success: boolean;
  skipped?: boolean;
  reason?: string;
  bindingsWritten?: number;
  error?: string;
}

export class BindingUploader {
  private connection: TcpConnection;
  private writer: BindingWriterFSM;

  constructor(host: string, port: number, password: string = '') {
    this.writer = new BindingWriterFSM(host, port, password);
    this.connection = this.writer.getConnection();
  }

  /**
   * Connect to master node
   */
  async connect(): Promise<void> {
    await this.writer.connect();
  }

  /**
   * Disconnect from master node
   */
  disconnect(): void {
    this.writer.disconnect();
  }

  /**
   * Request file info from a node (number of bindings + checksum)
   */
  async requestFileInfo(nodeAddress: number): Promise<FileInfo> {
    const FC_NODEBINDINGSFILECONTROL = 0x98;
    const FILE_INFO = 0x02;
    const BROADCAST_UNIT = 0xFF;

    return new Promise((resolve, reject) => {
      const timeout = setTimeout(() => {
        this.connection.off('file-control-response', handler);
        reject(new Error('Timeout waiting for FILE_INFO response'));
      }, 5000);

      const handler = (response: any) => {
        if (response.nodeAddress !== nodeAddress) return;
        if (response.data.length < 3) return;

        clearTimeout(timeout);
        this.connection.off('file-control-response', handler);

        const event = response.data[0];
        if (event !== 0x02) { // FILE_INFO event
          reject(new Error(`Unexpected file control event: 0x${event.toString(16)}`));
          return;
        }

        // Parse response (see ComApplication.cpp line 4699)
        let entryCount: number;
        let xorChecksum: number;

        if (response.data.length === 4) {
          // 2-byte count (>255 bindings)
          entryCount = (response.data[1] << 8) | response.data[2];
          xorChecksum = response.data[3];
        } else {
          // 1-byte count (<=255 bindings)
          entryCount = response.data[1];
          xorChecksum = response.data[2];
        }

        resolve({ nodeAddress, entryCount, xorChecksum });
      };

      this.connection.on('file-control-response', handler);

      // Send FILE_INFO request
      this.connection.sendCanBusMessage({
        nodeAddress,
        unitAddress: BROADCAST_UNIT,
        messageCode: FC_NODEBINDINGSFILECONTROL,
        data: [FILE_INFO]
      });
    });
  }

  /**
   * Upload bindings to a single node
   */
  async uploadToNode(nodeAddress: number, manager: BindingManager): Promise<UploadResult> {
    const bindings = manager.getBindings(nodeAddress);
    if (!bindings) {
      return {
        nodeAddress,
        success: false,
        error: 'Node not loaded in manager'
      };
    }

    try {
      await this.writer.writeBindingsToNode(nodeAddress, {
        nodeAddress,
        fileName: `bind${nodeAddress.toString(16).padStart(2, '0')}.txt`,
        bindings
      });

      // Mark as uploaded with current checksum
      const checksum = manager.calculateChecksum(nodeAddress);
      if (checksum !== null) {
        manager.markUploaded(nodeAddress, checksum, bindings.length);
      }

      return {
        nodeAddress,
        success: true,
        bindingsWritten: bindings.length
      };
    } catch (error) {
      return {
        nodeAddress,
        success: false,
        error: error instanceof Error ? error.message : String(error)
      };
    }
  }

  /**
   * Upload all bindings (force upload without checking)
   */
  async uploadAll(manager: BindingManager): Promise<UploadResult[]> {
    const results: UploadResult[] = [];
    const nodes = manager.getLoadedNodes();

    for (const nodeAddress of nodes) {
      console.log(`\n${'='.repeat(60)}`);
      console.log(`Uploading node 0x${nodeAddress.toString(16).padStart(2, '0')}`);
      console.log('='.repeat(60));
      
      const result = await this.uploadToNode(nodeAddress, manager);
      results.push(result);

      // Small delay between nodes
      await new Promise(resolve => setTimeout(resolve, 500));
    }

    return results;
  }

  /**
   * Upload only changed bindings (smart upload with change detection)
   */
  async uploadChanged(manager: BindingManager): Promise<UploadResult[]> {
    const results: UploadResult[] = [];
    const nodes = manager.getLoadedNodes();

    console.log('\n🔍 Checking which nodes need updating...\n');

    for (const nodeAddress of nodes) {
      try {
        // Request current file info from node
        console.log(`Checking node 0x${nodeAddress.toString(16).padStart(2, '0')}...`);
        const fileInfo = await this.requestFileInfo(nodeAddress);

        // Check if upload needed
        const needsUpload = manager.needsUpload(
          nodeAddress,
          fileInfo.xorChecksum,
          fileInfo.entryCount
        );

        if (!needsUpload) {
          console.log(`  ✓ Up to date (${fileInfo.entryCount} bindings, checksum 0x${fileInfo.xorChecksum.toString(16)})`);
          results.push({
            nodeAddress,
            success: true,
            skipped: true,
            reason: 'Already up to date'
          });
          continue;
        }

        const localChecksum = manager.calculateChecksum(nodeAddress);
        const localCount = manager.getBindings(nodeAddress)?.length ?? 0;
        console.log(`  ⚠ Needs update:`);
        console.log(`    Node: ${fileInfo.entryCount} bindings, checksum 0x${fileInfo.xorChecksum.toString(16)}`);
        console.log(`    Local: ${localCount} bindings, checksum 0x${localChecksum?.toString(16) ?? '??'}`);

        // Upload
        const result = await this.uploadToNode(nodeAddress, manager);
        results.push(result);

        // Small delay between nodes
        await new Promise(resolve => setTimeout(resolve, 500));

      } catch (error) {
        console.error(`  ✗ Error checking node: ${error instanceof Error ? error.message : String(error)}`);
        results.push({
          nodeAddress,
          success: false,
          error: error instanceof Error ? error.message : String(error)
        });
      }
    }

    return results;
  }

  /**
   * Upload only modified bindings (files edited in manager)
   */
  async uploadModified(manager: BindingManager): Promise<UploadResult[]> {
    const results: UploadResult[] = [];
    const modified = manager.getModifiedNodes();

    if (modified.length === 0) {
      console.log('✓ No modified bindings to upload');
      return results;
    }

    console.log(`\n📤 Uploading ${modified.length} modified node(s)...\n`);

    for (const nodeAddress of modified) {
      const result = await this.uploadToNode(nodeAddress, manager);
      results.push(result);

      // Small delay between nodes
      await new Promise(resolve => setTimeout(resolve, 500));
    }

    return results;
  }
}
