/*******************************************************************************

*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AnyWhere 工程参数配置文件
 */

#ifndef __AW_PRJ_PARAMS_H
#define __AW_PRJ_PARAMS_H

/**
 * \addtogroup grp_aw_prj_param
 * @{
 */

/*******************************************************************************
  硬件剪裁
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_hwcfg   硬件剪裁
 *
 * -# 注释掉外设宏定义即可禁能对应的外设，反之，则使能;
 * -# 使能某个外设后，该外设所依赖的软件组件会被自动配置进来，如无特殊需求，则
 *    无需进行下面的“组件剪裁”；
 * -# 中断控制器和GPIO是最基础的外设，默认被使能。
 * @{
 */
#define AW_DEV_IMX1050_LPUART1          /**< \brief iMX1050 LPUART1 (串口1) */
//#define AW_DEV_IMX1050_LPUART2          /**< \brief iMX1050 LPUART2 (串口2) */
//#define AW_DEV_IMX1050_LPUART3          /**< \brief iMX1050 LPUART3 (串口3) */
//#define AW_DEV_IMX1050_LPUART4          /**< \brief iMX1050 LPUART4 (串口4) */
//#define AW_DEV_IMX1050_LPUART5          /**< \brief iMX1050 LPUART5 (串口5) */
//#define AW_DEV_IMX1050_LPUART6          /**< \brief iMX1050 LPUART6 (串口6) */
//#define AW_DEV_IMX1050_LPUART7          /**< \brief iMX1050 LPUART7 (串口7) */
//#define AW_DEV_IMX1050_LPUART8          /**< \brief iMX1050 LPUART8 (串口8) */
//#define AW_DEV_IMX1050_LPI2C1           /**< \brief iMX1050 LPI2C1 (I2C1)*/
//#define AW_DEV_IMX1050_LPI2C2           /**< \brief iMX1050 LPI2C2 (I2C2)*/
//#define AW_DEV_IMX1050_LPSPI1           /**< \brief iMX1050 LPSPI1 (SPI1) */
//#define AW_DEV_IMX1050_LPSPI2           /**< \brief iMX1050 LPSPI2 (SPI2) */
//#define AW_DEV_IMX1050_LPSPI3           /**< \brief iMX1050 LPSPI3 (SPI3) */
//#define AW_DEV_IMX1050_LPSPI4           /**< \brief iMX1050 LPSPI4 (SPI4) */
#define AW_DEV_IMX1050_ENET             /**< \brief iMX1050 ENET (有线网卡) */
#define AW_DEV_IMX1050_EDMA             /**< \brief iMX1050 eDMA */
#define AW_DEV_IMX1050_GPT1_TIMER       /**< \brief iMX1050 GPT1 (GPT定时器1) */
//#define AW_DEV_IMX1050_GPT1_CAP         /**< \brief iMX1050 GPT1 CAP */
//#define AW_DEV_IMX1050_GPT1_PWM         /**< \brief iMX1050 GPT1 PWM */
//#define AW_DEV_IMX1050_GPT2_TIMER       /**< \brief iMX1050 GPT2 (GPT定时器2) */
//#define AW_DEV_IMX1050_GPT2_CAP         /**< \brief iMX1050 GPT2 CAP */
//#define AW_DEV_IMX1050_GPT2_PWM         /**< \brief iMX1050 GPT2 PWM */
//#define AW_DEV_IMX1050_PIT              /**< \brief iMX1050 PIT (PIT定时器) */
//#define AW_DEV_IMX1050_ADC1             /**< \brief iMX1050 ADC */
//#define AW_DEV_IMX1050_ADC2             /**< \brief iMX1050 ADC2 */
#define AW_DEV_IMX1050_USDHC1            /**< \brief iMX1050 USDHC1 (SD卡) */
//#define AW_DEV_IMX1050_USDHC2            /**< \brief iMX1050 USDHC2 (SD卡) */
//#define AW_DEV_IMX1050_SAI1             /**< \brief iMX1050 SAI1 */
#define AW_DEV_IMX1050_SAI3             /**< \brief iMX1050 SAI3 */
#define AW_DEV_IMX1050_MQS              /**< \brief iMX1050 MQS */
#define AW_DEV_IMX1050_OCOTP            /**< \brief iMX1050 OCOTP */
#define AW_DEV_IMX1050_FLEXSPI          /**< \brief iMX1050 FLEXSPI (用于读写SPI Flash)*/
//#define AW_CFG_FLEXSPI_FLASH_MTD        /**< \brief 使能Flexspi flash MTD设备 */
//#define AW_DEV_IMX1050_CAN1             /**< \brief iMX1050 FlexCAN1 */
//#define AW_DEV_IMX1050_CAN2             /**< \brief iMX1050 FlexCAN2 */
//#define AW_DEV_IMX1050_EFLEX_PWM1       /**< \brief iMX1050 eFlexPWM1 */
//#define AW_DEV_IMX1050_EFLEX_PWM4       /**< \brief iMX1050 eFlexPWM4 */
//#define AW_DEV_IMX1050_QTIMER1_PWM        /**< \brief iMX1050 QTimer1 PWM */
//#define AW_DEV_IMX1050_QTIMER1_CAP        /**< \brief iMX1050 QTimer1 CAP */
//#define AW_DEV_IMX1050_QTIMER1_TIMER0     /**< \brief IMX1050 QTimer1 TIMER */
//#define AW_DEV_IMX1050_QTIMER2_PWM        /**< \brief iMX1050 QTimer2 PWM */
//#define AW_DEV_IMX1050_QTIMER2_CAP        /**< \brief iMX1050 QTimer2 CAP */
//#define AW_DEV_IMX1050_QTIMER2_TIMER0     /**< \brief IMX1050 QTimer2 TIMER */
//#define AW_DEV_IMX1050_QTIMER3_PWM        /**< \brief iMX1050 QTimer3 PWM */
//#define AW_DEV_IMX1050_QTIMER3_CAP        /**< \brief iMX1050 QTimer3 CAP */
//#define AW_DEV_IMX1050_QTIMER3_TIMER0     /**< \brief IMX1050 QTimer3 TIMER */
//#define AW_DEV_IMX1050_QTIMER4_PWM        /**< \brief iMX1050 QTimer4 PWM */
//#define AW_DEV_IMX1050_QTIMER4_CAP        /**< \brief iMX1050 QTimer4 CAP */
//#define AW_DEV_IMX1050_QTIMER4_TIMER0     /**< \brief IMX1050 QTimer4 TIMER */
//#define AW_DEV_IMX1050_ENC1             /**< \brief iMX1050 正交解码外设(可用于编码器测速) */
//#define AW_DEV_IMX1050_TEMPMON          /**< \brief iMX1050 内部温度监控外设 */
//#define AW_DEV_IMX1050_ACMP1            /**< \brief iMX1050 内部模拟比较器外设 */
//#define AW_DEV_IMX1050_RTC              /**< \brief iMX1050 内部SNVS RTC */
#define AW_DEV_IMX1050_USBH1            /**< \brief USB Host 1 */
#define AW_DEV_IMX1050_USBH2            /**< \brief USB Host 2 */
//#define AW_DEV_IMX1050_USBD             /**< \brief USB Device */
//#define AW_DEV_IMX1050_SEMC             /**< \brief iMX1050 外部存储控制器 */
#define AW_DEV_IMX1050_TIMESTAMP        /**< \brief iMX1050 时间戳服务 */


