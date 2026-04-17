# Duotecno Configuration Reader - Quick Start

## ✅ What's Working Now

The TypeScript reader can successfully:

1. **Parse all binding files** (`bind*.txt`) - Understanding the low-level protocol format
2. **Parse node database** (`nodedatabase.txt`) - All nodes and their units
3. **Parse node configuration** (`nodeconfiginfo.txt`) - Configuration flags
4. **Parse binding metadata** (`bindingconfiginfo.txt`) - Human-readable binding names
5. **Generate comprehensive reports** - Complete installation overview

## 🚀 Running the Reader

```bash
cd reader
npm install
npm run dev
```

This will read the `configGM` directory and generate a complete report.

## 📊 Sample Output

```
📊 SUMMARY
Total Nodes:    6
Total Units:    202
Total Bindings: 19

🖥️  NODES
[FC] 20201107 (LCD Node)
  - 106 virtual units (moods, triggers, etc.)
  - FC:64 Kitchen (Audio)
  - FC:65 Tim Bedroom (Audio)
  - FC:66 Master Bedroom (Audio)
  ...

[10] Smartbox Living
  - 10:1D Saloon (Dimmer)
  - 10:1F Table (Dimmer)
  - 10:21 Wall (Relais)
  ...

🔗 BINDINGS
[0x000B] Help out
  Expression: (U1*U2)=U3
  Inputs: Dark outside, After 22h
  Outputs: Help out
```

## 📁 Project Structure

```
reader/
├── src/
│   ├── index.ts              ✅ Main entry point
│   ├── models/               ✅ TypeScript interfaces
│   │   ├── binding.ts        - Binding data structures
│   │   ├── node.ts           - Node/unit structures
│   │   ├── bindingConfig.ts  - Binding metadata
│   │   └── installation.ts   - Complete installation model
│   ├── parsers/              ✅ File parsers
│   │   ├── bindingFile.ts    - Parse bind*.txt
│   │   ├── nodeDatabase.ts   - Parse nodedatabase.txt
│   │   └── bindingConfig.ts  - Parse bindingconfiginfo.txt
│   └── readers/              ✅ High-level readers
│       └── installation.ts   - Combine all configs
├── configGM/                 📂 Sample configuration
└── package.json
```

## 🎯 Current Features

### Reading & Parsing
- ✅ **Binding files** - All binding types (N, I, C, G, B, P, Timer types)
- ✅ **Node database** - Complete node and unit information
- ✅ **Binding config** - Human-readable names and metadata
- ✅ **Error reporting** - Warnings for unparseable content

### Output & Reports
- ✅ **Console report** - Formatted installation overview
- ✅ **JSON export** - `npm run dev -- --json` for JSON output
- ✅ **Statistics** - Counts by type, errors, summaries

### Parsed Binding Types
- ✅ **Normal (N)** - Simple mappings
- ✅ **Immediate (I)** - Direct event-to-action
- ✅ **Conditional (C)** - Complex logic with operators
- ✅ **Group (G)** - Group operations
- ✅ **Block (B)** - Block bindings
- ✅ **Property (P)** - Property bindings
- ✅ **Timer (Te/Ti/To/Td/Tf/Tr)** - Timer types

## 📝 Next Steps (Future Enhancements)

### Additional Parsers (Not Yet Implemented)
- ⏳ `unitConfig_*.txt` - Unit-specific configurations
- ⏳ `MoodConfig_*.txt` - Mood/scene configurations
- ⏳ `scheduleconfig_*.txt` - Schedule/timer configs
- ⏳ `audioconfig_*.txt`, `roomctrlaudio_*.txt` - Audio settings
- ⏳ `ledscfg_*.txt` - LED configurations
- ⏳ Binary files (`.dat`, `.bin`) - Binary data formats

### File Writers (Not Yet Implemented)
- ⏳ Write back modified binding files
- ⏳ Update node configurations
- ⏳ Generate new bindings programmatically

## 🔍 Understanding the Output

### Node Format
```
[NodeAddress] NodeName
  Type: NodeType
  Physical Address: 0xHEX
  Software Version: VXX
  Units: Count
```

### Unit Format
```
[NodeAddr:UnitAddr] UnitName (UnitType)
```

### Binding Format
```
[0xBindingNumber] BindingName
  Expression: EvaluationString (e.g., '(U1*U2)=U3')
  Inputs: InputUnit1, InputUnit2, ...
  Outputs: OutputUnit1, ...
  Timers: TimerName (if any)
```

## 🛠️ Development

### Build
```bash
npm run build
```

### Watch mode
```bash
npm run watch
```

### Run built version
```bash
npm start
```

## 📚 Related Documentation

- [../specs/binding-files-format.md](../specs/binding-files-format.md) - Complete binding file specification
- [../specs/binding-entries-examples.md](../specs/binding-entries-examples.md) - Binding examples with breakdowns

## 💡 Usage Examples

### Read a different config directory
```bash
npm run dev -- /path/to/other/config
```

### Export to JSON
```bash
npm run dev -- --json > installation.json
```

### Use in code
```typescript
import { InstallationReader } from './readers/installation.js';

const installation = await InstallationReader.read('./configGM');
const summary = InstallationReader.getSummary(installation);

console.log(`Total nodes: ${summary.totalNodes}`);
console.log(`Total bindings: ${summary.totalBindings}`);
```

## ⚠️ Known Issues

Some binding formats generate warnings. These are being parsed but complex content extraction needs refinement:
- Block bindings with non-standard unit references
- Conditional bindings with complex expressions

These warnings don't prevent the tool from working - it still reads and reports on all bindings.
