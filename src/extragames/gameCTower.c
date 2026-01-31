//game requires this fixed https://github.com/abagames/crisp-game-lib-portable/issues/7
//see https://github.com/joyrider3774/crisp-game-lib-portable-sdl/commit/19c898cfb5a9ce179c31a619ada2f811d05b58d9
#include "cglp.h"

static char* title = "C TOWER";
static char* description = "[Hold] Climb";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {
    {
        "  ll  ",
        "  l  l",
        "lllll ",
        "  l   ",
        "ll ll ",
        "     l"
    },
    {
        "  ll  ",
        "l l   ",
        " lllll",
        "  l   ",
        " l lll",
        "l     "
    }
};
static int charactersCount = 2;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 50,
    .isDarkColor = true
};

typedef struct {
    float angle;
    float va;
    float tva;
    float y;
    float height;
    float speed;
} Player;

typedef struct {
    float angle;
    float y;
    float width;
    bool isFloating;
    bool isBonus;
    bool isAlive;
} Wall;

#define CTOWER_MAX_WALL_COUNT 128
static Wall walls[CTOWER_MAX_WALL_COUNT];
static int wallIndex;
static float nextWallDist;
static int nextBonusCount;
static int multiplier;
static const float radius = 40.0f;
static Player player;

static void update() {

    if (!ticks) {
        INIT_UNALIVED_ARRAY(walls);
        wallIndex = 0;
        player.angle = 0.0f;
        player.height = 0.0f;
        player.y = 80.0f;
        player.va = -1.0f;
        player.tva = -1.0f;
        player.speed = 3.0f;
        nextWallDist = 0.0f;
        nextBonusCount = 1;
        multiplier = 1;
    }

    if (input.isJustPressed) {
        play(LASER);
    }

    player.speed += ((input.isPressed ? 9.0f : 1.0f) * sqrt(difficulty) - player.speed) * 0.2f;
    player.va += (player.tva - player.va) * 0.2f;
    if (player.y > 80.0f) {
        player.y -= 0.01f;
    }
    player.angle += player.va * sqrt(difficulty) * 0.05f;
    const float scr = player.speed * 0.3f;
    player.height += scr;
    nextWallDist -= scr;

    if (nextWallDist < 0) {
        bool isBonus = false;
        nextBonusCount--;
        if (nextBonusCount < 0) {
            isBonus = true;
            nextBonusCount = rndi(9, 12);
        }
        
        ASSIGN_ARRAY_ITEM(walls, wallIndex, Wall, w);
        w->angle = rnd(0.0f, M_PI * 2.0f);
        w->width = rnd(0.5f, 1.0f) * M_PI / 2.0f;
        w->y = -5.0f;
        w->isFloating = isBonus || rnd(0.0f, 1.0f) < 0.3f;
        w->isBonus = isBonus;
        w->isAlive = true;
        wallIndex = wrap(wallIndex + 1, 0, CTOWER_MAX_WALL_COUNT);
        nextWallDist += rnd(30.0f, 40.0f);
    }
    FOR_EACH(walls, i) {
        ASSIGN_ARRAY_ITEM(walls, i, Wall, w);
        SKIP_IS_NOT_ALIVE(w);

        w->y += scr;
        float wa = player.angle - w->angle;
        float fa = clamp(wrap(wa - w->width / 2.0f, 0.0f, M_PI * 2.0f), M_PI / 2.0f, (M_PI / 2.0f) * 3.0f);
        float ta = clamp(wrap(wa + w->width / 2.0f, 0.0f, M_PI * 2.0f), M_PI / 2.0f, (M_PI / 2.0f) * 3.0f);

        if (fa < ta) {
            float fx = sinf(fa) * radius * (w->isFloating ? 1.2f : 1.1f);
            float tx = sinf(ta) * radius * (w->isFloating ? 1.2f : 1.1f);
            color = w->isBonus ? YELLOW : PURPLE;
            rect(fx + 50.0f, w->y, tx - fx, -5.0f);
        }
    }
    float ho = fmod(player.height, 400.0f) - 100.0f;
    // Draw backgrounds
    color = CYAN;
    rect(50.0f - radius, ho, radius * 2.0f, -100.0f);
    rect(50.0f - radius, fmod(ho + 200.0f, 400.0f), radius * 2.0f, -100.0f);
    
    color = LIGHT_CYAN;
    rect(50.0f - radius, fmod(ho + 100.0f, 400.0f), radius * 2.0f, -100.0f);
    rect(50.0f - radius, fmod(ho + 300.0f, 400.0f), radius * 2.0f, -100.0f);
    color = WHITE;

    float a = wrap(player.angle, -M_PI, M_PI);
    if (a > -M_PI / 2.0f && a < M_PI / 2.0f) {
        rect(50.0f + sinf(a) * radius, ho, 1.0f, -100.0f);
    }
    a = wrap(player.angle + M_PI / 2.0f, -M_PI, M_PI);
    if (a > -M_PI / 2.0f && a < M_PI / 2.0f) {
        rect(50.0f + sinf(a) * radius, fmod(ho + 100.0f, 400.0f), 1.0f, -100.0f);
    }
    a = wrap(player.angle + M_PI, -M_PI, M_PI);
    if (a > -M_PI / 2.0f && a < M_PI / 2.0f) {
        rect(50.0f + sinf(a) * radius, fmod(ho + 200.0f, 400.0f), 1.0f, -100.0f);
    }
    a = wrap(player.angle + (M_PI / 2.0f) * 3.0f, -M_PI, M_PI);
    if (a > -M_PI / 2.0f && a < M_PI / 2.0f) {
        rect(50.0f + sinf(a) * radius, fmod(ho + 300.0f, 400.0f), 1.0f, -100.0f);
    }

    FOR_EACH(walls, i) {
        ASSIGN_ARRAY_ITEM(walls, i, Wall, w);
        SKIP_IS_NOT_ALIVE(w);

        if (w->isFloating) {
            float wa = player.angle - w->angle;
            float fa = clamp(wrap(wa - w->width / 2.0f, -M_PI, M_PI), -M_PI / 2.0f, M_PI / 2.0f);
            float ta = clamp(wrap(wa + w->width / 2.0f, -M_PI, M_PI), -M_PI / 2.0f, M_PI / 2.0f);
            
            if (fa < ta) {
                float fx = sinf(fa) * radius * 1.2f;
                float tx = sinf(ta) * radius * 1.2f;
                color = w->isBonus ? LIGHT_YELLOW : LIGHT_PURPLE;
                float cfx = clamp(fx + 53.0f, 50.0f - radius, 50.0f + radius);
                float ctx = clamp(tx + 53.0f, 50.0f - radius, 50.0f + radius);
                rect(cfx, w->y + (w->isFloating ? 6.0f : 2.0f), ctx - cfx, -5.0f);
            }
        }
    }

    color = BLACK;
    characterOptions.isMirrorX = player.va < 0;
    char pc[2] = {'a' + (int)(player.height / 9.0f) % 2, '\0'};
    character(pc, 50.0f + 30.0f * player.va, player.y);
    characterOptions.isMirrorX = false;

    if (player.y > 97.0f) {
        play(EXPLOSION);
        gameOver();
    }

    FOR_EACH(walls, i) {
        ASSIGN_ARRAY_ITEM(walls, i, Wall, w);
        SKIP_IS_NOT_ALIVE(w);

        float wa = player.angle - w->angle;
        float fa = clamp(wrap(wa - w->width / 2.0f, -M_PI, M_PI), -M_PI / 2.0f, M_PI / 2.0f);
        float ta = clamp(wrap(wa + w->width / 2.0f, -M_PI, M_PI), -M_PI / 2.0f, M_PI / 2.0f);

        if (fa < ta) {
            float fx = sinf(fa) * radius * (w->isFloating ? 1.2f : 1.1f);
            float tx = sinf(ta) * radius * (w->isFloating ? 1.2f : 1.1f);

            if (!w->isFloating) {
                color = LIGHT_PURPLE;
                float cfx = clamp(fx + 51.0f, 50.0f - radius, 50.0f + radius);
                float ctx = clamp(tx + 51.0f, 50.0f - radius, 50.0f + radius);
                rect(cfx, w->y + 2.0f, ctx - cfx, -5.0f);
            }

            color = w->isBonus ? YELLOW : PURPLE;
            hasCollision = true;
            Collision c = rect(fx + 50.0f, w->y, tx - fx, -5.0f);

            if (c.isColliding.character['a'] || c.isColliding.character['b']) {
                if (w->isBonus) {
                    play(POWER_UP);
                    multiplier++;
                    w->isBonus = false;
                } else if (!w->isFloating) {
                    play(COIN);
                    player.angle = player.tva > 0 ? 
                        w->angle - w->width / 2.0f - 0.7f : 
                        w->angle + w->width / 2.0f + 0.7f;
                    player.tva *= -1.0f;
                    player.y = clamp(w->y + 6.0f, 0.0f, 99.0f);
                }
            }
        }

        if (w->y > 105.0f) {
            w->isAlive = false;
        }
    }

    color = BLACK;
    char scoreText[16];
    sprintf(scoreText, "x%d", multiplier);
    text(scoreText, 3, 9);
    addScore(scr * multiplier, 0.0f, 0.0f);
}

void addGameCTower() {
    addGame(title, description, characters, charactersCount, options, update);
}