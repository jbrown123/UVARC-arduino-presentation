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

#define BUTTON_DEBOUNCE_DELAY_MILLIS  50

void setup() {
  // turn on internal pullups for buttons
  pinMode(A1, INPUT_PULLUP);  // button 5
  pinMode(A2, INPUT_PULLUP);  // button 4
  pinMode(A3, INPUT_PULLUP);  // button 3
  pinMode(A4, INPUT_PULLUP);  // button 2
  pinMode(A5, INPUT_PULLUP);  // button 1

  pinMode(10, INPUT_PULLUP);  // tilt switch
  
  Serial.begin(57600);
}


byte last = 0;
int count = 0, lastCount = 0;
uint32_t lastTime = 0;

void loop() {
  uint32_t  now = millis();
  
//  byte b = buttonEvents();    // all the buttons, 1 event per press
  
//  byte b = readButtons();   // all the buttons, current state

//  byte b = digitalRead(A5);   // just the 1 button (no debounce)

  byte b = digitalRead(10); // the tilt switch, great to demo debounce issues
  
  if (b != last)
  {
//    Serial.println(b);  // comment this out to demo debounce
    count++;
  }
  
  last = b;

  // print count every 1/2 second if changed
  if (now % 500 == 0 && count != lastCount)
  {
    Serial.print("count = ");
    Serial.println(count);
    lastCount = count;
  }

/*
  // print light sensor (analog input) once per second
  if (now - lastTime >= 1000)
  {
    lastTime = now;
    Serial.print("light = ");
    Serial.println(lightSensor());
  }
*/  
}


/*
 * 27 D6~ (A7) Photoresistor input
 * 22 D30 (TXLED)  Photoresistor power
 */
uint16_t lightSensor(void)
{
  pinMode(30,OUTPUT);
  digitalWrite(30, HIGH);
  uint16_t retVal = analogRead(7);
  digitalWrite(30, LOW);
  return retVal;
}

/////////////////////////
// button stuff for bitlash
/*
 * 37 A1 (D19) Button 5 active low
 * 38 A2 (D20) Button 4 active low
 * 39 A3 (D21) Button 3 active low
 * 40 A4 (D22) Button 2 active low
 * 41 A5 (D23) Button 1 active low
 */
// return bitmap of buttons pressed or 0 for no button
// bit 0 = button 1, bit 1 = button 2, etc.
uint8_t readButtons(void)
{
  static uint8_t lastButtons = 0;
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

// return bitmap of buttons currently pressed since last check
// will only return a button bit on once per press
byte buttonEvents(void)
{
  static uint8_t lastButtons;
  uint8_t buttons, temp;

  buttons = temp = (uint8_t) readButtons();  // get the current (debuounced) state
  buttons = buttons & (buttons ^ lastButtons); // get only newly pressed buttons
  // this works because buttons ^ lastButtons says which ones changed since last time
  // and buttons & (changed) = only ones that are newly on
  lastButtons = temp; // remember what was actually on this time

  return buttons;
}


