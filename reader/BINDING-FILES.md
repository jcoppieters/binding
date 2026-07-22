# Duotecno Binding Files Format

Complete documentation for Duotecno binding configuration files (`bind[xx].txt`).

## Overview

Binding files store the automation logic that connects input devices (buttons, sensors) to output devices (lights, switches, motors). Each Duotecno node has its own binding file.

### File Naming Convention

```
bind[xx].txt
```

Where `[xx]` is the hexadecimal node address:
- `bind03.txt` - Node 3
- `bindfc.txt` - Node 252 (0xFC)
- `bind0a.txt` - Node 10 (0x0A)

### File Location

Files are stored in the configuration directory alongside other config files:
```
config/
├── bind03.txt
├── bind05.txt
├── bindfc.txt
├── Config/
│   ├── nodedatabase.txt
│   ├── bindingconfiginfo.txt
│   └── ...
└── ...
```

---

## File Structure

Each line represents **one binding entry** connecting inputs to outputs.

### Line Format

```
[NodeAddress]_[Flags]_[BindingNumber]_[Type][Content]
```

### Components

| Position | Field          | Length   | Description                            | Example      |
|----------|----------------|----------|----------------------------------------|--------------|
| 0-5      | Node Address   | 6 chars  | Target node address (hex, zero-padded) | `00000A`     |
| 6        | Separator      | 1 char   | Underscore                             | `_`          |
| 7-8      | Flags          | 2 chars  | Binding behavior flags (hex)           | `00`         |
| 9        | Separator      | 1 char   | Underscore                             | `_`          |
| 10-13    | Binding Number | 4 chars  | Unique binding ID on node (hex)        | `016D`       |
| 14       | Separator      | 1 char   | Underscore (added v0x0C06)             | `_`          |
| 15+      | Type + Content | Variable | Binding type and logic                 | `C(...)=...` |

### Example Entry

```
00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
```

**Parsing:**
- Node: `0x0A` (10)
- Flags: `0x00` 
- Binding #: `0x016D` (365)
- Type: `C` (Conditional)
- Logic: Two inputs OR'd together controlling one output

---

## Event Codes Reference

### Overview

Event codes (E codes) define the **trigger type** for input units in bindings. The old Duotecno binding software used specific terminology that maps to these codes.

### Core Event Code Definitions

From old software source code (`src/DTBindingSoftware/src/nodemess.h`):

```c
#define EV_UNITCONTROLTOGGLE      0x01  // "Event Long"
#define EV_UNITCONTROLPULSTOGGLE  0x02  // "Event Short Pulse + State"  
#define EV_UNITCONTROLPULS        0x03  // "Event Short Pulse"
#define EV_UNITCONTROLSTATUS      0x04  // Status request
```

### Device Type Support Matrix

Different device types support different combinations of event codes:

| Device Type | E01 (Lang) | E02 (Kort On/Off) | E03 (Kort) | Notes |
|-------------|------------|-------------------|------------|-------|
| **Button Inputs** (DTBS, physical switches) | ✅ **L** | ❌ | ✅ **P** | Only short (P) and long (L) |
| **Mood Units** (virtual/LCD) | ✅ Lang On/Off | ✅ Kort On/Off | ✅ Kort | All three types supported |
| **Switches/Dimmers** (state tracking) | ❌ | ✅ State | ❌ | Only E02 for state in conditionals |

### Old Software Interface Labels

#### For Button Inputs (Source → DTBS button)
- **P** = Short Pulse (E03 / 0x03) = "Kort"
- **L** = Long (E01 / 0x01) = "Lang"

Example from old UI: `Switch Living → Button 8`
```
├─ 🔘 boven links
   ├── P  Poort openen P    (E03 = Short Pulse)
   └── L  All off L         (E01 = Long)
```

#### For Mood Inputs (Source → Mood)
When a mood is the **target receiving a trigger**, dropdown shows:
- **"Short Pulse"** (E03) - Simple pulse/toggle the mood
- **"Short Pulse On/Off"** (E02) - With state tracking
- **"Long On/Off"** (E01) - Long press trigger

These are the INPUT event types that a mood can receive. Function codes:
- 0xFA6 = Short Pulse trigger
- 0xFA2 = Short Pulse On/Off (state-aware)
- 0xFA4 = Long On/Off (state-aware)

