#include "../../graphics.h"
#include <stdio.h>
#include <math.h>

#define WINDOW_W 1600
#define WINDOW_H 1024
#define MAX_N 100
#define BOUNDARY_SQR 4


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

void drawColor(Vector* v, Uint32 colorHex){
    S2D_setDrawColor(colorHex);
    S2D_drawPoint(*v);
    S2D_setDrawColor(DRAW_COLOR_TRANSPARENT);
}


void mandelBrotProc(float scaler, float boundary_sqr, int max_n){
    int count = 0;
    float v_r, v_z;
    float c_r, c_z;
    const int colorMult = 2;
    float absSqr = 0;
    Vector v;
    Color c = {.R = 0, .G = 0, .B = 0, .A = 255};
    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
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
            drawColor(&v, S2D_colorStructToHex(c));
        }
        
    }
    
}


int main(){
    S2D_initialize();
    S2D_createWindow("Fractals", WINDOW_W, WINDOW_H);
 
    int max_n = MAX_N;
    float boundary = 4.0f;
    S2D_setDrawColor(DRAW_COLOR_TRANSPARENT);
    S2D_clearScreen();
    mandelBrotProc(512, boundary, max_n);
    S2D_delay(1);
    S2D_presentRender();
    while (TRUE){
        S2D_eventDequeue(NULL);
    }

    return 0;
}