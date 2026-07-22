# Old Duotecno Software - Complete Menu Options by Device Type

This document lists ALL popup menu options from the old Duotecno binding software, extracted from the source code resource files and binding property strings.

Source files:
- `src/DTBindingSoftware/src/BindingSoftware.rc` (string resources)
- `src/DTBindingSoftware/src/app/AppBindings/AppBindingProperty/BindingPropertyStrings.h` (menu definitions)
- `src/DTBindingSoftware/src/BindingUIElement.h` (device-specific string entries)

---

## Event Codes (INPUT - What triggers a binding)

All input devices (buttons, sensors, moods when receiving) use these event types:

### Button/Control Input (DTBS, physical buttons)
```
Event Short Pulse              (E03 = 0x03)  - IDS_EV_CONTROLPULS
Event Short Pulse + State      (E02 = 0x02)  - IDS_EV_CONTROLPULSTOGGLE
─────────────────────
Event Long                     (E01 = 0x01)  - IDS_EV_CONTROLTOGGLE
```

**Note**: In practice, physical buttons (DTBS) only use E03 (Kort) and E01 (Lang), never E02.

### Mood/Virtual Input (LCD moods, virtual units when receiving)
```
Event Short Pulse              (E03 = 0x03)  - IDS_EV_CONTROLPULS
Event Short Pulse + State      (E02 = 0x02)  - IDS_EV_CONTROLPULSTOGGLE
─────────────────────
Event Long                     (E01 = 0x01)  - IDS_EV_CONTROLTOGGLE
```

Moods support ALL three event types (unlike buttons).

### Dimmer Input (when dimmer generates events)
```
Event Short Pulse + State      (E02)  - IDS_EVENT_STATE
```

### Switch/Relay Input (when switch generates events)
```
Event Short Pulse + State      (E02)  - IDS_EVENT_STATE
```

### Motor/Duoswitch Input
```
Up/Stop State                  (E02)  - IDS_EVENT_UPSTOP
Down/Stop State                (E01)  - IDS_EVENT_DOWNSTOP
```

### Temperature Sensor Input Events
```
Heating On                     - IDS_EV_HEATING_ON
Heating Off                    - IDS_EV_HEATING_OFF
Cooling On                     - IDS_EV_COOLING_ON
Cooling Off                    - IDS_EV_COOLING_OFF
Heating                        - IDS_EV_HEATING
Cooling                        - IDS_EV_COOLING
Increment                      - IDS_EV_SENSUNIV_INC
Decrement                      - IDS_EV_SENSUNIV_DEC
Increment puls                 - IDS_EV_SENSUNIV_INC_PULS
Decrement puls                 - IDS_EV_SENSUNIV_DEC_PULS
```

---

## Function Codes (OUTPUT - What action to perform)

### Mood/Virtual Output (LCD moods, virtual units when triggering)
```
Short Pulse                    - IDS_FC_VIRTUALSET_PULS           (VIRTUAL_OBJECT_METHOD_PULS)
Short Pulse On/Off             - IDS_FC_VIRTUALSET_PULSTOGGLE     (VIRTUAL_OBJECT_METHOD_PULSTOGGLE)
Long On/Off                    - IDS_FC_VIRTUALSET_LONG           (VIRTUAL_OBJECT_METHOD_TOGGLE)
```

**Hex Function Codes**:
- `0xFA6` = Short Pulse / Trigger mood
- `0xFA2` = Short Pulse On/Off (with state)
- `0xFA4` = Long On/Off (toggle with state)

