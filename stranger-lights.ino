#include "application.h"
#include "neopixel/neopixel.h"

#define PIN            3
#define NUMPIXELS      100
#define SPEED          100

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, WS2812B);
uint32_t blackColor = Adafruit_NeoPixel::Color(0, 0, 0);
int flickerDelay = 12;
int flickerMax = 11;
int intensityMax = 40;
int stutterAmount = 5;
int stutterOdds = 8;
int flickerAllOdds = 10;

struct ColorBase {
    float redBase;
    float greenBase;
    float blueBase;
};

ColorBase colorBases[NUMPIXELS];

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
}

/**
 * decrease intensity of light in a creepy way
 */
void flickerDown(int index, int from, int to, ColorBase base, float intensityIncrement) {
    for (int flickerNum=from; flickerNum > to; flickerNum--) {
      float intensity = intensityIncrement * (float)flickerNum; 
      uint32_t curColor = Adafruit_NeoPixel::Color((int)(base.redBase * intensity) , (int)(base.greenBase * intensity), (int)(base.blueBase * intensity) );

	  pixels.setPixelColor(index, curColor);
      pixels.show();
      delay(random(flickerDelay));

	  pixels.setPixelColor(index, blackColor);
      pixels.show();
      delay(random(flickerDelay));
    }
}

/**
 * increase intensity of individual LED in a creepy way
 */
void flickerUp(int index, int from, int to, ColorBase base, float intensityIncrement, bool doStutter) {
    // increase intensity in increments
    for (int flickerNum=from; flickerNum < to; flickerNum++) {
      float intensity = intensityIncrement * (float)flickerNum; 
      uint32_t curColor = Adafruit_NeoPixel::Color((int)(base.redBase * intensity) , (int)(base.greenBase * intensity), (int)(base.blueBase * intensity) );

      // black out for random period
      pixels.setPixelColor(index, blackColor);
      pixels.show();
      delay(random(flickerDelay));

      // show color at current intensity for random period
      pixels.setPixelColor(index, curColor);
      pixels.show();
      delay(random(flickerDelay));
      
      // stutter effect is to sometimes fade slightly back down then up again
      if (doStutter) {
        if (random(stutterOdds) == 1) {
          int downPos = flickerNum - random(stutterAmount);
          flickerDown(index, flickerNum, downPos, base, intensityIncrement);
          flickerUp(index, downPos, flickerNum, base, intensityIncrement, false);
        }
      }
    }
}

/**
 * illuminate group of lights using intensity and saturation effects
 */
void illuminateAll(int maxIdx, float intensity, int saturation) {
    for (int idx = 0; idx < maxIdx; idx++) {
        int r = min(colorBases[idx].redBase * intensity + saturation, 255);
        int g = min(colorBases[idx].greenBase * intensity + saturation, 255);
        int b = min(colorBases[idx].blueBase * intensity + saturation, 255); 
        uint32_t curColor = Adafruit_NeoPixel::Color(r, g, b);
        pixels.setPixelColor(idx, curColor);
    }
    pixels.show();
}


/**
 * flicker group of lights to random inensity
 */
void flickerAll(int maxIdx, int numFlickers, int flickerDelay) {
    for (int pulseNum = 0; pulseNum < numFlickers; pulseNum++) {
        illuminateAll(maxIdx, random(intensityMax), 0);
        delay(random(flickerDelay));
    }
    illuminateAll(maxIdx, intensityMax, 0);
}

/**
 * slowly saturate brightness 
 */
void saturateAll(int maxIdx, int repeat, int increment, int pulseDelay) {
    for (int i=0; i < repeat; i++) {
        for (int offset = 0; offset < 255; offset+=increment) {
            illuminateAll(NUMPIXELS, intensityMax, offset);
            delay(pulseDelay);
        }
        for (int offset = 255; offset > 0; offset-=increment) {
            illuminateAll(maxIdx, intensityMax, offset);
            delay(pulseDelay);
        }
    }
}

/**
 * establish a random color for all pixels
 */
void randomizeAllColors() {
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
    colorBases[idx].redBase = random(5);
    colorBases[idx].greenBase = random(5); 
    colorBases[idx].blueBase = random(5); 
  }
}

void loop() {	 
  delay(random(10000));
  randomizeAllColors();

  // slowly light each LED in order
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
    int numFlickers = random(flickerMax) + 15;
    float intensityIncrement = (float)intensityMax / (float)numFlickers;
    
    // flicker the current led from dark up to full intensity
    flickerUp(idx, 0, numFlickers, colorBases[idx], intensityIncrement, true);

    delay(random(SPEED) + 50);
     
    // sometimes flicker all illuminated leds 
    if (random(flickerAllOdds) == 3) {
      flickerAll(idx, 10, 40); 
    }
    
    // sometimes skip an led leaving it dark
    if (random(10) == 1) {
      idx++;
    }
  }
  
  // do strange things to all the LEDS
  saturateAll(NUMPIXELS, 5, 6, 20);
  delay (random(2000));
  
  // turn all off
  uint32_t black = Adafruit_NeoPixel::Color(0, 0, 0 );
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
   	pixels.setPixelColor(idx, black);
  }
  pixels.show();
}
