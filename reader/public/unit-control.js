/**
 * Unit Control Manager
 * Handles real-time unit state monitoring and control
 */

class UnitControlManager {
  constructor() {
    this.connected = false;
    this.unitStates = new Map(); // Map of nodeAddr:unitAddr -> state
    this.connectionConfig = null;
    this.pollingInterval = null; // For real-time state updates
    this.pollingNodeAddr = null;
    this.pollingUnitAddr = null;
  }

  /**
   * Check master connection status
   */
  async checkConnectionStatus() {
    try {
      const response = await fetch('/api/master/status');
      const data = await response.json();
      this.connected = data.connected && data.state === 'connected';
      return this.connected;
    } catch (error) {
      console.error('Failed to check connection status:', error);
      this.connected = false;
      return false;
    }
  }

  /**
   * Connect to master
   */
  async connect(host, port, password = '') {
    try {
      const response = await fetch('/api/master/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ host, port, password })
      });

      if (!response.ok) {
        const error = await response.json();
        throw new Error(error.error || error.message || 'Failed to connect');
      }

      const data = await response.json();
      this.connected = data.success;
      this.connectionConfig = { host, port, password };

      if (this.connected) {
        console.log(`✓ Connected to master: ${data.nodes} nodes, ${data.units} units discovered`);
      }

      return data;
    } catch (error) {
      console.error('Failed to connect to master:', error);
      throw error;
    }
  }

  /**
   * Disconnect from master
   */
  async disconnect() {
    try {
      // Stop any active polling
      this.stopPolling();
      
      const response = await fetch('/api/master/disconnect', {
        method: 'POST'
      });

      if (response.ok) {
        this.connected = false;
        this.connectionConfig = null;
        this.unitStates.clear();
      }
    } catch (error) {
      console.error('Failed to disconnect from master:', error);
    }
  }

  /**
   * Get state for a specific unit (loads fresh from server)
   */
  async getUnitState(nodeAddress, unitAddress) {
    try {
      const response = await fetch(`/api/units/${nodeAddress.toString(16)}/${unitAddress.toString(16)}`);
      
      if (!response.ok) {
        throw new Error('Failed to load unit state');
      }
      
      const unit = await response.json();
      const key = `${nodeAddress}:${unitAddress}`;
      this.unitStates.set(key, unit);
      
      return unit;
    } catch (error) {
      console.error('Failed to get unit state:', error);
      return null;
    }
  }

  /**
   * Set unit state (control the unit)
   * Waits for status response from server
   */
  async setUnitState(nodeAddress, unitAddress, value, status) {
    try {
      const response = await fetch(`/api/units/${nodeAddress.toString(16)}/${unitAddress.toString(16)}/set`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ value, status })
      });

      if (!response.ok) {
        const error = await response.json();
        throw new Error(error.error || 'Failed to set unit state');
      }

      const data = await response.json();
      
      // Update cached state
      const key = `${nodeAddress}:${unitAddress}`;
      this.unitStates.set(key, data.unit);
      
      return data;
    } catch (error) {
      console.error('Failed to set unit state:', error);
      throw error;
    }
  }

  /**
   * Get display value for unit state
   */
  getDisplayState(state) {
    if (!state) return 'Unknown';

    switch (state.type) {
      case 1: // DIM
        // Display as OFF if value is 0, regardless of status (better UX)
        return (state.status && state.value > 0) ? `On (${state.value}%)` : 'Off';
      
      case 2: // SWITCH
      case 3: // CONTROL
        return state.status ? 'On' : 'Off';
      
      case 4: // SENS (Temperature)
        if (state.temperature) {
          const tempC = state.temperature.temp / 10;
          const presets = ['Off', 'Sun', 'Half Sun', 'Moon', 'Half Moon'];
          const preset = presets[state.temperature.preset + 1] || 'Unknown';
          return `${tempC.toFixed(1)}°C - ${preset}`;
        }
        return 'Unknown';
      
      case 8: // DUOSWITCH (Motor)
        const motorStates = ['Stopped', 'Stopped Down', 'Stopped Up', 'Opening', 'Closing'];
        return motorStates[state.status] || 'Unknown';
      
      case 5: // AUDIO
      case 0x85: // AUDIO_HC4
        if (state.audio?.destination) {
          return state.status ? `On (Vol: ${state.audio.destination.volume})` : 'Off';
        }
        return state.status ? 'On' : 'Off';
      
      default:
        return state.status !== undefined ? state.status.toString() : 'Unknown';
    }
  }

  /**
   * Start polling for unit state updates
   * @param nodeAddress - Node address to poll
   * @param unitAddress - Unit address to poll
   * @param intervalMs - Polling interval in milliseconds (default: 1500ms)
   */
  startPolling(nodeAddress, unitAddress, intervalMs = 1500) {
    // Stop any existing polling
    this.stopPolling();
    
    if (!this.connected) {
      console.warn('Cannot start polling: not connected to master');
      return;
    }

    this.pollingNodeAddr = nodeAddress;
    this.pollingUnitAddr = unitAddress;
    
    // Poll immediately, then at intervals
    this.pollUnitState();
    
    this.pollingInterval = setInterval(() => {
      this.pollUnitState();
    }, intervalMs);
    
    console.log(`Started polling unit ${nodeAddress}:${unitAddress} every ${intervalMs}ms`);
  }

  /**
   * Stop polling for unit state updates
   */
  stopPolling() {
    if (this.pollingInterval) {
      clearInterval(this.pollingInterval);
      this.pollingInterval = null;
      console.log(`Stopped polling unit ${this.pollingNodeAddr}:${this.pollingUnitAddr}`);
      this.pollingNodeAddr = null;
      this.pollingUnitAddr = null;
    }
  }

  /**
   * Poll current unit state and update UI if changed
   */
  async pollUnitState() {
    if (!this.pollingNodeAddr || !this.pollingUnitAddr) return;
    
    try {
      const state = await this.getUnitState(this.pollingNodeAddr, this.pollingUnitAddr);
      if (state && typeof updateUnitControlsInView === 'function') {
        await updateUnitControlsInView(state);
      }
    } catch (error) {
      // Silently fail - don't spam console during polling
      // Only log if it's a connection error
      if (error.message && error.message.includes('connect')) {
        console.error('Polling error:', error);
        this.stopPolling();
      }
    }
  }
}

