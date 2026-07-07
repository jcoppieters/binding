/**
 * Project state — single source of truth for the app.
 *
 * Usage:
 *   import { state, dispatch } from './state.js';
 *   dispatch({ type: 'SET_PROJECT', project: { ... } });
 *   state.subscribe(s => console.log(s.project));
 */

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
      _state = { ..._state, project: action.project, dirty: false };
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
      _state = { ..._state, dirty: true, project: { ..._state.project, meta } };
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
  }
  notify();
}

export function makeId() {
  return Math.random().toString(36).slice(2, 10);
}
