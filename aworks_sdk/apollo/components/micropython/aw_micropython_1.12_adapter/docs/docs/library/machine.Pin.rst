.. currentmodule:: machine
.. _machine.Pin:

class Pin -- 控制 I/O 引脚
=============================

引脚对象用于控制I/O引脚（也称GPIO-通用输入/输出）。引脚对象通常与物理引脚相关联，该物理引脚可以驱动输出电压并读取输入电压值。
引脚类具有引脚模式（IN，OUT等）方法以及获取和设置数字逻辑电平的方法。

引脚对象通过使用明确指定某个I/O引脚的标识符来构建。每个标识符对应到一个引脚，标识符可能是整数，或者是一个带有端口和引脚号的元组。


构造函数
------------

.. class:: Pin(id, mode=-1, pull=-1, \*, value)

   访问与给定 `id` 相关联的引脚外设（GPIO），若在构造函数中给出了额外参数，则该参数用于初始化引脚。任何未指定设置都将保留先前状态。

   参数如下:

     - ``id`` 是强制性的，类型可以是：int（内部Pin标识符）和元组（[port, pin]），例如Pin(51)和Pin("uart",51)，其中int值可由引脚编号常量 ``Pin.board.*`` 常量代替，为避免用户使用引脚编号时出错，建议使用 ``Pin.board.*`` 常量，更多内容请参考 :ref:`引脚编号 <board>` 。

     - ``mode`` 指定引脚模式，可以是以下其中一种

       - ``Pin.IN`` - 配置引脚为输入。

       - ``Pin.OUT`` - 配置引脚为输出。

       - ``Pin.OPEN_DRAIN`` - 配置引脚为开漏输出 。
       
       如果对指定 ``id`` 引脚初始化，该参数是必须的。

     - ``pull`` 指定该引脚是否连接了（弱）上拉电阻，可以是以下之一：

       - ``None`` - 没有上拉或者下拉电阻。
       - ``Pin.PULL_UP`` - 启用上拉电阻。
       - ``Pin.PULL_DOWN`` - 启用下拉电阻。

       如果对指定 ``id`` 引脚初始化，若为未指定该参数，默认为浮空。

     - ``value`` 仅对 Pin.OUT 和 Pin.OPEN_DRAIN 模式有效，并指定初始输出引脚值（如果给定），否则引脚外设的状态保持不变。
      
      该参数使用时必须带关键字，例如 ``value = 1`` 。

方法
-------

.. method:: Pin.init(mode=-1, pull=-1, \*, value)

   使用给定的参数重新初始化引脚。仅设置那些指定的参数，其余引脚外围状态保持不变。有关参数的详细信息，请参见构造函数文档。

   
   返回  ``None`` 。


.. method:: Pin.value([x])
    
   此方法允许设置并获取引脚值，这取决于是否提供了参数 ``x`` 。

   如果未提供参数，则该方法获取引脚的数字逻辑点评，分别返回0 或 1 。
   
   如果提供了参数，则该方法设置引脚的数字逻辑电平值，取值0 或 1。

.. method:: Pin.on()

   将引脚设置为“1”输出电平。

.. method:: Pin.off()

   将引脚设置为“0”输出电平。

.. method:: Pin.mode([mode])

   获取或设置引脚模式。有关 ``mode`` 参见构造函数文档。

.. method:: Pin.pull([pull])

   获取或者设置引脚拉动状态。有关 ``pull`` 参数的详细信息，请参见构造函数。 

.. method:: Pin.irq(handler=None, trigger=(Pin.IRQ_FALLING | Pin.IRQ_RISING))

   配置一个中断处理程序，当引脚的触发器处于激活状态时，调用该程序。若引脚模式为 ``Pin.IN`` ，
   则触发源为引脚上的外部值。若引脚模式为 ``Pin.OUT`` ，触发源为引脚的输出缓冲区，否则，若引脚模式为
    ``Pin.OPEN_DRAIN`` ，状态为 “0” 时，触发器源为输出缓冲区；状态为 “1” 时，触发器源为外部引脚值。
    
   参数如下:

     - ``handler`` 是在中断触发时调用的可选函数。处理程序必须仅使用一个参数即 ``Pin`` 实例。  

     - ``trigger`` 配置中断触发条件可能的值为：

       - ``Pin.IRQ_FALLING`` 下降沿触发。
       - ``Pin.IRQ_RISING`` 上升沿触发。
       - ``Pin.IRQ_LOW_LEVEL`` 低电平触发。
       - ``Pin.IRQ_HIGH_LEVEL`` 高电平触发。

       这些值可以在多个时间中同时进行“或”运算。
    
    此方法返回一个回调对象（待实现...）。
    
常量
---------

以下常量用于配置引脚对象。

.. data:: Pin.IN
          Pin.OUT
          Pin.OPEN_DRAIN

   选择引脚模式

.. data:: Pin.PULL_UP
          Pin.PULL_DOWN

   选择是否有上拉/下拉电阻，若无上下拉电阻使用 ``None`` 值。

.. data:: Pin.IRQ_FALLING
          Pin.IRQ_RISING
          Pin.IRQ_LOW_LEVEL
          Pin.IRQ_HIGH_LEVEL
    
   选择IRQ 触发条件。

引脚编号常量
------------

.. _board:
.. data:: Pin.board.GPIO1_0
          Pin.board.GPIO1_1
          Pin.board.GPIO1_2
          Pin.board.GPIO1_3
          Pin.board.GPIO1_4
          Pin.board.GPIO1_5
          ...

   imxrt1052系列硬件引脚编号名称，此常量只适用于imxrt1052系列硬件平台。

   更多引脚编号，在REPL中输入 ``help(Pin.borad)`` 查看。
   
示例
----

::

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
