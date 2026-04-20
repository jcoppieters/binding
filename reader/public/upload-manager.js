/**
 * Upload Manager - Handles connection and uploading bindings to hardware
 */

class UploadManager {
  constructor() {
    this.isConnected = false;
    this.connectionInfo = null;
    this.initializeModal();
    this.loadConnectionSettings();
  }

  /**
   * Create the connection settings modal
   */
  initializeModal() {
    const modalHTML = `
      <div id="connection-modal" class="modal" style="display: none;">
        <div class="modal-backdrop" onclick="uploadManager.closeConnectionModal()"></div>
        <div class="modal-content connection-modal">
          <div class="modal-header">
            <h2>🔌 Connection Settings</h2>
            <button class="close-btn" onclick="uploadManager.closeConnectionModal()">✕</button>
          </div>
          
          <div class="modal-body">
            <p style="color: var(--text-muted); margin-bottom: 1.5rem;">
              Connect to the master node to upload binding changes to your Duotecno installation.
            </p>
            
            <div class="form-group">
              <label for="connection-host">Master Node IP Address</label>
              <input type="text" id="connection-host" placeholder="192.168.0.97" value="192.168.0.97">
              <small class="help-text">The IP address of your Duotecno master node</small>
            </div>
            
            <div class="form-group">
              <label for="connection-port">Port</label>
              <input type="number" id="connection-port" placeholder="5001" value="5001" min="1" max="65535">
              <small class="help-text">Usually 5001 for Duotecno systems</small>
            </div>
            
            <div class="form-group">
              <label for="connection-password">Password (Optional)</label>
              <input type="password" id="connection-password" placeholder="">
              <small class="help-text">Leave empty if no password is set</small>
            </div>
            
            <div id="connection-status" class="connection-status" style="display: none;"></div>
          </div>
          
          <div class="modal-footer">
            <button class="btn btn-secondary" onclick="uploadManager.closeConnectionModal()">Cancel</button>
            <button class="btn btn-primary" id="connect-btn" onclick="uploadManager.connect()">
              Connect
            </button>
          </div>
        </div>
      </div>
      
      <div id="upload-modal" class="modal" style="display: none;">
        <div class="modal-backdrop" onclick="uploadManager.closeUploadModal()"></div>
        <div class="modal-content upload-modal">
          <div class="modal-header">
            <h2>📤 Upload Bindings</h2>
            <button class="close-btn" onclick="uploadManager.closeUploadModal()">✕</button>
          </div>
          
          <div class="modal-body">
            <p style="color: var(--text-muted); margin-bottom: 1.5rem;">
              Upload your binding changes to the hardware installation.
            </p>
            
            <div class="upload-options">
              <div class="upload-option">
                <input type="radio" id="upload-changed" name="upload-mode" value="changed" checked>
                <label for="upload-changed">
                  <strong>Smart Upload (Recommended)</strong>
                  <small>Only upload nodes with changed bindings. Compares checksums to detect changes.</small>
                </label>
              </div>
              
              <div class="upload-option">
                <input type="radio" id="upload-modified" name="upload-mode" value="modified">
                <label for="upload-modified">
                  <strong>Upload Modified</strong>
                  <small>Upload only nodes modified in this session.</small>
                </label>
              </div>
              
              <div class="upload-option">
                <input type="radio" id="upload-all" name="upload-mode" value="all">
                <label for="upload-all">
                  <strong>Force Upload All</strong>
                  <small>Upload all bindings to all nodes (slower, use only if needed).</small>
                </label>
              </div>
            </div>
            
            <div id="upload-progress" style="display: none;">
              <div class="progress-bar">
                <div id="upload-progress-fill" class="progress-fill"></div>
              </div>
              <div id="upload-status" class="upload-status"></div>
            </div>
            
            <div id="upload-results" class="upload-results" style="display: none;"></div>
          </div>
          
          <div class="modal-footer">
            <button class="btn btn-secondary" onclick="uploadManager.closeUploadModal()">Cancel</button>
            <button class="btn btn-primary" id="start-upload-btn" onclick="uploadManager.startUpload()">
              Start Upload
            </button>
          </div>
        </div>
      </div>
    `;
    
    document.body.insertAdjacentHTML('beforeend', modalHTML);
  }

  /**
   * Load connection settings from localStorage
   */
  loadConnectionSettings() {
    const saved = localStorage.getItem('duotecno-connection');
    if (saved) {
      try {
        this.connectionInfo = JSON.parse(saved);
        document.getElementById('connection-host').value = this.connectionInfo.host || '192.168.0.97';
        document.getElementById('connection-port').value = this.connectionInfo.port || 5001;
      } catch (e) {
        console.error('Failed to load connection settings:', e);
      }
    }
  }

  /**
   * Save connection settings to localStorage
   */
  saveConnectionSettings(host, port) {
    this.connectionInfo = { host, port };
    localStorage.setItem('duotecno-connection', JSON.stringify(this.connectionInfo));
  }

  /**
   * Open connection settings modal
   */
  openConnectionModal() {
    document.getElementById('connection-modal').style.display = 'flex';
  }

  /**
   * Close connection settings modal
   */
  closeConnectionModal() {
    document.getElementById('connection-modal').style.display = 'none';
  }

  /**
   * Open upload modal
   */
  openUploadModal() {
    if (!this.isConnected) {
      alert('Please connect to the master node first');
      this.openConnectionModal();
      return;
    }
    
    // Reset upload UI
    document.getElementById('upload-progress').style.display = 'none';
    document.getElementById('upload-results').style.display = 'none';
    document.getElementById('start-upload-btn').disabled = false;
    
    document.getElementById('upload-modal').style.display = 'flex';
  }

