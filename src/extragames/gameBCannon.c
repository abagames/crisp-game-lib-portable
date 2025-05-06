//note: inaccuricy where cursor does not remain on balls center after a long time
//      put gameover in comments and play the game bit you'll see what i mean

#include "cglp.h"

#define float double

static char *title = "B CANNON";
static char *description = "[Tap]  Turn\n[Hold] Fire";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Cannon frame 1 'a'
        " ll   ",
        "ll    ",
        " l    ",
        " ll   ",
        "l     ",
        "      ",
    },
    {   // Cannon frame 2 'b'
        " ll   ",
        "ll    ",
        " l    ",
        "ll    ",
        "  l   ",
        "      ",
    },
    {   // Target 'c'
        "ll ll ",
        "l l l ",
        " lll  ",
        "l l l ",
        "ll ll ",
        "      ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 2,
    .isDarkColor = true
};

typedef struct {
    Vector pos;
    Vector vel;
    float radius;
    float invincibleTicks;
    bool isAlive;
} Ball;

typedef struct {
    Vector from;
    float angle;
    float length;
} Rope;

typedef struct {
    float x;
    float angle;
    int vx;
    float stopTicks;
    Vector pos;
} Player;

#define MAX_BALL_COUNT 100
static Ball balls[MAX_BALL_COUNT];
static int ballIndex;
static int nextBallCount;
static int nextBallCountMax;
static float nextBallTicks;
static float ballSpeed;
static Rope rope;
static Player player;
static Vector targetPos;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(balls);
        ballIndex = 0;
        nextBallCount = 0;
        nextBallCountMax = 2;
        nextBallTicks = 40;
        ballSpeed = 1;
        vectorSet(&rope.from, 0, 0);
        rope.angle = 0;
        rope.length = 0;
        player.x = 40;
        vectorSet(&player.pos, 40, 92);
        player.angle = 0;
        player.vx = 1;
        player.stopTicks = 0;
        vectorSet(&targetPos, 50, 50);
        barCenterPosRatio = 0;
    }

    color = LIGHT_BLACK;
    rect(0, 0, 100, 5);
    rect(0, 95, 100, 5);
    rect(0, 5, 5, 90);
    rect(95, 5, 5, 90);
    
    color = BLACK;
    if (rope.length > 0) {
    if (!bar(VEC_XY(rope.from), rope.length, rope.angle).isColliding.rect[LIGHT_BLACK]) {
        rope.length += sqrt(difficulty) * 2;
        } else {
        rope.length = 0;
        }
    }
    
    COUNT_IS_ALIVE(balls, ballsAlive);
    if (ballsAlive == 0) {  // No active balls
        nextBallTicks++;
        if (nextBallTicks > 60) {
            play(POWER_UP);
            nextBallTicks = 0;
            nextBallCount++;
            if (nextBallCount > nextBallCountMax) {
                nextBallCount = 1;
                ballSpeed += 0.2;
                nextBallCountMax = clamp(nextBallCountMax + 1, 1, 4);
            }

            float x = 50 - (nextBallCount - 1) * 10 - 20;
            for (int i = 0; i < nextBallCount; i++) {
                ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
                x += 20;
                vectorSet(&b->pos, x, 30);
                vectorSet(&b->vel, 0, ballSpeed);
                b->radius = 9;
                b->invincibleTicks = 0;
                b->isAlive = true;
                ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
            }
        }
    }

    float nearestDist = 200;
    color = YELLOW;
    Ball *nearestBall = NULL;
    FOR_EACH(balls, i) {
        ASSIGN_ARRAY_ITEM(balls, i, Ball, b);
        SKIP_IS_NOT_ALIVE(b);

        vectorAdd(&b->pos, b->vel.x, b->vel.y);

        if ((b->pos.x < 5 + b->radius && b->vel.x < 0) ||
            (b->pos.x > 95 - b->radius && b->vel.x > 0)) {
            play(HIT);
            b->vel.x *= -1;
        }
        if ((b->pos.y < 5 + b->radius && b->vel.y < 0) ||
            (b->pos.y > 95 - b->radius && b->vel.y > 0)) {
            play(HIT);
            b->vel.y *= -1;
        }

        b->invincibleTicks--;
        if (arc(b->pos.x, b->pos.y, b->radius, 0, M_PI * 2).isColliding.rect[BLACK] && 
            b->invincibleTicks < 0) {
            float radius = b->radius * 0.7;
            if (radius < 3) {
                play(JUMP);
                addScore(ballIndex, b->pos.x, b->pos.y);
                particle(b->pos.x, b->pos.y, 9, 1, 0, M_PI * 2);
                b->isAlive = false;
                continue;
            }

            play(COIN);
            float s = sqrt(b->vel.x * b->vel.x + b->vel.y * b->vel.y);
            float invincibleTicks = 30 / sqrt(difficulty);
            float oa = wrap(vectorAngle(&b->vel) - rope.angle - M_PI, -M_PI, M_PI);

            if (fabs(oa) > M_PI / 2) {
                oa = M_PI - fabs(oa);
            }
            if (fabs(oa) < M_PI / 5) {
                oa = M_PI / 5;
            }

            // Split ball into two
            ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, newBall1);
            vectorSet(&newBall1->pos, b->pos.x, b->pos.y);
            vectorSet(&newBall1->vel, s, 0);
            rotate(&newBall1->vel, rope.angle + oa);
            newBall1->radius = radius;
            newBall1->invincibleTicks = invincibleTicks;
            newBall1->isAlive = true;
            ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);

            ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, newBall2);
            vectorSet(&newBall2->pos, b->pos.x, b->pos.y);
            vectorSet(&newBall2->vel, s, 0);
            rotate(&newBall2->vel, rope.angle - oa);
            newBall2->radius = radius;
            newBall2->invincibleTicks = invincibleTicks;
            newBall2->isAlive = true;
            ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);

            b->isAlive = false;
            continue;
        }
        float d = distanceTo(&b->pos, player.pos.x, player.pos.y);
        if (d < nearestDist) {
            nearestDist = d;
            nearestBall = b;
        }
    }

    if (nearestBall != NULL) {
        Vector delta;
        vectorSet(&delta, nearestBall->vel.x, nearestBall->vel.y);
        vectorMul(&delta, 8 + ballSpeed);
        vectorAdd(&delta, nearestBall->pos.x, nearestBall->pos.y);
        vectorAdd(&delta, -targetPos.x, -targetPos.y);
        vectorMul(&delta, 0.1 * sqrt(difficulty));
        vectorAdd(&targetPos, delta.x, delta.y);
    }

    if (input.isJustPressed) {
        play(SELECT);
        player.vx *= -1;
    }

    if (input.isPressed) {
        if (nearestBall != NULL) {
            player.stopTicks++;
        }
    } else {
        player.stopTicks = 0;
        player.x += player.vx * sqrt(difficulty);
        if (player.x < 0) {
            player.angle++;
            player.x = 84 + player.x;
        } else if (player.x >= 84) {
            player.angle--;
            player.x = 84 - player.x;
        }
        player.angle = wrap(player.angle, 0, 4);
    }

    switch ((int)player.angle) {
        case 0:
            vectorSet(&player.pos, player.x + 8, 92);
            break;
        case 1:
            vectorSet(&player.pos, 8, player.x + 8);
            break;
        case 2:
            vectorSet(&player.pos, 92 - player.x, 8);
            break;
        case 3:
            vectorSet(&player.pos, 92, 92 - player.x);
            break;
    }

    color = BLACK;
    character("c", targetPos.x, targetPos.y);

    if (player.stopTicks == 10) {
        play(LASER);
        Vector diff;        
        rope.angle = angleTo(&(player.pos), targetPos.x, targetPos.y);
        vectorSet(&rope.from, player.pos.x, player.pos.y);
        rope.length = 1;
    }

    characterOptions.isMirrorX = player.vx < 0;
    characterOptions.rotation = (int)player.angle;
    char playerChar[2] = {'a' + ((int)(ticks / 20) % 2), '\0'};

    if (character(playerChar, player.pos.x, player.pos.y).isColliding.rect[YELLOW]) {
        play(EXPLOSION);
        gameOver();
    }
}

void addGameBCannon() {
    addGame(title, description, characters, charactersCount, options, update);
}

#undef float