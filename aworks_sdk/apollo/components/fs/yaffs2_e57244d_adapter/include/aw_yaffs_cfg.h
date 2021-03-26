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
 * \brief YAFFS config header file
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-04-25  cyl, first implementation.
 * \endinternal
 */

#ifndef __AW_YAFFS_CFG_H
#define __AW_YAFFS_CFG_H

#include "apollo.h"
#include "io/aw_fcntl.h"
#include "io/sys/aw_stat.h"


/******************************************************************************/

/* 是否打开yaffs调试信息 */
//#define AW_YAFFS_DEBUG_MSG

/** \brief 使用yaffs2 */
#define CONFIG_YAFFS_YAFFS2

/** \brief 使用yaffs定义的数据类型 */
#define CONFIG_YAFFS_DEFINES_TYPES

/** \brief 由yaffs提供文件类型定义及属性标识 */
#define CONFIG_YAFFS_PROVIDE_DEFS

/** \brief 使用yaffs direct interface */
#define CONFIG_YAFFS_DIRECT


#define YCHAR  char
#define YUCHAR unsigned char

/*
 * \brief 避免kmalloc之类的内存申请操作的编译错误,
 * 目的为了减少对yaffs源码的修改 
 */
#define GFP_NOFS 0


/** \brief 补充定义 */
#ifndef S_IREAD
#define S_IREAD     0000400
#endif

/** \brief 补充定义 */
#ifndef S_IWRITE
#define S_IWRITE    0000200
#endif


/** \brief 主要用于文件指示文件大小,内部偏移的数据类型 */
typedef uint64_t loff_t;


#endif /* __AW_YAFFS_CFG_H */

/* end of file */
