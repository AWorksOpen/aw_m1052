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
 * \brief 通用数据类型相关定义头文件
 *
 * \internal
 * \par modification history:
 * - 1.10 14-09-28 	zen, move some defines to aw_psp_types.h
 * - 1.00 13-01-15  orz, move defines from aw_common.h
 * \endinternal
 */

#ifndef __AW_TYPES_H
#define __AW_TYPES_H

#include "aw_psp_types.h"     /* 平台相关标准定义头文件 */

#include "aw_errno.h"   /* for aw_err_t */

/******************************************************************************/
/* bool type define */

typedef int             aw_bool_t;
#define AW_TRUE         (1)
#define AW_FALSE        (0)

/******************************************************************************/
typedef unsigned int    uint_t;     /**< \brief unsigned int type for short */
typedef unsigned char   uchar_t;    /**< \brief unsigned char type for short */
typedef unsigned long   ulong_t;    /**< \brief unsigned long type for short */
typedef unsigned short  ushort_t;   /**< \brief unsigned short type for short */

/******************************************************************************/
typedef aw_psp_tick_t  aw_tick_t;

/******************************************************************************/

/* function pointer type defines */

/** \brief  void (*pfunc) (void *) */
typedef void (*aw_pfuncvoid_t) (void *);

/** \brief  void *(*pfunc) (void *) */
typedef void *(*aw_pfunc_t) (void *);

#endif    /* __AW_TYPES_H    */

/* end of file */
