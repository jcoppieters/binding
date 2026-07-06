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

export function activate() {
  ensureCSS();
  if (!_unsubscribe) {
    _unsubscribe = state.subscribe(s => render(s));
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
    const w = lookupModule(m.model, moduleDefs)?.dinUnits ?? 2;
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
  card.onclick = () => openModuleConfig(moduleInstance);
  slot.append(card);

  const cdr = el('div', 'cdr'); slot.append(cdr);
  return slot;
}

function buildModuleCard(moduleInstance, def) {
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
  footer.append(dinEl, addrEl);
  face.append(footer);

  card.append(face);

  // Screw terminals bottom
  const termsBot = el('div', 'm-terms bot');
  for (let i = 0; i < screwCount; i++) termsBot.append(el('div', 'm-screw'));
  card.append(termsBot);

  return card;
}

// ─── Woning panel (multi-row snake) ──────────────────────────────────────────

const WONING_MAX_PER_ROW = 4;

function buildWoningPanel(woningDevices, modules) {
  const panel = el('div', 'woning-panel');
  panel.id = 'woning-panel';

  const hdr = el('div', 'woning-header');
  hdr.innerHTML = '🏠 Woning – schakelaars &amp; LCD <span class="seg2-badge">CAN Segment 2</span>';
  panel.append(hdr);

  // Split devices into rows (max WONING_MAX_PER_ROW per row)
  const rows = [];
  if (woningDevices.length === 0) {
    rows.push([]); // always at least one row for the + buttons
  } else {
    for (let i = 0; i < woningDevices.length; i += WONING_MAX_PER_ROW) {
      rows.push(woningDevices.slice(i, i + WONING_MAX_PER_ROW));
    }
  }

  const rowsWrap = el('div', 'woning-rows');

  rows.forEach((rowDevices, rowIdx) => {
    const isLastRow = rowIdx === rows.length - 1;
    const isOdd = rowIdx % 2 === 1; // odd rows go R→L (devices reversed in display)

    const rowEl = el('div', 'woning-row');
    rowEl.id = rowIdx === 0 ? 'woning-row' : `woning-row-${rowIdx}`;
    if (isOdd) rowEl.classList.add('woning-row-reverse');

    // CAN line (seg2 color)
    const canLine = el('div', 'can-line seg2');
    rowEl.append(canLine);

    // For odd rows the CAN enters from the RIGHT — reverse display so logical order is preserved
    const displayDevices = isOdd ? [...rowDevices].reverse() : rowDevices;

    // Left terminator on the very first row — REMOVED:
    // Segment 2 has no left terminator; it continues from the last cabinet rail.
    // The only terminator is at the far end of the last woning row.

    // Devices
    displayDevices.forEach(wd => rowEl.append(buildFieldDevice(wd, modules)));

    // Last row: + buttons then end terminator
    if (isLastRow) {
      const addSwBtn = el('div', 'add-fd-btn');
      addSwBtn.innerHTML = '<span style="font-size:10px">＋ SW</span>';
      addSwBtn.title = 'Schakelaar toevoegen';
      addSwBtn.onclick = () => openModulePicker({ woningType: 'switch' });

      const addLcdBtn = el('div', 'add-fd-btn');
      addLcdBtn.innerHTML = '<span style="font-size:10px">＋ LCD</span>';
      addLcdBtn.title = 'LCD toevoegen';
      addLcdBtn.onclick = () => openModulePicker({ woningType: 'lcd' });

      // On even last row: append right; on odd last row: prepend left
      if (isOdd) {
        rowEl.prepend(addLcdBtn);
        rowEl.prepend(addSwBtn);
      } else {
        rowEl.append(addSwBtn, addLcdBtn);
      }

      // End 60Ω terminator
      const termR = buildTerminator('SEG2');
      termR.id = 'woning-term-r';
      if (isOdd) rowEl.prepend(termR); else rowEl.append(termR);
    }

    rowsWrap.append(rowEl);

    // Snake connector between rows
    if (!isLastRow) {
      const conn = el('div', isOdd ? 'woning-snake-conn left' : 'woning-snake-conn right');
      rowsWrap.append(conn);
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

  // Collect rail-modules bounding rects for bidirectional snake
  const railRows = [...canvas.querySelectorAll('.rail-row')];
  const railInfos = railRows.map((row, idx) => {
    const modulesEl = row.querySelector('.rail-modules');
    const rect = modulesEl ? relRect(modulesEl) : null;
    return rect ? { rect, isOdd: idx % 2 === 1 } : null;
  }).filter(Boolean);

  // Draw snake between consecutive cabinet rails (alternating L/R)
  for (let i = 0; i + 1 < railInfos.length; i++) {
    const curr = railInfos[i], next = railInfos[i + 1];
    if (curr.isOdd) {
      // Odd→Even: snake on LEFT side
      const x = Math.min(curr.rect.left, next.rect.left) - 18;
      svgLine(curr.rect.left, curr.rect.cy, x, curr.rect.cy, CAN1);
      svgLine(x, curr.rect.cy, x, next.rect.cy, CAN1);
      svgLine(x, next.rect.cy, next.rect.left, next.rect.cy, CAN1);
      svgDot(x, curr.rect.cy, CAN1);
      svgDot(x, next.rect.cy, CAN1);
    } else {
      // Even→Odd: snake on RIGHT side
      const x = Math.max(curr.rect.right, next.rect.right) + 18;
      svgLine(curr.rect.right, curr.rect.cy, x, curr.rect.cy, CAN1);
      svgLine(x, curr.rect.cy, x, next.rect.cy, CAN1);
      svgLine(x, next.rect.cy, next.rect.right, next.rect.cy, CAN1);
      svgDot(x, curr.rect.cy, CAN1);
      svgDot(x, next.rect.cy, CAN1);
    }
  }

  // Drop from last cabinet rail to woning panel (Segment 2)
  const woningRows = canvas.querySelectorAll('[id^="woning-row"]');
  const woningRowRects = [...woningRows].map(r => relRect(r)).filter(Boolean);

  if (woningRowRects.length > 0 && railInfos.length > 0) {
    const lastRail = railInfos[railInfos.length - 1];
    const firstWoning = woningRowRects[0];
    // Exit x: left side of odd last rail, right side of even last rail
    const dropX = lastRail.isOdd ? lastRail.rect.left + 20 : lastRail.rect.right - 20;
    svgLine(dropX, lastRail.rect.bottom + 4, dropX, firstWoning.top - 4, CAN2);
    svgDot(dropX, lastRail.rect.bottom + 4, CAN2);
    svgDot(dropX, firstWoning.top - 4, CAN2);
  }

  // Woning row-to-row snake connectors
  for (let i = 0; i + 1 < woningRowRects.length; i++) {
    const r1 = woningRowRects[i], r2 = woningRowRects[i + 1];
    const isOddRow = i % 2 === 0; // after even row → right side connector
    if (isOddRow) {
      const x = Math.max(r1.right, r2.right) + 10;
      svgLine(r1.right, r1.cy, x, r1.cy, CAN2);
      svgLine(x, r1.cy, x, r2.cy, CAN2);
      svgLine(x, r2.cy, r2.right, r2.cy, CAN2);
      svgDot(x, r1.cy, CAN2);
      svgDot(x, r2.cy, CAN2);
    } else {
      const x = Math.min(r1.left, r2.left) - 10;
      svgLine(r1.left, r1.cy, x, r1.cy, CAN2);
      svgLine(x, r1.cy, x, r2.cy, CAN2);
      svgLine(x, r2.cy, r2.left, r2.cy, CAN2);
      svgDot(x, r1.cy, CAN2);
      svgDot(x, r2.cy, CAN2);
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

export function addCabinet(name = 'Nieuwe kast', widthUnits = 12) {
  dispatch({
    type: 'ADD_CABINET',
    cabinet: { id: makeId(), name, widthUnits, modules: [] },
  });
}

function openModuleConfig(moduleInstance) {
  // TODO P6-1: open per-module config modal
  alert(`Config voor ${moduleInstance.model} (nodeAddress: ${moduleInstance.nodeAddress ?? 'niet toegewezen'})`);
}

// ─── Wire buttons ─────────────────────────────────────────────────────────────

export function wireButtons() {
  const btnCabinet = document.getElementById('btn-add-cabinet');
  const btnCabinetEmpty = document.getElementById('btn-add-cabinet-empty');
  const btnModule = document.getElementById('btn-add-module');

  btnCabinet?.addEventListener('click', () => promptAddCabinet());
  btnCabinetEmpty?.addEventListener('click', () => promptAddCabinet());
  btnModule?.addEventListener('click', () => {
    const s = state.get();
    const c = s.project.railView.cabinets[0];
    if (!c) { alert('Voeg eerst een kast toe.'); return; }
    openModulePicker({ cabinetId: c.id });
  });
}

function promptAddCabinet() {
  const name = prompt('Naam van de kast:', 'Hoofdkast');
  if (!name) return;
  const widthStr = prompt('Breedte in M-eenheden (bijv. 12, 18, 24):', '12');
  const widthUnits = Math.max(4, Math.min(72, parseInt(widthStr, 10) || 12));
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
