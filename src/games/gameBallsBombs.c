#include "cglp.h"

static char *title = "BALLS BOMBS";
static char *description = "[Hold] Walk";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        "      ",
        " llll ",
        "llLlll",
        "lLllll",
        "llllll",
        "llllll",
        " llll ",
    },
    {
        "      ",
        "      ",
        "      ",
        " llll ",
        "lLLlll",
        "llllll",
        " llll ",
    },
    {
        "      ",
        "llllll",
        "ll l l",
        "ll l l",
        "llllll",
        " l  l ",
        " l  l ",
    },
    {
        "      ",
        "llllll",
        "ll l l",
        "ll l l",
        "llllll",
        "ll  ll",
        "      ",
    },
    {
        "    r ",
        "   l  ",
        " llll ",
        "llllll",
        "llllll",
        " llll ",
        "      ",
    },
    {
        "   r  ",
        "   l  ",
        " llll ",
        "llllll",
        "llllll",
        "  ll  ",
        "      ",
    },
    {
        "    r ",
        "   l  ",
        " llll ",
        "ll l l",
        "ll l l",
        " llll ",
        "      ",
    },
    {
        "   r  ",
        "   l  ",
        " llll ",
        "ll l l",
        "ll l l",
        " llll ",
        "      ",
    }
};
static int charactersCount = 8;

static Options options = {
    .viewSizeX = 150,
    .viewSizeY = 100,
    .soundSeed = 1,
    .isDarkColor = false
};

typedef struct {
    Vector pos;
    Vector vel;
    bool isAlive;
} Ball;

#define MAX_BALL_COUNT 128
static Ball balls[MAX_BALL_COUNT];
static int ballIndex;
static float nextBallDist;
static float playerX;
static float walkSpeed;
static bool isBombPlayer;
static float bombX;
static bool hasBomb;
static float nextBombDist;
static float explosionX;
static bool hasExplosion;
static float explosionXRadius;
static float wallX;
static int multiplier;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(balls);
        ballIndex = 0;
        nextBallDist = 0;
        playerX = 30;
        walkSpeed = 1;
        hasBomb = false;
        isBombPlayer = false;
        nextBombDist = 50;
        hasExplosion = false;
        explosionXRadius = 0;
        wallX = 99;
        multiplier = 1;
    }

    float sd = sqrtf(difficulty);
    float scr = playerX > 50 ? (playerX - 50) * 0.1f : 0;

    if (hasExplosion) {
        explosionX -= scr;
        explosionXRadius += 1;
        color = explosionXRadius > 60 ? LIGHT_RED : RED;

        float oldThickness = thickness;
        thickness = 3.0f;
        arc(explosionX, 87, explosionXRadius, M_PI - 0.1f, M_PI * 2 + 0.1f);
        if (explosionXRadius > 64) {
            hasExplosion = false;
            multiplier = 1;
        }
        thickness = oldThickness;
    }

    color = LIGHT_BLACK;
    rect(0, 90, 150, 10);
    wallX = wrap(wallX - scr, -5, 155);
    rect(wallX, 0, 1, 90);
    
    color = WHITE;
    rect(wrap(wallX + 99, -5, 155), 90, 1, 10);
    
    color = BLACK;
    nextBombDist -= isBombPlayer ? 0 : scr;
    if (nextBombDist < 0) {
        bombX = 153;
        hasBomb = true;
        nextBombDist += rnd(150, 180);
    }

    if (hasBomb) {
        bombX -= scr;
        char bombChar[2] = {'e' + (ticks / 20) % 2, '\0'};
        character(bombChar, bombX, 87);
    }

    if (input.isJustPressed) {
        play(LASER);
        walkSpeed = 2;
    }

    playerX += (input.isPressed ? sd * clamp(walkSpeed, 0, 1) : 0) - scr;
    walkSpeed *= 0.998f;

    char playerChar[2] = {(isBombPlayer ? 'g' : 'c') + (ticks / 20) % 2, '\0'};
    Collision playerCol = character(playerChar, playerX, 87);

    if (playerCol.isColliding.character['e'] || playerCol.isColliding.character['f']) {
        play(POWER_UP);
        isBombPlayer = true;
        hasBomb = false;
    }

    nextBallDist -= clamp(scr, sd * 0.2f, 99);
    if (nextBallDist < 0) {
        ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
        float posX = rnd(0, 1) < 0.1f ? rnd(-20, -3) : rnd(153, 200);
        vectorSet(&b->pos, posX, rnd(10, 40));
        vectorSet(&b->vel, (posX < 0 ? 2 : -1) * rnd(0.3f, 0.7f), 0);
        b->isAlive = true;
        ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
        nextBallDist += rnd(24, 32) / sd;
    }

    FOR_EACH(balls, i) {
        ASSIGN_ARRAY_ITEM(balls, i, Ball, b);
        SKIP_IS_NOT_ALIVE(b);

        b->vel.y += 0.1f;
        b->pos.x += b->vel.x * sd - scr;
        b->pos.y += b->vel.y * sd;

        if (b->pos.y > 87 && b->vel.y > 0) {
            play(HIT);
            b->vel.y *= -1.01f;
            b->pos.y = 87;
        }

        if (b->pos.x < 3 && b->vel.x < 0) {
            b->vel.x *= -1.01f * 2;
        }
        if (b->pos.x > 147 && b->vel.x > 0) {
            b->vel.x *= -1.01f / 2;
        }

        char ballChar[2] = {'a' + (b->pos.y > 80 && b->vel.y < 0 ? 1 : 0), '\0'};
        Collision ballCol = character(ballChar, b->pos.x, clamp(b->pos.y, 0, 87));

        if (ballCol.isColliding.rect[RED] || ballCol.isColliding.rect[LIGHT_RED]) {
            play(COIN);
            addScore(multiplier, VEC_XY(b->pos));
            particle(VEC_XY(b->pos), 9, 1, 0, M_PI * 2);
            multiplier++;
            b->isAlive = false;
        } else if (ballCol.isColliding.character['c'] || ballCol.isColliding.character['d']) {
            play(RANDOM);
            gameOver();
        } else if (ballCol.isColliding.character['g'] || ballCol.isColliding.character['h']) {
            play(EXPLOSION);
            explosionX = playerX;
            explosionXRadius = 6;
            hasExplosion = true;
            hasBomb = false;
            isBombPlayer = false;
            b->isAlive = false;
        }
    }
}

void addGameBallsBombs() {
    addGame(title, description, characters, charactersCount, options, update);
}