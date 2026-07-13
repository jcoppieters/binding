/**
 * Project state — single source of truth for the app.
 *
 * Usage:
 *   import { state, dispatch } from './state.js';
 *   dispatch({ type: 'SET_PROJECT', project: { ... } });
 *   state.subscribe(s => console.log(s.project));
 */

import { NodeType, hasCabinetUnits } from './unit-types.js';

const EMPTY_PROJECT = {
  meta: { name: 'Nieuw project', created: new Date().toISOString(), modified: new Date().toISOString(), version: '1' },
  railView: {
    cabinets: [{
      id: 'cabinet-0',
      name: 'Hoofdkast',
      widthUnits: 36,
      modules: [],
    }],
    woningDevices: [],
  },
  homeView: { floors: [{ id: 'floor-0', name: 'Gelijkvloers' }], rooms: [] },
  bindings: [],
  discoveredNodes: [], // Saved from last connection: nodes with units and their states
};

let _state = {
  project: EMPTY_PROJECT,
  modules: [],         // loaded from /api/modules
  activeView: 'rail',  // 'rail' | 'home' | 'mat'
  dirty: false,        // unsaved changes
  connected: false,    // TCP connection to master
  masterIp: null,
  discoveredNodes: [], // runtime-only: DiscoveredNode[] after TCP discovery
};

const _listeners = new Set();

function notify() {
  _listeners.forEach(fn => fn({ ..._state }));
}

export const state = {
  get() { return { ..._state }; },
  subscribe(fn) {
    _listeners.add(fn);
    fn({ ..._state }); // immediate call with current state
    return () => _listeners.delete(fn);
  },
};

