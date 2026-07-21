/**
 * Binding System for Home View
 * Handles device ports, wire drawing, and binding management
 */

import { state, dispatch, makeId } from '../app/state.js';

// Simple toast notification (avoids circular dependency on main.js)
function showToast(msg, type = '') {
  const toast = document.createElement('div');
  const bgColor = type === 'success' ? '#10b981' : type === 'error' ? '#ef4444' : '#3b82f6';
  toast.style.cssText = `
    position:fixed;
    bottom:24px;
    right:24px;
    background:${bgColor};
    color:#fff;
    padding:12px 20px;
    border-radius:8px;
    box-shadow:0 4px 12px rgba(0,0,0,0.15);
    font-size:14px;
    z-index:10000;
    animation:slideIn .3s ease-out;
  `;
  toast.textContent = msg;
  document.body.append(toast);
  setTimeout(() => {
    toast.style.animation = 'slideOut .3s ease-out';
    setTimeout(() => toast.remove(), 300);
  }, 2000);
}

// Device port definitions - controllers have outputs (RIGHT), controllables have inputs (LEFT)
export const DEVICE_PORTS = {
  // Controllers (have outputs on RIGHT side)
  input: {
    // Smartbox/module inputs (input_digital) are controllers
    // Physical button press → generates output event (kort/lang detection)
    inputs: [],
    outputs: [
      { id: 'kort', label: 'Kort', color: '#3b82f6' },  // blue - short press
      { id: 'lang', label: 'Lang', color: '#a855f7' }   // purple - long press
    ]
  },
  mood: {
    // Moods can be both controllers AND controllables
    // They receive triggers from buttons/sensors AND can trigger other devices
    inputs: [
      { id: 'kort', label: 'Kort', color: '#3b82f6' },      // blue - short press
      { id: 'lang', label: 'Lang', color: '#a855f7' },      // purple - long press
      { id: 'puls', label: 'Toggle', color: '#ec4899' },    // pink - toggle/trigger mood
      { id: 'schakel', label: 'Schakel', color: '#10b981' } // green - switch (legacy)
    ],
    outputs: [
      { id: 'trigger', label: 'Trigger', color: '#ec4899' } // pink - mood triggers other devices
    ]
  },
  sensor: {
    // Temperature/HVAC sensors (type 4) are CONTROLLABLE devices
    // They receive commands to change settings
    inputs: [
      { id: 'set-off', label: 'Uit', color: '#64748b' },         // grey - turn off
      { id: 'set-sun', label: 'Zon', color: '#fbbf24' },         // yellow - sun preset
      { id: 'set-hsun', label: 'Halve zon', color: '#f59e0b' },  // orange - half sun
      { id: 'set-moon', label: 'Maan', color: '#8b5cf6' },       // purple - moon preset
      { id: 'set-hmoon', label: 'Halve maan', color: '#6366f1' },// indigo - half moon
      { id: 'temp-up', label: 'Temp +', color: '#ef4444' },      // red - increase temp
      { id: 'temp-down', label: 'Temp -', color: '#3b82f6' },    // blue - decrease temp
      { id: 'set-cooling', label: 'Koelen', color: '#3b82f6' },  // blue - set cooling mode
      { id: 'set-heating', label: 'Verwarmen', color: '#ef4444' }// red - set heating mode
    ],
    outputs: []  // Sensors do not generate events in binding system
  },
  
  // Controllable devices (have inputs on LEFT side)
  lamp: {
    inputs: [
      { id: 'aan', label: 'Aan', color: '#10b981' },    // green
      { id: 'uit', label: 'Uit', color: '#ef4444' },    // red
      { id: 'schakel', label: 'Schakel', color: '#3b82f6' }  // blue
    ],
    outputs: []
  },
  relay: {
    inputs: [
      { id: 'aan', label: 'Aan', color: '#10b981' },
      { id: 'uit', label: 'Uit', color: '#ef4444' },
      { id: 'schakel', label: 'Schakel', color: '#3b82f6' }
    ],
    outputs: []
  },
  dimmer: {
    inputs: [
      { id: 'aan', label: 'Aan', color: '#10b981' },
      { id: 'uit', label: 'Uit', color: '#ef4444' },
      { id: 'schakel', label: 'Schakel', color: '#3b82f6' },
      { id: 'dim+', label: 'Dim+', color: '#f59e0b' },
      { id: 'dim-', label: 'Dim-', color: '#f59e0b' },
      { id: 'pir', label: 'PIR', color: '#eab308' },
      { id: 'setval', label: 'Stel waarde', color: '#a855f7' }
    ],
    outputs: []
  },
  motor: {
    inputs: [
      { id: 'op', label: 'Op', color: '#3b82f6' },
      { id: 'neer', label: 'Neer', color: '#3b82f6' },
      { id: 'stop', label: 'Stop', color: '#ef4444' }
    ],
    outputs: []
  }
};

