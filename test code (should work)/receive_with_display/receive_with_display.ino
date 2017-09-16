#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// For screensaver
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// stuff
volatile int nCounter = 1;

RF24 radio(7, 8); // CNS, CE

const byte address[6] = "00001";

void setup() {
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
    //nCounter++;
  }

  //displayScreen();

  digitalWrite(2, LOW); // turn off LED
}

// Displays nSides, nCounter, and nResult
void displayScreen()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  display.print(nCounter);
  
  display.display();
  
}
