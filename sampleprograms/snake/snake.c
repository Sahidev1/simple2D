#include "../../graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define WINDOW_W (3*256)
#define WINDOW_H (3*256)
#define POS_VECTOR_SIZE 512
#define SNAKE_START_POS_X 256
#define SNAKE_START_POS_Y 256
#define TILE_WIDTH 16
#define TILE_HEIGHT 16
#define SNAKE_COLOR (DRAW_COLOR_GREEN)
#define TEXTURE_PATH_FROM_ROOT_DIR "sampleprograms/snake/pixilapple.png"
#define FONT_PATH_FROM_ROOT_DIR "sampleprograms/snake/font.ttf"
#define BACKGROUND_COLOR 0xFFC0C0C0
#define MOVEMENT_INTERVAL_MS (8*16)
#define RERENDER_INTERVAL_MS 16
#define APPLE_TEXTRURE_WIDTH 16
#define APPLE_TEXTURE_HEIGHT 16
#define BASE_SCORE_INCR 4
#define HEAP_CHUNK_SIZE_INCREMENT_BYTES (1<<15)

typedef enum {NOT_DRAWN, DRAWN} snakeDrawState;
typedef enum {RIGHT, LEFT, DOWN, UP} Snakeheading;
typedef enum {GAME_ON, GAME_OVER} gameState;

static int g_posIndex = 0;
static int g_score = 0;
static Texture g_scoreTexture;
static gameState g_game_state;
static bool g_cond_texture_created = FALSE;


typedef struct {
    void* heap_chunk;
    int free_chunk_index;
    int chunk_size;
} heap_chunk;

static heap_chunk g_tiles_heapchunk;

typedef struct Tile{
    Rectangle data;
    struct Tile* next;
    struct Tile* prev;
    int posIndex;
} tile;

typedef struct {
    snakeDrawState draw_state;
    tile* tiles;
    tile* bottomTile;
    Snakeheading heading;
    int tileCount;
    int positions[POS_VECTOR_SIZE][2];
} snake;

typedef struct {
    Texture txt;
    Rectangle data;
} apple;

void init_heap_chunk(){
    g_tiles_heapchunk.free_chunk_index = 0;
    g_tiles_heapchunk.chunk_size = HEAP_CHUNK_SIZE_INCREMENT_BYTES;
    g_tiles_heapchunk.heap_chunk = malloc(HEAP_CHUNK_SIZE_INCREMENT_BYTES);
}

void* alloc_chunk(size_t alloc_size_bytes){
    if (g_tiles_heapchunk.free_chunk_index + alloc_size_bytes > g_tiles_heapchunk.chunk_size){
        g_tiles_heapchunk.heap_chunk = realloc(g_tiles_heapchunk.heap_chunk, g_tiles_heapchunk.chunk_size + HEAP_CHUNK_SIZE_INCREMENT_BYTES);
    }
    void* chunk = g_tiles_heapchunk.heap_chunk;
    chunk = chunk + g_tiles_heapchunk.free_chunk_index;
    g_tiles_heapchunk.free_chunk_index += alloc_size_bytes;
    return chunk;
}

void destroy_chunk(){
    free(g_tiles_heapchunk.heap_chunk);
}

void print_snake(snake* s){
    printf("--------------------SNAKE_PRINT-------------------\n");
    printf("snake tile count: %d\n", s->tileCount);
    printf("snake bottom tile ptr: %p\n", s->bottomTile);
    printf("snake heading: %d\n", s->heading);
    printf("snake draw state: %d\n", s->draw_state);
    printf("snake positions:\n");
    for (int i = 0; i < s->tileCount; i++){
        printf("x: %d, y: %d\n", s->positions[i][0], s->positions[i][1]);
    }
    tile* t_ptr = s->tiles;
    printf("snake tiles:\n");
    while (t_ptr != NULL){
        printf("x: %d, y: %d\n", t_ptr->data.origin.x, t_ptr->data.origin.y);
        printf("\tcurr ptr: %p ,prev ptr: %p, next ptr: %p\n", t_ptr ,t_ptr->prev, t_ptr->next);
        t_ptr = t_ptr->next;
    }
    printf("--------------------SNAKE_PRINT_END-------------------\n");
}

