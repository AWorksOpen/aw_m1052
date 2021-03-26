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
 * \brief SATA AHCI controller support head file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-16  anu, first implementation
 * \endinternal
 */


#ifndef __AWBL_SATA_AHCI_H
#define __AWBL_SATA_AHCI_H

#include "apollo.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "aw_delayed_work.h"
#include "awbl_satabus.h"

#ifdef AWBL_SATA_AHCI_USE_CPU_LOCK
#include "aw_spinlock.h"
#endif

struct awbl_sata_ahci_dev;

/** \brief ahci cmd req flag */
typedef awbl_satabus_cmd_req_flag_t awbl_ahci_cmd_req_flag_t;

/** \brief 该命令为ATAPI 命令包 */
#define AWBL_AHCI_CMD_REQ_FLAG_ATAPI      AWBL_SATABUS_CMD_REQ_FLAG_ATAPI    

/** \brief 该设备支持LBA寻址 */
#define AWBL_AHCI_CMD_REQ_FLAG_LBA        AWBL_SATABUS_CMD_REQ_FLAG_LBA      

/** \brief 该设备支持LBA48寻址 */
#define AWBL_AHCI_CMD_REQ_FLAG_LBA48      AWBL_SATABUS_CMD_REQ_FLAG_LBA48    

/** \brief 该设备支持NCQ */
#define AWBL_AHCI_CMD_REQ_FLAG_NCQ        AWBL_SATABUS_CMD_REQ_FLAG_NCQ      

/** \brief 命令为写操作 */
#define AWBL_AHCI_CMD_REQ_FLAG_WRITE      AWBL_SATABUS_CMD_REQ_FLAG_WRITE    

/** \brief sata ahci bus stat */
typedef awbl_satabus_host_stat_t awbl_sata_ahci_stat_t;

/** \brief 无 */
#define AWBL_SATA_AHCI_STAT_NONE          AWBL_SATABUS_HOST_STAT_NONE        

/** \brief 错误操作 */
#define AWBL_SATA_AHCI_STAT_ERR           AWBL_SATABUS_HOST_STAT_ERR         

/** \brief 插拔操作 */
#define AWBL_SATA_AHCI_STAT_PLUG          AWBL_SATABUS_HOST_STAT_PLUG        

/** \brief 请求取消操作 */
#define AWBL_SATA_AHCI_STAT_REQC          AWBL_SATABUS_HOST_STAT_REQC        

/** \brief 端口复位操作 */
#define AWBL_SATA_AHCI_STAT_PRESET        AWBL_SATABUS_HOST_STAT_PRESET      

/** \brief 端口停止操作 */
#define AWBL_SATA_AHCI_STAT_PSTOP         AWBL_SATABUS_HOST_STAT_PSTOP       

/** \brief AHCI port link status */
typedef awbl_satabus_host_port_link_stat_t awbl_sata_ahci_port_link_stat_t;

/** \brief AHCI port link status */
typedef awbl_satabus_host_cap_info_t awbl_sata_ahci_cap_info_t;

/** \brief AHCI 的命令请求 */
typedef awbl_satabus_cmd_req_t awbl_sata_ahci_cmd_req_t;

/** \brief AHCI 的命令请求执行后设备状态  */
typedef awbl_satabus_dev_stat_t awbl_sata_ahci_dev_stat_t;

/** \brief sata ahci port/cmdslot max num  */

/** \brief 最大的端口数目 AHCI最大32 */
#define AWBL_SATA_AHCI_PORT_NUM_MAX          32 

/** \brief 最大的命令槽数目 AHCI最大32 */
#define AWBL_SATA_AHCI_CMDSLOT_NUM_MAX       32 

/** \brief
 * 最大的命令槽数目 AHCI最大65535 (该处选8是为了cmd_table结构体对齐到
 * cache line size, 选取其他值也应该考虑)
 */
#define AWBL_SATA_AHCI_PRD_ENTRIES_NUM_MAX   8  

/** \brief sata ahci atapi 的命令长度 */
#define AWBL_SATA_AHCI_ATAPI_CMD_LEN          16 

/** \brief sata ahci task */
#define AWBL_SATA_AHCI_MONITOR_TASK_STACK_SIZE  8196 /**< \brief SATA AHCI 任务栈 */

/** \brief Generic Host Control */
typedef struct awbl_sata_ahci_ghc_regs {

    volatile uint32_t cap;
    volatile uint32_t ghc;
    volatile uint32_t is;
    volatile uint32_t pi;
    volatile uint32_t vs;
    volatile uint32_t ccc_ctl;
    volatile uint32_t ccc_ports;
    volatile uint32_t em_loc;
    volatile uint32_t em_ctl;
    volatile uint32_t cap2;
    volatile uint32_t bohc;

} awbl_sata_ahci_ghc_regs_t;

