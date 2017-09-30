/*
 * Plays a sound file streamed from Serial.
 *
 * Arduino ports 22 through 29 are connected to an R-2R resistor ladder for DAC,
 * (pin 29 is most-significant) the output of which connects to the speaker (see
 * README for details).
 */

#include <Arduino.h>
#include "timer.h"


// Timing information
const uint32_t CLOCK_SPEED = 16000000;
const uint32_t SAMPLE_RATE = 8000;
uint32_t BAUD_RATE = 128000;

// Signal sent to request another buffer of sound data from serial input
const byte BUFFER_SIGNAL = 1;

// Controls which pins are used for the digital sound output
volatile uint8_t* OUTPUT_FILTER_REGISTER = &DDRA;
volatile uint8_t* OUTPUT_REGISTER = &PORTA;

// Sound buffers
const uint32_t BUFFER_SIZE = 1000;
byte rb[BUFFER_SIZE];
byte wb[BUFFER_SIZE]; // Todo: volatile necessary?
byte* read_buffer = wb; // For easy swapping
byte* write_buffer = rb;

 // A flag to notify main process when buffers have been swapped by interrupt
volatile bool buffersSwapped;


/*
 * Fill the write buffer with the next batch of information through the serial port.
 * This will delay until the requested data is received.
 */
void fillWriteBuffer()
{
  // Signal for more data
  Serial.write(BUFFER_SIGNAL); Serial.flush();
  // Read the data one byte at a time
  uint32_t write_position = 0;
  while (write_position < BUFFER_SIZE)
  {
    if (Serial.available() > 0)
    {
      write_buffer[write_position++] = Serial.read();
    }
  }
}
/*
 * Swap the write and read buffers, so that the new read buffer can be read
 * while the data from the old one is overwritten. Because this operation is not
 * atomic, this function should *not* be called if it could be interrupted by
 * the buffer reading operation.
 */
void swapBuffers()
{
  byte* temp = read_buffer;
  read_buffer = write_buffer;
  write_buffer = temp;
}

/*
 * Reads one byte from the read buffer and outputs it to the output register.
 * If this is the last byte in that buffer, it will swap buffers and notify the
 * main process by setting the 'buffersSwapped' flag.
 */
void tickSound()
{
  static volatile uint32_t read_position = 0;

  // Update speaker to next part of buffer
  *OUTPUT_REGISTER = read_buffer[read_position];

  // Swap buffers upon overflow
  if (++read_position >= BUFFER_SIZE)
  {
    swapBuffers();
    buffersSwapped = true; // Let the main process know that buffers were swapped
    read_position = 0;
  }
}

int main()
{
  init();
  Serial.begin(BAUD_RATE);
  *OUTPUT_FILTER_REGISTER = B11111111; // Initialize the speaker registers

  // Fill write buffer before starting any reading
  fillWriteBuffer();
  // Swap buffers, giving read buffer some initial data
  swapBuffers();
  buffersSwapped = true;  // Immediately start writing to the new write buffer

  // Start reading sound buffer
  Timer3::initTimer(CLOCK_SPEED, SAMPLE_RATE, tickSound);

  // Continue reading while write buffer loads next data.
  while (true)
  {
    if (buffersSwapped) // Buffers have been swapped so write buffer is ready
    {
      // Overwrite new write buffer with new sound data
      fillWriteBuffer();
      buffersSwapped = false;
    }
  }

  Timer3::quitTimer();

  return 0;
}