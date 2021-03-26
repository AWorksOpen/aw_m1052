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
 * \file  rtk_sem_internal.h
 * \brief 定义rtk 信号量相关的内部操作函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_SEM_INTERNAL_H__
#define         __RTK_SEM_INTERNAL_H__

/**
 * \brief rtk 计数信号量全局初始化
 *
 * \param[in]   无
 * \retval      无
 */
void __rtk_semc_global_init(void);

/**
 * \brief rtk 二进制信号量全局初始化
 *
 * \param[in]   无
 * \retval      无
 */
void __rtk_semb_global_init(void);



#endif          /* __RTK_SEM_INTERNAL_H__*/
