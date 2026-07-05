/**
 * App entry point — initialises state, loads module DB, boots the UI.
 */

import { state, dispatch } from './state.js';
import { initRouter, switchView } from './router.js';

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

  document.getElementById('btn-save')?.addEventListener('click', () => saveProject());
  document.getElementById('btn-upload')?.addEventListener('click', () => uploadToHardware());

  // Subscribe to state changes → update header
  state.subscribe(updateHeaderUI);

  // Boot router (activates initial tab)
  initRouter();
}

// ─── Save / Upload ────────────────────────────────────────────────────────────

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
