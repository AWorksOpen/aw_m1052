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
 * - 1.00 12-08-23  zen, first implementation
 * \endinternal 
 */

#ifndef __AWBL_LPC11XX_GPIO_USRCFG_H
#define __AWBL_LPC11XX_GPIO_USRCFG_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_lpc11xx_gpio_usrcfg
 * @{
 */
 
/** 
 * \name LPC11XX GPIO ģ������
 * @{
 */

/**
 * \brief ���ô�������Ϣ����
 *
 * ����LPC11XX��Դ���ޣ�Ϊ�˱��ⲻ��Ҫ���ڴ����ģ��û����Խ��˲�������Ϊʵ����Ҫ
 * ���Ӵ����������ŵĸ�����
 * ���磬ֻ��3�����Żᱻ���Ӵ�����������Խ��˲��� #AWBL_LPC11XX_GPIO_TRIGINFO_COUNT
 * ����Ϊ3��
 *
 * \attention LCP11XX ��42��GPIO���ţ���ˣ��˲��������ЧֵΪ42
 */
#define AWBL_LPC11XX_GPIO_TRIGINFO_COUNT     3

/** @} */
/** @} grp_lpc11xx_gpio_usrcfg */

#ifdef __cplusplus
}
#endif	/* __cplusplus 	*/

#endif	/* __AWBL_LPC11XX_GPIO_USRCFG_H */

/* end of file */

