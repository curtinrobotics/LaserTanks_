#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
//#include "Powerups.h"
#include "Animations.h"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, PIN, NEO_GRB);
uint32_t powerUpCol[SOLID_COLOURS] = {0,0xFF0000,0xFF2000,0xFF7000,0x00FF00,0x00A0C0,0x0000FF,0xFF0060,0xFFC87D};

void ring_init(void) {
  strip.begin(); // this innitilises the light for the powerUp
  for(int i = 0; i < COLOUR_NUM;i++) {
    setRing(i);
    delay(150);
  }
   setRing(POWERUP_NONE);
}

void collect(int type, bool hidden) {
  setRing(type);
  pulseFlash(POWERUP_CLEANSE);
  if(hidden) {
    for(int j = 0; j<4; j++) {
      for(int i = 0; i < COLOUR_NUM-1;i++) {
        setRing(i);
        delay(50);
      }
    }
    rainbowCycle(0);
  }
  if(type < COLOUR_NUM) {
    theaterChase(type, 80);
    //setRing(Powerup_type);
  }
  //todo hidden/random and flag animations
  setRing(POWERUP_NONE);
}

void colorWipe(int type, uint16_t cooldown) //todo prevent new commands while animating
{
  uint32_t c = powerUpCol[type];
  uint8_t red = (c>>16) & 0xFF;
  uint8_t green = (c>>8) & 0xFF;
  uint8_t blue = c & 0xFF;
  //Serial.println(cooldown); //DEBUG
  Serial.print("Spawning");
  uint32_t pixelTime = cooldown*1000/16-(15*32);
  for(uint16_t i=0; i<16; i++) 
  {
    //Serial.print("Start: ");
    //Serial.print(millis());
    delay(pixelTime);
    //Serial.print("  Finish: ");
    //Serial.println(millis());
    Serial.print(".");
    //Fade in each pixel
    for(int j = 1; j<=32;j++) {
      strip.setPixelColor(i, red/32*j,green/32*j,blue/32*j);
      strip.show();
      delay(15);
    }
  }
  Serial.println();
}

void setRing(int type) 
{
  for(int i=0;i<16;i++)
  {
    strip.setPixelColor(i, powerUpCol[type]); 
  }
  strip.show();
}

void ringFlash(int type, uint16_t rate, int times) 
{
  for(int i=0; i < times; i++) 
  {
    setRing(powerUpCol[type]);
    delay(rate/2);
    setRing(COLOUR_WHITE);
    delay(rate/2);
  }
}

void dualTone(int type) { //todo set animation
  uint32_t colour_1;
  uint32_t colour_2;

  if(type == SPAWN_WARM) {
    colour_1 = COLOUR_RED;
    colour_2 = COLOUR_YELLOW;
    Serial.println("Warm");
  }
  else if(type == SPAWN_COOL) {
    colour_1 = COLOUR_GREEN;
    colour_2 = COLOUR_BLUE;
    Serial.println("Cool");
  }
  for(int i=0;i<16;i+=2)
  {
    strip.setPixelColor(i, powerUpCol[colour_1]); 
  }
  for(int i=1;i<16;i+=2)
  {
    strip.setPixelColor(i, powerUpCol[colour_2]); 
  }
  strip.show();
}

void pulseFlash(int type) 
{
  strip.setBrightness(255);
  for(int i = 0;i<16;i++) {
    strip.setBrightness(256-i*16);
    for(int j=0;j<16;j++)
    {
      strip.setPixelColor(j, powerUpCol[type]); 
      strip.show();
    }
    delay(1);
  }
  strip.setBrightness(255);
  setRing(powerUpCol[type]);
}

void ringBlink(int type, uint16_t rate, int times) 
{
  for(int i=0; i < times; i++) 
  {
    setRing(powerUpCol[type]);
    strip.show();
    delay(rate/2);
    setRing(COLOUR_OFF);
    strip.show();
    delay(rate/2);
  }
}

void theaterChase(int type, uint8_t wait) {
  uint32_t c = powerUpCol[type];
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 2; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+2) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+2) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void rainbowWipe(uint8_t cooldown) {
  uint16_t static i, j;
//  for(i=0; i< strip.numPixels(); i++) {
//    for(int j = 1; j<=32;j++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels())) & (255)));    
//      strip.show();
//      delay(wait/32);
//    }
//   }
    uint32_t pixelTime = cooldown*1000/16-(15*32);
    for(uint16_t i=0; i<16; i++) 
    {
    //Serial.print("Start: ");
    //Serial.print(millis());
    delay(pixelTime);
    //Serial.print("  Finish: ");
    //Serial.println(millis());
    Serial.print(".");
    //Fade in each pixel
    for(int j = 1; j<=32;j++) {
      uint32_t hue = Wheel(((i * 256 / strip.numPixels())) & (255));
      uint8_t red = (hue >> 16) & 0xFF;
      uint8_t green = (hue >> 8) & 0xFF;
      uint8_t blue = hue & 0xFF;
      strip.setPixelColor(i, red/32*j,green/32*j,blue/32*j);
      //strip.setPixelColor(i, );
      //strip.setBrightness(255/32*j);
      strip.show();
      delay(15);
    }
  }
  strip.setBrightness(0);
  strip.show();
  delay(60);
  strip.setBrightness(255);
  strip.show();
}

void rainbowCycle(uint8_t wait) {
  uint16_t static i, j;
  if(wait == 0) {
    i = 0;
    j = 0;
    return;
  }

  //for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    if(j == 0) j = 255;
    j--;
    delay(wait);
  //}
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
