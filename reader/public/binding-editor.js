/**
 * Binding Editor UI Component
 * 
 * This file provides the interactive UI for creating and editing bindings
 * between units in the Duotecno system.
 */

class BindingEditor {
  constructor() {
    this.installation = null;
    this.currentBinding = null;
    this.availableActions = [];
    this.availableEvents = [];
    this.onSaveCallback = null;
    
    this.initializeModal();
    this.loadEvents();
  }

  /**
   * Create the modal HTML structure
   */
  initializeModal() {
    const modalHTML = `
      <div id="binding-editor-modal" class="modal" style="display: none;">
        <div class="modal-backdrop" onclick="bindingEditor.close()"></div>
        <div class="modal-content binding-editor">
          <div class="modal-header">
            <h2>🔗 Create Binding</h2>
            <button class="close-btn" onclick="bindingEditor.close()">✕</button>
          </div>
          
          <div class="modal-body">
            <!-- Step 1: Select Input Unit -->
            <div class="editor-section" id="input-section">
              <div class="section-header">
                <span class="step-number">1</span>
                <h3>Select Input (Trigger)</h3>
              </div>
              <div class="section-content">
                <div class="form-group">
                  <label>Input Node</label>
                  <select id="input-node-select" onchange="bindingEditor.onInputNodeChange()">
                    <option value="">Select a node...</option>
                  </select>
                </div>
                <div class="form-group">
                  <label>Input Unit</label>
                  <select id="input-unit-select" onchange="bindingEditor.onInputUnitChange()">
                    <option value="">Select a unit...</option>
                  </select>
                </div>
                <div class="form-group">
                  <label>Event Type</label>
                  <select id="input-event-select" onchange="bindingEditor.onInputEventChange()">
                    <option value="">Select event...</option>
                  </select>
                  <small class="help-text" id="event-help"></small>
                </div>
              </div>
            </div>

            <!-- Step 2: Select Output Unit -->
            <div class="editor-section" id="output-section">
              <div class="section-header">
                <span class="step-number">2</span>
                <h3>Select Output (Action)</h3>
              </div>
              <div class="section-content">
                <div class="form-group">
                  <label>Output Node</label>
                  <select id="output-node-select" onchange="bindingEditor.onOutputNodeChange()">
                    <option value="">Select a node...</option>
                  </select>
                </div>
                <div class="form-group">
                  <label>Output Unit</label>
                  <select id="output-unit-select" onchange="bindingEditor.onOutputUnitChange()">
                    <option value="">Select a unit...</option>
                  </select>
                </div>
                <div class="form-group">
                  <label>Action</label>
                  <select id="output-action-select" onchange="bindingEditor.onOutputActionChange()">
                    <option value="">Select action...</option>
                  </select>
                  <small class="help-text" id="action-help"></small>
                </div>
                <div class="form-group" id="action-data-group" style="display: none;">
                  <label id="action-data-label">Value</label>
                  <input type="number" id="action-data-input" min="0" max="100" value="50">
                  <small class="help-text" id="data-help"></small>
                </div>
              </div>
            </div>

            <!-- Binding Preview -->
            <div class="editor-section" id="preview-section">
              <div class="section-header">
                <h3>📋 Preview</h3>
              </div>
              <div class="section-content">
                <div class="binding-preview">
                  <div class="preview-flow">
                    <div class="preview-box input-box">
                      <strong>Input:</strong>
                      <span id="preview-input">-</span>
                    </div>
                    <div class="preview-arrow">→</div>
                    <div class="preview-box output-box">
                      <strong>Output:</strong>
                      <span id="preview-output">-</span>
                    </div>
                  </div>
                  <div class="preview-details">
                    <small id="preview-description">Configure input and output to see preview</small>
                  </div>
                </div>
              </div>
            </div>
          </div>
          
          <div class="modal-footer">
            <button class="btn btn-secondary" onclick="bindingEditor.close()">Cancel</button>
            <button class="btn btn-primary" id="save-binding-btn" onclick="bindingEditor.save()" disabled>
              Create Binding
            </button>
          </div>
        </div>
      </div>
    `;
    
    document.body.insertAdjacentHTML('beforeend', modalHTML);
  }

