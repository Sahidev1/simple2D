#include "../graphics.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_image.h>



#include <stdio.h> //for debugging

typedef enum {
    QUIT = SDL_QUIT,
    KEY_PRESSED = SDL_KEYDOWN,
    KEY_RELEASED,
    KEYB_TEXT_INPUT = SDL_TEXTINPUT,

    MOUSE_MOVE = SDL_MOUSEMOTION,
    MOUSE_BUTTON_PRESSED,
    MOUSE_BUTTON_RELEASED,
    MOUSE_WHEEL_MOVED
} EventType;

typedef struct {
    SDL_Texture* texture;
    SDL_Surface* surface;
} internal_texture_data;



static SDL_Window* g_WINDOW;
static SDL_Renderer* g_RENDERER;
Drawstate g_drawstate;
static EventHandler evhData;
static EventHandler* g_evh = &evhData;

void handle_quit_signal(void*){
    SDL_DestroyRenderer(g_RENDERER);
    SDL_DestroyWindow(g_WINDOW);
    SDL_Quit();
    exit(0);
}

// iniatilizes the underlying library and datastructures, must be called first
int initialize(){
    g_evh->keyboard_eventhandler = FALSE;
    g_evh->mouse_eventhandler = FALSE;
    g_evh->app_quit = handle_quit_signal;
    return SDL_Init(SDL_INIT_VIDEO|SDL_INIT_TIMER|SDL_INIT_EVENTS) != 0 ? ERROR_INITIALIZE : 0;
}



int setDrawColor (Uint32 rgba){
    if(SDL_SetRenderDrawColor(g_RENDERER, rgba&0xFF, (rgba>>8)&0xFF, (rgba>>16)&0xFF, rgba>>24) != 0){
        return ERROR_SET_DRAW_COLOR;
    }
    g_drawstate.draw_color = rgba;
    return 0;
}

int setRenderScale(float x_scale, float y_scale){
    if (SDL_RenderSetScale(g_RENDERER, x_scale, y_scale) != 0) return UNSPECIFIED_ERROR;
    return 0;
}

Drawstate getDrawState(){
    return g_drawstate;
}

int clearScreen(){
    if (SDL_RenderClear(g_RENDERER) != 0) return ERROR_CLEAR_SCREEN;
    return 0;
}

// lets quit, mouse, keyboard events pass through from SDL
static int eventFilter(void* userdata, SDL_Event *event){
    Uint32 etypeCode = event->type>>8;
    if (etypeCode <= 0x4 && (etypeCode&0xd) != 0) return 1;
    return 0;
}



static int EventQueueFilter (void* userdata, SDL_Event *event, void* data){
    EventHandler* eh = (EventHandler*) userdata;
    
    if (event->type == QUIT) {
        if (eh->app_quit != NULL) eh->app_quit(NULL);
    }

    if (eh->keyboard_eventhandler_enabled && eh->keyboard_eventhandler != NULL){
        if (event->type == KEY_PRESSED || event->type == KEY_RELEASED){
            KeyboardEvent ke = {
                .timestamp = event->key.timestamp,
                .state = event->key.state,
                .repeated = event->key.repeat == 0 ? FALSE : TRUE,
                .keycode = event->key.keysym.scancode
            };
            memcpy(&ke.character , SDL_GetKeyName(event->key.keysym.sym), 20);
            eh->keyboard_eventhandler(&ke, data);
        }   
    }

/*
    if (event->type == MOUSE_MOVE || event->type == MOUSE_BUTTON_PRESSED || event->type == MOUSE_BUTTON_RELEASED || event->type == MOUSE_WHEEL_MOVED){
        if (eh->mouse_eventhandler_enabled && eh->mouse_eventhandler != NULL){
            eh->mouse_eventhandler(event);

        }
    }
*/
    if(eh->mouse_eventhandler_enabled && eh->mouse_eventhandler != NULL){
        if(event->type == MOUSE_BUTTON_PRESSED || event->type == MOUSE_BUTTON_RELEASED){
            MouseEvent me = {.type = BUTTON, .btn = {.button = event->button.button, .clicks = event->button.clicks,
            .state = event->button.state, .timestamp = event->button.timestamp, .x = event->button.x, .y = event->button.y }};
            eh->mouse_eventhandler(&me, data);
        }

        if (event->type = MOUSE_MOVE){
            MouseEvent me = {
                .type = MOVEMENT,
                .move = {
                    .button_state = event->motion.state,
                    .timestamp = event->motion.timestamp,
                    .x = event->motion.x,
                    .y = event->motion.y,
                    .xrel = event->motion.xrel,
                    .yrel = event->motion.yrel
                }
            };
            eh->mouse_eventhandler(&me, data);
        }
        
        if(event->type = MOUSE_WHEEL_MOVED){
            MouseEvent me = {
                .type = WHEEL,
                .wheel = {
                    .timestamp = event->wheel.timestamp,
                    .X_horizantal = event->wheel.preciseX,
                    .Y_vertical = event->wheel.preciseY
                }
            };
            eh->mouse_eventhandler(&me, data);
        }
        
    }


    return 1;
}