// Current binding context
let _currentBindingContext = null;
let _bindingWires = [];
let _drawingWire = null;
let _manuallyAddedDevices = new Set(); // Track devices manually added without bindings yet

/**
 * Show device bindings - opens the binding panel for a device
 */
export function showDeviceBindings(device, room) {
  const s = state.get();
  
  console.log('[bindings] showDeviceBindings called:', device.name, 'type:', device.type, 'id:', device.id, 'channelRef:', device.channelRef);
  
  // Load existing bindings for this device
  // For moods and multi-button switches, match by channelRef instead of deviceId
  const deviceBindings = (s.project.bindings || []).filter(b => {
    // For moods, match by channelRef (node/unit address)
    if (device.type === 'mood' && device.channelRef) {
      const deviceKey = `${device.channelRef.nodeAddress}-${device.channelRef.unitAddress}`;
      
      const fromKey = b.from.channelRef ? `${b.from.channelRef.nodeAddress}-${b.from.channelRef.unitAddress}` : null;
      const toKey = b.to.channelRef ? `${b.to.channelRef.nodeAddress}-${b.to.channelRef.unitAddress}` : null;
      
      const matches = fromKey === deviceKey || toKey === deviceKey;
      if (matches) {
        console.log('[bindings] Mood binding match:', b.id, 'from:', b.from.deviceId, 'to:', b.to.deviceId);
      }
      return matches;
    }
    
    // For regular devices, match by deviceId
    const matchesDevice = b.from.deviceId === device.id || b.to.deviceId === device.id;
    if (!matchesDevice) return false;
    
    // For multi-button switches, check channelRef
    if (device.isMultiButtonSwitch) {
      const currentChannelRef = JSON.stringify(device.channelRef);
      const fromMatches = b.from.deviceId === device.id && JSON.stringify(b.from.channelRef) === currentChannelRef;
      const toMatches = b.to.deviceId === device.id && JSON.stringify(b.to.channelRef) === currentChannelRef;
      return fromMatches || toMatches;
    }
    
    return true;
  });
  
  console.log('[bindings] Found', deviceBindings.length, 'bindings for', device.name);
  
  // Find all connected devices
  const connectedDeviceIds = new Set();
  deviceBindings.forEach(b => {
    if (b.from.deviceId !== device.id) connectedDeviceIds.add(b.from.deviceId);
    if (b.to.deviceId !== device.id) connectedDeviceIds.add(b.to.deviceId);
  });
  
  // Get connected device objects from all rooms
  const connectedDevices = [];
  s.project.homeView.rooms.forEach(r => {
    r.devices.forEach(d => {
      if (connectedDeviceIds.has(d.id)) {
        connectedDevices.push({ ...d, roomName: r.name });
      }
    });
  });
  
  // Also check global moods (not room-based)
  (s.project.homeView.moods || []).forEach(mood => {
    if (connectedDeviceIds.has(mood.id)) {
      connectedDevices.push({ ...mood, roomName: 'Moods' });
    }
  });
  
  // Set context
  _currentBindingContext = {
    device,
    room,
    otherDevices: connectedDevices
  };
  
  // Clear manually added devices when opening new binding context
  _manuallyAddedDevices.clear();
  
  // Copy bindings to local cache
  _bindingWires = deviceBindings.map(b => ({ ...b }));
  
  renderBindingPanel();
}

/**
 * Render the binding panel with devices and wires
 */
