#include "cglp.h"
#include "string.h"

static char *title = "CARD Q";
static char *description = "[Tap]\n Pull out a card";

static char characters[][CHARACTER_HEIGHT][CHARACTER_WIDTH + 1] = {
    {   // Number 10 character 'a'
        "l  l  ",
        "l l l ",
        "l l l ",
        "l l l ",
        "l l l ",
        "l  l  ",
    }
};
static int charactersCount = 1;

static Options options = {
    .viewSizeX = 100,
    .viewSizeY = 100,
    .soundSeed = 3,
    .isDarkColor = false
};

// Card number characters
static const char* numChars[] = {
    "", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"
};

typedef struct {
    int num;
    Vector pos;
    Vector tPos;
    Vector gPos;
    bool isAlive;
} Card;

typedef struct {
    int activeCardCount;
} GameState;

#define CARD_INTERVAL_X 15
#define CARD_ROW_COUNT 5
#define CARD_COLUMN_COUNT 5
#define MAX_CARDS (CARD_ROW_COUNT * CARD_COLUMN_COUNT)
#define MAX_PLACED_CARDS 20

static Card playerCards[MAX_CARDS];
static Card enemyCards[MAX_CARDS];
static Card placedCards[MAX_PLACED_CARDS];
static GameState playerState;
static GameState enemyState;
static int placedCardIndex;
static int placedCardNumbers[2];
static float centerY;
static float targetCenterY;
static int playerPrevMoveIndex;
static int enemyPrevMoveIndex;
static int enemyNextMoveIndex;
static bool hasEnemyNextMove;
static float enemyNextMoveTicks;
static float shuffleTicks;
static int shuffleCount;
static int penaltyIndex;
static float penaltyTicks;
static int multiplier;

// Forward declarations
static void movePos(Vector* p, Vector* tp, float ratio);
static float calcCardX(int i);
static float calcPlacedCardX(int i);
static void calcPlayerCardPos(Vector* pos, Vector p);
static void calcEnemyCardPos(Vector* pos, Vector p);
static void drawCard(float x, float y, int n, int gy, const char* edgeColor);
static void checkPlacedIndex(int idx, int ppi, Card* cards, int* outPi, int* outCn, int* outCi);
static int placeCard(int idx, int ppi, Card* cards);
static void dropCardsInColumn(Card* cards, int idx);
static void addPlacedCard(Card card, int pi);

static void addPlacedCard(Card card, int pi) {
    // Match JS push behavior
    if (placedCardIndex >= 19) {
        // Match JS shift() by moving everything left
        for (int i = 0; i < placedCardIndex - 1; i++) {
            placedCards[i] = placedCards[i + 1];
        }
        placedCardIndex--;
    }
    placedCards[placedCardIndex] = card;
    placedCards[placedCardIndex].tPos.x = calcPlacedCardX(pi);
    placedCards[placedCardIndex].tPos.y = 0;
    placedCards[placedCardIndex].isAlive = true;
    placedCardIndex++;
}

static void dropCardsInColumn(Card* cards, int idx) {
    // First count cards that need to move
    int moveCount = 0;
    Card movedCards[CARD_ROW_COUNT];
    for (int i = 0; i < MAX_CARDS; i++) {
        if (cards[i].isAlive && cards[i].gPos.x == idx) {
            movedCards[moveCount++] = cards[i];
            cards[i].isAlive = false;
        }
    }
    
    // Then place them back in order, matching JS forEach behavior
    for (int i = 0; i < moveCount; i++) {
        Card c = movedCards[i];
        c.gPos.y--;
        // Find free slot
        for (int j = 0; j < MAX_CARDS; j++) {
            if (!cards[j].isAlive) {
                cards[j] = c;
                if (cards == playerCards) {
                    calcPlayerCardPos(&cards[j].tPos, cards[j].gPos);
                } else {
                    calcEnemyCardPos(&cards[j].tPos, cards[j].gPos);
                }
                cards[j].isAlive = true;
                break;
            }
        }
    }
}