  /**
   * Close upload modal
   */
  closeUploadModal() {
    document.getElementById('upload-modal').style.display = 'none';
  }

  /**
   * Connect to master node
   */
  async connect() {
    const host = document.getElementById('connection-host').value.trim();
    const port = parseInt(document.getElementById('connection-port').value);
    const password = document.getElementById('connection-password').value;
    
    if (!host || !port) {
      alert('Please enter both host and port');
      return;
    }
    
    const connectBtn = document.getElementById('connect-btn');
    const statusDiv = document.getElementById('connection-status');
    
    connectBtn.disabled = true;
    connectBtn.textContent = 'Connecting...';
    statusDiv.style.display = 'block';
    statusDiv.className = 'connection-status connecting';
    statusDiv.textContent = 'Connecting to master node...';
    
    try {
      const response = await fetch('/api/upload/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ host, port, password })
      });
      
      const result = await response.json();
      
      if (result.success) {
        this.isConnected = true;
        this.saveConnectionSettings(host, port);
        
        statusDiv.className = 'connection-status connected';
        statusDiv.textContent = '✓ Connected successfully!';
        
        // Update header status
        this.updateConnectionStatus();
        
        setTimeout(() => {
          this.closeConnectionModal();
          statusDiv.style.display = 'none';
        }, 1500);
      } else {
        throw new Error(result.error || 'Connection failed');
      }
    } catch (error) {
      console.error('Connection error:', error);
      statusDiv.className = 'connection-status error';
      statusDiv.textContent = '✗ ' + error.message;
      this.isConnected = false;
      this.updateConnectionStatus();
    } finally {
      connectBtn.disabled = false;
      connectBtn.textContent = 'Connect';
    }
  }

  /**
   * Disconnect from master node
   */
  async disconnect() {
    try {
      await fetch('/api/upload/disconnect', { method: 'POST' });
      this.isConnected = false;
      this.updateConnectionStatus();
    } catch (error) {
      console.error('Disconnect error:', error);
    }
  }

  /**
   * Update connection status in header
   */
  updateConnectionStatus() {
    const statusEl = document.getElementById('connection-status-header');
    if (this.isConnected) {
      statusEl.innerHTML = '<span class="status-indicator connected"></span> Connected';
      statusEl.className = 'connection-status-header connected';
    } else {
      statusEl.innerHTML = '<span class="status-indicator disconnected"></span> Not Connected';
      statusEl.className = 'connection-status-header disconnected';
    }
  }

  /**
   * Start upload process
   */
  async startUpload() {
    const mode = document.querySelector('input[name="upload-mode"]:checked').value;
    
    const startBtn = document.getElementById('start-upload-btn');
    const progressDiv = document.getElementById('upload-progress');
    const resultsDiv = document.getElementById('upload-results');
    
    startBtn.disabled = true;
    progressDiv.style.display = 'block';
    resultsDiv.style.display = 'none';
    
    document.getElementById('upload-status').textContent = 'Uploading...';
    document.getElementById('upload-progress-fill').style.width = '50%';
    
    try {
      const endpoint = `/api/upload/${mode}`;
      const response = await fetch(endpoint, { method: 'POST' });
      const result = await response.json();
      
      document.getElementById('upload-progress-fill').style.width = '100%';
      
      // Show results
      setTimeout(() => {
        progressDiv.style.display = 'none';
        resultsDiv.style.display = 'block';
        
        let html = '<h3>Upload Complete</h3>';
        
        if (mode === 'changed') {
          html += `
            <div class="upload-summary">
              <div class="summary-stat">
                <span class="stat-value">${result.uploaded || 0}</span>
                <span class="stat-label">Uploaded</span>
              </div>
              <div class="summary-stat">
                <span class="stat-value">${result.skipped || 0}</span>
                <span class="stat-label">Skipped (unchanged)</span>
              </div>
              <div class="summary-stat">
                <span class="stat-value">${result.failed || 0}</span>
                <span class="stat-label">Failed</span>
              </div>
            </div>
          `;
        } else {
          html += `
            <div class="upload-summary">
              <div class="summary-stat">
                <span class="stat-value">${result.successful || 0}</span>
                <span class="stat-label">Successful</span>
              </div>
              <div class="summary-stat">
                <span class="stat-value">${result.failed || 0}</span>
                <span class="stat-label">Failed</span>
              </div>
              <div class="summary-stat">
                <span class="stat-value">${result.total || 0}</span>
                <span class="stat-label">Total</span>
              </div>
            </div>
          `;
        }
        
        if (result.results && result.results.length > 0) {
          html += '<div class="upload-details">';
          result.results.forEach(r => {
            const status = r.success ? '✓' : '✗';
            const statusClass = r.success ? 'success' : 'error';
            const nodeHex = '0x' + r.nodeAddress.toString(16).toUpperCase().padStart(2, '0');
            html += `
              <div class="upload-result-item ${statusClass}">
                <span class="result-status">${status}</span>
                <span class="result-node">Node ${nodeHex}</span>
                <span class="result-message">${r.skipped ? 'Skipped (unchanged)' : r.error || `${r.bindingsWritten || 0} bindings`}</span>
              </div>
            `;
          });
          html += '</div>';
        }
        
        resultsDiv.innerHTML = html;
        startBtn.textContent = 'Close';
        startBtn.disabled = false;
        startBtn.onclick = () => uploadManager.closeUploadModal();
      }, 500);
      
    } catch (error) {
      console.error('Upload error:', error);
      document.getElementById('upload-status').textContent = 'Upload failed: ' + error.message;
      startBtn.disabled = false;
    }
  }
}

// Initialize upload manager
const uploadManager = new UploadManager();
