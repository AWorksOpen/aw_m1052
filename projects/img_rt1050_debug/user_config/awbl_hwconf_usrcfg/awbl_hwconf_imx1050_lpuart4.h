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

/*******************************************************************************
  IMX1050 ����UART  ������Ϣ
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX1050_LPUART4_H_
#define AWBL_HWCONF_IMX1050_LPUART4_H_

#include "driver/serial/awbl_imx10xx_lpuart.h"
#include "imx1050_pin.h"
#include "aw_prj_params.h"

#ifdef AW_DEV_IMX1050_LPUART4

aw_local void __imx10xx_lpuart4_plat_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_LPUART4);

    int lpuart4_gpios[] = {GPIO2_16, GPIO2_17};

    if (aw_gpio_pin_request("lpuart4_gpios",
                             lpuart4_gpios,
                             AW_NELEMENTS(lpuart4_gpios)) == AW_OK) {
    }
    aw_gpio_pin_cfg(GPIO2_16, GPIO2_16_LPUART4_TX);
    aw_gpio_pin_cfg(GPIO2_17, GPIO2_17_LPUART4_RX);
}

//aw_local void __imx10xx_lpuart4_rs485_dir (uint8_t dir)
//{
//    //aw_gpio_set(GPIO17, dir);
//}

/* imx1050 lpuart4 �豸��Ϣ */
static char     _uart4_recv_buf[128];
aw_local awbl_imx10xx_lpuart_dev_info_t aw_const __g_imx10xx_lpuart4_devinfo = {
    INUM_LPUART4,                // irq
    IMX1050_LPUART4_BASE,        // �Ĵ�������ַ
    IMX1050_CLK_CG_LPUART4,      // ����ʱ��id
    _uart4_recv_buf,
    sizeof(_uart4_recv_buf),
    115200,
    __imx10xx_lpuart4_plat_init, // ƽ̨��ʼ������
    NULL                         // RS485������ƺ���

};

/* imx1050�ڲ�UART�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_uart_dev __g_imx10xx_lpuart4_dev;

#define AWBL_HWCONF_IMX1050_LPUART4     \
    {                                   \
        AWBL_IMX10XX_LPUART_DRV_NAME,   \
        IMX1050_LPUART4_COMID,          \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &__g_imx10xx_lpuart4_dev.dev,   \
        &__g_imx10xx_lpuart4_devinfo    \
    },
#else
#define AWBL_HWCONF_IMX1050_LPUART4
#endif

#endif /* AWBL_HWCONF_IMX1050_LPUART4_H_ */
