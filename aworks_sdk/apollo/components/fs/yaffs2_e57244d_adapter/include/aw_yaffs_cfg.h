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

/* �Ƿ��yaffs������Ϣ */
//#define AW_YAFFS_DEBUG_MSG

/** \brief ʹ��yaffs2 */
#define CONFIG_YAFFS_YAFFS2

/** \brief ʹ��yaffs������������� */
#define CONFIG_YAFFS_DEFINES_TYPES

/** \brief ��yaffs�ṩ�ļ����Ͷ��弰���Ա�ʶ */
#define CONFIG_YAFFS_PROVIDE_DEFS

/** \brief ʹ��yaffs direct interface */
#define CONFIG_YAFFS_DIRECT


#define YCHAR  char
#define YUCHAR unsigned char

/*
 * \brief ����kmalloc֮����ڴ���������ı������,
 * Ŀ��Ϊ�˼��ٶ�yaffsԴ����޸� 
 */
#define GFP_NOFS 0


/** \brief ���䶨�� */
#ifndef S_IREAD
#define S_IREAD     0000400
#endif

/** \brief ���䶨�� */
#ifndef S_IWRITE
#define S_IWRITE    0000200
#endif


/** \brief ��Ҫ�����ļ�ָʾ�ļ���С,�ڲ�ƫ�Ƶ��������� */
typedef uint64_t loff_t;


#endif /* __AW_YAFFS_CFG_H */

/* end of file */
