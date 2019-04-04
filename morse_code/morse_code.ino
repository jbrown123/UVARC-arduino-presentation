typedef struct
{
	char 	symbol;
	unsigned char code;
} morseCode;

// the first (left most) 1 in the code is a flag for where to start
// after that each 1 is a dit, each 0 is a dah

morseCode morse[] = {
	{'A', 0b110},
	{'B', 0b10111},
	{'C', 0b10101},
	{'D', 0b1011},
	{'E', 0b11},
	{'F', 0b11101},
	{'G', 0b1001},
	{'H', 0b11111},
	{'I', 0b111},
	{'J', 0b11000},
	{'K', 0b1010},
	{'L', 0b11011},
	{'M', 0b100},
	{'N', 0b101},
	{'O', 0b1000},
	{'P', 0b11001},
	{'Q', 0b10010},
	{'R', 0b1101},
	{'S', 0b1111},
	{'T', 0b10},
	{'U', 0b1110},
	{'V', 0b11110},
	{'W', 0b1100},
	{'X', 0b10110},
	{'Y', 0b10100},
	{'Z', 0b10011},
	{'0', 0b100000},
	{'1', 0b110000},
	{'2', 0b111000},
	{'3', 0b111100},
	{'4', 0b111110},
	{'5', 0b111111},
	{'6', 0b101111},
	{'7', 0b100111},
	{'8', 0b100011},
	{'9', 0b100001},
	{'&', 0b110111},
	{'\'', 0b1100001},
	{'@', 0b1100101},
	{')', 0b1010010},
	{'(', 0b101001},
	{':', 0b1000111},
	{',', 0b1001100},
	{'=', 0b101110},
	{'!', 0b1010100},
	{'.', 0b1101010},
	{'-', 0b1011110},
	{'+', 0b110101},
	{'"', 0b1101101},
	{'?', 0b1110011},
	{'/', 0b101101}
};

unsigned char GetMorse(char c)
{
	c = toupper(c);

	for (int i = 0; i < (sizeof(morse) / sizeof(morseCode)); i++)
	{
		if (c == morse[i].symbol)
			return(morse[i].code);
	}

	return 0;
}

/*
Lifted from https://morsecode.scphillips.com/morse.html

Prosign (?)	Morse
<AA> New line	di-dah-di-dah
<AR> End of message	di-dah-di-dah-dit
<AS> Wait	di-dah-di-di-dit
<BK> Break	dah-di-di-di-dah-di-dah
<BT> New paragraph	dah-di-di-di-dah
<CL> Going off the air ("clear")	dah-di-dah-di-di-dah-di-dit
<CT> Start copying	dah-di-dah-di-dah
<DO> Change to wabun code	dah-di-di-dah-dah-dah
<KN> Invite a specific station to transmit	dah-di-dah-dah-dit
<SK> End of transmission	di-di-di-dah-di-dah
<SN> Understood (also VE)	di-di-di-dah-dit
<SOS> Distress message	di-di-di-dah-dah-dah-di-di-dit

Other Phrases	Abbreviation
Over			K
Roger			R
See you later	CUL
Be seeing you	BCNU
You're			UR
Signal report	RST
Best regards	73
Love and kisses	88

Q Code (?)	Meaning
QSL		I acknowledge receipt
QSL?	Do you acknowledge?
QRX		Wait
QRX?	Should I wait?
QRV		I am ready to copy
QRV?	Are you ready to copy?
QRL		The frequency is in use
QRL?	Is the frequency in use?
QTH		My location is...
QTH?	What is your location?

Notes
If the duration of a dot is taken to be one unit then that of a dash is three units. 
The space between the components of one character is one unit, 
between characters is three units 
and between words seven units. 

To indicate that a mistake has been made and for the receiver to delete the last word, send di-di-di-di-di-di-di-dit (eight dits).

The prosigns are combinations of two letters sent together with no space in between. 
They are indicated here using angled brackets. 
The other abbreviations and Q codes are sent with the normal spacing.

*/

