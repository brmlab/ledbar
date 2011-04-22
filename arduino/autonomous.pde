#include "Tlc5940.h"

#define CH 3

#define TLCCH(tlc_num, ch_num) ((tlc_num)*16 + (ch_num))

unsigned int xr1 = 19543;
int c[CH];

//int cmax[CH] = { 2800, 4095, 3500 }; - max intensity
/* cca 2.7ohm resistor per channel */
int cmax[CH] = { 1600, 3900, 2400 }; // - same visual perception
int cpin[][CH] = {
  {TLCCH(0, 9), TLCCH(0, 10),TLCCH(0, 11)},
  {TLCCH(1, 9), TLCCH(1, 11),TLCCH(1, 10)},
  {TLCCH(1, 1), TLCCH(1, 2), TLCCH(1, 3)},
  {TLCCH(2, 12),TLCCH(2, 13),TLCCH(2, 14)},
  {TLCCH(3, 12),TLCCH(3, 13),TLCCH(3, 14)},
  {TLCCH(4, 12),TLCCH(4, 13),TLCCH(4, 14)},

  {TLCCH(0, 0), TLCCH(0, 1), TLCCH(0, 2)},
  {TLCCH(0, 3), TLCCH(0, 4), TLCCH(0, 5)},
  {TLCCH(0, 6), TLCCH(0, 7), TLCCH(0, 8)},
  {TLCCH(0, 12),TLCCH(0, 13),TLCCH(0, 14)},
  {TLCCH(1, 0), TLCCH(1, 4), TLCCH(1, 5)},
  {TLCCH(1, 6), TLCCH(1, 7), TLCCH(1, 8)},
  {TLCCH(1, 12),TLCCH(1, 13),TLCCH(1, 14)},
  {TLCCH(2, 0), TLCCH(2, 1), TLCCH(2, 2)},
  {TLCCH(2, 3), TLCCH(2, 4), TLCCH(2, 5)},
  {TLCCH(2, 6), TLCCH(2, 7), TLCCH(2, 8)},
  {TLCCH(2, 9), TLCCH(2, 10),TLCCH(2, 11)},
  {TLCCH(3, 0), TLCCH(3, 1), TLCCH(3, 2)},
  {TLCCH(3, 3), TLCCH(3, 4), TLCCH(3, 5)},
  {TLCCH(3, 6), TLCCH(3, 7), TLCCH(3, 8)},
  {TLCCH(3, 9), TLCCH(3, 10),TLCCH(3, 11)},
  {TLCCH(4, 0), TLCCH(4, 1), TLCCH(4, 2)},
  {TLCCH(4, 3), TLCCH(4, 4), TLCCH(4, 5)},
  {TLCCH(4, 6), TLCCH(4, 7), TLCCH(4, 8)},
  {TLCCH(4, 9), TLCCH(4, 10),TLCCH(4, 11)},
};
#define cpinsets (sizeof(cpin)/sizeof(cpin[0]))
int wait = 10;

void setup()
{
  Serial.begin(9600);
  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init();
  int i = 0;
  for (i = 0; i < CH; i++)
    c[i] = cmax[i] / 2;
  xr1 += analogRead(0);
}

int r(int ceiling)
{
  xr1 = 16807 * (xr1 & 0xfff) + (xr1 >> 12);
  return xr1 % ceiling;
}

/* One iteration of random colorspace walk. */
void random_walk()
{
  static const int maxstep = 2;
  static const int maxbounce = maxstep * 2;
  static const int maxgrad = 32;
  static const int cmaxgrad[CH] = {maxgrad, maxgrad, maxgrad};
  static const int dampening = 8; // less means tend to smaller gradient
  static int g[CH] = {0, 0, 0};

  int i;

  for (i = 0; i < CH; i++) {
    g[i] += r(maxstep) * (r(2) ? 1 : -1);
    /* dampening */ g[i] += (g[i] > 0 ? -1 : 1) * r(abs(g[i])) / dampening;
    if (g[i] < -cmaxgrad[i]) g[i] = -cmaxgrad[i] + r(maxbounce); else if (g[i] > cmaxgrad[i]) g[i] = cmaxgrad[i] - r(maxbounce);

    c[i] += g[i];
    if (c[i] < 0) { c[i] = 0; g[i] = -g[i] + r(maxbounce)-maxbounce/2; } else if (c[i] > cmax[i]) { c[i] = cmax[i]; g[i] = -g[i] + r(maxbounce)-maxbounce/2; }
  }
}

void rainbow()
{
  static int huephase = 0;
  static int huephase_i = 0;
#define HUEPHASE_LEN 32

#define huephase_to_c_inc(cc) (uint32_t) huephase_i * cmax[cc] / HUEPHASE_LEN
#define huephase_to_c_dec(cc) (cmax[cc] - (uint32_t) huephase_i * cmax[cc] / HUEPHASE_LEN)
  switch (huephase) {
    case 0: c[0] = cmax[0]; c[1] = huephase_to_c_inc(1); c[2] = 0; break;
    case 1: c[0] = huephase_to_c_dec(0); c[1] = cmax[1]; c[2] = 0; break;
    case 2: c[0] = 0; c[1] = cmax[1]; c[2] = huephase_to_c_inc(2); break;
    case 3: c[0] = 0; c[1] = huephase_to_c_dec(1); c[2] = cmax[2]; break;
    case 4: c[0] = huephase_to_c_inc(0); c[1] = 0; c[2] = cmax[2]; break;
    case 5: c[0] = cmax[0]; c[1] = 0; c[2] = huephase_to_c_dec(2); break;
  }
  
  huephase_i++;
  if (huephase_i > HUEPHASE_LEN) {
    huephase_i = 0;
    huephase = (huephase + 1) % 6;
  }
}

/* One iteration of constant brightest white (useful for tuning constants for particular LEDs). */
void white()
{
  int i;
  for (i = 0; i < CH; i++) {
    c[i] = cmax[i];
  }
}

/* White "breathing" effect to a certain degree of intensity. Good for identifying a point where further intensity change does not make any difference. */
void grey()
{
  static const int steps = 20;
  static int s = 0;
  static int d = 1;

  int i;
  for (i = 0; i < CH; i++) {
    c[i] = (uint32_t) cmax[i] * s / steps;
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
  Tlc.clear();
  
  random_walk();
  //rainbow();
  //white();
  //grey();

  int i;
  for (i = 0; i < CH; i++) {
    Serial.print(c[i], DEC); Serial.print(" ");
    int j;
    for (j = 0; j < cpinsets; j++) {
      Tlc.set(cpin[j][i], c[i]);
    }
  }
  Serial.println();

  /* Tlc.update() sends the data to the TLCs.  This is when the LEDs will
     actually change. */
  Tlc.update();

  delay(wait);
}
