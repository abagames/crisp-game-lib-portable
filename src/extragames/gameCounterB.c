#include "cglp.h"

static char* title = "COUNTER B";
static char* description = "[Hold] Beam";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{}};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 10,
    .isDarkColor = true
};

typedef struct {
    Vector pos;
    float angle;
    float speed;
    float beamLength;
    bool isAlive;
} Enemy;

typedef struct {
    Vector pos;
    float speed;
    float beamLength;
    float baseX;
    int invincibleTicks;
} Player;

typedef struct {
    Vector pos;
    float radius;
    Enemy* enemy;
    bool hasPos;
} Counter;

#define COUNTER_B_MAX_ENEMY_COUNT 128
static Enemy enemies[COUNTER_B_MAX_ENEMY_COUNT];
static int enemyIndex;
static float nextEnemyTicks;
static Player player;
static Counter counter;
static float scrX;
static int multiplier;

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(enemies);
        enemyIndex = 0;
        player.pos.x = 20;
        player.pos.y = 60;
        player.speed = 0;
        player.beamLength = 0;
        player.baseX = 30;
        player.invincibleTicks = 0;
        
        // Add initial enemy
        ASSIGN_ARRAY_ITEM(enemies, enemyIndex, Enemy, e);
        e->pos.x = 205;
        e->pos.y = player.pos.y;
        e->angle = M_PI;
        e->speed = 0.5;
        e->beamLength = 0;
        e->isAlive = true;
        enemyIndex = wrap(enemyIndex + 1, 0, COUNTER_B_MAX_ENEMY_COUNT);
        
        nextEnemyTicks = 9;
        counter.hasPos = false;
        counter.radius = 0;
        counter.enemy = NULL;
        multiplier = 1;
        scrX = 0;
    }

    float scr = (player.pos.x - player.baseX) * 0.1;
    player.baseX += (30 - player.baseX) * 0.001;
    
    if (player.pos.x < 0) {
        play(EXPLOSION);
        player.pos.x = 0;
        gameOver();
    }

    color = YELLOW;
    rect(0, 90, 200, 10);
    scrX = wrap(scrX - scr, 0, 200);
    color = WHITE;
    rect(scrX, 90, 2, 10);

    float beamSpeed = 4 * sqrt(difficulty);
    player.speed += ((input.isPressed ? 0.2 : 2) * sqrt(sqrt(difficulty)) - player.speed) * 0.2;
    player.pos.x += player.speed - scr;

    if (!counter.hasPos) {
        if (input.isPressed) {
            if (player.beamLength <= 0) {
                play(SELECT);
                color = CYAN;
                particle(VEC_XY(player.pos), 20, 3, 0, 0.3);
            }
            player.beamLength = clamp(player.beamLength + beamSpeed, 0, 300);
        } else {
            player.beamLength = 0;
        }
    } else {
        player.beamLength = counter.pos.x - player.pos.x;
        if (player.beamLength < 0 && counter.enemy != NULL) {
            counter.enemy->beamLength = -9999;
            counter.enemy = NULL;
        }
    }

    if (player.beamLength > 0) {
        color = LIGHT_CYAN;
        rect(player.pos.x, player.pos.y - 1, player.beamLength, 3);
        color = CYAN;
        box(player.pos.x + player.beamLength, player.pos.y, 5, 5);
    }

    if (counter.hasPos) {
        counter.pos.x -= scr;
        color = PURPLE;
        arc(VEC_XY(counter.pos), counter.radius, ticks * 0.1, ticks * 0.1 + M_PI * 2);
        particle(VEC_XY(counter.pos), 1, counter.radius * 0.1, 0, M_PI * 2);
        
        if (counter.enemy == NULL) {
            counter.radius -= sqrt(difficulty) * 2;
            if (counter.radius < 1) {
                counter.hasPos = false;
            }
        } else {
            counter.radius = clamp(
                counter.radius + (input.isPressed ? 1 : -2) * sqrt(difficulty),
                0,
                30
            );
            if (counter.radius < 1) {
                if (counter.enemy != NULL) {
                    counter.enemy->beamLength = -9999;
                }
                counter.hasPos = false;
            }
        }
    }

    nextEnemyTicks--;
    if (nextEnemyTicks < 0) {
        ASSIGN_ARRAY_ITEM(enemies, enemyIndex, Enemy, e);
        if (rnd(0, 1) < 0.6) {
            e->pos.x = rnd(150, 300);
            e->pos.y = -5;
        } else {
            e->pos.x = 205;
            e->pos.y = rnd(0, 85);
        }

        Vector targetPos;
        if (rnd(0, 1) < 0.05) {
            targetPos.x = player.pos.x + 9;
            targetPos.y = player.pos.y;
        } else {
            targetPos.x = e->pos.x - rnd(30, 150);
            targetPos.y = player.pos.y;
        }

        e->angle = angleTo(&e->pos, VEC_XY(targetPos));
        e->speed = rnd(1, sqrt(difficulty));
        e->beamLength = -rnd(0, 1) * sqrt(difficulty) * 20;
        e->isAlive = true;
        enemyIndex = wrap(enemyIndex + 1, 0, COUNTER_B_MAX_ENEMY_COUNT);
        nextEnemyTicks = rnd(30, 40) / difficulty;
    }

    FOR_EACH(enemies, i) {
        ASSIGN_ARRAY_ITEM(enemies, i, Enemy, e);
        SKIP_IS_NOT_ALIVE(e);

        addWithAngle(&e->pos, e->angle, e->speed);
        e->pos.x -= scr;

        if (counter.enemy == e && counter.hasPos) {
            e->beamLength = distanceTo(&e->pos, VEC_XY(counter.pos));
            e->speed *= 0.8;
        } else {
            e->beamLength += beamSpeed * 0.2;
            if (e->beamLength - beamSpeed * 0.3 < 0 && e->beamLength >= 0) {
                play(LASER);
                color = RED;
                particle(VEC_XY(e->pos), 9, 3, e->angle, 0.5);
            }
        }

        if (e->beamLength > 0) {
            color = LIGHT_RED;
            Vector beamEnd;
            vectorSet(&beamEnd, VEC_XY(e->pos));
            addWithAngle(&beamEnd, e->angle, e->beamLength);
            line(VEC_XY(e->pos), VEC_XY(beamEnd));
            color = RED;
            Collision coll = box(VEC_XY(beamEnd), 5, 5);

            if (!counter.hasPos && coll.isColliding.rect[CYAN]) {
                play(POWER_UP);
                multiplier = 1;
                counter.hasPos = true;
                counter.pos.x = player.pos.x + player.beamLength;
                counter.pos.y = player.pos.y;
                counter.enemy = e;
                counter.radius = 1;
                color = PURPLE;
                particle(VEC_XY(counter.pos), 30, 5, 0, M_PI * 2);
            }

            if (counter.enemy != e && 
                (coll.isColliding.rect[PURPLE] || beamEnd.y > 90 || beamEnd.x < -99)) {
                e->beamLength = -9999;
            }
        }

        color = BLACK;
        Vector barPos;
        vectorSet(&barPos, VEC_XY(e->pos));
        addWithAngle(&barPos, e->angle + (M_PI / 5) * 4, 3);
        thickness = 1;
        bar(VEC_XY(barPos), 2, e->angle);
        thickness = 2;
        Collision coll = bar(VEC_XY(e->pos), 3, e->angle);

        if (coll.isColliding.rect[PURPLE] || coll.isColliding.rect[LIGHT_CYAN]) {
            if (coll.isColliding.rect[PURPLE]) {
                play(COIN);
                addScore(multiplier, VEC_XY(e->pos));
                if (multiplier < 64) {
                    multiplier *= 2;
                }
            } else {
                play(HIT);
            }
            particle(VEC_XY(e->pos), 9, 1, 0, M_PI * 2);
            if (counter.enemy == e) {
                counter.enemy = NULL;
            }
            e->isAlive = false;
            continue;
        }

        if (e->beamLength < -999) {
            if (e->speed < 1) {
                e->speed += (1 - e->speed) * 0.1;
            }
            if (e->angle > 0) {
                e->angle += (M_PI - e->angle) * 0.05;
            }
        }

        if (e->pos.x < -5) {
            e->isAlive = false;
        }
    }

    Vector playerBarPos;
    vectorSet(&playerBarPos, VEC_XY(player.pos));
    addWithAngle(&playerBarPos, -(M_PI / 5) * 4, 3);
    
    color = player.invincibleTicks > 0 && player.invincibleTicks % 10 < 5 ? TRANSPARENT : BLUE;
    box(VEC_XY(playerBarPos), 3, 2);
    
    player.invincibleTicks--;
    
    if (box(VEC_XY(player.pos), 5, 3).isColliding.rect[LIGHT_RED] && 
        player.invincibleTicks < 0 &&
        !(counter.hasPos && 
          distanceTo(&counter.pos, VEC_XY(player.pos)) < counter.radius + 9)) {
        play(EXPLOSION);
        particle(VEC_XY(player.pos), 50, 4, 0, M_PI * 2);
        player.baseX -= 10;
        player.pos.x -= 10;
        player.invincibleTicks = 60;
    }
}

void addGameCounterB() {
    addGame(title, description, characters, charactersCount, options, update);
}