/**
 * Mood Editor modal — edit a mood's action list (what happens when it's triggered).
 *
 * Opened from the "⋯" button on a mood device card in the Binding Pane.
 * Data model: `project.moods[]` (MoodDefinition), keyed by the mood's own
 * channelRef — see models/project.ts and app/mood-types.js for the codec.
 *
 * Note: `userAdjustable` here is just a plain field we set for the installer's
 * own use (whether the end-user's App may adjust this mood) — this editor
 * itself is never gated by it.
 */

import { state, dispatch } from '../app/state.js';
import { showToast } from '../app/main.js';
import { CATEGORY_TO_MOOD_CODE, MOOD_CODE_TO_CATEGORY, MOOD_METHODS, defaultAction, toMsg, decodeMsg } from '../app/mood-types.js';
import { UNIT_TYPE_INFO } from '../app/unit-types.js';

const SUPPORTED_CATEGORIES = ['dimmer', 'relay', 'motor', 'sensor', 'mood'];

/**
 * All channels this editor can target, excluding the mood being edited itself.
 * Prefers devices placed in a room (nicer room-qualified label); falls back to
 * the raw node/unit catalog (`project.nodes[]`) for anything not yet placed,
 * so imported legacy actions targeting unplaced channels still show a real name.
 */
function getAssignableTargets(project, selfChannelRef) {
  const isSelf = (c) => c.nodeAddress === selfChannelRef.nodeAddress && c.unitAddress === selfChannelRef.unitAddress;
  const key = (c) => `${c.nodeAddress}-${c.unitAddress}`;
  const targets = [];
  const seen = new Set();

  for (const room of project.homeView.rooms) {
    for (const device of room.devices) {
      if (device.channelRef && SUPPORTED_CATEGORIES.includes(device.type) && !isSelf(device.channelRef)) {
        targets.push({ channelRef: device.channelRef, label: `${device.name} (${room.name})`, category: device.type });
        seen.add(key(device.channelRef));
      }
    }
  }

  for (const node of project.nodes) {
    for (const unit of node.units || []) {
      const channelRef = { nodeAddress: node.nodeAddress, unitAddress: unit.unitAddress };
      if (isSelf(channelRef) || seen.has(key(channelRef))) continue;
      const category = unit.type === 7 ? 'mood' : UNIT_TYPE_INFO[unit.type]?.category;
      if (category && SUPPORTED_CATEGORIES.includes(category)) {
        targets.push({ channelRef, label: `${unit.name || `0x${node.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}; 0x${unit.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`}`, category });
        seen.add(key(channelRef));
      }
    }
  }

  return targets;
}

function findTarget(targets, channelRef) {
  return targets.find(t => t.channelRef.nodeAddress === channelRef.nodeAddress && t.channelRef.unitAddress === channelRef.unitAddress);
}

/** Credentials/host needed for the moods HTTP API, or null when not connected. */
function moodsApiConfig() {
  const meta = state.get().project.meta;
  if (!meta.masterIp || !meta.moodsApiClientId || !meta.moodsApiRefreshToken) return null;
  return {
    host: meta.masterIp,
    apiPort: meta.masterApiPort ?? 8081,
    password: meta.masterPassword ?? '',
    clientId: meta.moodsApiClientId,
    refreshToken: meta.moodsApiRefreshToken,
  };
}

export function openMoodEditor(moodDevice) {
  const existing = document.getElementById('mood-editor-overlay');
  if (existing) existing.remove();

  const project = state.get().project;
  const channelRef = moodDevice.channelRef;
  const stored = (project.moods || []).find(
    m => m.channelRef.nodeAddress === channelRef.nodeAddress && m.channelRef.unitAddress === channelRef.unitAddress
  );

  const targets = getAssignableTargets(project, channelRef);

  // Working copy — only committed to state on Save.
  const mood = stored
    ? { ...stored, actions: stored.actions.map(a => ({ ...a })) }
    : { channelRef, name: moodDevice.name, description: '', userAdjustable: true, actions: [] };

  const overlay = document.createElement('div');
  overlay.id = 'mood-editor-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:3000;display:flex;align-items:center;justify-content:center';

  const dlg = document.createElement('div');
  dlg.style.cssText = 'background:#fff;border-radius:10px;width:640px;max-width:95vw;max-height:85vh;box-shadow:0 20px 60px rgba(0,0,0,.3);overflow:hidden;display:flex;flex-direction:column';

  dlg.innerHTML = `
    <div style="display:flex;align-items:center;padding:16px 20px;border-bottom:1px solid #eef1f8;background:#f8f9fd;font-size:15px;font-weight:600;color:#1a1f2e">
      <span>🎭 Mood bewerken</span>
      <div style="flex:1"></div>
      <button id="mood-editor-close" style="background:none;border:none;font-size:20px;color:#6a7899;cursor:pointer;line-height:1">×</button>
    </div>
    <div style="padding:20px;flex:1;overflow-y:auto">
      <div style="display:flex;gap:12px;margin-bottom:12px">
        <input type="text" id="mood-editor-name" value="${escapeAttr(mood.name)}" placeholder="Naam"
          style="flex:1;padding:8px 12px;border:1px solid #dde3ef;border-radius:6px;font-size:13px" />
        <input type="text" id="mood-editor-desc" value="${escapeAttr(mood.description || '')}" placeholder="Omschrijving (optioneel)"
          style="flex:1;padding:8px 12px;border:1px solid #dde3ef;border-radius:6px;font-size:13px" />
      </div>
      <label style="display:flex;align-items:center;gap:8px;font-size:13px;color:#4a5568;margin-bottom:16px;cursor:pointer">
        <input type="checkbox" id="mood-editor-adjustable" ${mood.userAdjustable ? 'checked' : ''} />
        Gebruiker mag deze mood aanpassen in de App
      </label>

      <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:8px">
        <div style="font-size:13px;font-weight:600;color:#1a1f2e">Acties</div>
        <div style="display:flex;gap:8px">
          <button id="mood-editor-import" style="padding:6px 12px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#4a5568;font-size:12px;cursor:pointer">📥 Importeer uit configbestand</button>
          <button id="mood-editor-add-action" style="padding:6px 12px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#4a5568;font-size:12px;cursor:pointer">+ Actie toevoegen</button>
        </div>
      </div>
      <div id="mood-editor-actions"></div>
    </div>
    <div style="display:flex;align-items:center;gap:8px;padding:16px 20px;border-top:1px solid #eef1f8;background:#f8f9fd">
      <button id="mood-editor-fetch" style="padding:8px 14px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#4a5568;font-size:13px;cursor:pointer" title="Huidige mood van de hardware lezen (overschrijft dit venster)">📥 Ophalen</button>
      <button id="mood-editor-send" style="padding:8px 14px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#4a5568;font-size:13px;cursor:pointer" title="Deze mood rechtstreeks naar de hardware sturen">📤 Versturen</button>
      <div style="flex:1"></div>
      <button id="mood-editor-cancel" style="padding:8px 20px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#6a7899;font-size:13px;cursor:pointer">Annuleer</button>
      <button id="mood-editor-save" style="padding:8px 20px;border:none;border-radius:6px;background:#ec4899;color:#fff;font-size:13px;font-weight:600;cursor:pointer" title="Opslaan in het projectbestand (.duo) — geen hardware nodig">Opslaan</button>
    </div>
  `;

  overlay.appendChild(dlg);
  document.body.appendChild(overlay);

  const actionsContainer = document.getElementById('mood-editor-actions');

  function renderActions() {
    actionsContainer.innerHTML = '';
    if (mood.actions.length === 0) {
      const empty = document.createElement('div');
      empty.style.cssText = 'font-size:12px;color:#a0aaba;padding:12px 0';
      empty.textContent = 'Nog geen acties. Voeg er een toe met de knop hierboven.';
      actionsContainer.appendChild(empty);
      return;
    }
    mood.actions.forEach((action, index) => actionsContainer.appendChild(buildActionRow(action, index)));
  }

  // Drag-and-drop reordering of actions — index of the row currently being dragged.
  let dragSrcIndex = null;

  function buildActionRow(action, index) {
    const target = findTarget(targets, action.channelRef);
    const decoded = decodeMsg(action.msg);
    const category = MOOD_CODE_TO_CATEGORY[decoded.code] || 'dimmer';

    const row = document.createElement('div');
    row.style.cssText = 'border:1px solid #eef1f8;border-radius:6px;padding:10px 12px;margin-bottom:8px;display:flex;flex-wrap:wrap;gap:8px;align-items:center';

    row.addEventListener('dragover', (e) => {
      if (dragSrcIndex === null) return;
      e.preventDefault();
      row.style.borderTop = dragSrcIndex > index ? '2px solid #ec4899' : '';
      row.style.borderBottom = dragSrcIndex < index ? '2px solid #ec4899' : '';
    });
    row.addEventListener('dragleave', () => { row.style.borderTop = ''; row.style.borderBottom = ''; });
    row.addEventListener('drop', (e) => {
      e.preventDefault();
      row.style.borderTop = ''; row.style.borderBottom = '';
      if (dragSrcIndex === null || dragSrcIndex === index) return;
      const [moved] = mood.actions.splice(dragSrcIndex, 1);
      mood.actions.splice(dragSrcIndex < index ? index - 1 : index, 0, moved);
      dragSrcIndex = null;
      renderActions();
    });

    // Drag handle — only this element starts the drag, so inputs/selects in the row stay usable.
    const handle = document.createElement('div');
    handle.textContent = '☰';
    handle.draggable = true;
    handle.title = 'Sleep om te herordenen';
    handle.style.cssText = 'cursor:grab;color:#a0aaba;font-size:13px;padding:2px 4px;user-select:none';
    handle.addEventListener('dragstart', (e) => {
      dragSrcIndex = index;
      e.dataTransfer.effectAllowed = 'move';
      e.dataTransfer.setData('text/plain', String(index));
    });
    handle.addEventListener('dragend', () => { dragSrcIndex = null; });
    row.appendChild(handle);

    // Target device select
    const targetSelect = document.createElement('select');
    targetSelect.style.cssText = 'flex:1;min-width:160px;padding:6px 8px;border:1px solid #dde3ef;border-radius:6px;font-size:12px';
    if (!target) {
      // Channel not (yet) placed as a room device — show it anyway so it isn't silently swapped.
      const opt = document.createElement('option');
      opt.value = `0x${action.channelRef.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}-0x${action.channelRef.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`;
      opt.textContent = `Onbekend toestel (0x${action.channelRef.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}; 0x${action.channelRef.unitAddress.toString(16).toUpperCase().padStart(2, '0')})`;
      opt.selected = true;
      targetSelect.appendChild(opt);
    }
    for (const t of targets) {
      const opt = document.createElement('option');
      opt.value = `0x${t.channelRef.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}-0x${t.channelRef.unitAddress.toString(16).toUpperCase().padStart(2, '0')}`;
      opt.textContent = t.label;
      opt.selected = target && t.channelRef.nodeAddress === target.channelRef.nodeAddress && t.channelRef.unitAddress === target.channelRef.unitAddress;
      targetSelect.appendChild(opt);
    }
    targetSelect.onchange = () => {
      const [nodeAddress, unitAddress] = targetSelect.value.split('-').map(Number);
      const newTarget = findTarget(targets, { nodeAddress, unitAddress });
      const code = CATEGORY_TO_MOOD_CODE[newTarget.category];
      const fresh = defaultAction(code);
      mood.actions[index] = { channelRef: newTarget.channelRef, msg: toMsg(fresh), delay: action.delay };
      renderActions();
    };
    row.appendChild(targetSelect);

    // Method select
    const methodSelect = document.createElement('select');
    methodSelect.style.cssText = 'padding:6px 8px;border:1px solid #dde3ef;border-radius:6px;font-size:12px';
    const methods = MOOD_METHODS[decoded.code] || [];
    for (const m of methods) {
      const opt = document.createElement('option');
      opt.value = String(m.value);
      opt.textContent = m.label;
      opt.selected = m.value === decoded.method;
      methodSelect.appendChild(opt);
    }
    methodSelect.onchange = () => {
      const patched = { code: decoded.code, method: Number(methodSelect.value), time: 0, value: 0 };
      action.msg = toMsg(patched);
      renderActions();
    };
    row.appendChild(methodSelect);

    // Conditional value/time input
    if (category === 'dimmer' && decoded.method === 3) {
      row.appendChild(buildNumberField('%', decoded.value, 0, 100, (v) => {
        action.msg = toMsg({ code: decoded.code, method: decoded.method, time: decoded.time, value: v });
      }));
    } else if (category === 'dimmer' && decoded.method === 0x13) {
      row.appendChild(buildNumberField('min', decoded.time, 1, 60, (v) => {
        action.msg = toMsg({ code: decoded.code, method: decoded.method, time: v, value: decoded.value });
      }));
    } else if (category === 'relay' && decoded.method === 2) {
      row.appendChild(buildNumberField('min', decoded.time, 1, 60, (v) => {
        action.msg = toMsg({ code: decoded.code, method: decoded.method, time: v, value: 0 });
      }));
    } else if (category === 'sensor' && decoded.method === 1) {
      row.appendChild(buildNumberField('°C', decoded.time / 10, 0, 40, (v) => {
        action.msg = toMsg({ code: decoded.code, method: decoded.method, time: Math.round(v * 10), value: 0 });
      }, 0.5));
    }

    // Delay
    const delayWrap = document.createElement('div');
    delayWrap.style.cssText = 'display:flex;align-items:center;gap:4px';
    delayWrap.innerHTML = '<span style="font-size:11px;color:#a0aaba">na</span>';
    const delayInput = document.createElement('input');
    delayInput.type = 'number';
    delayInput.min = '0';
    delayInput.step = '0.1';
    delayInput.value = String(action.delay || 0);
    delayInput.style.cssText = 'width:60px;padding:6px 8px;border:1px solid #dde3ef;border-radius:6px;font-size:12px';
    delayInput.onchange = () => { action.delay = Number(delayInput.value) || 0; };
    delayWrap.appendChild(delayInput);
    delayWrap.insertAdjacentHTML('beforeend', '<span style="font-size:11px;color:#a0aaba">sec</span>');
    row.appendChild(delayWrap);

    // Remove
    const removeBtn = document.createElement('button');
    removeBtn.textContent = '✕';
    removeBtn.title = 'Actie verwijderen';
    removeBtn.style.cssText = 'background:none;border:none;color:#dc3545;font-size:14px;cursor:pointer;padding:4px 6px';
    removeBtn.onclick = () => { mood.actions.splice(index, 1); renderActions(); };
    row.appendChild(removeBtn);

    return row;
  }

  function buildNumberField(unit, value, min, max, onChange, step = 1) {
    const wrap = document.createElement('div');
    wrap.style.cssText = 'display:flex;align-items:center;gap:4px';
    const input = document.createElement('input');
    input.type = 'number';
    input.min = String(min);
    input.max = String(max);
    input.step = String(step);
    input.value = String(value);
    input.style.cssText = 'width:70px;padding:6px 8px;border:1px solid #dde3ef;border-radius:6px;font-size:12px';
    input.onchange = () => onChange(Number(input.value) || 0);
    wrap.appendChild(input);
    wrap.insertAdjacentHTML('beforeend', `<span style="font-size:11px;color:#a0aaba">${unit}</span>`);
    return wrap;
  }

  document.getElementById('mood-editor-add-action').onclick = () => {
    if (targets.length === 0) return;
    const t = targets[0];
    const code = CATEGORY_TO_MOOD_CODE[t.category];
    mood.actions.push({ channelRef: t.channelRef, msg: toMsg(defaultAction(code)), delay: 0 });
    renderActions();
  };

  document.getElementById('mood-editor-import').onclick = async () => {
    const projectName = state.get().project?.meta?.name;
    if (!projectName) {
      showToast('Sla het project eerst op', 'error');
      return;
    }
    try {
      const { files } = await fetch(`/api/project/data/moods?projectName=${encodeURIComponent(projectName)}`).then(r => r.json());
      const hex = channelRef.nodeAddress.toString(16).padStart(2, '0');
      const path = (files || []).find(f => f.toLowerCase().endsWith(`moodconfig_${hex}.json`.toLowerCase()));
      if (!path) {
        showToast(`Geen MoodConfig gevonden voor dit toestel — upload eerst de config via "Opladen config..."`, 'error');
        return;
      }
      const { content } = await fetch(`/api/project/data/file?projectName=${encodeURIComponent(projectName)}&path=${encodeURIComponent(path)}`).then(r => r.json());
      const entries = JSON.parse(content);
      const entry = entries.find(e => e.id === channelRef.unitAddress);
      if (!entry) {
        showToast('Geen acties gevonden voor deze mood in het configbestand', 'error');
        return;
      }
      mood.actions = entry.data.map(d => ({ channelRef: { nodeAddress: d.nodeAddress, unitAddress: d.unitAddress }, msg: d.msg, delay: d.delay || 0 }));
      if (entry.userAdjustable !== undefined) document.getElementById('mood-editor-adjustable').checked = entry.userAdjustable;
      renderActions();
      showToast(`${mood.actions.length} acties geïmporteerd`, 'success');
    } catch (e) {
      showToast('Importeren mislukt: ' + e.message, 'error');
    }
  };

  // ── Live hardware read/write (moods HTTP API — separate from "Opslaan", which
  // only ever touches the local project file) ──────────────────────────────────
  const fetchBtn = document.getElementById('mood-editor-fetch');
  const sendBtn = document.getElementById('mood-editor-send');
  const apiConfig = moodsApiConfig();
  if (!apiConfig) {
    const reason = 'Verbind eerst met de master (met API poort) om moods rechtstreeks te lezen/versturen';
    fetchBtn.disabled = sendBtn.disabled = true;
    fetchBtn.style.opacity = sendBtn.style.opacity = '.4';
    fetchBtn.title = sendBtn.title = reason;
  }

  fetchBtn.onclick = async () => {
    try {
      const res = await fetch(`/api/moods-http/${channelRef.unitAddress}/fetch`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(apiConfig),
      });
      const data = await res.json();
      if (!res.ok) throw new Error(data.error || 'onbekende fout');
      document.getElementById('mood-editor-name').value = data.name ?? mood.name;
      document.getElementById('mood-editor-desc').value = data.description ?? '';
      document.getElementById('mood-editor-adjustable').checked = !!data.userAdjustable;
      mood.actions = (data.data || []).map(d => ({ channelRef: { nodeAddress: d.nodeAddress, unitAddress: d.unitAddress }, msg: d.msg, delay: d.delay || 0 }));
      renderActions();
      showToast(`Mood opgehaald van hardware (${mood.actions.length} acties)`, 'success');
    } catch (e) {
      showToast('Ophalen van hardware mislukt: ' + e.message, 'error');
    }
  };

  sendBtn.onclick = async () => {
    const payload = {
      id: channelRef.unitAddress,
      name: document.getElementById('mood-editor-name').value.trim() || mood.name,
      description: document.getElementById('mood-editor-desc').value.trim(),
      userAdjustable: document.getElementById('mood-editor-adjustable').checked,
      data: mood.actions.map(a => ({ nodeAddress: a.channelRef.nodeAddress, unitAddress: a.channelRef.unitAddress, msg: a.msg, delay: a.delay || 0 })),
    };
    try {
      const res = await fetch(`/api/moods-http/${channelRef.unitAddress}/send`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ...apiConfig, mood: payload }),
      });
      const data = await res.json();
      if (!res.ok) throw new Error(data.error || 'onbekende fout');
      showToast('Mood verstuurd naar hardware', 'success');
    } catch (e) {
      showToast('Versturen naar hardware mislukt: ' + e.message, 'error');
    }
  };

  renderActions();

  const close = () => overlay.remove();
  document.getElementById('mood-editor-close').onclick = close;
  document.getElementById('mood-editor-cancel').onclick = close;
  overlay.addEventListener('click', (e) => { if (e.target === overlay) close(); });

  document.getElementById('mood-editor-save').onclick = () => {
    dispatch({
      type: 'UPDATE_MOOD',
      channelRef,
      patch: {
        name: document.getElementById('mood-editor-name').value.trim() || moodDevice.name,
        description: document.getElementById('mood-editor-desc').value.trim(),
        userAdjustable: document.getElementById('mood-editor-adjustable').checked,
        actions: mood.actions,
      },
    });
    close();
  };
}

function escapeAttr(s) {
  return String(s).replace(/&/g, '&amp;').replace(/"/g, '&quot;').replace(/</g, '&lt;');
}
