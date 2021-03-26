
#ifndef __AW_DEMO_CONFIG_H
#define __AW_DEMO_CONFIG_H

#include "aw_prj_params.h"
#include "imx1050_pin.h"
#include "aw_cpu_clk.h"


#define DE_MODBUS_UART_ID               COM2
#define DE_PWMID                        PWM10       /* PWM例程 ID配置 */
#define DE_PWMID_GPIO                   GPIO1_18    /* PWM10 通道对应的引脚*/


#define DE_GPIO_PWM                     GPIO1_8      /* GPIO模拟产生PWM脉冲  */
#define DE_HWTIMER_INPUT_CAP_IO         GPIO4_24     /* GPT1输入捕获功能 IO 引脚 */

#define DE_CLK_GPT                      IMX1050_CLK_CG_GPT_SERIAL    /* GPT定时器时钟ID */


#define DM_GPIO_TRG_OUTPUT              GPIO3_0   /* GPIO中断例程 引脚定义 */
#define DM_GPIO_TRG_INTR                GPIO3_1


#define DM_GPIO_LED                     GPIO1_19  /* LED 引脚定义*/

#define DE_HWTIMER_NAME                 "imx10xx_gpt"       /* gpt定时器名称 */
#define DE_GPTID                        IMX1050_GPT1_BUSID      /* gpt1 ID号 */

#define DE_HWTIMER_TIMER_NAME           "imx10xx_qtmr"          /* qtimer定时器名称 */
#define DE_HWTIMER_TIMER_ID             IMX1050_QTIMER3_BUSID   /* qtimer3 ID号 */

#define DE_QTIMER_NAME                  "imx10xx_qtmr"          /* qtimer定时器名称 */
#define DE_QTIMER_ID                    IMX1050_QTIMER3_BUSID   /* qtimer3 ID号 */

#define DE_KEY_IO                       GPIO3_0    /* 按键引脚 */

#define DE_ACMPID                       IMX1050_ACMP1      /* 模拟比较器ID */
#define DE_ACMPID_GPIO                  GPIO1_22  /* 模拟比较器1通道1对应引脚 */


#define DE_TIMER_CAPID                  QTIMER3_CAP3  /* 定时器输入捕获ID */
#define DE_TIMER_CAPID_GPIO             GPIO1_19 /*QTIMER3_CAP3通道对应的引脚*/

#define DE_FB                          "imx1050_fb"
#define DE_NANDFLASH_DEV_NAME           AWBL_NAND_DRV_NAME
#define DE_NAND_FLASH_DEV_ID            1

#define DE_OTP_MEM                      "otp_osp"/* otp存储段名称和单元号，在awbl_hwconf_imx1050_ocotp.h 中查看修改  */
#define DE_NAND_NAME                    "/dev/mtd1" /* nandflash存储段名称*/

#define DE_DISK_NAME                    "/dev/rootfs"

#define DE_SPI_FLASH                    "/flexspi_flash0"/*spi flash名称*/

#define DM_LED                          0/*LED编号*/


#define DE_CAN_CH                       0/* CAN例程通道*/

#define DE_ENET_NAME                    "eth0"/*以太网名称*/

#define DE_ADC_CH                       0/*ADC通道号*/

#define DE_CAMERA_NAME                  "ov7725"/* 摄像头例程设备名字*/

#define DE_I2C_ID                       1/* i2c例程i2c ID*/

//#define DE_TFT_TS_NAME                  "bu21029muv"/* 电阻触摸屏设备名字*/
//#define DE_CAP_TS_NAME                  "ft5x06"    /* 电容触摸屏设备名字*/
#define DE_TS_NAME                      SYS_TS_ID   /* 触摸屏设备名字*/

#define DE_NAMD_FS_NAME                 "lffs"/* nand fs名字*/

/* rtc ID*/
#define DE_RTC_ID0                      RTC_ID0     /* 外部RTC PCF85063*/
#define DE_RTC_ID1                      RTC_ID1     /* imxrt1052内部RTC*/

#define DE_SD_DEV_NAME                  "/dev/sd0"/* SD卡设备名字*/

#define DE_SPI_DEV_BUSID                IMX1050_LPSPI3_BUSID/* SPI总线ID*/

#define DE_SPI_CS_PIN                   GPIO1_28/* SPI片选引脚*/

#define DE_FLEXSPI_NVRAM_NAME          "user_data"

#define DE_USB_BLK_NAME                 "/dev/h0-d1-0"/* USB块设备名字*/

#define DE_USB_DEV_NAME                 "/dev/usbd"

#define DE_WIFI_DEV_NAME                "wifi43362"/*wifi名称*/

#define DE_BUZZER_PWMID                 PWM10
#endif
