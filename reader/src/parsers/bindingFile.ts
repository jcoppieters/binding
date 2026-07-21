/**
 * Parser for bind*.txt files
 * Parses the low-level binding protocol format
 */

import { readFileSync } from 'fs';
import {
  BindingEntry,
  BindingFile,
  BindingType,
  UnitReference,
  BindingOperator,
} from '../models/binding.js';

export class BindingFileParser {
  /**
   * Parse a binding file from disk
   */
  static parse(filePath: string): BindingFile {
    const content = readFileSync(filePath, 'utf-8');
    const fileName = filePath.split('/').pop() || '';
    return this.parseFromContent(content, fileName);
  }

  /**
   * Parse binding content from string (for in-memory parsing)
   */
  static parseFromContent(content: string, fileName: string): BindingFile {
    const lines = content.split('\n').filter(line => line.trim());
    const nodeAddress = this.extractNodeAddressFromFileName(fileName);
    
    const entries: BindingEntry[] = [];
    const errors: string[] = [];
    
    for (let i = 0; i < lines.length; i++) {
      const line = lines[i].trim();
      if (!line) continue;
      
      try {
        const entry = this.parseBindingEntry(line);
        entries.push(entry);
      } catch (error) {
        const errorMsg = `Line ${i + 1}: ${error instanceof Error ? error.message : String(error)}`;
        errors.push(errorMsg);
        console.warn(`Error parsing binding: ${errorMsg}`);
      }
    }
    
    return {
      nodeAddress,
      fileName,
      entries,
      errorCount: errors.length,
      errors,
    };
  }
  
  /**
   * Extract node address from filename (e.g., "bindfc.txt" -> 0xFC)
   */
  static extractNodeAddressFromFileName(fileName: string): number {
    const match = fileName.match(/bind([0-9a-fA-F]{2})\.txt/);
    if (!match) {
      throw new Error(`Invalid binding filename: ${fileName}`);
    }
    return parseInt(match[1], 16);
  }
  
  /**
   * Parse a single binding entry line
   * Format: [NodeAddress]_[Flags]_[BindingNumber]_[Type][Content]
   */
  static parseBindingEntry(line: string): BindingEntry {
    // Expected format: 0000XX_XX_XXXX_[Type][Content]
    const parts = line.split('_');
    
    if (parts.length < 4) {
      throw new Error(`Invalid binding format: ${line}`);
    }
    
    // Parse header
    const nodeAddress = parseInt(parts[0].substring(4, 6), 16);
    const bindingFlags = parseInt(parts[1], 16);
    const bindingNumber = parseInt(parts[2], 16);
    
    // Type and content are in the 4th part (after 3rd underscore)
    const typeAndContent = parts.slice(3).join('_');
    
    // Determine binding type
    let bindingType: BindingType;
    let content: string;
    
    if (typeAndContent.startsWith('Te')) {
      bindingType = BindingType.TIMER_EXTENDED;
      content = typeAndContent.substring(2);
    } else if (typeAndContent.startsWith('Ti')) {
      bindingType = BindingType.TIMER_INTERVAL;
      content = typeAndContent.substring(2);
    } else if (typeAndContent.startsWith('To')) {
      bindingType = BindingType.TIMER_ONESHOT;
      content = typeAndContent.substring(2);
    } else if (typeAndContent.startsWith('Td')) {
      bindingType = BindingType.TIMER_DELAYED;
      content = typeAndContent.substring(2);
    } else if (typeAndContent.startsWith('Tf')) {
      bindingType = BindingType.TIMER_FLASHING;
      content = typeAndContent.substring(2);
    } else if (typeAndContent.startsWith('Tr')) {
      bindingType = BindingType.TIMER_REPEAT;
      content = typeAndContent.substring(2);
    } else {
      const typeChar = typeAndContent[0];
      bindingType = typeChar as BindingType;
      content = typeAndContent.substring(1);
    }
    
    const entry: BindingEntry = {
      nodeAddress,
      bindingFlags,
      bindingNumber,
      bindingType,
      content,
    };
    
    // Parse content based on type
    try {
      this.parseContent(entry);
    } catch (error) {
      console.warn(`Warning: Could not parse content for binding ${bindingNumber.toString(16)}: ${error}`);
    }
    
    return entry;
  }
  
