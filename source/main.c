#include <3ds.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAP_W 24
#define MAP_H 14
#define BOT_COUNT 8
#define FLOOR_LIMIT 200
#define PLAYER_PLOTS 3
#define SEED_TYPES 3

typedef enum { MODE_TITLE, MODE_DOORS, MODE_GARDEN, MODE_DOORS_END } Mode;
typedef struct { int x, y; } Point;
typedef struct { int crop; int growth; } Plot;
typedef struct { const char *name; Plot plot; int seeds; int coins; int seedType; } GardenBot;

static const char *const botNames[] = {
    "NightPulse", "NeonVanguard", "FrostByte", "ZeroDrift", "CobaltNova", "VectorShade", "PulseRift", "CyberMoth", "VoltStride", "PrismFlux", "HyperKite", "StaticVex", "AstroNomad", "IonViper", "DriftCircuit", "SolisEdge", "QuantumRove", "GlitchWarden", "ArcSignal", "NyxHorizon", "TurboCipher", "ChromeJolt", "AeroSpark", "ZenithPulse", "FluxHarbor",
    "StudForge", "BrickSentinel", "BloxDrift", "ClassicCrusader", "RetroValkyrie", "SpawnPointZero", "StudlyVanguard", "WeldBreaker", "BaseplateKing", "SwordFencer", "TixCollector", "RocketHopper", "ObbyRunner", "CrateSmasher", "CrownSeeker", "StuddedFox", "BlockworkGuild", "ClassicNomad", "GearMastery", "PlanksAndStuds", "VintageBloxxer", "BrickyardAce", "RetroHarbor", "StudsOnly", "OldSchoolRift",
    "SoftMochi", "PeachCove", "VelvetMoth", "HoneyDewdrop", "CloudberryVale", "FernWhisper", "DuskLinen", "MoonlitTulle", "ApricotMist", "OatMilkAura", "WillowBreeze", "MossyLantern", "SugarPlumDrift", "DewyPetal", "LavenderFable", "CoralThistle", "CalmClementine", "StarlightWren", "CozyClover", "MilkTeaCloud", "MapleHaze", "PastelFerry", "BirchHollow", "HoneyedFern", "QuietSolstice",
    "LagWizard", "WaffleGoblin", "YeetTornado", "PotatoGrenade", "BonkCaptain", "SirDropsFrames", "SpicyNoodleLord", "HonkGremlin", "AccidentalHero", "ToastOnFire", "MarshmallowThief", "PingAt999", "GoofySpatula", "LawnMowerPro", "SockStealer", "PigeonGeneral", "BroccoliBrawler", "CheeseMeteor", "ChaosTaco", "BamboozleDuck", "SnaccBandit", "BananaRiot", "Discombobulated", "HamsterOverlord", "NaptimeNinja",
    "QuickFlick", "SharpStrafe", "ClutchVortex", "ApexReaper", "RapidKite", "SwiftRift", "CrispAim", "PhantomSprint", "BlitzCobra", "LethalDash", "KeenReflex", "GrimVanguard", "StrikerZero", "RazorPace", "VenomSurge", "TalonStrike", "RogueTempo", "HavocDrift", "ScopeHunter", "IronCinder", "NightstalkerX", "FiercePuma", "GhostStrider", "SavagePixel", "RushFury",
    "AshValkyrie", "ObsidianCrow", "HollowEclipse", "NocturneFang", "ThornSpecter", "CinderWraith", "DreadLantern", "RuinDrifter", "SableVesper", "UmbraWolf", "GloomHarbor", "BleakOracle", "CrypticRaven", "DuskRevenant", "ShadeMonarch", "MourningStarlight", "PhantomVesper", "BitterAsh", "OnyxLament", "ShadowedRook", "VoidSentinel", "NightshadeKite", "GrimSolace", "AshenCrown", "ColdEmber",
    "EmberPaladin", "StarforgedKnight", "WyvernTamer", "RuneKeeper", "SilverValkyrie", "DragonvaleBard", "SunstoneRanger", "FrostfangDruid", "MythicOsprey", "DawnbreakerFox", "AstralAlchemist", "StormholdArcher", "BriarwoodScout", "CobaltSorcerer", "SkywardMinstrel", "GildedGriffin", "HollowvaleMage", "IronbarkCleric", "SunfireLancer", "MooncrestVoyager", "CinderKnight", "ValeSeeker", "Runeweaver", "SpireWanderer", "WildwoodMonk",
    "Kairo", "Vexis", "Zorya", "Ryker", "LyraX", "Jettix", "Solen", "Nyxie", "Tavon", "Zephy", "Orin", "Kaida", "Bexley", "Corvo", "Halen", "Moxie", "Sorenz", "Talia", "Vespera", "Wrenly", "Zarek", "Arlo", "CleoV", "Dexon", "Fynn",
    "toe_tickler67", "john/doe"
};
#define BOT_NAME_COUNT ((int)(sizeof(botNames) / sizeof(botNames[0])))

