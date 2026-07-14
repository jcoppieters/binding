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
  switch: {
    inputs: [],
    outputs: [
      { id: 'kort', label: 'Kort', color: '#3b82f6' },  // blue
      { id: 'lang', label: 'Lang', color: '#a855f7' }   // purple
    ]
  },
  button: {
    inputs: [],
    outputs: [
      { id: 'druk', label: 'Druk', color: '#10b981' }   // green
    ]
  },
  sensor: {
    inputs: [],
    outputs: [
      { id: 'trigger', label: 'Trigger', color: '#f59e0b' }  // orange
    ]
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
  
  const { device, room, otherDevices } = _currentBindingContext;
  
  // Header
  const header = document.createElement('div');
  header.style.cssText = 'display:flex;justify-content:space-between;align-items:center;padding:12px 16px;background:#fff;border-radius:8px;box-shadow:0 1px 4px rgba(0,0,0,0.05);margin-bottom:16px';
  header.innerHTML = `
    <div style="font-size:14px;font-weight:600;color:#1a1f2e">
      Bindingen voor ${device.icon} ${device.name} 
      <span style="color:#6a7899;font-weight:400">in ${room.name}</span>
      <span style="color:#9ca3af;font-size:12px;font-weight:400;margin-left:12px">— Sleep apparaten hierheen</span>
    </div>
  `;
  
  // Devices container (flex:1 to fill space)
  const devicesContainer = document.createElement('div');
  devicesContainer.id = 'binding-devices-container';
  devicesContainer.style.cssText = 'display:flex;flex-wrap:wrap;gap:24px;padding:20px;background:#fff;border-radius:8px;min-height:300px;flex:1;position:relative;align-content:flex-start';
  
  // Make container a drop zone
  setupDropZone(devicesContainer);
  
  // Add main device
  const mainCard = buildBindingDeviceCard(device, true);
  devicesContainer.appendChild(mainCard);
  
  // Add connected devices
  otherDevices.forEach(dev => {
    const card = buildBindingDeviceCard(dev, false);
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
      console.error('Drop error:', err);
    }
  };
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
  
  const ports = DEVICE_PORTS[device.type] || { inputs: [], outputs: [] };
  
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
  
  card.appendChild(deviceHeader);
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
