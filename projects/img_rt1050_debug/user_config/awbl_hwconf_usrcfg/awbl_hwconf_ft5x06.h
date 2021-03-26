/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief ����������������������Ϣ
 *
 * \internal
 * \par modification history:
 * - 1.01 16-08-18  anu, first implemetation
 * \endinternal
 */

#ifndef __AWBL_HWCONF_FT5X06_H
#define __AWBL_HWCONF_FT5X06_H

#ifdef AW_DEV_FT5X06

#include "aw_gpio.h"

#include "imx1050_pin.h"
#include "driver/input/touchscreen/awbl_ft5x06_ts.h"
#include "aw_ts.h"


/* configure infomation */
aw_local struct awbl_ft5x06_ts_devinfo __g_ft5x06_ts_devinfo={
        FT5x06_ADDRESS,
        -1,
        GPIO1_11,
        {
            "ft5x06",                    /**< \brief ������������ʾ������       */
            0,                            /**< \brief ������������ʾ������ID */
            0,                            /**< \brief ������������ʾ����ƷID */
            5,                            /**< \brief ֧����ഥ����ĸ���.    */
            3,                            /**< \brief ��������, ��СֵΪ1  */
            1,                            /**< \brief �˲�����, ���ܴ��ڲ�������, ��СֵΪ1 */
            30,                           /**< \brief ����ʱ��      */
            40,                           /**< \brief �������޷�ֵ,��������,���ݹ�����ͬ����ʾ�������������. */
            5,                            /**< \brief У׼���޷�ֵ,������֤У׼ϵ��,���ݹ�����ͬ����ʾ�������������. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief ���У׼�㷨   */
        },
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_ft5x06_ts_dev __g_ft5x06_ts;

#define AWBL_HWCONF_FT5X06              \
    {                                   \
        AWBL_FT5X06_TOUCHSCREEN_NAME,   \
        0,                              \
        AWBL_BUSID_I2C,                 \
        IMX1050_LPI2C1_BUSID,           \
        &(__g_ft5x06_ts.super),         \
        &(__g_ft5x06_ts_devinfo)        \
    },
#else

#define AWBL_HWCONF_FT5X06

#endif /* AW_DEV_FT5X06 */

#endif /* __AWBL_HWCONF_FT5X06_H */

