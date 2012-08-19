#include "../ledbar.h"

/* FIXME: cmin[] is not handled properly here, it's the level of 1, not 0. */

class Ledbar lb[NUM_TLCS];
int c[cpinsets][CH];

int wait = 10;

unsigned int xr1 = 19543;

void setup()
{
  Serial.begin(9600);
  int i = 0, led = 0;
  for (i = 0; i < NUM_TLCS; i++)
    lb[i].begin(B1100000 | i);
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      c[led][i] = map(5, 0, 10, cmin[led][i], cmax[led][i]);
      lb[cpin[led][i] >> 4].setPinMode(cpin[led][i] & 0xf, LPM_PWM);
    }
  }
  xr1 += analogRead(0);
}

int r(int ceiling)
{
  xr1 = 16807 * (xr1 & 0xfff) + (xr1 >> 12);
  return xr1 % ceiling;
}

/* One iteration of random colorspace walk. */
void random_walk(int led)
{
  static const int maxstep = 2;
  static const int maxbounce = maxstep * 2;
  static const int maxgrad = 16;
  static const int cmaxgrad[CH] = {maxgrad, maxgrad, maxgrad};
  static const int dampening = 8; // less means tend to smaller gradient
  static int g[cpinsets][CH];

  int i;

  for (i = 0; i < CH; i++) {
    g[led][i] += r(maxstep) * (r(2) ? 1 : -1);
    /* dampening */ g[led][i] += (g[led][i] > 0 ? -1 : 1) * r(abs(g[led][i])) / dampening;
    if (g[led][i] < -cmaxgrad[i]) g[led][i] = -cmaxgrad[i] + r(maxbounce); else if (g[led][i] > cmaxgrad[i]) g[led][i] = cmaxgrad[i] - r(maxbounce);

    c[led][i] += g[led][i];

    /* bounce */
    if (c[led][i] < cmin[led][i]) {
      c[led][i] = cmin[led][i];
      g[led][i] = -g[led][i] + r(maxbounce)-maxbounce/2;
    } else if (c[led][i] > cmax[led][i]) {
      c[led][i] = cmax[led][i];
      g[led][i] = -g[led][i] + r(maxbounce)-maxbounce/2;
    }
  }
}

void rainbow(int led)
{
  static int huephases[cpinsets];
  static int huephases_i[cpinsets];
#define HUEPHASE_LEN 512

  static int ini;
  if (!ini) {
    ini = 1;
    int v_max = 6 * HUEPHASE_LEN;
    for (int l = 0; l < cpinsets; l++) {
      int i = v_max * l / cpinsets;
      huephases[l] = i / HUEPHASE_LEN;
      huephases_i[l] = i % HUEPHASE_LEN;
    }
  }

  { int huephase = huephases[led], huephase_i = huephases_i[led];
  
#define huephase_to_c_inc(cc) map(huephase_i, 0, HUEPHASE_LEN, cmin[led][cc], cmax[led][cc])
#define huephase_to_c_dec(cc) map(HUEPHASE_LEN - huephase_i, 0, HUEPHASE_LEN, cmin[led][cc], cmax[led][cc])
  switch (huephase) {
    case 0: c[led][0] = cmax[led][0]; c[led][1] = huephase_to_c_inc(1); c[led][2] = cmin[led][2]; break;
    case 1: c[led][0] = huephase_to_c_dec(0); c[led][1] = cmax[led][1]; c[led][2] = cmin[led][2]; break;
    case 2: c[led][0] = cmin[led][0]; c[led][1] = cmax[led][1]; c[led][2] = huephase_to_c_inc(2); break;
    case 3: c[led][0] = cmin[led][0]; c[led][1] = huephase_to_c_dec(1); c[led][2] = cmax[led][2]; break;
    case 4: c[led][0] = huephase_to_c_inc(0); c[led][1] = cmin[led][1]; c[led][2] = cmax[led][2]; break;
    case 5: c[led][0] = cmax[led][0]; c[led][1] = cmin[led][1]; c[led][2] = huephase_to_c_dec(2); break;
  }
  
  huephase_i++;
  if (huephase_i > HUEPHASE_LEN) {
    huephase_i = 0;
    huephase = (huephase + 1) % 6;
  }
  
  huephases[led] = huephase; huephases_i[led] = huephase_i;
  }
}

/* One iteration of constant brightest white (useful for tuning constants for particular LEDs). */
void white(int led)
{
  int i;
  int mask = 1|2|4; // R, G, B
  for (i = 0; i < CH; i++) {
    c[led][i] = mask & (1 << i) ? map(255, 0, 255, cmin[led][i], cmax[led][i]) : 0;
  }
}


void custom(int led)
{
  int mled = cpinsets - 1 - led;
  int red   = map( led, 0, cpinsets - 1, 0, 100);
  int green = map(mled, 0, cpinsets - 1, 0, 100);;
  int blue  = 0;
  
  c[led][0] = map(red, 0, 100, cmin[led][0], cmax[led][0]);
  c[led][1] = map(green, 0, 100, cmin[led][1], cmax[led][1]);
  c[led][2] = map(blue, 0, 100, cmin[led][2], cmax[led][2]);
}


/* White "breathing" effect to a certain degree of intensity. Good for identifying a point where further intensity change does not make any difference. */
void grey(int led)
{
  static const int steps = 1000;
  static int s = 0;
  static int d = 1;

  int i;
  for (i = 0; i < CH; i++) {
    c[led][i] = map(s, 0, steps, cmin[led][i], cmax[led][i]);
  }
  if (s == steps) {
    d = -1;
  } else if (s == 0) {
    d = 1;
  }
  s += d;
}

void loop()
{
  int led;
  for (led = 0; led < cpinsets; led++) {
    //random_walk(led);
    rainbow(led);
    //white(led);
    //custom(led);
    //grey(led);
  }

  setbyc(lb, c);

  delay(wait);
}
