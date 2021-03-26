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
 * \brief AWBus SDIO interface
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-08  deo, first implementation
 * - 1.01 17-01-06  xdn, code refactoring
 * - 1.02 17-09-15  xdn, modify mem_alloc bug
 * - 1.03 18-04-20  xdn, modify static device enum way
 * \endinternal
 */


#ifndef _AWBL_SDIOBUS_H_
#define _AWBL_SDIOBUS_H_

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


#include "awbus_lite.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "string.h"
#include "aw_refcnt.h"
#include "aw_isr_defer.h"
#include "aw_timer.h"

#include "awbus_lite.h"
#include "aw_delayed_work.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "string.h"
#include "aw_task.h"


/**
 * \name SDIO command
 * \anchor grp_awbl_cmd_type
 * @{
 */

#define AWBL_SDIO_CMD0               0
#define AWBL_SDIO_CMD1               1
#define AWBL_SDIO_CMD2               2
#define AWBL_SDIO_CMD3               3
#define AWBL_SDIO_CMD4               4
#define AWBL_SDIO_CMD5               5
#define AWBL_SDIO_CMD6               6
#define AWBL_SDIO_CMD7               7
#define AWBL_SDIO_CMD8               8
#define AWBL_SDIO_CMD9               9
#define AWBL_SDIO_CMD10              10
#define AWBL_SDIO_CMD11              11
#define AWBL_SDIO_CMD12              12
#define AWBL_SDIO_CMD13              13
#define AWBL_SDIO_CMD14              14
#define AWBL_SDIO_CMD15              15
#define AWBL_SDIO_CMD16              16
#define AWBL_SDIO_CMD17              17
#define AWBL_SDIO_CMD18              18
#define AWBL_SDIO_CMD19              19
#define AWBL_SDIO_CMD20              20
#define AWBL_SDIO_CMD21              21
#define AWBL_SDIO_CMD22              22
#define AWBL_SDIO_CMD23              23
#define AWBL_SDIO_CMD24              24
#define AWBL_SDIO_CMD25              25
#define AWBL_SDIO_CMD26              26
#define AWBL_SDIO_CMD27              27
#define AWBL_SDIO_CMD28              28
#define AWBL_SDIO_CMD29              29
#define AWBL_SDIO_CMD31              31
#define AWBL_SDIO_CMD32              32
#define AWBL_SDIO_CMD33              33
#define AWBL_SDIO_CMD34              34
#define AWBL_SDIO_CMD35              35
#define AWBL_SDIO_CMD36              36
#define AWBL_SDIO_CMD37              37
#define AWBL_SDIO_CMD38              38
#define AWBL_SDIO_CMD39              39
#define AWBL_SDIO_CMD40              40
#define AWBL_SDIO_CMD41              41
#define AWBL_SDIO_CMD42              42
#define AWBL_SDIO_CMD43              43
#define AWBL_SDIO_CMD44              44
#define AWBL_SDIO_CMD45              45
#define AWBL_SDIO_CMD46              46
#define AWBL_SDIO_CMD47              47
#define AWBL_SDIO_CMD48              48
#define AWBL_SDIO_CMD49              49
#define AWBL_SDIO_CMD50              50
#define AWBL_SDIO_CMD51              51
#define AWBL_SDIO_CMD52              52
#define AWBL_SDIO_CMD53              53
#define AWBL_SDIO_CMD54              54
#define AWBL_SDIO_CMD55              55
#define AWBL_SDIO_CMD56              56
#define AWBL_SDIO_CMD57              57
#define AWBL_SDIO_CMD57              57
#define AWBL_SDIO_CMD58              58
#define AWBL_SDIO_CMD59              59

#define AWBL_SDIO_ACMD6              6
#define AWBL_SDIO_ACMD13             13
#define AWBL_SDIO_ACMD14             14
#define AWBL_SDIO_ACMD15             15
#define AWBL_SDIO_ACMD16             16
#define AWBL_SDIO_ACMD22             22
#define AWBL_SDIO_ACMD23             23
#define AWBL_SDIO_ACMD28             28
#define AWBL_SDIO_ACMD41             41
#define AWBL_SDIO_ACMD42             42
#define AWBL_SDIO_ACMD51             51

/** @} */

/**
 * \name SD mode R1 response
 * \anchor grp_awbl_sd_r1_rsp
 * @{
 */


/** \brief command argument out of range */
#define AWBL_SD_R1_OUT_OF_RANGE          (1 << 31)

/** \brief address error */
#define AWBL_SD_R1_ADDRESS_ERROR         (1 << 30)

/** \brief not support block length or transmit mismatch to the block length */
#define AWBL_SD_R1_BLOCK_LEN_ERROR       (1 << 29)

/** \brief erase sequence error */
#define AWBL_SD_R1_ERASE_SEQ_ERROR       (1 << 28)

/** \brief the block write or erase is invalid */
#define AWBL_SD_R1_ERASE_PARAM           (1 << 27)

/** \brief the block write is protected */
#define AWBL_SD_R1_WP_VIOLATION          (1 << 26)

/** \brief indicate whether the card is locked */
#define AWBL_SD_R1_CARD_IS_LOCKED        (1 << 25)

/** \brief indicate in the lock/unlock command��password verify error */
#define AWBL_SD_R1_LOCK_UNLOCK_FAILED    (1 << 24)

/** \brief CRC error */
#define AWBL_SD_R1_COM_CRC_ERROR         (1 << 23)

/** \brief ECC failed */
#define AWBL_SD_R1_CARD_ECC_FAILED       (1 << 21)

/** \brief card host error */
#define AWBL_SD_R1_CC_ERROR              (1 << 20)

/** \brief unknown error */
#define AWBL_SD_R1_ERROR                 (1 << 19)

#define AWBL_SD_R1_UNDERRUN              (1 << 18)
#define AWBL_SD_R1_OVERRUN               (1 << 17)

/** \brief CID/CSD overwrite */
#define AWBL_SD_R1_CID_CSD_OVERWRITE     (1 << 16)

/** \brief skip write protected area */
#define AWBL_SD_R1_WP_ERASE_SKIP         (1 << 15)

/** \brief indicate whether card ECC is forbid */
#define AWBL_SD_R1_CARD_ECC_DISABLED     (1 << 14)

#define AWBL_SD_R1_ERASE_RESET           (1 << 13)
#define AWBL_SD_R1_STATUS(x)             (x & 0xFFFFE000)

/** \brief current state */
#define AWBL_SD_R1_CURRENT_STATE(x)      ((x & 0x00001E00) >> 9)

/** \brief bus can transmit data */
#define AWBL_SD_R1_READY_FOR_DATA        (1 << 8)
#define AWBL_SD_R1_SWITCH_ERROR          (1 << 7)
#define AWBL_SD_R1_EXCEPTION_EVENT       (1 << 6)
/** \brief indicate whether the card into APP CMD mode */
#define AWBL_SD_R1_APP_CMD               (1 << 5)
#define AWBL_SD_R1_ALL_ERROR             0xF9F90000

/** @} */


/**
 * \name card status of R1 on SD mode
 * \anchor grp_awbl_sd_r1_state
 * @{
 */

#define AWBL_SD_R1_STATE_IDLE   0
#define AWBL_SD_R1_STATE_READY  1
#define AWBL_SD_R1_STATE_IDENT  2
#define AWBL_SD_R1_STATE_STBY   3
#define AWBL_SD_R1_STATE_TRAN   4
#define AWBL_SD_R1_STATE_DATA   5
#define AWBL_SD_R1_STATE_RCV    6
#define AWBL_SD_R1_STATE_PRG    7
#define AWBL_SD_R1_STATE_DIS    8

/** @} */


/**
 * \name card status of R1 on SPI mode
 * \anchor grp_awbl_spi_r1_state
 * @{
 */

/** \brief ָʾ���Ƿ��ڿ���״̬ */
#define AWBL_SD_SPI_R1_IN_IDLE_STATE       0x01

/** \brief ��������λ */
#define AWBL_SD_SPI_R1_ERASE_RESET         0x02

/** \brief ���ڵ�ǰ��״̬Ϊ�Ƿ�����  */
#define AWBL_SD_SPI_R1_ILLEGAL_CMD         0x04

/** \brief CRCУ��ʧ�� */
#define AWBL_SD_SPI_R1_COM_CRC_ERROR       0x08

/** \brief �������� */
#define AWBL_SD_SPI_R1_ERASE_SEQ_ERROR     0x10

/** \brief ��ַ���󣬷Ƕ�������ݳ��Ȳ�ƥ�� */
#define AWBL_SD_SPI_R1_ADDRESS_ERROR       0x20

/** \brief ����������������β��Ϸ�  */
#define AWBL_SD_SPI_R1_PARA_ERROR          0x40
#define AWBL_SD_SPI_R1_ALL_ERROR           0x7E
#define AWBL_SD_SPI_R1_MASK                0x7F

/** @} */



/**
 * \name SPIģʽ R2״̬
 * \anchor grp_awbl_spi_r2_state
 * @{
 */

/** \brief �������� */
#define AWBL_SD_SPI_R2_CARD_LOCKED         0x01

/** \brief ����д����,���� */
#define AWBL_SD_SPI_R2_WP_ERASE_SKIP       0x02

/** \brief LOCK��UNLOCK����ʧ�� */
#define AWBL_SD_SPI_R2_LOCK_UNLOCK_FAIL    0x02

/** \brief Ϊֹ���� */
#define AWBL_SD_SPI_R2_ERROR               0x04

/** \brief ���ڲ����������� */
#define AWBL_SD_SPI_R2_CC_ERROR            0x08

/** \brief ECC���� */
#define AWBL_SD_SPI_R2_CARD_ECC_ERROR      0x10

/** \brief ��д�������������д���� */
#define AWBL_SD_SPI_R2_WP_VIOLATION        0x20

/** \brief ����������Ч������Ч������� */
#define AWBL_SD_SPI_R2_ERASE_PARAM         0x40

/** \brief    */
#define AWBL_SD_SPI_R2_OUT_OF_RANGE        0x80
#define AWBL_SD_SPI_R2_CSD_OVERWRITE       0x80

/** @} */




/**
 * \name OCR�Ĵ���
 * \anchor grp_awbl_sd_ocr_range
 * @{
 */

/** \brief VDD ��ѹ 1.65 - 1.95 */
#define AWBL_SD_OCR_VDD_165_195    0x00000080

