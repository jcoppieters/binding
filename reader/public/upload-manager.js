/**
 * Upload Manager - Handles uploading bindings to hardware
 * Connection is now managed by MasterConnectionService (unit-control.js)
 */

class UploadManager {
  constructor() {
    this.initializeModal();
  }

  /**
   * Create the upload modal (connection modal removed - use header button)
   */
  initializeModal() {
    const modalHTML = `
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
   * Open upload modal
   */
  async openUploadModal() {
    // Check if master is connected via unit control
    try {
      const statusResponse = await fetch('/api/master/status');
      const status = await statusResponse.json();
      
      if (!status.connected) {
        alert('Please connect to the master node first using the connection button in the header.');
        return;
      }
    } catch (error) {
      console.error('Failed to check connection status:', error);
      alert('Failed to check connection status. Please ensure you are connected.');
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

  // Connection management removed - now handled by MasterConnectionService (unit-control.js)

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
