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

#ifndef AWBL_HWCONF_IMX1050_LPUART2_H_
#define AWBL_HWCONF_IMX1050_LPUART2_H_

#include "driver/serial/awbl_imx10xx_lpuart.h"
#include "imx1050_pin.h"
#include "aw_prj_params.h"

#ifdef AW_DEV_IMX1050_LPUART2

aw_local void __imx10xx_lpuart2_plat_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_LPUART2);

    int lpuart2_gpios[] = {GPIO1_18, GPIO1_19};

    if (aw_gpio_pin_request("lpuart2_gpios",
                             lpuart2_gpios,
                             AW_NELEMENTS(lpuart2_gpios)) == AW_OK) {
    }
    aw_gpio_pin_cfg(GPIO1_18, GPIO1_18_LPUART2_TX);
    aw_gpio_pin_cfg(GPIO1_19, GPIO1_19_LPUART2_RX);
}

//aw_local void __imx10xx_lpuart2_rs485_dir (uint8_t dir)
//{
//    //aw_gpio_set(GPIO17, dir);
//}

/* imx1050 lpuart2 �豸��Ϣ */
static char     _uart2_recv_buf[128];
aw_local awbl_imx10xx_lpuart_dev_info_t aw_const __g_imx10xx_lpuart2_devinfo = {
    INUM_LPUART2,                // irq
    IMX1050_LPUART2_BASE,        // �Ĵ�������ַ
    IMX1050_CLK_CG_LPUART2,      // ����ʱ��id
    _uart2_recv_buf,
    sizeof(_uart2_recv_buf),
    115200,
    __imx10xx_lpuart2_plat_init, // ƽ̨��ʼ������
    NULL                         // RS485������ƺ���

};

/* imx1050�ڲ�UART�豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx10xx_uart_dev __g_imx10xx_lpuart2_dev;

#define AWBL_HWCONF_IMX1050_LPUART2     \
    {                                   \
        AWBL_IMX10XX_LPUART_DRV_NAME,   \
        IMX1050_LPUART2_COMID,          \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &__g_imx10xx_lpuart2_dev.dev,   \
        &__g_imx10xx_lpuart2_devinfo    \
    },
#else
#define AWBL_HWCONF_IMX1050_LPUART2
#endif

#endif /* AWBL_HWCONF_IMX1050_LPUART2_H_ */
