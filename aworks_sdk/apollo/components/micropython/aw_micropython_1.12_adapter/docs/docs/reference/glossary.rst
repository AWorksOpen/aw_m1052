术语表
========

.. glossary::

    baremetal
        没有（成熟的）操作系统的系统，例如基于
        :term:`MCU` 的系统. 在裸机系统上运行时，
        MicroPython 的功能像一个小型操作系统，
        运行用户程序并提供命令解释器 (:term:`REPL`)。

    buffer protocol
        任何可以自动转换为字节的 Python 对象， 例如 ``bytes``，
        ``bytearray``， ``memoryview`` 和 ``str`` 对象，全都实现了
        ”缓冲协议（buffer protocol）“。

    board
        通常这是指包含 :term:`microcontroller <MCU>` 和支持的组件的PCB电路板。
        MicroPython 固件通常按板子提供，因为该固件既包含特定功能，又包含板级功能，
        例如驱动程序和引脚名称。

    bytecode
        通过编译Python源代码生成的 Python 程序的紧凑表示形式。
        这是 VM 实际执行的操作. 字节码通常在运行时自动生成，并且对用户不可见。
        请注意，虽然 :term:`CPython` 和 MicroPython 都使用字节码，但是格式却不同。
        您还可以使用 :term:`cross-compiler` 离线预编译代码。

    callee-owned tuple
        这是MicroPython特定的构造，出于效率原因
        其中一些内置函数或方法可能会重复使用相同的基础元祖对象来返回数据。
        这样避免了为每个调用分配新的元祖，并且减少了堆碎片。程序不应该包含
        被调用方拥有的元祖的引用，而应仅从它们中提取数据（或进行复制）。

    CircuitPython
        `Adafruit Industries <https://circuitpython.org>`_ 开发的一种MicroPython变体。

    CPython
        CPython 是 Python编程语言的参考实现，也是最著名的一种。
        但是，它是许多实现之一（包括Jython，IronPython，PyPy和MicroPython）。
        尽管MicroPython的实现与CPython完全不同，但是其目的是尽可能保持兼容性。

    cross-compiler
        也称为 ``mpy-cross``. 该工具可在您的PC上运行，并将将包含
        MicroPython代码的 :term:`.py file` 转换为包含MicroPython
        字节码的 :term:`.mpy file` 。这意味着它加载速度更快（电路板无需编译代码）
        并且在闪存上使用更少的空间（字节码更节省空间）。

    driver
        一个 MicroPython 库，实现对特定组件（例如传感器和显示器）的支持。

    FFI
        外来功能接口的缩写. :term:`MicroPython Unix port` 的一种机制，用于访问
        操作系统功能。这在 :term:`baremetal` 端口上不可用。

    filesystem
        大多数 MicroPython 端口和开发板都提供了一个存储在闪存中的文件系统，
        该文件系统可通过标准Python文件API（例如 ``open()``）供用户代码使用。
        一些主板还通过USB大容量存储使该内部文件系统对主机可用。
        

    frozen module
        经过交叉编译并捆绑到固件映像中的 Python 模块。由于代码直接从闪存执行，
        因此减少了RAM需求。

    Garbage Collector
        在Python（和MicroPython）中运行的后台进程，
        用于回收 :term:`heap` 中未使用的内存。

    GPIO
        通用输入/输出，控制微控制器上的电信号（通常称为引脚）的最简单方法。
        GPIO通常允许引脚为输入或输出，以及设置或获取其数字值（逻辑“0”或“1”）。
        MicroPython使用 :class:`machine.Pin` 和 :class:`machine.Signal` 类来抽象GPIO访问。
        

    GPIO port
        一组 :term:`GPIO` 引脚，通常基于这些引脚的硬件属性（例如可由同一寄存器控制）。

    heap
        MicroPython存储动态数据的RAM的区域，它由 :term:`Garbage Collector` 自动管理。
        不同的MCU和电路板可用于堆的RAM数量大不相同，因此这将影响程序的复杂程度。
        
    interned string
        MicroPython用于提高使用字符串的效率的优化，插入的由其（唯一）标识而不是其地址引用的字符串。
        因此可以仅通过其标识符进行快速比较。这也意味着可以在内存中对相同的字符串进行重复数据删除。
        内部字符串几乎总是对用户不可见的。


    MCU
        微控制器，与台式机，笔记本电脑或电话相比，微控制器的资源通常要少得多，但他们更小，
        更便宜并且所需的功率也少得多。MicroPyton的尺寸设计的很小，经过优化，足以在普通的
        现代微控制器上运行。


    micropython-lib
        MicroPython 通常（作为）以单个可执行文件/二进制文件的形式分发，只有很少的内置模块。
        没有可与CPython相媲美的广泛标准库。取而代之的是一个相关单独立的项目 `micropython-lib
        <https://github.com/micropython/micropython-lib>`_ ，该项目为CPython标准库中的许多模块提供了实现。
        
        其中一些模块是用纯Python实现的，并且可以在所有端口上使用。但是，这些模块大多数都使用
        FFI访问操作系统功能，因此只能在 :term:`MicroPython Unix port` 端口（对Windows的支持有限）
        上使用。
        
        与 :term:`CPython` stdlib不同，micropython-lib模块旨在单独安装-使用手动复制或者使用 :term:`upip`。
        

    MicroPython port
        MicroPython 支持不同的 :term:`boards <board>`，RTOS 和
        OS，并且可以相对容易地适应新系统。支持特定系统的MicroPython称为该        系统的“端口”。不同的端口可能具有广泛不同的功能。本文档旨在作为跨不同端口（“MicroPython核心”）可用的通用API的参考。请注意，某些端口可能仍会省略此处描述的某些API（例如，由于资源限制）。任何此类差异以及MicroPython核心功能以外的特定于端口的扩展，将在单独的特定于端口的文档中进行描述。
        
    MicroPython Unix port
        unix 端口是 :term:`MicroPython ports
        <MicroPython port>` 主要的端口之一。它旨在在与POSIX兼容的操作系统上运行，例如Linux， MacOS， FreeBSD， Solaris等，用作Windows端口的基础。Unix端口对于快速开发和测试MicroPython
        语言以及与机器无关的功能非常有用。它也可以以类似于 :term:`CPython` 的 ``python`` 可执行文件的方式工作。
        
    .mpy file
        使用 :term:`cross-compiler` 编译 :term:`.py file` 之后的输出文件，其包含
        MicroPython的字节码而不是Python源代码。
        
    native
        通常指“本机代码”，即目标微控制器的机器代码（例如ARM Thumb， Xtensa， x86/x64）。
        可以将 ``@native`` 装饰器应用于MicroPython函数以生成本机代码，而不是该函数的字节码，这可能会更快，但使用更多的RAM。
        
    port
        通常是 :term:`MicroPython port` 的缩写，但也可以是 :term:`GPIO port` 。

    .py file
        Python源码文件.

    REPL
        “读取，评估，打印，循环”的首字母缩写。这是交互式Python提示符，对于调试和测试段代码非常有用。
        大多数 MicroPython 开发板都通UART提供REPL，通常可以通过USB在主机PC进行访问。

    stream
        也称为“文件状对象”。一个Python对象，提供对基础数据的顺序读写访问。一个流对象实现了一个相应的
        接口，该接口由诸如``read()``， ``write()``， ``readinto()``， ``seek()``， ``flush()``，
        ``close()`` 等之类的方法组成。流是 MicroPython 中的重要概念；许多I/O对象实现了流接口，因此可以在不同的上下文中一致且可互换地使用。有关 
        MicroPython 中流的更多信息，请参见 :mod:`uio` 模块。
        
    UART
        “通用异步接收器/发送器”的首字母缩写。这是一个通过一对引脚（TX/RX）发送数据的外设。很多板包括
        一种至少一个UART可用于主机PC的方法作为USB上的串行端口。
        

    upip
        （字面意思是"micro pip"）。 MicroPython的软件包管理器，受 :term:`CPython` 的pip启发，单体积更小，功能却有所减小。upip在 :term:`Unix port <MicroPython Unix port>` 端口
        和 :term:`baremetal`  端口运行，这些端口提供文件系统额网络支持。