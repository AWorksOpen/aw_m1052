import utime
from machine import LED

def led():
    while 1:
        LED(0).toggle()
        utime.sleep_ms(500)

if __name__ == '__main__':
    led()