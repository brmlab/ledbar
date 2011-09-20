#include "Tlc5940.h"

#define CH 3

#define TLCCH(tlc_num, ch_num) ((tlc_num)*16 + (ch_num))

int cpin[][CH] = {
  {TLCCH(0, 2), TLCCH(0, 1), TLCCH(0, 0)},
  {TLCCH(0, 5), TLCCH(0, 4), TLCCH(0, 3)},
  {TLCCH(0, 8), TLCCH(0, 7), TLCCH(0, 6)},
  {TLCCH(0, 12), TLCCH(0, 11),TLCCH(0, 10)},
  {TLCCH(0, 15),TLCCH(0, 14),TLCCH(0, 13)},

  {TLCCH(1, 2), TLCCH(1, 1), TLCCH(1, 0)},
  {TLCCH(1, 5), TLCCH(1, 4), TLCCH(1, 3)},
  {TLCCH(1, 8), TLCCH(1, 7), TLCCH(1, 6)},
  {TLCCH(1, 12), TLCCH(1, 11), TLCCH(1, 10)},
  {TLCCH(1, 15), TLCCH(1, 14), TLCCH(1, 13)},
};
#define cpinsets (sizeof(cpin)/sizeof(cpin[0]))

/* cca 2.7ohm resistor per channel */
int cmax[cpinsets][CH] = {
  { 1600, 4000, 2200 },
  { 1600, 4000, 2200 },
  { 1600, 4000, 2200 },
  { 1600, 3900, 2000 },
  { 1600, 3700, 3000 },

  { 1600, 4000, 2200 },
  { 1600, 4000, 2200 },
  { 1600, 3400, 3000 },
  { 1600, 4000, 2200 },
  { 1600, 3400, 3000 },
};
int c[cpinsets][CH];

int wait = 10;

void setup()
{
  Serial.begin(38400);
  /* Call Tlc.init() to setup the tlc.
     You can optionally pass an initial PWM value (0 - 4095) for all channels.*/
  Tlc.init();
  int i = 0, led = 0;
  for (led = 0; led < cpinsets; led++)
    for (i = 0; i < CH; i++)
      c[led][i] = cmax[led][i];
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

void loop()
{
  Tlc.clear();
 
  int led, i;
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      while (!Serial.available());
      unsigned long s = (unsigned char) Serial.read();
//      Serial.print(s, DEC);
//      Serial.print(" ");
      Tlc.set(cpin[led][i], s * cmax[led][i] / 256);
    }
  }
//  Serial.println();

  Tlc.update();
}

