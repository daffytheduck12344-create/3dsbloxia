# 3DSBlox: Mystery Night

A native Nintendo 3DS homebrew prototype built with devkitPro/libctru. It is an original, single-player social-deduction game prototype—not a Roblox client and not an official Murder Mystery 2 remake. It uses original ASCII visuals and no Roblox/MM2 assets.

## Build on a devkitPro machine

Install devkitPro with the Nintendo 3DS development tools (`3ds-dev`) and make sure `DEVKITPRO` is set. From this repository root, run:

```sh
make
```

This produces `3dsblox.3dsx` and, where supported by the installed toolchain, `3dsblox.cia`. Copy the `.3dsx` to your 3DS SD card's `3ds/3dsblox/` directory and launch it with Homebrew Launcher. A real 3DS or emulator is required to play; this repository does not include a PC game build.

## Prototype gameplay

- The top screen shows a small, tile-based mystery arena.
- The bottom screen shows the player's role, controls, evidence, and round state.
- Circle Pad / D-pad move. A interacts: survivors collect evidence, the detective fires, and the culprit tags at close range. B returns to the title screen.
- Find all evidence and reach the exit as a survivor; stop the culprit as the detective; tag all the other characters as the culprit.
- The current NPC behavior and maps are deliberately small prototype placeholders.

## Online play

There is no online multiplayer in this build. Online play requires a server/protocol, identity and room management, authoritative game state, and a secure transport; the 3DS homebrew runtime does not make that appear automatically. See [`docs/ONLINE.md`](docs/ONLINE.md) for a safe, staged plan. Do not enter Nintendo Network credentials or real passwords into prototype software.

## Scope

This is an original homebrew game starter, not a Roblox engine. It does not load `.rbxl`/`.rbxlx`, run Roblox Lua, connect to Roblox, or implement the earlier 3D rendering/Studio blueprint. Native stereoscopic 3D, sound, richer maps, and online sessions remain future work. The old browser mockup in `index.html` is not the target build; the native source is under `source/`.
