# 3DSBlox

A dependency-free, browser-playable prototype inspired by the Roblox 3DS blueprint. It provides a small block world, a controllable block-built character, two-screen-style controls, a basic build mode, and an experimental importer for simple `.rbxlx` XML parts.

## Run it

Open `index.html` in a modern browser. No build step or external libraries are required. On a phone or tablet, use the lower touch workspace and controls.

## Controls

- **Move:** WASD or arrow keys; lower-screen ◀ / ▶ controls also move.
- **Jump:** Space (or tap JUMP).
- **Camera:** drag the game view or lower workspace.
- **Build:** tap STUDIO, choose a color slot, then tap the lower workspace to place a block.
- **Reset:** use the Reset world button.
- **Import:** use Import `.rbxlx` place for a best-effort import of basic `Part`, `WedgePart`, and `SpawnLocation` XML entries.

## Current scope and limitations

This is an early browser prototype, not a Roblox client, emulator, or native Nintendo 3DS homebrew build. It does not connect to Roblox services or run Roblox scripts. Native binary `.rbxl` parsing, complete `.rbxlx` property/CFrame mapping, real 3D rendering/stereoscopy, the 3DS hardware APIs and system keyboard, full R6 animation/physics, networking, and production memory/performance limits remain future work. The displayed 3D toggle is a visual prototype indicator, not actual stereoscopic rendering.

The initial implementation deliberately prioritizes a runnable game loop and build/play interaction before attempting format compatibility or device-specific rendering. See the supplied project blueprint in the repository conversation as the target specification.