static const char *const cropNames[SEED_TYPES] = {"Turnip", "Berry", "Starfruit"};
static const int cropValues[SEED_TYPES] = {3, 7, 15};
static PrintConsole topConsole, bottomConsole;
static Mode mode = MODE_TITLE;
static char room[MAP_H][MAP_W + 1];
static Point player = {2, 2}, exitDoor = {21, 11}, keyPos = {6, 3}, entity = {18, 9};
static int floorNumber = 1, hearts = 3, hiddenSeconds = 0, playerPlotIndex = 0, selectedSeed = 0;
static int playerSeeds[SEED_TYPES] = {3, 2, 0}, playerCoins = 0;
static Plot playerPlots[PLAYER_PLOTS];
static GardenBot bots[BOT_COUNT];
static int nameOffset = 0, gardenMinute = 0, doorsSeconds = 0, doorCooldown = 0;
static bool hasKey = false, entityActive = false;
static const char *doorEndText = "";

static int clamp(int value, int low, int high) { return value < low ? low : (value > high ? high : value); }
static int absInt(int n) { return n < 0 ? -n : n; }
static void clearConsole(void) { printf("\x1b[2J\x1b[H"); }

static void doorsBuildFloor(void) {
    int x, y;
    for (y = 0; y < MAP_H; ++y) {
        for (x = 0; x < MAP_W; ++x)
            room[y][x] = (x == 0 || y == 0 || x == MAP_W - 1 || y == MAP_H - 1) ? '#' : '.';
        room[y][MAP_W] = '\0';
    }
    /* Procedural room dividers with openings keep each floor navigable. */
    for (x = 2; x < MAP_W - 2; ++x) {
        if (x != 4 + (floorNumber * 3) % 14) room[4][x] = '#';
        if (x != 3 + (floorNumber * 5) % 15) room[8][x] = '#';
    }
    for (y = 2; y < MAP_H - 2; ++y) {
        if (y != 2 + floorNumber % 3 && y != 8 + floorNumber % 3) room[y][12] = '#';
    }
    room[4][4 + (floorNumber * 3) % 14] = '.';
    room[8][3 + (floorNumber * 5) % 15] = '.';
    room[2 + floorNumber % 3][12] = '.';
    room[8 + floorNumber % 3][12] = '.';
    /* A few deterministic obstacles vary the room without blocking its passages. */
    for (x = 0; x < 3; ++x) {
        int ox = 3 + (floorNumber * 7 + x * 5) % 17;
        int oy = 2 + (floorNumber * 3 + x * 4) % 9;
        if (room[oy][ox] == '.') room[oy][ox] = '#';
    }
    player.x = 2; player.y = 2;
    exitDoor.x = 21; exitDoor.y = 11;
    room[exitDoor.y][exitDoor.x] = '.';
    do { keyPos.x = 2 + rand() % (MAP_W - 4); keyPos.y = 2 + rand() % (MAP_H - 4); }
    while (room[keyPos.y][keyPos.x] != '.' || (keyPos.x == player.x && keyPos.y == player.y) || (keyPos.x == exitDoor.x && keyPos.y == exitDoor.y));
    do { entity.x = 2 + rand() % (MAP_W - 4); entity.y = 2 + rand() % (MAP_H - 4); }
    while (room[entity.y][entity.x] != '.' || (entity.x == keyPos.x && entity.y == keyPos.y));
    hasKey = false;
    hiddenSeconds = 0;
    entityActive = (floorNumber % 3 == 0 || floorNumber % 5 == 0);
    doorsSeconds = 0;
}

