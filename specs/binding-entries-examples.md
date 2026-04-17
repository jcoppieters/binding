# Binding Entry Examples

This document provides concrete examples of binding entries as written to `bind[xx].txt` files, with detailed breakdowns of each component.

## Example 1: Simple Control-to-Switch Binding (Type N - Normal)

### Scenario
Connect a control unit button (input) to a switch unit (output) for simple On/Off control.

### Binding String Format
```
0000[TargetNode]_[Flags]_[BindingNr]_NA0000[ReturnNode]U[ReturnUnit]=U[ForwardUnit]F[MessageCode]D[DataLen][Data]S
```

### Real Example
```
00000A_00_0001_NA00000AU05=U02F0400S
```

**Breakdown:**
- `00000A` - Target node address (Node 0x0A)
- `_` - Separator
- `00` - Binding flags
- `_` - Separator
- `0001` - Binding number (0x0001)
- `_` - Separator
- `N` - **Normal binding type**
- `A00000A` - Return unit address marker + node (0x0A)
- `U05` - Return unit number (Unit 5 - control input)
- `=` - Assignment operator
- `U02` - Forward unit number (Unit 2 - switch output)
- `F04` - Function/Message code (0x04 = SWITCH_OBJECT_METHOD_FSONOFF)
- `D` - Data marker
- `00` - Data length (0 bytes)
- `S` - End marker

**What it does:** When control unit 5 on node 0A sends a pulse event, switch unit 2 toggles On/Off based on state.

---

## Example 2: Immediate Binding (Type I)

### Scenario
Direct immediate binding from a control unit to a switch unit.

### Binding String Format
```
0000[ReturnNode]_[Flags]_[BindingNr]_IU[ReturnUnit]E[Event]>A0000[ForwardNode]U[ForwardUnit]F[MessageCode]D[DataLen][Data]S
```

### Real Example
```
000025_80_0010_IU03E03>A000025U02F0400S
```

**Breakdown:**
- `000025` - Target/Return node address (Node 0x25)
- `_` - Separator
- `80` - Binding flags (0x80 = normal polarity, 0x00 would be inverted)
- `_` - Separator
- `0010` - Binding number (0x0010)
- `_` - Separator
- `I` - **Immediate binding type**
- `U03` - Return unit number (Unit 3 - input)
- `E03` - Event code (0x03 = EV_UNITCONTROLPULS - control pulse event)
- `>` - Forward direction marker
- `A0000` - Address marker
- `25` - Forward node address (Node 0x25)
- `U02` - Forward unit number (Unit 2 - switch)
- `F04` - Function code (0x04 = SWITCH_OBJECT_METHOD_FSONOFF)
- `D` - Data marker
- `00` - Data length (0 bytes)
- `S` - End marker

**What it does:** Immediate response - when unit 3 (control) sends pulse event (0x03), unit 2 (switch) executes On/Off toggle function (0x04).

---

## Example 3: Conditional Binding (Type C)

### Scenario
Conditional binding with multiple inputs controlling one output.

### Binding String Format
```
0000[ForwardNode]_[Flags]_[BindingNr]_C([InputExpression])=[OutputExpression]
```

### Real Example
```
00000A_00_016D_C(A000025U03E01+A000025U04E01)=U02FB6D0103S
```

**Breakdown:**
- `00000A` - Forward/Target node address (Node 0x0A)
- `_` - Separator
- `00` - Binding flags
- `_` - Separator
- `016D` - Binding number (0x016D)
- `_` - Separator (added in version 0x0C06)
- `C` - **Conditional binding type**
- `(...)=...` - Conditional expression
  - `(` - Start of input condition
  - `A000025` - Address marker + Node address (0x25)
  - `U03` - Unit 3 (first control input)
  - `E01` - Event type (0x01 = EV_UNITCONTROLTOGGLE)
  - `+` - OR operator
  - `A000025U04E01` - Second control input (Unit 4, same event)
  - `)` - End of condition
  - `=` - Assignment
  - `U02` - Output unit (Unit 2 - switch)
  - `FB6` - Function with data (B6 = specific function)
  - `D01` - Data marker + length (1 byte)
  - `03` - Function data parameter
  - `S` - End marker

**What it does:** When either Unit 3 OR Unit 4 on node 0x25 sends a toggle event, Unit 2 on node 0x0A executes function 0xB6 with parameter 0x03.

---

## Example 4: Multi-Input with XOR Operator

### Scenario
Multiple control buttons controlling one switch with XOR logic (only one can be active).

### Real Example
```
00000F_00_0025_C(A00000FU01E02^A00000FU02E02)=U05F0400S
```

**Breakdown:**
- `00000F` - Target node (0x0F)
- `00` - Flags
- `0025` - Binding number
- `C` - Conditional
- `(A00000FU01E02^A00000FU02E02)` - Input expression:
  - Unit 1 event 0x02 (EV_UNITCONTROLPULSTOGGLE)
  - `^` - XOR operator
  - Unit 2 event 0x02
- `=U05F0400S` - Output: Unit 5, function 0x04, no data

