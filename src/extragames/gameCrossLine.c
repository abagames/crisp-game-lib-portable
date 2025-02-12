#include "cglp.h"

static char* title = "CROSS LINE";
static char* description = "[Tap]\n Connect line";

#define CS static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1]
CS = {{}};
static int charactersCount = 0;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 1,
    .isDarkColor = false
};

typedef struct {
    Vector pos;
    float vy;
    bool isAlive;
} Point;

typedef struct {
    Vector from;
    Vector to;
    float fromVy;
    float toVy;
    int addedCount;
    bool isAlive;
} Line;

#define CROSSLINE_MAX_POINT_COUNT 128
#define CROSSLINE_MAX_LINE_COUNT 128

static Point points[CROSSLINE_MAX_POINT_COUNT];
static Line lines[CROSSLINE_MAX_LINE_COUNT];
static float nextPointDist;
static Point* selectedPoint;
static float pointMaxY;
static int pointCount;
static int pointIndex;
static int lineIndex;

static void addLineParticle(Vector from, Vector to) {
    float d = distanceTo(&from, VEC_XY(to));
    Vector p;
    Vector o;
    vectorSet(&p, VEC_XY(from));
    vectorSet(&o, 7, 0);
    rotate(&o, angleTo(&from, VEC_XY(to)));
    
    for (int i = 0; i < floorf(d / 7); i++) {
        particle(VEC_XY(p), 5, 0.5, 0, M_PI * 2);
        vectorAdd(&p, VEC_XY(o));
    }
}

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(points);
        INIT_UNALIVED_ARRAY(lines);
        pointIndex = 0;
        lineIndex = 0;
        
        // Initialize first point
        ASSIGN_ARRAY_ITEM(points, pointIndex, Point, p);
        p->pos.x = 70;
        p->pos.y = 1;
        p->vy = 1;
        p->isAlive = true;
        pointIndex = wrap(pointIndex + 1, 0, CROSSLINE_MAX_POINT_COUNT);
        
        // Initialize first line
        ASSIGN_ARRAY_ITEM(lines, lineIndex, Line, l);
        l->from.x = 30;
        l->from.y = 99;
        l->to.x = 70;
        l->to.y = 1;
        l->fromVy = -1;
        l->toVy = 1;
        l->addedCount = 2;
        l->isAlive = true;
        lineIndex = wrap(lineIndex + 1, 0, CROSSLINE_MAX_LINE_COUNT);
        
        nextPointDist = 0;
        selectedPoint = &points[0];
        pointMaxY = 0;
        pointCount = 0;
    }

    float scr = sqrt(difficulty) * 0.03;
    if (pointMaxY < 40) {
        scr += (40 - pointMaxY) * 0.1;
    }

    nextPointDist -= scr;
    while (nextPointDist < 0) {
        float vy = (pointCount % 2 == 0) ? -1 : 1;
        int pc = (int)(floorf(pointCount / 2)) % 7;
        float x;
        if (pc == 3) {
            x = rnd(10, 30);
        } else if (pc == 6) {
            x = rnd(70, 90);
        } else {
            x = rnd(10, 90);
        }
        
        ASSIGN_ARRAY_ITEM(points, pointIndex, Point, p);
        p->pos.x = x;
        p->pos.y = vy > 0 ? -nextPointDist - 19 : 119 + nextPointDist;
        p->vy = vy;
        p->isAlive = true;
        pointIndex = wrap(pointIndex + 1, 0, CROSSLINE_MAX_POINT_COUNT);
        
        nextPointDist += 4;
        pointCount++;
    }

    color = LIGHT_RED;
    if (input.isJustPressed) {
        box(VEC_XY(input.pos), 5, 5);
    }

    pointMaxY = 0;
    bool isSelected = false;

    FOR_EACH(points, i) {
        ASSIGN_ARRAY_ITEM(points, i, Point, p);
        SKIP_IS_NOT_ALIVE(p);

        if (p->pos.x > 99) {
            p->isAlive = false;
            continue;
        }

        p->pos.y += scr * p->vy;
        color = TRANSPARENT;
        Collision c = box(VEC_XY(p->pos), 4, 4);

        if (p != selectedPoint && !isSelected && c.isColliding.rect[LIGHT_RED]) {
            play(SELECT);
            isSelected = true;
            if (selectedPoint == NULL) {
                selectedPoint = p;
            } else {
                color = RED;
                float a = angleTo(&p->pos, VEC_XY(selectedPoint->pos));
                line(VEC_XY(p->pos), VEC_XY(selectedPoint->pos));

                FOR_EACH(lines, j) {
                    ASSIGN_ARRAY_ITEM(lines, j, Line, l);
                    SKIP_IS_NOT_ALIVE(l);
                    l->addedCount--;
                }

                ASSIGN_ARRAY_ITEM(lines, lineIndex, Line, nl);
                nl->from = selectedPoint->pos;
                nl->to = p->pos;
                nl->fromVy = selectedPoint->vy;
                nl->toVy = p->vy;
                nl->addedCount = 2;
                nl->isAlive = true;
                lineIndex = wrap(lineIndex + 1, 0, CROSSLINE_MAX_LINE_COUNT);

                selectedPoint->pos.x = 999;
                selectedPoint = p;
            }
        }

        float my = p->vy > 0 ? p->pos.y : 100 - p->pos.y;
        if (my > pointMaxY) {
            pointMaxY = my;
        }
    }

    int multiplier = 1;
    FOR_EACH(lines, i) {
        ASSIGN_ARRAY_ITEM(lines, i, Line, l);
        SKIP_IS_NOT_ALIVE(l);

        l->from.y += scr * l->fromVy;
        l->to.y += scr * l->toVy;

        color = BLACK;
        if (line(VEC_XY(l->from), VEC_XY(l->to)).isColliding.rect[RED]) {
            if (l->addedCount <= 0) {
                addLineParticle(l->from, l->to);
                Vector scorePos;
                vectorSet(&scorePos, (l->from.x + l->to.x) / 2, (l->from.y + l->to.y) / 2);
                addScore(multiplier, VEC_XY(scorePos));
                
                if (multiplier == 1) {
                    play(POWER_UP);
                    color = RED;
                    Line* ll = &lines[(lineIndex - 1 + CROSSLINE_MAX_LINE_COUNT) % CROSSLINE_MAX_LINE_COUNT];
                    if (ll->isAlive) {
                        addLineParticle(ll->from, ll->to);
                    }
                }
                multiplier *= 2;
                l->isAlive = false;
                continue;
            }
        }

        color = l->fromVy > 0 ? LIGHT_BLUE : LIGHT_CYAN;
        box(VEC_XY(l->from), 3, 3);
        color = l->toVy > 0 ? LIGHT_BLUE : LIGHT_CYAN;
        box(VEC_XY(l->to), 3, 3);
        color = RED;

        if (l->from.y < 0 || l->from.y > 99) {
            play(EXPLOSION);
            text("X", VEC_XY(l->from));
            gameOver();
        }
        if (l->to.y < 0 || l->to.y > 99) {
            play(EXPLOSION);
            text("X", VEC_XY(l->to));
            gameOver();
        }
    }

    FOR_EACH(points, i) {
        ASSIGN_ARRAY_ITEM(points, i, Point, p);
        SKIP_IS_NOT_ALIVE(p);
        color = selectedPoint == p ? PURPLE : (p->vy > 0 ? BLUE : CYAN);
        box(VEC_XY(p->pos), 4, 4);
    }
}

void addGameCrossLine() {
    addGame(title, description, characters, charactersCount, options, update);
}