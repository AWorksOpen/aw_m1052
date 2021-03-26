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
 * \brief AnyWhere ���̲����Զ������ļ�
 */

#ifndef __AW_PRJ_PARAMS_AUTO_CFG_H
#define __AW_PRJ_PARAMS_AUTO_CFG_H




/**
 * \name Ŀǰϵͳ�¼����Ĭ�ϱ���Ҫ��
 * @{
 */
#ifndef AW_COM_EVENT
#define AW_COM_EVENT            /**< \brief ʹ���¼�ϵͳ */
#endif




/*
 * WiFi dependency:
 *     MCU <=> WiFi
 *     MCU <=> SPI FLASH(NVRAM)
 */
#if defined(AW_DEV_CYW43362_WIFI_SPI_0) || defined(AW_DEV_CYW43362_WIFI_SDIO_0)
    #ifndef AW_COM_NETWORK
        #define AW_COM_NETWORK
    #endif

    #ifdef AW_DEV_CYW43362_WIFI_SPI_0
        #define AW_DRV_AWBL_CYW43362_WIFI_SPI
        #define AW_DRV_CYW43362_WIFI_SPI
        /* MCU <=> WiFi */
        #ifndef AW_DEV_IMX1050_LPSPI3
            #define AW_DEV_IMX1050_LPSPI3
        #endif
    #else
        #define AW_DRV_AWBL_CYW43362_WIFI_SDIO
        /* MCU <=> WiFi */
        #ifndef AW_DEV_IMX1050_USDHC2
            #define AW_DEV_IMX1050_USDHC2
        #endif
         #ifdef AW_DEV_IMX1050_LPSPI3
             #error "AW_DEV_xxx_SPI1 AW_DEV_IMX1050_USDHC2 share the same hw!"
         #endif
    #endif
#endif


/**
 * \name SHELL �������������豸
 * @{
 */
#ifdef AW_COM_SHELL_SERIAL
#ifndef AW_COM_SERIAL
#define AW_COM_SERIAL
#endif

#ifndef AW_DEV_IMX1050_LPUART1
#define AW_DEV_IMX1050_LPUART1
#endif
#endif




/**
 * \name �ⲿFM175XX mifare����ģ��
 * @{
 */
#ifdef AW_DEV_FM175XX_SPI3_0
#define AW_DRV_FM175XX

#ifndef AW_DEV_IMX1050_LPSPI3
#define AW_DEV_IMX1050_LPSPI3
#endif
#endif
/** @} */



/**
 * \name ��Դ������(PWM)
 * @{
 */
#ifdef AW_DEV_PWM_BUZZER
#define AW_DRV_PWM_BUZZER

#ifndef AW_DEV_IMX1050_QTIMER3_PWM
#define AW_DEV_IMX1050_QTIMER3_PWM
#endif

#endif
/** @} */


/**
 * \name GPRS�豸
 * @{
 */
#if defined(AW_DEV_GPRS_SIM800) || \
    defined(AW_DEV_GPRS_ME909S) || \
    defined(AW_DEV_GPRS_U9300C) || \
	defined(AW_DEV_GPRS_EC20)
#ifndef AW_COM_NETWORK
#define AW_COM_NETWORK
#endif
#define AW_COM_GPRS
#endif
/** @} */



/**
 * \name OV7725�豸
 * @{
 */
#ifdef AW_DEV_OV7725
#define AW_DRV_OV7725

#ifndef AW_DEV_CAMERA
#define AW_DEV_CAMERA
#endif

#ifndef AW_DEV_IMX1050_CSI
#define AW_DEV_IMX1050_CSI
#endif

#ifndef AW_DEV_IMX1050_LPI2C1
#define AW_DEV_IMX1050_LPI2C1
#endif

#endif
/** @} */



/**
 * \name �ⲿNandFlash
 * @{
 */
#if (defined(AW_DEV_NANDFLASH_MX30LF1G08) ||\
        defined(AW_DEV_NANDFLASH_S34ML01G2) ||\
        defined(AW_DEV_NANDFLASH_S34ML02G2))
#define AW_DEV_IMX1050_NANDFLASH

