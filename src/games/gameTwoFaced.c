#include "cglp.h"

static char *title = "TWO FACED";
static char *description = "[Tap]  Turn\n[Hold] Go forward";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Match JS character exactly
        " RRRR ",
        "Rr rrR",
        "R rrrR",
        "RrrrrR",
        "RrrrrR",
        " RRRR ",
    }
};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 9,
    .isDarkColor = false
};

typedef struct {
    Vector pos;
    int side;
    float angle;
    float av;
    float speed;
    float baseSpeed;
} Head;

typedef struct {
    Vector pos;
    int side;
    float angle;
    bool isAlive;
} Tail;

typedef struct {
    Vector pos;
    int side;
    float angle;
} Item;

#define MAX_TAILS 256
static Head head;
static Tail tails[MAX_TAILS];
static int tailIndex;
static int tailCount;
static int activeTailCount;  // Track actual number of tail segments
static float lightTailCount;
static Item item;
static float multiplier;

static void getItem(float x, float y);
static void checkSide(Vector* pos, float* angle, int* side, float* av, bool hasAv);

static void update() {
    //barCenterPosRatio = 0.0f;
    
    if (!ticks) {
        vectorSet(&head.pos, 0, 20);
        head.side = -1;
        head.angle = -M_PI / 2;
        head.av = 1;
        head.speed = 1;
        head.baseSpeed = 1;

        INIT_UNALIVED_ARRAY(tails);
        tailIndex = 0;
        tailCount = 3;
        activeTailCount = 0;  // Initialize active count
        multiplier = 3;
        lightTailCount = 2;

        vectorSet(&item.pos, 0, 0);
        item.side = -1;
        item.angle = 0;
    }

    float sd = sqrt(difficulty);
    Vector v;
    vectorSet(&v, 0, 0);

    if (input.isJustPressed) {
        play(LASER);
        head.av *= -1;
        head.speed += 0.1f;
    }

    if (input.isPressed) {
        play(HIT);
        head.speed += 0.01f;
    } else {
        head.speed += (1 - head.speed) * 0.1f;
        head.angle += head.av * 0.03f * sd * head.speed * head.baseSpeed;
    }

    head.baseSpeed *= 1.002f;
    addWithAngle(&head.pos, head.angle, sd * 0.5f * head.speed * head.baseSpeed);
    checkSide(&head.pos, &head.angle, &head.side, &head.av, true);

    // Add new tail segment
    if (activeTailCount < MAX_TAILS) {
        ASSIGN_ARRAY_ITEM(tails, tailIndex, Tail, t);
        vectorSet(&t->pos, head.pos.x, head.pos.y);
        t->side = head.side;
        t->angle = head.angle;
        t->isAlive = true;
        tailIndex = wrap(tailIndex + 1, 0, MAX_TAILS);
        if (activeTailCount < tailCount * 9) {
            activeTailCount++;
        }
    }

    // Update item
    addWithAngle(&item.pos, item.angle, sd * 0.2f);
    checkSide(&item.pos, &item.angle, &item.side, NULL, false);

    // Draw item in background layer
    color = BLACK;
    if (item.side == -1) {
        character("a", item.pos.x + 150, item.pos.y + 50);
    }
    if (item.side == 1) {
        character("a", item.pos.x + 50, item.pos.y + 50);
    }

    // Draw head
    color = GREEN;
    thickness = 4;
    if (head.side == -1) {
        bar(head.pos.x + 150, head.pos.y + 50, 3, head.angle);
    }
    if (head.side == 1) {
        bar(head.pos.x + 50, head.pos.y + 50, 3, head.angle);
    }

    // Draw boxes
    color = LIGHT_YELLOW;
    box(50, 50, 80, 80);
    color = LIGHT_PURPLE;
    box(150, 50, 80, 80);

    // Draw item in foreground layer
    color = BLACK;
    if (item.side == -1) {
        character("a", item.pos.x + 50, item.pos.y + 50);
    }
    if (item.side == 1) {
        character("a", item.pos.x + 150, item.pos.y + 50);
    }

    // Update and draw tails
    color = LIGHT_GREEN;
    lightTailCount += (2 - lightTailCount) * 0.03f;

    for (int i = 0; i < tailCount; i++) {
        int ti = i * 9;
        if (ti >= activeTailCount) continue;  // Only process active tails

        int accessIndex = (tailIndex - 1 - ti + MAX_TAILS) % MAX_TAILS;
        Tail* t = &tails[accessIndex];
        if (!t->isAlive) continue;

        color = i < lightTailCount ? LIGHT_BLACK : LIGHT_GREEN;
        thickness = 3;

        if (t->side == -1) {
            Collision c = bar(t->pos.x + 50, t->pos.y + 50, 4, t->angle);
            if (item.side == -1 && c.isColliding.character['a']) {
                getItem(t->pos.x + 50, t->pos.y + 50);
            }
        }
        if (t->side == 1) {
            Collision c = bar(t->pos.x + 150, t->pos.y + 50, 4, t->angle);
            if (item.side == 1 && c.isColliding.character['a']) {
                getItem(t->pos.x + 150, t->pos.y + 50);
            }
        }
    }

    // Draw and check head collisions
    color = GREEN;
    thickness = 4;
    if (head.side == -1) {
        Collision c = bar(head.pos.x + 50, head.pos.y + 50, 3, head.angle);
        if (item.side == -1 && c.isColliding.character['a']) {
            getItem(head.pos.x + 50, head.pos.y + 50);
        }
        if (c.isColliding.rect[LIGHT_GREEN]) {
            play(EXPLOSION);
            gameOver();
            return;
        }
    }
    if (head.side == 1) {
        Collision c = bar(head.pos.x + 150, head.pos.y + 50, 3, head.angle);
        if (item.side == 1 && c.isColliding.character['a']) {
            getItem(head.pos.x + 150, head.pos.y + 50);
        }
        if (c.isColliding.rect[LIGHT_GREEN]) {
            play(EXPLOSION);
            gameOver();
            return;
        }
    }

    // Update multiplier
    int pp = ceil(multiplier);
    multiplier -= 0.01f;
    if (multiplier < 1) {
        multiplier = 1;
    }
    if (ceil(multiplier) < pp) {
        play(COIN);
    }

    color = BLACK;
    char multText[16];
    sprintf(multText, "x%d", (int)ceil(multiplier));
    text(multText, 3, 9);
}

