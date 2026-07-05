/**
 * Consolidate module database: group cosmetic variants into family files.
 *
 * BEFORE: one file per finish variant
 *   dtbs-b2ag.json, dtbs-b2ab.json, dtbs-b2fa.json, dtbs-b2fab.json …
 *
 * AFTER: one file per functional family
 *   dtbs-b2.json  →  { functionalModel, productLine, variants: [...] }
 *
 * Standalone products (no functionalModel, or unique) keep their individual files.
 *
 * Usage:
 *   npx tsx src/scripts/consolidate-modules.ts
 */

import { readFile, writeFile, unlink, mkdir } from 'fs/promises';
import { join, dirname } from 'path';
import { fileURLToPath } from 'url';
import { existsSync } from 'fs';

const __dirname = dirname(fileURLToPath(import.meta.url));
const MODULES_DIR = join(__dirname, '../../modules');

// ─── Functional model overrides (fix scraper edge cases) ─────────────────────
// Models where the auto-extracted functionalModel is wrong
const FM_OVERRIDES: Record<string, string> = {
  'DT1ET-BCr': 'DT1ET-4',   // Easy Touch Brushed Copper Rose → Easy Touch 4-button family
  'DT1ET-Bg':  'DT1ET-4',   // Easy Touch Brushed Gold → Easy Touch 4-button family
};

// ─── Product line labels derived from functionalModel ─────────────────────────
const PRODUCT_LINE_RULES: [RegExp, string][] = [
  [/^DTBS-ES1$/, 'Essence 1-knop'],
  [/^DTBS-ES2$/, 'Essence 2-knops'],
  [/^DTBS-ES4$/, 'Essence 4-knops'],
  [/^DTBS-B1$/, 'Serenity 1-knop'],
  [/^DTBS-B2$/, 'Serenity 2-knops'],
  [/^DTBS-B4$/, 'Serenity 4-knops'],
  [/^DTBS-H1$/, 'Velocity 1-hendel'],
  [/^DTBS-H2$/, 'Velocity 2-hendels'],
  [/^DTBS-H4$/, 'Velocity 4-hendels'],
  [/^DTBS-4I$/, 'Touchbutton Invisible'],
  [/^DTBS-4X$/, 'Touchbutton Trimless'],
  [/^DTBS-4/, 'Touchbutton 4-kanaals'],
  [/^DT1ET/, 'Easy Touch'],
  [/^DT1E/, 'Room-E'],
  [/^DT0C-10/, 'LED-touchscreen 10"'],
  [/^DT0C-7/, 'LED-touchscreen 7"'],
  [/^DT30/, 'Temperatuursensor'],
  [/^DT46/, 'Helo detector'],
  [/^DT40$/, 'Bewegingsdetector inbouw'],
  [/^DT41$/, 'Bewegingsdetector opbouw'],
  [/^DT41/, 'Bewegingsdetector opbouw'],
  [/^DT40-B/, 'Bewegingsdetector inbouw groot'],
  [/^DT40-S/, 'Bewegingsdetector inbouw klein'],
  [/^DT40-N/, 'Bewegingsdetector Niko'],
  [/^DT40-l/, 'Bewegingsdetector BTicino'],
];

function productLineFromFunctionalModel(fm: string): string {
  for (const [re, label] of PRODUCT_LINE_RULES) {
    if (re.test(fm)) return label;
  }
  return fm;
}

function slug(s: string): string {
  return s.toLowerCase().replace(/[^a-z0-9]+/g, '-').replace(/^-|-$/g, '');
}

// ─── Types ────────────────────────────────────────────────────────────────────

interface RawProduct {
  model: string;
  name: string;
  category: string;
  uiCategory?: string;
  functionalModel?: string;
  finish?: string;
  channelGroups?: unknown[];
  powerW?: number;
  dinUnits?: number;
  lengthMm?: number;
  isSmartboxBase?: boolean;
  isSmartboxPlugin?: boolean;
  slotCount?: number;
  maxLoadPerChannel?: number;
  channels?: number;
  sourceUrl: string;
  pdfUrl?: string;
  imageUrl?: string;
  imageFile?: string;
  [key: string]: unknown;
}

interface Variant {
  model: string;
  finish?: string;
  sourceUrl: string;
  pdfUrl?: string;
  imageUrl?: string;
  imageFile?: string;
}

interface FamilyProduct {
  functionalModel: string;
  productLine: string;
  category: string;
  uiCategory?: string;
  channelGroups?: unknown[];
  powerW?: number;
  dinUnits?: number;
  lengthMm?: number;
  variants: Variant[];
}

