#include "../ledbar.h"

class Ledbar lb[NUM_TLCS];
int c[cpinsets][CH];

int wait = 10;

void setup()
{
  Serial.begin(115200);
  int i = 0, led = 0;
  for (i = 0; i < NUM_TLCS; i++)
    lb[i].begin(B1100000 | i);
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      c[led][i] = cmax[led][i] / 2;
      lb[cpin[led][i] >> 4].setPinMode(cpin[led][i] & 0xf, LPM_PWM);
    }
  }
}

void loop()
{
  int led, i;
  while (!Serial.available());
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      unsigned long s = (unsigned char) Serial.read();
      // Serial.print(cpin[led][i], DEC); Serial.print("="); Serial.print(s, DEC); Serial.print("/"); Serial.print(cmax[led][i], DEC); Serial.print(" ");
      //lb[cpin[led][i] >> 4].setPinPWM(cpin[led][i] & 0xf, s);
      lb[cpin[led][i] >> 4].setPinPWM(cpin[led][i] & 0xf, s * cmax[led][i] / 256);
    }
  }
  // Serial.println();
}
