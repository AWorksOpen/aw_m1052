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

#ifndef __AWBL_IMX1050_FLEXSPI_NAND_H
#define __AWBL_IMX1050_FLEXSPI_NAND_H

#include "aw_sem.h"
#include "aw_cpu_clk.h"
#include "imx1050_inum.h"
#include "imx1050_reg_base.h"
#include "awbl_nvram.h"
#include "awbl_nand_bus.h"
#include "driver/flexspi/imx1050_flexspi_regs.h"

#include "driver/flexspi_nand/awbl_imx1050_flexspi_nand_service.h"

#define AWBL_IMX1050_FLEXSPI_NAND_NAME   "imx1050_flexspi_nand"


#define FLEXSPI_PORT_A1           0        /**< \brief Access flash on A1 port. */
#define FLEXSPI_PORT_A2           1        /**< \brief Access flash on A2 port. */
#define FLEXSPI_PORT_B1           2        /**< \brief Access flash on B1 port. */
#define FLEXSPI_PORT_B2           3        /**< \brief Access flash on B2 port. */
#define FLASH_STATUS_REG_QE_MASK  (1 << 6) /**< \brief Flash Quad Enable bit mask */


/* FlexSPI AHB读写映射基地址 */
#define FLEXSPI_AMBA_BASE         (0x60000000UL)



/**
 * LUT sequence definition for Serial NOR
 */
#define __FLEXSPI_SEQID_READ            0   /**< \brief 读数据 */
#define __FLEXSPI_SEQID_RDSR            1   /**< \brief 读状态寄存器 */
#define __FLEXSPI_SEQID_WRITE           2   /**< \brief 写数据 */
#define __FLEXSPI_SEQID_WREN            3   /**< \brief 写使能 */
#define __FLEXSPI_SEQID_WRDI            4   /**< \brief 写禁能 */
#define __FLEXSPI_SEQID_SE              5   /**< \brief 扇区擦除 */
#define __FLEXSPI_SEQID_CHIP_ERASE      6   /**< \brief 整片擦除 */
#define __FLEXSPI_SEQID_RDID            7   /**< \brief 读ID */
#define __FLEXSPI_SEQID_WRSR            8   /**< \brief 写状态寄存器 */
#define __FLEXSPI_SEQID_PP              9   /**< \brief 编程 */
#define __FLEXSPI_SEQID_RDCR            10  /**< \brief 读配置寄存器 */
#define __FLEXSPI_SEQID_EN4B            11  /**< \brief 进入4Byte模式 */
#define __FLEXSPI_SEQID_EX4B            12  /**< \brief 离开4Byte模式 */

/**
 * LUT sequence definition for Serial NAND
 * Reserved index:2,6,7,8,12,15
 */
#define __FLEXSPI_SEQID_NAND_READ_ID                        0    /**< \brief 读取nand flash ID */
#define __FLEXSPI_SEQID_NAND_READ_FEATURE                   1    /**< \brief 获取状态寄存器*/
#define __FLEXSPI_SEQID_NAND_WRITE_FEATURE                  2    /**< \brief 设置状态寄存器 */
#define __FLEXSPI_SEQID_NAND_WRITE_ENABLE                   3    /**< \brief 使能 */
#define __FLEXSPI_SEQID_NAND_WRITE_DISENABLE                4    /**< \brief 写禁止 */
#define __FLEXSPI_SEQID_NAND_BLOCK_ERASE                    5    /**< \brief 块擦除*/
#define __FLEXSPI_SEQID_NAND_PROGRAM_LOAD                   6    /**< \brief 4线写模式 */
#define __FLEXSPI_SEQID_NAND_PROGRAM_EXECUTE                7    /**< \brief 从nand cache回写到nand flash */
#define __FLEXSPI_SEQID_NAND_PAGE_READ_TO_CACHE             8    /**< \brief 从nand flash读数据到nand cache */
#define __FLEXSPI_SEQID_NAND_READ_FROM_CACHE                9    /**< \brief 4线模式从nand cache读数据到cpu */
#define __FLEXSPI_SEQID_NAND_RESET                          10   /**< \brief nand flash复位 */


/**
 * Status Register 1 ：Block Protcetion Register
 *
 * bit0： SP             //Block Protcetion Register Protect
 * bit1: Comp           //Write Protect Enable
 * bit2: Ivert          //0:upper(default), 1:lower(custom set)
 * bit3: BP0            //Block Protect Bit-0
 * bit4: BP1            //Block Protect Bit-1
 * bit5: BP2            //Block Protect Bit-2
 * bit6: Reserved       //--
 * bit7: BPRWD          //--
 */
#define NAND_STATUS_REGISTER_1                     0xA0

