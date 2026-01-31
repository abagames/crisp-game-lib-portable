#include "cglp.h"

static char *title = "CATE P";
static char *description = "[Tap]\n Turn & Shot";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        " llll  ",
        "llllll ",
        "lllrrr ",
        "lllrrr ",
        "llllll ",
        " llll  ",
    },
    {
        " llll  ",
        "llllll ",
        "lll    ",
        "lll    ",
        "llllll ",
        " llll  ",
    },
    {
        " l ll  ",
        "     l ",
        " lllll ",
        "     l ",
        " l ll  ",
        "       ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 11,
    .isDarkColor = true
};

#define MAX_CATE_COUNT 32

typedef struct {
    Vector pos;
    int angle;
    float speed;
    float ticks;
    bool isAppearing;
    bool isAlive;
} Cate;

typedef struct {
    Vector pos;
    int angle;
    float speed;
    float ticks;
    int count;
    bool isAlive;
} AppCate;

typedef struct {
    Vector pos;
    int angle;
} Player;

typedef struct {
    Vector pos;
    int angle;
    float multiplier;
    bool isAlive;
} Shot;

static Cate cates[MAX_CATE_COUNT];
static AppCate appCate;
static Cate* appearingCate;
static Player player;
static Shot shot;
static float appCateTicks;

// Static vector array for angle-based movement
static Vector angleVec[] = {
    {1, 0},    // 0
    {0.7, 0.7},// 1
    {0, 1},    // 2
    {-0.7, 0.7},// 3
    {-1, 0},   // 4
    {-0.7, -0.7},// 5
    {0, -1},   // 6
    {0.7, -0.7} // 7
};

static int addCate(Vector pos, int angle, float speed, float ticks, bool isAppearing) {
    FOR_EACH(cates, i) {
        ASSIGN_ARRAY_ITEM(cates, i, Cate, c);
        if (!c->isAlive) {
            vectorSet(&c->pos, VEC_XY(pos));
            c->angle = angle;
            c->speed = speed;
            c->ticks = ticks;
            c->isAppearing = isAppearing;
            c->isAlive = true;
            return i;  // Return the index where we added the Cate
        }
    }
    return -1;  // Return -1 if we couldn't add the Cate
}

