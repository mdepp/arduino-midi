"""
Streams a sound file though a serial port to the 'player' program. It is
expected that this file is already in the desired format (that is, 8-bit, 8khz
sample rate). 
"""

import serial
import time
import wave
import argparse

BAUD_RATE = 128000
BATCH_SIZE = 1000 # Size of the data batches expected by player

BATCH_SIGNAL = 1 # Received when player needs a new batch of data

# Read in sound file and port arguments
parser = argparse.ArgumentParser()
parser.add_argument('port', help='the serial port through which the Arduino is connected')
parser.add_argument('file', help='the sound file to stream to the Arduino')
args = parser.parse_args()
port_name = args.port
file_name = args.file

try:
    # Fill sound buffer with an 8-bit 8khz song (which is simple and fast since
    # file is already in this format). If not, this would need to convert
    # it to the required format of the player.
    print('Setting up sound buffer')
    sound_buffer = bytearray()
    # Populate sound buffer with data
    with wave.open(file_name, 'rb') as f:
        sound_buffer = bytearray(f.readframes(f.getnframes())) # Since sound file is 8-bit, 8khz

    # Initialize serial port with first command-line parameter
    print('Opening serial port')
    with serial.Serial(port_name, BAUD_RATE) as ser:
        
        # Write sound buffer in batches to player
        print('Playing music')
        current_pos = 0
        while True:
            byte_read = ser.read() # Wait forever until a batch is requested
            if ord(byte_read) == BATCH_SIGNAL:
                end_pos = current_pos+BATCH_SIZE

                if end_pos < len(sound_buffer): # Batch is contained in buffer bounds
                    ser.write(sound_buffer[current_pos:end_pos])
                else: # End point of sound buffer is in batch
                    ser.write(sound_buffer[current_pos:])
                    # Repeat song again from the start
                    end_pos -= BATCH_SIZE
                    ser.write(sound_buffer[:end_pos])
                
                current_pos = end_pos

except Exception as e:
    print(e)
except:
    print('Exception caught.')