#### For Mood Outputs (Mood → Target Device)
When a mood is the **source triggering another device**, the mood can send different event types:
- **"Event Short Pulse"** (E03) - Simple pulse trigger
- **"Event Short Pulse + State"** (E02) - Pulse with state tracking
- **"Event Long"** (E01) - Long press equivalent

These appear in the old software dropdown when:
- Binding from: Mood → Any controllable device
- Function codes: 0xFA6 (Pulse), 0xFA2 (Pulse+State), 0xFA4 (Long+State)
- The mood OUTPUT port can generate any of these three event types

Example: `Mood "All On" → Lamp` can trigger with E03 (instant), E02 (with state), or E01 (long equivalent)

### Common Binding Patterns

#### UX Redundancy Pattern
Many installations duplicate bindings with both Kort (E03) and Lang (E01) to the same target. This handles users who press buttons too long:

```
000011_80_0044_IU07E03>A0000FCU01FA6D0102S   # Short press
000011_80_0046_IU07E01>A0000FCU01FA6D0102S   # Long press
                  ^^                ^^
                  Same button         Same target mood
```

Both trigger the same action - improves UX by not requiring precise press duration.

#### State Tracking (E02)
Code 0x02 is used when state synchronization is needed:
- In conditional bindings checking switch/dimmer state
- For mood triggers that need On/Off state
- NOT used for simple button inputs

```
0000FC_80_000B_BU03E02>A0000FCU05S
                  ^^
                  E02 = State tracking enabled
```

---

## Binding Types

The character at position 15 indicates the binding type.

| Type | Code | Description | Use Case |
|------|------|-------------|----------|
| Normal | `N` | Standard binding | Simple input → output |
| Immediate | `I` | Direct immediate response | Fast real-time control |
| Conditional | `C` | Logic with conditions | Multiple inputs, AND/OR logic |
| Group | `G` | Group operations | Control multiple outputs |
| Property | `P` | Property-based | System properties |
| Timer Extended | `Te` | Extended timer | Complex timing |
| Timer Interval | `Ti` | Interval timer | Periodic actions |
| Timer One-shot | `To` | One-time timer | Delayed action |
| Timer Delayed | `Td` | Delayed action | Start delay |
| Timer Flashing | `Tf` | Flashing pattern | Blink effect |
| Timer Repeat | `Tr` | Repeating timer | Continuous cycle |

**Note:** Timer types always have uppercase 'T' followed by **lowercase** type letter.

---

## Unit Reference Format

Units within bindings use this format:

```
A[NodeAddress]U[UnitAddress]E[Event]
```

### Components

| Part | Description | Example |
|------|-------------|---------|
| `A` | Address marker | `A` |
| `NodeAddress` | 6-digit hex node address | `000025` |
| `U` | Unit marker | `U` |
| `UnitAddress` | 2-digit hex unit address | `03` |
| `E` | Event marker | `E` |
| `Event` | 2-digit event code | `01` |

### Example

```
A000025U03E01
```

**Meaning:** Node 37 (0x25), Unit 3, Event 1 (toggle)

---

## Unit Types

| Code | Type | Direction | Description |
|------|------|-----------|-------------|
| 0x01 | DIM | OUT | Dimmer unit |
| 0x02 | SWITCH | OUT | Switch/relay unit |
| 0x03 | CONTROL | IN | Control unit (buttons) |
| 0x04 | SENS | IN | Sensor unit (temperature) |
| 0x05 | AUDIO | OUT | Audio unit |
| 0x07 | LCD_VIRTUAL | IN/OUT | LCD virtual unit |
| 0x08 | DUOSWITCH | OUT | Motor/blinds (dual switch) |
| 0x09 | MOOD | OUT | Mood/scene unit |
| 0x0A | BOSERS | IN | Bose RS232 |
| 0x0B | AVMATRIX | OUT | AV Matrix |
| 0x0C | IRTX | OUT | IR Transmitter |
| 0x0D | RC5RX | IN | RC5 Receiver |

---

## Event Codes (Input Units)

Event codes define the trigger type for input units. The available events depend on the device type.

### Core Event Codes

