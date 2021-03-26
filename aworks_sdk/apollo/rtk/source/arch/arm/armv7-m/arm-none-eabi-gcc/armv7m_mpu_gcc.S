/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file armv7m_mpu_gcc.S
 * \brief rtk中armv7m架构MPU相关函数的arm-none-eabi-gcc汇编实现
 *
 * \par 源代码
 * \internal
 * \par Modification history
 * - 1.00 2018-06-13 sni, first version.
 * \endinternal
 */

#include "rtk_config.h"
#include "armv7m_gcc_common.h"

    .set MPU_CTL_ADDR               ,0xe000ed94
    .set __BIT_MPU_CTL_ENABLE       , (1 << 0) /* enable */

/*******************************************************************************
   global functions
*******************************************************************************/
ARM_FUNC_BEGIN(arch_mpu_disable)
    push    {r0-r12,lr}

    bl      armv7m_dcache_disable

    ldr     r1,=MPU_CTL_ADDR;
    mov     r0,#0
    str     r0,[r1];
    dsb

    pop     {r0-r12,lr}
    bx      lr
ARM_FUNC_END()

ARM_FUNC_BEGIN(__armv7m_mpu_enable)
    push    {r0-r12,lr}

    bl      armv7m_dcache_clean_all_no_context_save
    bl      armv7m_dcache_invalidate_all_no_context_save

    ldr     r1,=MPU_CTL_ADDR;
    mov     r0,#1
    str     r0,[r1];
    dsb

    pop     {r0-r12,lr}
    bx      lr
ARM_FUNC_END()

    .end

/* end of file */
