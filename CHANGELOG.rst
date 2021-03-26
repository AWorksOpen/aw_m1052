

版本记录
========
.. describe:: 2.3.0 <2021-03-24>
    
    * 驱动：添加 atgm332d 传感器驱动
            添加 fxos8700 传感器驱动
            添加 ADC KEY 驱动
            USB库更新至1.1.14，解决USB穿扩发送互斥锁冲突的问题
            更新 gm510 4g 模块的部分获取信息功能
            更新 4g 模块的短信功能
            修改 gprs 解码短信函数
            wifi 网卡增加混杂模式
            修复 shtc1传感器 采集不到数据就跳出循环的问题
            修复低温情况下芯片内部数据采样点可能会溢出的问题
            更新音频框架，mqs声卡驱动
            spi主机驱动增加硬件cs模式

    * 组件：添加 zlib解压缩 组件
            修复 littlefs文件系统的 bug
            实现自动挂载文件系统功能
            修复lffs文件系统操作文件描述符时未检测其是文件类型还是目录类型的bug
            新增异步请求应答机制aw_reqsched
            lwip从1.4版本升级至2.x版本
            增加CANFESTIVAL组件库
            移动awtk gui组件源码至sdk包根目录下
            
    * 工程：按照功能拆分gpt定时器配置文件
            删除MinSizeRel调试下载模式
            按照组件、驱动划分，将单个库文件拆分为多个库文件
            移动awtk工程至projects目录
            移动zws工程至projects目录

    * 例程：更新gprs例程
            修复spi例程异步传输时可能崩溃的问题
            修改USB设备模式自定义例程的bug
            添加usb例程的相关资料
            修复spi丛机例程通信乱码的问题
            优化了部分例程
            
.. describe:: 2.2.0 <2020-08-25>

    * net：              多网卡管理功能更新 
                         处理udp用静态路由表查找异常问题
                         增加支持使用域名来检测可用网卡
                         修正ping间歇读数过大与解析域名时消息显示错误问题
    * canopen：          增加canopen软件库
    * USB:               更新USB文件与USB库，修复若干个bug，增加mtp功能
    * gprs:              增加ec20和me909s的eth模式
                         增加控制gprs模块的gps功能接口
    * drivers：          更新nandflash组件及增加相关nanflash驱动
                         更新qtimer、eflexpwm驱动,添加精准输出pwm周期个数的功能
    * projects           更新LCD配置文件，添加动态配置和复位功能
                         更新双摄像头框架的配置文件
    * examples：         增加canopen主从站例程
                         增加USB HOST U盘测速、mtp例程
                         修改USB HOST 转串口例程逻辑

.. describe:: 2.1.0 <2020-04-07>

    * 文件系统：         修改文件系统默认挂载磁盘数为5
    * lffs文件系统：     mkfs 时增加检查该 dev_path 是否有被挂载的功能；
                         更新坏块处理功能
    * yaffs文件系统：    处理 lseek 操作的 bug
    * shell命令：        增加cp命令；
                         更改stack的shell指令文件，添加最大栈深信息；
                         更新wifi startap命令bug，提前检查参数格式；
                         增加cpuload指令
    * log日志：          增加log缓存
    * gprs:              修改 ec20 结构，更新gprs状态检查机制，完善log
    * wifi:              消除固件丢失后启动AP时发生的空指针引用
    * net：              ping_send释放失败的数据包
    * USB：              添加了USB HOST等时传输
                         添加了通用USB摄像头驱动
                         添加了USB转串口驱动(支持FTDI，CH340，CP210X和PL2303)
                         修正了USB HOST不能提交多个传输请求包bug
                         修改了USB Device模式的鼠标、键盘和虚拟串口驱动
                         修正了USB Device模式重复挂起的bug
    * drivers：          增加读写擦除flash保留扇区接口
                         增加时间戳服务
    * projects：         增加时间戳配置文件
    * examples：         更新awtk源码至v1.3rc3版本
                         修改usb_ms例程存储空间分配的bug
                         新添加USB Device鼠标、键盘、虚拟串口demo
                         新添加USB HOST摄像头和串口demo
                         新增ADC_ETC例程
                         新增lpi2c_slave例程
                         新增lpspi_slave例程

.. describe:: 2.0.1 <2019-12-31>

    * 添加传感器配置文件
    * 添加LCD屏配置文件
    * 修改OV7725驱动
    * 增加对spi-flash的写保护配置
    * 更新I2C驱动，添加外设异常时复位接口
    * 添加软件复位
    
