/**
 * Moods HTTP API client — talks directly to a Duotecno node's live HTTP
 * config API to read/write moods, independent of the TCP binding protocol.
 *
 * Two channels, per `specs/api - Authorization API - v1.05.pdf` and
 * `specs/api - moodscfg - v1.15.pdf`:
 *   - Auth API   — HTTPS, port given by the caller (default 8081).
 *                  `/auth/create` (first time) / `/auth/refresh` (reuse) →
 *                  { client_id, access_token, refresh_token, expiry_time }.
 *                  Self-signed certs on the device → TLS verification is
 *                  disabled for this client only.
 *   - Config API — plain HTTP, always port 8080. Every call carries
 *                  `Authorization: Basic base64(client_id:access_token)`.
 *
 * Not a singleton like `MasterConnectionService`: the moods API is stateless
 * per (host, port) pair, so each call takes the target explicitly and the
 * short-lived access token is cached per host+client so repeated calls don't
 * re-authenticate every time.
 */

import https from 'https';
import http from 'http';

const CONFIG_PORT = 8080; // fixed by spec — "the port is always 8080"

export interface MoodsApiCredentials {
  clientId: string;
  refreshToken: string;
}

interface CachedToken {
  clientId: string;
  accessToken: string;
  refreshToken: string;
  expiresAt: number; // ms epoch
}

function basicAuthHeader(id: string, secret: string): string {
  return 'Basic ' + Buffer.from(`${id}:${secret}`).toString('base64');
}

function request(options: https.RequestOptions | http.RequestOptions, useHttps: boolean, body?: unknown): Promise<{ status: number; json: any }> {
  const lib = useHttps ? https : http;
  return new Promise((resolve, reject) => {
    const req = lib.request(options, (res) => {
      let raw = '';
      res.on('data', (chunk) => (raw += chunk));
      res.on('end', () => {
        let json: any = null;
        try { json = raw ? JSON.parse(raw) : null; } catch { /* non-JSON error body */ }
        resolve({ status: res.statusCode || 0, json });
      });
    });
    req.on('error', reject);
    if (body !== undefined) req.write(JSON.stringify(body));
    req.end();
  });
}

/** In-memory access-token cache, keyed by `host:apiPort:clientId`. */
const tokenCache = new Map<string, CachedToken>();

export class MoodHttpService {
  /**
   * Ensure we hold a valid access token for this host, creating or
   * refreshing the client as needed. Returns credentials the caller should
   * persist (in `project.meta`) so future calls can skip client creation.
   */
  async authenticate(host: string, apiPort: number, password: string, existing?: MoodsApiCredentials): Promise<MoodsApiCredentials> {
    if (existing?.clientId && existing?.refreshToken) {
      try {
        const refreshed = await this.refresh(host, apiPort, existing.clientId, existing.refreshToken);
        return refreshed;
      } catch {
        // Refresh token no longer valid (device rebooted / client evicted) — fall through to create.
      }
    }
    return this.create(host, apiPort, password);
  }

  private async create(host: string, apiPort: number, password: string): Promise<MoodsApiCredentials> {
    const dummyClientId = Math.random().toString(36).slice(2, 18);
    const { status, json } = await request({
      hostname: host,
      port: apiPort,
      path: '/auth/create',
      method: 'GET',
      rejectUnauthorized: false,
      headers: { Authorization: basicAuthHeader(dummyClientId, password) },
    }, true);
    if (status !== 200 || !json?.access_token) {
      throw new Error(json?.error || `auth/create failed (${status})`);
    }
    this.cacheToken(host, apiPort, json);
    return { clientId: json.client_id, refreshToken: json.refresh_token };
  }

  private async refresh(host: string, apiPort: number, clientId: string, refreshToken: string): Promise<MoodsApiCredentials> {
    const { status, json } = await request({
      hostname: host,
      port: apiPort,
      path: '/auth/refresh',
      method: 'GET',
      rejectUnauthorized: false,
      headers: { Authorization: basicAuthHeader(clientId, refreshToken) },
    }, true);
    if (status !== 200 || !json?.access_token) {
      throw new Error(json?.error || `auth/refresh failed (${status})`);
    }
    this.cacheToken(host, apiPort, json);
    return { clientId: json.client_id, refreshToken: json.refresh_token };
  }

  private cacheToken(host: string, apiPort: number, json: any) {
    tokenCache.set(`${host}:${apiPort}:${json.client_id}`, {
      clientId: json.client_id,
      accessToken: json.access_token,
      refreshToken: json.refresh_token,
      expiresAt: Date.now() + (json.expiry_time ?? 2400) * 1000 - 30_000, // refresh 30s early
    });
  }

  /** Config API call (port 8080), authenticating/refreshing as needed. One retry on 401 expired_token. */
  private async configRequest(host: string, apiPort: number, credentials: MoodsApiCredentials, password: string, method: string, path: string, body?: unknown): Promise<any> {
    let token = tokenCache.get(`${host}:${apiPort}:${credentials.clientId}`);
    if (!token || Date.now() >= token.expiresAt) {
      const refreshed = await this.authenticate(host, apiPort, password, credentials);
      token = tokenCache.get(`${host}:${apiPort}:${refreshed.clientId}`);
    }
    if (!token) throw new Error('Kon geen toegangstoken verkrijgen');

    const doCall = (accessToken: string) => request({
      hostname: host,
      port: CONFIG_PORT,
      path,
      method,
      headers: {
        Authorization: basicAuthHeader(token!.clientId, accessToken),
        'Content-Type': 'application/json',
      },
    }, false, body);

    let { status, json } = await doCall(token.accessToken);
    if (status === 401) {
      // Access token expired server-side sooner than expected — refresh once and retry.
      const refreshed = await this.refresh(host, apiPort, token.clientId, token.refreshToken);
      const retried = tokenCache.get(`${host}:${apiPort}:${refreshed.clientId}`)!;
      ({ status, json } = await doCall(retried.accessToken));
    }
    if (status < 200 || status >= 300 || json?.status === 'error') {
      throw new Error(json?.error_detail || json?.error || `moods API call failed (${status})`);
    }
    return json;
  }

  /** GET /moodscfg/info — summary of all moods on this node. */
  getMoodsInfo(host: string, apiPort: number, credentials: MoodsApiCredentials, password: string) {
    return this.configRequest(host, apiPort, credentials, password, 'GET', '/moodscfg/info');
  }

  /** GET /moodscfg/[id] — a single mood's full header + data. */
  getMood(host: string, apiPort: number, credentials: MoodsApiCredentials, password: string, id: number) {
    return this.configRequest(host, apiPort, credentials, password, 'GET', `/moodscfg/${id}`);
  }

  /** POST /moodscfg/ — create/edit a single mood (header + data). */
  saveMood(host: string, apiPort: number, credentials: MoodsApiCredentials, password: string, mood: unknown) {
    return this.configRequest(host, apiPort, credentials, password, 'POST', '/moodscfg/', mood);
  }
}
