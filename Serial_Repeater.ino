#define OLED 0
#define IOT_BADGE_BUILD 1
#define AUTO_DISCOVER 1



#if OLED
////////////////////////////
// OLED display support (ASCII only)
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"

////////////////////////////
// OLED display support (ASCII only)

uint8_t flash_read_byte(uint32_t addr)
{
    uint8_t c;
    
    flash_read(addr, &c, 1);
    return c;
}

// 0X3C+SA0 - 0x3C or 0x3D
#define I2C_ADDRESS 0x3C

SSD1306AsciiAvrI2c oled;

#if INCLUDE_SCROLLING != 2
  #error "Please set INCLUDE_SCROLLING to 2 in SSD1306Ascii.h"
#endif

#define OLEDPRINT oled.print
#define OLEDPRINTLN oled.println

#else

#define OLEDPRINT Serial.print
#define OLEDPRINTLN Serial.println

#endif


#if AUTO_DISCOVER
#define BLE_PREAMBLE  "AT+"
#define BLE_RESPONSE  "OK+"
#define BLE_RESPONSE_LEN 8
#define BLE_TIMEOUT 2000

const char initCmds[][6] = 
{
  "RENEW",
  "CLEAR",
  "ERASE",
  "ROLE1",
  "IMME1",
  "NOTI1",
  "SHOW1",    // no need to show the names of the radios we find
  "RESET"
};
#define BLE_INIT_CMDS (sizeof(initCmds) / sizeof(initCmds[0]))


// set the returnData flag if you don't want wait to read the response
int bleWait(unsigned int len)
{
  static uint32_t startTime;
  startTime = millis();
  while (Serial1.available() < len)
  {
    if (millis() - startTime > BLE_TIMEOUT)
      return -1;
  }
  
  // look for OK response
  if (Serial1.read() != 'O' || Serial1.read() != 'K')
    return -1;

  OLEDPRINT("OK");

  // flush the rest of the response
  while (Serial1.available())
    OLEDPRINT((char) Serial1.read());

  OLEDPRINT("\n");
  
  return 0;
}

int bleCommand(const char cmds[][6], byte cmdCount)
{
  static char buffer[3+5+1] = BLE_PREAMBLE; // preamble, command, null
  //buffer[sizeof(buffer)-1] = '\0'; // null terminate for println
  static uint32_t delayStart;
  
  Serial1.write(buffer, 2);  // put us into command mode (reuse the "AT" that's in the buffer)
  delay(750);
  
  // flush inbound characters
  while (Serial1.available())
    Serial1.read();
  
  static byte i; // declaration can't be inside for() or it never gets reset to 0 on future invocations
  for (i=0; i < cmdCount; i++)
  {
    memcpy(buffer+3, cmds[i], 5);
    Serial1.write(buffer, 8);
#if OLED
    OLEDPRINTLN(buffer);
#endif

    if (bleWait(BLE_RESPONSE_LEN))
      return -1;

    // pause after renew or reset command
    // this is a hacky way to do this, but it works
    if (buffer[3] == 'R' && buffer[4] == 'E')
    {
      delay(1000);
    }
  }

  // flag BLE as success
  return 0;
}
#endif

void setup() 
{
  Serial.begin(9600);
  Serial1.begin(9600);

#if OLED
  flash_init();
#endif

  while(!Serial && millis() < 5000)
    ;
#if OLED
  oled.begin(&Adafruit128x64, I2C_ADDRESS);
  oled.setFont("sys5x7");
  oled.clear();
  Serial.println("BLE monitor");
  OLEDPRINTLN("BLE monitor");
#endif

#if AUTO_DISCOVER
#if OLED
  if (bleCommand(initCmds, BLE_INIT_CMDS))
    OLEDPRINTLN("BLE init FAIL");
  else
    OLEDPRINTLN("BLE init good");
#endif
#else
  bleCommand(initCmds, BLE_INIT_CMDS);

#endif
}

void loop() 
{
  unsigned long lastCharTime = 0;
  unsigned long lastDiscTime = 0;

  while (1)
  {
#if AUTO_DISCOVER
    if (millis() - lastDiscTime > 2000)
    {
      Serial.println();
      Serial1.print("AT+DISC?");
      lastDiscTime = millis();
    }
#endif
    
    if (lastCharTime && millis() - lastCharTime > 10)
    {
#if OLED
      if (oled.col())
        OLEDPRINTLN();
#endif
      lastCharTime = 0;
    }
    
    while (Serial1.available())
    {
      char c = Serial1.read();

      Serial.write(c);
#if OLED
      if (oled.col() > 127-5)
        OLEDPRINTLN();
      OLEDPRINT(c);
#endif
      lastCharTime = millis();
    }
  
    while (Serial.available())
      Serial1.write(Serial.read());
  }
}
