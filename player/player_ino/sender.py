import serial
import sys
import time
import wave
import progressbar

SAMPLE_RATE = 8000
BAUD_RATE = 128000

BUFFER_SIZE = 1000

sound_buffer = bytearray()
# Populate sound buffer with data
with wave.open('misc/Superman_Theme.wav', 'rb') as f:
    nframes = f.getnframes()
    sound_buffer = bytearray(f.readframes(nframes)) # Since sound file is 8-bit, 8khz

print('Finished setting up sound buffer')
current_pos = 0

try:
    port_name = sys.argv[1] if len(sys.argv) > 1 else 'COM3'

    with serial.Serial(port_name, BAUD_RATE, timeout=0) as ser:
        print('Serial port:', ser.name)

        while True:
            byte_read = ser.read()
            if byte_read:
                bytes_to_send = BUFFER_SIZE # ord(byte_read)
                # print(bytes_to_send)
                # print("Received request for {} bytes.".format(bytes_to_send))
                end_pos = min(current_pos+bytes_to_send, len(sound_buffer)-1)
                if current_pos <= end_pos:
                    # print(sound_buffer[current_pos:end_pos])
                    ser.write(sound_buffer[current_pos:end_pos])
                current_pos = end_pos


            if current_pos >= len(sound_buffer)-1:
                current_pos = 0

except Exception as e:
    print(e)
except:
    print('Exception caught.')