#define AW_DEV_GPIO_LED                 /**< \brief LED */
//#define AW_DEV_GPIO_KEY                 /**< \brief 按键 */
#define AW_DEV_PWM_BUZZER               /**< \brief PWM Buzzer(蜂鸣器，需要配套PWM) */
#define AW_DEV_GPIO_WDT                 /**< \brief GPIO看门狗 */
//#define AW_DEV_GPIO_I2C_0               /**< \brief GPIO 模拟 I2C */

//#define AW_CFG_SPI_FLASH_MTD           /* SPI flash 使能 标准 SPI FLASH MTD 接口操作 */
//#define AW_CFG_SPI_FLASH_NVRAM         /* SPI flash 使能 标准 SPI FLASH NVRAM 接口操作 */

//#define AW_DEV_EXTEND_PCF85063          /**< \brief PCF85063实时时钟(需要配套I2C外设) */

#define AW_DEV_CYW43362_WIFI_SPI_0      /**< CYW43362 WiFi模块(需配套SPI外设) */

#define AW_DEV_TFT_4_3                /**< \brief TFT-4.3(A、CAP)显示屏 */
//#define AW_DEV_AP_TFT7_0              /**< \brief AP_TFT7_0显示屏 */
//#define AW_DEV_LCD_TM070RDH12_24B       /**< \brief LCD-TM070RDH12-24B显示屏 */
//#define AW_DEV_LCD_640480W056TR         /**< \brief LCD-640480W056TR显示屏 */
//#define AW_DEV_LCD_800600W080TR         /**< \brief LCD-800600W080TR显示屏 */
#define AW_DEV_BU21029MUV               /**< \brief i2c电阻触摸屏(需要I2C外设) */
//#define AW_DEV_FT5X06                   /**< \brief i2c电容触摸屏(需要I2C外设) */
//#define AW_DEV_IMX1050_TS               /**< \brief 电阻触摸屏 */

