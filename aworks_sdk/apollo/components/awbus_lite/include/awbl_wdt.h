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
 * \brief AWBus WATCHDOG �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_wdt.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 16-01-12  may, add awbl_hwwdt_feed()
 * - 1.00 14-08-27  may, first implementation
 * \endinternal
 */

#ifndef __AWBL_WDT_H
#define __AWBL_WDT_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_wdt.h"


/**
 * \brief AWBus watchdog��Ϣ
 */
struct awbl_hwwdt_info {

    /** \brief ι������*/
    void  (*feed)(struct awbl_hwwdt_info *pinfo);

    /** \brief Ӳ�����Ź�ʱ�� */
    uint32_t  wdt_time;
};

/**
 * \brief AWBus watchdog �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void   awbl_wdt_init (void);

/**
 * \brief AWBus watchdog ιӲ�����Ź��ӿ�
 *
 * \attention ������Ӧ���� awbl_wdt_init() ֮�����
 */
aw_err_t awbl_hwwdt_feed (void);

/**
 * \brief AWBus ע��Ӳ�����Ź������ڻ�ȡ�����ײ�Ӳ�����Ź�����Ϣ
 */
void  awbl_wdt_device_register (struct awbl_hwwdt_info  *pwdt_info);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */



#endif /* __AWBL_WDT_H */

/* end of file */