#ifndef AW_COM_NANDBUS
#define AW_COM_NANDBUS
#endif

#ifndef AW_COM_FS_LFFS
#define AW_COM_FS_LFFS          /**< \brief ʹ�� LFFS �ļ�ϵͳ */
#endif


#define AW_DRV_NANDFLASH

#ifndef AW_DEV_IMX1050_SEMC
#define AW_DEV_IMX1050_SEMC
#endif
#endif
/** @} */




/**
 * \name �ⲿRTCоƬPCF85063
 * @{
 */
#ifdef AW_DEV_EXTEND_PCF85063
#ifndef AW_COM_RTC
#define AW_COM_RTC
#endif
#define AW_DRV_EXTEND_PCF85063
#define AW_DEV_IMX1050_LPI2C1
#endif
/** @} */


/**
 * \name �ڲ� SNVS RTC
 * @{
 */
#ifdef AW_DEV_IMX1050_RTC
#ifndef AW_COM_RTC
#define AW_COM_RTC
#endif
#define AW_DRV_IMX1050_RTC
#endif
/** @} */


/**
 * \name �ⲿ���败����(I2C�ӿ�)
 * @{
 */
#ifdef  AW_DEV_BU21029MUV
#define AW_DRV_BU21029MUV_TS
#define AW_DEV_IMX1050_LPI2C1
#endif
/** @} */




/**
 * \name �ⲿ���ݴ�����(I2C�ӿ�)
 * @{
 */
#ifdef  AW_DEV_FT5X06
#define AW_DRV_FT5X06_TS
#define AW_DEV_IMX1050_LPI2C1
#endif
/** @} */





/**
 * \name LPUART
 * @{
 */
#if defined AW_DEV_IMX1050_LPUART1  || \
    defined AW_DEV_IMX1050_LPUART2  || \
    defined AW_DEV_IMX1050_LPUART3  || \
    defined AW_DEV_IMX1050_LPUART4  || \
    defined AW_DEV_IMX1050_LPUART5  || \
    defined AW_DEV_IMX1050_LPUART6  || \
    defined AW_DEV_IMX1050_LPUART7  || \
    defined AW_DEV_IMX1050_LPUART8

#ifndef AW_COM_SERIAL
#define AW_COM_SERIAL
#endif

#define AW_DRV_IMX1050_LPUART

#endif
/** @} */




/**
 * \name I2C
 * @{
 */
#if defined AW_DEV_IMX1050_LPI2C1  || \
    defined AW_DEV_IMX1050_LPI2C2  || \
    defined AW_DEV_IMX1050_LPI2C3  || \
    defined AW_DEV_IMX1050_LPI2C4

#ifndef AW_COM_I2C
#define AW_COM_I2C
#endif

#define AW_DRV_IMX1050_LPI2C

#endif
/** @} */




/**
 * \name SPI
 * @{
 */
#if defined AW_DEV_IMX1050_LPSPI1  || \
    defined AW_DEV_IMX1050_LPSPI2  || \
    defined AW_DEV_IMX1050_LPSPI3  || \
    defined AW_DEV_IMX1050_LPSPI4

#ifndef AW_COM_SPI
#define AW_COM_SPI
#endif

#define AW_DRV_IMX1050_LPSPI

#endif
/** @} */





/**
 * \name ��̫��
 * @{
 */
#ifdef AW_DEV_IMX1050_ENET

#ifndef AW_COM_NETWORK
#define AW_COM_NETWORK
#endif

#define AW_DRV_IMX1050_ENET

#define AW_DEV_KSZ8081_PHY0
#define AW_DEV_DP83848_PHY
#endif
/** @} */




/**
 * \name �ڲ��������Ź�
 * @{
 */
#if defined AW_DEV_IMX1050_WDT1

#ifndef AW_COM_WDT
#define AW_COM_WDT
#endif

#define AW_DRV_IMX1050_WDT
#endif
/** @} */




/**
 * \name �ڲ����ڿ��Ź�
 * @{
 */
#if defined AW_DEV_IMX1050_RTWDT

