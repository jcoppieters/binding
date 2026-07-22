# Test Binding Import Fix

## Problem
- Wires connecting to WRONG side of mood boxes (right instead of left)
- Missing bindings even though old software shows them

## Root Cause
- Project file has ZERO bindings: `"bindings": []`
- Need to import bindings with FIXED converter code

## Test Steps

### 1. Start Server
```bash
cd reader
npm run dev
```

### 2. Open Browser
- Go to http://localhost:3000
- Hard refresh (Cmd+Shift+R) to clear cache

### 3. Open Project
- Load "Grote_Moerstraat.duo"
- Click on "beneden" room

### 4. Import Bindings
- Click "Import Bindings" button
- Select folder: `reader/configGM/`
- Should find files:
  - bind03.txt (410 bytes) - Node 0x03
  - bind10.txt (424 bytes) - Node 0x10
  - bind11.txt (396 bytes) - Node 0x11 ← DTBS "Switch Living"
  - bindfc.txt (216 bytes) - Node 0xFC
- Click "Import"

### 5. Verify Import
Console should show:
```
[converter] Converting X bindings...
[converter] Converted X bindings, Y devices to create, Z moods to create
```

Project should now have bindings array populated.

### 6. Test Binding Panel
- Click on "DTBS-ES4w" device (Switch Living)
- Select Button 8
- Click "Bindings" button
- Should see:
  - DTBS-ES4w (left box) with Button 8 selected
  - "All On" mood (right box)
  - Wire from Button 8 "Kort" port (blue dot on RIGHT side of DTBS)
  - Wire TO "All On" "Kort" port (blue dot on LEFT side of mood) ✅

### 7. Verify Correct Side
**CORRECT:** Wire goes from right side of button → left side of mood
**WRONG:** Wire goes from right side of button → right side of mood

### Expected Bindings for Button 8

From bind11.txt:
```
000011_80_001F_IU04E03>A000010U09FA6D0102S  # Button 4, E03 → Node 10 Unit 09, FA6
000011_80_0020_IU04E01>A000010U0AFA6D0102S  # Button 4, E01 → Node 10 Unit 0A, FA6
```

Button indexes are 0-based, so Button 4 in binding file = Button 5 in UI (1-based).

Wait, let me check which button is actually button 8...

From screenshot, user selected button 8 and saw "All On" mood. Let me check bind11.txt for the actual button 8 (unit address 7):
```
000011_80_001A_IU07E03>A000010U03FA6D0102S  # Unit 07, E03 → Node 10 Unit 03
```

This should trigger mood at Node 16 (0x10) Unit 3.

## Success Criteria
✅ Wires appear when clicking buttons
✅ Wires connect to LEFT side of mood boxes (input ports)
✅ Multiple bindings visible matching old software screenshot
✅ Correct moods show (Curtains, All On, Breakfast, etc.)
