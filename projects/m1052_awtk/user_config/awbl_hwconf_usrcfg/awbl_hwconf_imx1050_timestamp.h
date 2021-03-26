/*
 * awbl_hwconf_timestamp.h
 *
 *  Created on: 2020��4��7��
 *      Author: weike
 */

#ifndef __AWBL_HWCONF_IMX1050_TIMESTAMP_H
#define __AWBL_HWCONF_IMX1050_TIMESTAMP_H

#define AW_DEV_IMX1050_TIMESTAMP

#ifdef AW_DEV_IMX1050_TIMESTAMP

#include "aw_clk.h"
#include "imx1050_inum.h"
#include "driver/timestamp/awbl_imx1050_timestamp.h"


void __imx1050_timestamp_plfm_init(void)
{

}
/* �豸��Ϣ */
aw_local aw_const struct awbl_imx1050_timestamp_devinfo __g_imx1050_timestamp_devinfo = {
    100,                            /* �����������ڣ�ms�� */
    1000000,
    __imx1050_timestamp_plfm_init,
};
/* �豸ʵ���ڴ澲̬���� */
aw_local struct awbl_imx1050_timestamp_dev __g_imx1050_timestamp_dev;

#define AWBL_HWCONF_IMX1050_TIMESTAMP                  \
{                                                \
    AWBL_IMX1050_TIMESTAMP_NAME,                       \
    1,                                           \
    AWBL_BUSID_PLB,                              \
    0,                                           \
    (struct awbl_dev *)&__g_imx1050_timestamp_dev,     \
    &(__g_imx1050_timestamp_devinfo)                   \
},

#else
#define AWBL_HWCONF_IMX1050_TIMESTAMP
#endif  /* AW_DEV_IMX1050_TIMESTAMP */

#endif /* __AWBL_HWCONF_IMX1050_TIMESTAMP_H */
