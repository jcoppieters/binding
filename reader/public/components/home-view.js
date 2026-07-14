/**
 * Home View — Room-based device layout and configuration
 */

// @ts-check

import { state, dispatch, makeId } from '../app/state.js';
import { showDeviceBindings } from './home-view-binding.js';

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
  
  // Calculate width: 80% of parent container minus some margin
  // Min-width ensures rooms don't get too small on narrow screens
  card.style.cssText = `
    width:80%;
    min-width:600px;
    max-width:1400px;
    height:400px;
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
  
  card.onmouseenter = () => { card.style.boxShadow = '0 4px 20px rgba(0,0,0,0.12)'; };
  card.onmouseleave = () => { card.style.boxShadow = '0 2px 12px rgba(0,0,0,0.08)'; };

  const header = el('div', '');
  header.style.cssText = 'font-size:18px;font-weight:600;color:#1a1f2e;margin-bottom:16px;display:flex;justify-content:space-between;align-items:center;background:rgba(255,255,255,0.95);padding:12px 16px;border-radius:8px;box-shadow:0 1px 4px rgba(0,0,0,0.05)';
  header.innerHTML = `<span>${room.icon || '🚪'} ${room.name}</span>`;

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
  header.append(menuBtn);

  // Devices area (will hold device cards in the future)
  const devicesArea = el('div', '');
  devicesArea.style.cssText = 'flex:1;display:flex;flex-wrap:wrap;gap:12px;align-content:flex-start;padding:16px;border-radius:8px;background:rgba(248,249,253,0.6);overflow:auto';
  
  if (room.devices.length === 0) {
    const emptyMsg = el('div', '');
    emptyMsg.style.cssText = 'width:100%;text-align:center;color:#a0aaba;font-size:13px;padding:40px 0';
    emptyMsg.textContent = 'Geen apparaten in deze kamer';
    devicesArea.append(emptyMsg);
  } else {
    // Render device cards
    room.devices.forEach(device => {
      const deviceCard = el('div', '');
      deviceCard.style.cssText = `
        width:100px;
        height:100px;
        background:${device.color || '#fff'}15;
        border:2px solid ${device.color || '#dde3ef'}40;
        border-radius:8px;
        padding:8px;
        display:flex;
        flex-direction:column;
        align-items:center;
        justify-content:center;
        cursor:pointer;
        transition:all .15s;
      `;
      deviceCard.innerHTML = `<div style="font-size:28px;margin-bottom:4px">${device.icon || '💡'}</div><div style="font-size:11px;color:#1a1f2e;font-weight:500;text-align:center;word-break:break-word">${device.name}</div>`;
      deviceCard.onmouseenter = () => { 
        deviceCard.style.borderColor = device.color || '#c0c8d8';
        deviceCard.style.transform = 'translateY(-2px)'; 
        deviceCard.style.boxShadow = '0 4px 12px rgba(0,0,0,0.1)';
      };
      deviceCard.onmouseleave = () => { 
        deviceCard.style.borderColor = `${device.color || '#dde3ef'}40`;
        deviceCard.style.transform = 'translateY(0)'; 
        deviceCard.style.boxShadow = 'none';
      };
      
      // Make device draggable for binding panel
      deviceCard.draggable = true;
      deviceCard.ondragstart = (e) => {
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
      
      // Click to open binding panel
      deviceCard.onclick = (e) => {
        if (!e.defaultPrevented) {
          showDeviceBindings(device, room);
        }
      };
      
      devicesArea.append(deviceCard);
    });
  }

  const addDeviceBtn = el('button', '');
  addDeviceBtn.textContent = '+ Apparaat toevoegen';
  addDeviceBtn.style.cssText = 'margin-top:12px;font-size:13px;padding:8px 16px;background:#fff;border:1px solid #dde3ef;border-radius:6px;cursor:pointer;align-self:flex-start;transition:all .15s;color:#4a5568';
  addDeviceBtn.onmouseenter = () => { addDeviceBtn.style.background = '#f5f7fb'; addDeviceBtn.style.borderColor = '#c0c8d8'; };
  addDeviceBtn.onmouseleave = () => { addDeviceBtn.style.background = '#fff'; addDeviceBtn.style.borderColor = '#dde3ef'; };
  addDeviceBtn.onclick = (e) => {
    e.stopPropagation();
    promptAddDevice(room);
  };

  card.append(header, devicesArea, addDeviceBtn);

  return card;
}

function showDeviceBindings(device, room) {
  console.log('Show bindings for device:', device.name, 'in room:', room.name);
  // TODO: Implement binding panel visualization
  const bindingsCanvas = document.getElementById('bindings-canvas');
  const bindingsEmpty = document.getElementById('bindings-empty');
  if (bindingsCanvas && bindingsEmpty) {
    bindingsEmpty.style.display = 'none';
    bindingsCanvas.style.display = 'block';
    bindingsCanvas.innerHTML = `<div style="padding:20px;text-align:center;color:#6a7899">Bindingen voor <strong>${device.name}</strong> in <strong>${room.name}</strong> worden hier getoond...</div>`;
  }
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
  body.textContent = 'TODO: Show available channels grouped by type (dimmer, relay, motor, etc.)';

  const footer = el('div', 'modal-footer');
  const cancelBtn = el('button', 'modal-btn');
  cancelBtn.textContent = 'Sluiten';
  cancelBtn.onclick = () => overlay.remove();

  footer.append(cancelBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

function el(tag, className) {
  const element = document.createElement(tag);
  if (className) element.className = className;
  return element;
}
