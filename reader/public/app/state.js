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
  railView: { cabinets: [], woningDevices: [] },
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
      _state = { ..._state, connected: action.connected, masterIp: action.ip ?? _state.masterIp };
      break;
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
    case 'ADD_RAIL': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? { ...c, rails: [...c.rails, action.rail] } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_MODULE': {
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? {
          ...c, rails: c.rails.map(r =>
            r.id === action.railId ? { ...r, modules: [...r.modules, action.module] } : r
          )
        } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_WONING_DEVICE': {
      const railView = { ..._state.project.railView };
      railView.woningDevices = [...railView.woningDevices, action.device];
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
