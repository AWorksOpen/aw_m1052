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
 * \brief 
 *
 * \internal
 * \par modification history:
 * - 0.9 18-04-23 sni first version.
 * \endinternal
 */

#ifndef __ARMV4_GCC_COMMON_H__
#define __ARMV4_GCC_COMMON_H__

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
