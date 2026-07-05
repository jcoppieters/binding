/**
 * Duotecno product scraper
 *
 * Crawls duotecno.be product pages and builds a JSON module database in reader/modules/.
 *
 * Usage:
 *   tsx src/scripts/scrape-products.ts              # crawl all categories
 *   tsx src/scripts/scrape-products.ts --dry-run    # print discovered URLs only
 *   tsx src/scripts/scrape-products.ts --no-pdf     # skip PDF download/parse
 *
 * Output: one JSON file per product in reader/modules/<model>.json
 *         plus reader/modules/_index.json  (array of all products)
 */

import { parse } from 'node-html-parser';
import { createRequire } from 'module';
import { existsSync } from 'fs';
const _require = createRequire(import.meta.url);
// pdf-parse v1 exports the parse function directly
const pdfParse = _require('pdf-parse') as (buf: Buffer) => Promise<{ text: string }>;
import { writeFile, readFile, mkdir } from 'fs/promises';
import { join, dirname, extname } from 'path';
import { fileURLToPath } from 'url';

// ─── Config ──────────────────────────────────────────────────────────────────

const BASE = 'https://www.duotecno.be';
const DELAY_MS = 400; // polite crawl delay between requests

const CATEGORY_URLS = [
  'https://www.duotecno.be/nl/producten/modules-smartbox',
  'https://www.duotecno.be/nl/producten/touchscreen',
  'https://www.duotecno.be/nl/producten/schakelaars',
  'https://www.duotecno.be/nl/producten/essence-schakelaar',
  'https://www.duotecno.be/nl/producten/serenity-schakelaar',
  'https://www.duotecno.be/nl/producten/velocity-schakelaar',
  'https://www.duotecno.be/nl/producten/videofonie',
  'https://www.duotecno.be/nl/producten/afdekramen-stopcontacten',
  'https://www.duotecno.be/nl/producten/integrated-range',
  'https://www.duotecno.be/nl/producten/interfaces',
  'https://www.duotecno.be/nl/producten/detectoren',
  'https://www.duotecno.be/nl/producten/temperatuursensoren',
  'https://www.duotecno.be/nl/producten/applicaties',
  'https://www.duotecno.be/nl/producten/audio',
];

// Category slug → human label for the JSON output
const CATEGORY_LABELS: Record<string, string> = {
  'modules-smartbox': 'module',
  'touchscreen': 'lcd',
  'schakelaars': 'switch',
  'essence-schakelaar': 'switch',
  'serenity-schakelaar': 'switch',
  'velocity-schakelaar': 'switch',
  'videofonie': 'video-intercom',
  'afdekramen-stopcontacten': 'cover',
  'integrated-range': 'integrated',
  'interfaces': 'interface',
  'detectoren': 'detector',
  'temperatuursensoren': 'sensor',
  'applicaties': 'software',
  'audio': 'audio',
};

// ─── Types ────────────────────────────────────────────────────────────────────

export type ChannelType =
  | 'relay_no' | 'relay_nc' | 'relay_ssr'
  | 'dimmer_le' | 'dimmer_te' | 'dimmer_pwm' | 'dimmer_dc'
  | 'motor_updown' | 'motor_polar'
  | 'input_digital' | 'input_analog'
  | 'dali' | 'dmx' | 'audio' | 'can_switch';

export interface ChannelGroup {
  type: ChannelType;
  count: number;
  maxLoadW?: number;    // max load per channel in watts
  maxCurrentA?: number; // max current per channel in amps
  label?: string;
}

export interface DuotecnoProduct {
  model: string;
  name: string;
  description: string;
  category: string;

  // UI picker section this product appears in
  uiCategory: string;

  // Cosmetic variants: same function, different finish
  functionalModel?: string; // e.g. "DT1E-4" for all Room-E 4-button variants
  productLine?: string;     // e.g. "Room-E", "Serenity 4-knops", "Velocity 2-hendels"
  finish?: string;          // e.g. "Aluminium Zwart", "Forged Aluminium Grey"

  // Channel definitions (replaces old scalar `channels`)
  channelGroups?: ChannelGroup[];

  // Legacy scalar — kept during transition, remove once channelGroups is populated
  channels?: number;

  // Physical
  dinUnits?: number;
  lengthMm?: number;
  powerW?: number;
  maxLoadPerChannel?: number;

