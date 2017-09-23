/*
 * Some experiments with AVR timers.
 */

// From https://arduino-info.wikispaces.com/Timers-Arduino

#include <Arduino.h>

int main()
{
    DDRA = B11111111; // Initialize pins 22-29 for output

    // Initialize timer1
    noInterrupts();
    TCCR1A = 0;
    TCCR1B = 0;

    TCNT1 = 34286;            // preload timer 65536-16MHz/256/2Hz
    TCCR1B |= (1 << CS12);    // 256 prescaler 
    TIMSK1 |= (1 << TOIE1);   // enable timer overflow interrupt
    interrupts();             // enable all interrupts

    while (true) {}

    return 0;
}

// interrupt service routine that wraps a user defined function 
// supplied by attachInterrupt
ISR(TIMER1_OVF_vect)        
{
  TCNT1 = 34286;            // preload timer
  //digitalWrite(ledPin, digitalRead(ledPin) ^ 1);
  PORTA = PORTA ^ B11111111;
}