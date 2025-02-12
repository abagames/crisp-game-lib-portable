#include "cglp.h"

static char *title = "BAMBOO";
static char *description = "[Tap]  Turn\n[Hold] Through";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Runner frame 1 'b'
        "  ll  ",
        "  l  l",
        "lpppp ",
        "  prrr",
        " r    ",
        "r     ",
    },
    {   // Runner frame 2 'c'
        "   ll ",
        "   l  ",
        "lpppp ",
        " rp  l",
        "r  r  ",
        "    r ",
    }
};
static int charactersCount = 2;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 1,
    .isDarkColor = false
};

typedef struct {
    Vector pos;  // Using Vector for position
    float height;
    float speed;
    bool isAlive;
} Bamboo;

#define MAX_BAMBOO_COUNT 100
static Bamboo bamboos[MAX_BAMBOO_COUNT];
static int bambooIndex;
static float nextBambooTicks;
static Vector playerPos;  // Using Vector for player position
static float vx;
static float avx;
static float animTicks;
static int speedBambooTicks;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(bamboos);
        bambooIndex = 0;
        nextBambooTicks = 0;
        vectorSet(&playerPos, 190, 87);  // Set player starting position
        vx = 1;
        avx = 0;
        animTicks = 0;
        speedBambooTicks = 5;
    }

    color = BLACK;
    if (input.isJustPressed) {
        play(SELECT);
        vx *= -1;
    }

    playerPos.x = wrap(playerPos.x + vx * difficulty * (1 + avx), -3, 203);
    playerPos.y = 87;
    avx *= 0.9;
    animTicks += difficulty;

    characterOptions.isMirrorX = vx < 0;
    characterOptions.rotation = 0;
    char playerChar[2] = {(input.isPressed ? 'b' : 'a' + ((int)(animTicks / 20) % 2)), '\0'};
    character(playerChar, playerPos.x, playerPos.y);

    nextBambooTicks--;
    if (nextBambooTicks < 0) {
        speedBambooTicks--;
        ASSIGN_ARRAY_ITEM(bamboos, bambooIndex, Bamboo, b);
        vectorSet(&b->pos, rnd(5, 195), 0);  // Set bamboo starting position
        b->height = 0;
        b->speed = speedBambooTicks < 0 ? 2 : 1;
        b->isAlive = true;
        bambooIndex = wrap(bambooIndex + 1, 0, MAX_BAMBOO_COUNT);
        nextBambooTicks = rnd(70, 100) / difficulty;
        speedBambooTicks = rndi(4, 7);
    }

    FOR_EACH(bamboos, i) {
        ASSIGN_ARRAY_ITEM(bamboos, i, Bamboo, b);
        SKIP_IS_NOT_ALIVE(b);

        b->height += b->speed * difficulty * 0.14;
        float h = b->height / 4;
        float y = 90 - h / 2;
        if (h < 1) {
            y += (1 - h) * 3;
            h = 1;
        }

        bool collision = false;
        for (int j = 0; j < 4; j++) {
            if (b->height < 5) {
                color = LIGHT_YELLOW;
            } else if (b->height > 50) {
                color = GREEN;
            } else if (b->height > 25) {
                color = j % 2 == 0 ? GREEN : LIGHT_GREEN;
            } else if (b->height > 23) {
                color = YELLOW;
            } else {
                color = j % 2 == 0 ? YELLOW : LIGHT_YELLOW;
            }

            Collision cl = box(b->pos.x, y, (4 - j) * 2, ceil(h));
            if ((cl.isColliding.character['b'] || cl.isColliding.character['a']) && !input.isPressed) {
                collision = true;
            }
            y -= h;
        }

        if (collision) {
            if (b->height < 5) {
                // Do nothing
            } else if (b->height <= 25) {
                float s = ceil((b->height - 5) / 3);
                if (s == 7) {
                    s = 10;
                    play(POWER_UP);
                } else {
                    play(COIN);
                }
                addScore(s * s, b->pos.x, 90 - b->height);
                b->isAlive = false;
                continue;
            } else {
                play(HIT);
                b->speed *= 0.6;
                b->height *= 0.7;
                avx++;
                if (avx > 5) {
                    avx = 5;
                }
                vx *= -1;
                particle(b->pos.x, playerPos.y, 9, difficulty * (1 + avx) * 0.5, vx > 0 ? 0 : M_PI, 0.4);
                if (b->height <= 25) {
                    play(EXPLOSION);
                    b->isAlive = false;
                    continue;
                }
            }
        }

        if (b->height > 50) {
            b->speed *= 0.997;
        }
        if (b->height >= 89) {
            color = RED;
            text("X", b->pos.x, 3);
            play(RANDOM);
            gameOver();
        }
    }

    color = PURPLE;
    rect(0, 90, 200, 10);
}

void addGameBamboo() {
    addGame(title, description, characters, charactersCount, options, update);
}