static void startDoors(void) {
    floorNumber = 1;
    hearts = 3;
    doorsBuildFloor();
    mode = MODE_DOORS;
}

static bool doorsBlocked(int x, int y) {
    return x < 1 || y < 1 || x >= MAP_W - 1 || y >= MAP_H - 1 || room[y][x] == '#';
}

static void doorsMove(int dx, int dy) {
    int nx = player.x + dx, ny = player.y + dy;
    if (!doorsBlocked(nx, ny)) {
        player.x = nx; player.y = ny;
        if (!hasKey && player.x == keyPos.x && player.y == keyPos.y) hasKey = true;
        if (entityActive && hiddenSeconds == 0 && player.x == entity.x && player.y == entity.y) {
            --hearts;
            if (hearts <= 0) { doorEndText = "The shadow caught you. Try another run."; mode = MODE_DOORS_END; }
            else { player.x = 2; player.y = 2; hiddenSeconds = 0; }
        }
        if (player.x == exitDoor.x && player.y == exitDoor.y && hasKey) {
            if (floorNumber >= FLOOR_LIMIT) { doorEndText = "You escaped all 200 floors!"; mode = MODE_DOORS_END; }
            else { ++floorNumber; doorsBuildFloor(); }
        }
    }
}

static void doorsAction(void) {
    if (room[player.y][player.x] == 'H') {
        hiddenSeconds = 5;
        return;
    }
    if (player.x == exitDoor.x && player.y == exitDoor.y && !hasKey) return;
    /* If the key is adjacent, collect it with A as well as by walking over it. */
    if (!hasKey && absInt(player.x - keyPos.x) + absInt(player.y - keyPos.y) <= 1) hasKey = true;
}

static void doorsUpdateEntity(void) {
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};
    int tries;
    if (!entityActive) return;
    if (hiddenSeconds > 0) --hiddenSeconds;
    if (hiddenSeconds == 0 && entity.x == player.x && entity.y == player.y) {
        --hearts;
        if (hearts <= 0) { doorEndText = "The shadow caught you. Try another run."; mode = MODE_DOORS_END; return; }
        player.x = 2; player.y = 2;
    }
    for (tries = 0; tries < 8; ++tries) {
        int d = rand() % 4, nx = entity.x + dx[d], ny = entity.y + dy[d];
        if (!doorsBlocked(nx, ny)) { entity.x = nx; entity.y = ny; break; }
    }
}

static void doorsDraw(void) {
    char display[MAP_H][MAP_W + 1];
    int x, y;
    memcpy(display, room, sizeof(display));
    display[exitDoor.y][exitDoor.x] = hasKey ? '>' : 'D';
    if (!hasKey) display[keyPos.y][keyPos.x] = 'k';
    for (x = 3; x < MAP_W - 3; x += 8) if (display[6][x] == '.') display[6][x] = 'H';
    if (entityActive) display[entity.y][entity.x] = '!';
    display[player.y][player.x] = hiddenSeconds > 0 ? 'h' : '@';
    consoleSelect(&topConsole); clearConsole();
    printf("  NIGHT HALLS   FLOOR %03d / 200\n", floorNumber);
    printf("  Hearts: %d   Key: %s   Shadow: %s\n", hearts, hasKey ? "yes" : "no", entityActive ? "ACTIVE" : "quiet");
    printf("  ########################\n");
    for (y = 0; y < MAP_H; ++y) printf("  %s\n", display[y]);
    printf("  ########################\n");
    printf("  @ you  k key  D locked  > exit  H hide  ! shadow\n");
    consoleSelect(&bottomConsole); clearConsole();
    printf("  NIGHT HALLS - SOLO\n\n");
    printf("  Find the key (k), then reach\n  the door. Reach floor 200.\n\n");
    printf("  Circle Pad / D-pad: move\n  A: hide in H / interact\n  B: return to title\n\n");
    printf("  Hidden: %d sec\n", hiddenSeconds);
}

static void initGardenBots(void) {
    int i;
    for (i = 0; i < BOT_COUNT; ++i) {
        bots[i].name = botNames[(nameOffset + i) % BOT_NAME_COUNT];
        bots[i].plot.crop = 0;
        bots[i].plot.growth = 0;
        bots[i].seeds = 2 + (i % 3);
        bots[i].coins = i * 2;
        bots[i].seedType = i % SEED_TYPES;
    }
}

