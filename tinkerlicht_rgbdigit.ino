/*  Tinkerlicht with RGBDigit for Arduino Yún
    Copyright (C) 2015 Ralph Crützen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h> //https://github.com/adafruit/Adafruit_NeoPixel
#include <IRremote.h> //http://z3t0.github.io/Arduino-IRremote/
#include <DS3232RTC.h> //http://github.com/JChristensen/DS3232RTC
#include <Time.h> //https://github.com/PaulStoffregen/Time
#include <RGBDigit.h>
#include <Process.h>

#define LED_PIN 13
#define AUTO_TIMEOUT 100

#define DEBUG 0

#define NUMDIGITS 4
RGBDigit rgbDigit(NUMDIGITS); // uses default pin 12

byte nRun;
String color1 = "800000";
String color2 = "008000";

Process p;

void setup() {
  //debugprint("Init (bridge) variables... ");
  Bridge.begin();
  Bridge.put("prevTweet", " ");
  Bridge.put("prevMessage", " ");
  Bridge.put("color1", "000000");
  Bridge.put("color2", "000000");
  Bridge.put("newColor", "0");
  Bridge.put("auto", "0");
  //debugprintln("Done.");

  rgbDigit.begin();
  
  initTime();

  rgbDigit.setBrightness(16);
  if (DEBUG) { 
    //Serial.begin(9600);
    //delay(4000);
    //while(!Serial);
  }
  pinMode(LED_PIN, OUTPUT);
  nRun = 0;
}

void loop() {
  changeColor(color1, color2);
  digitalWrite(LED_PIN, HIGH);
  nRun++;
  if (nRun == AUTO_TIMEOUT) {
    nRun = 0;
    Bridge.put("auto", "1");
  }
  //debugprintln((String)nRun);
  searchTweet();
  digitalWrite(LED_PIN, LOW);
  waitToContinue(30); // in seconds 
}

void initTime() {
  int hours, minutes, seconds, days, months, years;
  String timeString;
  p.begin("date");
  p.addParameter("+%T-%D"); // hh:mm:ss-mm/dd/yy
  p.run();
  while (p.available()>0) {
    char c = p.read();
    timeString += c;
  } 
  int firstColon = timeString.indexOf(":");
  int secondColon = timeString.lastIndexOf(":");
  int dash = timeString.lastIndexOf("-");
  int firstSlash = timeString.indexOf("/");
  int secondSlash = timeString.lastIndexOf("/");
  hours = timeString.substring(0, firstColon).toInt();
  minutes = timeString.substring(firstColon + 1, secondColon).toInt();
  seconds = timeString.substring(secondColon + 1, dash).toInt();
  months = timeString.substring(dash + 1, firstSlash).toInt();
  days = timeString.substring(firstSlash + 1, secondSlash).toInt();
  years = timeString.substring(secondSlash).toInt();
  rgbDigit.setTimeDate(hours, minutes, seconds, days, months, years);  
}


void searchTweet() {
  //Process p;
  char message[6];
  char newColor[1];
  
  //debugprintln((String)freeRam());
  
  //debugprint("Starting tinkerlicht-parse-fb.py... ");
  p.runShellCommand("/mnt/sda1/arduino/tinkerlicht-parse-fb.py");
  //debugprintln("Done.");
 
  Bridge.get("color1", message, 6);
  color1 = "";
  for (int i=0; i<6; i++) {
    color1 += message[i];
  }
  
  //debugprintln("Color1 from bridge: " + color1);
  
  Bridge.get("color2", message, 6);
  color2 = "";
  for (int i=0; i<6; i++) {
    color2 += message[i];
  } 
  
  //debugprintln("Color2 from bridge: " + color2);
  
  Bridge.get("newColor", newColor, 1);
  if (newColor[0] == '1') {
    nRun = 0;
    //debugprintln("New color!");
    Bridge.put("newColor", "0");
    changeColor(color1, color2);
    tweetPhoto();
  }
}

void changeColor(String col1, String col2) {
  char red1[3]   = {col1[0],col1[1]};
  char green1[3] = {col1[2],col1[3]};
  char blue1[3]  = {col1[4],col1[5]};
  byte r1 = hexToByte(red1);
  byte g1 = hexToByte(green1);
  byte b1 = hexToByte(blue1);

  char red2[3]   = {col2[0],col2[1]};
  char green2[3] = {col2[2],col2[3]};
  char blue2[3]  = {col2[4],col2[5]};
  byte r2 = hexToByte(red2);
  byte g2 = hexToByte(green2);
  byte b2 = hexToByte(blue2);

  int h = rgbDigit.getHour();
  int h1 = h/10;
  int h2 = h - (h/10)*10;
  rgbDigit.setDigit(h1, 0, 1, 1, 1);
  rgbDigit.setDigit(h2, 1, 1, 1, 1);
  int m = rgbDigit.getMinute();
  int m1 = m/10;
  int m2 = m - (m/10)*10;
  rgbDigit.setDigit(m1, 2, 1, 1, 1);
  rgbDigit.setDigit(m2, 3, 1, 1, 1);
  rgbDigit.showDot(1, 80, 80, 80);  

  byte layout[3][3] = { {4,5,-1} , {0,3,6} , {1,2,-1} };

  int n = 3*NUMDIGITS;
  for(int i = 0; i < n; i++) {
    float factor1 = 1 - (float)i/(n-1);
    float factor2 = (float)i/(n-1);
    byte r = r1*factor1 + r2*factor2;
    byte g = g1*factor1 + g2*factor2;
    byte b = b1*factor1 + b2*factor2;
    //debugprintln("Factors: " + String(factor1) + " " + String(factor2));
    //debugprintln(String(r) + " " + String(g) + " " + String(b));
    for (int s = 0; s < 3; s++) {
      //debugprintln(String(layout[i%3][s]));
      if (layout[i%3][s] != -1)
        if (rgbDigit.isSegmentOn(i/3, layout[i%3][s]))
          rgbDigit.segmentOn(i/3, layout[i%3][s], r, g, b);
    }
  } 
}

void tweetPhoto() {
  //Process p; 
  //debugprint("Taking picture and tweeting... ");
  p.runShellCommand("/mnt/sda1/arduino/tinkerlicht-tweetphoto.py");  
  //debugprintln("Done.");
}

//*********************
//* Utility functions *
//*********************
/*
void debugprint(String s) {
  //if (DEBUG) Serial.print(s);
}

void debugprintln(String s) {
  //if (DEBUG) Serial.println(s);
}
*/
void waitToContinue(int duration) {
  //debugprintln("In waitToContinue()");
  int step = 1;
  for(int i = duration; i>0; i-=step) {
    digitalWrite(LED_PIN, HIGH);
    delay(step*500);
    digitalWrite(LED_PIN, LOW);
    //debugprint(String(i) + " ");
    delay(step*500);
  }
  //debugprintln("");
}

byte hexToByte(char hexString[]) {
  // https://github.com/benrugg/Arduino-Hex-Decimal-Conversion
  byte decValue = 0;
  char nextChar;
  for (int i = 0; i < 2; i++) {
    nextChar = char(hexString[i]);
    if (nextChar >= 48 && nextChar <= 57) nextChar = map(nextChar, 48, 57, 0, 9);
    if (nextChar >= 65 && nextChar <= 70) nextChar = map(nextChar, 65, 70, 10, 15);
    if (nextChar >= 97 && nextChar <= 102) nextChar = map(nextChar, 97, 102, 10, 15);
    nextChar = constrain(nextChar, 0, 15);
    decValue = (decValue * 16) + nextChar;
  }
  return decValue;
}
