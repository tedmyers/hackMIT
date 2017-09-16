/*
* Arduino Wireless Communication Tutorial
*     Example 1 - Transmitter Code
*                
* by Dejan Nedelkovski, www.HowToMechatronics.com
* Library: TMRh20/RF24, https://github.com/tmrh20/RF24/
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); // CNS, CE

const byte address[6] = "00001";

uint8_t counter = 0;
char counter_string[16] = {0};

void setup() {
  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_HIGH);
  radio.stopListening();
}

void loop() {
  counter++;
  const char text[] = "Hello World";
  sprintf(counter_string, "Hello World: %d", counter);
  radio.write(&counter_string, sizeof(counter_string));
  delay(500);
}
