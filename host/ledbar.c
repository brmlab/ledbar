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
#include <time.h>
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
    static float dir = 0.05;
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
    static const char *str = "xxx  x  x  x    x  xxx  x    xxx  xxx    x  xxx  x  x    x  xxx    xxx  x  x  x        "
                             "x  xxx  x    x  x  xxx    x  xxx  x  x    x    xxx  xxx  x  x        ";
    static int len;
    len = strlen(str);

    int t1 = t/10;
    int t2 = t%10;
    *r = 0;
    *b = 0;
    int c1 = str[(i+t1)%len] == 'x' ? 1 : 0;
    int c2 = str[(i+t1+1)%len] == 'x' ? 1 : 0;
    switch (2*c1+c2) {
        case 0: *g = 0; break;
        case 1: *g = t2/10.0; break;
        case 2: *g = 1-t2/10.0; break;
        case 3: *g = 1; break;
    }
}

// MiniPOV brmlab
void programY(int i, int t, double *r, double *g, double *b)
{

    static const char *bm[] = {
        "x             x     x  ",
        "xx   x   x x  x  xx xx ",
        "x x x x x x x x x x x x",
        "xx  x   x x x x  xx xx "
    };
    t = t/10 % 4;
    *r = 0;
    *g = t<4 && i<23 && bm[t][i]=='x' ? 1 : 0;
    *b = 0;
}

// clock - binary
void programU(int i, int t, double *r, double *g, double *b)
{
    int h, m, s;
    t = time(NULL) % 86400;
    h = t/3600;
    m = t/60%60;
    s = t%60;
    *r = *g = *b = 0;
    if (i == 0 || i == 6 || i == 7 || i == 14 || i == 15 || i == 22) *r = *g = *b = 1;
    else if (i <  6) *r = (h&(1<<( 5-i))) ? 1 : 0;
    else if (i < 14) *g = (m&(1<<(13-i))) ? 1 : 0;
    else if (i < 22) *b = (s&(1<<(21-i))) ? 1 : 0;
}

// clock - progressbar
void programI(int i, int t, double *r, double *g, double *b)
{
    double h, m, s;
    t = time(NULL) % 86400;
    h = 7.0*(t/3600)/24 - i;
    m = 8.0*(t/60%60)/60 - (i-7);
    s = 8.0*(t%60)/60 - (i-15);
    if (h<0) h = 0; else if (h>1) h = 1;
    if (m<0) m = 0; else if (m>1) m = 1;
    if (s<0) s = 0; else if (s>1) s = 1;
    *r = h;
    *g = i>=7 ? m : 0;
    *b = i>=15 ? s : 0;
}

// random moving lights
void programO(int i, int t, double *r, double *g, double *b)
{
#define PERIOD 1200
#define MAX_DIST 9
    t += PERIOD;
    unsigned int rnd, seed;
    int t1;
    double l, sel, dist, dir;

    t1 = t/PERIOD;
    seed = t1*3;
    rnd = rand_r(&seed) % BOXES;
    dir = (rand_r(&seed) % 1000)/10.0 - 50;
    sel = fmod(rnd+t/(PERIOD/dir), BOXES);
    if (sel < 0) sel += BOXES;
    l = 1-abs((t-t1*PERIOD)-PERIOD/2)/((double) PERIOD/2);
    l *= 3;
    if (l > 1) l = 1;
    dist = sel-i;
    if (dist < 0) dist = dist+BOXES;
    dist = fmin(dist, -dist+BOXES);
    l *= (double)(MAX_DIST-dist)/MAX_DIST;
    if (l < 0) l = 0;
    *r = l;

    t1 = (t-PERIOD/3)/PERIOD;
    seed = t1*3+1;
    rnd = rand_r(&seed) % BOXES;
    dir = (rand_r(&seed) % 1000)/10.0 - 50;
    sel = fmod(rnd+t/(PERIOD/dir), BOXES);
    if (sel < 0) sel += BOXES;
    l = 1-abs(fmod(t-PERIOD/3, PERIOD)-PERIOD/2)/((double) PERIOD/2);
    l *= 3;
    if (l > 1) l = 1;
    dist = sel-i;
    if (dist < 0) dist = dist+BOXES;
    dist = fmin(dist, -dist+BOXES);
    l *= (double)(MAX_DIST-dist)/MAX_DIST;
    if (l < 0) l = 0;
    *g = l;

    t1 = (t-PERIOD*2/3)/PERIOD;
    seed = t1*3+2;
    rnd = rand_r(&seed) % BOXES;
    dir = (rand_r(&seed) % 1000)/10.0 - 50;
    sel = fmod(rnd+t/(PERIOD/dir), BOXES);
    if (sel < 0) sel += BOXES;
    l = 1-abs(fmod(t-PERIOD*2/3, PERIOD)-PERIOD/2)/((double) PERIOD/2);
    l *= 3;
    if (l > 1) l = 1;
    dist = sel-i;
    if (dist < 0) dist = dist+BOXES;
    dist = fmin(dist, -dist+BOXES);
    l *= (double)(MAX_DIST-dist)/MAX_DIST;
    if (l < 0) l = 0;
    *b = l;
#undef PERIOD
#undef MAX_DIST
}

// alternative rainbow
void programP(int i, int t, double *r, double *g, double *b)
{
    double index = (double) i/BOXES;
    double time = 0.01*t;
    double phi = 6*index+time;
    int phase = ((int) phi) % 6;
    double part = fmod(phi, 1.0);
    double inc = part;
    double dec = 1-part;
    switch (phase) {
        case 0: *r=1;   *g=inc; *b=0;   break;
        case 1: *r=dec; *g=1;   *b=0;   break;
        case 2: *r=0;   *g=1;   *b=inc; break;
        case 3: *r=0;   *g=dec; *b=1;   break;
        case 4: *r=inc; *g=0;   *b=1;   break;
        case 5: *r=1;   *g=0;   *b=dec; break;
    }
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
    int lastUpdate = 0;
    int sleep;

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
                        case SDLK_y: program = programY; break;
                        case SDLK_u: program = programU; break;
                        case SDLK_i: program = programI; break;
                        case SDLK_o: program = programO; break;
                        case SDLK_p: program = programP; break;
                        case SDLK_ESCAPE: quit = 1; break;
                        default: break;
                    }
                    break;
            }
        }
        sleep = 25 - (SDL_GetTicks() - lastUpdate);
        if (sleep > 0) {
            SDL_Delay(sleep);
        }
        lastUpdate += 25;
    }
    SDL_Quit();
    return 0;
}