function renderBindingPanel() {
  const bindingsCanvas = document.getElementById('bindings-canvas');
  const bindingsEmpty = document.getElementById('bindings-empty');
  
  if (!bindingsCanvas || !_currentBindingContext) return;
  
  bindingsEmpty.style.display = 'none';
  bindingsCanvas.style.display = 'flex';
  bindingsCanvas.style.flexDirection = 'column';
  bindingsCanvas.innerHTML = '';
  
  // Get fresh device data from state (in case it was updated)
  const s = state.get();
  
  let freshDevice, freshRoom;
  
  // Handle moods (not in rooms) vs regular devices
  if (_currentBindingContext.device.type === 'mood') {
    // For moods, keep the device as-is (it's always fresh from discoveredNodes)
    freshDevice = _currentBindingContext.device;
    freshRoom = _currentBindingContext.room; // Virtual "Moods" room
  } else {
    // For regular devices, get from rooms
    freshRoom = s.project.homeView.rooms.find(r => r.id === _currentBindingContext.room.id);
    freshDevice = freshRoom?.devices.find(d => d.id === _currentBindingContext.device.id);
    
    if (!freshDevice) {
      // Device was deleted, close panel
      _currentBindingContext = null;
      bindingsEmpty.style.display = 'flex';
      bindingsCanvas.style.display = 'none';
      return;
    }
  }
  
  // Update context with fresh data
  _currentBindingContext.device = freshDevice;
  _currentBindingContext.room = freshRoom;
  
  // Reload bindings from state to ensure they're in sync
  // For moods and multi-button switches, match by channelRef
  const deviceBindings = (s.project.bindings || []).filter(b => {
    // For moods, match by channelRef (node/unit address)
    if (freshDevice.type === 'mood' && freshDevice.channelRef) {
      const deviceKey = `${freshDevice.channelRef.nodeAddress}-${freshDevice.channelRef.unitAddress}`;
      
      const fromKey = b.from.channelRef ? `${b.from.channelRef.nodeAddress}-${b.from.channelRef.unitAddress}` : null;
      const toKey = b.to.channelRef ? `${b.to.channelRef.nodeAddress}-${b.to.channelRef.unitAddress}` : null;
      
      return fromKey === deviceKey || toKey === deviceKey;
    }
    
    // For regular devices, match by deviceId
    const matchesDevice = b.from.deviceId === freshDevice.id || b.to.deviceId === freshDevice.id;
    if (!matchesDevice) return false;
    
    // For multi-button switches, check channelRef
    if (freshDevice.isMultiButtonSwitch) {
      const currentChannelRef = JSON.stringify(freshDevice.channelRef);
      const fromMatches = b.from.deviceId === freshDevice.id && JSON.stringify(b.from.channelRef) === currentChannelRef;
      const toMatches = b.to.deviceId === freshDevice.id && JSON.stringify(b.to.channelRef) === currentChannelRef;
      return fromMatches || toMatches;
    }
    
    return true;
  });
  _bindingWires = deviceBindings.map(b => ({ ...b }));
  
  // Rebuild otherDevices list based on current bindings (important for multi-button switches and moods)
  const connectedDeviceIds = new Set();
  const connectedChannelRefs = new Set(); // For moods, match by channelRef
  
  deviceBindings.forEach(b => {
    // Collect device IDs
    if (freshDevice.type === 'mood') {
      // For moods, collect ALL device IDs and channelRefs from bindings
      connectedDeviceIds.add(b.from.deviceId);
      connectedDeviceIds.add(b.to.deviceId);
      if (b.from.channelRef) {
        connectedChannelRefs.add(`${b.from.channelRef.nodeAddress}-${b.from.channelRef.unitAddress}`);
      }
      if (b.to.channelRef) {
        connectedChannelRefs.add(`${b.to.channelRef.nodeAddress}-${b.to.channelRef.unitAddress}`);
      }
    } else {
      // For regular devices, only collect IDs of OTHER devices
      if (b.from.deviceId !== freshDevice.id) connectedDeviceIds.add(b.from.deviceId);
      if (b.to.deviceId !== freshDevice.id) connectedDeviceIds.add(b.to.deviceId);
    }
  });
  
  // For moods, remove self from connected refs
  if (freshDevice.type === 'mood' && freshDevice.channelRef) {
    const selfKey = `${freshDevice.channelRef.nodeAddress}-${freshDevice.channelRef.unitAddress}`;
    connectedChannelRefs.delete(selfKey);
  }
  
  // Get connected device objects from all rooms
  const connectedDevices = [];
  s.project.homeView.rooms.forEach(r => {
    r.devices.forEach(d => {
      let deviceToAdd = null;
      
      // Match by device ID
      if (connectedDeviceIds.has(d.id)) {
        deviceToAdd = { ...d, roomName: r.name };
      } else if (d.channelRef) {
        // Also match by channelRef (for devices bound to moods)
        const key = `${d.channelRef.nodeAddress}-${d.channelRef.unitAddress}`;
        if (connectedChannelRefs.has(key)) {
          deviceToAdd = { ...d, roomName: r.name };
        }
      }
      
      // For multi-button switches, update activeButton to match the binding's channelRef
      if (deviceToAdd && deviceToAdd.isMultiButtonSwitch && deviceToAdd.buttons) {
        // Find which button is actually in the bindings
        for (const binding of deviceBindings) {
          const bindingRef = binding.from.deviceId === deviceToAdd.id ? binding.from.channelRef : 
                            binding.to.deviceId === deviceToAdd.id ? binding.to.channelRef : null;
          
          if (bindingRef) {
            // Find the button index that matches this channelRef
            const buttonIndex = deviceToAdd.buttons.findIndex(btn => 
              btn.ref.nodeAddress === bindingRef.nodeAddress &&
              btn.ref.unitAddress === bindingRef.unitAddress
            );
            
            if (buttonIndex >= 0) {
              deviceToAdd.activeButton = buttonIndex;
              deviceToAdd.channelRef = deviceToAdd.buttons[buttonIndex].ref;
              break; // Use the first matching binding
            }
          }
        }
      }
      
      if (deviceToAdd) {
        connectedDevices.push(deviceToAdd);
      }
    });
  });
  
  // Also check global moods (not room-based)
  (s.project.homeView.moods || []).forEach(mood => {
    if (connectedDeviceIds.has(mood.id)) {
      connectedDevices.push({ ...mood, roomName: 'Moods' });
    } else if (mood.channelRef) {
      // Also match by channelRef
      const key = `${mood.channelRef.nodeAddress}-${mood.channelRef.unitAddress}`;
      if (connectedChannelRefs.has(key)) {
        connectedDevices.push({ ...mood, roomName: 'Moods' });
      }
    }
  });
  
  // Add manually added devices (even if they don't have bindings yet)
  _manuallyAddedDevices.forEach(deviceId => {
    if (!connectedDevices.some(d => d.id === deviceId)) {
      // Check in rooms
      let found = false;
      s.project.homeView.rooms.forEach(r => {
        const device = r.devices.find(d => d.id === deviceId);
        if (device) {
          connectedDevices.push({ ...device, roomName: r.name });
          found = true;
        }
      });
      // Check in moods if not found in rooms
      if (!found) {
        const mood = (s.project.homeView.moods || []).find(m => m.id === deviceId);
        if (mood) {
          connectedDevices.push({ ...mood, roomName: 'Moods' });
        }
      }
    }
  });
  
  _currentBindingContext.otherDevices = connectedDevices;
  
  const { device, room, otherDevices } = _currentBindingContext;
  
  // Helper: check if device is a controller (has outputs)
  const isController = (dev) => {
    const ports = DEVICE_PORTS[dev.type];
    return ports && ports.outputs && ports.outputs.length > 0;
  };
  
  // Helper: check if device has output triggers (for ordering moods)
  const hasOutputTriggers = (dev) => {
    if (dev.type !== 'mood') return false;
    // Check if this mood appears as "from" in any bindings
    return _bindingWires.some(w => {
      if (w.from.deviceId === dev.id) return true;
      if (w.from.channelRef && dev.channelRef) {
        return w.from.channelRef.nodeAddress === dev.channelRef.nodeAddress &&
               w.from.channelRef.unitAddress === dev.channelRef.unitAddress;
      }
      return false;
    });
  };
  
  // Combine all devices and sort:
  // 1. Controllers (inputs/buttons) on left
  // 2. Moods with output triggers (middle-right)
  // 3. Other moods (right)
  // 4. Controllables (lamps/dimmers/motors) on right
  const allDevices = [device, ...otherDevices];
  const sortedDevices = allDevices.sort((a, b) => {
    const aIsController = isController(a);
    const bIsController = isController(b);
    const aIsMood = a.type === 'mood';
    const bIsMood = b.type === 'mood';
    const aHasOutputs = hasOutputTriggers(a);
    const bHasOutputs = hasOutputTriggers(b);
    
    // Controllers (non-mood) first
    if (aIsController && !aIsMood && bIsMood) return -1;
    if (bIsController && !bIsMood && aIsMood) return 1;
    if (aIsController && !aIsMood && !bIsController) return -1;
    if (bIsController && !bIsMood && !aIsController) return 1;
    
    // Among moods: those with output triggers first
    if (aIsMood && bIsMood) {
      if (aHasOutputs && !bHasOutputs) return -1;
      if (!aHasOutputs && bHasOutputs) return 1;
    }
    
    // Moods after controllers but before other controllables
    if (aIsMood && !bIsMood && !bIsController) return -1;
    if (bIsMood && !aIsMood && !aIsController) return 1;
    
    return 0;  // keep original order within same type
  });
  
  // Header
  const header = document.createElement('div');
  header.style.cssText = 'display:flex;justify-content:space-between;align-items:center;padding:12px 16px;background:#fff;border-radius:8px;box-shadow:0 1px 4px rgba(0,0,0,0.05);margin-bottom:16px';
  
  // Build header text with button/sensor info for multi-button switches
  let headerText = `${device.icon} ${device.name}`;
  if (device.isMultiButtonSwitch) {
    if (device.activeSensor) {
      headerText += ' <span style="color:#f59e0b;font-weight:600">🌡️ Sensor</span>';
    } else {
      const buttonNum = (device.activeButton || 0) + 1;
      headerText += ` <span style="color:${device.color};font-weight:600">Button ${buttonNum}</span>`;
    }
  }
  
  header.innerHTML = `
    <div style="font-size:14px;font-weight:600;color:#1a1f2e">
      Bindingen voor ${headerText}
      <span style="color:#6a7899;font-weight:400">in ${room.name}</span>
      <span style="color:#9ca3af;font-size:12px;font-weight:400;margin-left:12px">— Sleep extra apparaten hierheen uit de kamers</span>
    </div>
  `;
  
  // Devices container (flex:1 to fill space)
  const devicesContainer = document.createElement('div');
  devicesContainer.id = 'binding-devices-container';
  devicesContainer.style.cssText = 'display:flex;flex-wrap:wrap;gap:24px;padding:20px;background:#fff;border-radius:8px;min-height:300px;flex:1;position:relative;align-content:flex-start';
  
  // Make container a drop zone
  setupDropZone(devicesContainer);
  
  // Add all devices in sorted order (controllers first, then controllables)
  sortedDevices.forEach(dev => {
    const isPrimary = dev.id === device.id;
    const card = buildBindingDeviceCard(dev, isPrimary);
    devicesContainer.appendChild(card);
  });
  
  // SVG layer for wires
  const svgLayer = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
  svgLayer.style.cssText = 'position:absolute;top:0;left:0;width:100%;height:100%;pointer-events:none;z-index:1';
  svgLayer.id = 'binding-wires-svg';
  devicesContainer.appendChild(svgLayer);
  
  bindingsCanvas.appendChild(header);
  bindingsCanvas.appendChild(devicesContainer);
  
  // Render wires after a short delay (to allow DOM to settle)
  requestAnimationFrame(() => renderBindingWires());
}

