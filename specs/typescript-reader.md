# TypeScript Configuration Reader/Writer

## Overview

A TypeScript project for reading, parsing, and understanding Duotecno binding configuration files.

## Location

[../reader/](../reader/)

## Quick Start

```bash
cd reader
npm install
npm run dev
```

## What It Does

The reader parses a complete Duotecno configuration directory and generates a comprehensive installation report:

1. **Reads binding files** (`bind*.txt`) - Parses the low-level binding protocol
2. **Reads node database** (`nodedatabase.txt`) - All nodes and units in the installation
3. **Reads binding metadata** (`bindingconfiginfo.txt`) - Human-readable binding names
4. **Reads node configuration** (`nodeconfiginfo.txt`) - Node configuration flags
5. **Generates reports** - Complete installation overview with statistics

## Example Output

When run against the `configGM` directory, it outputs:

```
📊 SUMMARY
Total Nodes:    6
Total Units:    202
Total Bindings: 19

🖥️  NODES
[FC] 20201107 (LCD Node) - 106 units
[10] Smartbox Living - 41 units
[11] Switch Living - 9 units
[04] Up/Down - 6 units
...

🔗 BINDINGS
[0x000B] Help out
  Expression: (U1*U2)=U3
  Inputs: Dark outside, After 22h
  Outputs: Help out
...
```

## Features

### ✅ Currently Implemented

- Parse all binding file types (N, I, C, G, B, P, Timer)
- Parse node database with full unit information
- Parse binding configuration metadata
- Generate formatted reports
- Export to JSON
- Error reporting and warnings

### ⏳ Future Enhancements

- Parse mood configurations
- Parse schedule configurations
- Parse audio configurations
- Parse LED configurations
- Write back modified configurations
- Generate new bindings programmatically

## Documentation

See [../reader/QUICKSTART.md](../reader/QUICKSTART.md) for detailed usage instructions.

## Project Structure

```
reader/
├── src/
│   ├── models/        # TypeScript types and interfaces
│   ├── parsers/       # File format parsers
│   ├── readers/       # High-level readers
│   └── index.ts       # Main entry point
├── configGM/          # Sample configuration directory
└── package.json
```

## API Usage

```typescript
import { InstallationReader } from './readers/installation.js';

// Read complete installation
const installation = await InstallationReader.read('./configGM');

// Get summary statistics
const summary = InstallationReader.getSummary(installation);

// Print detailed report
InstallationReader.printReport(installation);

// Access specific data
for (const [nodeAddr, bindingFile] of installation.bindingFiles) {
  console.log(`Node 0x${nodeAddr.toString(16)}: ${bindingFile.entries.length} bindings`);
}
```

## Related Specifications

- [binding-files-format.md](binding-files-format.md) - Detailed binding file format specification
- [binding-entries-examples.md](binding-entries-examples.md) - Binding examples with detailed breakdowns
