#include <Aldi_NeoPixel.h>
//#include <LiquidCrystal.h>
#include <Process.h>

#define STRIP_PIN 6
#define LED_PIN 13
#define NUM_PIXELS 12
#define AUTO_TIMEOUT 100

#define DEBUG 0
#define DISPLAY 0

Aldi_NeoPixel strip = Aldi_NeoPixel(NUM_PIXELS, STRIP_PIN, NEO_BRG + NEO_KHZ800);

//LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

byte nRun;

void setup() { 
  pinMode(LED_PIN, OUTPUT);
  strip.begin();
  strip.show();
  
  if (DEBUG) { 
    Serial.begin(9600);
    delay(4000);
    while(!Serial);
  }
/*  
  if (DISPLAY) {
    lcd.begin(20, 2);
    //lcd.autoscroll();
  }
*/  
  debugprint("Init (bridge) variables... ");
  Bridge.begin();
  Bridge.put("prevTweet", " ");
  Bridge.put("prevMessage", " ");
  Bridge.put("color1", "000000");
  Bridge.put("color2", "000000");
  Bridge.put("newColor", "0");
  Bridge.put("auto", "0");
  nRun = 0;
  debugprintln("Done.");
}

void loop() {
  digitalWrite(LED_PIN, HIGH);
  nRun++;
  if (nRun == AUTO_TIMEOUT) {
    nRun = 0;
    Bridge.put("auto", "1");
  }
  debugprintln((String)nRun);
  searchTweet();
  digitalWrite(LED_PIN, LOW);
  waitToContinue(30); // in seconds 
}

void searchTweet() {
  Process p;
  char message[6];
  char cname[25];
  char newColor[1];
  
  debugprintln((String)freeRam());
  
  debugprint("Starting tinkerlicht-parse-fb.py... ");
  p.runShellCommand("/mnt/sda1/arduino/tinkerlicht-parse-fb.py");
  debugprintln("Done.");
 
  Bridge.get("color1", message, 6);
  String color1 = "";
  for (int i=0; i<6; i++) {
    color1 += message[i];
  }
  
  debugprintln("Color1 from bridge: " + color1);
  
  Bridge.get("color2", message, 6);
  String color2 = "";
  for (int i=0; i<6; i++) {
    color2 += message[i];
  }
  
  debugprintln("Color2 from bridge: " + color2);
  
  Bridge.get("newColor", newColor, 1);
  if (newColor[0] == '1') {
    nRun = 0;
    debugprintln("New color!");
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

  uint16_t n = strip.numPixels();

  for(uint16_t i=0; i<n; i++) {
    float factor1 = 1 - (float)i/(n - 1);
    float factor2 = (float)i/(n - 1);
    debugprintln("Factors: " + String(factor1) + " " + String(factor2));
    byte r = r1*factor1 + r2*factor2;
    byte g = g1*factor1 + g2*factor2;
    byte b = b1*factor1 + b2*factor2;
    debugprintln(String(r) + " " + String(g) + " " + String(b));
    strip.setPixelColor(i, strip.Color(r, g, b));
  }
  strip.show();  
}

void tweetPhoto() {
  Process p; 
  debugprint("Taking picture and tweeting... ");
  p.runShellCommand("/mnt/sda1/arduino/tinkerlicht-tweetphoto.py");  
  debugprintln("Done.");
}

//*********************
//* Utility functions *
//*********************

void debugprint(String s) {
  if (DEBUG) Serial.print(s);
  //if (DISPLAY) lcdprint(s);
}

void debugprintln(String s) {
  if (DEBUG) Serial.println(s);
  //if (DISPLAY) lcdprint(s);
}

/*
void lcdprint(String s) {
  lcd.clear();
  lcd.setCursor(0,0);
  for (int i=0; i< min(s.length(),20); i++) {
    lcd.write(s[i]);
  }
  if (s.length() > 20) {
    lcd.setCursor(0,1);
    for (int i=20; i<s.length(); i++) {
      lcd.write(s[i]);
    }
  } 
}
*/

void waitToContinue(int duration) {
  debugprintln("In waitToContinue()");
  int step = 1;
  for(int i = duration; i>0; i-=step) {
    digitalWrite(LED_PIN, HIGH);
    delay(step*500);
    digitalWrite(LED_PIN, LOW);
    debugprint(String(i) + " ");
    delay(step*500);
  }
  debugprintln("");
}

String getDateTime() {
  debugprintln("In getDateTime()");
  Process p;
  p.runShellCommand("/mnt/sda1/arduino/getdatetime.py");
  
  String dateTime = "";
  while (p.available() > 0) {
    dateTime += (char)p.read();
  }
  dateTime.trim();
  debugprintln(dateTime);
  return dateTime;
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

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

int freeRam() {
  extern int __heap_start, *__brkval; 
  int v; 
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval); 
}
