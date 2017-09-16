//timer interrupts
//by Amanda Ghassaei
//June 2012
//https://www.instructables.com/id/Arduino-Timer-Interrupts/

/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
*/

//timer setup for timer0, timer1, and timer2.

//timer1 will interrupt at 1Hz

//storage variables
boolean toggle1 = 0;

void setup(){
  
  //set pins as outputs
  pinMode(2, OUTPUT);

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

ISR(TIMER1_COMPA_vect)
{
//occurs w/frequency of 1Hz
  if (toggle1){
    digitalWrite(2,HIGH);
    toggle1 = 0;
  }
  else{
    digitalWrite(2,LOW);
    toggle1 = 1;
  }
}


void loop(){
  //do other things here
}
//  
//}
