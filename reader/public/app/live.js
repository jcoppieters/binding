/**
 * Live hardware connection (WebSocket)
 *
 * The server (`src/api/liveWs.ts`) is a pure relay: every message the master
 * connection receives from the IP socket is forwarded here verbatim, and
 * whatever we send is forwarded straight to the IP socket. No translation on
 * either side. This module is "the listener on the websocket" — it decodes
 * each incoming record and, for status messages, updates a small cache and
 * notifies subscribers. Device components read whatever fields they need
 * from that cache; commands are built here too (see protocol.js) and sent
 * as raw protocol messages.
 *
 * One shared socket + a small pub/sub emitter: views subscribe once (at
 * module load), not per device card — see home-view.js.
 */

import { decodeStatus } from './protocol.js';

let socket = null;
let wantConnected = false; // tracks intent, so onclose knows whether to retry
const listeners = new Set();

// "nodeAddress-unitAddress" -> last known { status, value? }
const liveUnits = new Map();

function key(nodeAddress, unitAddress) {
  return `${nodeAddress}-${unitAddress}`;
}

function notify() {
  for (const cb of listeners) cb();
}

function open() {
  const proto = location.protocol === 'https:' ? 'wss:' : 'ws:';
  socket = new WebSocket(`${proto}//${location.host}/ws/live`);

  socket.onmessage = (event) => {
    let record;
    try {
      record = JSON.parse(event.data);
    } catch {
      return;
    }
    const status = decodeStatus(record);
    if (!status) return; // not a status message this UI acts on \u2014 ignore, protocol allows it
    liveUnits.set(key(status.nodeAddress, status.unitAddress), status);
    notify();
  };

  socket.onclose = () => {
    socket = null;
    if (wantConnected) setTimeout(open, 2000);
  };

  socket.onerror = () => {
    socket?.close();
  };
}

/** Open the browser \u2194 server WebSocket. Call once the master is connected
 * (i.e. together with "Verbinden met master"), not before \u2014 there's nothing
 * to relay until then. Safe to call again after a master IP/port change: the
 * relay just keeps forwarding whatever the server-side master connection is,
 * so re-calling this only matters if the socket itself had been closed. */
export function connectLive() {
  wantConnected = true;
  liveUnits.clear();
  if (!socket) open();
}

/** Close the browser \u2194 server WebSocket (e.g. on "Verbreken"). */
export function disconnectLive() {
  wantConnected = false;
  socket?.close();
  socket = null;
  liveUnits.clear();
  notify();
}

/** Latest known live state for a unit, or undefined if unknown (not connected / no update yet). */
export function getLiveUnit(nodeAddress, unitAddress) {
  return liveUnits.get(key(nodeAddress, unitAddress));
}

/** Subscribe to "some unit's live state changed". Returns an unsubscribe function. */
export function onLiveUpdate(callback) {
  listeners.add(callback);
  return () => listeners.delete(callback);
}

/** Send a raw Duotecno protocol message (see protocol.js builders) straight to the master. */
export function sendRaw(message) {
  if (!socket || socket.readyState !== WebSocket.OPEN) {
    console.warn('[live] Not connected, cannot send message');
    return;
  }
  socket.send(JSON.stringify(message));
}