/** \brief VDD ��ѹ 2.0 ~ 2.1 */
#define AWBL_SD_OCR_VDD_20_21      0x00000100

/** \brief VDD ��ѹ 2.1 ~ 2.2 */
#define AWBL_SD_OCR_VDD_21_22      0x00000200

/** \brief VDD ��ѹ 2.2 ~ 2.3 */
#define AWBL_SD_OCR_VDD_22_23      0x00000400

/** \brief VDD ��ѹ 2.3 ~ 2.4 */
#define AWBL_SD_OCR_VDD_23_24      0x00000800

/** \brief VDD ��ѹ 2.4 ~ 2.5 */
#define AWBL_SD_OCR_VDD_24_25      0x00001000

/** \brief VDD ��ѹ 2.5 ~ 2.6 */
#define AWBL_SD_OCR_VDD_25_26      0x00002000

/** \brief VDD ��ѹ 2.6 ~ 2.7 */
#define AWBL_SD_OCR_VDD_26_27      0x00004000

/** \brief VDD ��ѹ 2.7 ~ 2.8 */
#define AWBL_SD_OCR_VDD_27_28      0x00008000

/** \brief VDD ��ѹ 2.8 ~ 2.9 */
#define AWBL_SD_OCR_VDD_28_29      0x00010000

/** \brief VDD ��ѹ 2.9 ~ 3.0 */
#define AWBL_SD_OCR_VDD_29_30      0x00020000

/** \brief VDD ��ѹ 3.0 ~ 3.1 */
#define AWBL_SD_OCR_VDD_30_31      0x00040000

/** \brief VDD ��ѹ 3.1 ~ 3.2 */
#define AWBL_SD_OCR_VDD_31_32      0x00080000

/** \brief VDD ��ѹ 3.2 ~ 3.3 */
#define AWBL_SD_OCR_VDD_32_33      0x00100000

/** \brief VDD ��ѹ 3.3 ~ 3.4 */
#define AWBL_SD_OCR_VDD_33_34      0x00200000

/** \brief VDD ��ѹ 3.4 ~ 3.5 */
#define AWBL_SD_OCR_VDD_34_35      0x00400000

/** \brief VDD ��ѹ 3.5 ~ 3.6 */
#define AWBL_SD_OCR_VDD_35_36      0x00800000

/** \brief 1.8V switching request */
#define AWBL_SD_OCR_S18R           0x01000000

/** \brief SDXC power control */
#define AWBL_SD_OCR_XPC            0x10000000

/** \brief Card Capacity Status (CCS)1 */
#define AWBL_SD_OCR_CCS            0x40000000

/** @} */


/**
 * \brief get cardinfo
 */
#define AWBL_DRVINFO_TO_CARDINFO_DECL(p_dev_drvinfo, p_drvinfo) \
        struct awbl_sdio_dev_drvinfo *p_dev_drvinfo =  \
            (struct awbl_sdio_dev_drvinfo *)AW_CONTAINER_OF( \
                    p_drvinfo,  \
                    struct awbl_sdio_dev_drvinfo,   \
                    super)


#define AWBL_SDIO_HOST_TO_HOST_INFO_DECL(p_info, p_host) \
        awbl_sdio_host_info_t *p_info = \
        (awbl_sdio_host_info_t *)AWBL_DEVINFO_GET( \
            &(p_host->super.super))

/**
 * \brief �õ��豸p_card��������Ϣ
 */
#define AWBL_SDIO_CARD_TO_HOST_INFO_DECL(p_info, p_card) \
        struct awbl_sdio_host_info *p_info = \
            (struct awbl_sdio_host_info *)AWBL_DEVINFO_GET( \
            &(((struct awbl_sdio_device *)p_card)->p_host->super.super))

/**
 * \brief �õ��豸p_card��HOST
 */
#define AWBL_SDIO_CARD_TO_HOST_DECL(p_host, p_card) \
        struct awbl_sdio_host *p_host = \
            ((struct awbl_sdio_device *)p_card)->p_host



/**
 * \brief �õ��豸Memory��p_card
 */
#define AWBL_SDIO_MEM_TO_CARD_DECL(p_sdio, p_memory) \
        struct awbl_sdio_device *p_sdio = \
            ((struct awbl_sdio_mem *)p_memory)->p_card

/**
 * \brief �õ��豸Sdio��p_card
 */
#define AWBL_SDIO_SDIO_TO_CARD_DECL(p_sdio, p_func) \
        struct awbl_sdio_device *p_sdio = \
            ((struct awbl_sdio_func *)p_func)->p_card

/**
 * \brief ��ȡMemory�豸
 */
#define AWBL_DEV_TO_MEM_DECL(p_card, p_awdev) \
        struct awbl_sdio_mem *p_card = \
                (struct awbl_sdio_mem *)AW_CONTAINER_OF( \
                    p_awdev, struct awbl_sdio_mem, super)

/**
 * \brief ��ȡSDIO��FUNC����
 */
#define AWBL_DEV_TO_FUNC_DECL(p_func, p_awdev) \
        struct awbl_sdio_func *p_func = \
            (struct awbl_sdio_func *)AW_CONTAINER_OF( \
                    p_awdev, struct awbl_sdio_func, super)


/**
 * \brief ��ȡSDIO��������Ϣ
 */
#define AWBL_FUNC_TO_SDIOCARD_INFO_DECL(p_cardinfo, p_func) \
    struct awbl_sdio_card_info *p_cardinfo =    \
        (struct awbl_sdio_card_info *)AW_CONTAINER_OF(  \
                p_func->super.p_driver, \
                struct awbl_sdio_card_info, \
                super)


/**
 * \brief ��ȡhost������Ϣ
 */
#define AWBL_HOST_TO_HSOST_DRV_INFO_DECL(p_drvinfo, p_host) \
    struct awbl_sdio_host_drvinfo *p_drvinfo =  \
        (struct awbl_sdio_host_drvinfo *)AW_CONTAINER_OF(  \
                p_host->super.super.p_driver,   \
                struct awbl_sdio_host_drvinfo,  \
                super)

/** \brief SDIO���߿�� */
typedef enum awbl_sdio_bus_width {
    AWBL_SDIO_BUS_WIDTH_1B, /**< \brief 1λ������ */
    AWBL_SDIO_BUS_WIDTH_4B, /**< \brief 4λ������ */
    AWBL_SDIO_BUS_WIDTH_8B, /**< \brief 8λ������ */
} awbl_sdio_bus_width_t;



/** \brief command */
typedef struct awbl_sdio_cmd{
    struct aw_list_head  cmd_node;
    uint32_t             cmd;        /**< \brief SDIO ����*/
    uint32_t             arg;        /**< \brief ������� */
    uint8_t              opt;        /**< \brief ������� */
#define AWBL_SDIO_NO_DATA       0
#define AWBL_SDIO_RD_DATA       1
#define AWBL_SDIO_WR_DATA       2

    void                *p_data;     /**< \brief ���ݻ����� */
    uint32_t             blk_size;   /**< \brief ������С */
    uint32_t             nblock;     /**< \brief ��������� */
    uint8_t              rsp_type;   /**< \brief ������Ӧ���� */
#define AWBL_SDIO_RSP_NONE      0
#define AWBL_SDIO_RSP_R1        1
#define AWBL_SDIO_RSP_R1B       2
#define AWBL_SDIO_RSP_R2        3
#define AWBL_SDIO_RSP_R3        4
#define AWBL_SDIO_RSP_R4        5
#define AWBL_SDIO_RSP_R5        6
#define AWBL_SDIO_RSP_R6        7
#define AWBL_SDIO_RSP_R7        8

    uint32_t            *p_rsp;      /**< \brief ��Ӧ���� */
    uint8_t              retries;    /**< \brief ��ʱ�ط����� */
}awbl_sdio_cmd_t;

/** \brief message */
typedef struct awbl_sdio_msg {
    struct aw_list_head  msg_node;
    struct aw_list_head  cmd_list;
    int                  status;            /**< \brief ���״̬  */
    void                *p_arg;             /**< \brief �ص��������� */
    void (*pfn_complete) (void *p_arg);     /**< \brief ������ɻص�����*/
}awbl_sdio_msg_t;


/** \brief SDIO���жϣ��ж϶�����������1���ã�SDIO�������߿��пɲ����ж�  */
typedef void (*aw_pfn_sdio_ext_isr_t) (void *p_arg);



//#define SDIO_TRANS_ERRCNT_ENABLE

#define SDIO_DETECT_TASK_PRIO              6
#define SDIO_DETECT_TASK_STACK_SIZE        4096

#ifdef SDIO_TRANS_ERRCNT_ENABLE
    #define SDIO_TRANS_ERRCNT_LIMIT        10
#endif

/** \brief SDIO HOST ������  */
typedef struct awbl_sdio_host {
    struct awbl_busctlr             super;
    struct awbl_sdio_device        *p_card;

    uint8_t                         state;      /**< \brief SDIO device state */
#define  AW_SDIO_DEV_STAT_DEVREGIST    (1 << 0) /**< \brief had registered*/
#define  AW_SDIO_DEV_STAT_UNINSTALL    (1 << 1) /**< \brief dev uninstalled */
    aw_bool_t                          changed; /**< \brief is changed */
    aw_bool_t                          removed; /**< \brief is removed */
#ifdef SDIO_TRANS_ERRCNT_ENABLE
    uint8_t                         err_cnt;        /**< \brief error cnt */
#endif
    AW_TASK_DECL(                   detect_task,    /**< \brief ��ʶ������ */
                                    SDIO_DETECT_TASK_STACK_SIZE);
    aw_spinlock_isr_t               dev_lock;       /**< \brief �豸�� */
    AW_MUTEX_DECL(                  dev_mux);       /**< \brief �����ź��� */
    aw_bool_t                          lock;        /**< \brief ����־ */
    struct aw_list_head             msg_list;       /**< \brief ��Ϣ���� */

#if 0 
    /** \brief SDIO �����жϻص����ж����������߸��� */
    aw_pfn_sdio_ext_isr_t    pfn_irq;
    /** \brief SDIO �ص��������� */
    void                     *p_irq_arg;
#endif 

}awbl_sdio_host_t;