.. describe:: 2.0.0 <2019-08-27>

    * 修改教程文档
    * 修改生产固件代码
    * 删除dcd文件中的代码并更新rt1052flash烧写算法，将sdram的初始化移到startup.S中，避免flash自启动时会卡死在内部bootloader的问题
    * 修改ftpd运行buffer、添加my_libc函数重写
    * 将flash启动时dcd文件中的sdram初始化代码，移到startup_rt1050_gcc.S中执行以避免启动时卡在内部bootloader的问题
    * 修改SCCB通信，使其兼容硬件I2C
    * 修改LWIP配置，提高TCP速率
    * 添加pll3时钟初始化代码，启动后重新配置pll3的频率
    * 修改SDIO驱动，避免程序在SDIO初始化卡死
    * 添加SAI3与MQS自动化配置、LED和QTimer3的CAP冲突检测


.. describe:: 1.1.0-alpha2 <2019-07-24>

    ++ New feature
    
    * 优化flash烧写算法，提高烧写固件到flash的速度
    * 添加代码重定位的功能,支持将部分代码拷贝到内部ram中运行
    
    ++ Bugfixes
    
    * 修复从Flash启动，导致在921600波特率下误差过大的问题


.. describe:: 1.1.0-alpha <2019-07-9>

    ++ New feature
    
    * 添加csm300x的配置文件
    * MDK能够使用标准libc(前面只能使用microlib)
    * 添加音频驱动遇到不支持的音频格式时，提示用户的功能
    * CAN驱动不再支持单次发送、单次自发自收
    * awtk：添加帧缓冲设备，awtk适配文件，优化双缓冲驱动、添加三缓存驱动
    * 添加QTime定时器驱动设备
    * 增加触摸屏初始化，增加数据清除命令
    * 开源驱动和中间层
    
    ++ Bugfixes
    
    * 修复LCD背光调节
    * fm175xx错误处理中内存bug
    * 修正RTK信号量超时和give同时可能产生的错误处理，解决以太网速度慢的问题
    * 修复FlexSPI驱动中四线模式中导致少量的Flash无法正常烧写的问题
    * 解决音频驱动在播放不支持音频时会崩溃的问题
    * 修正enet驱动速度和双工模式改变后工作不正常的问题
    * 修复CAN驱动中对自发自收寄存器配置错误的问题，修复驱动中中断状态处理错误写法
    * 修复keil工程启动时未初始化堆栈的问题
    * 解决ARMCC不能编译C++的问题


