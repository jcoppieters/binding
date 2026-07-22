# Old Duotecno Software - Complete Menu Options by Device Type

This document lists ALL popup menu options from the old Duotecno binding software, extracted from the source code resource files and binding property strings.

Source files:
- `src/DTBindingSoftware/src/BindingSoftware.rc` (string resources)
- `src/DTBindingSoftware/src/app/AppBindings/AppBindingProperty/BindingPropertyStrings.h` (menu definitions)
- `src/DTBindingSoftware/src/BindingUIElement.h` (device-specific string entries)
- `src/DTBindingSoftware/src/resource.h` (resource ID numeric values)

---

## Quick Reference: Resource ID Ranges

- Event Codes (IDS_EV_*): 3048-3058, 5100-5103
- Function Codes - Dimmer (IDS_FC_DIMSET_*): 4000-4018
- Function Codes - Switch (IDS_FC_SWITCHSET_*): 4024-4031
- Function Codes - Sensor (IDS_FC_SENSSET_*): 4040-4063
- Function Codes - Control (IDS_FC_CONTROLSET_*): 4056-4059
- Function Codes - Virtual/Mood (IDS_FC_VIRTUALSET_*): 4088-4091
- Function Codes - IR TX (IDS_FC_IRTXSET_*): 4104-4108
- Function Codes - RC5 RX (IDS_FC_RC5RXSET_*): 4120-4121
- Function Codes - Motor (IDS_FC_DUOSWITCH_*): 4200-4209

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

---

## Appendix: Resource ID Numeric Values

From `src/DTBindingSoftware/src/resource.h` - complete mapping of IDS_* constants to numeric values.

### Event Codes (IDS_EV_*)
```
#define IDS_EV_CONTROLTOGGLE            3048    // "Event Long"
#define IDS_EV_CONTROLPULSTOGGLE        3049    // "Event Short Pulse + State"
#define IDS_EV_CONTROLPULS              3050    // "Event Short Pulse"
#define IDS_EV_SELECTOR                 3051    // "Selector Event"
#define IDS_EV_HEATING_ON               3053    // "Heating On"
#define IDS_EV_HEATING_OFF              3054    // "Heating Off"
#define IDS_EV_COOLING_ON               3055    // "Cooling On"
#define IDS_EV_COOLING_OFF              3056    // "Cooling Off"
#define IDS_EV_HEATING                  3057    // "Heating"
#define IDS_EV_COOLING                  3058    // "Cooling"
#define IDS_EV_SENSUNIV_INC             5100    // "Increment"
#define IDS_EV_SENSUNIV_DEC             5101    // "Decrement"
#define IDS_EV_SENSUNIV_INC_PULS        5102    // "Increment puls"
#define IDS_EV_SENSUNIV_DEC_PULS        5103    // "Decrement puls"
```

### Function Codes - Dimmer (IDS_FC_DIMSET_*)
```
#define IDS_FC_DIMSET_SELECTOR          4000    // "Selector Event"
#define IDS_FC_DIMSET_SPEED             4001    // "Set Dim speed"
#define IDS_FC_DIMSET_RANGE             4002    // "Set Dim Range"
#define IDS_FC_DIMSET_VALUE             4003    // "Set Dim Value"
#define IDS_FC_DIMSET_ONOFF             4004    // "Dim On/Off"
#define IDS_FC_DIMSET_UP                4005    // "Dim Up"
#define IDS_FC_DIMSET_DOWN              4006    // "Dim Down"
#define IDS_FC_DIMSET_UPDN              4007    // "Dim Up/Down"
#define IDS_FC_DIMSET_UPDNONOFF         4008    // "Up/Dn/On/Off"
#define IDS_FC_DIMSET_OFF               4009    // "Set Dim Off"
#define IDS_FC_DIMSET_ON                4010    // "Set Dim On"
#define IDS_FC_DIMSET_ONOFFCS           4011    // "On/Off f(Status)"
#define IDS_FC_DIMSET_VALUEOFF          4012    // "Dim Value/Off"
#define IDS_FC_DIMSET_CONFIG            4013    // "Config Dimmer"
#define IDS_FC_DIMSET_INC               4014    // "Inc. Dim Value"
#define IDS_FC_DIMSET_DEC               4015    // "Dec. Dim Value"
#define IDS_FC_DIMSET_ONUP              4016    // "Dim On Up"
#define IDS_FC_DIMSET_ONDN              4017    // "Dim On Down"
#define IDS_FC_DIMSET_PIR               4018    // "Dim Pir On"
```

### Function Codes - Switch (IDS_FC_SWITCHSET_*)
```
#define IDS_FC_SWITCHSET_SELECTOR       4024    // "Selector Event"
#define IDS_FC_SWITCHSET_ONOFF          4025    // "Switch On/Off"
#define IDS_FC_SWITCHSET_OFF            4026    // "Switch Off"
#define IDS_FC_SWITCHSET_ON             4027    // "Switch On"
#define IDS_FC_SWITCHSET_ONOFFFS        4028    // "On/Off f(Status)"
#define IDS_FC_SWITCHSET_CONFIG         4029    // "Config Switch"
#define IDS_FC_SWITCHSET_PIR            4030    // "Switch Pir On"
#define IDS_FC_SWITCHSET_TIMERVALUE     4031    // "Timed Value"
```