  // Smartbox plug-in system
  isSmartboxBase?: boolean;   // DT0B-01, DT0B-LITE, DT0B-MINI
  isSmartboxPlugin?: boolean; // DT0B-LE, DT0B-TE, DT0B-R, etc.
  slotCount?: number;         // slots available on a Smartbox base

  // References
  sourceUrl: string;
  pdfUrl?: string;
  imageUrl?: string;
  imageFile?: string;
  pdfText?: string; // raw PDF text (debug, stripped before writing)
}

// ─── Helpers ─────────────────────────────────────────────────────────────────

const sleep = (ms: number) => new Promise(r => setTimeout(r, ms));

async function fetchHtml(url: string): Promise<string> {
  const res = await fetch(url, {
    headers: { 'Accept-Language': 'nl', 'User-Agent': 'DuotecnoScraper/1.0' },
  });
  if (!res.ok) throw new Error(`HTTP ${res.status} for ${url}`);
  return res.text();
}

async function fetchBytes(url: string): Promise<Buffer> {
  const res = await fetch(url, {
    headers: { 'User-Agent': 'DuotecnoScraper/1.0' },
  });
  if (!res.ok) throw new Error(`HTTP ${res.status} for ${url}`);
  return Buffer.from(await res.arrayBuffer());
}

function categoryFromUrl(url: string): string {
  // e.g. https://www.duotecno.be/nl/producten/modules-smartbox/... → modules-smartbox
  const match = url.match(/\/nl\/producten\/([^/]+)/);
  return match ? match[1] : 'unknown';
}

// ─── Product page parsing ─────────────────────────────────────────────────────

function parseProductPage(html: string, url: string, categorySlug: string): DuotecnoProduct | null {
  const root = parse(html);

  // Model name — first <h1>
  const h1 = root.querySelector('h1');
  const model = h1?.text.trim().replace(/\u00AD/g, '') ?? ''; // remove soft hyphens
  if (!model) return null;

  // Short description — first <h2> that doesn't look like a boilerplate heading
  const h2s = root.querySelectorAll('h2');
  const descriptionNode = h2s.find(h => {
    const t = h.text.trim();
    return t.length > 3 && !t.toLowerCase().includes('vertel') && !t.toLowerCase().includes('showroom');
  });
  const name = descriptionNode?.text.trim() ?? '';

  // Spec block: "Lengte: 157,7 mm  Aantal modules: 9"
  const bodyText = root.text;

  const lengthMatch = bodyText.match(/Lengte:\s*([\d,\.]+)\s*mm/i);
  const lengthMm = lengthMatch ? parseFloat(lengthMatch[1].replace(',', '.')) : undefined;

  const dinMatch = bodyText.match(/Aantal\s+modules?:\s*(\d+)/i);
  const dinUnits = dinMatch ? parseInt(dinMatch[1]) : undefined;

  // Channels: parse from description / body text heuristics
  const channelMatch = name.match(/^(\d+)\s*kan\./i);
  const channels = channelMatch ? parseInt(channelMatch[1]) : undefined;

  // Max load per channel (e.g. 500W, 5A, 130W)
  const loadMatch = name.match(/(\d+)\s*W\b/i);
  const maxLoadPerChannel = loadMatch ? parseInt(loadMatch[1]) : undefined;

  // PDF link
  const pdfLink = root.querySelectorAll('a[href]').find(a => a.getAttribute('href')?.endsWith('.pdf'));
  const pdfUrl = pdfLink ? new URL(pdfLink.getAttribute('href')!, BASE).href : undefined;

  // Image: first product image (not logo/footer)
  const imgs = root.querySelectorAll('img[src]');
  const productImg = imgs.find(img => {
    const src = img.getAttribute('src') ?? '';
    return src.includes('/Products/') || src.includes('/uploads/images/Products');
  });
  const imageUrl = productImg ? new URL(productImg.getAttribute('src')!, BASE).href : undefined;

  const category = CATEGORY_LABELS[categorySlug] ?? categorySlug;

  // Channel groups from description; for switches also try model number
  let channelGroups = channelGroupsFromDescription(name) ?? undefined;
  if (!channelGroups && category === 'switch') {
    // e.g. DT1E-4Ab → 4, DTBS-B2FA → 2, DTBS-H4FBr → 4
    const mch = model.match(/-(\d+)[A-Za-z]/);
    if (mch) channelGroups = [{ type: 'input_digital', count: parseInt(mch[1]) }];
  }

  // Cosmetic variant extraction (switches, sensors, lcd)
  const { functionalModel, finish } = extractVariantInfo(model, name, category);
  const productLine = deriveProductLine(model);

  // Smartbox flags
  const isSmartboxBase = model in SMARTBOX_BASES ? true : undefined;
  const slotCount = SMARTBOX_BASES[model] ?? undefined;
  const isSmartboxPlugin = model in SMARTBOX_PLUGINS ? true : undefined;
  const pluginChannelType = SMARTBOX_PLUGINS[model];
  const pluginChannelGroups: ChannelGroup[] | undefined = pluginChannelType
    ? [{ type: pluginChannelType, count: 1 }]
    : undefined;

  // UI picker category — drives which section of the "Add module" dialog shows this product
  const uiCategory = deriveUiCategory(model, category, channelGroups ?? pluginChannelGroups, !!isSmartboxBase, !!isSmartboxPlugin);

  return {
    model,
    name,
    description: name,
    category,
    uiCategory,
    ...(functionalModel && { functionalModel }),
    ...(productLine && { productLine }),
    ...(finish && { finish }),
    ...(channelGroups ?? pluginChannelGroups ? { channelGroups: channelGroups ?? pluginChannelGroups } : {}),
    ...(isSmartboxBase && { isSmartboxBase }),
    ...(slotCount && { slotCount }),
    ...(isSmartboxPlugin && { isSmartboxPlugin }),
    dinUnits,
    lengthMm,
    maxLoadPerChannel,
    sourceUrl: url,
    pdfUrl,
    imageUrl,
  };
}

