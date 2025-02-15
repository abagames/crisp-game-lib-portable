#include "cglp.h"

static char *title = "CHARGE BEAM";
static char *description = "[Tap]     Shot\n[Hold]    Charge\n[Release] Fire";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {        
        "      ",
        "rllbb ",
        "lllccb",
        "llyl b",
        "      ",
        "      "
    },
    {
        "  r rr",
        "rrrrrr",
        "  grr ",
        "  grr ",
        "rrrrrr",
        "  r rr"
    },
    {
        " LLLL ",
        "LyyyyL",
        "LyyyyL",
        "LyyyyL",
        "LyyyyL",
        " LLLL "
    },
    {
        "   bbb",
        "  bccb",
        "bbllcb",
        "bcllcb",
        "  bccb",
        "   bbb"
    },
    {
        "      ",
        "l llll",
        "l llll",
        "      ",
        "      ",
        "      "
    }
};
static int charactersCount = 5;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 60,
    .soundSeed = 1,
    .isDarkColor = false
};

typedef struct {
    float x;
    float size;
    char type;
    bool isAlive;
} Obj;

typedef struct {
    float x;
    float vx;
    bool isAlive;
} InhalingCoin;

#define MAX_OBJS 100
#define MAX_INHALING_COINS 50

static Obj objs[MAX_OBJS];
static InhalingCoin inhalingCoins[MAX_INHALING_COINS];
static float nextObjDist;
static int coinMultiplier;
static int coinPenaltyMultiplier;
static int enemyMultiplier;
static float shotX;
static int shotSize;
static float charge;
static float penaltyVx;
static char prevType;

typedef struct {
    float x;
    float vx;
} Player;