static void update() {
    if (!ticks) {
        // Initialize placed cards
        placedCardNumbers[0] = 2;
        placedCardNumbers[1] = 12;
        placedCardIndex = 0;
        playerState.activeCardCount = 0;
        enemyState.activeCardCount = 0;
        
        for (int i = 0; i < 2; i++) {
            Card card = {0};
            card.num = placedCardNumbers[i];
            vectorSet(&card.pos, calcPlacedCardX(i), 0);
            card.tPos = card.pos;
            card.isAlive = true;
            addPlacedCard(card, i);
        }

        // Initialize player cards
        for (int i = 0; i < MAX_CARDS; i++) {
            Vector gPos = { i % CARD_COLUMN_COUNT, floor(i / CARD_COLUMN_COUNT) };
            playerCards[i].gPos = gPos;
            if (gPos.y == 0) {
                // First row has fixed values
                static const int firstRow[] = {1, 3, 3, 11, 13};
                playerCards[i].num = firstRow[(int)gPos.x];
            } else {
                playerCards[i].num = rnd(1, 14);
            }
            calcPlayerCardPos(&playerCards[i].pos, gPos);
            playerCards[i].tPos = playerCards[i].pos;
            playerCards[i].isAlive = true;
            playerState.activeCardCount++;
        }

        // Initialize enemy cards
        for (int i = 0; i < MAX_CARDS; i++) {
            Vector gPos = { i % CARD_COLUMN_COUNT, floor(i / CARD_COLUMN_COUNT) };
            enemyCards[i].gPos = gPos;
            enemyCards[i].num = rnd(1, 14);
            calcEnemyCardPos(&enemyCards[i].pos, gPos);
            enemyCards[i].tPos = enemyCards[i].pos;
            enemyCards[i].isAlive = true;
            enemyState.activeCardCount++;
        }

        centerY = targetCenterY = 40;
        playerPrevMoveIndex = enemyPrevMoveIndex = 0;
        hasEnemyNextMove = false;
        enemyNextMoveTicks = 120;
        shuffleTicks = shuffleCount = 0;
        penaltyTicks = -1;
        multiplier = 1;
    }

    shuffleTicks++;
    if (shuffleTicks > 60) {
        bool isPlacable = false;
        bool isPlayerPlacable = false;

        for (int i = 0; i < CARD_COLUMN_COUNT; i++) {
            int pi, cn, ci;
            checkPlacedIndex(i, playerPrevMoveIndex, playerCards, &pi, &cn, &ci);
            if (pi >= 0) {
                isPlacable = isPlayerPlacable = true;
                break;
            }

            checkPlacedIndex(i, enemyPrevMoveIndex, enemyCards, &pi, &cn, &ci);
            if (pi >= 0) {
                isPlacable = true;
                break;
            }
        }

        if (!isPlayerPlacable) {
            enemyNextMoveTicks *= 0.3f;
        }

        shuffleCount++;
        if (!isPlacable || shuffleCount > 2) {
            play(POWER_UP);
            
            // Move old cards out
            for (int i = 0; i < placedCardIndex; i++) {
                if (placedCards[i].pos.x < 50) {
                    placedCards[i].tPos.x = -50;
                } else {
                    placedCards[i].tPos.x = 150;
                }
            }

            // Add new cards
            placedCardNumbers[0] = rnd(1, 14);
            placedCardNumbers[1] = rnd(1, 14);

            for (int i = 0; i < 2; i++) {
                Card card = {0};
                card.num = placedCardNumbers[i];
                card.pos.x = i == 0 ? -5 : 105;
                card.pos.y = 0;
                card.isAlive = true;
                addPlacedCard(card, i);
            }

            shuffleCount = 0;
        }
        shuffleTicks = 0;
    }

    // Handle input
    int pci = floor((input.pos.x - 50) / CARD_INTERVAL_X + CARD_COLUMN_COUNT / 2.0f);
    
    if (input.isJustPressed) {
        if (pci >= 0 && pci < CARD_COLUMN_COUNT) {
            int result = placeCard(pci, playerPrevMoveIndex, playerCards);
            if (result < 0) {
                play(HIT);
                penaltyIndex = pci;
                penaltyTicks = 60;
                targetCenterY += 5;
                multiplier = 1;
                shuffleTicks = shuffleCount = 0;
            } else {
                play(COIN);
                playerPrevMoveIndex = result;
                targetCenterY -= 5;
                addScore(multiplier, result == 0 ? 8 : 92, centerY);
                multiplier++;
            }
        }
    }

    // Enemy AI
    enemyNextMoveTicks--;
    if (enemyNextMoveTicks < 0) {
        enemyNextMoveTicks = rnd(50, 70) / sqrt(difficulty);
        
        if (hasEnemyNextMove) {
            int pi, cn, ci;
            checkPlacedIndex(enemyNextMoveIndex, enemyPrevMoveIndex, enemyCards, &pi, &cn, &ci);
            if (pi < 0) {
                enemyNextMoveTicks *= 3;
            } else {
                play(SELECT);
                placeCard(enemyNextMoveIndex, enemyPrevMoveIndex, enemyCards);
                enemyPrevMoveIndex = pi;
                targetCenterY += 5;
                multiplier = 1;
            }
        }

        hasEnemyNextMove = false;
        int ni = rnd(0, CARD_COLUMN_COUNT);
        
        for (int i = 0; i < CARD_COLUMN_COUNT; i++) {
            int pi, cn, ci;
            checkPlacedIndex(ni, enemyPrevMoveIndex, enemyCards, &pi, &cn, &ci);
            if (pi >= 0) {
                if (pi != enemyPrevMoveIndex) {
                    enemyNextMoveTicks *= 1.5f;
                }
                hasEnemyNextMove = true;
                enemyNextMoveIndex = ni;
                break;
            }
            ni = wrap(ni + 1, 0, CARD_COLUMN_COUNT);
        }
    }

    // Update positions
    centerY += (targetCenterY - centerY) * 0.1f;

    // Draw player cards
    for (int i = 0; i < MAX_CARDS; i++) {
        Card* c = &playerCards[i];
        if (!c->isAlive) continue;

        movePos(&c->pos, &c->tPos, 0.2f);
        const char* ec = (c->gPos.y == 0 && c->gPos.x == pci) ? "green" : NULL;
        drawCard(c->pos.x, c->pos.y + centerY, c->num, c->gPos.y, ec);
    }

    // Draw enemy cards
    for (int i = 0; i < MAX_CARDS; i++) {
        Card* c = &enemyCards[i];
        if (!c->isAlive) continue;

        movePos(&c->pos, &c->tPos, 0.2f);
        const char* ec = (c->gPos.y == 0 && c->gPos.x == enemyNextMoveIndex && hasEnemyNextMove) ? "red" : NULL;
        drawCard(c->pos.x, c->pos.y + centerY, c->num, c->gPos.y, ec);
    }

    // Draw placed cards
    for (int i = 0; i < placedCardIndex; i++) {
        Card* c = &placedCards[i];
        if (!c->isAlive) continue;

        movePos(&c->pos, &c->tPos, 0.2f);
        drawCard(c->pos.x, c->pos.y + centerY, c->num, 0, NULL);
    }

    // Draw penalty
    if (penaltyTicks > 0) {
        penaltyTicks--;
        color = RED;
        text("X", calcCardX(penaltyIndex), centerY + 6);
        color = BLACK;
    }

    // Update center position
    if (targetCenterY < 16) {
        targetCenterY += (16 - targetCenterY) * 0.1f;
    }

    if (centerY > 94) {
        play(EXPLOSION);
        gameOver();
    }
}

