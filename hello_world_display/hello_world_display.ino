// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

SSD1306AsciiAvrI2c oled;

void setup() {
	Serial.begin(57600);
	
	oled.begin(&Adafruit128x64, I2C_ADDRESS);
	oled.setFont(System5x7);
	oled.clear();
}

void loop() {
	oled.print(millis()/1000);
	oled.println(" Hello world!");

	Serial.print(millis());
	Serial.println("  Hello, World!");
	delay(1000);
}
