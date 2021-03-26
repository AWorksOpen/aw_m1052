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
 * \brief imx10xx lpi2c driver  header file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-18  like, first implementation
 * \endinternal
 */

#ifndef AWBL_IMX10XX_LPI2C_H
#define AWBL_IMX10XX_LPI2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_i2cbus.h"
#include "aw_i2c.h"
#include "aw_sem.h"
#include "aw_cpu_clk.h"

#define AWBL_IMX10XX_LPI2C_NAME               "imx10xx_lpi2c"

/**
 * \brief imx10xx LPI2C �豸��Ϣ
 */
struct awbl_imx10xx_lpi2c_devinfo {

    /** \brief �̳��� AWBus LPI2C �������豸��Ϣ */
    struct awbl_i2c_master_devinfo lpi2c_master_devinfo;

    uint32_t    sda_filter_wide;        /**< SDA�˲����  (ns) */
    uint32_t    scl_filter_wide;        /**< SCL�˲����  (ns) */
    uint32_t    bus_idle_timeout;       /**< ���߿���ʱ�� */

    uint32_t    regbase;                /**< �Ĵ�������ַ */
    int         inum;                   /**< �жϺ� */
    aw_clk_id_t ref_clk_id;             /**< ����ʱ��ID */
    void (*pfunc_plfm_init)(void);      /**< ƽ̨��س�ʼ��������ʱ�ӡ���ʼ������ */
    void (*pfunc_plfm_rst)(void);       /**< i2c�������� */
};

/**
 * \brief imx10xx I2C �豸ʵ��
 */
struct awbl_imx10xx_lpi2c_dev {

    /** \brief �̳��� AWBus I2C ���������� */
    struct awbl_i2c_master  super;

    /** \brief �������ݼ��� */
    size_t      data_ptr;

    /** \brief ����ͬ������������ź��� */
    AW_SEMB_DECL(i2c_sem);

    /** \brief ֹͣ�źſ��Ʊ��  */
    uint32_t    stop_ctrl_flg;

    /** \brief  ��ǰ trans ����ֵ */
    uint32_t    cur_trans_index;

    /** \brief imxrt1050 lpi2c ������Ϣ�ṹ����� */
    struct aw_i2c_config i2c_cfg_par;

    /* ���ݸ��жϵ�дbuffer����  */
    struct awbl_trans_buf  *p_trans_buf;

    /* ���ݸ��жϵ�дbuffer���� */
    uint32_t                trans_buf_num;
	
    /* �������  */
    uint32_t err_count;
};

/**
 * \brief imx10xx LPI2C master driver register
 */
void awbl_imx10xx_lpi2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* AWBL_IMX10XX_LPI2C_H */
