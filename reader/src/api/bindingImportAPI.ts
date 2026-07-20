/**
 * Binding Import API
 * 
 * Handles importing binding files from disk, parsing them,
 * and matching them to existing Rail View devices.
 */

import { Router } from 'express';
import { BindingFileParser } from '../parsers/bindingFile.js';
import { BindingType } from '../models/binding.js';

const router = Router();

/**
 * POST /api/bindings/import
 * 
 * Parse bind*.txt files and categorize bindings by type
 * 
 * Body: {
 *   files: Array<{ fileName: string, content: string }>
 * }
 * 
 * Response: {
 *   totalBindings: number,
 *   simpleBindings: number,
 *   complexBindings: number,
 *   bindings: {
 *     simple: Array<ParsedBinding>,
 *     complex: Array<ParsedBinding>
 *   },
 *   errors: Array<{ fileName: string, error: string }>
 * }
 */
router.post('/import', async (req, res) => {
  try {
    const { files } = req.body;

    if (!files || !Array.isArray(files)) {
      return res.status(400).json({ error: 'Invalid request: files array required' });
    }

    const allBindings = [];
    const errors = [];

    // Parse each file
    for (const file of files) {
      try {
        const parsed = BindingFileParser.parseFromContent(file.content, file.fileName);
        
        // Add fileName and nodeAddress to each binding
        const nodeAddress = parsed.nodeAddress;
        
        for (const binding of parsed.entries) {
          allBindings.push({
            ...binding,
            fileName: file.fileName,
            nodeAddress
          });
        }

        // Collect any parsing errors
        if (parsed.errors && parsed.errors.length > 0) {
          for (const error of parsed.errors) {
            errors.push({ fileName: file.fileName, error });
          }
        }

      } catch (err) {
        console.error(`[import] Failed to parse ${file.fileName}:`, err);
        errors.push({ 
          fileName: file.fileName, 
          error: err.message || 'Parsing failed' 
        });
      }
    }

    // Categorize bindings
    const simple = [];
    const complex = [];

    for (const binding of allBindings) {
      const type = binding.bindingType;
      
      // Simple bindings: Type I (Immediate) and N (Normal)
      if (type === BindingType.IMMEDIATE || type === BindingType.NORMAL) {
        simple.push(binding);
      } else {
        // Complex bindings: C, Td, Tf, Tr, Ti, To, G, P
        complex.push(binding);
      }
    }

    res.json({
      totalBindings: allBindings.length,
      simpleBindings: simple.length,
      complexBindings: complex.length,
      bindings: {
        simple,
        complex
      },
      errors
    });

  } catch (err) {
    console.error('[import] Error:', err);
    res.status(500).json({ error: err.message || 'Import failed' });
  }
});

/**
 * POST /api/bindings/match
 * 
 * Match parsed bindings to existing Rail View devices
 * 
 * Body: {
 *   bindings: Array<ParsedBinding>,
 *   project: Project
 * }
 * 
 * Response: {
 *   matched: Array<MatchedBinding>,
 *   unmatched: Array<UnmatchedBinding>
 * }
 */
router.post('/match', async (req, res) => {
  try {
    const { bindings, project } = req.body;

    if (!bindings || !Array.isArray(bindings)) {
      return res.status(400).json({ error: 'Invalid request: bindings array required' });
    }

    if (!project) {
      return res.status(400).json({ error: 'Invalid request: project required' });
    }

    const matched = [];
    const unmatched = [];

    // Build a lookup map for modules by node address
    const modulesByNodeAddress = new Map();
    
    // Add cabinet modules
    for (const cabinet of (project.railView?.cabinets || [])) {
      for (const module of (cabinet.modules || [])) {
        if (module.nodeAddress != null) {
          if (!modulesByNodeAddress.has(module.nodeAddress)) {
            modulesByNodeAddress.set(module.nodeAddress, []);
          }
          modulesByNodeAddress.get(module.nodeAddress).push({
            ...module,
            location: 'cabinet',
            cabinetId: cabinet.id
          });
        }
      }
    }

    // Add woning devices
    for (const device of (project.railView?.woningDevices || [])) {
      if (device.nodeAddress != null) {
        if (!modulesByNodeAddress.has(device.nodeAddress)) {
          modulesByNodeAddress.set(device.nodeAddress, []);
        }
        modulesByNodeAddress.get(device.nodeAddress).push({
          ...device,
          location: 'woning'
        });
      }
    }

    // Match bindings to modules
    for (const binding of bindings) {
      // Extract node/unit addresses from binding
      // Binding format: A[NodeAddress]U[UnitAddress]E[Event]
      const inputMatch = binding.input?.match(/A([0-9A-F]{6})U([0-9A-F]{2})E([0-9A-F]{2})/i);
      const outputMatch = binding.output?.match(/A([0-9A-F]{6})U([0-9A-F]{2})(?:F([0-9A-F]{2}))?/i);

      if (!inputMatch || !outputMatch) {
        unmatched.push({
          ...binding,
          reason: 'Invalid binding format'
        });
        continue;
      }

      const inputNodeAddr = parseInt(inputMatch[1], 16);
      const inputUnitAddr = parseInt(inputMatch[2], 16);
      const outputNodeAddr = parseInt(outputMatch[1], 16);
      const outputUnitAddr = parseInt(outputMatch[2], 16);

      // Find modules
      const inputModules = modulesByNodeAddress.get(inputNodeAddr) || [];
      const outputModules = modulesByNodeAddress.get(outputNodeAddr) || [];

      if (inputModules.length === 0) {
        unmatched.push({
          ...binding,
          reason: `Input node 0x${inputNodeAddr.toString(16).padStart(2, '0').toUpperCase()} not found in Rail View`
        });
        continue;
      }

      if (outputModules.length === 0) {
        unmatched.push({
          ...binding,
          reason: `Output node 0x${outputNodeAddr.toString(16).padStart(2, '0').toUpperCase()} not found in Rail View`
        });
        continue;
      }

      // For now, just take the first matching module
      // TODO: More sophisticated matching based on unit address and channel type
      const inputModule = inputModules[0];
      const outputModule = outputModules[0];

      matched.push({
        ...binding,
        inputModule: {
          moduleInstanceId: inputModule.id,
          nodeAddress: inputNodeAddr,
          unitAddress: inputUnitAddr,
          model: inputModule.model,
          location: inputModule.location
        },
        outputModule: {
          moduleInstanceId: outputModule.id,
          nodeAddress: outputNodeAddr,
          unitAddress: outputUnitAddr,
          model: outputModule.model,
          location: outputModule.location
        }
      });
    }

    res.json({
      matched,
      unmatched
    });

  } catch (err) {
    console.error('[match] Error:', err);
    res.status(500).json({ error: err.message || 'Matching failed' });
  }
});

export default router;
