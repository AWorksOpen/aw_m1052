/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AWBL_HWCONF_IMX1050_CAN2_H
#define __AWBL_HWCONF_IMX1050_CAN2_H

#ifdef AW_DEV_IMX1050_CAN2

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_rngbuf.h"

#include "driver/can/awbl_imx10xx_can.h"

#define AW_CFG_CAN2_CTLR_HS         AW_CAN_CTLR_HS/**< \brief ����CAN */
#define AW_CFG_CAN2_RX_BUF_SIZE     512


aw_local void __imx1050_can2_plfm_init (void)
{
    int can2_gpios[] = {
        GPIO1_14,  /* CAN2 TX */
        GPIO1_15,  /* CAN2 RX */
    };

    if (aw_gpio_pin_request("can2_gpios",
                             can2_gpios,
                             AW_NELEMENTS(can2_gpios)) == AW_OK) {

        /* ����CAN2 TX���� */
        aw_gpio_pin_cfg(GPIO1_14, GPIO1_14_FLEXCAN2_TX);

        /* ����CAN2 RX���� */
        aw_gpio_pin_cfg(GPIO1_15, GPIO1_15_FLEXCAN2_RX);
    }

    aw_clk_enable(IMX1050_CLK_CG_CAN2);
    aw_clk_enable(IMX1050_CLK_CG_CAN2_SERIAL);
}


aw_local struct aw_rngbuf  __g_can2_rd_buf_ctr;          /**< \brief CAN2���ձ��Ļ��λ��� */

AW_RNGBUF_SPACE_DEFINE(
        __g_can2_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CAN2_RX_BUF_SIZE);

/** \brief CAN2 �豸��Ϣ */
aw_local aw_const struct awbl_imx10xx_can_dev_info __g_imx1050_can2_devinfo = {
    {
        IMX1050_FLAX_CAN2_BUSID,                   /**< \brief ��ǰͨ���� */
        AW_CFG_CAN2_CTLR_HS,                       /**< \brief ����������*/
        sizeof(struct aw_can_std_msg),             /**< \brief һ֡���ĳ���*/
        AW_CFG_CAN2_RX_BUF_SIZE,                   /**< \brief ���ջ�������С*/
        AW_RNGBUF_SPACE_GET(__g_can2_rx_msg_buff), /**< \brief ���ջ������׵�ַ*/
        &__g_can2_rd_buf_ctr                       /**< \brief ����rngbuf������*/
    },
    IMX1050_CAN2_BASE,                             /**< \brief �Ĵ�������ַ*/
    INUM_CAN2,                                     /**< \brief �жϺ� */
    __imx1050_can2_plfm_init                       /**< \brief ƽ̨��ʼ������*/
};

/* \brief CAN2�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_can_dev __g_imx1050_can2_dev;

#define AWBL_HWCONF_IMX1050_CAN2     \
    {                                \
        AWBL_IMX10XX_CAN_NAME,       \
        2,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        &(__g_imx1050_can2_dev.dev), \
        &(__g_imx1050_can2_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX1050_CAN2

#endif  /* AW_DEV_IMX1050_CAN2 */

#endif  /* __AWBL_HWCONF_IMX1050_CAN2_H */

/* end of file */
