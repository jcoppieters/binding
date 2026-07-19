/**
 * Module picker modal — lets the installer choose a module from the database.
 *
 * Used for:
 *   - Adding a DIN module to a rail (cabinetId + railId)
 *   - Adding a woning field device (woningType: 'switch' | 'lcd')
 */

import { state, dispatch, makeId } from '../app/state.js';

// ─── Open ─────────────────────────────────────────────────────────────────────

/**
 * @param {{ cabinetId?, railId?, woningType? }} context
 */
export function openModulePicker(context) {
  const s = state.get();
  if (!s.modules || Object.keys(s.modules).length === 0) {
    alert('Module database niet geladen. Herstart de server.');
    return;
  }
  buildAndShow(s.modules, context);
}

// ─── Build & show ─────────────────────────────────────────────────────────────

function buildAndShow(modules, context) {
  // Remove any existing picker
  document.getElementById('module-picker-overlay')?.remove();

  // Determine which categories to show
  const isWoning = !!context.woningType;
  const targetCategories = isWoning
    ? (context.woningType === 'switch' ? ['Schakelaar'] : ['LCD/Touchscreen'])
    : ['Dimmer', 'Relais', 'Motor', 'Input', 'DALI', 'DMX', 'Audio', 'Smartbox', 'Infrastructuur'];

  // Group modules by uiCategory
  const groups = {};
  for (const m of Object.values(modules)) {
    const cat = m.uiCategory;
    if (!targetCategories.includes(cat)) continue;
    if (!groups[cat]) groups[cat] = [];
    groups[cat].push(m);
  }

  const overlay = el('div', '');
  overlay.id = 'module-picker-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:1000;display:flex;align-items:center;justify-content:center';

  const dialog = el('div', '');
  // Explicit height needed for flex:1 children to work correctly inside max-height containers
  dialog.style.cssText = 'background:#fff;border-radius:12px;width:720px;max-width:95vw;height:min(80vh,640px);display:flex;flex-direction:column;box-shadow:0 20px 60px rgba(0,0,0,.3)';

  // Header
  const hdr = el('div', '');
  hdr.style.cssText = 'display:flex;align-items:center;padding:16px 20px;border-bottom:1px solid #dde3ef;background:#f8f9fd;border-radius:12px 12px 0 0';
  const title = el('h2', ''); title.style.cssText = 'font-size:16px;font-weight:700;color:#1a1f2e;flex:1';
  title.textContent = isWoning
    ? (context.woningType === 'switch' ? '＋ Schakelaar toevoegen' : '＋ LCD toevoegen')
    : (context._replaceModuleId ? '🔄 Module type wijzigen' : '＋ Module toevoegen');
  const closeBtn = el('button', ''); closeBtn.textContent = '✕';
  closeBtn.style.cssText = 'background:none;border:none;font-size:18px;cursor:pointer;color:#6a7899;padding:4px 8px';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(title, closeBtn);

  // Category tabs
  const tabs = el('div', '');
  tabs.style.cssText = 'display:flex;gap:4px;padding:12px 20px;border-bottom:1px solid #dde3ef;flex-wrap:wrap;background:#fff';
  let activeTab = targetCategories[0];

  // Module grid (content area)
  const content = el('div', '');
  // min-height:0 is required for flex:1 children to scroll properly in flex containers
  content.style.cssText = 'flex:1;min-height:0;overflow-y:auto;padding:16px 20px';

  // Selected state
  let selectedModel = null;
  let selectedVariant = null;

  function renderGrid(cat) {
    activeTab = cat;
    tabs.querySelectorAll('button').forEach(b => {
      b.style.background = b.dataset.cat === cat ? '#e08c00' : '#f0f2f8';
      b.style.color = b.dataset.cat === cat ? '#fff' : '#4a5568';
    });

    const items = groups[cat] ?? [];
    content.innerHTML = '';
    selectedModel = null; selectedVariant = null;
    updateConfirmBtn();

    if (!items.length) {
      content.innerHTML = '<p style="color:#a0aaba;font-size:13px;padding:8px">Geen producten in deze categorie.</p>';
      return;
    }

    const grid = el('div', '');
    grid.style.cssText = 'display:grid;grid-template-columns:repeat(auto-fill,minmax(180px,1fr));gap:12px';

    for (const item of items) {
      try {
        const card = buildPickerCard(item, (model, variant) => {
          selectedModel = model;
          selectedVariant = variant;
          grid.querySelectorAll('.picker-card').forEach(c => {
            c.style.border = '2px solid #dde3ef';
            c.style.background = '#f8f9fd';
          });
          const found = grid.querySelector(`[data-picker-model="${model}"]`);
          if (found) { found.style.border = '2px solid #e08c00'; found.style.background = '#fef3e0'; }
          updateConfirmBtn();
        });
        grid.append(card);
      } catch (err) {
        console.error('[picker] buildPickerCard failed for', item.model ?? item.functionalModel, err);
      }
    }

    if (!grid.children.length) {
      content.innerHTML = '<p style="color:#f05050;font-size:12px;padding:8px">Fout bij laden van modules. Zie browser console.</p>';
      return;
    }

    content.append(grid);
  }

  // Build tabs
  for (const cat of targetCategories) {
    if (!groups[cat]?.length) continue;
    const btn = el('button', '');
    btn.dataset.cat = cat;
    btn.textContent = cat;
    btn.style.cssText = 'padding:5px 12px;border-radius:5px;border:none;cursor:pointer;font-size:12px;transition:all .12s;background:#f0f2f8;color:#4a5568';
    btn.onclick = () => renderGrid(cat);
    tabs.append(btn);
  }

  // Footer
  const footer = el('div', '');
  footer.style.cssText = 'display:flex;justify-content:flex-end;gap:8px;padding:12px 20px;border-top:1px solid #dde3ef;background:#f8f9fd;border-radius:0 0 12px 12px';
  const cancelBtn = el('button', ''); cancelBtn.textContent = 'Annuleren';
  cancelBtn.style.cssText = 'padding:7px 18px;border-radius:6px;border:1px solid #dde3ef;background:#fff;color:#4a5568;cursor:pointer;font-size:13px';
  cancelBtn.onclick = () => overlay.remove();

  const confirmBtn = el('button', '');
  confirmBtn.textContent = (context._replaceWoningId || context._replaceModuleId) ? 'OK' : 'Toevoegen';
  confirmBtn.style.cssText = 'padding:7px 18px;border-radius:6px;border:none;background:#e08c00;color:#fff;cursor:pointer;font-size:13px;font-weight:600;opacity:.4';
  confirmBtn.disabled = true;
  confirmBtn.id = 'picker-confirm';

  function updateConfirmBtn() {
    confirmBtn.disabled = !selectedModel;
    confirmBtn.style.opacity = selectedModel ? '1' : '.4';
  }

  confirmBtn.onclick = () => {
    if (!selectedModel) return;
    onConfirm(context, selectedModel, selectedVariant);
    overlay.remove();
  };

  footer.append(cancelBtn, confirmBtn);

  dialog.append(hdr, tabs, content, footer);
  overlay.append(dialog);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);

  // Show first available category (in targetCategories order, not insertion order)
  const firstCat = targetCategories.find(cat => groups[cat]?.length);
  if (firstCat) renderGrid(firstCat);
}

