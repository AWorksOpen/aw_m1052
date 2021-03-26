.. currentmodule:: machine
.. _machine.UART:

class UART -- 双工串行通信总线
=============================================

UART实现标准的UART/USART双工串行通信协议。在物理级别上，它有两线组成：RX和TX。通信单元是一个可以为8位和9位宽的字符（请勿与字符串字符混淆）

构造函数
------------

.. class:: UART(id, ...)

   构造一个具有给定id的UART对象。

方法
-------

.. method:: UART.init(baudrate=9600, bits=8, parity=None, stop=1, \*, ...)

   使用给定参数初始化UART总线:

     - *baudrate* 时钟速率。
     - *bits* 字符位数 5,6,7或8。
     - *parity* 奇偶校验， ``None`` （无）， 0（偶）或1（寄）。
     - *stop* 是停止位，1，1.5或2。
     - *timeout* 读超时时间(单位：ms)。
     - *timeout_char* 码间超时时间(单位：ms)。
     
.. method:: UART.deinit()

   关闭 UART 总线。

.. method:: UART.any()

   返回一个整数，该整数即为不阻塞情况下可读取的字符数，若如无可用字符，则返回0；若有可用字符，则返回可读字符数。

.. method:: UART.read([nbytes])

   读取字符，如果指定 ``nbytes`` ，则最多读取该数量的字节，否则读取尽可能多的数据。

   返回值：一个包括读入字节的字节对象，超时则返回 ``None`` 。

.. method:: UART.readinto(buf[, nbytes])

   将字节读入``buf``.  如果指定 ``nbytes`` ，则最多读取改数量的字节，否则，最多读取 ``len(buf)`` 字节。

   返回值: 读取并存入 ``buf`` 的字节数，若超时则返回 ``None`` 。

.. method:: UART.readline()

   读取一行，并以一个换行符结束。
   
   返回值：读取的行，若超时，则返回 ``None`` 。

.. method:: UART.write(buf)

   将字节缓冲区写入总线。
   
   返回值：写入的字节数，若超时返回 ``None`` 。

.. method:: UART.writechar(char)

   将一个字符写入总线。
   
   返回值：无。
   
.. method:: UART.readchar()
    
   从总线读取一个字符。
   
   返回值：无。

示例
----


UART对象可使用下列方式创建并初始化::

    from machine import UART

    # init with given baudrate
    uart = UART(2, 9600)          

    # init with given parameters               
    uart.init(115200, bits=8, parity=None, stop=1) 

UART串口对象的运行类似于 `stream`  对象，其读写都使用标准流方法进行::

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