.. describe:: 1.0.5-alpha <2019-04-15>

    * 添加lua驱动，屏蔽modbus驱动
    * 为keil工程中添加JLink Device型号配置文件
    * 修改网卡驱动,适配新的mii驱动
    * 更新apollo和imxrt10xx_common子模块,更新lpuart驱动以避免signed char被writel写入到寄存器时的隐含类型提升问题
    * 修复了GPIO无法读取IO输出的bug
    * 修改ci脚本和Python脚本,修改 build_sdk.py 脚本，用于生成不同的sdk包,为构建脚本添加README说明
    * 现在使用imxrt10xx_common子模块中提供的LPSPI驱动
    * --修改触摸设备ID，增加工程通用触摸ID宏 --修改flexspi NVRAM数据区域配置，将区域调整至尾部 --更新子模块，修改了触摸数据保存配置
    * 更新api参考手册，删除程序中V1.0.3版本标识
    * 子模块添加canfestival的适配
    * 添加canfestival的适配，修改can配置文件
    * 添加m1052和EasyARM烧写工程编译检查
    * 添加ublox配置文件
    * 将TRUE和FALSE替换为AW_开头的宏，另外修正了一些直接与TRUE和FALSE比较的代码
    * 删除掉flexio模拟的uart，因为这基本不会有人用，而且因为串口框架更新，这个驱动也不符合规范
    * 更新apollo，替换bool_t为aw_bool_t，现在aw_types.h中不再定义bool_t了
    * 现在使用imxrt10xx_common中的lpuart驱动    
    * 更新文档模板 致远电子 -> 立功科技
    * "更新1050 lpi2c 驱动，适配了合并trans的中间层"
    * 添加了imxrt10xx公用模块
    * 删除用不到的common代码，准备添加公用驱动模块
    * 优化easy_arm_rt1052 MDK工程 完善rt1052 dcd与FLM配置文件
    * 更新example子模块，以便使用最新的串口例程，现在串口没有poll模式和rx callback了
    * 改正《M1050 SDK工程链接脚本配置指南》文档中的一处错误。
    * 修正前次提交错误，导致aw_serial_ioctl在非任务环境下调用而崩溃的问题
    * 添加ZLG电机示例工程，发给电机组的sdk本地编译并不加密
    * 上传一些以前未上传的核心板和底板的原理图资料。
    * 解决内部温度计驱动的一处编译错误。
    * 优化rt105x sdram配置 优化、完善rt105x MDK工程
    * 添加提取examples的scons脚本，在工程中增加sqlite3头文件路径，针对不同开发板添加demo_config适配头文件
    * 提交evk板子对应的sdram资料
    * 更新spi驱动
    * 修正keil环境下编译的代码调试的时候不能显示反汇编的问题
    * 更新子模块
    * 更新API手册
    * 更新changelog.rst
    * 在的LCD开启的情况下，频繁存取SDRAM的内存，会出现偶发性的错误问题(特别是flash xip执行时)。将cache策略配置修改后，问题解决。
    * 更新changelog.rst文件，更新apollo、examples子模块
    * 更新apollo以便使用新的中断框架，提高中断效率，解决AW_DEFER_START导致的armcc编译不通过的问题
    * 修正gpio驱动中因为__gpio_pin_cfg和__gpio_pin_request忘记释放互斥锁而导致的中断一直处于关闭状态的bug
    * --ECHI初始化接口变更 --更新USB库，解决1.0  1.1设备通信错误问题 --更新子模块
    * 修正img_rt1050_debug eclipse工程配置文件，主要增加refresh策略
    * 更新easy_arm_1052 keil快速入门手册、eclipse快速入门手册、用户手册、M1052快速入门手册、API参考手册，添加API手册构建工具
    * 现在整个elf文件data段减少了63K左右，也就是最终的bin文件小了63K左右
    * 更新apollo至007b4aff9cb6d44ad2537dd80a49d3515c5aa815
    * 修改EasyARM RT1052的keil文档中FLM算法RAM分配的插图，并重新提交烧写工具的相关源码。
    * 经测试，现在使用新架构的rtk，实时性更好，未来可以支持CPU直接跳转到ISR，以提高关键中断代码的响应时间
    * 在调试配置文件和初始化入口代码中尽快禁用rtwdog，以避免出现调试过程中初始化bss和data段的时候因为rtwdog而复位
    * 修正sqlite3.c编译可能会卡死的问题
    * 现在idle状态不在修改CLPCR寄存器了
    * 使用apollo中通用的armcc工具来构建编译环境
    * 现在armcc和gcc使用不同的构建路径，这样编译完gcc后，编译armcc，再编译gcc的话不会再重新编译
    * 修改所有的E开头的错误码为AW_E开头
    * 添加sqlite3组件
    * 修正了FlexSPI Flash启动工具将keil编译的srec文件转换为imx文件工作不正常的问题

    
    **examples commits info**
    
    * 修改canopen的注释
    * demo_mem_info.c中超出编译器data边界，修改TRUNK_SIZE 为(30 * 1024)； demo_i2c_rtc_async.c中数据校验输出逻辑错误
    * 增加spi_flash里程里面包含的头文件路径
    * 修改了一些外设的bug（nand_mtd，spi_flash，spi_flash_fs ）
    * TRUE FALSE更新
    * CNT226和ublox例程demo公用
    * 修改CANopen例程，删除非标准化的例程，canopen、canfestival共用一个例程
    * 更新gpio demo
    * 修改imx1052 1.05版本测试的bug
    * 更新sqlite3 demo
    * --更新awtk软件包
    * 添加lua demo
    * --更新AWTK demo --修改目录结构和相关配置
    * 更新gps
    * boot_t类型 -> aw_bool_t类型
    * 现在串口已经不再支持POLL模式和接收回调函数了,故而修改了串口例程
    * sqlite3 demo 挂载nand flash
    * 增加slab例程
    * 增加easyarm1052_awtk_debug工程
    * 暂时屏蔽sdk中没有的API，更改readme.txt，将demo中的gpio和与开发板有关的信息更改为标准宏
    * 更新spi flash示例
    * 更新gprs例程名称
    * 更改触摸校准文件，在demo中增加显示屏尺寸自动适配操作
    * 更新shell demo
    * 更新enet_web_demo
    * 更新awtk工程配置
    * 修改wdt的示例，更简单，不用创建任务，并且可以验证不喂狗会重启
    * --更新AWTK 以及对应例程
    * 更改uffs为lffs，添加hwtimer捕获示例
    * 更新定时器示例
    * 更新示例
    * 更新示例
    * 之前部分例子编码被改成utf-8，与其他不用，在eclipse上显示部分中文乱码，统一编码，把这部分例子编码还原
    * 添加sqlite3 demo 添加gpt demo
    * 修改外设用例，都以线程方式执行
    * 修改gprs例程
    * --修改log例程注释
    * 在create_dox.py中增加例程简介
    * 添加python脚本，自动生成dox文件
    * 添加dox文件夹，在生成的doxygen文档中为例程分组及生成例程连接
    * 为生成完整的doxygen文档修改例程中的注释
    * 添加"gprs状态检测"例程
    * 添加web用例
    * demo_semb_entry的函数原型修正
    * 修正aw_list base示例程序中函数传参错误导致执行不正常的问题
    
    

