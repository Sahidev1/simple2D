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

// ERROR CODES
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
#define ERROR_SET_RENDER_SCALE (0xE)



#include <SDL2/SDL_stdinc.h>


//simple boolean type
typedef enum {FALSE, TRUE} bool;

// Keyboard Key state and mouse button state
typedef enum {RELEASED, PRESSED} keyState;
typedef enum keyState BtnState;

// Mouse event types
typedef enum  {BUTTON, MOVEMENT, WHEEL} MouseEventType;

// Keyboard keycodes
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


// String display direction
typedef enum {
    LTR = 0x1,
    RTL,
    TTB,
    BTT
}stringWriteDir;





/*
    Keyboard event structure
    timestamp: time of event
    state: key state (pressed or released)
    repeated: if the key is repeated, if it is held down 
    keycode: the key code
    character: Human readable description of the key pressed
*/
typedef struct {
    Uint32 timestamp;
    keyState state;
    bool repeated;
    Uint16 keycode;
    char character[20];
} KeyboardEvent;

/*
    Mouse button event structure
    timestamp: time of event
    button: the index of the button pressed
    state: the state of the button (pressed or released)
    clicks: number of clicks (1 for single click, 2 for double click)
    x: x coordinate of the mouse cursor
    y: y coordinate of the mouse cursor
*/
typedef struct {
    Uint32 timestamp;
    Uint8 button;
    keyState state;
    Uint8 clicks;
    Sint32 x;
    Sint32 y;
} MouseBtnEvent;

/*
    Mouse movement event structure
    timestamp: time of event
    button_state: the state of the mouse buttons
    x: x coordinate of the mouse cursor
    y: y coordinate of the mouse cursor
    xrel: relative movement along the x axis
    yrel: relative movement along the y axis
*/
typedef struct {
    Uint32 timestamp;
    Uint32 button_state;
    Sint32 x;
    Sint32 y;
    Sint32 xrel;       
    Sint32 yrel;
}MouseMoveEvent;

/*
    Mouse wheel event structure
    timestamp: time of event
    X_horizantal: horizontal movement of the wheel
    Y_vertical: vertical movement of the wheel
*/
typedef struct {
    Uint32 timestamp;
    float X_horizantal;
    float Y_vertical;
} MouseWheelEvent;


/*
    Mouse event structure
    type determines which access to the union is valid

    type: the type of the event
    btn: mouse button event
    move: mouse movement event
    wheel: mouse wheel event
*/
typedef struct {
    MouseEventType type;
    union {
        MouseBtnEvent btn;
        MouseMoveEvent move;
        MouseWheelEvent wheel;
    };
} MouseEvent;

/*
    Texture structure
    formatcode: the format of the texture
    width: the width of the texture
    height: the height of the texture
    bytes_per_pixel: the number of bytes per pixel
    pitch: the pitch of the texture
    pixels: the pixel data
    internal_: internal data, DO NOT OVERWRITE!
*/
typedef struct {
    Uint32 formatcode;
    int width;
    int height;
    Uint8 bytes_per_pixel;
    int pitch;
    void* pixels;
    void* internal_;
} Texture;



/*
    Event handler structure
    mouse_eventhandler_enabled: if the mouse event handler is enabled
    keyboard_eventhandler_enabled: if the keyboard event handler is enabled
    mouse_eventhandler: the mouse event handler function
    keyboard_eventhandler: the keyboard event handler function
    app_quit: the application quit function
*/
typedef struct {
    bool mouse_eventhandler_enabled;
    bool keyboard_eventhandler_enabled;
    void (*mouse_eventhandler) (MouseEvent* ,void*);
    void (*keyboard_eventhandler) (KeyboardEvent*, void*);
    void (*app_quit) (void*);
} EventHandler;

/*
    Drawstate structure
    draw_w: the width of the drawing area
    draw_h: the height of the drawing area
    draw_color: the current draw color
*/
typedef struct {
    int draw_w;
    int draw_h;
    Uint32 draw_color;
} Drawstate;

/*
    Vector structure
    Essentially a vector from top left corner of the screen as origin to the point (x,y)
*/
typedef struct {
  int x;
  int y;  
} Vector;

