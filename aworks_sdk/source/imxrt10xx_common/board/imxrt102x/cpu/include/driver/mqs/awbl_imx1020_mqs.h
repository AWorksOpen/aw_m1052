/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief The IMX1020 Medium Quality Sound driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-12-07, hsg, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX1020_MQS_H
#define __AWBL_IMX1020_MQS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "soc/awsa_soc_internal.h"


#define AWBL_IMX1020_MQS_NAME   "awbl_imx1020_mqs"

/**
 * \brief imx1020 MQS �豸��Ϣ
 */
struct awbl_imx1020_mqs_devinfo {

    uint32_t        iomux_grp2_regaddr;   /**< \brief IOMUX_GRP2 �Ĵ�����ַ                         */
    aw_clk_id_t     ref_clk_id;           /**< \brief refence clock id*/
    void (*pfn_plfm_init)(void);          /**< \brief ƽ̨��س�ʼ�� */
    int             card_id;              /**< \brief ����ID */
};

/**
 * \brief imx1020 MQS �豸ʵ��
 */
struct awbl_imx1020_mqs_dev {
    struct awbl_dev  dev;
    const struct awbl_imx1020_mqs_devinfo *p_devinfo;
    struct awsa_soc_codec                  codec;
    struct awsa_soc_dai                    dai[3];
    struct awsa_soc_card                   soc_card;
    struct awsa_soc_pcm                    soc_pcm;
};


/**
 * \brief ʹ��MQS
 *
 * \return AW_OK
 */
aw_err_t awbl_imx1020_mqs_enable (void);


/**
 * \brief ����MQS
 *
 * \return AW_OK
 */
aw_err_t awbl_imx1020_mqs_disable (void);


/**
 * \brief imx1020 MQS ����ע��
 */
void awbl_imx1020_mqs_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */

