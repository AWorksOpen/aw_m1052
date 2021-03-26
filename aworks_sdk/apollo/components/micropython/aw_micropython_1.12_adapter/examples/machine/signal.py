from machine import Pin, Signal

# Suppose you have an active-high LED on pin GPIO1_19
led1_pin = Pin(Pin.board.GPIO1_19, Pin.OUT)

# Ĭ�ϵ͵�ƽ����LED��
led1_pin.value(0)
led1_pin.value(1)

# invertΪTrue���øߵ�ƽ����LED��
led1 = Signal(led1_pin, invert=True)

# Now lighting up them looks the same
led1.value(1)
led1.value(0)

# Even better:
led1.on()
led1.off()