// ─── Picker card ──────────────────────────────────────────────────────────────

function buildPickerCard(item, onSelect) {
  const isFamily = !!item.variants;
  const model = item.functionalModel ?? item.model;
  const name = item.productLine ?? item.name ?? model;

  const card = el('div', 'picker-card');
  card.dataset.pickerModel = model;
  card.style.cssText = 'border:2px solid #dde3ef;border-radius:8px;padding:10px;cursor:pointer;background:#f8f9fd;transition:all .12s;display:flex;flex-direction:column;gap:6px';
  card.onmouseover = () => { if (card.style.border !== '2px solid #e08c00') card.style.background = '#f0f4fc'; };
  card.onmouseout = () => { if (card.style.border !== '2px solid #e08c00') card.style.background = '#f8f9fd'; };

  // Image
  const imgEl = el('div', '');
  imgEl.style.cssText = 'width:100%;height:80px;border-radius:4px;background:#e8ecf5;display:flex;align-items:center;justify-content:center;overflow:hidden;flex-shrink:0';
  const firstVariant = isFamily ? item.variants[0] : item;
  if (firstVariant?.imageFile) {
    const img = document.createElement('img');
    img.src = `/modules/images/${firstVariant.imageFile.replace('images/','')}`;
    img.style.cssText = 'max-width:100%;max-height:100%;object-fit:contain;padding:4px';
    img.onerror = () => { imgEl.textContent = categoryIcon(item.uiCategory); imgEl.style.fontSize = '28px'; };
    imgEl.append(img);
  } else {
    imgEl.textContent = categoryIcon(item.uiCategory);
    imgEl.style.fontSize = '28px';
  }
  card.append(imgEl);

  // Name
  const nameEl = el('div', '');
  nameEl.style.cssText = 'font-size:12px;font-weight:600;color:#1a1f2e;line-height:1.3';
  nameEl.textContent = name;
  card.append(nameEl);

  // Specs
  const specs = [];
  if (item.channelGroups?.length) specs.push(channelGroupsSummary(item.channelGroups));
  if (item.dinUnits) specs.push(`${item.dinUnits} DIN`);
  if (item.powerW) specs.push(`${item.powerW}W`);
  if (specs.length) {
    const specEl = el('div', '');
    specEl.style.cssText = 'font-size:10px;color:#6a7899';
    specEl.textContent = specs.join(' · ');
    card.append(specEl);
  }

  // Variant finish selector (for families with variants)
  let finishSelect = null;
  if (isFamily && item.variants.length > 1) {
    finishSelect = document.createElement('select');
    finishSelect.style.cssText = 'font-size:11px;padding:3px 6px;border-radius:4px;border:1px solid #dde3ef;width:100%;color:#4a5568;margin-top:2px';
    item.variants.forEach(v => {
      const opt = document.createElement('option');
      opt.value = v.model;
      opt.textContent = v.finish ?? v.model;
      finishSelect.append(opt);
    });
    card.append(finishSelect);
  }

  card.onclick = (e) => {
    if (e.target === finishSelect) return; // don't intercept select clicks
    const specificModel = finishSelect ? finishSelect.value : (isFamily ? item.variants[0].model : item.model);
    const variant = isFamily ? item.variants.find(v => v.model === specificModel) : null;
    onSelect(specificModel, variant);
  };
  finishSelect?.addEventListener('change', () => {
    const specificModel = finishSelect.value;
    const variant = item.variants.find(v => v.model === specificModel);
    onSelect(specificModel, variant);
    // Update image
    if (variant?.imageFile) {
      const img = imgEl.querySelector('img');
      if (img) img.src = `/modules/images/${variant.imageFile.replace('images/','')}`;
    }
  });

  return card;
}

