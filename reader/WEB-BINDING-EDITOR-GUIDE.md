# 🔗 Duotecno Web-Based Binding Editor

## Overview

This guide explains how to use the new web-based binding editor for the Duotecno system. The editor provides a modern, user-friendly interface for creating bindings between units without needing extensive training or manual file editing.

## Key Features

### ✅ What's Implemented

1. **Interactive Binding Creation**
   - Step-by-step wizard for creating bindings
   - Context-aware action lists based on unit types
   - Real-time binding preview
   - Validation and error checking

2. **Unit-Type-Specific Actions**
   - **Relais/Switch**: On/Off (Status), Switch PIR On, Switch On/Off, Switch Off, Switch On
   - **Dimmer**: On/Off (Status), Dim Value/Off, Dim PIR On, Dim Up/Down, Set Dim Value, etc.
   - **Rolluik/Shutter**: Up/Down, Stop, Up, Down, Up/Stop, Down/Stop
   - **Temperature Sensor**: Heating, Cooling

3. **User-Friendly Interface**
   - Modern, responsive design
   - Visual flow representation
   - Contextual help text (descriptions from manual)
   - Pre-selection when opening from a unit view

4. **REST API**
   - High-level endpoints for binding management
   - Automatic binding string generation
   - Validation and error handling

## How to Use

### Starting the Server

```bash
cd reader
npm install
npm run server:enhanced
```

The server will start at `http://localhost:3000`

### Viewing Your Installation

1. **Open your browser** to `http://localhost:3000`
2. **Browse nodes** in the left sidebar
3. **Click a node** to see all its units
4. **Click a unit** to see its bindings

### Creating a New Binding

#### Method 1: From Unit View

1. Navigate to a unit detail page
2. Click the **"➕ Add New Binding"** button
3. The editor will open with the current unit pre-selected as input

#### Method 2: From Scratch

1. Click any unit
2. Click **"Add New Binding"**
3. Follow the wizard:

**Step 1: Select Input (Trigger)**
- Choose the input node (where the button/sensor is)
- Choose the input unit (the control button or sensor)
- Choose the event type (Short Pulse, Toggle, etc.)

**Step 2: Select Output (Action)**
- Choose the output node (where the device to control is)
- Choose the output unit (dimmer, relay, shutter, etc.)
- Choose the action from the context-specific list
- If the action requires data (e.g., dim value), enter it

**Step 3: Preview and Save**
- Review the binding preview
- Click **"Create Binding"**
- The binding is added to the installation

### Understanding Action Types

#### Relais/Switch Actions

| Action | Description | Requires Data |
|--------|-------------|---------------|
| **On/Off (Status)** | Toggle based on current state | No |
| **Switch PIR On** | Activate via PIR sensor with timeout | Yes (time in seconds) |
| **Switch On/Off** | Choose On or Off (always executes chosen state first) | No |
| **Switch Off** | Always turn off | No |
| **Switch On** | Always turn on | No |

#### Dimmer Actions

| Action | Description | Requires Data |
|--------|-------------|---------------|
| **On/Off (Status)** | Toggle dimmer, restore last value | No |
| **Dim Value/Off** | Set to specific value or turn off | Yes (0-100%) |
| **Dim PIR On** | Activate at preset value for preset time | Yes (value %) |
| **Dim Up/Down** | Continuous dimming (hold button) | No |
| **Up/Dn/On/Off** | Combined control, reverses last action | No |
| **Dim Up** | Only brighten | No |
| **Dim Down** | Only darken | No |
| **Set Dim Value** | Always start at specific value | Yes (0-100%) |

#### Shutter/Rolluik Actions

| Action | Description | Requires Data |
|--------|-------------|---------------|
| **Up/Down** | Toggle direction based on last action | No |
| **Stop** | Halt movement | No |
| **Up** | Move up | No |
| **Down** | Move down | No |
| **Up/Stop** | Choose what happens first, then alternate | No |
| **Down/Stop** | Choose what happens first, then alternate | No |

#### Temperature Sensor Actions

| Action | Description | Requires Data |
|--------|-------------|---------------|
| **Heating** | Activate when temp below setpoint | No |
| **Cooling** | Activate when temp above setpoint | No |

### Event Types

When selecting an input, you choose which event triggers the binding:

- **Event Short Pulse**: Brief button press
- **Event Toggle**: Status change (on/off toggle)
- **Event Pulse Toggle**: Combined pulse and toggle

### Saving and Uploading

After creating bindings:

1. **Save to filesystem**: The bindings are saved to the binding files
2. **Upload to nodes**: Use the upload API to send bindings to actual hardware

```bash
# Save all modified bindings
curl -X POST http://localhost:3000/api/bindings/save-all

# Upload changed bindings (smart detection)
curl -X POST http://localhost:3000/api/upload/changed
```

## API Reference

### Binding Editor Endpoints

#### Get Available Actions for Unit Type
```http
GET /api/editor/actions/:unitType
```

Response:
```json
{
  "unitType": "Dimmer",
  "actions": [
    {
      "code": 16,
      "label": "On/Off (Status)",
      "description": "De dimmer wordt geactiveerd...",
      "requiresData": false
    }
  ]
}
```

#### Get Available Events
```http
GET /api/editor/events
```

Response:
```json
{
  "events": [
    {
      "code": 3,
      "label": "Event Short Pulse",
      "description": "Korte puls op de ingang"
    }
  ]
}
```

