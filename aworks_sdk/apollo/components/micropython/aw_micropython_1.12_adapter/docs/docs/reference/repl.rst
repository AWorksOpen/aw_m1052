.. _repl:
MicroPython交互式解释器(又称REPL)
=======================================================

本节介绍 MicroPython 交互式解释器模式的一些特征。常用的术语是 REPL (read-eval-print-loop)，它将用于引用此交互式提示。

自动缩进
-----------

当键入以冒号结尾的python语句时（例如if，for，while），提示将为三个点（...），并且光标将缩进4个空格。当您按回车键时，下一行将以常规语句的相同缩进级别继续，或在适当的情况下以其他缩进级别继续。如果按退格键，则它将撤销一级缩进。

如果光标从头开始一直返回，则按回车键将执行您输入的代码。下面显示了输入for语句后看到的内容（下划线显示了光标结束的位置）：

    >>> for i in range(30):
    ...     _

如果随后输入if语句，这将提供其他缩进级别：

    >>> for i in range(30):
    ...     if i > 3:
    ...         _

现在输入 ``break`` ，然后按回车键，然后按退格键：

    >>> for i in range(30):
    ...     if i > 3:
    ...         break
    ...     _

最后输入 ``print(i)``, 按回车键, 按退格键 ，然后再次按回车键：

    >>> for i in range(30):
    ...     if i > 3:
    ...         break
    ...     print(i)
    ...
    0
    1
    2
    3
    >>>

如果前两行都是空格，则不会应用自动缩进。这意味着您可以通过按两次回车键来完成输入符合语句，然后第三次按完成并执行。

自动补全
---------------

在REPL上键入命令时，如果到目前为止键入的行与某物名称的开头相对应，则按TAB键将显示可能输入的内容。例如，首先通过输入 ``import machine`` 并按回车键来导入 ``machine`` 模块。然后键入 ``m`` 并点击TAB，则其将扩展为 machine 。键入一个 `.` 并点击TAB，您将看到如下内容：

    >>> machine.
    __name__        info            unique_id       reset
    bootloader      freq            rng             idle
    sleep           deepsleep       disable_irq     enable_irq
    Pin

该词将尽可能多地扩展，指导存在多种可能性为止。例如，键入  ``machine.Pin.AF3`` ，然后按TAB键，它将展开为 ``machine.Pin.AF3_TIM`` 。再次按TAB键，将显示可能的扩展：

    >>> machine.Pin.AF3_TIM
    AF3_TIM10       AF3_TIM11       AF3_TIM8        AF3_TIM9
    >>> machine.Pin.AF3_TIM

中断正在运行的程序
------------------------------

您可以通过按Ctrl-C中断正在运行的程序。如果您的程序没有截获 KeyboardInterrupt 异常，这将引发 KeyboardInterrupt ，将您带回REPL。

例如:

    >>> for i in range(1000000):
    ...     print(i)
    ...
    0
    1
    2
    3
    ...
    6466
    6467
    6468
    Traceback (most recent call last):
      File "<stdin>", line 2, in <module>
    KeyboardInterrupt:
    >>>

粘贴模式
----------

如果要将一些代码粘贴到终端窗口中，则自动缩进功能将事情变得混乱。例如，如果您具有一下python代码：

::

   def foo():
       print('This is a test to show paste mode')
       print('Here is a second line')
   foo()

然后尝试将其粘贴到常规REPL中，那么您将看到类似一下内容:

    >>> def foo():
    ...         print('This is a test to show paste mode')
    ...             print('Here is a second line')
    ...             foo()
    ...
    Traceback (most recent call last):
      File "<stdin>", line 3
    IndentationError: unexpected indent

如果按Ctrl-E，则将进入粘贴模式，该模式实际上将关闭自动缩进功能，并将提示从 ``>>>`` 更改为 ``===``。例如：

    >>>
    paste mode; Ctrl-C to cancel, Ctrl-D to finish
    === def foo():
    ===     print('This is a test to show paste mode')
    ===     print('Here is a second line')
    === foo()
    ===
    This is a test to show paste mode
    Here is a second line
    >>>

粘贴模式允许粘贴空白行。粘的文本就像文件一样被编译。按Ctrl-D退出粘贴模式并启动编译。


软复位
----------

软复位会重置python解释器，但是不会尝试重置与 MicroPython 开发板（USB串行或wifi）的连接方法。

您可以通过按Ctrl-D从REPL执行软重置，或者通过执行以下命令从python代码执行软重置： ::

    machine.soft_reset()

例如，如果您重置 MicroPython 开发板，并执行dir()命令，您将看到类似以下的内容：

    >>> dir()
    ['__name__', 'pyb']

现在创建一些变量并重复dir()命令：

    >>> i = 1
    >>> j = 23
    >>> x = 'abc'
    >>> dir()
    ['j', 'x', '__name__', 'pyb', 'i']
    >>>

现在，如果您输入Ctrl-D并重复执行dir()命令，您将看到变量不再存在：

.. code-block:: python

    MPY: sync filesystems
    MPY: soft reboot
    MicroPython v1.5-51-g6f70283-dirty on 2015-10-30; PYBv1.0 with STM32F405RG
    Type "help()" for more information.
    >>> dir()
    ['__name__', 'pyb']
    >>>

特殊变量_(下划线)
-----------------------------------

使用REPL时，您可以执行计算并查看结果。 MicroPython将前一个语句的结果存储在变量_（下划线）中。因此，
您可以使用下划线将结果保存到变量中。例如：

    >>> 1 + 2 + 3 + 4 + 5
    15
    >>> x = _
    >>> x
    15
    >>>

原始模式
--------

原始模式不是人们通常会使用的模式。它旨在用于程序设计。本质上，它的行为类似于粘贴模式，其中关闭了回声。

使用Ctrl-A进入原始模式。然后，您发送python代码，然后发送Ctrl-D。Ctrl-D将被“确定”确认，然后将编译并执行python代码。任何输出（或错误）将被发回。输入Ctrl-B将离开原始模式并返回常规（也称友好）REPL。

``tools/pyboard.py`` 程序使用原始REPL来在MicroPython 板上执行python文件。