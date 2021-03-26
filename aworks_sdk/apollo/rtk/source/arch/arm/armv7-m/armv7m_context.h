/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https:/*www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ARMv7-M �߳�������
 *
 * ��ģ��Ϊ(ARMv7-M�ܹ�)���쳣ģ��ʵ�֡�
 *
 * \par ����
 *
 * - armv7-m �߳���������صĶ���
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-08  zen, first implementation
 * \endinternal
 */

#ifndef __ARMV7M_CONTEXT_H
#define __ARMV7M_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "rtk_config.h"

/* \brief ȷ��������뻷���Ƿ�ʹ���˸����� */
#if (  ((defined( __CC_ARM ) || defined(__ARMCOMPILER_VERSION)) && defined ( __TARGET_FPU_VFP )) \
    || (defined ( __ICCARM__ ) && defined ( __ARMVFP__ )) \
    || (defined ( __GNUC__ ) && defined ( __VFP_FP__ ) && !defined(__SOFTFP__)) )
#define __RTK_ARMV7M_USE_FPU    1
#else
#define __RTK_ARMV7M_USE_FPU    0
#endif

/* armv7m �������б�־�Ķ��� */
#define ARMV7M_CONTEXT_FLAGS_FPU_ENABLE     (1 << 0)
#define ARMV7M_CONTEXT_FLAGS_PRIVILEGED     (1 << 1)

#if __RTK_ARMV7M_USE_FPU || CONFIG_RTK_USERMODE
#define __RTK_ARMV7M_CONTEXT_USE_FLAGS      1
#else
#define __RTK_ARMV7M_CONTEXT_USE_FLAGS      0
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __ARMV7M_EXC_H */

/* end of file */
