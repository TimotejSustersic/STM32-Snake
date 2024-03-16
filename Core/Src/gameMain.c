#include "gameMain.h"
#include "main.h"

#define BUTTON_LEFT         GPIO_PIN_6 // A
#define BUTTON_UP           GPIO_PIN_6 // B
#define BUTTON_RIGHT        GPIO_PIN_8 // A
#define BUTTON_DOWN         GPIO_PIN_7 // B

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
//          0: Gor
// 3: Levo          1: Desno
//          2: Dol
int smer = -1;

static void gameStart();
static void game();
static void gameOver();
static void DrawCanvas();
static void DrawGridCell();
static uint32_t ButtonPressed(int GPIO, int pin);
static void resetVariables();
static void addtoSnake(int x, int y, int new);
static void Init_Buttons(void);

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

     DrawGridCell(0, 0, 2);
    
    gameStage = 1;
}

// game logic
static void game() {

    // initial
    BSP_LED_Off(LED_GREEN);

    // New Location //
    ////////////////////////////
    int x_new = head->x; 
    int y_new = head->y;
    
    if (ButtonPressed(GPIOB, BUTTON_UP) && smer != 2)
        smer = 0;
    else if (ButtonPressed(GPIOA, BUTTON_RIGHT) && smer != 3)
        smer = 1;
    else if (ButtonPressed(GPIOB, BUTTON_DOWN)  && smer != 0)
        smer = 2;
    else if (ButtonPressed(GPIOA, BUTTON_LEFT)  && smer != 1)
        smer = 3;
    
    if (smer > 3)
        smer = 0;
    
    // rabis 4 gumbe
    if (smer == 0) 
        y_new--;    
    else if (smer == 1) 
        x_new++;
    else if (smer == 2) 
        y_new++;    
    else if (smer == 3) 
        x_new--;
    ////////////////////////////

    // Colision //
    ////////////////////////////
    if (
        x_new >= x_grid || x_new <= 0 ||
        y_new >= y_grid || y_new <= 0
    ) {
        gameStage = 2;
        return;
    }
    node_t* tempNode = head;
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
        BSP_LED_On(LED_GREEN);

        // naris nov jabuk
        x_apple = rand() % x_grid;
        y_apple = rand() % y_grid;
        DrawGridCell(x_apple, y_apple, 3);
    }
    ////////////////////////////

    addtoSnake(x_new, y_new, appleCollision);

    HAL_Delay(1000);
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

// SET == prtisnjen
static uint32_t ButtonPressed(int GPIO, int pin) {

    uint32_t startTime = HAL_GetTick();
    
    int pressed = HAL_GPIO_ReadPin(GPIO, pin);

    while (pressed == GPIO_PIN_SET)
        pressed = HAL_GPIO_ReadPin(GPIO, pin);
            
    return HAL_GetTick() - startTime;
}

static void resetVariables() {

    BSP_LED_Off(LED_GREEN);
    BSP_LED_Off(LED_RED);

    // pobrisemo kaco
    while (snakeLength > 1)
    {
        node_t* deletingNode = head;
        head = head->next;
        free(deletingNode);
        snakeLength--;
    }

    head->x = x_grid / 2;
    head->y = y_grid / 2;
    head->next = NULL;
    head->prev = NULL;
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

    GPIO_InitStruct.Pin = BUTTON_LEFT | BUTTON_RIGHT;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = BUTTON_UP | BUTTON_DOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}