/** \brief SDIO_host����ע����Ϣ  */
typedef struct awbl_sdio_host_drvinfo {
    struct awbl_drvinfo      super;

    /** \brief ��Ϣ���亯�� */
    aw_err_t (*xfr) (struct awbl_sdio_host *p_host);

    /** \brief ������������ */
    uint32_t (*set_speed) (struct awbl_sdio_host *p_host,
                           uint32_t               speed);

    /** \brief �������߿�� */
    aw_err_t (*set_bus_width) (struct awbl_sdio_host *p_host,
                               awbl_sdio_bus_width_t  width);

    /** \brief ���һ��ʱ�Ӳ��� */
    aw_err_t (*dummy_clks) (struct awbl_sdio_host *p_host,
                            uint8_t                nclks);

    /** \brief ��λ������ */
    void     (*host_reset) (struct awbl_sdio_host *p_host);

    /** \brief ��ȡ�˿�״̬ */
    aw_err_t (*status_get)(struct awbl_sdio_host *p_host,
                           uint8_t               *p_status);

    /** \brief ������״̬ */
#define AWBL_SDIO_CD_PLUG       0x01   /**< \brief ������ */
#define AWBL_SDIO_CD_STUBBS     0x02   /**< \brief ���γ� */

    /** \brief ��ֻ��״̬ */
#define AWBL_SDIO_READ_ONLY     0x10   /**< \brief ֻ�� */
#define AWBL_SDIO_WRITE_EN      0x20   /**< \brief дʹ�� */
}awbl_sdio_host_drvinfo_t;



/** \brief SDIO host ��Ϣ  */
typedef struct awbl_sdio_host_info {
    uint8_t                 bus_index;        /**< \brief ����ID */
    uint8_t                 mode;             /**< \brief ����ģʽ */
#define AWBL_SDIO_SPI_M               0       /**< \brief SDIO SPIģʽ */
#define AWBL_SDIO_SD_1B_M             1       /**< \brief SDIO SD 1��ģʽ */
#define AWBL_SDIO_SD_4B_M             2       /**< \brief SDIO SD 4��ģʽ */
#define AWBL_SDIO_SD_8B_M             3       /**< \brief SDIO SD 8��ģʽ */

    uint8_t                 connect_type;
#define AWBL_SDIO_CON_DEV_STATIC      0       /**< \brief ���Ӿ�̬�豸 */
#define AWBL_SDIO_CON_DEV_DYNAMIC     1       /**< \brief ���Ӷ�̬�豸 */

    aw_bool_t                  crc_en;      /**< \brief CRCУ��ʹ�� */
    aw_bool_t                  dbl_rate_en; /**< \brief ����ģʽʹ�� */
    uint32_t                ocr_valid;      /**< \brief ֧�ֵ�ORC����ѹ��Χ�� */
}awbl_sdio_host_info_t;





/** \brief SDIO CIS tuple  */
struct sdio_func_tuple {
    struct sdio_func_tuple *next;
    uint8_t                 code;
    uint8_t                 size;
    
    /** \brief C99 flexible array(incomplete type) */
    uint8_t                 data[]; 
#if 0     
    uint8_t                 data[0]; /**< \brief GNU C zero length array */
#endif 
};



/** \brief CID , only in memory card */
typedef struct awbl_sdcard_cid {
    uint32_t    mid;
    uint16_t    oid;
    char        pnm[8];
    uint8_t     prv;
    uint32_t    psn;
    uint16_t    year;
    uint8_t     month;
    uint8_t     hwrev;
    uint8_t     fwrev;
}awbl_sdio_card_cid_t;



/** \brief SCR register, only in memory card */
typedef struct awbl_sdcard_scr {
    uint8_t             structure;          /**< \brief structure version */
    uint8_t             sda_vsn;            /**< \brief SDA version */
    uint8_t             sda_spec3;
    uint8_t             bus_widths;         /**< \brief bus width */
#define AW_SD_SCR_BUS_WIDTH_1  (1<<0)
#define AW_SD_SCR_BUS_WIDTH_4  (1<<2)
    uint8_t             cmds;               /**< \brief CMD support */
#define AW_SD_SCR_CMD20_SUPPORT   (1<<0)
#define AW_SD_SCR_CMD23_SUPPORT   (1<<1)
    uint8_t             erase_value;        /**< \brief erase_value */
}awbl_sdio_card_scr_t;


/** \brief SSR register, only in memory card */
typedef struct awbl_sdcard_ssr {
    uint32_t            au_size;            /**< \brief In sectors */
    uint32_t            erase_timeout;      /**< \brief In milliseconds */
    uint32_t            erase_offset;       /**< \brief In milliseconds */
}awbl_sdio_card_ssr_t;


/** \brief CSD register, only in memory card */
typedef struct awbl_sdcard_csd {
    uint8_t     mmca_vsn;                   /**< \brief MMC version */
    uint32_t    max_tr_speed;               /**< \brief MAX transfer speed */
    uint16_t    cmd_class;                  /**< \brief command class  */
    uint16_t    sector_size;                /**< \brief sector size */
    uint32_t    sector_cnt;                 /**< \brief sector count */
    uint16_t    block_size;                 /**< \brief block size */
}awbl_sdcard_csd_t;


/** \brief card memory function information(sd/mmc or combo card) */
typedef struct awbl_sdio_mem_card_info {
    uint32_t    ocr;                        /**< \brief OCR */
    uint32_t    rca;                        /**< \brief address */
    uint16_t    attribute;                  /**< \brief attribute */

    struct awbl_sdcard_csd csd;             /**< \brief CSD registers */
    struct awbl_sdcard_ssr ssr;             /**< \brief SSR registers */
    struct awbl_sdcard_cid cid;             /**< \brief CID registers */
    struct awbl_sdcard_scr scr;             /**< \brief SCR registers */
}awbl_sdio_mem_card_info_t;


typedef struct awbl_sdio_cccr {
    uint8_t         cccr_vsn;
    uint8_t         sdio_vsn;
    uint8_t         sd_vsn;

    uint32_t        multi_block :1,  /**< \brief SMB */
                    low_speed   :1,  /**< \brief LSC */
                    wide_bus    :1,  /**< \brief S8B */
                    high_power  :1,  /**< \brief SMPC */
                    high_speed  :1,  /**< \brief SHS */
                    disable_cd  :1;
}awbl_sdio_cccr_t;


typedef struct awbl_sdio_cis {
    
    /** \brief SDIO Card manufacturer code  (0x20) */
    uint16_t    tplmid_manf;         
    
    /** \brief manufacturer information     (0x20) */
    uint16_t    tplmid_card;         
    
    /** \brief function max block size      (0x22) */
    uint16_t    tplfe_max_blk_size;  
    
    /** \brief max block size, only in func0(0x22) */
    uint32_t    tplfe_max_speed;     
}awbl_sdio_cis_t;


/**
 * \brief sdio card bfr register information
 */
typedef struct awbl_sdio_fbr {
    
    /** \brief Standard Function interface code */
    uint8_t interface;            
    
    /** \brief extension of the SDIO Standard Function */
    uint8_t  ext_interface_code;  
    
    /** \brief csa suport */
    aw_bool_t   csa_support;         
    
    /** \brief func cis data addr */
    uint32_t cis_addr;            
    
    /** \brief pointer to the desired byte in the CSA */
    uint32_t csa_addr;            
    
    /** \brief func csa data addr */
    uint8_t  csa_access_window;   
    
    /** \brief function block size */
    uint16_t blk_size;            
}awbl_sdio_fbr_t;


/**
 * \brief io function information(sdio card or combo card)
 */
typedef struct awbl_sdio_io_card_info {
    uint32_t    ocr;            /**< \brief OCR support */
    uint32_t    rca;            /**< \brief address */
    uint16_t    attribute;      /**< \brief attribute */

    awbl_sdio_cccr_t cccr;      /**< \brief CCCR registers */
    awbl_sdio_cis_t  cis;       /**< \brief CIS  registers */
    awbl_sdio_fbr_t  fbr;       /**< \brief FBR  registers */
}awbl_sdio_io_card_info_t;



/** \brief sdio device information */
typedef struct awbl_sdio_dev_info {
    uint8_t      type;

    uint8_t      class;
    uint16_t     vendor;
    uint16_t     device;

    void        *p_drv;
    void       (*pfn_plfm_init)(void);
}awbl_sdio_dev_info_t;


/** \brief SDIO memory device  */
typedef struct awbl_sdio_mem {
    struct awbl_dev             super;
    struct awbl_sdio_device    *p_card;     /**< \brief card that function in */
    awbl_sdio_mem_card_info_t   mem_info;   /**< \brief memory information */

    uint32_t                    raw_cid[4]; /**< \brief raw card CID */
    uint32_t                    raw_csd[4]; /**< \brief raw card CSD */
    uint32_t                    raw_scr[2]; /**< \brief raw card SCR */

    uint8_t                     name[32];

    uint16_t                    num_info;  /**< \brief number of info strings */
    const uint8_t             **info;      /**< \brief info strings */
    struct sdio_func_tuple     *tuples;
    struct awbl_devhcf          devhcf;   /**< \brief hardware configure */
    struct awbl_sdio_dev_info   devinfo;  /**< \brief sdio device information */
    void                       *p_drv;
}awbl_sdio_memory_t;



#define AWBL_SDIO_MAX_FUNC  7

/** \brief SDIO function device  */
typedef struct awbl_sdio_func {
    struct awbl_dev             super;
    struct awbl_sdio_device    *p_card;     /**< \brief card that function in */
    awbl_sdio_io_card_info_t    io_info;    /**< \brief private  information */
    uint8_t                     name[32];   /**< \brief function name   */
    uint8_t                     num;        /**< \brief function number */
    uint8_t                     class;      /**< \brief class of the function */
    
    
#define AWBL_SDIO_CLASS_MASK     0x0F  /**< \brief class mask */
#define AWBL_SDIO_CLASS_NONE     0x00  /**< \brief nonstandard interface */
#define AWBL_SDIO_CLASS_UART     0x01  /**< \brief UART standard interface */
#define AWBL_SDIO_CLASS_BT_A     0x02  /**< \brief Type-A bluetooth interface */
#define AWBL_SDIO_CLASS_BT_B     0x03  /**< \brief Type-B bluetooth interface */
#define AWBL_SDIO_CLASS_GPS      0x04  /**< \brief GPS standard interface */
#define AWBL_SDIO_CLASS_CAMERA   0x05  /**< \brief Camera standard interface */
#define AWBL_SDIO_CLASS_PHS      0x06  /**< \brief PHS standard interface  */
#define AWBL_SDIO_CLASS_WLAN     0x07  /**< \brief WLAN standard interface  */
#define AWBL_SDIO_CLASS_ATA      0x08  /**< \brief Embedded SDIO-ATA interface */

/** \brief Type-A bluetooth AMP interface */
#define AWBL_SDIO_CLASS_BT_AMP   0x09  

/** \brief Not assigned, reserved for future use */
#define AWBL_SDIO_CLASS_EXT      0x0F  

    struct awbl_sdio_cis        cis;       /**< \brief Function 0 CIS*/
    uint16_t                    num_info;  /**< \brief number of info strings */
    const uint8_t             **info;      /**< \brief info strings */
    struct sdio_func_tuple     *tuples;
    struct awbl_sdio_dev_info   devinfo;  /**< \brief sdio device information */
    struct awbl_devhcf          devhcf;   /**< \brief hardware configure */
    void                       *p_drv;

}awbl_sdio_function_t;