export function dispatch(action) {
  switch (action.type) {
    case 'SET_PROJECT':
      _state = { 
        ..._state, 
        project: action.project, 
        dirty: false,
        // Restore discovered nodes from project file (if any)
        discoveredNodes: action.project.discoveredNodes ?? [],
      };
      break;
    case 'SET_MODULES':
      _state = { ..._state, modules: action.modules };
      break;
    case 'SET_VIEW':
      _state = { ..._state, activeView: action.view };
      break;
    case 'SET_DIRTY':
      _state = { ..._state, dirty: action.dirty ?? true };
      break;
    case 'SET_CONNECTION':
      _state = {
        ..._state,
        connected: action.connected,
        masterIp: action.ip ?? _state.masterIp,
        discoveredNodes: action.nodes ?? _state.discoveredNodes,
      };
      break;
    case 'SET_MASTER_CONFIG': {
      const meta = { ..._state.project.meta, masterIp: action.masterIp };
      if (action.masterPassword !== undefined) meta.masterPassword = action.masterPassword;
      if (action.masterPort !== undefined) meta.masterPort = action.masterPort;
      _state = { ..._state, dirty: true, project: { ..._state.project, meta } };
      break;
    }
    case 'ADD_DISCOVERED_NODES': {
      // Classify: CONTROL/LCD_VIRTUAL non-master nodes go to woning; rest to cabinet
      const cabinetNodes = [];
      const woningNodes = [];
      // NodeType constants (from Duotecno protocol)
      // 1=Standard, 4=Gateway(TCP server), 8=Modem(config only), 32=GUI(LCD touchscreen)
      for (const node of action.nodes) {
        // NodeType 32 (kGUINode) = LCD/touchscreen field device -> always woning
        if (node.type === NodeType.kGUINode)  { woningNodes.push(node); continue; }
        // NodeType 8 (kModemNode) = config modem -> always cabinet
        if (node.type === NodeType.kModemNode){ cabinetNodes.push(node); continue; }
        // NodeType 4 (kGatewayNode) = TCP server / master -> always cabinet
        if (node.type === NodeType.kGatewayNode){ cabinetNodes.push(node); continue; }
        // For Standard (1): cabinet if any output hardware present
        const ctrlCount = (node.units ?? []).filter(u => u.type === 3).length; // kInput
        const moodCount = (node.units ?? []).filter(u => u.type === 7).length; // kMood
        if (!hasCabinetUnits(node.units) && (ctrlCount > 0 || moodCount > 0)) {
          woningNodes.push(node);
        } else {
          cabinetNodes.push(node);
        }
      }
      const railView = { ..._state.project.railView };
      // Cabinet modules
      if (cabinetNodes.length > 0) {
        let cabinets = [...railView.cabinets];
        if (cabinets.length === 0) cabinets.push({ id: makeId(), name: 'Hoofdkast', widthUnits: 36, modules: [] });
        const last = { ...cabinets[cabinets.length - 1], modules: [...cabinets[cabinets.length - 1].modules] };
        for (const node of cabinetNodes) {
          last.modules.push({ id: makeId(), model: 'UNKNOWN', nodeAddress: node.nodeAddress, physicalAddress: node.physicalAddress, name: node.name || null, position: last.modules.length });
        }
        cabinets[cabinets.length - 1] = last;
        railView.cabinets = cabinets;
      }
      // Woning devices
      if (woningNodes.length > 0) {
        railView.woningDevices = [
          ...railView.woningDevices,
          ...woningNodes.map(node => ({ id: makeId(), model: 'UNKNOWN', nodeAddress: node.nodeAddress, name: node.name || null, position: 99 }))
        ];
      }
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'SET_PROJECT_NAME':
      _state = {
        ..._state,
        dirty: true,
        project: { ..._state.project, meta: { ..._state.project.meta, name: action.name, modified: new Date().toISOString() } },
      };
      break;
    case 'ADD_CABINET': {
      const railView = { ..._state.project.railView };
      railView.cabinets = [...railView.cabinets, action.cabinet];
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'SET_CABINET': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? { ...c, ...action.patch } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_MODULE': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId
          ? { ...c, modules: [...c.modules, action.module] }
          : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'UPDATE_MODULE': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? {
          ...c, modules: c.modules.map(m =>
            m.id === action.moduleId ? { ...m, ...action.patch } : m
          )
        } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'REMOVE_MODULE': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId
          ? { ...c, modules: c.modules.filter(m => m.id !== action.moduleId) }
          : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'MOVE_MODULE': {
      // Swap module at index with its neighbour (direction: -1=left, +1=right)
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c => {
        if (c.id !== action.cabinetId) return c;
        const mods = [...c.modules];
        const idx = mods.findIndex(m => m.id === action.moduleId);
        const target = idx + action.direction;
        if (idx < 0 || target < 0 || target >= mods.length) return c;
        [mods[idx], mods[target]] = [mods[target], mods[idx]];
        return { ...c, modules: mods };
      });
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_WONING_DEVICE': {
      const railView = { ..._state.project.railView };
      railView.woningDevices = [...railView.woningDevices, action.device];
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'REMOVE_WONING_DEVICE': {
      const railView = { ..._state.project.railView };
      railView.woningDevices = railView.woningDevices.filter(d => d.id !== action.deviceId);
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'UPDATE_WONING_DEVICE': {
      const railView = { ..._state.project.railView };
      railView.woningDevices = railView.woningDevices.map(d =>
        d.id === action.deviceId ? { ...d, ...action.patch } : d
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'MOVE_WONING_DEVICE': {
      const railView = { ..._state.project.railView };
      const devs = [...railView.woningDevices];
      const idx = devs.findIndex(d => d.id === action.deviceId);
      const target = idx + action.direction;
      if (idx >= 0 && target >= 0 && target < devs.length) {
        [devs[idx], devs[target]] = [devs[target], devs[idx]];
        railView.woningDevices = devs;
      }
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_ROOM': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = [...homeView.rooms, action.room];
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'UPDATE_ROOM': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.map(r =>
        r.id === action.roomId ? { ...r, ...action.patch } : r
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'REMOVE_ROOM': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.filter(r => r.id !== action.roomId);
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'ADD_FLOOR': {
      const homeView = { ..._state.project.homeView };
      homeView.floors = [...homeView.floors, action.floor];
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'UPDATE_FLOOR': {
      const homeView = { ..._state.project.homeView };
      homeView.floors = homeView.floors.map(f =>
        f.id === action.floorId ? { ...f, ...action.patch } : f
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'REMOVE_FLOOR': {
      const homeView = { ..._state.project.homeView };
      homeView.floors = homeView.floors.filter(f => f.id !== action.floorId);
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'SELECT_ROOM': {
      _state = { ..._state, selectedRoomId: action.roomId };
      break;
    }
  }
  notify();
}

export function makeId() {
  return Math.random().toString(36).slice(2, 10);
}