// Global instance
const unitControl = new UnitControlManager();

/**
 * Open connection modal
 */
function openUnitControlConnection() {
  const modal = document.createElement('div');
  modal.className = 'modal-overlay';
  modal.innerHTML = `
    <div class="modal">
      <div class="modal-header">
        <h2>Connect to Master for Unit Control</h2>
        <button class="modal-close" onclick="this.closest('.modal-overlay').remove()">✕</button>
      </div>
      <div class="modal-body">
        <div class="form-group">
          <label for="master-host">Master IP Address</label>
          <input type="text" id="master-host" value="192.168.0.97" placeholder="192.168.0.97" required>
        </div>
        <div class="form-group">
          <label for="master-port">Port</label>
          <input type="text" id="master-port" value="5001" placeholder="5001" pattern="[0-9]+" required>
          <small style="color: #64748b; font-size: 0.75rem;">Default: 5001</small>
        </div>
        <div class="form-group">
          <label for="master-password">Password</label>
          <input type="password" id="master-password" autocomplete="off" value="GMTecno" placeholder="Password - obligatory">
        </div>
        <div id="connection-error" class="error-message" style="display: none;"></div>
      </div>
      <div class="modal-footer">
        <button class="btn btn-secondary" onclick="this.closest('.modal-overlay').remove()">Cancel</button>
        <button class="btn btn-primary" onclick="connectToMasterFromModal()">Connect</button>
      </div>
    </div>
  `;
  
  document.body.appendChild(modal);
}

/**
 * Connect to master from modal
 */
async function connectToMasterFromModal() {
  const hostInput = document.getElementById('master-host');
  const portInput = document.getElementById('master-port');
  const passwordInput = document.getElementById('master-password');
  const errorDiv = document.getElementById('connection-error');

  const host = hostInput.value.trim();
  const portStr = portInput.value.trim();
  const password = passwordInput.value;

  // Validate inputs
  if (!host) {
    errorDiv.textContent = 'Please enter the master IP address';
    errorDiv.style.display = 'block';
    hostInput.focus();
    return;
  }

  if (!portStr || !/^\d+$/.test(portStr)) {
    errorDiv.textContent = 'Please enter a valid port number';
    errorDiv.style.display = 'block';
    portInput.focus();
    return;
  }

  const port = parseInt(portStr, 10);
  if (port < 1 || port > 65535) {
    errorDiv.textContent = 'Port must be between 1 and 65535';
    errorDiv.style.display = 'block';
    portInput.focus();
    return;
  }

  try {
    errorDiv.style.display = 'none';
    
    await unitControl.connect(host, port, password);
    
    // Close modal
    document.querySelector('.modal-overlay').remove();
    
    // Update UI
    updateUnitControlStatus();
    
    // Show success message
    showNotification('Connected to master successfully!', 'success');
    
  } catch (error) {
    console.error('Connection error:', error);
    errorDiv.textContent = error.message || 'Failed to connect to master';
    errorDiv.style.display = 'block';
  }
}

