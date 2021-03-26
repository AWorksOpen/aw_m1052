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

/**
 * \file
 * \brief AWorks ֡��������
 *
 * \internal
 * \par modification history:
 * - 1.01  19-05-06  cat, rename.
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */


#ifndef __AWBL_IMX1050_FB_H
#define __AWBL_IMX1050_FB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_fb.h"

#include "driver/display/awbl_imx1050_lcdif.h"
#include "aw_spinlock.h"

#define AWBL_IMX1050_FB_NAME      "imx1050_fb"

/** \brief �豸��Ϣ */
typedef struct awbl_imx1050_fb_devinfo {
    awbl_imx1050_lcdif_devinfo_t lcdif_info;
} awbl_imx1050_fb_devinfo_t;

/** \brief LCDC�豸 */
typedef struct awbl_imx1050_fb {

    awbl_dev_t               dev;
    awbl_imx1050_lcdif_t     lcdif;
    awbl_fb_info_t           fb_info;

} awbl_imx1050_fb_t;


/**
 * \brief iMX RT1050 ֡��������ע��
 */
void awbl_imx1050_fb_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_IMX1050_FB_H */

/* end of file */