// ─── PDF parsing ──────────────────────────────────────────────────────────────

/**
 * Extract structured specs from PDF text.
 *
 * Duotecno datasheets use a two-column table where ALL headers come first, then ALL values.
 * e.g.: "EIGEN VERBRUIK" ... "100mA @ 24 VAC"  (not adjacent in extracted text)
 *
 * Strategy: scan the full text for known value patterns instead of looking near headings.
 */
function extractFromPdfText(text: string, product: DuotecnoProduct): void {
  // Own power: "100mA @ 24 VAC" anywhere in text → VA = mA × V / 1000
  const maVacMatch = text.match(/([\d,\.]+)\s*mA\s*@\s*([\d,\.]+)\s*V(?:AC)?/i);
  if (maVacMatch) {
    const mA = parseFloat(maVacMatch[1].replace(',', '.'));
    const V  = parseFloat(maVacMatch[2].replace(',', '.'));
    product.powerW = Math.round(mA * V / 1000 * 10) / 10;
  }
  // Fallback: explicit W value near "eigen verbruik" or "power consumption"
  if (!product.powerW) {
    const wMatch = text.match(/(?:eigen\s+verbruik|power\s+consumption)[^0-9]{0,40}([\d,\.]+)\s*W/i);
    if (wMatch) product.powerW = parseFloat(wMatch[1].replace(',', '.'));
  }

  // DIN units + length: "9 modules (157,5 mm)"
  const dinMatch = text.match(/(\d+)\s+modules?\s+\(([\d,\.]+)\s*mm\)/i);
  if (dinMatch) {
    if (!product.dinUnits) product.dinUnits = parseInt(dinMatch[1]);
    if (!product.lengthMm) product.lengthMm = parseFloat(dinMatch[2].replace(',', '.'));
  }

  // Channels from PDF heading e.g. "DT05-06TE 6 kanaals 500W"
  if (!product.channels) {
    const chM = text.match(/(\d+)\s*kanaals?\b/i);
    if (chM) product.channels = parseInt(chM[1]);
  }

  product.pdfText = text.slice(0, 1500);
}

// ─── Channel group extraction ─────────────────────────────────────────────────

/**
 * Derive channelGroups from a product name/description string.
 * Handles the common Duotecno naming patterns.
 *
 * NOTE: motor check comes BEFORE relay check so "X kan. YA relais NO up/down"
 * is not mis-classified as a relay.
 */
