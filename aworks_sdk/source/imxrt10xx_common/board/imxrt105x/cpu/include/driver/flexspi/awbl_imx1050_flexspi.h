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
 * \brief iMX RT1050 FlexSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-26  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX1050_FLEXSPI_H
#define __AWBL_IMX1050_FLEXSPI_H

#include "aw_sem.h"
#include "aw_cpu_clk.h"
#include "imx1050_inum.h"
#include "imx1050_reg_base.h"
#include "awbl_nvram.h"
#include "driver/flexspi/awbl_imx1050_flexspi_op.h"
#include "driver/flexspi/imx1050_flexspi_regs.h"
#include "driver/flexspi/awbl_imx1050_flexspi_service.h"
#include "driver/flexspi/awbl_imx1050_flexspi_nvram.h"

#define AWBL_IMX1050_FLEXSPI_NAME   "imx1050_flexspi"


#define FLEXSPI_PORT_A1           0        /**< \brief Access flash on A1 port. */
#define FLEXSPI_PORT_A2           1        /**< \brief Access flash on A2 port. */
#define FLEXSPI_PORT_B1           2        /**< \brief Access flash on B1 port. */
#define FLEXSPI_PORT_B2           3        /**< \brief Access flash on B2 port. */
#define FLASH_STATUS_REG_QE_MASK  (1 << 6) /**< \brief Flash Quad Enable bit mask */


/* FlexSPI AHB��дӳ�����ַ */
#define FLEXSPI_AMBA_BASE         (0x60000000UL)


/**
 * \brief �ⲿ�����豸���ò���
 */
typedef struct __flexspi_device_config {

    uint32_t flash_size;           /**< \brief Flash�Ĵ�С����λ(KByte)*/
    uint32_t sector_size;          /**< \brief Flash������С ��λ(KByte)*/
    uint32_t page_size;            /**< \brief Flashҳ��С ��λ(Byte)*/
    uint32_t block_size;           /**< \brief Flash���С ��λ(Byte)*/
    uint16_t cs_interval;          /**< \brief CSѡ�е��ͷŵļ��ʱ��,ʱ��Ϊ��ֵ���Դ���ʱ������ */
    uint8_t cs_hold_time;          /**< \brief CS�ı���ʱ�� */
    uint8_t cs_setup_time;         /**< \brief CS�Ľ���ʱ�� */
    uint8_t columnspace;           /**< \brief �е�ַλ�� */
    uint8_t data_valid_time;       /**< \brief ���ݵ���Чʱ�� */
    flexspi_ahb_write_wait_unit_t ahb_write_wait_unit;    /**< \brief AHBд�ȴ����ʱ�䵥Ԫ*/
    uint16_t ahb_write_wait_interval;   /**< \brief AHBд�ȴ�ʱ����  */

} flexspi_device_config_t;


/**
 * \brief Transfer structure for FLEXSPI.
 */
typedef struct __flexspi_transfer {

    uint32_t  device_address;         /**< \brief Operation device address */
    uint8_t   port;                   /**< \brief Operation port */
    flexspi_command_type_t cmd_type;  /**< \brief Execution command type */
    uint8_t   seq_index;              /**< \brief Sequence ID for command  */
    uint8_t   seq_num;                /**< \brief Sequence number for command */
    uint8_t  *p_buf;                  /**< \brief Data buffer */
    size_t    data_size;              /**< \brief Data size in bytes */
} __flexspi_transfer_t;

/**
 * \brief iMX RT1050 Flexspi �豸��Ϣ
 */
typedef struct awbl_imx1050_flexspi_devinfo {

    uint32_t         regbase;              /**< \brief �Ĵ�������ַ */
    int              inum;                 /**< \brief �жϺ� */
    uint8_t          port;                 /**< \brief FlashоƬ���ӵĶ˿� */
    flexspi_device_config_t  dev_cfg;      /**< \brief �ⲿFlash�豸�Ĳ������� */

    uint8_t          addr_width;           /**< \brief ��ַ��ȣ�0x18��ʾ24λ��0x20��ʾ32λ */

    awbl_flexspi_read_mode_t read_mode;    /**< \brief ��ģʽ */
    uint8_t          read_opcode;          /**< \brief �������� */
    uint8_t          program_opcode;       /**< \brief ��̲����� */
    uint8_t          erase_opcode;         /**< \brief ���������� */
    uint8_t          read_dummy;           /**< \brief ������ȴ����� */

    /** \brief Flexspi nor flash NVRAM��Ϣ */
    struct awbl_flexspi_nor_flash_nvram_info nvram_info;

    void             *p_mtd;               /**< \brief MTD������   */
    const void       *p_info;              /**< \brief MTD������Ϣ   */

    aw_clk_id_t      ref_clk_id;           /**< \brief the clock which the counter use */
    /** \brief ƽ̨��س�ʼ��������ʱ�ӡ���ʼ�����ŵȲ��� */
    void (*pfunc_plfm_init)(void);

    uint8_t          bp_flag;              /**< \brief Block Protection */

} awbl_imx1050_flexspi_devinfo_t;

/**
 * \brief iMX RT1050  Flexspi �豸ʵ��
 */
typedef struct awbl_imx1050_flexspi_dev {

    struct awbl_dev super;     /**< \brief �̳��� AWBus �豸 */
    struct awbl_flexspi_servfuncs  serv_func;

    uint8_t   xip_en;          /**< \brief ��¼IXPģʽ�Ƿ�ʹ�� */
    uint32_t  state;           /**< \brief ��ǰ�����״̬ */
    uint32_t  *p_data;         /**< \brief ��ǰ��������� */
    uint32_t  data_size;       /**< \brief ��ǰ��������ݴ�С */

    AW_SEMB_DECL(flexspi_trans_sync);     /**< \brief ͬ����������ź��� */
    AW_MUTEX_DECL(flexspi_trans_lock);    /**< \brief ͬ��������������� */

} awbl_imx1050_flexspi_dev_t;


/**
 * \brief iMX RT1050 FlexSPI master driver register
 */
void awbl_imx1050_flexspi_drv_register (void);


#endif /* __AWBL_IMX1050_FLEXSPI_H */

/* end of file */
