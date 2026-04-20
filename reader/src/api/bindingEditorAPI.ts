/**
 * Binding Editor API - High-level API for creating bindings in a user-friendly way
 * This provides simplified endpoints that abstract the complex binding string format
 */

import { Router } from 'express';
import { getActionsForUnitType, INPUT_EVENTS, FunctionCode, InputEventType } from '../models/unitActions.js';
import { BindingManager } from '../services/BindingManager.js';

const router = Router();

/**
 * Represents a binding creation request
 */
interface CreateBindingRequest {
  // Input configuration
  inputNodeAddress: number;
  inputUnitAddress: number;
  inputEvent: InputEventType;
  
  // Output configuration
  outputNodeAddress: number;
  outputUnitAddress: number;
  outputAction: FunctionCode;
  outputActionData?: number;  // Optional data (e.g., dim value, time delay)
  
  // Binding configuration
  flags?: number;            // Binding flags (default: 0x00)
  inverted?: boolean;        // Invert polarity (default: false)
}

/**
 * API: Get available actions for a unit type
 * GET /api/editor/actions/:unitType
 */
router.get('/actions/:unitType', (req, res) => {
  try {
    const { unitType } = req.params;
    const actions = getActionsForUnitType(unitType);
    
    res.json({
      unitType,
      actions: actions.map(action => ({
        code: action.code,
        label: action.label,
        description: action.description,
        requiresData: action.requiresData,
        dataType: action.dataType,
        dataRange: action.dataRange,
      })),
    });
  } catch (error) {
    res.status(500).json({
      error: error instanceof Error ? error.message : 'Failed to get actions',
    });
  }
});

/**
 * API: Get available input events
 * GET /api/editor/events
 */
router.get('/events', (_req, res) => {
  try {
    res.json({
      events: INPUT_EVENTS.map(event => ({
        code: event.code,
        label: event.label,
        description: event.description,
      })),
    });
  } catch (error) {
    res.status(500).json({
      error: error instanceof Error ? error.message : 'Failed to get events',
    });
  }
});

/**
 * API: Create a binding using high-level parameters
 * POST /api/editor/bindings/create
 * 
 * This endpoint takes human-friendly parameters and converts them to the
 * proper binding string format that the Duotecno system understands.
 * 
 * Example request body:
 * {
 *   "inputNodeAddress": 10,       // Node 0x0A
 *   "inputUnitAddress": 5,         // Unit 5 (control button)
 *   "inputEvent": 3,               // Event 0x03 (PULSE)
 *   "outputNodeAddress": 10,       // Node 0x0A
 *   "outputUnitAddress": 2,        // Unit 2 (dimmer)
 *   "outputAction": 16,            // FunctionCode.DIM_ON_OFF_STATUS
 *   "flags": 0,
 *   "inverted": false
 * }
 */
router.post('/bindings/create', (req, res) => {
  try {
    const request: CreateBindingRequest = req.body;
    
    // Validate required fields - check for null, undefined, and NaN
    if (
      request.inputNodeAddress == null ||
      typeof request.inputNodeAddress !== 'number' ||
      isNaN(request.inputNodeAddress) ||
      request.inputUnitAddress == null ||
      typeof request.inputUnitAddress !== 'number' ||
      isNaN(request.inputUnitAddress) ||
      request.inputEvent == null ||
      typeof request.inputEvent !== 'number' ||
      isNaN(request.inputEvent) ||
      request.outputNodeAddress == null ||
      typeof request.outputNodeAddress !== 'number' ||
      isNaN(request.outputNodeAddress) ||
      request.outputUnitAddress == null ||
      typeof request.outputUnitAddress !== 'number' ||
      isNaN(request.outputUnitAddress) ||
      request.outputAction == null ||
      typeof request.outputAction !== 'number' ||
      isNaN(request.outputAction)
    ) {
      return res.status(400).json({
        error: 'Missing or invalid required fields: all addresses, event, and action must be valid numbers',
        received: {
          inputNodeAddress: request.inputNodeAddress,
          inputUnitAddress: request.inputUnitAddress,
          inputEvent: request.inputEvent,
          outputNodeAddress: request.outputNodeAddress,
          outputUnitAddress: request.outputUnitAddress,
          outputAction: request.outputAction,
        },
      });
    }
    
    // Generate binding string
    const bindingString = generateBindingString(request);
    
    return res.json({
      success: true,
      bindingString,
      preview: {
        input: `Node ${request.inputNodeAddress.toString(16).padStart(2, '0').toUpperCase()}, Unit ${request.inputUnitAddress}, Event ${request.inputEvent}`,
        output: `Node ${request.outputNodeAddress.toString(16).padStart(2, '0').toUpperCase()}, Unit ${request.outputUnitAddress}, Action ${request.outputAction}`,
      },
    });
  } catch (error) {
    return res.status(500).json({
      error: error instanceof Error ? error.message : 'Failed to create binding',
    });
  }
});