From the old binding software source code (`nodemess.h` and `BindingSoftware.rc`):

| Code | Constant | Old UI Label | Description |
|------|----------|--------------|-------------|
| 0x01 | EV_UNITCONTROLTOGGLE | "Event Long" | Long press / Long On/Off |
| 0x02 | EV_UNITCONTROLPULSTOGGLE | "Event Short Pulse + State" | Short press with state tracking |
| 0x03 | EV_UNITCONTROLPULS | "Event Short Pulse" | Short press / pulse trigger |
| 0x04 | EV_UNITCONTROLSTATUS | - | Status request |

### Device-Specific Event Support

Different device types support different event codes:

#### Button Inputs (DTBS, Physical Switches)

Button inputs support **only 2 event types**:

| Code | Label | Old UI | Description |
|------|-------|--------|-------------|
| 0x03 | Kort | **P** | Short pulse - immediate trigger |
| 0x01 | Lang | **L** | Long press - held down |

**Note:** Code 0x02 (Short Pulse + State) is **not used** for button inputs. Many installations use both Kort (0x03) and Lang (0x01) to the same target to handle users who press too long (UX redundancy).

#### Mood Inputs (Virtual Units)

Mood units support **all 3 event types**:

| Code | Label | Old UI Label | Description |
|------|-------|--------------|-------------|
| 0x03 | Kort | "Short Pulse" | Instant trigger |
| 0x02 | Kort On/Off | "Short Pulse On/Off" | Trigger with state |
| 0x01 | Lang On/Off | "Long On/Off" | Long trigger with state |

#### Switch/Dimmer Inputs (State Tracking)

State-tracking devices (switches, dimmers) use event code **0x02** for state management in conditional bindings.

### Event Codes in Action

**Button to Light (Short press):**
```
000011_80_001A_IU07E03>A000010U03FA6D0102S
                  ^^
                  E03 = Short Pulse (Kort)
```

**Button to Light (Long press):**
```
000011_80_0020_IU04E01>A000010U0AFA6D0102S
                  ^^
                  E01 = Long Press (Lang)
```

**Mood to Mood (Short with state):**
```
0000FC_80_000B_BU03E02>A0000FCU05S
                  ^^
                  E02 = Short Pulse + State
```

---

## Binding Examples

### Example 1: Simple Switch Control (Type N - Normal)

**Scenario:** Button controls a light switch

```
00000A_00_0001_NA00000AU05=U02F0400S
```

**Breakdown:**
- **Node:** `0x0A` (10)
- **Flags:** `0x00` (normal)
- **Binding #:** `0x0001` (1)
- **Type:** `N` (Normal)
- **Input:** Node 0x0A, Unit 5 (button)
- **Output:** Unit 2 (switch), Function 0x04 (On/Off toggle)

**Action:** When button 5 is pressed, switch 2 toggles On/Off.

---

### Example 2: Immediate Binding (Type I)

**Scenario:** Direct button-to-switch connection

```
000025_80_0010_IU03E03>A000025U02F0400S
```

**Breakdown:**
- **Node:** `0x25` (37)
- **Flags:** `0x80` (inverted polarity)
- **Binding #:** `0x0010` (16)
- **Type:** `I` (Immediate)
- **Input:** Unit 3, Event 3 (pulse)
- **Output:** Node 0x25, Unit 2, Function 0x04

**Action:** Immediate response when unit 3 sends pulse → unit 2 toggles.

---

### Example 3: Conditional Binding (Type C)

**Scenario:** Two buttons control one light (OR logic)

```
00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
```

**Breakdown:**
- **Node:** `0x0A`
- **Flags:** `0x00`
- **Binding #:** `0x016D` (365)
- **Type:** `C` (Conditional)
- **Condition:** `(Input1 + Input2)` where `+` = OR
- **Input 1:** Node 0x25, Unit 3, Event 1
- **Input 2:** Node 0x25, Unit 4, Event 1
- **Output:** Unit 2, Function 0xB6, Data: 0x03

**Action:** Either button 3 OR button 4 triggers output unit 2.

---

### Example 4: XOR Logic (Type C)

**Scenario:** Two-way switch (only one active at a time)

