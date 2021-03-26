
#ifndef __AW_DEMO_CONFIG_H
#define __AW_DEMO_CONFIG_H

#include "aw_prj_params.h"
#include "imx1050_pin.h"
#include "aw_cpu_clk.h"


#define DE_MODBUS_UART_ID               COM2
#define DE_PWMID                        PWM10       /* PWM���� ID���� */
#define DE_PWMID_GPIO                   GPIO1_18    /* PWM10 ͨ����Ӧ������*/


#define DE_GPIO_PWM                     GPIO1_8      /* GPIOģ�����PWM����  */
#define DE_HWTIMER_INPUT_CAP_IO         GPIO4_24     /* GPT1���벶���� IO ���� */

#define DE_CLK_GPT                      IMX1050_CLK_CG_GPT_SERIAL    /* GPT��ʱ��ʱ��ID */


#define DM_GPIO_TRG_OUTPUT              GPIO3_0   /* GPIO�ж����� ���Ŷ��� */
#define DM_GPIO_TRG_INTR                GPIO3_1


#define DM_GPIO_LED                     GPIO1_19  /* LED ���Ŷ���*/

#define DE_HWTIMER_NAME                 "imx10xx_gpt"       /* gpt��ʱ������ */
#define DE_GPTID                        IMX1050_GPT1_BUSID      /* gpt1 ID�� */

#define DE_HWTIMER_TIMER_NAME           "imx10xx_qtmr"          /* qtimer��ʱ������ */
#define DE_HWTIMER_TIMER_ID             IMX1050_QTIMER3_BUSID   /* qtimer3 ID�� */

#define DE_QTIMER_NAME                  "imx10xx_qtmr"          /* qtimer��ʱ������ */
#define DE_QTIMER_ID                    IMX1050_QTIMER3_BUSID   /* qtimer3 ID�� */

#define DE_KEY_IO                       GPIO3_0    /* �������� */

#define DE_ACMPID                       IMX1050_ACMP1      /* ģ��Ƚ���ID */
#define DE_ACMPID_GPIO                  GPIO1_22  /* ģ��Ƚ���1ͨ��1��Ӧ���� */


#define DE_TIMER_CAPID                  QTIMER3_CAP3  /* ��ʱ�����벶��ID */
#define DE_TIMER_CAPID_GPIO             GPIO1_19 /*QTIMER3_CAP3ͨ����Ӧ������*/

#define DE_FB                          "imx1050_fb"
#define DE_NANDFLASH_DEV_NAME           AWBL_NAND_DRV_NAME
#define DE_NAND_FLASH_DEV_ID            1

#define DE_OTP_MEM                      "otp_osp"/* otp�洢�����ƺ͵�Ԫ�ţ���awbl_hwconf_imx1050_ocotp.h �в鿴�޸�  */
#define DE_NAND_NAME                    "/dev/mtd1" /* nandflash�洢������*/

#define DE_DISK_NAME                    "/dev/rootfs"

#define DE_SPI_FLASH                    "/flexspi_flash0"/*spi flash����*/

#define DM_LED                          0/*LED���*/


#define DE_CAN_CH                       0/* CAN����ͨ��*/

#define DE_ENET_NAME                    "eth0"/*��̫������*/

#define DE_ADC_CH                       0/*ADCͨ����*/

#define DE_CAMERA_NAME                  "ov7725"/* ����ͷ�����豸����*/

#define DE_I2C_ID                       1/* i2c����i2c ID*/

//#define DE_TFT_TS_NAME                  "bu21029muv"/* ���败�����豸����*/
//#define DE_CAP_TS_NAME                  "ft5x06"    /* ���ݴ������豸����*/
#define DE_TS_NAME                      SYS_TS_ID   /* �������豸����*/

#define DE_NAMD_FS_NAME                 "lffs"/* nand fs����*/

/* rtc ID*/
#define DE_RTC_ID0                      RTC_ID0     /* �ⲿRTC PCF85063*/
#define DE_RTC_ID1                      RTC_ID1     /* imxrt1052�ڲ�RTC*/

#define DE_SD_DEV_NAME                  "/dev/sd0"/* SD���豸����*/

#define DE_SPI_DEV_BUSID                IMX1050_LPSPI3_BUSID/* SPI����ID*/

#define DE_SPI_CS_PIN                   GPIO1_28/* SPIƬѡ����*/

#define DE_FLEXSPI_NVRAM_NAME          "user_data"

#define DE_USB_BLK_NAME                 "/dev/h0-d1-0"/* USB���豸����*/

#define DE_USB_DEV_NAME                 "/dev/usbd"

#define DE_WIFI_DEV_NAME                "wifi43362"/*wifi����*/

#define DE_BUZZER_PWMID                 PWM10
#endif