/** \brief Vendor Specific registers */
typedef struct awbl_sata_ahci_vs_regs {

    volatile uint8_t  rsvd[96];

} awbl_sata_ahci_vs_regs_t;

/** \brief Port x Vendor Specific */
typedef struct awbl_sata_ahci_port_vs_regs {

    volatile uint8_t  rsvd[16];

} awbl_sata_ahci_port_vs_regs_t;

/** \brief Port x port control registers */
typedef struct awbl_sata_ahci_port_regs {

    volatile uint32_t clb;        /** \brief Port x Command List Base Address */
    volatile uint32_t clbu;
    volatile uint32_t fb;         /** \brief Port x FIS Base Address */
    volatile uint32_t fbu;
    volatile uint32_t is;
    volatile uint32_t ie;
    volatile uint32_t cmd;
    volatile uint8_t  rsvd0[4];
    volatile uint32_t tfd;
    volatile uint32_t sig;
    volatile uint32_t ssts;
    volatile uint32_t sctl;
    volatile uint32_t serr;
    volatile uint32_t sact;
    volatile uint32_t ci;
    volatile uint32_t sntf;
    volatile uint32_t fbs;
    volatile uint8_t  rsvd1[44];

    awbl_sata_ahci_port_vs_regs_t vs;

} awbl_sata_ahci_port_regs_t;

/** \brief ahci ctrl registers */
typedef struct awbl_sata_ahci_regs {

    awbl_sata_ahci_ghc_regs_t  ghc;
    volatile uint8_t           rsvd0[116];
    awbl_sata_ahci_vs_regs_t   vs;
    awbl_sata_ahci_port_regs_t port[AWBL_SATA_AHCI_PORT_NUM_MAX];

} awbl_sata_ahci_regs_t;

/** \brief sata ahci Command Comamnd header Structure */
typedef struct awbl_sata_ahci_cmd_header {

    uint32_t options;         /**< \brief DW0 : Description information */
    
    /**
     *\brief DW1 : status indicates current byte count transfered 
     * on device write/reads 
     */
    uint32_t prd_byte_cnt;    
    uint32_t cmd_tbl_addr;    /**< \brief DW2 : command table base address */
    uint32_t cmd_tbl_addr_hi; /**< \brief DW3 : command table address high */
    uint32_t reserved[4];     /**< \brief DW4:DW7 reserved */

} awbl_sata_ahci_cmd_header_t;

/** \brief DMA Setup FIS */
typedef struct awbl_sata_ahci_dsfis {

    uint8_t  fis_type;   /**< \brief DW0:00 res,res,flags(AIDR-PMPORT),FIStype */
    uint8_t  flags;
    uint16_t reserved1;
    uint32_t dma_buf_low;        /**< \brief DW1:04 DMA Buffer Identifier Low */
    uint32_t dma_buf_high;       /**< \brief DW2:08 DMA Buffer Identifier High */
    uint32_t reserved2;          /**< \brief DW3:0C reserved */
    uint32_t dma_buf_offset;     /**< \brief DW4:10 DMA buffer offset */
    uint32_t dma_transfer_count; /**< \brief DW5:14 DMA Transfer count */
    uint32_t reserved3;          /**< \brief DW6:18 reserved */

} awbl_sata_ahci_dsfis_t;

/** \brief PIO Setup Fis */
typedef struct awbl_sata_ahci_psfis {

    uint8_t  fis_type;       /**< \brief DW0:00 flags, fistype, status, error */
    uint8_t  flags;
    uint8_t  status;
    uint8_t  error;
    uint8_t  lba_low;        /**< \brief DW1:04 Device,lbaHigh,lbaMid,lbaLow */
    uint8_t  lba_mid;
    uint8_t  lba_high;
    uint8_t  device;
    
    /** \brief DW2:08 resr,lbaHigh_exp,lbaMid_exp,lbaLow_exp */
    uint8_t  lba_low_exp;  
    uint8_t  lba_mid_exp;
    uint8_t  lba_high_exp;
    uint8_t  reserved1;
    uint16_t sector_count;     /**< \brief DW3:0C rerved, sector count*/
    uint8_t  reserved2;
    uint8_t  e_status;
    uint16_t transfer_count;   /**< \brief DW4:10 reserved */
    uint16_t reserved3;

} awbl_sata_ahci_psfis_t;