/**
 * Setup drop zone for dragging devices into binding panel
 */
function setupDropZone(container) {
  container.ondragover = (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'copy';
    container.style.background = '#eff6ff';
    container.style.boxShadow = 'inset 0 0 0 2px #3b82f6';
  };
  
  container.ondragleave = (e) => {
    const rect = container.getBoundingClientRect();
    if (e.clientX < rect.left || e.clientX >= rect.right || 
        e.clientY < rect.top || e.clientY >= rect.bottom) {
      container.style.background = '#fff';
      container.style.boxShadow = '';
    }
  };
  
  container.ondrop = (e) => {
    e.preventDefault();
    container.style.background = '#fff';
    container.style.boxShadow = '';
    
    try {
      const data = JSON.parse(e.dataTransfer.getData('application/json'));
      
      if ((data.sourceType === 'room' || data.sourceType === 'moods') && data.device) {
        // Check if binding context exists (device selected)
        if (!_currentBindingContext) {
          showToast('Selecteer eerst een apparaat om bindings te bekijken', 'info');
          return;
        }
        
        // Check if device is already in panel
        if (data.device.id === _currentBindingContext.device.id || 
            _currentBindingContext.otherDevices.some(d => d.id === data.device.id)) {
          showToast('Apparaat al aanwezig', 'info');
          return;
        }
        
        // Add to other devices
        const deviceToAdd = { ...data.device };
        if (data.sourceType === 'moods') {
          deviceToAdd.roomName = 'Moods'; // Mark as mood device
        }
        _currentBindingContext.otherDevices.push(deviceToAdd);
        _manuallyAddedDevices.add(data.device.id); // Track manually added device
        renderBindingPanel();
        showToast(`${data.device.icon} ${data.device.name} toegevoegd`, 'success');
      }
    } catch (err) {
      console.error('Binding panel drop error:', err);
    }
  };
}

