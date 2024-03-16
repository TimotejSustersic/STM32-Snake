#include "gameMain.h"
#include "main.h"
#include <stdlib.h>

#define BUTTON_LEFT         GPIO_PIN_6 // B
#define BUTTON_RIGHT        GPIO_PIN_7 // B
#define BUTTON_UP           GPIO_PIN_6 // A
#define BUTTON_DOWN         GPIO_PIN_8 // A

#define LEFT    1
#define UP      2
#define RIGHT   3
#define DOWN    4

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
int cellSize = 14;

int x_border, y_border;
int x_apple = NULL, y_apple = NULL;

int snakeLength = 0;
int smer;

static void gameStart();
static void game();
static void gameOver();
static void DrawCanvas();
static void DrawGridCell();
// static uint32_t ButtonPressed(int GPIO, int pin);
static void resetVariables();
static void addtoSnake(int x, int y, int new);
static void Init_Buttons(void);
static node_t* push(int x, int y);

void gameMain() {

    Init_Buttons();

    // LTCD setup
    BSP_LCD_GetXSize(0, &x_size);
    BSP_LCD_GetYSize(0, &y_size);
    
    UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
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
    resetVariables();
    DrawCanvas();

    // 2. wait for user input
    while (!BSP_PB_GetState(BUTTON_USER_PIN)) { }
    
    gameStage = 1;
}

// game logic
static void game() {

    DrawGridCell(0, 0, 1);

    // initial
    BSP_LED_Off(LED_GREEN);

    // New Location //
    ////////////////////////////
    int x_new = head->x; 
    int y_new = head->y;
    
    if (HAL_GPIO_ReadPin(GPIOB, BUTTON_LEFT) && smer != RIGHT)
        smer = LEFT;
    if (HAL_GPIO_ReadPin(GPIOB, BUTTON_RIGHT) && smer != LEFT)
        smer = RIGHT;
    if (HAL_GPIO_ReadPin(GPIOA, BUTTON_DOWN) && smer != UP)
        smer = DOWN;
    if (HAL_GPIO_ReadPin(GPIOA, BUTTON_UP) && smer != DOWN)
        smer = UP;
    
    // rabis 4 gumbe
    if (smer == DOWN) 
        y_new++;    
    else if (smer == RIGHT) 
        x_new++;
    else if (smer == UP) 
        y_new--;    
    else if (smer == LEFT) 
        x_new--;
    ////////////////////////////

    // Colision //
    ////////////////////////////
    node_t* tmpNode = head->next;
    while (tmpNode != NULL)
    {
        // if new location would hit the body
        if (x_new == tmpNode->x && y_new == tmpNode->y) {
            gameStage = 0;
            return;
        }
        
        tmpNode = tmpNode->next;
    }
    ////////////////////////////

    // naris nov jabuk
    if (x_apple == NULL || y_apple == NULL) {
        x_apple = rand() % x_grid;
        y_apple = rand() % y_grid;
        DrawGridCell(x_apple, y_apple, 3);
    }

    // Apple //
    ////////////////////////////
    int appleCollision = 0;
    if (x_new == x_apple && y_new == y_apple) {
        appleCollision = 2;
        snakeLength++;
        BSP_LED_On(LED_GREEN);

        x_apple = NULL;
        y_apple = NULL;
    }
    ////////////////////////////

    addtoSnake(x_new, y_new, appleCollision);

    HAL_Delay(300);
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
    HAL_Delay(5000);
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
        gameStage = 0;
    }

    int x_pos = x * cellSize + x_border;
    int y_pos = y * cellSize + y_border;

    // snake Body
    if (type == 1) {
        // UTIL_LCD_DrawRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_LIGHTMAGENTA);
        UTIL_LCD_FillRect(x_pos, y_pos, cellSize, cellSize, UTIL_LCD_COLOR_ST_GREEN_LIGHT);
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

// SET == prtisnjen
// static uint32_t ButtonPressed(int GPIO, int pin) {

//     uint32_t startTime = HAL_GetTick();
    
//     int pressed = HAL_GPIO_ReadPin(GPIO, pin);

//     while (pressed == GPIO_PIN_SET)
//         pressed = HAL_GPIO_ReadPin(GPIO, pin);
            
//     return HAL_GetTick() - startTime;
// }

static node_t* push(int x, int y) {

    node_t* newNode = (node_t*)malloc(sizeof(node_t));

    newNode->next = NULL;

    if (head == NULL) {
        head = newNode;  
        DrawGridCell(x, y, 2); 
    } 
    else {
        DrawGridCell(x, y, 1); 
    }

    if (tail == NULL) {
        tail = newNode;
        newNode->prev = NULL;
    }
    else {
        newNode->prev = tail;
        tail = newNode;
    }

    newNode->x = x;
    newNode->y = y;

    snakeLength++;

    return newNode;
}

static void resetVariables() {

    BSP_LED_Off(LED_GREEN);
    BSP_LED_Off(LED_RED);

    // pobrisemo kaco ce je prevelika
    while (snakeLength > 3)
    {
        node_t* deletingNode = head;
        head = head->next;
        free(deletingNode);
        snakeLength--;
    }

    // create a snake
    int x = x_grid / 2;
    int y = y_grid / 2;

    if (snakeLength == 0)
        push(x, y);
    if (snakeLength == 1)
        push(x, y+1);
    if (snakeLength == 2)
        push(x, y+2);

    smer = LEFT;
}

static void addtoSnake(int x, int y, int new) {

    // dodamo na head
    // v pomnilnik
    node_t* newNode = (node_t*)malloc(sizeof(node_t));
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
        
        node_t* deletingNode = tail;

        tail->prev->next = NULL;    
        tail = tail->prev;

        // z grida
        DrawGridCell(deletingNode->x, deletingNode->y, 0);
        // s pomnilnika
        free(deletingNode);
    }
}

static void Init_Buttons(void) {

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
   
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;

    GPIO_InitStruct.Pin = BUTTON_UP | BUTTON_DOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BUTTON_LEFT | BUTTON_RIGHT;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}