static void update() {
    if (!ticks) {
        // Initialize game state
        INIT_UNALIVED_ARRAY(cates);
        appCateTicks = 0;
        appearingCate = NULL;
        
        vectorSet(&player.pos, 50.0f, 50.0f);
        player.angle = 0;
        
        shot.isAlive = false;
    }

    // Draw border
    color = LIGHT_BLUE;
    rect(0, 0, 99, 6);
    rect(0, 93, 99, 6);
    rect(0, 6, 6, 87);
    rect(93, 6, 6, 87);

    // Player movement and shot - using same boundaries as JavaScript's isInRect(9, 9, 82, 82)
    if ((ticks && input.isJustPressed) || 
        player.pos.x < 9 || player.pos.x > (9 + 82) ||
        player.pos.y < 9 || player.pos.y > (9 + 82)) {
        if (!shot.isAlive && input.isJustPressed) {
            play(POWER_UP);
            vectorSet(&shot.pos, VEC_XY(player.pos));
            shot.angle = player.angle;
            shot.multiplier = 1;
            shot.isAlive = true;
        }
        player.pos.x = clamp(player.pos.x, 9, 9 + 81);
        player.pos.y = clamp(player.pos.y, 9, 9 + 81);
        player.angle = wrap(player.angle + 2, 0, 8);
    }

    // Move player
    player.pos.x += angleVec[player.angle].x * difficulty * 0.2f;
    player.pos.y += angleVec[player.angle].y * difficulty * 0.2f;

    // Draw player
    color = BLACK;
    characterOptions.rotation = player.angle / 2;
    character(shot.isAlive ? "b" : "a", VEC_XY(player.pos));
    characterOptions.rotation = 0;

    // Handle shot
    if (shot.isAlive) {
                
        if (shot.pos.x < 9 || shot.pos.x > (9 + 82) || 
            shot.pos.y < 9 || shot.pos.y > (9 + 82)) {
            shot.isAlive = false;
        } else {
            shot.pos.x += angleVec[shot.angle].x * difficulty * 1.5f;
            shot.pos.y += angleVec[shot.angle].y * difficulty * 1.5f;
            color = BLACK;
            characterOptions.rotation = shot.angle / 2;
            character("c", VEC_XY(shot.pos));
            characterOptions.rotation = 0;
        }
    }

    // Spawn new Cate
    if (appCateTicks <= 0) {
        appCate.isAlive = true;
        vectorSet(&appCate.pos, rnd(9.0f, 90.0f), rnd(0.0f, 1.0f) < 0.5f ? -3.0f : 103.0f);
        
        if (rnd(0.0f, 1.0f) < 0.5f) {
            float temp = appCate.pos.x;
            appCate.pos.x = appCate.pos.y;
            appCate.pos.y = temp;
        }

        // Determine initial angle based on spawn position
        if (appCate.pos.x > 99) appCate.angle = 4;
        else if (appCate.pos.y > 99) appCate.angle = 6;
        else if (appCate.pos.x < 0) appCate.angle = 0;
        else appCate.angle = 2;

        appCate.speed = rnd(1.0f, difficulty) * 0.2f;
        appCate.ticks = rnd(0.0f, 999.0f);
        appCate.count = rndi(5, 9);
        
        appCateTicks = rnd(60.0f, 90.0f) / difficulty;
    }

    // Process appearing Cate
    appCate.ticks += appCate.speed;
    if (appCate.count <= 0) {
        appCateTicks--;
    }

    // Check for new Cate spawning
    if(appearingCate != NULL)
        if (appearingCate->pos.x >= 3 && appearingCate->pos.x <= (3 + 94) &&
            appearingCate->pos.y >= 3 && appearingCate->pos.y <= (3 + 94)) {
            appearingCate = NULL;
        }

    // Spawn Cate if possible
    if (appCate.count > 0 && appearingCate == NULL) {
    play(LASER);
    
    Vector newPos;
    vectorSet(&newPos, VEC_XY(appCate.pos));
    int newCateIndex = addCate(
        newPos, 
        appCate.angle, 
        appCate.speed, 
        appCate.ticks, 
        true
    );
    
    if (newCateIndex >= 0) {
        appearingCate = &cates[newCateIndex];
        appCate.count--;
    }
}
    // Process existing Cates
    FOR_EACH(cates, i) {
        ASSIGN_ARRAY_ITEM(cates, i, Cate, c);
        SKIP_IS_NOT_ALIVE(c);

        c->pos.x += angleVec[c->angle].x * c->speed;
        c->pos.y += angleVec[c->angle].y * c->speed;

        // Handle appearing state
        if (c->isAppearing) {
            c->isAppearing = !(
                c->pos.x >= 9 && c->pos.x <= 9 + 81 &&
                c->pos.y >= 9 && c->pos.y <= 9 + 81
            );
        } else {
            int hitCount = 0;
            if (c->pos.x < 9 || c->pos.x > (9 + 81)) hitCount++;
            if (c->pos.y < 9 || c->pos.y > (9 + 81)) hitCount++;

            if (hitCount == 1) {
                c->angle = wrap(c->angle + 3, 0, 8);
                c->pos.x = clamp(c->pos.x, 9, 9 + 81);
                c->pos.y = clamp(c->pos.y, 9, 9 + 81);
            } else if (hitCount == 2) {
                c->angle = wrap(c->angle + 4, 0, 8);
                c->pos.x = clamp(c->pos.x, 9, 9 + 81);
                c->pos.y = clamp(c->pos.y, 9, 9 + 81);
            }
        }

        // Update ticks
        c->ticks += c->speed;
        int t = wrap((int)c->ticks, 0, 8);
        float lo = t < 4 ? (5 * t) / 5.0f - 2 : (5 * (8 - t)) / 5.0f - 2;

        // Draw Cate trajectory
        color = RED;
        Vector o;
        vectorSet(&o, 3.0f, 0.0f);
        rotate(&o, (c->angle * M_PI) / 4.0f + M_PI / 2);
        Vector boxPos1;
        vectorSet(&boxPos1, VEC_XY(c->pos));
        vectorAdd(&boxPos1, o.x, o.y);
        Vector o2;
        vectorSet(&o2, lo, 0.0f);
        rotate(&o2, (c->angle * M_PI) / 4.0f);
        vectorAdd(&boxPos1, o2.x, o2.y);
        box(VEC_XY(boxPos1), 2, 2);

        vectorSet(&o, -3.0f, 0.0f);
        rotate(&o, (c->angle * M_PI) / 4.0f + M_PI / 2);
        Vector boxPos2;
        vectorSet(&boxPos2, VEC_XY(c->pos));
        vectorAdd(&boxPos2, o.x, o.y);
        vectorSet(&o2, -lo, 0.0f);
        rotate(&o2, (c->angle * M_PI) / 4.0f);
        vectorAdd(&boxPos2, o2.x, o2.y);
        box(VEC_XY(boxPos2), 2, 2);

        // Check collision
        color = GREEN;
        Collision col = box(VEC_XY(c->pos), 5, 5);        
        if (col.isColliding.character['c']) {
            if (shot.isAlive) {
                play(COIN);
                addScore(shot.multiplier, VEC_XY(c->pos));
                particle(VEC_XY(c->pos), 9.0f, sqrt(shot.multiplier), 0.0f, M_PI * 2.0f);
                shot.multiplier++;

                if (c == appearingCate) {
                    appearingCate = NULL;
                }
                
                c->isAlive = false;
            }
        }
        
        if (col.isColliding.character['a'] || col.isColliding.character['b']) {
            play(EXPLOSION);
            gameOver();
        }
    }
}

void addGameCateP() {
    addGame(title, description, characters, charactersCount, options, update);
}