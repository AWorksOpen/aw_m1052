/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Apollo 延时标准接口平台支持文件
 *
 * \internal
 * \par modification history:
 * - 1.00 13-06-03  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_DELAY_H
#define __AW_PSP_DELAY_H

/******************************************************************************/
/* 调整此参数, 使得 aw_udelay(x) 延时时间为 x 微秒 */
#define  __UDELAY_FACTOR   26

/******************************************************************************/
static aw_inline void aw_udelay(uint32_t us)
{
    while (us--) {
        volatile uint32_t i = __UDELAY_FACTOR;
        while (i--);
    }
}

#endif      /* __AW_PSP_DELAY_H */

/* end of file */
