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
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

// Function prototypes
void init_timer1(void);
void display_splash(void);
void choose_faction(void);
void game_start(void);
void init_all(void);
void setAll(CRGB color);
void bootAnim(void);
void begin_battle(void);
void parse_text(char * text);

// Global Variables
volatile uint8_t counter;
char data[32] = {0};
volatile int nCounter = 1;
//char * recv_text;
char recv_text[32];
volatile bool transmit_flag = 0;
char command_string[32] = {0};
bool start_transmitting = 0;
uint16_t iii; // counter variable
char counter_string[64] = {0};
const char comma[2] = ",";

// Defines
#define BUTTON_PIN 3
#define RED_FACTION     1
#define BLUE_FACTION    2
#define MAX_NODES       3
#define OLED_RESET      4
#define PIN             6
#define NUMPIXELS       5
#define UNDEF_FACTION   0
#define UNDEF_MODE      0
#define BATTLE_MODE     1 


// Struct with node data
struct node {
  char data[8];
//  uint8_t node_number;
  uint8_t faction;
  uint8_t mode;
//  uint32_t last_seen;
} nodes[MAX_NODES];

Adafruit_SSD1306 display(OLED_RESET);

RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";

CRGB leds[NUMPIXELS]; //Array object to hold leds.
CRGB basicColor;
CRGB factionColor;
int brightness;

void setup() {

  init_all();
  display_splash();
  delay(1000);
  choose_faction();
  delay(1000); // pause for a sec
  randomSeed(analogRead(0));
  basicColor = CRGB(random(255), random(255),random(255));

  // Start animations
  game_start();
  bootAnim();
  start_transmitting = 1;
}

ISR(TIMER1_COMPA_vect)
{
  transmit_flag = 1;
}

void loop() {
  
  if (radio.available()) {
    radio.read(&recv_text, sizeof(recv_text));
    digitalWrite(2, HIGH); // flash LED
    parse_text(recv_text);
  
    //delay(50);
    nCounter++;
  }
  digitalWrite(2, LOW); // turn off LED

  if (transmit_flag)
  { 
    //occurs w/frequency of 2Hz
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.stopListening();
  
    // Send command info string here
    //counter++;
    //const char text[] = "Hello World";
    sprintf(counter_string, "%s,%d,%d", nodes[0].data, nodes[0].faction,nodes[0].mode);
    radio.write(&counter_string, sizeof(counter_string));
  
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening();
    transmit_flag = 0;
  }

  // Enable timer to allow transmission
  if (start_transmitting)
  {
      // enable timer compare interrupt (transmission)
      TIMSK1 |= (1 << OCIE1A);
      start_transmitting = 0;
  }
}





/*  Functions */

void parse_text(char text[])
{
  char * tempstr;
  tempstr = strtok(text,comma);
  strcpy(data, tempstr);
  //char* data = tempstr;
  
  char * tempstr_faction;
  tempstr_faction = strtok(text,comma);
  nodes[1].faction = atoi(tempstr_faction);

  char * tempstr_mode;
  tempstr_mode = strtok(text,comma);
  nodes[1].mode = atoi(tempstr_mode);

//  char * tempstr_data;
//  char * tempstr_faction;
//  char * tempstr_mode;
//  sprintf(tempstr_faction, "Faction #%d", faction);
//  sprintf(tempstr_mode, "Mode #%d", mode);
//  char text2[64];
//  sprintf(text2, "data:%s,faction:%d,mode:%d", nodes[1].data, nodes[1].faction,nodes[1].mode);
  
  // display stuff
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.print(text);
  display.println(data);
//  display.println(tempstr_faction);
//  display.println(tempstr_mode);
  display.display();
  //end display stuff
}

void new_node(void)
{
  if ( nodes[1].faction == nodes[0].faction )
  { // gain +1 brightness
    
  }
  else // opposing faction
  {
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print("You face an opposing faction. Do you choose to fight?");
    display.display();

    
     if ( digitalRead(BUTTON_PIN) == LOW )
  {
    iii=0;
    while ( digitalRead(BUTTON_PIN) == LOW )
    {
      iii++;
      delay(1);
      if (iii>=3000)
      {
        nodes[0].mode = BATTLE_MODE;
      }
    }
  }
  }
}


void init_all(void)
{
  FastLED.addLeds<NEOPIXEL,PIN>(leds, NUMPIXELS); //Initialize leds
  
  pinMode(2, OUTPUT); //LED
  pinMode(BUTTON_PIN, INPUT);
  digitalWrite(BUTTON_PIN, HIGH);
  
  // OLED display
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr for the 128x64
  
  init_timer1();

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();
}

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
      setAll(CRGB::Red);
      if ( digitalRead(BUTTON_PIN) == LOW )
      { 
        nodes[0].faction = RED_FACTION;
        break;
      }
      delay(1);
    }

    if (nodes[0].faction == RED_FACTION)
    { break; }
    
    for (iii=0;iii<1000;iii++)
    {
      setAll(CRGB::Blue);
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
  display.setCursor(0,20);
  
  if ( nodes[0].faction == RED_FACTION )
  {
    display.println("RED");
    display.println("Faction");
  }
  else if ( nodes[0].faction == BLUE_FACTION )
  {
    display.println("BLUE");
    display.println("Faction");
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

  if (nodes[0].faction == RED_FACTION) { //red
    factionColor = CRGB::Red;
  }
  else if (nodes[0].faction == BLUE_FACTION)
  {
    factionColor = CRGB::Blue;
  }
  else
  { //error
  }
  
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

void begin_battle(void)
{
  //TODO: fill

  // Wait for up to (10s?) for another player to enter battle mode


}

