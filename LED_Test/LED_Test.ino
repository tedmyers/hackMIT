#include <FastLED.h>

#define numLeds 6
#define LED_Pin 5

CRGB factionColor;
CRGB basicColor;

CRGB leds[numLeds];

void setup() {
  //Setup LEDs
  randomSeed(analogRead(5));
  FastLED.addLeds<NEOPIXEL,LED_Pin>(leds, numLeds);

  //write initial state
  for (int i=0; i < numLeds; i++)
  {
    if (i%2 == 0) {leds[i] = CRGB::Red;}
    else {leds[i] = CRGB::Blue;}
    FastLED.show();
  }
 //set initial faction and body colors
  if (int(random(2)) == 1) {factionColor = CRGB::Red;}
  else {factionColor = CRGB::Blue;}

  basicColor = CRGB(random(255), random(255),random(255));
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int brightness = 255;
 // setAll(CRGB(random(255), random(255),random(255)));
  delay(500);
  bootAnim();
 // happy();
}

void happy() {
  int delayTime = 150; //delay time in ms
  //Single dot chase.

  //Write all black.`
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  //Write first yellow
  leds[0] = CRGB::Yellow;

  FastLED.show();

  //Cycle through to the end, then bounce back twice

  for (int x = 0; x < 1; x++)
  { 
    for (int i = 0; i < numLeds; i++)
    {
      leds[i] = CRGB::Yellow;
      FastLED.show();
      leds[i] = CRGB::Black;
  
      delay(delayTime);
    }
  
    for (int i = numLeds-2; i > 0; i--)
    {
      leds[i] = CRGB::Yellow;
      FastLED.show();
      leds[i] = CRGB::Black;
  
      delay(delayTime);
    }
  }

  //Flash all twice
   //Write all yellow.
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Yellow;
  }

  FastLED.show();

  delay(300);

     //Write all yellow.
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  FastLED.show();

  delay(300);

     //Write all yellow.
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Yellow;
  }

  FastLED.show();

  delay(300);

     //Write all yellow.
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  FastLED.show();

  delay(300);
}
void setAll(CRGB color) {
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}
void bootAnim() {
 FastLED.setBrightness(50); //very faint start
  //blackout
  setAll(CRGB::Black);
  
  delay(500);

  //Single faint red around display (1 and 5) ((subject to changeeee))
  leds[0] = CRGB::Red;
  leds[4] = CRGB::Red;
  FastLED.show();

  delay(350); //flicker
  setAll(CRGB::Black);
  delay(75);
  
  leds[0] = CRGB::Red;
  leds[4] = CRGB::Red;
  FastLED.show();

  delay(400);

  FastLED.setBrightness(100);
  FastLED.show();
  delay(300);
  
  FastLED.setBrightness(80);
  FastLED.show();
  delay(150);
  
  FastLED.setBrightness(200);
  FastLED.show();
  delay(1500);

  //Flicker
  FastLED.setBrightness(20);
  FastLED.show();
  delay(50);

  FastLED.setBrightness(200);
  FastLED.show();
  delay(50);

  FastLED.setBrightness(20);
  FastLED.show();
  delay(50);

  FastLED.setBrightness(200);
  FastLED.show();
  delay(50);

  CRGB Or = 0xFF8000;
  //Inner LEDs orange, outer red
  leds[0] = Or;
  leds[4] = Or;
  leds[1] = CRGB::Red;
  leds[3] = CRGB::Red;
   FastLED.setBrightness(200);
  FastLED.show();
  delay(1250);

  setAll(CRGB::Black);
  FastLED.show();
  delay(50);
  setAll(0xFF8000);
  FastLED.setBrightness(255);

  FastLED.show();

  delay(2500);

  //Loop a ring around, setting values according to a randomly chosen color and faction. Start at 50% brightness (128)./
  
  //Loop a ring around, setting values according to a randomly chosen color and faction. Start at 50% brightness (128).
  for (int i = 0; i < numLeds; i++)
  {
    if (i == 2)
    {
      leds[i] = factionColor;
      continue;
    }
    leds[i] =  basicColor;

    FastLED.show();
  
    delay(75);
  
  }
 
delay(5000); 

}