// ─── Main ─────────────────────────────────────────────────────────────────────

async function run() {
  const indexPath = join(MODULES_DIR, '_index.json');
  const raw = JSON.parse(await readFile(indexPath, 'utf-8')) as RawProduct[];

  // Step 1: group by functionalModel (applying overrides first)
  const families = new Map<string, RawProduct[]>();
  const standalone: RawProduct[] = [];

  for (const p of raw) {
    // Apply any model-level overrides
    if (FM_OVERRIDES[p.model]) p.functionalModel = FM_OVERRIDES[p.model];

    if (p.functionalModel) {
      const key = p.functionalModel;
      if (!families.has(key)) families.set(key, []);
      families.get(key)!.push(p);
    } else {
      standalone.push(p);
    }
  }

  console.log(`Families: ${families.size}  |  Standalone: ${standalone.length}`);

  const newIndex: (FamilyProduct | RawProduct)[] = [];
  const filesToDelete: string[] = [];
  const filesToWrite: { path: string; data: object }[] = [];

  // Step 2a: families with 2+ variants → consolidated family file
  for (const [fm, members] of families) {
    const first = members[0];

    if (members.length === 1) {
      // Sole member of its group — treat as standalone but add productLine
      const enriched = { ...first, productLine: productLineFromFunctionalModel(fm) };
      standalone.push(enriched as unknown as RawProduct);
      continue;
    }

    const productLine = productLineFromFunctionalModel(fm);
    const family: FamilyProduct = {
      functionalModel: fm,
      productLine,
      category: first.category,
      ...(first.uiCategory && { uiCategory: first.uiCategory }),
      ...(first.channelGroups && { channelGroups: first.channelGroups }),
      ...(first.powerW != null && { powerW: first.powerW }),
      ...(first.dinUnits && { dinUnits: first.dinUnits }),
      ...(first.lengthMm && { lengthMm: first.lengthMm }),
      variants: members.map(m => ({
        model: m.model,
        ...(m.finish && { finish: m.finish }),
        sourceUrl: m.sourceUrl,
        ...(m.pdfUrl && { pdfUrl: m.pdfUrl }),
        ...(m.imageUrl && { imageUrl: m.imageUrl }),
        ...(m.imageFile && { imageFile: m.imageFile }),
      })),
    };

    newIndex.push(family);

    // Schedule write of family file
    const familySlug = slug(fm);
    filesToWrite.push({ path: join(MODULES_DIR, `${familySlug}.json`), data: family });

    // Schedule deletion of old individual variant files
    for (const m of members) {
      const oldFile = join(MODULES_DIR, `${slug(m.model)}.json`);
      if (existsSync(oldFile)) filesToDelete.push(oldFile);
    }
  }

  // Step 2b: standalone products — keep as-is (but add productLine if derivable)
  for (const p of standalone) {
    const productLine = p.functionalModel ? productLineFromFunctionalModel(p.functionalModel) : undefined;
    const enriched = productLine ? { ...p, productLine } : p;
    newIndex.push(enriched as RawProduct);
    // Rewrite individual file with productLine added
    const fileSlug = slug(p.model);
    filesToWrite.push({ path: join(MODULES_DIR, `${fileSlug}.json`), data: enriched });
  }

  // Step 3: execute writes
  console.log(`\nWriting ${filesToWrite.length} files…`);
  for (const { path, data } of filesToWrite) {
    await writeFile(path, JSON.stringify(data, null, 2));
  }

  // Step 4: delete old variant files (only files that were replaced by family files)
  console.log(`Deleting ${filesToDelete.length} old variant files…`);
  for (const f of filesToDelete) {
    await unlink(f).catch(() => {});
  }

  // Step 5: write new _index.json
  await writeFile(indexPath, JSON.stringify(newIndex, null, 2));

  console.log(`\nDone.`);
  console.log(`  Family files written: ${families.size}`);
  console.log(`  Standalone files: ${standalone.length}`);
  console.log(`  Old variant files deleted: ${filesToDelete.length}`);
  console.log(`  Index entries: ${newIndex.length}`);

  // Summary of families
  console.log('\nFamilies:');
  for (const item of newIndex) {
    if ('variants' in item) {
      const f = item as FamilyProduct;
      console.log(`  ${f.functionalModel.padEnd(12)} ${f.productLine.padEnd(25)} ${f.variants.length} variants`);
    }
  }
}

run().catch(e => { console.error(e); process.exit(1); });
