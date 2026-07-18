/**
 * Home View — Room-based device layout and configuration
 */

// @ts-check

import { state, dispatch, makeId } from '../app/state.js';
import { showDeviceBindings } from './home-view-binding.js';

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
  
  // Drag to reposition within room
  let dragStartX, dragStartY, deviceStartX, deviceStartY;
  let isDraggingPosition = false;
  let dragThresholdMet = false;
  
  // In-room repositioning via mousedown (works for all devices)
  deviceCard.onmousedown = (e) => {
    if (e.target === menuBtn) return; // Don't start drag if clicking menu
    
    dragStartX = e.clientX;
    dragStartY = e.clientY;
    
    // Get current position - for positioned devices use x/y, for static use current visual position
    if (hasPosition) {
      deviceStartX = device.x || 0;
      deviceStartY = device.y || 0;
    } else {
      // For static devices, we need to calculate position relative to container
      // when they become absolutely positioned during drag
      const rect = deviceCard.getBoundingClientRect();
      const containerRect = container.getBoundingClientRect();
      deviceStartX = rect.left - containerRect.left + container.scrollLeft;
      deviceStartY = rect.top - containerRect.top + container.scrollTop;
    }
    
    isDraggingPosition = false;
    dragThresholdMet = false;
    
    const onMouseMove = (e) => {
      const dx = e.clientX - dragStartX;
      const dy = e.clientY - dragStartY;
      const distance = Math.sqrt(dx*dx + dy*dy);
      
      // Only start dragging if moved more than 5px
      if (!dragThresholdMet && distance > 5) {
        dragThresholdMet = true;
        isDraggingPosition = true;
        deviceCard.style.cursor = 'grabbing';
        // Make sure it's absolutely positioned during drag
        deviceCard.style.position = 'absolute';
      }
      
      if (dragThresholdMet) {
        // Constrain to container bounds (100x100 device card)
        const maxX = container.offsetWidth - 100;
        const maxY = container.offsetHeight - 100;
        const newX = Math.max(0, Math.min(maxX, deviceStartX + dx));
        const newY = Math.max(0, Math.min(maxY, deviceStartY + dy));
        
        deviceCard.style.left = newX + 'px';
        deviceCard.style.top = newY + 'px';
      }
    };
    
    const onMouseUp = (e) => {
      document.removeEventListener('mousemove', onMouseMove);
      document.removeEventListener('mouseup', onMouseUp);
      document.removeEventListener('keydown', onKeyDown);
      
      deviceCard.style.cursor = 'move';
      
      if (isDraggingPosition) {
        const dx = e.clientX - dragStartX;
        const dy = e.clientY - dragStartY;
        // Constrain to container bounds (100x100 device card)
        const maxX = container.offsetWidth - 100;
        const maxY = container.offsetHeight - 100;
        const newX = Math.max(0, Math.min(maxX, deviceStartX + dx));
        const newY = Math.max(0, Math.min(maxY, deviceStartY + dy));
        
        dispatch({
          type: 'UPDATE_DEVICE_POSITION',
          roomId: room.id,
          deviceId: device.id,
          x: newX,
          y: newY
        });
      }
      
      isDraggingPosition = false;
      dragThresholdMet = false;
    };
    
    const onKeyDown = (e) => {
      if (e.key === 'Escape') {
        // Cancel drag - restore original position
        document.removeEventListener('mousemove', onMouseMove);
        document.removeEventListener('mouseup', onMouseUp);
        document.removeEventListener('keydown', onKeyDown);
        
        if (isDraggingPosition) {
          // Restore original position visually
          if (hasPosition) {
            deviceCard.style.left = device.x + 'px';
            deviceCard.style.top = device.y + 'px';
          } else {
            // For static devices, remove absolute positioning
            deviceCard.style.position = '';
            deviceCard.style.left = '';
            deviceCard.style.top = '';
          }
        }
        
        deviceCard.style.cursor = 'move';
        isDraggingPosition = false;
        dragThresholdMet = false;
      }
    };
    
    document.addEventListener('mousemove', onMouseMove);
    document.addEventListener('mouseup', onMouseUp);
    document.addEventListener('keydown', onKeyDown);
  };
  
  // Click to open binding panel (only if not dragging)
  deviceCard.onclick = (e) => {
    if (isDraggingPosition) {
      e.preventDefault();
      return;
    }
    showDeviceBindings(device, room);
  };
  
  // HTML5 drag to binding panel (for all devices)
  deviceCard.ondragstart = (e) => {
    // If we're repositioning within room (threshold met), cancel HTML5 drag
    if (dragThresholdMet) {
      e.preventDefault();
      return false;
    }
    // Otherwise allow HTML5 drag to binding panel
    e.dataTransfer.effectAllowed = 'copy';
    e.dataTransfer.setData('application/json', JSON.stringify({ 
      device: { ...device, roomName: room.name },
      sourceType: 'room'
    }));
    deviceCard.style.opacity = '0.5';
  };
  
  deviceCard.ondragend = () => {
    deviceCard.style.opacity = '1';
  };
  
  // Always allow draggable for drag to binding panel
  deviceCard.setAttribute('draggable', 'true');
  
  // Tooltip
  deviceCard.title = hasPosition 
    ? 'Sleep binnen kamer om te verplaatsen, sleep naar binding panel om te koppelen'
    : 'Sleep naar binding panel of positioneer in kamer';
  
  return deviceCard;
}