void keyboard_eventhandler(KeyboardEvent* ke, void* data){
    snake* s = (snake*) data;
    int keycode_diff = ke->keycode - KEYCODE_ARROW_RIGHT;
    if (keycode_diff >= 0 && keycode_diff <= 3 && ke->state == PRESSED){
        if (s->heading + keycode_diff != 5 && s->heading + keycode_diff != 1) s->heading = keycode_diff;
    }
}

void set_tile(tile *t, Rectangle *rect, tile* next, tile* prev){
    t->data.origin = rect->origin;
    t->data.h = rect->h, t->data.w = rect->w;
    t->next = next;
    t->prev = prev;
    t->posIndex = g_posIndex++;
}

void initialize_snake(snake* s){
    tile* t = (tile*) alloc_chunk(sizeof(tile));
    Vector origin = {.x = SNAKE_START_POS_X, .y = SNAKE_START_POS_Y};
    Rectangle rect = {.origin = origin, .h = TILE_HEIGHT, .w=TILE_HEIGHT};

    set_tile(t, &rect, NULL, NULL);
    s->tiles = t;
    s->bottomTile = NULL;
    s->tileCount = 1;
    s->positions[0][0] = SNAKE_START_POS_X;
    s->positions[0][1] = SNAKE_START_POS_Y;
    s->draw_state = NOT_DRAWN;
    s->heading = rand()%4;
}

