#include "application.h"
#include "neopixel/neopixel.h"

#define PIN            3
#define NUMPIXELS      100
#define SPEED          600

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, WS2812B);
uint32_t blackColor = Adafruit_NeoPixel::Color(0, 0, 0);
int flickerDelay = 17;
int flickerMax = 30;
int intensityMax = 13;

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
}

void loop() {	 
  delay(random(15000));
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
    float redBase = random(4); 
    float greenBase = random(4); 
    float blueBase = random(4); 
    int numFlickers = random(flickerMax) + 15;
    for (int flickerNum=0; flickerNum < numFlickers; flickerNum++) {
      float intensity = ((float)intensityMax / (float)numFlickers) * (float)flickerNum; 
      uint32_t curColor = Adafruit_NeoPixel::Color((int)(redBase * intensity) , (int)(greenBase * intensity), (int)(blueBase * intensity) );
	  pixels.setPixelColor(idx, curColor);
      pixels.show();
      delay(random(flickerDelay));
	  pixels.setPixelColor(idx, blackColor);
      pixels.show();
      delay(random(flickerDelay));
    }
    uint32_t finalColor = Adafruit_NeoPixel::Color(redBase * intensityMax , greenBase * intensityMax, blueBase * intensityMax );
	pixels.setPixelColor(idx, finalColor);
    pixels.show();
    delay(random(SPEED) + 50);
  }
  
  delay (random(2000));
  
  uint32_t black = Adafruit_NeoPixel::Color(0, 0, 0 );
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
   	pixels.setPixelColor(idx, black);
  }
  pixels.show();
}