#ifndef AW_COM_WDT
#define AW_COM_WDT
#endif

#define AW_DRV_IMX1050_RTWDT
#endif
/** @} */




/**
 * \name DMA
 * @{
 */
#ifdef AW_DEV_IMX1050_EDMA
#define AW_DRV_IMX1050_EDMA
#endif
/** @} */




/**
 * \name GPT(��ʱ��)
 * @{
 */
#if defined AW_DEV_IMX1050_GPT1_TIMER || \
    defined AW_DEV_IMX1050_GPT2_TIMER
#define AW_DRV_IMX1050_GPT_TIMER
#endif
/** @} */




/**
 * \name GPT CAP
 * @{
 */
#if defined AW_DEV_IMX1050_GPT1_CAP || \
    defined AW_DEV_IMX1050_GPT2_CAP

#ifndef AW_COM_CAP
#define AW_COM_CAP
#endif

#define AW_DRV_IMX1050_GPT_CAP
#endif
/** @} */




/**
 * \name GPT PWM
 * @{
 */
#if defined AW_DEV_IMX1050_GPT1_PWM || \
    defined AW_DEV_IMX1050_GPT2_PWM

#ifndef AW_COM_PWM
#define AW_COM_PWM
#endif

#define AW_DRV_IMX1050_GPT_PWM
#endif
/** @} */




/**
 * \name PIT(��ʱ��)
 * @{
 */
#ifdef AW_DEV_IMX1050_PIT
#define AW_DRV_IMX1050_PIT
#endif
/** @} */




/**
 * \name �ڲ�ADC
 * @{
 */
#if defined  AW_DEV_IMX1050_ADC1 || \
    defined  AW_DEV_IMX1050_ADC2

#ifndef AW_COM_ADC
#define AW_COM_ADC
#endif

#define AW_DRV_IMX1050_ADC
#endif
/** @} */



/**
 * \name SDIO����
 * @{
 */
#if defined(AW_DEV_IMX1050_USDHC1) || defined(AW_DEV_IMX1050_USDHC2)
#ifndef AW_COM_SDIOBUS
#define AW_COM_SDIOBUS
#endif

#ifndef AW_COM_SDCARD
#define AW_COM_SDCARD
#endif

#ifndef AW_COM_FS_FATFS
#define AW_COM_FS_FATFS         /**< \brief ʹ�� FAT �ļ�ϵͳ */
#endif

#define AW_DRV_IMX1050_USDHC
#endif
/** @} */




/**
 * \name MQS����
 * @{
 */
#ifdef AW_DEV_IMX1050_MQS
#define AW_DRV_IMX1050_MQS
#ifndef AW_COM_AWSA
#define AW_COM_AWSA
#endif
#ifndef AW_DEV_IMX1050_SAI3
#define AW_DEV_IMX1050_SAI3
#endif
#endif
/** @} */


/**
 * \name SAI��Ƶ
 * @{
 */
#if defined AW_DEV_IMX1050_SAI1 || \
    defined AW_DEV_IMX1050_SAI2 || \
    defined AW_DEV_IMX1050_SAI3
#define AW_DRV_IMX1050_SAI
#ifndef AW_COM_AWSA
#define AW_COM_AWSA
#endif
#endif
/** @} */


/**
 * \name �ڲ�OTP�洢��
 * @{
 */
#ifdef AW_DEV_IMX1050_OCOTP
#define AW_DRV_IMX1050_OCOTP
#endif
/** @} */




/**
 * \name FlexSPI
 * @{
 */
#ifdef AW_DEV_IMX1050_FLEXSPI
#define AW_DRV_IMX1050_FLEXSPI

#if defined (AW_CFG_FLEXSPI_FLASH_MTD)

#ifndef AW_COM_FS_LFFS
#define AW_COM_FS_LFFS
#endif

#endif

#endif
/** @} */




/**
 * \name �ڲ�CAN������
 * @{
 */
#if defined AW_DEV_IMX1050_CAN1 || \
    defined AW_DEV_IMX1050_CAN2

#define AW_DRV_IMX1050_CAN

#endif
/** @} */




