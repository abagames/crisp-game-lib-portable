//does not seem to work correctly
//player missiles targetting seems of
//over time a small floating point value is accumlated and it affects the drawin of player's plane exhaust
//players plane exhaust is drawn in front of player instead of behind (could be related to SDL)

#include "cglp.h"

static char *title = "ACCEL B";
static char *description = "[Tap]  Fire\n[Hold] Accel";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Player 'a'
        "      ",
        "      ",
        "ll    ",
        " lllll",
        "      ",
        "      ",
    },
    {   // Enemy 'b'
        "      ",
        "   lll",
        " lllll",
        "llllll",
        "   ll ",
        "      ",
    }
};
static int charactersCount = 2;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 20,
    .isDarkColor = false
};

typedef struct {
    Vector pos;
    float vx;
} Player;

typedef struct {
    Vector pos;
    Vector vel;
    Vector target;
    float ticks;
    float exTicks;
    float smokeTicks;
    bool isAlive;
} PlayerMissile;

typedef struct {
    Vector pos;
    float vx;
    float ma;  // missile angle
    float fireTicks;
    bool isAlive;
} Enemy;

typedef struct {
    Vector pos;
    float angle;
    float va;  // velocity angle
    float speed;
    float smokeTicks;
    bool isAlive;
} Missile;

typedef struct {
    Vector pos;
    Vector vel;
    float ticks;
    bool isEnemy;
    bool isAlive;
} Smoke;

typedef struct {
    Vector pos;
    Vector size;
} Forest;

#define MAX_PLAYER_MISSILES 100
#define MAX_ENEMIES 100
#define MAX_MISSILES 100
#define MAX_SMOKES 1000
#define MAX_FORESTS 7

static Player player;
static PlayerMissile playerMissiles[MAX_PLAYER_MISSILES];
static Enemy enemies[MAX_ENEMIES];
static int enemyIndex;
static float nextEnemyDist;
static Missile missiles[MAX_MISSILES];
static int missileIndex;
static Smoke smokes[MAX_SMOKES];
static int smokeIndex;
static Forest forests[MAX_FORESTS];
static int multiplier;

