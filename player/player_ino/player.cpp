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
volatile uint32_t read_position;
uint32_t end_position;
uint32_t write_position;
uint8_t CHUNK_SIZE = 32; // Must be smaller than buffer size

uint32_t BAUD_RATE = 128000;

// Speaker
volatile uint8_t* output_filter_register = &DDRA;
volatile uint8_t* output_register = &PORTA;

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
  write_position = 0;
  read_position = 0;
  end_position = BUFFER_SIZE - 1;
}

void initSpeaker()
{
  *output_filter_register = B11111111;
}


void tickSound()
{
  // Update speaker to next part of buffer
  *output_register = buffer[read_position];

    // Circle to start of buffer upon overflow
    read_position = (read_position+1) % end_position;
}

uint32_t positionDifference(uint32_t lhs, uint32_t rhs)
{
  if (lhs >= rhs)
    return lhs - rhs;
  else
    return lhs+BUFFER_SIZE - rhs;
}

int main()
{
  init();
  Serial.begin(BAUD_RATE);
  initSoundBuffer();
  initSpeaker();

  // Set up some data in sound buffer
  Serial.write(CHUNK_SIZE); Serial.flush();
  while (write_position < CHUNK_SIZE)
  {
    if (Serial.available() > 0)
    {
      buffer[write_position] = Serial.read();
      write_position = (write_position+1)%BUFFER_SIZE;
    }
  }

  // Start reading sound buffer
  Timer3::initTimer(CLOCK_SPEED, SAMPLE_RATE, &tickSound);

  // Continue writing data from serial ports
  uint32_t expected_position = write_position;
  while (true)
  {
    // Assumes writing is always ahead of reading
    if (positionDifference(expected_position, read_position) < CHUNK_SIZE) // Write is falling behind read
    {
      // So signal to receive a new chunk
      Serial.write(CHUNK_SIZE);
      expected_position = (expected_position+CHUNK_SIZE)%BUFFER_SIZE;
    }
    // Read all incoming bits (no protection against looping back and overwriting write position though...)
    if (Serial.available())
    {
      buffer[write_position] = Serial.read();
      write_position = (write_position+1) % BUFFER_SIZE;
    }
  }

  Timer3::quitTimer();

  return 0;
}