static void addCoinPenalty(float x, bool isShotHit) {
    play(POWER_UP);
    particle(x, 30, 9, 5, 0, M_PI * 2);
    if (isShotHit) {
        shotX = -1;
    }
    addScore(-coinPenaltyMultiplier, x + sqrt(coinPenaltyMultiplier) * 4, 50);
    if (coinPenaltyMultiplier < 64) {
        coinPenaltyMultiplier *= 2;
    }
    penaltyVx += 0.5;
}

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(objs);
        INIT_UNALIVED_ARRAY(inhalingCoins);
        objs[0] = (Obj){.x = 150, .size = 1, .type = 'e', .isAlive = true};
        for (int i = 0; i < 3; i++) {
            objs[i + 1] = (Obj){.x = 160 + i * 10, .size = 1, .type = 'c', .isAlive = true};
        }
        nextObjDist = 30;
        coinMultiplier = enemyMultiplier = coinPenaltyMultiplier = 1;
        shotX = -1;
        shotSize = 0;
        charge = 0;
        penaltyVx = 0;
        prevType = 'c';
    }

    if (shotX < 0) {
        text("BEAM", 30, 55);
        if (input.isPressed && charge < 99) {
            play(HIT);
            charge += difficulty * 1.5;
            color = CYAN;
            float c = charge;
            float x = 60;
            if (c < 25) {
                rect(x, 53, c, 5);
                shotSize = 1;
            } else {
                rect(x, 53, 25, 5);
                c -= 25;
                x += 27;
                shotSize = 1;
                while (c > 9) {
                    rect(x, 53, 9, 5);
                    x += 11;
                    c -= 9;
                    shotSize++;
                }
                rect(x, 53, c, 5);
                shotSize++;
            }
            color = BLACK;
        } else if (charge > 0) {
            play(LASER);
            shotX = 10;
            charge = 0;
            coinMultiplier = enemyMultiplier = coinPenaltyMultiplier = 1;
        }
    }

    if (shotX >= 0) {
        shotX += difficulty * 2.5;
        float x = shotX;
        if (shotSize == 1) {
            character("e", shotX, 30);
        } else {
            for (int i = 0; i < shotSize; i++) {
                if (shotSize % 2 == 1 && i == 0) {
                    character("d", x, 30);
                    x += 6;
                } else {
                    if (i % 2 == shotSize % 2) {
                        character("d", x, 27);
                    } else {
                        character("d", x, 33);
                        x += 6;
                    }
                }
            }
        }
        if (shotX > 203) {
            shotX = -1;
        }
    }

    penaltyVx -= 0.02;
    if (penaltyVx < 0) {
        penaltyVx = 0;
    }
    const float vx = (-difficulty - penaltyVx) * 0.5;
    color = RED;
    for (int i = 0; i < ceil(penaltyVx * 2 + 0.1); i++) {
        text("<", i * 6 + 3, 48);
    }
    color = BLACK;
    nextObjDist += vx;

    if (nextObjDist < 0) {
        char type = prevType != 'c' && rnd(0, 1) < 0.5 ? 'c' : 'e';
        prevType = type;
        int c = rndi(3, 9);
        float x = 200;
        if (type == 'c') {
            for (int i = 0; i < c; i++) {
                FOR_EACH(objs, j) {
                    ASSIGN_ARRAY_ITEM(objs, j, Obj, o);
                    if (!o->isAlive) {
                        o->x = x;
                        o->size = 1;
                        o->type = 'c';
                        o->isAlive = true;
                        break;
                    }
                }
                x += 10;
            }
        } else {
            for (int i = 0; i < c; i++) {
                float size = rnd(0, 1) < 0.3 ? rndi(2, 6) : 1;
                FOR_EACH(objs, j) {
                    ASSIGN_ARRAY_ITEM(objs, j, Obj, o);
                    if (!o->isAlive) {
                        o->x = x;
                        o->size = size;
                        o->type = 'e';
                        o->isAlive = true;
                        break;
                    }
                }
                x += 10 + ceil((size - 1) / 2) * 6;
            }
        }
        x += 10;
        nextObjDist = x - 200;
    }

    float minCoinX = 999;
    float minEnemyX = 999;

    FOR_EACH(objs, i) {
        ASSIGN_ARRAY_ITEM(objs, i, Obj, o);
        SKIP_IS_NOT_ALIVE(o);

        o->x += vx;
        if (o->type == 'c') {
            if (o->x < minCoinX) {
                minCoinX = o->x;
            }
        } else {
            if (o->x < minEnemyX) {
                minEnemyX = o->x;
            }
        }

        if (o->type == 'c') {
            Collision col = character("c", o->x, 30);
            if (col.isColliding.character['d'] || col.isColliding.character['e']) {
                addCoinPenalty(o->x, col.isColliding.character['e']);
                o->isAlive = false;
            } else if (col.isColliding.character['b'] || col.isColliding.character['c']) {
                o->isAlive = false;
            }
        } else {
            float x = o->x;
            Collision col = {0};
            for (int j = 0; j < o->size; j++) {
                if ((int)o->size % 2 == 1 && j == 0) {
                    Collision tempCol = character("b", x, 30);
                    for (int k = 0; k < ASCII_CHARACTER_COUNT; k++) {
                        col.isColliding.character[k] |= tempCol.isColliding.character[k];
                    }
                    x += 6;
                } else {
                    if (j % 2 == (int)o->size % 2) {
                        Collision tempCol = character("b", x, 27);
                        for (int k = 0; k < ASCII_CHARACTER_COUNT; k++) {
                            col.isColliding.character[k] |= tempCol.isColliding.character[k];
                        }
                    } else {
                        Collision tempCol = character("b", x, 33);
                        for (int k = 0; k < ASCII_CHARACTER_COUNT; k++) {
                            col.isColliding.character[k] |= tempCol.isColliding.character[k];
                        }
                        x += 6;
                    }
                }
            }

            if (col.isColliding.character['d'] || col.isColliding.character['e']) {
                play(EXPLOSION);
                if (col.isColliding.character['e']) {
                    shotX = -1;
                }
                if (o->size <= shotSize) {
                    particle(o->x, 30, o->size * 3, 1, 0, M_PI * 2);
                    addScore(enemyMultiplier * o->size, o->x, 30);
                    enemyMultiplier++;
                    if (o->size > 1) {
                        shotSize -= o->size;
                        if (shotSize <= 0) {
                            shotX = -1;
                        }
                    }
                    o->isAlive = false;
                } else {
                    o->size -= shotSize;
                    shotX = -1;
                }
            }
            if (col.isColliding.character['b'] || col.isColliding.character['c']) {
                o->isAlive = false;
            }
        }
    }

    if (minCoinX > 200 && minEnemyX > 200) {
        nextObjDist = 0;
    }

    if (minCoinX < minEnemyX) {
        FOR_EACH(objs, i) {
            ASSIGN_ARRAY_ITEM(objs, i, Obj, o);
            SKIP_IS_NOT_ALIVE(o);
            if (o->type == 'c' && o->x < minEnemyX) {
                FOR_EACH(inhalingCoins, j) {
                    ASSIGN_ARRAY_ITEM(inhalingCoins, j, InhalingCoin, ic);
                    if (!ic->isAlive) {
                        ic->x = o->x;
                        ic->vx = -1;
                        ic->isAlive = true;
                        break;
                    }
                }
                o->isAlive = false;
            }
        }
    }

    // Draw player and check for collision
   if (character("a", 10, 30).isColliding.character['b']) {
    play(RANDOM);
    gameOver();
  }


    FOR_EACH(inhalingCoins, i) {
        ASSIGN_ARRAY_ITEM(inhalingCoins, i, InhalingCoin, ic);
        SKIP_IS_NOT_ALIVE(ic);

        ic->x += ic->vx;
        ic->vx -= 0.1;
        Collision col = character("c", ic->x, 30);
        if (col.isColliding.character['d'] || col.isColliding.character['e']) {
            addCoinPenalty(ic->x, col.isColliding.character['e']);
            ic->isAlive = false;
        } else if (ic->x < 10) {
            play(COIN);
            addScore(coinMultiplier, 10 + sqrt(coinMultiplier) * 4, 30);
            if (coinMultiplier < 64) {
                coinMultiplier *= 2;
            }
            ic->isAlive = false;
        }
    }
}

void addGameChargeBeam() {
    addGame(title, description, characters, charactersCount, options, update);
}