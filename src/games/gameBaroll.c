#include "cglp.h"

static char *title = "BAROLL";
static char *description = "[Tap]  Jump\n[Hold] Slow down";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS =  {
    {   // Barrel 'a'
        " llll ",
        "l    l",
        "l ll l",
        "llllll",
        "llllll",
        " llll ",
    },
    {   // Runner frame 1 'b' - simple running pose
        "  l   ",
        "  l   ",
        " lll  ",
        " l l  ",
        "  l   ",
        " l    ",
    },
    {   // Runner frame 2 'c' - alternate running pose
        "    l ",
        "   l  ",
        " lllll",
        "l ll  ",
        " l  l ",
        "l   l ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 9,
    .isDarkColor = false
};

typedef enum {
    FALL,
    ROLL
} BarrelMode;

typedef struct {
    Vector pos;
    float vy;
    float speed;
    BarrelMode mode;
    float angle;
    bool isAlive;
} Barrel;

typedef enum {
    RUN,
    JUMPING
} PlayerMode;

#define MAX_BARREL_COUNT 100
static Barrel barrels[MAX_BARREL_COUNT];
static int barrelIndex;
static float barrelAddingTicks;
static Vector pos;
static Vector vel;
static PlayerMode mode;
static float bx;
static float anim;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(barrels);
        barrelIndex = 0;
        barrelAddingTicks = 0;
        pos.x = 9;
        pos.y = 86;
        vel.x = 0;
        vel.y = 0;
        mode = RUN;
        bx = 0;
        anim = 0;
    }

    rect(0, 90, 200, 9);
    float df = sqrt(difficulty);
    barrelAddingTicks -= df;

    if (barrelAddingTicks < 0) {
        play(LASER);
        ASSIGN_ARRAY_ITEM(barrels, barrelIndex, Barrel, b);
        float minX = mode == RUN ? 10 : 100;
        b->pos.x = rnd(minX, 200);
        b->pos.y = -5;
        b->vy = 0;
        b->speed = rnd(1, df);
        b->mode = FALL;
        b->angle = rnd(0, 99);
        b->isAlive = true;
        barrelIndex = wrap(barrelIndex + 1, 0, MAX_BARREL_COUNT);
        barrelAddingTicks += rndi(30, 90);
    }

    vel.x = df * (input.isPressed ? 1 : 2);
    score += vel.x - df;

    FOR_EACH(barrels, i) {
        ASSIGN_ARRAY_ITEM(barrels, i, Barrel, b);
        SKIP_IS_NOT_ALIVE(b);

        if (b->mode == FALL) {
            b->vy += b->speed * 0.2;
            b->vy *= 0.92;
            b->pos.y += b->vy * sqrt(df);
            if (b->pos.y > 85) {
                play(SELECT);
                b->pos.y = 86;
                b->mode = ROLL;
            }
        } else {
            b->pos.x -= b->speed * df;
            b->angle += b->speed * df * 0.2;
        }
        b->pos.x -= vel.x;

        characterOptions.rotation = 3 - ((int)b->angle % 4);
        character("a", b->pos.x, b->pos.y);

        if (b->pos.x <= -5) {
            b->isAlive = false;
        }
    }

    if (mode == RUN) {
        if (input.isJustPressed) {
            play(JUMP);
            mode = JUMPING;
            vel.y = -3.6;
        }
    } else {
        pos.y += vel.y;
        vel.y += input.isPressed ? 0.1 : 0.2;
        if (pos.y > 85) {
            pos.y = 86;
            if (input.isPressed) {
                play(JUMP);
                vel.y = -3;
            } else {
                mode = RUN;
            }
        }
    }

    // Player animation and collision
    anim += df * (input.isPressed ? 0.1 : 0.2) * (mode == RUN ? 1 : 0.5);
    characterOptions.rotation = 0;  // Reset rotation
    char playerChar[2] = {'b' + ((int)floor(anim) % 2), '\0'};
    if (character(playerChar, pos.x, pos.y).isColliding.character['a']) {
        play(EXPLOSION);
        gameOver();
    }

    bx -= vel.x;
    if (bx < -9) {
        bx += 200;
    }
    color = LIGHT_BLACK;
    rect(bx, 90, 3, 9);
    color = BLACK;
}

void addGameBaroll() {
    addGame(title, description, characters, charactersCount, options, update);
}