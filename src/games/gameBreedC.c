#include "cglp.h"

static char *title = "BREED C";
static char *description = "[Tap]\n Erase blocks\n (4 or more\n  linked)";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Empty character set as per JS version
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
        "      ",
    }
};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 1,
    .isDarkColor = false
};

#define GRID_SIZE 15
#define COLOR_COUNT 4
static const char* colors[COLOR_COUNT] = {"red", "green", "blue", "yellow"};

static int grid[GRID_SIZE][GRID_SIZE];
static int prevGrid[GRID_SIZE][GRID_SIZE];
static bool erasingGrid[GRID_SIZE][GRID_SIZE];
static float erasingTicks;
static int erasingCount;
static float nextGridTicks;
static int nextGridCount;
static int multiplier;

// Forward declarations of helper functions
static void addGrid(void);
static void addHorizontal(void);
static void addVertical(void);
static int downHorizontal(void);
static int downVertical(void);
static int checkErasingDown(void);
static int checkErasingUp(void);
static int drawGrid(void);

static void update() {
    const int cgp = floor(GRID_SIZE / 2);
    
    if (!ticks) {
        // Initialize grids to -1
        for (int x = 0; x < GRID_SIZE; x++) {
            for (int y = 0; y < GRID_SIZE; y++) {
                grid[x][y] = -1;
                prevGrid[x][y] = -1;
                erasingGrid[x][y] = false;
            }
        }
        nextGridTicks = 0;
        nextGridCount = 0;
        erasingTicks = 0;
        multiplier = 1;
    }

    // Calculate input grid position
    Vector ip;
    ip.x = floor((input.pos.x - 50) / 6 + GRID_SIZE / 2);
    ip.y = floor((input.pos.y - 53) / 6 + GRID_SIZE / 2);

    // Draw input highlight
    if (ip.x >= 0 && ip.x < GRID_SIZE && ip.y >= 0 && ip.y < GRID_SIZE) {
        color = LIGHT_BLACK;
        box((ip.x - GRID_SIZE / 2) * 6 + 53, (ip.y - GRID_SIZE / 2) * 6 + 56, 7, 7);
    }

    erasingTicks--;
    if (erasingTicks >= 0) {
        if (erasingTicks == 0) {
            for (int i = 0; i < 99; i++) {
                int dc;
                if (erasingCount % 2 == 0) {
                    dc = downHorizontal() + downVertical();
                } else {
                    dc = downVertical() + downHorizontal();
                }
                if (dc == 0) {
                    break;
                }
            }
            nextGridTicks = 120 / sqrt(difficulty);
            erasingCount++;
        }
        drawGrid();
        return;
    }

    if (grid[cgp][cgp] < 0) {
        grid[cgp][cgp] = rnd(0, COLOR_COUNT);
    }

    if (input.isJustPressed) {
        if (ip.x >= 0 && ip.x < GRID_SIZE && 
            ip.y >= 0 && ip.y < GRID_SIZE && 
            grid[(int)ip.x][(int)ip.y] >= 0) {
            
            // Reset erasing grid
            for (int x = 0; x < GRID_SIZE; x++) {
                for (int y = 0; y < GRID_SIZE; y++) {
                    erasingGrid[x][y] = false;
                }
            }

            erasingGrid[(int)ip.x][(int)ip.y] = true;
            int tec = 1;

            for (int i = 0; i < 99; i++) {
                int ec = checkErasingDown() + checkErasingUp();
                tec += ec;
                if (ec == 0) {
                    break;
                }
            }

            if (tec < 4) {
                play(HIT);
                addScore(-tec, input.pos.x, input.pos.y);
                addGrid();
                addGrid();
            } else {
                play(POWER_UP);
                addScore(tec * multiplier, input.pos.x, input.pos.y);
                multiplier++;
                
                for (int x = 0; x < GRID_SIZE; x++) {
                    for (int y = 0; y < GRID_SIZE; y++) {
                        if (erasingGrid[x][y]) {
                            grid[x][y] = -1;
                        }
                    }
                }
                erasingTicks = ceil(60 / sqrt(difficulty));
                drawGrid();
                return;
            }
        } else {
            addGrid();
            addGrid();
        }
    }

    nextGridTicks--;
    if (nextGridTicks < 0) {
        addGrid();
    }

    if (drawGrid() >= GRID_SIZE * GRID_SIZE) {
        play(EXPLOSION);
        gameOver();
    }
}

static int drawGrid() {
    color = BLACK;
    char multText[16];
    sprintf(multText, "x%d", multiplier);
    text(multText, 3, 9);

    int gc = 0;
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            const int c = grid[x][y];
            if (c >= 0) {
                // Set color based on grid value
                switch(c) {
                    case 0: color = RED; break;
                    case 1: color = GREEN; break;
                    case 2: color = BLUE; break;
                    case 3: color = YELLOW; break;
                }
                box(53 + (x - (GRID_SIZE / 2.0f)) * 6, 56 + (y - (GRID_SIZE / 2.0f)) * 6, 5, 5);
                gc++;
            }
        }
    }
    return gc;
}