static void movePos(Vector* p, Vector* tp, float ratio) {
    Vector diff = {tp->x - p->x, tp->y - p->y};
    vectorMul(&diff, ratio);
    vectorAdd(p, diff.x, diff.y);

    if (distanceTo(p, tp->x, tp->y) < 1) {
        p->x = tp->x;
        p->y = tp->y;
    }
}

static float calcCardX(int i) {
    return 50.5f + (i - CARD_COLUMN_COUNT/2.0f + 0.5f) * CARD_INTERVAL_X;
}

static float calcPlacedCardX(int i) {
    return 50 + (i - 0.5f) * 25;
}

static void calcPlayerCardPos(Vector* pos, Vector p) {
    pos->x = calcCardX(p.x);
    pos->y = (p.y + 1) * 11;
}

static void calcEnemyCardPos(Vector* pos, Vector p) {
    pos->x = calcCardX(p.x);
    pos->y = (p.y + 1) * -11;
}

static void drawCard(float x, float y, int n, int gy, const char* edgeColor) {
    if (y < -5 || y > 105) return;

    // Edge color logic matching JS
    if (edgeColor != NULL) {
        if (strcmp(edgeColor, "green") == 0) color = GREEN;
        else if (strcmp(edgeColor, "red") == 0) color = RED;
    } else {
        color = gy == 0 ? BLACK : LIGHT_BLACK;
    }
    box(x, y, 9, 10);

    // Draw card background
    color = WHITE;
    box(x, y, 7, 8);

    // Draw number
    color = gy == 0 ? BLACK : LIGHT_BLACK;
    if (n == 10) {
        character("a", x, y);
    } else {
        text(numChars[n], x, y);
    }
}

