#include "gameMain.h"
#include "main.h"

uint32_t x_size, y_size;

// grid size
int x_grid, y_grid;
int cellSize = 17;
int x_border, y_border;

int gameStage = 0;

static void gameStart();
static void game();
static void gameOver();
static void DrawCanvas();
static void DrawGridCell();

void gameMain() {

    // LTCD setup
    BSP_LCD_GetXSize(0, &x_size);
    BSP_LCD_GetYSize(0, &y_size);
    
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);

    UTIL_LCD_SetFont(&Font16);
    UTIL_LCD_SetLayer(0);

    x_grid = x_size / cellSize;
    y_grid = y_size / cellSize;

    x_border = (x_size - (x_grid * cellSize)) / 2; 
    y_border = (y_size - (y_grid * cellSize)) / 2; 

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
    DrawCanvas();

    // 2. wait for user input
    while (!BSP_PB_GetState(BUTTON_USER_PIN)) {

        BSP_LED_On(LED_GREEN);
    }
    
    gameStage = 1;
}

// game logic
static void game() {

    UTIL_LCD_Clear(UTIL_LCD_COLOR_DARKGRAY);
}

static void gameOver() {

    // 1. Stop game

    // 2. Draw Game over and display score

    // 3. wait for 5 seconds
    HAL_Delay(500);

}

static void DrawCanvas() {

    UTIL_LCD_Clear(UTIL_LCD_COLOR_DARKGRAY);

    // main platform
    UTIL_LCD_DrawRect(x_border, y_border, x_size - 2*x_border, y_size - 2*y_border, UTIL_LCD_COLOR_WHITE);

    DrawGridCell(5, 5, 1);

    // grid
    for (int i = 0; i < x_grid; i++) {
        for (int j = 0; j < y_grid; j++) {
            DrawGridCell(i, j, 0);
        }
    }
}

static void DrawGridCell(int x, int y, int type) {

    int x_pos = x * cellSize + x_border;
    int y_pos = y * cellSize + y_border;

    // snake Body
    if (type == 1) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_LIGHTMAGENTA);
    }
    // snake Head
    if (type == 2) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_MAGENTA);
    }
    // apple
    else if (type == 3) {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_DARKRED);
    }
    // empty cell
    else {
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_BLACK);
        UTIL_LCD_DrawRect(x_pos, y_pos, cellSize, cellSize, 0xFF070707UL);
    }
}