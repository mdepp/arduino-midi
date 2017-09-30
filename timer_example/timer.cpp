/*
 * Some experiments with AVR timers.
 */

#include <avr/io.h>
#include <avr/interrupt.h>


int main()
{
  // Adapted from http://www.engblaze.com/microcontroller-tutorial-avr-and-arduino-timer-interrupts/
  
  // Initialize pin 13 for output
  DDRB = B10000000;

  // Initialize timer 1
  cli(); // Disable global interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  OCR1A = 15624; // Set compare match register to desired timer count
  TCCR1B |= (1 << WGM12); // Turn on CTC mode
  //TIMSK1 = (1 << TOIE1); // Enable overflow interrupts
  TCCR1B |= (1 << CS10) | (1 << CS12); // clk/1024 scaling
  TIMSK1 |= (1 << OCIE1A); // Enable compare interrupt
  sei(); // Enable global interrupts

  while (true) {}
}

ISR(TIMER1_COMPA_vect) // Called when timer1 matches comparison
{
  PORTB ^= B10000000; // Toggle LED
}