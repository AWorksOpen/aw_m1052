:mod:`utime` -- 时间相关功能
======================================

.. module:: utime
   :synopsis: 时间相关功能

|see_cpython_module| :mod:`python:time`.

``utime`` 模块提供了获取当前时间和日期，测量时间间隔以及延时功能。


**初始时间**: 端口使用 2000-01-01 00:00:00 UTC为时间标准。

**维持实际的日历日期/时间**: 这需要一个实时时钟（RTC），在具有基础OS（包括某些RTOS）的系统上，
RTC可能是隐式的，设置和维护实际日历时间是OS/RTOS的职责，并且是在MicroPyhton之外完成的，它仅使用OS API查询日期/时间。但是在裸机系统上，系统时间取决于 ``machine.RTC()`` 对象，当前日历时间使用 ``machine.RTC().datetime(tuple)`` 功能设置，并通过以下方式进行维护：

* 通过备用电池（对于特定的板可能是备用的可选组件）
* 使用网络时间协议（需要端口/用户进行设置）.
* 由用户在每次上电时手动设置（许多板在硬复位期间保持RTC时间，某些情况下可能需要重新设置RTC时间）.

如果实际时间不是通过系统/MicroPython RTC维持，那么一些函数结果可能不是和预期的相同。

函数
---------

.. function:: localtime([secs])

   从 :class:`初始时间` 以来的时间（以秒为单位）转换为包含以下内容的8元组：（年，月，日，时，分，秒，一周中某日，一年中某天），如果参数 ``secs`` 未提供或为 ``None`` ，则使用当前RTC时间。
   
   * 年份包括世纪（例如2014年）。
   * 月：1-12
   * 日：1-31
   * 时：0-23
   * 分：0-59
   * 秒：0-59
   * 周一至周日为0-6
   * 年中某日为1-366

.. function:: mktime()

   这是本地时间的反函数，它的参数是一个表示本地时间的8元组，他返回一个整数，该整数是2000年1月1日以来的秒数。

.. function:: sleep(seconds)

   睡眠指定的时间（单位：s），某些主板可能支持浮点数，某些板可能不支持浮点数，因为他们向下兼容 :class:`sleep_ms()` 和 :class:`sleep_us()` 。

.. function:: sleep_ms(ms)

   延迟 ``ms`` 毫秒，应为正整数或0。

.. function:: sleep_us(us)

   延迟 ``us`` 微妙，应为正整数或0。

.. function:: ticks_ms()

    返回一个任意点的递增毫秒计数器，该值会在某个点之后回绕。
    
    回绕值未显示公开，但为了简化讨论，我们将其称为TICKS_MAX，值的周期为TICKS_MAX+1，相同的周期值用于所有的 :class:`ticks_ms()` 、 :class:`ticks_us()`  和 :class:`ticks_cpu()` 函数。
    因此这些函数的返回值范围是（0 ~ TICKS_MAX），对这些返回值唯一可用的操作是 :class:`ticks_diff()` 和 :class:`ticks_add()`。
    
    注意:直接对这些值执行标准数学运算（+、-）或关系运算符（<, <=, >, >=）将导致无效的结果，
    执行数学运算，然后将其结果值作为参数传递给 :class:`ticks_diff()` 或 :class:`ticks_add()` 也将导致无效的结果。

.. function:: ticks_us()

   与 :class:`ticks_ms()` 类似，单位为微妙。

.. function:: ticks_cpu()

   与 :class:`ticks_ms()` 和 :class:`ticks_us()` 类似，具有更高的精度（使用CPU时钟）。

.. function:: ticks_add(ticks, delta)

   用一个给定数字 ``delta`` 抵消 ``ticks`` 值，该数字可以是正或负，该函数允许计算 ``ticks`` 之前或之后的节拍值，
   并且 ``ticks`` 参数必须调用 :class:`ticks_ms()`, :class:`ticks_us()` 或者 :class:`ticks_cpu` 获得，``delta`` 可以是任意整数
   ，该函数对于计算事件/任务的截止时间非常有用。（注意：您必须使用 :class:`ticks_diff()` 函数来处理截止时间。）
   
   示例::

        # Find out what ticks value there was 100ms ago
        print(ticks_add(time.ticks_ms(), -100))

        # Calculate deadline for operation and test for it
        deadline = ticks_add(time.ticks_ms(), 200)
        while ticks_diff(deadline, time.ticks_ms()) > 0:
            do_a_little_of_something()

        # Find out TICKS_MAX used by this port
        print(ticks_add(0, -1))


.. function:: ticks_diff(ticks1, ticks2)
    
   计算连续两次调用 :ref:`ticks_ms()` ， :ref:`ticks_us()` 或 :ref:`ticks_cpu()` 之间的周期。
   
   因为这些函数返回的值可能会回绕，因此直接对它们相减可能会产生错误的结果，应使用 ``ticks_diff()`` ，此函数实现了
   环形算法，即使对回绕值（时间间隔不应太长）也能正确计算。

   ``ticks_diff()`` 设计使用于各种模式，其中包括:

   * 使用超时轮询，此种情况下，事件顺序已知，只需处理 ``ticks_diff()`` 的返回值 ::

        # Wait for GPIO pin to be asserted, but at most 500us
        start = time.ticks_us()
        while pin.value() == 0:
            if time.ticks_diff(time.ticks_us(), start) > 500:
                raise TimeoutError

   * 调度事件，此情况下，若某一事件超期，则 ``ticks_diff()`` 的结果可能为负::

        # This code snippet is not optimized
        now = time.ticks_ms()
        scheduled_time = task.scheduled_time()
        if ticks_diff(scheduled_time, now) > 0:
            print("Too early, let's nap")
            sleep_ms(ticks_diff(scheduled_time, now))
            task.run()
        elif ticks_diff(scheduled_time, now) == 0:
            print("Right at time!")
            task.run()
        elif ticks_diff(scheduled_time, now) < 0:
            print("Oops, running late, tell task to run faster!")
            task.run(run_faster=true)

   注意: 请勿将 :class:`time()` 值传递给 :class:`ticks_diff()`, 您应在此使用正常的数学运算，请注意 :class:`time()` 可能会溢出。

.. function:: time()

   如果底层RTC是按照上述设置和维护的，则返回自 :class:`初始时间` 以来的秒数（以整数形式）。如果未设置RTC，
   则返回一个特定于端口的参考时间点以来的秒数（对于不具有后备电池的嵌入式板，通常在电源启动或复位后）。
   如果要开发可移植的MicroPython程序，则不应依赖此函数提供高于秒的精度，如果需要更高的精度，请使用 :class:`ticks_ms()` 和 :class:`ticks_us()` 函数，如果需要使用日历时间，则不带参数的 :class:`localtime` 是更好的选择。

   .. admonition:: 与 CPython 的区别
      :class: attention

      在CPython中，此函数返回自Unix纪元（1970年1月1日00:00:UTC）以来的秒数（作为浮点数），通常具有微妙的精度。 使用MicroPython，只有Unix端口使用相同纪元，若浮点精度允许，则返回此秒级精度，嵌入式硬件通常不具有浮点精度去表示长时间范围和次秒级精度，因此返回整数值。一些嵌入式硬件也缺少电池供电的RTC，因此自上次上电以来或是从其他想多的，特定于硬件的时间点（例如复位）起的秒数。
      
示例
----

::

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