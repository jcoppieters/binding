/**
 * App entry point — initialises state, loads module DB, boots the UI.
 */

import { state, dispatch } from './state.js';
import { initRouter, switchView } from './router.js';
import { wireButtons as wireRailButtons } from '../components/rail-view.js';
import { wireButtons as wireHomeButtons } from '../components/home-view.js';

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
    const ip = s.masterIp ?? s.project.meta.masterIp ?? null;
    connEl.textContent = s.connected ? `● ${ip}` : (ip ? `○ ${ip}` : '○ Niet verbonden');
    connEl.className = 'connection-status ' + (s.connected ? 'connected' : 'disconnected');
    connEl.title = s.connected ? 'Verbonden — klik om te beheren' : 'Niet verbonden — klik om te verbinden';
  }
}

async function init() {
  // Load module DB in parallel with DOM ready
  await loadModules();

  // Wire header interactions
  document.getElementById('connection-status')?.addEventListener('click', () => openConnectModal());
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

  // Wire Home View action buttons
  wireHomeButtons();
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

// ─── Connect modal ────────────────────────────────────────────────────────────

async function openConnectModal() {
  document.getElementById('connect-overlay')?.remove();
  const s = state.get();
  const savedIp = s.project.meta.masterIp ?? '';
  const savedPw = s.project.meta.masterPassword ?? '';
  const savedPort = s.project.meta.masterPort ?? 5001;

  const overlay = document.createElement('div');
  overlay.id = 'connect-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:3000;display:flex;align-items:center;justify-content:center';

  const dlg = document.createElement('div');
  dlg.style.cssText = 'background:#fff;border-radius:10px;width:380px;max-width:95vw;box-shadow:0 20px 60px rgba(0,0,0,.3);overflow:hidden';

  const hdr = document.createElement('div');
  hdr.style.cssText = 'display:flex;align-items:center;padding:14px 18px;border-bottom:1px solid #eef1f8;background:#1a2540;font-size:14px;font-weight:600;color:#fff';
  hdr.innerHTML = '<span style="flex:1">🔌 Verbinden met master</span>';
  const closeBtn = document.createElement('button');
  closeBtn.textContent = '✕';
  closeBtn.style.cssText = 'background:none;border:none;font-size:18px;color:#8090b0;cursor:pointer;padding:2px 6px';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = document.createElement('div');
  body.style.cssText = 'padding:20px;display:flex;flex-direction:column;gap:12px';

  const note = document.createElement('p');
  note.style.cssText = 'font-size:12px;color:#6a7899;margin:0';
  note.textContent = 'Verbind met de Duotecno master via TCP (poort 5001). Na verbinding worden alle nodes ontdekt en vergeleken met het project.';
  body.append(note);

  const ipLabel = document.createElement('label');
  ipLabel.style.cssText = 'font-size:11px;font-weight:600;color:#6a7899;display:block';
  ipLabel.textContent = 'IP adres';
  const ipInput = document.createElement('input');
  ipInput.type = 'text';
  ipInput.placeholder = '192.168.0.97';
  ipInput.value = savedIp;
  ipInput.style.cssText = 'width:100%;box-sizing:border-box;padding:7px 10px;border:1px solid #dde3ef;border-radius:6px;font-size:14px;margin-top:3px';

  const pwLabel = document.createElement('label');
  pwLabel.style.cssText = 'font-size:11px;font-weight:600;color:#6a7899;display:block';
  pwLabel.textContent = 'Wachtwoord';
  const pwInput = document.createElement('input');
  pwInput.type = 'password';
  pwInput.placeholder = 'wachtwoord';
  pwInput.value = savedPw;
  pwInput.style.cssText = 'width:100%;box-sizing:border-box;padding:7px 10px;border:1px solid #dde3ef;border-radius:6px;font-size:14px;margin-top:3px';

  const portLabel = document.createElement('label');
  portLabel.style.cssText = 'font-size:11px;font-weight:600;color:#6a7899;display:block';
  portLabel.textContent = 'Poort';
  const portInput = document.createElement('input');
  portInput.type = 'number';
  portInput.placeholder = '5001';
  portInput.value = savedPort;
  portInput.style.cssText = 'width:100%;box-sizing:border-box;padding:7px 10px;border:1px solid #dde3ef;border-radius:6px;font-size:14px;margin-top:3px';

  body.append(ipLabel, ipInput, pwLabel, pwInput, portLabel, portInput);

  // Status area
  const statusDiv = document.createElement('div');
  statusDiv.style.cssText = 'font-size:12px;color:#6a7899;min-height:18px;margin-top:4px';
  body.append(statusDiv);

  const footer = document.createElement('div');
  footer.style.cssText = 'display:flex;justify-content:flex-end;gap:8px;padding:12px 18px;border-top:1px solid #eef1f8;background:#f8f9fd';

  const disconnectBtn = document.createElement('button');
  disconnectBtn.textContent = 'Verbreken';
  disconnectBtn.style.cssText = 'padding:7px 14px;border-radius:6px;border:1px solid #fca5a5;color:#ef4444;background:#fff;cursor:pointer;font-size:13px';
  disconnectBtn.disabled = !s.connected;
  disconnectBtn.style.opacity = s.connected ? '1' : '.4';
  disconnectBtn.onclick = async () => {
    await fetch('/api/master/disconnect', { method: 'POST' });
    dispatch({ type: 'SET_CONNECTION', connected: false, ip: null, nodes: [] });
    overlay.remove();
    showToast('Verbinding verbroken', '');
  };

  const connectBtn = document.createElement('button');
  connectBtn.textContent = 'Verbinden';
  connectBtn.title = 'Snel verbinden: haalt node-adressen op (geen volledige unit-scan)';
  connectBtn.style.cssText = 'padding:7px 14px;border-radius:6px;border:1px solid #dde3ef;background:#fff;color:#4a5568;cursor:pointer;font-size:13px';

  const fullScanBtn = document.createElement('button');
  fullScanBtn.textContent = '🔍 Scan alles';
  fullScanBtn.title = 'Verbinden + volledige scan van alle nodes en units (voor automatische type-herkenning)';
  fullScanBtn.style.cssText = 'padding:7px 16px;border-radius:6px;border:none;background:#e08c00;color:#fff;cursor:pointer;font-size:13px;font-weight:600';

  // Shared connect logic
  async function doConnect(fullScan) {
    const ip = ipInput.value.trim();
    const pw = pwInput.value;
    const port = parseInt(portInput.value) || 5001;
    if (!ip) { ipInput.style.borderColor = '#ef4444'; return; }

    connectBtn.disabled = true;
    fullScanBtn.disabled = true;
    statusDiv.style.color = '#6a7899';
    statusDiv.textContent = 'TCP verbinding opzetten…';

    try {
      dispatch({ type: 'SET_MASTER_CONFIG', masterIp: ip, masterPassword: pw, masterPort: port });

      const res = await fetch('/api/master/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ host: ip, port: port, password: pw, fullScan }),
      });
      const data = await res.json();
      if (!res.ok || !data.success) {
        statusDiv.style.color = '#ef4444';
        statusDiv.textContent = `Verbinding mislukt: ${data.message ?? data.error ?? 'onbekende fout'}`;
        connectBtn.disabled = false; fullScanBtn.disabled = false;
        return;
      }

      // Poll: quick = 3 polls (4.5s), full = 20 polls (30s)
      const maxPolls = fullScan ? 20 : 3;
      statusDiv.textContent = fullScan ? 'Nodes + units ontdekken…' : 'Nodes zoeken…';
      let nodes = [];
      for (let i = 0; i < maxPolls; i++) {
        await new Promise(r => setTimeout(r, 1500));
        const statusRes = await fetch('/api/master/status');
        const statusData = await statusRes.json();
        const nodeCount = statusData.nodes ?? 0;
        statusDiv.textContent = fullScan
          ? `Ontdekken… (${nodeCount} nodes)`
          : `Verbonden — ${nodeCount} nodes gevonden`;
        if (statusData.status === 'ready' || (!fullScan && nodeCount > 0)) {
          const nodesRes = await fetch('/api/master/nodes');
          nodes = await nodesRes.json();
          break;
        }
        if (statusData.status === 'error') break;
      }
      if (!nodes.length) {
        // Last attempt
        const nodesRes = await fetch('/api/master/nodes');
        nodes = await nodesRes.json();
      }

      dispatch({ type: 'SET_CONNECTION', connected: true, ip, nodes });

      // Auto-add unmatched discovered nodes to project
      const projectModules = state.get().project.railView.cabinets.flatMap(c => c.modules).filter(m => m.nodeAddress != null);
      const projectWoning = state.get().project.railView.woningDevices.filter(d => d.nodeAddress != null);
      const allProjectNodes = new Set([...projectModules, ...projectWoning].map(m => m.nodeAddress));
      const discoveredAddrs = new Set(nodes.map(n => n.nodeAddress));
      const matched = [...projectModules, ...projectWoning].filter(m => discoveredAddrs.has(m.nodeAddress)).length;
      const unmatched = [...projectModules, ...projectWoning].filter(m => !discoveredAddrs.has(m.nodeAddress)).length;
      const extraNodes = nodes.filter(n => !allProjectNodes.has(n.nodeAddress));

      if (extraNodes.length > 0) {
        dispatch({ type: 'ADD_DISCOVERED_NODES', nodes: extraNodes });
      }

      overlay.remove();
      showToast(
        `Verbonden met ${ip}${fullScan ? ' (volledig)' : ' (snel)'} — ${nodes.length} nodes: ${matched} gekoppeld${unmatched ? `, ${unmatched} niet gevonden` : ''}${extraNodes.length ? `, ${extraNodes.length} nieuw` : ''}`,
        'success'
      );
    } catch (err) {
      statusDiv.style.color = '#ef4444';
      statusDiv.textContent = `Fout: ${err.message}`;
      connectBtn.disabled = false; fullScanBtn.disabled = false;
    }
  }

  connectBtn.onclick = () => doConnect(false);
  fullScanBtn.onclick = () => doConnect(true);

  footer.append(disconnectBtn, connectBtn, fullScanBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', e => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
  ipInput.focus();
  if (!savedIp) ipInput.select();
}

async function saveProject() {
  const s = state.get();
  // Include discovered nodes in the project file
  const projectToSave = {
    ...s.project,
    discoveredNodes: s.discoveredNodes,
  };
  try {
    const res = await fetch('/api/project/save', {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify(projectToSave),
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

if (document.readyState === 'loading') {
  document.addEventListener('DOMContentLoaded', init);
} else {
  // DOM is already loaded
  init();
}
