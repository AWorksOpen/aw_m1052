/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-08  zen, first implementation
 * \endinternal
 */

#ifndef __ARMV8M_GCC_COMMON_H__
#define __ARMV8M_GCC_COMMON_H__

    .syntax unified
    .text

#define ARM_LOCAL_FUNC_BEGIN(name)  .section .text.name; \
                                        .type name, %function; \
                                        .func; \
                                         name: \

#define ARM_FUNC_BEGIN(name)        .global name ; \
                                        ARM_LOCAL_FUNC_BEGIN(name)

#define ARM_FUNC_END()              .endfunc;.text

#endif