  /**
   * Parse binding content based on type
   */
  static parseContent(entry: BindingEntry): void {
    switch (entry.bindingType) {
      case BindingType.NORMAL:
        this.parseNormalBinding(entry);
        break;
      case BindingType.IMMEDIATE:
        this.parseImmediateBinding(entry);
        break;
      case BindingType.CONDITIONAL:
        this.parseConditionalBinding(entry);
        break;
      case BindingType.GROUP:
        this.parseGroupBinding(entry);
        break;
      case BindingType.BLOCK:
        this.parseBlockBinding(entry);
        break;
      default:
        // For timer types and others, store raw content
        entry.expression = entry.content;
    }
  }
  
  /**
   * Parse Normal binding: NA0000[NodeAddress]U[Unit]=[Unit]F[Function]D[Data]S
   */
  static parseNormalBinding(entry: BindingEntry): void {
    const content = entry.content;
    
    // Extract return unit (after N and before =)
    const equalsIndex = content.indexOf('=');
    if (equalsIndex === -1) {
      throw new Error('Normal binding missing = operator');
    }
    
    const returnPart = content; // Don't skip 'N', it's already removed
    const forwardPart = content.substring(equalsIndex + 1);
    
    // Check if starts with 'A' (address marker)
    if (returnPart[0] === 'A') {
      // Parse return unit: A0000XXUxx
      const returnUnit = this.parseUnitReference(returnPart.substring(0, equalsIndex));
      entry.inputUnits = [returnUnit];
    }
    
    // Parse forward part: UxxFxxD[data]S
    const outputUnit = this.parseOutputUnit(forwardPart);
    entry.outputUnits = [outputUnit];
  }
  
  /**
   * Parse Immediate binding: IU[Unit]E[Event]>A0000[Node]U[Unit]F[Function]D[Data]S
   */
  static parseImmediateBinding(entry: BindingEntry): void {
    const content = entry.content;
    
    const arrowIndex = content.indexOf('>');
    if (arrowIndex === -1) {
      throw new Error('Immediate binding missing > operator');
    }
    
    // Parse input: UxxExx (I is already removed)
    const inputPart = content.substring(0, arrowIndex);
    const input = this.parseInputUnit(inputPart, entry.nodeAddress);
    entry.inputUnits = [input];
    
    // Parse output: A0000xxUxxFxxD[data]S
    const outputPart = content.substring(arrowIndex + 1);
    const output = this.parseUnitReference(outputPart);
    entry.outputUnits = [output];
  }
  
  /**
   * Parse Conditional binding: C([expression])=[output]
   */
  static parseConditionalBinding(entry: BindingEntry): void {
    const content = entry.content;
    
    if (!content.startsWith('(')) {
      throw new Error('Conditional binding must start with (');
    }
    
    const equalsIndex = content.indexOf(')=');
    if (equalsIndex === -1) {
      throw new Error('Conditional binding missing )= operator');
    }
    
    const expression = content.substring(1, equalsIndex); // Between ( and )=
    const outputPart = content.substring(equalsIndex + 2);
    
    entry.expression = expression;
    
    // Parse inputs and operators from expression
    const { units, operators } = this.parseExpression(expression);
    entry.inputUnits = units;
    entry.operators = operators;
    
    // Parse output
    const output = this.parseOutputUnit(outputPart);
    entry.outputUnits = [output];
  }
  
