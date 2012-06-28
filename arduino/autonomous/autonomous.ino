#include "../ledbar.h"

#define CH 3

#define TLCCH(tlc_num, ch_num) (ch_num) /* FIXME */

unsigned int xr1 = 19543;

int cpin[][CH] = {
  {TLCCH(0, 2), TLCCH(0, 1), TLCCH(0, 0)},
  {TLCCH(0, 5), TLCCH(0, 4), TLCCH(0, 3)},
  {TLCCH(0, 8), TLCCH(0, 7), TLCCH(0, 6)},
  {TLCCH(0, 12), TLCCH(0, 11),TLCCH(0, 10)},
  {TLCCH(0, 15),TLCCH(0, 14),TLCCH(0, 13)},

#if 0
  {TLCCH(1, 2), TLCCH(1, 1), TLCCH(1, 0)},
  {TLCCH(1, 5), TLCCH(1, 4), TLCCH(1, 3)},
  {TLCCH(1, 8), TLCCH(1, 7), TLCCH(1, 6)},
  {TLCCH(1, 12), TLCCH(1, 11), TLCCH(1, 10)},
  {TLCCH(1, 15), TLCCH(1, 14), TLCCH(1, 13)},
#endif
};
#define cpinsets (sizeof(cpin)/sizeof(cpin[0]))

/* cca 2.7ohm resistor per channel */
int cmax[cpinsets][CH] = {
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 240, 230 },
  { 100, 230, 188 },
};
int c[cpinsets][CH];

int wait = 10;

void setup()
{
  Serial.begin(9600);
  Ledbar.begin(B1100000);
  int i = 0, led = 0;
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      c[led][i] = cmax[led][i] / 2;
      Ledbar.setPinMode(cpin[led][i], LPM_PWM);
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
    if (c[led][i] < 0) { c[led][i] = 0; g[led][i] = -g[led][i] + r(maxbounce)-maxbounce/2; } else if (c[led][i] > cmax[led][i]) { c[led][i] = cmax[led][i]; g[led][i] = -g[led][i] + r(maxbounce)-maxbounce/2; }
  }
}

void rainbow(int led)
{
  static int huephases[cpinsets];
  static int huephases_i[cpinsets];
#define HUEPHASE_LEN 128//512

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
  
#define huephase_to_c_inc(cc) (uint32_t) huephase_i * cmax[led][cc] / HUEPHASE_LEN
#define huephase_to_c_dec(cc) (cmax[led][cc] - (uint32_t) huephase_i * cmax[led][cc] / HUEPHASE_LEN)
  switch (huephase) {
    case 0: c[led][0] = cmax[led][0]; c[led][1] = huephase_to_c_inc(1); c[led][2] = 0; break;
    case 1: c[led][0] = huephase_to_c_dec(0); c[led][1] = cmax[led][1]; c[led][2] = 0; break;
    case 2: c[led][0] = 0; c[led][1] = cmax[led][1]; c[led][2] = huephase_to_c_inc(2); break;
    case 3: c[led][0] = 0; c[led][1] = huephase_to_c_dec(1); c[led][2] = cmax[led][2]; break;
    case 4: c[led][0] = huephase_to_c_inc(0); c[led][1] = 0; c[led][2] = cmax[led][2]; break;
    case 5: c[led][0] = cmax[led][0]; c[led][1] = 0; c[led][2] = huephase_to_c_dec(2); break;
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
    c[led][i] = mask & (1 << i) ? cmax[led][i] : 0;
  }
}


void custom(int led)
{
  long red =   100 - abs (led-9) * 10;
  long green = 30;
  long blue =  100 - abs(led-1) * 10;
  
  c[led][0] = red  * cmax[led][0] / 100;
  c[led][1] = green* cmax[led][1] / 100;
  c[led][2] = blue * cmax[led][2] / 100;
}


/* White "breathing" effect to a certain degree of intensity. Good for identifying a point where further intensity change does not make any difference. */
void grey(int led)
{
  static const int steps = 20;
  static int s = 0;
  static int d = 1;

  int i;
  for (i = 0; i < CH; i++) {
    c[led][i] = (uint32_t) cmax[led][i] * s / steps;
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

  int i;
  for (i = 0; i < CH; i++) {
    for (led = 0; led < cpinsets; led++) {
      //Serial.print(cpin[led][i], DEC); Serial.print("="); Serial.print(c[led][i], DEC); Serial.print("/"); Serial.print(cmax[led][i], DEC); Serial.print(" ");
      Ledbar.setPinPWM(cpin[led][i], c[led][i]);
    }
  }
  //Serial.print(NUM_TLCS, DEC);
  //Serial.println();

  delay(wait);
}
