# Duotecno Configuration Reader/Writer

TypeScript project to read, parse, and write Duotecno binding configuration files.

## Features

- Parse binding files (`bind*.txt`) - low-level binding protocol format
- Parse node database (`nodedatabase.txt`) - node and unit information
- Parse binding config info (`bindingconfiginfo.txt`) - human-readable binding metadata
- Parse node config info (`nodeconfiginfo.txt`) - node configuration flags
- Parse mood, schedule, audio, and other configuration files
- Display complete installation configuration
- Write modified configurations back to files

## Installation

```bash
npm install
```

## Usage

### Development
```bash
npm run dev
```

### Build
```bash
npm run build
npm start
```

## File Formats

See `../specs/` directory for detailed documentation on file formats:
- [binding-files-format.md](../specs/binding-files-format.md) - Binding file protocol
- [binding-entries-examples.md](../specs/binding-entries-examples.md) - Binding examples

## Project Structure

```
src/
  ├── index.ts              # Main entry point
  ├── parsers/              # File parsers
  │   ├── bindingFile.ts    # Parse bind*.txt files
  │   ├── nodeDatabase.ts   # Parse nodedatabase.txt
  │   ├── bindingConfig.ts  # Parse bindingconfiginfo.txt
  │   ├── nodeConfig.ts     # Parse nodeconfiginfo.txt
  │   └── ...               # Other parsers
  ├── models/               # TypeScript interfaces/types
  │   ├── binding.ts        # Binding data structures
  │   ├── node.ts           # Node data structures
  │   └── installation.ts   # Complete installation model
  ├── writers/              # File writers
  │   └── ...               # Writer implementations
  └── utils/                # Utility functions
      ├── parser.ts         # Common parsing utilities
      └── formatter.ts      # Output formatting
```

## Example

```typescript
import { InstallationReader } from './readers/installation';

const installation = await InstallationReader.read('./configGM');
console.log(installation.printSummary());
```