/** \brief SDIO device */
typedef struct awbl_sdio_device {
    struct awbl_sdio_host      *p_host;     /**< \brief host that the card in */
    struct awbl_sdio_mem       *p_sdcard;   /**< \brief sdcard */
    struct awbl_sdio_func      *p_iofunc[AWBL_SDIO_MAX_FUNC];
    awbl_sdio_io_card_info_t    io_info;    /**< \brief io common information */

    uint32_t                    rca;            /**< \brief RCA */
    uint32_t                    ocr;            /**< \brief OCR */
    uint8_t                     type;           /**< \brief type of SDIO card */
#define AWBL_SDIO_TYPE_MMC                   0  /**< \brief MMC */
#define AWBL_SDIO_TYPE_SD                    1  /**< \brief SD */
#define AWBL_SDIO_TYPE_IO                    2  /**< \brief SDIO */
#define AWBL_SDIO_TYPE_COMBO                 3  /**< \brief COMBO */
#define AWBL_SDIO_TYPE_ILLEGAL               4  /**< \brief ILLEGAL */

    uint8_t                     name[32];       /**< \brief name of the card */
    uint8_t                     func_num;       /**< \brief function num  */

    uint16_t                    num_info;       /**< \brief number of info */
    const uint8_t             **info;           /**< \brief info strings */
    struct sdio_func_tuple     *tuples;         /**< \brief tuples */

    struct aw_refcnt            ref;            /**< \brief ref */

}awbl_sdio_device_t;




AWBL_METHOD_IMPORT(awbl_sdio_match_type);


struct sdio_io_dev_id {
    uint8_t      class;         /**< \brief Standard interface or SDIO_ANY_ID */
    uint16_t     vendor;        /**< \brief Vendor or SDIO_ANY_ID */
    uint16_t     device;        /**< \brief Device ID or SDIO_ANY_ID */
    void        *driver_data;   /**< \brief Data private to the driver */
};


/** \brief SDIO device driver information   */
struct awbl_sdio_dev_drvinfo {
    struct awbl_drvinfo          super;
    uint8_t                      type;
    union {
        const struct sdio_io_dev_id id_table;
    };
};

/** \brief SDIO timeout obj */
struct awbl_sdio_timeout_obj {
    aw_tick_t   ticks;
    aw_tick_t   timeout;
};


/**
 * \brief �豸���ü�����ؽӿ�
 * \note  �ϲ�����ʹ�ô��豸ʱ��Ҫ�Ȼ�ȡ���ü�����ʹ������ͷ����ü���
 */
aw_err_t aw_sdio_mem_refcnt_get(struct awbl_sdio_mem *p_sdcard);

aw_err_t aw_sdio_mem_refcnt_put(struct awbl_sdio_mem *p_sdcard);

aw_err_t aw_sdio_func_refcnt_get(struct awbl_sdio_func *p_func);

aw_err_t aw_sdio_func_refcnt_put(struct awbl_sdio_func *p_func);


/**
 * \brief set timeout
 * \param[in] t     timeout obj
 * \param[in] ms    ms to timeout
 */
aw_local aw_inline void awbl_sdio_timeout_set (struct awbl_sdio_timeout_obj *p_t,
                                               uint32_t                      ms)
{
    p_t->timeout = aw_ms_to_ticks(ms);
    p_t->ticks   = aw_sys_tick_get();
}

/**
 * \brief check if timeout
 * \param[in] t     timeout obj
 */
aw_local aw_inline aw_bool_t awbl_sdio_timeout (struct awbl_sdio_timeout_obj *p_t)
{
    return ((aw_sys_tick_diff(p_t->ticks, aw_sys_tick_get())) >= p_t->timeout);
}

/**
 * \brief post a message to controller
 *
 * \param[in]   p_host       :  SDIO HOST
 * \param[in]   p_msg        :  message to post
 */
aw_local aw_inline void awbl_sdio_msg_in (struct awbl_sdio_host *p_host,
                                          struct awbl_sdio_msg  *p_msg)
{
    aw_spinlock_isr_take(&(p_host->dev_lock));
    aw_list_add_tail(&(p_msg->msg_node),
                     &(p_host->msg_list));
    p_msg->status = -AW_EISCONN;
    aw_spinlock_isr_give(&(p_host->dev_lock));
}

/**
 * \brief get a message from controller
 *
 * \param[in]   p_host       :  SDIO HOST
 *
 * \retval  message
 */
aw_local aw_inline awbl_sdio_msg_t *
awbl_sdio_msg_out (struct awbl_sdio_host *p_host)
{
    struct awbl_sdio_msg *p_msg = NULL;

    aw_spinlock_isr_take(&(p_host->dev_lock));
    if (!aw_list_empty(&(p_host->msg_list))) {

        p_msg = (struct awbl_sdio_msg*)
                    aw_list_first_entry(
                        &p_host->msg_list,
                        struct awbl_sdio_msg,
                        msg_node);
        aw_list_del(&p_msg->msg_node);
    }
    aw_spinlock_isr_give(&(p_host->dev_lock));
    return p_msg;
}

/**
 * \brief check if message empty
 *
 * \param[in]   p_host         :  SDIO HOST
 */
aw_local aw_inline int awbl_sdio_msg_empty (struct awbl_sdio_host *p_host)
{
    return aw_list_empty_careful(&p_host->msg_list);
}

/**
 * \brief config command
 * \param p_cmd     CMD object
 * \param cmd       CMD number
 * \param arg       ARG in CMD
 * \param opt       read or write
 * \param p_data    data to be transfered
 * \param blk_size  block size of data
 * \param nblk      block count
 * \param rsp_type  response type
 * \param retries   retry times
 */
aw_local aw_inline void aw_sdio_config_cmd (struct awbl_sdio_cmd *p_cmd,
                                            uint8_t               cmd,
                                            uint32_t              arg,
                                            uint8_t               opt,
                                            void                 *p_data,
                                            uint32_t              blk_size,
                                            uint32_t              nblk,
                                            uint8_t              rsp_type,
                                            void                 *p_rsp,
                                            uint8_t               retries)
{
    p_cmd->cmd      = cmd;
    p_cmd->arg      = arg;
    p_cmd->opt      = opt;
    p_cmd->p_data   = p_data;
    p_cmd->blk_size = blk_size;
    p_cmd->nblock   = nblk;
    p_cmd->rsp_type = rsp_type;
    p_cmd->p_rsp    = p_rsp;
    p_cmd->retries  = retries;
}

/**
 * \brief initialize message
 * \param p_msg     message object
 */
aw_local aw_inline void aw_sdio_msg_init (struct awbl_sdio_msg  *p_msg)
{
    memset(p_msg, 0, sizeof(*p_msg));

    AW_INIT_LIST_HEAD(&(p_msg->cmd_list));

    p_msg->status = -AW_ENOTCONN;
}

/**
 * \brief add command
 * \param p_msg     message object
 * \param p_cmd     command to be added
 */
aw_local aw_inline void aw_sdio_cmd_add_tail (struct awbl_sdio_msg *p_msg,
                                              struct awbl_sdio_cmd *p_cmd)
{
    aw_list_add_tail(&p_cmd->cmd_node, &p_msg->cmd_list);
}

/**
 * \brief delete a command
 * \param p_cmd     command to be deleted
 */
aw_local aw_inline void aw_sdio_cmd_del (struct awbl_sdio_cmd *p_cmd)
{
    aw_list_del(&p_cmd->cmd_node);
}

/**
 * \brief create a host controller
 * \param p_host     host controller
 */
aw_err_t awbl_sdio_host_create (struct awbl_sdio_host  *p_host);


/**
 * \brief host port state change
 * \param p_host         host handle
 */
void awbl_sdio_port_change (struct awbl_sdio_host *p_host, uint8_t state);

/**
 * \brief host dynamic device reinit
 * \param p_host         host handle
 */
void awbl_sdio_host_dynamic_reinit (struct awbl_sdio_host *p_host);


/**
 * \brief reset host
 *
 * \param[in]  p_host       : SDIO host
 *
 * \retval AW_OK            : success
 * \retval -AW_EINVAL       : parameter err
 * \retval -AW_ENOTSUP      : HOST can't support
 */
aw_err_t awbl_sdio_host_reset (struct awbl_sdio_host *p_host);

/**
 * \brief output clocks
 *
 * \param[in]  p_host       : SDIO host
 * \param[in]  clks         :  �����ʱ��������
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t awbl_sdio_host_dummy_clks (struct awbl_sdio_host *p_host,
                                    uint8_t                clks);

/**
 * \brief ����SDIO HOST �����ٶ�
 *
 * \param[in]  p_host       : SDIO host
 * \param[in]  speed        : �����ٶ�
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t awbl_sdio_host_set_speed (struct awbl_sdio_host *p_host,
                                   uint32_t               speed);

/**
 * \brief ����SDIO HOST ���߿��
 *
 * \param[in]  p_host       : SDIO host
 * \param[in]  width        : ���߿��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t awbl_sdio_host_set_bus_width (struct awbl_sdio_host *p_host,
                                       awbl_sdio_bus_width_t  width);

/**
 * \brief �첽��    ��һ��������Ϣ��SDIO HOST
 *
 * \param[in]  p_host       : SDIO host
 * \param[in]  p_msg        : SDIO��Ϣ
 * \param[in]  p_arg        : ��ʱʱ�䣨ms��
 * \param[in]  pfn_complete : ������Ϣ������ɻص�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t awbl_sdio_host_async (struct awbl_sdio_host *p_host,
                               struct awbl_sdio_msg  *p_msg,
                               void                  *p_arg,
                               void (*pfn_complete) (void *p_arg));

/**
 * \brief ͬ������һ��������Ϣ��SDIO HOST
 *
 * \param[in]   p_host      : SDIO host
 * \param[in]   p_msg       : SDIO��Ϣ
 * \param[in]   timeout     : ��ʱʱ�䣨ms��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t awbl_sdio_host_sync (struct awbl_sdio_host *p_host,
                              struct awbl_sdio_msg  *p_msg,
                              int                    timeout);

/**
 * [awbl_sdio_host_get_port_status  description]
 * @param  p_host   [description]
 * @param  p_status [description]
 * @return          [description]
 */
