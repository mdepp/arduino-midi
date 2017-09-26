import serial
import sys
import time

SAMPLE_RATE = 8000
BAUD_RATE = 128000

sound_buffer = bytearray([0]*3000000)
current_pos = 0
state = 0
for i in range(len(sound_buffer)):
    if (int(SAMPLE_RATE/440) != 0) and (i % int(SAMPLE_RATE/440) == 0):
        state = 0 if state == 7 else 7
    sound_buffer[i] = state
print(sound_buffer)

try:
    port_name = sys.argv[1] if len(sys.argv) > 1 else 'COM3'

    with serial.Serial(port_name, BAUD_RATE, timeout=0) as ser:
        print('Serial port:', ser.name)

        while True:
            byte_read = ser.read()
            if byte_read:
                bytes_to_send = ord(byte_read)
                # print("Received request for {} bytes.".format(bytes_to_send))
                time.sleep(1/440)
                end_pos = min(current_pos+bytes_to_send, len(sound_buffer)-1)
                if current_pos <= end_pos:
                    # print(sound_buffer[current_pos:end_pos])
                    ser.write(sound_buffer[current_pos:end_pos])
                    current_pos = end_pos

except Exception as e:
    print(e)
except:
    print('Exception caught.')
