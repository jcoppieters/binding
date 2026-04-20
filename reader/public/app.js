/**
 * Duotecno Configuration Viewer - Frontend
 */

let installation = null;
let currentNodeAddress = null;
let currentUnitAddress = null;
let filterWithBindings = true;
let filterWithoutBindings = true;
let currentUnitTypeFilter = 'all'; // all, control, sens, virtual, relais, dimmer, motor, audio

// Event type names
const EVENT_NAMES = {
  0x01: 'Toggle',
  0x02: 'Pulse Toggle',
  0x03: 'Pulse',
  0x04: 'Manual Dimming',
  0x05: 'Relay Toggle',
  0x06: 'Sensor Display',
  0x07: 'Display Value'
};

/**
 * Initialize the application
 */
async function init() {
  setupEventListeners();
  await loadInstallation();
  await updateUploadButtonState();
}

/**
 * Setup event listeners
 */
function setupEventListeners() {
  document.getElementById('reload-btn').addEventListener('click', reloadInstallation);
  document.getElementById('filter-with-bindings').addEventListener('change', handleFilterChange);
  document.getElementById('filter-without-bindings').addEventListener('change', handleFilterChange);
}

/**
 * Handle filter checkbox changes
 */
function handleFilterChange(e) {
  if (e.target.id === 'filter-with-bindings') {
    filterWithBindings = e.target.checked;
  } else if (e.target.id === 'filter-without-bindings') {
    filterWithoutBindings = e.target.checked;
  }
  
  // Re-render current view
  if (currentUnitAddress !== null) {
    showUnitDetail(currentNodeAddress, currentUnitAddress);
  } else if (currentNodeAddress !== null) {
    showNodeDetail(currentNodeAddress);
  }
}

/**
 * Load installation data from API
 */
async function loadInstallation() {
  try {
    showLoading();
    const response = await fetch('/api/installation');
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    installation = await response.json();
    hideLoading();
    renderApp();
  } catch (error) {
    console.error('Error loading installation:', error);
    showError(`Failed to load configuration: ${error.message}`);
  }
}

/**
 * Reload installation from server
 */
async function reloadInstallation() {
  try {
    showLoading();
    const response = await fetch('/api/reload', { method: 'POST' });
    
    if (!response.ok) {
      throw new Error(`HTTP error! status: ${response.status}`);
    }
    
    await loadInstallation();
    await updateUploadButtonState();
  } catch (error) {
    console.error('Error reloading installation:', error);
    showError(`Failed to reload configuration: ${error.message}`);
  }
}

/**
 * Show loading state
 */
function showLoading() {
  document.getElementById('loading').style.display = 'flex';
  document.getElementById('error').style.display = 'none';
  document.getElementById('main-content').style.display = 'none';
}

/**
 * Hide loading state
 */
function hideLoading() {
  document.getElementById('loading').style.display = 'none';
  document.getElementById('main-content').style.display = 'flex';
}

/**
 * Show error message
 */
function showError(message) {
  document.getElementById('loading').style.display = 'none';
  document.getElementById('main-content').style.display = 'none';
  document.getElementById('error').style.display = 'flex';
  document.getElementById('error-message').textContent = message;
}

/**
 * Render the application
 */
function renderApp() {
  renderSummary();
  renderNodesList();
  
  // Show first node by default
  if (installation.nodes.length > 0) {
    showNodeDetail(installation.nodes[0].nodeAddress);
  }
}

/**
 * Render summary section
 */
function renderSummary() {
  const { summary } = installation;
  const html = `
    <div class="summary-item">
      <span>Nodes</span>
      <strong>${summary.totalNodes}</strong>
    </div>
    <div class="summary-item">
      <span>Units</span>
      <strong>${summary.totalUnits}</strong>
    </div>
    <div class="summary-item">
      <span>Bindings</span>
      <strong>${summary.totalBindings}</strong>
    </div>
  `;
  document.getElementById('summary-content').innerHTML = html;
}

