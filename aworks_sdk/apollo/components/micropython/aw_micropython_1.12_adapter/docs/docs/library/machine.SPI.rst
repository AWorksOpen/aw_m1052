.. currentmodule:: machine
.. _machine.SPI:

class SPI -- 串行外设接口总线接口 (主机)
=====================================================================

SPI是由主机驱动的同步串行协议。在物理级别，总线由3条线路组成： SCK, MOSI, MISO。
多个设备可以共享一个总线。每个设备应该有一个单独的第四条信号线SS（从选择），以选择总线上与之进行通信的特定设备。SS信号的管理应该在用户代码中进行（通过 :ref:`machine.Pin类 <machine.Pin>` ）。

构造函数
------------

.. class:: SPI(id, ...)

   在给定的总线上构造一个SPI对象 `id` 。值 `id` 取决于特定端口及其硬件。值0、1等同于选择硬件SPI模块 #0, #1等，值-1可用于SPI的位触发（软件）实现。
   
   如果没有其他参数，则将创建SPI对象，但不会对其进行初始化（它具有上一次总线初始化的设置）
   。如果给出额外的参数，总线将被初始化，请参阅 ``init`` 以获取初始化参数。

方法
-------

.. method:: SPI.init(baudrate=1000000, \*, polarity=0, phase=0, bits=8, firstbit=SPI.MSB, sck=None, mosi=None, miso=None,cs_pin)

   使用给定的参数初始化 SPI 总线:

     - ``baudrate`` 是 SCK 时钟速率。
     - ``polarity`` 可以是 0 或者 1，是时钟线空闲时所处的电平。
     - ``phase`` 相位可以是 0 或者 1，分别在第一个或者第二个时钟边沿采集数据。
     - ``bits`` 传输数据的长度，一般为 8 位。
     - ``firstbit`` 传输数据从高位开始还是从低位开始，可以是 ``SPI.MSB`` 或 ``SPI.LSB`` 。
     - ``sck``, ``mosi``, ``miso`` 是总线信号所用的引脚对象（:ref:`machine.Pin <machine.Pin>`) 此参数适用于软件SPI对象，对于硬件SPI对象由构造函数的 ``id`` 参数选择。
   
.. method:: SPI.deinit()

   关闭 SPI 总线.

.. method:: SPI.read(nbytes, write=0x00)

   当持续写入由 ``write`` 给定的单个字节时，读取由 ``nbytes`` 指定的字节数。用读取的数据返回一个 ``bytes`` 对象。
   
.. method:: SPI.readinto(buf, write=0x00)

   当持续写入由 ``write`` 给定的单个字节时，读取n个字节到 ``buf`` 缓冲区，返回 ``None`` 。
   
.. method:: SPI.write(buf)

    写入 ``buf`` 内包含的字节。返回 ``None`` 。

.. method:: SPI.write_readinto(write_buf, read_buf)

   在读取数据到 ``read_buf`` 的同时，从 ``write_buf`` 中写入数据。
   缓冲区可以是不同也可以是相同的，但是两个缓冲区的长度必须一致。

   返回 ``None``。

常量
---------

.. data:: SPI.MASTER

   用于初始化SPI总线主控，仅用于软件SPI。

.. data:: SPI.MSB

   将第一位设置为最高有效位。

.. data:: SPI.LSB

   将第一位设置为最低有效位。

示例
----

::

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