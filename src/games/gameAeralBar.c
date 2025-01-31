#include "cglp.h"

static char *title = "AERIAL BAR";
static char *description = "[Tap]  Jump\n[Hold] Fly";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
    "      ",
}};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 9000, .isDarkColor = false
};

typedef struct {
    Vector pos;     // Changed to Vector for position
    float length;
    float angle;
    float angleVel;
    bool isHeld;
    bool isAlive;
} Bar;

typedef struct {
    Vector pos;
    float length;
    float angle;
    float angleVel;
    float center;
    Bar *bar;
    Vector vel;
} Player;

#define AERIAL_BAR_MAX_COUNT 32
static Bar bars[AERIAL_BAR_MAX_COUNT];
static int barIndex;
static float nextBarDist;
static Player player;
static float flyingTicks;
static float ceilingY;
static float targetCeilingY;

static void addBar(float x, float length, float angle, float angleVel, bool isHeld) {
    ASSIGN_ARRAY_ITEM(bars, barIndex, Bar, b);
    vectorSet(&b->pos, x, 0);  // Using vectorSet for position
    b->length = length;
    b->angle = angle;
    b->angleVel = angleVel;
    b->isHeld = isHeld;
    b->isAlive = true;
    barIndex = wrap(barIndex + 1, 0, AERIAL_BAR_MAX_COUNT);
}

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(bars);
        barIndex = 0;
        addBar(50, 50, M_PI / 2, 0.03, true);
        vectorSet(&player.pos, 0, 0);
        vectorSet(&player.vel, 0, 0);
        player.length = 10;
        player.angle = 0;
        player.angleVel = 0;
        player.center = 0.2;
        player.bar = &bars[0];
        flyingTicks = 0;
        ceilingY = targetCeilingY = 10;
    }

    ceilingY += (targetCeilingY - ceilingY) * 0.1;
    color = LIGHT_CYAN;
    rect(0, 0, 100, ceilingY);
    color = LIGHT_BLUE;
    rect(0, 90, 100, 10);

    float scr = difficulty * 0.02;
    if (player.pos.x > 50) {
        scr += (player.pos.x - 50) * 0.1;
    }
    player.pos.x -= scr;

    if (player.bar != NULL) {
        Bar *b = player.bar;
        Vector endPos;
        vectorSet(&endPos, b->pos.x, ceilingY);
        addWithAngle(&endPos, b->angle, b->length);
        vectorSet(&player.pos, endPos.x, endPos.y);
        player.angleVel += b->angleVel * b->length * 0.003;
        
        if (b->pos.x < 0) {
            color = RED;
            text("X", 3, ceilingY);
            play(EXPLOSION);
            gameOver();
            return;
        }

        if (input.isJustPressed) {
            play(SELECT);
            vectorSet(&player.vel, 0, 0);
            addWithAngle(&player.vel, b->angle + M_PI / 2, 
                (b->angleVel * b->length + player.angleVel * 3) * sqrt(difficulty));
            vectorAdd(&player.vel, 0, -sqrt(difficulty) * 0.5);
            player.bar = NULL;
            flyingTicks = 1;
        }
    } else {
        flyingTicks += difficulty;
        vectorAdd(&player.pos, player.vel.x, player.vel.y);
        player.vel.y += (input.isPressed ? 0.01 : 0.1) * sqrt(difficulty);
        vectorMul(&player.vel, input.isPressed ? 0.99 : 0.95);

        if (player.pos.y > 89) {
            play(HIT);
            player.vel.y *= -1.5;
            player.pos.y = 88;
            targetCeilingY += 10;
            flyingTicks = -9999;
            FOR_EACH(bars, i) {
                ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
                SKIP_IS_NOT_ALIVE(b);
                b->isHeld = false;
            }
        }

        if (player.pos.x < 0) {
            color = RED;
            text("X", 3, player.pos.y);
            play(EXPLOSION);
            gameOver();
            return;
        }
    }

    player.angleVel *= 0.99;
    player.angle += player.angleVel;
    color = CYAN;

    float oldThickness = thickness;
    thickness = 4;
    Collision c = bar(VEC_XY(player.pos), player.length, player.angle);
    thickness = oldThickness;
    if (c.isColliding.rect[LIGHT_CYAN] && player.vel.y < 0) {
        player.vel.y *= -0.5;
    }

    nextBarDist -= scr;
    while (nextBarDist < 0) {
        float length = rnd(20, 50);
        addBar(110, length, M_PI / 2 - rnd(0, M_PI / 4), 
          (rnd(0, 1) * 2 - 1) * rnd(0.02, 0.04), false); //rnds(0.02, 0.04)
        nextBarDist = length + rnd(0, 20); //rnd(20)
    }

    FOR_EACH(bars, i) {
        ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
        SKIP_IS_NOT_ALIVE(b);
        b->pos.x -= scr;  // Using Vector position
        b->angleVel += cos(b->angle) * b->length * 0.00005 * sqrt(difficulty);
        b->angle += b->angleVel;
        
        color = BLACK;
        Vector endPos;
        vectorSet(&endPos, b->pos.x, ceilingY);
        addWithAngle(&endPos, b->angle, b->length);
        line(b->pos.x, ceilingY, endPos.x, endPos.y);
        
        color = b->isHeld ? BLACK : BLUE;
        Collision c = box(VEC_XY(endPos), 5, 5);
        
        if (c.isColliding.rect[LIGHT_BLUE]) {
            play(EXPLOSION);
            color = RED;
            text("X", endPos.x, endPos.y);
            gameOver();
            return;
        }

        if (!b->isHeld && player.bar == NULL && c.isColliding.rect[CYAN]) {
            play(POWER_UP);
            if (flyingTicks > 0) {
                addScore(ceil(flyingTicks), VEC_XY(endPos));
            }
            player.bar = b;
            b->isHeld = true;
            targetCeilingY = clamp(targetCeilingY - 5, 10, 99);
        }

        b->isAlive = b->pos.x >= -30;
    }
}

void addGameAerialBar() {
    addGame(title, description, characters, charactersCount, options, update);
}