/** \brief Register FIS - H2D_FIS, D2H_FIS */
typedef struct awbl_sata_ahci_rfis {

    /** \brief DW0:00 flags, fistype, status, error */
    
    /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_type_t */
    uint8_t type;   
    uint8_t flags;

    union {
        
        /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_status_t */
        uint8_t status;     
        
        /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_cmd_type_t */
        uint8_t command;    
    } u1;

    union {
        
        /** \brief 错误状态  */
        uint8_t error;    

        /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_feature_t */
        uint8_t features;   
    } u2;

    /** \brief DW1:04 Device,lbaHigh,lbaMid,lbaLow */
    
    /** \brief 计算方法可以参考 awbl_ata_identify_data_t 有描述如何进行计算 */
    uint8_t lba_low;        
    uint8_t lba_mid;
    uint8_t lba_high;
    
    /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_device_t */
    uint8_t device;         

    /** \brief DW2:08 resr,lbaHigh_exp,lbaMid_exp,lbaLow_exp */
    uint8_t lba_low_exp;
    uint8_t lba_mid_exp;
    uint8_t lba_high_exp;
    uint8_t reserved1;

    /** \brief DW3:0c rerved, sector count*/
    uint16_t sector_count;
    uint8_t  reserved2;
    uint8_t  control;

    /** \brief DW4:10 reserved */
    uint32_t reserved3;

} awbl_sata_ahci_rfis_t;

/** \brief SetDevice Bits FIS */
typedef struct awbl_sata_ahci_sdbfis {

    /** \brief DW0:00 error, (R,stats_hi,stats_low),flags(I,PMP),fistype */
    uint8_t  fis_type; 
    uint8_t  flags;
    uint8_t  status;
    uint8_t  error;

    uint32_t reserved; /**< \brief DW1:04 reserved */

} awbl_sata_ahci_sdbfis_t;

/** 
 * \brief Memory map receive FIS structure size is 256 bytes 数据结构的地址AHCI
 * 要求 256字节对齐 
 */
typedef struct awbl_sata_ahci_rxfis {

    awbl_sata_ahci_dsfis_t  dma_setup_fis;   /**< \brief 00-1B: DMA Setup FIS */
    uint32_t                reserved1;       /**< \brief 1C-1F reserved */
    awbl_sata_ahci_psfis_t  pio_setup_fis;   /**< \brief 20-33: PIO Setup FIS */
    uint32_t                reserved2[3];    /**< \brief 34-3F reserved */
    
    /** \brief 40-53: D2H Register FIS */
    awbl_sata_ahci_rfis_t   d2h_reg_fis;     
    uint32_t                reserved3;       /**< \brief 54-57 reserved */
    
    /** \brief 58-5f: SetupDevice Bits FIS */
    awbl_sata_ahci_sdbfis_t set_dev_bit_fis; 

    uint32_t  unknown_fis[16];       /**< \brief 60-A0: Unknow FIS */
    uint32_t  reserved[24];          /**< \brief A0-FF: reserved */

} awbl_sata_ahci_rxfis_t;

/** \brief Physical Region Descriptor Table or PRDT entry */
typedef struct awbl_sata_ahci_prd_tbl {

    uint32_t data_base_addr;         /**< \brief DW0 : address */
    uint32_t data_base_addr_hi;      /**< \brief DW1 : address high */
    uint32_t reserved;               /**< \brief DW2 : reservered */
    
    /**
     * \brief DW3 : flag and data byte count, maximum lenght of 4MB, 
     * bit0 is always 1 for even byte count 
     */
    uint32_t flags_data_byte_cnt;    

} awbl_sata_ahci_prd_tbl_t;

/**
* \brief AhciCmdTable
*
* Command Table Data structure
*/
typedef struct awbl_sata_ahci_cmd_table {

    awbl_sata_ahci_rfis_t    cmd_fis;        /**< \brief 00:3F Command FIS */
    uint32_t                 reserved1[11];
    
    /** \brief 40:4F ATAPI Command */
    uint8_t                  atapi_cmd[AWBL_SATA_AHCI_ATAPI_CMD_LEN];       
    uint32_t                 reserved2[12];  /**< \brief 50:7F reserved */
    
    /** \brief 80: PRD table entries upto 65535 */
    awbl_sata_ahci_prd_tbl_t prd_table[AWBL_SATA_AHCI_PRD_ENTRIES_NUM_MAX]; 

} awbl_sata_ahci_cmd_table_t;

/** \brief sata ahci Command List Structure 数据结构的地址AHCI要求 1024字节对齐 */
typedef struct awbl_sata_ahci_cmd_list {

    awbl_sata_ahci_cmd_header_t cmd_header[AWBL_SATA_AHCI_CMDSLOT_NUM_MAX];

} awbl_sata_ahci_cmd_list_t;

/*
 * \brief AHCI port dma trans mem 一个cmd_slot 对应一个段 dma mem 用于地
 * 址不对齐，数据不对齐时拷贝使用，
 * 一段 dma mem需要2段cache line 大小的空间，用于p_s_dma起始段 p_e_dma结束段
 */
typedef struct awbl_sata_ahci_dma_mem {

    uint8_t *p_s_mem;   /**< \brief 开始的内存 */
    uint8_t *p_e_mem;   /**< \brief 结束的内存 */

} awbl_sata_ahci_dma_mem_t;