/**
 * Build button selector for multi-button switches
 * @param {object} device - Device with isMultiButtonSwitch, buttons, and sensors
 * @returns {HTMLElement}
 */
function buildButtonSelector(device) {
  const container = document.createElement('div');
  container.style.cssText = 'display:flex;flex-direction:column;gap:6px;align-items:center;width:100%';
  
  const label = document.createElement('div');
  label.style.cssText = 'font-size:11px;color:#6a7899;font-weight:500';
  label.textContent = 'Selecteer button of sensor:';
  
  // Determine grid layout (2 columns for buttons)
  const cols = 2;
  const totalButtons = device.buttons ? device.buttons.length : (device.buttonCount || 0);
  
  console.log('[button-selector] Building selector for', device.name, 'totalButtons:', totalButtons, 'buttons array:', device.buttons?.length, 'buttonCount:', device.buttonCount);
  
  const grid = document.createElement('div');
  grid.style.cssText = `
    display:grid;
    grid-template-columns:repeat(${cols}, 1fr);
    gap:4px;
    width:100%;
  `;
  
  // Add button selectors
  for (let i = 0; i < totalButtons; i++) {
    const btn = document.createElement('button');
    const isActive = !device.activeSensor && i === device.activeButton;
    
    btn.textContent = (i + 1).toString();
    btn.title = `Button ${i + 1}`;
    btn.style.cssText = `
      width:100%;
      height:32px;
      border-radius:4px;
      border:2px solid ${isActive ? device.color : '#d1d5db'};
      background:${isActive ? device.color : '#fff'};
      color:${isActive ? '#fff' : '#4b5563'};
      font-size:13px;
      font-weight:${isActive ? '700' : '500'};
      cursor:pointer;
      transition:all .15s;
    `;
    
    btn.onclick = (e) => {
      e.stopPropagation();
      // Update device to use this button
      const buttonUnit = device.buttons[i];
      console.log('[button-click] Switching to button', i + 1, 'channelRef:', buttonUnit.ref);
      dispatch({
        type: 'UPDATE_DEVICE',
        deviceId: device.id,
        patch: { 
          activeButton: i,
          activeSensor: false,
          channelRef: buttonUnit.ref
        }
      });
      
      // Show toast
      showToast(`Button ${i + 1} geselecteerd`, 'success');
      
      // Re-render binding panel after state update settles
      setTimeout(() => renderBindingPanel(), 50);
    };
    
    btn.onmouseenter = () => {
      if (!isActive) {
        btn.style.borderColor = '#9ca3af';
        btn.style.background = '#f9fafb';
      }
    };
    
    btn.onmouseleave = () => {
      if (!isActive) {
        btn.style.borderColor = '#d1d5db';
        btn.style.background = '#fff';
      }
    };
    
    grid.appendChild(btn);
  }
  
  // Add temperature sensor button if present
  if (device.hasTempSensor && device.sensors && device.sensors.length > 0) {
    const sensorBtn = document.createElement('button');
    const isActive = device.activeSensor;
    
    sensorBtn.innerHTML = '🌡️';
    sensorBtn.title = 'Temperatuur sensor';
    sensorBtn.style.cssText = `
      width:100%;
      height:32px;
      border-radius:4px;
      border:2px solid ${isActive ? '#f59e0b' : '#d1d5db'};
      background:${isActive ? '#f59e0b' : '#fff'};
      color:${isActive ? '#fff' : '#4b5563'};
      font-size:18px;
      font-weight:${isActive ? '700' : '500'};
      cursor:pointer;
      transition:all .15s;
      grid-column:1 / -1;
    `;
    
    sensorBtn.onclick = (e) => {
      e.stopPropagation();
      // Update device to use temperature sensor
      const sensorUnit = device.sensors[0];
      console.log('[sensor-click] Switching to sensor, channelRef:', sensorUnit.ref);
      dispatch({
        type: 'UPDATE_DEVICE',
        deviceId: device.id,
        patch: { 
          activeSensor: true,
          channelRef: sensorUnit.ref
        }
      });
      
      // Show toast
      showToast(`Temperatuur sensor geselecteerd`, 'success');
      
      // Re-render binding panel after state update settles
      setTimeout(() => renderBindingPanel(), 50);
    };
    
    sensorBtn.onmouseenter = () => {
      if (!isActive) {
        sensorBtn.style.borderColor = '#9ca3af';
        sensorBtn.style.background = '#f9fafb';
      }
    };
    
    sensorBtn.onmouseleave = () => {
      if (!isActive) {
        sensorBtn.style.borderColor = '#d1d5db';
        sensorBtn.style.background = '#fff';
      }
    };
    
    grid.appendChild(sensorBtn);
  }
  
  container.appendChild(label);
  container.appendChild(grid);
  
  return container;
}

