from machine import I2C

i2c = I2C(1)                             # create on bus 1
i2c = I2C(1, freq=200000, timeout=1000)  # create and init as a master
i2c.init(freq=2000000)                   # init freq


#Basic methods for slave are write and read:

addr = i2c.scan()[0]
buf = bytearray(3)
i2c.writeto(addr, buf, stop=True)      # write 3 bytes
i2c.readfrom(addr,len(buf), stop=True) # read 3 bytes
data = bytearray(3)                    # create a buffer
i2c.readfrom_into(addr, data)          # receive 3 bytes, writing them into data
addr = i2c.scan()[0]
buf = bytearray(3)
i2c.writeto(addr, buf, stop=True)      # write 3 bytes
i2c.readfrom(addr,len(buf), stop=True) # read 3 bytes
data = bytearray(3)                    # create a buffer
i2c.readfrom_into(addr, data)          # receive 3 bytes, writing them into data

reg = 0x02                             # register address
i2c.writeto_mem(addr, reg, buf)
i2c.readfrom_mem_into(addr, reg, data)

Try some raw read and writes
reg2 = bytearray(3)
reg2[0] = 0x02                       # register address
reg2[1] = 0x01                       # data
reg2[2] = 0x03
i2c.writeto(addr, reg2, stop=False)  # After writing the register address,
                                  # The data on the bus is started to be sent
# now read
print(i2c.readfrom(addr, 2, stop=False)[0])
reg2_read = bytearray(2)
i2c.readfrom_into(addr, reg2_read, stop=False)
reg2_read.decode()

i2c.deinit()                             # turn off the peripheral