.. describe:: 1.0.4-alpha <2018-11-23>
    * GPIO中断添加"aw_int_xx()"函数支持
    * 将"awbl_imx1050_gpio_private.h"文件重命名为"imx1050_pin_property.h"文件
    * 更新1050 pxp驱动
    * 添加HTS221,SHTC1,LIS3MDL用户配置文件及修改工程配置文件
    * 更新imx1052的内部RTC驱动，适配日历时间接口。
    * 添加CANopen例程以及Modbus主站例程
    * 添加CANfestival-3.8组件
    * 添加了《EasyARM RT1052 SDK 快速入门手册 keil版》的源文件,纠正《EasyARM RT1052 SDK用户参考手册》中存在的问题。
    * 配置SDRAM到144Mhz, 同步修改GDB DCD文件。
    * 更新《AWorks M1052 快速入门手册 eclipse》
    * 更新《AWorks M1052 快速入门手册 keil》
    * 更新《AWorks M1052 用户手册》


    * 修改触摸BUG，去掉不合理的过滤
    * add LPCD feature in FM175XX driver.
    * remove AWBL_FM175XX_RFU1E_REG setting in fm175xx_for_piccb_config fucntion.
    * Setting TypeBreg(0x1E) bit field TxEGT to 11b;
    * add features for supporting PICCB.
    * add some annotation.
    * change ``awbl_fm175xx_cmd_execute`` function complexity and fix an-collision of several card in RF.
    * fix bug: Authent directive using 5ms timeout instead of ``AW_SEM_WAIT_FOREVER`` for the semaphore.
    * change loop checking to irq trigger in FM175XX driver.
    * little modify for driving FM17550
    * 修复了传感器标准层传入NULL的Bug，并添加HTS221，SHTC1和LIS3MDL传感器的驱动
    * 更新pcf85263的驱动，添加日历时间获取和设置接口。
    * 更新GPRS，解决strstr传入NULL崩溃问题
    * 修改了awbl_spibus.c中_spi_check_trans函数返回值错误的问题
    * 添加适配最新的CANfestival3.8组件
    * 添加canfestival-3.0组件
    * 修改uffs adaper编译不过的问题
    * 修改SPI驱动框架
    * 修改为单通道灰度图解码
    * 修正Wi-Fi设备异常时net-tools状态记录失去同步的问题
    * 更新can标准接口中的例程
    * 解决CAN中间层发送由于底下驱动返回错误，导致一直处于busy无法再发送，直到总线错误才可恢复的bug



.. describe:: 1.0.3-alpha <2018-10-31>
    * 增加EasyARM RT1052的eclipse工程和Keil工程
    * 修改SPI驱动框架，更新了SPI驱动，由于SPI DMA方式存在逻辑上
      的Bug, 现在的SPI驱动中去掉了DMA的支持，SPI的配置头文件有较大变化
    * 修改已有文档的一些错误。
    * 增加《i.MX RT105x系列产品生产固件烧写指南 V1.0.0》
    * 增加《EasyARM RT1052 SDK 快速入门文档 eclipse V1.0.0》
    * 增加《EasyARM RT1052 SDK 用户参考手册 V1.0.0》
    * 增加《EasyARM RT1052 SDK 快速入门文档 keil V1.0.0》

.. describe:: 1.0.2-alpha <2018-08-09>

    * 修复若干驱动bug
    * 增加二维码组件
    * 增加4G模块

.. describe:: 1.0.1-alpha <2018-07-02>

    * 测试版本

.. describe:: 1.0.0-alpha <2018-04-27>

    * 测试版本