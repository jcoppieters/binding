/**
 * Unit Picker Dialog
 * 
 * Shows available hardware units (channels) from Rail View modules.
 * Replaces the device type picker with actual hardware selection.
 */

import { state, dispatch } from '../app/state.js';
import { getAllUnitsWithUsage, getAvailableUnits, createChannelRef } from '../app/unit-helpers.js';

let _currentRoomId = null;
let _allUnits = [];
let _filteredUnits = [];
let _filterType = 'all'; // 'all' | 'controller' | 'controllable'
let _searchQuery = '';

/**
 * Update filter button style based on active state
 */
function updateFilterButtonStyle(btn, isActive) {
  btn.style.cssText = `padding:8px 16px;border-radius:6px;border:1px solid ${isActive ? '#3b82f6' : '#d1d5db'};background:${isActive ? '#eff6ff' : '#fff'};color:${isActive ? '#1e40af' : '#4b5563'};font-size:13px;cursor:pointer;font-weight:${isActive ? '600' : '400'}`;
}

/**
 * Open unit picker for adding device to room
 * @param {string} roomId - Room to add device to
 */
export function openUnitPicker(roomId) {
  _currentRoomId = roomId;
  
  const s = state.get();
  const modules = s.modules;
  
  // Generate all units with usage tracking
  _allUnits = getAllUnitsWithUsage(s.project, modules);
  _filteredUnits = _allUnits;
  _filterType = 'all';
  _searchQuery = '';
  
  renderPicker();
}

function renderPicker() {
  // Remove existing picker
  const existing = document.getElementById('unit-picker-overlay');
  if (existing) existing.remove();
  
  // Create overlay
  const overlay = document.createElement('div');
  overlay.id = 'unit-picker-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,0.5);display:flex;align-items:center;justify-content:center;z-index:10000';
  
  // Create dialog
  const dialog = document.createElement('div');
  dialog.style.cssText = 'background:#fff;border-radius:12px;width:90%;max-width:800px;max-height:80vh;display:flex;flex-direction:column;box-shadow:0 20px 60px rgba(0,0,0,0.3)';
  
  // Header
  const header = document.createElement('div');
  header.style.cssText = 'padding:24px;border-bottom:1px solid #e5e7eb';
  header.innerHTML = `
    <h2 style="margin:0;font-size:18px;color:#1a1f2e">Kies een beschikbaar kanaal</h2>
    <p style="margin:6px 0 0;font-size:13px;color:#6a7899">Selecteer een kanaal uit je Rail View configuratie</p>
  `;
  
  // Filter tabs
  const filters = document.createElement('div');
  filters.style.cssText = 'display:flex;gap:8px;padding:16px 24px;border-bottom:1px solid #e5e7eb;background:#f8f9fd';
  
  const tabs = [
    { id: 'all', label: 'Alles', icon: '📟' },
    { id: 'controllable', label: 'Uitgang (lamp, relais, motor)', icon: '⚡' },
    { id: 'controller', label: 'Ingang (schakelaar, sensor)', icon: '🔘' },
  ];
  
  const filterButtons = {};
  
  tabs.forEach(tab => {
    const btn = document.createElement('button');
    btn.textContent = `${tab.icon} ${tab.label}`;
    btn.dataset.filterId = tab.id;
    updateFilterButtonStyle(btn, _filterType === tab.id);
    btn.onclick = () => {
      _filterType = tab.id;
      // Update all button styles
      Object.entries(filterButtons).forEach(([id, button]) => {
        updateFilterButtonStyle(button, id === _filterType);
      });
      applyFilters();
    };
    filterButtons[tab.id] = btn;
    filters.append(btn);
  });
  
  // Search box
  const searchBox = document.createElement('div');
  searchBox.style.cssText = 'padding:16px 24px;border-bottom:1px solid #e5e7eb';
  const searchInput = document.createElement('input');
  searchInput.type = 'text';
  searchInput.placeholder = 'Zoek op naam, module, kast...';
  searchInput.style.cssText = 'width:100%;padding:10px 12px;border:1px solid #d1d5db;border-radius:6px;font-size:13px';
  searchInput.value = _searchQuery;
  searchInput.oninput = (e) => {
    _searchQuery = e.target.value.toLowerCase();
    applyFilters();
  };
  searchBox.append(searchInput);
  
  // Units list (scrollable)
  const list = document.createElement('div');
  list.id = 'unit-picker-list';
  list.style.cssText = 'flex:1;overflow-y:auto;padding:16px 24px;min-height:300px';
  
  renderUnitsList(list);
  
  // Footer
  const footer = document.createElement('div');
  footer.style.cssText = 'padding:16px 24px;border-top:1px solid #e5e7eb;display:flex;justify-content:flex-end;gap:8px';
  
  const cancelBtn = document.createElement('button');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.style.cssText = 'padding:10px 20px;border:1px solid #d1d5db;border-radius:6px;background:#fff;color:#4b5563;font-size:13px;cursor:pointer;font-weight:500';
  cancelBtn.onclick = () => overlay.remove();
  
  footer.append(cancelBtn);
  
  dialog.append(header, filters, searchBox, list, footer);
  overlay.append(dialog);
  document.body.append(overlay);
  
  // Close on overlay click
  overlay.addEventListener('click', (e) => {
    if (e.target === overlay) overlay.remove();
  });
  
  // Focus search
  searchInput.focus();
}

