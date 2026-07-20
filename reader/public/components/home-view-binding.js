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
    // Virtual moods (from master node 0xFC) are controllers
    // Mood activation → generates output event
    inputs: [],
    outputs: [
      { id: 'trigger', label: 'Trigger', color: '#10b981' }  // green
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

/**
 * Show device bindings - opens the binding panel for a device
 */
export function showDeviceBindings(device, room) {
  const s = state.get();
  
  // Load existing bindings for this device
  const deviceBindings = (s.project.bindings || []).filter(b => 
    b.from.deviceId === device.id || b.to.deviceId === device.id
  );
  
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
  
  // Set context
  _currentBindingContext = {
    device,
    room,
    otherDevices: connectedDevices
  };
  
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
  const freshRoom = s.project.homeView.rooms.find(r => r.id === _currentBindingContext.room.id);
  const freshDevice = freshRoom?.devices.find(d => d.id === _currentBindingContext.device.id);
  
  if (!freshDevice) {
    // Device was deleted, close panel
    _currentBindingContext = null;
    bindingsEmpty.style.display = 'flex';
    bindingsCanvas.style.display = 'none';
    return;
  }
  
  // Update context with fresh data
  _currentBindingContext.device = freshDevice;
  _currentBindingContext.room = freshRoom;
  
  // Refresh other devices as well
  _currentBindingContext.otherDevices = _currentBindingContext.otherDevices.map(otherDev => {
    const otherRoom = s.project.homeView.rooms.find(r => r.devices.some(d => d.id === otherDev.id));
    const fresh = otherRoom?.devices.find(d => d.id === otherDev.id);
    return fresh ? { ...fresh, roomName: otherRoom.name } : otherDev;
  }).filter(d => d); // Remove any deleted devices
  
  const { device, room, otherDevices } = _currentBindingContext;
  
  // Helper: check if device is a controller (has outputs)
  const isController = (dev) => {
    const ports = DEVICE_PORTS[dev.type];
    return ports && ports.outputs && ports.outputs.length > 0;
  };
  
  // Combine all devices and sort: controllers first, then controllables
  const allDevices = [device, ...otherDevices];
  const sortedDevices = allDevices.sort((a, b) => {
    const aIsController = isController(a);
    const bIsController = isController(b);
    if (aIsController && !bIsController) return -1;  // a first
    if (!aIsController && bIsController) return 1;   // b first
    return 0;  // keep original order within same type
  });
  
  // Header
  const header = document.createElement('div');
  header.style.cssText = 'display:flex;justify-content:space-between;align-items:center;padding:12px 16px;background:#fff;border-radius:8px;box-shadow:0 1px 4px rgba(0,0,0,0.05);margin-bottom:16px';
  header.innerHTML = `
    <div style="font-size:14px;font-weight:600;color:#1a1f2e">
      Bindingen voor ${device.icon} ${device.name} 
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
      
      if (data.sourceType === 'room' && data.device) {
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
        _currentBindingContext.otherDevices.push(data.device);
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
      completeWire(e.detail.from, { deviceId: device.id, portId: port.id });
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
    from: { deviceId: device.id, portId: port.id },
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
  
  _bindingWires.forEach(wire => {
    // Find port elements
    const fromPort = document.querySelector(`[data-device-id="${wire.from.deviceId}"][data-port-id="${wire.from.portId}"]`);
    const toPort = document.querySelector(`[data-device-id="${wire.to.deviceId}"][data-port-id="${wire.to.portId}"]`);
    
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