/**
 * \name �ڲ��������ӿ�
 * @{
 */
#ifdef AW_DEV_IMX1050_TS
#define AW_DRV_IMX1050_TS
#endif
/** @} */




/**
 * \name �ڲ�eFlexPWM
 * @{
 */
#if defined AW_DEV_IMX1050_EFLEX_PWM4 || \
    defined AW_DEV_IMX1050_EFLEX_PWM1
#ifndef AW_COM_PWM
#define AW_COM_PWM
#endif

#define AW_DRV_IMX1050_EFLEX_PWM
#endif
/** @} */




/**
 * \name �ڲ�QTIMER PWM
 * @{
 */
#if defined AW_DEV_IMX1050_QTIMER1_PWM || \
    defined AW_DEV_IMX1050_QTIMER2_PWM || \
    defined AW_DEV_IMX1050_QTIMER3_PWM || \
    defined AW_DEV_IMX1050_QTIMER4_PWM
#ifndef AW_COM_PWM
#define AW_COM_PWM
#endif

#define AW_DRV_IMX1050_QTIMER_PWM
#endif
/** @} */

/**
 * \name �ڲ�QTIMER TIMER
 * @{
 */
#if defined AW_DEV_IMX1050_QTIMER1_TIMER0 || \
    defined AW_DEV_IMX1050_QTIMER2_TIMER0 || \
    defined AW_DEV_IMX1050_QTIMER3_TIMER0 || \
    defined AW_DEV_IMX1050_QTIMER4_TIMER0
#define AW_DRV_IMX1050_QTIMER_TIMER
#endif
/** @} */

/**
 * \name �ڲ�QTIMER CAP
 * @{
 */
#if defined AW_DEV_IMX1050_QTIMER1_CAP || \
    defined AW_DEV_IMX1050_QTIMER2_CAP || \
    defined AW_DEV_IMX1050_QTIMER3_CAP || \
    defined AW_DEV_IMX1050_QTIMER4_CAP
#ifndef AW_COM_CAP
#define AW_COM_CAP
#endif

#define AW_DRV_IMX1050_QTIMER_CAP
#endif
/** @} */



/**
 * \name �ڲ�ENC(����������)�豸
 * @{
 */
#ifdef AW_DEV_IMX1050_ENC1
#define AW_DRV_IMX1050_ENC1
#endif
/** @} */




/**
 * \name �ڲ��¶ȴ�����
 * @{
 */
#ifdef AW_DEV_IMX1050_TEMPMON
#ifndef AW_COM_TEMPMON
#define AW_COM_TEMPMON
#endif
#define AW_DRV_IMX1050_TEMPMON
#endif
/** @} */

/**
 * \name ������HTS221�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_HTS221
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_HTS221
#endif
/** @} */

/**
 * \name ������SHTC1�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_SHTC1
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_SHTC1
#endif
/** @} */

/**
 * \name ������LIS3MDL�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_LIS3MDL
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_LIS3MDL
#endif
/** @} */

/**
 * \name ������BME280�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_BME280
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_BME280
#endif
/** @} */

/**
 * \name ������MMC5883MA�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_MMC5883MA
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_MMC5883MA
#endif
/** @} */

/**
 * \name ������BMA253�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_BMA253
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_BMA253
#endif
/** @} */

/**
 * \name ������BMP280�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_BMP280
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_BMP280
#endif
/** @} */

/**
 * \name ������LPS22HB�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_LPS22HB
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_LPS22HB
#endif
/** @} */

/**
 * \name ������LSM6DSL�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_LSM6DSL
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_LSM6DSL
#endif
/** @} */

/**
 * \name ������BH1730�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_BH1730
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_BH1730
#endif
/** @} */

/**
 * \name ������LTR553�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_LTR553
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_LTR553
#endif
/** @} */

/**
 * \name ������LTR553�豸
 * @{
 */
#ifdef AW_DEV_SENSOR_BMG160
#ifndef AW_COM_SENSOR
#define AW_COM_SENSOR
#endif
#define AW_DRV_SENSOR_BMG160
#endif
/** @} */


/**
 * \name �ڲ�ACMPģ��Ƚ���
 * @{
 */