/**
 * Render nodes list in sidebar
 */
function renderNodesList() {
  const html = installation.nodes.map(node => {
    const isActive = node.nodeAddress === currentNodeAddress ? 'active' : '';
    return `
      <div class="node-item ${isActive}" onclick="showNodeDetail(${node.nodeAddress})">
        <div class="node-item-header">
          <span>${node.name}</span>
          <span class="node-item-address">0x${node.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}</span>
        </div>
        <div class="node-item-meta">
          ${node.nodeType} • ${node.numberOfUnits} units
        </div>
      </div>
    `;
  }).join('');
  
  document.getElementById('nodes-list').innerHTML = html;
}

/**
 * Show node detail view
 */
function showNodeDetail(nodeAddress) {
  currentNodeAddress = nodeAddress;
  currentUnitAddress = null;
  
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  // Update node list highlighting
  renderNodesList();
  
  // Render breadcrumb
  const breadcrumb = `
    <span>Home</span>
    <span class="breadcrumb-separator">/</span>
    <span>${node.name}</span>
  `;
  document.getElementById('breadcrumb').innerHTML = breadcrumb;
  
  // Filter units based on checkbox state and unit type
  const filteredUnits = node.units.filter(unit => {
    // Filter by bindings checkbox
    if (unit.hasBindings && !filterWithBindings) return false;
    if (!unit.hasBindings && !filterWithoutBindings) return false;
    
    // Filter by unit type
    if (currentUnitTypeFilter !== 'all') {
      const unitType = unit.unitType.toLowerCase();
      if (currentUnitTypeFilter === 'control' && unitType !== 'control') return false;
      if (currentUnitTypeFilter === 'sens' && unitType !== 'sens') return false;
      if (currentUnitTypeFilter === 'virtual' && unitType !== 'virtual') return false;
      if (currentUnitTypeFilter === 'relais' && unitType !== 'relais') return false;
      if (currentUnitTypeFilter === 'dimmer' && unitType !== 'dimmer') return false;
      if (currentUnitTypeFilter === 'motor' && unitType !== 'motor') return false;
      if (currentUnitTypeFilter === 'audio' && !unitType.includes('audio')) return false;
    }
    
    return true;
  });
  
  // Count units by type for tab badges
  const unitTypeCounts = {
    all: node.units.length,
    control: node.units.filter(u => u.unitType.toLowerCase() === 'control').length,
    sens: node.units.filter(u => u.unitType.toLowerCase() === 'sens').length,
    virtual: node.units.filter(u => u.unitType.toLowerCase() === 'virtual').length,
    relais: node.units.filter(u => u.unitType.toLowerCase() === 'relais').length,
    dimmer: node.units.filter(u => u.unitType.toLowerCase() === 'dimmer').length,
    motor: node.units.filter(u => u.unitType.toLowerCase() === 'motor').length,
    audio: node.units.filter(u => u.unitType.toLowerCase().includes('audio')).length,
  };
  
  // Render node detail
  const html = `
    <div class="node-detail">
      <div class="detail-header">
        <h2>🖥️ ${node.name}</h2>
        <button class="btn-icon" onclick="renameNode(${nodeAddress})" title="Rename node">
          ✏️ Rename
        </button>
        <button class="btn-icon btn-hardware" onclick="pushNodeNameToHardware(${nodeAddress})" title="Push name to hardware">
          📤 Push to Hardware
        </button>
      </div>
      
      <div class="node-meta">
        <div class="meta-item">
          <span class="meta-label">Address</span>
          <span class="meta-value">0x${node.nodeAddress.toString(16).toUpperCase().padStart(2, '0')}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Type</span>
          <span class="meta-value">${node.nodeType}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Physical Address</span>
          <span class="meta-value">0x${node.physicalAddress.toString(16).toUpperCase()}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Software</span>
          <span class="meta-value">${node.softwareVersion}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Total Units</span>
          <span class="meta-value">${node.numberOfUnits}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Units with Bindings</span>
          <span class="meta-value">${node.units.filter(u => u.hasBindings).length}</span>
        </div>
      </div>
      
      <div class="unit-type-tabs">
        <button class="unit-type-tab ${currentUnitTypeFilter === 'all' ? 'active' : ''}" onclick="setUnitTypeFilter('all')">
          All <span class="tab-badge">${unitTypeCounts.all}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'control' ? 'active' : ''}" onclick="setUnitTypeFilter('control')"}>
          Inputs <span class="tab-badge">${unitTypeCounts.control}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'sens' ? 'active' : ''}" onclick="setUnitTypeFilter('sens')"}>
          Sensors <span class="tab-badge">${unitTypeCounts.sens}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'virtual' ? 'active' : ''}" onclick="setUnitTypeFilter('virtual')">
          Moods <span class="tab-badge">${unitTypeCounts.virtual}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'relais' ? 'active' : ''}" onclick="setUnitTypeFilter('relais')">
          Relais <span class="tab-badge">${unitTypeCounts.relais}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'dimmer' ? 'active' : ''}" onclick="setUnitTypeFilter('dimmer')">
          Dimmers <span class="tab-badge">${unitTypeCounts.dimmer}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'motor' ? 'active' : ''}" onclick="setUnitTypeFilter('motor')">
          Motors <span class="tab-badge">${unitTypeCounts.motor}</span>
        </button>
        <button class="unit-type-tab ${currentUnitTypeFilter === 'audio' ? 'active' : ''}" onclick="setUnitTypeFilter('audio')">
          Audio <span class="tab-badge">${unitTypeCounts.audio}</span>
        </button>
      </div>
      
      ${filteredUnits.length > 0 ? `
        <h3>📋 Units (${filteredUnits.length})</h3>
        <table class="units-table">
          <thead>
            <tr>
              <th>Address</th>
              <th>Name</th>
              <th>Type</th>
              <th>Bindings</th>
            </tr>
          </thead>
          <tbody>
            ${filteredUnits.map(unit => {
              const addr = `0x${unit.address[0].toString(16).toUpperCase().padStart(2, '0')}:${unit.address[1].toString(16).toUpperCase().padStart(2, '0')}`;
              const bindingCount = unit.bindingsAsInput.length + unit.bindingsAsOutput.length + unit.bindingsFromOtherNodes.length;
              
              return `
                <tr>
                  <td><code>${addr}</code></td>
                  <td>
                    <a class="unit-link" onclick="showUnitDetail(${node.nodeAddress}, ${unit.address[1]})">${unit.name}</a>
                  </td>
                  <td>${unit.unitType}</td>
                  <td>
                    ${unit.hasBindings 
                      ? `<span class="binding-badge">${bindingCount}</span>`
                      : `<span class="binding-badge no-bindings">None</span>`
                    }
                  </td>
                </tr>
              `;
            }).join('')}
          </tbody>
        </table>
      ` : `
        <div class="empty-state">
          <div class="empty-state-icon">🔍</div>
          <p>No units match the current filter settings</p>
        </div>
      `}
    </div>
  `;
  
  document.getElementById('detail-view').innerHTML = html;
}

