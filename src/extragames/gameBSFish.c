#include "cglp.h"

static char *title = "BS FISH";
static char *description = "[Hold] Speed up";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Fish 'a'
        "      ",
        "      ",
        "  rrr ",
        "rrbrrr",
        " rrr r",
        "      ",
    },
    {   // Bird frame 1 'b'
        "      ",
        " ll   ",
        "  ll  ",
        "llllly",
        "      ",
        "      ",
    },
    {   // Bird frame 2 'c'
        "      ",
        "      ",
        "llllly",
        "  ll  ",
        " ll   ",
        "      ",
    }
};
static int charactersCount = 3;

static Options options = {
    .viewSizeX = 200,
    .viewSizeY = 100,
    .soundSeed = 4,
    .isDarkColor = false
};

typedef struct {
    Vector pos;
    float vx;
    float ticks;
} Bird;

typedef enum {
    NORMAL,
    EYE,
    BIG,
    FAKE
} FishType;

typedef struct {
    Vector pos;
    Vector vel;
    FishType type;
    bool isAlive;
} Fish;

Collision characterScaled(char* chr, float x, float y, float scaleX, float scaleY) {
    // Get character index from the input character (a, b, c, etc.)
    int charIndex = chr[0] - 'a';
    if (charIndex < 0 || charIndex >= charactersCount) {
        Collision empty = {0};
        return empty;
    }

    // Calculate the offset to center the character
    float offsetX = (CHARACTER_WIDTH * scaleX) / 2.0f;
    float offsetY = (CHARACTER_HEIGHT * scaleY) / 2.0f;
    
    // Initialize aggregate collision result
    Collision aggregateCollision = {0};
    
    // For each pixel in the character grid
    for (int gridY = 0; gridY < CHARACTER_HEIGHT; gridY++) {
        for (int gridX = 0; gridX < CHARACTER_WIDTH; gridX++) {
            char pixel = characters[charIndex][gridY][gridX];
            
            // Skip empty spaces
            if (pixel == ' ') continue;
            
            // Store current color
            int prevColor = color;
            
            // Determine the color to use
            if (color == BLACK) {
                // Use the original character's color based on the pixel value
                switch (pixel) {
                    case 'w': color = WHITE; break;
                    case 'r': color = RED; break;
                    case 'g': color = GREEN; break;
                    case 'y': color = YELLOW; break;
                    case 'b': color = BLUE; break;
                    case 'p': color = PURPLE; break;
                    case 'c': color = CYAN; break;
                    case 'l': color = BLACK; break;
                    case 'R': color = LIGHT_RED; break;
                    case 'G': color = LIGHT_GREEN; break;
                    case 'Y': color = LIGHT_YELLOW; break;
                    case 'B': color = LIGHT_BLUE; break;
                    case 'P': color = LIGHT_PURPLE; break;
                    case 'C': color = LIGHT_CYAN; break;
                    case 'L': color = LIGHT_BLACK; break;
                    default: color = BLACK; break;
                }
            }
            // else: use the current color value
            
            // Calculate scaled pixel position
            float pixelX = x + (gridX * scaleX) - offsetX;
            float pixelY = y + (gridY * scaleY) - offsetY;
            
            // Draw the scaled pixel and get its collision
            Collision pixelCollision = box(pixelX, pixelY, scaleX, scaleY);
            
            // Merge collision information with aggregate
            for (int i = 0; i < COLOR_COUNT; i++) {
                aggregateCollision.isColliding.rect[i] |= pixelCollision.isColliding.rect[i];
            }
            for (int i = 0; i < ASCII_CHARACTER_COUNT; i++) {
                aggregateCollision.isColliding.text[i] |= pixelCollision.isColliding.text[i];
                aggregateCollision.isColliding.character[i] |= pixelCollision.isColliding.character[i];
            }
            
            // Restore previous color
            color = prevColor;
        }
    }
    
    return aggregateCollision;
}

#define MAX_FISHES 100
static Bird bird;
static Fish fishes[MAX_FISHES];
static int fishIndex;
static float nextFishTicks;
static int nextBigFishCount;
static float scrX;
static int multiplier;
static int isGameOver;