static void refreshBotNames(void) {
    int i;
    nameOffset = (nameOffset + BOT_COUNT) % BOT_NAME_COUNT;
    for (i = 0; i < BOT_COUNT; ++i) bots[i].name = botNames[(nameOffset + i) % BOT_NAME_COUNT];
}

static void gardenAction(void) {
    Plot *plot = &playerPlots[playerPlotIndex];
    if (plot->crop == 0) {
        if (playerSeeds[selectedSeed] > 0) {
            --playerSeeds[selectedSeed];
            plot->crop = selectedSeed + 1;
            plot->growth = 0;
        }
    } else if (plot->growth >= 4) {
        int type = plot->crop - 1;
        playerCoins += cropValues[type];
        playerSeeds[type] += 1;
        plot->crop = 0;
        plot->growth = 0;
    }
}

static void gardenTick(void) {
    int i;
    ++gardenMinute;
    for (i = 0; i < PLAYER_PLOTS; ++i) if (playerPlots[i].crop && playerPlots[i].growth < 4) ++playerPlots[i].growth;
    for (i = 0; i < BOT_COUNT; ++i) {
        GardenBot *bot = &bots[i];
        if (bot->plot.crop) {
            if (bot->plot.growth < 4) ++bot->plot.growth;
            else {
                bot->coins += cropValues[bot->plot.crop - 1];
                ++bot->seeds;
                bot->plot.crop = 0;
                bot->plot.growth = 0;
            }
        } else if (bot->seeds > 0) {
            --bot->seeds;
            bot->plot.crop = bot->seedType + 1;
            bot->plot.growth = 0;
        }
    }
}

static void gardenDraw(void) {
    int i;
    consoleSelect(&topConsole); clearConsole();
    printf("  POCKET GARDEN  |  DAY %d\n\n", gardenMinute / 12 + 1);
    printf("  Your plots (A plants / harvests):\n");
    for (i = 0; i < PLAYER_PLOTS; ++i) {
        Plot *p = &playerPlots[i];
        printf("  %c Plot %d: %-9s %s\n", i == playerPlotIndex ? '>' : ' ', i + 1,
               p->crop ? cropNames[p->crop - 1] : "Empty",
               p->crop ? (p->growth >= 4 ? "READY!" : "growing") : "plant a seed");
    }
    printf("\n  Garden neighbors (each has own plot/seeds):\n");
    for (i = 0; i < BOT_COUNT; ++i) {
        GardenBot *b = &bots[i];
        printf("  %-20.20s %s %d  seed:%d coins:%d\n", b->name,
               b->plot.crop ? cropNames[b->plot.crop - 1] : "Empty",
               b->plot.growth, b->seeds, b->coins);
    }
    consoleSelect(&bottomConsole); clearConsole();
    printf("  POCKET GARDEN - OFFLINE\n\n");
    printf("  Coins: %d\n", playerCoins);
    printf("  Selected seed: %s\n", cropNames[selectedSeed]);
    printf("  Seeds: Turnip %d  Berry %d\n         Starfruit %d\n\n", playerSeeds[0], playerSeeds[1], playerSeeds[2]);
    printf("  A: plant / harvest ready plot\n");
    printf("  L / R: choose your plot\n");
    printf("  X: change seed type\n");
    printf("  Y: give neighbors new names\n");
    printf("  B: return to title\n\n");
    printf("  Bot name roster: %d names\n", BOT_NAME_COUNT);
}

static void drawTitle(void) {
    consoleSelect(&topConsole); clearConsole();
    printf("\n\n          3DSBLOX\n\n");
    printf("       TWO ORIGINAL GAMES\n\n");
    printf("   NIGHT HALLS: solo horror\n");
    printf("   POCKET GARDEN: grow with bots\n\n");
    printf("   A - Night Halls\n");
    printf("   X - Pocket Garden\n");
    printf("   START - quit\n");
    consoleSelect(&bottomConsole); clearConsole();
    printf("  NIGHT HALLS\n");
    printf("  Single-player procedural\n  key-and-door horror. 200 floors.\n\n");
    printf("  POCKET GARDEN\n");
    printf("  Plant, grow, and harvest with\n  eight independent garden bots.\n\n");
    printf("  Original homebrew prototype.\n");
    printf("  No network / online mode.\n");
}

