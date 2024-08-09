#include "../../graphics.h"
#include <stdio.h>
#include <math.h>
#include <pthread.h>

/*
    Multithreaded Mandelbrot set generator
    default thread count is 4
    usage: ./mandelbrot <thread_count>
*/

#define WINDOW_W 3*512
#define WINDOW_H 1024
#define MAX_N 100
#define BOUNDARY_SQR 4
#define SCALER 512.0f

Uint32 g_pixData[WINDOW_W][WINDOW_H];

typedef struct {
    float scaler;
    float boundary_sqr;
    int max_n;
    int window_w;
    int window_h;
    int x_start_pos;
} ThreadData;

int x_transform(int x){
    return x - WINDOW_W / 2;
    
}

int y_transform(int y){
    return y- WINDOW_H/2;
}

void add(float c0_r, float c0_z, float* c1_r, float* c1_z){
    *c1_r  = c0_r + *c1_r;
    *c1_z = c0_z + *c1_z; 
}

void mul(float c0_r, float c0_z, float* c1_r, float* c1_z){
    float tempr = *c1_r, tempz = * c1_z; 
    *c1_r = c0_r* (*c1_r) - c0_z* (*c1_z);
    *c1_z = c0_r*tempz + c0_z*tempr;
}

void print_complex(float r, float z){
    printf("%f + %fi\n", r, z);
}

void mandelBrotFn(float *v_r, float *v_z, float c_r, float c_z){
    mul(*v_r, *v_z, v_r, v_z);
    add(c_r, c_z, v_r, v_z);
}


void mandelBrotProc(float scaler, float boundary_sqr, int max_n, int x_start_pos, int window_w, int window_h){
    int count = 0;
    float v_r, v_z;
    float c_r, c_z;
    const int colorMult = 2;
    float absSqr = 0;
    Vector v;
    Color c = {.R = 0, .G = 0, .B = 0, .A = 255};
    for (int i =x_start_pos; i < window_w; i++)
    {
        for (int j = 0; j < window_h; j++)
        {
            count = 0;
            v_r = 0.0f, v_z = 0.0f;
            c_r = ((float)x_transform(i))/scaler;
            c_z = ((float)y_transform(j))/scaler;
            while((absSqr = pow(v_r, 2) + pow(v_z, 2)) < boundary_sqr && count < max_n){
                mandelBrotFn(&v_r, &v_z, c_r, c_z);
                count++;
            }
            v.x = i, v.y = j;
            c.R = count * colorMult;
            c.G = (Uint8)2*absSqr;
            g_pixData[i][j] = S2D_colorStructToHex(c);
        }
        
    }
    
}

void drawPixels(int x_start_pos, int x_end_pos){
    for (int i = x_start_pos; i < x_end_pos; i++){
        for(int j = 0; j < WINDOW_H; j++){
            S2D_setDrawColor(g_pixData[i][j]);
            S2D_drawPoint((Vector){i,j});
        }
    }
}


void* thread_fun(void* params){
    ThreadData* d = (ThreadData*) params;
    mandelBrotProc(d->scaler, d->boundary_sqr, d->max_n, d->x_start_pos, d->window_w, d->window_h);
}




int main(int gc, char** gv){
    int threadCount = 4;
    S2D_initialize();
    S2D_createWindow("Fractals", WINDOW_W, WINDOW_H);
    if (gc > 1) {
        threadCount = atoi(gv[1]);
    }

    pthread_t threads[threadCount];
    ThreadData d[threadCount];

    int tick = S2D_getTicks();

    int x_segment_size = WINDOW_W / threadCount;
    int x_start_pos = 0;
    for (int i = 0; i < threadCount; i++){
        d[i].boundary_sqr = BOUNDARY_SQR;
        d[i].max_n = MAX_N;
        d[i].scaler = SCALER;
        d[i].x_start_pos = x_start_pos;
        d[i].window_w = x_start_pos + x_segment_size;
        d[i].window_h = WINDOW_H;
        x_start_pos += x_segment_size; 

        pthread_create(&threads[i], NULL, thread_fun, &d[i]);
    }

    for (int i = 0; i < threadCount; i++){
        pthread_join(threads[i], NULL);
        drawPixels(d[i].x_start_pos, d[i].window_w);
    }
    printf("Thread count: %d\n", threadCount);
    printf("Time taken: %d\n", S2D_getTicks() - tick);
    S2D_presentRender();
    while (TRUE){
        S2D_eventDequeue(NULL);
    }

    return 0;
}