#ifdef AW_DEV_IMX1050_ACMP1
#define AW_DRV_IMX1050_ACMP1
#endif
/** @} */




/**
 * \name �ڲ�USB host������
 * @{
 */
#if defined AW_DEV_IMX1050_USBH1 || \
    defined AW_DEV_IMX1050_USBH2
#ifndef AW_COM_USBH
#define AW_COM_USBH
#endif

#ifndef AW_COM_FS_FATFS
#define AW_COM_FS_FATFS         /**< \brief ʹ�� FAT �ļ�ϵͳ */
#endif

#define AW_DRV_IMX1050_USBH
#endif
/** @} */




/**
 * \name �ڲ�USB Device������
 * @{
 */
#ifdef AW_DEV_IMX1050_USBD
#ifndef AW_COM_USBD
#define AW_COM_USBD
#endif
#define AW_DRV_IMX1050_USBD
#endif
/** @} */




/**
 * \name �ⲿ�洢���߿�����
 * @{
 */
#ifdef AW_DEV_IMX1050_SEMC
#define AW_DRV_IMX1050_SEMC
#endif
/** @} */



/**
 * \name LED�豸
 * @{
 */
#ifdef AW_DEV_GPIO_LED
#ifndef AW_COM_LED
#define AW_COM_LED
#endif
#define AW_DRV_GPIO_LED
#endif
/** @} */




/**
 * \name �ⲿGPIO ���Ź�
 * @{
 */
#ifdef AW_DEV_GPIO_WDT
#ifndef AW_COM_WDT
#define AW_COM_WDT
#endif
#define AW_DRV_GPIO_WDT
#endif
/** @} */




/**
 * \name ��������
 * @{
 */
#ifdef AW_DEV_GPIO_KEY
#define AW_DRV_GPIO_KEY
#define AW_COM_EVENT
#define AW_COM_INPUT_EV                /**< \brief ʹ�������¼����� */
#define AW_COM_INPUT_EV_KEY            /**< \brief ʹ�ܰ����¼� */
#endif
/** @} */




/**
 * \name I2C(GPIOģ��)
 * @{
 */
#ifdef AW_DEV_GPIO_I2C_0
#ifndef AW_COM_I2C
#define AW_COM_I2C
#endif
#define AW_DRV_GPIO_I2C_0
#endif
/** @} */


#ifdef AW_DEV_GC0308
#define AW_DRV_GC0308
#ifndef AW_DEV_CAMERA
#define AW_DEV_CAMERA
#endif
#endif


/**
 * \name CSI������
 * @{
 */
#ifdef AW_DEV_IMX1050_CSI
#define AW_DRV_IMX1050_CSI
#endif
/** @} */



/**
 * \name ʱ���
 * @{
 */
#ifdef AW_DEV_IMX1050_TIMESTAMP
#define AW_DRV_IMX1050_TIMESTAMP
#endif
/** @} */




/**
 * \name LoRa sx127x
 * @{
 */
#ifdef AW_DEV_SX127X
#define AW_DRV_AWBL_SX127X
#define AW_COM_LRNET
#endif
/** @} */




/**
 * \name ��ʾ���
 * @{
 */
#if defined  AW_DEV_TFT_4_3 || \
    defined  AW_DEV_AP_TFT7_0 || \
    defined  AW_DEV_LCD_TM070RDH12_24B || \
    defined  AW_DEV_LCD_640480W056TR || \
    defined  AW_DEV_LCD_800600W080TR
#define AW_DRV_TFT_PANEL

#ifndef AW_DEV_FB
#define AW_DEV_FB
#endif

#define AW_DEV_IMX1050_PXP

#ifndef AW_DEV_IMX1050_EFLEX_PWM1
#define AW_DEV_IMX1050_EFLEX_PWM1
#ifndef AW_COM_PWM
#define AW_COM_PWM
#endif

#define AW_DRV_IMX1050_EFLEX_PWM
#endif

#endif
/** @} */

#ifdef AW_DEV_FB
#define AW_DRV_FB
#endif



/**
 * \name ��ΪME909S
 * @{
 */
