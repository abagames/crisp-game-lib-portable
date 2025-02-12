//note < > in center don't seem to draw 100% the same as in js version

#include "cglp.h"

static char *title = "ANT LION";
static char *description = "[Hold] Walk";

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
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 2,
    .isDarkColor = false
};

typedef struct {
    float angle;
    float av;      // Angular velocity
    float dist;
    struct Bar* bar;  // Pointer to current bar being grabbed
    float barLength;
    float ticks;
} Ant;

typedef struct Bar {
    float angle;
    float av;      // Angular velocity
    float length;
    float dist;
    float speed;
    bool isAlive;
} Bar;

#define MAX_BAR_COUNT 100
static Bar bars[MAX_BAR_COUNT];
static int barIndex;
static float nextBarTicks;
static float sandTicks;
static int multiplier;
static Ant ant;
static Vector cp;  // Center point

static void update() {
    barCenterPosRatio = 0.0f;  // Important for bar positioning

    if (!ticks) {
        INIT_UNALIVED_ARRAY(bars);
        barIndex = 0;
        nextBarTicks = 0;
        sandTicks = 0;
        multiplier = 0;

        // Initialize ant
        ant.angle = M_PI / 2;
        ant.av = 0;
        ant.dist = 40;
        ant.bar = NULL;
        ant.barLength = 0;
        ant.ticks = 0;

        // Set center point
        vectorSet(&cp, 50, 50);
    }
    
    // Draw sand circle
    color = LIGHT_YELLOW;
    sandTicks += difficulty;   
    thickness = 5;
    arc(50, 50, wrap(70 - ((int)(sandTicks / 3) % 70), 0, 70), 0, M_PI * 2);

    // Draw center arrows
    thickness = 1;
    color = RED;
    text("<", 48.0f + sinf(ticks * 0.1f) * 2.0f, 50);
    text(">", 52.0f - sinf(ticks * 0.1f) * 2.0f, 50);

    color = BLACK;
    Vector ap;  // Ant position
    vectorSet(&ap, cp.x, cp.y);
    Collision c;

    if (ant.bar == NULL) {
        if (input.isPressed) {
            play(HIT);
            ant.ticks += difficulty;
        }
        
        
        // Move ant
        addWithAngle(&ap, ant.angle, ant.dist);
        ant.av += ((input.isPressed ? 1 : -1) * 0.02f * difficulty - ant.av) * 0.1f;
        ant.angle += ant.av;
        ant.dist -= 0.05f * difficulty * 
            ((ap.x >= 0 && ap.x <= 99 && ap.y >= 0 && ap.y <= 99) ? 1 : 2);

        // Draw ant body
        thickness = 3;
        c = bar(VEC_XY(ap), 3, ant.angle + M_PI / 2);

        // Draw legs exactly like JS
        addWithAngle(&ap, ant.angle, 3);
        thickness = 2;
        barCenterPosRatio = 0.5f + sinf(ant.ticks * 0.3f) * 0.4f;
        bar(VEC_XY(ap), 5, ant.angle + M_PI / 2);
        addWithAngle(&ap, ant.angle, -6);
        barCenterPosRatio = 0.5f - sinf(ant.ticks * 0.3f) * 0.4f;
        bar(VEC_XY(ap), 5, ant.angle + M_PI / 2);
        barCenterPosRatio = 0.0f;
    } else {
        play(LASER);
        ant.ticks += difficulty;
        ant.barLength += difficulty * 0.3f;
        ant.bar->speed += difficulty * 0.002f;
        
        ant.angle = ant.bar->angle;
        ant.dist = ant.bar->dist + ant.barLength;
        
        addWithAngle(&ap, ant.angle, ant.dist);
        thickness = 4;
        c = bar(VEC_XY(ap), 3, ant.angle);

        addWithAngle(&ap, ant.angle + M_PI / 2, 3);
        thickness = 2;
        barCenterPosRatio = 0.5 + sinf(ant.ticks * 0.3) * 0.4;
        bar(VEC_XY(ap), 5, ant.angle);
        addWithAngle(&ap, ant.angle + M_PI / 2, -6);
        barCenterPosRatio = 0.5 - sinf(ant.ticks * 0.3) * 0.4;
        bar(VEC_XY(ap), 5, ant.angle);
        barCenterPosRatio = 0.0f;

        if (ant.barLength > ant.bar->length + 6) {
            ant.bar = NULL;
        }
    }

    if (c.isColliding.text['<'] || c.isColliding.text['>']) {
        play(RANDOM);  // Changed to match JS "lucky"
        gameOver();
        return;
    }

    nextBarTicks--;
    if (nextBarTicks < 0) {
        float length = rnd(10, 30);
        ASSIGN_ARRAY_ITEM(bars, barIndex, Bar, b);
        b->angle = rnd(0, M_PI * 2);
        b->av = (rnd(0.5f, 1.0f) * difficulty * -0.05f) / length;
        b->length = length;
        b->dist = 75;
        b->speed = rnd(0.5f, 1.0f) * difficulty * 0.2f;
        b->isAlive = true;
        barIndex = wrap(barIndex + 1, 0, MAX_BAR_COUNT);
        nextBarTicks = rnd(150, 180) / difficulty;
    }

    // Update and draw bars
    color = BLUE;
    FOR_EACH(bars, i) {
        ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
        SKIP_IS_NOT_ALIVE(b);

        b->angle += b->av;
        Vector p;
        vectorSet(&p, cp.x, cp.y);
        addWithAngle(&p, b->angle, b->dist);

        b->dist -= b->speed * 
            ((p.x >= 0 && p.x <= 99 && p.y >= 0 && p.y <= 99) ? 1 : 5);

        thickness = 3;
        Collision barCol = bar(VEC_XY(p), b->length, b->angle);
        
        if (barCol.isColliding.rect[BLACK] && ant.bar == NULL) {
            play(COIN);
            multiplier += 2;
            addScore(multiplier, VEC_XY(ap));
            ant.bar = b;
            ant.barLength = ant.dist - b->dist;
        }

        if (b->dist < 0) {
            b->length += b->dist;
            b->dist = 0;
        }

        if (b->length < 0) {
            play(SELECT);
            if (ant.bar == b) {
                ant.bar = NULL;
            }
            if (multiplier > 1) {
                multiplier--;
            }
            b->isAlive = false;
        }
    }

    // Reset thickness before text
    thickness = 1;
    // Draw multiplier
    color = BLACK;
    char multText[16];
    sprintf(multText, "x%d", multiplier);
    text(multText, 3, 9);
}

void addGameAntLion() {
    addGame(title, description, characters, charactersCount, options, update);
}