/**
 * Import Bindings Modal
 * 
 * Imports bind*.txt binding files already uploaded to the project's data
 * folder (via "Opladen config...") and converts them into visual bindings.
 */

import { state, dispatch } from '../app/state.js';
import { showToast } from '../app/main.js';
import { openUploadDataModal } from './upload-data-modal.js';

export function openImportBindingsModal() {
  // Check if modal already exists
  const existing = document.getElementById('import-bindings-overlay');
  if (existing) { existing.remove(); return; }

  const overlay = document.createElement('div');
  overlay.id = 'import-bindings-overlay';
  overlay.style.cssText = 'position:fixed;inset:0;background:rgba(0,0,0,.5);z-index:3000;display:flex;align-items:center;justify-content:center';

  const dlg = document.createElement('div');
  dlg.style.cssText = 'background:#fff;border-radius:10px;width:600px;max-width:95vw;max-height:80vh;box-shadow:0 20px 60px rgba(0,0,0,.3);overflow:hidden;display:flex;flex-direction:column';

  // Header
  const hdr = document.createElement('div');
  hdr.style.cssText = 'display:flex;align-items:center;padding:16px 20px;border-bottom:1px solid #eef1f8;background:#f8f9fd;font-size:15px;font-weight:600;color:#1a1f2e';
  hdr.innerHTML = `
    <span>📥 Importeer bindings</span>
    <div style="flex:1"></div>
    <button id="import-close" style="background:none;border:none;font-size:20px;color:#6a7899;cursor:pointer;line-height:1">×</button>
  `;
  dlg.appendChild(hdr);

  // Body
  const body = document.createElement('div');
  body.style.cssText = 'padding:24px;flex:1;overflow-y:auto';
  body.innerHTML = `
    <div style="margin-bottom:20px">
      <p style="font-size:13px;color:#6a7899;line-height:1.6;margin-bottom:12px">
        Importeert bind*.txt bestanden die eerder via <strong>"Opladen config..."</strong> naar dit project zijn geüpload.
      </p>
      <p style="font-size:12px;color:#a0aaba;line-height:1.5">
        <strong>Let op:</strong> Alleen eenvoudige bindings (Type I/N) worden gevisualiseerd. 
        Complexe bindings (logica/timers) worden als "legacy" opgeslagen.
      </p>
    </div>

    <div id="import-empty-state" style="display:none;margin-bottom:20px;padding:16px;border:1px solid #dde3ef;border-radius:6px;background:#f9fafb;text-align:center">
      <div id="import-empty-message" style="font-size:13px;color:#6a7899;margin-bottom:12px"></div>
      <button id="import-open-upload" style="padding:8px 16px;border:none;border-radius:6px;background:#3b82f6;color:#fff;font-size:13px;cursor:pointer;font-weight:500">
        📤 Opladen config...
      </button>
    </div>

    <div id="import-files-list" style="display:none;margin-bottom:20px">
      <div style="font-size:12px;font-weight:600;color:#4a5568;margin-bottom:8px">
        Binding-bestanden in dit project (<span id="import-file-count">0</span>)
      </div>
      <div 
        id="import-files-container" 
        style="max-height:200px;overflow-y:auto;border:1px solid #eef1f8;border-radius:6px;background:#f9fafb;padding:8px"
      ></div>
    </div>

    <div id="import-status" style="display:none;padding:12px;border-radius:6px;background:#eef1f8;font-size:12px;color:#6a7899;line-height:1.5"></div>
  `;
  dlg.appendChild(body);

  // Footer
  const footer = document.createElement('div');
  footer.style.cssText = 'display:flex;align-items:center;gap:8px;padding:16px 20px;border-top:1px solid #eef1f8;background:#f8f9fd';
  footer.innerHTML = `
    <div style="flex:1;font-size:12px;color:#a0aaba" id="import-info">
      Bestanden laden...
    </div>
    <button id="import-cancel" style="padding:8px 20px;border:1px solid #dde3ef;border-radius:6px;background:#fff;color:#6a7899;font-size:13px;cursor:pointer">
      Annuleer
    </button>
    <button id="import-submit" disabled style="padding:8px 20px;border:none;border-radius:6px;background:#e08c00;color:#fff;font-size:13px;font-weight:600;cursor:not-allowed;opacity:0.5">
      Importeer bindings
    </button>
  `;
  dlg.appendChild(footer);

  overlay.appendChild(dlg);
  document.body.appendChild(overlay);

  // State
  let selectedFiles = [];

  // Wire up events
  const closeBtn = document.getElementById('import-close');
  const cancelBtn = document.getElementById('import-cancel');
  const submitBtn = document.getElementById('import-submit');

  closeBtn?.addEventListener('click', () => overlay.remove());
  cancelBtn?.addEventListener('click', () => overlay.remove());
  overlay.addEventListener('click', (e) => {
    if (e.target === overlay) overlay.remove();
  });

  document.getElementById('import-open-upload')?.addEventListener('click', () => {
    overlay.remove();
    openUploadDataModal();
  });

  function showEmptyState(message) {
    document.getElementById('import-empty-message').textContent = message;
    document.getElementById('import-empty-state').style.display = 'block';
    const infoText = document.getElementById('import-info');
    if (infoText) infoText.textContent = 'Geen bestanden om te importeren';
  }

  // Updates the file list UI + enables submit once binding files are loaded
  // from the project's data folder.
  function applySelectedFiles(bindFiles) {
    selectedFiles = bindFiles;

    const filesList = document.getElementById('import-files-list');
    const filesContainer = document.getElementById('import-files-container');
    const fileCount = document.getElementById('import-file-count');
    const infoText = document.getElementById('import-info');

    if (fileCount) fileCount.textContent = bindFiles.length;
    if (filesList) filesList.style.display = 'block';
    if (infoText) infoText.textContent = `${bindFiles.length} bind*.txt bestanden gevonden`;

    if (filesContainer) {
      filesContainer.innerHTML = bindFiles
        .slice().sort((a, b) => a.name.localeCompare(b.name))
        .map(f => {
          const nodeAddr = f.name.match(/bind([0-9a-fA-F]{2})\.txt/i)?.[1]?.toUpperCase() || '??';
          return `
            <div style="padding:6px 8px;font-size:12px;color:#4a5568;display:flex;align-items:center;gap:8px">
              <span style="font-family:monospace;color:#6a7899">0x${nodeAddr}</span>
              <span>${f.name}</span>
              <span style="margin-left:auto;font-size:11px;color:#a0aaba">${(f.size / 1024).toFixed(1)} KB</span>
            </div>
          `;
        })
        .join('');
    }

    if (submitBtn) {
      submitBtn.disabled = false;
      submitBtn.style.cursor = 'pointer';
      submitBtn.style.opacity = '1';
    }
  }

  // Load binding files already stored on the server for this project (uploaded
  // earlier via "Opladen config...") — this is now the only source for import.
  const projectName = state.get().project?.meta?.name;
  if (!projectName) {
    showEmptyState('Sla het project eerst op, upload dan de config via "Opladen config..." om bindings te kunnen importeren.');
  } else {
    fetch(`/api/project/data/list?projectName=${encodeURIComponent(projectName)}`)
      .then(res => res.json())
      .then(async ({ files }) => {
        if (!files || files.length === 0) {
          showEmptyState('Dit project heeft nog geen binding-bestanden. Upload eerst de config.');
          return;
        }
        const loaded = await Promise.all(files.map(async (path) => {
          const res = await fetch(`/api/project/data/file?projectName=${encodeURIComponent(projectName)}&path=${encodeURIComponent(path)}`);
          const { content } = await res.json();
          return { name: path.split('/').pop(), size: content.length, _preloadedContent: content };
        }));
        applySelectedFiles(loaded);
      })
      .catch(() => showEmptyState('Dit project heeft nog geen binding-bestanden. Upload eerst de config.'));
  }

  // Submit import
  submitBtn?.addEventListener('click', async () => {
    if (selectedFiles.length === 0) return;

    try {
      submitBtn.disabled = true;
      submitBtn.textContent = 'Importeren...';

      const filesToImport = selectedFiles.map((file) => ({
        fileName: file.name,
        content: file._preloadedContent
      }));

      console.log('[import] Sending request with', filesToImport.length, 'files');
      console.log('[import] First file:', filesToImport[0]?.fileName, 'content length:', filesToImport[0]?.content?.length);

      // Send to backend for parsing
      const statusDiv = document.getElementById('import-status');
      if (statusDiv) {
        statusDiv.style.display = 'block';
        statusDiv.textContent = 'Parsing binding files...';
      }

      const res = await fetch('/api/import/bindings', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ files: filesToImport })
      });

      if (!res.ok) {
        throw new Error(`Import failed: ${res.statusText}`);
      }

      const result = await res.json();

      console.log('[import] Result:', result);

      // Convert simple bindings to visual format
      console.log('[import] Converting bindings to visual format...');
      
      const currentProject = state.get().project;
      
      const convertRes = await fetch('/api/convert/convert', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({
          bindings: result.bindings.simple,
          project: currentProject
        })
      });

      if (!convertRes.ok) {
        throw new Error(`Conversion failed: ${convertRes.statusText}`);
      }

      const converted = await convertRes.json();
      console.log('[import] Converted:', converted);

      // Add converted bindings and devices to project
      // Note: moods need no creation step — they're derived live from project.nodes (Type 7 units)
      if (converted.devicesToCreate.length > 0 || converted.visualBindings.length > 0) {
        // Create "Imported" room if it doesn't exist (for non-mood devices)
        if (converted.devicesToCreate.length > 0) {
          let importedRoom = currentProject.homeView.rooms.find(r => r.name === 'Geïmporteerd');
          if (!importedRoom) {
            importedRoom = {
              id: `room-imported-${Date.now()}`,
              name: 'Geïmporteerd',
              floorId: currentProject.homeView.floors[0]?.id || 'floor-0',
              devices: []
            };
            dispatch({ type: 'ADD_ROOM', room: importedRoom });
          }

          // Add devices to imported room
          const startIndex = importedRoom.devices.length;
          converted.devicesToCreate.forEach((device, i) => {
            // Simple grid positioning - 5 columns, 120px spacing
            const index = startIndex + i;
            device.x = 50 + (index % 5) * 120;
            device.y = 50 + Math.floor(index / 5) * 100;
            
            console.log(`[import] Adding device ${device.name} at (${device.x}, ${device.y})`);
            
            dispatch({
              type: 'ADD_DEVICE_TO_ROOM',
              roomId: importedRoom.id,
              device
            });
          });
        }

        // Add bindings to project
        for (const binding of converted.visualBindings) {
          console.log(`[import] Adding binding: ${binding.from.deviceId} (${binding.from.portId}) → ${binding.to.deviceId} (${binding.to.portId})`);
          
          dispatch({
            type: 'ADD_BINDING',
            binding
          });
        }
      }

      if (statusDiv) {
        const msg = [
          `✅ Import voltooid!`,
          `📁 ${result.filesParsed} files parsed`,
          `📊 ${result.totalBindings} total bindings`,
          `✅ ${result.simpleBindings} simple (Type I/N)`,
          `⏳ ${result.complexBindings} complex (opgeslagen als legacy)`,
          ``,
          `🔄 Conversie:`,
          `➕ ${converted.devicesToCreate.length} apparaten aangemaakt`,
          `🔗 ${converted.visualBindings.length} bindings toegevoegd`,
        ];
        if (converted.warnings && converted.warnings.length > 0) {
          msg.push(`⚠️ ${converted.warnings.length} waarschuwingen`);
        }
        statusDiv.innerHTML = msg.join('<br>');
        statusDiv.style.background = '#e8f5e9';
        statusDiv.style.borderLeft = '4px solid #4caf50';
        statusDiv.style.color = '#2e7d32';
      }

      // Show detailed binding breakdown
      const bindingsDetail = document.createElement('div');
      bindingsDetail.style.cssText = 'margin-top:16px;padding:12px;border:1px solid #eef1f8;border-radius:6px;background:#f9fafb;max-height:300px;overflow-y:auto';
      
      let detailHTML = '<div style="font-size:12px;font-weight:600;color:#4a5568;margin-bottom:8px">Geïmporteerde Bindings per Bestand</div>';
      
      // Group bindings by file
      const bindingsByFile = {};
      for (const binding of [...result.bindings.simple, ...result.bindings.complex]) {
        const file = binding.fileName;
        if (!bindingsByFile[file]) {
          bindingsByFile[file] = { simple: 0, complex: 0 };
        }
        if (binding.bindingType === 'I' || binding.bindingType === 'N') {
          bindingsByFile[file].simple++;
        } else {
          bindingsByFile[file].complex++;
        }
      }
      
      for (const [file, counts] of Object.entries(bindingsByFile)) {
        const nodeAddr = file.match(/bind([0-9a-fA-F]{2})\.txt/i)?.[1]?.toUpperCase() || '??';
        detailHTML += `
          <div style="padding:6px 8px;font-size:12px;color:#4a5568;display:flex;align-items:center;gap:8px;border-bottom:1px solid #eef1f8">
            <span style="font-family:monospace;color:#6a7899;font-weight:600">0x${nodeAddr}</span>
            <span style="flex:1">${file}</span>
            <span style="color:#4caf50">✅ ${counts.simple}</span>
            <span style="color:#ff9800">⏳ ${counts.complex}</span>
          </div>
        `;
      }
      
      // Add warnings if any
      if (converted.warnings && converted.warnings.length > 0) {
        detailHTML += '<div style="margin-top:12px;font-size:12px;font-weight:600;color:#ff9800;margin-bottom:4px">⚠️ Waarschuwingen</div>';
        for (const warning of converted.warnings.slice(0, 10)) { // Show first 10 warnings
          detailHTML += `<div style="padding:4px 8px;font-size:11px;color:#6a7899;border-left:2px solid #ff9800;margin-bottom:2px">${warning}</div>`;
        }
        if (converted.warnings.length > 10) {
          detailHTML += `<div style="padding:4px 8px;font-size:11px;color:#a0aaba;font-style:italic">... en ${converted.warnings.length - 10} meer</div>`;
        }
      }
      
      bindingsDetail.innerHTML = detailHTML;
      
      if (statusDiv && statusDiv.parentNode) {
        statusDiv.parentNode.insertBefore(bindingsDetail, statusDiv.nextSibling);
      }

      // Show success message
      showToast(`Import geslaagd: ${converted.visualBindings.length} bindings en ${converted.devicesToCreate.length} apparaten toegevoegd`, 'success');
      
      // Don't auto-close, let user review the results
      submitBtn.textContent = 'Sluiten';
      submitBtn.disabled = false;
      submitBtn.onclick = () => overlay.remove();

    } catch (err) {
      console.error('[import] Error:', err);
      showToast(`Import mislukt: ${err.message}`, 'error');
      submitBtn.disabled = false;
      submitBtn.textContent = 'Importeer bindings';
    }
  });
}
