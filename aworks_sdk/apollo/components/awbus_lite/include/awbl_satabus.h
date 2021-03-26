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
 * \brief AWBus SATA bus type implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 17-06-07  anu, first implementation
 * \endinternal
 */

#ifndef __AWBL_SATABUS_H
#define __AWBL_SATABUS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "aw_delayed_work.h"
#include "aw_sem.h"

struct awbl_satabus_host;

typedef uint32_t awbl_satabus_cmd_req_flag_t;

/** \brief 该命令为ATAPI 命令包 */
#define AWBL_SATABUS_CMD_REQ_FLAG_ATAPI      (0X0001) 

/** \brief 该设备支持LBA寻址 */
#define AWBL_SATABUS_CMD_REQ_FLAG_LBA        (0X0002) 

/** \brief 该设备支持LBA48寻址 */
#define AWBL_SATABUS_CMD_REQ_FLAG_LBA48      (0X0004) 

/** \brief 该设备支持NCQ */
#define AWBL_SATABUS_CMD_REQ_FLAG_NCQ        (0X0008) 

/** \brief 命令为写操作 */
#define AWBL_SATABUS_CMD_REQ_FLAG_WRITE      (0X0010) 

/** \brief atapi 的命令长度 */
#define AWBL_SATABUS_ATAPI_CMD_LEN        16    

/** \brief sata bus stat */
typedef uint8_t awbl_satabus_host_stat_t;
#define AWBL_SATABUS_HOST_STAT_NONE      (0X00) /**< \brief 无 */
#define AWBL_SATABUS_HOST_STAT_ERR       (0X01) /**< \brief 错误操作 */
#define AWBL_SATABUS_HOST_STAT_PLUG      (0X02) /**< \brief 插拔操作 */
#define AWBL_SATABUS_HOST_STAT_REQC      (0X03) /**< \brief 请求取消操作 */
#define AWBL_SATABUS_HOST_STAT_PRESET    (0X04) /**< \brief 端口复位操作 */
#define AWBL_SATABUS_HOST_STAT_PSTOP     (0X05) /**< \brief 端口停止操作 */

typedef enum awbl_satabus_dev_type {

    AWBL_SATABUS_DEV_TYPE_DISK = 0, /**< \brief 设备类型大存储容量类型 */
    AWBL_SATABUS_DEV_TYPE_CDROM   , /**< \brief 光驱类型 */
    AWBL_SATABUS_DEV_TYPE_ILLEGAL , /**< \brief 设备类型无效类型 */

} awbl_satabus_dev_type_t;

/** \brief 最大的设备数目 一端口一设备 AHCI最大32端口 */
#define AWBL_SATABUS_DEV_MAX_NUM          32 

/** \brief 端口连接状态 */
typedef struct awbl_satabus_host_port_link_stat {

    aw_bool_t is_change;        /**< \brief 是否发生过变化 */

    aw_bool_t is_online;        /**< \brief 是否在线 */

} awbl_satabus_host_port_link_stat_t;

/** \brief 主机控制器具备的功能信息 */
typedef struct awbl_satabus_host_cap_info {

    uint32_t port_max_num;   /**< \brief 最大的端口数量  */

} awbl_satabus_host_cap_info_t;

/** \brief 命令请求执行后设备状态  */
typedef struct awbl_satabus_dev_stat {

    awbl_satabus_host_stat_t host_stat; /**< \brief 主机状态 */

    uint8_t  status;         /**< \brief Completion status    */
    
    /**
     * \brief Error type 若上层是ATA/ATAPI命令集 
     *  具体错误可以参考 awbl_ata_err_code_t  
     */
    uint8_t  error;          
    uint8_t  device;         /**< \brief Device head          */
    uint16_t sector_cnt;     /**< \brief Sector count         */

    union {
        uint64_t dw;
        struct {
            uint8_t  low;        /**< \brief LBA Low              */
            uint8_t  mid;        /**< \brief LBA Mid              */
            uint8_t  high;       /**< \brief LBA High             */
            uint8_t  low_exp;    /**< \brief LBA Low_exp          */
            uint8_t  mid_exp;    /**< \brief LBA Mid_exp          */
            uint8_t  high_exp;   /**< \brief LBA High_exp         */
        } byte;
    } lba;

} awbl_satabus_dev_stat_t;

