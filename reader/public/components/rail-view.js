/**
 * Rail View component — renders cabinets/rails/modules from project state.
 *
 * Activated by router.js when the Rail tab is selected.
 */

import { state, dispatch, makeId } from '../app/state.js';
import { openModulePicker } from './module-picker.js';

// ─── CSS injection ────────────────────────────────────────────────────────────

let _cssLoaded = false;
function ensureCSS() {
  if (_cssLoaded) return;
  _cssLoaded = true;
  const link = document.createElement('link');
  link.rel = 'stylesheet';
  link.href = '/components/rail-view.css';
  document.head.appendChild(link);
}

// ─── Activation ───────────────────────────────────────────────────────────────

let _unsubscribe = null;
let _resizeCleanup = null;

export function activate() {
  ensureCSS();
  if (!_unsubscribe) {
    _unsubscribe = state.subscribe(s => render(s));
  }
  // Re-render on window resize so woning rows reflow to the new canvas width
  if (!_resizeCleanup) {
    let _resizeTimer = null;
    const handler = () => {
      clearTimeout(_resizeTimer);
      _resizeTimer = setTimeout(() => render(state.get()), 120);
    };
    window.addEventListener('resize', handler, { passive: true });
    _resizeCleanup = () => window.removeEventListener('resize', handler);
  }
  wireSVG();
}

// ─── Render ───────────────────────────────────────────────────────────────────

function render(s) {
  const { railView } = s.project;
  const canvas = document.getElementById('rail-canvas');
  const emptyState = document.getElementById('rail-empty');
  if (!canvas) return;

  const hasCabinets = railView.cabinets.length > 0;
  if (emptyState) emptyState.style.display = hasCabinets ? 'none' : 'flex';

  // Remove old cabinet cards (keep empty state and SVG overlay)
  canvas.querySelectorAll('.cabinet-card,.woning-panel').forEach(el => el.remove());

  // Render each cabinet
  railView.cabinets.forEach(cabinet => {
    canvas.appendChild(buildCabinetCard(cabinet, s.modules));
  });

  // Render woning panel (field devices)
  if (railView.woningDevices.length > 0 || hasCabinets) {
    canvas.appendChild(buildWoningPanel(railView.woningDevices, s.modules));
  }

  // Update sidebar badge counts
  updateSidebarCounts(railView, s.modules);

  // Redraw CAN SVG
  setTimeout(() => drawCANBus(canvas), 80);
}

// ─── Cabinet card ─────────────────────────────────────────────────────────────

function buildCabinetCard(cabinet, modules) {
  const rails = reflowToRails(cabinet, modules);

  const totalW = cabinet.modules
    .reduce((s, m) => s + (lookupModule(m.model, modules)?.powerW ?? 0), 0);
  const psusNeeded = Math.ceil(totalW / 60);

  const card = el('div', 'cabinet-card');
  card.dataset.cabinetId = cabinet.id;
  // Fixed width based on capacity (keeps SVG snake at predictable x-coordinate)
  card.style.width = `${cabinet.widthUnits * PX_PER_M + 80}px`;

  // Header — click to edit cabinet properties
  const hdr = el('div', 'cabinet-header');
  hdr.title = 'Klik om kast eigenschappen te bewerken';
  hdr.style.cursor = 'pointer';
  hdr.onclick = () => openCabinetModal(cabinet);
  const h3 = el('h3');
  h3.textContent = `🗄️ ${cabinet.name}`;
  const stats = el('div', 'cabinet-stats');
  stats.innerHTML = `
    <div class="cabinet-stat"><span>Rails:</span><span class="val">${rails.length}</span></div>
    <div class="cabinet-stat"><span>Breedte:</span><span class="val">${cabinet.widthUnits}M</span></div>
    <div class="cabinet-stat"><span>Modules:</span><span class="val">${cabinet.modules.length}</span></div>
    ${totalW > 0 ? `<div class="cabinet-stat warn"><span>Verbruik:</span><span class="val">${totalW.toFixed(1)} W</span></div>` : ''}
  `;
  hdr.append(h3, stats);
  card.append(hdr);

  if (totalW > 0) {
    const psu = el('div', 'psu-summary');
    psu.innerHTML = `⚡ Totaalverbruik: <strong>${totalW.toFixed(1)} W</strong> &nbsp;|&nbsp; Voedingen nodig: <strong>${psusNeeded}×60W</strong>`;
    card.append(psu);
  }

  // Render reflowed rails
  rails.forEach((rail, railIdx) => {
    card.append(buildRailRow(cabinet.id, rail, railIdx, railIdx === rails.length - 1, modules));
  });

  return card;
}

// ─── Reflow modules into virtual rails ────────────────────────────────────────

function reflowToRails(cabinet, moduleDefs) {
  const maxM = cabinet.widthUnits ?? 12;
  const rails = [];
  let rail = null;
  let used = 0;

  for (const m of cabinet.modules) {
    const w = lookupModule(m.model, moduleDefs)?.dinUnits ?? 9; // unknown modules assumed 9M (default rendering size)
    if (!rail || used + w > maxM) {
      rail = { id: `${cabinet.id}-r${rails.length}`, label: `Rail ${rails.length + 1}`, modules: [] };
      rails.push(rail);
      used = 0;
    }
    rail.modules.push(m);
    used += w;
  }

  // Always at least one rail (for the + button)
  if (rails.length === 0) {
    rails.push({ id: `${cabinet.id}-r0`, label: 'Rail 1', modules: [] });
  }

  return rails;
}

// ─── Cabinet config modal ─────────────────────────────────────────────────────

