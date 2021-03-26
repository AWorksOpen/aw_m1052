/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 17-09-23  win, first implementation
 * \endinternal
 */

#include "core/awsa_core_internal.h"
#include "soc/awsa_soc_internal.h"

void awsa_cfg_init (void)
{
    awsa_core_init();
    awsa_soc_init();
}

/* end of file */
