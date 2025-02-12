//seems to work but initial coin spawn values are different from JS version probably due to rnd
//which makes coins spawn more to the left initially while on js version more to the right
#include "cglp.h"

static char *title = "BALANCE";
static char *description = "[Slide] Move";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Empty character set as per JS version
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
    }
};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 150,
    .viewSizeY = 100,
    .soundSeed = 8,
    .isDarkColor = false
};

typedef struct {
    float x;
    float vx;
    float angle;
    float length;
    float angleVel;
} Pillar;

typedef struct {
    Vector pos;
    Vector vel;
    bool isAlive;
} Coin;

#define MAX_COINS 100
static Pillar pillar;
static Coin coins[MAX_COINS];
static int coinIndex;
static float nextCoinTicks;
static float coinX;
static float lx;
static float wind;
static const float minLength = 20;

static void update() {
    barCenterPosRatio = 0.0f;
    thickness = 1;  // Default thickness

    if (!ticks) {
        pillar.x = 50;
        pillar.vx = 0;
        pillar.angle = 0;
        pillar.length = 20;
        pillar.angleVel = 0;
        
        INIT_UNALIVED_ARRAY(coins);
        coinIndex = 0;
        nextCoinTicks = 0;
        coinX = rnd(0, 1) < 0.5f ? 20 : 80;
        lx = 50;
        wind = 0;
    }

    color = LIGHT_BLACK;
    rect(-50, 90, 200, 10);
    
    color = WHITE;
    rect(lx, 90, 1, 10);

    float o = input.pos.x - pillar.x;
    if (fabs(o) < 99) {
        pillar.vx += o * 0.005f;
    }

    wind += rnd(-0.01f, 0.01f);  // Simplified rnds
    wind *= 0.98f;
    pillar.vx -= wind;
    pillar.vx *= 0.95f;
    pillar.x += pillar.vx * difficulty;
    pillar.angleVel -= pillar.vx * 0.002f;
    pillar.angleVel += pillar.angle * 0.0001f * pillar.length;
    pillar.angleVel *= 1 - 0.1f * sqrt(difficulty);
    pillar.angle += pillar.angleVel * difficulty;

    Vector tp;
    vectorSet(&tp, pillar.x, 90);
    addWithAngle(&tp, pillar.angle - M_PI / 2, pillar.length);

    float scr = (50 - tp.x) * 0.2f;
    lx = wrap(lx + scr, -5, 105);
    pillar.x += scr;
    tp.x += scr;

    if (pillar.length < minLength) {
        gameOver();
        return;
    }

    color = BLACK;
    thickness = 3;
    bar(pillar.x, 90, pillar.length, pillar.angle - M_PI / 2);
    
    color = RED;
    bar(pillar.x, 90, clamp(pillar.length, 0, minLength), pillar.angle - M_PI / 2);

    color = PURPLE;
    thickness = 1;
    Collision c = box(tp.x, tp.y, 5, 5);  // Same size for both dimensions
    if (c.isColliding.rect[LIGHT_BLACK]) {
        play(EXPLOSION);
        pillar.length /= 2;
        pillar.angleVel *= -0.5f;
        if (pillar.length >= minLength) {
            pillar.angle /= 2;
        }
    }

    nextCoinTicks--;
    if (nextCoinTicks < 0) {
        ASSIGN_ARRAY_ITEM(coins, coinIndex, Coin, coin);
        vectorSet(&coin->pos, coinX, -3);
        vectorSet(&coin->vel, rnd(-1.0f, 1.0f), 0);  // Simplified rnds
        coin->isAlive = true;
        coinIndex = wrap(coinIndex + 1, 0, MAX_COINS);
        
        coinX = wrap(coinX + rnd(-1.0f, 1.0f) + scr, -20, 120);
        nextCoinTicks = 5;
    }

    color = YELLOW;
    FOR_EACH(coins, i) {
        ASSIGN_ARRAY_ITEM(coins, i, Coin, coin);
        SKIP_IS_NOT_ALIVE(coin);

        coin->vel.x += wind / 2;
        vectorAdd(&coin->pos, coin->vel.x, coin->vel.y);
        coin->pos.x += scr;
        coin->vel.y += 0.03f;
        vectorMul(&coin->vel, 0.98f);

        Collision cl = box(coin->pos.x, coin->pos.y, 5, 5);
        if (cl.isColliding.rect[BLACK] || cl.isColliding.rect[RED] || 
            cl.isColliding.rect[PURPLE]) {
            
            bool isPurple = cl.isColliding.rect[PURPLE];
            play(isPurple ? POWER_UP : COIN);
            
            int times = isPurple ? 3 : 1;
            for (int t = 0; t < times; t++) {
                addScore(ceil(pillar.length), coin->pos.x, coin->pos.y - t * 6);
                if (pillar.length < 64) {
                    pillar.length++;
                }
            }
            coin->isAlive = false;
            continue;
        }

        if (cl.isColliding.rect[LIGHT_BLACK]) {
            coin->isAlive = false;
        }
    }
}

void addGameBalance() {
    addGame(title, description, characters, charactersCount, options, update);
}