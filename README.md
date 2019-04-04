# UVARC-arduino-presentation
Information, documentation, examples, source code for my introduction to Arduino presentation to the Utah Valley Amateur Radio Club (UVARC)

## Arduino Information
**What is "Arduino"?**

[Arduino](https://www.arduino.cc) is an open-source electronics platform based on easy-to-use hardware and software. Arduino boards are able to read inputs - light on a sensor, a finger on a button, or a Twitter message - and turn it into an output - activating a motor, turning on an LED, publishing something online. You can tell your board what to do by sending a set of instructions to the microcontroller on the board. You can program the microprocessor using [Scratch](http://s4a.cat/), the [Arduino IDE](https://www.arduino.cc/en/Main/Software) , or the [GCC tool chain](https://www.instructables.com/id/AVR-Programming-with-Arduino-AVRdude-and-AVR-gcc/).

The original Arduinos were based on the [Atmel AVR](https://en.wikipedia.org/wiki/AVR_microcontrollers) line of microprocessors (Atmel was acquired by Microchip in 2016). The AVR chip contains all the subsystems necessary for an embedded controller.  They include a CPU, clock, timers, RAM, FLASH program storage, EEPROM, serial UART, ADC, PWM, even internal pull up resistors. This makes them ideal for creating simple embedded devices because they don't require the external support components that other processors need.

Arduinos have a very low "barrier to entry" both from a cost perspective (boards are less than $25, all the software you need is open source) and in terms of the learning curve.  Check out the [Arduino Getting Started Guide](https://www.arduino.cc/en/Guide/HomePage) to walk you through the process.

## Adobe IoT badge
Jared and I created the IoT badge for an Adobe internal engineering conference. One of our primary goals was to be able to reuse the badge after the conference.

There are two different badges that you might have received. One has a display, the other does not. Beyond that, they are basically identical.  You can add a display to the badges that don't have one, but you'll need to either talk to me or Jared to get the particulars of what you need.

**NOTE:** If you have a badge without a display, running code that attempts to use the display will hang (stop execution) the program. You can load a different program to recover your badge.

## What's in this repo?
This repo contains a wealth of information about the Adobe badges. I've focused on the Adobe badges here because

1) Those are the badges we handed out at the club meeting.

2) There is already plenty of information about generic Arduino programming on the arduino.cc site and a host of other places on the web.  Repeating that information here would be redundant.

In particular, look at the "How to reuse the IoT badge" PDF and the production schematic that are included in the root of the repo.

Additionally, you will find a PDF of the presentation slides that I used at the club meeting.

Also in the root of the repo is an Arduino sketch called "serial_repeater.ino". This file will allow you to communicate with the BLE radio on the badge. Basically it turns the badges CPU into a pass-through connector so that anything you send to the serial port gets sent on to the radio and anything from the radio gets sent to the serial port.

You will also find a number of directories here that contain various demonstration programs for the badge.  Each one shows off one or more of the components or subsystems on the badge. For example, hello_world_leds puts some random colors on the two RGB LEDs.

#### Bitlash

[Bitlash](http://bitlash.net/) is an open source interpreted language shell and embedded programming environment for the Arduino.

The Bitlash interpreter runs entirely on the Arduino and interprets commands that you type in a terminal window or send programmatically to the serial port:

```
bitlash here! v2.0 (c) 2012 Bill Roy -type HELP- 942 bytes free
> print "Hello, world!", millis()
Hello, world! 11939
>
```

Most (but not all) of the badges we handed out at the meeting have bitlash installed. You can check this by using a serial terminal (like the one in the Arduino IDE) and connect to the badge at 57600 baud.  If you type the command "help" you should get a response from the badge (make sure your "line ending" is set to "carriage return").

The version of bitlash we put on the badges is highly optimized and some features have been disabled to save space. The 328 processor used on the badge has limited program memory (32K) and we used all but about 8 bytes of that space.

Once you write a new program to the badge using the IDE, the existing bitlash interpreter will be overwritten and you wont be able to recover it.  It's not a problem as long as you don't expect to be able to use it again.
