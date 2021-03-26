MicroPython port to AWorks RTOS
===============================

这是MicroPython到AWorks RTOS的适配端口，该端口正在开发过程中

AWorks支持的所有开发板，都可以使用MicroPython，但并非所有开发版都经过测试，目前测试过的开发板有（M1052）。

目前支持的功能：

* 通过AWorks 串口控制台进入REPL（交互式提示）。
* `utime` 时间测量以及延迟模块。
* `machine.Pin` GPIO控制类。
* `machine.LED` LED控制类。
* `machine.I2C` I2C控制类。
* `machine.SPI ` SPI控制类。
* `usocket` 网络模块 (IPv4/IPv6).
* "Frozen modules" support to allow to bundle Python modules together
  with firmware. Including complete applications, including with
  run-on-boot capability.

Over time, bindings for various Zephyr subsystems may be added.


运行
--------

下载 AWorks RTOS 固件到开发板，在AWorks Shell系统输入 `python` 命令进入 `REPL` 交互界面。


示例
-------------

 LED闪烁:

    import time
    from machine import Pin
    
    LED = Pin(("led_pin", Pin.board.GPIO1_19), Pin.OUT)
    while True:
        LED.value(1)
        time.sleep(0.5)
        LED.value(0)
        time.sleep(0.5)

以上代码设置M1052的LED引脚GPIO1_19为输出，并以2Hz的频率闪烁。要执行上述示例，请将其复制到粘贴板，在MicroPython中使用 Ctrl+E进入粘贴模式，按 Ctrl+D完成粘贴模式。

使用I2C扫描I2C从设别的示例:

    from machine import I2C
    
    i2c = I2C("I2C_0")
    i2c.scan()
