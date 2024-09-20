#include <U8g2lib.h>

#include <EEPROM.h>
#include <Bounce2.h>
#include <Encoder.h>
#include <IRremote.h>
#include <IRremoteInt.h>
#include <TimerOne.h>
//#include <aRest.h>
//#include <SoftwareSerial.h>

#include "preamp.h" 

#include <Wire.h>
#include <OneWire.h>
//#include <LiquidCrystal_I2C.h>

Preamp* preamp;

void setup()
{
  // put your setup code here, to run once:
  preamp = new Preamp();
  preamp->init();
}

void loop()
{
  // put your main code here, to run repeatedly:
  preamp->gerer();  
}
