#include "cglp.h"

static char *title = "BOMB UP";
static char *description = "[Tap]\n Throw\n Blast";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Player standing 'a'
        "llllll",
        "ll l l",
        "ll l l",
        "llllll",
        " l  l ",
        " l  l ",
    },
    {   // Player jumping 'b'
        "llllll",
        "ll l l",
        "ll l l",
        "llllll",
        "ll  ll",
        "      ",
    },
    {   // Bomb 'c'
        "  r   ",
        " r    ",
        " ll   ",
        "llll  ",
        "llll  ",
        " ll   ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 90,
    .isDarkColor = true
};

typedef struct {
    Vector pos;
    Vector vel;
} MobileEntity;  // For player and bomb which don't need isAlive

typedef struct {
    Vector pos;
    Vector vel;
    bool isAlive;
} Rock;  // For rocks that need isAlive flag

typedef struct {
    Vector pos;
    float ticks;
    bool exists;
} Explosion;

#define MAX_ROCKS 100
static MobileEntity player;
static MobileEntity bomb;
static bool hasBomb;
static Explosion explosion;
static Rock rocks[MAX_ROCKS];
static int rockIndex;
static float nextRockTicks;
static float nextRockX;
static float nextRockVelX;
static float wallY;
static int multiplier;

static void update() {
    if (!ticks) {
        // Initialize player
        vectorSet(&player.pos, 50, 50);
        vectorSet(&player.vel, 0, 0);
        
        // Initialize others
        hasBomb = false;
        explosion.exists = false;
        INIT_UNALIVED_ARRAY(rocks);
        rockIndex = 0;
        nextRockTicks = 0;
        nextRockX = 80;
        nextRockVelX = 0;
        wallY = 0;
        multiplier = 1;
    }

    float scr = 0;
    if (player.pos.y < 40) {
        scr = (40 - player.pos.y) * 0.3f;
    } else if (player.pos.y > 60) {
        scr = (60 - player.pos.y) * 0.2f;
    }

    // Update player
    player.vel.y += 0.02f;
    if ((player.pos.x < 7 && player.vel.x < 0) || 
        (player.pos.x > 93 && player.vel.x > 0)) {
        player.vel.x *= -0.7f;
        player.pos.x = clamp(player.pos.x, 7, 93);
    }
    vectorMul(&player.vel, 0.99f);
    vectorAdd(&player.pos, player.vel.x, player.vel.y);
    player.pos.y += scr;

    // Draw player
    color = BLACK;
    characterOptions.isMirrorX = player.vel.x < 0;
    char playerChar[2] = {player.vel.y < 0 ? 'a' : 'b', '\0'};
    character(playerChar, player.pos.x, player.pos.y);

    if (!hasBomb) {
        character("c", player.pos.x + (player.vel.x < 0 ? -2 : 2), player.pos.y);
    }

    // Handle input
    if (input.isJustPressed) {
        if (!hasBomb) {
            play(SELECT);
            hasBomb = true;
            float bombX = player.pos.x + (player.vel.x < 0 ? -2 : 2);
            vectorSet(&bomb.pos, bombX, player.pos.y);
            bomb.vel.x = player.vel.x + (player.vel.x < 0 ? -0.5f : 0.5f);
            bomb.vel.y = player.vel.y;
            multiplier = 1;
        } else {
            play(POWER_UP);
            explosion.exists = true;
            vectorSet(&explosion.pos, bomb.pos.x, bomb.pos.y);
            explosion.ticks = 0;
            color = LIGHT_RED;
            particle(bomb.pos.x, bomb.pos.y, 20, 3, 0, M_PI * 2);
            float d = distanceTo(&bomb.pos, player.pos.x, player.pos.y);
            float a = angleTo(&bomb.pos, player.pos.x, player.pos.y);
            addWithAngle(&player.vel, a, 20 / d);
            hasBomb = false;
        }
    }

    // Update bomb
    if (hasBomb) {
        if ((bomb.pos.x < 7 && bomb.vel.x < 0) ||
            (bomb.pos.x > 93 && bomb.vel.x > 0)) {
            bomb.vel.x *= -0.7f;
        }
        bomb.vel.y += 0.15f;
        vectorMul(&bomb.vel, 0.98f);
        vectorAdd(&bomb.pos, bomb.vel.x, bomb.vel.y);
        bomb.pos.y += scr;
        character("c", bomb.pos.x, bomb.pos.y);
        if (bomb.pos.y > 103) {
            hasBomb = false;
        }
    }

    // Update explosion
    if (explosion.exists) {
        explosion.ticks++;
        float r = sin(explosion.ticks * 0.15f) * 25;
        if (r < 0) {
            explosion.exists = false;
        } else {
            color = LIGHT_RED;
            arc(explosion.pos.x, explosion.pos.y, r, 0, M_PI * 2);
        }
    }

    // Update rocks
    nextRockTicks--;
    nextRockVelX += (rnd(0, 1) * 2 - 1) * 0.1f * sqrt(difficulty);
    nextRockVelX *= 0.99f;
    nextRockX += nextRockVelX;
    if ((nextRockX < 7 && nextRockVelX < 0) ||
        (nextRockX > 93 && nextRockVelX > 0)) {
        nextRockVelX *= -0.7f;
        nextRockX = clamp(nextRockX, 7, 93);
    }

    if (nextRockTicks < 0) {
        bool isBottom = rnd(0, 1) > (player.pos.y > 30 ? 0.05f : 0.9f);  // Single rnd() call
        ASSIGN_ARRAY_ITEM(rocks, rockIndex, Rock, r);
        vectorSet(&r->pos, nextRockX, isBottom ? 103 : -3);
        float yvel = isBottom ? -rnd(0.5f, sqrt(difficulty)) : player.vel.y / 2;
        vectorSet(&r->vel, 0, yvel);
        r->isAlive = true;
        rockIndex = wrap(rockIndex + 1, 0, MAX_ROCKS);
        nextRockTicks = rnd(8, 10) / difficulty;
    }

            // Update and draw rocks
    color = RED;
    FOR_EACH(rocks, i) {
        ASSIGN_ARRAY_ITEM(rocks, i, Rock, r);
        SKIP_IS_NOT_ALIVE(r);

        // Match JS update order exactly
        vectorAdd(&r->pos, r->vel.x, r->vel.y);
        r->vel.y += 0.01f;
        vectorMul(&r->vel, 0.99f);
        vectorAdd(&r->pos, r->vel.x, r->vel.y);
        r->pos.y += scr;

        // Fix particle to match JS (only pass angle once)
        float angle = vectorAngle(&r->vel);
        particle(r->pos.x, r->pos.y, 0.1f, vectorLength(&r->vel) * -0.5f, 
                angle, angle);  // Note: might only need one angle param

        Collision c = box(r->pos.x, r->pos.y, 5, 5);
        if (c.isColliding.rect[LIGHT_RED]) {
            play(COIN);
            addScore(multiplier, r->pos.x, r->pos.y);
            multiplier++;
            particle(r->pos.x, r->pos.y, 9, 1, 0, M_PI * 2);
            r->isAlive = false;
            continue;
        } else if (c.isColliding.character['a'] || c.isColliding.character['b']) {
            play(EXPLOSION);
            gameOver();
            return;
        }

        if (r->pos.y < -50 || r->pos.y > 150) {
            r->isAlive = false;
        }
    }

    // Draw walls
    wallY += scr;
    color = LIGHT_BLACK;
    for (int i = 0; i < 18; i++) {
        box(3, i * 6 + ((int)wallY % 6), 4, 4);
        box(97, i * 6 + ((int)wallY % 6), 4, 4);
    }
}

void addGameBombup() {
    addGame(title, description, characters, charactersCount, options, update);
}