#ifdef AW_DEV_GPRS_ME909S
#define AW_DRV_GPRS_ME909S
#endif
/** @} */




/**
 * \name SIM800 GPRS
 * @{
 */
#ifdef AW_DEV_GPRS_SIM800
#define AW_DRV_GPRS_SIM800
#endif
/** @} */




/**
 * \name ����U9300
 * @{
 */
#ifdef AW_DEV_GPRS_U9300C
#define AW_DRV_GPRS_U9300C
#endif
/** @} */

/**
 * \name ��ԶEC20
 * @{
 */
#ifdef AW_DEV_GPRS_EC20
#define AW_DRV_GPRS_EC20
#endif
/** @} */

/*
 * \name gps�豸
 * @{
 */
#ifdef AW_DEV_GNSS_UBLOX
#define AW_DEV_IMX1050_LPUART3
#define AW_DRV_UBLOX
#define AW_COM_GNSS
#endif

/**
 * \name �ļ�ϵͳ������IO��ϵͳ
 * @{
 */
#if defined AW_COM_FS_RAWFS || \
    defined AW_COM_FS_FATFS || \
    defined AW_COM_FS_LFFS || \
    defined AW_COM_FS_TXFS || \
    defined AW_COM_FS_YAFFS

#ifndef AW_COM_IO_SYSTEM
#define AW_COM_IO_SYSTEM
#endif

#ifndef AW_COM_MOUNT
#define AW_COM_MOUNT
#endif

#ifndef AW_COM_FS_ROOTFS
#define AW_COM_FS_ROOTFS
#endif
#endif
/** @} */




/**
 * \name �����ļ�ϵͳ�������豸
 * @{
 */
#if defined AW_COM_FS_RAWFS || \
    defined AW_COM_FS_FATFS || \
    defined AW_COM_FS_TXFS
#ifndef AW_COM_BLOCK_DEV
#define AW_COM_BLOCK_DEV
#endif
#endif
/** @} */




/**
 * \name �����ļ�ϵͳ����mtd�豸
 * @{
 */
#if defined AW_COM_FS_LFFS || \
    defined AW_COM_FS_YAFFS
#ifndef AW_COM_MTD_DEV
#define AW_COM_MTD_DEV
#endif
#endif
/** @} */




/*******************************************************************************
  ��ͻ���
*******************************************************************************/
/** \cond */
#if defined(AW_DEV_CSM300X) && defined(AW_DEV_FM175XX_SPI3_0)
#error "AW_DEV_CSM300X and AW_DEV_FM175XX_SPI3_0 cannot defined at the same time!"
#endif


/* ����ͬʱʹ��*/
#if defined(AW_DEV_IMX1050_WDT1) && defined(AW_DEV_IMX1050_RTWDT)
#error "AW_DEV_IMX1050_WDT1 and AW_DEV_IMX1050_RTWDT cannot defined at the same time!"
#endif


/* ����ͬʱʹ��ADC1��ADC2 */
//#if defined(AW_DEV_IMX1050_ADC1) && defined(AW_DEV_IMX1050_ADC2)
//#error "AW_DEV_IMX1050_ADC1 and AW_DEV_IMX1050_ADC2  cannot defined at the same time!"
//#endif

/* ����ͬʱʹ��TFT-4.3(A��CAP)�� LCD-TM070RDH12-24B��AW_DEV_AP_TFT7_0��AW_DEV_LCD_640480W056TR��AW_DEV_LCD_800600W080TR��ʾ�� */
#if (defined(AW_DEV_TFT_4_3) && defined(AW_DEV_LCD_TM070RDH12_24B))   || \
    (defined(AW_DEV_TFT_4_3) && defined (AW_DEV_AP_TFT7_0))           || \
    (defined(AW_DEV_TFT_4_3) && defined(AW_DEV_LCD_640480W056TR))     || \
    (defined(AW_DEV_TFT_4_3) && defined(AW_DEV_LCD_800600W080TR))     || \
    (defined(AW_DEV_LCD_TM070RDH12_24B) && defined(AW_DEV_AP_TFT7_0)) || \
    (defined(AW_DEV_LCD_TM070RDH12_24B) && defined(AW_DEV_LCD_640480W056TR)) || \
    (defined(AW_DEV_LCD_TM070RDH12_24B) && defined(AW_DEV_LCD_800600W080TR)) || \
    (defined(AW_DEV_AP_TFT7_0) && defined(AW_DEV_LCD_640480W056TR))   || \
    (defined(AW_DEV_AP_TFT7_0) && defined(AW_DEV_LCD_800600W080TR)) || \
    (defined(AW_DEV_LCD_640480W056TR) && defined(AW_DEV_LCD_800600W080TR))
