#include <3ds.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAP_W 24
#define MAP_H 14
#define BOT_COUNT 3
#define ROUND_SECONDS 120
#define TASK_COUNT 3

typedef enum { ROLE_SURVIVOR, ROLE_DETECTIVE, ROLE_CULPRIT } Role;
typedef enum { MODE_TITLE, MODE_ROUND, MODE_END } Mode;
typedef struct { int x, y; Role role; bool alive; } Actor;

static PrintConsole topConsole, bottomConsole;
static Mode mode = MODE_TITLE;
static Role playerRole = ROLE_SURVIVOR;
static Actor player;
static Actor bots[BOT_COUNT];
static char terrain[MAP_H][MAP_W];
static bool taskDone[TASK_COUNT];
static const int taskX[TASK_COUNT] = {4, 18, 12};
static const int taskY[TASK_COUNT] = {3, 4, 10};
static const int exitX = 21, exitY = 11;
static int evidence = 0;
static int roundTime = ROUND_SECONDS;
static int cooldown = 0;
static const char *endMessage = "";

static const char *roleName(Role role) {
    switch (role) {
        case ROLE_DETECTIVE: return "DETECTIVE";
        case ROLE_CULPRIT: return "CULPRIT";
        default: return "SURVIVOR";
    }
}

static void buildMap(void) {
    int x, y;
    for (y = 0; y < MAP_H; ++y) {
        for (x = 0; x < MAP_W; ++x) {
            terrain[y][x] = (x == 0 || y == 0 || x == MAP_W - 1 || y == MAP_H - 1) ? '#' : '.';
        }
        terrain[y][MAP_W] = '\0';
    }
    /* Original, simple arena layout. */
    for (x = 5; x < 10; ++x) terrain[5][x] = '#';
    for (y = 2; y < 7; ++y) terrain[y][14] = '#';
    for (x = 9; x < 15; ++x) terrain[9][x] = '#';
    terrain[5][7] = '.';
    terrain[4][14] = '.';
    terrain[9][12] = '.';
}

static bool blocked(int x, int y) {
    return x < 1 || y < 1 || x >= MAP_W - 1 || y >= MAP_H - 1 || terrain[y][x] == '#';
}

static bool occupied(int x, int y, const Actor *except) {
    int i;
    if (&player != except && player.alive && player.x == x && player.y == y) return true;
    for (i = 0; i < BOT_COUNT; ++i) {
        if (&bots[i] != except && bots[i].alive && bots[i].x == x && bots[i].y == y) return true;
    }
    return false;
}

static void placeActor(Actor *actor, int x, int y) {
    actor->x = x;
    actor->y = y;
    actor->alive = true;
}

static void startRound(void) {
    int i, culpritIndex, detectiveIndex;
    buildMap();
    memset(taskDone, 0, sizeof(taskDone));
    evidence = 0;
    roundTime = ROUND_SECONDS;
    cooldown = 0;
    culpritIndex = rand() % (BOT_COUNT + 1);
    do { detectiveIndex = rand() % (BOT_COUNT + 1); } while (detectiveIndex == culpritIndex);
    for (i = 0; i <= BOT_COUNT; ++i) {
        Role role = i == culpritIndex ? ROLE_CULPRIT : (i == detectiveIndex ? ROLE_DETECTIVE : ROLE_SURVIVOR);
        int x = 2 + rand() % (MAP_W - 4), y = 2 + rand() % (MAP_H - 4);
        Actor *actor = i == 0 ? &player : &bots[i - 1];
        int attempts = 0;
        while ((blocked(x, y) || occupied(x, y, actor)) && attempts++ < 100) {
            x = 2 + rand() % (MAP_W - 4);
            y = 2 + rand() % (MAP_H - 4);
        }
        placeActor(actor, x, y);
        actor->role = role;
        if (i == 0) playerRole = role;
    }
    mode = MODE_ROUND;
}

static void finishRound(const char *message) {
    endMessage = message;
    mode = MODE_END;
}

static void moveActor(Actor *actor, int dx, int dy) {
    int nx = actor->x + dx, ny = actor->y + dy;
    if (!blocked(nx, ny) && !occupied(nx, ny, actor)) {
        actor->x = nx;
        actor->y = ny;
    }
}

