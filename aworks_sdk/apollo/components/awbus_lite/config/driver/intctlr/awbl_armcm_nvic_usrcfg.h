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
 * \brief �ⲿʹ�õ������ļ�
 *
 * �û��ڴ��ļ��ж�ģ�����ز�����������
 *
 * \internal
 * \par modification history:
 * - 1.00 12-08-23  zen, first implemetation
 * \endinternal 
 */

#ifndef __AWBL_ARMCM_NVIC_USRCFG_H
#define __AWBL_ARMCM_NVIC_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \addtogroup grp_armcm_nvic_usrcfg
 * @{
 */
 
/** 
 * \name ARM Cortex-M NVIC ģ������
 * @{
 */

/**
 * \brief ���������ж���Ϣ����
 *
 * ����CPU��Դ�������ޣ�Ϊ�˱��ⲻ��Ҫ��RAM���ģ��û����Խ��˲�������Ϊʵ��
 * ��Ҫ���ж���Ŀ��
 */
#define AW_CFG_ARMCM_NVIC_ISRINFO_COUNT     10

/** @} */
/** @} grp_armcm_nvic_usrcfg */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_ARMCM_NVIC_USRCFG_H */

/* end of file */

