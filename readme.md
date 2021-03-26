### SDK目录说明

| 目录               | 说明                             |
| ------------------ | -------------------------------- |
| aworks_awtk_rt1052 | awtk库、源码，以及rt1052适配代码 |
| aworks_sdk         | aworks库、源代码                 |
| examples           | 应用例程                         |
| projects           | 工程目录                         |
| tools              | 烧录工具                         |

### projects工程目录

| 目录                 | 说明                                                         |
| -------------------- | ------------------------------------------------------------ |
| img_rt1050_debug     | eclipse工程，m1052开发板模板工程                             |
| img_rt1050_debug_mdk | keil工程，m1052开发板模板工程                                |
| m1052_awtk           | eclipse工程，适配了awtk GUI组件                              |
| m1052_awtk_mdk       | keil工程，适配了awtk GUI组件                                 |
| m1052_zws            | eclipse工程，适配了ZWS云平台，<br />文档目录``projects\m1052_zws\tools\doc`` |

- **注意:**

为了防止用户自己配置工程出错，不建议用户自己配置awtk工程和zws工程，可直接使用已经配置好的m1052_awtk、 m1052_awtk_mdk 和 m1052_zws工程，或通过拷贝的方式新建工程。

### 例程说明

| 例程类型    | 说明                                                         |
| ----------- | ------------------------------------------------------------ |
| application | 应用例程，具有具体应用场景的应用方案，如 web 服务器、 shell 应用等 |
| component   | 组件的基本使用例程，如文件系统、网络等                       |
| feature     | CPU 特殊外设例程，如浮点运算、中断响应测试等例程             |
| peripheral  | 基础外设例程，如 SPI、 I2C、 CAN、 ADC 等                    |
| system      | 系统内核例程，如任务、消息、信号量、软件定时器等             |

### 参考文档

sdk使用方法，请点击链接：https://manual.zlg.cn/web/#/23?page_id=942 ，阅读在线文档。