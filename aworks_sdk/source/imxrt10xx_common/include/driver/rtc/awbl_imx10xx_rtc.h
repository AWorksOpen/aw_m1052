/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief imx10xx SNVS RTC����
 *
 * \internal
 * \par modification history:
 * - 1.00 17-08-30  mex, first implementation
 * \endinternal
 */

#ifndef AWBL_IMX10xx_RTC_H
#define AWBL_IMX10xx_RTC_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_rtc.h"
#include "aw_sem.h"

/** \brief ������ */
#define AWBL_IMX10XX_RTC_NAME    "imx10xx_rtc"

/**
 * \brief imx10xx RTC �豸��Ϣ
 */
struct awbl_imx10xx_rtc_devinfo {

    /** \brief RTC ����������Ϣ */
    struct awbl_rtc_servinfo  rtc_servinfo;

    uint32_t                  regbase;  /**< �Ĵ�������ַ */
    void (*pfunc_plfm_init)(void);      /**< ƽ̨��س�ʼ��������ʱ�� */
};


/**
 * \brief imx10xx RTC �豸ʵ��
 */
struct awbl_imx10xx_rtc_dev {

    /** \brief �����豸 */
    struct awbl_dev super;

    /** \brief �ṩ RTC ���� */
    struct awbl_rtc_service rtc_serv;

    /** \brief �豸�� */
    AW_MUTEX_DECL(dev_lock);
};

/**
 * \brief imx10xx RTC driver register
 */
void awbl_imx10xx_rtc_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* AWBL_IMX10xx_RTC_H */

/* end of file */