  /**
   * Load available events from API
   */
  async loadEvents() {
    try {
      const response = await fetch('/api/editor/events');
      const data = await response.json();
      this.availableEvents = data.events;
    } catch (error) {
      console.error('Failed to load events:', error);
    }
  }

  /**
   * Open the editor with installation context
   * @param {Object} installation - Installation data
   * @param {Function} onSave - Callback when binding is saved
   * @param {Object} editData - Optional binding data for editing mode
   */
  open(installation, onSave, editData = null) {
    this.installation = installation;
    this.onSaveCallback = onSave;
    this.isEditMode = !!editData;
    
    if (editData && editData.inputUnits && editData.outputUnits) {
      // Edit mode - pre-fill with existing binding data
      const firstInput = editData.inputUnits[0];
      const firstOutput = editData.outputUnits[0];
      
      this.currentBinding = {
        inputNodeAddress: firstInput?.nodeAddress || null,
        inputUnitAddress: firstInput?.unitAddress || null,
        inputEvent: firstInput?.event || null,
        outputNodeAddress: firstOutput?.nodeAddress || null,
        outputUnitAddress: firstOutput?.unitAddress || null,
        outputAction: firstOutput?.action || null,
        outputActionData: firstOutput?.actionData != null ? firstOutput.actionData : null,
      };
      
      // Update modal title
      document.querySelector('#binding-editor-modal .modal-header h2').textContent = '✏️ Edit Binding';
    } else {
      // Create mode
      this.currentBinding = {
        inputNodeAddress: null,
        inputUnitAddress: null,
        inputEvent: null,
        outputNodeAddress: null,
        outputUnitAddress: null,
        outputAction: null,
        outputActionData: null,
      };
      
      // Update modal title
      document.querySelector('#binding-editor-modal .modal-header h2').textContent = '🔗 Create Binding';
    }
    
    this.populateNodeSelects();
    this.populateEventSelect();
    
    // If in edit mode, trigger cascading selections
    if (this.isEditMode) {
      setTimeout(() => this.prefillEditData(), 100);
    }
    
    this.updatePreview();
    
    document.getElementById('binding-editor-modal').style.display = 'flex';
  }
  
  /**
   * Pre-fill form fields in edit mode
   */
  prefillEditData() {
    const inputNodeSelect = document.getElementById('input-node-select');
    const outputNodeSelect = document.getElementById('output-node-select');
    
    if (this.currentBinding.inputNodeAddress != null) {
      inputNodeSelect.value = this.currentBinding.inputNodeAddress;
      this.onInputNodeChange();
      
      setTimeout(() => {
        const inputUnitSelect = document.getElementById('input-unit-select');
        if (this.currentBinding.inputUnitAddress != null) {
          inputUnitSelect.value = this.currentBinding.inputUnitAddress;
          this.onInputUnitChange();
          
          setTimeout(() => {
            const eventSelect = document.getElementById('input-event-select');
            if (this.currentBinding.inputEvent != null) {
              eventSelect.value = this.currentBinding.inputEvent;
              this.onInputEventChange();
            }
          }, 50);
        }
      }, 50);
    }
    
    if (this.currentBinding.outputNodeAddress != null) {
      outputNodeSelect.value = this.currentBinding.outputNodeAddress;
      this.onOutputNodeChange();
      
      setTimeout(() => {
        const outputUnitSelect = document.getElementById('output-unit-select');
        if (this.currentBinding.outputUnitAddress != null) {
          outputUnitSelect.value = this.currentBinding.outputUnitAddress;
          this.onOutputUnitChange();
          
          setTimeout(() => {
            const actionSelect = document.getElementById('output-action-select');
            if (this.currentBinding.outputAction != null) {
              actionSelect.value = this.currentBinding.outputAction;
              this.onOutputActionChange();
              
              setTimeout(() => {
                const dataInput = document.getElementById('action-data-input');
                if (this.currentBinding.outputActionData != null && dataInput) {
                  dataInput.value = this.currentBinding.outputActionData;
                }
              }, 50);
            }
          }, 50);
        }
      }, 50);
    }
  }