aw_err_t awbl_sdio_host_get_port_status (struct awbl_sdio_host *p_host,
                                         uint8_t               *p_status);


/**
 * \brief �����λ�洢�����������״̬(CMD0)
 *
 * \param[in]   p_card      : SDIO�豸
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_go_idle_state (struct awbl_sdio_device *p_card);

aw_err_t aw_sdio_host_go_idle_state (awbl_sdio_host_t *p_host);

/**
 * \brief Ӧ�ó����ʼ��(CMD8)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   ocr         : SDIO HOST֧�ֵ�OCR��Χ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_if_cond (struct awbl_sdio_device *p_card,
                               uint32_t                 ocr);

aw_err_t aw_sdio_host_send_if_cond (struct awbl_sdio_host *p_host,
                                    uint32_t                 ocr);

/**
 * \brief Ӧ�ó����ʼ��(CMD55 + ACMD41)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : ��Ե�ַ
 * \param[in]   ocr         : SDIO HOST֧�ֵ�OCR��Χ
 * \param[in]   p_rocr      : ��ȡ��SDIO�豸֧�ֵ�OCR��Χ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_app_op_cond (struct awbl_sdio_device *p_card,
                                   uint32_t                 rca,
                                   uint32_t                 ocr,
                                   uint32_t                *p_rocr);

aw_err_t aw_sdio_host_send_app_op_cond (struct awbl_sdio_host *p_host,
                                   uint32_t                 rca,
                                   uint32_t                 ocr,
                                   uint32_t                *p_rocr);
/**
 * \brief SD SPI ģʽ��ȡOCR֧��(CMD8)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   highcap     : VHS
 * \param[in]   p_ocrp      : ��ȡ��SD��֧�ֵ�OCR
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_spi_read_ocr (struct awbl_sdio_device *p_card,
                               uint8_t                  highcap,
                               uint32_t                *p_ocrp);

/**
 * \brief ��ȡ�洢����CID(CMD10)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : �洢������Ե�ַ
 * \param[in]   p_cid       : �Ĵ���cid����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_cid (struct awbl_sdio_device *p_card,
                           uint32_t                 rca,
                           uint32_t                *p_cid);

/**
 * \brief ��ȡ�洢�����е�CID(CMD2)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   p_cid       : �Ĵ���cid��������
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_all_send_cid (struct awbl_sdio_device *p_card,
                               uint32_t                *p_cid);

/**
 * \brief ��ȡ�洢����Ե�ַ(CMD3)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[out]  p_rca       : �洢�����ص�rcaֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_relative_addr (struct awbl_sdio_device *p_card,
                                     uint32_t                *p_rca);

/**
 * \brief ���ô洢����Ե�ַ(CMD3)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[out]  rca         : �Ĵ���rcaֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_mmc_set_relative_addr (struct awbl_sdio_device *p_card,
                                        uint32_t                 rca);

/**
 * \brief ��ȡ�洢���Ĵ���EXT_CSDֵ(CMD8)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[out]  p_ext_csd   : �Ĵ���ext_csdֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_mmc_send_ext_csd (struct awbl_sdio_device *p_card,
                                   uint32_t                *p_ext_csd);

/**
 * \brief ��ȡ�洢���Ĵ���CSDֵ(CMD9)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : �洢����Ե�ַ
 * \param[out]  p_csd       : �Ĵ���csdֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_csd (struct awbl_sdio_device *p_card,
                           uint32_t                 rca,
                           uint32_t                *p_csd);

/**
 * \brief ѡ��ָ����Ե�ַ�Ŀ�(CMD7)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : �洢����Ե�ַ
 * \param[out]  select      : �Ĵ���scrֵ��1 / 0��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_select_card (struct awbl_sdio_device *p_card,
                              uint32_t                 rca,
                              uint8_t                  select);
/**
 * \brief ��ȡ�洢���Ĵ���SCRֵ(CMD55 + ACMD51)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : �洢����Ե�ַ
 * \param[out]  p_scr       : �Ĵ���scrֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_app_send_scr (struct awbl_sdio_device *p_card,
                               uint32_t                 rca,
                               uint32_t                *p_scr);

/**
 * \brief ��ȡ�洢���Ĵ���SSRֵ(CMD55 + ACMD13)
 *
 * \param[in]   p_card      : SDIO�豸
 * \param[in]   rca         : �洢����Ե�ַ
 * \param[out]  p_ssr       : �Ĵ���ssrֵ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_app_sd_status (struct awbl_sdio_device *p_card,
                                uint32_t                 rca,
                                uint32_t                *p_ssr);

/**
 * \brief SD�洢����չ���ܲ���(CMD6)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  mode         : ����ģʽ
 * \param[in]  group        : ������
 * \param[in]  value        : ���õ�ֵ
 * \param[in]  p_resp       : �洢����Ӧ����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 *
 * \note CMD6��SDЭ��1.00��2.00����Ҫ�����л����������ģʽ��3.00���������Ķ��塣
 */
aw_err_t aw_sdio_sd_switch (struct awbl_sdio_device *p_card,
                            int                      mode,
                            int                      group,
                            uint8_t                  value,
                            uint8_t                 *p_resp);

/**
 * \brief MMC�洢����չ���ܲ���(CMD6)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  access       : ����ģʽ��0x3д�룩
 * \param[in]  index        : ��չCSD�Ĵ�����ַ
 * \param[in]  value        : ���üĴ�����ֵ
 * \param[in]  cmd_set      : ��������
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_mmc_switch (struct awbl_sdio_device *p_card,
                             uint8_t                  access,
                             uint8_t                  index,
                             uint8_t                  value,
                             uint8_t                  cmd_set);

/**
 * \brief MMC�洢����չ���ܲ���(CMD6)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  width        : ���߿��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_mmc_buswidth_switch (struct awbl_sdio_device *p_card,
                                      awbl_sdio_bus_width_t    width);


/**
 * \brief ʹ�ܴ洢��CRC(CMD59)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  crc_en       : CRCʹ�ܣ�enable/disable  1/0��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_spi_set_crc(struct awbl_sdio_device *p_card,
                             uint8_t                  crc_en);

aw_err_t aw_sdio_host_spi_set_crc(awbl_sdio_host_t *p_host,
                                  uint8_t           crc_en);

/**
 * \brief ���ô洢�����߿��(CMD55 + ACMD6)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  rca          : �洢����Ե�ַ
 * \param[out] width        : ���߿��
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_app_set_bus_width (struct awbl_sdio_device *p_card,
                                    uint32_t                 rca,
                                    awbl_sdio_bus_width_t    width);

/**
 * \brief ��ѹswitch��(CMD11)
 *
 * \param[in]  p_card       : SDIO�豸
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_voltage_witch (struct awbl_sdio_device *p_card);

/**
 * \brief ����HOST��ѹ֧��(CMD1)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  ocr          : SDIO HOST ֧�ֵ�OCR
 * \param[out] p_rocr       : �洢�����ص�OCR֧�ַ�Χ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_op_cond (struct awbl_sdio_device *p_card,
                               uint32_t                 ocr,
                               uint32_t                *p_rocr);

/**
 * \brief ���ô洢�����С(CMD16)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  blk_len      : ���С
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_set_blocklen (struct awbl_sdio_device *p_card,
                               uint32_t                 blk_len);

/**
 * \brief ��������(CMD17)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ���
 * \param[out] p_buf        : ���ݻ�����
 * \param[out] sector_size  : ���С
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_read_single_block (struct awbl_sdio_device *p_card,
                                    uint32_t                 sector,
                                    uint8_t                 *p_buf,
                                    uint16_t                 sector_size);

/**
 * \brief д������(CMD24)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ���
 * \param[out] p_buf        : ���ݻ�����
 * \param[out] sector_size  : ���С
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_write_single_block (struct awbl_sdio_device *p_card,
                                     uint32_t                 sector,
                                     uint8_t                 *p_buf,
                                     uint16_t                 sector_size);

/**
 * \brief ֹͣ����(CMD12)
 *
 * \param[in]  p_card       : SDIO�豸
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_stop_transmission (struct awbl_sdio_device *p_card);

/**
 * \brief ��ȡ��״̬(CMD13)
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  rca          : �洢����Ե�ַ
 * \param[out] p_status     : �洢��״̬
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_send_status (struct awbl_sdio_device *p_card,
                              uint32_t                 rca,
                              uint32_t                *p_status);

/**
 * \brief ͬ���Ӵ洢���ж�ȡN�����ݿ�(CMD18 + (CMD12))
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ��ʼ���
 * \param[in]  p_buf        : ���ݻ�����
 * \param[in]  sector_cnt   : �����
 * \param[in]  sector_size  : ���С
 * \param[out] stop_en      : ֹͣ����ʹ�ܣ�ʹ�ܺ�����ᷢ��ֹͣ����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_read_multiple_block (struct awbl_sdio_device *p_card,
                                      uint32_t                 sector,
                                      uint8_t                 *p_buf,
                                      uint32_t                 sector_cnt,
                                      uint16_t                 sector_size,
                                      uint8_t                  stop_en);

/**
 * \brief ͬ����洢����д��N�����ݿ�(CMD25 + (CMD12))
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ��ʼ���
 * \param[in]  p_buf        : ���ݻ�����
 * \param[in]  sector_cnt   : �����
 * \param[in]  sector_size  : ���С
 * \param[out] stop_en      : ֹͣ����ʹ�ܣ�ʹ�ܺ�����ᷢ��ֹͣ����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_write_multiple_block (struct awbl_sdio_device *p_card,
                                       uint32_t                 sector,
                                       uint8_t                 *p_buf,
                                       uint32_t                 sector_cnt,
                                       uint16_t                 sector_size,
                                       uint8_t                  stop_en);

/**
 * \brief �첽�Ӵ洢���ж�ȡ1�����ݿ�
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ��ʼ���
 * \param[in]  p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���С
 * \param[out] p_rsp        : ��Ӧ����
 * \param[in]  p_arg        : �ص���������
 * \param[in]  pfn_complete : ��ɻص�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_read_single_block_async (struct awbl_sdio_device *p_card,
                                          uint32_t                 sector,
                                          uint8_t                 *p_buf,
                                          uint16_t                 sector_size,
                                          uint32_t                *p_rsp,
                                          void                    *p_arg,
                                          void (*pfn_complete) (void *p_arg));


/**
 * \brief �첽��洢����д��1�����ݿ�
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  sector       : ��ʼ���
 * \param[in]  p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���С
 * \param[out] p_rsp        : ��Ӧ����
 * \param[in]  p_arg        : �ص���������
 * \param[in]  pfn_complete : ��ɻص�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_write_single_block_async (struct awbl_sdio_device *p_card,
                                           uint32_t                 sector,
                                           uint8_t                 *p_buf,
                                           uint16_t                 sector_size,
                                           uint32_t                *p_rsp,
                                           void                    *p_arg,
                                           void (*pfn_complete) (void *p_arg));



/**
 * [aw_sdio_static_sdio_enum  description]
 * @param  p_func [description]
 * @return        [description]
 */
