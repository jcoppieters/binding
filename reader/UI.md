# UI

ok, we now have the ./public/demo1-5.html, it's a good start.

General: we need to keep the size of the html generating code small, so we can easily work on 1 part and don't up with very long html generating code. So all small building blocks in kind of a nice library folder structure. Same for the modals.

## What is missing?

1) an extra tab with an overview of available resources:
- relais: total: 24 / used: 3
- dimmers: 16 / 9 used
- digital inputs: 16 / ... 
etc
Need, so the installer can add more modules if he needs more and we can show him which modules available to choose from.

2) When connecting for example a switch to a dimmer/lamp
- a box for the switch with outputs: long, short
- a box for the dimmer with inputs: toggle, on, off, dim-up, dim-down, on-by-pir, set-dim-value
- whe should be able to draw lines between the output and the inputs.
- We need this for a number of types: mood actions, up/down motors, relais, analog inputs, virtual relais, ...

3) We need NOT, AND and OR blocks to put between inputs and output. So we can enable disable input signals. "add logic" -> "choose" -> use when drawing lines.

4) When adding lights/dimmers, up/downs, etc... we don't need to select a module first, for relais we should show the list of available relais (we could show from which module they come, but it should not matter), for dimmers we could also show the type of dimmer (phase, leading/trailing edge, 12v, 230v, ...)

5) when adding lights, etc... in a room, we need to be able to give the light a name.

6) we need to be able to add rooms. And move them on our layout (already works I think). Perhaps if possible click on them and then only see this room in full browser window width.

7) perhaps we need json files for the description of available modules with a list of types in the modules (4 x leading edge dimmer, 8 x inputs, 2 up/downs), the power usage, the width in "M"s (18mm I think)

8) for the cabinet used, we need to specify how many DIN rails (or be able to add rails) + how wide it is in "M"s.

9) we need to be able to add CAN bus switches and LCD.

10) LCD's can have a specific configuration window with multiple panes in a modal-window. But actually all modules / switches will have their own configuration modal to view and/or edit settings and attributes.

11) We need a mood editor, but I already have 1 in the mobile app also in a modal-window.

modal-windows, can be real browser windows or in window modals that are added to the html tree and removed when done.

## to be researched

1) Can we query an installation and get all info needed to continue configuration (of course we won't have rooms etc... so all in 1 room, we won't have placement in a cabinet, so we just need to generate it, ...)