/* Floating point vector */
typedef struct {
    float x;
    float y;
} fVector;

/*
    Rectangle structure
    origin: A vector to the top left corner of the rectangle
    w: width of the rectangle
    h: height of the rectangle
*/
typedef struct{
    Vector origin;
    int w, h;
} Rectangle;

/* Floating point rectangle */
typedef struct{
    fVector origin;
    float w, h;
} fRectangle;

/*
    Color structure
    R: Red color component
    G: Green color component
    B: Blue color component
    A: Alpha color component

    Use the colorHexToStruct and colorStructToHex functions to convert between the two
*/
typedef struct{
    Uint8 R;
    Uint8 G;
    Uint8 B;
    Uint8 A;
} Color;

/*
    String render data structure
    font_fpath: the file path to the font file
    string_write_direction: the direction to write the string
    font_size: the size of the font
    string: the string to render
    foreground_color: the color of the string
    wrapLength: the length to wrap the, essentialy pixelwidth before newline
*/
typedef struct {
    char* font_fpath;
    stringWriteDir string_write_direction;
    int font_size;
    char* string;
    Color foreground_color;
    Uint32 wrapLength;
} StringRenderData;

/*
    Initialize the graphics library, this function must be called before any other function
    Returns 0 on success, error code ERROR_INITIALIZE on failure
*/
int initialize();

/*
    Create a window with the specified title, width and height
    Returns 0 on success, error code ERROR_CREATE_WINDOW on failure
*/
int create_window(const char *title, int w, int h);

/* cause a millisecond delay */
void delay(int ms);

/*
    Set the draw color to the specified color
    Any subsequent drawing function calls will use this color
    Returns 0 on success, error code ERROR_SET_DRAW_COLOR on failure
*/
int setDrawColor(Uint32 rgba);


/*
    Set the render scale to the specified scale
    x_scale: the x scale
    y_scale: the y scale
    Returns 0 on success, error code ERROR_SET_RENDER_SCALE on failure
*/
int setRenderScale(float x_scale, float y_scale);

/*
    Get the current draw state
    Returns the current draw state
*/
Drawstate getDrawState();

/*
    Clear the screen with the current draw color
    Returns 0 on success, error code ERROR_CLEAR_SCREEN on failure
*/
int clearScreen();

/*
    Draw a point at the specified coordinates
    c: the coordinates of the point
    Returns 0 on success, error code ERROR_DRAW_POINT on failure
*/
int drawPoint(Vector c);

/* 
    Draw point using float coordinates
    Returns 0 on success, error code ERROR_DRAW_POINT on failure
*/
int drawPointF(fVector p);

/*
    Draw a set of points
    cords: the coordinates of the points
    count: the number of points
    Returns 0 on success, error code ERROR_DRAW_POINT on failure
*/
int drawPoints(const Vector *cords, int count);

/*
    Draw a set of points using float coordinates
    points: the coordinates of the points
    count: the number of points
    Returns 0 on success, error code ERROR_DRAW_POINT on failure
*/
int drawPointsF(const fVector *points, int count);

/*
    Draw a rectangle
    rect: the rectangle to draw
    Returns 0 on success, error code ERROR_DRAW_RECT on failure
*/
int drawRectangle(const Rectangle *rect);

/*
    Draw a rectangle using float coordinates
    rect: the rectangle to draw
    Returns 0 on success, error code ERROR_DRAW_RECT on failure
*/
int drawRectangleF(const fRectangle *rect);

/*
    Draw a line from c0 to c1
    c0: the start point of the line
    c1: the end point of the line
    Returns 0 on success, error code ERROR_DRAW_LINE on failure
*/
int drawLine(Vector c0, Vector c1);

/*
    Draw a line from c0 to c1 using float vectors
    c0: the start point of the line
    c1: the end point of the line
    Returns 0 on success, error code ERROR_DRAW_LINE on failure
*/
int drawLineF(fVector c0, fVector c1);

/*
    Fill a rectangle with the current draw color
    rect: the rectangle to fill
    Returns 0 on success, error code ERROR_RECT_FILL on failure
*/
int fillRectangle(const Rectangle *rect);

