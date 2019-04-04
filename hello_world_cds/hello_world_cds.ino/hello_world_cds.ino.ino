// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

SSD1306AsciiAvrI2c oled;

//------------------------------------------------------------------------------
// the following two configurations are based on discussion at
// https://forums.adafruit.com/viewtopic.php?f=47&t=25438

// config when the connection pins are above display
static const uint8_t MEM_TYPE Adafruit128x64PinsAbove[] = {
    SSD1306_SEGREMAP | 0x1,            // column 127 mapped to SEG0
    SSD1306_COMSCANDEC                 // column scan direction reversed
};
// config when the connection pins are below display
static const uint8_t MEM_TYPE Adafruit128x64PinsBelow[] = {
    SSD1306_SEGREMAP,            // column 0 mapped to SEG0
    SSD1306_COMSCANINC           // column scan direction forward
};

void setOrientation(SSD1306AsciiAvrI2c oled, uint8_t dir)
{
  const uint8_t* table = dir == 0 ? Adafruit128x64PinsAbove : Adafruit128x64PinsBelow;
  for (uint8_t i = 0; i < sizeof(Adafruit128x64PinsAbove); i++) {
    oled.ssd1306WriteCmd(pgm_read_byte(table + i));
  }
}

const int TILT_SWITCH_PIN = 10;

void setup() {
	Serial.begin(57600);
	
	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(System5x7);
	oled.clear();
	oled.set2X();

	pinMode(30,OUTPUT);	// light sensor power
	pinMode(TILT_SWITCH_PIN, INPUT_PULLUP); // tilt switch
}

void loop() {
	updateSerial();
	updateOled();
}

void updateSerial()
{
	static uint32_t lastTime = 0;

	if (millis() != lastTime)
	{
		lastTime = millis();
		Serial.println(lightSensor());
	}
}
void updateOled()
{
	static uint32_t lastTime = 0;
	
	if (millis() - lastTime >= 1000)
	{
		lastTime = millis();

		setOrientation(oled, tiltSwitch());
		oled.clear();
	
		oled.print("Light: "); oled.print(lightSensor());
	}	
}

/*
 * 27 D6~ (A7) Photoresistor input
 * 22 D30 (TXLED)  Photoresistor power
 */
int lightSensor(void)
{
	digitalWrite(30, HIGH);
	uint16_t retVal = analogRead(7);
	digitalWrite(30, LOW);
	return retVal;
}

////////////////////////////
// Tilt switch returns up/down status
#define TILT_UP 1
#define TILT_DOWN 0

//
// returns TILT_UP when the badge is facing the user direction (numbers right side up)
//
int tiltSwitch(void)
{
  if (digitalRead(TILT_SWITCH_PIN) == 0)
    return TILT_UP;
  else
    return TILT_DOWN;
}