function channelGroupsFromDescription(name: string): ChannelGroup[] | undefined {
  const n = name.toLowerCase();

  // ── Motor / up-down (must come before relay) ──────────────────────────────
  const motorMatch = n.match(/(\d+)\s*kan.*?(\d+)\s*a.*up.down/);
  if (motorMatch) return [{ type: 'motor_updown', count: parseInt(motorMatch[1]), maxCurrentA: parseInt(motorMatch[2]) }];

  const polarMatch = n.match(/(\d+)\s*kan.*polar/);
  if (polarMatch) return [{ type: 'motor_polar', count: parseInt(polarMatch[1]) }];

  // ── Dimmers ───────────────────────────────────────────────────────────────
  const teMatch = n.match(/(\d+)\s*kan.*?(\d+)\s*w.*trail/);
  if (teMatch) return [{ type: 'dimmer_te', count: parseInt(teMatch[1]), maxLoadW: parseInt(teMatch[2]) }];

  const leMatch = n.match(/(\d+)\s*kan.*?(\d+)\s*w.*lead/);
  if (leMatch) return [{ type: 'dimmer_le', count: parseInt(leMatch[1]), maxLoadW: parseInt(leMatch[2]) }];

  const pwmMatch = n.match(/(\d+)\s*kan.*pwm/);
  if (pwmMatch) return [{ type: 'dimmer_pwm', count: parseInt(pwmMatch[1]) }];

  const dcMatch = n.match(/(\d+)\s*kan.*(?:0\/10|1\/10)/);
  if (dcMatch) return [{ type: 'dimmer_dc', count: parseInt(dcMatch[1]) }];

  // ── Relay ─────────────────────────────────────────────────────────────────
  const relNoMatch = n.match(/(\d+)\s*kan.*?(\d+(?:[\.,]\d+)?)\s*a\s+relais?\s+no/);
  if (relNoMatch) return [{ type: 'relay_no', count: parseInt(relNoMatch[1]), maxCurrentA: parseFloat(relNoMatch[2]) }];

  const relNcMatch = n.match(/(\d+)\s*kan.*?(\d+)\s*a.*relay.*nc/);
  if (relNcMatch) return [{ type: 'relay_nc', count: parseInt(relNcMatch[1]), maxCurrentA: parseInt(relNcMatch[2]) }];

  // ── Mixed input ───────────────────────────────────────────────────────────
  const mixedInputMatch = n.match(/(\d+)\s*kan.*?no.nc.*?(\d+)\s*kan.*anal/);
  if (mixedInputMatch) return [
    { type: 'input_digital', count: parseInt(mixedInputMatch[1]) },
    { type: 'input_analog', count: parseInt(mixedInputMatch[2]) },
  ];

  const analogMatch = n.match(/(\d+).*kanaals?.*anal.*input/);
  if (analogMatch) return [{ type: 'input_analog', count: parseInt(analogMatch[1]) }];

  // ── DALI / DMX ────────────────────────────────────────────────────────────
  const daliMatch = n.match(/dali.*?(\d+)\s*kan/);
  if (daliMatch) return [{ type: 'dali', count: parseInt(daliMatch[1]) }];

  const dmxMatch = n.match(/dmx.*?(\d+)\s*kan/);
  if (dmxMatch) return [{ type: 'dmx', count: parseInt(dmxMatch[1]) }];

  // ── Switch buttons ────────────────────────────────────────────────────────
  const btnMatch = n.match(/(\d+)\s*(?:drukknopp?|hendels?|kanalen|buttons?)\b/);
  if (btnMatch) return [{ type: 'input_digital', count: parseInt(btnMatch[1]) }];

  return undefined;
}

/**
 * Extract functionalModel and finish from a product model+name.
 * Works for the switch/sensor product lines that have many cosmetic variants.
 *
 * functionalModel: strip the trailing color/finish suffix (non-digit letters at end)
 *   DT1E-4Ab     → DT1E-4    (greedy: last digit group)
 *   DTBS-B4FAb   → DTBS-B4
 *   DT0C-10Ag    → DT0C-10
 *   DTBS-4CCLw   → DTBS-4
 *
 * finish: text after ' | ' separator, or everything after known line prefixes
 */
