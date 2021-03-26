/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx GPT CAP����
 *
 * ������ʵ���˶�ʱ���Ĳ����ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-09  licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_GPT_CAP_H
#define __AWBL_IMX10xx_GPT_CAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "awbl_cap.h"
#include "driver/timer/imx10xx_gpt_regs.h"

#define AWBL_IMX10XX_GPT_CAP_NAME   "imx10xx_gpt_cap"

/** \brief CAP ͨ��ѡ���б� */
typedef enum gpt_cap_channel_sel
{
    GPT_CAP_Channel_0 = 0,    /**< \brief GPT CAP Channel 0 */
    GPT_CAP_Channel_1,        /**< \brief GPT CAP Channel 1 */
    GPT_CAP_Channel_all,
} gpt_cap_channel_sel_t;

/** \brief iMX RT10xx GPT CAP �豸���� */
typedef struct awbl_imx10xx_gpt_cap_devcfg {

    uint8_t                 pid;        /**< \brief CAP ID */
    gpt_cap_channel_sel_t   ch;         /**< \brief CAP��Ӧ��ͨ�� */
    int                     gpio;       /**< \brief CAP��Ӧ������ */
    uint32_t                func;       /**< \brief CAP���Ź��� */
} awbl_imx10xx_gpt_cap_devcfg_t;

/** \brief �豸������Ϣ */
typedef struct awbl_imx10xx_gpt_cap_devinfo {
    uint32_t                    regbase;        /**< \brief �Ĵ�������ַ */
    const aw_clk_id_t           clk_id;         /**< \brief ʱ�� ID */
    int                         inum;           /**< \brief �жϺ� */

    /** \brief CAP ������Ϣ */
    aw_const struct awbl_imx10xx_gpt_cap_devcfg *cap_devcfg;
    uint8_t                     pnum;           /**< \brief ʹ�õ���CAPͨ����*/

    struct awbl_cap_servinfo    cap_servinfo;

    /** \brief ָ��ƽ̨��ʼ��������ָ�� */
    void                        (*pfn_plfm_init) (void);
} awbl_imx10xx_gpt_cap_devinfo_t;

/** \brief �豸ʵ�� */
typedef struct awbl_imx10xx_gpt_cap_dev {
    awbl_dev_t                  dev;            /**< \brief AWBus �豸ʵ���ṹ�� */
    struct awbl_cap_service     cap_serv;       /**< \brief ÿ���豸�ṩһ��CAP������ϲ� */

    struct {
        cap_callback_t          callback_func;  /**< \brief callback function  */
        void                   *p_arg;          /**< \brief The parameter for callback function */
    } callback_info[2];                         /**< \brief Most support 2 channel */
} awbl_imx10xx_gpt_cap_dev_t;

/**
 * \brief �� IMX10xx GPT CAP ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_gpt_cap_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_GPT_CAP_H */

/* end of file */
