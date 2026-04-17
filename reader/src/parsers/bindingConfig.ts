/**
 * Parser for bindingconfiginfo.txt
 * Contains human-readable binding metadata with names
 */

import { readFileSync } from 'fs';
import {
  BindingConfigInfo,
  BindingConfigDatabase,
  BindingInput,
  BindingOutput,
  BindingTimer,
} from '../models/bindingConfig.js';

export class BindingConfigParser {
  /**
   * Parse bindingconfiginfo.txt file
   */
  static parse(filePath: string): BindingConfigDatabase {
    const content = readFileSync(filePath, 'utf-8');
    const lines = content.split('\n');
    
    const bindings: BindingConfigInfo[] = [];
    let currentBinding: Partial<BindingConfigInfo> | null = null;
    let currentInput: Partial<BindingInput> | null = null;
    let currentOutput: Partial<BindingOutput> | null = null;
    let currentTimer: Partial<BindingTimer> | null = null;
    
    for (const line of lines) {
      const trimmed = line.trim();
      
      if (!trimmed || trimmed.startsWith(';')) {
        continue;
      }
      
      if (trimmed.startsWith('BINDING=')) {
        // Save previous binding
        if (currentBinding) {
          bindings.push(this.finalizeBinding(currentBinding));
        }
        
        currentBinding = {
          index: parseInt(trimmed.split('=')[1]),
          inputs: [],
          outputs: [],
          timers: [],
        };
        currentInput = null;
        currentOutput = null;
        currentTimer = null;
      } else if (trimmed.startsWith('INPUT=')) {
        // Save previous input
        if (currentInput && currentBinding) {
          currentBinding.inputs!.push(this.finalizeInput(currentInput));
        }
        
        currentInput = {
          id: parseInt(trimmed.split('=')[1]),
        };
        currentOutput = null;
        currentTimer = null;
      } else if (trimmed.startsWith('OUTPUT=')) {
        // Save previous output
        if (currentOutput && currentBinding) {
          currentBinding.outputs!.push(this.finalizeOutput(currentOutput));
        }
        
        currentOutput = {
          id: parseInt(trimmed.split('=')[1]),
        };
        currentInput = null;
        currentTimer = null;
      } else if (trimmed.startsWith('TIMER=')) {
        // Save previous timer
        if (currentTimer && currentBinding) {
          currentBinding.timers!.push(this.finalizeTimer(currentTimer));
        }
        
        currentTimer = {
          id: parseInt(trimmed.split('=')[1]),
        };
        currentInput = null;
        currentOutput = null;
      } else if (currentInput) {
        this.parseInputProperty(currentInput, trimmed);
      } else if (currentOutput) {
        this.parseOutputProperty(currentOutput, trimmed);
      } else if (currentTimer) {
        this.parseTimerProperty(currentTimer, trimmed);
      } else if (currentBinding) {
        this.parseBindingProperty(currentBinding, trimmed);
      }
    }
    
    // Save last items
    if (currentInput && currentBinding) {
      currentBinding.inputs!.push(this.finalizeInput(currentInput));
    }
    if (currentOutput && currentBinding) {
      currentBinding.outputs!.push(this.finalizeOutput(currentOutput));
    }
    if (currentTimer && currentBinding) {
      currentBinding.timers!.push(this.finalizeTimer(currentTimer));
    }
    if (currentBinding) {
      bindings.push(this.finalizeBinding(currentBinding));
    }
    
    return { bindings };
  }
  
  static parseBindingProperty(binding: Partial<BindingConfigInfo>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'BindingNR':
        binding.bindingNumber = parseInt(value, 16);
        break;
      case 'BindingFlags':
        binding.bindingFlags = parseInt(value, 16);
        break;
      case 'BindingStrEval':
        binding.evaluationString = value.replace(/^'|'$/g, ''); // Remove quotes
        break;
      case 'BindingStrName':
        binding.name = value.replace(/^'|'$/g, '').replace(/;$/, '');
        break;
    }
  }
  
  static parseInputProperty(input: Partial<BindingInput>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'Id':
        input.id = parseInt(value);
        break;
      case 'Name':
        input.name = value.replace(/^'|'$/g, '').replace(/;$/, '');
        break;
      case 'Address':
        input.address = value.replace(/;$/, '').split(';').map(s => parseInt(s, 16));
        break;
      case 'Event':
        input.event = parseInt(value, 16);
        break;
      case 'Type':
        input.type = parseInt(value);
        break;
    }
  }
  
  static parseOutputProperty(output: Partial<BindingOutput>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'Id':
        output.id = parseInt(value);
        break;
      case 'Address':
        output.address = value.replace(/;$/, '').split(';').map(s => parseInt(s, 16));
        break;
      case 'Name':
        output.name = value.replace(/^'|'$/g, '').replace(/;$/, '');
        break;
      case 'Type':
        output.type = parseInt(value);
        break;
      case 'Data':
        output.data = value.replace(/;$/, '');
        break;
    }
  }
  
  static parseTimerProperty(timer: Partial<BindingTimer>, line: string): void {
    const [key, value] = line.split('=').map(s => s.trim());
    
    switch (key) {
      case 'Id':
        timer.id = parseInt(value);
        break;
      case 'Name':
        timer.name = value.replace(/^'|'$/g, '').replace(/;$/, '');
        break;
      case 'Type':
        timer.type = value.replace(/^'|'$/g, '').replace(/;$/, '');
        break;
      case 'Data':
        timer.data = value.replace(/;$/, '');
        break;
    }
  }
  
  static finalizeBinding(binding: Partial<BindingConfigInfo>): BindingConfigInfo {
    return {
      index: binding.index!,
      bindingNumber: binding.bindingNumber || 0,
      bindingFlags: binding.bindingFlags || 0,
      evaluationString: binding.evaluationString || '',
      name: binding.name || '',
      inputs: binding.inputs || [],
      outputs: binding.outputs || [],
      timers: binding.timers || [],
    };
  }
  
  static finalizeInput(input: Partial<BindingInput>): BindingInput {
    return {
      id: input.id!,
      name: input.name || '',
      address: input.address || [],
      event: input.event || 0,
      type: input.type || 0,
    };
  }
  
  static finalizeOutput(output: Partial<BindingOutput>): BindingOutput {
    return {
      id: output.id!,
      address: output.address || [],
      name: output.name || '',
      type: output.type || 0,
      data: output.data || '',
    };
  }
  
  static finalizeTimer(timer: Partial<BindingTimer>): BindingTimer {
    return {
      id: timer.id!,
      name: timer.name || '',
      type: timer.type || '',
      data: timer.data || '',
    };
  }
}
