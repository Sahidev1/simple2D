#include "../../graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WINDOW_W (3*256)
#define WINDOW_H (3*256)
#define SNAKE_START_POS_X 256
#define SNAKE_START_POS_Y 256
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define POS_VECTOR_SIZE ((WINDOW_W/TILE_WIDTH)* (WINDOW_H/TILE_HEIGHT)) 
#define SNAKE_COLOR (DRAW_COLOR_GREEN)
#define TEXTURE_PATH_FROM_ROOT_DIR "sampleprograms/snake/pixilapple.png"
#define FONT_PATH_FROM_ROOT_DIR "sampleprograms/snake/font.ttf"
#define BACKGROUND_COLOR 0xFFC0C0C0
#define MOVEMENT_INTERVAL_MS (8*16)
#define RERENDER_INTERVAL_MS (16)
#define APPLE_TEXTRURE_WIDTH 16
#define APPLE_TEXTURE_HEIGHT 16
#define BASE_SCORE_INCR 4
#define HEAD 0
#define BOTTOM(s) (s->tileCount - 1)

typedef enum {NOT_DRAWN, DRAWN} snakeDrawState;
typedef enum {RIGHT, LEFT, DOWN, UP} Snakeheading;
typedef enum {GAME_ON, GAME_OVER} gameState;

static int g_posIndex = 0;
static int g_score = 0;
static Texture g_scoreTexture;
static gameState g_game_state;
static bool g_cond_texture_created = FALSE;
static bool g_pause = FALSE;
static bool waiting_keyEvent = FALSE;
static Color snakeRGBA;
static bool alterColors = FALSE;
static bool headingLock = FALSE;

typedef enum {X , Y} coord;


typedef struct {
    Snakeheading heading;
    int tileCount;
    int positions[POS_VECTOR_SIZE][2];
} snake;

typedef struct {
    Texture txt;
    Rectangle data;
} apple;

void print_snake(snake* s){
    printf("--------------------SNAKE_PRINT-------------------\n");
    printf("snake tile count: %d\n", s->tileCount);
    printf("snake heading: %d\n", s->heading);
    printf("snake positions:\n");
    for (int i = 0; i < s->tileCount; i++){
        printf("snake %d position: x: %d, y: %d\n", i, s->positions[i][0], s->positions[i][1]);
    }
    printf("--------------------SNAKE_PRINT_END-------------------\n");
}

void keyboard_eventhandler(KeyboardEvent* ke, void* data){
    if (g_game_state == GAME_OVER) return;
    snake* s = (snake*) data;
    if (ke->keycode == KEYCODE_SPACE && ke->state == PRESSED){
        g_pause = !g_pause;
    }
    int keycode_diff = ke->keycode - KEYCODE_ARROW_RIGHT;
    if (keycode_diff >= 0 && keycode_diff <= 3 && ke->state == PRESSED){
        if (!headingLock && s->heading + keycode_diff != 5 && s->heading + keycode_diff != 1){
            s->heading = keycode_diff;
            headingLock = TRUE;
        }
        printf("\teventhandler heading val: %d\n", s->heading);
    }
}

void initialize_snake(snake* s){
    s->tileCount = 1;
    s->positions[HEAD][0] = SNAKE_START_POS_X;
    s->positions[HEAD][1] = SNAKE_START_POS_Y;
    s->heading = rand()%4;
}

void snakeAppendTile(snake* s){
    Vector origin;
    Snakeheading heading;
 
    if (s->tileCount > 1) {
        Vector bottomPos = {s->positions[BOTTOM(s)][X], s->positions[BOTTOM(s)][Y]};
        Vector bottom_prevPos = {s->positions[s->tileCount - 2][X], s->positions[s->tileCount - 2][Y]};
        Vector vDiff = {.x = bottomPos.x - bottom_prevPos.x, .y = bottomPos.y - bottom_prevPos.y};
        if (vDiff.x > 0) heading = LEFT;
        else if (vDiff.x < 0) heading = RIGHT;
        else if (vDiff.y > 0) heading = UP;
        else if (vDiff.y < 0) heading = DOWN;
    } else {
        heading = s->heading;
    }


    switch (heading){
        case DOWN:
            origin.x = s->positions[BOTTOM(s)][X];
            origin.y = s->positions[BOTTOM(s)][Y] - TILE_HEIGHT;
            break;
        case UP:
            origin.x = s->positions[BOTTOM(s)][X];
            origin.y = s->positions[BOTTOM(s)][Y] + TILE_HEIGHT;
            break;
        case LEFT:
            origin.x = s->positions[BOTTOM(s)][X] + TILE_WIDTH;
            origin.y = s->positions[BOTTOM(s)][Y];
            break;
        case RIGHT:
            origin.x = s->positions[BOTTOM(s)][X] - TILE_WIDTH;
            origin.y = s->positions[BOTTOM(s)][Y];
            break;
        default:
            break;
    }

    ++s->tileCount;
    s->positions [BOTTOM(s)][X] = origin.x;
    s->positions [BOTTOM(s)][Y] = origin.y;
}

