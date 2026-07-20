/**
 * Home View — Room-based device layout and configuration
 */

// @ts-check

import { state, dispatch, makeId } from '../app/state.js';
import { showDeviceBindings } from './home-view-binding.js';
import { openUnitPicker } from './unit-picker.js';

/**
 * Create a reusable dropdown menu positioned near an anchor element
 * @param {Array} items - Array of menu items: { label, icon?, onClick, color?, hoverBg? }
 * @param {HTMLElement} anchorElement - Element to position menu near
 * @param {string} menuId - Unique ID for the menu
 * @returns {HTMLElement} The created menu element
 */
function createDropdownMenu(items, anchorElement, menuId) {
  // Close any existing menu with same ID
  const existing = document.getElementById(menuId);
  if (existing) existing.remove();
  
  const menu = el('div', '');
  menu.id = menuId;
  menu.style.cssText = `
    position:fixed;
    background:white;
    border:1px solid #dde3ef;
    border-radius:6px;
    box-shadow:0 4px 12px rgba(0,0,0,0.15);
    padding:4px;
    z-index:10000;
    min-width:160px;
  `;
  
  // Position near anchor
  const rect = anchorElement.getBoundingClientRect();
  menu.style.left = (rect.left - 140) + 'px';
  menu.style.top = (rect.bottom + 4) + 'px';
  
  // Add items
  for (const item of items) {
    const btn = el('button', '');
    btn.style.cssText = `
      width:100%;
      text-align:left;
      padding:8px 12px;
      background:none;
      border:none;
      border-radius:4px;
      cursor:pointer;
      font-size:13px;
      color:${item.color || '#1a1f2e'};
      display:flex;
      align-items:center;
      gap:8px;
    `;
    btn.textContent = `${item.icon ? item.icon + ' ' : ''}${item.label}`;
    btn.onmouseenter = () => { btn.style.background = item.hoverBg || '#f0f4fc'; };
    btn.onmouseleave = () => { btn.style.background = 'none'; };
    btn.onclick = () => {
      menu.remove();
      item.onClick();
    };
    menu.append(btn);
  }
  
  document.body.append(menu);
  
  // Close on outside click
  setTimeout(() => {
    const closeHandler = (e) => {
      if (!menu.contains(e.target)) {
        menu.remove();
        document.removeEventListener('click', closeHandler);
      }
    };
    document.addEventListener('click', closeHandler);
  }, 0);
  
  return menu;
}

// Helper to build a device card with drag-and-drop and menu
function buildDeviceCard(device, room, container) {
  const deviceCard = el('div', '');
  
  // Position: absolute if device has x/y coordinates, otherwise static flow
  const hasPosition = device.x !== undefined && device.y !== undefined;
  
  // Add CSS classes
  deviceCard.className = `device-card ${hasPosition ? 'positioned' : 'static'} ${device.type || 'lamp'}`;
  
  // Only set position in inline styles
  if (hasPosition) {
    deviceCard.style.left = device.x + 'px';
    deviceCard.style.top = device.y + 'px';
  }
  
  // Device content
  const icon = el('div', 'device-icon');
  icon.textContent = device.icon || '💡';
  
  const name = el('div', 'device-name');
  name.textContent = device.name;
  
  // Menu button (top-right corner)
  const menuBtn = el('button', 'device-menu-btn');
  menuBtn.textContent = '⋯';
  menuBtn.title = 'Apparaat opties';
  menuBtn.onclick = (e) => {
    e.stopPropagation();
    e.preventDefault();
    openDeviceMenu(device, room, menuBtn);
  };
  
  deviceCard.append(icon, name, menuBtn);
  
  // Show menu button on hover
  deviceCard.onmouseenter = () => { 
    menuBtn.style.opacity = '1';
  };
  deviceCard.onmouseleave = () => { 
    menuBtn.style.opacity = '0';
  };
  
  // HTML5 drag for both repositioning and binding
  deviceCard.ondragstart = (e) => {
    if (e.target === menuBtn) {
      e.preventDefault();
      return;
    }
    
    e.dataTransfer.effectAllowed = 'copyMove'; // Allow both copy (binding panel) and move (room repositioning)
    e.dataTransfer.setData('application/json', JSON.stringify({ 
      device: { ...device, roomName: room.name },
      sourceType: 'room',
      roomId: room.id
    }));
    deviceCard.style.opacity = '0.5';
  };
  
  deviceCard.ondragend = () => {
    deviceCard.style.opacity = '1';
  };
  
  // Click to open binding panel
  deviceCard.onclick = (e) => {
    if (e.target === menuBtn) return;
    showDeviceBindings(device, room);
  };
  
  // Always draggable
  deviceCard.setAttribute('draggable', 'true');
  
  // Tooltip
  deviceCard.title = hasPosition 
    ? 'Sleep om te verplaatsen of naar binding panel te kopiëren'
    : 'Sleep naar binding panel of positioneer in kamer';
  
  return deviceCard;
}

// Device menu: rename, move to room, or delete
function openDeviceMenu(device, room, anchorElement) {
  const s = state.get();
  const otherRooms = s.project.homeView.rooms.filter(r => r.id !== room.id);
  
  const menuItems = [
    {
      label: 'Hernoem',
      icon: '✏️',
      onClick: () => promptRenameDevice(device, room)
    }
  ];
  
  if (otherRooms.length > 0) {
    menuItems.push({
      label: 'Verplaats naar...',
      onClick: () => openRoomSelector(device, room, otherRooms)
    });
  }
  
  menuItems.push({
    label: 'Verwijder',
    color: '#dc3545',
    hoverBg: '#fff0f0',
    onClick: () => {
      if (confirm(`Apparaat "${device.name}" verwijderen uit ${room.name}?`)) {
        dispatch({
          type: 'REMOVE_DEVICE_FROM_ROOM',
          roomId: room.id,
          deviceId: device.id
        });
      }
    }
  });
  
  createDropdownMenu(menuItems, anchorElement, 'device-menu');
}