/**
 * Status Register 2 ：Secure OTP
 *
 * bit0： QE             //Quad Enable
 * bit1: Reserved       //--
 * bit2: Reserved       //--
 * bit3: Reserved       //--
 * bit4: ECC-E          //Enable ECC
 * bit5: Reserved       //--
 * bit6: OTP-E          //OTP Enable
 * bit7: OTP-P          //OTP Protect
 */
#define NAND_STATUS_REGISTER_2                     0xB0

/**
 * Status Register 3 ：Status
 *
 * bit0： OIP            //BUSY Flag
 * bit1: WEL            //Write Enable Flag
 * bit2: E-FAIL         //Erase Failed Flag
 * bit3: P-FAIL         //Program Failed Flag
 * bit4: ECC-0          //ECC Status bit0
 * bit5: ECC-1          //ECC Status bit1
 * bit6: Reserved       //--
 * bit7: Reserved       //--
 */
#define NAND_STATUS_REGISTER_3                     0xC0

/**
 * Feature Settings Register Address
 */
//todo;

#define SR3_OIP                                    0x01
#define SR3_WEL                                    0x02
#define SR3_E_FAIL                                 0x04
#define SR3_P_FAIL                                 0x08
#define SR3_ECC_MASK                               0x30

/**
 * ECC Status
 */
#define SUCCESS_WITH_NO_ECC_ERROR                  0x00
#define SUCCESS_WITH_1_4BITS_ECC_ERROR             0x10
#define FAILED_WITH_OVER_4BITS_ECC_ERROR           0x20

typedef uint8_t __flexspi_bus_mode_t;
#define FLEXSPI_BUS_MODE_PAD1       0x01
#define FLEXSPI_BUS_MODE_PAD4       0x04


/**
 * \brief 外部闪存设备配置参数
 */
typedef struct __flexspi_nand_device_config {

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

} __flexspi_nand_device_config_t;


/**
 * \brief Transfer structure for FLEXSPI.
 */
typedef struct __flexspi_nand_transfer {

    uint32_t  device_address;         /**< \brief Operation device address */
    uint8_t   port;                   /**< \brief Operation port */
    flexspi_command_type_t cmd_type;  /**< \brief Execution command type */
    uint8_t   seq_index;              /**< \brief Sequence ID for command  */
    uint8_t   seq_num;                /**< \brief Sequence number for command */
    uint8_t  *p_buf;                  /**< \brief Data buffer */
    size_t    data_size;              /**< \brief Data size in bytes */
} __flexspi_nand_transfer_t;

/**
 * \brief iMX RT1050 Flexspi 设备信息
 */
typedef struct awbl_imx1050_flexspi_nand_devinfo {

    uint32_t                regbase;              /**< \brief 寄存器基地址 */
    int                     inum;                 /**< \brief 中断号 */
    uint8_t                 port;                 /**< \brief Flash芯片连接的端口 */
    __flexspi_nand_device_config_t  dev_cfg;      /**< \brief 外部Flash设备的参数配置 */

    uint8_t                 addr_width;           /**< \brief 地址宽度，0x18表示24位，0x20表示32位 */
    aw_clk_id_t             ref_clk_id;           /**< \brief the clock which the counter use */
    __flexspi_bus_mode_t    bus_width;            /**< \brief nandflash read and write data bus width */
    uint32_t                lock_block;           /**< \brief nandflash default lock block num */

    uint8_t                 nandbus_index;
    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

} awbl_imx1050_flexspi_nand_devinfo_t;

/**
 * \brief iMX RT1050  Flexspi 设备实例
 */
typedef struct awbl_imx1050_flexspi_nand_dev {

    struct awbl_dev super;     /**< \brief 继承自 AWBus 设备 */
    struct awbl_flexspi_servfuncs  serv_func;
    struct awbl_nandbus            nandbus;

    uint8_t   xip_en;          /**< \brief 记录IXP模式是否使能 */
    uint32_t  state;           /**< \brief 当前传输的状态 */
    uint32_t  *p_data;         /**< \brief 当前传输的数据 */
    uint32_t  data_size;       /**< \brief 当前传输的数据大小 */

    AW_SEMB_DECL(flexspi_trans_sync);     /**< \brief 同步传输操作信号量 */
    AW_MUTEX_DECL(flexspi_trans_lock);    /**< \brief 同步传输操作互斥锁 */

} awbl_imx1050_flexspi_nand_dev_t;


/**
 * \brief iMX RT1050 FlexSPI master driver register
 */
void awbl_imx1050_flexspi_nand_drv_register (void);


#endif /* __AWBL_IMX1050_FLEXSPI_H */

/* end of file */