//#define AW_DEV_NANDFLASH_MX30LF1G08     /**< \brief NAND FLASH(需要 SEMC 外设)  */
//#define AW_DEV_NANDFLASH_S34ML01G2      /**< \brief NAND FLASH(需要 SEMC 外设)  */
//#define AW_DEV_NANDFLASH_S34ML02G2      /**< \brief NAND FLASH(需要 SEMC 外设)  */

//#define AW_DEV_FM175XX_SPI3_0         /**< \brief Mifare卡驱动(需要SPI外设) */

//#define AW_DEV_SX127X                 /**< \brief LoRa sx127x  */
//#define AW_DEV_IMX1050_CSI
//#define AW_DEV_OV7725
//#define AW_DEV_GC0308

//#define AW_DEV_GPRS_SIM800
//#define AW_DEV_GPRS_ME909S
//#define AW_DEV_GPRS_U9300C
//#define AW_DEV_GPRS_EC20

//#define AW_DEV_GNSS_UBLOX               /*gps设备*/

//#define AW_DEV_CSM300X         /**< \brief uart/spi转can总线模块  */

//#define AW_DEV_I2C_ZSN603               /* ZSN603(RFID芯片)  I2C模式 */
//#define AW_DEV_UART_ZSN603              /* ZSN603(RFID芯片)  UART模式 */

/** @} grp_aw_plfm_params_hwcfg */




/*******************************************************************************
  组件剪裁
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_sftcfg   组件剪裁
 *
 * -# 注释掉组件宏定义即可禁能对应的组件，反之，则使能;
 * -# 使能某个组件后，该外设所依赖的软件组件会被自动配置进来;
 * -# 若某个硬件外设依赖这里的组件，当该硬件外设使能时，无论这些被依赖的组件
 *    是否被使能，都将被自动配置进来。
 * -# 注释中带“(必须)”字样的，表示该组件必须被使能；注释中带“(自动配置)”字样的
 *    表示该组件是可以被自动剪裁的，通常，这些组件保持禁能就好；
 *    其它组件则根据自己的需要，如果需要用到就使能，未使用就可以禁能。
 * @{
 */
#define AW_COM_CONSOLE          /**< \brief 控制台组件，控制 aw_kprintf 输出使能，还包括AW_INFOF,AW_ERRF,AW_LOGF等  */
#define AW_COM_SERIAL           /**< \brief 通用串行组件(串口) */
#define AW_COM_SHELL_SERIAL       /**< \brief 使能串口shell */
//#define AW_COM_SHELL_TELNET     /**< \brief 使用telent远程连接shell */
#define AW_COM_SDCARD
//#define AW_COM_NETWORK          /**< \brief 使能网络协议栈 */
//#define AW_COM_FTPD             /**< \brief ftp server 组件 */
#define AW_COM_BLOCK_DEV        /**< \brief 使能块设备驱动框架 */
//#define AW_COM_MTD_DEV          /**< \brief 使能 MTD(Flash)设备驱动框架 */

#define AW_COM_IO_SYSTEM        /**< \brief 使能 I/O 子系统 */
#define AW_COM_FS_ROOTFS        /**< \brief 使能根文件系统 */
//#define AW_COM_FS_RAWFS         /**< \brief 使能 RAW 文件系统 */
#define AW_COM_FS_FATFS         /**< \brief 使能 FAT 文件系统 */
#define AW_COM_FS_LFFS          /**< \brief 使能 LFFS 文件系统 */
//#define AW_COM_FS_LITTLEFS      /**< \brief 使能 LITTLEFS 文件系统 */
//#define AW_COM_FS_TXFS          /**< \brief 使能事务型文件系统 */
#define AW_COM_FS_YAFFS         /**< \brief 使能 YAFFS 文件系统 */
#define AW_COM_MOUNT            /**< \brief 存储卷挂载点管理 */

