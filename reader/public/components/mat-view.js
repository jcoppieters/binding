/**
 * Materials View — Bill of materials (BOM) with quantities, power, and costs
 */

// @ts-check

import { state } from '../app/state.js';

/**
 * @typedef {import('../app/state.js').ModuleInstance} ModuleInstance
 * @typedef {import('../app/state.js').WoningDevice} WoningDevice
 * @typedef {import('../app/state.js').ModuleDef} ModuleDef
 */

/** Called when the mat view tab is activated */
export function activate() {
  const s = state.get();
  renderMaterialsView(s.project, s.modules);
}

/** Mount export button handlers */
export function mount(container) {
  document.getElementById('btn-export-excel')?.addEventListener('click', exportToExcel);
  document.getElementById('btn-export-pdf')?.addEventListener('click', exportToPDF);
}

/** Export materials list to CSV (opens in Excel) */
function exportToExcel() {
  const s = state.get();
  const data = collectMaterialsData(s.project, s.modules);
  
  const csv = [
    ['Model', 'Omschrijving', 'Categorie', 'Aantal', 'DIN (M)', 'Vermogen (W)', 'Prijs/st (€)', 'Totaal (€)'],
    ...data.rows.map(r => [
      r.model,
      r.name,
      r.category,
      r.count,
      r.totalDin > 0 ? r.totalDin.toFixed(1) : '',
      r.totalPower > 0 ? r.totalPower : '',
      r.priceEur > 0 ? r.priceEur : '',
      r.totalPrice > 0 ? r.totalPrice : ''
    ]),
    [],
    ['TOTAAL', '', '', data.totals.modules, data.totals.din.toFixed(1), data.totals.power, '', data.totals.cost]
  ].map(row => row.join(';')).join('\n');

  const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
  const link = document.createElement('a');
  link.href = URL.createObjectURL(blob);
  link.download = `${s.project.metadata.name || 'Project'}_materiaallijst.csv`;
  link.click();
}

/** Export to PDF (placeholder — would need jsPDF or server-side generation) */
function exportToPDF() {
  alert('PDF export: implementatie volgt (gebruik Excel export voor nu)');
}

/**
 * Collect materials data for both rendering and export
 * @param {Object} project 
 * @param {Array} modules 
 * @returns {{ rows: Array, totals: Object }}
 */
function collectMaterialsData(project, modules) {
  const allInstances = [
    ...project.railView.cabinets.flatMap(c => c.modules),
    ...project.railView.woningDevices
  ];

  const modelCounts = new Map();
  
  for (const inst of allInstances) {
    const key = inst.model;
    if (!modelCounts.has(key)) {
      modelCounts.set(key, { count: 0, instances: [] });
    }
    const entry = modelCounts.get(key);
    entry.count++;
    entry.instances.push(inst);
  }

  const rows = [];
  
  for (const [model, { count }] of modelCounts.entries()) {
    // modules is now an object { 'DT0B-01': {...}, 'DT10-AU': {...} }
    const def = modules[model];
    
    // If not found, create minimal entry with just model name
    const name = def?.productLine ?? def?.name ?? model;
    const finish = def?.finish ? ` (${def.finish})` : '';
    const category = def?.uiCategory ?? def?.category ?? '—';
    const dinUnits = def?.dinUnits ?? 0;
    const powerW = def?.powerW ?? 0;
    const priceEur = def?.priceEur ?? 0;
    
    rows.push({
      model,
      name: name + finish,
      category,
      count,
      dinUnits,
      powerW,
      priceEur,
      totalDin: dinUnits * count,
      totalPower: powerW * count,
      totalPrice: priceEur * count
    });
  }

  rows.sort((a, b) => {
    if (a.category !== b.category) return a.category.localeCompare(b.category);
    return a.name.localeCompare(b.name);
  });

  const totals = {
    modules: rows.reduce((sum, r) => sum + r.count, 0),
    din: rows.reduce((sum, r) => sum + r.totalDin, 0),
    power: rows.reduce((sum, r) => sum + r.totalPower, 0),
    cost: rows.reduce((sum, r) => sum + r.totalPrice, 0)
  };

  return { rows, totals };
}