/**
 * Disconnect from master
 */
async function disconnectFromMaster() {
  await unitControl.disconnect();
  updateUnitControlStatus();
  showNotification('Disconnected from master', 'info');
}

/**
 * Update unit control status in header
 */
function updateUnitControlStatus() {
  const statusElement = document.getElementById('unit-control-status');
  if (!statusElement) return;

  if (unitControl.connected) {
    statusElement.className = 'connection-status-header connected';
    statusElement.innerHTML = '<span class="status-indicator connected"></span> Unit Control Connected';
    statusElement.onclick = disconnectFromMaster;
  } else {
    statusElement.className = 'connection-status-header disconnected';
    statusElement.innerHTML = '<span class="status-indicator disconnected"></span> Unit Control Disconnected';
    statusElement.onclick = openUnitControlConnection;
  }
}

/**
 * Show notification
 */
function showNotification(message, type = 'info') {
  const notification = document.createElement('div');
  notification.className = `notification notification-${type}`;
  notification.textContent = message;
  document.body.appendChild(notification);

  setTimeout(() => notification.classList.add('show'), 10);
  setTimeout(() => {
    notification.classList.remove('show');
    setTimeout(() => notification.remove(), 300);
  }, 3000);
}

/**
 * Toggle switch state
 */
async function toggleSwitch(nodeAddress, unitAddress) {
  try {
    // Get current state
    const state = await unitControl.getUnitState(nodeAddress, unitAddress);
    if (!state) return;

    // Toggle and send command (status: 0=off, 1=on)
    const newStatus = state.status ? 0 : 1;
    console.log(`Toggle switch: current=${state.status}, new=${newStatus}`);
    const result = await unitControl.setUnitState(nodeAddress, unitAddress, null, newStatus);
    
    // Update UI
    if (result.success) {
      updateUnitControlsInView(result.unit);
    }
  } catch (error) {
    showNotification('Failed to control unit: ' + error.message, 'error');
  }
}

/**
 * Set switch state directly (for ON/OFF buttons)
 */
async function setSwitchState(nodeAddress, unitAddress, on) {
  try {
    const status = on ? 1 : 0;
    console.log(`Set switch state: on=${on}, status=${status}`);
    const result = await unitControl.setUnitState(nodeAddress, unitAddress, null, status);
    
    // Update UI
    if (result.success) {
      updateUnitControlsInView(result.unit);
    }
  } catch (error) {
    showNotification('Failed to control switch: ' + error.message, 'error');
  }
}

/**
 * Set dimmer value
 */
async function setDimmerValue(nodeAddress, unitAddress, value) {
  try {
    const result = await unitControl.setUnitState(nodeAddress, unitAddress, value, null);
    if (result.success) {
      updateUnitControlsInView(result.unit);
    }
  } catch (error) {
    showNotification('Failed to control dimmer: ' + error.message, 'error');
  }
}

/**
 * Send dimmer method directly (raw method number)
 * @param nodeAddress - Node address
 * @param unitAddress - Unit address
 * @param method - Dimmer method number (5=UP, 6=DOWN, 9=OFF, 10=ON, etc.)
 */
async function sendDimmerMethod(nodeAddress, unitAddress, method) {
  try {
    const response = await fetch(`/api/units/${nodeAddress.toString(16)}/${unitAddress.toString(16)}/dimmer-method`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ method })
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.error || 'Failed to send dimmer method');
    }

    const result = await response.json();
    if (result.success) {
      updateUnitControlsInView(result.unit);
      showNotification(`Method 0x${method.toString(16).toUpperCase().padStart(2, '0')} sent`, 'success');
    }
  } catch (error) {
    showNotification('Failed to send method: ' + error.message, 'error');
  }
}

/**
 * Control motor
 */