/**
 * Set unit type filter and re-render
 */
function setUnitTypeFilter(filterType) {
  currentUnitTypeFilter = filterType;
  if (currentNodeAddress !== null) {
    showNodeDetail(currentNodeAddress);
  }
}

/**
 * Show unit detail view with bindings
 */
function showUnitDetail(nodeAddress, unitAddress) {
  currentNodeAddress = nodeAddress;
  currentUnitAddress = unitAddress;
  
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  const unit = node.units.find(u => u.address[1] === unitAddress);
  if (!unit) return;
  
  // Render breadcrumb
  const breadcrumb = `
    <span>Home</span>
    <span class="breadcrumb-separator">/</span>
    <a onclick="showNodeDetail(${nodeAddress})">${node.name}</a>
    <span class="breadcrumb-separator">/</span>
    <span>${unit.name}</span>
  `;
  document.getElementById('breadcrumb').innerHTML = breadcrumb;
  
  // Render unit detail
  const addr = `0x${unit.address[0].toString(16).toUpperCase().padStart(2, '0')}:${unit.address[1].toString(16).toUpperCase().padStart(2, '0')}`;
  
  let html = `
    <div class="unit-detail">
      <div class="detail-header">
        <h2>📦 ${unit.name}</h2>
        <button class="btn-icon" onclick="renameUnit(${nodeAddress}, ${unitAddress})" title="Rename unit">
          ✏️ Rename
        </button>
        <button class="btn-icon btn-hardware" onclick="pushUnitNameToHardware(${nodeAddress}, ${unitAddress})" title="Push name to hardware">
          📤 Push to Hardware
        </button>
      </div>
      
      <div class="unit-meta">
        <div class="meta-item">
          <span class="meta-label">Address</span>
          <span class="meta-value">${addr}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Type</span>
          <span class="meta-value">${unit.unitType}</span>
        </div>
        <div class="meta-item">
          <span class="meta-label">Node</span>
          <span class="meta-value">${node.name}</span>
        </div>
      </div>
  `;
  
  // Bindings where this unit is an input (triggers action)
  if (unit.bindingsAsInput.length > 0) {
    html += `
      <div class="bindings-section">
        <h3>⚡ Outgoing Bindings (${unit.bindingsAsInput.length})</h3>
        <p style="color: var(--text-muted); font-size: 0.875rem; margin-bottom: 1rem;">
          When this unit triggers an event, it controls these targets:
        </p>
        ${unit.bindingsAsInput.map(binding => renderBinding(binding, node, unit, 'input')).join('')}
      </div>
    `;
  }
  
  // Bindings where this unit is an output (receives action)
  if (unit.bindingsAsOutput.length > 0) {
    html += `
      <div class="bindings-section">
        <h3>📥 Incoming Bindings (${unit.bindingsAsOutput.length})</h3>
        <p style="color: var(--text-muted); font-size: 0.875rem; margin-bottom: 1rem;">
          This unit is controlled by these sources:
        </p>
        ${unit.bindingsAsOutput.map(binding => renderBinding(binding, node, unit, 'output')).join('')}
      </div>
    `;
  }
  
  // Bindings from other nodes
  if (unit.bindingsFromOtherNodes.length > 0) {
    html += `
      <div class="bindings-section">
        <h3>🔗 Cross-Node Bindings (${unit.bindingsFromOtherNodes.length})</h3>
        <p style="color: var(--text-muted); font-size: 0.875rem; margin-bottom: 1rem;">
          This unit is referenced in bindings from other nodes:
        </p>
        ${unit.bindingsFromOtherNodes.map(binding => {
          const otherNode = installation.nodes.find(n => n.nodeAddress === binding.nodeAddress);
          return renderBinding(binding, otherNode, unit, 'cross-node');
        }).join('')}
      </div>
    `;
  }
  
  if (!unit.hasBindings) {
    html += `
      <div class="empty-state">
        <div class="empty-state-icon">📭</div>
        <p>This unit has no bindings configured</p>
      </div>
    `;
  }
  
  // Add binding creation button
  html += `
    <div class="unit-binding-actions">
      <button class="add-binding-btn" onclick="openBindingEditor(${nodeAddress}, ${unitAddress})">
        ➕ Add New Binding
      </button>
    </div>
  `;
  
  html += `</div>`;
  
  document.getElementById('detail-view').innerHTML = html;
}