/**
 * Render the materials view table
 * @param {Object} project - Project data
 * @param {ModuleDef[]} modules - Module database
 */
function renderMaterialsView(project, modules) {
  const table = document.getElementById('mat-table');
  const empty = document.getElementById('mat-empty');
  if (!table || !empty) return;

  const { rows, totals } = collectMaterialsData(project, modules);

  if (rows.length === 0) {
    table.style.display = 'none';
    empty.style.display = 'flex';
    return;
  }

  empty.style.display = 'none';
  table.style.display = 'table';

  // Render table
  table.innerHTML = `
    <thead>
      <tr style="background:#1a2540;color:#fff;text-align:left">
        <th style="padding:10px 12px;font-weight:600;border-bottom:2px solid #e08c00">Model</th>
        <th style="padding:10px 12px;font-weight:600;border-bottom:2px solid #e08c00">Omschrijving</th>
        <th style="padding:10px 12px;font-weight:600;border-bottom:2px solid #e08c00">Categorie</th>
        <th style="padding:10px 12px;text-align:center;font-weight:600;border-bottom:2px solid #e08c00">Aantal</th>
        <th style="padding:10px 12px;text-align:right;font-weight:600;border-bottom:2px solid #e08c00">DIN (M)</th>
        <th style="padding:10px 12px;text-align:right;font-weight:600;border-bottom:2px solid #e08c00">Vermogen (W)</th>
        <th style="padding:10px 12px;text-align:right;font-weight:600;border-bottom:2px solid #e08c00">Prijs/st (€)</th>
        <th style="padding:10px 12px;text-align:right;font-weight:600;border-bottom:2px solid #e08c00">Totaal (€)</th>
      </tr>
    </thead>
    <tbody>
      ${rows.map((r, i) => `
        <tr style="background:${i % 2 === 0 ? '#fff' : '#f8f9fd'}">
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;font-family:monospace;font-size:11px;color:#6a7899">${r.model}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;color:#1a1f2e">${r.name}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;color:#6a7899;font-size:12px">${r.category}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;text-align:center;font-weight:600;color:#1a1f2e">${r.count}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;text-align:right;color:#6a7899">${r.totalDin > 0 ? r.totalDin.toFixed(1) : '—'}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;text-align:right;color:#6a7899">${r.totalPower > 0 ? r.totalPower : '—'}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;text-align:right;color:#6a7899">${r.priceEur > 0 ? formatEuro(r.priceEur) : '—'}</td>
          <td style="padding:8px 12px;border-bottom:1px solid #e2e8f0;text-align:right;font-weight:600;color:#1a1f2e">${r.totalPrice > 0 ? formatEuro(r.totalPrice) : '—'}</td>
        </tr>
      `).join('')}
    </tbody>
    <tfoot>
      <tr style="background:#fef3e0;border-top:2px solid #e08c00">
        <td colspan="3" style="padding:12px 12px;font-weight:700;color:#1a1f2e">Totaal (${totals.modules} items)</td>
        <td style="padding:12px 12px;text-align:center;font-weight:700;color:#1a1f2e">${totals.modules}</td>
        <td style="padding:12px 12px;text-align:right;font-weight:700;color:#1a1f2e">${totals.din > 0 ? totals.din.toFixed(1) : '—'}</td>
        <td style="padding:12px 12px;text-align:right;font-weight:700;color:#1a1f2e">${totals.power > 0 ? totals.power + ' W' : '—'}</td>
        <td style="padding:12px 12px;text-align:right"></td>
        <td style="padding:12px 12px;text-align:right;font-weight:700;color:#e08c00;font-size:15px">${totals.cost > 0 ? formatEuro(totals.cost) : '—'}</td>
      </tr>
    </tfoot>
  `;
}

/**
 * Format price as Euro currency
 * @param {number} amount 
 * @returns {string}
 */
function formatEuro(amount) {
  return new Intl.NumberFormat('nl-BE', { 
    style: 'currency', 
    currency: 'EUR',
    minimumFractionDigits: 0,
    maximumFractionDigits: 0
  }).format(amount);
}
