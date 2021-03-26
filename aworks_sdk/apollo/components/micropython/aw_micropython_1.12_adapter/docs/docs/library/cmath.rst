:mod:`cmath` -- 复数的数学函数
==========================================================

.. module:: cmath
   :synopsis: 复数的数学函数

|见CPython模块| :mod:`python:cmath`.

 ``cmath`` 模块提供了一些用于处理复数函数的功能。此模块需要浮点支持。


函数
---------

.. function:: cos(z)

   返回``z``的余弦值。

.. function:: exp(z)

   返回 ``z`` 的指数。

.. function:: log(z)

   返回 ``z`` 的自然对数。分支切口沿负实轴。

.. function:: log10(z)

   返回以10为底的对数 ``z``. 分支切口沿负实轴。

.. function:: phase(z)

   返回数字 ``z`` 的相位, 范围为 (-pi, +pi].

.. function:: polar(z)

   以元祖形式返回 ``z`` 的极性形式。

.. function:: rect(r, phi)

   返回模数为 ``r`` 并且相位为 ``phi`` 的复数。

.. function:: sin(z)

   返回 ``z`` 的正弦.

.. function:: sqrt(z)

   返回 ``z`` 的平方根.

常量
---------

.. data:: e

   自然对数的底

.. data:: pi

   圆的周长与其直径之比