```
00000F_00_0025_C(A00000FU01E02^A00000FU02E02)=U05F0400S
```

**Breakdown:**
- **Type:** `C` (Conditional)
- **Condition:** `(Input1 ^ Input2)` where `^` = XOR
- **Input 1:** Node 0x0F, Unit 1, Event 2
- **Input 2:** Node 0x0F, Unit 2, Event 2
- **Output:** Unit 5, Function 0x04

**Action:** Button 1 XOR Button 2 controls unit 5 (two-way switch behavior).

---

### Example 5: Dimmer Control (Type N)

**Scenario:** Button dims a light to 50%

```
000003_00_0042_NA000003U01E01=U02F03D0132S
```

**Breakdown:**
- **Node:** `0x03`
- **Type:** `N` (Normal)
- **Input:** Unit 1 (button)
- **Output:** Unit 2 (dimmer), Function 0x03 (set value)
- **Data:** `D01` = 1 byte, `32` = 50 (decimal)

**Action:** Button press sets dimmer to 50%.

---

### Example 6: Motor Control (Type N)

**Scenario:** Button controls blinds up/down

```
000005_00_0012_NA000005U10E01=U08F0400S
```

**Breakdown:**
- **Node:** `0x05`
- **Input:** Unit 16 (0x10) - button
- **Output:** Unit 8 - motor (DUOSWITCH)
- **Function:** 0x04 (UP)

**Action:** Button press moves motor up.

---

## Conditional Operators

Conditional bindings (Type C) support logic operators:

| Operator | Symbol | Description |
|----------|--------|-------------|
| OR | `+` | Any input triggers |
| AND | `*` | All inputs required |
| XOR | `^` | Only one input active |
| NOT | `!` | Invert logic |

### Complex Example

```
C((A000003U01E01*A000003U02E01)+(A000003U03E01))=U05F0400S
```

**Logic:** `(Input1 AND Input2) OR Input3` triggers Output

---

## Function Codes (Output Actions)

Function codes define the action to perform on output devices. Different device types support different function codes.

### Common Function Codes

| Code | Name | Units | Description |
|------|------|-------|-------------|
| 0x03 | SET_VALUE | DIM | Set dimmer to value |
| 0x04 | ON_OFF_TOGGLE | SWITCH, DIM | Toggle On/Off |
| 0x09 | OFF | DIM, SWITCH | Turn off |
| 0x0A | ON | DIM, SWITCH | Turn on (restore) |
| 0x04 | MOTOR_UP | DUOSWITCH | Move up |
| 0x05 | MOTOR_DOWN | DUOSWITCH | Move down |
| 0x03 | MOTOR_STOP | DUOSWITCH | Stop motor |

### Mood Output Actions

When a mood is the **output** (target) of a binding, the old software interface shows three trigger options:

| Event Code | Old UI Label | Description |
|------------|--------------|-------------|
| 0x03 | "Event Short Pulse" | Instant trigger - pulse the mood |
| 0x02 | "Event Short Pulse + State" | Trigger with state tracking |
| 0x01 | "Event Long" | Long press trigger |

**Example - Button triggers mood:**
```
000011_80_0018_IU00E03>A0000FCU09FA6D0102S
                          ^^^^
                          FC = Node 252 (moods node)
                             U09 = Mood #9
```

### Function Code Details

**0xFA6:** Toggle/Schakel - Switch device state
```
U02FA6D0102S
   ^^^
   Function 0xFA6 = Toggle
```

**0xFA2:** Aan - Turn on
```
U02FA2D0102S
   ^^^
   Function 0xFA2 = On
```

**0xFA4:** Uit - Turn off
```
U02FA4D0102S
   ^^^
   Function 0xFA4 = Off
```

**0xFB6:** Dimmer/Motor Up
```
U02FB6D0103S
   ^^^
   Function 0xFB6 = Up/Dim Up
```

---

## Binding Flags

The 2-character flag field controls binding behavior:

| Flag | Value | Description |
|------|-------|-------------|
| Normal | `00` | Standard behavior |
| Inverted | `80` | Inverted polarity |
| Disabled | `01` | Binding disabled |

**Example:**
```
000003_80_0001_...
        ^^
        Inverted flag
```

---

## Data Format

Function data follows this pattern:

