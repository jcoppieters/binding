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
 * POST /bindings
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
router.post('/bindings', async (req, res): Promise<void> => {
  try {
    console.log('[import] Request received');
    console.log('[import] Body type:', typeof req.body);
    console.log('[import] Body keys:', req.body ? Object.keys(req.body) : 'null');
    console.log('[import] Files:', req.body?.files ? `${req.body.files.length} files` : 'undefined');
    
    const { files } = req.body;

    if (!files || !Array.isArray(files)) {
      console.error('[import] Invalid request - files is not an array:', files);
      res.status(400).json({ error: 'Invalid request: files array required' });
      return;
    }

    const allBindings = [];
    const errors = [];

    // Parse each file
    for (const file of files) {
      console.log(`[import] Parsing ${file.fileName}...`);
      try {
        const parsed = BindingFileParser.parseFromContent(file.content, file.fileName);
        console.log(`[import] ${file.fileName}: ${parsed.entries.length} bindings, ${parsed.errors.length} errors`);
        
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
        const error = err instanceof Error ? err : new Error(String(err));
        errors.push({ 
          fileName: file.fileName, 
          error: error.message || 'Parsing failed' 
        });
      }
    }

    console.log(`[import] Total bindings parsed: ${allBindings.length}`);
    console.log(`[import] Total errors: ${errors.length}`);

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

    console.log(`[import] Categorized: ${simple.length} simple, ${complex.length} complex`);

    res.json({
      totalBindings: allBindings.length,
      simpleBindings: simple.length,
      complexBindings: complex.length,
      bindings: {
        simple,
        complex
      },
      errors,
      filesParsed: files.length
    });

  } catch (err) {
    console.error('[import] Error:', err);
    const error = err instanceof Error ? err : new Error(String(err));
    res.status(500).json({ error: error.message || 'Import failed' });
  }
});

/**
 * POST /match
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
router.post('/match', async (req, res): Promise<void> => {
  try {
    const { bindings, project } = req.body;

    if (!bindings || !Array.isArray(bindings)) {
      res.status(400).json({ error: 'Invalid request: bindings array required' });
      return;
    }

    if (!project) {
      res.status(400).json({ error: 'Invalid request: project required' });
      return;
    }

    const matched = [];
    const unmatched = [];

    // Build a lookup map for modules by node address
    const modulesByNodeAddress = new Map();
    
    // Add cabinet modules — module shape now lives in project.nodes[],
    // cabinet.modules just holds the placement (nodeAddress + position).
    for (const cabinet of (project.railView?.cabinets || [])) {
      for (const moduleRef of (cabinet.modules || [])) {
        if (moduleRef.nodeAddress != null) {
          const node = (project.nodes || []).find((n: any) => n.nodeAddress === moduleRef.nodeAddress);
          if (!modulesByNodeAddress.has(moduleRef.nodeAddress)) {
            modulesByNodeAddress.set(moduleRef.nodeAddress, []);
          }
          modulesByNodeAddress.get(moduleRef.nodeAddress).push({
            nodeAddress: moduleRef.nodeAddress,
            model: node?.model,
            location: 'cabinet',
            cabinetId: cabinet.id
          });
        }
      }
    }

    // Add house devices — module/device shape now lives in project.nodes[],
    // railView.house just holds the placement (nodeAddress + position).
    for (const houseRef of (project.railView?.house || [])) {
      const node = (project.nodes || []).find((n: any) => n.nodeAddress === houseRef.nodeAddress);
      if (houseRef.nodeAddress != null) {
        if (!modulesByNodeAddress.has(houseRef.nodeAddress)) {
          modulesByNodeAddress.set(houseRef.nodeAddress, []);
        }
        modulesByNodeAddress.get(houseRef.nodeAddress).push({
          nodeAddress: houseRef.nodeAddress,
          model: node?.model,
          location: 'house'
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
          nodeAddress: inputNodeAddr,
          unitAddress: inputUnitAddr,
          model: inputModule.model,
          location: inputModule.location
        },
        outputModule: {
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
    const error = err instanceof Error ? err : new Error(String(err));
    res.status(500).json({ error: error.message || 'Matching failed' });
  }
});

export default router;