// ─── Confirm action ───────────────────────────────────────────────────────────

function onConfirm(context, model, variant) {
  // Helper: is this model a woning-type (switch/LCD field device)?
  const moduleDB = state.get().modules;
  const def = Object.values(moduleDB).find(m => m.model === model || m.functionalModel === model
    || m.variants?.some(v => v.model === model));
  const isWoningModel = def?.category === 'switch' || def?.category === 'lcd';

  if (context.woningType) {
    if (context._replaceWoningId) {
      dispatch({ type: 'UPDATE_WONING_DEVICE', deviceId: context._replaceWoningId, patch: { model } });
    } else {
      dispatch({ type: 'ADD_WONING_DEVICE', device: { id: makeId(), model, name: null, position: 99 } });
    }
  } else if (context._replaceModuleId) {
    if (isWoningModel) {
      // Selected model is a field device (LCD/switch) — move from cabinet to woning
      dispatch({ type: 'REMOVE_MODULE', cabinetId: context.cabinetId, moduleId: context._replaceModuleId });
      dispatch({ type: 'ADD_WONING_DEVICE', device: {
        id: makeId(), model, name: null, position: 99,
        nodeAddress: context._keepNodeAddress ?? undefined,
      }});
    } else {
      const patch = { model };
      if (context._keepNodeAddress != null) patch.nodeAddress = context._keepNodeAddress;
      dispatch({ type: 'UPDATE_MODULE', cabinetId: context.cabinetId, moduleId: context._replaceModuleId, patch });
    }
  } else {
    dispatch({ type: 'ADD_MODULE', cabinetId: context.cabinetId, module: { id: makeId(), model, position: 99 } });
  }
}

// ─── Helpers ──────────────────────────────────────────────────────────────────

function el(tag, cls) {
  const e = document.createElement(tag);
  if (cls) e.className = cls;
  return e;
}

function channelGroupsSummary(groups) {
  return groups.map(g => {
    const label = { dimmer_te:'TE', dimmer_le:'LE', dimmer_pwm:'PWM', dimmer_dc:'DC',
      relay_no:'NO', relay_nc:'NC', relay_ssr:'SSR', motor_updown:'Op/neer',
      motor_polar:'Polar', input_digital:'Digital', input_analog:'Analoog',
      dali:'DALI', dmx:'DMX', audio:'Audio' }[g.type] ?? g.type;
    return `${g.count}× ${label}`;
  }).join(', ');
}

function categoryIcon(cat) {
  return { Dimmer:'💡', Relais:'⚡', Motor:'🔄', Input:'🔲', DALI:'🔆', DMX:'🎨',
    Audio:'🔊', Smartbox:'📦', Infrastructuur:'⚙️', Schakelaar:'🔲',
    'LCD/Touchscreen':'🖥', Temperatuursensor:'🌡', Bewegingsdetector:'👁' }[cat] ?? '📦';
}