### Dimmer Output - Extended Options
```
On/Off f(Status)               - IDS_FC_DIMSET_ONOFFCS            (DIMMER_OBJECT_METHOD_FSCVONOFF)
Dim Value/Off                  - IDS_FC_DIMSET_VALUEOFF           (DIMMER_OBJECT_METHOD_FSCVOFF)
─────────────────────
Dim Pir On                     - IDS_FC_DIMSET_PIR                (DIMMER_OBJECT_METHOD_PIRON)
─────────────────────
Dim Up/Down                    - IDS_FC_DIMSET_UPDN               (DIMMER_OBJECT_METHOD_FSUD)
Up/Dn/On/Off                   - IDS_FC_DIMSET_UPDNONOFF          (DIMMER_OBJECT_METHOD_FSUDONOFF)
Dim Up                         - IDS_FC_DIMSET_UP                 (DIMMER_OBJECT_METHOD_UP)
Dim Down                       - IDS_FC_DIMSET_DOWN               (DIMMER_OBJECT_METHOD_DN)
─────────────────────
Dim On/Off                     - IDS_FC_DIMSET_ONOFF              (DIMMER_OBJECT_ATTR_ONOFF)
Set Dim Off                    - IDS_FC_DIMSET_OFF                (DIMMER_OBJECT_METHOD_OFF)
Set Dim On                     - IDS_FC_DIMSET_ON                 (DIMMER_OBJECT_METHOD_ONCV)
Set Dim Value                  - IDS_FC_DIMSET_VALUE              (DIMMER_OBJECT_ATTR_VALUE)
Set Dim Range                  - IDS_FC_DIMSET_RANGE              (DIMMER_OBJECT_ATTR_RANGE)
─────────────────────
Dim On Up                      - IDS_FC_DIMSET_ONUP               (DIMMER_OBJECT_METHOD_ONUP)
Dim On Down                    - IDS_FC_DIMSET_ONDN               (DIMMER_OBJECT_METHOD_ONDN)
─────────────────────
Dec. Dim Value                 - IDS_FC_DIMSET_DEC                (DIMMER_OBJECT_METHOD_DEC)
Inc. Dim Value                 - IDS_FC_DIMSET_INC                (DIMMER_OBJECT_METHOD_INC)
```

### Dimmer Output - Basic Options
```
On/Off f(Status)               - IDS_FC_DIMSET_ONOFFCS
Dim Value/Off                  - IDS_FC_DIMSET_VALUEOFF
─────────────────────
Dim Pir On                     - IDS_FC_DIMSET_PIR
─────────────────────
Dim Up/Down                    - IDS_FC_DIMSET_UPDN
Dim Up                         - IDS_FC_DIMSET_UP
Dim Down                       - IDS_FC_DIMSET_DOWN
─────────────────────
Set Dim Off                    - IDS_FC_DIMSET_OFF
Set Dim On                     - IDS_FC_DIMSET_ON
Set Dim Value                  - IDS_FC_DIMSET_VALUE
─────────────────────
Dec. Dim Value                 - IDS_FC_DIMSET_DEC
Inc. Dim Value                 - IDS_FC_DIMSET_INC
```

### Dimmer Output - Simple Options
```
On/Off f(Status)               - IDS_FC_DIMSET_ONOFFCS
─────────────────────
Dim Up/Down                    - IDS_FC_DIMSET_UPDN
Dim Up                         - IDS_FC_DIMSET_UP
Dim Down                       - IDS_FC_DIMSET_DOWN
─────────────────────
Dim Pir On                     - IDS_FC_DIMSET_PIR
```

### Switch/Relay Output - Extended Options
```
On/Off f(Status)               - IDS_FC_SWITCHSET_ONOFFFS         (SWITCH_OBJECT_METHOD_FSONOFF)
Switch Off                     - IDS_FC_SWITCHSET_OFF             (SWITCH_OBJECT_METHOD_OFF)
Switch On                      - IDS_FC_SWITCHSET_ON              (SWITCH_OBJECT_METHOD_ON)
─────────────────────
Switch Pir On                  - IDS_FC_SWITCHSET_PIR             (SWITCH_OBJECT_METHOD_FSCVONOFFTIMEDPIR)
─────────────────────
Switch On/Off                  - IDS_FC_SWITCHSET_ONOFF           (SWITCH_OBJECT_ATTR_ONOFF)
```

### Switch/Relay Output - Basic Options
```
On/Off f(Status)               - IDS_FC_SWITCHSET_ONOFFFS
Switch Off                     - IDS_FC_SWITCHSET_OFF
Switch On                      - IDS_FC_SWITCHSET_ON
─────────────────────
Switch Pir On                  - IDS_FC_SWITCHSET_PIR
```

### Motor/Duoswitch Output - Extended Options
```
Up / Down                      - IDS_FC_DUOSWITCH_UPDN            (DUOSWITCH_OBJECT_METHOD_FSUPDOWN)
─────────────────────
Stop                           - IDS_FC_DUOSWITCH_STOP            (DUOSWITCH_OBJECT_METHOD_STOP)
Up                             - IDS_FC_DUOSWITCH_UP              (DUOSWITCH_OBJECT_METHOD_UP)
Down                           - IDS_FC_DUOSWITCH_DOWN            (DUOSWITCH_OBJECT_METHOD_DOWN)
─────────────────────
Up / Stop                      - IDS_FC_DUOSWITCH_UPSTOP          (DUOSWITCH_OBJECT_ATTR_UPSTOP)
Down / Stop                    - IDS_FC_DUOSWITCH_DNSTOP          (DUOSWITCH_OBJECT_ATTR_DOWNSTOP)
```