int eventDequeue(void* data){
    SDL_Event event;
    int status = SDL_PollEvent(&event);
    if (status == 0) return 0;
    else {
        return EventQueueFilter(g_evh, &event, data);
    }
}

int create_window(const char *title, int w, int h){
    int code = 0;
    Uint32 flags = 0;
    g_WINDOW = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        w, h, flags);

    if (g_WINDOW == NULL) return ERROR_CREATE_WINDOW;
    SDL_SetEventFilter(eventFilter, NULL);

    SDL_GetWindowSize(g_WINDOW, &g_drawstate.draw_w, &g_drawstate.draw_h);
    
    g_RENDERER = SDL_CreateRenderer(g_WINDOW,0, 0);
    if (g_RENDERER == NULL) return ERROR_CREATE_RENDERER;

    if ((code = setDrawColor(DRAW_COLOR_DEFAULT)) != 0) return code;

    if((code = SDL_RenderClear(g_RENDERER)) != 0) return code;
    SDL_RenderPresent(g_RENDERER);
    return 0;
}



int drawPoint(Vector p){
    return SDL_RenderDrawPoint(g_RENDERER, p.x, p.y ) != 0 ? ERROR_DRAW_COORD : 0;
}

int drawPointF(fVector p){
    return SDL_RenderDrawPointF(g_RENDERER, p.x, p.y ) != 0 ? ERROR_DRAW_COORD : 0;
}

int drawPoints(const Vector *points, int count){
    return SDL_RenderDrawPoints(g_RENDERER, (SDL_Point *) points, count) != 0 ? ERROR_DRAW_COORD : 0;
}

int drawPointsF(const fVector *points, int count){
    return SDL_RenderDrawPointsF(g_RENDERER, (SDL_FPoint *) points, count) != 0 ? ERROR_DRAW_COORD : 0;
}

int  drawLine(Vector c0, Vector c1){
    return SDL_RenderDrawLine(g_RENDERER, c0.x, c0.y, c1.x, c1.y) !=0 ? ERROR_DRAW_LINE: 0;
}

int drawLineF(fVector c0, fVector c1){
    return SDL_RenderDrawLineF(g_RENDERER, c0.x, c0.y, c1.x, c1.y) !=0 ? ERROR_DRAW_LINE: 0;
}

void convert_rectange_SDL2(const Rectangle* rect, SDL_Rect* rect_sdl){
    rect_sdl->x = rect->origin.x;
    rect_sdl->y = rect->origin.y;
    rect_sdl->w = rect->w;
    rect_sdl->h = rect->h;
} 

void convert_rectange_SDL2F(const fRectangle* rect, SDL_FRect* rect_sdl){
    rect_sdl->x = rect->origin.x;
    rect_sdl->y = rect->origin.y;
    rect_sdl->w = rect->w;
    rect_sdl->h = rect->h;
}

int drawRectangle(const Rectangle* rect){
    SDL_Rect rect_sdl;
    convert_rectange_SDL2(rect, &rect_sdl);
    return SDL_RenderDrawRect(g_RENDERER, &rect_sdl) != 0 ? ERROR_DRAW_RECT : 0;
}

int drawRectangleF(const fRectangle* rect){
    SDL_FRect rect_sdl;
    convert_rectange_SDL2F(rect, &rect_sdl);
    return SDL_RenderDrawRectF(g_RENDERER, &rect_sdl) != 0 ? ERROR_DRAW_RECT : 0;
}

int fillRectangle(const Rectangle* rect){
    SDL_Rect rect_sdl;
    convert_rectange_SDL2(rect, &rect_sdl);
    return SDL_RenderFillRect(g_RENDERER, &rect_sdl) != 0 ? ERROR_RECT_FILL : 0;
}


int fillRectangleF(const fRectangle* rect){
    SDL_FRect rect_sdl;
    convert_rectange_SDL2F(rect, &rect_sdl);
    return SDL_RenderFillRectF(g_RENDERER, &rect_sdl) != 0 ? ERROR_RECT_FILL : 0;
}

int draw_and_fill_rectangle(const Rectangle* rect){
    int code = 0;
    if ((code = drawRectangle(rect)) != 0) return code;
    if ((code = fillRectangle(rect)) != 0) return code;
    return 0;
}

