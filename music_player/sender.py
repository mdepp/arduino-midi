import serial
import sys
import time
import wave

BAUD_RATE = 128000
BATCH_SIZE = 1000 # Size of the data batches expected by player

BATCH_SIGNAL = 1 # Received when player needs a new batch of data


try:
    # Fill sound buffer with an 8-bit 8kz song (which is simple and fast since
    # file is already in this format). If not, this would need to convert
    # it to the required format of the player.
    print('Setting up sound buffer')
    sound_buffer = bytearray()
    # Populate sound buffer with data
    with wave.open('misc/Planescape_Torment-_Main_Theme.wav', 'rb') as f:
        sound_buffer = bytearray(f.readframes(f.getnframes())) # Since sound file is 8-bit, 8khz

    # Initialize serial port with first command-line parameter
    print('Opening serial port')
    port_name = sys.argv[1] if len(sys.argv) > 1 else 'COM3'
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