function openCabinetModal(cabinet) {
  document.getElementById('cabinet-modal-overlay')?.remove();

  const overlay = el('div', 'modal-overlay');
  overlay.id = 'cabinet-modal-overlay';

  const dlg = el('div', 'modal-dialog');
  dlg.style.width = '340px';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>Kast configuratie</strong>`;
  const closeBtn = el('button', 'modal-close'); closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');

  // Name
  const nameLabel = el('label', 'modal-label'); nameLabel.textContent = 'Naam';
  const nameInput = el('input', 'modal-input');
  nameInput.value = cabinet.name;
  nameInput.type = 'text';

  // Width
  const wLabel = el('label', 'modal-label'); wLabel.textContent = 'Breedte (M-eenheden)';
  const wInput = el('input', 'modal-input');
  wInput.type = 'number'; wInput.min = '4'; wInput.max = '72'; wInput.step = '1';
  wInput.value = String(cabinet.widthUnits ?? 12);

  const hint = el('p', 'modal-hint');
  hint.textContent = 'Typische waarden: 12M (compact), 18M (standaard), 24M (breed). Modules worden automatisch herverdeeld.';

  body.append(nameLabel, nameInput, wLabel, wInput, hint);

  const footer = el('div', 'modal-footer');
  const saveBtn = el('button', 'modal-btn-primary'); saveBtn.textContent = 'Opslaan';
  saveBtn.onclick = () => {
    const name = nameInput.value.trim() || cabinet.name;
    const widthUnits = Math.max(4, Math.min(72, parseInt(wInput.value, 10) || cabinet.widthUnits));
    dispatch({ type: 'SET_CABINET', cabinetId: cabinet.id, patch: { name, widthUnits } });
    overlay.remove();
  };
  const cancelBtn = el('button', 'modal-btn'); cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  footer.append(cancelBtn, saveBtn);

  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
  nameInput.select();
}

// ─── Rail row ─────────────────────────────────────────────────────────────────

function buildRailRow(cabinetId, rail, railIdx, isLastRail, modules) {
  const isOddRail = railIdx % 2 === 1;
  const row = el('div', 'rail-row');
  row.dataset.railId = rail.id;

  const lbl = el('div', 'rail-label');
  lbl.textContent = rail.label;
  row.append(lbl);

  const railModules = el('div', 'rail-modules');
  railModules.style.position = 'relative';
  // Odd rails run R→L so the CAN bus snake continues unbroken
  if (isOddRail) railModules.style.flexDirection = 'row-reverse';
  const canLine = el('div', 'can-line'); railModules.append(canLine);

  // Single 60Ω terminator — only on the very first rail (left end of Segment 1)
  if (railIdx === 0) {
    const termL = buildTerminator('SEG1');
    termL.id = `term-l-${rail.id}`;
    railModules.append(termL);
  }

  // Module slots
  rail.modules.forEach((m, idx) => {
    const slot = buildModuleSlot(cabinetId, m, idx, modules);
    railModules.append(slot);
    if (idx === rail.modules.length - 1) slot.id = `last-slot-${rail.id}`;
  });

  // Add module button — only on the last rail
  if (isLastRail) {
    const addBtn = el('div', 'add-module-btn');
    addBtn.textContent = '＋';
    addBtn.title = 'Module toevoegen';
    addBtn.onclick = () => openModulePicker({ cabinetId });
    railModules.append(addBtn);
  }

  row.append(railModules);
  return row;
}

// ─── Module slot ──────────────────────────────────────────────────────────────

function buildModuleSlot(cabinetId, moduleInstance, idx, modules) {
  const def = lookupModule(moduleInstance.model, modules);
  const slot = el('div', 'module-slot');
  slot.dataset.moduleId = moduleInstance.id;

  const cdl = el('div', 'cdl'); slot.append(cdl);

  const card = buildModuleCard(moduleInstance, def);
  card.onclick = () => openModuleDetail(moduleInstance, cabinetId);
  slot.append(card);

  const cdr = el('div', 'cdr'); slot.append(cdr);
  return slot;
}

// ─── Unit-type capability helpers ────────────────────────────────────────────

// Map Duotecno UnitType byte → human label + channel-group type
const UNIT_TYPE_INFO = {
  1: { label: 'Dimmer',     icon: '💡', channelTypes: ['dimmer_le','dimmer_te','dimmer_pwm','dimmer_dc'] },
  2: { label: 'Relais',     icon: '⚡', channelTypes: ['relay_no','relay_nc','relay_ssr'] },
  3: { label: 'Input',      icon: '🔲', channelTypes: ['input_digital','input_analog'] },
  4: { label: 'Sensor',     icon: '🌡', channelTypes: [] },
  5: { label: 'Audio',      icon: '🔊', channelTypes: ['audio'] },
  7: { label: 'LCD',        icon: '🖥',  channelTypes: [] },
  8: { label: 'Motor',      icon: '🔄', channelTypes: ['motor_updown','motor_polar'] },
  18: { label: 'Alarm',     icon: '🚨', channelTypes: [] },
};

function unitCountsByType(discoveredNode) {
  const counts = {};
  for (const u of (discoveredNode?.units ?? [])) {
    counts[u.type] = (counts[u.type] || 0) + 1;
  }
  return counts;
}

function capabilitySummary(discoveredNode) {
  const counts = unitCountsByType(discoveredNode);
  return Object.entries(counts).map(([type, count]) => {
    const info = UNIT_TYPE_INFO[type] ?? { label: `Type ${type}`, icon: '?', channelTypes: [] };
    return `${info.icon} ${count}× ${info.label}`;
  }).join('  ');
}

/** Suggest module models from the DB based on discovered unit types */
function suggestModules(discoveredNode, moduleDefs) {
  if (!discoveredNode?.units?.length || !moduleDefs?.length) return [];
  const counts = unitCountsByType(discoveredNode);

  // Build a flat list of all modules (standalone + family variants collapsed to family)
  const candidates = [];
  for (const m of moduleDefs) {
    if (!m.channelGroups?.length) continue; // skip modules without channel info
    let score = 0;
    for (const g of m.channelGroups) {
      const unitTypeEntry = Object.entries(UNIT_TYPE_INFO)
        .find(([, info]) => info.channelTypes.includes(g.type));
      if (!unitTypeEntry) continue;
      const unitType = Number(unitTypeEntry[0]);
      const discovered = counts[unitType] ?? 0;
      if (discovered === 0) { score -= 10; continue; } // has channels we didn't see
      const diff = Math.abs(discovered - g.count);
      score += (diff === 0) ? 10 : (diff <= 2 ? 5 : 1);
    }
    // Penalty for extra unit types not in the module
    for (const [uType, cnt] of Object.entries(counts)) {
      if (cnt === 0) continue;
      const info = UNIT_TYPE_INFO[uType];
      if (!info) continue;
      const hasMatchingGroup = m.channelGroups.some(g => info.channelTypes.includes(g.type));
      if (!hasMatchingGroup) score -= 5;
    }
    if (score > 0) {
      const model = m.functionalModel ?? m.model;
      if (!candidates.find(c => c.model === model)) {
        candidates.push({ model, name: m.productLine ?? m.name ?? model, score });
      }
    }
  }
  return candidates.sort((a, b) => b.score - a.score).slice(0, 5);
}

// ─── Unknown module card ──────────────────────────────────────────────────────

function buildUnknownModuleCard(moduleInstance) {
  const card = el('div', 'module-card module-unknown');

  const discovered = state.get().discoveredNodes;
  const discoveredNode = moduleInstance.nodeAddress != null
    ? discovered.find(n => n.nodeAddress === moduleInstance.nodeAddress)
    : null;
  const counts = discoveredNode ? unitCountsByType(discoveredNode) : {};

  // Screw terminals top
  const termsTop = el('div', 'm-terms');
  for (let i = 0; i < 4; i++) termsTop.append(el('div', 'm-screw'));
  card.append(termsTop);

  const face = el('div', 'm-face');

  const badge = el('div', 'm-type');
  badge.textContent = '⚠ Onbekend';
  badge.style.color = '#f59e0b';
  face.append(badge);

  const addrEl = el('div', 'm-model');
  addrEl.textContent = moduleInstance.nodeAddress != null
    ? `N:0x${moduleInstance.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}`
    : 'Geen adres';
  face.append(addrEl);

  if (moduleInstance.name) {
    const nameEl = el('div', 'm-desc'); nameEl.textContent = moduleInstance.name;
    face.append(nameEl);
  }

  // Show unit type breakdown
  if (Object.keys(counts).length) {
    const capsEl = el('div', 'm-desc');
    capsEl.style.cssText = 'font-size:6.5px;color:#f0c060;margin-top:3px;line-height:1.5';
    capsEl.textContent = Object.entries(counts).map(([t, n]) => {
      const info = UNIT_TYPE_INFO[t] ?? { icon: '?', label: `T${t}` };
      return `${info.icon}${n}`;
    }).join(' ');
    face.append(capsEl);
  }

  const hint = el('div', 'm-desc');
  hint.style.cssText = 'font-size:6px;color:#a0a880;margin-top:4px';
  hint.textContent = 'Klik → wijs module toe';
  face.append(hint);

  card.append(face);

  const termsBot = el('div', 'm-terms bot');
  for (let i = 0; i < 4; i++) termsBot.append(el('div', 'm-screw'));
  card.append(termsBot);

  return card;
}

function buildModuleCard(moduleInstance, def) {
  // Special handling for UNKNOWN (auto-discovered, unidentified) modules
  if (moduleInstance.model === 'UNKNOWN') {
    return buildUnknownModuleCard(moduleInstance);
  }

  const card = el('div', 'module-card');
  if (def?.dinUnits >= 12) card.classList.add('wide');
  else if (def?.dinUnits <= 4) card.classList.add('narrow');

  // Screw terminals top
  const termsTop = el('div', 'm-terms');
  const screwCount = def?.dinUnits ? Math.max(2, Math.min(10, def.dinUnits)) : 4;
  for (let i = 0; i < screwCount; i++) termsTop.append(el('div', 'm-screw'));
  card.append(termsTop);

  // Face plate
  const face = el('div', 'm-face');
  const typeEl = el('div', 'm-type');
  typeEl.textContent = uiCategoryLabel(def?.uiCategory);
  face.append(typeEl);

  const modelEl = el('div', 'm-model');
  modelEl.textContent = moduleInstance.model;
  face.append(modelEl);

  if (def?.name) {
    const desc = el('div', 'm-desc');
    desc.textContent = def.name;
    face.append(desc);
  }

  // Channel dots
  if (def?.channelGroups?.length) {
    const chs = el('div', 'm-channels');
    def.channelGroups.forEach(g => {
      const count = Math.min(g.count, 12);
      for (let i = 0; i < count; i++) {
        const dot = el('div', `ch ${g.type}`);
        chs.append(dot);
      }
    });
    face.append(chs);
  }

  // CAN ports
  const ports = el('div', 'm-ports');
  ports.append(el('div', 'm-port seg1'), el('div', 'm-port seg1'));
  face.append(ports);

  // Footer
  const footer = el('div', 'm-footer');
  const dinEl = el('span', 'm-din');
  dinEl.textContent = def?.powerW ? `${def.powerW}W` : '';
  const addrEl = el('span', 'm-addr');
  addrEl.textContent = moduleInstance.nodeAddress ? `N:${moduleInstance.nodeAddress.toString(16).toUpperCase().padStart(2,'0')}` : '—';

  // Discovery status badge (runtime — based on state.discoveredNodes)
  const discovered = state.get().discoveredNodes;
  if (discovered.length > 0 && moduleInstance.nodeAddress != null) {
    const found = discovered.some(n => n.nodeAddress === moduleInstance.nodeAddress);
    const badge = el('span', found ? 'm-badge-found' : 'm-badge-missing');
    badge.textContent = found ? '✓' : '!';
    badge.title = found ? 'Node gevonden in netwerk' : 'Node NIET gevonden in netwerk';
    footer.append(dinEl, addrEl, badge);
  } else {
    footer.append(dinEl, addrEl);
  }
  face.append(footer);

  card.append(face);

  // Screw terminals bottom
  const termsBot = el('div', 'm-terms bot');
  for (let i = 0; i < screwCount; i++) termsBot.append(el('div', 'm-screw'));
  card.append(termsBot);

  return card;
}

// ─── Woning device detail modal ───────────────────────────────────────────────

function openWoningDeviceDetail(wd, modules) {
  document.getElementById('woning-device-overlay')?.remove();

  const s = state.get();
  const def = lookupModule(wd.model, modules);
  const devs = s.project.railView.woningDevices;
  const idx = devs.findIndex(d => d.id === wd.id);

  const overlay = el('div', 'modal-overlay');
  overlay.id = 'woning-device-overlay';

  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:520px;max-width:95vw';

  const hdr = el('div', 'modal-header');
  hdr.innerHTML = `<strong>${def?.productLine ?? def?.name ?? wd.model}</strong>`;
  const closeBtn = el('button', 'modal-close'); closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = el('div', 'modal-body');
  body.style.cssText = 'display:flex;gap:20px';

  // Left: image
  const left = el('div', '');
  left.style.cssText = 'flex:0 0 130px;display:flex;flex-direction:column;gap:8px';
  if (def?.imageFile) {
    const imgWrap = el('div', '');
    imgWrap.style.cssText = 'width:130px;height:100px;border-radius:8px;border:1px solid #e2e8f0;overflow:hidden;background:#f8f9fd;display:flex;align-items:center;justify-content:center';
    const img = document.createElement('img');
    img.src = `/modules/images/${def.imageFile.replace('images/', '')}`;
    img.style.cssText = 'max-width:100%;max-height:100%;object-fit:contain;padding:4px';
    imgWrap.append(img);
    left.append(imgWrap);
  }
  const specDiv = el('div', '');
  specDiv.style.cssText = 'font-size:11px;color:#6a7899;line-height:1.6';
  if (def?.uiCategory) specDiv.innerHTML += `<div>${def.uiCategory}</div>`;
  if (def?.powerW) specDiv.innerHTML += `<div>${def.powerW}W</div>`;
  left.append(specDiv);

  // Right: fields
  const right = el('div', '');
  right.style.cssText = 'flex:1;display:flex;flex-direction:column;gap:12px';

  const nameLabel = el('label', 'modal-label'); nameLabel.textContent = 'Label';
  const nameInput = el('input', 'modal-input');
  nameInput.value = wd.name ?? '';
  nameInput.placeholder = def?.productLine ?? wd.model;

  const addrLabel = el('label', 'modal-label'); addrLabel.textContent = 'Node adres (hex)';
  const addrInput = el('input', 'modal-input');
  addrInput.value = wd.nodeAddress != null ? wd.nodeAddress.toString(16).toUpperCase().padStart(2, '0') : '';
  addrInput.placeholder = '--';
  addrInput.maxLength = 2;
  addrInput.style.width = '80px';

  right.append(nameLabel, nameInput, addrLabel, addrInput);
  body.append(left, right);

  const footer = el('div', 'modal-footer');
  footer.style.justifyContent = 'space-between';

  const leftBtns = el('div', '');
  leftBtns.style.cssText = 'display:flex;gap:6px';

  const moveLeftBtn = el('button', 'modal-btn');
  moveLeftBtn.innerHTML = '\u2190';
  moveLeftBtn.disabled = idx <= 0;
  moveLeftBtn.onclick = () => {
    dispatch({ type: 'MOVE_WONING_DEVICE', deviceId: wd.id, direction: -1 });
    overlay.remove();
  };

  const moveRightBtn = el('button', 'modal-btn');
  moveRightBtn.innerHTML = '\u2192';
  moveRightBtn.disabled = idx < 0 || idx >= devs.length - 1;
  moveRightBtn.onclick = () => {
    dispatch({ type: 'MOVE_WONING_DEVICE', deviceId: wd.id, direction: +1 });
    overlay.remove();
  };

  const deleteBtn = el('button', 'modal-btn');
  deleteBtn.textContent = '\ud83d\uddd1 Verwijder';
  deleteBtn.style.cssText += 'color:#ef4444;border-color:#fca5a5';
  deleteBtn.onclick = () => {
    if (confirm(`Verwijder ${wd.model}?`)) {
      dispatch({ type: 'REMOVE_WONING_DEVICE', deviceId: wd.id });
      overlay.remove();
    }
  };

  leftBtns.append(moveLeftBtn, moveRightBtn, deleteBtn);

  const rightBtns = el('div', '');
  rightBtns.style.cssText = 'display:flex;gap:6px';
  const cancelBtn = el('button', 'modal-btn'); cancelBtn.textContent = 'Sluiten';
  cancelBtn.onclick = () => overlay.remove();
  const saveBtn = el('button', 'modal-btn-primary'); saveBtn.textContent = 'Opslaan';
  saveBtn.onclick = () => {
    const name = nameInput.value.trim() || null;
    const addrStr = addrInput.value.trim();
    const nodeAddress = addrStr ? parseInt(addrStr, 16) : undefined;
    const patch = { name };
    if (nodeAddress != null && !isNaN(nodeAddress)) patch.nodeAddress = nodeAddress;
    dispatch({ type: 'UPDATE_WONING_DEVICE', deviceId: wd.id, patch });
    overlay.remove();
  };
  rightBtns.append(cancelBtn, saveBtn);

  footer.append(leftBtns, rightBtns);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}



const WONING_MAX_PER_ROW = 4;

function buildWoningPanel(woningDevices, modules) {
  const panel = el('div', 'woning-panel');
  panel.id = 'woning-panel';

  const hdr = el('div', 'woning-header');
  hdr.textContent = '🏠 Woning – schakelaars & LCD';
  panel.append(hdr);

  // Compute max items per row dynamically from canvas width
  const canvas = document.getElementById('rail-canvas');
  const panelInner = (canvas?.clientWidth ?? 800) - 76; // subtract canvas pad(40) + panel pad(36)
  const maxItemW = 154; // worst-case: LCD card (130px) + margin (8px) + gap (16px)
  const endOverhead = 280; // terminator + 2 add-buttons + margins/gaps + buffer
  const maxPerRow = Math.max(3, Math.floor((panelInner - endOverhead) / maxItemW));

  // Split devices into rows of maxPerRow
  const rows = [];
  if (woningDevices.length === 0) {
    rows.push([]);
  } else {
    for (let i = 0; i < woningDevices.length; i += maxPerRow) {
      rows.push(woningDevices.slice(i, i + maxPerRow));
    }
  }

  const rowsWrap = el('div', 'woning-rows');

  rows.forEach((rowDevices, rowIdx) => {
    const isLastRow = rowIdx === rows.length - 1;
    const isOdd = rowIdx % 2 === 1; // odd rows run R→L (flex-direction:row-reverse)

    const rowEl = el('div', 'woning-row');
    rowEl.id = rowIdx === 0 ? 'woning-row' : `woning-row-${rowIdx}`;
    if (isOdd) rowEl.classList.add('woning-row-reverse');

    // CAN line (same segment as cabinet — orange)
    rowEl.append(el('div', 'can-line'));

    // Devices in logical order; flex-direction handles visual reversal for odd rows
    rowDevices.forEach(wd => rowEl.append(buildFieldDevice(wd, modules)));

    // Last row: + buttons and terminator
    // For even rows: append to right end; for odd rows (row-reverse): append = visually left end
    if (isLastRow) {
      const addSwBtn = el('div', 'add-fd-btn');
      addSwBtn.innerHTML = '<span style="font-size:10px">＋ SW</span>';
      addSwBtn.title = 'Schakelaar toevoegen';
      addSwBtn.onclick = () => openModulePicker({ woningType: 'switch' });

      const addLcdBtn = el('div', 'add-fd-btn');
      addLcdBtn.innerHTML = '<span style="font-size:10px">＋ LCD</span>';
      addLcdBtn.title = 'LCD toevoegen';
      addLcdBtn.onclick = () => openModulePicker({ woningType: 'lcd' });

      const termR = buildTerminator('SEG1');
      termR.id = 'woning-term-r';

      // Always append — row-reverse handles visual position for odd rows
      rowEl.append(addSwBtn, addLcdBtn, termR);
    }

    rowsWrap.append(rowEl);

    // CSS spacer div between rows (visual snake handled by SVG)
    if (!isLastRow) {
      rowsWrap.append(el('div', isOdd ? 'woning-snake-conn left' : 'woning-snake-conn right'));
    }
  });

  panel.append(rowsWrap);
  return panel;
}

// ─── Field device card (with real product image) ──────────────────────────────

function buildFieldDevice(wd, modules) {
  const def = lookupModule(wd.model, modules);
  const isLcd = def?.category === 'lcd';

  const fd = el('div', 'field-device');
  fd.dataset.deviceId = wd.id;
  fd.style.cursor = 'pointer';
  fd.title = 'Klik voor details';
  fd.onclick = (e) => { e.stopPropagation(); openWoningDeviceDetail(wd, modules); };
  fd.append(el('div', 'cdl'));

  // Device card — try to show actual product image
  const card = el('div', isLcd ? 'fd-card fd-card-lcd' : 'fd-card');

  if (def?.imageFile) {
    const img = document.createElement('img');
    img.src = `/modules/images/${def.imageFile.replace('images/', '')}`;
    img.style.cssText = 'max-width:100%;max-height:100%;object-fit:contain;padding:4px';
    img.onerror = () => { card.textContent = isLcd ? '🖥' : '🔲'; card.style.fontSize = '28px'; };
    card.append(img);
  } else {
    card.textContent = isLcd ? '🖥' : '🔲';
    card.style.fontSize = '28px';
  }

  fd.append(card);

  const lbl = el('div', 'fd-label');
  lbl.textContent = wd.name ?? def?.productLine ?? def?.name?.slice(0, 18) ?? wd.model;
  fd.append(lbl);

  if (wd.nodeAddress != null) {
    const addr = el('div', 'fd-addr');
    addr.textContent = `N:${wd.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}`;
    fd.append(addr);
  }

  fd.append(el('div', 'cdr'));
  return fd;
}



// ─── Terminator ───────────────────────────────────────────────────────────────

function buildTerminator(label) {
  const t = el('div', 'terminator');
  const body = el('div', 'term-body'); body.textContent = '60Ω';
  const lbl = el('div', 'term-label'); lbl.textContent = label;
  t.append(body, lbl);
  return t;
}

// ─── CAN SVG snake ────────────────────────────────────────────────────────────

function wireSVG() {
  let svg = document.getElementById('can-svg');
  if (!svg) {
    svg = document.createElementNS('http://www.w3.org/2000/svg', 'svg');
    svg.id = 'can-svg';
    document.getElementById('rail-canvas')?.prepend(svg);
  }
}

function drawCANBus(canvas) {
  const svg = document.getElementById('can-svg');
  if (!svg || !canvas) return;
  const cRect = canvas.getBoundingClientRect();
  svg.setAttribute('width', canvas.scrollWidth);
  svg.setAttribute('height', canvas.scrollHeight);
  svg.innerHTML = '';

  const CAN1 = '#e08c00', CAN2 = '#2060d0';

  function relRect(el) {
    if (!el) return null;
    const r = el.getBoundingClientRect();
    return {
      left: r.left - cRect.left + canvas.scrollLeft,
      right: r.right - cRect.left + canvas.scrollLeft,
      top: r.top - cRect.top + canvas.scrollTop,
      bottom: r.bottom - cRect.top + canvas.scrollTop,
      cx: (r.left + r.right) / 2 - cRect.left + canvas.scrollLeft,
      cy: (r.top + r.bottom) / 2 - cRect.top + canvas.scrollTop,
    };
  }

  function svgLine(x1, y1, x2, y2, col, w = 4) {
    const l = document.createElementNS('http://www.w3.org/2000/svg', 'line');
    l.setAttribute('x1', x1); l.setAttribute('y1', y1);
    l.setAttribute('x2', x2); l.setAttribute('y2', y2);
    l.setAttribute('stroke', col); l.setAttribute('stroke-width', w);
    l.setAttribute('stroke-linecap', 'round');
    svg.appendChild(l);
  }

  function svgDot(x, y, col) {
    const c = document.createElementNS('http://www.w3.org/2000/svg', 'circle');
    c.setAttribute('cx', x); c.setAttribute('cy', y); c.setAttribute('r', 6);
    c.setAttribute('fill', col); c.setAttribute('stroke', 'white'); c.setAttribute('stroke-width', '2');
    svg.appendChild(c);
  }

  // Cabinet rail snakes — one set of snakes per cabinet, using cabinet card edges for the bend
  const cabinetEls = [...canvas.querySelectorAll('.cabinet-card')];
  cabinetEls.forEach(cabinetEl => {
    const cabRect = relRect(cabinetEl);
    const railRowEls = [...cabinetEl.querySelectorAll('.rail-row')];
    for (let i = 0; i + 1 < railRowEls.length; i++) {
      const m1 = railRowEls[i].querySelector('.rail-modules');
      const m2 = railRowEls[i + 1].querySelector('.rail-modules');
      if (!m1 || !m2) continue;
      const r1 = relRect(m1), r2 = relRect(m2);
      if (i % 2 === 0) {
        // Even→Odd: right-side snake (bend just outside cabinet right edge)
        const x = cabRect.right + 18;
        svgLine(r1.right, r1.cy, x, r1.cy, CAN1);
        svgLine(x, r1.cy, x, r2.cy, CAN1);
        svgLine(x, r2.cy, r2.right, r2.cy, CAN1);
        svgDot(x, r1.cy, CAN1); svgDot(x, r2.cy, CAN1);
      } else {
        // Odd→Even: left-side snake (bend just outside cabinet left edge)
        const x = cabRect.left - 18;
        svgLine(r1.left, r1.cy, x, r1.cy, CAN1);
        svgLine(x, r1.cy, x, r2.cy, CAN1);
        svgLine(x, r2.cy, r2.left, r2.cy, CAN1);
        svgDot(x, r1.cy, CAN1); svgDot(x, r2.cy, CAN1);
      }
    }
  });

  // Drop from last cabinet’s last rail to first woning row (single CAN segment = orange)
  const woningRows = canvas.querySelectorAll('[id^="woning-row"]');
  const woningRowRects = [...woningRows].map(r => relRect(r)).filter(Boolean);

  if (cabinetEls.length > 0 && woningRowRects.length > 0) {
    const lastCabEl = cabinetEls[cabinetEls.length - 1];
    const lastRailEls = [...lastCabEl.querySelectorAll('.rail-row')];
    const lastRailIdx = lastRailEls.length - 1;
    const lastModulesEl = lastRailEls[lastRailIdx]?.querySelector('.rail-modules');
    const lastRect = lastModulesEl ? relRect(lastModulesEl) : null;
    if (lastRect) {
      const isLastOdd = lastRailIdx % 2 === 1;
    // Exit x: left edge of odd last rail, right edge of even last rail (exact CAN bus endpoint)
      const dropX = isLastOdd ? lastRect.left : lastRect.right;
      svgLine(dropX, lastRect.bottom + 4, dropX, woningRowRects[0].top - 4, CAN1);
      svgDot(dropX, lastRect.bottom + 4, CAN1);
      svgDot(dropX, woningRowRects[0].top - 4, CAN1);
    }
  }

  // Woning row-to-row snake connectors (single CAN segment = orange)
  for (let i = 0; i + 1 < woningRowRects.length; i++) {
    const r1 = woningRowRects[i], r2 = woningRowRects[i + 1];
    if (i % 2 === 0) {
      const x = Math.max(r1.right, r2.right) + 10;
      svgLine(r1.right, r1.cy, x, r1.cy, CAN1);
      svgLine(x, r1.cy, x, r2.cy, CAN1);
      svgLine(x, r2.cy, r2.right, r2.cy, CAN1);
      svgDot(x, r1.cy, CAN1); svgDot(x, r2.cy, CAN1);
    } else {
      const x = Math.min(r1.left, r2.left) - 10;
      svgLine(r1.left, r1.cy, x, r1.cy, CAN1);
      svgLine(x, r1.cy, x, r2.cy, CAN1);
      svgLine(x, r2.cy, r2.left, r2.cy, CAN1);
      svgDot(x, r1.cy, CAN1); svgDot(x, r2.cy, CAN1);
    }
  }
}

// ─── Sidebar counts ───────────────────────────────────────────────────────────

function updateSidebarCounts(railView, modules) {
  const cabinetsEl = document.getElementById('cabinets-list');
  if (cabinetsEl) {
    cabinetsEl.innerHTML = railView.cabinets.map(c => {
      const rails = reflowToRails(c, modules);
      return `<div class="sidebar-item active"><span class="icon">🗄️</span> ${c.name} <span class="badge">${rails.length} rails, ${c.widthUnits}M</span></div>`;
    }).join('');
  }
  const seg1 = document.getElementById('seg1-count');
  const seg2 = document.getElementById('seg2-count');
  if (seg1) seg1.textContent = String(railView.cabinets.reduce((s, c) => s + c.modules.length, 0));
  if (seg2) seg2.textContent = String(railView.woningDevices.length);
}

// ─── Actions ──────────────────────────────────────────────────────────────────

const PX_PER_M = 22; // pixels per DIN M-unit (9M module=160px visual + ~40px overhead → ~22px/M)
const CABINET_WIDTHS = [18, 36, 54, 82]; // standard cabinet widths in M-units

export function addCabinet(name = 'Nieuwe kast', widthUnits = 36) {
  dispatch({
    type: 'ADD_CABINET',
    cabinet: { id: makeId(), name, widthUnits, modules: [] },
  });
}

function openModuleDetail(moduleInstance, cabinetId) {
  document.getElementById('module-detail-overlay')?.remove();

  const s = state.get();
  const def = lookupModule(moduleInstance.model, s.modules);
  const cabinet = s.project.railView.cabinets.find(c => c.id === cabinetId);
  const moduleIdx = cabinet?.modules.findIndex(m => m.id === moduleInstance.id) ?? -1;
  const totalModules = cabinet?.modules.length ?? 0;

  const overlay = el('div', 'modal-overlay');
  overlay.id = 'module-detail-overlay';

  const dlg = el('div', 'modal-dialog');
  dlg.style.cssText = 'width:660px;max-width:95vw';

  // Header
  const hdr = el('div', 'modal-header');
  const isUnknown = moduleInstance.model === 'UNKNOWN';
  hdr.innerHTML = `<strong>${isUnknown ? '⚠ Onbekende node' : (def?.productLine ?? def?.name ?? moduleInstance.model)}</strong>`;
  const closeBtn = el('button', 'modal-close'); closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  // Body
  const body = el('div', 'modal-body');
  body.style.cssText = 'display:flex;gap:20px;flex-direction:row';

  // -- Left: image + specs
  const left = el('div', '');
  left.style.cssText = 'flex:0 0 180px;display:flex;flex-direction:column;gap:10px';

  if (def?.imageFile) {
    const imgWrap = el('div', '');
    imgWrap.style.cssText = 'width:180px;height:140px;border-radius:8px;border:1px solid #e2e8f0;overflow:hidden;background:#f8f9fd;display:flex;align-items:center;justify-content:center';
    const img = document.createElement('img');
    img.src = `/modules/images/${def.imageFile.replace('images/','')}`;
    img.style.cssText = 'max-width:100%;max-height:100%;object-fit:contain;padding:6px';
    imgWrap.append(img);
    left.append(imgWrap);
  }

  const specs = el('div', '');
  specs.style.cssText = 'font-size:12px;color:#6a7899;display:flex;flex-direction:column;gap:4px';
  if (def?.uiCategory)  specs.innerHTML += `<div><b>${def.uiCategory}</b></div>`;
  if (def?.dinUnits)    specs.innerHTML += `<div>Breedte: <b>${def.dinUnits}M</b></div>`;
  if (def?.powerW)      specs.innerHTML += `<div>Verbruik: <b>${def.powerW}W</b></div>`;
  if (def?.lengthMm)    specs.innerHTML += `<div>Lengte: ${def.lengthMm}mm</div>`;
  if (moduleInstance.nodeAddress != null) {
    specs.innerHTML += `<div>Node: <b>0x${moduleInstance.nodeAddress.toString(16).toUpperCase().padStart(2,'0')}</b></div>`;
  }
  left.append(specs);

  // -- Right: channel groups + form fields
  const right = el('div', '');
  right.style.cssText = 'flex:1;display:flex;flex-direction:column;gap:14px';

  // For UNKNOWN modules: show discovered capabilities + auto-suggestions
  if (isUnknown) {
    const s = state.get();
    const discoveredNode = moduleInstance.nodeAddress != null
      ? s.discoveredNodes.find(n => n.nodeAddress === moduleInstance.nodeAddress)
      : null;

    if (discoveredNode) {
      // Capabilities breakdown
      const capSect = el('div', '');
      const capTitle = el('div', ''); capTitle.style.cssText = 'font-size:11px;font-weight:700;color:#6a7899;text-transform:uppercase;letter-spacing:.5px;margin-bottom:6px';
      capTitle.textContent = 'Ontdekte kanalen';
      capSect.append(capTitle);

      const counts = unitCountsByType(discoveredNode);
      for (const [uType, count] of Object.entries(counts)) {
        const info = UNIT_TYPE_INFO[uType] ?? { icon: '?', label: `Type ${uType}` };
        const row = el('div', ''); row.style.cssText = 'display:flex;align-items:center;gap:8px;margin-bottom:5px';
        const dots = el('div', ''); dots.style.cssText = 'display:flex;gap:3px;flex-wrap:wrap;max-width:80px';
        for (let i = 0; i < Math.min(count, 12); i++) {
          const d = el('div', ''); d.style.cssText = 'width:10px;height:10px;border-radius:2px;background:#f59e0b;flex-shrink:0';
          dots.append(d);
        }
        const lbl = el('div', ''); lbl.style.cssText = 'font-size:12px;color:#1a1f2e';
        lbl.textContent = `${count}× ${info.icon} ${info.label}`;
        row.append(dots, lbl);
        capSect.append(row);
      }

      const nodeInfo = el('div', ''); nodeInfo.style.cssText = 'font-size:11px;color:#6a7899;margin-top:6px';
      nodeInfo.innerHTML = `<b>Node type:</b> 0x${discoveredNode.type?.toString(16)?.toUpperCase() ?? '?'}`
        + `<br><b>Fysiek adres:</b> 0x${(moduleInstance.physicalAddress ?? discoveredNode.physicalAddress ?? 0).toString(16).toUpperCase().padStart(2,'0')}`
        + `<br><b>Naam:</b> ${discoveredNode.name || '\u2014'}`;
      capSect.append(nodeInfo);
      right.append(capSect);

      // Auto-suggestions
      const suggestions = suggestModules(discoveredNode, s.modules);
      if (suggestions.length) {
        const sugSect = el('div', '');
        const sugTitle = el('div', ''); sugTitle.style.cssText = 'font-size:11px;font-weight:700;color:#6a7899;text-transform:uppercase;letter-spacing:.5px;margin-bottom:6px';
        sugTitle.textContent = 'Mogelijke modules';
        sugSect.append(sugTitle);
        suggestions.forEach(sug => {
          const btn = el('button', 'modal-btn');
          btn.style.cssText = 'display:block;width:100%;text-align:left;margin-bottom:4px;font-size:12px';
          btn.textContent = `${sug.model} — ${sug.name}`;
          btn.onclick = () => {
            const s2 = state.get();
            const m2 = s2.modules;
            // Find if this model is in a family (use functionalModel)
            const familyOrStandalone = m2.find(m => (m.functionalModel ?? m.model) === sug.model);
            const specificModel = familyOrStandalone?.variants?.[0]?.model ?? sug.model;
            dispatch({ type: 'UPDATE_MODULE', cabinetId, moduleId: moduleInstance.id, patch: { model: specificModel } });
            overlay.remove();
          };
          sugSect.append(btn);
        });
        right.append(sugSect);
      }
    }
  } else if (def?.channelGroups?.length) {
    // Known module: show channel groups (existing code)
    const chSect = el('div', '');
    const chTitle = el('div', ''); chTitle.style.cssText = 'font-size:11px;font-weight:700;color:#6a7899;text-transform:uppercase;letter-spacing:.5px;margin-bottom:6px';
    chTitle.textContent = 'Kanalen';
    chSect.append(chTitle);
    const CH_LABELS = { dimmer_te:'Dimmer TE', dimmer_le:'Dimmer LE', dimmer_pwm:'Dimmer PWM', dimmer_dc:'0-10V', relay_no:'Relais NO', relay_nc:'Relais NC', relay_ssr:'Relais SSR', motor_updown:'Motor Op/neer', motor_polar:'Motor Polar', input_digital:'Digitale ingang', input_analog:'Analoge ingang', dali:'DALI', dmx:'DMX', audio:'Audio' };
    const CH_COLORS = { dimmer_te:'#f59e0b', dimmer_le:'#f59e0b', dimmer_pwm:'#f59e0b', dimmer_dc:'#f59e0b', relay_no:'#3b82f6', relay_nc:'#3b82f6', relay_ssr:'#3b82f6', motor_updown:'#ef4444', motor_polar:'#ef4444', input_digital:'#10b981', input_analog:'#10b981', dali:'#eab308', dmx:'#eab308', audio:'#8b5cf6' };
    def.channelGroups.forEach(g => {
      const row = el('div', '');
      row.style.cssText = 'display:flex;align-items:center;gap:8px;margin-bottom:6px';
      const dots = el('div', ''); dots.style.cssText = 'display:flex;gap:3px;flex-wrap:wrap;max-width:100px';
      for (let i = 0; i < Math.min(g.count, 16); i++) {
        const d = el('div', '');
        d.style.cssText = `width:10px;height:10px;border-radius:2px;background:${CH_COLORS[g.type] ?? '#999'};flex-shrink:0`;
        dots.append(d);
      }
      const lbl = el('div', '');
      lbl.style.cssText = 'font-size:12px;color:#1a1f2e';
      lbl.textContent = `${g.count}× ${CH_LABELS[g.type] ?? g.type}`;
      if (g.maxLoadW) { const sub = el('span',''); sub.style.cssText='font-size:10px;color:#a0aaba'; sub.textContent=` (max ${g.maxLoadW}W)`; lbl.append(sub); }
      if (g.maxCurrentA) { const sub = el('span',''); sub.style.cssText='font-size:10px;color:#a0aaba'; sub.textContent=` (max ${g.maxCurrentA}A)`; lbl.append(sub); }
      row.append(dots, lbl);
      chSect.append(row);
    });
    right.append(chSect);
  }

  // Fields: name + nodeAddress
  const nameLabel = el('label', 'modal-label'); nameLabel.textContent = 'Label (optioneel)';
  const nameInput = el('input', 'modal-input');
  nameInput.value = moduleInstance.name ?? '';
  nameInput.placeholder = def?.productLine ?? moduleInstance.model;
  nameInput.type = 'text';

  const addrLabel = el('label', 'modal-label'); addrLabel.textContent = 'Node adres (hex, bijv. 01)';
  const addrInput = el('input', 'modal-input');
  addrInput.value = moduleInstance.nodeAddress != null ? moduleInstance.nodeAddress.toString(16).toUpperCase().padStart(2,'0') : '';
  addrInput.placeholder = '--';
  addrInput.maxLength = 2;
  addrInput.style.width = '80px';
  addrInput.pattern = '[0-9a-fA-F]{1,2}';

  right.append(nameLabel, nameInput, addrLabel, addrInput);
  body.append(left, right);

  // Footer
  const footer = el('div', 'modal-footer');
  footer.style.justifyContent = 'space-between';

  const leftBtns = el('div', '');
  leftBtns.style.cssText = 'display:flex;gap:6px';

  const moveLeftBtn = el('button', 'modal-btn');
  moveLeftBtn.innerHTML = '← Links';
  moveLeftBtn.title = 'Verschuif module naar links';
  moveLeftBtn.disabled = moduleIdx <= 0;
  moveLeftBtn.onclick = () => {
    dispatch({ type: 'MOVE_MODULE', cabinetId, moduleId: moduleInstance.id, direction: -1 });
    overlay.remove();
  };

  const moveRightBtn = el('button', 'modal-btn');
  moveRightBtn.innerHTML = 'Rechts →';
  moveRightBtn.title = 'Verschuif module naar rechts';
  moveRightBtn.disabled = moduleIdx < 0 || moduleIdx >= totalModules - 1;
  moveRightBtn.onclick = () => {
    dispatch({ type: 'MOVE_MODULE', cabinetId, moduleId: moduleInstance.id, direction: +1 });
    overlay.remove();
  };

  const deleteBtn = el('button', 'modal-btn');
  deleteBtn.textContent = '🗑 Verwijder';
  deleteBtn.style.cssText += 'color:#ef4444;border-color:#fca5a5';
  deleteBtn.onclick = () => {
    if (confirm(`Verwijder ${moduleInstance.model}?`)) {
      dispatch({ type: 'REMOVE_MODULE', cabinetId, moduleId: moduleInstance.id });
      overlay.remove();
    }
  };

  leftBtns.append(moveLeftBtn, moveRightBtn, deleteBtn);

  const rightBtns = el('div', '');
  rightBtns.style.cssText = 'display:flex;gap:6px';

  // For UNKNOWN modules: offer a "Assign module type" button
  if (isUnknown) {
    const assignBtn = el('button', 'modal-btn-primary');
    assignBtn.textContent = '🔍 Wijs module toe';
    assignBtn.title = 'Kies het correcte module type voor deze node';
    assignBtn.onclick = () => {
      overlay.remove();
      // Open module picker, and on confirm UPDATE_MODULE with the chosen model
      openModulePicker({
        cabinetId,
        _replaceModuleId: moduleInstance.id,
        _keepNodeAddress: moduleInstance.nodeAddress,
      });
    };
    rightBtns.append(assignBtn);
  }

  const cancelBtn = el('button', 'modal-btn'); cancelBtn.textContent = 'Sluiten';
  cancelBtn.onclick = () => overlay.remove();

  const saveBtn = el('button', 'modal-btn-primary'); saveBtn.textContent = 'Opslaan';
  saveBtn.style.display = isUnknown ? 'none' : '';
  saveBtn.onclick = () => {
    const name = nameInput.value.trim() || null;
    const addrStr = addrInput.value.trim();
    const nodeAddress = addrStr ? parseInt(addrStr, 16) : undefined;
    const patch = { name };
    if (nodeAddress != null && !isNaN(nodeAddress)) patch.nodeAddress = nodeAddress;
    dispatch({ type: 'UPDATE_MODULE', cabinetId, moduleId: moduleInstance.id, patch });
    overlay.remove();
  };

  rightBtns.append(cancelBtn, saveBtn);
  footer.append(leftBtns, rightBtns);

  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  nameInput.focus();
}

// ─── Wire buttons ─────────────────────────────────────────────────────────────

export function wireButtons() {
  const btnCabinet = document.getElementById('btn-add-cabinet');
  const btnCabinetEmpty = document.getElementById('btn-add-cabinet-empty');
  const btnModule = document.getElementById('btn-add-module');
  const btnSwitch = document.getElementById('btn-add-switch');
  const btnLcd = document.getElementById('btn-add-lcd');

  btnCabinet?.addEventListener('click', () => promptAddCabinet());
  btnCabinetEmpty?.addEventListener('click', () => promptAddCabinet());
  btnModule?.addEventListener('click', () => {
    const s = state.get();
    const c = s.project.railView.cabinets[0];
    if (!c) { alert('Voeg eerst een kast toe.'); return; }
    openModulePicker({ cabinetId: c.id });
  });
  btnSwitch?.addEventListener('click', () => openModulePicker({ woningType: 'switch' }));
  btnLcd?.addEventListener('click', () => openModulePicker({ woningType: 'lcd' }));
}

function promptAddCabinet() {
  const name = prompt('Naam van de kast:', 'Hoofdkast');
  if (!name) return;
  const widthStr = prompt('Breedte in M-eenheden (18, 36, 54 of 82):', '36');
  const w = parseInt(widthStr, 10);
  const widthUnits = CABINET_WIDTHS.includes(w) ? w : (CABINET_WIDTHS.reduce((a, b) => Math.abs(b - w) < Math.abs(a - w) ? b : a, 36));
  addCabinet(name, widthUnits);
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

function el(tag, className = '') {
  const e = document.createElement(tag);
  if (className) e.className = className;
  return e;
}

function lookupModule(model, modules) {
  if (!modules?.length) return null;
  // Check standalone entries
  const standalone = modules.find(m => m.model === model);
  if (standalone) return standalone;
  // Check variant within families
  for (const m of modules) {
    if (m.variants) {
      const v = m.variants.find(v => v.model === model);
      if (v) return { ...m, ...v }; // merge family properties with variant
    }
  }
  return null;
}

function uiCategoryLabel(uiCategory) {
  const map = {
    Dimmer: 'Dimmer', Relais: 'Relais', Motor: 'Motor', Input: 'Input',
    DALI: 'DALI', DMX: 'DMX', Audio: 'Audio', Smartbox: 'Smartbox',
    'Smartbox plugin': 'Plugin', Infrastructuur: 'Infra',
  };
  return map[uiCategory] ?? uiCategory ?? '';
}
