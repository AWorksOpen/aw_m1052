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
 * \brief Apollo ��ʱ��׼�ӿ�ƽ̨֧���ļ�
 *
 * \internal
 * \par modification history:
 * - 1.00 13-06-03  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_DELAY_H
#define __AW_PSP_DELAY_H

/******************************************************************************/
/* �����˲���, ʹ�� aw_udelay(x) ��ʱʱ��Ϊ x ΢�� */
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