/*
From:
http://en.wikipedia.org/wiki/Morse_code

The speed of Morse code is typically specified in "words per minute" (WPM). 
In text-book, full-speed Morse, a dah is conventionally 3 times as long as a dit. 
The spacing between dits and dahs within a character is the length of one dit; 
between letters in a word it is the length of a dah (3 dits); 
and between words it is 7 dits. 

The Paris standard defines the speed of Morse transmission as the dot and dash 
timing needed to send the word "Paris" a given number of times per minute. 
The word Paris is used because it is precisely 50 "dits" based on the text book timing.

Under this standard, the time for one "dit" can be computed by the formula:
T = 1200 / W
Where: W is the desired speed in words-per-minute, and T is one dit-time in milliseconds.
*/

int wpm = 10;

#define DIT_LEN (1200 / wpm)
#define DAH_LEN (DIT_LEN * 3)
#define SYMBOL_SPACING (DIT_LEN)
#define LETTER_SPACING (DIT_LEN * 3)
#define WORD_SPACING (DIT_LEN * 7)

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

const int TILT_SWITCH_PIN = 10;

void setup() {
	Serial.begin(57600);

	// Start up the LED strip
	strip.begin();
	
	// Update the strip, to start they are all 'off'
	strip.show();
	
	// turn on internal pullups for buttons
	pinMode(A1, INPUT_PULLUP);  // button 5
	pinMode(A2, INPUT_PULLUP);  // button 4
	pinMode(A3, INPUT_PULLUP);  // button 3
	pinMode(A4, INPUT_PULLUP);  // button 2
	pinMode(A5, INPUT_PULLUP);  // button 1
	
	pinMode(TILT_SWITCH_PIN, INPUT_PULLUP); // tilt switch
}

char *message = "Arduino rocks!";

int intensity = 16;

void loop() {
	Serial.print(millis());
	Serial.println("  Sending morse\n");
	Serial.print("Intensity: "); Serial.println(intensity);
	Serial.print("WPM: "); Serial.println(wpm);
	

	for (char *s = message; *s != 0; s++)
	{
		switch (readButtons())
		{
			case 1:	// button 1
				if (wpm > 1)
					wpm--;
				break;
				
			case 2:	// button 2
				if (wpm < 30)
					wpm++;
				break;

			case 4:	// button 3
				wpm = 10;
				intensity = 16;
				break;

			case 8:	// button 4
				if (intensity > 16)
					intensity -= 16;
				break;

			case 16: // button 5
				if (intensity < 128)
					intensity += 16;
				break;
		}

		Serial.print("\tLight: "); Serial.println(lightSensor());
		
		char c = *s;

		Serial.print(c);
		
		if (c == ' ')
		{
			delay(WORD_SPACING - LETTER_SPACING);
			Serial.println();
			continue;
		}
		char m = GetMorse(c);

		if (m == 0)
		{
			Serial.println(" not found");
			continue;
		}

		Serial.print(": ");
		bool enabled = false;
		for (int i = 7; i >= 0; i--)
		{
			if (enabled)
			{
				Serial.print(m & (1 << i) ? "dit " : "dah ");
				flash(m & (1 << i) ? DIT_LEN : DAH_LEN);
				if (i != 0)
				{
					delay(SYMBOL_SPACING);
				}
			}

			// the first 1 we find is the 'start' bit (it's skipped)
			if (m & (1 << i))
				enabled=true;
		}
		delay(LETTER_SPACING);
		Serial.println();
	}

	Serial.println();
	
	// wait a bit before we repeat
	delay(1000);
}

void flash(unsigned int duration)
{
	setColor(-1, intensity, intensity, intensity);
	delay(duration);
	setColor(-1, 0, 0, 0);
}


/*
 * 27 D6~ (A7) Photoresistor input
 * 22 D30 (TXLED)  Photoresistor power
 */
int lightSensor(void)
{
	pinMode(30,OUTPUT);
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