int draw_and_fill_rectangleF(const fRectangle* rect){
    int code = 0;
    if ((code = drawRectangleF(rect)) != 0) return code;
    if ((code = fillRectangleF(rect)) != 0) return code;
    return 0;
}


int createTexture(const char *file, Texture* text){
    SDL_Surface* surf = IMG_Load(file);
    if (surf == NULL) return ERROR_CREATE_TEXTURE;


    SDL_Surface* converted_surf = SDL_ConvertSurfaceFormat(surf, SDL_PIXELFORMAT_RGBA32, 0);
    if (converted_surf == NULL) {
        SDL_FreeSurface(surf);
        return ERROR_CREATE_TEXTURE;
    }

    text->formatcode = converted_surf->format->format;
    text->pixels = converted_surf->pixels;
    text->pitch = converted_surf->pitch;
    text->width = converted_surf->w;
    text->height = converted_surf->h;
    text->bytes_per_pixel = converted_surf->format->BytesPerPixel;

    SDL_FreeSurface(surf);

    SDL_Texture* texture = SDL_CreateTextureFromSurface(g_RENDERER, converted_surf);

    if (texture == NULL) return ERROR_CREATE_TEXTURE;

    internal_texture_data* idata = malloc(sizeof(internal_texture_data));
    idata->texture = texture;
    idata->surface = converted_surf;
    text->internal_ = (void*) idata;
    return 0;
}

void destroyTexture(Texture* txt){
    SDL_DestroyTexture(((internal_texture_data*)txt->internal_)->texture);
    SDL_FreeSurface(((internal_texture_data*)txt->internal_)->surface);
    free((internal_texture_data*)txt->internal_);
    txt->internal_ = NULL;
    txt->pixels = NULL;
}

void* safeAccessTexturePixel(Texture* txt, unsigned int x_pixel, unsigned int y_pixel){
    if (txt->internal_ == NULL) return NULL;
    internal_texture_data* idata = (internal_texture_data*) txt->internal_;
    if (x_pixel >= txt->width || y_pixel >= txt->height) return NULL;
    //pitch = width * bytes_per_pixel + (POTENTIAL PADDING)
    return txt->pixels + (y_pixel * txt->pitch) + x_pixel*txt->bytes_per_pixel;
}



int drawTexture(Texture* txt, Rectangle* rect){
    if (txt->internal_ == NULL) return ERROR_DRAW_TEXTURE;
    SDL_Texture* text = ((internal_texture_data*)txt->internal_)->texture;
    SDL_Rect sdlRect;
    convert_rectange_SDL2(rect, &sdlRect);
    return SDL_RenderCopy(g_RENDERER, text, NULL, &sdlRect);
}

int updateTexture(Texture* txt){
    internal_texture_data* idata = (internal_texture_data*) txt->internal_;
    SDL_Texture* newText = SDL_CreateTextureFromSurface(g_RENDERER,idata->surface);
    if (newText == NULL) return ERROR_CREATE_TEXTURE;
    SDL_DestroyTexture(idata->texture);
    idata->texture = newText;
    return 0;
}

int drawTextureNative(Texture* txt, Vector origin){
    Rectangle rect = {.origin = origin, .w=txt->width, .h=txt->height};
    return drawTexture(txt, &rect);
}

void presentRender (){
    return SDL_RenderPresent(g_RENDERER);
}

void createCoord(Vector* coord, int x, int y){
    coord->x = x;
    coord->y = y;
}

void createRectangle(Rectangle* rect, Vector origin, int width, int height){
    rect->origin.x = origin.x;
    rect->origin.y = origin.y;
    rect->w = width;
    rect->h = height;
}

int addKeyboardEventhandler(void (*fun_ptr)(KeyboardEvent*, void*)){
    g_evh->keyboard_eventhandler_enabled = TRUE;
    g_evh->keyboard_eventhandler = fun_ptr;
}

int addMouseEventHandler(void (*fun_ptr) (MouseEvent*, void*)){
    g_evh->mouse_eventhandler_enabled = TRUE;
    g_evh->mouse_eventhandler = fun_ptr;
}

Color colorHexToStruct (Uint32 rgbaHEX){
    Color color;
    color.R = (Uint8) rgbaHEX;
    color.G = (Uint8) (rgbaHEX>>8);
    color.B = (Uint8) (rgbaHEX>>16);
    color.A = (Uint8) (rgbaHEX>>24);
    return color;
}

Uint32 colorStructToHex (Color color){
    return color.R | (color.G<<8) | (color.B<<16) | (color.A<<24);
}


void delay(int ms){
    SDL_Delay(ms);
}