/** \brief SATABUS 的命令请求 */
typedef struct awbl_satabus_cmd_req {

    struct awbl_satabus_cmd_req *p_next; /**< \brief the next segment or NULL */

    union {
        struct awbl_sata_cmd_ata {
            
            /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_cmd_type_t */
            uint8_t  cmd_type;   
            
            /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_feature_t */
            uint8_t  features;   
            
            /** \brief 扇区的个数 */
            uint16_t sector_cnt; 
            union {
                /**
                 * \brief 计算方法可以参考 awbl_ata_identify_data_t 
                 *  有描述如何进行计算 
                 */
                uint64_t lba;
                struct {
                    uint16_t cylinder; /**< \brief 柱面 : 0 到 Cylinders - 1 */
                    uint16_t head;     /**< \brief 磁头 : 0 到 Heads - 1   */
                    
                    /** \brief 扇区 : 1 到 Sectors per track(注意是从 1 开始) */
                    uint16_t sector;   
                } chs;
            } seek;
        } ata;

        struct awbl_sata_cmd_atapi {
            
             /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_cmd_type_t */
            uint8_t  cmd_type;         
            
            /** \brief 若上层是ATA/ATAPI命令集，可参考 awbl_ata_fis_feature_t */
            uint8_t  features;          
            
            /** \brief 扇区的个数 */
            uint16_t sector_cnt;        
            uint8_t  cmd_pkt[AWBL_SATABUS_ATAPI_CMD_LEN];
        } atapi;
    } cmd;

    awbl_satabus_cmd_req_flag_t flag; /**< \brief 命令请求的标志 */

    uint8_t  *p_buffer;               /**< \brief Data Buffer              */
    uint32_t  trans_cnt;              /**< \brief Total Number of bytes    */

    awbl_satabus_dev_stat_t dev_stat;

    aw_err_t (*pfn_complete_cb) (
    
            /** \brief 请求完成后的回调函数 */
            struct awbl_satabus_cmd_req *p_req, void *p_arg); 

    void *p_arg; /**< \brief 请求完成后的回调函数参数，应用需要可设置 */

} awbl_satabus_cmd_req_t;

/** \brief SATA Bus host controller driver information  */
struct awbl_satabus_host_drv_funs {

    /** \brief reset port */
    aw_err_t (*pfn_port_reset) (
            struct awbl_satabus_host *p_host, uint32_t port_num);

    /** \brief get controller info */
    aw_err_t (*pfn_cap_info_get) (struct awbl_satabus_host     *p_host,
                                  awbl_satabus_host_cap_info_t *p_cap_info);

    /** \brief port is connect or disconnect */
    aw_err_t (*pfn_port_link_stat_get) (
            struct awbl_satabus_host           *p_host,
            uint32_t                            port_num,
            awbl_satabus_host_port_link_stat_t *p_link_stat);

    /** \brief command request */
    aw_err_t (*pfn_cmd_request) (
            struct awbl_satabus_host *p_host,
            uint32_t                  port_num,
            awbl_satabus_cmd_req_t   *p_req);

    /** \brief cancle command request */
    aw_err_t (*pfn_cmd_request_cancel) (
            struct awbl_satabus_host *p_host,
            uint32_t                  port_num,
            awbl_satabus_cmd_req_t   *p_req);

};

/** \brief SATA Bus host information  */
struct awbl_satabus_host_info {

    uint8_t bus_index;     /**< \brief bus ID */

};

/** \brief SATA Bus dev information  */
struct awbl_satabus_dev_info {

    struct awbl_drvinfo super;

    awbl_satabus_dev_type_t type;

};

/** \brief SATA Bus host controller  */
struct awbl_satabus_host {

    struct awbl_busctlr      bus_ctrl;
    
    /** \brief memory for device */
    struct awbl_satabus_dev *p_dev[AWBL_SATABUS_DEV_MAX_NUM];
    
    /** \brief period works for slot */
    struct aw_delayed_work   dwork_detect;  

    struct awbl_satabus_host_info *p_info;

    struct awbl_satabus_host_drv_funs *p_drvfuns;
    
    
    /** \brief 每位代表某个端口的设备创建情况 1为已经创建  */
    uint32_t dev_created;  
    
    /** \brief 主机控制器具备的功能信息 */
    awbl_satabus_host_cap_info_t cap_info;  

};

/** \brief SATA Bus device  */
struct awbl_satabus_dev {

    struct awbl_dev           super;
    struct awbl_devhcf        devhcf; /**< \brief device information */
    struct awbl_satabus_host *p_host; /**< \brief sata host controller */

    AW_SEMB_DECL(opt_semb);           /**< \brief 用于同步操作的二进制信号量 */

    char name[20];                    /**< \brief card name */

    uint32_t port_num;                /**< \brief 对应的端口号 */

    void *p_drv;                      /**< \brief 驱动，供驱动层使用  */
};

aw_err_t awbl_satabus_cmd_request_sync (
        struct awbl_satabus_dev *p_dev, awbl_satabus_cmd_req_t *p_req);

aw_err_t awbl_satabus_cmd_request_async (
        struct awbl_satabus_dev *p_dev,
        awbl_satabus_cmd_req_t  *p_req);

aw_err_t awbl_satabus_cmd_request_cancel (
        struct awbl_satabus_dev *p_dev,
        awbl_satabus_cmd_req_t  *p_req);

aw_err_t awbl_satabus_create (struct awbl_satabus_host          *p_host,
                              struct awbl_satabus_host_info     *p_info,
                              struct awbl_satabus_host_drv_funs *p_drvfuns);

void awbl_satabus_init (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SATABUS_H */

/* end of file */
