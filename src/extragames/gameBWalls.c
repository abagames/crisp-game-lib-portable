#include "cglp.h"

static char *title = "B WALLS";
static char *description = "[Tap] Shoot";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Player 'a'
        "  ll  ",
        "  ll  ",
        "rrLLrr",
        "rrLLrr",
        "  ll  ",
        "      ",
    },
    {   // Enemy 'b'
        "l ll l",
        " lyyl ",
        "lyyyyl",
        " ylly ",
        " ylly ",
        "  ll  ",
    },
    {   // Shot 'c'
        " y    ",
        "yyy   ",
        "lll   ",
        "yyy   ",
        "lll   ",
        " y    ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 19,
    .isDarkColor = false
};

typedef struct {
    float y;
    float width;
    float interval;
    float ox;
    float vx;
    bool isAlive;
} Wall;

// Helper function to match JS modulo behavior
static float jsModulo(float x, float m) {
    float result = fmod(x, m);
    return result >= 0 ? result : result + m;
}

#define MAX_WALLS 100
static Wall walls[MAX_WALLS];
static int wallIndex;
static float nextWallDist;
static float scr;
static float wallTicks;
static float shotY;
static bool hasShot;
static int multiplier;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(walls);
        wallIndex = 0;
        nextWallDist = 0;
        scr = 0;
        wallTicks = 0;
        shotY = 0;
        hasShot = false;
        multiplier = 1;
    }

    const float wallStopInterval = 120 / sqrt(difficulty);
    const bool isWallStopping = jsModulo(wallTicks / wallStopInterval, 1.0f) > 0.5f;

    if (!hasShot && input.isJustPressed) {
        play(POWER_UP);
        shotY = 90;
        hasShot = true;
        multiplier = 1;
    }

    if (hasShot) {
        shotY -= sqrt(difficulty) * 3;
        character("c", 50, shotY);
        if (shotY > 66) {
            scr += (shotY - 66) * 0.1f;
        }
    }

    if (!isWallStopping) {
        play(HIT);
    }

    float maxY = 0;
    FOR_EACH(walls, i) {
        ASSIGN_ARRAY_ITEM(walls, i, Wall, w);
        SKIP_IS_NOT_ALIVE(w);

        w->y += scr;
        if (w->y > maxY) {
            maxY = w->y;
        }

        if (!isWallStopping) {
            w->ox = jsModulo(w->ox + w->vx, w->width + w->interval);
        }

        // Draw all wall segments
        float x = w->ox - w->width;
        color = YELLOW;
        while (x < 99) {
            Collision c = rect(x, w->y, w->width, 5);
            if (hasShot && c.isColliding.character['c']) {
                play(SELECT);
                hasShot = false;
                shotY = 0;
            }
            x += w->width + w->interval;
        }

        color = BLACK;
        Collision c = character("b", 50, w->y - 3);
        if (hasShot && c.isColliding.character['c']) {
            play(EXPLOSION);
            addScore(multiplier, 50, w->y - 3);
            multiplier *= 2;
            w->isAlive = false;
            continue;
        }
    }

    if (!hasShot) {
        wallTicks++;
    } else if (shotY < -9 || (!isWallStopping && shotY < maxY + 7)) {
        play(SELECT);
        hasShot = false;
        shotY = 0;
    }

    nextWallDist -= scr;
    if (nextWallDist < 0) {
        const float w = rnd(10, 20);
        const float i = rnd(20, 40);
        
        ASSIGN_ARRAY_ITEM(walls, wallIndex, Wall, wall);
        wall->y = -9 - nextWallDist;
        wall->width = w;
        wall->interval = i;
        wall->ox = rnd(0, w + i);
        wall->vx = 0;
        wall->isAlive = true;

        bool isValid = false;
        for (int attempt = 0; attempt < 99; attempt++) {
            float vx = rnd(5, 10) * RNDPM() * sqrt(difficulty); // rnds(5, 10)
            if (fabs(jsModulo(vx * wallStopInterval * 0.5f, w + i)) > 19) {
                wall->vx = vx;
                isValid = true;
                break;
            }
        }
        if (!isValid) {
            wall->width = 0;
        }

        wallIndex = wrap(wallIndex + 1, 0, MAX_WALLS);
        nextWallDist += 11;
    }

    if (maxY < 40) {
        scr += (40 - maxY) * 0.01f;
    } else {
        scr *= 0.5f;
    }
    scr += difficulty * 0.01f;

    color = BLACK;
    if (character("a", 50, 90).isColliding.rect[YELLOW]) {
        play(RANDOM);  // Equivalent to "lucky" in JS
        gameOver();
    }
}

void addGameBwalls() {
    addGame(title, description, characters, charactersCount, options, update);
}