#### Create Binding
```http
POST /api/editor/bindings/create
Content-Type: application/json

{
  "inputNodeAddress": 10,
  "inputUnitAddress": 5,
  "inputEvent": 3,
  "outputNodeAddress": 10,
  "outputUnitAddress": 2,
  "outputAction": 16,
  "outputActionData": 50,
  "flags": 0,
  "inverted": false
}
```

Response:
```json
{
  "success": true,
  "bindingString": "00000A_00_0000_IU05E03>A00000AU02F1000S",
  "preview": {
    "input": "Node 0A, Unit 5, Event 3",
    "output": "Node 0A, Unit 2, Action 16"
  }
}
```

## Architecture

### File Structure

```
reader/
├── src/
│   ├── api/
│   │   └── bindingEditorAPI.ts     # High-level binding editor API
│   ├── models/
│   │   └── unitActions.ts          # Unit action definitions
│   └── server-enhanced.ts          # Main server with all APIs
├── public/
│   ├── binding-editor.js           # Binding editor UI component
│   ├── binding-editor.css          # Editor styles
│   ├── app.js                      # Main frontend application
│   └── index.html                  # Entry point
```

### Data Flow

```
User Action (UI)
    ↓
Binding Editor Component
    ↓
Create Binding Request (High-level API)
    ↓
Generate Binding String (Format conversion)
    ↓
Binding Manager (Add to node)
    ↓
Save to Filesystem
    ↓
Upload to Hardware (Optional)
```

## Comparison with Old Software

### Old Software
- ❌ Desktop application (Windows only)
- ❌ Complex UI requiring training
- ❌ Manual typing of addresses and codes
- ❌ Limited visual feedback
- ❌ No validation until upload

### New Web-Based Editor
- ✅ Web-based (works anywhere)
- ✅ Intuitive step-by-step wizard
- ✅ Dropdown selections with context-aware options
- ✅ Real-time preview and validation
- ✅ Immediate error feedback
- ✅ Modern, responsive design
- ✅ Pre-selection and smart defaults

## Examples

### Example 1: Connect Button to Light (On/Off)

**Scenario**: Kitchen button (Node 0x0A, Unit 5) controls kitchen light (Node 0x0A, Unit 2)

**Steps**:
1. Open binding editor
2. **Input**: Node 0x0A, Unit 5 (kitchen button), Event "Short Pulse"
3. **Output**: Node 0x0A, Unit 2 (kitchen relay), Action "On/Off (Status)"
4. Preview shows: "kitchen button → Event Short Pulse ⇒ kitchen light → On/Off (Status)"
5. Click "Create Binding"

**Result**: 
```
Binding String: 00000A_00_0000_IU05E03>A00000AU02F0000S
When button is pressed, light toggles based on current state
```

### Example 2: Connect Button to Dimmer (Dim Up/Down)

**Scenario**: Living room button long-press dims the chandelier

**Steps**:
1. **Input**: Node 0x0A, Unit 3 (living button), Event "Pulse Toggle"
2. **Output**: Node 0x0A, Unit 8 (chandelier dimmer), Action "Dim Up/Down"
3. Create binding

**Result**:
```
When button is held down, dimmer brightens or darkens continuously
```

### Example 3: PIR Sensor activates Light at 50%

**Scenario**: Hallway PIR sensor turns on light at 50% for 60 seconds

**Steps**:
1. **Input**: Node 0x15, Unit 1 (PIR sensor), Event "Short Pulse"
2. **Output**: Node 0x15, Unit 2 (hallway dimmer), Action "Dim PIR On"
3. **Data**: Enter 50 (for 50% brightness)
4. Create binding

**Result**:
```
When PIR detects motion, dimmer goes to 50% and stays on for preset time
```

## Troubleshooting

### Actions not showing
- Make sure the output unit type is supported (Dimmer, Switch, Shutter, etc.)
- Check console for API errors

### Binding not saved
- Verify the binding manager has loaded the node's binding file
- Check file permissions on the config directory

### Can't see newly created binding
- Click the reload button to refresh the installation data
- Check `/api/bindings/:nodeAddress` to see if binding was added

## Future Enhancements

- [ ] Visual drag-and-drop binding creator
- [ ] Conditional bindings (AND, OR, XOR operators)
- [ ] Timer-based bindings
- [ ] Binding templates/presets
- [ ] Bulk binding operations
- [ ] Import/export bindings
- [ ] Test bindings before saving
- [ ] Undo/redo functionality

## Development

### Adding New Unit Actions

To add support for a new unit type:

1. Edit `reader/src/models/unitActions.ts`
2. Add function codes to `FunctionCode` enum
3. Create action array (e.g., `MYLIGHTS_ACTIONS`)
4. Add mapping in `UNIT_ACTIONS_MAP`
5. Restart server

### Testing

```bash
# Test the API
curl http://localhost:3000/api/editor/actions/Dimmer

# Test binding creation
curl -X POST http://localhost:3000/api/editor/bindings/create \
  -H "Content-Type: application/json" \
  -d '{
    "inputNodeAddress": 10,
    "inputUnitAddress": 5,
    "inputEvent": 3,
    "outputNodeAddress": 10,
    "outputUnitAddress": 2,
    "outputAction": 16
  }'
```

## Summary

The new web-based binding editor provides a modern, user-friendly alternative to the old desktop software. It automates the complexity of binding string generation while providing context-aware options based on unit types. Users can create bindings through an intuitive wizard without needing to memorize function codes or binding syntax.

**Key Benefits**:
- 🌐 Works in any browser
- 🚀 No installation required
- 📱 Responsive design
- 🎯 Context-aware options
- ✨ Real-time validation
- 📋 Visual preview
