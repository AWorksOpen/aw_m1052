from machine import Pin
import utime

# create an output pin on pin #0
p0 = Pin(Pin.board.GPIO3_1)
p0.init( Pin.OUT,Pin.PULL_UP)

# set the value low then high
p0.value(1)

# create an input pin on pin #2, with a pull up resistor
p2 = Pin(Pin.board.GPIO3_0, Pin.IN, None)

# read and print the pin value
print(p2.value())

# configure an irq callback
p2.irq(lambda p:print(p),Pin.IRQ_FALLING)

while True:
    p0.value(0)
    utime.sleep_ms(1000)
    p0.value(1)