#define AW_COM_USBH             /**< \brief USB协议栈裁(Host)组件 */
#define AW_COM_USBD             /**< \brief USB协议栈裁(Device)组件 */

//#define AW_COM_FTL              /**< \brief FLASH 管理层，负责MTD设备的均衡磨损、坏块管理等 */
//#define AW_COM_LOG              /**< \brief 系统日志组件 */

//#define AW_COM_CANFESTIVAL      /**< \brief CAN应用高层协议栈CANFestival组件 */
//#define AW_COM_CANOPEN          /**< \brief CAN应用高层协议栈CANopen组件 */

/** @} grp_aw_plfm_params_sftcfg */


//#define AW_COM_SHELL_LUA              /*< \breif 注册lua组件shell命令*/
//#define AW_COM_SHELL_LUAC              /*< \breif 注册luac组件shell命令*/

//#define AW_COM_SHELL_MPY          /**< \brief 注册python命令*/

//#define AW_COM_AWSA                 /**< \brief 注册音频框架*/

/*******************************************************************************
 传感器裁剪
 ******************************************************************************/
/**
 * \addtogroup sensor_aw_plfm_params_hwcfg   传感器裁剪与ID分配
 * @{
 */
//#define AW_DEV_SENSOR_HTS221           /**< \brief 温湿度传感器HTS221 */
//#define AW_DEV_SENSOR_SHTC1            /**< \brief 温湿度传感器SHTC1  */
//#define AW_DEV_SENSOR_LIS3MDL          /**< \brief 三轴磁传感器LIS3MDL  */
//#define AW_DEV_SENSOR_BME280           /**< \brief 温湿度传感器BME280 */
//#define AW_DEV_SENSOR_BMA253           /**< \brief 三轴加速度传感器BMA253  */
//#define AW_DEV_SENSOR_BMP280           /**< \brief 气压传感器BMP280  */
//#define AW_DEV_SENSOR_LPS22HB          /**< \brief 气压传感器LPS22HB  */
//#define AW_DEV_SENSOR_LSM6DSL          /**< \brief 三轴加速度和三轴陀螺仪传感器LSM6DSL  */
//#define AW_DEV_SENSOR_BH1730           /**< \brief 光照强度及近距离传感器BH1730  */
//#define AW_DEV_SENSOR_BMG160           /**< \brief 三轴陀螺仪传感器BMG160  */
//#define AW_DEV_SENSOR_MMC5883MA        /**< \brief 三轴磁传感器MMC5883MA  */
//#define AW_DEV_SENSOR_LTR553           /**< \brief 光照强度及近距离传感器LTR-553ALS-01  */

#define SENSOR_HTS221_START_ID          0    /**< \brief 温、湿度传感器HTS221起始ID，               占用0-1 chn：2 */
#define SENSOR_SHTC1_START_ID           2    /**< \brief 温、湿度传感器SHTC1起始ID，                  占用2-3 chn：2 */
#define SENSOR_LIS3MDL_START_ID         4    /**< \brief 三轴磁、温度传感器LIS3MDL起始ID，      占用4-7 chn：4 */
#define SENSOR_BME280_START_ID          8    /**< \brief 气压、温、湿度传感器BME280起始ID，     占用8-10 chn：3 */
#define SENSOR_BMA253_START_ID          11   /**< \brief 三轴加速度、温度传感器BMA253起始ID， 占用11-14 chn：4 */
#define SENSOR_BMP280_START_ID          15   /**< \brief 温度、气压传感器BMP280起始ID，            占用15-16 chn：2 */
#define SENSOR_LPS22HB_START_ID         17   /**< \brief 温度、气压传感器LPS22HB起始ID，          占用17-18 chn：2 */
#define SENSOR_LSM6DSL_START_ID         19   /**< \brief 三轴加速度、三轴陀螺仪、温度传感器LSM6DSL起始ID， 占用19-25 chn：7 */
#define SENSOR_BH1730_0_START_ID        26   /**< \brief 光照强度及近距离传感器BH1730，              占用26 chn：1*/
#define SENSOR_BMG160_START_ID          27   /**< \brief 三轴陀螺仪传感器BMG160起始ID，             占用27-29 chn：3*/
#define SENSOR_MMC5883MA_START_ID       30   /**< \brief 三轴磁传感器MMC5883MA起始ID，             占用30 - 33 */   /* INT引脚与wifi的spi引脚冲突 */
#define SENSOR_LTR553_0_START_ID        34   /**< \brief 光照强度及近距离传感器LTR-553ALS-01起始ID，占用34 - 35 */