static void update() {
    if (!ticks) {
        vectorSet(&bird.pos, 20, 20);
        bird.vx = 1;
        bird.ticks = 0;
        isGameOver = 0;

        INIT_UNALIVED_ARRAY(fishes);
        fishIndex = 0;
        nextFishTicks = 0;
        nextBigFishCount = 3;
        scrX = 0;
        multiplier = 1;
    }

    if (input.isJustPressed) {
        play(SELECT);
    }

    float scr = bird.pos.x > 30 ? (bird.pos.x - 30) * 0.1f * sqrt(difficulty) : 0;
    bird.vx += ((input.isPressed ? 3 * sqrt(difficulty) : 0.1f) - bird.vx) * 0.2f;
    bird.ticks += bird.vx;
    bird.pos.x += bird.vx - scr;

    color = BLACK;
    char birdChar[2] = {'b' + ((int)floor(bird.ticks / 10.0f)) % 2, '\0'};
    character(birdChar, bird.pos.x, bird.pos.y);

    nextFishTicks--;
    if (nextFishTicks < 0) {
        Vector pos = {rnd(130, 220), 120};
        Vector vel = {-rnd(1, 1.5f) * 0.5f * sqrt(difficulty), -2.5f * sqrt(difficulty)};
        FishType type = NORMAL;
        nextBigFishCount--;
        
        if (nextBigFishCount < 0) {
            type = rnd(0, 1) < 0.5f ? BIG : FAKE;
            nextBigFishCount = rnd(3, 9);
        }

        if (type == BIG) {
            if (rnd(0, 1) < 0.7f) {
                vel.y *= 1.125f;
                vel.x *= 0.9f;
            } else {
                vel.y *= 0.97f;
                vel.x *= 1.5f;
            }
        }

        ASSIGN_ARRAY_ITEM(fishes, fishIndex, Fish, f);
        f->pos = pos;
        f->vel = vel;
        f->type = type;
        f->isAlive = true;
        fishIndex = wrap(fishIndex + 1, 0, MAX_FISHES);
        nextFishTicks = rnd(40, 60) / difficulty;
    }

    FOR_EACH(fishes, i) {
        ASSIGN_ARRAY_ITEM(fishes, i, Fish, f);
        SKIP_IS_NOT_ALIVE(f);

        Vector pp = f->pos;  // Previous position
        f->vel.y += 0.03f * difficulty;
        vectorAdd(&f->pos, f->vel.x, f->vel.y);
        f->pos.x -= scr;

        color = BLACK;
        const float sc = (f->type == BIG || f->type == FAKE) ? 6 : 1;
        const float wy = 50 + 2 * sc;

        if (f->pos.y > wy) {
            color = BLUE;
        } else {
            if (f->type == FAKE) {
                const int eyeX = 2, eyeY = 1;
                static const int points[][2] = {
                                  {2,0}, {3,0}, {4,0},
                    {0,1}, {1,1}, {2,1}, {3,1}, {4,1}, {5,1},
                           {1,2}, {2,2}, {3,2},              {5,2}
                };

                for (int p = 0; p < 13; p++) {
                    ASSIGN_ARRAY_ITEM(fishes, fishIndex, Fish, newFish);
                    vectorSet(&newFish->pos, 
                        f->pos.x + points[p][0] * 6 - 15,
                        f->pos.y + points[p][1] * 6 - 6);
                    newFish->vel = f->vel;
                    newFish->type = (points[p][0] == eyeX && points[p][1] == eyeY) ? EYE : NORMAL;
                    newFish->isAlive = true;
                    fishIndex = wrap(fishIndex + 1, 0, MAX_FISHES);
                }
                f->isAlive = false;
                continue;
            }

            color = (f->type == EYE) ? BLUE : BLACK;
        }

        Collision c = characterScaled("a", f->pos.x, f->pos.y, sc, sc);
        if((c.isColliding.character['b'] || c.isColliding.character['c']) && (f->type == BIG))
        {
            isGameOver = true;
        }
        
        if (f->type != BIG && distanceTo(&f->pos, bird.pos.x, bird.pos.y) < 6) {
            addScore(multiplier, f->pos.x, f->pos.y);
            if (f->type == NORMAL) {
                play(COIN);
                multiplier++;
            } else {
                play(POWER_UP);
                multiplier += 10;
            }
            f->isAlive = false;
            continue;
        }

        if (f->pos.x < -18 || f->pos.y > 120) {
            if (f->type != BIG && multiplier > 1) {
                multiplier--;
            }
            f->isAlive = false;
            continue;
        }

        if ((pp.y - wy) * (f->pos.y - wy) < 0) {
            play(HIT);
        }
    }

    if(isGameOver)
    {
        play(EXPLOSION);
        gameOver();
    }

    scrX = wrap(scrX - scr, -10, 210);
    color = BLUE;
    rect(0, 50, 200, 2);
    color = CYAN;

    for (int i = 0; i < 5; i++) {
        rect(wrap(scrX + (220.0f / 5) * i, -10, 210), 50, 9, 2);
    }

    color = BLACK;
    char multText[16];
    sprintf(multText, "x%d", multiplier);
    text(multText, 3, 9);
}

void addGameBsfish() {
    addGame(title, description, characters, charactersCount, options, update);
}