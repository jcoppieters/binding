# TODO — Extra's

## open

- **Writing bindings to hardware (big task, not started — do NOT wire into the upload button yet)**
  - Current state: the "Upload to hardware" button (`/api/upload/changed|all`) only ever uploads
    whatever raw `bind*.txt` lines are loaded into `BindingManager` — which today come **only**
    from an eager legacy folder scan (`reader/configValies`, itself a stale/broken default — fixed
    the startup-noise part already). There is **no serializer** that turns our own project data
    (`project.bindings[]` visual wires, or the new `project.legacyBindings[]` raw entries below)
    back into `bind*.txt` line format. So today, "Upload to hardware" does not upload your actual
    project's bindings — this needs fixing before that button can be trusted.
  - Why this is hard, not just a quick wire-up: each binding line's `D[data]` payload is
    function-code-specific and not uniform. From real sample files:
    - Fixed/safe-looking: toggle `FA6` → always `D0102`, turn-on `FA2` → always `D010B` (same
      bytes in every sample seen).
    - Variable/unsafe to guess: `FA3` → `D0207FF` in one binding, `D0104` in another (looks like it
      encodes a level or channel — not derivable from samples alone); mood-trigger `F9F` encodes
      the target mood unit inside its data bytes.
    - Complex binding types (`C` conditional, `G` group, `P` property, `Te/Ti/To/Td/Tf/Tr` timers)
      have entirely different content grammars (expressions, operators, timer durations) with no
      serializer at all yet.
  - Getting a data payload wrong means uploading **incorrect commands to real relays/dimmers/motors**
    on live hardware — not a cosmetic bug. Do not guess; verify against
    `src/DTBindingSoftware/.../BindingFileEntryOperations.cpp` (or get confirmation from real
    hardware round-trip tests) function-code by function-code before wiring any of this into the
    actual upload path.
  - Suggested approach when this is picked up:
    1. Build a `BindingFileWriter` that only handles the verified-fixed function codes first
       (toggle/on/off), rejecting/warning on anything else.
    2. Extend function-code by function-code, using the C++ source as ground truth for the data
       payload — not inference from a handful of examples.
    3. Only then consider wiring project bindings into `BindingManager` before an upload call.

- **Complex bindings are now preserved on import, not yet reconstructable**
  - `project.legacyBindings[]` (see `models/project.ts`) now holds the exact original line text
    for every Conditional/Group/Property/Timer binding encountered during import — previously these
    were parsed, counted, and then silently discarded (the import summary said "opgeslagen als
    legacy" but nothing was actually saved). They're preserved verbatim now so nothing is lost, but
    there is still no code that turns them back into upload-ready binding lines — that's the same
    task as above.

- audit
  - of inline styles versus classes
  - the use of classes
  - why inline style, when it can be done with a class.
  - Style definitions in .html files, but also in .css files

- connection from the html/public part of the project, the UI to hardware
  - use a from the UI a WebSocket to server
  - Servert connects to the master probably like now, normal TCP socket
  - Live view in all devices of the "Home View", I mean lamp on/off, motor open/closed, dimmer on/off/percentage, etc... 
  - and then buttons: clickable -> sending commands over the websocket.

  for how to, etc... look into the /Users/johan/Development/duotecno/duotecnopro (probably ../duotecnopro)


- clean up of files
  - in public and src
  - old binding testing
  - don't throw await files that give us insights on how to write and upload binding files

- datastructures in the xxx.duo file
  - there is a node address in every unit ?? event state/status/value is not needed, but we need it in memory once connected.
  - I see multiple definitions of the same units and/or nodes, can they all refer to an array such as the "discovered nodes", all others (rooms, rail, bindings) could refer to there by node and unit address.
  - Mark the correct module as "master" 

---

## questions
- Why are there still "imported-id-xxx" in the MG.duo file?
- rooms with: "isMultiUnit": true -> nodeAddres + array of buttons and sensors
  - could that also be generic? List of units? with their type (button/sensor/... other?)
  - activeUnit? ("activeButton": 0, "activeSensor": false)? 
  - Difference between channelRef and nodeAddres? Why channelRef for non-multi devices.
- Binding array
  - with id's and node+unit? why? If a node/unit doesn't exist, create it?
  - deviceId why?
- moods:
  - why "channelRef"? with nodeAddress/unitAddress
  
