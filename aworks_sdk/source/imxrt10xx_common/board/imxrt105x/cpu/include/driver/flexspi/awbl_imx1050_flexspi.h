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


/* FlexSPI AHB读写映射基地址 */
#define FLEXSPI_AMBA_BASE         (0x60000000UL)


/**
 * \brief 外部闪存设备配置参数
 */
typedef struct __flexspi_device_config {

    uint32_t flash_size;           /**< \brief Flash的大小，单位(KByte)*/
    uint32_t sector_size;          /**< \brief Flash扇区大小 单位(KByte)*/
    uint32_t page_size;            /**< \brief Flash页大小 单位(Byte)*/
    uint32_t block_size;           /**< \brief Flash块大小 单位(Byte)*/
    uint16_t cs_interval;          /**< \brief CS选中到释放的间隔时间,时间为该值乘以串行时钟周期 */
    uint8_t cs_hold_time;          /**< \brief CS的保持时间 */
    uint8_t cs_setup_time;         /**< \brief CS的建立时间 */
    uint8_t columnspace;           /**< \brief 列地址位宽 */
    uint8_t data_valid_time;       /**< \brief 数据的有效时间 */
    flexspi_ahb_write_wait_unit_t ahb_write_wait_unit;    /**< \brief AHB写等待间隔时间单元*/
    uint16_t ahb_write_wait_interval;   /**< \brief AHB写等待时间间隔  */

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
 * \brief iMX RT1050 Flexspi 设备信息
 */
typedef struct awbl_imx1050_flexspi_devinfo {

    uint32_t         regbase;              /**< \brief 寄存器基地址 */
    int              inum;                 /**< \brief 中断号 */
    uint8_t          port;                 /**< \brief Flash芯片连接的端口 */
    flexspi_device_config_t  dev_cfg;      /**< \brief 外部Flash设备的参数配置 */

    uint8_t          addr_width;           /**< \brief 地址宽度，0x18表示24位，0x20表示32位 */

    awbl_flexspi_read_mode_t read_mode;    /**< \brief 读模式 */
    uint8_t          read_opcode;          /**< \brief 读操作数 */
    uint8_t          program_opcode;       /**< \brief 编程操作数 */
    uint8_t          erase_opcode;         /**< \brief 擦除操作数 */
    uint8_t          read_dummy;           /**< \brief 虚拟读等待周期 */

    /** \brief Flexspi nor flash NVRAM信息 */
    struct awbl_flexspi_nor_flash_nvram_info nvram_info;

    void             *p_mtd;               /**< \brief MTD对象句柄   */
    const void       *p_info;              /**< \brief MTD对象信息   */

    aw_clk_id_t      ref_clk_id;           /**< \brief the clock which the counter use */
    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

    uint8_t          bp_flag;              /**< \brief Block Protection */

} awbl_imx1050_flexspi_devinfo_t;

/**
 * \brief iMX RT1050  Flexspi 设备实例
 */
typedef struct awbl_imx1050_flexspi_dev {

    struct awbl_dev super;     /**< \brief 继承自 AWBus 设备 */
    struct awbl_flexspi_servfuncs  serv_func;

    uint8_t   xip_en;          /**< \brief 记录IXP模式是否使能 */
    uint32_t  state;           /**< \brief 当前传输的状态 */
    uint32_t  *p_data;         /**< \brief 当前传输的数据 */
    uint32_t  data_size;       /**< \brief 当前传输的数据大小 */

    AW_SEMB_DECL(flexspi_trans_sync);     /**< \brief 同步传输操作信号量 */
    AW_MUTEX_DECL(flexspi_trans_lock);    /**< \brief 同步传输操作互斥锁 */

} awbl_imx1050_flexspi_dev_t;


/**
 * \brief iMX RT1050 FlexSPI master driver register
 */
void awbl_imx1050_flexspi_drv_register (void);


#endif /* __AWBL_IMX1050_FLEXSPI_H */

/* end of file */
