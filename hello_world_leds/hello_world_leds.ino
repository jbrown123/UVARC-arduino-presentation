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

void setup() {
	Serial.begin(57600);

	// Start up the LED strip
	strip.begin();
	
	// Update the strip, to start they are all 'off'
	strip.show();
}

void loop() {
	Serial.print(millis());
	Serial.println("  Hello, World!");

	setColor((millis()/1000) % 2, random(0,128), random(0,128), random(0,128));
	
	delay(1000);
}

// set an LED to a particular color
// params are LED, R, G, B
// LED: 0 or 1 (or -1/255 for all)
// R, G, B: intensity for each color 0-127
void setColor(int8_t led, uint8_t red, uint8_t green, uint8_t blue)
{
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


