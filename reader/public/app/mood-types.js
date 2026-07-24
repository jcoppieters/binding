/**
 * Mood action codec — encodes/decodes the hex `msg` string used for one mood
 * action (a single "set this channel to this value" step within a mood).
 *
 * Ported from the reference Angular app's `models/mood.ts` (`toMsg`/`decodeMsg`),
 * which itself mirrors the wire format used by the Master HTTP API
 * (`/moodscfg/`, `/config/download|upload/moods.json`) and the legacy
 * `MoodConfig_XX.json` files. Verified against a real sample:
 *   "A2020400" → dimmer (A2), method 4 (raw "set"), time 0 → decodes to method 0 (off)
 *   "A3020100" → switch (A3), method 1 (set), time 0 → method 0 (off)
 *
 * Only the channel types our editor lets you target are handled here
 * (dimmer, relay, motor, sensor/temperature, mood). Audio/AV/IR actions can
 * exist in imported data but aren't editable in this tool yet.
 */

/** 2-char hex "type code" prefixing every mood action msg. */
export const UnitMoodType = Object.freeze({
  kTemperature:    '88',
  kExtendedAudio:  '9F',
  kDimmer:         'A2',
  kSwitch:         'A3',
  kInput:          'A8',
  kMood:           'A6',
  kAV:             'CA',
  kIRTX:           'AD',
  kSwitchingMotor: 'B6',
});

/** Device category (RoomDevice.type / DeviceType) → mood code, for the types this editor supports. */
export const CATEGORY_TO_MOOD_CODE = Object.freeze({
  dimmer: UnitMoodType.kDimmer,
  relay:  UnitMoodType.kSwitch,
  motor:  UnitMoodType.kSwitchingMotor,
  sensor: UnitMoodType.kTemperature,
  mood:   UnitMoodType.kMood,
});

/** Inverse of `CATEGORY_TO_MOOD_CODE` — derive the device category straight from a decoded action's code. */
export const MOOD_CODE_TO_CATEGORY = Object.freeze({
  [UnitMoodType.kDimmer]:         'dimmer',
  [UnitMoodType.kSwitch]:         'relay',
  [UnitMoodType.kSwitchingMotor]: 'motor',
  [UnitMoodType.kTemperature]:    'sensor',
  [UnitMoodType.kMood]:           'mood',
});

function hexD(n) { return Math.floor(n).toString(16); }
/** 2-digit hex byte (nibble-split, matches upstream `hValue`). */
function hVal(n) { return hexD(n / 16) + hexD(n % 16); }
/** Read the `inx`-th hex byte (2 chars) out of `msg`. */
function sVal(msg, inx) { return parseInt(msg.substr(inx * 2, 2) || '0', 16); }

/**
 * Encode a mood action into its hex `msg` string.
 * @param {{code:string, method:number, time?:number, value?:number}} u
 * @returns {string}
 */
export function toMsg(u) {
  const time = u.time || 0;
  const value = u.value || 0;
  let msg;
  switch (u.code) {
    case UnitMoodType.kDimmer:
      if (u.method === 0) msg = '0400';
      else if (u.method === 1) msg = '0401';
      else if (u.method === 5) msg = '05';
      else if (u.method === 6) msg = '06';
      else if (u.method === 3) msg = '03' + hVal(value);
      else msg = hVal(u.method) + hVal(time) + hVal(value); // e.g. 0x13 PIR-on
      break;
    case UnitMoodType.kSwitch:
      msg = (u.method < 2) ? ('01' + hVal(u.method)) : ('07' + hVal(time));
      break;
    case UnitMoodType.kInput:
    case UnitMoodType.kMood:
      msg = ((u.method < 3) ? '03' : '04') + ((u.method % 2 === 1) ? '00' : '01');
      break;
    case UnitMoodType.kTemperature:
      if (u.method === 1) msg = '01' + hVal(Math.floor(time / 256)) + hVal(time % 256) + hVal(value);
      else msg = hVal(u.method) + hVal(time);
      break;
    case UnitMoodType.kSwitchingMotor:
      msg = hVal(u.method); // up/down/stop (4/5/3)
      break;
    default:
      msg = hVal(u.method || 0);
  }
  return (u.code + hVal(msg.length / 2) + msg).toUpperCase();
}

