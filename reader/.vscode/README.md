# VS Code Configuration for Duotecno Reader

This folder contains VS Code configurations for debugging and building the TypeScript project.

## 🐛 Debugging

Three debug configurations are available in the Debug panel (F5):

### 1. Debug TypeScript (tsx) - **Recommended**
- Directly debugs TypeScript files using tsx
- No build step required
- Set breakpoints in `.ts` files
- Uses default config path (`./configGM`)

### 2. Debug Compiled JS
- Debugs the compiled JavaScript
- Automatically builds TypeScript before debugging
- Uses source maps to map back to `.ts` files
- Good for production-like debugging

### 3. Debug with Custom Config Path
- Same as option 1 but explicitly sets configGM path
- Modify the `args` in launch.json to change the path

## 🔨 Build Tasks

Available via `Terminal > Run Task...` or `Cmd/Ctrl + Shift + B`:

### TypeScript Watch (Default Build Task)
- **Command**: `tsc: watch - tsconfig.json`
- Continuously compiles TypeScript in the background
- Shows errors in the Problems panel
- Press `Cmd/Ctrl + Shift + B` to start

### TypeScript Build (One-time)
- **Command**: `tsc: build - tsconfig.json`
- Compiles TypeScript once
- Used as pre-launch task for "Debug Compiled JS"

### NPM Watch
- **Command**: `npm: watch`
- Runs `npm run watch` (which runs `tsc --watch`)
- Alternative to the TypeScript watch task

## 🚀 Quick Start

1. **Start the watch task** (optional but recommended):
   - Press `Cmd/Ctrl + Shift + B`
   - Select `tsc: watch - tsconfig.json`
   - This will show TypeScript errors as you type

2. **Start debugging**:
   - Open `src/index.ts`
   - Set a breakpoint (click left of line number)
   - Press `F5` or go to Run and Debug panel
   - Select "Debug TypeScript (tsx)"
   - The debugger will stop at your breakpoint

## 📁 Output

- Compiled JavaScript: `dist/` folder
- Source maps: `dist/**/*.js.map`
- Type declarations: `dist/**/*.d.ts`

## 💡 Tips

- Use the integrated terminal to see console output
- TypeScript errors appear in the Problems panel (`Cmd/Ctrl + Shift + M`)
- The watch task will auto-restart when you save files
- Breakpoints work in TypeScript files when using tsx or source maps
