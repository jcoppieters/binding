#!/usr/bin/env tsx
/**
 * Merge pricing data from product-codes.csv into _index.json
 * 
 * CSV format: model;description;quantity;unitPrice;totalPrice;;
 * Example: DT0B-ULTRA;Smartbox Ultra...;0 st.;1.650 €;0 €;;
 * 
 * Adds `priceEur` field to matching variants in _index.json
 */

import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const MODULES_DIR = path.join(__dirname, '../../modules');
const CSV_PATH = path.join(MODULES_DIR, 'product-codes.csv');
const INDEX_PATH = path.join(MODULES_DIR, '_index.json');
const OUTPUT_PATH = INDEX_PATH; // overwrite in place

interface PriceMap {
  [model: string]: number;
}

function parsePrice(priceStr: string): number | null {
  // "1.650 €" or "1.800 €" → 1650 or 1800
  const match = priceStr.match(/([0-9.]+)\s*€/);
  if (!match) return null;
  const numStr = match[1].replace(/\./g, ''); // remove thousand separators
  const num = parseInt(numStr, 10);
  return isNaN(num) ? null : num;
}

function parseCsv(filePath: string): PriceMap {
  const content = fs.readFileSync(filePath, 'utf-8');
  const lines = content.split('\n');
  const priceMap: PriceMap = {};
  let lineCount = 0;
  let priceCount = 0;

  for (const line of lines) {
    lineCount++;
    const cols = line.split(';');
    if (cols.length < 4) continue;
    
    const model = cols[0]?.trim();
    const priceStr = cols[3]?.trim();
    
    if (!model || !priceStr) continue;
    // Skip lines that are just section headers or totals
    if (model === '0' || model === '' || priceStr === '') continue;
    
    const price = parsePrice(priceStr);
    if (price !== null && price > 0) {
      priceMap[model] = price;
      priceCount++;
    }
  }

  console.log(`✓ Parsed ${lineCount} CSV lines → ${priceCount} prices extracted`);
  return priceMap;
}

function mergeIntoIndex(indexPath: string, priceMap: PriceMap, outputPath: string): void {
  const modules = JSON.parse(fs.readFileSync(indexPath, 'utf-8'));
  
  let familyCount = 0;
  let variantCount = 0;
  let matchedCount = 0;
  let standaloneCount = 0;

  for (const module of modules) {
    familyCount++;

    // Standalone modules (no variants array)
    if (!module.variants && module.model) {
      standaloneCount++;
      const price = priceMap[module.model];
      if (price !== undefined) {
        module.priceEur = price;
        matchedCount++;
      }
      continue;
    }

    // Family modules (with variants)
    if (module.variants && Array.isArray(module.variants)) {
      for (const variant of module.variants) {
        variantCount++;
        const price = priceMap[variant.model];
        if (price !== undefined) {
          variant.priceEur = price;
          matchedCount++;
        }
      }
    }
  }

  fs.writeFileSync(outputPath, JSON.stringify(modules, null, 2), 'utf-8');
  
  console.log(`✓ Processed ${familyCount} module families (${standaloneCount} standalone, ${variantCount} variants)`);
  console.log(`✓ Matched ${matchedCount} prices`);
  console.log(`✓ Written to ${path.relative(process.cwd(), outputPath)}`);
}

// ──────────────────────────────────────────────────────────────────────────────

function main() {
  console.log('Merging prices from product-codes.csv into _index.json...\n');

  if (!fs.existsSync(CSV_PATH)) {
    console.error(`❌ CSV file not found: ${CSV_PATH}`);
    process.exit(1);
  }

  if (!fs.existsSync(INDEX_PATH)) {
    console.error(`❌ Index file not found: ${INDEX_PATH}`);
    process.exit(1);
  }

  const priceMap = parseCsv(CSV_PATH);
  mergeIntoIndex(INDEX_PATH, priceMap, OUTPUT_PATH);

  console.log('\n✅ Done!');
}

main();
