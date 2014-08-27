/*
  DS3234 Library
  
  Demonstrates the use a DS3234 RTC module with SPI library.
  
  SCK  pin D13
  MOSI pin D11
  MISO pin D12
  SS   pin D10
*/

#include <ds3234.h>
#include <SPI.h>

#define CS 10 // CS pin

Ds3234 rtc(CS);

void setup() {
  Serial.begin(9600);
  // Set Date (year, month, date)
  // ex. 2011/9/9
  rtc.SetDate(11, 9, 9);
  // Set Time (ampm, hour, minute, sec)
  // ex. PM 2:25:30
  rtc.SetTime(1, 2, 25, 30);
  // ex. 16:20;25
  // rtc.SetTime(16, 20, 25);
  Serial.print("RTC Hour Mode:");
  Serial.println(rtc.GetHourMode());
}

void loop() {
  byte year, month, date, ampm, hour, minute, sec;
  rtc.GetDate(year, month, date);
  rtc.GetTime(ampm, hour, minute, sec);
  // rtc.GetTime(hour, minute, sec);
  Serial.print("Date:20");
  Serial.print(year, DEC);
  Serial.print("/");
  Serial.print(month, DEC);
  Serial.print("/");
  Serial.print(date, DEC);
  Serial.print("\tTime:");
  if (ampm == 0) Serial.print("AM ");
  else Serial.print("PM ");
  Serial.print(hour, DEC);
  Serial.print(":");
  Serial.print(minute, DEC);
  Serial.print(":");
  Serial.println(sec, DEC);
  delay(1000);
}
