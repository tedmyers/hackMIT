#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Function prototypes
void init_timer1(void);

// Global Variables
volatile uint8_t counter;
uint8_t node_number, faction, happiness_level;
char data[32] = {0};
volatile int nCounter = 1;
//char command_string[256] = {0};

// leds
#define PIN            6
#define NUMPIXELS      5


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// For screensaver
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";
const uint16_t identifier;

CRGB leds[NUMPIXELS]; //Array object to hold leds.
CRGB basicColor;
CRGB factionColor;
int brightness;

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  randomSeed(analogRead(0));

  //set initial color

  basicColor = CRGB(random(255), random(255),random(255));
  
  init_timer1();
  FastLED.addLeds<NEOPIXEL,PIN>(leds, NUMPIXELS); //Initialize leds

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  pinMode(2, OUTPUT);

  //Generate identifier, from 0 to 2048
  identifier = random(2048); 

  // OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  //Initialize text, show temporary splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Display Test - waiting for input");
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.println("Insert message here!");
  display.display();
  
  delay(500);
}

ISR(TIMER1_COMPA_vect)
{
  //occurs w/frequency of 2Hz
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();

  // Send command info string here
  counter++;
  const char text[] = "Hello World";
  char counter_string[16] = {0};
  sprintf(counter_string, "Hello World: %d", counter);
  radio.write(&counter_string, sizeof(counter_string));

  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

void loop() {
  
  // Send this at 2Hz to all nodes
  // data, node_number, faction, happiness level
  // change later
  //node_number=1;faction=2;happiness_level=3;
  //sprintf(command_string,"%c,%d,%d,%d\n\r",data,node_number,faction,happiness_level);
  
 
  }
  
  if (radio.available()) {
    char* recv_text;
    recv_text = (char *) malloc(32);
    strcpy(recv_text,"test data");
    
    radio.read(&recv_text, sizeof(recv_text));
    digitalWrite(2, HIGH); // flash LED

    // display stuff
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.print(recv_text);
    display.display();
    //end display stuff

    free(recv_text);
    
    delay(50);
    nCounter++;
  }

  digitalWrite(2, LOW); // turn off LED
}


/*  Functions */

void init_timer1()
{
  cli();//stop interrupts

  //set timer1 interrupt at 2Hz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;//initialize counter value to 0
  // set compare match register for 1hz increments
  OCR1A = 7811;// = (16*10^6) / (2*1024) - 1 (must be <65536)
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS12 and CS10 bits for 1024 prescaler
  TCCR1B |= (1 << CS12) | (1 << CS10);  
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}

//Function sets all leds the indicated color. Useful for blackouts or washes.
void setAll(CRGB color) {
  for (int i = 0; i < NUMPIXELS; i++)
  {
    leds[i] = color;
  }
  FastLED.show();
}

//Function called for the "birth" of the star - plays boot anim. Hardcoded boot anim. Assumes LED 3 is mid.
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

  //Loop a ring around, setting values according to a randomly chosen color and faction. Start at 50% brightness (128).
  for (int i = 0; i < NUMPIXELS; i++)
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
  
}
