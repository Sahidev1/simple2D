#ifndef GRAPHICS_H
#define GRAPHICS_H


// RGBA color codes, coded in LSB = Red and MSB = alpha
#define DRAW_COLOR_DEFAULT (0xFF000000)
#define DRAW_COLOR_BLACK (0xFF000000)
#define DRAW_COLOR_WHITE (0xFFFFFFFF)
#define DRAW_COLOR_RED (0xFF0000FF)
#define DRAW_COLOR_GREEN (0xFF00FF00)
#define DRAW_COLOR_BLUE (0xFFFF0000)
#define DRAW_COLOR_YELLOW (0xFF00FFFF)
#define DRAW_COLOR_TRANSPARENT (0x00000000)

#define UNSPECIFIED_ERROR (0xff)
#define ERROR_INITIALIZE (0x1)
#define ERROR_CREATE_WINDOW (0x2)
#define ERROR_CREATE_RENDERER (0x3)
#define ERROR_SET_DRAW_COLOR (0x4)
#define ERROR_CLEAR_SCREEN (0x5)
#define ERROR_DRAW_POINT (0x6)
#define ERROR_DRAW_COORD (0x7)
#define ERROR_DRAW_RECT (0x8)
#define ERROR_RECT_FILL (0x9)
#define ERROR_DRAW_LINE (0xA)
#define ERROR_CREATE_TEXTURE (0xB)
#define ERROR_DRAW_TEXTURE (0xC)
#define ERROR_DESTROYED_TEXTURE (0xD)



#include <SDL2/SDL_stdinc.h>

typedef enum {FALSE, TRUE} bool;

typedef enum {RELEASED, PRESSED} keyState;
typedef enum keyState BtnState;

typedef enum  {BUTTON, MOVEMENT, WHEEL} MouseEventType;

typedef enum {
    KEYCODE_UNKNOWN = 0,
    KEYCODE_A = 4,
    KEYCODE_B,
    KEYCODE_C,
    KEYCODE_D,
    KEYCODE_E,
    KEYCODE_F,
    KEYCODE_G,
    KEYCODE_H,
    KEYCODE_I,
    KEYCODE_J,
    KEYCODE_K,
    KEYCODE_L,
    KEYCODE_M,  
    KEYCODE_N,
    KEYCODE_O,
    KEYCODE_P,
    KEYCODE_Q,
    KEYCODE_R,
    KEYCODE_S,
    KEYCODE_T,
    KEYCODE_U,
    KEYCODE_V,
    KEYCODE_W,
    KEYCODE_X,
    KEYCODE_Y,
    KEYCODE_Z,
    KEYCODE_1,
    KEYCODE_2,
    KEYCODE_3,
    KEYCODE_4,
    KEYCODE_5,
    KEYCODE_6,
    KEYCODE_7,
    KEYCODE_8,
    KEYCODE_9,
    KEYCODE_0,
    KEYCODE_RETURN,
    KEYCODE_ESCAPE,
    KEYCODE_BACKSPACE,
    KEYCODE_TAB,
    KEYCODE_SPACE,
    KEYCODE_ARROW_RIGHT = 79,
    KEYCODE_ARROW_LEFT,
    KEYCODE_ARROW_DOWN,
    KEYCODE_ARROW_UP
} Keycodes;


typedef struct {
    Uint32 timestamp;
    keyState state;
    bool repeated;
    Uint16 keycode;
    char character[20];
} KeyboardEvent;

typedef struct {
    Uint32 timestamp;
    Uint8 button;
    keyState state;
    Uint8 clicks;
    Sint32 x;
    Sint32 y;
} MouseBtnEvent;

typedef struct {
    Uint32 timestamp;
    Uint32 button_state;
    Sint32 x;
    Sint32 y;
    Sint32 xrel;       
    Sint32 yrel;
}MouseMoveEvent;

typedef struct {
    Uint32 timestamp;
    float X_horizantal;
    float Y_vertical;
} MouseWheelEvent;

typedef struct {
    MouseEventType type;
    union {
        MouseBtnEvent btn;
        MouseMoveEvent move;
        MouseWheelEvent wheel;
    };
} MouseEvent;

typedef struct {
    Uint32 formatcode;
    int width;
    int height;
    Uint8 bytes_per_pixel;
    int pitch;
    void* pixels;
    void* internal_;
} Texture;




typedef struct {
    bool mouse_eventhandler_enabled;
    bool keyboard_eventhandler_enabled;
    void (*mouse_eventhandler) (MouseEvent* ,void*);
    void (*keyboard_eventhandler) (KeyboardEvent*, void*);
    void (*app_quit) (void*);
} EventHandler;

typedef struct {
    int draw_w;
    int draw_h;
    Uint32 draw_color;
} Drawstate;

typedef struct {
  int x;
  int y;  
} Vector;

typedef struct {
    float x;
    float y;
} fVector;

typedef struct{
    Vector origin;
    int w, h;
} Rectangle;

typedef struct{
    fVector origin;
    float w, h;
} fRectangle;

typedef struct{
    Uint8 R;
    Uint8 G;
    Uint8 B;
    Uint8 A;
} Color;

int initialize();
int create_window(const char *title, int w, int h);
void delay(int ms);
int setDrawColor(Uint32 rgba);
int setRenderScale(float x_scale, float y_scale);
Drawstate getDrawState();
int clearScreen();

int drawPoint(Vector c);
int drawPointF(fVector p);
int drawPoints(const Vector *cords, int count);
int drawPointsF(const fVector *points, int count);

int drawRectangle(const Rectangle *rect);
int drawRectangleF(const fRectangle *rect);
int drawLine(Vector c0, Vector c1);
int drawLineF(fVector c0, fVector c1);
int fillRectangle(const Rectangle *rect);
int fillRectangleF(const fRectangle *rect);
int draw_and_fill_rectangle(const Rectangle *rect);

void destroyTexture(Texture *txt);
void *safeAccessTexturePixel(Texture *txt, unsigned int x_pixel, unsigned int y_pixel);
int draw_and_fill_rectangleF(const fRectangle *rect);
int createTexture(const char *file, Texture *text);
int drawTexture(Texture *text, Rectangle *rect);
int updateTexture(Texture *txt);
int drawTextureNative(Texture *txt, Vector origin);

void presentRender();
void createCoord(Vector *coord, int x, int y);
void createRectangle(Rectangle *rect, Vector origin, int width, int height);
int addKeyboardEventhandler(void (*fun_ptr)(KeyboardEvent *, void *));
int addMouseEventHandler(void (*fun_ptr)(MouseEvent *, void *));
int eventDequeue(void *data);

Color colorHexToStruct(Uint32 rgbaHEX);
Uint32 colorStructToHex(Color color);

#endif