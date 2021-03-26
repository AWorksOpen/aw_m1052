/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_TS_H
#define __AWBL_HWCONF_IMX1050_TS_H

#ifdef AW_DEV_IMX1050_TS

#include "driver/input/touchscreen/awbl_imx1050_ts.h"
#include "aw_ts.h"



aw_local void __imx1050_ts_plfm_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_TSC_DIG);
}


aw_local awbl_imx1050_ts_devinfo_t __g_imx1050_ts_devinfo = {
        IMX1050_TS_CTRL_BASE,     /* �Ĵ�������ַ */
        1,                        /* x_plus_ch  : ADC channel 1 */
        2,                        /* x_minus_ch : ADC channel 2 */
        3,                        /* y_plus_ch  : ADC channel 3 */
        4,                        /* y_minus_ch : ADC channel 4 */
        {
            "imx105x-ts",                    /**< \brief ������������ʾ������       */
            0x01,                         /**< \brief ������������ʾ������ID */
            0x01,                         /**< \brief ������������ʾ����ƷID */
            5,                            /**< \brief ֧����ഥ����ĸ���.    */
            35,                           /**< \brief ��������, ��СֵΪ1  */
            8,                            /**< \brief �˲�����, ���ܴ��ڲ�������, ��СֵΪ1 */
            60,                           /**< \brief ����ʱ��      */
            120,                          /**< \brief �������޷�ֵ,��������,���ݹ�����ͬ����ʾ�������������. */
            5,                            /**< \brief У׼���޷�ֵ,������֤У׼ϵ��,���ݹ�����ͬ����ʾ�������������. */
            AW_TS_LIB_FIVE_POINT,         /**< \brief ���У׼�㷨   */
        },
       __imx1050_ts_plfm_init     /* ƽ̨��ʼ�� */
};

/* �豸ʵ���ڴ澲̬���� */
aw_local awbl_imx1050_ts_dev_t __g_imx1050_ts;

#define AWBL_HWCONF_IMX1050_TS          \
    {                                   \
        AWBL_IMX1050_TOUCHSCREEN_NAME,  \
        0,                              \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &(__g_imx1050_ts.adev),         \
        &(__g_imx1050_ts_devinfo)       \
    },

#else
#define AWBL_HWCONF_IMX1050_TS

#endif  /* AW_DEV_IMX1050_TS */

#endif  /* __AWBL_HWCONF_IMX1050_TS_H */

/* end of file */
