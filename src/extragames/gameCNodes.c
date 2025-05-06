#include "cglp.h"

static char *title = "C NODES";
static char *description = "[Tap] Cut";

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
    .viewSizeX = 100, .viewSizeY = 100, .soundSeed = 6, .isDarkColor = false};

typedef struct {
    Vector pos;
    Vector vel;
    int color; // Uses predefined color constants
    bool isAlive;
} Node;

typedef struct {
    Node* node1;
    Node* node2;
    float length;
    bool isAlive;
} Line;

#define MAX_NODES 64
#define MAX_LINES 64

static Node nodes[MAX_NODES];
static Line lines[MAX_LINES];
static float nextNodeDist;
static float maxY;
static float scr;
static float pressedScr;
static int multiplier = 1;
static const float fixedY = 20;

static Node* findFreeNode() {
    FOR_EACH(nodes, i) {
        ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
        if (!n->isAlive) {
            return n;
        }
    }
    return NULL;
}

static Line* findFreeLine() {
    FOR_EACH(lines, i) {
        ASSIGN_ARRAY_ITEM(lines, i, Line, l);
        if (!l->isAlive) {
            return l;
        }
    }
    return NULL;
}

static void checkRedNodes() {
    FOR_EACH(nodes, i) {
        ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
        if (n->color == RED) {
            n->color = BLUE;
        }
    }

    for (int iteration = 0; iteration < 9; iteration++) {
        bool isAddingRed = false;
        FOR_EACH(lines, j) {
            ASSIGN_ARRAY_ITEM(lines, j, Line, l);
            SKIP_IS_NOT_ALIVE(l);
            
            if (l->node1->color == BLACK || l->node1->color == RED ||
                l->node2->color == BLACK || l->node2->color == RED) {
                
                if (l->node1->color == BLUE) {
                    l->node1->color = RED;
                    isAddingRed = true;
                }
                if (l->node2->color == BLUE) {
                    l->node2->color = RED;
                    isAddingRed = true;
                }
            }
        }
        
        if (!isAddingRed) {
            break;
        }
    }
}

static void update() {
    if (!ticks) {
        INIT_UNALIVED_ARRAY(nodes);
        INIT_UNALIVED_ARRAY(lines);
        nextNodeDist = 0;
        scr = 0;
        pressedScr = 0;
        multiplier = 1;
    }

    color = LIGHT_RED;
    rect(0, 0, 100, fixedY);
    
    color = LIGHT_BLACK;
    if (input.isJustPressed && input.pos.y > fixedY) {
        play(HIT);
        box(VEC_XY(input.pos), 7, 7);
        multiplier = 1;
        pressedScr = 1;
    }

    scr = difficulty * 0.03 + pressedScr;
    pressedScr *= 0.7;
    if (maxY < fixedY + 9) {
        scr += (fixedY + 9 - maxY) * 0.1;
    }

    nextNodeDist -= scr;
    if (nextNodeDist < 0) {
        Node* nn = findFreeNode();
        if (nn) {
            vectorSet(&nn->pos, rnd(10, 90), 0);
            vectorSet(&nn->vel, 0, 0);
            nn->color = BLACK;
            nn->isAlive = true;

            // Try to create lines connecting to existing nodes
            FOR_EACH(nodes, i) {
                ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
                SKIP_IS_NOT_ALIVE(n);
                
                if (n->color == BLACK && 
                    (fabsf(nn->pos.x - n->pos.x) / 2 + fabsf(nn->pos.y - n->pos.y) < 30) && 
                    rnd(0, 1) < 0.7) {
                    Line* newLine = findFreeLine();
                    if (newLine) {
                        newLine->node1 = nn;
                        newLine->node2 = n;
                        newLine->length = distanceTo(&nn->pos, VEC_XY(n->pos));
                        newLine->isAlive = true;
                    }
                }
            }
            
            play(LASER);
            nextNodeDist = 4;
        }
    }

    color = BLACK;
    FOR_EACH(lines, i) {
        ASSIGN_ARRAY_ITEM(lines, i, Line, l);
        SKIP_IS_NOT_ALIVE(l);
        
        float d = distanceTo(&l->node1->pos, VEC_XY(l->node2->pos)) - l->length;
        float a = angleTo(&l->node1->pos, VEC_XY(l->node2->pos));
        
        addWithAngle(&l->node1->vel, a, d * 0.02);
        addWithAngle(&l->node2->vel, a, -d * 0.02);

        thickness = 2;
        if (line(VEC_XY(l->node1->pos), VEC_XY(l->node2->pos)).isColliding.rect[LIGHT_BLACK]) {
            play(SELECT);
            l->isAlive = false;
        }

        l->isAlive &= !(l->node1->pos.y > 100 && l->node2->pos.y > 100);
    }

    maxY = 0;
    FOR_EACH(nodes, i) {
        ASSIGN_ARRAY_ITEM(nodes, i, Node, n);
        SKIP_IS_NOT_ALIVE(n);
        
        n->pos.y += scr;
        if (maxY < n->pos.y) {
            maxY = n->pos.y;
        }

        if (n->pos.y > fixedY) {
            if (n->color == BLACK) {
                n->color = BLUE;
                checkRedNodes();
            }
            vectorAdd(&n->pos, VEC_XY(n->vel));
            n->vel.y += 0.03;
            vectorMul(&n->vel, 0.95);
        } else {
            vectorSet(&n->vel, 0, 0);
        }

        color = (n->color == BLACK) ? BLACK : 
                (n->color == RED) ? RED : BLUE;
        box(VEC_XY(n->pos), 5, 5);

        if (n->pos.y > 100 && n->color == RED) {
            play(EXPLOSION);
            color = RED;
            text("X", n->pos.x, 95);
            gameOver();
        }

        if (n->pos.y > 140) {
            play(POWER_UP);
            addScore(multiplier, n->pos.x - 1, 99);
            if (multiplier < 64) {
                multiplier *= 2;
            }
            n->isAlive = false;
        }
    }

    if (input.isPressed) {
        checkRedNodes();
    }
}

void addGameCNodes() {
    addGame(title, description, characters, charactersCount, options, update);
}