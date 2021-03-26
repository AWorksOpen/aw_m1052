/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx SDIO driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-13  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_SDIO_H
#define __AWBL_IMX10xx_SDIO_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_sdiobus.h"
#include "aw_clk.h"

#define AWBL_IMX10XX_SDIO_NAME               "imx10xx_sdio"
#define __SDIO_SPI_TRANS_TASK_STACK_SIZE     4096

/**
 * \brief imx10xx sdio �豸ʵ��
 */
typedef struct awbl_imx10xx_sdio_dev {
    /** \brief �̳��� AWBus SDIO ���������� */
    awbl_sdio_host_t        host;
    AW_MUTEX_DECL(dev_mux);
    AW_TASK_DECL(imx10xx_sdio_trans_task,
                 __SDIO_SPI_TRANS_TASK_STACK_SIZE);
    AW_SEMB_DECL(sem_sync);
    awbl_sdio_msg_t        *p_cur_msg;
    uint8_t                *p_dma_buf;
    int                     dma_buf_len;
    volatile uint32_t       sd_status;          /**< SDMMC����״̬*/
    AW_SEMB_DECL(cc_sem);                       /**< MMC�ź���ʵ�� */
    aw_semb_id_t            cc_sem_id;          /**< MMC�ź���ID  */
    AW_SEMB_DECL(tc_sem);                       /**< MMC�ź���ʵ��*/
    aw_semb_id_t            tc_sem_id;          /**< MMC�ź���ID  */

    /** \brief ������״̬ */
    uint8_t                 cd_state;

    void (*sdio_card_int_enable)(void *);    /**< \brief SDIO���ж�ʹ�� */
    void (*sdio_card_int_unmask)(void *);    /**< \brief SDIO���ж�״̬ȡ������ */

} awbl_imx10xx_sdio_dev_t;

/* sdio host �豸��Ϣ  */
typedef struct awbl_imx10xx_sdio_dev_info {
    awbl_sdio_host_info_t   host_info;
    uint32_t                regbase;
    aw_clk_id_t             ref_clk_id;         /**< ����ʱ�ӵ�ID */
    uint32_t                task_trans_prio;    /**< �����������ȼ� */

    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);
    int                     inum;                /**< SDMMC�жϺ�      */

    /** \brief ��ȡ���Ƿ���� */
    aw_bool_t               (*pfn_cd) (awbl_imx10xx_sdio_dev_t *p_dev);

    /** \brief ���ж�(Card interrupt)�ص����� */
    void (*card_int_callback)(void);

} awbl_imx10xx_sdio_dev_info_t;


/**
 * \brief iMX RT10xx SDIO ����ע��
 */
void awbl_imx10xx_sdio_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_IMX10xx_SDIO_H */