/**
 * API: Create and add a binding in one request
 * POST /api/editor/bindings/add
 * 
 * Combines binding creation with adding it to a node's binding list
 */
router.post('/bindings/add', (req, res) => {
  try {
    const { targetNodeAddress, binding, bindingManager } = req.body;
    
    if (!bindingManager) {
      return res.status(500).json({
        error: 'BindingManager not available in request context',
      });
    }
    
    // Generate binding string
    const bindingString = generateBindingString(binding as CreateBindingRequest);
    
    // Add to binding manager
    const nodeAddr = parseInt(targetNodeAddress, 16);
    const success = (bindingManager as BindingManager).addBinding(nodeAddr, bindingString);
    
    if (!success) {
      return res.status(404).json({ error: 'Target node not found' });
    }
    
    const state = (bindingManager as BindingManager).getState(nodeAddr);
    
    return res.json({
      success: true,
      bindingString,
      state,
    });
  } catch (error) {
    return res.status(500).json({
      error: error instanceof Error ? error.message : 'Failed to add binding',
    });
  }
});

/**
 * Generate a binding string from high-level parameters
 * 
 * Format for Immediate binding (Type I):
 * 0000[ReturnNode]_[Flags]_[BindingNr]_IU[ReturnUnit]E[Event]>A0000[ForwardNode]U[ForwardUnit]F[MessageCode]D[DataLen][Data]S
 * 
 * Example output: 
 * 00000A_00_0001_IU05E03>A00000AU02F1000S
 */
function generateBindingString(request: CreateBindingRequest): string {
  const {
    inputNodeAddress,
    inputUnitAddress,
    inputEvent,
    outputNodeAddress,
    outputUnitAddress,
    outputAction,
    outputActionData,
    flags = 0x00,
    inverted = false,
  } = request;
  
  // Validate all required fields are numbers
  if (typeof inputNodeAddress !== 'number' || isNaN(inputNodeAddress)) {
    throw new Error(`Invalid inputNodeAddress: ${inputNodeAddress}`);
  }
  if (typeof inputUnitAddress !== 'number' || isNaN(inputUnitAddress)) {
    throw new Error(`Invalid inputUnitAddress: ${inputUnitAddress}`);
  }
  if (typeof inputEvent !== 'number' || isNaN(inputEvent)) {
    throw new Error(`Invalid inputEvent: ${inputEvent}`);
  }
  if (typeof outputNodeAddress !== 'number' || isNaN(outputNodeAddress)) {
    throw new Error(`Invalid outputNodeAddress: ${outputNodeAddress}`);
  }
  if (typeof outputUnitAddress !== 'number' || isNaN(outputUnitAddress)) {
    throw new Error(`Invalid outputUnitAddress: ${outputUnitAddress}`);
  }
  if (typeof outputAction !== 'number' || isNaN(outputAction)) {
    throw new Error(`Invalid outputAction: ${outputAction}`);
  }
  
  // Format node addresses as hex (6 chars, padded with 0000 prefix)
  const inputNodeHex = `0000${inputNodeAddress.toString(16).padStart(2, '0')}`.toUpperCase();
  const outputNodeHex = `0000${outputNodeAddress.toString(16).padStart(2, '0')}`.toUpperCase();
  
  // Flags: 0x80 = normal polarity, 0x00 = inverted
  const flagsHex = (inverted ? 0x00 : flags).toString(16).padStart(2, '0').toUpperCase();
  
  // Binding number - we'll use a placeholder, the BindingManager should assign proper numbers
  const bindingNr = '0000';
  
  // Format unit addresses
  const inputUnitHex = inputUnitAddress.toString(16).padStart(2, '0').toUpperCase();
  const outputUnitHex = outputUnitAddress.toString(16).padStart(2, '0').toUpperCase();
  
  // Format event code
  const eventHex = inputEvent.toString(16).padStart(2, '0').toUpperCase();
  
  // Format function/action code
  const actionHex = outputAction.toString(16).padStart(2, '0').toUpperCase();
  
  // Format data (if provided)
  let dataSection = 'D00'; // Default: no data (length = 00)
  if (outputActionData != null) {  // Check for both null and undefined
    const dataHex = outputActionData.toString(16).padStart(2, '0').toUpperCase();
    const dataLength = '01'; // 1 byte of data
    dataSection = `D${dataLength}${dataHex}`;
  }
  
  // Build the complete binding string (Immediate type)
  const bindingString = [
    inputNodeHex,
    '_',
    flagsHex,
    '_',
    bindingNr,
    '_',
    'I',                    // Immediate binding type
    'U', inputUnitHex,      // Input unit
    'E', eventHex,          // Event code
    '>',                    // Direction marker
    'A', outputNodeHex,     // Output address
    'U', outputUnitHex,     // Output unit
    'F', actionHex,         // Function code
    dataSection,            // Data (if any)
    'S',                    // End marker
  ].join('');
  
  return bindingString;
}

