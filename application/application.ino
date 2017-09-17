#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <FastLED.h>
#include <Pitches.h>
s
#ifdef __AVR__
  #include <avr/power.h>
#endif

// Function prototypes
void init_timer1(void);
void display_splash(void);
void choose_faction(void);
void game_start(void);

// Global Variables
volatile uint8_t counter;
uint8_t node_number, faction, happiness_level;
char data[32] = {0};
volatile int nCounter = 1;
char recv_text[64];
volatile bool transmit_flag = 0;
char command_string[32] = {0};
bool start_transmitting = 0;
uint16_t iii; // counter variable

#define BUTTON_PIN 3

#define UNDEF_FACTION 0
#define RED_FACTION   1
#define BLUE_FACTION  2

#define MAX_NODES 3

// Struct with node data
struct node {
  char data[8];
//  uint8_t node_number;
  uint8_t faction;
  uint8_t mode;
//  uint32_t last_seen;
} nodes[MAX_NODES];

// leds
#define PIN            6
#define NUMPIXELS      5


#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// graphics?

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };




RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";

CRGB leds[NUMPIXELS]; //Array object to hold leds.
CRGB basicColor;
CRGB factionColor;
int brightness;


//Music 
int happy[] = { NOTE_C4, NOTE_E4, NOTE_G4, NOTE_C5};
int noteDurationsHappy[] = {4, 4, 4, 1.5};

int SAD[] = { NOTE_C5, NOTE_B4, NOTE_AS4};
int noteDurationsSad[] = {2, 2, 1};


int INTRO[] = {NOTE_C4, NOTE_E4, NOTE_G4, NOTE_D4, NOTE_F4, NOTE_A4, NOTE_E4, NOTE_G4, NOTE_B4, NOTE_C5};
int noteDurationsIntro[] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 1};

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif


//  // miniature bitmap display
//  display.clearDisplay();
//  display.drawBitmap(32, 0,  sun16_glcd_bmp, 64, 64, 1);
//  display.display();
//  while(1); // remove later

//  //Generate identifier, from 0 to 2048
//  pinMode(0, INPUT);
//  randomSeed(analogRead(0));
//  nodes[0].node_number = random(2048);
  

  randomSeed(analogRead(0));

  //set initial color

  basicColor = CRGB(random(255), random(255),random(255));
  if (node[0].faction == 1) { //red
    factionColor = CRGB::Red;
  }
  else { //blue
    factionColor = CRGB::Blue;
  }

  brightness = 128; //half brightness, level 5
  
  init_timer1();
  FastLED.addLeds<NEOPIXEL,PIN>(leds, NUMPIXELS); //Initialize leds

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
  
  pinMode(2, OUTPUT); //LED
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  
  // OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr for the 128x64
  
  //Initialize text, show temporary splash screen
  display_splash();
  delay(500);

  // Ask for faction
  choose_faction();
  delay(1000); // pause for a sec

  // Start animations
  game_start();

  start_transmitting = 1; //remove later
}

ISR(TIMER1_COMPA_vect)
{
  transmit_flag = 1;
}

void loop() {
  

  // Ask to choose faction
  // 

  // For now just test colors
  for(int i=0;i<NUMPIXELS;i++){
    //RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(255,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.

 

  }
  
  if (radio.available()) {
    
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
    
    //delay(50);
    nCounter++;

    //todo: note address, record millis last seen
  }

  digitalWrite(2, LOW); // turn off LED

  if (transmit_flag)
  {  
    //occurs w/frequency of 2Hz
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.stopListening();
  
    // Send command info string here
    counter++;
    const char text[] = "Hello World";
    char counter_string[32] = {0};
    sprintf(counter_string, "%s,%d,%d", text, nodes[0].faction,nodes[0].mode);
    radio.write(&counter_string, sizeof(counter_string));
  
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening();
    transmit_flag = 0;
  }

  // Send this at 2Hz to all nodes
  // data, node_number, faction, mode
  if (start_transmitting)
  {
      // enable timer compare interrupt (transmission)
      TIMSK1 |= (1 << OCIE1A);
      start_transmitting = 0;
  }
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
//  // enable timer compare interrupt
//  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}


void display_splash(void)
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setTextColor(WHITE);
  display.setCursor(30,0);
  display.println("NOVA");
  display.setTextSize(1.5);
  display.setTextColor(BLACK, WHITE);
  display.setCursor(22,24);
  display.println("A Star is Born");
  display.display();

  while ( digitalRead(BUTTON_PIN) == HIGH ) {};
  
}

// Need to add LED functions in
// I don't think this code works yet - always chooses blue
void choose_faction(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.println("Choose a Faction");
  display.setTextSize(1.5);
  display.setCursor(20,40);
  display.print("RED");
  display.setCursor(80,40);
  display.println("BLUE");
  display.display();

  // cycle between colors every 1s
  nodes[0].faction = UNDEF_FACTION;
  while ( nodes[0].faction == UNDEF_FACTION ) 
  {
    for (iii=0;iii<1000;iii++)
    {
      //TODO: show red LEDs
      if ( digitalRead(BUTTON_PIN) == LOW )
      { 
        nodes[0].faction = RED_FACTION;
        break;
      }
      delay(1);
    }
    
    for (iii=0;iii<1000;iii++)
    {
      //TODO: show blue LEDs
      if ( digitalRead(BUTTON_PIN) == LOW )
      { 
        nodes[0].faction = BLUE_FACTION; 
        break;
      }
      delay(1);
    }
  }

  // Display result and exit
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  
  if ( nodes[0].faction == RED_FACTION )
  {
    display.println("RED Faction Chosen");
  }
  else if ( nodes[0].faction == BLUE_FACTION )
  {
    display.println("BLUE Faction Chosen");
  }
  else
  {
    display.println("no faction chosen");
  }
      display.display();

}

void game_start(void)
{
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.print("A Star is Born");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.print("A Star is Born.");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.print("A Star is Born..");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.print("A Star is Born...");
  display.display();
  delay(1000);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(15,0);
  display.print("A Star is Born....");
  display.display();
  delay(1000);
    
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

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 11; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsIntro[thisNote];
    tone(5, INTRO[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  }


  
  delay(1000);

  normalState();
  
}

void happy() {

  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsHappy[thisNote];
    tone(5, happy[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  }
  
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

void sad() {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < 4; thisNote++) {

    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurationsSad[thisNote];
    tone(5, SAD[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
  }
  int delayTime = 150; //delay time in ms
  //Single dot chase.

  //Write all black.`
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  //Write first yellow
  leds[0] = CRGB::Red;

  FastLED.show();

  //Cycle through to the end, then bounce back twice

  for (int x = 0; x < 1; x++)
  { 
    for (int i = 0; i < numLeds; i++)
    {
      leds[i] = CRGB::Red;
      FastLED.show();
      leds[i] = CRGB::Black;
  
      delay(delayTime);
    }
  
    for (int i = numLeds-2; i > 0; i--)
    {
      leds[i] = CRGB::Red;
      FastLED.show();
      leds[i] = CRGB::Black;
  
      delay(delayTime);
    }
  }

  //Flash all twice
   //Write all yellow.
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Red;
  }

  FastLED.show();

  delay(300);

  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Black;
  }

  FastLED.show();

  delay(300);
  for (int i = 0; i < numLeds; i++)
  {
    leds[i] = CRGB::Red;
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

  normalState();
}

void normalState() {
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

  FastLED.setBrightness(brightness);
  FastLED.show();
}