function applyFilters() {
  // Filter by type
  if (_filterType === 'all') {
    _filteredUnits = _allUnits.filter(u => !u.usage); // Only unused
  } else {
    _filteredUnits = getAvailableUnits(_allUnits, _filterType);
  }
  
  // Filter by search query
  if (_searchQuery) {
    _filteredUnits = _filteredUnits.filter(u => 
      u.label.toLowerCase().includes(_searchQuery) ||
      u.moduleName.toLowerCase().includes(_searchQuery) ||
      u.moduleModel.toLowerCase().includes(_searchQuery) ||
      u.cabinetName?.toLowerCase().includes(_searchQuery)
    );
  }
  
  // Re-render list
  const list = document.getElementById('unit-picker-list');
  if (list) renderUnitsList(list);
}

function renderUnitsList(container) {
  container.innerHTML = '';
  
  if (_filteredUnits.length === 0) {
    container.innerHTML = `
      <div style="text-align:center;padding:40px;color:#9ca3af">
        <div style="font-size:48px;margin-bottom:16px">🔍</div>
        <div style="font-size:14px;font-weight:500;color:#6b7280">Geen beschikbare kanalen</div>
        <div style="font-size:12px;margin-top:6px">Voeg modules toe in Rail View of pas je zoekopdracht aan</div>
      </div>
    `;
    return;
  }
  
  // Group by module (using moduleInstanceId for unique grouping)
  const grouped = {};
  for (const unit of _filteredUnits) {
    const key = unit.moduleInstanceId || unit.woningDeviceId || unit.moduleModel;
    if (!grouped[key]) {
      grouped[key] = {
        moduleName: unit.moduleName,
        moduleModel: unit.moduleModel,
        cabinetName: unit.cabinetName,
        nodeAddress: unit.nodeAddress,
        units: [],
      };
    }
    grouped[key].units.push(unit);
  }
  
  // Render groups
  for (const [key, group] of Object.entries(grouped)) {
    // Group header
    const groupHeader = document.createElement('div');
    groupHeader.style.cssText = 'margin-bottom:8px;margin-top:24px;font-size:12px;font-weight:600;color:#6a7899;display:flex;align-items:center;gap:8px';
    const nodeHex = group.nodeAddress != null ? ` (Node 0x${group.nodeAddress.toString(16).toUpperCase().padStart(2, '0')})` : '';
    groupHeader.innerHTML = `<span>📦 ${group.moduleName}${nodeHex}</span>`;
    container.append(groupHeader);
    
    // Units in this module
    for (const unit of group.units) {
      const card = createUnitCard(unit);
      container.append(card);
    }
  }
}

function createUnitCard(unit) {
  const card = document.createElement('div');
  card.style.cssText = 'padding:12px 16px;border:1px solid #e5e7eb;border-radius:8px;margin-bottom:8px;cursor:pointer;transition:all 0.15s;display:flex;align-items:center;justify-content:space-between';
  
  card.onmouseenter = () => {
    card.style.background = '#f8f9fd';
    card.style.borderColor = '#3b82f6';
  };
  card.onmouseleave = () => {
    card.style.background = '#fff';
    card.style.borderColor = '#e5e7eb';
  };
  
  card.onclick = () => selectUnit(unit);
  
  const left = document.createElement('div');
  left.style.cssText = 'display:flex;align-items:center;gap:12px;flex:1';
  
  const icon = document.createElement('div');
  icon.style.cssText = 'font-size:24px;flex-shrink:0';
  icon.textContent = unit.icon;
  
  const info = document.createElement('div');
  
  // Map channel type to readable plugin type
  const channelTypeLabels = {
    dimmer_le: 'LE fase aansnijding',
    dimmer_te: 'TE fase afsnijding',
    dimmer_pwm: 'PWM LED',
    dimmer_dc: '0-10V / 1-10V',
    relay_no: 'Relais NO',
    relay_nc: 'Relais NC',
    relay_ssr: 'Relais SSR',
    motor_updown: 'Motor op/neer',
    motor_polar: 'Motor polair',
    input_digital: 'Digitale ingang',
    input_analog: 'Analoge ingang',
  };
  const channelLabel = channelTypeLabels[unit.channelType] || unit.channelType;
  const unitHex = `0x${unit.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`;
  
  info.innerHTML = `
    <div style="font-size:14px;font-weight:500;color:#1a1f2e">${unit.label}</div>
    <div style="font-size:12px;color:#6a7899;margin-top:2px">
      ${channelLabel} · Unit ${unitHex}
    </div>
  `;
  
  left.append(icon, info);
  
  const badge = document.createElement('div');
  badge.style.cssText = 'padding:4px 8px;border-radius:4px;background:#d1fae5;color:#065f46;font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:0.5px';
  badge.textContent = 'Beschikbaar';
  
  card.append(left, badge);
  
  return card;
}