/**
 * Render a single binding
 */
function renderBinding(binding, currentNode, currentUnit, direction) {
  const bindingNumber = `0x${binding.bindingNumber.toString(16).toUpperCase().padStart(4, '0')}`;
  
  let flowHTML = '';
  
  if (direction === 'input') {
    // Show: [This Unit] → [Output Units]
    binding.inputUnits.forEach((input, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">•</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(input, true);
      flowHTML += `</div>`;
    });
    
    if (binding.operators && binding.operators.length > 0) {
      flowHTML += `<span class="binding-arrow">${binding.operators.join(' ')}</span>`;
    }
    
    flowHTML += '<span class="binding-arrow">⇒</span>';
    
    binding.outputUnits.forEach((output, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">+</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(output, false);
      flowHTML += `</div>`;
    });
  } else if (direction === 'cross-node') {
    // Show full binding flow for cross-node bindings: [Input Units] → [Output Units]
    binding.inputUnits.forEach((input, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">•</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(input, true);
      flowHTML += `</div>`;
    });
    
    if (binding.operators && binding.operators.length > 0) {
      flowHTML += `<span class="binding-arrow">${binding.operators.join(' ')}</span>`;
    }
    
    flowHTML += '<span class="binding-arrow">⇒</span>';
    
    binding.outputUnits.forEach((output, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">+</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(output, false);
      flowHTML += `</div>`;
    });
  } else {
    // Show: [Input Units] → [This Unit]
    binding.inputUnits.forEach((input, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">•</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(input, true);
      flowHTML += `</div>`;
    });
    
    if (binding.operators && binding.operators.length > 0) {
      flowHTML += `<span class="binding-arrow">${binding.operators.join(' ')}</span>`;
    }
    
    flowHTML += '<span class="binding-arrow">⇒</span>';
    
    binding.outputUnits.forEach((output, idx) => {
      if (idx > 0) flowHTML += '<span class="binding-arrow">+</span>';
      
      flowHTML += `<div class="binding-unit">`;
      flowHTML += renderUnitReference(output, false);
      flowHTML += `</div>`;
    });
  }
  
  // For cross-node bindings, show which node this binding is from
  const nodeInfo = direction === 'cross-node' 
    ? ` <span class="binding-node-info">From: ${currentNode?.name || 'Unknown Node'}</span>` 
    : '';
  
  // Show edit/delete buttons for all bindings (pass binding object for edit)
  const bindingDataJSON = JSON.stringify(binding).replace(/"/g, '&quot;');
  const actionsHTML = `
    <div class="binding-actions">
      <button class="btn-icon" onclick='editBinding(${currentNode.nodeAddress}, ${binding.bindingNumber}, JSON.parse("${bindingDataJSON}"))' title="Edit binding">
        ✏️
      </button>
      <button class="btn-icon btn-danger" onclick="deleteBinding(${currentNode.nodeAddress}, ${binding.bindingNumber})" title="Delete binding">
        🗑️
      </button>
    </div>
  `;
  
  return `
    <div class="binding-card">
      <div class="binding-header">
        <div class="binding-header-left">
          <span class="binding-type">[${binding.bindingType}]</span>
          <span class="binding-number">${bindingNumber}${nodeInfo}</span>
        </div>
        ${actionsHTML}
      </div>
      <div class="binding-flow">
        ${flowHTML}
      </div>
    </div>
  `;
}

/**
 * Render a unit reference (input or output)
 */
function renderUnitReference(unitRef, isInput) {
  const node = installation.nodes.find(n => n.nodeAddress === unitRef.nodeAddress);
  const unit = node?.units.find(u => u.address[1] === unitRef.unitAddress);
  
  const nodeName = node?.name || `Node 0x${unitRef.nodeAddress.toString(16).toUpperCase()}`;
  const unitName = unit?.name || `Unit 0x${unitRef.unitAddress.toString(16).toUpperCase()}`;
  
  let html = `
    <a class="binding-unit-link" onclick="showUnitDetail(${unitRef.nodeAddress}, ${unitRef.unitAddress})">
      ${nodeName} → ${unitName}
    </a>
  `;
  
  if (isInput && unitRef.event !== undefined) {
    const eventName = EVENT_NAMES[unitRef.event] || `Event 0x${unitRef.event.toString(16).toUpperCase()}`;
    const eventChar = unitRef.eventChar ? ` ${unitRef.eventChar}'` : '';
    html += `<div class="binding-event">[Event: 0x${unitRef.event.toString(16).toUpperCase().padStart(2, '0')}] (${eventName})${eventChar}</div>`;
  }
  
  return html;
}

/**
 * Open the binding editor for creating a new binding
 */
async function openBindingEditor(presetNodeAddress = null, presetUnitAddress = null) {
  const returnToNode = currentNodeAddress;
  
  // Open the editor with a callback to handle saving
  bindingEditor.open(installation, async (bindingString, bindingData) => {
    console.log('Binding created:', bindingString);
    console.log('Binding data:', bindingData);
    
    try {
      // Determine target node (where the binding will be stored)
      const targetNodeAddress = bindingData.outputNodeAddress;
      
      // Add binding to the target node
      const response = await fetch(`/api/bindings/${targetNodeAddress.toString(16).padStart(2, '0')}`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ bindingString }),
      });
      
      const result = await response.json();
      
      if (result.success) {
        // Show success message
        alert('✅ Binding created successfully!');
        
        // Reload the installation to show the new binding
        await reloadInstallation();
        
        // Return to the original node if one was selected
        if (returnToNode !== null) {
          showNodeDetail(returnToNode);
        }
      } else {
        alert('❌ Failed to add binding: ' + (result.error || 'Unknown error'));
      }
    } catch (error) {
      console.error('Failed to add binding:', error);
      alert('❌ Error adding binding: ' + error.message);
    }
  });
  
  // If preset values are provided, pre-select them in the editor
  if (presetNodeAddress !== null && presetUnitAddress !== null) {
    // Wait a bit for the modal to render
    setTimeout(() => {
      const inputNodeSelect = document.getElementById('input-node-select');
      const inputUnitSelect = document.getElementById('input-unit-select');
      
      if (inputNodeSelect) {
        inputNodeSelect.value = presetNodeAddress;
        bindingEditor.onInputNodeChange();
        
        if (inputUnitSelect) {
          setTimeout(() => {
            inputUnitSelect.value = presetUnitAddress;
            bindingEditor.onInputUnitChange();
          }, 100);
        }
      }
    }, 100);
  }
}

