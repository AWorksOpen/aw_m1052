.. _micropython_lib:

MicroPython函数库
=====================

.. warning::

   本章重要摘要

   * MicroPython 为每个模块实现了Python功能的子集。
   * 为了简化可扩展性， 标准 Python 模块的 MicroPython 版本 通常有 ``u`` ("micro") 前缀。
   * 任何特定的 MicroPython 变体或端口都可能会缺少本通用文档中描述的任何功能（由于资源限制或者其他限制）。


本章介绍了MicroPython内置模块(函数和类库). 这些模块分为几类:

* 实现标准 Python 功能的子集并且不打算由用户扩展的模块。
* 实现 Python 功能的子集，并提供用户扩展（通过Python代码）的模块。
* 实现对Python标准库的 MicroPython 扩展模块。
* 特定于特定 :term:`MicroPython port` 的模块，因此不可移植。

注意模块及其内容的可用性: 本文档通常希望描述在MicroPython工程中实现的所有模块和功能/类。但是，MicroPython是高度可配置的，并且特定板卡/嵌入式系统的每个端口仅提供MicroPython库的一个子集。对于正式支持的端口，会努力过滤掉不适用的项目，或者使用 "Availability:"子句标记各个描述，以描述哪些端口提供给特定功能。

考虑到这一点，仍请注意，本文档中描述的模块（甚至整个模块）中的功能和类在特定系统上的特定的MicroPython版本中 **可能不可用** 。 查找特定功能的可用性/不可用性的常规信息的最佳位置是“常规信息”部分，其中包含与特定 :term:`MicroPython port` 有关的信息。

在某些端口上，你可以通过在 REPL 中输入以下内容来发现可用性，这些库可以导入::

    help(modules)

除了本文档中描述的内置库之外，Python标准库的许多其他模块，以及对它的进一步MicroPython扩展都可以在 :term:`micropython-lib` 中找到。

.. _cpython_diffs:

Python标准库和微库
---------------------------------------------

下面的标准 Python 库已经被微型化，以适应MicroPython的原理。这些库提供了该模块的核心功能，旨在代替标准Python库。
以下某些模块使用标准Python名称，但是前缀为 "u"， 例如 ``ujson`` 替代了 ``json``. 这表示该模块是
微库，即仅实现了CPython模块功能的的子集。通过以不同方式命名他们，用户可以选择编写Python级别的模块来扩展功能，以更好的与CPython兼容（实际上，这是由上述提到的 :term:`micropython-lib` 项目完成的）。

在某些嵌入式平台上， 添加 Python 级别的包装器模块以实现与CPython的命名兼容非常麻烦，而微模块可以通过其u命名和非u命名来使用。该非u命名可被您的包路径中的同名文件覆盖。例如， ``import json`` 将首先搜索一个文件 ``json.py`` 或目录 ``json`` ，若搜寻到相关内容，则加载该数据包。若未搜寻到目标信息，则后退以加载内置 ``ujson`` 模块。


.. toctree::
   :maxdepth: 1

   builtins.rst
   cmath.rst
   gc.rst
   math.rst
   sys.rst
   uarray.rst
   uasyncio.rst
   ubinascii.rst
   ucollections.rst
   uerrno.rst
   uhashlib.rst
   uheapq.rst
   uio.rst
   ujson.rst
   uos.rst
   ure.rst
   uselect.rst
   usocket.rst
   ussl.rst
   ustruct.rst
   utime.rst
   uzlib.rst
   _thread.rst


MicroPython-特定库
------------------------------

MicroPython 实现的特定功能可在以下库找到。

.. toctree::
   :maxdepth: 1
   
   btree.rst
   framebuf.rst
   machine.rst
   micropython.rst
   .. network.rst:
   .. ubluetooth.rst:
   ucryptolib.rst
   uctypes.rst

AWorks-特定库
------------------------------

AWorks实现的特定功能可在以下库找到。

.. toctree::
   :maxdepth: 1
   