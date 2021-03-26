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
 * \brief RTK ƽ̨��ر�׼����ͷ�ļ�
 *
 * \par ˵��
 *  ����������aworks1.0��ģ��(*.<b>c</b>�ļ�)�У����ļ����뱻ֱ�ӻ��ӵذ�����
 *
 * \par ��ͷ�ļ���Ҫ����ı�׼�����У�\n
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-05  orz, move some defines to aw_common.h
 * - 1.00 12-08-27  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_COMMON_H
#define __AW_PSP_COMMON_H

#include "rtk.h"
#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#define AW_PLATFORM        "aworks1.0"

/** CPUĬ��ʹ��ʹ��С�ˣ�����bsp�и������������ */
#ifndef AW_CPU_ENDIAN
#define AW_CPU_ENDIAN   AW_LITTLE_ENDIAN
#endif

/******************************************************************************/
#if defined(AW_IMG_PRJ_BUILD) || defined(AW_BSP_LIB_BUILD)
#include "aw_bsp_common.h"
#endif
    
#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_COMMON_H    */

/* end of file */
