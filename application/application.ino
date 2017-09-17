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

// Global Variables
volatile uint8_t counter;
uint8_t node_number, faction, happiness_level;
volatile int nCounter = 1;
volatile bool transmit_flag = 0;
//char command_string[256] = {0};

// neopixel
#define PIN            6
#define NUMPIXELS      3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// For screensaver
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

RF24 radio(7, 8); // CNS, CE
const byte address[6] = "00001";

void setup() {
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  init_timer1();
  pixels.begin(); // Initialize NeoPixel Library

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  pinMode(2, OUTPUT);

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

// Occurs 2kHz
ISR(TIMER1_COMPA_vect)
{
  transmit_flag = 1;
}

void loop() {
  
  // Send this at 2Hz to all nodes
  // data, node_number, faction, happiness level
  // change later
  //node_number=1;faction=2;happiness_level=3;
  //sprintf(command_string,"%c,%d,%d,%d\n\r",data,node_number,faction,happiness_level);
  
  // For now, just test colors
  for(int i=0;i<NUMPIXELS;i++){
    //RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(255,0,0));
    pixels.show(); // This sends the updated pixel color to the hardware.
  }
  
  if (radio.available()) {
    char* recv_text = (char *) malloc(32);
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

  if (transmit_flag)
  {
    //occurs w/frequency of 2Hz
    radio.openWritingPipe(address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.stopListening();
  
    // Send command info string here
    counter++;
    const char text[] = "Hello World";
    char* counter_string = (char *) malloc(32);
    sprintf(counter_string, "Hello World: %d", counter);
    radio.write(&counter_string, sizeof(counter_string));
    free(counter_string);
  
    radio.openReadingPipe(0, address);
    radio.setPALevel(RF24_PA_HIGH);
    radio.startListening();
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
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);

  sei();//allow interrupts
}

