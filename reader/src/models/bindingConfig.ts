/**
 * Binding configuration info models
 * Based on bindingconfiginfo.txt format
 */

/**
 * Input unit in a binding (from bindingconfiginfo.txt)
 */
export interface BindingInput {
  id: number;
  name: string;
  address: number[];  // [nodeAddr, unitAddr]
  event: number;      // Event code
  type: number;       // Unit type
}

/**
 * Output unit in a binding (from bindingconfiginfo.txt)
 */
export interface BindingOutput {
  id: number;
  address: number[];  // [nodeAddr, unitAddr]
  name: string;
  type: number;       // Unit type
  data: string;       // Function data (hex string)
}

/**
 * Timer in a binding (from bindingconfiginfo.txt)
 */
export interface BindingTimer {
  id: number;
  name: string;
  type: string;       // Timer type
  data: string;       // Timer data (hex string)
}

/**
 * Binding configuration metadata (human-readable)
 */
export interface BindingConfigInfo {
  index: number;
  bindingNumber: number;    // Binding number (hex)
  bindingFlags: number;     // Binding flags (hex)
  evaluationString: string; // e.g., '(U1*U2)=U3'
  name: string;            // Human-readable name
  inputs: BindingInput[];
  outputs: BindingOutput[];
  timers?: BindingTimer[];
}

/**
 * Complete binding configuration info database
 */
export interface BindingConfigDatabase {
  bindings: BindingConfigInfo[];
}