#error "AW_DEV_TFT_4_3 and AW_DEV_LCD_TM070RDH12_24B and AW_DEV_AP_TFT7_0 and AW_DEV_LCD_640480W056TR and AW_DEV_LCD_800600W080TR cannot defined at the same time!"
#endif

/* ����ͬʱʹ��USB Device��USB HOST1 */
#if defined(AW_DEV_IMX1050_USBH1) && defined(AW_DEV_IMX1050_USBD)
#error "AW_DEV_IMX1050_USBH1 and AW_DEV_IMX1050_USBD  cannot defined at the same time!"
#endif

/* ����ͬʱʹ��WIFI��AW_DEV_CYW43362_WIFI_SDIO_0 */
#if defined(AW_DEV_CYW43362_WIFI_SPI_0) && defined(AW_DEV_CYW43362_WIFI_SDIO_0)
#error "AW_DEV_CYW43362_WIFI_SPI_0 and AW_DEV_CYW43362_WIFI_SDIO_0  cannot defined at the same time!"
#endif

/* ����ͬʱʹ��WIFI�͵��败���� */
#if defined(AW_DEV_CYW43362_WIFI_SPI_0) && defined(AW_DEV_IMX1050_TS)
#error "AW_DEV_CYW43362_WIFI_SPI_0 and AW_DEV_IMX1050_TS  cannot defined at the same time!"
#endif

/* ���繤�߱�������������� */
#ifndef AW_COM_NETWORK
#ifdef AW_NET_TOOLS
#error "AW_NET_TOOLS can not work without AW_COM_NETWORK"
#undef AW_NET_TOOLS
#endif
#endif

/* ����ͷ��������̫����I2C1��ͻ */
#if (defined(AW_DEV_OV7725) || defined(AW_DEV_GC0308) || defined(AW_DEV_GC0328)) \
    && (defined(AW_DEV_IMX1050_ENET) || defined(AW_DEV_IMX1050_LPSPI3) || defined(AW_DEV_IMX1050_GPT2))
#error "AW_DEV_OV7725 and AW_DEV_IMX1050_ENET or AW_DEV_IMX1050_LPSPI3 or AW_DEV_IMX1050_GPT2 cannot defined at the same time!"
#endif

#if defined(AW_DEV_IMX1050_CAN1) && defined(AW_DEV_IMX1050_CSI)
#error "AW_DEV_IMX1050_CAN1 and AW_DEV_IMX1050_CSI cannot defined at the same time!"
#endif

/* QTimer3��PWM��CAP���ܲ���ͬʱʹ�� */
#if defined(AW_DEV_IMX1050_QTIMER3_PWM) && defined(AW_DEV_IMX1050_QTIMER3_CAP)
#error "AW_DEV_IMX1050_QTIMER3_PWM and AW_DEV_IMX1050_QTIMER3_CAP cannot defined at the same time!"
#endif

/* LED��QTimer3��CAP���ܲ���ͬʱʹ�� */
#if defined(AW_DEV_GPIO_LED) && defined(AW_DEV_IMX1050_QTIMER3_CAP)
#error "AW_DEV_GPIO_LED and AW_DEV_IMX1050_QTIMER3_CAP cannot defined at the same time!"
#endif

/* CANFestival��������CAN���� */
#ifdef AW_COM_CANFESTIVAL
#ifndef AW_DRV_IMX1050_CAN
#error "AW_COM_CANFESTIVAL can not work without AW_DRV_IMX1050_CAN"
#endif
#endif