static void update() {

    thickness = 1;  // Default thickness

    if (!ticks) {
        // Initialize player
        vectorSet(&player.pos, 20, 45);
        player.vx = 0;

        characterOptions.rotation = 0;
        characterOptions.isMirrorX = false;
        characterOptions.isMirrorY = false;
        // Initialize arrays
        INIT_UNALIVED_ARRAY(playerMissiles);
        INIT_UNALIVED_ARRAY(enemies);
        INIT_UNALIVED_ARRAY(missiles);
        INIT_UNALIVED_ARRAY(smokes);
        enemyIndex = 0;
        missileIndex = 0;
        smokeIndex = 0;
        nextEnemyDist = 0;
        multiplier = 1;

        // Initialize forests
        for (int i = 0; i < MAX_FORESTS; i++) {
            vectorSet(&forests[i].pos, -99 - i * 40, 0);
            vectorSet(&forests[i].size, 0, 0);
        }
    }

    float scr = (player.pos.x - 20) * 0.1f;

    // Draw ground
    color = YELLOW;
    rect(0, 90, 200, 10);

    // Draw and update forests
    color = GREEN;
    for (int i = 0; i < MAX_FORESTS; i++) {
        Forest* f = &forests[i];
        box(f->pos.x, f->pos.y, f->size.x, f->size.y);
        f->pos.x -= scr;
        if (f->pos.x < -99) {
            f->pos.x += 300 + rnd(0, 99);
            f->pos.y = rnd(90, 99);
            f->size.x = rnd(30, 50);
            f->size.y = rnd(5, 9);
        }
    }

    // Update smokes
    FOR_EACH(smokes, i) {
        ASSIGN_ARRAY_ITEM(smokes, i, Smoke, s);
        SKIP_IS_NOT_ALIVE(s);

        vectorAdd(&s->pos, s->vel.x, s->vel.y);
        s->pos.x -= scr;
        vectorMul(&s->vel, 0.95f);
        
        color = s->isEnemy && s->ticks < 20 ? LIGHT_RED : LIGHT_BLACK;
        float boxSize = 3 + cos(s->ticks * 0.03f) * 5;  // Calculate size once
        box(s->pos.x, s->pos.y, boxSize, boxSize);  // Same size for width/height
        
        s->ticks += sqrt(difficulty);
        if (s->ticks > 60) {
            s->isAlive = false;
        }
    }

    // Update player
    player.vx += ((input.isPressed ? 2 : 0.2f) * sqrt(difficulty) - player.vx) * 0.2f;
    player.pos.x += player.vx - scr;

    

    color = BLUE;
    character("a", player.pos.x, player.pos.y);

    color = PURPLE;
    rect(player.pos.x - 3, player.pos.y, -player.vx * 3, 1);
    
    // Count active player missiles
    int activeMissiles = 0;
    FOR_EACH(playerMissiles, i) {
        if (playerMissiles[i].isAlive) activeMissiles++;
    }

    if (activeMissiles == 0) {
        color = CYAN;
        box(player.pos.x, player.pos.y + 3, 5, 2);

        if (input.isJustPressed) {
            play(SELECT);
            multiplier = 1;

             if (input.isJustPressed) {
            play(SELECT);
            multiplier = 1;

            // Create missiles for each enemy
            FOR_EACH(enemies, i) {
                ASSIGN_ARRAY_ITEM(enemies, i, Enemy, e);
                SKIP_IS_NOT_ALIVE(e);

                ASSIGN_ARRAY_ITEM(playerMissiles, enemyIndex, PlayerMissile, pm);
                vectorSet(&pm->pos, player.pos.x, player.pos.y + 3);
                Vector v;  // Create temporary vector for velocity
                vectorSet(&v, sqrt(difficulty) * 2, 0);  // Only X component
                pm->vel = v;  // Copy the vector
                Vector t;  // Create temporary vector for target
                vectorSet(&t, e->pos.x, e->pos.y);
                pm->target = t;  // Copy the vector
                pm->ticks = 0;
                pm->exTicks = 0;
                pm->smokeTicks = 0;
                pm->isAlive = true;
                enemyIndex = wrap(enemyIndex + 1, 0, MAX_PLAYER_MISSILES);
            }

            // Create missiles for each enemy missile
            FOR_EACH(missiles, i) {
                ASSIGN_ARRAY_ITEM(missiles, i, Missile, m);
                SKIP_IS_NOT_ALIVE(m);

                ASSIGN_ARRAY_ITEM(playerMissiles, enemyIndex, PlayerMissile, pm);
                vectorSet(&pm->pos, player.pos.x, player.pos.y + 3);
                Vector v;  // Create temporary vector for velocity
                vectorSet(&v, sqrt(difficulty) * 2, 0);  // Only X component
                pm->vel = v;  // Copy the vector
                Vector t;  // Create temporary vector for target
                vectorSet(&t, m->pos.x, m->pos.y);
                pm->target = t;  // Copy the vector
                pm->ticks = 0;
                pm->exTicks = 0;
                pm->smokeTicks = 0;
                pm->isAlive = true;
                enemyIndex = wrap(enemyIndex + 1, 0, MAX_PLAYER_MISSILES);
            }
        }
        }
    }

    // Update player missiles
    int prevActiveMissiles = activeMissiles;
    activeMissiles = 0;
    
    FOR_EACH(playerMissiles, i) {
        ASSIGN_ARRAY_ITEM(playerMissiles, i, PlayerMissile, m);
        SKIP_IS_NOT_ALIVE(m);

        vectorAdd(&m->pos, m->vel.x, m->vel.y);
        m->pos.x += sqrt(difficulty) - scr;

        if (m->exTicks > 0) {
            if (m->pos.y > 90) {
                vectorSet(&m->vel, 0, 0);
            }
            m->exTicks += sqrt(difficulty);
            vectorMul(&m->vel, 0.9f);
            color = RED;
            float boxSize = 3 + cos(m->exTicks * 0.05f) * 9;
            box(m->pos.x, m->pos.y, boxSize, boxSize);
            if (m->exTicks > 30) {
                m->isAlive = false;
            }
            continue;  // Skip rest of missile code when exploding, matching JS
        }

        float d = distanceTo(&m->pos, m->target.x, m->target.y);
        if (d < 9 || m->pos.y > 95 || m->ticks > 120) {
            play(POWER_UP);
            m->exTicks = 1;
            float s = vectorLength(&m->vel);
            float angle = angleTo(&m->pos, m->target.x, m->target.y);
            vectorSet(&m->vel, 0, 0);
            addWithAngle(&m->vel, angle, s);
        }

        float mv = (sqrt(difficulty) / sqrt(d + 9)) * 
                   (m->ticks < 9 ? 0.1f : m->ticks < 20 ? 3 : 1);
        float targetAngle = angleTo(&m->pos, m->target.x, m->target.y);
        addWithAngle(&m->vel, targetAngle, mv);
        vectorMul(&m->vel, m->ticks < 20 ? 0.7f : 0.95f);
        m->ticks += sqrt(difficulty);

        color = CYAN;
        thickness = 2;  // Match JS thickness
        bar(VEC_XY(m->pos), 3, vectorAngle(&m->vel));
        thickness = 1;  // Reset thickness

        m->smokeTicks += sqrt(difficulty);
        if (m->smokeTicks > 5) {
            ASSIGN_ARRAY_ITEM(smokes, smokeIndex, Smoke, s);
            vectorSet(&s->pos, m->pos.x, m->pos.y);
            vectorSet(&s->vel, m->vel.x * 0.5f, m->vel.y * 0.5f);
            s->ticks = 0;
            s->isEnemy = false;
            s->isAlive = true;
            smokeIndex = wrap(smokeIndex + 1, 0, MAX_SMOKES);
            m->smokeTicks -= 5;
        }
        activeMissiles++;
    }

    if (prevActiveMissiles > 0 && activeMissiles == 0) {
        play(COIN);
    }

    // Spawn enemies
    nextEnemyDist -= scr;
    if (nextEnemyDist < 0) {
        ASSIGN_ARRAY_ITEM(enemies, enemyIndex, Enemy, e);
        vectorSet(&e->pos, 203, rnd(0, 1) < 0.5f ? rnd(5, 35) : rnd(55, 85));
        e->vx = rnd(0, sqrt(difficulty)) * 0.5f;
        e->ma = M_PI + rnd(0.2, M_PI / 5) * RNDPM(),
        e->fireTicks = ceil(rnd(10, 30) / sqrt(difficulty));
        e->isAlive = true;
        enemyIndex = wrap(enemyIndex + 1, 0, MAX_ENEMIES);
        nextEnemyDist += rnd(40, 60) / difficulty;
    }

    // Update enemies
    FOR_EACH(enemies, i) {
        ASSIGN_ARRAY_ITEM(enemies, i, Enemy, e);
        SKIP_IS_NOT_ALIVE(e);

        e->pos.x -= e->vx + scr;
        e->fireTicks--;

        if (e->fireTicks == 0) {
            play(LASER);
            ASSIGN_ARRAY_ITEM(missiles, missileIndex, Missile, m);
            vectorSet(&m->pos, e->pos.x, e->pos.y + 3);
            m->angle = e->ma;
            m->va = 0;
            m->speed = sqrt(difficulty);
            m->smokeTicks = 0;
            m->isAlive = true;
            missileIndex = wrap(missileIndex + 1, 0, MAX_MISSILES);
        }

        color = PURPLE;
        Collision c = character("b", e->pos.x, e->pos.y);
        if (c.isColliding.rect[RED]) {
            play(EXPLOSION);
            particle(e->pos.x, e->pos.y, 15, 2, 0, M_PI * 2);
            addScore(multiplier, e->pos.x, e->pos.y);
            if (multiplier < 64) {
                multiplier *= 2;
            }
            e->isAlive = false;
            continue;
        }

        if (e->fireTicks > 0) {
            color = BLACK;
            thickness = 3;  // Match JS thickness
            bar(e->pos.x, e->pos.y + 3, 3, e->ma);
            thickness = 1;  // Reset thickness
        }

        if (e->pos.x < -3) {
            e->isAlive = false;
        }
    }

    // Update missiles
    float vva = sqrt(difficulty) * 0.0005f;
    FOR_EACH(missiles, i) {
        ASSIGN_ARRAY_ITEM(missiles, i, Missile, m);
        SKIP_IS_NOT_ALIVE(m);

        addWithAngle(&m->pos, m->angle, m->speed);
        m->pos.x -= scr;

        float ta = angleTo(&m->pos, player.pos.x, player.pos.y);
        float oy = wrap(ta - m->angle, -M_PI, M_PI);
        m->va += (oy > 0 ? 1 : -1) * vva;
        m->angle = clamp(wrap(m->angle + m->va, 0, M_PI * 2),
                        (M_PI / 4) * 3, (M_PI / 4) * 5);

        color = BLACK;
        thickness = 3;  // Match JS thickness
        Collision c = bar(VEC_XY(m->pos), 3, m->angle);
        thickness = 1;  // Reset thickness

        m->smokeTicks += sqrt(difficulty);
        if (m->smokeTicks > 9) {
            ASSIGN_ARRAY_ITEM(smokes, smokeIndex, Smoke, s);
            vectorSet(&s->pos, m->pos.x, m->pos.y);
            vectorSet(&s->vel, 0, 0);
            addWithAngle(&s->vel, m->angle, m->speed * 0.3f);
            s->ticks = 0;
            s->isEnemy = true;
            s->isAlive = true;
            smokeIndex = wrap(smokeIndex + 1, 0, MAX_SMOKES);
            m->smokeTicks -= 9;
        }

        color = RED;
        if (c.isColliding.rect[RED]) {
            play(HIT);
            particle(m->pos.x, m->pos.y, 9, 2, 0, M_PI * 2);
            addScore(multiplier, m->pos.x, m->pos.y);
            if (multiplier < 64) {
                multiplier *= 2;
            }
            m->isAlive = false;
            continue;
        } else if (c.isColliding.character['a']) {
            play(EXPLOSION);
            //gameOver();
        }

        if (m->pos.y > 90) {
            particle(m->pos.x, m->pos.y, 9, 1, 0, M_PI * 2);
            m->isAlive = false;
            continue;
        }

        if (m->pos.x < -3) {
            m->isAlive = false;
        }
    }
}

void addGameAccelB() {
    addGame(title, description, characters, charactersCount, options, update);
}