/**
 * Delete a binding
 */
async function deleteBinding(nodeAddress, bindingIndex) {
  const nodeHex = nodeAddress.toString(16).padStart(2, '0').toUpperCase();
  const returnToNode = currentNodeAddress;
  
  if (!confirm(`Are you sure you want to delete binding #${bindingIndex} from node 0x${nodeHex}?\n\nThis will remove the binding from memory.`)) {
    return;
  }
  
  try {
    const response = await fetch(`/api/bindings/${nodeHex}/${bindingIndex}`, {
      method: 'DELETE'
    });
    
    const result = await response.json();
    
    if (result.success) {
      alert('✅ Binding deleted successfully!');
      
      // Reload to show updated bindings
      await reloadInstallation();
      await updateUploadButtonState();
      
      // Return to the original node view
      if (returnToNode !== null) {
        showNodeDetail(returnToNode);
      }
    } else {
      alert('❌ Failed to delete binding: ' + (result.error || 'Unknown error'));
    }
  } catch (error) {
    console.error('Failed to delete binding:', error);
    alert('❌ Error deleting binding: ' + error.message);
  }
}

/**
 * Edit a binding
 */
async function editBinding(nodeAddress, bindingIndex, binding) {
  // Store the current node for returning after edit
  const returnToNode = currentNodeAddress;
  
  // Extract binding data for editing
  const editData = {
    bindingIndex: binding.bindingNumber,
    nodeAddress: nodeAddress,
    inputUnits: binding.inputUnits,
    outputUnits: binding.outputUnits,
    operators: binding.operators || []
  };
  
  // Open editor in edit mode
  bindingEditor.open(installation, async (bindingString, bindingData) => {
    try {
      // Update the binding
      const response = await fetch(`/api/bindings/${nodeAddress.toString(16).padStart(2, '0')}/${bindingIndex}`, {
        method: 'PUT',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ bindingString }),
      });
      
      const result = await response.json();
      
      if (result.success) {
        alert('✅ Binding updated successfully!');
        await reloadInstallation();
        await updateUploadButtonState();
        
        // Return to the original node
        if (returnToNode !== null) {
          showNodeDetail(returnToNode);
        }
      } else {
        alert('❌ Failed to update binding: ' + (result.error || 'Unknown error'));
      }
    } catch (error) {
      console.error('Failed to update binding:', error);
      alert('❌ Error updating binding: ' + error.message);
    }
  }, editData);
}

