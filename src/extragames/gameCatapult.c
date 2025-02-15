#include "cglp.h"

static char *title = "CATAPULT";
static char *description = "[Tap] Throw";

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
    .viewSizeX = 200, 
    .viewSizeY = 100, 
    .soundSeed = 2, 
    .isDarkColor = false
};

#define MAX_BARS 32

// Bar structure with lifecycle management
typedef struct {
    Vector pos;
    float width;
    bool isPassed;
    bool isAlive;
} Bar;

// Game state structures
typedef struct {
    Vector pos;
    Vector vel;
    Bar* bar;
    float barPos;
    float grv;
} Ball;

// Global game variables
static Bar bars[MAX_BARS];
static Ball ball;
static float nextBarDist;
static float baseScore;
static int passedCount;
static int nextBarIndex;

// Initialize a new bar
static void initBar(Bar* bar, float x, float y, float width) {
    vectorSet(&bar->pos, x, y);
    bar->width = width;
    bar->isPassed = false;
    bar->isAlive = true;
}

// Game update function
static void update() {
    // Initial game setup
    if (!ticks) {
        // Reset all bars to inactive
        INIT_UNALIVED_ARRAY(bars);
        nextBarIndex = 0;
        
        // Create initial bar
        initBar(&bars[0], 200, 60, 20);
        bars[0].isPassed = true;
        
        // Initialize ball
        vectorSet(&ball.pos, 200, 60);
        vectorSet(&ball.vel, 0, 0);
        ball.bar = &bars[0];
        ball.barPos = -10;
        ball.grv = 0.1;
        
        // Reset game variables
        baseScore = 1;
        passedCount = 0;
        nextBarDist = 50;
        nextBarIndex = wrap(nextBarIndex + 1, 0, MAX_BARS);
    }
    
    // Screen scrolling logic
    bool ip = input.isPressed;
    float scr = difficulty * 0.05;
    if (ball.pos.x > 30) {
        scr += (ball.pos.x - 30) * 0.1;
    }
    ball.pos.x -= scr;
    
    // Ball on bar logic
    if (ball.bar != NULL) {
        // Update bar position
        ball.barPos += (ip ? -1 : 1) * difficulty * 0.2;
        
        // Gradually reduce base score
        if (baseScore > 1) {
            baseScore -= 0.5;
        }
        
        // Calculate ball position on bar
        Vector barBase, tempVec;
        vectorSet(&barBase, ball.bar->pos.x, ball.bar->pos.y);
        vectorSet(&tempVec, barBase.x, barBase.y);
        
        // Position ball based on input state
        addWithAngle(&tempVec, ip ? -M_PI / 4 : (-M_PI / 4) * 3, 4);
        addWithAngle(&tempVec, ip ? -(M_PI / 4) * 3 : (M_PI / 4) * 3, ball.barPos);
        
        vectorSet(&ball.pos, tempVec.x, tempVec.y);
        
        // Check ball release conditions
        if (fabsf(ball.barPos) > ball.bar->width * 0.65) {
            ball.bar = NULL;
            vectorSet(&ball.vel, 0, 0);
        } else if (ball.barPos > 0) {
            // Prediction visualization
            color = ball.barPos > ball.bar->width * 0.45 ? LIGHT_RED : LIGHT_BLACK;
            
            // Predict ball trajectory
            Vector predPos, predVel;
            vectorSet(&predPos, ball.bar->pos.x, ball.bar->pos.y);
            addWithAngle(&predPos, -M_PI / 4, 4);
            addWithAngle(&predPos, (-M_PI / 4) * 3, ball.barPos);
            
            vectorSet(&predVel, 0, 0);
            addWithAngle(&predVel, -M_PI / 4, sqrtf(ball.barPos) * sqrtf(difficulty));
            
            // Initial velocity application
            vectorAdd(&predPos, predVel.x, predVel.y);
            
            // Draw prediction trail
            for (int i = 0; i < 99; i++) {
                vectorAdd(&predPos, predVel.x, predVel.y);
                predVel.y += ball.grv;
                
                if (i % 5 == 0) {
                    box(predPos.x, predPos.y, 3, 3);
                }
                
                if (predPos.y > 99) {
                    break;
                }
            }
            
            color = BLACK;
            
            // Ball launch
            if (input.isJustPressed) {
                play(JUMP);
                vectorSet(&ball.vel, 0, 0);
                addWithAngle(&ball.vel, -M_PI / 4, sqrtf(ball.barPos) * sqrtf(difficulty));
                vectorAdd(&ball.pos, ball.vel.x, ball.vel.y);
                ball.bar = NULL;
                passedCount = 0;
            }
        }
    } else {
        // Ball in flight
        vectorAdd(&ball.pos, ball.vel.x, ball.vel.y);
        ball.vel.y += ball.grv;
        baseScore += ball.vel.x * 0.1;
    }
    
    // Laser sound on tap
    if (input.isJustPressed) {
        play(LASER);
    }
    
    // Draw ball
    box(ball.pos.x, ball.pos.y, 5, 5);
    
    // Game over conditions
    if (ball.pos.y > 99 || ball.pos.x < 0) {
        play(EXPLOSION);
        gameOver();
    }
    
    // Bar management
    FOR_EACH(bars, i) {
        ASSIGN_ARRAY_ITEM(bars, i, Bar, b);
        SKIP_IS_NOT_ALIVE(b);
        
        // Scroll bar
        b->pos.x -= scr;
        
        // Bar interaction
        barCenterPosRatio = 0.5;
        thickness = 3;
        Collision barCollision = bar(b->pos.x, b->pos.y, b->width, ip ? M_PI / 4 : -M_PI / 4);
        
        // Check for bar collision
        if (barCollision.isColliding.rect[BLACK] && ball.bar == NULL) {
            play(POWER_UP);
            
            // Score calculation
            addScore(ceilf(baseScore), ball.pos.x, ball.pos.y);
            
            // Position calculation
            Vector p;
            vectorSet(&p, b->pos.x, b->pos.y);
            addWithAngle(&p, ip ? -M_PI / 4 : (-M_PI / 4) * 3, 4);
            
            // Set new bar
            ball.bar = b;
            ball.barPos = clamp(
                distanceTo(&p, ball.pos.x, ball.pos.y) * (ball.pos.x > p.x ? -1 : 1),
                -b->width * 0.4,
                b->width * 0.4
            );
            ball.grv = 0.1 * difficulty;
            b->isPassed = true;
        }
        
        // Passing bars
        if (!b->isPassed && b->pos.x + b->width * 0.5 < ball.pos.x) {
            play(COIN);
            b->isPassed = true;
            passedCount++;
            baseScore += passedCount * 10;
            addScore(passedCount * 10, b->pos.x, b->pos.y);
        }
        
        // Remove bars off screen
        b->isAlive = b->pos.x >= -b->width / 2;
    }
    
    // Spawn new bars
    nextBarDist -= scr;
    if (nextBarDist < 0) {
        // Find next available bar slot
        int initialIndex = nextBarIndex;
        while (bars[nextBarIndex].isAlive) {
            nextBarIndex = wrap(nextBarIndex + 1, 0, MAX_BARS);
            
            // If all bars are alive, stop
            if (nextBarIndex == initialIndex) return;
        }
        
        // Create new bar
        Bar* newBar = &bars[nextBarIndex];
        newBar->width = rnd(20, 30);
        vectorSet(&newBar->pos, 200 + newBar->width / 2, rnd(50, 90));
        newBar->isPassed = false;
        newBar->isAlive = true;
        
        nextBarDist += newBar->width / 2 + rnd(30, 50);
        
        // Move to next index
        nextBarIndex = wrap(nextBarIndex + 1, 0, MAX_BARS);
    }
    
    // Display base score
    text(intToChar(ceilf(baseScore)), 3, 9);
}

// Add game to the menu
void addGameCatapult() {
    addGame(title, description, characters, charactersCount, options, update);
}