# Development Instructions

## Running the Web Server

### Option 1: Using VS Code Debug (Recommended)

1. Open the **Run and Debug** panel (⇧⌘D)
2. Select **"Debug Web Server"** from the dropdown
3. Press F5 or click the green play button
4. Server will start at http://localhost:3000

**Important:** After making code changes to TypeScript files, you must:
1. Stop the debug session (Shift+F5 or red square button)
2. Restart with F5

The server automatically uses the `configValies` directory.

### Option 2: Using Terminal

```bash
npm run server:enhanced
```

Or with tsx directly:

```bash
npx tsx src/server-enhanced.ts
```

## Project Scripts

- `npm run dev` - Run CLI reader tool (not the web server!)
- `npm run server:enhanced` - Run web server
- `npm run build` - Compile TypeScript to JavaScript
- `npm run test:upload` - Test binding upload functionality

## Note on Auto-Save

**Bindings are automatically saved to disk** when you create, edit, or delete them. This ensures the UI displays correctly. The changes are written to the binding files in `configValies/`.

If you want to revert changes:
1. Keep backups of your binding files before testing
2. Or use git to track changes to the config files
