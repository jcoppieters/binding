/**
 * Home View — Room-based device layout and configuration
 */

// @ts-check

import { state, dispatch, makeId } from '../app/state.js';

let _unsubscribe = null;

/** Called when the home view tab is activated */
export function activate() {
  const s = state.get();
  renderRoomsList(s.project.homeView);
  renderHomeCanvas(s.project.homeView);

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

  // Render each room as a draggable card
  for (const room of homeView.rooms) {
    const card = buildRoomCard(room);
    canvas.append(card);
  }
}

function buildRoomCard(room) {
  const card = el('div', 'room-card');
  card.style.cssText = `
    position:absolute;
    left:${room.canvasX}px;
    top:${room.canvasY}px;
    width:${room.canvasWidth}px;
    height:${room.canvasHeight}px;
    background:#fff;
    border:2px solid #dde3ef;
    border-radius:8px;
    padding:12px;
    cursor:move;
    box-shadow:0 2px 8px rgba(0,0,0,0.08);
    transition:box-shadow .15s;
  `;
  card.onmouseenter = () => { card.style.boxShadow = '0 4px 16px rgba(0,0,0,0.12)'; };
  card.onmouseleave = () => { card.style.boxShadow = '0 2px 8px rgba(0,0,0,0.08)'; };

  const header = el('div', '');
  header.style.cssText = 'font-size:14px;font-weight:600;color:#1a1f2e;margin-bottom:8px;display:flex;justify-content:space-between;align-items:center';
  header.innerHTML = `<span>${room.icon || '🚪'} ${room.name}</span>`;

  const menuBtn = el('button', '');
  menuBtn.style.cssText = 'background:none;border:none;color:#6a7899;cursor:pointer;font-size:16px;padding:2px 4px';
  menuBtn.textContent = '⋯';
  menuBtn.onclick = (e) => {
    e.stopPropagation();
    openRoomMenu(room);
  };
  header.append(menuBtn);

  const devicesList = el('div', '');
  devicesList.style.cssText = 'font-size:11px;color:#6a7899;line-height:1.6';
  if (room.devices.length === 0) {
    devicesList.textContent = 'Geen apparaten';
  } else {
    devicesList.innerHTML = room.devices.map(d => `• ${d.name}`).join('<br>');
  }

  const addDeviceBtn = el('button', 'modal-btn');
  addDeviceBtn.textContent = '+ Apparaat';
  addDeviceBtn.style.cssText = 'margin-top:8px;font-size:11px;padding:4px 10px;width:100%';
  addDeviceBtn.onclick = (e) => {
    e.stopPropagation();
    promptAddDevice(room);
  };

  card.append(header, devicesList, addDeviceBtn);

  // TODO: Implement drag to move
  // makeDraggable(card, room);

  return card;
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

  const deleteBtn = el('button', 'modal-btn');
  deleteBtn.textContent = '🗑 Verwijder';
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