/** @} sensor_aw_plfm_params_hwcfg */


/**
 * \name 组件自动配置和检测
 * @{
 */
#include "aw_prj_param_auto_cfg.h"
/** @} */


/*******************************************************************************
  资源配置
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_rescfg   资源配置
 * @{
 */
/**
 * \name 控制台组件资源配置
 * @{
 */
#ifdef AW_COM_CONSOLE
#define AW_CFG_CONSOLE_SHOW_BANNER      1                        /** \brief 控制台启动时，是否显示欢迎标识：0-不显示 1-显示 */
#define AW_CFG_CONSOLE_COMID            IMX1050_LPUART1_COMID    /** \brief 控制台串口号 */
#define AW_CFG_CONSOLE_BAUD_RATE        115200                   /** \brief 控制台波特率 */
#endif
/** @} */


/**
 * \name shell组件资源配置
 * @{
 */
#ifdef AW_COM_SHELL_SERIAL
#define AW_CFG_SHELL_THREAD_PRIO        6u                       /**< \brief serial shell 线程优先级 */
#define AW_CFG_SHELL_THREAD_STK_SIZ     (10 * 1024)              /**< \brief serial shell 线程堆栈大小 */
#define AW_CFG_SHELL_SERIAL_COMID       IMX1050_LPUART1_COMID    /**< \brief serial shell 使用的串口设备号 */
#define AW_SHELL_SYS_CMD                                         /**< \brief 使能shell IO操作，如:"ls" "cd" "mkdir"等命令 */
#endif

#ifdef AW_COM_SHELL_TELNET
#define AW_CFG_TELNET_TASK_PRIO         1       /**< \brief telent shell 线程优先级  */
#define AW_CFG_TELNET_TASK_STACK_SIZE   8192    /**< \brief telent shell 线程堆栈大小 */
#define AW_CFG_TELNET_MAX_SESSION       2       /**< \brief telent shell 最大同时连接数目 */
#define AW_CFG_TELNET_LISTEN_PORT       23      /**< \brief telent shell 网络端口号 */
#define AW_CFG_TELNET_USER_NAME         "admin" /**< \brief telent shell 用户登入名 */
#define AW_CFG_TELNET_USER_PSW          "123456" /**< \brief telent shell 用户登入密码 */
#endif
/** @} */

/**
 * \name net tools
 * @{
 */
#define AW_NET_TOOLS                    /**< \brief 使能命令行网络配置工具 */
/** @} */


/** \name 事件系统资源配置
 * @{
 */
#ifdef AW_COM_EVENT
#define AW_CFG_EVENT_TASK_STK_SIZE  4096    /**< \brief 事件框架处理任务堆栈大小 */
#define AW_CFG_EVENT_TASK_PRIO      8       /**< \brief 事件框架处理任务优先级 */
#define AW_CFG_EVENT_MSG_NUM        16      /**< \brief 事件框架最大并发消息个数 */

//#define AW_COM_INPUT_EV                /**< \brief 使能输入事件管理 */
//#define AW_COM_INPUT_EV_KEY            /**< \brief 使能按键事件 */
//#define AW_COM_INPUT_EV_ABS            /**< \brief 使能绝对事件，触摸屏事件依赖此组件 */
#endif
/** @} */


/**
 * \name FTP 组件资源配置
 * FTP 组件需要文件系统支持，因而需要打开文件系统，并且需要调用文件系统的mount接口，
 * 创建一个文件根节点
 * @{
 */
#ifdef AW_COM_FTPD
#define FTPD_MAX_SESSIONS            1         /**< \brief FTPD 最大同时连接数目 */
#define FTPD_LISTEN_TASK_PRIO        6          /**< \brief FTPD 任务优先级 */
#define FTPD_LISTEN_TASK_STK_SIZE    4096       /**< \brief FTPD 栈大小 */
#define FTPD_CMD_BUF_SIZE            0x5000    /**< \brief FTPD 运行buffer 大小 */
#endif
/** @} */


/**
 * \name 块设备驱动框架统资源配置
 * @{
 */
#ifdef AW_COM_BLOCK_DEV
#define AW_BLOCK_DEV_EVT_CNT   5
#endif
/** @} */


/** \name I/O 系统裁剪配置
 * @{
 */
#ifdef AW_COM_IO_SYSTEM
#define AW_CFG_IO_OPEN_FILES_MAX    10
#endif
/** @} */


