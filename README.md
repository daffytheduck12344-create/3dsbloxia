# 3DSBlox

A native Nintendo 3DS homebrew project containing two original local games:

- **Night Halls** — solo key-and-door horror exploration with generated rooms, a roaming shadow on selected floors, hiding spots, three hearts, and a 200-floor goal.
- **Pocket Garden** — a small garden game where you plant and harvest crops while eight offline bots tend their own plots, seeds, and coin totals. The bot-name pool contains all 200 names supplied for this project, plus `toe_tickler67` and `john/doe` as display-name examples. The `john/doe` slash is treated as display text, not a valid Roblox username.

This is original homebrew code and ASCII presentation. It is not a Roblox client, does not use Roblox or DOORS assets, and does not connect to any service. Garden neighbors are local simulated bots; there is no online mode.

## Build

On a devkitPro machine with the Nintendo 3DS development tools installed and `DEVKITPRO` set, run:

```sh
make
```

The toolchain produces `3dsblox.3dsx` and may also produce `3dsblox.cia` depending on the installed template/toolchain. Copy the `.3dsx` to `sdmc:/3ds/3dsblox/` and run it using Homebrew Launcher. A real 3DS or compatible emulator is required; this is not a PC game.

## Controls

### Title screen
- **A:** Night Halls (single-player horror)
- **X:** Pocket Garden (offline bots)
- **START:** quit

### Night Halls
- **Circle Pad / D-pad:** move one tile
- **A:** hide in a marked closet / interact
- Find the key (`k`), then reach the exit (`D` changes to `>` after you have the key). Reach floor 200.
- The shadow appears on selected floors. Hide in `H` closets when it is nearby. You have three hearts.
- **B:** title screen

### Pocket Garden
- **A:** plant the selected seed on an empty plot or harvest a ready crop
- **L / R:** select one of your three plots
- **X:** cycle seed type
- **Y:** assign the next names from the full supplied bot-name roster
- Bots plant, grow, harvest, and replenish their own seeds independently.
- **B:** title screen

## Current limitations

The game is a first playable native prototype. Floor layouts are lightweight procedural tile rooms, the shadow uses simple grid movement, and garden growth is timer-based. Progress is not saved persistently yet. There is no online play, multiplayer, sound, 3D stereoscopy, or Roblox place-file support. The old browser mockup is no longer the game target.