### Motor/Duoswitch Output - Basic Options
```
Up / Down                      - IDS_FC_DUOSWITCH_UPDN
─────────────────────
Stop                           - IDS_FC_DUOSWITCH_STOP
Up                             - IDS_FC_DUOSWITCH_UP
Down                           - IDS_FC_DUOSWITCH_DOWN
```

### Temperature Sensor/HVAC Output
```
On/Off f(Status)               - IDS_FC_SENSSET_SET_ONOFF         (SENS_OBJECT_METHOD_FSONOFF)
Sensor On/Off                  - IDS_FC_SENSSET_ONOFF             (SENS_OBJECT_ATTR_CONTROL_ONOFF)
Inc./Dec. setpoint             - IDS_FC_SENSSET_SETPOINTINCDEC    (SENS_OBJECT_METHOD_FSINC_DEC)
Inc. setpoint                  - IDS_FC_SENSSET_INC               (SENS_OBJECT_METHOD_INC)
Dec. Setpoint                  - IDS_FC_SENSSET_DEC               (SENS_OBJECT_METHOD_DEC)
Set Preset                     - IDS_FC_SENSSET_PRESET            (SENS_OBJECT_ATTR_PRESET)
Working setpoint               - IDS_FC_SENSSET_SETPOINT          (SENS_OBJECT_ATTR_WORKING_SET_POINT)
Workingmode                    - IDS_FC_SENSSET_WORKINGMODE       (SENS_OBJECT_ATTR_WORKING_MODE)
Fanspeed                       - IDS_FC_SENSSET_FANSPEED          (SENS_OBJECT_ATTR_FAN_SPEED)
Hysterisis Value               - IDS_FC_SENSSET_HYSTVALUES
Hysterisis Offset              - IDS_FC_SENSSET_HYSTOFFSET
Sample Interval                - IDS_FC_SENSSET_INTERVAL
PID Control unit               - IDS_FC_SENSSET_PIDUNIT
PID Control params             - IDS_FC_SENSSET_PIDPARAMS
Swing Angle                    - IDS_FC_SENSSET_SWINGANGLE
Swing Mode                     - IDS_FC_SENSSET_SWINGMODE
```

### IR TX (Infrared Transmitter) Output
```
Start IRTX                     - IDS_FC_IRTXSET_START
Stop IRTX                      - IDS_FC_IRTXSET_STOP
Send IR-Code                   - IDS_FC_IRTXSET_PULS
Config                         - IDS_FC_IRTXSET_CONFIG
Flush Databases                - IDS_FC_IRTXSET_FLUSH
```

### RC5 RX (IR Receiver) Input
```
Event Short Pulse              - IDS_RC5RXSET_PULS                (RC5RX_EVENT_PULS)
Event Short Pulse + State      - IDS_RC5RXSET_PULSTOGGLE          (RC5RX_EVENT_PULSTOGGLE)
─────────────────────
Event Long                     - IDS_RC5RXSET_LONG                (RC5RX_EVENT_TOGGLE)
```

### RC5 RX Output
```
Iniciate Receive Code          - IDS_FC_RC5RXSET_CODE
Config                         - IDS_FC_RC5RXSET_CONFIG
```

### Alarm Output
```
Alarm Arm                      - IDS_ALARM_ARM                    (ALARM_OBJECT_METHOD_ARM)
Alarm Keypad                   - IDS_ALARM_KEYPAD                 (ALARM_OBJECT_METHOD_KEYPAD)
```

### Bose Audio System Output (Extensive options)
```
Source On                      - IDS_IRAUDIO_SOURCEON             (AUDIO_OBJECT_ATTR_SOURCE_ON)
On/Off f(Status)               - IDS_IRAUDIO_FSCSONOFF            (AUDIO_OBJECT_METHOD_FSCSONOFF)
─────────────────────
Volume Up/Down                 - IDS_IRAUDIO_FSVOLUPDN            (AUDIO_OBJECT_METHOD_FSVOLUPDOWN)
Volume Up                      - IDS_IRAUDIO_VOLUP                (AUDIO_OBJECT_METHOD_VOLUP)
Volume Down                    - IDS_IRAUDIO_VOLDOWN              (AUDIO_OBJECT_METHOD_VOLDOWN)
Set Volume                     - IDS_IRAUDIO_VOLUME               (AUDIO_OBJECT_ATTR_VOLUME)
─────────────────────
Preset Up                      - IDS_IRAUDIO_PRESSUP              (AUDIO_OBJECT_METHOD_PRESUP)
Preset Down                    - IDS_IRAUDIO_PRESSDN              (AUDIO_OBJECT_METHOD_PRESDOWN)
Source Function                - IDS_IRAUDIO_SRC_FUNCTION         (AUDIO_OBJECT_METHOD_SOURCE_FUNCTION)
Destination Function           - IDS_IRAUDIO_DEST_FUNCTION        (AUDIO_OBJECT_METHOD_DESTINATION_FUNCTION)
─────────────────────
Source On                      - IDS_IRAUDIO_CSON                 (AUDIO_OBJECT_METHOD_CSON)
Off                            - IDS_IRAUDIO_OFF                  (AUDIO_OBJECT_METHOD_OFF)
On/Off                         - IDS_IRAUDIO_ONOFF                (AUDIO_OBJECT_ATTR_ONOFF)
─────────────────────
Source Inc                     - IDS_IRAUDIO_SOURCEINC            (AUDIO_OBJECT_METHOD_SOURCEINC)
Source Dec                     - IDS_IRAUDIO_SOURCEDEC            (AUDIO_OBJECT_METHOD_SOURCEDEC)
```

