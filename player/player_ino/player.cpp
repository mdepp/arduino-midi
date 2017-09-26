/*
 * Plays a sound file from memory
 * 
 * Arduino ports 22 through 29 are connected to an R-2R resistor ladder for DAC,
 * (pin 29 is most-significant) the output of which connects to the speaker.
 */

#include <Arduino.h>
#include "timer.h"


const uint32_t CLOCK_SPEED = 16000000;

// Sound buffer
const uint32_t BUFFER_SIZE = 5000;
byte buffer[BUFFER_SIZE];
const uint32_t SAMPLE_RATE = 8000;
uint32_t current_position, end_position;

// Speaker
uint8_t* output_filter_register = &DDRA;
uint8_t* output_register = &PORTA;

void initSoundBuffer()
{
  // Init with 440hz square wave
  byte state = 0;
  for (uint32_t i=0; i<BUFFER_SIZE; ++i)
  {
    if (SAMPLE_RATE/440 > 0)
    {
      if (i % (SAMPLE_RATE/440) == 0)
      {
        state ^= B11111111;
      }
    }
    else
    {
      state ^= B11111111;
    }
    buffer[i] = state;
  }
  current_position = 0;
  end_position = BUFFER_SIZE - 1;
}

void initSpeaker()
{
  *output_filter_register = B11111111;
}

int main()
{
  Serial.begin(9600);
  initSoundBuffer();
  initSpeaker();
  // Initialize timer 1
  uint32_t actual_sample_rate = Timer3::initTimer(CLOCK_SPEED, SAMPLE_RATE, &tickSound);
  Serial.print("Actual sample rate: "); Serial.println(actual_sample_rate);
  
  while (true) {};

  Timer3::quitTimer();

  return 0;
}


void tickSound()
{
  // Circle to start of buffer upon overflow
  if (current_position == end_position)
    current_position = 0;
  // Update speaker to next part of buffer
  *output_register = buffer[++current_position];
}