static void getItem(float x, float y) {
    play(POWER_UP);
    head.baseSpeed = 1;
    addScore(ceil(multiplier), x, y);
    if (tailCount < 25) {
        tailCount++;
    }
    multiplier += tailCount;
    
    // Use rnd for random positioning matching JS rnds
    item.pos.x = rnd(-35, 35);
    item.pos.y = rnd(-35, 35);
    item.angle = rnd(-M_PI, M_PI);
    item.side = head.side * -1;
}

static void checkSide(Vector* pos, float* angle, int* side, float* av, bool hasAv) {
    Vector v;
    vectorSet(&v, 0, 0);
    addWithAngle(&v, *angle, 1);

    if ((pos->x < -40 && v.x < 0) || (pos->x > 40 && v.x > 0)) {
        *side *= -1;
        v.x *= -1;
        if (hasAv && av != NULL) {
            *av *= -1;
            lightTailCount += 2;
        }
    }
    if ((pos->y < -40 && v.y < 0) || (pos->y > 40 && v.y > 0)) {
        *side *= -1;
        v.y *= -1;
        if (hasAv && av != NULL) {
            *av *= -1;
            lightTailCount += 2;
        }
    }

    if (lightTailCount >= tailCount - 1) {
        lightTailCount = tailCount - 1;
    }
    *angle = vectorAngle(&v);
}

void addGameTwofaced() {
    addGame(title, description, characters, charactersCount, options, update);
}