/** \name I/O 系统存储卷挂载点裁剪配置
 * @{
 */
#ifdef AW_COM_MOUNT
#define AW_CFG_MOUNT_POINTS         5

#endif
/** @} */


/** \name RAW文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_RAWFS
#define AW_CFG_RAWFS_VOLUMES    1   /**< \brief RAW 文件系统卷个数 */
#define AW_CFG_RAWFS_FILES      1   /**< \brief RAW 文件系统同时打开文件数*/
#define AW_CFG_RAWFS_BLOCK_SIZE 512 /**< \brief RAW 文件系统最大块大小 */
#endif
/** @} */


/** \name FAT文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_FATFS
#define AW_CFG_FATFS_VOLUMES    3    /**< \brief FAT 文件系统卷个数 */
#define AW_CFG_FATFS_FILES      10    /**< \brief FAT 文件系统同时打开文件数 */
#define AW_CFG_FATFS_BLOCK_SIZE 4096 /**< \brief FAT 文件系统最大块大小 */
#endif
/** @} */


/** \name LFFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_LFFS
#define AW_CFG_LFFS_VOLUMES     2   /**< \brief LFFS 文件系统卷个数 */
#define AW_CFG_LFFS_FILES       10   /**< \brief LFFS 文件系统同时打开文件数*/
#endif
/** @} */


/** \name LITTLEFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_LITTLEFS
#define AW_CFG_LITTLEFS_VOLUMES     2   /**< \brief LITTLEFS 文件系统卷个数 */
#define AW_CFG_LITTLEFS_FILES       10   /**< \brief LITTLEFS 文件系统同时打开文件数*/
#endif
/** @} */


/** \name TFFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_TXFS
#define AW_CFG_TXFS_VOLUMES     2   /**< \brief TXFS 文件系统卷个数 */
#define AW_CFG_TXFS_FILES       10   /**< \brief TXFS 文件系统同时打开文件数*/
#define AW_CFG_TXFS_BUF_CNT     50  /** \brief TXFS 文件系统缓存个数，值越大读写越快，但掉电后丢失数据越多 */
#endif
/** @} */


/** \name YAFFS文件系统资源配置
 * @{
 */
#ifdef AW_COM_FS_YAFFS
#define AW_CFG_YAFFS_VOLUMES     2   /**< \brief YAFFS 文件系统卷个数 */
#define AW_CFG_YAFFS_FILES       10   /**< \brief YAFFS 文件系统同时打开文件数*/
#endif
/** @} */


/** \name USB协议栈裁剪配置
 * @{
 */
#ifdef AW_COM_USBH
#define AW_DRV_USBH_MASS_STORAGE               /**< \brief 大容量存储驱动(U盘&硬盘) */
#define AW_DRV_USBH_UVC                        /**< \brief USB通用摄像头类*/
//#define AW_DRV_USBH_HID                        /**< \brief USB人体接口设备类*/
//#define AW_DRV_USBH_CDC_ECM                    /**< \brief USB通讯设备类：网络控制模型驱动 */
#define AW_DRV_USBH_CDC_SERIAL                 /**< \brief USB通信设备类：USB转串口驱动*/
#endif

#if defined(AW_COM_USBH) || defined(AW_COM_USBD)
#ifdef AW_DRV_USBH_UVC
#define AW_CFG_USB_MEM_SIZE          (640 * 1024)  /**< \brief USB协议栈内存使用空间大小 */
#else
#define AW_CFG_USB_MEM_SIZE          (128 * 1024)  /**< \brief USB协议栈内存使用空间大小 */
#endif
#endif

#ifdef AW_DRV_USBH_MASS_STORAGE

#ifndef AW_COM_BLOCK_DEV
#define AW_COM_BLOCK_DEV
#endif

#define AW_CFG_USB_MS_BUF_SIZE       (16 * 1024)  /**< \brief U盘数据交互缓存，占用AW_CFG_USB_MEM_SIZE空间，影响U盘读写性能，推荐16k */

#endif
/** @} */


/** \name CAN波特率表
 * @{
 */

/** \brief 波特率表，使用CAN波特率计算工具计算得出,仅供参考
 *         (目前输入时钟为PLL3 6分频 再2分频 40MHz)
 *                        tseg1 tseg2 sjw smp brp
 */