/**
 * Parse a binding string into high-level components
 * This is the reverse operation of generateBindingString
 */
function parseBindingString(bindingString: string): CreateBindingRequest | null {
  try {
    // Example: 00000A_00_0001_IU05E03>A00000AU02F1000S
    const regex = /^0000([0-9A-F]{2})_([0-9A-F]{2})_([0-9A-F]{4})_IU([0-9A-F]{2})E([0-9A-F]{2})>A0000([0-9A-F]{2})U([0-9A-F]{2})F([0-9A-F]{2})D([0-9A-F]{2})([0-9A-F]*)S$/i;
    
    const match = bindingString.match(regex);
    if (!match) return null;
    
    const [
      _,
      inputNode,
      flags,
      _bindingNr,
      inputUnit,
      event,
      outputNode,
      outputUnit,
      action,
      dataLength,
      dataBytes,
    ] = match;
    
    const request: CreateBindingRequest = {
      inputNodeAddress: parseInt(inputNode, 16),
      inputUnitAddress: parseInt(inputUnit, 16),
      inputEvent: parseInt(event, 16),
      outputNodeAddress: parseInt(outputNode, 16),
      outputUnitAddress: parseInt(outputUnit, 16),
      outputAction: parseInt(action, 16),
      flags: parseInt(flags, 16),
      inverted: parseInt(flags, 16) === 0x00,
    };
    
    // Parse data if present
    const dataLen = parseInt(dataLength, 16);
    if (dataLen > 0 && dataBytes) {
      request.outputActionData = parseInt(dataBytes.substring(0, 2), 16);
    }
    
    return request;
  } catch (error) {
    console.error('Failed to parse binding string:', error);
    return null;
  }
}

/**
 * API: Parse a binding string
 * POST /api/editor/bindings/parse
 * Body: { bindingString: "..." }
 */
router.post('/bindings/parse', (req, res) => {
  try {
    const { bindingString } = req.body;
    
    if (!bindingString) {
      return res.status(400).json({ error: 'bindingString required' });
    }
    
    const parsed = parseBindingString(bindingString);
    
    if (!parsed) {
      return res.status(400).json({ error: 'Invalid binding string format' });
    }
    
    return res.json({
      success: true,
      binding: parsed,
    });
  } catch (error) {
    return res.status(500).json({
      error: error instanceof Error ? error.message : 'Failed to parse binding',
    });
  }
});

export default router;
export { CreateBindingRequest, generateBindingString, parseBindingString };