static void drawDoorsEnd(void) {
    consoleSelect(&topConsole); clearConsole();
    printf("\n\n       NIGHT HALLS\n\n");
    printf("       %s\n\n", doorEndText);
    printf("       Reached floor %d\n\n", floorNumber);
    printf("       A: start over\n       B: title\n");
    consoleSelect(&bottomConsole); clearConsole();
    printf("  SOLO RUN COMPLETE\n\n  Press A to try again,\n  or B to return to the menu.\n");
}

int main(void) {
    bool needsDraw = true;
    u64 lastTick = osGetTime();
    srand((unsigned int)time(NULL));
    memset(playerPlots, 0, sizeof(playerPlots));
    initGardenBots();
    gfxInitDefault();
    consoleInit(GFX_TOP, &topConsole);
    consoleInit(GFX_BOTTOM, &bottomConsole);
    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        u32 held = hidKeysHeld();
        CirclePosition circle;
        hidCircleRead(&circle);
        if (down & KEY_START) break;
        if (mode == MODE_TITLE) {
            if (down & KEY_A) { startDoors(); needsDraw = true; }
            if (down & KEY_X) { mode = MODE_GARDEN; needsDraw = true; lastTick = osGetTime(); }
        } else if (mode == MODE_DOORS_END) {
            if (down & KEY_A) { startDoors(); needsDraw = true; }
            if (down & KEY_B) { mode = MODE_TITLE; needsDraw = true; }
        } else if (mode == MODE_DOORS) {
            if (down & KEY_B) { mode = MODE_TITLE; needsDraw = true; }
            if (doorCooldown > 0) --doorCooldown;
            if (doorCooldown == 0) {
                int dx = 0, dy = 0;
                if ((held & KEY_LEFT) || circle.dx < -60) dx = -1;
                else if ((held & KEY_RIGHT) || circle.dx > 60) dx = 1;
                else if ((held & KEY_UP) || circle.dy > 60) dy = -1;
                else if ((held & KEY_DOWN) || circle.dy < -60) dy = 1;
                if (dx || dy) { doorsMove(dx, dy); doorCooldown = 7; needsDraw = true; }
            }
            if ((down & KEY_A) && mode == MODE_DOORS) { doorsAction(); needsDraw = true; }
            if (mode == MODE_DOORS && osGetTime() - lastTick >= 1000) {
                u64 now = osGetTime();
                int ticks = (int)((now - lastTick) / 1000);
                lastTick += (u64)ticks * 1000;
                doorsSeconds += ticks;
                while (ticks-- > 0 && mode == MODE_DOORS) doorsUpdateEntity();
                needsDraw = true;
            }
        } else if (mode == MODE_GARDEN) {
            if (down & KEY_B) { mode = MODE_TITLE; needsDraw = true; }
            if (down & KEY_A) { gardenAction(); needsDraw = true; }
            if (down & KEY_L) { playerPlotIndex = (playerPlotIndex + PLAYER_PLOTS - 1) % PLAYER_PLOTS; needsDraw = true; }
            if (down & KEY_R) { playerPlotIndex = (playerPlotIndex + 1) % PLAYER_PLOTS; needsDraw = true; }
            if (down & KEY_X) { selectedSeed = (selectedSeed + 1) % SEED_TYPES; needsDraw = true; }
            if (down & KEY_Y) { refreshBotNames(); needsDraw = true; }
            if (osGetTime() - lastTick >= 1000) {
                u64 now = osGetTime();
                int ticks = (int)((now - lastTick) / 1000);
                lastTick += (u64)ticks * 1000;
                while (ticks-- > 0) gardenTick();
                needsDraw = true;
            }
        }
        if (needsDraw) {
            if (mode == MODE_TITLE) drawTitle();
            else if (mode == MODE_DOORS) doorsDraw();
            else if (mode == MODE_GARDEN) gardenDraw();
            else drawDoorsEnd();
            needsDraw = false;
        }
        gspWaitForVBlank();
    }
    gfxExit();
    return 0;
}
