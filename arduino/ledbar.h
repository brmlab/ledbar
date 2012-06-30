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


/** Current ledbar configuration: */

#define NUM_TLCS 2

#define TLCCH(tlc_num, ch_num) ((tlc_num) << 4 | (ch_num))

#define CH 3

const int cpin[5 * NUM_TLCS][CH] = {
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

#if 0
  {TLCCH(2, 0), TLCCH(2, 1), TLCCH(2, 2)},
  {TLCCH(2, 3), TLCCH(2, 4), TLCCH(2, 5)},
  {TLCCH(2, 6), TLCCH(2, 7), TLCCH(2, 8)},
  {TLCCH(2, 9), TLCCH(2, 10),TLCCH(2, 11)},
  {TLCCH(2, 12),TLCCH(2, 13),TLCCH(2, 14)},
#endif
};
#define cpinsets (sizeof(cpin)/sizeof(cpin[0]))

/* cca 2.7ohm resistor per channel */
const int cmax[cpinsets][CH] = {
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 240, 230 },
  { 100, 230, 188 },

  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 240, 230 },
  { 100, 230, 188 },
#if 0

  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 250, 138 },
  { 100, 240, 230 },
  { 100, 230, 188 },
#endif
};
