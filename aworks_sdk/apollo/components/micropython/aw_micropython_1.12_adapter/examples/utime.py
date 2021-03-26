import utime

utime.mktime(utime.localtime())
utime.sleep(10)
utime.time()

ticks1 = utime.ticks_ms()
utime.sleep_ms(1000)
utime.ticks_diff(utime.ticks_ms(),ticks1)

ticks1 = utime.ticks_us()
utime.sleep_us(1000000)
utime.ticks_diff(utime.ticks_us(),ticks1)

ticks1 = utime.ticks_cpu()
utime.sleep(1)
utime.ticks_diff(utime.ticks_cpu(),ticks1)

deadline = ticks_add(ticks_ms(), 1000)
utime.sleep(1)
print(ticks_diff(deadline, ticks_ms()))