/* CANopen��������CAN���� */
#ifdef AW_COM_CANOPEN
#ifndef AW_DRV_IMX1050_CAN
#error "AW_COM_CANOPEN can not work without AW_DRV_IMX1050_CAN"
#endif
#endif

/*RTC�ʹ����豸���õ�I2C1���裬����ͬʱʹ��*/
#if defined(AW_DEV_EXTEND_PCF85063) && (defined(AW_DEV_BU21029MUV) || defined(AW_DEV_FT5X06))
#error "AW_DEV_EXTEND_PCF85063 and AW_DEV_BU21029MUV or AW_DEV_FT5X06 cannot defined at the same time!"
#endif

/*RTC�ʹ����豸���õ�I2C1���裬����ͬʱʹ��*/
#if defined(AW_DEV_IMX1050_LPI2C2) && (defined(AW_DEV_IMX1050_FLEXSPI))
#error "AW_DEV_IMX1050_LPI2C2 and AW_DEV_IMX1050_FLEXSPI cannot defined at the same time!"
#endif

#if defined(AW_DEV_IMX1050_QTIMER3_TIMER0) && defined(AW_DEV_IMX1050_LPI2C1)
#error "AW_DEV_IMX1050_QTIMER3_TIMER0 and AW_DEV_IMX1050_LPI2C1 can't defined at the same time!"
#endif

/*�����豸�Ͱ�����enc���ų�ͻ������ͬʱʹ��*/
#if (defined(AW_DEV_FT5X06) || defined(AW_DEV_BU21029MUV)) && (defined(AW_DEV_IMX1050_ENC1) || defined(AW_DEV_GPIO_KEY))
#warning "AW_DEV_FT5X06 or AW_DEV_BU21029MUV and AW_DEV_IMX1050_ENC1 or AW_DEV_GPIO_KEY cannot defined at the same time!"
#endif

/* ����ͬʱʹ��AW_DEV_IMX1050_GPT1_TIMER��AW_DEV_IMX1050_GPT1_CAP��AW_DEV_IMX1050_GPT1_PWM */
#if (defined(AW_DEV_IMX1050_GPT1_CAP) && (defined(AW_DEV_IMX1050_GPT1_PWM) || defined(AW_DEV_IMX1050_GPT1_TIMER))) || \
    (defined(AW_DEV_IMX1050_GPT1_PWM) && (defined(AW_DEV_IMX1050_GPT1_CAP) || defined(AW_DEV_IMX1050_GPT1_TIMER))) || \
    (defined(AW_DEV_IMX1050_GPT1_TIMER) && (defined(AW_DEV_IMX1050_GPT1_CAP) || defined(AW_DEV_IMX1050_GPT1_PWM)))
#error "AW_DEV_IMX1050_GPT1_CAP and AW_DEV_IMX1050_GPT1_PWM and AW_DEV_IMX1050_GPT1_TIMER cannot defined at the same time!"
#endif

/* ����ͬʱʹ��AW_DEV_IMX1050_GPT2_TIMER��AW_DEV_IMX1050_GPT2_CAP��AW_DEV_IMX1050_GPT2_PWM */
#if (defined(AW_DEV_IMX1050_GPT2_CAP) && (defined(AW_DEV_IMX1050_GPT2_PWM) || defined(AW_DEV_IMX1050_GPT2_TIMER))) || \
    (defined(AW_DEV_IMX1050_GPT2_PWM) && (defined(AW_DEV_IMX1050_GPT2_CAP) || defined(AW_DEV_IMX1050_GPT2_TIMER))) || \
    (defined(AW_DEV_IMX1050_GPT2_TIMER) && (defined(AW_DEV_IMX1050_GPT2_CAP) || defined(AW_DEV_IMX1050_GPT2_PWM)))
#error "AW_DEV_IMX1050_GPT2_CAP and AW_DEV_IMX1050_GPT2_PWM and AW_DEV_IMX1050_GPT2_TIMER cannot defined at the same time!"
#endif

#endif  /* __AW_PRJ_PARAMS_AUTO_CFG_H */

/* end of file */
