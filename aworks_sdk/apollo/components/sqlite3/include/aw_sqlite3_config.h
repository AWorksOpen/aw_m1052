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
 * \brief Sqlite3's vfs implementation.(AWorks platform)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-01-18  cyl, first implementation.
 * \endinternal
 */

#ifndef __AW_SQLITE3_CONFIG_H
#define __AW_SQLITE3_CONFIG_H

/** \brief ʹ�÷�windowsƽ̨��unix-likeƽ̨ʱ��Ҫ���ú궨��Ϊ1 */
#define SQLITE_OS_OTHER         1

/** \brief ��aw_sqlite3_io.c�ļ��еĴ�����������Ҫ����ú� */
#define SQLITE_OS_AWORKS

/** \brief ��aw_sqlite3_os.c�ļ��е�������ӿ���ش�����������Ҫ����ú� */
#define SQLITE_MUTEX_AWORKS

/** \brief ��aw_sqlite3_os.c�ļ��е��ڴ����ӿ���ش�����������Ҫ����ú� */
#define SQLITE_AWORKS_MALLOC

/**
 * \brief ����Ҫ��AWorks��OS�ӿڲ㺯���ĵ��ý��е��ԣ�
 *        ����Ҫʹ��SQLITE_AWORKS_DEBUG�궨��
 */
#if 0
#  define SQLITE_AWORKS_DEBUG
#endif

/* #define SQLITE_DEBUG 1*/

#endif /* __AW_SQLITE3_CONFIG_H */

/* end of file */