/**
 * Prompt user to rename a device
 */
function promptRenameDevice(device, room) {
  const newName = prompt('Nieuwe naam voor apparaat:', device.name);
  if (newName && newName.trim() && newName !== device.name) {
    dispatch({
      type: 'UPDATE_DEVICE',
      roomId: room.id,
      deviceId: device.id,
      patch: { name: newName.trim() }
    });
  }
}

// Room selector for moving device
function openRoomSelector(device, fromRoom, targetRooms) {
  const modal = el('div', '');
  modal.style.cssText = `
    position:fixed;
    inset:0;
    background:rgba(0,0,0,0.4);
    display:flex;
    align-items:center;
    justify-content:center;
    z-index:10001;
  `;
  
  const panel = el('div', '');
  panel.style.cssText = `
    background:white;
    border-radius:8px;
    padding:24px;
    box-shadow:0 8px 24px rgba(0,0,0,0.2);
    max-width:400px;
    width:90%;
  `;
  
  const title = el('h3', '');
  title.style.cssText = 'margin:0 0 16px 0;font-size:16px;color:#1a1f2e';
  title.textContent = `Verplaats "${device.name}" naar:`;
  
  const roomsList = el('div', '');
  roomsList.style.cssText = 'display:flex;flex-direction:column;gap:8px;margin-bottom:16px';
  
  targetRooms.forEach(targetRoom => {
    const roomBtn = el('button', '');
    roomBtn.style.cssText = `
      padding:12px 16px;
      background:#f8f9fd;
      border:1px solid #dde3ef;
      border-radius:6px;
      cursor:pointer;
      font-size:14px;
      text-align:left;
      display:flex;
      align-items:center;
      gap:8px;
    `;
    roomBtn.innerHTML = `<span style="font-size:18px">${targetRoom.icon || '🏠'}</span><span>${targetRoom.name}</span>`;
    roomBtn.onmouseenter = () => { roomBtn.style.background = '#e8f0ff'; };
    roomBtn.onmouseleave = () => { roomBtn.style.background = '#f8f9fd'; };
    roomBtn.onclick = () => {
      dispatch({
        type: 'MOVE_DEVICE_TO_ROOM',
        fromRoomId: fromRoom.id,
        toRoomId: targetRoom.id,
        deviceId: device.id
      });
      modal.remove();
    };
    roomsList.append(roomBtn);
  });
  
  const cancelBtn = el('button', '');
  cancelBtn.style.cssText = `
    padding:8px 16px;
    background:none;
    border:1px solid #dde3ef;
    border-radius:6px;
    cursor:pointer;
    font-size:14px;
    width:100%;
  `;
  cancelBtn.textContent = 'Annuleer';
  cancelBtn.onclick = () => modal.remove();
  
  panel.append(title, roomsList, cancelBtn);
  modal.append(panel);
  document.body.append(modal);
  
  modal.onclick = (e) => {
    if (e.target === modal) modal.remove();
  };
}

let _unsubscribe = null;

/** Called when the home view tab is activated */
export function activate() {
  const s = state.get();
  renderRoomsList(s.project.homeView);
  renderHomeCanvas(s.project.homeView);
  initResizableDivider();

  // Subscribe to state changes if not already subscribed
  if (!_unsubscribe) {
    _unsubscribe = state.subscribe(s => {
      // Only re-render if home view is active
      if (s.activeView === 'home') {
        renderRoomsList(s.project.homeView);
        renderHomeCanvas(s.project.homeView);
      }
    });
  }
}

/** Mount button handlers */
export function mount(container) {
  // This is called by router on first load
}

/** Wire buttons (called from main.js after DOM ready) */
export function wireButtons() {
  document.getElementById('btn-add-room')?.addEventListener('click', () => promptAddFloor());
  document.getElementById('btn-add-room-empty')?.addEventListener('click', () => promptAddRoom());
}

// ─── Sidebar: Rooms List ──────────────────────────────────────────────────────

function renderRoomsList(homeView) {
  const list = document.getElementById('rooms-list');
  if (!list) return;

  list.innerHTML = '';

  // Group rooms by floor
  const floorMap = new Map();
  for (const floor of homeView.floors) {
    floorMap.set(floor.id, { ...floor, rooms: [] });
  }
  for (const room of homeView.rooms) {
    const floor = floorMap.get(room.floorId);
    if (floor) floor.rooms.push(room);
  }

  // Render each floor + its rooms
  for (const floor of homeView.floors) {
    const floorData = floorMap.get(floor.id);
    if (!floorData) continue;

    // Floor header
    const floorHeader = el('div', 'sidebar-label');
    floorHeader.style.cssText = 'margin-top:12px;display:flex;justify-content:space-between;align-items:center';
    floorHeader.innerHTML = `<span>${floor.name}</span>`;
    
    const floorMenu = el('span', '');
    floorMenu.style.cssText = 'font-size:10px;color:#a0aaba;cursor:pointer;padding:2px 4px;border-radius:3px';
    floorMenu.textContent = '⋯';
    floorMenu.title = 'Floor opties';
    floorMenu.onclick = (e) => {
      e.stopPropagation();
      openFloorMenu(floor, floorMenu);
    };
    floorHeader.append(floorMenu);
    
    list.append(floorHeader);

    // Rooms in this floor
    if (floorData.rooms.length === 0) {
      const addRoomBtn = el('button', '');
      addRoomBtn.style.cssText = 'margin:4px 8px;padding:5px 8px;border-radius:5px;border:1px dashed #dde3ef;color:#a0aaba;font-size:11px;cursor:pointer;text-align:left;transition:all .15s;background:none;width:calc(100% - 16px)';
      addRoomBtn.textContent = '+ Kamer';
      addRoomBtn.onmouseenter = () => { addRoomBtn.style.borderColor = '#e08c00'; addRoomBtn.style.color = '#e08c00'; addRoomBtn.style.background = '#fef9f0'; };
      addRoomBtn.onmouseleave = () => { addRoomBtn.style.borderColor = '#dde3ef'; addRoomBtn.style.color = '#a0aaba'; addRoomBtn.style.background = 'none'; };
      addRoomBtn.onclick = () => promptAddRoomToFloor(floor);
      list.append(addRoomBtn);
    } else {
      for (const room of floorData.rooms) {
        const item = el('div', 'sidebar-item');
        item.innerHTML = `<span class="icon">${room.icon || '🚪'}</span> ${room.name} <span class="badge">${room.devices.length}</span>`;
        item.onclick = () => selectRoom(room.id);
        list.append(item);
      }
    }
  }
}