```
D[Length][Data]S
```

- `D` - Data marker
- `Length` - Hex length (2 chars)
- `Data` - Hex data bytes
- `S` - End marker

### Examples

**No data:**
```
D00S
```

**1 byte (value 50):**
```
D0132S
```

**2 bytes:**
```
D020A14S
```

---

## File Operations

### Reading Binding Files

```typescript
import { BindingFileReader } from './readers/BindingFileReader.js';

const bindingFile = await BindingFileReader.readFile('bind03.txt');

console.log('Node:', bindingFile.nodeAddress);
console.log('Bindings:', bindingFile.bindings.length);

bindingFile.bindings.forEach((binding, index) => {
  console.log(`${index}: ${binding.raw}`);
});
```

### Writing Binding Files

```typescript
import { BindingManager } from './services/BindingManager.js';

const manager = new BindingManager();

// Load existing
manager.load(3, bindingFile, 'bind03.txt');

// Add new binding
manager.addBinding(3, '000003_00_0999_NA000003U01E01=U02F0400S');

// Save to disk
await manager.writeToFile(3);
```

### Uploading to Hardware

```typescript
import { BindingUploader } from './communication/BindingUploader.js';

const uploader = new BindingUploader('192.168.1.100', 5001, '');
const result = await uploader.uploadToNode(3, manager);

console.log('Uploaded:', result.uploadedBindings);
```

---

## Normalization Rules

Before saving, binding strings are normalized:

1. **Convert to uppercase** - All characters except timer type letters
2. **Fix timer types** - `TE`→`Te`, `TI`→`Ti`, `TO`→`To`, `TD`→`Td`, `TF`→`Tf`, `TR`→`Tr`
3. **Insert underscore** - Ensure underscore at position 14
4. **Validate format** - Check syntax and structure

**Example:**
```
Input:  "00000a_00_0001_na00000au05=u02f0400s"
Output: "00000A_00_0001_NA00000AU05=U02F0400S"
```

---

## Validation

### Valid Binding Rules

✅ **Valid:**
- Node address matches file name (`bind03.txt` → `000003_...`)
- Unique binding numbers per node
- Valid unit references (node/unit exist)
- Correct syntax for binding type
- Proper data length

❌ **Invalid:**
- Non-ASCII characters
- Missing separators
- Invalid node addresses
- Duplicate binding numbers
- Malformed expressions

### Error Codes

| Code | Description |
|------|-------------|
| `RESULT_EC_SUCCESS` | Valid binding |
| `RESULT_EC_NON_ASCII` | Non-ASCII character |
| `RESULT_EC_NO_END` | Missing end marker |
| `RESULT_EC_NO_DATA` | Empty line |
| `RESULT_EC_NULL` | Null/empty |

---

## Best Practices

### 1. **Use Meaningful Binding Numbers**

Group related bindings:
```
0001-0099: Living room
0100-0199: Bedroom
0200-0299: Kitchen
```

### 2. **Document Complex Logic**

Add comments to `bindingconfiginfo.txt` explaining conditional bindings.

### 3. **Test Before Upload**

Always test bindings in the editor before uploading to hardware.

### 4. **Backup Before Changes**

Create backup copies before bulk modifications.

### 5. **Use Smart Upload**

Use `POST /api/upload/changed` instead of `/upload/all` to only upload modified nodes.

---

## Troubleshooting

### Binding Not Working

1. **Check syntax** - Validate format matches spec
2. **Verify addresses** - Ensure node/unit numbers are correct
3. **Check flags** - Wrong flags can invert behavior
4. **Test events** - Verify input unit generates correct events
5. **Upload to hardware** - Changes don't take effect until uploaded

### Parse Errors

**"Wrong binding syntax"**
- Check underscore positions (6, 9, 14)
- Verify all addresses use 6-digit format
- Ensure proper separators

**"Non-ASCII character"**
- File encoding must be ASCII
- Remove any special characters
- Check for hidden control characters

---

## Additional Resources

- [API Reference](API-REFERENCE.md) - REST API documentation
- [Hardware Upload Guide](HARDWARE-UPLOAD.md) - Upload procedures
- [Duotecno Protocol Spec](../specs/typescript-reader.md) - Low-level protocol