### Function Codes - Sensor/HVAC (IDS_FC_SENSSET_*)
```
#define IDS_FC_SENSSET_SELECTOR         4040    // "Selector Event"
#define IDS_FC_SENSSET_SETPOINT         4041    // "Working setpoint"
#define IDS_FC_SENSSET_HYSTVALUES       4042    // "Hysterisis Value"
#define IDS_FC_SENSSET_ONOFF            4043    // "Sensor On/Off"
#define IDS_FC_SENSSET_SETPOINTINCDEC   4044    // "Inc./Dec. setpoint"
#define IDS_FC_SENSSET_INC              4045    // "Inc. setpoint"
#define IDS_FC_SENSSET_DEC              4046    // "Dec. Setpoint"
#define IDS_FC_SENSSET_INTERVAL         4047    // "Sample Interval"
#define IDS_FC_SENSSET_SET_ONOFF        4048    // "On/Off f(Status)"
#define IDS_FC_SENSSET_HYSTOFFSET       4049    // "Hysterisis Offset"
#define IDS_FC_SENSSET_PRESET           4050    // "Set Preset"
#define IDS_FC_SENSSET_WORKINGMODE      4051    // "Workingmode"
#define IDS_FC_SENSSET_PIDUNIT          4052    // "PID Control unit"
#define IDS_FC_SENSSET_PIDPARAMS        4053    // "PID Control params"
#define IDS_FC_SENSSET_FANSPEED         4061    // "Fanspeed"
#define IDS_FC_SENSSET_SWINGANGLE       4062    // "Swing Angle"
#define IDS_FC_SENSSET_SWINGMODE        4063    // "Swing Mode"
```

### Function Codes - Control (IDS_FC_CONTROLSET_*)
```
#define IDS_FC_CONTROLSET_CHECK         4056    // "Check"
#define IDS_FC_CONTROLSET_PULS          4057    // "Short"
#define IDS_FC_CONTROLSET_PULS_STATUS   4058    // "Short Status"
#define IDS_FC_CONTROLSET_LONG          4059    // "Long"
```

### Function Codes - Virtual/Mood (IDS_FC_VIRTUALSET_*)
```
#define IDS_FC_VIRTUALSET_CHECK         4088    // "Check"
#define IDS_FC_VIRTUALSET_PULS          4089    // "Short Pulse"
#define IDS_FC_VIRTUALSET_PULSTOGGLE    4090    // "Short Pulse On/Off"
#define IDS_FC_VIRTUALSET_LONG          4091    // "Long On/Off"
```

### Function Codes - IR TX (IDS_FC_IRTXSET_*)
```
#define IDS_FC_IRTXSET_START            4104    // "Start IRTX"
#define IDS_FC_IRTXSET_STOP             4105    // "Stop IRTX"
#define IDS_FC_IRTXSET_PULS             4106    // "Send IR-Code"
#define IDS_FC_IRTXSET_CONFIG           4107    // "Config"
#define IDS_FC_IRTXSET_FLUSH            4108    // "Flush Databases"
```

### Function Codes - RC5 RX (IDS_FC_RC5RXSET_*)
```
#define IDS_FC_RC5RXSET_CODE            4120    // "Iniciate Receive Code"
#define IDS_FC_RC5RXSET_CONFIG          4121    // "Config"
```

### Function Codes - Motor/Duoswitch (IDS_FC_DUOSWITCH_*)
```
#define IDS_FC_DUOSWITCH_SELECTOR       4200    // "Selector Event"
#define IDS_FC_DUOSWITCH_UPSTOP         4201    // "Up / Stop"
#define IDS_FC_DUOSWITCH_DNSTOP         4202    // "Down / Stop"
#define IDS_FC_DUOSWITCH_STOP           4203    // "Stop"
#define IDS_FC_DUOSWITCH_UP             4204    // "Up"
#define IDS_FC_DUOSWITCH_DOWN           4205    // "Down"
#define IDS_FC_DUOSWITCH_UPDN           4206    // "Up / Down"
#define IDS_FC_DUOSWITCH_CONFIG         4207    // "Config"
#define IDS_FC_DUOSWITCH_STOPTIMER      4208    // "Config Stop Timer"
#define IDS_FC_DUOSWITCH_SWITCHTIMER    4209    // "Config switch Timer"
```

### Usage in Binding Files

When reading bind*.txt files, you'll see hex function codes like:
- `FA6` = 0xFA6 = Function code for mood/switch toggle
- `FA2` = 0xFA2 = Function code for mood status/switch on
- `FA4` = 0xFA4 = Function code for mood long/switch off

These function codes map to different ports depending on the target device type:
- **Moods**: 0xFA6→kort, 0xFA2→status, 0xFA4→lang (INPUT ports)
- **Switches/Dimmers**: 0xFA6→schakel, 0xFA2→aan, 0xFA4→uit (actions)
