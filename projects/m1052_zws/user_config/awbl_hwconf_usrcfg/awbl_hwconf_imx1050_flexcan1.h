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

#ifndef __AWBL_HWCONF_IMX1050_CAN1_H
#define __AWBL_HWCONF_IMX1050_CAN1_H

#ifdef AW_DEV_IMX1050_CAN1

#include "awbus_lite.h"
#include "aw_gpio.h"
#include "aw_rngbuf.h"

#include "driver/can/awbl_imx10xx_can.h"

#define AW_CFG_CAN1_CTLR_HS         AW_CAN_CTLR_HS/**< \brief ����CAN */
#define AW_CFG_CAN1_RX_BUF_SIZE     512

aw_local void __imx1050_can1_plfm_init (void)
{
    int can1_gpios[] = {
        GPIO1_24,  /* CAN1 TX */
        GPIO1_25,  /* CAN1 RX */
    };

    if (aw_gpio_pin_request("can1_gpios",
                             can1_gpios,
                             AW_NELEMENTS(can1_gpios)) == AW_OK) {
        /* ����CAN TX���� */
        aw_gpio_pin_cfg(GPIO1_24, GPIO1_24_FLEXCAN1_TX);

        /* ����CAN RX���� */
        aw_gpio_pin_cfg(GPIO1_25, GPIO1_25_FLEXCAN1_RX);
    }

    aw_clk_enable(IMX1050_CLK_CG_CAN1);
    aw_clk_enable(IMX1050_CLK_CG_CAN1_SERIAL);
}

aw_local struct aw_rngbuf  __g_can1_rd_buf_ctr;          /**< \brief CAN0���ձ��Ļ��λ��� */

AW_RNGBUF_SPACE_DEFINE(
        __g_can1_rx_msg_buff, struct aw_can_std_msg, AW_CFG_CAN1_RX_BUF_SIZE);

/** \brief CAN1 �豸��Ϣ */
aw_local aw_const struct awbl_imx10xx_can_dev_info __g_imx1050_can1_devinfo = {
    {
        IMX1050_FLAX_CAN1_BUSID,                   /**< \brief ��ǰͨ���� */
        AW_CFG_CAN1_CTLR_HS,                       /**< \brief ����������*/
        sizeof(struct aw_can_std_msg),             /**< \brief һ֡���ĳ���*/
        AW_CFG_CAN1_RX_BUF_SIZE,                   /**< \brief ���ջ�������С*/
        AW_RNGBUF_SPACE_GET(__g_can1_rx_msg_buff), /**< \brief ���ջ������׵�ַ*/
        &__g_can1_rd_buf_ctr                       /**< \brief ����rngbuf������*/
    },
    IMX1050_CAN1_BASE,                             /**< \brief �Ĵ�������ַ*/
    INUM_CAN1,                                     /**< \brief �жϺ� */
    __imx1050_can1_plfm_init                       /**< \brief ƽ̨��ʼ������*/
};

/* \brief CAN1�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_can_dev __g_imx1050_can1_dev;

#define AWBL_HWCONF_IMX1050_CAN1     \
    {                                \
        AWBL_IMX10XX_CAN_NAME,       \
        1,                           \
        AWBL_BUSID_PLB,              \
        0,                           \
        &(__g_imx1050_can1_dev.dev), \
        &(__g_imx1050_can1_devinfo)  \
    },

#else
#define AWBL_HWCONF_IMX1050_CAN1

#endif  /* AW_DEV_IMX1050_CAN1 */

#endif  /* __AWBL_HWCONF_IMX1050_CAN1_H */

/* end of file */
