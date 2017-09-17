#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_NeoPixel.h>
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

// neopixel
#define PIN            6
#define NUMPIXELS      3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

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

/*
static const unsigned char PROGMEM sun64_glcd_bmp[] =
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000011, B00111111, B11110000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000111, B11111111, B11111111, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00001111, B11111111, B11111111, B11000000, B00000000, B00000000,
  B00000000, B00000000, B00111111, B11111111, B11111111, B11110000, B00000000, B00000000,
  B00000000, B00000000, B01111111, B11111111, B11111111, B11111000, B00000000, B00000000,
  B00000000, B00000001, B11111111, B11111111, B11011111, B11111111, B10000000, B00000000,
  B00000000, B00000011, B11111111, B11111111, B11111111, B11111111, B11000000, B00000000,
  B00000000, B00000111, B11111111, B11111111, B11111111, B11111111, B11000000, B00000000,
  B00000000, B00111111, B11111111, B11111111, B11111111, B11111111, B11000000, B00000000,
  B00000000, B00111111, B11111111, B11111111, B11111111, B11111111, B11100000, B00000000,
  B00000000, B00011111, B11110011, B11111111, B11111111, B11111111, B11100000, B00000000,
  B00000000, B00111111, B11111011, B11111111, B11111111, B11111111, B11110000, B00000000,
  B00000000, B01111111, B11111111, B11111111, B11111111, B11111101, B11111000, B00000000,
  B00000000, B01111111, B11111111, B11111111, B11111111, B11111001, B11111000, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111100, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111100, B00000000,
  B00000001, B11111110, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111101, B11111111, B11111111, B10000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B10000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11101111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111111, B11111111, B11111100, B11111111, B00000000,
  B00000011, B11111111, B11111111, B11111100, B11111111, B11111100, B11111111, B00000000,
  B00000011, B11111111, B01111111, B11111101, B11111111, B11111111, B11111111, B00000000,
  B00000011, B11111111, B00111111, B11111111, B11111111, B11111111, B11111111, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000001, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111100, B00000000,
  B00000000, B01111111, B11111111, B11111111, B11111111, B11111011, B11111110, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111110, B00000000,
  B00000000, B11111111, B11111111, B11111111, B11111111, B11111111, B11111100, B00000000,
  B00000000, B00011111, B11111101, B11111111, B11111111, B11111111, B11100000, B00000000,
  B00000000, B00011111, B11111111, B11111111, B11111111, B11111111, B11100000, B00000000,
  B00000000, B00001111, B11111111, B11111111, B11111111, B11111111, B11000000, B00000000,
  B00000000, B00000111, B11111111, B11111111, B11110011, B11111111, B11000000, B00000000,
  B00000000, B00000011, B11111111, B11111111, B11110111, B11111111, B00000000, B00000000,
  B00000000, B00000001, B11111111, B11111111, B11111111, B11111110, B00000000, B00000000,
  B00000000, B00000000, B01111111, B11111111, B11111111, B11111100, B00000000, B00000000,
  B00000000, B00000000, B00111111, B11111111, B11111111, B11110000, B00000000, B00000000,
  B00000000, B00000000, B00001111, B11111111, B11111111, B11000000, B00000000, B00000000,
  B00000000, B00000000, B00001111, B11111111, B11111111, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00001111, B00111111, B11110000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000110, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000,
  B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000, B00000000 };
  */


RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";

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
  
  init_timer1();
  pixels.begin(); // Initialize NeoPixel Library
  
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

