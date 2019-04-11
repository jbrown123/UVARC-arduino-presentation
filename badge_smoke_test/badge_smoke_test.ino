/*
 * Pin assignments (from schematic) updated to RC2.1
 * physical pin #, Arduino name, (alternate name / function), use on our board
 * 20 D0 (RX)  BLE HM-10
 * 21 D1 (TX)  BLE HM-10
 * 19 D2 (SDA) OLED display SSD1306
 * 18 D3~ (SCL)  OLED display SSD1306
 * 25 D4 (A6)  LPD_SDI data for LPD8806 RGB LED controller
 * 31 D5~    LPD_SCL clock for LPD8806 RGB LED controller
 * 27 D6~ (A7) Photoresistor input
 * 01 D7   IR input (output from IR receiver)
 * 28 D8 (A8)  IR high power (active low; this is the ground return through the resistor; set to INPUT to deactivate)
 * 29 D9~ (A9) IR low power (active low; this is the ground return through the resistor; set to INPUT to deactivate)
 * 30 D10~ (A10) Tilt switch (1 = hanging; 0 = flipped up)
 * 12 D11~     IR on back (active low; this is the ground return through the resistor; set to INPUT to deactivate)
 * 26 D12 (A12)  NC
 * 32 D13~ IR ouptut (goes to all LEDs; set ground pin [see above] as OUTPUT LOW to activate)
 * 11 D14 (MISO) SPI (ICSP & flash memory)
 * 09 D15 (SCK)  SPI (ICSP & flash memory)
 * 10 D16 (MOSI) SPI (ICSP & flash memory)
 * 08 D17 (SS) SPI chip select for flash memory (active low)
 * 36 A0 (D18) Battery voltage
 * 37 A1 (D19) Button 5 active low
 * 38 A2 (D20) Button 4 active low
 * 39 A3 (D21) Button 3 active low
 * 40 A4 (D22) Button 2 active low
 * 41 A5 (D23) Button 1 active low
 * ** D24 (D4/A6)  repeat
 * ** D25 (D6/A7)  repeat
 * ** D26 (D8/A8)  repeat
 * ** D27 (D9/A9)  repeat
 * ** D28 (D10/A10) repeat
 * ** D29 (D12/A11) repeat
 * 22 D30 (TXLED)  Photoresistor power
 */

#include <LPD8806.h>

// Number of RGB LEDs in strand:
int nLEDs = 2;

// Chose 2 pins for output; can be any valid output pins:
int dataPin  = 4;
int clockPin = 5;

// First parameter is the number of LEDs in the strand.  The LED strips
// are 32 LEDs per meter but you can extend or cut the strip.  Next two
// parameters are SPI data and clock pins:
LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

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

	// Start up the LED strip
	strip.begin();
	
	// Update the strip, to start they are all 'off'
	strip.show();

	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(System5x7);
	oled.clear();
	oled.set2X();

	// turn on internal pullups for buttons
	pinMode(A1, INPUT_PULLUP);  // button 5
	pinMode(A2, INPUT_PULLUP);  // button 4
	pinMode(A3, INPUT_PULLUP);  // button 3
	pinMode(A4, INPUT_PULLUP);  // button 2
	pinMode(A5, INPUT_PULLUP);  // button 1
	
	pinMode(30,OUTPUT);	// light sensor power
	pinMode(TILT_SWITCH_PIN, INPUT_PULLUP); // tilt switch
}

void loop() {
	updateSerial();
	updateOled();
	updateLeds();
}

void updateLeds()
{
	static uint32_t lastTime = 0;

	if (millis() - lastTime >= 250)
	{
		lastTime = millis();

		int range = constrain(lightSensor() / 8, 8, 127);
		setColor(0, random(0,range), random(0,range), random(0,range));
		setColor(1, random(0,range), random(0,range), random(0,range));
	}
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
	static uint8_t lastTilt = -1;
	
	if (millis() - lastTime >= 250)
	{
		lastTime = millis();

		if (tiltSwitch() != lastTilt)
		{
			lastTilt = tiltSwitch();
			setOrientation(oled, tiltSwitch());
			oled.clear();
		}
		else
			oled.setCursor(0,0);

		oled.print("Sec: "); oled.println(millis() / 1000);
	
		oled.print("Light: "); oled.clearToEOL(); oled.println(lightSensor());

		oled.print("Button: "); oled.clearToEOL(); oled.println(readButtons());
	}	
}

/*
 * 27 D6~ (A7) Photoresistor input
 * 22 D30 (TXLED)  Photoresistor power
 */
int lightSensor(void)
{
	digitalWrite(30, HIGH);	// power on
	uint16_t retVal = analogRead(7);
	digitalWrite(30, LOW);	// power off
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


#define BUTTON_DEBOUNCE_DELAY_MILLIS  50
/*
 * 37 A1 (D19) Button 5 active low
 * 38 A2 (D20) Button 4 active low
 * 39 A3 (D21) Button 3 active low
 * 40 A4 (D22) Button 2 active low
 * 41 A5 (D23) Button 1 active low
 */
// return bitmap of buttons pressed or 0 for no button
// bit 0 = button 1, bit 1 = button 2, etc.
unsigned char readButtons(void)
{
  static byte lastButtons = 0;
  static uint32_t  lastButtonTime = 0;
  byte buttons = 0;
  
  // port F mappings:
  // PF0 = A5 / D23
  // PF1 = A4 / D22
  // PF2 & PF3 not used
  // PF4 = A3 / D21
  // PF5 = A2 / D20
  // PF6 = A1 / D19
  // PF7 = A0 / D18
  buttons = PINF & B01110011;
  buttons = (buttons & B00000011) | ((buttons & B01110000) >> 2);
  buttons = buttons ^ B00011111;

  if (buttons != lastButtons && (millis() - lastButtonTime) > BUTTON_DEBOUNCE_DELAY_MILLIS)
  {
      lastButtons = buttons;
      lastButtonTime = millis();
  }
  return lastButtons;
}

unsigned char buttonEvents(void)
{
	unsigned char buttons, temp;
	static unsigned char lastButtons;
	
	buttons = temp = (uint8_t) readButtons();  // get the current (debuounced) state
	buttons = buttons & (buttons ^ lastButtons); // get only newly pressed buttons
	// this works because buttons ^ lastButtons says which ones changed since last time
	// and buttons & (changed) = only ones that are newly on
	lastButtons = temp; // remember what was actually on this time

	return buttons;
}


// set an LED to a particular color
// params are LED, R, G, B
// LED: 0 or 1 (or -1/255 for all)
// R, G, B: intensity for each color 0-127
void setColor(int8_t led, uint8_t red, uint8_t green, uint8_t blue)
{
	red = constrain(red, 0, 127);
	green = constrain(green, 0, 127);
	blue = constrain(blue, 0, 127);
	
  // user-right LED is 0 and has G,B,R
  // user-left LED is 1 and has G,R,B
  if (led == -1)
  {
    setColor(++led, red, green, blue);
    led++;
  }
  strip.setPixelColor(
    led, // which LED
    green, // Green intensity
    led == 0 ? blue : red, // B/R intensity
    led == 0 ? red : blue  // R/B intensity
  );

  strip.show();
}

