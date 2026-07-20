/**
 * Image Crop Modal
 * Shows a modal dialog for cropping images with drag/resize functionality
 */

/**
 * Show crop modal for an image
 * @param {string} imageDataUrl - Base64 data URL of the image
 * @param {Function} onCrop - Callback with cropped image data URL
 */
export function showCropModal(imageDataUrl, onCrop) {
  const overlay = document.createElement('div');
  overlay.className = 'modal-overlay';
  
  const dlg = document.createElement('div');
  dlg.className = 'modal-dialog';
  dlg.style.cssText = 'width:800px;max-width:95vw';

  const hdr = document.createElement('div');
  hdr.className = 'modal-header';
  hdr.innerHTML = '<strong>Grondplan bijsnijden</strong>';
  
  const closeBtn = document.createElement('button');
  closeBtn.className = 'modal-close';
  closeBtn.textContent = '✕';
  closeBtn.onclick = () => overlay.remove();
  hdr.append(closeBtn);

  const body = document.createElement('div');
  body.className = 'modal-body';
  body.style.padding = '16px';
  
  // Create canvas container
  const canvasContainer = document.createElement('div');
  canvasContainer.style.cssText = `
    position:relative;
    width:100%;
    max-height:60vh;
    overflow:auto;
    background:#f3f4f6;
    border-radius:8px;
    display:flex;
    justify-content:center;
    align-items:center;
  `;
  
  const canvas = document.createElement('canvas');
  canvas.style.cssText = 'display:block;max-width:100%;height:auto;cursor:crosshair';
  
  const cropOverlay = document.createElement('div');
  cropOverlay.style.cssText = `
    position:absolute;
    border:2px solid #e08c00;
    background:rgba(224,140,0,0.1);
    cursor:move;
    box-shadow:0 0 0 9999px rgba(0,0,0,0.5);
  `;
  
  // Create resize handles
  const handles = [];
  const handlePositions = [
    { pos: 'nw', cursor: 'nw-resize', x: 0, y: 0 },
    { pos: 'n', cursor: 'n-resize', x: 0.5, y: 0 },
    { pos: 'ne', cursor: 'ne-resize', x: 1, y: 0 },
    { pos: 'e', cursor: 'e-resize', x: 1, y: 0.5 },
    { pos: 'se', cursor: 'se-resize', x: 1, y: 1 },
    { pos: 's', cursor: 's-resize', x: 0.5, y: 1 },
    { pos: 'sw', cursor: 'sw-resize', x: 0, y: 1 },
    { pos: 'w', cursor: 'w-resize', x: 0, y: 0.5 }
  ];
  
  handlePositions.forEach(h => {
    const handle = document.createElement('div');
    handle.style.cssText = `
      position:absolute;
      width:10px;
      height:10px;
      background:#fff;
      border:2px solid #e08c00;
      border-radius:50%;
      cursor:${h.cursor};
      transform:translate(-50%, -50%);
    `;
    handle.dataset.pos = h.pos;
    handle.dataset.x = h.x;
    handle.dataset.y = h.y;
    cropOverlay.append(handle);
    handles.push(handle);
  });
  
  // Crop coordinates (declared here so they're accessible in cropBtn.onclick)
  let cropX, cropY, cropW, cropH;
  
  // Load image
  const img = new Image();
  img.onload = () => {
    // Set canvas size to image size
    canvas.width = img.width;
    canvas.height = img.height;
    
    const ctx = canvas.getContext('2d');
    ctx.drawImage(img, 0, 0);
    
    // Initial crop area (80% of image, centered)
    const cropMargin = 0.1;
    cropX = img.width * cropMargin;
    cropY = img.height * cropMargin;
    cropW = img.width * 0.8;
    cropH = img.height * 0.8;
    
    const updateCropOverlay = () => {
      const containerRect = canvasContainer.getBoundingClientRect();
      const canvasRect = canvas.getBoundingClientRect();
      const scaleX = canvasRect.width / canvas.width;
      const scaleY = canvasRect.height / canvas.height;
      
      // Account for canvas offset within centered container
      const offsetX = canvasRect.left - containerRect.left;
      const offsetY = canvasRect.top - containerRect.top;
      
      cropOverlay.style.left = (offsetX + cropX * scaleX) + 'px';
      cropOverlay.style.top = (offsetY + cropY * scaleY) + 'px';
      cropOverlay.style.width = (cropW * scaleX) + 'px';
      cropOverlay.style.height = (cropH * scaleY) + 'px';
      
      // Update handle positions
      handles.forEach(handle => {
        const hx = parseFloat(handle.dataset.x);
        const hy = parseFloat(handle.dataset.y);
        handle.style.left = (hx * 100) + '%';
        handle.style.top = (hy * 100) + '%';
      });
    };
    
    updateCropOverlay();
    window.addEventListener('resize', updateCropOverlay);
    
    // Drag/resize functionality
    let activeAction = null; // 'move' or 'resize'
    let resizeHandle = null;
    let dragStartX = 0;
    let dragStartY = 0;
    let cropStartX = 0;
    let cropStartY = 0;
    let cropStartW = 0;
    let cropStartH = 0;
    
    handles.forEach(handle => {
      handle.onmousedown = (e) => {
        activeAction = 'resize';
        resizeHandle = handle.dataset.pos;
        dragStartX = e.clientX;
        dragStartY = e.clientY;
        cropStartX = cropX;
        cropStartY = cropY;
        cropStartW = cropW;
        cropStartH = cropH;
        e.stopPropagation();
        e.preventDefault();
      };
    });
    
    cropOverlay.onmousedown = (e) => {
      if (activeAction === 'resize') return;
      activeAction = 'move';
      dragStartX = e.clientX;
      dragStartY = e.clientY;
      cropStartX = cropX;
      cropStartY = cropY;
      e.preventDefault();
    };
    
    document.onmousemove = (e) => {
      if (!activeAction) return;
      
      const canvasRect = canvas.getBoundingClientRect();
      const scaleX = canvas.width / canvasRect.width;
      const scaleY = canvas.height / canvasRect.height;
      
      const deltaX = (e.clientX - dragStartX) * scaleX;
      const deltaY = (e.clientY - dragStartY) * scaleY;
      
      if (activeAction === 'move') {
        cropX = Math.max(0, Math.min(canvas.width - cropW, cropStartX + deltaX));
        cropY = Math.max(0, Math.min(canvas.height - cropH, cropStartY + deltaY));
      } else if (activeAction === 'resize') {
        const minSize = 50;
        
        if (resizeHandle.includes('w')) {
          const newX = Math.max(0, Math.min(cropStartX + cropStartW - minSize, cropStartX + deltaX));
          cropW = cropStartW + (cropStartX - newX);
          cropX = newX;
        }
        if (resizeHandle.includes('e')) {
          cropW = Math.max(minSize, Math.min(canvas.width - cropStartX, cropStartW + deltaX));
        }
        if (resizeHandle.includes('n')) {
          const newY = Math.max(0, Math.min(cropStartY + cropStartH - minSize, cropStartY + deltaY));
          cropH = cropStartH + (cropStartY - newY);
          cropY = newY;
        }
        if (resizeHandle.includes('s')) {
          cropH = Math.max(minSize, Math.min(canvas.height - cropStartY, cropStartH + deltaY));
        }
      }
      
      updateCropOverlay();
    };
    
    document.onmouseup = () => {
      activeAction = null;
      resizeHandle = null;
    };
  };
  img.src = imageDataUrl;
  
  canvasContainer.append(canvas, cropOverlay);
  body.append(canvasContainer);

  const footer = document.createElement('div');
  footer.className = 'modal-footer';
  footer.style.marginTop = '16px';
  
  const cancelBtn = document.createElement('button');
  cancelBtn.className = 'modal-btn';
  cancelBtn.textContent = 'Annuleren';
  cancelBtn.onclick = () => overlay.remove();
  
  const cropBtn = document.createElement('button');
  cropBtn.className = 'modal-btn-primary';
  cropBtn.textContent = 'Bijsnijden';
  cropBtn.onclick = () => {
    // Create new canvas with cropped image
    const croppedCanvas = document.createElement('canvas');
    croppedCanvas.width = cropW;
    croppedCanvas.height = cropH;
    
    const ctx = croppedCanvas.getContext('2d');
    ctx.drawImage(canvas, cropX, cropY, cropW, cropH, 0, 0, cropW, cropH);
    
    const croppedDataUrl = croppedCanvas.toDataURL('image/png');
    overlay.remove();
    onCrop(croppedDataUrl);
  };

  footer.append(cancelBtn, cropBtn);
  dlg.append(hdr, body, footer);
  overlay.append(dlg);
  overlay.addEventListener('click', (e) => { if (e.target === overlay) overlay.remove(); });
  document.body.append(overlay);
}