/**
 * Build a device card for the binding panel
 */
function buildBindingDeviceCard(device, isPrimary) {
  const card = document.createElement('div');
  card.style.cssText = `
    background:${device.color || '#fff'}15;
    border:2px solid ${isPrimary ? device.color : (device.color + '80')};
    border-radius:8px;
    padding:16px;
    display:flex;
    flex-direction:column;
    align-items:center;
    gap:12px;
    position:relative;
    z-index:2;
  `;
  card.dataset.deviceId = device.id;
  
  // Device header
  const deviceHeader = document.createElement('div');
  deviceHeader.style.cssText = 'display:flex;flex-direction:column;align-items:center;gap:4px';
  deviceHeader.innerHTML = `
    <div style="font-size:32px">${device.icon}</div>
    <div style="font-size:13px;font-weight:600;color:#1a1f2e">${device.name}</div>
    <div style="font-size:11px;color:#6a7899">${device.type}</div>
  `;
  
  card.appendChild(deviceHeader);
  
  // Button/sensor selector for multi-button switches
  if (device.isMultiButtonSwitch) {
    const buttonSelector = buildButtonSelector(device);
    card.appendChild(buttonSelector);
  }
  
  // Determine which ports to show
  let deviceType = device.type;
  if (device.isMultiButtonSwitch && device.activeSensor) {
    // Show sensor ports when temp sensor is selected
    deviceType = 'sensor';
  }
  
  const ports = DEVICE_PORTS[deviceType] || { inputs: [], outputs: [] };
  
  const portsContainer = document.createElement('div');
  portsContainer.style.cssText = 'display:flex;gap:32px;align-items:flex-start';
  
  // Input ports (left side)
  if (ports.inputs.length > 0) {
    const inputsCol = document.createElement('div');
    inputsCol.style.cssText = 'display:flex;flex-direction:column;gap:8px;align-items:flex-start';
    
    ports.inputs.forEach(port => {
      const portEl = buildPortElement(device, port, 'input');
      inputsCol.appendChild(portEl);
    });
    
    portsContainer.appendChild(inputsCol);
  }
  
  // Output ports (right side)
  if (ports.outputs.length > 0) {
    const outputsCol = document.createElement('div');
    outputsCol.style.cssText = 'display:flex;flex-direction:column;gap:8px;align-items:flex-end';
    
    ports.outputs.forEach(port => {
      const portEl = buildPortElement(device, port, 'output');
      outputsCol.appendChild(portEl);
    });
    
    portsContainer.appendChild(outputsCol);
  }
  
  card.appendChild(portsContainer);
  
  return card;
}