#define AW_CFG_CAN_BTR_1000K {15,  4,   4,   0,   2  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_800K  {6,   3,   3,   0,   5  } /**< \brief 采样点 70% */
#define AW_CFG_CAN_BTR_500K  {15,  4,   4,   0,   4  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_250K  {15,  4,   4,   0,   8  } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_125K  {15,  4,   4,   0,   16 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_100K  {15,  4,   4,   0,   20 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_50K   {15,  4,   4,   0,   40 } /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_20K   {15,  4,   4,   0,   100} /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_10K   {15,  4,   4,   0,   200} /**< \brief 采样点 80% */
#define AW_CFG_CAN_BTR_6_25K {16,  8,   4,   0,   256} /**< \brief 采样点 68% */

/** @} */

/** \name system资源配置
 * @{
 */
#define AW_CFG_TICKS_PER_SECOND             1000                    /** \brief 系统时钟节拍频率配置 */
#define AW_CFG_MAIN_TASK_STACK_SIZE         (1024 * 10)              /** \brief main任务堆栈大小，推荐 >= 512 */
#define AW_CFG_MAIN_TASK_PRIO               1                       /** \brief main任务优先级 */

#define AW_CFG_ISR_DEFER_TASK_PRIO          AW_TASK_SYS_PRIORITY(0) /** \brief ISR defer(系统任务高优先级延迟作业) 配置 */
#define AW_CFG_ISR_DEFER_TASK_CNT           3                       /** \brief 运作任务数量 */
#define AW_CFG_ISR_DEFER_TASK_STACK_SIZE    (1024 * 10)             /** \brief 每个任务栈大小 */

#define AW_CFG_NOR_DEFER_TASK_PRIO          7                       /** \brief Normal defer(系统任务普通优先级延迟作业) 配置 */
#define AW_CFG_NOR_DEFER_TASK_CNT           3                       /** \brief 运作任务数量 */
#define AW_CFG_NOR_DEFER_TASK_STACK_SIZE    (1024 * 10)             /** \brief 每个任务栈大小 */

#define AW_CFG_AWBUS_LITE_DRIVER_MAX        80                      /** \brief AWBus lite 支持的最大驱动数  */
#define AW_CFG_AWBUS_LITE_BUSTYPE_MAX       16                      /** \brief AWBus lite 支持的最大总线类型数  */

#define AW_CFG_ARMCM_NVIC_ISRINFO_COUNT     42                      /** \brief 中断向量数  */
/** @} */

/** @} grp_aw_plfm_params_rescfg */





/*******************************************************************************
  资源ID
*******************************************************************************/
/**
 * \addtogroup grp_aw_plfm_params_idcfg   资源ID配置
 * @{
 */
/* ENET ID 分配 */
#define IMX1050_MII_BUSID               0

/* LPSPI ID分配 */
#define IMX1050_LPSPI1_BUSID            0
#define IMX1050_LPSPI2_BUSID            1
#define IMX1050_LPSPI3_BUSID            2
#define IMX1050_LPSPI4_BUSID            3

/* Timer ID分配 */
#define IMX1050_PIT_BUSID               0

/* Timer ID分配 */
#define IMX1050_GPT1_BUSID              1
#define IMX1050_GPT2_BUSID              2

/* Qtimer ID分配 */
#define IMX1050_QTIMER1_BUSID           3
#define IMX1050_QTIMER2_BUSID           4
#define IMX1050_QTIMER3_BUSID           5
#define IMX1050_QTIMER4_BUSID           6

/* I2C  ID分配 */
#define IMX1050_LPI2C1_BUSID            1
#define IMX1050_LPI2C2_BUSID            2
#define IMX1050_LPI2C3_BUSID            3
#define IMX1050_LPI2C4_BUSID            4
#define IMX1050_GPIO_I2C0_BUSID         5

/* 串口 ID 分配 */
#define IMX1050_LPUART1_COMID           COM0
#define IMX1050_LPUART2_COMID           COM1
#define IMX1050_LPUART3_COMID           COM2
#define IMX1050_LPUART4_COMID           COM3
#define IMX1050_LPUART5_COMID           COM4
#define IMX1050_LPUART6_COMID           COM5
#define IMX1050_LPUART7_COMID           COM6
#define IMX1050_LPUART8_COMID           COM7
#define IMX1050_FLEXIO2_UART0_COMID     COM8

/* CAN ID分配 */
#define IMX1050_FLAX_CAN1_BUSID         0
#define IMX1050_FLAX_CAN2_BUSID         1
#define AWBL_UART_CAN_BUSID             2

