#include <Arduino.h>

/*
 * Simple test of port manipulation and digital-to-analogue conversion.
 * 
 * Arduino ports 22 through 29 are connected to an R-2R resistor ladder for DAC,
 * (pin 29 is most-significant) the output of which connects to the speaker.
 */

int main()
{
  DDRA = B11111111; // Initialize for output

  uint32_t NOTE_MAX = 128;
  uint32_t freq_hz = 440;
  uint32_t period_us = 1000000 / freq_hz;
  uint32_t delay_time_us = period_us / NOTE_MAX / 2;

  while (true) // Output a triangular wave
  {
    for (int i=0; i<NOTE_MAX; ++i)
    {
        PORTA = i;
        delayMicroseconds(delay_time_us);
    }
    for (int i=NOTE_MAX-1; i>=0; --i)
    {
        PORTA = i;
        delayMicroseconds(delay_time_us);
    }
  }
  return 0;
}