void snakeAppendTile(snake* s){
    tile* newHead = (tile*) alloc_chunk(sizeof(tile));
    Vector origin;
    if (s->bottomTile == NULL){
        switch (s->heading)
        {
        case DOWN:
            origin.x = s->tiles->data.origin.x;
            origin.y = s->tiles->data.origin.y + s->tiles->data.h;
            break;
        case UP:
            origin.x = s->tiles->data.origin.x;
            origin.y = s->tiles->data.origin.y - s->tiles->data.h;
            break;
        case LEFT:
            origin.x = s->tiles->data.origin.x - s->tiles->data.w;
            origin.y = s->tiles->data.origin.y;
            break;
        case RIGHT:
            origin.x = s->tiles->data.origin.x + s->tiles->data.w;
            origin.y = s->tiles->data.origin.y;
            break;
        default:
            break;
        }
    }
    else {
        Vector bottomPos = s->bottomTile->data.origin;
        Vector bottom_prevPos = s->bottomTile->prev->data.origin;
        Vector vDiff = {.x = bottomPos.x - bottom_prevPos.x, .y = bottomPos.y - bottom_prevPos.y};
        if (vDiff.x > 0){
            origin.x = bottomPos.x + s->bottomTile->data.w;
            origin.y = bottomPos.y;
        }
        else if (vDiff.x < 0){
            origin.x = bottomPos.x - s->bottomTile->data.w;
            origin.y = bottomPos.y;
        }
        else if (vDiff.y > 0){
            origin.x = bottomPos.x;
            origin.y = bottomPos.y + s->bottomTile->data.h;
        }
        else if (vDiff.y < 0){
            origin.x = bottomPos.x;
            origin.y = bottomPos.y - s->bottomTile->data.h;
        }
    }
    Rectangle rect = {.origin = origin, .h = TILE_HEIGHT, .w = TILE_WIDTH};
    set_tile(newHead, &rect, NULL, NULL);
    s->positions [newHead->posIndex][0] = origin.x;
    s->positions [newHead->posIndex][1] = origin.y;
    if (s->bottomTile == NULL){
        s->tiles->next = newHead;
        s->bottomTile = newHead;
        s->bottomTile->prev = s->tiles;
    } else {
        newHead->prev = s->bottomTile;
        s->bottomTile->next = newHead;
        s->bottomTile = s->bottomTile->next;
    }
    s->tileCount++;
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
    if (TRUE){
        tile* t_ptr = s->tiles;
        S2D_setDrawColor(SNAKE_COLOR);
        while (t_ptr != NULL){
            S2D_drawFillRectangle(&t_ptr->data);
            t_ptr = t_ptr->next;
        }
        s->draw_state = DRAWN;
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
    if (s->bottomTile == NULL){
        movement(&s->tiles->data.origin, s->heading, s->tiles->data.h, s->tiles->data.w);
        s->positions[0][0] = s->tiles->data.origin.x;
        s->positions[0][1] = s->tiles->data.origin.y;

    }
    else {
        Vector currHeadPos = s->tiles->data.origin;
        movement(&s->tiles->data.origin, s->heading, s->tiles->data.h, s->tiles->data.w);
        s->positions[0][0] = s->tiles->data.origin.x;
        s->positions[0][1] = s->tiles->data.origin.y;
        s->bottomTile->data.origin = currHeadPos;
        s->positions[s->bottomTile->posIndex][0] = currHeadPos.x;
        s->positions[s->bottomTile->posIndex][1] = currHeadPos.y;
        if (s->tileCount > 2){
            tile* head = s->tiles;
            tile* nextToHead = s->tiles->next;
            tile* bottom = s->bottomTile;
            tile* prevBottom = s->bottomTile->prev;

            head->next = bottom;
            bottom->prev = head;
            bottom->next = nextToHead;
            nextToHead->prev = bottom;
            prevBottom->next = NULL;
            s->bottomTile = prevBottom;
        }

    }
}



void drawApple(apple* a){
    S2D_drawTexture(&(a->txt), &(a->data));
}

bool snakeCollisionCheck(snake *s){
    int head_xpos = s->positions[s->tiles->posIndex][0];
    int head_ypos = s->positions[s->tiles->posIndex][1];
    int x_max = WINDOW_W, y_max = WINDOW_H;
    int* positions =  (int*)s->positions;
    int x, y;
    for (int i = 0; i < s->tileCount; i++){
        x = *(positions + 2*i);
        y = *(positions + 2*i + 1);
        if (x < 0 || x >= x_max || y < 0 || y >= y_max) return TRUE;
        if (i != s->tiles->posIndex && head_xpos == x && head_ypos == y ) return TRUE;
    }
    return FALSE;
}

bool snakeAppleCollisionCheck(apple* a, snake* s){
    Vector snakeVect = s->tiles->data.origin;
    Vector appleVect = a->data.origin;
    int snake_w = s->tiles->data.w;
    int apple_w = a->data.w;
    int snake_h = s->tiles->data.h;
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
    init_heap_chunk();
    S2D_initialize();
    S2D_createWindow("Snake", WINDOW_W, WINDOW_H);
    S2D_addKeyboardEventhandler(keyboard_eventhandler);
    S2D_setDrawColor(BACKGROUND_COLOR);
    S2D_clearScreen();

    snake s;
    apple a;
    printf("retcode: %d\n",init_apple(&a));
    initialize_snake(&s);
    S2D_presentRender();
    print_snake(&s);
    
    Uint32 ticks_init = S2D_getTicks();
    Uint32 ticks_curr;
    bool collisionState = FALSE;
    int prevScore = g_score;
    g_game_state = GAME_ON; 
    Texture* gameover_txt = createGameOverTexture(20, (Color){100, 255,0, 255}, "GAME OVER!");
    Rectangle game_over_dims = {.origin={WINDOW_W/4, WINDOW_H/4}, .w = 4*gameover_txt->width, .h = 4*gameover_txt->height};
    createScoreTexture(20, (Color){0,0,255,255});

    while(g_game_state == GAME_ON){
        S2D_eventDequeue(&s);
        ticks_curr = S2D_getTicks();
        if (ticks_curr - ticks_init >= MOVEMENT_INTERVAL_MS && !collisionState){
            snakeMove(&s);
            collisionState = snakeCollisionCheck(&s);
            if (collisionState) g_game_state = GAME_OVER;

            print_snake(&s);
            printf("apple pos x: %d, y: %d\n", a.data.origin.x, a.data.origin.y);
            printf("SCORE = %d, prevscore = %d\n", g_score, prevScore);
            ticks_init = ticks_curr;
        }

        if (snakeAppleCollisionCheck(&a, &s)){
                g_score += BASE_SCORE_INCR * s.tileCount * (1 + (s.tileCount/10));
                reposition_apple(&a, &s);
                snakeAppendTile(&s);
        }
        
        S2D_delay(RERENDER_INTERVAL_MS);
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
    destroy_chunk();

    while(S2D_getTicks() < ticks_curr + LOOP_TICKS){
        S2D_eventDequeue(NULL);
    }

    return 0;
}