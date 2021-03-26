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

/** \brief ������ΪATAPI ����� */
#define AWBL_SATABUS_CMD_REQ_FLAG_ATAPI      (0X0001) 

/** \brief ���豸֧��LBAѰַ */
#define AWBL_SATABUS_CMD_REQ_FLAG_LBA        (0X0002) 

/** \brief ���豸֧��LBA48Ѱַ */
#define AWBL_SATABUS_CMD_REQ_FLAG_LBA48      (0X0004) 

/** \brief ���豸֧��NCQ */
#define AWBL_SATABUS_CMD_REQ_FLAG_NCQ        (0X0008) 

/** \brief ����Ϊд���� */
#define AWBL_SATABUS_CMD_REQ_FLAG_WRITE      (0X0010) 

/** \brief atapi ������� */
#define AWBL_SATABUS_ATAPI_CMD_LEN        16    

/** \brief sata bus stat */
typedef uint8_t awbl_satabus_host_stat_t;
#define AWBL_SATABUS_HOST_STAT_NONE      (0X00) /**< \brief �� */
#define AWBL_SATABUS_HOST_STAT_ERR       (0X01) /**< \brief ������� */
#define AWBL_SATABUS_HOST_STAT_PLUG      (0X02) /**< \brief ��β��� */
#define AWBL_SATABUS_HOST_STAT_REQC      (0X03) /**< \brief ����ȡ������ */
#define AWBL_SATABUS_HOST_STAT_PRESET    (0X04) /**< \brief �˿ڸ�λ���� */
#define AWBL_SATABUS_HOST_STAT_PSTOP     (0X05) /**< \brief �˿�ֹͣ���� */

typedef enum awbl_satabus_dev_type {

    AWBL_SATABUS_DEV_TYPE_DISK = 0, /**< \brief �豸���ʹ�洢�������� */
    AWBL_SATABUS_DEV_TYPE_CDROM   , /**< \brief �������� */
    AWBL_SATABUS_DEV_TYPE_ILLEGAL , /**< \brief �豸������Ч���� */

} awbl_satabus_dev_type_t;

/** \brief �����豸��Ŀ һ�˿�һ�豸 AHCI���32�˿� */
#define AWBL_SATABUS_DEV_MAX_NUM          32 

/** \brief �˿�����״̬ */
typedef struct awbl_satabus_host_port_link_stat {

    aw_bool_t is_change;        /**< \brief �Ƿ������仯 */

    aw_bool_t is_online;        /**< \brief �Ƿ����� */

} awbl_satabus_host_port_link_stat_t;

/** \brief �����������߱��Ĺ�����Ϣ */
typedef struct awbl_satabus_host_cap_info {

    uint32_t port_max_num;   /**< \brief ���Ķ˿�����  */

} awbl_satabus_host_cap_info_t;

/** \brief ��������ִ�к��豸״̬  */
typedef struct awbl_satabus_dev_stat {

    awbl_satabus_host_stat_t host_stat; /**< \brief ����״̬ */

    uint8_t  status;         /**< \brief Completion status    */
    
    /**
     * \brief Error type ���ϲ���ATA/ATAPI��� 
     *  ���������Բο� awbl_ata_err_code_t  
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

/** \brief SATABUS ���������� */
typedef struct awbl_satabus_cmd_req {

    struct awbl_satabus_cmd_req *p_next; /**< \brief the next segment or NULL */

    union {
        struct awbl_sata_cmd_ata {
            
            /** \brief ���ϲ���ATA/ATAPI������ɲο� awbl_ata_cmd_type_t */
            uint8_t  cmd_type;   
            
            /** \brief ���ϲ���ATA/ATAPI������ɲο� awbl_ata_fis_feature_t */
            uint8_t  features;   
            
            /** \brief �����ĸ��� */
            uint16_t sector_cnt; 
            union {
                /**
                 * \brief ���㷽�����Բο� awbl_ata_identify_data_t 
                 *  ��������ν��м��� 
                 */
                uint64_t lba;
                struct {
                    uint16_t cylinder; /**< \brief ���� : 0 �� Cylinders - 1 */
                    uint16_t head;     /**< \brief ��ͷ : 0 �� Heads - 1   */
                    
                    /** \brief ���� : 1 �� Sectors per track(ע���Ǵ� 1 ��ʼ) */
                    uint16_t sector;   
                } chs;
            } seek;
        } ata;

        struct awbl_sata_cmd_atapi {
            
             /** \brief ���ϲ���ATA/ATAPI������ɲο� awbl_ata_cmd_type_t */
            uint8_t  cmd_type;         
            
            /** \brief ���ϲ���ATA/ATAPI������ɲο� awbl_ata_fis_feature_t */
            uint8_t  features;          
            
            /** \brief �����ĸ��� */
            uint16_t sector_cnt;        
            uint8_t  cmd_pkt[AWBL_SATABUS_ATAPI_CMD_LEN];
        } atapi;
    } cmd;

    awbl_satabus_cmd_req_flag_t flag; /**< \brief ��������ı�־ */

    uint8_t  *p_buffer;               /**< \brief Data Buffer              */
    uint32_t  trans_cnt;              /**< \brief Total Number of bytes    */

    awbl_satabus_dev_stat_t dev_stat;

    aw_err_t (*pfn_complete_cb) (
    
            /** \brief ������ɺ�Ļص����� */
            struct awbl_satabus_cmd_req *p_req, void *p_arg); 

    void *p_arg; /**< \brief ������ɺ�Ļص�����������Ӧ����Ҫ������ */

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
    
    
    /** \brief ÿλ����ĳ���˿ڵ��豸������� 1Ϊ�Ѿ�����  */
    uint32_t dev_created;  
    
    /** \brief �����������߱��Ĺ�����Ϣ */
    awbl_satabus_host_cap_info_t cap_info;  

};

/** \brief SATA Bus device  */
struct awbl_satabus_dev {

    struct awbl_dev           super;
    struct awbl_devhcf        devhcf; /**< \brief device information */
    struct awbl_satabus_host *p_host; /**< \brief sata host controller */

    AW_SEMB_DECL(opt_semb);           /**< \brief ����ͬ�������Ķ������ź��� */

    char name[20];                    /**< \brief card name */

    uint32_t port_num;                /**< \brief ��Ӧ�Ķ˿ں� */

    void *p_drv;                      /**< \brief ��������������ʹ��  */
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
