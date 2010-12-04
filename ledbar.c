/*
 * Copyright (c) 2010 Pavol Rusnak <stick@gk2.sk>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <math.h>
#include <SDL.h>

#define min(x,y) ( (x)<(y) ? (x) : (y) )
#define max(x,y) ( (x)>(y) ? (x) : (y) )

#define DEBUG 1

#define RESX 800
#define RESY 600
#define BPP 32
#define BOXES 23

SDL_Rect rects[BOXES];

void (*program)(int i, int t, double *r, double *g, double *b);

// rainbow
void programQ(int i, int t, double *r, double *g, double *b)
{
    double index = 1.0*i/BOXES;
    double time = 0.01*t;
    *r = (sin(M_PI*2*index+time)+1.0)/2;
    *g = (sin(M_PI*2*index+time+M_PI*2/3)+1.0)/2;
    *b = (sin(M_PI*2*index+time+M_PI*4/3)+1.0)/2;
}

// rainbow with different speeds
void programW(int i, int t, double *r, double *g, double *b)
{
    double index = 1.0*i/BOXES;
    double time = 0.01*t;
    *r = (sin(M_PI*2*index+time)+1.0)/2;
    *g = (sin(M_PI*2*index+time/1.618)+1.0)/2;
    *b = (sin(M_PI*2*index+time*1.618)+1.0)/2;
}

// knight rider
void programE(int i, int t, double *r, double *g, double *b)
{
    static float pos = BOXES/2;
    static float dir = 0.01;
    *r = max(1 - fabsl((pos-i)/BOXES)*4, 0);
    *g = 0;
    *b = 0;
    pos += dir;
    if (pos < 0 || pos > BOXES) dir = -dir;
}

// binary
void programR(int i, int t, double *r, double *g, double *b)
{
    t /= 10;
    *r = 0;
    *g = t & (1<<(BOXES-i-1)) ? 1 : 0;
    *b = 0;
}

// morse code
void programT(int i, int t, double *r, double *g, double *b)
{
// . -> 'x'   - -> 'xxx'   interelement -> ' '
// interletter -> '   '    interword -> '       '
//    static const char *str = "brmlab rulez ";
    static const char *str = "xxx x x x   x xxx x   xxx xxx   x xxx x x   x xxx   xxx x x x       x xxx x   x x xxx   x xxx x x   x   xxx xxx x x       ";
    static int len;
    len = strlen(str);

    t /= 50;
    *r = 0;
    *g = str[(i+t)%len]!=' ' ? 1 : 0;
    *b = 0;
}

void drawScreen(SDL_Surface* screen, int t)
{
    int i;
    double r, g, b;

    if(SDL_MUSTLOCK(screen) && SDL_LockSurface(screen) < 0) return;

    for (i=0; i<BOXES; ++i) {
        program(i, t, &r, &g, &b);
#ifdef DEBUG
        printf("%2d %d %f %f %f\n", i, t, r, g, b);
#endif
        SDL_FillRect(screen, &rects[i], SDL_MapRGB(screen->format, r*255, g*255, b*255));
    }

    if (SDL_MUSTLOCK(screen)) SDL_UnlockSurface(screen);
    SDL_Flip(screen);
}


int main(int argc, char* argv[])
{
    SDL_Surface *screen;
    SDL_Event event;
    int quit = 0;
    int t = 0;
    int size;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (!(screen = SDL_SetVideoMode(RESX, RESY, BPP, SDL_HWSURFACE))) {
        SDL_Quit();
        return 1;
    }
    size = (RESX-(BOXES+1)*4)/BOXES;
    for (t=0; t<BOXES; ++t) {
        rects[t].h = size;
        rects[t].w = size;
        rects[t].x = t*(size+4) + (RESX-(size+4)*BOXES+4)/2;
        rects[t].y = (RESY-size)/2;
    }
    program = programQ;
    while (!quit) {
        drawScreen(screen,t++);
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    quit = 1; break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.sym) {
                        case SDLK_q: program = programQ; break;
                        case SDLK_w: program = programW; break;
                        case SDLK_e: program = programE; break;
                        case SDLK_r: program = programR; break;
                        case SDLK_t: program = programT; break;
                        case SDLK_ESCAPE: quit = 1; break;
                        default: break;
                    }
                    break;
            }
        }
    }
    SDL_Quit();
    return 0;
}