/**
 * Update upload button state based on modified nodes
 */
async function updateUploadButtonState() {
  try {
    const response = await fetch('/api/bindings/modified');
    const result = await response.json();
    const uploadBtn = document.getElementById('upload-btn');
    
    if (!uploadBtn) return;
    
    if (result.modified && result.modified.length > 0) {
      // Has unsaved changes - make button blue/prominent
      uploadBtn.classList.remove('btn-secondary');
      uploadBtn.classList.add('btn-primary');
      uploadBtn.classList.add('has-changes');
      uploadBtn.title = `${result.modified.length} node(s) with unsaved changes`;
    } else {
      // No unsaved changes - make button secondary
      uploadBtn.classList.remove('btn-primary', 'has-changes');
      uploadBtn.classList.add('btn-secondary');
      uploadBtn.title = 'Upload to Hardware';
    }
  } catch (error) {
    console.error('Error checking modified nodes:', error);
  }
}

/**
 * Rename a node
 */
async function renameNode(nodeAddress) {
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  const newName = prompt(`Rename node "${node.name}":`, node.name);
  if (!newName || newName === node.name) return;
  
  // Ask if user wants to push to hardware
  const pushToHardware = confirm('Do you want to push this name to the hardware node?\n\n✅ Yes - Save to file AND hardware\n❌ No - Save to file only');
  
  try {
    const response = await fetch(`/api/nodes/${nodeAddress.toString(16).padStart(2, '0')}/rename`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name: newName, pushToHardware }),
    });
    
    const result = await response.json();
    
    if (result.success) {
      let message = '✅ Node renamed successfully!';
      if (result.hardwarePush) {
        message += result.hardwarePush.success 
          ? '\n📤 Name pushed to hardware!' 
          : `\n⚠️ Hardware push failed: ${result.hardwarePush.error}`;
      }
      alert(message);
      await reloadInstallation();
      showNodeDetail(nodeAddress);
    } else {
      alert('❌ Failed to rename node: ' + (result.error || 'Unknown error'));
    }
  } catch (error) {
    console.error('Failed to rename node:', error);
    alert('❌ Error renaming node: ' + error.message);
  }
}