static void checkPlacedIndex(int idx, int ppi, Card* cards, int* outPi, int* outCn, int* outCi) {
    *outPi = -1;
    *outCn = -1;
    *outCi = -1;

    // Find card at idx
    for (int i = 0; i < MAX_CARDS; i++) {
        if (cards[i].isAlive && cards[i].gPos.y == 0 && cards[i].gPos.x == idx) {
            *outCn = cards[i].num;
            *outCi = i;
            break;
        }
    }

    // Check if it can be placed
    for (int i = 0; i < 2; i++) {
        if ((ppi == 1 || *outPi == -1) && 
            (*outCn == wrap(placedCardNumbers[i] - 1, 1, 14) || 
             *outCn == wrap(placedCardNumbers[i] + 1, 1, 14))) {
            *outPi = i;
        }
    }
}

static int placeCard(int idx, int ppi, Card* cards) {
    int pi, cn, ci;
    checkPlacedIndex(idx, ppi, cards, &pi, &cn, &ci);
    if (pi == -1) {
        return -1;
    }

    // Save the card we're moving
    Card movedCard = cards[ci];
    cards[ci].isAlive = false;

    // Update placed card numbers
    placedCardNumbers[pi] = cn;

    // Add to placed cards
    addPlacedCard(movedCard, pi);

    // Drop other cards in column
    dropCardsInColumn(cards, idx);

    // Add new card at bottom of column
    Vector gPos = {idx, CARD_ROW_COUNT - 1};
    for (int i = 0; i < MAX_CARDS; i++) {
        if (!cards[i].isAlive) {
            cards[i].gPos = gPos;
            cards[i].num = rnd(1, 14);
            if (cards == playerCards) {
                calcPlayerCardPos(&cards[i].pos, gPos);
            } else {
                calcEnemyCardPos(&cards[i].pos, gPos);
            }
            cards[i].tPos = cards[i].pos;
            cards[i].isAlive = true;
            break;
        }
    }

    shuffleTicks = shuffleCount = 0;
    return pi;
}

void addGameCardq() {
    addGame(title, description, characters, charactersCount, options, update);
}