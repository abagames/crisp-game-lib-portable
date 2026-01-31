//note yellow circle when clicking with mouse seems off

#include "cglp.h"

static char *title = "B BLAST";
static char *description = "[Hold]\n Select area\n[Release]\n Erase balls";

// No custom characters needed
#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{"      ",
       "      ",
       "      ",
       "      ",
       "      ",
       "      "}};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 5,
    .isDarkColor = true
};

typedef struct {
    Vector pos;
    Vector vel;
    float radius;
    float targetRadius;
    bool isRed;
    bool isAlive;
} Ball;

#define MAX_BALL_COUNT 200
static Ball balls[MAX_BALL_COUNT];
static int ballIndex;
static float nextBallTicks;
static float nextRedBallTicks;
static Vector pos;
static float radius;
static float radiusSlowTicks;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(balls);
        ballIndex = 0;
        nextRedBallTicks = 0;
        nextBallTicks = 60;
        vectorSet(&pos, 0, 0);
        radius = -1;  // Using -1 instead of undefined
        radiusSlowTicks = 0;
    }

    nextRedBallTicks--;
    if (nextRedBallTicks < 0) {
        play(JUMP);
        ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
        vectorSet(&b->pos, rnd(19, 80), 10);
        vectorSet(&b->vel, 0, 0);
        b->radius = 0;
        b->targetRadius = 9;
        b->isRed = true;
        b->isAlive = true;
        ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
        nextRedBallTicks = 300 / difficulty;
    }

    nextBallTicks--;
    if (nextBallTicks < 0) {
        play(HIT);
        ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
        vectorSet(&b->pos, rnd(9, 90), rnd(20, 80));
        vectorSet(&b->vel, 0, 0);
        b->radius = 0;
        b->targetRadius = rnd(5, 10);
        b->isRed = false;
        b->isAlive = true;
        ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
        nextBallTicks = 30 / difficulty;
    }

    bool hasRed = false;
    FOR_EACH(balls, i) {
        ASSIGN_ARRAY_ITEM(balls, i, Ball, b);
        SKIP_IS_NOT_ALIVE(b);

        b->radius += (b->targetRadius - b->radius) * 0.05;

        // Ball collision
        FOR_EACH(balls, j) {
            ASSIGN_ARRAY_ITEM(balls, j, Ball, ob);
            SKIP_IS_NOT_ALIVE(ob);
            if (b == ob) continue;

            float d = distanceTo(&b->pos, ob->pos.x, ob->pos.y) - b->radius - ob->radius;
            if (d < 0) {                
                float angle = angleTo(&ob->pos, b->pos.x, b->pos.y);
                addWithAngle(&b->vel, angle, -d / b->radius);
            }
        }

        // Wall collision
        if ((b->pos.x < b->radius && b->vel.x < 0) ||
            (b->pos.x > 99 - b->radius && b->vel.x > 0)) {
            b->vel.x *= -0.7;
        }
        if (b->pos.y > 110 - b->radius && b->vel.y > 0) {
            b->vel.y *= -0.7;
        }

        // Physics
        b->vel.y += 0.05;
        vectorMul(&b->vel, 0.95);
        float velLength = sqrt(b->vel.x * b->vel.x + b->vel.y * b->vel.y);
        if (velLength > 1) {
            vectorMul(&b->vel, 0.5);
        }
        vectorAdd(&b->pos, b->vel.x, b->vel.y);

        if (b->isRed) {
            hasRed = true;
            if (b->pos.y > 99 - b->radius) {
                play(COIN);
                addScore(10, b->pos.x, b->pos.y);
                b->isRed = false;
            }
        }

        color = b->isRed ? RED : BLACK;
        thickness = b->radius * 0.4;
        arc(b->pos.x, b->pos.y, b->radius, 0, M_PI * 2);

        if (b->pos.y - b->radius < 0) {
            play(RANDOM);
            color = RED;
            text("X", b->pos.x, 7);
            gameOver();
        }
    }

    if (!hasRed) {
        nextRedBallTicks = 0;
    }

    if (input.isJustPressed) {
        play(SELECT);
        pos.x = clamp(input.pos.x, 0, 99);
        pos.y = clamp(input.pos.y, 0, 99);
        radius = 1;
    }

    if (radius >= 0 && input.isPressed) {
        radius += 0.5 * difficulty * (1 - radiusSlowTicks / 60);
        color = YELLOW;
        thickness = 2;
        arc(pos.x, pos.y, radius, 0, M_PI * 2);
    }

    if (radius >= 0 && input.isJustReleased) {
        play(LASER);
        radiusSlowTicks = 60;
        int multiplier = 1;
        
        // Store positions of red balls to spawn new balls later
        Vector redPositions[MAX_BALL_COUNT];
        int redCount = 0;

        FOR_EACH(balls, i) {
            ASSIGN_ARRAY_ITEM(balls, i, Ball, b);
            SKIP_IS_NOT_ALIVE(b);

            if (distanceTo(&b->pos, pos.x, pos.y) - b->radius - radius < 0) {
                if (b->isRed) {
                    play(EXPLOSION);
                    redPositions[redCount].x = b->pos.x;
                    redPositions[redCount].y = b->pos.y;
                    redCount++;
                    color = RED;
                    particle(b->pos.x, b->pos.y, 30, 4, M_PI * 2, 0.4);
                } else {
                    play(COIN);
                    color = BLACK;
                    particle(b->pos.x, b->pos.y, b->radius, b->radius * 0.3, M_PI * 2, 0.4);
                }
                addScore(multiplier, b->pos.x, b->pos.y);
                multiplier++;
                b->isAlive = false;
            }
        }

        // Spawn new balls from red ball positions
        for (int i = 0; i < redCount; i++) {
            for (int j = 0; j < 9; j++) {
                ASSIGN_ARRAY_ITEM(balls, ballIndex, Ball, b);
                vectorSet(&b->pos, redPositions[i].x + rnd(-3, 3), 
                                  redPositions[i].y + rnd(-3, 3));
                vectorSet(&b->vel, 0, 0);
                b->radius = 0;
                b->targetRadius = rnd(12, 15);
                b->isRed = false;
                b->isAlive = true;
                ballIndex = wrap(ballIndex + 1, 0, MAX_BALL_COUNT);
            }
        }
    }

    if (radiusSlowTicks > 0) {
        radiusSlowTicks--;
    }
}

void addGameBBlast() {
    addGame(title, description, characters, charactersCount, options, update);
}