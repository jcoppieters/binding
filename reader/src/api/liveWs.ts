/**
 * Live WebSocket bridge — a pure relay between the browser and the master's
 * TCP connection. Nothing more: every message the master connection receives
 * from the IP socket is forwarded to all connected browsers verbatim, and
 * whatever a browser sends is forwarded straight to the IP socket. No
 * filtering, no translation into "control" commands — that decoding now
 * happens client-side (see `public/app/live.js` and `public/app/protocol.js`),
 * the same way `MasterConnectionService` itself decodes incoming bytes.
 */

import { WebSocketServer, WebSocket } from 'ws';
import type { Server } from 'http';
import type { CommRecord } from '../communication/DuotecnoProtocol.js';
import { MasterConnectionService } from '../services/MasterConnectionService.js';

export function attachLiveWebSocket(httpServer: Server, masterService: MasterConnectionService): void {
  const wss = new WebSocketServer({ server: httpServer, path: '/ws/live' });

  // IP socket -> browsers. `rest` is just the parser's unconsumed buffer
  // tail, not part of the message itself — drop it before broadcasting.
  masterService.on('message', ({ rest, ...record }: CommRecord) => {
    const data = JSON.stringify(record);
    for (const client of wss.clients) {
      if (client.readyState === WebSocket.OPEN) client.send(data);
    }
  });

  // Browser -> IP socket. Each client sends a raw Duotecno message
  // (a plain number array, e.g. [163, 3, 16, 2]) which is relayed as-is.
  wss.on('connection', (ws) => {
    ws.on('message', (raw) => {
      let msg: unknown;
      try {
        msg = JSON.parse(raw.toString());
      } catch {
        return; // ignore malformed input
      }
      if (Array.isArray(msg)) {
        masterService.sendRawMessage(msg as number[]);
      }
    });
  });
}
