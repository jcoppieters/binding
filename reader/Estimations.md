## Work estimate: Full Configuration Software system

Based on the current codebase state (Foundation, Rail View, Connection all done).

### What "working system" means per your list

| Feature group | Phase | Complexity | Estimate |
|---|---|---|---|
| P1-5/6 + P7-1/2 *(this session)* | P1/P7 | Low | **2–3 days** |
| Home view — rooms, floors, add devices | P2 | Medium | **4–5 days** |
| Visual wiring diagram — connect inputs to dimmers / relays / motors | P3 core | **High** | **8–12 days** |
| Logic blocks (NOT/AND/OR) + timers in wiring | P3 advanced | High | **3–4 days** |
| Serialize wiring → `bind*.txt` + upload to hardware | P7 + P3-6 | High | **4–5 days** |
| Per-module config: Smartbox slots, switch labels, LCD config | P6 | Medium | **3–4 days** |
| Materiaallijst (BOM) + Excel/PDF export | P9 | Low | **2–3 days** |
| Moods editor (per LCD node) | P4 | Medium | **3–4 days** |
| Scheduling editor (weeks/days/alarms) | P5 | Medium | **3–4 days** |
| Temperature sensor + heating/cooling config | P6 extension | Medium | **2–3 days** |
| Discovery diff panel + node-type lookup table | P8 | Low | **2–3 days** |

### Total

| Scenario | Days | Calendar time (1 dev) |
|---|---|---|
| Lean / optimistic | ~36 days | **~7–8 weeks** |
| Realistic (includes testing, iteration, edge cases) | ~50 days | **~10–12 weeks** |
| Conservative (unknowns, hardware access gaps) | ~65 days | **~3.5 months** |

### Key risk factors

1. **Visual wiring diagram** is the largest single chunk (~40% of remaining work). It's novel UI (drag-and-drop, port model) with no prior code to build on.
2. **Binding serialization** (wiring → `bind*.txt`) requires deep protocol knowledge. Unknown edge cases will surface during integration with real hardware.
3. **Hardware access** — moods/schedules/temperature config can only be validated against a live Duotecno master. If the test install is unavailable, this adds overhead.
4. **Temperature/HVAC**: the protocol units are clear (`kRegister`, `kTemperature`) but the config options (setpoint, hysteresis, heating vs cooling mode) need to be confirmed with Duotecno engineers.

### Recommended milestone dates (to present)

| Milestone | What you can show | ETA |
|---|---|---|
| **M1 — Rail Designer** | Add/configure all modules, save/load project, resources panel | +3 weeks |
| **M2 — Home View** | Rooms, floors, devices assigned to rooms | +5–6 weeks |
| **M3 — Wiring MVP** | Wire switch → dimmer/relay/motor, upload to hardware | +10–12 weeks |
| **M4 — Full feature** | Moods, scheduling, alarms, BOM export, temperature/HVAC | +16–18 weeks |

---
