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
 * \brief contains the global variable daylight.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-09-26 orz, first implementation.
 * \endinternal
 */

#ifndef __RTT_STATFS_H__
#define __RTT_STATFS_H__

#include <rtthread.h>

struct statfs
{
    /** \brief block size */
    rt_size_t f_bsize;      
    /** \brief total data blocks in file system */
    rt_size_t f_blocks;  
    /** \brief free blocks in file system */
    rt_size_t f_bfree;     
};

#endif
