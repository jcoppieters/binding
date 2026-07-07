/**
 * App entry point — initialises state, loads module DB, boots the UI.
 */

import { state, dispatch } from './state.js';
import { initRouter, switchView } from './router.js';
import { wireButtons as wireRailButtons } from '../components/rail-view.js';

// ─── Bootstrap ────────────────────────────────────────────────────────────────

async function loadModules() {
  try {
    const res = await fetch('/api/modules');
    if (!res.ok) throw new Error(`HTTP ${res.status}`);
    const modules = await res.json();
    dispatch({ type: 'SET_MODULES', modules });
    console.log(`[app] Module DB loaded: ${modules.length} entries`);
  } catch (e) {
    console.warn('[app] Could not load module DB:', e.message);
  }
}

function updateHeaderUI(s) {
  // Project name
  const nameEl = document.getElementById('project-name');
  if (nameEl) nameEl.textContent = s.project.meta.name;

  // Save indicator
  const dotEl = document.getElementById('save-dot');
  if (dotEl) dotEl.style.background = s.dirty ? '#f0a020' : '#4cd04c';

  // Connection status
  const connEl = document.getElementById('connection-status');
  if (connEl) {
    connEl.textContent = s.connected ? `● ${s.masterIp}` : '● Niet verbonden';
    connEl.className = 'connection-status ' + (s.connected ? 'connected' : 'disconnected');
  }
}

async function init() {
  // Load module DB in parallel with DOM ready
  await loadModules();

  // Wire header interactions
  document.getElementById('project-name')?.addEventListener('dblclick', () => {
    const name = prompt('Project naam:', state.get().project.meta.name);
    if (name) dispatch({ type: 'SET_PROJECT_NAME', name });
  });

  document.getElementById('btn-open')?.addEventListener('click', () => openProject());
  document.getElementById('btn-save')?.addEventListener('click', () => saveProject());
  document.getElementById('btn-upload')?.addEventListener('click', () => uploadToHardware());

  // Subscribe to state changes → update header
  state.subscribe(updateHeaderUI);

  // Boot router (activates initial tab)
  initRouter();

  // Wire Rail View action buttons
  wireRailButtons();
}

// ─── Save / Upload ────────────────────────────────────────────────────────────

async function openProject() {
  try {
    const res = await fetch('/api/project/list');
    const files = await res.json();

    // Build a simple modal listing available projects
    const existing = document.getElementById('open-project-overlay');
    if (existing) { existing.remove(); return; }

    const overlay = document.createElement('div');
    overlay.id = 'open-project-overlay';
    overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:3000;display:flex;align-items:center;justify-content:center';

    const dlg = document.createElement('div');
    dlg.style.cssText = 'background:#fff;border-radius:10px;width:380px;max-width:95vw;box-shadow:0 20px 60px rgba(0,0,0,.3);overflow:hidden';

    const hdr = document.createElement('div');
    hdr.style.cssText = 'display:flex;align-items:center;padding:14px 18px;border-bottom:1px solid #eef1f8;background:#f8f9fd;font-size:14px;font-weight:600;color:#1a1f2e';
    hdr.innerHTML = '<span style="flex:1">📂 Project openen</span>';
    const closeBtn = document.createElement('button');
    closeBtn.textContent = '✕';
    closeBtn.style.cssText = 'background:none;border:none;font-size:18px;color:#6a7899;cursor:pointer;padding:2px 6px';
    closeBtn.onclick = () => overlay.remove();
    hdr.append(closeBtn);

    const body = document.createElement('div');
    body.style.cssText = 'padding:16px;max-height:300px;overflow-y:auto';

    if (!files.length) {
      body.innerHTML = '<p style="color:#a0aaba;font-size:13px;text-align:center;padding:20px 0">Geen opgeslagen projecten gevonden.<br><small>Sla eerst een project op via 💾 Opslaan.</small></p>';
    } else {
      files.forEach(filename => {
        const row = document.createElement('button');
        row.style.cssText = 'display:block;width:100%;text-align:left;padding:10px 12px;border:1px solid #dde3ef;border-radius:6px;margin-bottom:6px;background:#f8f9fd;cursor:pointer;font-size:13px;color:#1a1f2e;transition:background .12s';
        row.onmouseenter = () => row.style.background = '#eef1f8';
        row.onmouseleave = () => row.style.background = '#f8f9fd';
        row.textContent = '📄 ' + filename.replace('.duo', '');
        row.onclick = async () => {
          try {
            const loadRes = await fetch(`/api/project/load?file=${encodeURIComponent(filename)}`);
            if (!loadRes.ok) throw new Error('Laden mislukt');
            const project = await loadRes.json();
            dispatch({ type: 'SET_PROJECT', project });
            overlay.remove();
            showToast(`Project "${project.meta.name}" geladen`, 'success');
          } catch {
            showToast('Laden mislukt', 'error');
          }
        };
        body.append(row);
      });
    }

    dlg.append(hdr, body);
    overlay.append(dlg);
    overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
    document.body.append(overlay);
  } catch {
    showToast('Kan projectlijst niet laden', 'error');
  }
}

async function saveProject() {
  const s = state.get();
  try {
    const res = await fetch('/api/project/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(s.project),
    });
    if (res.ok) {
      dispatch({ type: 'SET_DIRTY', dirty: false });
      showToast('Project opgeslagen', 'success');
    } else {
      showToast('Opslaan mislukt', 'error');
    }
  } catch {
    showToast('Opslaan mislukt (geen verbinding)', 'error');
  }
}

async function uploadToHardware() {
  showToast('Uploaden naar hardware…', '');
  try {
    const res = await fetch('/api/upload/changed', { method: 'POST' });
    const data = await res.json();
    if (data.error) { showToast(data.error, 'error'); return; }
    const { uploaded = 0, skipped = 0, failed = 0 } = data;
    showToast(`Upload klaar: ${uploaded} nodes, ${skipped} ongewijzigd, ${failed} mislukt`, failed ? 'error' : 'success');
  } catch {
    showToast('Upload mislukt (geen verbinding)', 'error');
  }
}

// ─── Toast ────────────────────────────────────────────────────────────────────

export function showToast(msg, type = '') {
  const el = document.getElementById('toast');
  if (!el) return;
  el.textContent = msg;
  el.className = 'toast show' + (type ? ` toast-${type}` : '');
  clearTimeout(el._timeout);
  el._timeout = setTimeout(() => el.classList.remove('show'), 3000);
}

// ─── Start ────────────────────────────────────────────────────────────────────

document.addEventListener('DOMContentLoaded', init);
