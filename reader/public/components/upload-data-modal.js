/**
 * Upload Data Modal
 *
 * Lets the user pick the legacy install's data folder (bind*.txt, MoodConfig,
 * etc.) and copy it into this project's companion `<name>.data/` folder on
 * the server, so it can be (re)used later for importing bindings/moods or
 * uploading to hardware — without re-browsing the folder every time.
 */

import { state } from '../app/state.js';
import { showToast } from '../app/main.js';

export function openUploadDataModal() {
  const existing = document.getElementById('upload-data-overlay');
  if (existing) { existing.remove(); return; }

  const projectName = state.get().project?.meta?.name;
  if (!projectName) {
    showToast('Sla het project eerst op voordat je bestanden uploadt', 'error');
    return;
  }

  const overlay = document.createElement('div');
  overlay.id = 'upload-data-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:3000;display:flex;align-items:center;justify-content:center';

  const dlg = document.createElement('div');
  dlg.style.cssText = 'background:#fff;border-radius:10px;width:600px;max-width:95vw;max-height:80vh;box-shadow:0 20px 60px rgba(0,0,0,.3);overflow:hidden;display:flex;flex-direction:column';

  dlg.innerHTML = `
    <div style="display:flex;align-items:center;padding:16px 20px;border-bottom:1px solid #eef1f8;background:#f8f9fd;font-size:15px;font-weight:600;color:#1a1f2e">
      <span>📤 Opladen config...</span>
      <div style="flex:1"></div>
      <button id="upload-data-close" style="background:none;border:none;font-size:20px;color:#6a7899;cursor:pointer;line-height:1">×</button>
    </div>
    <div style="padding:24px;flex:1;overflow-y:auto">
      <p style="font-size:13px;color:#6a7899;line-height:1.6;margin-bottom:16px">
        Selecteer de <strong>Config folder</strong> van de oude software (bind*.txt, MoodConfig, ...).
        De bestanden worden gekopieerd naar <code>${projectName}.data/</code> op de server, zodat je
        ze later opnieuw kan gebruiken om te importeren of naar hardware te sturen.
      </p>
      <div style="display:flex;gap:8px;margin-bottom:16px">
        <input type="text" id="upload-data-path" readonly placeholder="Geen folder geselecteerd..."
          style="flex:1;padding:8px 12px;border:1px solid #dde3ef;border-radius:6px;font-size:13px;background:#f9fafb;color:#4a5568" />
        <button id="upload-data-select" style="padding:8px 16px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#4a5568;font-size:13px;cursor:pointer;font-weight:500">
          Selecteer folder
        </button>
      </div>
      <input type="file" id="upload-data-input" webkitdirectory directory multiple style="display:none" />
      <div id="upload-data-status" style="display:none;padding:12px;border-radius:6px;background:#eef1f8;font-size:12px;color:#6a7899;line-height:1.5"></div>
    </div>
    <div style="display:flex;align-items:center;gap:8px;padding:16px 20px;border-top:1px solid #eef1f8;background:#f8f9fd">
      <div style="flex:1;font-size:12px;color:#a0aaba" id="upload-data-info">Selecteer een folder om te starten</div>
      <button id="upload-data-cancel" style="padding:8px 20px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#6a7899;font-size:13px;cursor:pointer">Annuleer</button>
      <button id="upload-data-submit" disabled style="padding:8px 20px;border:none;border-radius:6px;background:#e08c00;color:#fff;font-size:13px;font-weight:600;cursor:not-allowed;opacity:0.5">Upload</button>
    </div>
  `;

  overlay.appendChild(dlg);
  document.body.appendChild(overlay);

  let selectedFiles = [];

  const closeBtn = document.getElementById('upload-data-close');
  const cancelBtn = document.getElementById('upload-data-cancel');
  const selectBtn = document.getElementById('upload-data-select');
  const folderInput = document.getElementById('upload-data-input');
  const submitBtn = document.getElementById('upload-data-submit');
  const pathInput = document.getElementById('upload-data-path');
  const infoText = document.getElementById('upload-data-info');
  const statusDiv = document.getElementById('upload-data-status');

  closeBtn?.addEventListener('click', () => overlay.remove());
  cancelBtn?.addEventListener('click', () => overlay.remove());
  overlay.addEventListener('click', (e) => { if (e.target === overlay) overlay.remove(); });
  selectBtn?.addEventListener('click', () => folderInput?.click());

  folderInput?.addEventListener('change', (e) => {
    const files = Array.from(e.target.files);
    if (files.length === 0) return;

    selectedFiles = files;
    const folderName = files[0].webkitRelativePath.split('/')[0] || 'Config';
    pathInput.value = `${folderName} (${files.length} bestanden)`;
    infoText.textContent = `${files.length} bestanden gevonden`;
    submitBtn.disabled = false;
    submitBtn.style.cursor = 'pointer';
    submitBtn.style.opacity = '1';
  });

  submitBtn?.addEventListener('click', async () => {
    if (selectedFiles.length === 0) return;

    submitBtn.disabled = true;
    submitBtn.textContent = 'Uploaden...';
    statusDiv.style.display = 'block';
    statusDiv.textContent = `Bestanden lezen (0/${selectedFiles.length})...`;

    try {
      const files = [];
      for (let i = 0; i < selectedFiles.length; i++) {
        const file = selectedFiles[i];
        const relPath = file.webkitRelativePath.split('/').slice(1).join('/') || file.name;
        files.push({ path: relPath, contentBase64: await readFileAsBase64(file) });
        statusDiv.textContent = `Bestanden lezen (${i + 1}/${selectedFiles.length})...`;
      }

      statusDiv.textContent = 'Uploaden naar server...';

      const res = await fetch('/api/project/data/upload', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ projectName, files })
      });
      const result = await res.json();

      if (!res.ok) throw new Error(result.error || 'Upload mislukt');

      statusDiv.innerHTML = `✅ ${result.written} bestanden opgeslagen in <code>${result.folder}</code>`;
      statusDiv.style.background = '#e8f5e9';
      statusDiv.style.color = '#2e7d32';
      showToast(`${result.written} bestanden geüpload`, 'success');

      submitBtn.textContent = 'Sluiten';
      submitBtn.disabled = false;
      submitBtn.onclick = () => overlay.remove();
    } catch (err) {
      console.error('[upload-data] Error:', err);
      showToast(`Upload mislukt: ${err.message}`, 'error');
      statusDiv.textContent = `❌ ${err.message}`;
      submitBtn.disabled = false;
      submitBtn.textContent = 'Upload';
    }
  });
}

function readFileAsBase64(file) {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = () => resolve(reader.result.split(',')[1] || '');
    reader.onerror = () => reject(new Error(`Failed to read ${file.name}`));
    reader.readAsDataURL(file);
  });
}