static void useAction(void) {
    int i;
    if (playerRole == ROLE_SURVIVOR) {
        for (i = 0; i < TASK_COUNT; ++i) {
            if (!taskDone[i] && player.x == taskX[i] && player.y == taskY[i]) {
                taskDone[i] = true;
                ++evidence;
                return;
            }
        }
        if (evidence == TASK_COUNT && abs(player.x - exitX) + abs(player.y - exitY) <= 1) {
            finishRound("You escaped with the evidence!");
        }
    } else if (playerRole == ROLE_DETECTIVE) {
        /* Prototype short-range action: click/tap near the culprit to arrest. */
        for (i = 0; i < BOT_COUNT; ++i) {
            if (bots[i].alive && bots[i].role == ROLE_CULPRIT &&
                abs(player.x - bots[i].x) + abs(player.y - bots[i].y) <= 3) {
                bots[i].alive = false;
                finishRound("The culprit was stopped!");
                return;
            }
        }
    } else {
        for (i = 0; i < BOT_COUNT; ++i) {
            if (bots[i].alive && abs(player.x - bots[i].x) + abs(player.y - bots[i].y) <= 1) {
                bots[i].alive = false;
                if (bots[i].role == ROLE_DETECTIVE) {
                    finishRound("The detective caught you!");
                    return;
                }
            }
        }
        for (i = 0; i < BOT_COUNT; ++i) {
            if (bots[i].alive && bots[i].role == ROLE_SURVIVOR) {
                finishRound("You tagged a player!");
                return;
            }
        }
    }
}

static void updateBots(void) {
    int i;
    static const int dx[4] = {1, -1, 0, 0};
    static const int dy[4] = {0, 0, 1, -1};
    for (i = 0; i < BOT_COUNT; ++i) {
        Actor *bot = &bots[i];
        int direction;
        if (!bot->alive) continue;
        if (bot->role == ROLE_CULPRIT && playerRole != ROLE_CULPRIT &&
            abs(bot->x - player.x) + abs(bot->y - player.y) <= 1) {
            player.alive = false;
            finishRound("You were tagged. Round over.");
            return;
        }
        if (bot->role == ROLE_CULPRIT && playerRole == ROLE_DETECTIVE &&
            abs(bot->x - player.x) + abs(bot->y - player.y) <= 3) continue;
        direction = rand() % 4;
        moveActor(bot, dx[direction], dy[direction]);
    }
}

static void drawTitle(void) {
    consoleSelect(&topConsole);
    printf("\x1b[2J\x1b[H");
    printf("        MYSTERY NIGHT\n\n");
    printf("  A tiny original 3DS mystery game\n\n");
    printf("       . . . # . . .\n");
    printf("       . ? . . ! . .\n");
    printf("       . . . # . . .\n\n");
    printf("   Press A to start a round\n");
    printf("   Press START to exit\n");
    consoleSelect(&bottomConsole);
    printf("\x1b[2J\x1b[H");
    printf("  MYSTERY NIGHT - 3DS\n\n");
    printf("  Original social-deduction\n");
    printf("  game prototype.\n\n");
    printf("  Circle Pad / D-pad: move\n");
    printf("  A: role action / interact\n");
    printf("  B: return to title\n\n");
    printf("  Offline prototype only.\n");
}