void reposition_apple(apple* a, snake* s){
    a->data.origin.x = rand()% (WINDOW_W - APPLE_TEXTRURE_WIDTH); 
    a->data.origin.y = rand()% (WINDOW_H - APPLE_TEXTURE_HEIGHT);
}

int init_apple(apple* a){
    int retcode;
    reposition_apple(a, NULL);
    retcode = S2D_createTexture(TEXTURE_PATH_FROM_ROOT_DIR, &a->txt);
    a->data.w = a->txt.width;
    a->data.h = a->txt.height;
    return retcode;
}

void drawSnake(snake* s){
        int i = 0;
        Rectangle rect;
        rect.w = TILE_WIDTH, rect.h = TILE_HEIGHT;
        Color copy = snakeRGBA;
        while (i < s->tileCount){
            rect.origin.x = s->positions[i][X];
            rect.origin.y = s->positions[i][Y];
            S2D_setDrawColor(S2D_colorStructToHex(snakeRGBA));
            S2D_fillRectangle(&rect);
            S2D_setDrawColor(DRAW_COLOR_BLACK);
            S2D_drawRectangle(&rect);
            i++;
        }
        if(alterColors){
            snakeRGBA.R = (snakeRGBA.R + 1)%255;
            alterColors = FALSE;
        }
}

void movement(Vector* pos, Snakeheading heading, int verticalMovementIncrement, int horizontalMovementIncrement){
    switch (heading){
        case DOWN:
            pos->y += verticalMovementIncrement;
            break;
        case UP: 
            pos->y -= verticalMovementIncrement;
            break;
        case LEFT:
            pos->x -= horizontalMovementIncrement;
            break;
        case RIGHT:
            pos->x += horizontalMovementIncrement;
            break;
        default:
            break;
    }
}

void snakeMove(snake* s){
    Vector pos;
    Vector prevHeadPos;
    pos = (Vector){.x = s->positions[HEAD][X], .y = s->positions[HEAD][Y]};
    prevHeadPos = pos;
    movement(&pos, s->heading, TILE_HEIGHT, TILE_WIDTH);
    s->positions[HEAD][X] = pos.x, s->positions[HEAD][Y] = pos.y;

    int i = 1;
    pos = prevHeadPos;
    while (i < s->tileCount){
        prevHeadPos.x = s->positions[i][X], prevHeadPos.y = s->positions[i][Y];
        s->positions[i][X] = pos.x, s->positions[i][Y] = pos.y;
        pos = prevHeadPos;
        i++;
    }
}



void drawApple(apple* a){
    S2D_drawTexture(&(a->txt), &(a->data));
}

bool snakeCollisionCheck(snake *s){
    int head_xpos = s->positions[HEAD][X];
    int head_ypos = s->positions[HEAD][Y];
    int x_max = WINDOW_W, y_max = WINDOW_H;
    int x, y;
    if (head_xpos < 0 || head_xpos >= x_max || head_ypos < 0 || head_ypos >= y_max) return TRUE;
    for (int i = 1; i < s->tileCount; i++){
        x = s->positions[i][X], y = s->positions[i][Y];
        if (head_xpos == x && head_ypos == y ) return TRUE;
    }
    return FALSE;
}

bool snakeAppleCollisionCheck(apple* a, snake* s){
    Vector snakeVect = (Vector){s->positions[HEAD][X], s->positions[HEAD][Y]};
    Vector appleVect = a->data.origin;
    int snake_w = TILE_WIDTH;
    int apple_w = a->data.w;
    int snake_h = TILE_HEIGHT;
    int apple_h = a->data.h;
    bool c0, c1;
    if (appleVect.x > snakeVect.x){
        c0 =snakeVect.x + snake_w <= appleVect.x;
    }else {
        c0 = appleVect.x + apple_w <= snakeVect.x;
    }
    if(appleVect.y > snakeVect.y){
        c1 = appleVect.y >= snakeVect.y + snake_h;
    }else {
        c1 = snakeVect.y >= appleVect.y + apple_h;
    }
    return !(c0 || c1);
}

