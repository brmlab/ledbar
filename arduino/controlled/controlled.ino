#include "../ledbar.h"

class Ledbar lb[NUM_TLCS];
int c[cpinsets][CH];

int wait = 10;

void setup()
{
  Serial.begin(57600);
  int i = 0, led = 0;
  for (i = 0; i < NUM_TLCS; i++)
    lb[i].begin(B1100000 | i);
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      c[led][i] = cmax[led][i] / 2;
      lb[cpin[led][i] >> 4].setPinMode(cpin[led][i] & 0xf, LPM_PWM);
    }
  }
  Serial.println("- ready");
}

void loop()
{
  int led, i;
  /* Wait for synchronization. */
  do {
    while (!Serial.available());
  } while (Serial.read() != 0xAC);
  /* Set LEDs. */
  for (led = 0; led < cpinsets; led++) {
    for (i = 0; i < CH; i++) {
      while (!Serial.available());
      unsigned char s = Serial.read();
      //c[led][i] = s;
      c[led][i] = s == 0 ? 0 : map(s, 1, 255, cmin[led][i], cmax[led][i]);
      //Serial.print(c[led][i], DEC);
      //Serial.print(" ");
    }
  }
  setbyc(lb, c);
  //Serial.println(".");
}