  /**
   * Close the editor
   */
  close() {
    document.getElementById('binding-editor-modal').style.display = 'none';
    this.currentBinding = null;
  }

  /**
   * Populate node select dropdowns
   */
  populateNodeSelects() {
    const inputSelect = document.getElementById('input-node-select');
    const outputSelect = document.getElementById('output-node-select');
    
    // Clear existing options
    inputSelect.innerHTML = '<option value="">Select a node...</option>';
    outputSelect.innerHTML = '<option value="">Select a node...</option>';
    
    // Add nodes
    this.installation.nodes.forEach(node => {
      const option = document.createElement('option');
      option.value = node.nodeAddress;
      option.textContent = `${node.name || 'Node ' + node.nodeAddress.toString(16).toUpperCase()}`;
      
      inputSelect.appendChild(option.cloneNode(true));
      outputSelect.appendChild(option);
    });
  }

  /**
   * Populate event select
   */
  populateEventSelect() {
    const select = document.getElementById('input-event-select');
    select.innerHTML = '<option value="">Select event...</option>';
    
    this.availableEvents.forEach(event => {
      const option = document.createElement('option');
      option.value = event.code;
      option.textContent = event.label;
      option.dataset.description = event.description;
      select.appendChild(option);
    });
  }

  /**
   * Handle input node change
   */
  onInputNodeChange() {
    const value = document.getElementById('input-node-select').value;
    if (!value) {
      this.currentBinding.inputNodeAddress = null;
      this.updatePreview();
      return;
    }
    
    const nodeAddr = parseInt(value);
    if (isNaN(nodeAddr)) {
      this.currentBinding.inputNodeAddress = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.inputNodeAddress = nodeAddr;
    const node = this.installation.nodes.find(n => n.nodeAddress === nodeAddr);
    
    // Populate input units (typically control/input units)
    const select = document.getElementById('input-unit-select');
    select.innerHTML = '<option value="">Select a unit...</option>';
    
    if (node && node.units) {
      node.units
        .filter(unit => this.isInputUnit(unit))
        .forEach(unit => {
          const option = document.createElement('option');
          option.value = unit.address[1];
          option.textContent = `${unit.name} (${unit.unitType})`;
          select.appendChild(option);
        });
    }
    
    this.updatePreview();
  }

  /**
   * Handle input unit change
   */
  onInputUnitChange() {
    const value = document.getElementById('input-unit-select').value;
    if (value === '') {
      this.currentBinding.inputUnitAddress = null;
      this.updatePreview();
      return;
    }
    
    const unitAddr = parseInt(value);
    if (isNaN(unitAddr)) {
      this.currentBinding.inputUnitAddress = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.inputUnitAddress = unitAddr;
    this.updatePreview();
  }

  /**
   * Handle input event change
   */
  onInputEventChange() {
    const value = document.getElementById('input-event-select').value;
    if (!value) {
      this.currentBinding.inputEvent = null;
      this.updatePreview();
      return;
    }
    
    const eventCode = parseInt(value);
    if (isNaN(eventCode)) {
      this.currentBinding.inputEvent = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.inputEvent = eventCode;
    
    // Show event description
    const option = document.getElementById('input-event-select').selectedOptions[0];
    if (option && option.dataset.description) {
      document.getElementById('event-help').textContent = option.dataset.description;
    }
    
    this.updatePreview();
  }

  /**
   * Handle output node change
   */
  onOutputNodeChange() {
    const value = document.getElementById('output-node-select').value;
    if (!value) {
      this.currentBinding.outputNodeAddress = null;
      this.updatePreview();
      return;
    }
    
    const nodeAddr = parseInt(value);
    if (isNaN(nodeAddr)) {
      this.currentBinding.outputNodeAddress = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.outputNodeAddress = nodeAddr;
    const node = this.installation.nodes.find(n => n.nodeAddress === nodeAddr);
    
    // Populate output units (units that can be controlled)
    const select = document.getElementById('output-unit-select');
    select.innerHTML = '<option value="">Select a unit...</option>';
    
    if (node && node.units) {
      node.units
        .filter(unit => this.isOutputUnit(unit))
        .forEach(unit => {
          const option = document.createElement('option');
          option.value = unit.address[1];
          option.textContent = `${unit.name} (${unit.unitType})`;
          option.dataset.unitType = unit.unitType;
          select.appendChild(option);
        });
    }
    
    this.updatePreview();
  }

  /**
   * Handle output unit change
   */
  async onOutputUnitChange() {
    const value = document.getElementById('output-unit-select').value;
    if (value === '') {
      this.currentBinding.outputUnitAddress = null;
      this.updatePreview();
      return;
    }
    
    const unitAddr = parseInt(value);
    if (isNaN(unitAddr)) {
      this.currentBinding.outputUnitAddress = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.outputUnitAddress = unitAddr;
    
    // Get unit type and load available actions
    const option = document.getElementById('output-unit-select').selectedOptions[0];
    const unitType = option.dataset.unitType;
    
    await this.loadActionsForUnitType(unitType);
    this.updatePreview();
  }

  /**
   * Load available actions for a unit type
   */
  async loadActionsForUnitType(unitType) {
    try {
      const response = await fetch(`/api/editor/actions/${encodeURIComponent(unitType)}`);
      const data = await response.json();
      this.availableActions = data.actions;
      
      // Populate action select
      const select = document.getElementById('output-action-select');
      select.innerHTML = '<option value="">Select action...</option>';
      
      this.availableActions.forEach(action => {
        const option = document.createElement('option');
        option.value = action.code;
        option.textContent = action.label;
        option.dataset.description = action.description;
        option.dataset.requiresData = action.requiresData;
        option.dataset.dataType = action.dataType || 'value';
        option.dataset.dataRange = JSON.stringify(action.dataRange || [0, 100]);
        select.appendChild(option);
      });
    } catch (error) {
      console.error('Failed to load actions:', error);
    }
  }

  /**
   * Handle output action change
   */
  onOutputActionChange() {
    const value = document.getElementById('output-action-select').value;
    if (!value) {
      this.currentBinding.outputAction = null;
      this.updatePreview();
      return;
    }
    
    const actionCode = parseInt(value);
    if (isNaN(actionCode)) {
      this.currentBinding.outputAction = null;
      this.updatePreview();
      return;
    }
    
    this.currentBinding.outputAction = actionCode;
    
    // Show action description
    const option = document.getElementById('output-action-select').selectedOptions[0];
    if (option && option.dataset.description) {
      document.getElementById('action-help').textContent = option.dataset.description;
    }
    
    // Show/hide data input based on requirements
    const requiresData = option.dataset.requiresData === 'true';
    const dataGroup = document.getElementById('action-data-group');
    
    if (requiresData) {
      dataGroup.style.display = 'block';
      const dataType = option.dataset.dataType;
      const dataRange = JSON.parse(option.dataset.dataRange);
      
      const input = document.getElementById('action-data-input');
      input.min = dataRange[0];
      input.max = dataRange[1];
      input.value = Math.round((dataRange[0] + dataRange[1]) / 2);
      
      const label = document.getElementById('action-data-label');
      label.textContent = dataType === 'time' ? 'Time (seconds)' : 'Value (%)';
      
      document.getElementById('data-help').textContent = 
        `Range: ${dataRange[0]} - ${dataRange[1]}`;
    } else {
      dataGroup.style.display = 'none';
      this.currentBinding.outputActionData = null;
    }
    
    this.updatePreview();
  }

  /**
   * Update the binding preview
   */
  updatePreview() {
    const inputPreview = document.getElementById('preview-input');
    const outputPreview = document.getElementById('preview-output');
    const description = document.getElementById('preview-description');
    const saveBtn = document.getElementById('save-binding-btn');
    
    // Input preview
    if (this.currentBinding.inputNodeAddress !== null && 
        this.currentBinding.inputUnitAddress !== null &&
        this.currentBinding.inputEvent !== null) {
      const node = this.installation.nodes.find(n => n.nodeAddress === this.currentBinding.inputNodeAddress);
      const unit = node?.units.find(u => u.address[1] === this.currentBinding.inputUnitAddress);
      const event = this.availableEvents.find(e => e.code === this.currentBinding.inputEvent);
      
      inputPreview.textContent = `${unit?.name || 'Unit'} → ${event?.label || 'Event'}`;
    } else {
      inputPreview.textContent = '-';
    }
    
    // Output preview
    if (this.currentBinding.outputNodeAddress !== null &&
        this.currentBinding.outputUnitAddress !== null &&
        this.currentBinding.outputAction !== null) {
      const node = this.installation.nodes.find(n => n.nodeAddress === this.currentBinding.outputNodeAddress);
      const unit = node?.units.find(u => u.address[1] === this.currentBinding.outputUnitAddress);
      const action = this.availableActions.find(a => a.code === this.currentBinding.outputAction);
      
      outputPreview.textContent = `${unit?.name || 'Unit'} → ${action?.label || 'Action'}`;
    } else {
      outputPreview.textContent = '-';
    }
    
    // Check if binding is complete
    const isComplete = this.currentBinding.inputNodeAddress !== null &&
                       !isNaN(this.currentBinding.inputNodeAddress) &&
                       this.currentBinding.inputUnitAddress !== null &&
                       !isNaN(this.currentBinding.inputUnitAddress) &&
                       this.currentBinding.inputEvent !== null &&
                       !isNaN(this.currentBinding.inputEvent) &&
                       this.currentBinding.outputNodeAddress !== null &&
                       !isNaN(this.currentBinding.outputNodeAddress) &&
                       this.currentBinding.outputUnitAddress !== null &&
                       !isNaN(this.currentBinding.outputUnitAddress) &&
                       this.currentBinding.outputAction !== null &&
                       !isNaN(this.currentBinding.outputAction);
    
    saveBtn.disabled = !isComplete;
    
    if (isComplete) {
      description.textContent = '✅ Binding is ready to be created';
      description.style.color = 'var(--success-color)';
    } else {
      description.textContent = 'Configure input and output to see preview';
      description.style.color = 'var(--text-muted)';
    }
  }

  /**
   * Save the binding
   */
  async save() {
    // Get action data if required
    const actionSelect = document.getElementById('output-action-select');
    const selectedOption = actionSelect.selectedOptions[0];
    const requiresData = selectedOption?.dataset.requiresData === 'true';
    
    if (requiresData) {
      this.currentBinding.outputActionData = parseInt(document.getElementById('action-data-input').value);
    }
    
    console.log('Creating binding with data:', this.currentBinding);
    
    try {
      const response = await fetch('/api/editor/bindings/create', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify(this.currentBinding),
      });
      
      const result = await response.json();
      
      if (result.success) {
        console.log('Binding created:', result.bindingString);
        
        // Call the callback with the binding string
        if (this.onSaveCallback) {
          await this.onSaveCallback(result.bindingString, this.currentBinding);
        }
        
        this.close();
      } else {
        alert('Failed to create binding: ' + (result.error || 'Unknown error'));
      }
    } catch (error) {
      console.error('Failed to save binding:', error);
      alert('Failed to save binding: ' + error.message);
    }
  }

  /**
   * Check if a unit can be used as an input (trigger)
   */
  isInputUnit(unit) {
    const inputTypes = ['control', 'input', 'switch', 'button', 'sensor'];
    return inputTypes.some(type => unit.unitType.toLowerCase().includes(type));
  }

  /**
   * Check if a unit can be used as an output (target)
   */
  isOutputUnit(unit) {
    const outputTypes = ['dimmer', 'switch', 'relais', 'duoswitch', 'shutter', 'blind', 'rolluik'];
    return outputTypes.some(type => unit.unitType.toLowerCase().includes(type));
  }
}

// Create global instance
const bindingEditor = new BindingEditor();