/**
 * Build a port element (dot + label)
 */
function buildPortElement(device, port, direction) {
  const portRow = document.createElement('div');
  portRow.style.cssText = `
    display:flex;
    align-items:center;
    gap:8px;
    ${direction === 'input' ? 'flex-direction:row-reverse' : 'flex-direction:row'};
  `;
  
  const label = document.createElement('span');
  label.style.cssText = 'font-size:11px;color:#4a5568;white-space:nowrap';
  label.textContent = port.label;
  
  const dot = document.createElement('div');
  dot.style.cssText = `
    width:12px;
    height:12px;
    border-radius:50%;
    background:${port.color};
    border:2px solid #fff;
    box-shadow:0 1px 3px rgba(0,0,0,0.2);
    cursor:${direction === 'output' ? 'crosshair' : 'pointer'};
    transition:all .15s;
  `;
  dot.dataset.deviceId = device.id;
  dot.dataset.portId = port.id;
  dot.dataset.direction = direction;
  dot.title = `${device.name} - ${port.label}`;
  
  dot.onmouseenter = () => {
    dot.style.transform = 'scale(1.3)';
    dot.style.boxShadow = '0 2px 6px rgba(0,0,0,0.3)';
  };
  dot.onmouseleave = () => {
    dot.style.transform = 'scale(1)';
    dot.style.boxShadow = '0 1px 3px rgba(0,0,0,0.2)';
  };
  
  // Wire drawing interaction
  if (direction === 'output') {
    dot.onmousedown = (e) => startWireDrawing(e, device, port);
  } else {
    dot.addEventListener('wireConnect', (e) => {
      completeWire(e.detail.from, { 
        deviceId: device.id, 
        portId: port.id,
        channelRef: device.channelRef  // Store channelRef for multi-button switches
      });
    });
  }
  
  portRow.appendChild(label);
  portRow.appendChild(dot);
  
  return portRow;
}

/**
 * Start drawing a wire from an output port
 */
function startWireDrawing(e, device, port) {
  e.preventDefault();
  e.stopPropagation();
  
  const dot = e.target;
  const rect = dot.getBoundingClientRect();
  const container = document.getElementById('binding-devices-container');
  const containerRect = container.getBoundingClientRect();
  
  _drawingWire = {
    from: { 
      deviceId: device.id, 
      portId: port.id,
      channelRef: device.channelRef  // Store channelRef for multi-button switches
    },
    color: port.color,
    startX: rect.left + rect.width / 2 - containerRect.left,
    startY: rect.top + rect.height / 2 - containerRect.top
  };
  
  const onMouseMove = (e) => {
    const mouseX = e.clientX - containerRect.left;
    const mouseY = e.clientY - containerRect.top;
    
    // Draw temporary wire
    const svg = document.getElementById('binding-wires-svg');
    if (!svg) return;
    
    svg.innerHTML = '';
    const path = createWirePath(_drawingWire.startX, _drawingWire.startY, mouseX, mouseY, _drawingWire.color);
    svg.appendChild(path);
  };
  
  const onMouseUp = (e) => {
    document.removeEventListener('mousemove', onMouseMove);
    document.removeEventListener('mouseup', onMouseUp);
    
    // Check if we're over an input port
    const target = document.elementFromPoint(e.clientX, e.clientY);
    if (target && target.dataset.direction === 'input') {
      target.dispatchEvent(new CustomEvent('wireConnect', {
        detail: { from: _drawingWire.from, color: _drawingWire.color }
      }));
    }
    
    _drawingWire = null;
    renderBindingWires();
  };
  
  document.addEventListener('mousemove', onMouseMove);
  document.addEventListener('mouseup', onMouseUp);
}

/**
 * Complete a wire connection
 */
function completeWire(from, to) {
  const binding = {
    id: makeId(),
    from,
    to,
    color: _drawingWire.color
  };
  
  // Add to local cache
  _bindingWires.push(binding);
  
  // Dispatch to state
  dispatch({ type: 'ADD_BINDING', binding });
  
  // Remove devices from manually added set now that they have bindings
  _manuallyAddedDevices.delete(from.deviceId);
  _manuallyAddedDevices.delete(to.deviceId);
  
  renderBindingWires();
  showToast('Binding toegevoegd', 'success');
}

