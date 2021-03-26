/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ep24cxx ͨ��EEPROM����
 *
 * EP24CXX ��ָ�볣����CAT24C02��CAT24C08��һϵ��EEPROM����оƬ��оƬ������
 * 1kbit �� 1Mbit��
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "ep24cxx"
 *  - ��������: AWBL_BUSID_I2C
 *  - �豸��Ϣ: struct awbl_ep24cxx_devinfo
 *
 * \par 2.�����豸
 *
 *  - CAT24C02��CAT24C04��CAT24C256��ȫϵ��EEPROM��
 *  - ������CAT24CXX���ݵ�EEPROM
 *
 * \par 3.���ʽӿ�
 *
 *  - \ref grp_aw_serv_nvram
 *
 * \par 4.�豸����/����
 *
 *  - \ref grp_awbl_ep24cxx_hwconf
 *
 * \par 5.�������
 *
 * �ܹ�ʹ�ñ�����֮ǰ��Ҫ��������������:
 *
 *  - AW_COM_AWBL_NVRAM (NVRAM ͨ�ÿ�)
 *  - AW_COM_AWBL_I2CBUS (I2C-bus ͨ�ÿ�)
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-22  zen, first implementation
 * \endinternal
 */

/**
 * \addtogroup grp_awbl_drv_ep24cxx
 * \copydetails awbl_ep24cxx.h
 * @{
 */

/**
 * \defgroup grp_awbl_ep24cxx_hwconf    �豸����/����
 *
 * EP24CXX Ӳ���豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 * \note ��Ҫ��չ����ƬEEPROM���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ��
 *       ���֡�
 *
 * \include  hwconf_usrcfg_nvram_ep24cxx.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1���洢�������б��û����á� \n
 * �ڴ��б��н�EEPROM�豸�Ĵ洢�ռ仮��Ϊ���ɴ洢�Σ�Ȼ��Ϳ���ʹ�ñ�׼ NVRAM
 * �ӿ� aw_nvram_set() �� aw_nvram_get() ����д��Щ�洢�Σ�������Ϣ��ο�
 * \ref grp_aw_serv_nvram ��
 *
 * - ��2��I2C �ӻ���ַ \n
 * ����EEPROM���û��ֲ��Լ�Ŀ��ϵͳ��Ӳ�������д�˲�����
 *
 * - ��3��оƬ�ͺ� \n
 * ����д���ͺ���ο� \ref grp_awbl_ep24cxx_model ��
 *
 * - ��4���Ƿ��з�ҳ \n
 * �����EEPROM���û��ֲ���д�˲����� true -�з�ҳ�� false -�޷�ҳ��
 * \note ���繫˾��FRAM�޷�ҳ����д false ��
 *
 * - ��5��д���� \n
 * �����EEPROM���û��ֲ���д�˲�������λ��ms��
 * \note ���繫˾��FRAM��д���ڣ���д 0 ��
 *
 * - ��6��EP24CXX ��Ԫ�� \n
 * Ϊÿ�� EP24CXX �豸���䲻ͬ�ĵ�Ԫ��
 *
 * - ��7��I2C���߱�� \n
 * EEPROM����I2C���߱�ţ�ϵͳ��I2C���߱����ο� \ref grp_aw_plfm_i2c_busid
 */

/** \defgroup grp_awbl_ep24cxx_model */

/** @} grp_awbl_drv_ep24cxx */

#ifndef __AWBL_EP24CXX_H
#define __AWBL_EP24CXX_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_task.h"
#include "awbl_i2cbus.h"
#include "awbl_nvram.h"
#include "aw_sem.h"

#define AWBL_EP24CXX_NAME   "ep24cxx"

/**
 * \addtogroup grp_awbl_ep24cxx_model EEPROM оƬ�ͺŶ���
 * @{
 * ֻҪ��CAT EEPROM ϵ��оƬ���ݣ�����������ĳ���ͺš�
 *
 * ���磬ON ��˾��оƬ CAT24C04 CAT24L04 �� RAMTRON ��˾�� FM24C04 FM24L04
 * �������ͺ� AWBL_EP24CXX_EP24C04
 */
//#define AWBL_EP24CXX_EP24C01    (0u)        /**< \brief 128 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C02    (1u)        /**< \brief 256 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C04    (2u)        /**< \brief 512 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C08    (3u)        /**< \brief 1024 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C16    (4u)        /**< \brief 2048 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C32    (5u)        /**< \brief 4096 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C64    (6u)        /**< \brief 8192 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C128   (7u)        /**< \brief 16384 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C256   (8u)        /**< \brief 32768 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C512   (9u)        /**< \brief 65536 Byte EEPROM */
//#define AWBL_EP24CXX_EP24C1024  (10u)       /**< \brief 131072 Byte EEPROM */
//
//#define AWBL_DA24XX_DA2460      (11u)       /**< \brief 256 Byte EEPROM */


/*
 * bit[14:0]��bytes of page size
 * bit[15]��are there pages��1:has pages  0:has no pages
 * bit[18:16]��data address bits number in slave address
 * bit[19]��bytes of the register address length��0:1byte  1:2bytes
 * bit[23:20]��max size, (2^n)*128 bytes in the eeprom
 * bit[31:24]��write time 0~255 ms
 */

#define AWBL_EP24CXX_EP24C01    \
    ( (16u) | (1u << 15) | (0u << 16) | (0u << 19) | (0u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C02    \
    ( (16u) | (1u << 15) | (0u << 16) | (0u << 19) | (1u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C04    \
    ( (16u) | (1u << 15) | (1u << 16) | (0u << 19) | (2u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C08    \
    ( (16u) | (1u << 15) | (2u << 16) | (0u << 19) | (3u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C16    \
    ( (16u) | (1u << 15) | (3u << 16) | (0u << 19) | (4u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C32    \
    ( (32u) | (1u << 15) | (0u << 16) | (1u << 19) | (5u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C64    \
    ( (32u) | (1u << 15) | (0u << 16) | (1u << 19) | (6u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C128   \
    ( (64u) | (1u << 15) | (0u << 16) | (1u << 19) | (7u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C256   \
    ( (64u) | (1u << 15) | (0u << 16) | (1u << 19) | (8u << 20) | (5u << 24) )
#define AWBL_EP24CXX_EP24C512   \
    ( (128u) | (1u << 15) | (0u << 16) | (1u << 19) | (9u << 20) | (10u << 24) )
#define AWBL_EP24CXX_EP24C1024  \
   ( (256u) | (1u << 15) | (1u << 16) | (1u << 19) | (10u << 20) | (10u << 24) )


#define AWBL_EP24CXX_DS2460      ( \
/* page size */                        (8u)       | \
/* has page  */                        (1u << 15) | \
/* data bits in slave address */       (0u << 16) | \
/* register address length  */         (0u << 19) | \
/* max size  */                        (1u << 20) | \
/* write time  */                      (10u << 24) \
                                )

/** @} */

#define AWBL_EP24CXX_TPMAX      (11u)
#define AWBL_EP24CXX_TPCNT      (AWBL_EP24CXX_TPMAX + 1)

/**
 * \brief EP24CXX �豸��Ϣ
 */
struct awbl_ep24cxx_devinfo {

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

    /** \brief оƬ�ͺ� */
    uint32_t type;

    /** \brief ����ʧ�Դ洢�������б� */
    const struct awbl_nvram_segment *p_seglst;

    /** \brief ����ʧ�Դ洢�������б���Ŀ�� */
    size_t seglst_count;
};

/**
 * \brief EP24CXX �豸ʵ��
 */
struct awbl_ep24cxx_dev {
    struct awbl_i2c_device  super;      /**< \brief �̳���AWBus I2C device */
    AW_MUTEX_DECL(devlock);             /**< \brief �豸�� */
};

/**
 * \brief lpc11xx driver register
 */
void awbl_ep24cxx_drv_register(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_EP24CXX_H */

/* end of file */