aw_err_t aw_sdio_static_sdio_enum (struct awbl_sdio_func *p_func);

/**
 * [aw_sdio_host_send_status  description]
 * @param  p_host   [description]
 * @param  rca      [description]
 * @param  p_status [description]
 * @return          [description]
 */
aw_err_t aw_sdio_host_send_status (struct awbl_sdio_host *p_host, 
                                   uint32_t               rca, 
                                   uint32_t              *p_status);

/**
 * [aw_sdio_dynamic_dev_enumerate  description]
 * @param  p_host [description]
 * @return        [description]
 */
aw_err_t aw_sdio_dynamic_dev_enumerate (struct awbl_sdio_host *p_host);

/**
 * [aw_sdio_host_io_reset description]
 * @param  p_host [description]
 * @return        [description]
 */
aw_err_t aw_sdio_host_io_reset(awbl_sdio_host_t *p_host);

/**
 * [aw_sdio_host_send_op_cond  description]
 * @param  p_host [description]
 * @param  ocr    [description]
 * @param  p_rocr [description]
 * @return        [description]
 */
aw_err_t aw_sdio_host_send_op_cond (struct awbl_sdio_host *p_host, 
                                    uint32_t               ocr, 
                                    uint32_t              *p_rocr);

/**
 * [aw_sdio_static_memmory_enum  description]
 * @param  p_sdcard [description]
 * @return          [description]
 */
aw_err_t aw_sdio_static_memmory_enum (struct awbl_sdio_mem *p_sdcard);


/**
 * \name SDIO card register addr
 * \anchor grp_awbl_sdio_addr
 * @{
 */

/** \brief CCCR/SDIO Revision */
#define AWBL_SDIO_ADDR_CCCR_REV             ((uint32_t)0x00)

#define  SDIO_CCCR_REV_1_00  0  /* CCCR/FBR Version 1.00 */
#define  SDIO_CCCR_REV_1_10  1  /* CCCR/FBR Version 1.10 */
#define  SDIO_CCCR_REV_1_20  2  /* CCCR/FBR Version 1.20 */
#define  SDIO_CCCR_REV_3_00  3  /* CCCR/FBR Version 3.00 */

#define  SDIO_SDIO_REV_1_00  0  /* SDIO Spec Version 1.00 */
#define  SDIO_SDIO_REV_1_10  1  /* SDIO Spec Version 1.10 */
#define  SDIO_SDIO_REV_1_20  2  /* SDIO Spec Version 1.20 */
#define  SDIO_SDIO_REV_2_00  3  /* SDIO Spec Version 2.00 */
#define  SDIO_SDIO_REV_3_00  4  /* SDIO Spec Version 3.00 */

/** \brief SD Revision */
#define AWBL_SDIO_ADDR_CCCR_SDREV           ((uint32_t)0x01)

/** \brief I/O Enable */
#define AWBL_SDIO_ADDR_CCCR_IOEN            ((uint32_t)0x02)

/** \brief I/O Ready */
#define AWBL_SDIO_ADDR_CCCR_IORDY           ((uint32_t)0x03)

/** \brief Interrupt Enable */
#define AWBL_SDIO_ADDR_CCCR_INTEN           ((uint32_t)0x04)

/** \brief Interrupt Pending */
#define AWBL_SDIO_ADDR_CCCR_INTPEND         ((uint32_t)0x05)

/** \brief I/O Abort */
#define AWBL_SDIO_ADDR_CCCR_IOABORT         ((uint32_t)0x06)

/** \brief Bus Interface control */
#define AWBL_SDIO_ADDR_CCCR_BICTRL          ((uint32_t)0x07)

/** \brief Card Capabilities */
#define AWBL_SDIO_ADDR_CCCR_CAPABLITIES     ((uint32_t)0x08)

#define  SDIO_CCCR_CAP_SDC  0x01  /**< \brief can do CMD52 while data transfer */
#define  SDIO_CCCR_CAP_SMB  0x02  /**< \brief can do multi-block xfers (CMD53) */
#define  SDIO_CCCR_CAP_SRW  0x04  /**< \brief supports read-wait protocol */
#define  SDIO_CCCR_CAP_SBS  0x08  /**< \brief supports suspend/resume */
#define  SDIO_CCCR_CAP_S4MI 0x10  /**< \brief interrupt during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_E4MI 0x20  /**< \brief enable ints during 4-bit CMD53 */
#define  SDIO_CCCR_CAP_LSC  0x40  /**< \brief low speed card */
#define  SDIO_CCCR_CAP_4BLS 0x80  /**< \brief 4 bit low speed card */

/** \brief Common CIS Base Address Pointer Register 0 (LSB) */
#define AWBL_SDIO_ADDR_CCCR_CISPTR_0        ((uint32_t)0x09)

/** \brief Common CIS Base Address Pointer Register 1 */
#define AWBL_SDIO_ADDR_CCCR_CISPTR_1        ((uint32_t)0x0A)

/** \brief Common CIS Base Address Pointer Register 2 (MSB - only bit 1 valid)*/
#define AWBL_SDIO_ADDR_CCCR_CISPTR_2        ((uint32_t)0x0B)

/** \brief Bus Suspend Register */
#define AWBL_SDIO_ADDR_CCCR_BUSSUSP         ((uint32_t)0x0C)

/** \brief Function Select Register */
#define AWBL_SDIO_ADDR_CCCR_FUNCSEL         ((uint32_t)0x0D)

/** \brief Exec Flags Register */
#define AWBL_SDIO_ADDR_CCCR_EXECFLAGS       ((uint32_t)0x0E)

/** \brief Ready Flags Register */
#define AWBL_SDIO_ADDR_CCCR_RDYFLAGS        ((uint32_t)0x0F)

/** \brief Function 0 (Bus) SDIO Block Size Register 0 (LSB) */
#define AWBL_SDIO_ADDR_CCCR_BLKSIZE_0       ((uint32_t)0x10)

/** \brief Function 0 (Bus) SDIO Block Size Register 1 (MSB) */
#define AWBL_SDIO_ADDR_CCCR_BLKSIZE_1       ((uint32_t)0x11)

/** \brief Power Control */
#define AWBL_SDIO_ADDR_CCCR_POWER_CONTROL   ((uint32_t)0x12)
#define  SDIO_POWER_SMPC    0x01    /* Supports Master Power Control */
#define  SDIO_POWER_EMPC    0x02    /* Enable Master Power Control */

/** \brief Bus Speed Select  (control device entry into high-speed clocking mode)  */
#define AWBL_SDIO_ADDR_CCCR_SPEED_CONTROL   ((uint32_t)0x13)
#define  SDIO_SPEED_SHS     0x01    /* Supports High-Speed mode */
#define  SDIO_SPEED_EHS     0x02    /* Enable High-Speed mode */

/** \brief UHS-I Support */
#define AWBL_SDIO_ADDR_CCCR_UHS_I           ((uint32_t)0x14)

/** \brief Drive Strength */
#define AWBL_SDIO_ADDR_CCCR_DRIVE           ((uint32_t)0x15)

/** \brief Interrupt Extension */
#define AWBL_SDIO_ADDR_CCCR_INTEXT          ((uint32_t)0x16)


/** \brief Function x (Backplane) Info */
#define AWBL_SDIO_ADDR_INFO_FBR_F(x)          (0x0 + ((x) * 256))

/** \brief Function x Extended standard SDIO Function interface code */
#define AWBL_SDIO_ADDR_EXTFUNC_CODE_FBR_F(x)  (0x1 + ((x) * 256))

/** \brief Function x (Backplane) High Power */
#define AWBL_SDIO_ADDR_HP_FBR_F(x)            (0x2 + ((x) * 256))

/** \brief Function x (Backplane) CIS Base Address Pointer Register 0 (LSB) */
#define AWBL_SDIO_ADDR_CISPTR_FBR_F(x)        (0x9 + ((x) * 256))

/** \brief Function x (Backplane) CIS Base Address Pointer Register 0 (LSB) */
#define AWBL_SDIO_ADDR_CSAPTR_FBR_F(x)        (0xC + ((x) * 256))

/** \brief Data access window to Function 1 Code Storage Area */
#define AWBL_SDIO_ADDR_CSA_WINDOW_FBR_F(x)    (0xF + ((x) * 256))

/** \brief Function x (Backplane) SDIO Block Size Register 0 (LSB) */
#define AWBL_SDIO_ADDR_BLKSIZE_FBR_F(x)       (0x10 + ((x) * 256))

/** @} */


/**
 * \name SDIO card register bit
 * \anchor grp_awbl_sdio_reg_bit
 * @{
 */

/** \brief SDIO spec revision mask */
#define AWBL_SDIO_REV_SDIOID_MASK       ((uint32_t)0xF0)

/** \brief CCCR format version mask */
#define AWBL_SDIO_REV_CCCRID_MASK       ((uint32_t)0x0F)

/** \brief SD format version mask */
#define AWBL_SD_REV_PHY_MASK            ((uint32_t)0x0F)