/*
    Fill a rectangle with the current draw color using float coordinates
    rect: the rectangle to fill
    Returns 0 on success, error code ERROR_RECT_FILL on failure
*/
int fillRectangleF(const fRectangle *rect);

/*
    Draw and fill a rectangle with the current draw color
    On success, returns 0, otherwise returns an error code
*/
int draw_and_fill_rectangle(const Rectangle *rect);

/*
    Draw and fill a rectangle with the current draw color using float coordinates
    On success, returns 0, otherwise returns an error code
*/
int draw_and_fill_rectangleF(const fRectangle *rect);




/*
    Allows safe access to a texture pixel data in memory
    txt: the texture memory pixel data to access
    x_pixel: the x coordinate of the pixel
    y_pixel: the y coordinate of the pixel
    Returns a pointer to the pixel data on success, NULL on failure
*/
void *safeAccessTexturePixel(Texture *txt, unsigned int x_pixel, unsigned int y_pixel);


/*
    Create a texture from a file
    file: the file path of the image
    text: the texture to create
    Returns 0 on success, error code ERROR_CREATE_TEXTURE on failure
*/
int createTexture(const char *file, Texture *text);


/*
    Destroy a texture instance
*/
void destroyTexture(Texture *txt);

/*
    Draw a texture to the screen at the specified rectangle
    text: the texture to draw
    rect: the rectangle to draw the texture on
    Returns 0 on success, error code ERROR_DRAW_TEXTURE on failure
*/
int drawTexture(Texture *text, Rectangle *rect);

/*
    Draw a texture on a rectangle which is set to the textures native dimensions
    text: the texture to draw
    origin: Vector to the top left corner of the texture rectangle
*/
int drawTextureNative(Texture *txt, Vector origin);


/*
    Create a texture WITH UTF8 text from a string.
    Note that direction in the stringRenderData struct is ignored due to setting it to any value causes a bug, so for now only LTR render is supported
    txt: the texture to create
    d: the string render data
    Returns 0 on success, error code ERROR_CREATE_TEXTURE on failure
*/
int createUTF8Texture(Texture *txt, StringRenderData *d);


/*
    Set the string render data
    d: the string render data structure to set
    font_fpath: the file path to the font file
    direction: the direction to write the string
    font_size: the size of the font
    string: the string to render
    fg_color: the color of the string
    wraplength: the length to wrap the string, essentialy pixelwidth before newline
*/
void setStringRenderData(StringRenderData *d, char *font_fpath, stringWriteDir direction, int font_size, char *string, Color fg_color, Uint32 wraplength);

/*
    Update the texture with new pixel data Call this function after modifying the pixel data of a texture or the texture will not be updated
    txt: the texture to update
    Returns 0 on success, error code ERROR_DESTROYED_TEXTURE on failure
*/
int updateTexture(Texture *txt);

/*
    Present the render to the screen
    This function must be called to render the drawn objects and textures to the screen
*/
void presentRender();

/*
    Sets the vector to the specified coordinates
*/
void setCoord(Vector *coord, int x, int y);

/*
    Sets the rectangle to the specified coordinates and dimensions
*/
void setRectangle(Rectangle *rect, Vector origin, int width, int height);

/*
    Add a keyboard event handler function
    fun_ptr: the function pointer to the keyboard event handler
*/
void addKeyboardEventhandler(void (*fun_ptr)(KeyboardEvent *, void *));


/*
    Add a mouse event handler function
    fun_ptr: the function pointer to the mouse event handler
*/
void addMouseEventHandler(void (*fun_ptr)(MouseEvent *, void *));

/*
    Dequeues an event from the event queue that is then passed along to the corresponding event handler, can be looped to dequeue all events
    data: the data to pass along to the event handler
    Returns 1 if an event was dequeued, 0 otherwise
*/
int eventDequeue(void *data);


/*
    converts a hex rgba color code to a color struct
*/
Color colorHexToStruct(Uint32 rgbaHEX);

/*
    converts a color struct to a hex rgba color code
*/
Uint32 colorStructToHex(Color color);

#endif