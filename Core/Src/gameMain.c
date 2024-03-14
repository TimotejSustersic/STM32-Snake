#include "gameMain.h"
#include "main.h"

// snake body list
typedef struct node{
    int x, y;
    struct node *next, *prev;
} node_t;

node_t *head, *tail;




uint32_t x_size, y_size;
int gameStage = 0;

// grid size
int x_grid, y_grid;
int cellSize = 17;

int x_border, y_border;
int x_apple,  y_apple;


int snakeLength = 1;
// 0 => Desno, 1 => Dol, 2 => Levo, 3 => Gor, 
int smer = 0;

static void gameStart();
static void game();
static void gameOver();
static void DrawCanvas();
static void DrawGridCell();
static int ButtonPressed(int button);
static void resetVariables();
static void addtoSnake(int x, int y, int new);

void gameMain() {

    // LTCD setup
    BSP_LCD_GetXSize(0, &x_size);
    BSP_LCD_GetYSize(0, &y_size);
    
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
    UTIL_LCD_SetLayer(0);

    x_grid = x_size / cellSize;
    y_grid = y_size / cellSize;

    x_border = (x_size - (x_grid * cellSize)) / 2; 
    y_border = (y_size - (y_grid * cellSize)) / 2; 

    // create start node
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    head = newNode;
    tail = newNode;

    /**
     * 1. Game
     * 2. Game Over
    */
    while (1)
    {
        // game
        if (gameStage == 0)
            gameStart();
        else if (gameStage == 1)
            game();
        else
            gameOver();
    }
    
}

static void gameStart() {

    // 1. Draw shit and reset variables
    resetVariables();
    DrawCanvas();

    // 2. wait for user input
    while (!BSP_PB_GetState(BUTTON_USER_PIN)) { }
    
    gameStage = 1;
}

// game logic
static void game() {

    // initial
    BSP_LED_Off(LED_GREEN);


    // New Location //
    ////////////////////////////
    int x_new, y_new;

    // ce prtisnes gumb zamenjas smer za 90 v desno
    if (ButtonPressed(BUTTON_USER_PIN)) {
        // TODO nared smeri
    }
    ////////////////////////////

    // Colision //
    ////////////////////////////
    if (
        x_new >= x_grid || x_new <= 0 ||
        y_new >= y_grid || y_new <= 0 ||
    ) {
        gameStage = 2;
        return;
    }
    struct node *tempNode = head;
    while (tempNode != NULL)
    {
        // if new location would hit the body
        if (x_new == tempNode->x || y_new == tempNode->y) {
            gameStage = 2;
            return;
        }
        
        tempNode = tempNode->next;
    }
    ////////////////////////////

    // Apple //
    ////////////////////////////
    int appleCollision = 0;
    if (x_new == x_apple && y_new == y_apple) {
        appleCollision = 1;
        snakeLength++;

        // naris nov jabuk
        x_apple = rand() % x_grid;
        y_apple = rand() % y_grid;
        DrawGridCell(x_apple, y_apple, 3);
    }
    ////////////////////////////

    addtoSnake(x_new, y_new, appleCollision);

    HAL_Delay(500);
}

static void gameOver() {

    // Draw Game over and display score
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

    BSP_LED_Off(LED_GREEN);
    BSP_LED_On(LED_RED);

    UTIL_LCD_SetFont(&Font16);
    UTIL_LCD_DrawRect(400, 400, 400, 50, UTIL_LCD_COLOR_BLACK);

    char go;
    sprintf(go, "Game Over");
    UTIL_LCD_DisplayStringAt(450, 450, (uint8_t *)go, CENTER_MODE);
    char sc;
    sprintf(sc, "Score: %d", snakeLength);
    UTIL_LCD_DisplayStringAt(450, 500, (uint8_t *)sc, CENTER_MODE);

    // wait for 5 seconds
    // HAL_Delay(5000);
    // back to start
    gameStage = 0;
}

static void DrawCanvas() {    

    UTIL_LCD_Clear(UTIL_LCD_COLOR_BLACK);
    // main platform
    UTIL_LCD_DrawRect(x_border, y_border, x_size - 2*x_border, y_size - 2*y_border, UTIL_LCD_COLOR_WHITE);

    // grid
    for (int i = 0; i < x_grid; i++) 
        for (int j = 0; j < y_grid; j++) 
            DrawGridCell(i, j, 0);
}

static void DrawGridCell(int x, int y, int type) {

    // if we want to draw off grid its an error and game over
    if (
        x >= x_grid || 
        y >= y_grid ||
        x < 0 || 
        y < 0           
    ) {
        gameStage = 2;
    }

    int x_pos = x * cellSize + x_border;
    int y_pos = y * cellSize + y_border;

    // snake Body
    if (type == 1) {
        // UTIL_LCD_DrawRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_LIGHTMAGENTA);
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_LIGHTMAGENTA);
    }
    // snake Head
    else if (type == 2) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_MAGENTA);
    }
    // apple
    else if (type == 3) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_DARKRED);
    }
    // empty cell
    else if (type == 0) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_BLACK);
        UTIL_LCD_DrawRect(x_pos, y_pos, cellSize, cellSize, 0xFF070707UL);
    }
}

// todo nared da returna kolk je caku in odstej od delaya
static int ButtonPressed(int button) {

    int buttonPressed = BSP_PB_GetState(button);
    while (buttonPressed)
        if (!BSP_PB_GetState(button))
            return 1;
    return 0;
}

static void resetVariables() {

    BSP_LED_Off(LED_GREEN);
    BSP_LED_Off(LED_RED);

    // pobrisemo kaco
    while (snakeLength > 1)
    {
        struct node *deletingNode = head;
        head = head->next;
        free(deletingNode);
        snakeLength--;
    }

    startNode->x = x_grid / 2;
    startNode->y = y_grid / 2;
    startNode->next = NULL;
    startNode->prev = NULL;
}

static void addtoSnake(int x, int y, int new) {

    // dodamo na head
    // v pomnilnik
    struct node *newNode = (struct node*) malloc(sizeof(struct node));
    newNode->x = x;
    newNode->y = y;

    head->prev = newNode;
    newNode->next = head;
    newNode->prev = NULL;
    head = newNode;

    // na grid
    DrawGridCell(newNode->x, newNode->y, 2); // head
    DrawGridCell(newNode->next->x, newNode->next->y, 1); // change head color to body color
    ///////////////////

    ///////////////////
    // brisemo zadnjega ce nismo jabolka pojedl
    if (!new) {
        
        struct node *deletingNode = last;

        last->prev->next = NULL;    
        last = last->prev;

        // z grida
        DrawGridCell(deletingNode->x, deletingNode->y, 0);
        // s pomnilnika
        free(deletingNode);
    }
}