**What it does:** Unit 1 XOR Unit 2 controls Unit 5 switch - prevents both from being pressed simultaneously.

---

## Common Event Codes

### Input Events (from Control Units)
| Code | Constant | Description |
|------|----------|-------------|
| 0x01 | EV_UNITCONTROLTOGGLE | Toggle event |
| 0x02 | EV_UNITCONTROLPULSTOGGLE | Pulse toggle event |
| 0x03 | EV_UNITCONTROLPULS | Simple pulse event |
| 0x04 | EV_UNITCONTROLSTATUS | Status event |

### Switch Function Codes
| Code | Constant | Description |
|------|----------|-------------|
| 0x02 | SWITCH_OBJECT_METHOD_OFF | Turn off |
| 0x03 | SWITCH_OBJECT_METHOD_ON | Turn on |
| 0x04 | SWITCH_OBJECT_METHOD_FSONOFF | Toggle On/Off based on state |
| 0x07 | SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR | PIR timed On/Off |

---

## Binding Types Summary

| Type | Symbol | Format | Use Case |
|------|--------|--------|----------|
| Normal | N | `N[Address]=[Unit]F[Function]D[Data]S` | Simple direct mapping |
| Immediate | I | `IU[Unit]E[Event]>[Address]U[Unit]F[Function]...` | Direct event-to-action |
| Conditional | C | `C([InputExpr])=[OutputExpr]` | Complex logic with operators |
| Group | G | `G[GroupDef]` | Group operations |
| Property | P | `P[Properties]` | Property bindings |
| Timer | Te/Ti/To/Td/Tf/Tr | With timer expressions | Timer-based actions |

---

## Operators in Conditional Bindings

| Symbol | Operator | Description |
|--------|----------|-------------|
| `+` | OR | Any input triggers output |
| `*` | AND | All inputs must be active |
| `^` | XOR | Only one input at a time |
| `!` | NOT | Inverts logic |

---

## Auto-Binding Generation

The software can automatically generate common binding patterns through the `CBindingConversionAuto` class:

### Switch Auto-Bindings
- **AUTO_BINDING_SWITCH_ONOFF**: Short event triggers On/Off toggle
- **AUTO_BINDING_SWITCH_PIR**: PIR sensor triggers 3-minute timer at 99%

Code reference: [BindingConversionAuto.cpp](../src/DTBindingSoftware/src/BindingConversionAuto.cpp#L1544)

### Dimmer Auto-Bindings
- **AUTO_BINDING_DIM_ONOFF**: Short event for On/Off
- **AUTO_BINDING_DIM_UPDN**: Long event for Dim Up/Down
- **AUTO_BINDING_DIM_PIR**: PIR sensor control

### DuoSwitch Auto-Bindings (Blinds/Shutters)
- **AUTO_BINDING_DUOSWITCH_UPDOWN**: Up/Down based on state
- **AUTO_BINDING_DUOSWITCH_STOP**: Stop command
- **AUTO_BINDING_DUOSWITCH_UP**: Move up
- **AUTO_BINDING_DUOSWITCH_DOWN**: Move down

---

## Building Binding Entries in Code

### Formatter Classes
The software uses specialized formatter classes to build binding strings:

1. **CBindingFileEntryFormatterN** - Normal bindings
   - File: [BindingFileEntryFormatterN.cpp](../src/DTBindingSoftware/src/app/AppBindings/AppBindingFileEntry/BindingFileEntryFormatterN.cpp)
   - Format: Node + Flags + Number + Return Address + Forward Unit + Function

2. **CBindingFileEntryFormatterI** - Immediate bindings  
   - File: [BindingFileEntryFormatterI.cpp](../src/DTBindingSoftware/src/app/AppBindings/AppBindingFileEntry/BindingFileEntryFormatterI.cpp)
   - Format: Node + Flags + Number + Unit + Event + Forward Address + Function

3. **CBindingFileEntryFormatterC** - Conditional bindings
   - File: [BindingFileEntryFormatterC.cpp](../src/DTBindingSoftware/src/app/AppBindings/AppBindingFileEntry/BindingFileEntryFormatterC.cpp)
   - Format: Node + Flags + Number + Conditional Expression

### Key Code Snippet (Normal Binding)
```cpp
m_szBindingFileEntry.Format(_T("0000%02X_%02X_%04X_NA0000%02XU%02X=U%02XF%02XD"),
    m_pForwardUnit->GetNodeAddress(),    // Forward node
    bBindingFlags,                        // Flags
    m_pBindingEntry->GetBindingNr(),     // Binding number
    m_pReturnUnit->GetNodeAddress(),     // Return node
    m_pReturnUnit->GetUnitAddress(),     // Return unit (input)
    m_pForwardUnit->GetUnitAddress(),    // Forward unit (output)
    m_pForwardUnit->BindingData.Get_MessageCode()); // Function code
```

---

## Related Documentation
- [Binding Files Format](binding-files-format.md) - Complete file structure specification
- [Unit Types](binding-files-format.md#unit-types-binding_unit_type_) - All available unit types
