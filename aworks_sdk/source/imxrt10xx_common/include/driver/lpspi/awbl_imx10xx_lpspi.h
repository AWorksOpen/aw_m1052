/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10XX LPSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-17  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10XX_LPSPI_H
#define __AWBL_IMX10XX_LPSPI_H

#include "awbl_spibus.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_cpu_clk.h"


#define AWBL_IMX10XX_LPSPI_NAME   "imx10xx_lpspi"


/**
 * \brief iMX RT10XX LPSPI �豸��Ϣ
 */
typedef struct awbl_imx10xx_lpspi_devinfo {

    /** \brief �̳��� AWBus SPI �������豸��Ϣ */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t         regbase;            /**< \brief �Ĵ�������ַ */
    int              inum;               /**< \brief �жϺ� */
    aw_clk_id_t      ref_clk_id;         /**< \brief the clock which the counter use */
    aw_bool_t        hard_cs;            /**< \brief Ӳ��CSģʽ */
    aw_bool_t        cs_active_H;        /**< \brief Ӳ��CS�ߵ�ƽ��Ч */
    uint8_t          cs_sck;             /**< \brief cs��Ч����һ��sck edge��delay cycle */
    uint8_t          sck_cs;             /**< \brief ���һ��sck edge��cs��Ч��delay cycle */
    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

} awbl_imx10xx_lpspi_devinfo_t;

/**
 * \brief iMX RT10XX  SPI �豸ʵ��
 */
typedef struct awbl_imx10xx_lpspi_dev {

    /** \brief �̳��� AWBus SPI ���������� */
    struct awbl_spi_master  super;

    uint32_t    write_index;
    uint32_t    read_nbytes;
    uint32_t    nbytes_to_recv;   /**< /brief �����յ��ֽ��� */
    const void  *p_write_buf;
    void        *p_read_buf;
    void       (*bpw_func) (void* p_arg);
    uint32_t    cur_speed;
    uint8_t     cur_cs_sck_t;
    uint8_t     cur_bpw;
    aw_bool_t   continuous_flg;

    /** \brief ����ͬ������������ź��� */
    AW_SEMB_DECL(lpspi_sem_sync);

} awbl_imx10xx_lpspi_dev_t;


/**
 * \brief iMX RT10XX LPSPI master driver register
 */
void awbl_imx10xx_lpspi_drv_register (void);


#endif /* __AWBL_IMX10XX_LPSPI_H */
