/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef  __AWBL_HWCONF_IMX1050_SAI3_H
#define  __AWBL_HWCONF_IMX1050_SAI3_H

#ifdef AW_DEV_IMX1050_SAI3

/*******************************************************************************
 SAI3������������Ϣ
*******************************************************************************/
#include "aworks.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "driver/sai/awbl_imx1050_sai.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_gpio.h"

#define  EDMA_EVENTS_SAI3_TX  IMX_DMATYPE_SAI3_TX

aw_local void __imx1050_sai3_plat_init (void)
{
    aw_clk_rate_set(IMX1050_CLK_PLL_AUDIO, SAI_SAMPLE_RATE*32*2*8*8);
    aw_clk_parent_set(IMX1050_CLK_SAI3_CLK_SEL ,IMX1050_CLK_PLL_AUDIO);

    /* �Ƚ���ʱ�ӣ������������ */
    aw_clk_disable(IMX1050_CLK_CG_SAI3);
}

/* �豸��Ϣ */
aw_local aw_const struct awbl_imx1050_sai_devinfo __g_imx1050_sai3_devinfo = {
    IMX1050_SAI3_BASE,                  /* �Ĵ�����ַ */
    INUM_SAI3,                          /* �жϺ� */
    IMX1050_CLK_SAI3_CLK_ROOT,                  /* ʱ��id */
    AW_BL_IMX1050_SAI_MCLK_SEL_1,       /* ʹ��MQSʱ����ֵ����Ϊ 1 */
    {"imxrt1050_sai0_playback", "imxrt1050_sai0_capture"},
    {"imxrt1050_dai_sai0"},
    AWSA_SOC_DAI_FMT_LEFT_J,
    __imx1050_sai3_plat_init,           /* ƽ̨��س�ʼ�� */
    IMX_DMATYPE_SAI3_TX,
    IMX_DMATYPE_SAI3_RX,
};

/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx1050_sai_dev __g_imx1050_sai3_dev;

#define AWBL_HWCONF_IMX1050_SAI3                    \
    {                                               \
        AWBL_IMX1050_SAI_NAME,                      \
        2,                                          \
        AWBL_BUSID_PLB,                             \
        0,                                          \
        (struct awbl_dev *)&__g_imx1050_sai3_dev,   \
        &__g_imx1050_sai3_devinfo                   \
    },

#else

#define AWBL_HWCONF_IMX1050_SAI3

#endif /* AW_DEV_IMX1050_SAI3 */

#endif /* __AWBL_HWCONF_IMX1050_SAI3_H */


