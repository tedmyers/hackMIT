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

// neopixel
#define PIN            6
#define NUMPIXELS      3
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// For screensaver
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// stuff
volatile int nCounter = 1;

RF24 radio(7, 8); // CNS, CE

const byte address[6] = "00001";

void setup() {
  
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif

  pixels.begin(); // This initializes the NeoPixel library.

  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.startListening();

  pinMode(2, OUTPUT);

  //display
  // by default, we'll generate the high voltage from the 3.3v line internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  // Set splash screen to start
  boolean showSplash = 1;
  
  //Initialize text
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Testing");
  display.println("Display Test Program");
  display.setTextSize(1);
  display.setTextColor(BLACK, WHITE);
  display.println("Insert message here!");
  display.display();
  
  delay(2000);
  showSplash = 0;
}

void loop() {

  for(int i=0;i<NUMPIXELS;i++){
    //RGB values, from 0,0,0 up to 255,255,255
    pixels.setPixelColor(i, pixels.Color(255-nCounter,nCounter,255-nCounter));

    pixels.show(); // This sends the updated pixel color to the hardware.
   //delay(delayval); // Delay for a period of time (in milliseconds).
  }
  
  if (radio.available()) {
    char text[32] = "";
    radio.read(&text, sizeof(text));
    Serial.println(text);
    digitalWrite(2, HIGH); // flash LED

    // display stuff
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.print(text);
    display.display();
    //end display stuff
    
    delay(100);
    nCounter++;
  }

  //displayScreen();
  digitalWrite(2, LOW); // turn off LED
}

//// Displays nSides, nCounter, and nResult
//void displayScreen()
//{
//  display.clearDisplay();
//  display.setCursor(0,0);
//  display.setTextSize(2);
//  display.setTextColor(WHITE);
//  
//  display.print(nCounter);
//  
//  display.display();
//  
//}
