/**
 * Duotecno Configuration Viewer - Frontend
 */

let installation = null;
let currentNodeAddress = null;
let currentUnitAddress = null;
let filterWithBindings = true;
let filterWithoutBindings = true;

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
  
  // Filter units based on checkbox state
  const filteredUnits = node.units.filter(unit => {
    if (unit.hasBindings && !filterWithBindings) return false;
    if (!unit.hasBindings && !filterWithoutBindings) return false;
    return true;
  });
  
  // Render node detail
  const html = `
    <div class="node-detail">
      <h2>🖥️ ${node.name}</h2>
      
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
      
      ${filteredUnits.length > 0 ? `
        <h3>📋 Units (${filteredUnits.length} of ${node.units.length})</h3>
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
                    ${unit.hasBindings 
                      ? `<a class="unit-link" onclick="showUnitDetail(${node.nodeAddress}, ${unit.address[1]})">${unit.name}</a>`
                      : `<span class="unit-link no-bindings">${unit.name}</span>`
                    }
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
      <h2>📦 ${unit.name}</h2>
      
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
        ${unit.bindingsFromOtherNodes.map(ref => {
          const otherNode = installation.nodes.find(n => n.nodeAddress === ref.nodeAddress);
          return `
            <div class="binding-card">
              <div class="binding-header">
                <span class="binding-type">[${ref.bindingType}]</span>
                <span class="binding-number">From: ${otherNode?.name || 'Unknown'} (0x${ref.nodeAddress.toString(16).toUpperCase()})</span>
              </div>
            </div>
          `;
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
  
  return `
    <div class="binding-card">
      <div class="binding-header">
        <span class="binding-type">[${binding.bindingType}]</span>
        <span class="binding-number">${bindingNumber}</span>
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

// Initialize on page load
document.addEventListener('DOMContentLoaded', init);