function selectUnit(unit) {
  const s = state.get();
  const room = s.project.homeView.rooms.find(r => r.id === _currentRoomId);
  if (!room) return;
  
  // Prompt for custom name
  const defaultName = unit.label;
  const customName = prompt(`Geef dit apparaat een naam:`, defaultName);
  if (!customName) return; // Cancelled
  
  // Find available position (avoiding collisions)
  const { x, y } = findAvailablePosition(room);
  
  // Detect multi-button switches (Essence, Serenity, etc.)
  const buttonInfo = detectMultiButtonSwitch(unit, s);
  
  // Create device with channel reference
  const device = {
    id: makeId(),
    type: mapChannelTypeToDeviceType(unit.channelType),
    name: customName,
    channelRef: createChannelRef(unit),
    x,
    y,
    
    // Store display properties from unit
    icon: unit.icon,
    color: getColorForChannelType(unit.channelType),
    
    // Multi-button switch info
    ...buttonInfo,
  };
  
  dispatch({
    type: 'ADD_DEVICE_TO_ROOM',
    roomId: _currentRoomId,
    device,
  });
  
  // Close picker
  document.getElementById('unit-picker-overlay')?.remove();
  
  // Show toast
  const s2 = state.get();
  if (s2.showToast) {
    s2.showToast(`${device.icon} ${device.name} toegevoegd`, 'success');
  }
}

/**
 * Detect if unit belongs to a multi-button switch and return button info
 * @param {object} unit - The selected unit
 * @param {object} s - Current state
 * @returns {object} Button info or empty object
 */
function detectMultiButtonSwitch(unit, s) {
  // Only for input_digital channels
  if (unit.channelType !== 'input_digital') return {};
  
  // Check if module is a multi-button switch (2+ buttons)
  const moduleDef = s.modules?.[unit.moduleModel];
  if (!moduleDef) return {};
  
  // Find input_digital channel groups
  const inputGroups = moduleDef.channelGroups?.filter(g => g.type === 'input_digital') || [];
  const totalButtons = inputGroups.reduce((sum, g) => sum + g.count, 0);
  
  // Only consider as multi-button if 2 or more buttons
  if (totalButtons < 2) return {};
  
  return {
    buttonCount: totalButtons,
    activeButton: unit.unitAddress, // Current button (0-indexed)
  };
}

function makeId() {
  return Math.random().toString(36).slice(2, 10);
}

/**
 * Find an available position in the room, avoiding device collisions.
 * Starts at (50, 50) and increments by 50px until finding empty spot.
 */
function findAvailablePosition(room) {
  const roomWidth = parseInt(room.width) || 500;
  const roomHeight = parseInt(room.height) || 400;
  const devices = room.devices || [];
  
  let topX = 50; let x = 50;
  let topY = 50; let y = 50;

  // Check for device at exact position
  while (hasDeviceAt(devices, x, y)) {
    x += 50;
    y += 50;
    
    // Wrap x or y if exceeds bounds
    if ((x > roomWidth - 100) || (y > roomHeight - 100)) {
      topX += 75;
      x = topX;
      y = topY;
    }

  }
  
  return { x, y };
}

/**
 * Check if a device exists at exact position.
 */
function hasDeviceAt(devices, x, y) {
  return devices.some(device => device.x === x && device.y === y);
}

function mapChannelTypeToDeviceType(channelType) {
  if (channelType.startsWith('dimmer_')) return 'dimmer';
  if (channelType.startsWith('relay_')) return 'relay';
  if (channelType.startsWith('motor_')) return 'motor';
  if (channelType.startsWith('input_')) return 'input';
  if (channelType === 'temperature') return 'sensor';
  return 'relay'; // default
}

function getColorForChannelType(channelType) {
  const colors = {
    'dimmer_le': '#fbbf24',
    'dimmer_te': '#fbbf24',
    'dimmer_pwm': '#fbbf24',
    'dimmer_dc': '#fbbf24',
    'relay_no': '#60a5fa',
    'relay_nc': '#60a5fa',
    'relay_ssr': '#60a5fa',
    'motor_updown': '#34d399',
    'motor_polar': '#34d399',
    'input_digital': '#a78bfa',
    'input_analog': '#f472b6',
    'temperature': '#f59e0b',  // orange for temperature sensors
  };
  return colors[channelType] || '#9ca3af';
}
