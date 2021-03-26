.. currentmodule:: machine
.. _machine.I2C:

class I2C -- 双线串行协议
=======================================

I2C是一个设备间通信的双线串行协议，其物理层包含两条线 SCL 和 SDA，分别为时钟和数据线。

I3C对象在特定的总线上创建。可在创建时或创建后初始化。

打印I2C对象可提供关于其配置的信息。

构造函数
------------

.. class:: I2C(id=-1, \*, scl, sda, freq=200000, timeout=1000)

   构造并返回一个新的 I2C 对象：

      - *id* 标识特定的I2C外设，默认值-1为软件I2C，该实现可以（在大多数情况下）与SCL和SDA的任意引脚一起使用。如果id为-1
        则SCL和SDA须被指定，其他允许的值取决于特定的端口，此种情况下不需要指定SCL和SDA。
      - *scl* 应为一个指定用于SCL的引脚对象。
      - *sda* 应为一个指定用于SDA的引脚对象。
      - *freq* 应为一个人设置SCL的最大频率。
      - *timeout* 码间超时时间（单位：ms）。

通用方法
---------------

.. method:: I2C.init(scl, sda, \*, freq=200000, timeout=1000)

  使用给定参数初始化I2C总线，参数详细含义见构造函数文档。

.. method:: I2C.deinit()

   关闭I2C总线。
.. method:: I2C.scan()

   扫描介于0x08和0x77之间的所有I2C地址，并返回一个包含所有相应对象的列表。若在总线上发送地址（包括一个位）后将
   SDA线拉低，设备就会响应。

原始 I2C 操作
------------------------

以下方法实现了原始I2C主总线操作，且可组合起来执行任何I2C事务。若您需要对总线进行更多控制，即可使用该方法，否则可使用标准方法（见下）。

这些方法仅在软件I2C上可用。

.. method:: I2C.start()

   在总线上产生一个启动条件（SCL为高电平时，SDA转换为低电平）。

.. method:: I2C.stop()

   在总线上产生一个停止条件（SCL为高时，SDA转换为高）。

.. method:: I2C.readinto(buf, nack=True, /)

   从总线上读取字节并将其存储到 *buf* ，字节数读取的是 *buf* 的长度。接收除最后一个字节外的所有字节后，将在总线上发送ACK，NACK将在最后一个字节后发送。
   
.. method:: I2C.write(buf)

   将 *buf* 中的字节写入总线。检查每个字节之后是否收到ACK，若未接收到，则停止传输其余字节，该函数返回已接收到的的ACK数。

标准总线操作
-----------------------

以下方法实现了针对给定的从属设备的标准I2C主机读写操作。

.. method:: I2C.readfrom(addr, nbytes, stop=True, /)

   从 *addr* 指定的从属设备中读取 *nbytes* 。如果 *stop* 为真，则在传输结束时会生成STOP条件。返回读取数据的 `bytes` 对象

.. method:: I2C.readfrom_into(addr, buf, stop=True, /)

   从 *addr* 指定的从站读入 *buf* 。读取的字节数将为 *buf* 的长度。如果 *stop* 为真，则在传输结束时会生成STOP条件。
   
   该方法返回 ``None`` 。

.. method:: I2C.writeto(addr, buf, stop=True, /)

   将字节 *buf* 写入 *addr* 指定的从站。如果在写的过程中受到了 *NACK* 信号，那么就不会发送剩余的字节。
   如果  *stop* 为真，那么在传输结束时会产生一个停止信号，即使受到NACK。这个函数返回接收到的ACK数量。

内存操作
-----------------

一些I2C设备可作为读取和写入的内存设备（或一组寄存器）。此时，有个与I2C处理相关的地址：丛书地址与内存地址。以下方法是与设备通信的便捷函数。

.. method:: I2C.readfrom_mem(addr, memaddr, nbytes, \*, addrsize=8)

   从 *memaddr* 指定的内存地址开始，从 *addr* 指定的从站中将 *nbytes* 字节读取到缓冲区中。
   *addrsize* 指定地址大小（7bits/10bits）。
   
   返回值为读取的数据字节对象。

.. method:: I2C.readfrom_mem_into(addr, memaddr, buf, \*, addrsize=8)

   从 *addr* 指定的从机设备中 *memaddr* 地址读取数据到 *buf* 中，读取的字节数是 *buf* 长度。
   *addrsize* 指定地址大小（7bits/10bits）。

   此方法无返回值。

.. method:: I2C.writeto_mem(addr, memaddr, buf, \*, addrsize=8)

   将方法 *buf* 的数据写入 *addr* 指定的从站中的 *memaddr* 地址。
   *addrsize* 指定地址大小（7bits/10bits）。
   
   此方法无返回值。
   
示例
----

::

    from machine import I2C

    i2c = I2C(1)                             # create on bus 1
    i2c = I2C(1, freq=200000, timeout=1000)  # create and init as a master
    i2c.init(freq=2000000)                   # init freq

    Basic methods for slave are write and read:

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