/** \brief AHCI port dma trans mem 使用的数据长度  */
typedef struct awbl_sata_ahci_dma_mem_usize {

    uint32_t s_size;   /**< \brief 开始的内存使用的数据长度 */
    uint32_t e_size;   /**< \brief 结束的内存使用的数据长度 */

} awbl_sata_ahci_dma_mem_usize_t;

/** \brief SATA AHCI PORT */
struct awbl_sata_ahci_port {

    struct awbl_sata_ahci_dev  *p_ahci_dev;

    awbl_sata_ahci_cmd_list_t  *p_cmd_list;

    awbl_sata_ahci_rxfis_t     *p_rx_fis;

    awbl_sata_ahci_cmd_table_t *p_cmd_tbl;

    /** \brief DMA内存分配情况 */
    awbl_sata_ahci_dma_mem_t       dma_mem[AWBL_SATA_AHCI_CMDSLOT_NUM_MAX];

    /** \brief DMA内存的使用情况 */
    awbl_sata_ahci_dma_mem_usize_t dma_mem_size[AWBL_SATA_AHCI_CMDSLOT_NUM_MAX];

    AW_MUTEX_DECL(opt_mutex);             /**< \brief 接口操作的互斥锁 */

    struct aw_delayed_work reset_dwork;   /**< \brief 端口PHY复位延迟作业任务 */
    struct aw_delayed_work error_dwork;   /**< \brief 端口错误延迟作业任务 */
    struct aw_delayed_work plug_dwork;    /**< \brief 端口插拔延迟作业任务 */

    
    /** \brief 正在处理的请求 */
    awbl_sata_ahci_cmd_req_t *p_req[AWBL_SATA_AHCI_CMDSLOT_NUM_MAX]; 

    awbl_sata_ahci_port_link_stat_t link_stat; /**< \brief 端口连接状态 */

    uint32_t port_num;                         /**< \brief 端口号  */

    uint32_t cmd_issued;                       /**< \brief 正被处理的命令 */

};

 /** \brief AHCI controller information */
struct awbl_sata_ahci_info {

    uint32_t reg_base_addr;        /**< \brief 寄存器基址 */

    int      monitor_task_prio;    /**< \brief ahci任务优先级 */

};

/** \brief SATA controller AHCI */
struct awbl_sata_ahci_dev {

    awbl_sata_ahci_regs_t      *p_regs;

    struct awbl_sata_ahci_port *p_port;

    struct awbl_sata_ahci_info *p_info;

#ifdef AWBL_SATA_AHCI_USE_CPU_LOCK
    aw_spinlock_isr_t cpu_lock;   /**< \brief cpu中断锁 */
#endif

    AW_SEMB_DECL(irq_semb);       /**< \brief 中断事件  */

    /** \brief 该任务用于监控处理事件（中断、插拔、失能等） */
    AW_TASK_DECL(ahci_monitor_task, AWBL_SATA_AHCI_MONITOR_TASK_STACK_SIZE); 

    /** \brief 用于存放 cmd list、rx_fis、cmd_table 分配的内存*/
    uint8_t *p_mem;               

    uint32_t port_max_num;        /**< \brief 最大的端口号  */
    uint32_t cmdslot_max_num;     /**< \brief 最大的命令槽个号  */
};

aw_err_t awbl_sata_ahci_port_reset (
        struct awbl_sata_ahci_dev *p_this,
        uint32_t                   port_num);

aw_err_t awbl_sata_ahci_cap_info_get (
        struct awbl_sata_ahci_dev *p_this,
        awbl_sata_ahci_cap_info_t *p_cap_info);

aw_err_t awbl_sata_ahci_port_link_stat_get (
        struct awbl_sata_ahci_dev       *p_this,
        uint32_t                         port_num,
        awbl_sata_ahci_port_link_stat_t *p_link_stat);

aw_err_t awbl_sata_ahci_cmd_req (
        struct awbl_sata_ahci_dev *p_this,
        uint32_t                   port_num,
        awbl_sata_ahci_cmd_req_t  *p_req);

/** \brief 目前的实现会导致所有请求均删除，未找到可以删除单一传输的方法 */
aw_err_t awbl_sata_ahci_cmd_req_cancel (
        struct awbl_sata_ahci_dev *p_this,
        uint32_t                   port_num,
        awbl_sata_ahci_cmd_req_t  *p_req);

aw_err_t awbl_sata_ahci_interrupt (struct awbl_sata_ahci_dev *p_this);

aw_err_t awbl_sata_ahci_init (
        struct awbl_sata_ahci_dev *p_this, struct awbl_sata_ahci_info *p_info);

aw_err_t awbl_sata_ahci_deinit (struct awbl_sata_ahci_dev *p_this);

#endif /* __AWBL_SATA_AHCI_H */

/* end of file */
