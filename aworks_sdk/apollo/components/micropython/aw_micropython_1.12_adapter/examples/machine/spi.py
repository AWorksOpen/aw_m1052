from machine import SPI
spi = SPI(1)
spi.init(baudrate=600000,  polarity=0, phase=0, bits=8, firstbit=SPI.MSB)

buf = bytearray(4)

spi.read(4, write=0x00)                      # Read a number of bytes specified by nbytes while
                                             # continuously writing the single byte given by write.

spi.readinto(buf, write=0x00)                # Read into the buffer specified by buf while
                                             # continuously writing the single byte given by write.

spi.write(buf )                              # Write the bytes contained in buf. Returns None

spi.write_readinto(write_buf, read_buf )     # Write the bytes from write_buf while reading into read_buf.
                                             # The buffers can be the same or different, but
                                             # both buffers must have the same length. Returns None

spi.deinit()                                 #Turn off the SPI bus