/**
 * Decode a mood action's hex `msg` back into { code, method, time, value }.
 * Inverse of `toMsg` (only for the channel types this editor supports).
 * @param {string} msg
 */
export function decodeMsg(msg) {
  const code = msg.substring(0, 2).toUpperCase();
  let method = sVal(msg, 2);
  let time = sVal(msg, 3);
  let value = sVal(msg, 4);

  switch (code) {
    case UnitMoodType.kDimmer:
      if (method === 4) { method = time ? 1 : 0; value = 0; }
      else if (method === 3) { value = time; time = 0; }
      break;
    case UnitMoodType.kSwitch:
      if (method === 1) method = time ? 1 : 0;
      break;
    case UnitMoodType.kInput:
    case UnitMoodType.kMood:
      if (method === 3) method = time + 1;      // short off/on → 1/2
      else if (method === 4) method = time + 3; // long off/on  → 3/4
      time = 0;
      break;
    case UnitMoodType.kTemperature:
      if (method === 1) { time = sVal(msg, 3) * 256 + sVal(msg, 4); value = sVal(msg, 5); }
      break;
    case UnitMoodType.kSwitchingMotor:
      break;
  }
  return { code, method, time, value };
}

/** Sensible default action (freshly added row) for a given mood code. */
export function defaultAction(code) {
  switch (code) {
    case UnitMoodType.kDimmer:         return { code, method: 1, time: 0, value: 0 };  // aan
    case UnitMoodType.kSwitch:         return { code, method: 1, time: 0, value: 0 };  // aan
    case UnitMoodType.kMood:
    case UnitMoodType.kInput:          return { code, method: 2, time: 0, value: 0 };  // kort aan
    case UnitMoodType.kTemperature:    return { code, method: 3, time: 0, value: 0 };  // aan
    case UnitMoodType.kSwitchingMotor: return { code, method: 3, time: 0, value: 0 };  // stop
    default:                           return { code, method: 0, time: 0, value: 0 };
  }
}

/** Method dropdown options per mood code (value = raw `method`, label = Dutch text). */
export const MOOD_METHODS = Object.freeze({
  [UnitMoodType.kDimmer]: [
    { value: 0, label: 'Uit' },
    { value: 1, label: 'Aan' },
    { value: 3, label: 'Zet waarde (%)' },
    { value: 5, label: 'Dimmen +' },
    { value: 6, label: 'Dimmen -' },
    { value: 0x13, label: 'PIR aan (tijdelijk)' },
  ],
  [UnitMoodType.kSwitch]: [
    { value: 0, label: 'Uit' },
    { value: 1, label: 'Aan' },
    { value: 2, label: 'PIR aan (tijdelijk)' },
  ],
  [UnitMoodType.kSwitchingMotor]: [
    { value: 3, label: 'Stop' },
    { value: 4, label: 'Op' },
    { value: 5, label: 'Neer' },
  ],
  // kInput reuses kMood's method list/semantics (both are the "toggle-like" channels).
  [UnitMoodType.kMood]: [
    { value: 1, label: 'Kort uit' },
    { value: 2, label: 'Kort aan' },
    { value: 3, label: 'Lang uit' },
    { value: 4, label: 'Lang aan' },
  ],
  [UnitMoodType.kInput]: [
    { value: 1, label: 'Kort uit' },
    { value: 2, label: 'Kort aan' },
    { value: 3, label: 'Lang uit' },
    { value: 4, label: 'Lang aan' },
  ],
  [UnitMoodType.kTemperature]: [
    { value: 1, label: 'Zet temperatuur' },
    { value: 2, label: 'Uit' },
    { value: 3, label: 'Aan' },
  ],
});