static void addGrid() {
    play(COIN);
    multiplier = 1;
    
    // Copy current grid to previous
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            prevGrid[x][y] = grid[x][y];
        }
    }

    if (nextGridCount % 2 == 0) {
        addHorizontal();
    } else {
        addVertical();
    }
    nextGridCount++;
    nextGridTicks = 120 / sqrt(difficulty);
}

static void addHorizontal() {
    const int cx = floor(GRID_SIZE / 2);
    
    // Move left side
    for (int x = 0; x < cx - 1; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            grid[x][y] = prevGrid[x + 1][y];
        }
    }
    
    // Move right side
    for (int x = GRID_SIZE - 1; x > cx + 1; x--) {
        for (int y = 0; y < GRID_SIZE; y++) {
            grid[x][y] = prevGrid[x - 1][y];
        }
    }
    
    // Handle center column
    for (int y = 0; y < GRID_SIZE; y++) {
        const int c = prevGrid[cx][y];
        if (c >= 0) {
            const int nx = rnd(0, 1) < 0.5f ? -1 : 1;
            const int nc = wrap(c + rnd(0, COLOR_COUNT - 1), 0, COLOR_COUNT);
            grid[cx + nx][y] = nc;
            grid[cx][y] = nc;
            grid[cx - nx][y] = c;
        }
    }
}

static void addVertical() {
    const int cy = floor(GRID_SIZE / 2);
    
    // Move top side
    for (int y = 0; y < cy - 1; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[x][y] = prevGrid[x][y + 1];
        }
    }
    
    // Move bottom side
    for (int y = GRID_SIZE - 1; y > cy + 1; y--) {
        for (int x = 0; x < GRID_SIZE; x++) {
            grid[x][y] = prevGrid[x][y - 1];
        }
    }
    
    // Handle center row
    for (int x = 0; x < GRID_SIZE; x++) {
        const int c = prevGrid[x][cy];
        if (c >= 0) {
            const int ny = rnd(0, 1) < 0.5f ? -1 : 1;
            const int nc = wrap(c + rnd(0, COLOR_COUNT - 1), 0, COLOR_COUNT);
            grid[x][cy + ny] = nc;
            grid[x][cy] = nc;
            grid[x][cy - ny] = c;
        }
    }
}

static int downHorizontal() {
    int dc = 0;
    const int cx = floor(GRID_SIZE / 2);

    // Move from left to center
    for (int x = cx; x >= 1; x--) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[x][y] == -1 && grid[x - 1][y] >= 0) {
                grid[x][y] = grid[x - 1][y];
                grid[x - 1][y] = -1;
                dc++;
            }
        }
    }

    // Move from right to center
    for (int x = cx; x <= GRID_SIZE - 2; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (grid[x][y] == -1 && grid[x + 1][y] >= 0) {
                grid[x][y] = grid[x + 1][y];
                grid[x + 1][y] = -1;
                dc++;
            }
        }
    }
    
    return dc;
}

static int downVertical() {
    int dc = 0;
    const int cy = floor(GRID_SIZE / 2);

    // Move from top to center
    for (int y = cy; y > -1; y--) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[x][y] == -1 && grid[x][y - 1] >= 0) {
                grid[x][y] = grid[x][y - 1];
                grid[x][y - 1] = -1;
                dc++;
            }
        }
    }

    // Move from bottom to center
    for (int y = cy; y <= GRID_SIZE - 2; y++) {
        for (int x = 0; x < GRID_SIZE; x++) {
            if (grid[x][y] == -1 && grid[x][y + 1] >= 0) {
                grid[x][y] = grid[x][y + 1];
                grid[x][y + 1] = -1;
                dc++;
            }
        }
    }
    
    return dc;
}

static int checkErasingDown() {
    int ec = 0;
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int y = 0; y < GRID_SIZE; y++) {
            if (!erasingGrid[x][y]) {
                continue;
            }
            const int c = grid[x][y];
            if (x < GRID_SIZE - 1 && !erasingGrid[x + 1][y] && grid[x + 1][y] == c) {
                erasingGrid[x + 1][y] = true;
                ec++;
            }
            if (y < GRID_SIZE - 1 && !erasingGrid[x][y + 1] && grid[x][y + 1] == c) {
                erasingGrid[x][y + 1] = true;
                ec++;
            }
        }
    }
    return ec;
}

static int checkErasingUp() {
    int ec = 0;
    for (int x = GRID_SIZE - 1; x >= 0; x--) {
        for (int y = GRID_SIZE - 1; y >= 0; y--) {
            if (!erasingGrid[x][y]) {
                continue;
            }
            const int c = grid[x][y];
            if (x > 0 && !erasingGrid[x - 1][y] && grid[x - 1][y] == c) {
                erasingGrid[x - 1][y] = true;
                ec++;
            }
            if (y > 0 && !erasingGrid[x][y - 1] && grid[x][y - 1] == c) {
                erasingGrid[x][y - 1] = true;
                ec++;
            }
        }
    }
    return ec;
}

void addGameBreedc() {
    addGame(title, description, characters, charactersCount, options, update);
}