/** \brief SDIOD_CCCR_IOEN Bits */
#define AWBL_SDIO_ENABLE_FUNC(x)        ((x) << (uint32_t)0x01)

/** \brief SDIOD_CCCR_IORDY Bits */
#define AWBL_SDIO_READY_FUNC(x)         ((x) << (uint32_t)0x01)

/** \brief SDIOD_CCCR_INTEN Bits */
#define AWBL_INTR_CTL_EN_FUNC(x)        ((x) << (uint32_t)0x01)

/** \brief SDIOD_CCCR_INTPEND Bits */
#define AWBL_INTR_STATUS_FUNC(x)        ((x) << (uint32_t)0x01)

/** \brief SDIOD_CCCR_IOABORT Bits */

/** \brief I/O card reset */
#define AWBL_IO_ABORT_RESET_ALL         ((uint32_t)0x08)

/** \brief abort selction: function x */
#define AWBL_IO_ABORT_FUNC_MASK         ((uint32_t)0x07)


/** \brief SDIOD_CCCR_BICTRL Bits */

/** \brief Card Detect disable */
#define AWBL_BUS_CARD_DETECT_DIS        ((uint32_t)0x80)

/** \brief support continuous SPI interrupt */
#define AWBL_BUS_SPI_CONT_INTR_CAP      ((uint32_t)0x40)

/** \brief continuous SPI interrupt enable */
#define AWBL_BUS_SPI_CONT_INTR_EN       ((uint32_t)0x20)

/** \brief bus width mask */
#define AWBL_BUS_SD_DATA_WIDTH_MASK     ((uint32_t)0x03)

/** \brief bus width 4-bit mode */
#define AWBL_BUS_SD_DATA_WIDTH_4BIT     ((uint32_t)0x02)

/** \brief bus width 1-bit mode */
#define AWBL_BUS_SD_DATA_WIDTH_1BIT     ((uint32_t)0x00)

/** \brief SDIOD_CCCR_CAPABLITIES Bits */

/** \brief 4-bit support for low speed card */
#define AWBL_SDIO_CAP_4BLS              ((uint32_t)0x80)

/** \brief low speed card */
#define AWBL_SDIO_CAP_LSC               ((uint32_t)0x40)

/** \brief enable interrupt between block of data in 4-bit mode */
#define AWBL_SDIO_CAP_E4MI              ((uint32_t)0x20)

/** \brief support interrupt between block of data in 4-bit mode */
#define AWBL_SDIO_CAP_S4MI              ((uint32_t)0x10)

/** \brief support suspend/resume */
#define AWBL_SDIO_CAP_SBS               ((uint32_t)0x08)

/** \brief support read wait */
#define AWBL_SDIO_CAP_SRW               ((uint32_t)0x04)

/** \brief support multi-block transfer */
#define AWBL_SDIO_CAP_SMB               ((uint32_t)0x02)

/** \brief Support Direct commands during multi-byte transfer */
#define AWBL_SDIO_CAP_SDC               ((uint32_t)0x01)


/** \brief SDIOD_CCCR_POWER_CONTROL Bits */
/** \brief supports master power control (RO) */
#define AWBL_SDIO_POWER_SMPC            ((uint32_t)0x01)

/** \brief enable master power control (allow > 200mA) (RW) */
#define AWBL_SDIO_POWER_EMPC            ((uint32_t)0x02)



/** \brief SDIOD_CCCR_SPEED_CONTROL Bits */

/** \brief supports high-speed [clocking] mode (RO) */
#define AWBL_SDIO_SPEED_SHS             ((uint32_t)0x01)

/** \brief enable high-speed [clocking] mode (RW) */
#define AWBL_SDIO_SPEED_EHS             ( (uint32_t)0x02 )

/** @} */


/**
 * \name CIS tulpe type
 * \anchor grp_awbl_sdio_cistpl
 * @{
 */

/** \brief Null tuple */
#define AWBL_SDIO_CISTPL_NULL                      0x00

/** \brief Checksum control */
#define AWBL_SDIO_CISTPL_CHECKSUM                  0x10

/** \brief Level 1 version/product-information */
#define AWBL_SDIO_CISTPL_VERS_1                    0x15

/** \brief The Alternate Language String Tuple */
#define AWBL_SDIO_CISTPL_ALTSTR                    0x16

/** \brief Manufacturer Identification String Tuple */
#define AWBL_SDIO_CISTPL_MANFID                    0x20

/** \brief Function Identification Tuple */
#define AWBL_SDIO_CISTPL_FUNCID                    0x21

/** \brief Function Extensions */
#define AWBL_SDIO_CISTPL_FUNCE                     0x22

/** \brief Additional information */
#define AWBL_SDIO_CISTPL_SDIO_STD                  0x91

/** \brief Reserved for future use with SDIO devices. */
#define AWBL_SDIO_CISTPL_SDIO_EXT                  0x92

/** \brief The End-of-chain Tuple */
#define AWBL_SDIO_CISTPL_END                       0xFF

/** @} */



/**
 * \brief SDIO������дһ���ֽڵ�����
 *
 * \param[in]  p_host    : SDIO����HOST
 * \param[in]  func_num  : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��SDIO
 *                            �����ж������
 * \param[in]  opt       : ������������д
 * \param[in]  ram_flg   : �鴫��ģʽʹ�ܣ���TRUEΪ�鴫�䣬��FALSEΪ�ֽڴ���
 * \param[in]  addr      : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ��ΧΪ
 *                          0x0~0x1ffff
 * \param[out] p_data    : ���ݻ�����
 *
 * \retval AW_OK         : ����ɹ�
 * \retval -AW_ENOMEM    : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED : ������ȡ��
 * \retval -AW_EIO       : SDIO����I/O����
 * \retval -AW_EBUSY     : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL    : ��������
 * \retval -AW_ETIME     : HOST����ʱ
 * \retval -AW_ENOTSUP   : HOST��֧�ָù���
 *
 * \note �˽ӿڴ������ݲ���ʹ�������ߣ�����ʹ��CMD�ߣ�������������δ��ʼ��ʱ��
 *       SDIO���Ĵ�������    
 */
aw_err_t aw_sdio_host_io_rw_direct (awbl_sdio_host_t  *p_host,
                                    uint8_t            func_num,
                                    uint8_t                 opt,
                                    aw_bool_t             ram_flg,
                                    uint32_t           addr,
                                    uint8_t           *p_data);
/**
 * \brief SDIO����д����
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�
 *                            һ��SDIO�����ж������
 * \param[in]  opt          : ������������д
 * \param[in]  blk_flg      : �鴫��ģʽʹ�ܣ���TRUEΪ�鴫�䣬��FALSEΪ�ֽڴ���
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  blksize      : ���ݿ�Ĵ�С
 * \param[in]  blkorbytes   : ��д�Ŀ�������ֽڸ���
 * \param[in]  op_flg       : ��ַ������־������ÿ��һ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ���ΪTRUEÿ��ȡһ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_rw_externed (awbl_sdio_host_t  *p_host,
                                      uint8_t            func_num,
                                      uint8_t            opt,
                                      aw_bool_t          blk_flg,
                                      uint32_t           addr,
                                      uint8_t           *p_buf,
                                      size_t             blksize,
                                      size_t             blkorbytes,
                                      aw_bool_t          op_flg);
/**
 * \brief �����λSDIO�������ع鵽idle״̬
 *
 * \param[in] p_host        : SDIO����HOST
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_go_idle_state (awbl_sdio_host_t *p_host);

/**
 * \brief ����SDIO�����ݴ���Ŀ��С
 *
 * \param[in] p_host        : SDIO����HOST
 * \param[in] func_num      : Ҫ���ÿ��С�Ĺ��ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ
 *                            �Ĺ��ܣ�һ��SDIO�������1~6��IO����
 * \param[in] blk_len       : ���С��ÿ�����ܿ����õ������С���Դ�CIS�����
 *                            �����ɵ��ö�CIS�Ľӿڻ�ȡ�����õ������С��
 *                            blk_lenӦС�ڵ��������С
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_set_blocklen (awbl_sdio_host_t *p_host,
                                       uint8_t           func_num,
                                       uint32_t          blk_len);

/**
 * \brief ����SDIO�����߿��
 *
 * \param[in] p_host        : SDIO����HOST
 * \param[in] width         : ���߿�ȣ�������߿��ö�ٶ���
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_app_set_bus_width (awbl_sdio_host_t     *p_host,
                                            awbl_sdio_bus_width_t width);
/**
 * \brief ��SDIO�����ͳ�ʼ������
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  ocr          : HOST֧�ֵĵ�ѹ��Χ�����͸�SDIO��
 * \param[out] p_rocr       : SDIO��֧�ֵĵ�ѹ��Χ
 * \param[out] p_func_num   : SDIO��֧�ֵĹ���������������FUNC0
 *
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_send_app_op_cond (awbl_sdio_host_t *p_host,
                                           uint32_t          ocr,
                                           uint32_t         *p_rocr,
                                           uint8_t          *p_func_num);
/**
 * \brief ȡ��SDIO��ָ��IO���ܵ����ݴ���
 *
 * \param[in] p_host        : SDIO����HOST
 * \param[in] func_num      : Ҫȡ������Ĺ��ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ��
 *                            ���ܣ�һ��SDIO�������1~6��IO����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_transfer_abort (awbl_sdio_host_t *p_host,
                                         uint8_t           func_num);
/**
 * \brief ��SDIO����ȡN��������
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO����
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���ݿ��С
 * \param[in]  sector_cnt   : ��ȡ���ݿ����
 * \param[in]  op_fix       : ��ַ������־������ÿ��ȡһ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ���ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_read_multiple_block (awbl_sdio_host_t *p_host,
                                              uint8_t           func_num,
                                              uint32_t          addr,
                                              uint8_t          *p_buf,
                                              uint16_t          sector_size,
                                              uint32_t          sector_cnt,
                                              aw_bool_t         op_fix);

/**
 * \brief ��SDIO��д��N��������
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO����
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ��д������,��ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���ݿ��С
 * \param[in]  sector_cnt   : д�����ݿ����
 * \param[in]  op_fix       : ��ַ������־������ÿдһ���ֽڵ����ݵ�ַ�Ƿ��Զ���
 *                            �ӣ���ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_write_multiple_block (awbl_sdio_host_t *p_host,
                                               uint8_t           func_num,
                                               uint32_t          addr,
                                               uint8_t          *p_buf,
                                               uint16_t          sector_size,
                                               uint32_t          sector_cnt,
                                               aw_bool_t         op_fix);
/**
 * \brief ��SDIO����ȡN���ֽ�����
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�����ж�����ܡ�
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  data_len     : ��ȡ���ݵĳ���
 * \param[in]  op_fix       : ��ַ������־������ÿ��һ���ֽڵ����ݵ�ַ�Ƿ��Զ���
 *                            �ӣ���ΪTRUEÿ��ȡһ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_read_bytes(awbl_sdio_host_t *p_host,
                                    uint8_t           func_num,
                                    uint32_t          addr,
                                    uint8_t          *p_buf,
                                    size_t            data_len,
                                    aw_bool_t         op_fix);


/**
 * \brief ��SDIO��д��N���ֽ�����
 *
 * \param[in]  p_host       : SDIO����HOST
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO���ܡ�
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ��д�����ݵ�ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  data_len     : д�����ݵĳ���
 * \param[in]  op_fix       : ��ַ������־������ÿдһ���ֽڵ����ݵ�ַ�Ƿ��Զ���
 *                            �ӣ���ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_host_io_write_bytes (awbl_sdio_host_t *p_host,
                                      uint8_t           func_num,
                                      uint32_t          addr,
                                      uint8_t          *p_buf,
                                      size_t            data_len,
                                      aw_bool_t         op_fix);

/**
 * \brief ����SDIO�ж�
 *
 * \param[in] p_card        : SDIO�豸
 * \param[in] pfn_sdio_isr  : �жϻص�����
 * \param[in] p_arg         : �ص���������
 *
 * \retval AW_OK            : ���ӳɹ�
 * \retval -AW_EINVAL       : ��Ч����
 * \retval -AW_EBUSY        : �ж��Ѿ�������
 */
