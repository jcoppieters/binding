/**
 * Project state — single source of truth for the app.
 *
 * Usage:
 *   import { state, dispatch } from './state.js';
 *   dispatch({ type: 'SET_PROJECT', project: { ... } });
 *   state.subscribe(s => console.log(s.project));
 *
 * Data model: `project.nodes[]` is the single source of truth for hardware
 * (keyed by nodeAddress — real discovered nodes are <= 255, temp planning
 * nodes created before discovery start at TEMP_NODE_ADDRESS_START). Everything
 * else (railView.cabinets/house, room device channelRef/buttons/sensors,
 * binding endpoints) references hardware by nodeAddress + unitAddress only —
 * never a separate synthetic id.
 */

import { NodeType, hasCabinetUnits } from './unit-types.js';

/** Temporary node addresses (planning, before discovery) start here — see project.ts. */
export const TEMP_NODE_ADDRESS_START = 1024;

/** Allocate the next unused temporary node address. */
export function allocateTempNodeAddress(nodes) {
  const used = new Set(nodes.map(n => n.nodeAddress));
  let addr = TEMP_NODE_ADDRESS_START;
  while (used.has(addr)) addr++;
  return addr;
}

/** Merge partial node data into `nodes[]`, creating the entry if it doesn't exist yet. */
function upsertNode(nodes, nodeAddress, patch) {
  const idx = nodes.findIndex(n => n.nodeAddress === nodeAddress);
  if (idx === -1) return [...nodes, { nodeAddress, units: [], ...patch }];
  const next = [...nodes];
  next[idx] = { ...next[idx], ...patch };
  return next;
}

const EMPTY_PROJECT = {
  meta: { name: 'Nieuw project', created: new Date().toISOString(), modified: new Date().toISOString(), version: '1' },
  nodes: [],
  railView: {
    cabinets: [{
      id: 'cabinet-0',
      name: 'Hoofdkast',
      widthUnits: 36,
      modules: [],
    }],
    house: [],
  },
  homeView: {
    floors: [{ id: 'floor-0', name: 'Gelijkvloers' }],
    rooms: [],
  },
  bindings: [],
};

