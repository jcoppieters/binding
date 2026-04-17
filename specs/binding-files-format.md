# Binding Files Format (bind[xx].txt)

## Overview

The Duotecno binding software creates text files named `bind[xx].txt` where `xx` is the hexadecimal node address of a Duotecno node (e.g., `bindfc.txt`, `bind03.txt`).

## File Locations

### Writing (Save)
1. **[src/DTBindingSoftware/src/BindingFile1.cpp](../src/DTBindingSoftware/src/BindingFile1.cpp#L359)**
   - Main save function: `CBindingFile::Save()`
   - Writes to path from `TMGetFileSettings()->GetFullyQualifiedPathName()`
   - Format: `sprintf("%sbind%02x.txt", szPathName, bNodeAddress)`

2. **[src/DTBindingSoftware/libraries/dtapprcfgmfclib/apprcfg/apprcfgtasks.cpp](../src/DTBindingSoftware/libraries/dtapprcfgmfclib/apprcfg/apprcfgtasks.cpp#L422)**
   - Used for uploading binding files to remote devices (App Remote Config)
   - Format: `sprintf("%s\\bind%02x.txt", this->getPath(), nodeAddress)`

### Reading (Load)
- **[src/DTBindingSoftware/src/BindingFile1.cpp](../src/DTBindingSoftware/src/BindingFile1.cpp#L448)**
  - Load function: `CBindingFile::Load()`
  - Parses files created by `sscanf(fileName,"bind%x.txt",&temp)`

## File Structure

Each line in a binding file represents one **binding entry**.

### Line Format

```
[NodeAddress]_[Flags]_[BindingNumber]_[Type][Content]
```

**Example:**
```
00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
```

### Components

| Position | Field | Length | Description | Example |
|----------|-------|--------|-------------|---------|
| 0-5 | Node Address | 6 hex chars | Target node address (first 4 are always 0000) | `00000A` |
| 6 | Separator | 1 char | Underscore | `_` |
| 7-8 | Binding Flags | 2 hex chars | Flags for the binding | `00` |
| 9 | Separator | 1 char | Underscore | `_` |
| 10-13 | Binding Number | 4 hex chars | Unique binding ID on this node | `016D` |
| 14 | Separator | 1 char | Underscore (added in version 0x0C06) | `_` |
| 15+ | Type + Content | Variable | Binding type and logic | `C(...)=...` |

## Binding Types

The character at position 15 (or 14 in older formats) indicates the binding type:

| Type | Description | Parser Class |
|------|-------------|--------------|
| `C` | Conditional binding | `CBindingFileEntryParserC` |
| `I` | Immediate binding | `CBindingFileEntryParserI` |
| `N` | Normal binding | `CBindingFileEntryParserN` |
| `G` | Group binding | `CBindingFileEntryParserG` |
| `P` | Property binding | `CBindingFileEntryParserP` |
| `Te` | Timer Extended | Timer types |
| `Ti` | Timer Interval | Timer types |
| `To` | Timer One-shot | Timer types |
| `Td` | Timer Delayed | Timer types |
| `Tf` | Timer Flashing | Timer types |
| `Tr` | Timer Repeat | Timer types |

**Note:** Timer types always have uppercase 'T' followed by a **lowercase** type letter.

## Unit Reference Format

Units within bindings are referenced as:

```
A[NodeAddress]U[UnitAddress]E[Event]
```

Example: `A000025U03E01`
- `A` = Address marker
- `000025` = Node address (hex)
- `U` = Unit marker
- `03` = Unit address on that node
- `E` = Event marker
- `01` = Event type

### Unit Types (BINDING_UNIT_TYPE_*)

| Code | Type | Direction | Description |
|------|------|-----------|-------------|
| 0x01 | DIM | OUT | Dimmer unit |
| 0x02 | SWITCH | OUT | Switch/relay unit |
| 0x03 | CONTROL | IN | Control unit (buttons/inputs) |
| 0x04 | SENS | IN | Sensor unit |
| 0x05 | AUDIO | OUT | Audio unit |
| 0x06 | LCD | - | Not used anymore |
| 0x07 | LCD_VIRTUAL | IN/OUT | LCD virtual unit |
| 0x08 | DUOSWITCH | OUT | Dual switch (blinds/shutters) |
| 0x09 | SELECTOR | - | Not used anymore |
| 0x0A | BOSERS | IN | Bose RS232 |
| 0x0B | AVMATRIX | OUT | AV Matrix |
| 0x0C | IRTX | OUT | IR Transmitter |
| 0x0D | RC5RX | IN | RC5 Receiver |
| 0x0E | VIDEOMUX | OUT | Video Multiplexer |
| 0x12 | ALARM_GALAXY | IN | Galaxy Alarm |
| 0x85 | AUDIO_HC4 | OUT | HC4 Audio specific |
| 0x90 | BOSELF48 | - | Bose LF48 specific |
| 0x91 | BOSEV35 | - | Bose V35 specific |

## Normalization Process

Before writing to file, bindings are normalized by `CBindingFileEntryOperations::normalize()`:

1. **Convert to uppercase** - All characters except timer type letters
2. **Fix timer types** - Replace `TE`→`Te`, `TI`→`Ti`, `TO`→`To`, `TD`→`Td`, `TF`→`Tf`, `TR`→`Tr`
3. **Insert underscore** - Ensure underscore at position 14 (between binding number and type)
4. **Upgrade format** - Apply version upgrades via `Upgrade()` function

### Code Reference
- Normalization: [BindingFileEntryOperations.cpp:631](../src/DTBindingSoftware/src/app/AppBindings/AppBindingFileEntry/BindingFileEntryOperations.cpp#L631)
- Upgrade: [BindingFileEntryOperations.cpp:720](../src/DTBindingSoftware/src/app/AppBindings/AppBindingFileEntry/BindingFileEntryOperations.cpp#L720)

## Parsing

Files are parsed line by line with a maximum buffer length of 300 characters per line.

### Parse Results

| Result Code | Description |
|-------------|-------------|
| `RESULT_EC_SUCCESS` | Line parsed successfully |
| `RESULT_EC_NON_ASCII` | Non-ASCII character found |
| `RESULT_EC_NO_END` | Erroneous end of line |
| `RESULT_EC_NO_DATA` | No data on line |
| `RESULT_EC_NULL` | Null character/empty line |

### Parser State Machine

The parser uses a state machine pattern:
1. `ParseNodeAddress` - Extract node address (6 chars + underscore)
2. `ParseBindingFlags` - Extract flags (2 chars + underscore)
3. `ParseBindingNumber` - Extract binding number (4 chars)
4. Type-specific parsing based on binding type character

## Error Handling

Errors during load are collected in a `CStringArray`:
- Format: `"Error in 'bind[XX].txt' #[LineNum]: [ErrorMessage]"`
- Example: `"Error in 'bind03.txt' #14: Wrong binding syntax"`
- Errors can be logged to: `logfiles\bindingfiles errors.txt`

## Version History

- **0x0901**: Added normalization failure messages
- **0x0902**: Added extra length validation, syntax checking with `CBindingFileEntry::Parse()`
- **0x0C06**: Ensured underscore between binding number and type (at position 14)

## Related Files

- `BindingFile1.cpp` - Main file I/O
- `BindingFileEntryOperations.cpp` - Normalization and comparison
- `BindingFileEntryParser*.cpp` - Type-specific parsers
- `BindingConstants.h` - Unit type definitions
- `apprcfgtasks.cpp` - Remote upload functionality
