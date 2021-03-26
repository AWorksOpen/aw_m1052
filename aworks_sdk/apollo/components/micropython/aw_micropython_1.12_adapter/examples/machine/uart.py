from machine import UART

# init with given baudrate
uart = UART(2, 9600)

# init with given parameters
uart.init(115200, bits=8, parity=None, stop=1)

# write characters
uart.write('Hello,welcome to micropython.\nThis is UART class')

# read a line
uart.readline()

if uart.any():
    # read 20 characters, returns a bytes object
    uart.read(20)

# write 5 characters
uart.write('abcde')

buf = bytearray(2)
# read and store into the given buffer
uart.readinto(buf)

# read all available characters
uart.read()