/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1010 ocotp driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2017-11-15  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1010_OCOTP_H
#define __AWBL_IMX1010_OCOTP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_task.h"
#include "awbl_nvram.h"
#include "aw_sem.h"

#define AWBL_IMX1010_OCOTP_NAME   "imx1010_ocotp"

/**
 * \brief iMX RT1010 ocotp 设备信息
 */
struct awbl_imx1010_ocotp_devinfo {

    /** \brief 非易失性存储段配置列表 */
    const struct awbl_nvram_segment *p_seglst;

    /** \brief 非易失性存储段配置列表条目数 */
    size_t seglst_count;

    /** \brief 寄存器基地址 */
    uint32_t regbase;

    /** \brief 输入时钟ID */
    uint32_t clk_id;
};


/**
 * \brief iMX RT1010 ocotp 设备实例
 */
struct awbl_imx1010_ocotp_dev {
	struct awbl_dev super;
};


/**
 * \brief iMX RT1010 ocotp 驱动注册
 */
void awbl_imx1010_ocotp_register(void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX1010_OCOTP_H */

/* end of file */
