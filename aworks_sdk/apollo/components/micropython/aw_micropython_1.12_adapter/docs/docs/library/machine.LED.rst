

calss LED -- LED对象
=============================

LED对象用于控制开发板LED灯的状态。

构造函数
------------

.. class:: machine.LED(id)

创建一个 LED 对象：

    - ``id`` 是LED的编号，取值范围 0 ~ 3 （视具体平台而定）。

方法
------------

.. method:: LED.on()
   
   打开LED灯。
   
.. method:: LED.off()
   
   关闭LED灯
   
.. method:: LED.toggle() 

   翻转LED灯
   
.. method:: LED.set(value)

    设置LED状态，value参数为0或1。

示例
----

使用引脚模块前需要使用 ``from machine import LED`` 导入 LED 模块，示例见 :numref:`LED_code` 。

 .. code-block:: py
    :caption: LED示例
    :name: LED_code
    :linenos:
 
     import utime
     from machine import LED

     def led():
         while 1:
             LED(0).toggle()
             utime.sleep_ms(500)

     if __name__ == '__main__':
         led()