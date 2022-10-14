// This #include statement was automatically added by the Particle IDE.
#include <neopixel.h>

#include "application.h"
#include <neopixel.h>

#define PIN            D0
#define NUMPIXELS      100
#define MIN_IDX        0
#define SPEED 80

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, WS2812B);
uint32_t blackColor = Adafruit_NeoPixel::Color(0, 0, 0);
int flickerDelay = 10;
int flickerBase = 15;
int flickerMax = 30;
int colorBaseMax = 4;
int intensityMax = 70;
int stutterAmount = 10;
int stutterOdds = 8;
int flickerAllOdds = 8;
int skipOdds = 4;
int flickerAllNum = 8;
int flickerAllMillis = 40;

struct ColorBase {
    float redBase;
    float greenBase;
    float blueBase;
};

ColorBase colorBases[NUMPIXELS];

void setup() {
//  STARTUP(WiFi.selectAntenna(ANT_INTERNAL));
//  WiFi.off();
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

void flickerUp(int index, int from, int to, ColorBase base, float intensityIncrement, bool doStutter);

void stutter(int index, int flickerNum, ColorBase base, float intensityIncrement) {
    if (random(stutterOdds) == 1) {
        int downPos = flickerNum - random(stutterAmount);
        flickerDown(index, flickerNum, downPos, base, intensityIncrement);
        flickerUp(index, downPos, flickerNum, base, intensityIncrement, false);
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
        stutter(index, flickerNum, base, intensityIncrement);
      }
    }
}

/**
 * illuminate group of lights using intensity and saturation effects
 */
void illuminateAll(int minIdx, int maxIdx, float intensity, int saturation) {
    for (int idx = minIdx; idx < maxIdx; idx++) {
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
void flickerAll(int minIdx, int maxIdx, int numFlickers, int flickerDelay) {
    for (int pulseNum = 0; pulseNum < numFlickers; pulseNum++) {
        illuminateAll(minIdx, maxIdx, random(intensityMax), 0);
        delay(random(flickerDelay));
    }
    illuminateAll(minIdx, maxIdx, intensityMax, 0);
}

/**
 * slowly saturate brightness 
 */
void saturateAll(int minIdx, int maxIdx, int repeat, int increment, int pulseDelay) {
    for (int i=0; i < repeat; i++) {
        for (int offset = 0; offset < 255; offset+=increment) {
            illuminateAll(minIdx, maxIdx, intensityMax, offset);
            delay(pulseDelay);
        }
        for (int offset = 255; offset > 0; offset-=increment) {
            illuminateAll(minIdx, maxIdx, intensityMax, offset);
            delay(pulseDelay);
        }
    }
}

/**
 * establish a random color for all pixels
 */
void randomizeAllColors() {
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
    colorBases[idx].redBase = random(colorBaseMax);
    colorBases[idx].greenBase = random(colorBaseMax); 
    colorBases[idx].blueBase = random(colorBaseMax); 
        // sometimes skip an led leaving it dark
    if (random(skipOdds) == 0) {
      colorBases[idx].redBase = 0;
      colorBases[idx].greenBase = 0; 
      colorBases[idx].blueBase = 0; 
    }

  }
}

/*
 * 
 */
void turnAllOff() {
  uint32_t black = Adafruit_NeoPixel::Color(0, 0, 0 );
  for (int idx = 0; idx < NUMPIXELS; idx++) { 
   	pixels.setPixelColor(idx, black);
  }
  pixels.show();
}

/*
 * go through lights from last idx to start and light them in a strange way
 */
void beStrange() {
  // slowly light each LED in order
  for (int idx = NUMPIXELS-1; idx >= MIN_IDX; idx--) { 
    int numFlickers = random(flickerMax-flickerBase) + flickerBase;
    float intensityIncrement = (float)intensityMax / (float)numFlickers;
    
    // flicker the current led from dark up to full intensity
    flickerUp(idx, 0, numFlickers, colorBases[idx], intensityIncrement, true);

    delay(random(SPEED) + 50);
     
    // sometimes flicker all illuminated leds 
    if (random(flickerAllOdds) == 0) {
      flickerAll(idx, NUMPIXELS-1, flickerAllNum, flickerAllMillis); 
    }
  }    
}

void loop() {	 
//  delay(random(10000));
  randomizeAllColors();

  beStrange();
  
  // do strange things to all the LEDS
  saturateAll(MIN_IDX, NUMPIXELS, 3, 6, 20);
  turnAllOff();
}
