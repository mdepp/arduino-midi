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
const uint32_t BUFFER_SIZE = 1000;
byte rb[BUFFER_SIZE];
byte wb[BUFFER_SIZE]; // Todo: volatile necessary?
byte* read_buffer = rb;
byte* write_buffer = wb;

const uint32_t SAMPLE_RATE = 8000; //440; //8000;

volatile uint32_t read_position;
uint32_t write_position;

volatile bool buffersSwapped; // Triggered by 'read' event when it swaps read/write buffers

uint32_t BAUD_RATE = 128000;

// Speaker
volatile uint8_t* output_filter_register = &DDRA;
volatile uint8_t* output_register = &PORTA;

void initSoundBuffers()
{
  for (uint32_t i=0; i<BUFFER_SIZE; ++i)
  {
    read_buffer[i] = write_buffer[i] = 0;
  }
  write_position = 0;
  read_position = 0;
  buffersSwapped = false;
}

void initSpeaker()
{
  *output_filter_register = B11111111;
}

void swap(byte** first, byte** second)
{
  byte* temp = *first;
  *first = *second;
  *second = temp;
}

void tickSound()
{
  // Update speaker to next part of buffer
  *output_register = read_buffer[read_position];

  // Swap buffers upon overflow
  ++read_position;
  if (read_position >= BUFFER_SIZE)
  {
    swap(&read_buffer, &write_buffer);
    buffersSwapped = true;
    read_position = 0;
  }
}

int main()
{
  init();
  Serial.begin(BAUD_RATE);
  initSoundBuffers();
  initSpeaker();

  // Fill write buffer
  Serial.write(1); Serial.flush();
  write_position = 0;
  while (write_position < BUFFER_SIZE)
  {
    if (Serial.available() > 0)
    {
      write_buffer[write_position] = Serial.read();
      ++write_position;
    }
  }
  // Swap buffers, giving read buffer some initial data
  write_position = 0;
  swap(&write_buffer, &read_buffer);
  buffersSwapped = true;

  // Start reading sound buffer
  Timer3::initTimer(CLOCK_SPEED, SAMPLE_RATE, tickSound);

  // Continue reading while write buffer loads next data.
  while (true)
  {
    if (buffersSwapped) // Raad has overflowed read buffer and swapped buffers
    {
      // So write new data to the new write buffer
      Serial.write(1); // Request another buffer of data
      write_position = 0;
      while (write_position < BUFFER_SIZE)
      {
        if (Serial.available() > 0)
        {
          write_buffer[write_position] = Serial.read();
          ++write_position;
        }
      }
      buffersSwapped = false;
    }
  }

  Timer3::quitTimer();

  return 0;
}
