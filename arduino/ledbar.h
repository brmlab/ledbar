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
