/**
 * Minimal client-side mirror of the Duotecno wire protocol — just enough to
 * build outgoing control messages and decode incoming status messages.
 * Mirrors `reader/src/communication/DuotecnoProtocol.ts` (server-side, the
 * source of truth) — keep the two in sync if the protocol changes.
 */

// Outgoing command codes (see Cmd enum server-side, matches smartapp's Protocol)
export const Cmd = Object.freeze({
  SetDimmer: 162,
  SetSwitch: 163,
  SetControl: 168,  // moods / inputs (short pulse or long on/off)
  SetMotor: 182,
  SetSensor: 136,   // temperature presets / on-off
});

// Incoming status record cmd codes (see Rec enum + handleStatusUpdate server-side)
export const StatusCmd = Object.freeze({
  Mood: 4,
  Dimmer: 5,
  Switch: 6,
  Sensor: 7,
  Motor: 38,
});

export function buildSetSwitch(nodeAddress, unitAddress, on) {
  return [Cmd.SetSwitch, on ? 3 : 2, nodeAddress, unitAddress]; // 3=on, 2=off
}

/** value: <0 = ON/restore last value, 0 = OFF, 1-99 = dim to value */
export function buildSetDimmer(nodeAddress, unitAddress, value) {
  if (value < 0) return [Cmd.SetDimmer, 10, nodeAddress, unitAddress];
  if (value === 0) return [Cmd.SetDimmer, 9, nodeAddress, unitAddress];
  return [Cmd.SetDimmer, 3, nodeAddress, unitAddress, Math.max(1, Math.min(99, value))];
}

/** action: 3=stop, 4=up, 5=down */
export function buildSetMotor(nodeAddress, unitAddress, action) {
  return [Cmd.SetMotor, action, nodeAddress, unitAddress];
}

/** Trigger a mood (short pulse — the mood turns itself off again after ~1.2s on the master). */
export function buildMoodTrigger(nodeAddress, unitAddress) {
  return [Cmd.SetControl, 2, nodeAddress, unitAddress]; // 2 = short pulse
}

/** preset: 0=sun, 1=half sun, 2=moon, 3=half moon */
export function buildSensorPreset(nodeAddress, unitAddress, preset) {
  return [Cmd.SetSensor, 13, nodeAddress, unitAddress, preset]; // 13 = select preset
}

/** inc: true = temp up, false = temp down (bumps the active preset's setpoint) */
export function buildSensorIncDec(nodeAddress, unitAddress, inc) {
  return [Cmd.SetSensor, inc ? 5 : 6, nodeAddress, unitAddress];
}

export function buildSensorOnOff(nodeAddress, unitAddress, on) {
  return [Cmd.SetSensor, 3, nodeAddress, unitAddress, on ? 1 : 0];
}

/**
 * Decode a relayed status record's nodeAddress/unitAddress/status/value —
 * same byte offsets as MasterConnectionService.handleStatusUpdate() uses
 * server-side. Returns null for status types this UI doesn't act on (e.g.
 * sensors, which have a much richer payload).
 */
export function decodeStatus(record) {
  const msg = record.message;
  if (!record.isStatus || !msg || msg.length < 4) return null;
  const nodeAddress = msg[2];
  const unitAddress = msg[3];
  switch (record.cmd) {
    case StatusCmd.Switch:
    case StatusCmd.Motor:
      return { nodeAddress, unitAddress, status: msg[6] };
    case StatusCmd.Mood:
      return { nodeAddress, unitAddress, status: msg[6] };
    case StatusCmd.Dimmer:
      return { nodeAddress, unitAddress, status: msg[6], value: msg[7] };
    default:
      return null;
  }
}
