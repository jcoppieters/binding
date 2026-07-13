/**
 * Units View — debug/monitoring view showing all discovered units from master
 */

import { state } from '../app/state.js';
import { UnitType } from '../app/unit-types.js';

const UNIT_TYPE_LABELS = {
  [UnitType.kNoType]: 'Unknown',
  [UnitType.kDimmer]: 'Dimmer',
  [UnitType.kSwitch]: 'Relay',
  [UnitType.kInput]: 'Input',
  [UnitType.kTemperature]: 'Temperature',
  [UnitType.kExtendedAudio]: 'Audio (ext)',
  [UnitType.kMood]: 'Mood',
  [UnitType.kSwitchingMotor]: 'Motor',
  [UnitType.kAudio]: 'Audio',
  [UnitType.kAV]: 'AV',
  [UnitType.kIRTX]: 'IR TX',
  [UnitType.kVideo]: 'Video',
  [UnitType.kCamera]: 'Camera',
  [UnitType.kLightbulb]: 'Lightbulb',
  [UnitType.kRegister]: 'Register',
};

const UNIT_TYPE_ICONS = {
  [UnitType.kNoType]: '❓',
  [UnitType.kDimmer]: '💡',
  [UnitType.kSwitch]: '🔌',
  [UnitType.kInput]: '🎚️',
  [UnitType.kTemperature]: '🌡️',
  [UnitType.kExtendedAudio]: '🔊',
  [UnitType.kMood]: '🎭',
  [UnitType.kSwitchingMotor]: '🎬',
  [UnitType.kAudio]: '🔈',
  [UnitType.kAV]: '📺',
  [UnitType.kIRTX]: '📡',
  [UnitType.kVideo]: '🎥',
  [UnitType.kCamera]: '📷',
  [UnitType.kLightbulb]: '💡',
  [UnitType.kRegister]: '📋',
};

let _unsubscribe = null;

export function mount(container) {
  container.classList.add('units-view');
  if (_unsubscribe) _unsubscribe();
  _unsubscribe = state.subscribe(s => render(container, s));
}

export function unmount() {
  if (_unsubscribe) { _unsubscribe(); _unsubscribe = null; }
}

function render(container, s) {
  if (!s.connected || !s.discoveredNodes.length) {
    container.innerHTML = `
      <div style="padding:40px;text-align:center;color:#6a7899">
        <div style="font-size:48px;margin-bottom:16px">🔌</div>
        <h2 style="margin:0 0 8px 0;font-size:18px;color:#1a2540">Not Connected</h2>
        <p style="margin:0;font-size:14px">Connect to the master to view discovered units</p>
      </div>
    `;
    return;
  }

  const nodes = s.discoveredNodes;
  const totalUnits = nodes.reduce((sum, n) => sum + (n.units?.length ?? 0), 0);

  // Group units by type for statistics
  const unitsByType = {};
  for (const node of nodes) {
    for (const unit of node.units ?? []) {
      const type = unit.type ?? UnitType.kNoType;
      if (!unitsByType[type]) unitsByType[type] = [];
      unitsByType[type].push(unit);
    }
  }

  const stats = Object.entries(unitsByType)
    .map(([type, units]) => ({
      type: parseInt(type),
      label: UNIT_TYPE_LABELS[type] ?? `Type ${type}`,
      icon: UNIT_TYPE_ICONS[type] ?? '❓',
      count: units.length
    }))
    .sort((a, b) => b.count - a.count);

  container.innerHTML = `
    <div class="units-header">
      <h1>📊 Discovered Units</h1>
      <div class="units-summary">
        ${nodes.length} nodes · ${totalUnits} units
      </div>
    </div>

    <div class="units-stats">
      ${stats.map(s => `
        <div class="stat-card">
          <div class="stat-icon">${s.icon}</div>
          <div class="stat-label">${s.label}</div>
          <div class="stat-count">${s.count}</div>
        </div>
      `).join('')}
    </div>

    <div class="units-nodes">
      ${nodes.map(node => renderNode(node)).join('')}
    </div>
  `;
}

function renderNode(node) {
  const units = node.units ?? [];
  const nodeAddr = node.nodeAddress?.toString(16).toUpperCase().padStart(2, '0') ?? '??';
  
  return `
    <div class="units-node">
      <div class="units-node-header">
        <div class="units-node-title">
          <span class="units-node-addr">Node ${nodeAddr}</span>
          <span class="units-node-name">${node.name || 'Unnamed'}</span>
        </div>
        <div class="units-node-meta">
          Type ${node.type} · ${units.length} units
        </div>
      </div>
      <div class="units-view-table">
        <div class="units-view-table-header">
          <div class="col-addr">Unit</div>
          <div class="col-type">Type</div>
          <div class="col-name">Name</div>
          <div class="col-status">Status</div>
          <div class="col-value">Value</div>
        </div>
        ${units.map(unit => renderUnit(unit)).join('')}
      </div>
    </div>
  `;
}

function renderUnit(unit) {
  const unitAddr = unit.unitAddress?.toString(16).toUpperCase().padStart(2, '0') ?? '??';
  const typeLabel = UNIT_TYPE_LABELS[unit.type] ?? `Type ${unit.type}`;
  const typeIcon = UNIT_TYPE_ICONS[unit.type] ?? '❓';
  
  let statusBadge = '';
  if (unit.status !== undefined) {
    statusBadge = `<span class="status-badge status-${unit.status}">${unit.status}</span>`;
  }

  let valueDisplay = '—';
  if (unit.value !== undefined) {
    valueDisplay = unit.value;
    // Special formatting for different unit types
    if (unit.type === UnitType.kDimmer || unit.type === UnitType.kSwitch) {
      valueDisplay += '%';
    } else if (unit.type === UnitType.kTemperature && unit.temperature) {
      valueDisplay = `${(unit.temperature.temp / 10).toFixed(1)}°C`;
      if (unit.temperature.preset !== undefined) {
        valueDisplay += ` (preset: ${(unit.temperature.preset / 10).toFixed(1)}°C)`;
      }
    }
  }

  return `
    <div class="units-view-table-row">
      <div class="col-addr">${unitAddr}</div>
      <div class="col-type">
        <span class="type-icon">${typeIcon}</span>
        ${typeLabel}
      </div>
      <div class="col-name">${unit.name || '—'}</div>
      <div class="col-status">${statusBadge}</div>
      <div class="col-value">${valueDisplay}</div>
    </div>
  `;
}