// Device menu: move to room or delete
function openDeviceMenu(device, room, anchorElement) {
  // Close any existing menu
  const existing = document.getElementById('device-menu');
  if (existing) existing.remove();
  
  const menu = el('div', '');
  menu.id = 'device-menu';
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
  
  // Move to room option
  const s = state.get();
  const otherRooms = s.project.homeView.rooms.filter(r => r.id !== room.id);
  
  if (otherRooms.length > 0) {
    const moveBtn = el('button', '');
    moveBtn.style.cssText = `
      width:100%;
      text-align:left;
      padding:8px 12px;
      background:none;
      border:none;
      border-radius:4px;
      cursor:pointer;
      font-size:13px;
      color:#1a1f2e;
      display:flex;
      align-items:center;
      gap:8px;
    `;
    moveBtn.textContent = 'Verplaats naar...';
    moveBtn.onmouseenter = () => { moveBtn.style.background = '#f0f4fc'; };
    moveBtn.onmouseleave = () => { moveBtn.style.background = 'none'; };
    moveBtn.onclick = () => {
      menu.remove();
      openRoomSelector(device, room, otherRooms);
    };
    menu.append(moveBtn);
  }
  
  // Delete option
  const deleteBtn = el('button', '');
  deleteBtn.style.cssText = `
    width:100%;
    text-align:left;
    padding:8px 12px;
    background:none;
    border:none;
    border-radius:4px;
    cursor:pointer;
    font-size:13px;
    color:#dc3545;
    display:flex;
    align-items:center;
    gap:8px;
  `;
  deleteBtn.textContent = 'Verwijder';
  deleteBtn.onmouseenter = () => { deleteBtn.style.background = '#fff0f0'; };
  deleteBtn.onmouseleave = () => { deleteBtn.style.background = 'none'; };
  deleteBtn.onclick = () => {
    menu.remove();
    if (confirm(`Apparaat "${device.name}" verwijderen uit ${room.name}?`)) {
      dispatch({
        type: 'REMOVE_DEVICE_FROM_ROOM',
        roomId: room.id,
        deviceId: device.id
      });
    }
  };
  menu.append(deleteBtn);
  
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
  }, 10);
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
  document.getElementById('btn-add-room')?.addEventListener('click', () => promptAddRoom());
  document.getElementById('btn-add-room-toolbar')?.addEventListener('click', () => promptAddRoom());
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
      openFloorMenu(floor);
    };
    floorHeader.append(floorMenu);
    
    list.append(floorHeader);

    // Rooms in this floor
    if (floorData.rooms.length === 0) {
      const empty = el('div', '');
      empty.style.cssText = 'padding:8px 10px;font-size:11px;color:#a0aaba;font-style:italic';
      empty.textContent = 'Geen kamers';
      list.append(empty);
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
  const iconInput = el('input', 'modal-input');
  iconInput.placeholder = '🛋️ 🍴 🛏️ 🚿 ...';
  iconInput.maxLength = 4;
  iconInput.style.width = '80px';

  const floorLabel = el('label', 'modal-label');
  floorLabel.textContent = 'Verdieping';
  const floorSelect = el('select', 'modal-input');
  for (const floor of floors) {
    const option = document.createElement('option');
    option.value = floor.id;
    option.textContent = floor.name;
    floorSelect.append(option);
  }

  body.append(nameLabel, nameInput, iconLabel, iconInput, floorLabel, floorSelect);

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
        icon: iconInput.value.trim() || undefined,
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

function openFloorMenu(floor) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:300px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>${floor.name}</strong>`;
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');

  const renameBtn = el('button', 'modal-btn');
  renameBtn.textContent = '✏️ Hernoem';
  renameBtn.style.cssText = 'width:100%;margin-bottom:8px';
  renameBtn.onclick = () => {
    overlay.remove();
    promptRenameFloor(floor);
  };

  const deleteBtn = el('button', 'modal-btn');
  deleteBtn.textContent = '🗑 Verwijder';
  deleteBtn.style.cssText = 'width:100%;color:#ef4444;border-color:#fca5a5';
  deleteBtn.onclick = () => {
    const s = state.get();
    const roomsOnFloor = s.project.homeView.rooms.filter(r => r.floorId === floor.id);
    if (roomsOnFloor.length > 0) {
      alert(`Kan verdieping niet verwijderen: bevat nog ${roomsOnFloor.length} kamer(s)`);
      return;
    }
    if (confirm(`Verwijder verdieping "${floor.name}"?`)) {
      dispatch({ type: 'REMOVE_FLOOR', floorId: floor.id });
      overlay.remove();
    }
  };

  body.append(renameBtn, deleteBtn);

  const footer = el('div', 'modal-footer');
  const closeFooterBtn = el('button', 'modal-btn');
  closeFooterBtn.textContent = 'Sluiten';
  closeFooterBtn.onclick = () => overlay.remove();
  footer.append(closeFooterBtn);

  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
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
    promptAddDevice(room);
  };

  const menuBtn = el('button', '');
  menuBtn.style.cssText = 'background:none;border:none;color:#6a7899;cursor:pointer;font-size:18px;padding:4px 8px;border-radius:4px';
  menuBtn.textContent = '⋯';
  menuBtn.title = 'Kamer opties';
  menuBtn.onmouseenter = () => { menuBtn.style.background = '#f0f4fc'; };
  menuBtn.onmouseleave = () => { menuBtn.style.background = 'none'; };
  menuBtn.onclick = (e) => {
    e.stopPropagation();
    openRoomMenu(room);
  };
  
  headerButtons.append(addDeviceBtn, menuBtn);
  header.append(roomTitle, headerButtons);

  // Devices area (positioned relative for absolute device placement)
  const devicesArea = el('div', '');
  devicesArea.style.cssText = 'flex:1;position:relative;padding:16px;border-radius:8px;background:rgba(248,249,253,0.6);overflow:hidden';
  
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

function openRoomMenu(room) {
  const overlay = el('div', 'modal-overlay');
  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:300px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>${room.name}</strong>`;
  const closeBtn = el('button', 'modal-close');
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');

  const renameBtn = el('button', 'modal-btn');
  renameBtn.textContent = '✏️ Hernoem';
  renameBtn.style.cssText = 'width:100%;margin-bottom:8px';
  renameBtn.onclick = () => {
    overlay.remove();
    promptRenameRoom(room);
  };
  
  // Floor plan upload/change
  const floorplanBtn = el('button', 'modal-btn');
  floorplanBtn.textContent = room.backgroundImage ? '🗺️ Grondplan wijzigen' : '🗺️ Grondplan toevoegen';
  floorplanBtn.style.cssText = 'width:100%;margin-bottom:8px';
  floorplanBtn.onclick = () => {
    overlay.remove();
    promptAddFloorplan(room);
  };
  
  // Remove floor plan (only if exists)
  let removeFloorplanBtn = null;
  if (room.backgroundImage) {
    removeFloorplanBtn = el('button', 'modal-btn');
    removeFloorplanBtn.textContent = '🗑 Verwijder grondplan';
    removeFloorplanBtn.style.cssText = 'width:100%;margin-bottom:8px;color:#f59e0b;border-color:#fde68a';
    removeFloorplanBtn.onclick = () => {
      if (confirm('Grondplan verwijderen?')) {
        dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { backgroundImage: null } });
        overlay.remove();
      }
    };
  }

  const deleteBtn = el('button', 'modal-btn');
  deleteBtn.textContent = '🗑 Verwijder kamer';
  deleteBtn.style.cssText = 'width:100%;color:#ef4444;border-color:#fca5a5';
  deleteBtn.onclick = () => {
    if (room.devices.length > 0 && !confirm(`Kamer bevat ${room.devices.length} apparaat/apparaten. Toch verwijderen?`)) {
      return;
    }
    if (confirm(`Verwijder kamer "${room.name}"?`)) {
      dispatch({ type: 'REMOVE_ROOM', roomId: room.id });
      overlay.remove();
    }
  };

  body.append(renameBtn, floorplanBtn);
  if (removeFloorplanBtn) body.append(removeFloorplanBtn);
  body.append(deleteBtn);

  const footer = el('div', 'modal-footer');
  const closeFooterBtn = el('button', 'modal-btn');
  closeFooterBtn.textContent = 'Sluiten';
  closeFooterBtn.onclick = () => overlay.remove();
  footer.append(closeFooterBtn);

  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
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
  const iconInput = el('input', 'modal-input');
  iconInput.value = room.icon || '';
  iconInput.maxLength = 4;
  iconInput.style.width = '80px';

  body.append(nameLabel, nameInput, iconLabel, iconInput);

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
    dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { name, icon: iconInput.value.trim() || undefined } });
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
      dispatch({ type: 'UPDATE_ROOM', roomId: room.id, patch: { backgroundImage: dataUrl } });
      
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

// ─── Add Device to Room ───────────────────────────────────────────────────────

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
    { type: 'switch', label: 'Schakelaar', icon: '🔘', color: '#a855f7' },
    { type: 'button', label: 'Drukknop', icon: '🔳', color: '#10b981' },
    { type: 'sensor', label: 'Sensor', icon: '🌡️', color: '#f59e0b' },
    { type: 'lamp', label: 'Lamp', icon: '💡', color: '#fbbf24' },
    { type: 'relay', label: 'Relais', icon: '⚡', color: '#3b82f6' },
    { type: 'dimmer', label: 'Dimmer', icon: '🎚️', color: '#8b5cf6' },
    { type: 'motor', label: 'Motor', icon: '🪟', color: '#06b6d4' }
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

// ─── Helpers ──────────────────────────────────────────────────────────────────

function el(tag, className) {
  const element = document.createElement(tag);
  if (className) element.className = className;
  return element;
}