/**
 * Render all binding wires
 */
function renderBindingWires() {
  const svg = document.getElementById('binding-wires-svg');
  if (!svg) return;
  
  svg.innerHTML = '';
  
  console.log('[wires] Rendering', _bindingWires.length, 'wires');
  
  // Get all devices currently displayed in binding panel (to check their active buttons)
  const allDevicesInPanel = [_currentBindingContext.device, ..._currentBindingContext.otherDevices];
  
  _bindingWires.forEach(wire => {
    // Filter wires based on active button/sensor of multi-button switches
    // Check if 'from' device is multi-button switch with different active button
    const fromDevice = allDevicesInPanel.find(d => d.id === wire.from.deviceId);
    if (fromDevice?.isMultiButtonSwitch) {
      const wireChannelRef = JSON.stringify(wire.from.channelRef);
      const activeChannelRef = JSON.stringify(fromDevice.channelRef);
      if (wireChannelRef !== activeChannelRef) {
        console.log('[wires] Skipping wire (from multi-button mismatch):', wire.id);
        return; // Skip this wire
      }
    }
    
    // Check if 'to' device is multi-button switch with different active button
    const toDevice = allDevicesInPanel.find(d => d.id === wire.to.deviceId);
    if (toDevice?.isMultiButtonSwitch) {
      const wireChannelRef = JSON.stringify(wire.to.channelRef);
      const activeChannelRef = JSON.stringify(toDevice.channelRef);
      if (wireChannelRef !== activeChannelRef) {
        console.log('[wires] Skipping wire (to multi-button mismatch):', wire.id);
        return; // Skip this wire
      }
    }
    
    // Find port elements
    const fromPort = document.querySelector(`[data-device-id="${wire.from.deviceId}"][data-port-id="${wire.from.portId}"]`);
    const toPort = document.querySelector(`[data-device-id="${wire.to.deviceId}"][data-port-id="${wire.to.portId}"]`);
    
    console.log('[wires] Wire', wire.id, 'from:', wire.from.deviceId, wire.from.portId, 'found:', !!fromPort, 'to:', wire.to.deviceId, wire.to.portId, 'found:', !!toPort);
    
    if (!fromPort || !toPort) return;
    
    const container = document.getElementById('binding-devices-container');
    const containerRect = container.getBoundingClientRect();
    const fromRect = fromPort.getBoundingClientRect();
    const toRect = toPort.getBoundingClientRect();
    
    const x1 = fromRect.left + fromRect.width / 2 - containerRect.left;
    const y1 = fromRect.top + fromRect.height / 2 - containerRect.top;
    const x2 = toRect.left + toRect.width / 2 - containerRect.left;
    const y2 = toRect.top + toRect.height / 2 - containerRect.top;
    
    const path = createWirePath(x1, y1, x2, y2, wire.color);
    path.style.cursor = 'pointer';
    path.style.pointerEvents = 'stroke';
    path.onclick = () => {
      if (confirm('Deze binding verwijderen?')) {
        deleteWire(wire.id);
      }
    };
    
    console.log('[wires] Drew wire:', wire.id);
    svg.appendChild(path);
  });
}

/**
 * Create an SVG path for a wire
 */
function createWirePath(x1, y1, x2, y2, color) {
  const path = document.createElementNS('http://www.w3.org/2000/svg', 'path');
  
  // Curved path
  const dx = x2 - x1;
  const dy = y2 - y1;
  const dist = Math.sqrt(dx * dx + dy * dy);
  const curve = Math.min(dist / 2, 50);
  
  const d = `M ${x1} ${y1} C ${x1 + curve} ${y1}, ${x2 - curve} ${y2}, ${x2} ${y2}`;
  
  path.setAttribute('d', d);
  path.setAttribute('stroke', color);
  path.setAttribute('stroke-width', '3');
  path.setAttribute('fill', 'none');
  path.setAttribute('stroke-linecap', 'round');
  
  return path;
}

/**
 * Delete a wire
 */
function deleteWire(wireId) {
  const wireToDelete = _bindingWires.find(w => w.id === wireId);
  if (!wireToDelete) return;
  
  _bindingWires = _bindingWires.filter(w => w.id !== wireId);
  dispatch({ type: 'REMOVE_BINDING', binding: wireToDelete });
  renderBindingWires();
  showToast('Binding verwijderd', 'info');
}

// Subscribe to state changes to update binding panel when devices change
state.subscribe((newState) => {
  // Only re-render if binding panel is open and devices have changed
  if (_currentBindingContext) {
    renderBindingPanel();
  }
});