#if 0 
aw_err_t aw_sdio_int_connnect (struct awbl_sdio_device *p_card,
                               aw_pfn_sdio_ext_isr_t    pfn_sdio_isr,
                               void                    *p_arg);
#endif 
/**
 * \brief ȡ������SDIO�ж�
 *
 * \param[in] p_card        : SDIO�豸
 *
 * \retval AW_OK            : ȡ�����ӳɹ�
 * \retval -AW_EINVAL       : ��Ч����
 */
#if 0 
aw_err_t aw_sdio_int_disconnnect (struct awbl_sdio_device *p_card);
#endif 
/**
 * \brief SDIO������Ͽ���IO���ܳ�ʼ��
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[out] p_info       : IO��������Ϣ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_func_init (struct awbl_sdio_device        *p_card);
/**
 * \brief SD������Ͽ��Ĵ洢���ܳ�ʼ��
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[out] p_cardinfo   : �洢��������Ϣ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_mem_func_init (struct awbl_sdio_device        *p_card);

/**
 * \brief ��Ͽ���ʼ������ʼ��IO���ܺʹ洢���ܣ�
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[out] p_comboinfo  : ��Ͽ�������Ϣ
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_combo_init (struct awbl_sdio_device          *p_card);

/**
 * \brief SDIO������дһ���ֽڵ�����
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�����ж������
 * \param[in]  opt          : ������������д
 * \param[in]  ram_flg      : �鴫��ģʽʹ�ܣ���TRUEΪ�鴫�䣬��FALSEΪ�ֽڴ���
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ
 *                            ��ΧΪ0x0~0x1ffff
 * \param[out] p_data       : ���ݻ�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 *
 * \note �˽ӿڴ������ݲ���ʹ�������ߣ�����ʹ��CMD�ߣ�������������δ��ʼ��ʱ��
 *       SDIO���Ĵ�������
 */
aw_err_t aw_sdio_io_rw_direct (struct awbl_sdio_device *p_card,
                               uint8_t                  func_num,
                               uint8_t                  opt,
                               aw_bool_t                ram_flg,
                               uint32_t                 addr,
                               uint8_t                 *p_data);

/**
 * \brief SDIO����д����
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�����ж������
 * \param[in]  opt          : ������������д
 * \param[in]  blk_flg      : �鴫��ģʽʹ�ܣ���TRUEΪ�鴫�䣬��FALSEΪ�ֽڴ���
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ
 *                            ��ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  blksize      : ���ݿ�Ĵ�С
 * \param[in]  blkorbytes   : ��д�Ŀ�������ֽڸ���
 * \param[in]  op_flg       : ��ַ������־������ÿ��һ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ���ΪTRUEÿ��ȡһ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_rw_externed (struct awbl_sdio_device *p_card,
                                 uint8_t                  func_num,
                                 uint8_t                  opt,
                                 aw_bool_t                blk_flg,
                                 uint32_t                 addr,
                                 uint8_t                 *p_buf,
                                 size_t                   blksize,
                                 size_t                   blkorbytes,
                                 aw_bool_t                op_flg);

/**
 * \brief �����λSDIO�������ع鵽idle״̬
 *
 * \param[in]  p_card       : SDIO�豸
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_go_idle_state (struct awbl_sdio_device *p_card);

/**
 * \brief ����SDIO�����ݴ���Ŀ��С
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : Ҫ���ÿ��С�Ĺ��ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ
 *                            �Ĺ��ܣ�һ��SDIO�������1~6��IO����
 * \param[in]  blk_len      : ���С��ÿ�����ܿ����õ������С���Դ�CIS�����
 *                            �����ɵ��ö�CIS�Ľӿڻ�ȡ�����õ������С��
 *                            blk_lenӦС�ڵ��������С
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_set_blocklen (struct awbl_sdio_device *p_card,
                                  uint8_t                  func_num,
                                  uint32_t                 blk_len);
/**
 * \brief ����SDIO�����߿��
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  width        : ���߿�ȣ�������߿��ö�ٶ���
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_app_set_bus_width (struct awbl_sdio_device *p_card,
                                       awbl_sdio_bus_width_t    width);
/**
 * \brief ��SDIO�����ͳ�ʼ������
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  ocr          : HOST֧�ֵĵ�ѹ��Χ�����͸�SDIO��
 * \param[out] p_rocr       : SDIO��֧�ֵĵ�ѹ��Χ
 * \param[out] p_rfunc_num  : SDIO��֧�ֵĹ���������������FUNC0
 *
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_send_app_op_cond (struct awbl_sdio_device *p_card,
                                      uint32_t                 ocr,
                                      uint32_t                *p_rocr,
                                      uint8_t                 *p_rfunc_num);
/**
 * \brief ȡ��SDIO��ָ��IO���ܵ����ݴ���
 *
 * \param[in] p_card        : SDIO�豸
 * \param[in] func_num      : Ҫȡ������Ĺ��ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ��
 *                            ���ܣ�һ��SDIO�������1~6��IO����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_transfer_abort (struct awbl_sdio_device *p_card,
                                    uint8_t                  func_num);
/**
 * \brief ��SDIO����ȡN��������
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO����
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ
 *                            ��ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���ݿ��С
 * \param[in]  sector_cnt   : ��ȡ���ݿ����
 * \param[in]  op_fix       : ��ַ������־������ÿ��ȡһ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ���ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ�����
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_read_multiple_block (struct awbl_sdio_device *p_card,
                                         uint8_t                  func_num,
                                         uint32_t                 addr,
                                         uint8_t                 *p_buf,
                                         uint16_t                 sector_size,
                                         uint32_t                 sector_cnt,
                                         aw_bool_t                op_fix);

/**
 * \brief ��SDIO��д��N��������
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO����
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ��д������,��ַ
 *                            ��ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  sector_size  : ���ݿ��С
 * \param[in]  sector_cnt   : д�����ݿ����
 * \param[in]  op_fix       : ��ַ������־������ÿдһ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ���ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_write_multiple_block (struct awbl_sdio_device *p_card,
                                          uint8_t                  func_num,
                                          uint32_t                 addr,
                                          uint8_t                 *p_buf,
                                          uint16_t                 sector_size,
                                          uint32_t                 sector_cnt,
                                          aw_bool_t                op_fix);

/**
 * \brief ��SDIO��д��N���ֽ�����
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�������1~6��IO���ܡ�
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ��д�����ݵ�ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  data_len     : д�����ݵĳ���
 * \param[in]  op_fix       : ��ַ������־������ÿдһ���ֽڵ����ݵ�ַ�Ƿ��Զ�
 *                            ���ӣ��� ΪTRUEÿд��һ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_write_bytes (struct awbl_sdio_device *p_card,
                                 uint8_t                  func_num,
                                 uint32_t                 addr,
                                 uint8_t                 *p_buf,
                                 size_t                   data_len,
                                 aw_bool_t                op_fix);
/**
 * \brief ��SDIO����ȡN���ֽ�����
 *
 * \param[in]  p_card       : SDIO�豸
 * \param[in]  func_num     : ���ܺţ�0~7, 0ΪCIA����1~n��ʾ��ͬ�Ĺ��ܣ�һ��
 *                            SDIO�����ж�����ܡ�
 * \param[in]  addr         : SDIO��func_num���ܵ�ַ���Ӹõ�ַ���ȡ����,��ַ��
 *                            ΧΪ0x0~0x1ffff
 * \param[out] p_buf        : ���ݻ�����
 * \param[in]  data_len     : ��ȡ���ݵĳ���
 * \param[in]  op_fix       : ��ַ������־������ÿ��һ���ֽڵ����ݵ�ַ�Ƿ��Զ���
 *                            �ӣ���ΪTRUEÿ��ȡһ���ֽڵ�ַ�����Զ���1����Ϊ
 *                            FALSE��ַ��һֱ����䡣
 *
 * \retval AW_OK            : ����ɹ�
 * \retval -AW_ENOMEM       : �ڴ�ռ䲻��
 * \retval -AW_ECANCELED    : ������ȡ��
 * \retval -AW_EIO          : SDIO����I/O����
 * \retval -AW_EBUSY        : SDIO HOSTæ�����ڴ�������msg
 * \retval -AW_EINVAL       : ��������
 * \retval -AW_ETIME        : HOST����ʱ
 * \retval -AW_ENOTSUP      : HOST��֧�ָù���
 */
aw_err_t aw_sdio_io_read_bytes(struct awbl_sdio_device *p_card,
                               uint8_t                  func_num,
                               uint32_t                 addr,
                               uint8_t                 *p_buf,
                               size_t                   data_len,
                               aw_bool_t                op_fix);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* _AWBL_SDIOBUS_H_ */

/* end of file */