/**
 * Rename a unit
 */
async function renameUnit(nodeAddress, unitAddress) {
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  const unit = node.units.find(u => u.address[1] === unitAddress);
  if (!unit) return;
  
  const newName = prompt(`Rename unit "${unit.name}":`, unit.name);
  if (!newName || newName === unit.name) return;
  
  // Ask if user wants to push to hardware
  const pushToHardware = confirm('Do you want to push this name to the hardware unit?\n\n✅ Yes - Save to file AND hardware\n❌ No - Save to file only');
  
  try {
    const response = await fetch(`/api/nodes/${nodeAddress.toString(16).padStart(2, '0')}/units/${unitAddress.toString(16).padStart(2, '0')}/rename`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name: newName, pushToHardware }),
    });
    
    const result = await response.json();
    
    if (result.success) {
      let message = '✅ Unit renamed successfully!';
      if (result.hardwarePush) {
        message += result.hardwarePush.success 
          ? '\n📤 Name pushed to hardware!' 
          : `\n⚠️ Hardware push failed: ${result.hardwarePush.error}`;
      }
      alert(message);
      await reloadInstallation();
      showUnitDetail(nodeAddress, unitAddress);
    } else {
      alert('❌ Failed to rename unit: ' + (result.error || 'Unknown error'));
    }
  } catch (error) {
    console.error('Failed to rename unit:', error);
    alert('❌ Error renaming unit: ' + error.message);
  }
}

