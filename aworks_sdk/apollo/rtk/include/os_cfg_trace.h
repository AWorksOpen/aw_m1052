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
 * \brief OS Trace �������ļ�
 */

#ifndef   __OS_CFG_TRACE_H
#define   __OS_CFG_TRACE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief  �򿪻��߹رո� Trace ��ص�ȫ��/���ֺꡣ
 *
 *  ����Ϊ 1 �򿪣�Ϊ��رա�
 *
 */
#ifndef OS_CFG_TRACE_EN
#define OS_CFG_TRACE_EN                 1
#endif

#if (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))
/**
 * \brief �Ƿ����� Trace_Start/Trace_Stop ���ơ�
 *
 *  ����Ϊ 1 �򿪣�Ϊ��رա�
 *
 *  ������ rtk_init() ��ʼ��¼�¼��������Զ��忪ʼ��¼��λ�ã�
 *  ���Դ�����꣬ʹ�� Trace_Start/Trace_Stop ���á�
 */
#ifndef OS_CFG_TRACE_CONTROL_EN
#define OS_CFG_TRACE_CONTROL_EN         1
#endif

/**
 * \brief �Ƿ���Ӳ��� API �Ľ��롣
 *
 *  ����Ϊ 1 �򿪣�Ϊ��رա�
 */
#ifndef OS_CFG_TRACE_API_ENTER_EN
#define OS_CFG_TRACE_API_ENTER_EN       1
#endif

/**
 * \brief �Ƿ���Ӳ��� API ���˳���
 *
 *  ����Ϊ 1 �򿪣�Ϊ��رա�
 */
#ifndef OS_CFG_TRACE_API_EXIT_EN
#define OS_CFG_TRACE_API_EXIT_EN        1
#endif

#endif  /* (defined(OS_CFG_TRACE_EN) && (OS_CFG_TRACE_EN > 0u))*/


#ifdef __cplusplus
}
#endif

#endif /* __OS_CFG_TRACE_H*/

/* End of file  */