async function controlMotor(nodeAddress, unitAddress, action) {
  try {
    // action: 3=stop, 4=up/open, 5=down/close
    const result = await unitControl.setUnitState(nodeAddress, unitAddress, null, action);
    if (result.success) {
      updateUnitControlsInView(result.unit);
    }
  } catch (error) {
    showNotification('Failed to control motor: ' + error.message, 'error');
  }
}

/**
 * Send protocol command directly (for testing/debugging)
 * @param nodeAddress - Node address
 * @param unitAddress - Unit address  
 * @param type - Unit type (1=DIM, 2=SWITCH, 3=CONTROL, etc.)
 * @param value - Command value/method specific
 */
async function sendProtocolCommand(nodeAddress, unitAddress, type, value) {
  try {
    const result = await unitControl.setUnitState(nodeAddress, unitAddress, value, null);
    if (result.success) {
      updateUnitControlsInView(result.unit);
      showNotification('Command sent successfully', 'success');
    }
  } catch (error) {
    showNotification('Failed to send command: ' + error.message, 'error');
  }
}

/**
 * Send sensor preset command
 * @param nodeAddress - Node address
 * @param unitAddress - Unit address
 * @param preset - Preset number (0=off, 1=sun, 2=half sun, 3=moon, 4=half moon)
 */
async function sendSensorPreset(nodeAddress, unitAddress, preset) {
  try {
    const response = await fetch(`/api/units/${nodeAddress.toString(16)}/${unitAddress.toString(16)}/sensor-control`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ preset })
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.error || 'Failed to set sensor preset');
    }

    const result = await response.json();
    if (result.success && result.unit) {
      updateUnitControlsInView(result.unit);
    }
    
    const presetNames = ['OFF', 'SUN', 'HALF SUN', 'MOON', 'HALF MOON'];
    showNotification(`Set preset to ${presetNames[preset] || preset}`, 'success');
  } catch (error) {
    showNotification('Failed to set preset: ' + error.message, 'error');
  }
}

/**
 * Adjust sensor temperature (increment or decrement)
 * @param nodeAddress - Node address
 * @param unitAddress - Unit address
 * @param action - 'inc' to increase, 'dec' to decrease
 */
async function adjustSensorTemp(nodeAddress, unitAddress, action) {
  try {
    const response = await fetch(`/api/units/${nodeAddress.toString(16)}/${unitAddress.toString(16)}/sensor-control`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ action })
    });

    if (!response.ok) {
      const error = await response.json();
      throw new Error(error.error || 'Failed to adjust temperature');
    }

    const result = await response.json();
    if (result.success && result.unit) {
      updateUnitControlsInView(result.unit);
    }
    
    showNotification(`Temperature ${action === 'inc' ? 'increased' : 'decreased'}`, 'success');
  } catch (error) {
    showNotification('Failed to adjust temperature: ' + error.message, 'error');
  }
}

/**
 * Update unit controls in the current view
 */