/**
 * Push node name to hardware (separate button)
 */
async function pushNodeNameToHardware(nodeAddress) {
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  if (!confirm(`Push node name "${node.name}" to hardware node 0x${nodeAddress.toString(16).toUpperCase()}?`)) {
    return;
  }
  
  try {
    const response = await fetch(`/api/nodes/${nodeAddress.toString(16).padStart(2, '0')}/push-name`, {
      method: 'POST',
    });
    
    const result = await response.json();
    
    if (result.success) {
      alert(`✅ Node name "${node.name}" pushed to hardware successfully!`);
    } else {
      alert(`❌ Failed to push name: ${result.error || 'Unknown error'}`);
    }
  } catch (error) {
    console.error('Failed to push node name:', error);
    alert('❌ Network error. Check console for details.');
  }
}

/**
 * Push unit name to hardware (separate button)
 */
async function pushUnitNameToHardware(nodeAddress, unitAddress) {
  const node = installation.nodes.find(n => n.nodeAddress === nodeAddress);
  if (!node) return;
  
  const unit = node.units.find(u => u.address[1] === unitAddress);
  if (!unit) return;
  
  if (!confirm(`Push unit name "${unit.name}" to hardware?\n\nNode: 0x${nodeAddress.toString(16).toUpperCase()}\nUnit: 0x${unitAddress.toString(16).toUpperCase()}`)) {
    return;
  }
  
  try {
    const response = await fetch(`/api/nodes/${nodeAddress.toString(16).padStart(2, '0')}/units/${unitAddress.toString(16).padStart(2, '0')}/push-name`, {
      method: 'POST',
    });
    
    const result = await response.json();
    
    if (result.success) {
      alert(`✅ Unit name "${unit.name}" pushed to hardware successfully!`);
    } else {
      alert(`❌ Failed to push name: ${result.error || 'Unknown error'}`);
    }
  } catch (error) {
    console.error('Failed to push unit name:', error);
    alert('❌ Network error. Check console for details.');
  }
}

/**
 * Push all names to hardware
 */
async function pushAllNamesToHardware() {
  if (!confirm('Push ALL node and unit names to hardware?\n\nThis may take several minutes depending on the number of nodes and units.')) {
    return;
  }
  
  const startTime = Date.now();
  
  try {
    const response = await fetch('/api/hardware/push-all-names', {
      method: 'POST',
    });
    
    const result = await response.json();
    
    const duration = ((Date.now() - startTime) / 1000).toFixed(1);
    
    let message = `Operation completed in ${duration}s\n\n`;
    message += `✅ Nodes: ${result.successfulNodes}/${result.totalNodes}\n`;
    message += `✅ Units: ${result.successfulUnits}/${result.totalUnits}\n`;
    
    if (result.errors.length > 0) {
      message += `\n⚠️ ${result.errors.length} errors:\n`;
      message += result.errors.slice(0, 5).join('\n');
      if (result.errors.length > 5) {
        message += `\n... and ${result.errors.length - 5} more errors`;
      }
    }
    
    alert(message);
  } catch (error) {
    console.error('Failed to push all names:', error);
    alert('❌ Network error. Check console for details.');
  }
}

// Initialize on page load
document.addEventListener('DOMContentLoaded', init);
