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
 * \brief AWBus PLB���߽ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_plb.h"
 * \endcode
 * ��ģ��Ϊ PLB ���ߵľ����ʵ��
 *
 * �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-21  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_PLB_H
#define __AWBL_PLB_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \biref PLB bus ����ע����Ϣ�ṹ��
 *
 * \see struct awbl_drvinfo
 */
typedef struct awbl_plb_drvinfo{
    struct awbl_drvinfo super;
} awbl_plb_drvinfo_t;

/**
 * \brief AWBus PLB ����ע�ắ��
 */
int awbl_plb_register(void);

/**
 * \brief AWBus PLB ���߳�ʼ����һ�׶�
 *
 * ��һ�׶Σ�AWBus ��ʵ�����в��� PLB �����豸����ִ��ʵ���ĵ�һ�׶γ�ʼ��������
 * �����ڴ˽׶���ֻ����Ĭ�豸�Ĳ�����
 */
int awbl_plb_init1(struct awbl_dev *p_plbdev);

#ifdef __cplusplus
}
#endif  /* __cplusplus 	*/

#endif  /* __AWBL_PLB_H */

/* end of file */
