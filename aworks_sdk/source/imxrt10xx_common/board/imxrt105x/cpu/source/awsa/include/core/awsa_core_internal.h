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

#ifndef __AWSA_CORE_INTERNAL_H
#define __AWSA_CORE_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief AWorks platform */
#include "aw_common.h"
#include "aw_vdebug.h"
#include "aw_mem.h"
#include "aw_assert.h"
#include "aw_list.h"
#include "aw_input.h"
#include "aw_input_code.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "aw_time.h"
#include "aw_task.h"
#include "aw_io.h"
#include "aw_ioctl_generic.h"

/** \brief libc */
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>

/** \brief top interface */
#include "awsa_pcm.h"
#include "awsa_mixer.h"

/**
 * \brief find last (most-significant) bit set
 * \note This is defined the same way as ffs.
 *       Such as fls(0) = 0, fls(1) = 1, fls(0x80000000) = 32.
 * \param[in] x: the word to search
 * \return the most-significant bit
 */
aw_static_inline
int awsa_core_fls (int x)
{
    int r = 32;

    if (!x) {
        return 0;
    }

    if (!(x & 0xffff0000u)) {
        x <<= 16;
        r -= 16;
    }

    if (!(x & 0xff000000u)) {
        x <<= 8;
        r -= 8;
    }

    if (!(x & 0xf0000000u)) {
        x <<= 4;
        r -= 4;
    }

    if (!(x & 0xc0000000u)) {
        x <<= 2;
        r -= 2;
    }

    if (!(x & 0x80000000u)) {
        x <<= 1;
        r -= 1;
    }

    return r;
}

#ifndef AWSA_CORE_ARRAY_SIZE
#define AWSA_CORE_ARRAY_SIZE(ar) (sizeof (ar) / sizeof ((ar) [0]))
#endif

/** \brief core middle layer */
#include "core/awsa_core.h"
#include "core/awsa_core_jack.h"
#include "core/awsa_core_pcm.h"
#include "core/awsa_core_pcm_lib.h"
#include "core/awsa_core_ctl.h"
#include "core/awsa_core_tlv.h"
#include "core/awsa_core_debug.h"

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_INTERNAL_H */

/* end of file */