  /**
   * Parse Group binding: GA0000[Node]U[Unit]E[Event]=[Unit]F[Function]D[Data]S
   */
  static parseGroupBinding(entry: BindingEntry): void {
    const content = entry.content;
    
    const equalsIndex = content.indexOf('=');
    if (equalsIndex === -1) {
      throw new Error('Group binding missing = operator');
    }
    
    const inputPart = content.substring(0, equalsIndex); // G is already removed
    const outputPart = content.substring(equalsIndex + 1);
    
    const input = this.parseUnitReference(inputPart);
    entry.inputUnits = [input];
    
    const output = this.parseOutputUnit(outputPart);
    entry.outputUnits = [output];
  }
  
  /**
   * Parse Block binding: BU[Unit]E[Event]>A0000[Node]U[Unit]S
   */
  static parseBlockBinding(entry: BindingEntry): void {
    const content = entry.content;
    
    const arrowIndex = content.indexOf('>');
    if (arrowIndex === -1) {
      throw new Error('Block binding missing > operator');
    }
    
    const inputPart = content.substring(0, arrowIndex); // B is already removed
    const outputPart = content.substring(arrowIndex + 1);
    
    const input = this.parseInputUnit(inputPart, entry.nodeAddress);
    entry.inputUnits = [input];
    
    const output = this.parseUnitReference(outputPart);
    entry.outputUnits = [output];
  }
  
  /**
   * Parse unit reference: A0000[Node]U[Unit]E[Event][Char] or A0000[Node]U[Unit]F... or A0000[Node]U[Unit]S
   */
  static parseUnitReference(text: string): UnitReference {
    const aIndex = text.indexOf('A');
    if (aIndex === -1) {
      throw new Error(`Unit reference missing A marker: ${text}`);
    }
    
    const uIndex = text.indexOf('U', aIndex);
    if (uIndex === -1) {
      throw new Error(`Unit reference missing U marker: ${text}`);
    }
    
    const nodeAddress = parseInt(text.substring(aIndex + 5, uIndex), 16);
    
    const eIndex = text.indexOf('E', uIndex);
    let unitAddress: number;
    let event: number | undefined;
    let eventChar: string | undefined;
    let targetMoodUnit: number | undefined;
    
    if (eIndex !== -1) {
      unitAddress = parseInt(text.substring(uIndex + 1, eIndex), 16);
      // Event is 2 hex digits after E
      const eventStr = text.substring(eIndex + 1, eIndex + 3);
      event = parseInt(eventStr, 16);
      // Check if there's an event character after the event code
      if (text.length > eIndex + 3) {
        eventChar = text[eIndex + 3];
      }
    } else {
      // No E marker - parse output unit format: UxxXXXD[data]S
      // where xx = 2-digit unit, XXX = 3-digit function code
      
      // Unit address is always 2 hex digits after U
      unitAddress = parseInt(text.substring(uIndex + 1, uIndex + 3), 16);
      
      // Check for D marker (data)
      const dIndex = text.indexOf('D', uIndex);
      
      if (dIndex !== -1 && dIndex > uIndex + 3) {
        // Function code is 3 hex digits between unit and D marker
        const functionCode = parseInt(text.substring(uIndex + 3, uIndex + 6), 16);
        event = functionCode;
        
        // Special handling for mood trigger function (0xF9F)
        // Data format: D02XXYY where XX is the target mood unit
        if (functionCode === 0xF9F) {
          const dataStr = text.substring(dIndex + 1);
          console.log(`[parser] Found 0xF9F trigger: node=${nodeAddress}, unit=${unitAddress}, data=${dataStr}`);
          // Extract mood unit from data: D02XXYY -> XX
          if (dataStr.length >= 6) {
            targetMoodUnit = parseInt(dataStr.substring(2, 4), 16);
            console.log(`[parser] Extracted targetMoodUnit: ${targetMoodUnit} (0x${targetMoodUnit.toString(16)})`);
          } else {
            console.log(`[parser] WARNING: Data too short, cannot extract mood unit`);
          }
        }
      }
    }
    
    const ref: UnitReference = {
      nodeAddress,
      unitAddress,
      event,
      eventChar,
    };
    
    // Add targetMoodUnit if present
    if (targetMoodUnit !== undefined) {
      (ref as any).targetMoodUnit = targetMoodUnit;
    }
    
    return ref;
  }
  
