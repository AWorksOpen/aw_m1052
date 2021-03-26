:mod:`machine` --- 硬件相关函数
====================================================

.. module:: machine
   :synopsis: 硬件相关函数

``machine`` 模块包含相关硬件的特定功能。该模块中的大多数功能都可以实现对系统中硬件块（例如CPU,计时器，总线等）的直接且不受限制的访问和控制。使用不当会导致故障，锁定，电路板崩溃，在极端情况下还可能导致硬件损坏。

.. _machine_callbacks:

使用 :mod:`machine` 模块中函数和类方法的回调的注释：所有这些回调都应被视为在中断上下文中执行，这对于ID >= 0
的物理设备和ID为负-1的“虚拟”设备都是正确的（这些“虚拟”设备在真实硬件和真实硬件中断之上仍然是薄薄的垫片）。
参见 :ref:`isr_rules`。

复位相关函数
-----------------------

.. function:: reset()

   类似于按下RESET按钮的方式重置设备。

.. attention::
   待实现...

.. function:: soft_reset()

   对解释器进行软重置，删除所有Python对象并重置Python堆。它尝试保留用户连接到 MicroPython REPL的方法（例如：串行 、USB、Wifi）

.. function:: reset_cause()

   获取重置原因。有关可能的返回值，请参见 :ref:`constants <machine_constants>` 。

中断相关函数
---------------------------

.. function:: disable_irq()

   禁用中断请求。返回先前的IRQ状态，应将其视为不透明值。该返回值应传递给 :class:`enable_irq()` 函数
   ，以将中断恢复到 :class:`disable_irq()` 调用之前的原始状态。

.. function:: enable_irq(state)
   
   重新启用中断请求，该 *state* 参数应该是上一次调用 :class:`disable_irq()` 函数的返回值。
   

电源相关函数
-----------------------

.. function:: freq()

    返回以Hz为单位的 CPU 频率。

.. function:: idle()

   Gates the clock to the CPU, useful to reduce power consumption at any time during
   short or long periods. Peripherals continue working and execution resumes as soon
   as any interrupt is triggered (on many ports this includes system timer
   interrupt occurring at regular intervals on the order of millisecond).

.. function:: sleep()

   .. note:: This function is deprecated, use `lightsleep()` instead with no arguments.

.. function:: lightsleep([time_ms])
              deepsleep([time_ms])

   Stops execution in an attempt to enter a low power state.

   If *time_ms* is specified then this will be the maximum time in milliseconds that
   the sleep will last for.  Otherwise the sleep can last indefinitely.

   With or without a timout, execution may resume at any time if there are events
   that require processing.  Such events, or wake sources, should be configured before
   sleeping, like `Pin` change or `RTC` timeout.

   The precise behaviour and power-saving capabilities of lightsleep and deepsleep is
   highly dependent on the underlying hardware, but the general properties are:

   * A lightsleep has full RAM and state retention.  Upon wake execution is resumed
     from the point where the sleep was requested, with all subsystems operational.

   * A deepsleep may not retain RAM or any other state of the system (for example
     peripherals or network interfaces).  Upon wake execution is resumed from the main
     script, similar to a hard or power-on reset. The `reset_cause()` function will
     return `machine.DEEPSLEEP` and this can be used to distinguish a deepsleep wake
     from other resets.

.. function:: wake_reason()

   Get the wake reason. See :ref:`constants <machine_constants>` for the possible return values.

   Availability: ESP32, WiPy.

其他函数 
-----------------------

.. function:: unique_id()

   Returns a byte string with a unique identifier of a board/SoC. It will vary
   from a board/SoC instance to another, if underlying hardware allows. Length
   varies by hardware (so use substring of a full value if you expect a short
   ID). In some MicroPython ports, ID corresponds to the network MAC address.

.. function:: time_pulse_us(pin, pulse_level, timeout_us=1000000, /)

   Time a pulse on the given *pin*, and return the duration of the pulse in
   microseconds.  The *pulse_level* argument should be 0 to time a low pulse
   or 1 to time a high pulse.

   If the current input value of the pin is different to *pulse_level*,
   the function first (*) waits until the pin input becomes equal to *pulse_level*,
   then (**) times the duration that the pin is equal to *pulse_level*.
   If the pin is already equal to *pulse_level* then timing starts straight away.

   The function will return -2 if there was timeout waiting for condition marked
   (*) above, and -1 if there was timeout during the main measurement, marked (**)
   above. The timeout is the same for both cases and given by *timeout_us* (which
   is in microseconds).

.. function:: rng()

   Return a 24-bit software generated random number.

   Availability: WiPy.

.. _machine_constants:

常量
---------

.. data:: machine.IDLE
          machine.SLEEP
          machine.DEEPSLEEP

    IRQ wake values.

.. data:: machine.PWRON_RESET
          machine.HARD_RESET
          machine.WDT_RESET
          machine.DEEPSLEEP_RESET
          machine.SOFT_RESET

    Reset causes.

.. data:: machine.WLAN_WAKE
          machine.PIN_WAKE
          machine.RTC_WAKE

    Wake-up reasons.

类
-------

.. toctree::
   :maxdepth: 1

   ..官方micropython文档无LED类
   machine.LED.rst 
   machine.Pin.rst
   machine.Signal.rst
   .. machine.ADC.rst:
   machine.UART.rst
   machine.SPI.rst
   machine.I2C.rst
   ..machine.RTC.rst
   .. machine.Timer.rst..
   .. machine.WDT.rst:
   .. machine.SD.rst:
   .. machine.SDCard.rst: