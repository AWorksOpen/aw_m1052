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
 * \brief gt911 touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.00 14-07-17  ops, first implemetation
 * \endinternal
 */
#ifndef AWBL_GT911_TS_H_
#define AWBL_GT911_TS_H_

#include "awbl_input.h"
#include "awbl_ts.h"
#include "aw_task.h"
#include "aw_i2c.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

//#define GT911_DEBUG

#define CFG_SAVE    1
#define CFG_NOTSAVE 0


#define GT911_CHIPTYPE_REG       0x8000        /**< \brief ��ȡоƬ����*/
#define GT911_COMMAND_REG        0x8040        /**< \brief ���ƼĴ���  */
#define GT911_CONFIGVERSION_REG  0x8047        /**< \brief �����ļ��汾��*/
#define GT911_CHECKSUM_REG       0X80FF        /**< \brief У��ͼĴ��� */
#define GT911_ID_VERSION_REG     0x8140        /**< \brief �̼��汾.  */
#define GT911_VENDORID_REG       0x814A        /**< \brief ģ��ѡ����Ϣ */
#define GT911_GSTID_REG          0x814E        /**< \brief ������Ϣ */
#define GT911_TP1_REG            0x8150        /**< \brief ��һ��������������ʼ��ַ*/
#define GT911_TP2_REG            0x8158        /**< \brief �ڶ���������������ʼ��ַ*/
#define GT911_TP3_REG            0x8160        /**< \brief ������������������ʼ��ַ*/
#define GT911_TP4_REG            0x8168        /**< \brief ���ĸ�������������ʼ��ַ*/
#define GT911_TP5_REG            0x8170        /**< \brief �����������������ʼ��ַ*/


/**
 * \name   GT911�����豸I2C�ӻ���ַ
 * \remark GT911 ���������豸��ַ
 * \remark ��I2C�豸��ַ��7λ�豸��ַ��1λ��д����λ��ɣ���7λΪ��ַ��bit0Ϊ��д����λ
 * \remark �豸�ĵ�ַ��ѡ������������ϵ��ʼ��ʱ����Reset ��INT �ڲ�ͬ״̬
 * @{
 */
#define GT911_ADDRESS1        (0x5D)
#define GT911_ADDRESS2        (0x14)

#define AWBL_GT911_TOUCHSCREEN_NAME            "gt911"

typedef struct awbl_gt911_ts_devinfo {
    /** \brief GT911�豸��ַ               */
    int  dev_addr;

    /** \brief ����GT911�豸�ĸ�λ���� */
    int   rst_pin;

    /** \brief ����GT011�豸���ж����� */
    int   int_pin;

    /** \brief �û������x��������� */
    uint16_t x_usr_output_max;
    /** \brief �û������y��������� */
    uint16_t y_usr_output_max;

    awbl_ts_service_info_t ts_serv_info;

}awbl_gt911_ts_devinfo_t;

typedef struct awbl_gt911_ts_dev {

    /** \brief �̳��� AWBus�豸 */
    struct awbl_dev super;

    /** \bried �̳���ts����.   */
    struct awbl_ts_service ts_serv;

    /** \brief gt911���豸�ṹ.*/
    aw_i2c_device_t i2c_gt911;

    /** \brief ��һ�°��±�־ */
    uint8_t is_debounce;

}awbl_gt911_ts_dev_t;

/** \brief ����������ע��*/
void awbl_gt911_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_GT911_TS_H_ */



