  /**
   * Parse input unit: UxxExx
   */
  static parseInputUnit(text: string, defaultNodeAddress: number): UnitReference {
    const uIndex = text.indexOf('U');
    const eIndex = text.indexOf('E');
    
    if (uIndex === -1) {
      throw new Error(`Input unit missing U marker: ${text}`);
    }
    
    const unitAddress = parseInt(text.substring(uIndex + 1, eIndex !== -1 ? eIndex : undefined), 16);
    
    let event: number | undefined;
    if (eIndex !== -1) {
      event = parseInt(text.substring(eIndex + 1, eIndex + 3), 16);
    }
    
    return {
      nodeAddress: defaultNodeAddress,
      unitAddress,
      event,
    };
  }
  
  /**
   * Parse output unit: UxxFxxD[len][data]S
   */
  static parseOutputUnit(text: string): UnitReference {
    const uIndex = text.indexOf('U');
    const fIndex = text.indexOf('F');
    
    if (uIndex === -1) {
      throw new Error(`Output unit missing U marker: ${text}`);
    }
    
    const unitAddress = parseInt(text.substring(uIndex + 1, fIndex !== -1 ? fIndex : 3), 16);
    
    const ref: UnitReference = {
      nodeAddress: 0, // Will be set from context
      unitAddress,
    };
    
    // Parse function code if present
    if (fIndex !== -1) {
      const dIndex = text.indexOf('D', fIndex);
      if (dIndex !== -1) {
        ref.event = parseInt(text.substring(fIndex + 1, dIndex), 16);
        
        // Special handling for mood trigger function (0xF9F)
        // Data format: D02XXYY where XX is the target mood unit
        if (ref.event === 0xF9F) {
          const dataStr = text.substring(dIndex + 1);
          console.log(`[parser] Found 0xF9F trigger: node=${ref.nodeAddress}, unit=${ref.unitAddress}, data=${dataStr}`);
          // Extract mood unit from data: D02XXYY -> XX
          if (dataStr.length >= 6) {
            const moodUnit = parseInt(dataStr.substring(2, 4), 16);
            // Store the target mood unit in a special property
            (ref as any).targetMoodUnit = moodUnit;
            console.log(`[parser] Extracted targetMoodUnit: ${moodUnit} (0x${moodUnit.toString(16)})`);
          } else {
            console.log(`[parser] WARNING: Data too short, cannot extract mood unit`);
          }
        }
      } else {
        // Function without data
        ref.event = parseInt(text.substring(fIndex + 1), 16);
      }
    }
    
    return ref;
  }
  
  /**
   * Parse conditional expression with operators
   */
  static parseExpression(expression: string): {
    units: UnitReference[];
    operators: BindingOperator[];
  } {
    const units: UnitReference[] = [];
    const operators: BindingOperator[] = [];
    
    // Find all unit references (A0000xxUxxExx)
    const unitRegex = /A0000([0-9A-Fa-f]{2})U([0-9A-Fa-f]{2})E([0-9A-Fa-f]{2})/g;
    let match;
    
    while ((match = unitRegex.exec(expression)) !== null) {
      units.push({
        nodeAddress: parseInt(match[1], 16),
        unitAddress: parseInt(match[2], 16),
        event: parseInt(match[3], 16),
      });
    }
    
    // Find operators between units
    for (const op of ['+', '*', '^', '!']) {
      if (expression.includes(op)) {
        operators.push(op as BindingOperator);
      }
    }
    
    return { units, operators };
  }
}
