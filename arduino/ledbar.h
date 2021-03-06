/* Generic ledbar library taking care of I2C Arduino communication
 * with TLC59116. */

#include <Wire.h>

enum LedbarPinMode {
	LPM_OFF,
	LPM_ON,
	LPM_PWM,
	LPM_GRPPWM,
};

class Ledbar {
	public:
	void begin(unsigned char address); /* 7-bit TLC address */
	/* pin is 0 .. 15 */
	void setPinMode(int pin, enum LedbarPinMode pinMode);
	void setPinPWM(int pin, unsigned char dutyCycle);
	void setAllPinPWM(unsigned char dutyCycles[16]);
	private:
	unsigned char address;
	unsigned char rawPinModes[4];
} Ledbar;

void Ledbar::begin(unsigned char address_)
{
	address = address_;
	memset(&rawPinModes, 0, sizeof(rawPinModes));

	Wire.begin();
	Wire.beginTransmission(address);
	Wire.write(0x00); // reg 0
	Wire.write(0x01); // broadcast on, [5bit]=0 turns on oscillator
	Wire.endTransmission();
}

void Ledbar::setPinMode(int pin, enum LedbarPinMode pinMode)
{
	Wire.beginTransmission(address);
	rawPinModes[pin / 4] &= ~(0x3     << (pin % 4 * 2));
	rawPinModes[pin / 4] |=  (pinMode << (pin % 4 * 2));
	Wire.write(0x14 + pin / 4); Wire.write(rawPinModes[pin / 4]);
	Wire.endTransmission();
}

void Ledbar::setPinPWM(int pin, unsigned char dutyCycle)
{
	Wire.beginTransmission(address);
	Wire.write(0x2 + pin); Wire.write(dutyCycle);
	Wire.endTransmission();
}

void Ledbar::setAllPinPWM(unsigned char dutyCycles[16])
{
	Wire.beginTransmission(address);
	Wire.write(0b10100000 /* autoincrement */ | 0x2);
	for (int i = 0; i < 16; i++)
		Wire.write(dutyCycles[i]);
	Wire.endTransmission();
}


/** Current ledbar configuration: */

#define NUM_TLCS 4
#define LEDS_PER_TLC 5

#define TLCCH(tlc_num, ch_num) ((tlc_num) << 4 | (ch_num))

#define CH 3

const int cpin[LEDS_PER_TLC * NUM_TLCS][CH] = {
  {TLCCH(0, 0), TLCCH(0, 1), TLCCH(0, 2)},
  {TLCCH(0, 3), TLCCH(0, 4), TLCCH(0, 5)},
  {TLCCH(0, 6), TLCCH(0, 7), TLCCH(0, 8)},
  {TLCCH(0, 9), TLCCH(0, 10),TLCCH(0, 11)},
  {TLCCH(0, 12),TLCCH(0, 13),TLCCH(0, 14)},

  {TLCCH(1, 0), TLCCH(1, 1), TLCCH(1, 2)},
  {TLCCH(1, 3), TLCCH(1, 4), TLCCH(1, 5)},
  {TLCCH(1, 6), TLCCH(1, 7), TLCCH(1, 8)},
  {TLCCH(1, 9), TLCCH(1, 10),TLCCH(1, 11)},
  {TLCCH(1, 12),TLCCH(1, 13),TLCCH(1, 14)},

  {TLCCH(2, 0), TLCCH(2, 1), TLCCH(2, 2)},
  {TLCCH(2, 3), TLCCH(2, 4), TLCCH(2, 5)},
  {TLCCH(2, 6), TLCCH(2, 7), TLCCH(2, 8)},
  {TLCCH(2, 9), TLCCH(2, 10),TLCCH(2, 11)},
  {TLCCH(2, 12),TLCCH(2, 13),TLCCH(2, 14)},

  {TLCCH(3, 0), TLCCH(3, 1), TLCCH(3, 2)},
  {TLCCH(3, 3), TLCCH(3, 4), TLCCH(3, 5)},
  {TLCCH(3, 6), TLCCH(3, 7), TLCCH(3, 8)},
  {TLCCH(3, 9), TLCCH(3, 10),TLCCH(3, 11)},
  {TLCCH(3, 12),TLCCH(3, 13),TLCCH(3, 14)},
};
#define cpinsets (sizeof(cpin)/sizeof(cpin[0]))

/* cca 2.7ohm resistor per channel */
const int cmin[cpinsets][CH] = { // level of 1 (not 0!)
  {   4,   0,   0 }, /* box 1 */
  {   5,   0,   0 }, /* box 2 */
  {   5,   0,   0 }, /* box 3 */
  {   5,   0,   0 }, /* box 4 */
  {   5,   0,   0 }, /* box 5 */

  {   5,   0,   0 }, /* box 6 */
  {   4,   0,   0 }, /* box 7 */
  {   5,   0,   0 }, /* box 8 */
  {   5,   0,   0 }, /* box 9 */
  {   5,   0,   0 }, /* box 10 */

  {   4,   0,   0 }, /* box 11 */
  {   5,   0,   0 }, /* box 12 */
  {   5,   0,   0 }, /* box 13 */
  {   5,   0,   0 }, /* box 14 */
  {   5,   0,   0 }, /* box 15 */

  {   5,   0,   0 }, /* box 16 */
  {   4,   0,   0 }, /* box 17 */
  {   5,   0,   0 }, /* box 18 */
  {   5,   0,   0 }, /* box 19 */
  {   5,   0,   0 }, /* box 20 */
};
const int cmax[cpinsets][CH] = { // level of 255
  { 120, 250, 190 }, /* box 11 */
  { 100, 250, 180 }, /* box 2 */
  { 100, 250, 180 }, /* box 3 */
  { 100, 250, 180 }, /* box 4 */
  { 100, 250, 180 }, /* box 5 */

  {  80, 250, 190 }, /* box 6 */
  {  90, 250, 160 }, /* box 7 */
  { 100, 250, 180 }, /* box 8 */
  { 100, 250, 230 }, /* box 9 */
  { 100, 250, 180 }, /* box 10 */

  { 120, 250, 190 }, /* box 11 */
  { 100, 250, 180 }, /* box 12 */
  { 100, 250, 180 }, /* box 13 */
  { 100, 250, 180 }, /* box 14 */
  { 100, 250, 180 }, /* box 15 */

  {  80, 250, 190 }, /* box 16 */
  {  90, 250, 160 }, /* box 17 */
  { 100, 250, 180 }, /* box 18 */
  { 100, 250, 230 }, /* box 19 */
  { 100, 250, 180 }, /* box 20 */
};

static void setbyc(class Ledbar *lb, int (*c)[CH])
{
  for (int i = 0; i < NUM_TLCS; i++) {
    // XXX: We assume static allocation of channels to pins 0..14 (in order)
    // TODO: Just use unsigned char for c[] ?
    unsigned char dutycycles[16];
    int *cblock = (int *) &c[i * LEDS_PER_TLC];
    for (int j = 0; j < 16; j++) {
      dutycycles[j] = cblock[j];
    }
    lb[i].setAllPinPWM(dutycycles);
  }
}