function extractVariantInfo(model: string, name: string, category: string): { functionalModel?: string; finish?: string } {
  if (!['switch', 'lcd', 'sensor', 'detector'].includes(category)) return {};

  // functionalModel: everything up to and including the last digit
  const fmMatch = model.match(/^(.*\d)/);
  const functionalModel = fmMatch ? fmMatch[1] : undefined;

  // finish: after ' | ' first, then strip known product-line prefixes
  let finish: string | undefined;
  if (name.includes(' | ')) {
    finish = name.split(' | ').pop()?.trim();
  } else {
    // Remove known product-line prefixes to leave just the finish description
    const prefixes = [
      "Room-E ", "Easy Touch '", "Easy Touch ", "Bewegingsdetector Duotecno ",
      "Temperatuursensor in ", "Temperatuursensor ",
    ];
    for (const p of prefixes) {
      if (name.startsWith(p)) {
        finish = name.slice(p.length).replace(/^'|'$|"$/g, '').trim();
        break;
      }
    }
    finish ??= name; // fallback: full name
  }

  return { functionalModel, finish };
}

// ─── UI category derivation ───────────────────────────────────────────────────

/**
 * Two separate pickers in the UI:
 *
 * CABINET PICKER (Rail View "+ Module" button):
 *   Dimmer | Relais | Motor | Input | DALI | DMX | Smartbox | Infrastructuur
 *
 * WONING PICKER (Rail View "+ Schakelaar" / "+ LCD" buttons):
 *   Schakelaar | LCD/Touchscreen | Temperatuursensor | Bewegingsdetector
 *
 * Smartbox PLUGINS are not shown in either picker; they appear only inside
 * the Smartbox slot configurator modal.
 */
type UiCategory =
  // Cabinet modules
  | 'Dimmer' | 'Relais' | 'Motor' | 'Input' | 'DALI' | 'DMX' | 'Audio'
  | 'Smartbox' | 'Smartbox plugin' | 'Infrastructuur'
  // Woning / field devices
  | 'Schakelaar' | 'LCD/Touchscreen' | 'Temperatuursensor' | 'Bewegingsdetector'
  // Accessories / not in picker
  | 'Accessoire';

function deriveUiCategory(
  model: string,
  category: string,
  groups: ChannelGroup[] | undefined,
  isSmartboxBase: boolean,
  isSmartboxPlugin: boolean,
): UiCategory {
  if (isSmartboxBase) return 'Smartbox';
  if (isSmartboxPlugin) return 'Smartbox plugin';

  const firstType = groups?.[0]?.type;

  // ── Woning / field devices (never go in the cabinet picker) ───────────────
  if (category === 'lcd') return 'LCD/Touchscreen';
  if (category === 'sensor') return 'Temperatuursensor';
  if (category === 'detector') return 'Bewegingsdetector';
  if (category === 'switch') return 'Schakelaar';
  if (category === 'video-intercom') return 'Accessoire'; // separate picker if needed
  if (category === 'cover') return 'Accessoire';
  if (category === 'software') return 'Accessoire';
  if (category === 'audio' && !firstType) return 'Accessoire'; // speakers/external audio

  // ── Cabinet modules derived from channel type ─────────────────────────────
  if (firstType) {
    if (firstType.startsWith('dimmer_')) return 'Dimmer';
    if (firstType.startsWith('relay_')) return 'Relais';
    if (firstType.startsWith('motor_')) return 'Motor';
    if (firstType === 'input_digital' || firstType === 'input_analog') return 'Input';
    if (firstType === 'dali') return 'DALI';
    if (firstType === 'dmx') return 'DMX';
    if (firstType === 'audio') return 'Audio';
    if (firstType === 'can_switch') return 'Infrastructuur';
  }

  // ── Audio interfaces (DT10-*) ─────────────────────────────────────────────
  if (category === 'interface' && model.startsWith('DT10-')) return 'Audio';

  // ── Master / gateway ──────────────────────────────────────────────────────
  if (['DT18-HS', 'DT18-PRO', 'DT18-GT'].includes(model)) return 'Infrastructuur';

  // ── Other interfaces (alarm, HVAC, Modbus) ────────────────────────────────
  if (category === 'interface') return 'Infrastructuur';

  // ── Power, bus converters, accessories ────────────────────────────────────
  if (category === 'module') return 'Infrastructuur';

  return 'Accessoire';
}



const SMARTBOX_BASES: Record<string, number> = {
  'DT0B-01': 4,
  'DT0B-LITE': 4,
  'DT0B-MINI': 3,
};

const SMARTBOX_PLUGINS: Record<string, ChannelType> = {
  'DT0B-LE': 'dimmer_le',
  'DT0B-TE': 'dimmer_te',
  'DT0B-DC': 'dimmer_dc',
  'DT0B-PW': 'dimmer_pwm',
  'DT0B-R': 'relay_no',
  'DT0B-SSR': 'relay_ssr',
};

// ─── Product line derivation ──────────────────────────────────────────────────

/**
 * Maps model prefix → human-readable product family name.
 * Used to group cosmetic variants by their functional product line in the UI.
 * Order matters: more specific prefixes must come before broader ones.
 */
const PRODUCT_LINE_RULES: [RegExp, string][] = [
  // Essence
  [/^DTBS-ES1/, 'Essence 1-knop'],
  [/^DTBS-ES2/, 'Essence 2-knops'],
  [/^DTBS-ES4/, 'Essence 4-knops'],
  // Serenity
  [/^DTBS-B1/, 'Serenity 1-knop'],
  [/^DTBS-B2/, 'Serenity 2-knops'],
  [/^DTBS-B4/, 'Serenity 4-knops'],
  // Velocity
  [/^DTBS-H1/, 'Velocity 1-hendel'],
  [/^DTBS-H2/, 'Velocity 2-hendels'],
  [/^DTBS-H4/, 'Velocity 4-hendels'],
  // Touchbutton (specific variants before generic)
  [/^DTBS-4INV/, 'Touchbutton Invisible'],
  [/^DTBS-4X /, 'Touchbutton Trimless'],
  [/^DTBS-4/, 'Touchbutton 4-kanaals'],
  // Easy Touch (before Room-E since DT1ET starts with DT1E)
  [/^DT1ET/, 'Easy Touch'],
  // Room-E
  [/^DT1E/, 'Room-E'],
  // LCD
  [/^DT0C-10/, 'LED-touchscreen 10"'],
  [/^DT0C-7/, 'LED-touchscreen 7"'],
  // Sensors
  [/^DT30/, 'Temperatuursensor'],
  // Detectors
  [/^DT46/, 'Helo detector'],
  [/^DT40-Bb|^DT40-Bw/, 'Bewegingsdetector inbouw groot'],
  [/^DT40-Sb|^DT40-Sw/, 'Bewegingsdetector inbouw klein'],
  [/^DT40-Na|^DT40-Nc|^DT40-Nw/, 'Bewegingsdetector Niko'],
  [/^DT40-IN/, 'Bewegingsdetector inbouw (custom front)'],
  [/^DT40-l/, 'Bewegingsdetector BTicino'],
  [/^DT41/, 'Bewegingsdetector opbouw'],
];

function deriveProductLine(model: string): string | undefined {
  for (const [re, line] of PRODUCT_LINE_RULES) {
    if (re.test(model)) return line;
  }
  return undefined;
}



/**
 * Returns product URLs found on a page.
 * Also returns sub-category URLs to recurse into.
 */
function extractLinksFromPage(html: string, pageUrl: string): { products: string[]; subCategories: string[] } {
  const root = parse(html);
  const products: string[] = [];
  const subCategories: string[] = [];
  const seen = new Set<string>();

  for (const a of root.querySelectorAll('a[href]')) {
    const href = a.getAttribute('href') ?? '';
    let abs: string;
    try { abs = new URL(href, BASE).href; } catch { continue; }

    if (!abs.startsWith(`${BASE}/nl/producten/`) || seen.has(abs)) continue;
    seen.add(abs);

    const path = abs.replace(`${BASE}/nl/producten/`, '');
    const depth = path.split('/').length;
    const hasVariant = abs.includes('?variant=');

    if (hasVariant) {
      products.push(abs);
    } else if (depth >= 1 && !abs.endsWith('/nl/producten/')) {
      // Looks like a sub-category (no variant query param)
      subCategories.push(abs);
    }
  }

  return { products, subCategories };
}

async function collectProductUrls(startUrls: string[]): Promise<Map<string, string>> {
  // Map: productUrl → categorySlug
  const result = new Map<string, string>();
  const visited = new Set<string>();
  const queue = [...startUrls];

  while (queue.length > 0) {
    const url = queue.shift()!;
    if (visited.has(url)) continue;
    visited.add(url);

    const catSlug = categoryFromUrl(url);
    console.log(`  Crawling category: ${url}`);

    let html: string;
    try {
      html = await fetchHtml(url);
    } catch (e) {
      console.warn(`  SKIP (fetch failed): ${url} — ${e}`);
      await sleep(DELAY_MS);
      continue;
    }

    const { products, subCategories } = extractLinksFromPage(html, url);

    for (const p of products) {
      if (!result.has(p)) result.set(p, catSlug);
    }
    for (const s of subCategories) {
      if (!visited.has(s)) queue.push(s);
    }

    await sleep(DELAY_MS);
  }

  return result;
}

// ─── Main ─────────────────────────────────────────────────────────────────────

async function run() {
  const args = process.argv.slice(2);
  const dryRun = args.includes('--dry-run');
  const noPdf = args.includes('--no-pdf');
  const noImages = args.includes('--no-images');

  const __dirname = dirname(fileURLToPath(import.meta.url));
  const outDir = join(__dirname, '../../modules');
  const imgDir = join(outDir, 'images');
  await mkdir(outDir, { recursive: true });
  await mkdir(imgDir, { recursive: true });

  console.log('=== Duotecno product scraper ===\n');

  // Step 1: collect all product URLs
  console.log('Step 1: Discovering product pages...');
  const productMap = await collectProductUrls(CATEGORY_URLS);
  console.log(`\nFound ${productMap.size} product URLs.\n`);

  if (dryRun) {
    for (const [url, cat] of productMap) console.log(`[${cat}] ${url}`);
    return;
  }

  // Step 2: scrape each product page
  console.log('Step 2: Scraping product pages...');
  const products: DuotecnoProduct[] = [];
  const seenModels = new Set<string>();

  for (const [url, catSlug] of productMap) {
    await sleep(DELAY_MS);
    let product: DuotecnoProduct | null = null;

    try {
      const html = await fetchHtml(url);
      product = parseProductPage(html, url, catSlug);
    } catch (e) {
      console.warn(`  SKIP: ${url} — ${e}`);
      continue;
    }

    if (!product) {
      console.warn(`  SKIP (no model found): ${url}`);
      continue;
    }

    // Deduplicate by model name (different variant URLs can map to same page content)
    if (seenModels.has(product.model)) {
      console.log(`  DUP  ${product.model}`);
      continue;
    }
    seenModels.add(product.model);
    console.log(`  OK   ${product.model.padEnd(18)} ${product.name.slice(0, 60)}`);

    // Step 3: download and parse PDF
    if (!noPdf && product.pdfUrl) {
      await sleep(DELAY_MS);
      try {
        const pdfBuf = await fetchBytes(product.pdfUrl);
        const parsed = await pdfParse(pdfBuf);
        extractFromPdfText(parsed.text, product);
        console.log(`       PDF ok, powerW=${product.powerW ?? '?'}`);
      } catch (e) {
        console.warn(`       PDF failed: ${e}`);
      }
    }

    // Step 4: image — always set imageFile from cache; only download if !noImages
    if (product.imageUrl) {
      const ext = extname(new URL(product.imageUrl).pathname) || '.webp';
      const slug = product.model.toLowerCase().replace(/[^a-z0-9]+/g, '-');
      const imgFile = `${slug}${ext}`;
      const imgPath = join(imgDir, imgFile);
      if (existsSync(imgPath)) {
        product.imageFile = `images/${imgFile}`;
        if (!noImages) console.log(`       Image cached`);
      } else if (!noImages) {
        await sleep(DELAY_MS / 2);
        try {
          const imgBuf = await fetchBytes(product.imageUrl);
          await writeFile(imgPath, imgBuf);
          product.imageFile = `images/${imgFile}`;
          console.log(`       Image ok (${(imgBuf.length / 1024).toFixed(0)} KB)`);
        } catch (e) {
          console.warn(`       Image failed: ${e}`);
        }
      }
    }

    products.push(product);
  }

  // Step 4: write individual JSON files
  console.log('\nStep 3: Writing JSON files...');
  const index: Omit<DuotecnoProduct, 'pdfText'>[] = [];

  for (const p of products) {
    const { pdfText: _, ...clean } = p; // strip raw PDF text from individual files
    const slug = p.model.toLowerCase().replace(/[^a-z0-9]+/g, '-');
    const filePath = join(outDir, `${slug}.json`);
    await writeFile(filePath, JSON.stringify(clean, null, 2));
    index.push(clean);
  }

  // Step 5: write index
  await writeFile(join(outDir, '_index.json'), JSON.stringify(index, null, 2));

  console.log(`\nDone. ${products.length} products written to ${outDir}`);
  console.log(`Index: ${join(outDir, '_index.json')}`);
}

run().catch(e => { console.error(e); process.exit(1); });
