:mod:`gc` -- 控制垃圾收集器
==========================================

.. module:: gc
   :synopsis: 控制垃圾收集器

|见CPython模块| :mod:`python:gc`.

函数
---------

.. function:: enable()

   启用自动垃圾收集。

.. function:: disable()

   禁用自动垃圾收集.  堆内存仍然可以分配，并且仍然可以使用,
   :meth:`gc.collect` 手动启动垃圾收集。

.. function:: collect()

   运行垃圾回收。

.. function:: mem_alloc()

   返回分配的堆RAM字节数。

   .. admonition:: 与 CPython 不同
      :class: attention

      此功能是 MicroPython 扩展。

.. function:: mem_free()

   返回可用堆内存 RAM 的字节数或-1（数量未知）。

   .. admonition:: 与 CPython 不同
      :class: attention

      此功能是 MicroPython 扩展。

.. function:: threshold([amount])

   设置或查询其他GC分配阈值，通常一个集合仅当无法满足新分配时，即在内存不足（OOM）条件下才触发收集。如果调用此函数，则除了OOM之外，每次分配 *amount* 个字节之后，都会触发一次收集（总之，自前一段时间后，已经分配了相当数量的字节数）。 *amount* 通常指定为小于小于整个堆的大小，目的是在堆耗尽时更早触发收集，并希望尽早收集将防止过多的内存碎片。这是一种启发式措施，其效果因应用程序而异，并且 *amount* 参数的最佳值也有所不同。

   调用不带参数的函数将会返回阈值的当前值，-1表示禁用配置阈值。

   .. admonition:: 与 CPython 不同
      :class: attention

      此功能是 MicroPython 扩展。CPython 有一个类似的函数 ``set_threshold()``, 
      但是由于不同的GC实现, 其签名和语义也不同。