### Bose Audio System Input
```
Source On                      - IDS_BOSERS_SOURCEON              (BOSELF50_EVENT_SOURCEON)
Source Off                     - IDS_BOSERS_SOURCEOFF             (BOSELF50_EVENT_SOURCEOFF)
─────────────────────
Destination Off                - IDS_BOSERS_DESTOFF               (BOSELF50_EVENT_BOSERS_DESTOFF)
Destination On                 - IDS_BOSERS_DESTON                (BOSELF50_EVENT_BOSERS_DESTON)
```

### Bose Audio Virtual Input (Extended)
```
Source On                      - IDS_BOSERS_SOURCEON
Source Off                     - IDS_BOSERS_SOURCEOFF
─────────────────────
Preset AUX                     - IDS_BOSERS_PRESET_AUX
Tune AUX                       - IDS_BOSERS_TUNE_AUX
Preset Video 1                 - IDS_BOSERS_PRESET_VIDEO1
Tune Video 1                   - IDS_BOSERS_TUNE_VIDEO1
Preset Video 2                 - IDS_BOSERS_PRESET_VIDEO2
Tune Video 2                   - IDS_BOSERS_TUNE_VIDEO2
Preset Tape                    - IDS_BOSERS_PRESET_TAPE
Tune Tape                      - IDS_BOSERS_TUNE_TAPE
─────────────────────
Transport AUX                  - IDS_BOSERS_TRANSPORT_AUX
Transport Video 1              - IDS_BOSERS_TRANSPORT_VIDEO1
Transport Video 2              - IDS_BOSERS_TRANSPORT_VIDEO2
Transport Tape                 - IDS_BOSERS_TRANSPORT_TAPE
```

---

## Binding Operators (Logic gates and timers)

### Operators
```
AND operator                   - IDS_OPERATOR_AND
OR operator                    - IDS_OPERATOR_OR
XOR Operator                   - IDS_OPERATOR_XOR
TIMER Operator                 - IDS_OPERATOR_TIMER
```

### Timer Types
```
Edge delay                     - IDS_TIMER_TE
Input Delay                    - IDS_TIMER_TI
Output Delay                   - IDS_TIMER_TO
Falling edge delay             - IDS_TIMER_TF
Delay                          - IDS_TIMER_TD
Rising edge delay              - IDS_TIMER_TR
```

---

## Control Input Options (Additional)
```
Check                          - IDS_FC_CONTROLSET_CHECK
Short                          - IDS_FC_CONTROLSET_PULS
Short Status                   - IDS_FC_CONTROLSET_PULS_STATUS
Long                           - IDS_FC_CONTROLSET_LONG
```

---

## Notes

### Separator Entries
The string `IDS_SEPERATOR` (0xFF) is used throughout to create visual separators in dropdown menus, grouping related options together.

### Extended vs Basic vs Simple
Many device types have multiple option sets:
- **Extended**: Full feature set for advanced configurations
- **Basic**: Common operations without advanced features
- **Simple**: Minimal essential operations only

The old software likely shows different option sets based on:
- User preference/settings
- License level
- Device capabilities
- Context (expert mode vs basic mode)

### Event vs Function Codes
- **Event codes** (E01, E02, E03): Define what INPUT triggers a binding
- **Function codes** (F): Define what OUTPUT action to perform
- A binding connects: `Source Device + Event Code → [Operator] → Target Device + Function Code`

### Common Patterns
- **f(Status)**: Function depends on current state (toggle behavior)
- **PIR**: Motion sensor triggered action (often with timer)
- **Inc/Dec**: Increment or decrement values
- **FS** prefix: Function with status/state awareness
- **Attr** vs **Object/Method**: Attribute setting vs action execution