function selectRoom(roomId) {
  // TODO: implement canvas zoom/focus
  console.log('Select room:', roomId);
  dispatch({ type: 'SELECT_ROOM', roomId });
}

// ─── Prompt: Add Room ─────────────────────────────────────────────────────────

function promptAddRoom() {
  const s = state.get();
  const floors = s.project.homeView.floors;
  
  if (floors.length === 0) {
    alert('Voeg eerst een verdieping toe.');
    return;
  }

  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:400px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = '<strong>Nieuwe kamer</strong>';
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');

  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.placeholder = 'Woonkamer, Keuken, Slaapkamer...';
  nameInput.type = 'text';

  const iconLabel = el('label', 'modal-label');
  iconLabel.textContent = 'Icoon (optioneel)';
  const iconPicker = createIconPicker(iconLabel, '');

  const floorLabel = el('label', 'modal-label');
  floorLabel.textContent = 'Verdieping';
  const floorSelect = el('select', 'modal-input');
  for (const floor of floors) {
    const option = document.createElement('option');
    option.value = floor.id;
    option.textContent = floor.name;
    floorSelect.append(option);
  }

  body.append(nameLabel, nameInput, iconLabel, iconPicker.container, floorLabel, floorSelect);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  const addBtn = el('button', 'modal-btn-primary');
  addBtn.textContent = 'Toevoegen';
  addBtn.onclick = () => {
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    
    dispatch({
      type: 'ADD_ROOM',
      room: {
        id: makeId(),
        name,
        icon: iconPicker.getValue() || undefined,
        floorId: floorSelect.value,
        canvasX: 50,
        canvasY: 50,
        canvasWidth: 200,
        canvasHeight: 150,
        devices: []
      }
    });
    overlay.remove();
  };

  footer.append(cancelBtn, addBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}

// ─── Floor Menu ───────────────────────────────────────────────────────────────

function openFloorMenu(floor, anchorElement) {
  const menuItems = [
    {
      label: '+ Kamer',
      onClick: () => promptAddRoomToFloor(floor)
    },
    {
      label: 'Hernoem',
      onClick: () => promptRenameFloor(floor)
    },
    {
      label: 'Verwijder',
      onClick: () => {
        const s = state.get();
        const roomsOnFloor = s.project.homeView.rooms.filter(r => r.floorId === floor.id);
        if (roomsOnFloor.length > 0) {
          alert(`Kan verdieping niet verwijderen: bevat nog ${roomsOnFloor.length} kamer(s)`);
          return;
        }
        if (confirm(`Verwijder verdieping "${floor.name}"?`)) {
          dispatch({ type: 'REMOVE_FLOOR', floorId: floor.id });
        }
      },
      style: 'color:#ef4444'
    }
  ];

  createDropdownMenu(menuItems, anchorElement, 'floor-menu');
}

function promptAddRoomToFloor(floor) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:400px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>Nieuwe kamer — ${floor.name}</strong>`;
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');

  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.placeholder = 'Woonkamer, Keuken, Slaapkamer...';
  nameInput.type = 'text';

  const iconLabel = el('label', 'modal-label');
  iconLabel.textContent = 'Icoon (optioneel)';
  const iconPicker = createIconPicker(iconLabel, '');

  body.append(nameLabel, nameInput, iconLabel, iconPicker.container);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  const addBtn = el('button', 'modal-btn-primary');
  addBtn.textContent = 'Toevoegen';
  addBtn.onclick = () => {
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    
    dispatch({
      type: 'ADD_ROOM',
      room: {
        id: makeId(),
        name,
        icon: iconPicker.getValue() || undefined,
        floorId: floor.id,
        canvasX: 50,
        canvasY: 50,
        canvasWidth: 200,
        canvasHeight: 150,
        devices: []
      }
    });
    overlay.remove();
  };

  footer.append(cancelBtn, addBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}

function promptAddFloor() {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:350px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = '<strong>Nieuwe verdieping</strong>';
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.placeholder = 'Gelijkvloers, 1e verdieping, Zolder...';
  nameInput.type = 'text';

  body.append(nameLabel, nameInput);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  const addBtn = el('button', 'modal-btn-primary');
  addBtn.textContent = 'Toevoegen';
  addBtn.onclick = () => {
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    
    dispatch({
      type: 'ADD_FLOOR',
      floor: {
        id: makeId(),
        name
      }
    });
    overlay.remove();
  };

  footer.append(cancelBtn, addBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}

function promptRenameFloor(floor) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:350px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = '<strong>Verdieping hernoemen</strong>';
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.value = floor.name;
  nameInput.type = 'text';
  body.append(nameLabel, nameInput);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  const saveBtn = el('button', 'modal-btn-primary');
  saveBtn.textContent = 'Opslaan';
  saveBtn.onclick = () => {
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    dispatch({ type: 'UPDATE_FLOOR', floorId: floor.id, patch: { name } });
    overlay.remove();
  };

  footer.append(cancelBtn, saveBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
  nameInput.select();
}

// ─── Canvas: Room Rectangles ──────────────────────────────────────────────────

function renderHomeCanvas(homeView) {
  const canvas = document.getElementById('home-canvas');
  const empty = document.getElementById('home-empty');
  if (!canvas || !empty) return;

  if (homeView.rooms.length === 0) {
    empty.style.display = 'flex';
    // Clear any existing room cards
    const existing = canvas.querySelectorAll('.room-card');
    existing.forEach(el => el.remove());
    return;
  }

  empty.style.display = 'none';

  // Clear existing room cards
  const existing = canvas.querySelectorAll('.room-card');
  existing.forEach(el => el.remove());

  // Render each room as a big horizontal card (80% of container width)
  for (const room of homeView.rooms) {
    const card = buildRoomCard(room);
    canvas.append(card);
  }
}

function buildRoomCard(room) {
  const card = el('div', 'room-card');
  
  // Use room.width and room.height if set, otherwise default values
  const roomWidth = room.width || '80%';
  const roomHeight = room.height || '400px';
  
  // Calculate width: 80% of parent container minus some margin
  // Min-width ensures rooms don't get too small on narrow screens
  card.style.cssText = `
    width:${roomWidth};
    height:${roomHeight};
    min-width:600px;
    min-height:300px;
    max-width:1400px;
    background:#fff;
    border:2px solid #dde3ef;
    border-radius:12px;
    padding:20px;
    flex-shrink:0;
    box-shadow:0 2px 12px rgba(0,0,0,0.08);
    transition:box-shadow .15s;
    display:flex;
    flex-direction:column;
    position:relative;
    background-size:cover;
    background-position:center;
    background-repeat:no-repeat;
  `;
  
  // Add background image if set
  if (room.backgroundImage) {
    card.style.backgroundImage = `url(${room.backgroundImage})`;
  }
  
  // Resize handle in bottom-right corner (both width and height)
  const resizeHandle = el('div', '');
  resizeHandle.style.cssText = `
    position:absolute;
    bottom:8px;
    right:8px;
    width:20px;
    height:20px;
    cursor:nwse-resize;
    background:rgba(59,130,246,0.15);
    border:2px solid #3b82f6;
    border-radius:4px;
    opacity:0;
    transition:opacity .15s;
    z-index:10;
  `;
  resizeHandle.title = 'Sleep om grootte aan te passen';
  
  card.onmouseenter = () => { 
    card.style.boxShadow = '0 4px 20px rgba(0,0,0,0.12)'; 
    resizeHandle.style.opacity = '0.7';
  };
  card.onmouseleave = () => { 
    card.style.boxShadow = '0 2px 12px rgba(0,0,0,0.08)'; 
    resizeHandle.style.opacity = '0';
  };
  resizeHandle.onmouseenter = () => { resizeHandle.style.opacity = '1'; };
  
  // Handle resize drag
  resizeHandle.onmousedown = (e) => {
    e.stopPropagation();
    e.preventDefault();
    
    const startX = e.clientX;
    const startY = e.clientY;
    const startWidth = card.offsetWidth;
    const startHeight = card.offsetHeight;
    
    const onMouseMove = (e) => {
      const dx = e.clientX - startX;
      const dy = e.clientY - startY;
      const newWidth = Math.max(400, startWidth + dx);
      const newHeight = Math.max(200, startHeight + dy);
      card.style.width = newWidth + 'px';
      card.style.height = newHeight + 'px';
    };
    
    const onMouseUp = () => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      
      // Save new width and height to room
      dispatch({
        type: 'UPDATE_ROOM',
        roomId: room.id,
        patch: { 
          width: card.style.width,
          height: card.style.height
        }
      });
    };
    
    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
  };
  
  card.append(resizeHandle);

  const header = el('div', '');
  header.style.cssText = 'font-size:18px;font-weight:600;color:#1a1f2e;margin-bottom:16px;display:flex;justify-content:space-between;align-items:center;background:rgba(255,255,255,0.95);padding:12px 16px;border-radius:8px;box-shadow:0 1px 4px rgba(0,0,0,0.05)';
  
  const roomTitle = el('span', '');
  roomTitle.textContent = `${room.icon || '🚪'} ${room.name}`;
  
  const headerButtons = el('div', '');
  headerButtons.style.cssText = 'display:flex;gap:8px;align-items:center';
  
  const addDeviceBtn = el('button', '');
  addDeviceBtn.textContent = '+ Apparaat';
  addDeviceBtn.style.cssText = 'background:none;border:none;color:#3b82f6;cursor:pointer;font-size:13px;padding:4px 8px;border-radius:4px;font-weight:500';
  addDeviceBtn.title = 'Apparaat toevoegen';
  addDeviceBtn.onmouseenter = () => { addDeviceBtn.style.background = '#eff6ff'; };
  addDeviceBtn.onmouseleave = () => { addDeviceBtn.style.background = 'none'; };
  addDeviceBtn.onclick = (e) => {
    e.stopPropagation();
    openUnitPicker(room.id);
  };

  const menuBtn = el('button', '');
  menuBtn.style.cssText = 'background:none;border:none;color:#6a7899;cursor:pointer;font-size:18px;padding:4px 8px;border-radius:4px';
  menuBtn.textContent = '⋯';
  menuBtn.title = 'Kamer opties';
  menuBtn.onmouseenter = () => { menuBtn.style.background = '#f0f4fc'; };
  menuBtn.onmouseleave = () => { menuBtn.style.background = 'none'; };
  menuBtn.onclick = (e) => {
    e.stopPropagation();
    openRoomMenu(room, menuBtn);
  };
  
  headerButtons.append(addDeviceBtn, menuBtn);
  header.append(roomTitle, headerButtons);

  // Devices area (positioned relative for absolute device placement)
  const devicesArea = el('div', '');
  devicesArea.style.cssText = 'flex:1;position:relative;padding:16px;border-radius:8px;background:rgba(248,249,253,0.6);overflow:hidden';
  
  // Make room a drop zone for repositioning devices
  devicesArea.ondragover = (e) => {
    e.preventDefault();
    e.dataTransfer.dropEffect = 'move';
    // Visual feedback: highlight room border
    devicesArea.style.background = 'rgba(239,246,255,0.8)';
    devicesArea.style.outline = '2px dashed #3b82f6';
  };
  
  devicesArea.ondragleave = (e) => {
    // Check if we really left the container (not just moving between children)
    const rect = devicesArea.getBoundingClientRect();
    if (e.clientX < rect.left || e.clientX >= rect.right || 
        e.clientY < rect.top || e.clientY >= rect.bottom) {
      devicesArea.style.background = 'rgba(248,249,253,0.6)';
      devicesArea.style.outline = '';
    }
  };
  
  devicesArea.ondrop = (e) => {
    e.preventDefault();
    devicesArea.style.background = 'rgba(248,249,253,0.6)';
    devicesArea.style.outline = '';
    
    try {
      const data = JSON.parse(e.dataTransfer.getData('application/json'));
      if (data.sourceType === 'room' && data.device) {
        if (data.roomId === room.id) {
          // Device dropped in its own room - reposition it
          const rect = devicesArea.getBoundingClientRect();
          const x = e.clientX - rect.left - 50; // Center device (100px / 2)
          const y = e.clientY - rect.top - 50;
          
          // Constrain to container bounds
          const maxX = devicesArea.offsetWidth - 100;
          const maxY = devicesArea.offsetHeight - 100;
          const newX = Math.max(0, Math.min(maxX, x));
          const newY = Math.max(0, Math.min(maxY, y));
          
          dispatch({
            type: 'UPDATE_DEVICE_POSITION',
            roomId: room.id,
            deviceId: data.device.id,
            x: newX,
            y: newY
          });
        } else {
          // Device dropped from another room - move it to drop location
          const rect = devicesArea.getBoundingClientRect();
          const x = e.clientX - rect.left - 50; // Center device (100px / 2)
          const y = e.clientY - rect.top - 50;
          
          // Constrain to container bounds
          const maxX = devicesArea.offsetWidth - 100;
          const maxY = devicesArea.offsetHeight - 100;
          const newX = Math.max(0, Math.min(maxX, x));
          const newY = Math.max(0, Math.min(maxY, y));
          
          dispatch({
            type: 'MOVE_DEVICE_TO_ROOM',
            deviceId: data.device.id,
            fromRoomId: data.roomId,
            toRoomId: room.id,
            x: newX,
            y: newY
          });
        }
      }
    } catch (err) {
      console.error('Drop error in room:', err);
    }
  };
  
  if (room.devices.length === 0) {
    const emptyMsg = el('div', '');
    emptyMsg.style.cssText = 'width:100%;text-align:center;color:#a0aaba;font-size:13px;padding:40px 0';
    emptyMsg.textContent = 'Geen apparaten in deze kamer';
    devicesArea.append(emptyMsg);
  } else {
    // Render device cards directly in devicesArea
    room.devices.forEach(device => {
      const deviceCard = buildDeviceCard(device, room, devicesArea);
      devicesArea.append(deviceCard);
    });
  }

  card.append(header, devicesArea);

  return card;
}

// ─── Resizable Divider ────────────────────────────────────────────────────────

let _dividerInitialized = false;

function initResizableDivider() {
  if (_dividerInitialized) return;
  
  const divider = document.getElementById('home-divider');
  const topPanel = document.getElementById('home-rooms-panel');
  const bottomPanel = document.getElementById('home-bindings-panel');
  
  if (!divider || !topPanel || !bottomPanel) return;
  
  let isResizing = false;
  let startY = 0;
  let startTopHeight = 0;
  
  divider.addEventListener('mousedown', (e) => {
    isResizing = true;
    startY = e.clientY;
    startTopHeight = topPanel.offsetHeight;
    document.body.style.cursor = 'row-resize';
    e.preventDefault();
  });
  
  document.addEventListener('mousemove', (e) => {
    if (!isResizing) return;
    
    const deltaY = e.clientY - startY;
    const newTopHeight = startTopHeight + deltaY;
    const container = topPanel.parentElement;
    const totalHeight = container.offsetHeight;
    const minHeight = 200;
    const bottomMinHeight = 100;
    
    if (newTopHeight >= minHeight && (totalHeight - newTopHeight) >= bottomMinHeight) {
      const topFlex = newTopHeight / (totalHeight - newTopHeight);
      topPanel.style.flex = topFlex.toFixed(3);
      bottomPanel.style.flex = '1';
    }
  });
  
  document.addEventListener('mouseup', () => {
    if (isResizing) {
      isResizing = false;
      document.body.style.cursor = '';
    }
  });
  
  _dividerInitialized = true;
}

function openRoomMenu(room, anchorElement) {
  const menuItems = [
    {
      label: 'Hernoem',
      icon: '✏️',
      onClick: () => promptRenameRoom(room)
    },
    {
      label: room.backgroundImage ? 'Grondplan wijzigen' : 'Grondplan toevoegen',
      icon: '🗺️',
      onClick: () => promptAddFloorplan(room)
    }
  ];
  
  if (room.backgroundImage) {
    menuItems.push({
      label: 'Verwijder grondplan',
      icon: '🗑',
      color: '#f59e0b',
      hoverBg: '#fef3c7',
      onClick: () => {
        if (confirm('Grondplan verwijderen?')) {
          dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { backgroundImage: null } });
        }
      }
    });
  }
  
  menuItems.push({
    label: 'Verwijder kamer',
    icon: '🗑',
    color: '#ef4444',
    hoverBg: '#fee2e2',
    onClick: () => {
      if (room.devices.length > 0 && !confirm(`Kamer bevat ${room.devices.length} apparaat/apparaten. Toch verwijderen?`)) {
        return;
      }
      if (confirm(`Verwijder kamer "${room.name}"?`)) {
        dispatch({ type: 'REMOVE_ROOM', roomId: room.id });
      }
    }
  });
  
  createDropdownMenu(menuItems, anchorElement, 'room-menu');
}

function promptRenameRoom(room) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:350px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = '<strong>Kamer hernoemen</strong>';
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.value = room.name;
  nameInput.type = 'text';

  const iconLabel = el('label', 'modal-label');
  iconLabel.textContent = 'Icoon';
  const iconPicker = createIconPicker(iconLabel, room.icon || '');

  body.append(nameLabel, nameInput, iconLabel, iconPicker.container);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  const saveBtn = el('button', 'modal-btn-primary');
  saveBtn.textContent = 'Opslaan';
  saveBtn.onclick = () => {
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { name, icon: iconPicker.getValue() || undefined } });
    overlay.remove();
  };

  footer.append(cancelBtn, saveBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
  nameInput.select();
}

// ─── Add/Change Floor Plan ────────────────────────────────────────────────────

function promptAddFloorplan(room) {
  const fileInput = document.createElement('input');
  fileInput.type = 'file';
  fileInput.accept = 'image/*';
  fileInput.style.display = 'none';
  
  fileInput.onchange = (e) => {
    const file = e.target.files?.[0];
    if (!file) return;
    
    if (!file.type.startsWith('image/')) {
      alert('Alleen afbeeldingen zijn toegestaan');
      return;
    }
    
    const reader = new FileReader();
    reader.onload = (evt) => {
      const dataUrl = evt.target.result;
      showCropModal(dataUrl, (croppedDataUrl) => {
        dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { backgroundImage: croppedDataUrl } });
        
        // Show toast notification
        const toast = el('div', '');
        toast.style.cssText = `
          position:fixed;
          bottom:24px;
          right:24px;
          background:#10b981;
          color:#fff;
          padding:12px 20px;
          border-radius:8px;
          box-shadow:0 4px 12px rgba(0,0,0,0.15);
          font-size:14px;
          z-index:10000;
          animation:slideIn .3s ease-out;
        `;
        toast.textContent = '✓ Grondplan toegevoegd';
        document.body.append(toast);
        setTimeout(() => {
          toast.style.animation = 'slideOut .3s ease-out';
          setTimeout(() => toast.remove(), 300);
        }, 2000);
      });
    };
    reader.onerror = () => {
      alert('Fout bij laden van afbeelding');
    };
    reader.readAsDataURL(file);
    
    fileInput.remove();
  };
  
  document.body.append(fileInput);
  fileInput.click();
}

function showCropModal(imageDataUrl, onCrop) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:800px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = '<strong>Grondplan bijsnijden</strong>';
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  body.style.padding = '16px';
  
  // Create canvas container
  const canvasContainer = el('div', '');
  canvasContainer.style.cssText = `
    position:relative;
    width:100%;
    max-height:60vh;
    overflow:auto;
    background:#f3f4f6;
    border-radius:8px;
    display:flex;
    justify-content:center;
    align-items:center;
  `;
  
  const canvas = document.createElement('canvas');
  canvas.style.cssText = 'display:block;max-width:100%;height:auto;cursor:crosshair';
  
  const cropOverlay = el('div', '');
  cropOverlay.style.cssText = `
    position:absolute;
    border:2px solid #e08c00;
    background:rgba(224,140,0,0.1);
    cursor:move;
    box-shadow:0 0 0 9999px rgba(0,0,0,0.5);
  `;
  
  // Create resize handles
  const handles = [];
  const handlePositions = [
    { pos: 'nw', cursor: 'nw-resize', x: 0, y: 0 },
    { pos: 'n', cursor: 'n-resize', x: 0.5, y: 0 },
    { pos: 'ne', cursor: 'ne-resize', x: 1, y: 0 },
    { pos: 'e', cursor: 'e-resize', x: 1, y: 0.5 },
    { pos: 'se', cursor: 'se-resize', x: 1, y: 1 },
    { pos: 's', cursor: 's-resize', x: 0.5, y: 1 },
    { pos: 'sw', cursor: 'sw-resize', x: 0, y: 1 },
    { pos: 'w', cursor: 'w-resize', x: 0, y: 0.5 }
  ];
  
  handlePositions.forEach(h => {
    const handle = el('div', '');
    handle.style.cssText = `
      position:absolute;
      width:10px;
      height:10px;
      background:#fff;
      border:2px solid #e08c00;
      border-radius:50%;
      cursor:${h.cursor};
      transform:translate(-50%, -50%);
    `;
    handle.dataset.pos = h.pos;
    handle.dataset.x = h.x;
    handle.dataset.y = h.y;
    cropOverlay.append(handle);
    handles.push(handle);
  });
  
  // Load image
  const img = new Image();
  img.onload = () => {
    // Set canvas size to image size
    canvas.width = img.width;
    canvas.height = img.height;
    
    const ctx = canvas.getContext('2d');
    ctx.drawImage(img, 0, 0);
    
    // Initial crop area (80% of image, centered)
    const cropMargin = 0.1;
    let cropX = img.width * cropMargin;
    let cropY = img.height * cropMargin;
    let cropW = img.width * 0.8;
    let cropH = img.height * 0.8;
    
    const updateCropOverlay = () => {
      const rect = canvas.getBoundingClientRect();
      const scaleX = rect.width / canvas.width;
      const scaleY = rect.height / canvas.height;
      
      cropOverlay.style.left = (cropX * scaleX) + 'px';
      cropOverlay.style.top = (cropY * scaleY) + 'px';
      cropOverlay.style.width = (cropW * scaleX) + 'px';
      cropOverlay.style.height = (cropH * scaleY) + 'px';
      
      // Update handle positions
      handles.forEach(handle => {
        const hx = parseFloat(handle.dataset.x);
        const hy = parseFloat(handle.dataset.y);
        handle.style.left = (hx * 100) + '%';
        handle.style.top = (hy * 100) + '%';
      });
    };
    
    updateCropOverlay();
    window.addEventListener('resize', updateCropOverlay);
    
    // Drag/resize functionality
    let activeAction = null; // 'move' or 'resize'
    let resizeHandle = null;
    let dragStartX = 0;
    let dragStartY = 0;
    let cropStartX = 0;
    let cropStartY = 0;
    let cropStartW = 0;
    let cropStartH = 0;
    
    handles.forEach(handle => {
      handle.onmousedown = (e) => {
        activeAction = 'resize';
        resizeHandle = handle.dataset.pos;
        dragStartX = e.clientX;
        dragStartY = e.clientY;
        cropStartX = cropX;
        cropStartY = cropY;
        cropStartW = cropW;
        cropStartH = cropH;
        e.stopPropagation();
        e.preventDefault();
      };
    });
    
    cropOverlay.onmousedown = (e) => {
      if (activeAction === 'resize') return;
      activeAction = 'move';
      dragStartX = e.clientX;
      dragStartY = e.clientY;
      cropStartX = cropX;
      cropStartY = cropY;
      e.preventDefault();
    };
    
    document.onmousemove = (e) => {
      if (!activeAction) return;
      
      const rect = canvas.getBoundingClientRect();
      const scaleX = canvas.width / rect.width;
      const scaleY = canvas.height / rect.height;
      
      const deltaX = (e.clientX - dragStartX) * scaleX;
      const deltaY = (e.clientY - dragStartY) * scaleY;
      
      if (activeAction === 'move') {
        cropX = Math.max(0, Math.min(canvas.width - cropW, cropStartX + deltaX));
        cropY = Math.max(0, Math.min(canvas.height - cropH, cropStartY + deltaY));
      } else if (activeAction === 'resize') {
        const minSize = 50;
        
        if (resizeHandle.includes('w')) {
          const newX = Math.max(0, Math.min(cropStartX + cropStartW - minSize, cropStartX + deltaX));
          cropW = cropStartW + (cropStartX - newX);
          cropX = newX;
        }
        if (resizeHandle.includes('e')) {
          cropW = Math.max(minSize, Math.min(canvas.width - cropStartX, cropStartW + deltaX));
        }
        if (resizeHandle.includes('n')) {
          const newY = Math.max(0, Math.min(cropStartY + cropStartH - minSize, cropStartY + deltaY));
          cropH = cropStartH + (cropStartY - newY);
          cropY = newY;
        }
        if (resizeHandle.includes('s')) {
          cropH = Math.max(minSize, Math.min(canvas.height - cropStartY, cropStartH + deltaY));
        }
      }
      
      updateCropOverlay();
    };
    
    document.onmouseup = () => {
      activeAction = null;
      resizeHandle = null;
    };
  };
  img.src = imageDataUrl;
  
  canvasContainer.append(canvas, cropOverlay);
  body.append(canvasContainer);

  const footer = el('div', 'modal-footer');
  footer.style.marginTop = '16px';
  
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  
  const cropBtn = el('button', 'modal-btn-primary');
  cropBtn.textContent = 'Bijsnijden';
  cropBtn.onclick = () => {
    // Create new canvas with cropped image
    const croppedCanvas = document.createElement('canvas');
    croppedCanvas.width = cropW;
    croppedCanvas.height = cropH;
    
    const ctx = croppedCanvas.getContext('2d');
    ctx.drawImage(canvas, cropX, cropY, cropW, cropH, 0, 0, cropW, cropH);
    
    const croppedDataUrl = croppedCanvas.toDataURL('image/png');
    overlay.remove();
    onCrop(croppedDataUrl);
  };

  footer.append(cancelBtn, cropBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', (e) => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
}

// ─── Add Device to Room (DEPRECATED - replaced by unit-picker.js) ────────────
// TODO: Remove this once unit picker is fully tested and working
/*
function promptAddDevice(room) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:500px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>Apparaat toevoegen — ${room.name}</strong>`;
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  
  // Device types with icons and colors
  const deviceTypes = [
    { type: 'lamp', label: 'Lamp', icon: '💡', color: '#fbbf24' },
    { type: 'relay', label: 'Relais', icon: '⚡', color: '#3b82f6' },
    { type: 'dimmer', label: 'Dimmer', icon: '🎚️', color: '#8b5cf6' },
    { type: 'motor', label: 'Motor', icon: '🪟', color: '#06b6d4' },
    { type: 'sensor', label: 'Sensor', icon: '🌡️', color: '#f59e0b' }
  ];
  
  const typeLabel = el('label', 'modal-label');
  typeLabel.textContent = 'Type apparaat';
  
  const typeGrid = el('div', '');
  typeGrid.style.cssText = 'display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:8px;margin-bottom:16px';
  
  let selectedType = null;
  const typeButtons = [];
  
  deviceTypes.forEach(dt => {
    const btn = el('button', '');
    btn.style.cssText = `
      padding:12px;
      border:2px solid #dde3ef;
      border-radius:8px;
      background:#fff;
      cursor:pointer;
      display:flex;
      flex-direction:column;
      align-items:center;
      gap:4px;
      transition:all .15s;
    `;
    btn.innerHTML = `
      <div style="font-size:28px">${dt.icon}</div>
      <div style="font-size:12px;color:#1a1f2e;font-weight:500">${dt.label}</div>
    `;
    btn.onclick = () => {
      selectedType = dt;
      typeButtons.forEach(b => {
        b.style.borderColor = '#dde3ef';
        b.style.background = '#fff';
      });
      btn.style.borderColor = dt.color;
      btn.style.background = `${dt.color}15`;
    };
    btn.onmouseenter = () => {
      if (selectedType !== dt) {
        btn.style.borderColor = '#c0c8d8';
      }
    };
    btn.onmouseleave = () => {
      if (selectedType !== dt) {
        btn.style.borderColor = '#dde3ef';
      }
    };
    typeButtons.push(btn);
    typeGrid.append(btn);
  });
  
  const nameLabel = el('label', 'modal-label');
  nameLabel.textContent = 'Naam apparaat';
  nameLabel.style.marginTop = '16px';
  
  const nameInput = el('input', 'modal-input');
  nameInput.type = 'text';
  nameInput.placeholder = 'Bijv. Lamp woonkamer';
  
  body.append(typeLabel, typeGrid, nameLabel, nameInput);

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  
  const addBtn = el('button', 'modal-btn-primary');
  addBtn.textContent = 'Toevoegen';
  addBtn.onclick = () => {
    if (!selectedType) {
      alert('Selecteer een type apparaat');
      return;
    }
    const name = nameInput.value.trim();
    if (!name) {
      nameInput.style.borderColor = '#ef4444';
      return;
    }
    
    // Set initial position at center of room (or near center)
    // Default room width is ~500px, height ~400px, device is 100x100
    const initialX = Math.max(0, (room.width || 500) / 2 - 50);
    const initialY = Math.max(0, (room.height || 400) / 2 - 50);
    
    const device = {
      id: makeId(),
      name,
      type: selectedType.type,
      icon: selectedType.icon,
      color: selectedType.color,
      x: initialX,
      y: initialY
    };
    
    dispatch({ type: 'ADD_DEVICE_TO_ROOM', roomId: room.id, device });
    overlay.remove();
  };

  footer.append(cancelBtn, addBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}
*/

// ─── Helpers ──────────────────────────────────────────────────────────────────

function el(tag, className) {
  const element = document.createElement(tag);
  if (className) element.className = className;
  return element;
}

/**
 * Create an icon picker with clickable emoji grid.
 * @param {HTMLElement} labelElement - The label element to update with selected icon
 * @param {string} initialValue - Initial icon value
 * @returns {Object} - { container: HTMLElement, getValue: () => string }
 */
function createIconPicker(labelElement, initialValue = '') {
  const roomIcons = ['🛋️', '🍴', '🛏️', '🚿', '🖥️', '🚪', '🏠', '📺', '🎮', '📚', '🧺', '🌿', '🎨', '🎵', '🔧'];
  
  let selectedIcon = initialValue;
  
  // Update label text
  const updateLabel = (icon) => {
    const baseText = labelElement.textContent.split(':')[0].trim();
    labelElement.textContent = icon ? `${baseText}: ${icon}` : baseText;
  };
  
  // Initial label update
  if (initialValue) updateLabel(initialValue);
  
  // Create picker grid
  const container = el('div', '');
  container.style.cssText = 'display:grid;grid-template-columns:repeat(5,1fr);gap:6px;margin-top:8px';
  
  roomIcons.forEach(icon => {
    const btn = el('button', '');
    btn.type = 'button';
    btn.textContent = icon;
    btn.style.cssText = `
      padding:8px;
      border:2px solid ${selectedIcon === icon ? '#e08c00' : '#e5e7eb'};
      border-radius:6px;
      background:${selectedIcon === icon ? '#fef9f0' : '#fff'};
      font-size:20px;
      cursor:pointer;
      transition:all .15s;
    `;
    
    btn.onmouseenter = () => {
      if (selectedIcon !== icon) {
        btn.style.borderColor = '#d1d5db';
        btn.style.background = '#f9fafb';
      }
    };
    
    btn.onmouseleave = () => {
      if (selectedIcon !== icon) {
        btn.style.borderColor = '#e5e7eb';
        btn.style.background = '#fff';
      }
    };
    
    btn.onclick = () => {
      // Deselect all
      container.querySelectorAll('button').forEach(b => {
        b.style.borderColor = '#e5e7eb';
        b.style.background = '#fff';
      });
      
      // Select this one
      selectedIcon = icon;
      btn.style.borderColor = '#e08c00';
      btn.style.background = '#fef9f0';
      updateLabel(icon);
    };
    
    container.append(btn);
  });
  
  return {
    container,
    getValue: () => selectedIcon
  };
}

