
/**********************************
          Dice Roller v0.72
              Ted Myers
              5/25/2015
     
*************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

// stuff
volatile int nRolls = 1;

// limits max/min values of nRolls, nSides, and nModifier
#define MAX_VALUE 100

// For screensaver
#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 

// Splash screen
boolean showSplash;

void setup()
{   
  // by default, we'll generate the high voltage from the 3.3v line internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  
  // Set splash screen to start
  showSplash = 1;
  
  //Initialize text
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
}

void loop() 
{
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.println("Ted's");
    display.println("Dice      Roller");
    display.setTextSize(1);
    display.setTextColor(BLACK, WHITE);
    display.println("Press button to roll!");
    display.display();
    
    delay(2000);
    showSplash = 0;

    while(1)
    {
    displayScreen();
    delay(500);
    nRolls++;
    delay(500);
    }
}

// Displays nSides, nRolls, and nResult
void displayScreen()
{
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  
  display.print(nRolls);
  
  display.display();
  
}

