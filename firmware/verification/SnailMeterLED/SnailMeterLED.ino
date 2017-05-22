/*
 * Setup LED strip by receive serial command 57600 N81
 * Command list:
 *  'R' - setup LED to Red 
 *  'G' - setup LED to Green
 *  'B' - setup LED to Blue
 * Hardware Connection: 
 *  DI- pin 11
 *  CL - pin 12
*/
#include <APA102.h>

// Define which pins to use.
const uint8_t dataPin = 11;
const uint8_t clockPin = 12;

// Create an object for writing to the LED strip.
APA102<dataPin, clockPin> ledStrip;

// Set the number of LEDs to control.
const uint16_t ledCount = 7;

// Create a buffer for holding the colors (3 bytes per color).
rgb_color colors[ledCount];

// Set the brightness to use (the maximum is 31).
const uint8_t brightness = 1;

rgb_color Rgb(uint8_t r, uint8_t g, uint8_t b)
{
  return (rgb_color){r, g, b};
}
/* Converts a color from HSV to RGB.
 * h is hue, as a number between 0 and 360.
 * s is the saturation, as a number between 0 and 255.
 * v is the value, as a number between 0 and 255. */
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v)
{
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch((h / 60) % 6){
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }
    return rgb_color(r, g, b);
}


void setup() {
  Serial.begin(115200); // open serial connection to USB Serial port
  Serial1.begin(57600); // open internal serial connection to MT7688AN
  pinMode(13, OUTPUT);
}

void set_color(uint8_t r, uint8_t g, uint8_t b){
  for(uint16_t i = 0; i < ledCount; i++)
  {
    colors[i] = Rgb(r,g,b);

  }

  ledStrip.write(colors, ledCount, brightness);
  
}
void loop() {
   int c = Serial1.read();
   if (c != -1) {
      switch(c) {
        case '0':
           digitalWrite(13, 0);
           break;
        case '1':
           digitalWrite(13, 1);
           break;
        case 'R':
          set_color(255,0,0);
          break;
        case 'G':
          set_color(0,255,0);
          break;
        case 'B':
          set_color(0,0,255);
          break;
        
      break; }
    } 
}