/* USB总线 ID 分配 */
#define IMX1050_USBH1_BUSID             0
#define IMX1050_USBH2_BUSID             1

/* ENC ID 分配 */
#define IMX1050_ENC1                    1
#define IMX1050_ENC2                    2
#define IMX1050_ENC3                    3
#define IMX1050_ENC4                    4

/* ACMP ID 分配 */
#define IMX1050_ACMP1                   1
#define IMX1050_ACMP2                   2
#define IMX1050_ACMP3                   3
#define IMX1050_ACMP4                   4


/* NAND总线 ID 分配 */
#define IMX1050_NAND0_BUSID             0

/* PWM ID分配 */
/* 用于eFlex PWM4 */
#define PWM0                            0
#define PWM1                            1
#define PWM2                            2
#define PWM3                            3
#define PWM4                            4
#define PWM5                            5
#define PWM6                            6
#define PWM7                            7

/* 用于QTimer3 PWM*/
#define PWM8                            8
#define PWM9                            9
#define PWM10                           10
#define PWM11                           11

/* PWM ID分配 */
/* 用于eFlex PWM1 */
#define PWM12                           12
#define PWM13                           13


/* 用于GPT1 PWM */
#define PWM14                           14

/* 用于GPT2 PWM */
#define PWM15                           15

/* 用于QTimer1 PWM*/
#define PWM16                           16
#define PWM17                           17
#define PWM18                           18
#define PWM19                           19

/* 用于QTimer2 PWM*/
#define PWM20                           20
#define PWM21                           21
#define PWM22                           22
#define PWM23                           23

/* 用于QTimer4 PWM*/
#define PWM24                           24
#define PWM25                           25
#define PWM26                           26
#define PWM27                           27

/* RTC ID 分配 */
#define RTC_ID0                         0
#define RTC_ID1                         1

/* 用于QTimer3 CAP */
#define QTIMER3_CAP0                    0
#define QTIMER3_CAP1                    1
#define QTIMER3_CAP2                    2
#define QTIMER3_CAP3                    3

/* 用于GPT1 CAP */
#define GPT1_CAP0                       4
#define GPT1_CAP1                       5

/* 用于GPT2 CAP */
#define GPT2_CAP0                       6
#define GPT2_CAP1                       7

/* 用于QTimer1 CAP */
#define QTIMER1_CAP0                    8
#define QTIMER1_CAP1                    9
#define QTIMER1_CAP2                    10
#define QTIMER1_CAP3                    11

/* 用于QTimer2 CAP */
#define QTIMER2_CAP0                    12
#define QTIMER2_CAP1                    13
#define QTIMER2_CAP2                    14
#define QTIMER2_CAP3                    15

/* 用于QTimer4 CAP */
#define QTIMER4_CAP0                    16
#define QTIMER4_CAP1                    17
#define QTIMER4_CAP2                    18
#define QTIMER4_CAP3                    19

/** \name canopen 资源配置
 * @{
 */
#if defined(AW_COM_CANOPEN) || defined(AW_COM_CANFESTIVAL)
#define AW_COP_CFG_TIMER_NAME  "imx10xx_gpt"
#define AW_COP_CFG_TIMER_UNIT  1
#define AW_COP_CFG_TIMER_ID    0
#endif

#ifdef AW_DEV_BU21029MUV
#define SYS_TS_ID  "bu21029muv"
#elif defined(AW_DEV_FT5X06)
#define SYS_TS_ID  "ft5x06"
#elif defined(AW_DEV_IMX1050_TS)
#define SYS_TS_ID  "imx105x-ts"
#else
#define SYS_TS_ID  "none"
#endif

#ifdef AW_DEV_TFT_4_3
#define SYS_LCD_X_RES 480
#define SYS_LCD_Y_RES 272
#elif defined(AW_DEV_AP_TFT7_0)||defined(AW_DEV_LCD_TM070RDH12_24B)
#define SYS_LCD_X_RES 800
#define SYS_LCD_Y_RES 480
#elif defined(AW_DEV_LCD_640480W056TR)
#define SYS_LCD_X_RES 640
#define SYS_LCD_Y_RES 480
#elif defined(AW_DEV_LCD_800600W080TR)
#define SYS_LCD_X_RES 800
#define SYS_LCD_Y_RES 600
#endif

/** @} grp_aw_plfm_params_idcfg */

/** @} grp_aw_prj_param */

#include "aw_prj_param_auto_cfg.h"
#endif  /* __AW_PRJ_PARAMS_H */

/* end of file */