static void drawRound(void) {
    char display[MAP_H][MAP_W + 1];
    int x, y, i;
    memcpy(display, terrain, sizeof(display));
    display[exitY][exitX] = 'E';
    if (playerRole == ROLE_SURVIVOR) {
        for (i = 0; i < TASK_COUNT; ++i) if (!taskDone[i]) display[taskY[i]][taskX[i]] = '*';
    }
    for (i = 0; i < BOT_COUNT; ++i) if (bots[i].alive) display[bots[i].y][bots[i].x] = bots[i].role == ROLE_CULPRIT ? '!' : (bots[i].role == ROLE_DETECTIVE ? 'D' : 'o');
    display[player.y][player.x] = '@';
    consoleSelect(&topConsole);
    printf("\x1b[2J\x1b[H");
    printf(" MYSTERY NIGHT  %3d sec\n", roundTime);
    printf("########################\n");
    for (y = 0; y < MAP_H; ++y) printf("%s\n", display[y]);
    printf("########################\n");
    printf(" @ YOU  * EVIDENCE  E EXIT\n");
    printf(" ! CULPRIT  D DETECTIVE  o\n");
    consoleSelect(&bottomConsole);
    printf("\x1b[2J\x1b[H");
    printf("  ROLE: %s\n\n", roleName(playerRole));
    if (playerRole == ROLE_SURVIVOR) {
        printf("  Evidence: %d / %d\n", evidence, TASK_COUNT);
        printf("  Find all * then reach E.\n\n");
        printf("  Stand on * and press A.\n");
    } else if (playerRole == ROLE_DETECTIVE) {
        printf("  Stop the culprit (!).\n\n");
        printf("  Get within 3 tiles and A.\n");
    } else {
        printf("  Tag the other players.\n\n");
        printf("  Move next to a character\n");
        printf("  and press A.\n");
    }
    printf("\n  Circle Pad / D-pad: move\n");
    printf("  B: give up / title screen\n");
}

static void drawEnd(void) {
    consoleSelect(&topConsole);
    printf("\x1b[2J\x1b[H");
    printf("\n\n       ROUND COMPLETE\n\n");
    printf("       %s\n\n", endMessage);
    printf("       Press A to play again\n");
    consoleSelect(&bottomConsole);
    printf("\x1b[2J\x1b[H");
    printf("  Your role: %s\n\n", roleName(playerRole));
    printf("  Press A for another round.\n");
    printf("  Press B for the title screen.\n");
}

int main(void) {
    bool needsDraw = true;
    u64 lastSecond = osGetTime();
    srand((unsigned int)time(NULL));
    gfxInitDefault();
    consoleInit(GFX_TOP, &topConsole);
    consoleInit(GFX_BOTTOM, &bottomConsole);
    consoleSelect(&topConsole);
    while (aptMainLoop()) {
        hidScanInput();
        u32 down = hidKeysDown();
        u32 held = hidKeysHeld();
        CirclePosition circle;
        hidCircleRead(&circle);
        if (down & KEY_START) break;
        if (mode == MODE_TITLE) {
            if (down & KEY_A) { startRound(); lastSecond = osGetTime(); needsDraw = true; }
        } else if (mode == MODE_END) {
            if (down & KEY_A) { startRound(); lastSecond = osGetTime(); needsDraw = true; }
            if (down & KEY_B) { mode = MODE_TITLE; needsDraw = true; }
        } else {
            if (down & KEY_B) { mode = MODE_TITLE; needsDraw = true; }
            if (cooldown > 0) --cooldown;
            if (cooldown == 0) {
                int dx = 0, dy = 0;
                if (held & KEY_LEFT || circle.dx < -60) dx = -1;
                else if (held & KEY_RIGHT || circle.dx > 60) dx = 1;
                else if (held & KEY_UP || circle.dy > 60) dy = -1;
                else if (held & KEY_DOWN || circle.dy < -60) dy = 1;
                if (dx || dy) { moveActor(&player, dx, dy); cooldown = 7; needsDraw = true; }
            }
            if ((down & KEY_A) && mode == MODE_ROUND) { useAction(); needsDraw = true; }
            if (mode == MODE_ROUND && (osGetTime() - lastSecond >= 1000)) {
                u64 now = osGetTime();
                roundTime -= (int)((now - lastSecond) / 1000);
                lastSecond += ((now - lastSecond) / 1000) * 1000;
                updateBots();
                if (mode == MODE_ROUND && roundTime <= 0) finishRound("Time ran out!");
                needsDraw = true;
            }
        }
        if (needsDraw) {
            if (mode == MODE_TITLE) drawTitle();
            else if (mode == MODE_ROUND) drawRound();
            else drawEnd();
            needsDraw = false;
        }
        gspWaitForVBlank();
    }
    gfxExit();
    return 0;
}
