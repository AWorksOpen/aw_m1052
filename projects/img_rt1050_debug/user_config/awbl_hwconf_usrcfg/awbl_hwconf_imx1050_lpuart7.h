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
  IMX1050 内置UART  配置信息
*******************************************************************************/

#ifndef AWBL_HWCONF_IMX1050_LPUART7_H_
#define AWBL_HWCONF_IMX1050_LPUART7_H_

#include "driver/serial/awbl_imx10xx_lpuart.h"
#include "imx1050_pin.h"
#include "aw_prj_params.h"

#ifdef AW_DEV_IMX1050_LPUART7

aw_local void __imx10xx_lpuart7_plat_init (void)
{
    aw_clk_enable(IMX1050_CLK_CG_LPUART7);

    int lpuart7_gpios[] = {GPIO3_8, GPIO3_9};

    if (aw_gpio_pin_request("lpuart7_gpios",
                             lpuart7_gpios,
                             AW_NELEMENTS(lpuart7_gpios)) == AW_OK) {
    }
    aw_gpio_pin_cfg(GPIO3_8, GPIO3_8_LPUART7_TX);
    aw_gpio_pin_cfg(GPIO3_9, GPIO3_9_LPUART7_RX);
}

//aw_local void __imx10xx_lpuart7_rs485_dir (uint8_t dir)
//{
//    //aw_gpio_set(GPIO17, dir);
//}

/* imx1050 lpuart7 设备信息 */
static char     _uart7_recv_buf[128];
aw_local awbl_imx10xx_lpuart_dev_info_t aw_const __g_imx10xx_lpuart7_devinfo = {
    INUM_LPUART7,                // irq
    IMX1050_LPUART7_BASE,        // 寄存器基地址
    IMX1050_CLK_CG_LPUART7,      // 输入时钟id
    _uart7_recv_buf,
    sizeof(_uart7_recv_buf),
    115200,
    __imx10xx_lpuart7_plat_init, // 平台初始化函数
    NULL                         // RS485方向控制函数

};

/* imx1050内部UART设备实例内存静态分配 */
aw_local struct awbl_imx10xx_uart_dev __g_imx10xx_lpuart7_dev;

#define AWBL_HWCONF_IMX1050_LPUART7     \
    {                                   \
        AWBL_IMX10XX_LPUART_DRV_NAME,   \
        IMX1050_LPUART7_COMID,          \
        AWBL_BUSID_PLB,                 \
        0,                              \
        &__g_imx10xx_lpuart7_dev.dev,   \
        &__g_imx10xx_lpuart7_devinfo    \
    },
#else
#define AWBL_HWCONF_IMX1050_LPUART7
#endif

#endif /* AWBL_HWCONF_IMX1050_LPUART7_H_ */