void createScoreTexture(int font_size, Color fcolor){
    StringRenderData sd;
    sd.font_fpath = FONT_PATH_FROM_ROOT_DIR;
    sd.font_size = font_size;
    sd.foreground_color = fcolor;
    sd.string = malloc(20*sizeof(char));
    sprintf(sd.string, "SCORE: %d ", g_score);
    if(g_cond_texture_created) S2D_destroyTexture(&g_scoreTexture);
    S2D_createUTF8Texture(&g_scoreTexture, &sd);
    free(sd.string);
}

Texture* createGameOverTexture(int font_size, Color color, char* game_over_msg){
    Texture* txt = malloc(sizeof(Texture));
    StringRenderData sd;
    sd.font_fpath = FONT_PATH_FROM_ROOT_DIR;
    sd.font_size = font_size;
    sd.foreground_color = color;
    sd.string = game_over_msg;
    S2D_createUTF8Texture(txt, &sd);
    return txt;
}

int main (){
    srand(time(NULL));
    S2D_initialize();
    S2D_createWindow("Snake", WINDOW_W, WINDOW_H);
    S2D_addKeyboardEventhandler(keyboard_eventhandler);
    S2D_setDrawColor(BACKGROUND_COLOR);
    S2D_clearScreen();
    snakeRGBA = (Color){rand()%255,rand()%255,0 ,255};

    snake s;
    apple a;
    init_apple(&a);
    initialize_snake(&s);
    S2D_presentRender();
    print_snake(&s);
    
    Uint32 ticks_init = S2D_getTicks();
    Uint32 ticks_curr;
    Uint32 prev_frame_ticker = ticks_curr;
    Uint32 curr_frame_ticker = ticks_curr;
    bool collisionState = FALSE;
    int prevScore = g_score;
    g_game_state = GAME_ON; 
    Texture* gameover_txt = createGameOverTexture(20, (Color){100, 255,0, 255}, "GAME OVER!");
    Rectangle game_over_dims = {.origin={WINDOW_W/4, WINDOW_H/4}, .w = 4*gameover_txt->width, .h = 4*gameover_txt->height};
    createScoreTexture(20, (Color){0,0,255,255});
    int tile_appends = 1;

    while(g_game_state == GAME_ON){
        S2D_eventDequeue(&s);
        if (g_pause) {
            S2D_delay(100);
            continue;
        }
        ticks_curr = S2D_getTicks();
        if (ticks_curr - ticks_init >= MOVEMENT_INTERVAL_MS && !collisionState){
            snakeMove(&s);
            collisionState = snakeCollisionCheck(&s);
            if (collisionState) g_game_state = GAME_OVER;
            print_snake(&s);
            ticks_init = ticks_curr;
            alterColors = TRUE;
            headingLock = FALSE;
        }

        if (snakeAppleCollisionCheck(&a, &s)){
                g_score += BASE_SCORE_INCR * s.tileCount * (1 + (s.tileCount/10));
                reposition_apple(&a, &s);
                for(int i = 0; i < tile_appends; i++){
                    snakeAppendTile(&s);
                }
                tile_appends++;
        }
        
        curr_frame_ticker = S2D_getTicks();
        if (curr_frame_ticker - prev_frame_ticker >= RERENDER_INTERVAL_MS){
            if(curr_frame_ticker - prev_frame_ticker > RERENDER_INTERVAL_MS) printf("\t----RERENDER MISS %d\n", curr_frame_ticker - prev_frame_ticker);
            prev_frame_ticker = curr_frame_ticker;
        }
        S2D_delay(1);
        S2D_setDrawColor(BACKGROUND_COLOR);
        S2D_clearScreen();
        drawSnake(&s);
        drawApple(&a);
        
        if (g_score != prevScore){
            createScoreTexture(20, (Color){0,0,255,255});
            prevScore = g_score;
        }
        S2D_drawTextureNative(&g_scoreTexture, (Vector){8,8});      
        if(g_game_state == GAME_OVER){
            S2D_drawTexture(gameover_txt, &game_over_dims); 
        }
     
        S2D_presentRender();
    }

    ticks_curr = S2D_getTicks();
    const int LOOP_TICKS = 4000;

    //cleanup
    S2D_destroyTexture(gameover_txt);
    S2D_destroyTexture(&g_scoreTexture);
    S2D_destroyTexture(&a.txt);

    while(S2D_getTicks() < ticks_curr + LOOP_TICKS){
        S2D_eventDequeue(NULL);
    }

    return 0;
}