async function updateUnitControlsInView(state) {
  if (!unitControl.connected) return;
  
  // If no state provided, fetch it
  if (!state && currentNodeAddress !== null && currentUnitAddress !== null) {
    state = await unitControl.getUnitState(currentNodeAddress, currentUnitAddress);
  }
  
  if (!state) return;

  // Update status display
  const statusDisplay = document.getElementById('unit-status-display');
  if (statusDisplay) {
    statusDisplay.textContent = unitControl.getDisplayState(state);
    // For dimmers, consider value=0 as OFF (better UX)
    const isOn = (state.type === 1) ? (state.status && state.value > 0) : !!state.status;
    statusDisplay.className = `unit-status-display ${isOn ? 'status-on' : 'status-off'}`;
  }

  // Update switch toggle and label
  const switchToggle = document.getElementById('unit-switch-toggle');
  const switchLabel = document.getElementById('unit-switch-label');
  const switchStatusValue = document.getElementById('switch-status-value');
  
  if (switchToggle) {
    // For dimmers, consider value=0 as OFF (better UX)
    const isOn = (state.type === 1) ? (state.status && state.value > 0) : !!state.status;
    switchToggle.checked = isOn;
  }
  if (switchLabel) {
    const isOn = (state.type === 1) ? (state.status && state.value > 0) : !!state.status;
    switchLabel.textContent = isOn ? 'ON' : 'OFF';
  }
  if (switchStatusValue) {
    const isOn = !!state.status;
    switchStatusValue.textContent = isOn ? 'ON' : 'OFF';
    switchStatusValue.className = `status-value ${isOn ? 'status-on' : 'status-off'}`;
  }

  // Update dimmer slider
  const dimmerSlider = document.getElementById('unit-dimmer-slider');
  const dimmerValue = document.getElementById('unit-dimmer-value');
  const dimmerBrightnessDisplay = document.getElementById('dimmer-brightness-display');
  const dimmerStatusValue = document.getElementById('dimmer-status-value');
  
  if (dimmerSlider && typeof state.value === 'number') {
    dimmerSlider.value = state.value;
    if (dimmerValue) {
      dimmerValue.textContent = `${state.value}%`;
    }
    if (dimmerBrightnessDisplay) {
      dimmerBrightnessDisplay.textContent = `${state.value}%`;
    }
  }
  
  // Update dimmer status display (OFF=0, ON=1, PIRTIMED=2)
  if (dimmerStatusValue && state.type === 1) {
    const statusMap = { 0: 'OFF', 1: 'ON', 2: 'PIRTIMED' };
    const statusText = statusMap[state.status] || (state.status ? 'ON' : 'OFF');
    const isOn = (state.status && state.value > 0);
    dimmerStatusValue.textContent = statusText;
    dimmerStatusValue.className = `status-value ${isOn ? 'status-on' : 'status-off'}`;
  }
  
  // Update temperature display
  const sensorTempDisplay = document.getElementById('sensor-temp-display');
  const sensorSetpointDisplay = document.getElementById('sensor-setpoint-display');
  const sensorPresetDisplay = document.getElementById('sensor-preset-display');
  
  if (state.temperature) {
    if (sensorTempDisplay) {
      const tempC = (state.temperature.temp / 10).toFixed(1);
      sensorTempDisplay.textContent = `${tempC}°C`;
    }
    
    if (sensorPresetDisplay) {
      const presets = ['Off', 'Sun', 'Half Sun', 'Moon', 'Half Moon'];
      const presetIdx = state.temperature.preset ?? -1;
      const presetName = presetIdx >= 0 && presetIdx < presets.length ? presets[presetIdx] : 'Off';
      sensorPresetDisplay.textContent = presetName;
    }
    
    if (sensorSetpointDisplay) {
      const presetIdx = state.temperature.preset ?? -1;
      if (presetIdx >= 0) {
        const presetValues = [0, state.temperature.sun, state.temperature.hsun, state.temperature.moon, state.temperature.hmoon];
        if (presetIdx < presetValues.length) {
          const setpointC = (presetValues[presetIdx] / 10).toFixed(1);
          sensorSetpointDisplay.textContent = `${setpointC}°C`;
        }
      } else {
        sensorSetpointDisplay.textContent = '--°C';
      }
    }
  }
  
  // Update control/input status
  const controlStatusValue = document.getElementById('control-status-value');
  if (controlStatusValue) {
    const controlStatus = state.status ? 'Pressed' : 'Released';
    controlStatusValue.textContent = controlStatus;
    controlStatusValue.className = `status-value ${state.status ? 'status-on' : 'status-off'}`;
  }
  
  // Update mood/virtual status
  const moodStatusValue = document.getElementById('mood-status-value');
  if (moodStatusValue) {
    const moodStatus = state.status ? 'ON' : 'OFF';
    moodStatusValue.textContent = moodStatus;
    moodStatusValue.className = `status-value ${state.status ? 'status-on' : 'status-off'}`;
  }
  
  // Update motor status
  const motorStatusValue = document.getElementById('motor-status-value');
  if (motorStatusValue) {
    const motorStates = ['Stopped', 'Stopped Down', 'Stopped Up', 'Opening', 'Closing'];
    const motorStatus = motorStates[state.status] || 'Unknown';
    const isMoving = (state.status === 3 || state.status === 4);
    motorStatusValue.textContent = motorStatus;
    motorStatusValue.className = `status-value ${isMoving ? 'status-on' : 'status-off'}`;
  }
  
  // Update generic status
  const genericStatus = document.getElementById('generic-status');
  const genericValue = document.getElementById('generic-value');
  if (genericStatus) {
    genericStatus.textContent = `Status: ${state.status !== undefined ? state.status : 'Unknown'}`;
  }
  if (genericValue) {
    genericValue.textContent = `Value: ${state.value !== undefined ? state.value : 'Unknown'}`;
  }
}

// Initialize status on page load
document.addEventListener('DOMContentLoaded', async () => {
  await unitControl.checkConnectionStatus();
  updateUnitControlStatus();
});