let _state = {
  project: EMPTY_PROJECT,
  modules: [],         // loaded from /api/modules
  activeView: 'rail',  // 'rail' | 'home' | 'mat'
  dirty: false,        // unsaved changes
  connected: false,    // TCP connection to master
  masterIp: null,
  discoveredNodes: [], // runtime-only, session-scoped: DiscoveredNode[] after TCP discovery (last raw scan, for online-status display; hardware data itself lives in project.nodes)
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
      if (action.masterPort !== undefined) meta.masterPort = action.masterPort;
      _state = { ..._state, dirty: true, project: { ..._state.project, meta } };
      break;
    }
    case 'ADD_DISCOVERED_NODES': {
      // Upsert every discovered node into project.nodes[] by nodeAddress —
      // never overwrite a user-given name, always refresh units/type/physicalAddress.
      let nodes = [..._state.project.nodes];
      for (const dn of action.nodes) {
        const existing = nodes.find(n => n.nodeAddress === dn.nodeAddress);
        nodes = upsertNode(nodes, dn.nodeAddress, {
          physicalAddress: dn.physicalAddress,
          type: dn.type,
          name: existing?.name ?? dn.name,
          units: dn.units ?? [],
        });
      }

      // Place genuinely new nodes (not yet referenced anywhere in railView) into cabinet/house.
      const placedAddrs = new Set([
        ..._state.project.railView.cabinets.flatMap(c => c.modules.map(m => m.nodeAddress)),
        ..._state.project.railView.house.map(h => h.nodeAddress),
      ]);
      const newNodes = action.nodes.filter(dn => !placedAddrs.has(dn.nodeAddress));

      const cabinetNodes = [];
      const houseNodes = [];
      for (const node of newNodes) {
        // NodeType 32 (kGUINode) = LCD/touchscreen field device -> always house
        if (node.type === NodeType.kGUINode) { houseNodes.push(node); continue; }
        // NodeType 8 (kModemNode) = config modem -> always cabinet
        if (node.type === NodeType.kModemNode) { cabinetNodes.push(node); continue; }
        // NodeType 4 (kGatewayNode) = TCP server / master -> always cabinet
        if (node.type === NodeType.kGatewayNode) { cabinetNodes.push(node); continue; }
        // Standard (1): cabinet if any output hardware present, else house if it's a controller
        const ctrlCount = (node.units ?? []).filter(u => u.type === 3).length; // kInput
        const moodCount = (node.units ?? []).filter(u => u.type === 7).length; // kMood
        if (!hasCabinetUnits(node.units) && (ctrlCount > 0 || moodCount > 0)) {
          houseNodes.push(node);
        } else {
          cabinetNodes.push(node);
        }
      }

      const railView = { ..._state.project.railView };
      if (cabinetNodes.length > 0) {
        let cabinets = [...railView.cabinets];
        if (cabinets.length === 0) cabinets.push({ id: makeId(), name: 'Hoofdkast', widthUnits: 36, modules: [] });
        const last = { ...cabinets[cabinets.length - 1], modules: [...cabinets[cabinets.length - 1].modules] };
        for (const node of cabinetNodes) {
          last.modules.push({ nodeAddress: node.nodeAddress, position: last.modules.length });
        }
        cabinets[cabinets.length - 1] = last;
        railView.cabinets = cabinets;
      }
      if (houseNodes.length > 0) {
        railView.house = [
          ...railView.house,
          ...houseNodes.map(node => ({ nodeAddress: node.nodeAddress, position: 99 })),
        ];
      }
      _state = { ..._state, dirty: true, project: { ..._state.project, nodes, railView } };
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
      // action: { cabinetId, model, nodeAddress? } — nodeAddress omitted = new
      // temp planning module; provided = re-placing existing hardware (e.g. moved from house).
      const nodeAddress = action.nodeAddress ?? allocateTempNodeAddress(_state.project.nodes);
      const nodes = upsertNode(_state.project.nodes, nodeAddress, { model: action.model });
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId
          ? { ...c, modules: [...c.modules, { nodeAddress, position: c.modules.length }] }
          : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, nodes, railView } };
      break;
    }
    case 'UPDATE_MODULE': {
      // action: { cabinetId, nodeAddress, patch: { model?, name?, newNodeAddress?, slots? } }
      const { nodeAddress, patch } = action;
      const targetAddr = patch.newNodeAddress ?? nodeAddress;
      let nodes = _state.project.nodes;
      if (patch.newNodeAddress && patch.newNodeAddress !== nodeAddress) {
        nodes = nodes.filter(n => n.nodeAddress !== nodeAddress); // drop temp node entry
      }
      if (patch.model !== undefined || patch.name !== undefined) {
        nodes = upsertNode(nodes, targetAddr, {
          ...(patch.model !== undefined ? { model: patch.model } : {}),
          ...(patch.name !== undefined ? { name: patch.name } : {}),
        });
      }
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? {
          ...c, modules: c.modules.map(m =>
            m.nodeAddress === nodeAddress
              ? { ...m, nodeAddress: targetAddr, ...(patch.slots !== undefined ? { slots: patch.slots } : {}) }
              : m
          )
        } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, nodes, railView } };
      break;
    }
    case 'UPDATE_MODULE_SLOTS': {
      // Update Smartbox+ plugin slots configuration
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId ? {
          ...c, modules: c.modules.map(m =>
            m.nodeAddress === action.nodeAddress ? { ...m, slots: action.slots } : m
          )
        } : c
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'REMOVE_MODULE': {
      // Removes the placement only — the node record is kept (harmless registry entry;
      // it can be re-placed later, e.g. by re-running discovery).
      const railView = { ..._state.project.railView };
      railView.cabinets = railView.cabinets.map(c =>
        c.id === action.cabinetId
          ? { ...c, modules: c.modules.filter(m => m.nodeAddress !== action.nodeAddress) }
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
        const idx = mods.findIndex(m => m.nodeAddress === action.nodeAddress);
        const target = idx + action.direction;
        if (idx < 0 || target < 0 || target >= mods.length) return c;
        [mods[idx], mods[target]] = [mods[target], mods[idx]];
        return { ...c, modules: mods };
      });
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'ADD_HOUSE_DEVICE': {
      // action: { model, nodeAddress? } — same semantics as ADD_MODULE.
      const nodeAddress = action.nodeAddress ?? allocateTempNodeAddress(_state.project.nodes);
      const nodes = upsertNode(_state.project.nodes, nodeAddress, { model: action.model });
      const railView = { ..._state.project.railView };
      railView.house = [...railView.house, { nodeAddress, position: 99 }];
      _state = { ..._state, dirty: true, project: { ..._state.project, nodes, railView } };
      break;
    }
    case 'REMOVE_HOUSE_DEVICE': {
      const railView = { ..._state.project.railView };
      railView.house = railView.house.filter(d => d.nodeAddress !== action.nodeAddress);
      _state = { ..._state, dirty: true, project: { ..._state.project, railView } };
      break;
    }
    case 'UPDATE_HOUSE_DEVICE': {
      // action: { nodeAddress, patch: { model?, name?, newNodeAddress? } }
      const { nodeAddress, patch } = action;
      const targetAddr = patch.newNodeAddress ?? nodeAddress;
      let nodes = _state.project.nodes;
      if (patch.newNodeAddress && patch.newNodeAddress !== nodeAddress) {
        nodes = nodes.filter(n => n.nodeAddress !== nodeAddress);
      }
      if (patch.model !== undefined || patch.name !== undefined) {
        nodes = upsertNode(nodes, targetAddr, {
          ...(patch.model !== undefined ? { model: patch.model } : {}),
          ...(patch.name !== undefined ? { name: patch.name } : {}),
        });
      }
      const railView = { ..._state.project.railView };
      railView.house = railView.house.map(d => d.nodeAddress === nodeAddress ? { ...d, nodeAddress: targetAddr } : d);
      _state = { ..._state, dirty: true, project: { ..._state.project, nodes, railView } };
      break;
    }
    case 'MOVE_HOUSE_DEVICE': {
      const railView = { ..._state.project.railView };
      const devs = [...railView.house];
      const idx = devs.findIndex(d => d.nodeAddress === action.nodeAddress);
      const target = idx + action.direction;
      if (idx >= 0 && target >= 0 && target < devs.length) {
        [devs[idx], devs[target]] = [devs[target], devs[idx]];
        railView.house = devs;
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
    case 'ADD_DEVICE_TO_ROOM': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.map(r => {
        if (r.id === action.roomId) {
          return { ...r, devices: [...(r.devices || []), action.device] };
        }
        return r;
      });
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'UPDATE_DEVICE_POSITION': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.map(r => {
        if (r.id === action.roomId) {
          return {
            ...r,
            devices: r.devices.map(d =>
              d.id === action.deviceId
                ? { ...d, x: action.x, y: action.y }
                : d
            )
          };
        }
        return r;
      });
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'UPDATE_DEVICE': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.map(r => {
        // If roomId specified, only update in that room
        if (action.roomId && r.id !== action.roomId) return r;
        
        // Update device if found in this room
        const hasDevice = r.devices.some(d => d.id === action.deviceId);
        if (!hasDevice && action.roomId) return r; // Not in this room
        
        return {
          ...r,
          devices: r.devices.map(d => {
            if (d.id === action.deviceId) {
              const updated = { ...d, ...action.patch };
              return updated;
            }
            return d;
          })
        };
      });
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'MOVE_DEVICE_TO_ROOM': {
      const homeView = { ..._state.project.homeView };
      let deviceToMove = null;
      let targetRoom = null;
      
      // Find target room to get dimensions
      targetRoom = homeView.rooms.find(r => r.id === action.toRoomId);
      
      // Remove from source room
      homeView.rooms = homeView.rooms.map(r => {
        if (r.id === action.fromRoomId) {
          const device = r.devices.find(d => d.id === action.deviceId);
          if (device) {
            // Use provided x,y or default to center of target room
            const initialX = action.x !== undefined ? action.x : Math.max(0, (targetRoom?.width || 500) / 2 - 50);
            const initialY = action.y !== undefined ? action.y : Math.max(0, (targetRoom?.height || 400) / 2 - 50);
            deviceToMove = { ...device, x: initialX, y: initialY };
          }
          return { ...r, devices: r.devices.filter(d => d.id !== action.deviceId) };
        }
        return r;
      });
      
      // Add to target room
      if (deviceToMove) {
        homeView.rooms = homeView.rooms.map(r => {
          if (r.id === action.toRoomId) {
            return { ...r, devices: [...r.devices, deviceToMove] };
          }
          return r;
        });
      }
      
      _state = { ..._state, dirty: true, project: { ..._state.project, homeView } };
      break;
    }
    case 'REMOVE_DEVICE_FROM_ROOM': {
      const homeView = { ..._state.project.homeView };
      homeView.rooms = homeView.rooms.map(r => {
        if (r.id === action.roomId) {
          return { ...r, devices: r.devices.filter(d => d.id !== action.deviceId) };
        }
        return r;
      });
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
    case 'ADD_BINDING': {
      // Skip if binding with same ID already exists (prevent duplicates on re-import)
      const bindingExists = _state.project.bindings.some(b => b.id === action.binding.id);
      if (bindingExists) {
        console.warn(`[state] Skipping duplicate binding: ${action.binding.id}`);
        break;
      }
      const bindings = [..._state.project.bindings, action.binding];
      _state = { ..._state, dirty: true, project: { ..._state.project, bindings } };
      break;
    }
    case 'REMOVE_BINDING': {
      // Match by channelRef (nodeAddress+unitAddress) + portId on both ends —
      // the single unambiguous identity every binding endpoint always carries.
      const sameEnd = (a, b) =>
        a.channelRef.nodeAddress === b.channelRef.nodeAddress &&
        a.channelRef.unitAddress === b.channelRef.unitAddress &&
        a.portId === b.portId;
      const bindings = _state.project.bindings.filter(b =>
        !(sameEnd(b.from, action.binding.from) && sameEnd(b.to, action.binding.to))
      );
      _state = { ..._state, dirty: true, project: { ..._state.project, bindings } };
      break;
    }
  }
  notify();
}

export function makeId() {
  return Math.random().toString(36).slice(2, 10);
}

