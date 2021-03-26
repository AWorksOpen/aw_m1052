/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
*
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx FLEXCAN
 *
 * \internal
 * \par modification history:
 * - 1.01 18-09-13  anu, modify brp and reset
 * - 1.00 17-12-14  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_common.h"
#include "aw_int.h"
#include "aw_task.h"
#include "awbl_can.h"
#include "aw_can.h"
#include "aw_clk.h"
#include "driver/can/awbl_imx10xx_can.h"


/*******************************************************************************
  local defines
*******************************************************************************/

/**
 * \brief imx10xx CAN 寄存器块定义
 */
typedef struct imx10xx_can_regs {
    uint32_t mcr;               /**< \brief 模块配置寄存器 0x00 */
    uint32_t ctrl1;             /**< \brief 控制寄存器1 0x04 */
    uint32_t timer;             /**< \brief 自由行定时器寄存器 0x08 */
    uint32_t reserved0;         /**< \brief 保留寄存器 0x0C */
    uint32_t rxmgmask;          /**< \brief 接收邮箱全局掩码定时器 0x10*/
    uint32_t rx14mask;          /**< \brief 接收缓冲区14掩码寄存器 0x14 */
    uint32_t rx15mask;          /**< \brief 接收缓冲区15掩码寄存器 0x18 */
    uint32_t ecr;               /**< \brief 错误计数寄存器 0x1C */
    uint32_t esr1;              /**< \brief 错误及状态寄存器1 0x20 */
    uint32_t imask2;            /**< \brief 中断掩码寄存器2 0x24 */
    uint32_t imask1;            /**< \brief 中断掩码寄存器1 0x28 */
    uint32_t iflag2;            /**< \brief 中断标志寄存器2 0x2C */
    uint32_t iflag1;            /**< \brief 中断标志寄存器1 0x30 */
    uint32_t ctrl2;             /**< \brief 控制寄存器2 0x34 */
    uint32_t esr2;              /**< \brief 错误及状态寄存器1 0x38 */
    uint32_t reserved1[2];      /**< \brief 保留寄存器 0x3C~0x40 */
    uint32_t crcr;              /**< \brief CRC寄存器 0x44 */
    uint32_t rxfgmask;          /**< \brief 接收FIFO全局掩码寄存器 0x48 */
    uint32_t rxfir;             /**< \brief 接收FIFO信息寄存器 0x4C */
    uint32_t reserved2[12];     /**< \brief 保留寄存器 0x50~0x7C */

    /**
     *  mb0~mb5   => RX FIFO             (0x80~0xDC)
     *  mb6~mb37  => ID Filter table     (0xE0~0x2DC)
     *  mb38~mb63 => rest message buffer (0x2E0~0x47C)
     */
    uint32_t mb[64][4];         /**< \brief 报文缓冲区寄存器 */

    uint32_t reserved3[256];    /**< \brief 保留寄存器 ~0x87C */
    uint32_t rximr[64];         /**< \brief 接收独立掩码寄存器0~63 0x880~ */
    uint32_t reserved4[24];     /**< \brief 保留寄存器 ~0x9DC */
    uint32_t gfwr;              /**< \brief 故障滤波宽度寄存器 0x9E0 */
} imx10xx_can_regs_t;

/* can控制器进入sleep模式时需要操作的寄存器 */
#define IOMUXC_GPR_GPR4             (0x400AC010UL)
#define CAN1_STOP_REQ               (1UL)
#define CAN2_STOP_REQ               (2UL)
#define CAN1_STOP_ACK               (17UL)
#define CAN2_STOP_ACK               (18UL)


/***************************   MAX OFFSET   *************************/
#define __FLEXCAN_REG_MAX_OFFSET       0x080        /* 报文缓冲区的首地址 */
#define __FLEXCAN_BUF_MAX_OFFSET       0x2DC        /* 滤波表的结束地址 */

/****************************   __HW_CAN_MCR BIT   ****************************/
#define __CAN_MCR_DIS      31      /* Disable FlexCAN clocks */
#define __CAN_MCR_FRZ      30      /* FlexCAN enabled to enter debug mode */
#define __CAN_MCR_FEN      29      /* enable FIFO */
#define __CAN_MCR_HALT     28      /* FlexCAN enters debug mode */
#define __CAN_MCR_NOTRDY   27      /* FlexCAN in low-power stop or debug mode */
#define __CAN_MCR_WAKEMSK  26      /* wakeup interrupt is enabled */
#define __CAN_MCR_SOFTRST  25      /* soft reset cycle initiated/completed */
#define __CAN_MCR_FRZACK   24      /* FlexCAN has entered/exited debug mode */
#define __CAN_MCR_SUPV     23      /* supervisor/user mode */
#define __CAN_MCR_SELFWAKE 22      /* self wake enabled */
#define __CAN_MCR_WRN      21      /* auto-power save */
#define __CAN_MCR_LPMACK   20      /* FlexCAN entered low-power stop mode */
#define __CAN_MCR_SRX      17      /* 禁止自接收 */
#define __CAN_MCR_IRMQ     16      /* 兼容早期FLEXCAN版本(使能独立滤波掩码) */
#define __CAN_MCR_AEN      12      /* 传输中止功能 */
#define __CAN_MCR_IDAM     8       /* 设置滤波表格式 */

/*******************   MCR Module Configuration Register   ********************/
#define __FLEXCAN_DIS      (0x01 << __CAN_MCR_DIS)       /* Disable FlexCAN clocks */
#define __FLEXCAN_FRZ      (0x01 << __CAN_MCR_FRZ)       /* FlexCAN enabled to enter debug mode */
#define __FLEXCAN_FEN      (0x01 << __CAN_MCR_FEN)       /* enable FIFO */
#define __FLEXCAN_HALT     (0x01 << __CAN_MCR_HALT)      /* FlexCAN enters debug mode */
#define __FLEXCAN_NOTRDY   (0x01 << __CAN_MCR_NOTRDY)    /* FlexCAN in lowpower stop/debug mode */
#define __FLEXCAN_WAKEMSK  (0x01 << __CAN_MCR_WAKEMSK)   /* wakeup interrupt is enabled */
#define __FLEXCAN_IDAM     (0x03 << __CAN_MCR_IDAM)      /* FlexCAN RX FIFO filter format */
#define __FLEXCAN_SOFTRST  (0x01 << __CAN_MCR_SOFTRST)   /* soft reset cycle initiated */
#define __FLEXCAN_FRZACK   (0x01 << __CAN_MCR_FRZACK)    /* FlexCAN has enter/exited debug mode */
#define __FLEXCAN_SUPV     (0x01 << __CAN_MCR_SUPV)      /* supervisor/user mode */
#define __FLEXCAN_SELFWAKE (0x01 << __CAN_MCR_SELFWAKE)  /* self wake enabled */
#define __FLEXCAN_WRN      (0x01 << __CAN_MCR_WRN)       /* auto-power save */
#define __FLEXCAN_LPMACK   (0x01 << __CAN_MCR_LPMACK)    /* FlexCAN entered low-power stop mode */

/*****************************   __HW_CAN_CTRL1 BIT   *************************/
#define __CAN_CTRL1_PRESDIV     24
#define __CAN_CTRL1_RJW         22
#define __CAN_CTRL1_PSEG1       19
#define __CAN_CTRL1_PSEG2       16
#define __CAN_CTRL1_BOFF        15
#define __CAN_CTRL1_ERR         14
#define __CAN_CTRL1_LPB         12
#define __CAN_CTRL1_TWRN        11
#define __CAN_CTRL1_RWRN        10
#define __CAN_CTRL1_SMP         7
#define __CAN_CTRL1_LOM         3
#define __CAN_CTRL1_PROP        0

/*******************   CTRL Module Configuration Register   *******************/
#define __FLEXCAN_PRESDIV       (0xFF << __CAN_CTRL1_PRESDIV)
#define __FLEXCAN_RJW           (0x03 << __CAN_CTRL1_RJW)
#define __FLEXCAN_PSEG1         (0x07 << __CAN_CTRL1_PSEG1)
#define __FLEXCAN_PSEG2         (0x07 << __CAN_CTRL1_PSEG2)
#define __FLEXCAN_PROP          (0x07 << __CAN_CTRL1_PROP)
#define __FLEXCAN_SMP           (0x01 << __CAN_CTRL1_SMP)
#define __FLEXCAN_BOFF          (0x01 << __CAN_CTRL1_BOFF)
#define __FLEXCAN_ERR           (0x01 << __CAN_CTRL1_ERR)

/*****************************   __HW_CAN_ESR1 BIT   **************************/
#define __CAN_ESR1_TWRN         17
#define __CAN_ESR1_RWRN         16
#define __CAN_ESR1_TW_C         9
#define __CAN_ESR1_RW_C         8
#define __CAN_ESR1_BOFF         2
#define __CAN_ESR1_ERR          1
#define __CAN_ESR1_WAK          0

/*******************   ESR1 Module Configuration Register   *******************/
#define __FLEXCAN_ESR1_TWRN     0x0002
#define __FLEXCAN_ESR1_RWRN     0x0001

#define __FLEXCAN_ESR1_BIT1     0x8000
#define __FLEXCAN_ESR1_BIT0     0x4000
#define __FLEXCAN_ESR1_ACK      0x2000
#define __FLEXCAN_ESR1_CRC      0x1000
#define __FLEXCAN_ESR1_FRM      0x0800
#define __FLEXCAN_ESR1_STF      0x0400
#define __FLEXCAN_ESR1_TWRN_INT 0x20000
#define __FLEXCAN_ESR1_RWRN_INT 0x10000
#define __FLEXCAN_ESR1_TX_WRN   0x0100
#define __FLEXCAN_ESR1_RX_WRN   0x0080

#define __FLEXCAN_ESR1_BOFF     0x0004
#define __FLEXCAN_ESR1_ERR      0x0002
#define __FLEXCAN_ESR1_WAK      0x0001

/***************************   __HW_CAN_MBn OFFSET   **************************/
#define __HW_CAN_MBN_BLOCK0     (0x0000U)
#define __HW_CAN_MBN_BLOCK1     (0x0004U)
#define __HW_CAN_MBN_BLOCK2     (0x0008U)
#define __HW_CAN_MBN_BLOCK3     (0x000CU)

/******************************* MBN BLOCK0 bit  ******************************/
#define __CAN_MBN_BLK0_CODE     (24) /* 控制码 */
#define __CAN_MBN_BLK0_SSR      (22) /* 替代远程帧 */
#define __CAN_MBN_BLK0_IDE      (21) /* 标准帧 */
#define __CAN_MBN_BLK0_RTR      (20) /* 远程帧 */
#define __CAN_MBN_BLK0_LEN      (16) /* 长度 */
#define __CAN_MBN_BLK0_TIME     (0)  /* 时间戳 */

/************************* MBN BLOCK0 config register  ************************/
#define __FLEXCAN_MBN_BLK0_CODE  (0x0F   << __CAN_MBN_BLK0_CODE)/* 控制码 */
#define __FLEXCAN_MBN_BLK0_SSR   (0x01   << __CAN_MBN_BLK0_SSR) /* 替代远程帧 */
#define __FLEXCAN_MBN_BLK0_IDE   (0x01   << __CAN_MBN_BLK0_IDE) /* 标准帧 */
#define __FLEXCAN_MBN_BLK0_RTR   (0x01   << __CAN_MBN_BLK0_RTR) /* 远程帧 */
#define __FLEXCAN_MBN_BLK0_LEN   (0x0F   << __CAN_MBN_BLK0_LEN) /* 长度 */
#define __FLEXCAN_MBN_BLK0_TIME  (0xFFFF << __CAN_MBN_BLK0_TIME)/* 时间戳 */

#define __CAN_MAX_LEN             8     /* 最大8字节的数据长度  */
#define __CAN_SEND_TYPE           0x0F  /* 发送类型 bit0-3 */
#define __CAN_SEND_CODE           0x0C  /* 发送代码 */
#define __CAN_SEND_STOP           0x09  /* 停止发送代码 */
#define __CAN_SEND_DONE           0x08  /* 发送完成代码 */
#define __CAN_SEND_DONE_RTR       0x04  /* 远程帧发送完成代码 */

/************* valid bits in CAN ID for frame formats *************************/
#define __CAN_MBN_BLK1_SFF        18   /* standard frame bit */

#define __FLEXCAN_SFF_MASK 0x000007FFU /* standard frame format (SFF) */
#define __FLEXCAN_EFF_MASK 0x1FFFFFFFU /* extended frame format (EFF) */

/**************************** CAN FIFO AND FILTER TABLE ***********************/
#define __IMX_RX_FIFO             5     /* 接收FIFO利用的MB */
#define __IMX_MBN_TABLE_ID        6     /* 设置滤波表的MB */
#define __IMX_TABLE_ID_CNT        32    /* 设置滤波表id的个数 */

#define __IMX_TABLE_ID_MAXIMUM    128   /* 设置滤波表id的最大值 */


/*********************************   MBn Count  *******************************/
#define __CAN_MAX_MBN             38   /* massage buffer 0-63 */

/***************************  CAN MBn write read select  **********************/
#define __IMX_MBN_RECV_CHN        0    /* 接收通道的 Message Buffer FIFO (从接收FIFO开始读取) */
#define __IMX_MBN_SEND_CHN        __CAN_MAX_MBN    /* 发送通道的 Message Buffer last (从保留报文MB38开始发送) */

/*****************************   REG OPERATION  *******************************/
#define __IMX_CAN_REG_READ8(addr)                    \
                                     readb((volatile void __iomem *)addr)

#define __IMX_CAN_REG_WRITE8(addr, data)            \
                                     writeb(data, (volatile void __iomem *)addr)

#define __IMX_CAN_REG_READ32(addr)                 AW_REG_READ32(addr)

#define __IMX_CAN_REG_WRITE32(addr, data)          AW_REG_WRITE32(addr, data)

#define __IMX_CAN_REG_BIT_SET32(addr, bit)         AW_REG_BIT_SET32(addr, bit)

#define __IMX_CAN_REG_BIT_CLR32(addr, bit)         AW_REG_BIT_CLR32(addr, bit)

#define __IMX_CAN_REG_BIT_SET_MASK32(addr, mask)     \
                                               AW_REG_BIT_SET_MASK32(addr, mask)

#define __IMX_CAN_REG_BIT_CLR_MASK32(addr, mask)    \
                                               AW_REG_BIT_CLR_MASK32(addr, mask)

typedef union __can_mbn_data {
    uint32_t dword;
    struct {
        uint8_t byte3;
        uint8_t byte2;
        uint8_t byte1;
        uint8_t byte0;
    } data;
} __can_mbn_data_t;

/** \brief 从p_serv获得can_dev宏操作 */
#define __IMX10xx_CAN_GET_THIS(p_serv) \
        AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_can_dev, can_serv)

/** \brief 从p_dev获得can_dev宏操作 */
#define __IMX10xx_CAN_GET_DEV(p_dev) \
        AW_CONTAINER_OF(p_dev, struct awbl_imx10xx_can_dev, dev)

/** \brief 从p_dev获得can_devinfo宏操作 */
#define __IMX10xx_CAN_GET_INFO(p_dev) \
        ((struct awbl_imx10xx_can_dev_info *)AWBL_DEVINFO_GET(p_dev))

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx10xx_can_inst_init (awbl_dev_t *p_dev);
aw_local void __imx10xx_can_inst_init2 (awbl_dev_t *p_dev);
aw_local void __imx10xx_can_inst_connect (awbl_dev_t *p_dev);

aw_local aw_can_err_t __imx10xx_can_init (
        struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode);

aw_local aw_can_err_t __imx10xx_can_start (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_set_haltfrz (
        struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reset_haltfrz (
        struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reset (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_sleep (struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_wakeup (struct awbl_can_service *p_serv);

aw_local aw_can_err_t __imx10xx_can_int_enable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask);
aw_local aw_can_err_t __imx10xx_can_int_disable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask);
aw_local aw_can_err_t __imx10xx_can_baudrate_set (
        struct awbl_can_service *p_serv, const aw_can_baud_param_t *p_can_btr0);
aw_local aw_can_err_t __imx10xx_can_baudrate_get (
        struct awbl_can_service *p_serv, aw_can_baud_param_t *p_can_btr0);
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_get (
        struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_can_err_reg);
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_clr (
        struct awbl_can_service *p_serv);
aw_local aw_can_err_t __imx10xx_can_reg_msg_read (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg);
aw_local aw_can_err_t __imx10xx_can_reg_msg_write (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg);
aw_local aw_can_err_t __imx10xx_can_msg_snd_stop (
        struct awbl_can_service *p_serv);

aw_local aw_can_err_t __imx10xx_can_filter_table_set (
        struct awbl_can_service *p_serv, uint8_t *p_filterbuff, size_t length);
aw_local aw_can_err_t __imx10xx_can_filter_table_get (
        struct awbl_can_service *p_serv,
        uint8_t                 *p_filterbuff,
        size_t                  *p_length);
aw_local aw_can_err_t __imx10xx_can_intstatus_get (
        struct awbl_can_service *p_serv,
        awbl_can_int_type_t     *p_int_type,
        aw_can_bus_err_t        *p_bus_err);
aw_local aw_can_err_t __imx10xx_can_reg_write (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length);
aw_local aw_can_err_t __imx10xx_can_reg_read (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length);


/*******************************************************************************
    globals
*******************************************************************************/

/** \brief sio driver functions */
awbl_can_drv_funcs_t __g_imx10xx_flexcan_drv_funcs = {
    __imx10xx_can_init,
    __imx10xx_can_start,
    __imx10xx_can_set_haltfrz,
    __imx10xx_can_sleep,
    __imx10xx_can_wakeup,
    __imx10xx_can_int_enable,
    __imx10xx_can_int_disable,
    __imx10xx_can_baudrate_set,
    __imx10xx_can_baudrate_get,
    __imx10xx_can_errreg_cnt_get,
    __imx10xx_can_errreg_cnt_clr,
    __imx10xx_can_reg_msg_read,
    __imx10xx_can_reg_msg_write,
    __imx10xx_can_msg_snd_stop,
    __imx10xx_can_filter_table_set,
    __imx10xx_can_filter_table_get,
    __imx10xx_can_intstatus_get,
    __imx10xx_can_reg_write,
    __imx10xx_can_reg_read
};

aw_const struct awbl_drvfuncs g_imx10xx_can_awbl_drvfuncs = {
    __imx10xx_can_inst_init,        /* devInstanceInit */
    __imx10xx_can_inst_init2,       /* devInstanceInit2 */
    __imx10xx_can_inst_connect      /* devConnect */
};

/*******************************************************************************
    static
*******************************************************************************/

/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_can_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_CAN_NAME,               /* p_drvname*/
    &g_imx10xx_can_awbl_drvfuncs,        /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/**
 * \brief initialize imx10xx can device
 *
 * This is the imx10xx can initialization routine.
 *
 * \note
 * This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation and I/O.
 */
aw_local void __imx10xx_can_inst_init (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief soft reset imx10xx can
 * FLEXCAN resets its internal state machines and
 * some of the memory mapped registers.
 */

/**
 * \brief initialize imx10xx can device
 *
 * This routine initializes the imx10xx can device.
 */
aw_local void __imx10xx_can_inst_init2 (awbl_dev_t *p_dev)
{
    /* MMU is already enabled when it comes here and safe to use spinlock. */
    awbl_imx10xx_can_dev_t       *p_this;
    awbl_imx10xx_can_dev_info_t  *p_info;

    /* get the device */
    p_this = __IMX10xx_CAN_GET_DEV(p_dev);

    /* get the device info */
    p_info = __IMX10xx_CAN_GET_INFO(p_dev);

    p_this->filtercnt = 0;

    /* Call platform initialize function first */
    if (NULL != p_info->pfn_plfm_init) {
        p_info->pfn_plfm_init();
    }

    awbl_can_service_init(&(p_this->can_serv),
                          &p_info->servinfo,
                          &__g_imx10xx_flexcan_drv_funcs);

    aw_int_connect(p_info->int_num,\
                   (aw_pfuncvoid_t)awbl_can_isr_handler,\
                   (void*)&p_this->can_serv);

    aw_int_enable(p_info->int_num);

    awbl_can_service_register(&p_this->can_serv);
}

/**
 * \brief connect imx10xx can device to system
 *
 * Nothing to do here.  We want serial channels available for output reasonably
 * early during the boot process.  Because this is a serial channel, we connect
 * to the I/O system in __imx10xx_can_inst_init2() instead of here.
 */
aw_local void __imx10xx_can_inst_connect (awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief 通过寄存器基地址判断是CAN1还是CAN2
 */
aw_local int __imx10xx_can_no (uint32_t base)
{
    if (base == 0x401D0000) {
        return 1;
    } else if (base == 0X401D4000) {
        return 2;
    }
    return -1;
}


/** \brief CAN 底层部分的初始化 */
aw_local aw_can_err_t __imx10xx_can_init (
        struct awbl_can_service *p_serv, aw_can_work_mode_type_t work_mode)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t mbreg;
    uint8_t  i;

    /* 工作模式判断 */
    if (work_mode & AW_CAN_WORK_MODE_NON_ISO) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    __imx10xx_can_reset(p_serv);

    p_this->sendtype = AW_CAN_MSG_FLAG_SEND_NORMAL;

    if (work_mode & AW_CAN_WORK_MODE_LISTEN_ONLY) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_LOM);
    } else {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_LOM);
    }

    __IMX_CAN_REG_WRITE8(&p_reg->mcr, __CAN_MAX_MBN);    /* set max MBs*/
    __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_SRX); /* disable recv by self */
    __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_LPB); /* Loop Back disabled */

    mbreg = (uint32_t)(&p_reg->mb[0]);      /* 获取MB0的首地址  */

    for (i = 0; i <= __CAN_MAX_MBN; i++) {
        __IMX_CAN_REG_WRITE32(mbreg + __HW_CAN_MBN_BLOCK0, 0);  /* clear MBs CS */
        __IMX_CAN_REG_WRITE32(mbreg + __HW_CAN_MBN_BLOCK1, 0);  /* clear MBs ID */
        __IMX_CAN_REG_WRITE32(mbreg + __HW_CAN_MBN_BLOCK2, 0);  /* clear MBs DATAL */
        __IMX_CAN_REG_WRITE32(mbreg + __HW_CAN_MBN_BLOCK3, 0);  /* clear MBs DATAH */
        mbreg += 0x10;                                          /* mbn reg offset is 0x10 */
    }

    __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_AEN);
    __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_FEN);        /* 使能FIFO */
    __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_IRMQ);       /* 使能独立滤波掩码 */

    /* 下面的操作是设置滤波，模拟进入旁路模式 */
    __IMX_CAN_REG_BIT_CLR_MASK32(&p_reg->mcr, __FLEXCAN_IDAM);
    __IMX_CAN_REG_BIT_SET_MASK32(&p_reg->mcr, 0);
    __IMX_CAN_REG_WRITE32(&p_reg->mb[6], 0);
    __IMX_CAN_REG_WRITE32(&p_reg->rximr[0], 0);
    __IMX_CAN_REG_WRITE32(&p_reg->rxfgmask, 0);

    /* 设置ctr2寄存器 */
    AW_REG_BITS_SET32(&p_reg->ctrl2, 24, 4, 0xF);

    return AW_CAN_ERR_NONE;
}

/** \brief CAN 进入冻结模式  */
aw_local aw_can_err_t __imx10xx_can_set_haltfrz (
        struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    __IMX_CAN_REG_BIT_SET_MASK32(&p_reg->mcr, (__FLEXCAN_FRZ | __FLEXCAN_HALT));

    while (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_FRZACK)) {
        __IMX_CAN_REG_BIT_SET_MASK32(
            &p_reg->mcr, (__FLEXCAN_FRZ | __FLEXCAN_HALT));
    }

    return AW_CAN_ERR_NONE;
}

/** \brief CAN 退出冻结模式  */
aw_local aw_can_err_t __imx10xx_can_reset_haltfrz (
        struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    __IMX_CAN_REG_BIT_CLR_MASK32(&p_reg->mcr, (__FLEXCAN_FRZ | __FLEXCAN_HALT));

    while ((__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_FRZACK)) {
        __IMX_CAN_REG_BIT_CLR_MASK32(
            &p_reg->mcr, (__FLEXCAN_FRZ | __FLEXCAN_HALT));
    }

    return AW_CAN_ERR_NONE;
}

/** \brief CAN启动 */
aw_local aw_can_err_t __imx10xx_can_start (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    __IMX_CAN_REG_BIT_CLR_MASK32(&p_reg->mcr, (__FLEXCAN_DIS | __FLEXCAN_HALT));

    return AW_CAN_ERR_NONE;
}

/** \brief CAN复位（停止） */
aw_local aw_can_err_t __imx10xx_can_reset (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_SOFTRST);

    return AW_CAN_ERR_NONE;
}

/** \brief CAN休眠  */
aw_local aw_can_err_t __imx10xx_can_sleep (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);
    int                          can_no = -1;

    if (__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_SOFTRST) {
        return -AW_CAN_ERR_SLEEP_IN_CFG_MODE;
    }

    can_no = __imx10xx_can_no(p_info->reg_base);
    if (can_no == -1) {
        return -AW_CAN_ERR_NO_OP;
    }

    __IMX_CAN_REG_BIT_SET_MASK32(&p_reg->mcr, (__FLEXCAN_SELFWAKE));

    if (can_no == 1) {
        AW_REG_BIT_SET32(IOMUXC_GPR_GPR4, CAN1_STOP_REQ);
        while (!AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN1_STOP_ACK));
    } else if (can_no == 2) {
        AW_REG_BIT_SET32(IOMUXC_GPR_GPR4, CAN2_STOP_REQ);
        while (!AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN2_STOP_ACK));
    }
    while (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_LPMACK)) {
        ;
    }

    return AW_CAN_ERR_NONE;
}

/** \brief CAN唤醒 */
aw_local aw_can_err_t __imx10xx_can_wakeup (struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);
    int                          can_no = -1;

    can_no = __imx10xx_can_no(p_info->reg_base);
    if (can_no == -1) {
        return -AW_CAN_ERR_NO_OP;
    }

    __IMX_CAN_REG_BIT_CLR_MASK32(&p_reg->mcr, (__FLEXCAN_SELFWAKE));

    if (can_no == 1) {
        AW_REG_BIT_CLR32(IOMUXC_GPR_GPR4, CAN1_STOP_REQ);
        while (AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN1_STOP_ACK));
    } else if (can_no == 2) {
        AW_REG_BIT_CLR32(IOMUXC_GPR_GPR4, CAN2_STOP_REQ);
        while (AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN2_STOP_ACK));
    }

    while ((__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_LPMACK)) {
        ;
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 中断使能  */
aw_local aw_can_err_t __imx10xx_can_int_enable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    if (int_mask & AWBL_CAN_INT_WAKE_UP) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_WAKEMSK);

        if (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_WAKEMSK)) {
            return -AW_CAN_ERR_CANNOT_SET_WAKEUPINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_ERROR) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_ERR);

        if (!(__IMX_CAN_REG_READ32(&p_reg->ctrl1) & __FLEXCAN_ERR)) {
            return -AW_CAN_ERR_CANNOT_SET_ERRINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_BUS_OFF) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_BOFF);

        if (!(__IMX_CAN_REG_READ32(&p_reg->ctrl1) & __FLEXCAN_BOFF)) {
            return -AW_CAN_ERR_CANNOT_SET_BOFFINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_RX) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_RX_FIFO);
    }

    if (int_mask & AWBL_CAN_INT_TX) {
#if (__IMX_MBN_SEND_CHN < 32)
        __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_MBN_SEND_CHN);       //shauew tx
#else
        __IMX_CAN_REG_BIT_SET32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));       //shauew tx
#endif
    }

    if (int_mask & AWBL_CAN_INT_WARN) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_WRN);
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_TWRN);
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_RWRN);
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 中断禁能  */
aw_local aw_can_err_t __imx10xx_can_int_disable (
        struct awbl_can_service *p_serv, awbl_can_int_type_t int_mask)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    if ((int_mask > (AWBL_CAN_INT_BUS_OFF |
                     AWBL_CAN_INT_WAKE_UP |
                     AWBL_CAN_INT_TX      |
                     AWBL_CAN_INT_RX))
            && (int_mask != AWBL_CAN_INT_ALL)) {
        return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;
    }

    if (int_mask & AWBL_CAN_INT_WAKE_UP) {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->mcr, __CAN_MCR_WAKEMSK);

        if ((__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_WAKEMSK)) {
            return -AW_CAN_ERR_CANNOT_SET_WAKEUPINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_ERROR) {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_ERR);

        if ((__IMX_CAN_REG_READ32(&p_reg->ctrl1) & __FLEXCAN_ERR)) {
            return -AW_CAN_ERR_CANNOT_SET_ERRINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_BUS_OFF) {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_BOFF);

        if ((__IMX_CAN_REG_READ32(&p_reg->ctrl1) & __FLEXCAN_BOFF)) {
            return -AW_CAN_ERR_CANNOT_SET_BOFFINT;
        }
    }

    if (int_mask & AWBL_CAN_INT_RX) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_RX_FIFO);
    }

    if (int_mask & AWBL_CAN_INT_TX) {
#if (__IMX_MBN_SEND_CHN < 32)
        __IMX_CAN_REG_BIT_CLR32(&p_reg->imask1, __IMX_MBN_SEND_CHN);        //shauew tx
#else
        __IMX_CAN_REG_BIT_CLR32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));        //shauew tx
#endif
    }

    if (int_mask & AWBL_CAN_INT_WARN) {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_TWRN);
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_RWRN);
    }

    return AW_CAN_ERR_NONE;
}


/** \brief 波特率设置 */
aw_local aw_can_err_t __imx10xx_can_baudrate_set (
        struct awbl_can_service *p_serv, const aw_can_baud_param_t *p_can_btr0)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint8_t  pseg1, prop;

    aw_bool_t need_reset_haltfrz = AW_FALSE;

    if (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
        __imx10xx_can_set_haltfrz(p_serv);
        need_reset_haltfrz = AW_TRUE;
    }

    __IMX_CAN_REG_BIT_CLR_MASK32(&p_reg->ctrl1,
                                 __FLEXCAN_PRESDIV |
                                 __FLEXCAN_RJW     |
                                 __FLEXCAN_PSEG1   |
                                 __FLEXCAN_PSEG2   |
                                 __FLEXCAN_PROP);

    /* | SYNC_SEG |     Time Segment 1     | Time Segment 2 | */
    /* |          | (PROP_SEG + PSEG1 + 2) |   (PSEG2 + 1)  | */
    if ((p_can_btr0->tseg1 - 2) > (__FLEXCAN_PSEG1 >> __CAN_CTRL1_PSEG1)) {
        pseg1 = (__FLEXCAN_PSEG1 >> __CAN_CTRL1_PSEG1);
        prop = (p_can_btr0->tseg1 - 2) - pseg1;
    } else {
        pseg1 = p_can_btr0->tseg1 - 2;
        prop = 0;
    }

    __IMX_CAN_REG_BIT_SET_MASK32(
        &p_reg->ctrl1,
        (((p_can_btr0->tseg2 - 1)  << __CAN_CTRL1_PSEG2) |
        ((pseg1)                   << __CAN_CTRL1_PSEG1) |
        ((prop)                    << __CAN_CTRL1_PROP)  |
        ((p_can_btr0->sjw - 1)     << __CAN_CTRL1_RJW)   |
        ((p_can_btr0->brp - 1)     << __CAN_CTRL1_PRESDIV)));


    if (p_can_btr0->smp) {
        __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_SMP);
    } else {
        __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_SMP);
    }

    if ((need_reset_haltfrz) &&
        (__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
        __imx10xx_can_reset_haltfrz(p_serv);
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 波特率获取 */
aw_local aw_can_err_t __imx10xx_can_baudrate_get (
        struct awbl_can_service *p_serv, aw_can_baud_param_t *p_can_btr0)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t regval;

    regval = __IMX_CAN_REG_READ32(&p_reg->ctrl1);

    /* | SYNC_SEG |     Time Segment 1     | Time Segment 2 | */
    /* |          | (PROP_SEG + PSEG1 + 2) |   (PSEG2 + 1)  | */
    p_can_btr0->brp   = 
        ((regval & __FLEXCAN_PRESDIV) >> __CAN_CTRL1_PRESDIV) + 1;
    p_can_btr0->sjw   = ((regval & __FLEXCAN_RJW)     >> __CAN_CTRL1_RJW) + 1;
    p_can_btr0->smp   = ((regval & __FLEXCAN_SMP)     >> __CAN_CTRL1_SMP);
    p_can_btr0->tseg1 = ((regval & __FLEXCAN_PSEG1)   >> __CAN_CTRL1_PSEG1) +
                        ((regval & __FLEXCAN_PROP)    >> __CAN_CTRL1_PROP) +
                        2;
    p_can_btr0->tseg2 = ((regval & __FLEXCAN_PSEG2)   >> __CAN_CTRL1_PSEG2) + 1;
    
    return AW_CAN_ERR_NONE;

}

/** \brief 获取错误计数 */
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_get (
        struct awbl_can_service *p_serv, aw_can_err_cnt_t *p_can_err_reg)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint16_t err_count = __IMX_CAN_REG_READ32(&p_reg->ecr);

    p_can_err_reg->rx_err_cnt = err_count >> 8; /* 高8位为RX_ERR_CNT */
    p_can_err_reg->tx_err_cnt = err_count;      /* 低8位为TX_ERR_CNT */

    return AW_CAN_ERR_NONE;

}

/** \brief 清除错误计数 */
aw_local aw_can_err_t __imx10xx_can_errreg_cnt_clr (
        struct awbl_can_service *p_serv)
{
//    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
//    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
//    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);
//
//    if(!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
//        __imx10xx_can_set_haltfrz(p_serv);
//    }
//
//    /* 清零错误计数器 */
//    __IMX_CAN_REG_WRITE32(&p_reg->ecr, 0x00);
//
//    if(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT) {
//        __imx10xx_can_reset_haltfrz(p_serv);
//    }
//    return AW_CAN_ERR_NONE;
    return -AW_CAN_ERR_NOT_SUPPORT;
}

/** \brief read massage buffer reg */
aw_local aw_can_err_t __imx10xx_can_reg_msg_read (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint8_t  externflag;
    uint8_t  remoteflag;

    aw_can_std_msg_t *p_can_msg = (aw_can_std_msg_t *)p_canmsg;

    uint32_t reg_value  = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_RECV_CHN]) +
                                               __HW_CAN_MBN_BLOCK0);

    p_can_msg->can_msg.timestamp = reg_value & __FLEXCAN_MBN_BLK0_TIME;

    p_can_msg->can_msg.length = (reg_value & __FLEXCAN_MBN_BLK0_LEN) >>
                                 __CAN_MBN_BLK0_LEN;

    p_can_msg->can_msg.flags = 0;

    remoteflag = (reg_value & __FLEXCAN_MBN_BLK0_RTR) ? 1 : 0;
    if (remoteflag){
        p_can_msg->can_msg.flags |= AW_CAN_MSG_FLAG_REMOTE;
    }

    externflag = (reg_value & __FLEXCAN_MBN_BLK0_IDE) ? 1 : 0;

    if (externflag){
        p_can_msg->can_msg.flags |= AW_CAN_MSG_FLAG_EXTERND;
        p_can_msg->can_msg.id = (__IMX_CAN_REG_READ32(
                (uint32_t)(&p_reg->mb[__IMX_MBN_RECV_CHN]) + __HW_CAN_MBN_BLOCK1))
                    & __FLEXCAN_EFF_MASK;
    } else {
        p_can_msg->can_msg.id = (__IMX_CAN_REG_READ32(
                (uint32_t)(&p_reg->mb[__IMX_MBN_RECV_CHN]) + __HW_CAN_MBN_BLOCK1) >>
                __CAN_MBN_BLK1_SFF) & __FLEXCAN_SFF_MASK;
    }

    if (!remoteflag) {
        reg_value  = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_RECV_CHN]) +
                                          __HW_CAN_MBN_BLOCK2);

        p_can_msg->msgbuff[0] = ((__can_mbn_data_t *)&reg_value)->data.byte0;
        p_can_msg->msgbuff[1] = ((__can_mbn_data_t *)&reg_value)->data.byte1;
        p_can_msg->msgbuff[2] = ((__can_mbn_data_t *)&reg_value)->data.byte2;
        p_can_msg->msgbuff[3] = ((__can_mbn_data_t *)&reg_value)->data.byte3;

        reg_value  = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_RECV_CHN]) +
                                          __HW_CAN_MBN_BLOCK3);

        p_can_msg->msgbuff[4] = ((__can_mbn_data_t *) &reg_value)->data.byte0;
        p_can_msg->msgbuff[5] = ((__can_mbn_data_t *) &reg_value)->data.byte1;
        p_can_msg->msgbuff[6] = ((__can_mbn_data_t *) &reg_value)->data.byte2;
        p_can_msg->msgbuff[7] = ((__can_mbn_data_t *) &reg_value)->data.byte3;
    }

    p_can_msg->can_msg.chn = p_info->servinfo.chn;

    __IMX_CAN_REG_READ32(&p_reg->timer);

    /* 接收完数据清除中断标志位 */
    __IMX_CAN_REG_BIT_SET32(&p_reg->iflag1, __IMX_RX_FIFO);

    return AW_CAN_ERR_NONE;
}

/** \brief write massage buffer reg */
aw_local aw_can_err_t __imx10xx_can_reg_msg_write (
        struct awbl_can_service *p_serv, struct aw_can_msg *p_canmsg)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t reg_value;
    uint8_t  externflag;
    uint8_t  remoteflag;
    uint8_t  sendtype;

    aw_can_std_msg_t *p_can_msg = (aw_can_std_msg_t *)p_canmsg;

    externflag = (p_can_msg->can_msg.flags & AW_CAN_MSG_FLAG_EXTERND) ? 1 : 0;
    remoteflag = (p_can_msg->can_msg.flags & AW_CAN_MSG_FLAG_REMOTE) ? 1 : 0;
    sendtype   = (p_can_msg->can_msg.flags & __CAN_SEND_TYPE); /* 发送类型 bit0-3 */

    if(p_can_msg->can_msg.length > __CAN_MAX_LEN) {
        return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
    }

    /* 发送类型判断 */
    if ((sendtype == AW_CAN_MSG_FLAG_SEND_ONCE) || \
        (sendtype == AW_CAN_MSG_FLAG_SEND_SELFONCE)) {
        return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;  /* 该CAN控制器不支持单次传输 */
    }

    switch (sendtype) {

    case AW_CAN_MSG_FLAG_SEND_SELF :
        if (p_this->sendtype != AW_CAN_MSG_FLAG_SEND_SELF) {
            __imx10xx_can_set_haltfrz(p_serv);
            __IMX_CAN_REG_BIT_SET32(&p_reg->ctrl1, __CAN_CTRL1_LPB);
            __IMX_CAN_REG_BIT_CLR32(&p_reg->mcr, __CAN_MCR_SRX);
            p_this->sendtype = AW_CAN_MSG_FLAG_SEND_SELF;
            __imx10xx_can_reset_haltfrz(p_serv);
        }
        break;

    default :
        if (p_this->sendtype != AW_CAN_MSG_FLAG_SEND_NORMAL) {
            __imx10xx_can_set_haltfrz(p_serv);
            __IMX_CAN_REG_BIT_CLR32(&p_reg->ctrl1, __CAN_CTRL1_LPB);
            __IMX_CAN_REG_BIT_SET32(&p_reg->mcr, __CAN_MCR_SRX);
            p_this->sendtype = AW_CAN_MSG_FLAG_SEND_NORMAL;
            __imx10xx_can_reset_haltfrz(p_serv);
        }
        break;
    }

    __IMX_CAN_REG_BIT_CLR_MASK32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) +
                                 __HW_CAN_MBN_BLOCK1, __FLEXCAN_EFF_MASK);

    if (p_can_msg->can_msg.flags & AW_CAN_MSG_FLAG_EXTERND){
        __IMX_CAN_REG_BIT_SET_MASK32(
                (uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) + __HW_CAN_MBN_BLOCK1,
                (p_can_msg->can_msg.id & __FLEXCAN_EFF_MASK));
    } else {
        __IMX_CAN_REG_BIT_SET_MASK32(
                (uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) + __HW_CAN_MBN_BLOCK1,
                (((p_can_msg->can_msg.id) & __FLEXCAN_SFF_MASK) <<
                        __CAN_MBN_BLK1_SFF));
    }

    if (!remoteflag)
    {
        __can_mbn_data_t msgdata;

        msgdata.data.byte0 = p_can_msg->msgbuff[0];
        msgdata.data.byte1 = p_can_msg->msgbuff[1];
        msgdata.data.byte2 = p_can_msg->msgbuff[2];
        msgdata.data.byte3 = p_can_msg->msgbuff[3];

        __IMX_CAN_REG_WRITE32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) + __HW_CAN_MBN_BLOCK2,
                              msgdata.dword);

        msgdata.data.byte0 = p_can_msg->msgbuff[4];
        msgdata.data.byte1 = p_can_msg->msgbuff[5];
        msgdata.data.byte2 = p_can_msg->msgbuff[6];
        msgdata.data.byte3 = p_can_msg->msgbuff[7];

        __IMX_CAN_REG_WRITE32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) + __HW_CAN_MBN_BLOCK3,
                              msgdata.dword);
    }

    reg_value = __CAN_SEND_CODE << __CAN_MBN_BLK0_CODE;

    if (externflag) {
        reg_value |= __FLEXCAN_MBN_BLK0_SSR;
    }

    reg_value |= externflag << __CAN_MBN_BLK0_IDE;

    reg_value |= remoteflag << __CAN_MBN_BLK0_RTR;

    reg_value |= p_can_msg->can_msg.length << __CAN_MBN_BLK0_LEN ;

    reg_value |= p_can_msg->can_msg.timestamp & __FLEXCAN_MBN_BLK0_TIME;

    __IMX_CAN_REG_WRITE32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) + __HW_CAN_MBN_BLOCK0,
                          reg_value);

    return AW_CAN_ERR_NONE;
}

/** \brief stop massage buffer reg send */
aw_local aw_can_err_t __imx10xx_can_msg_snd_stop (
        struct awbl_can_service *p_serv)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t reg_value;
    uint32_t timeout;
#if (__IMX_MBN_SEND_CHN < 32)
    __IMX_CAN_REG_BIT_CLR32(&p_reg->imask1, __IMX_MBN_SEND_CHN);       //shauew tx
#else
    __IMX_CAN_REG_BIT_CLR32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));       //shauew tx
#endif
    reg_value = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) +
                                     __HW_CAN_MBN_BLOCK0);

    reg_value = (reg_value & __FLEXCAN_MBN_BLK0_CODE) >>
                __CAN_MBN_BLK0_CODE;

    if ((reg_value == __CAN_SEND_DONE) || (reg_value == __CAN_SEND_DONE_RTR)) {
#if (__IMX_MBN_SEND_CHN < 32)
        if (__IMX_CAN_REG_READ32(&p_reg->iflag1) & (0x01 << (__IMX_MBN_SEND_CHN))) {   //shauew tx
            __IMX_CAN_REG_BIT_SET32(&p_reg->iflag1, __IMX_MBN_SEND_CHN);
            __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_MBN_SEND_CHN);
            return -AW_CAN_ERR_ABORT_TRANSMITTED;
        } else {
            __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_MBN_SEND_CHN);     //shauew tx
            return AW_CAN_ERR_NONE;
        }
#else
        if (__IMX_CAN_REG_READ32(&p_reg->iflag2) & (0x01 << (__IMX_MBN_SEND_CHN - 32))) {   //shauew tx
            __IMX_CAN_REG_BIT_SET32(&p_reg->iflag2, (__IMX_MBN_SEND_CHN - 32));
            __IMX_CAN_REG_BIT_SET32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));
            return -AW_CAN_ERR_ABORT_TRANSMITTED;
        } else {
            __IMX_CAN_REG_BIT_SET32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));     //shauew tx
            return AW_CAN_ERR_NONE;
        }
#endif
    }

    reg_value = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) +
                                     __HW_CAN_MBN_BLOCK0);

    reg_value &= ~__FLEXCAN_MBN_BLK0_CODE;
    reg_value |= __CAN_SEND_STOP << __CAN_MBN_BLK0_CODE;

    __IMX_CAN_REG_WRITE32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) +
                          __HW_CAN_MBN_BLOCK0, reg_value);

    timeout = 100000;

#if (__IMX_MBN_SEND_CHN < 32)
    while ((!(__IMX_CAN_REG_READ32(&p_reg->iflag1) & (0x01 << __IMX_MBN_SEND_CHN)))  //shauew tx
            && (--timeout)) {
        ;
    }
#else
    while ((!(__IMX_CAN_REG_READ32(&p_reg->iflag2) & (0x01 << (__IMX_MBN_SEND_CHN - 32))))  //shauew tx
            && (--timeout)) {
        ;
    }
#endif
    reg_value = __IMX_CAN_REG_READ32((uint32_t)(&p_reg->mb[__IMX_MBN_SEND_CHN]) +
                                     __HW_CAN_MBN_BLOCK0);

    reg_value = (reg_value & __FLEXCAN_MBN_BLK0_CODE) >> __CAN_MBN_BLK0_CODE;

#if (__IMX_MBN_SEND_CHN < 32)
    __IMX_CAN_REG_BIT_SET32(&p_reg->iflag1, __IMX_MBN_SEND_CHN);    //shauew tx

    __IMX_CAN_REG_BIT_SET32(&p_reg->imask1, __IMX_MBN_SEND_CHN);    //shauew tx
#else
    __IMX_CAN_REG_BIT_SET32(&p_reg->iflag2, (__IMX_MBN_SEND_CHN - 32));    //shauew tx

    __IMX_CAN_REG_BIT_SET32(&p_reg->imask2, (__IMX_MBN_SEND_CHN - 32));    //shauew tx
#endif

    if ((reg_value == __CAN_SEND_DONE ) || (reg_value == __CAN_SEND_DONE_RTR)) {
        return -AW_CAN_ERR_ABORT_TRANSMITTED;
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 验收滤波相关函数 滤波表是以小端模式传输
 *
 *  \note 以下是滤波表设置说明:
 *      1. 每调用滤波设置函数将会把上次的滤波表覆盖, 以最后一次设置的滤波表来进行帧滤波。
 *      2. 当参数length为0时，表示清除所有滤波, 此时可以CAN不对ID和格式等进行滤波, 可以接收所有帧。
 *      3. 当参数length不为0时,表示需要CAN进行滤波设置，滤波表格式如下：
 *          第一段部分表示全ID滤波格式
 *          ----------------------------------------------------------
 *          | rtr(31) | ide(30) | id(29~1) | reserved(0) |
 *          ----------------------------------------------------------
 *          rtr 远程帧标志.若此位为1,则接收对应ID的远程帧,否则接收数据帧
 *          ide 拓展帧标志.若此位为1,则接收对应ID的拓展帧,否则接收标准帧
 *          id  仲裁ID.其中(29~19)为标准帧ID段,(29~1)为拓展帧ID段
 *          rsv 保留位.
 *
 *          第二段部分表示掩码ID, 它与第二段一一对应, 表明当
 *          对应位为1时报文的仲裁位必须符合第二段才接收,对应位为0时不作要求.
 *          ----------------------------------------------------------
 *          | rtr(31) | ide(30) | id(29~1) | reserved(0) |
 *          ----------------------------------------------------------
 *      4. 现在最多可配置32条滤波。
 *
 *     如下给出一个设置滤波的示例：
 *      #define __ID_ARB(id, ext, rmt)  (rmt << 31) | (ext << 30) | (id << (ext ? 1 : 19))
 *      #define __ID_MSK_STD(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 19))  // 标准帧掩码
 *      #define __ID_MSK_EXT(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 1))   // 扩展帧掩码
 *
 *      aw_can_err_t ret;
 *
 *      //接收ID为0x1,0x2,0x3,0x4的标准数据帧, 其他帧过滤掉。 注意：ID条目数不可大于32
 *      uint32_t filter[] = {
 *          //第一段部分表示全ID滤波格式
 *          __ID_ARB(0x1, 0, 0),                     // 仲裁位(标准数据帧) 滤波ID1
 *          __ID_ARB(0x2, 0, 0),                     // 仲裁位(标准数据帧) 滤波ID2
 *          __ID_ARB(0x3, 0, 0),                     // 仲裁位(标准数据帧) 滤波ID3
 *          __ID_ARB(0x4, 0, 0),                     // 仲裁位(标准数据帧) 滤波ID4
 *
 *          //第二段部分表示掩码ID, 它与第二段一一对应. 也就是滤波ID1与掩码ID1对应, 滤波ID2与掩码ID2对应...
 *          __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1), // 掩码位(全标准帧滤波) 掩码ID1
 *          __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1), // 掩码位(全标准帧滤波) 掩码ID2
 *          __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1), // 掩码位(全标准帧滤波) 掩码ID3
 *          __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1), // 掩码位(全标准帧滤波) 掩码ID4
 *      }
 *
 *      //CAN初始化代码
 *
 *      //设置滤波
 *      ret = aw_can_filter_table_set (CAN_CHN, (uint8_t*)&filter, sizeof(filter));
 *      if (ret == AW_CAN_ERR_NONE) {
 *          AW_INFOF(("CAN: filter table set success.\r\n"));
 *      } else {
 *          AW_INFOF(("CAN: filter table set failed.\r\n"));
 *      }
 *
 *      //接收CAN帧
 *
 *      //清除滤波
 *      ret = aw_can_filter_table_set (CAN_CHN, NULL, 0);
 *      if (ret == AW_CAN_ERR_NONE) {
 *          AW_INFOF(("CAN: filter table set success.\r\n"));
 *      } else {
 *          AW_INFOF(("CAN: filter table set failed.\r\n"));
 *      }
 *
 */
aw_local aw_can_err_t __imx10xx_can_filter_table_set (
        struct awbl_can_service *p_serv, uint8_t *p_filterbuff, size_t length)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t i;
    uint32_t filcnt         = ((length / 4) / 2);
    uint32_t rximaskreg     = (uint32_t)(&p_reg->mb[6]);   /* 获取滤波表的首地址*/
    uint32_t *p_filbuf      = (uint32_t *)p_filterbuff;

    /* 参数检查 */
    if ((0 != length) && (p_filterbuff == NULL)) {
        return -AW_CAN_ERR_ILLEGAL_MASK_VALUE;
    }
    if (filcnt > __IMX_TABLE_ID_CNT) {
        return -AW_CAN_ERR_ILLEGAL_DATA_LENGTH;
    }

    /* 记录滤波数目 */
    p_this->filtercnt = filcnt;

    /* 进入冻结模式 */
    if (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
        __imx10xx_can_set_haltfrz(p_serv);
    }

    /**
     * 首先清空之前的滤波配置
     */
    for (i = 0; i < __IMX_TABLE_ID_MAXIMUM; i++) {          /* id */
        __IMX_CAN_REG_WRITE32(rximaskreg, 0);
        rximaskreg += 4;
    }
    for (i = 0; i < __IMX_TABLE_ID_CNT; i++) {              /* 独立mask */
        __IMX_CAN_REG_WRITE32(&p_reg->rximr[i], 0);
    }
    __IMX_CAN_REG_WRITE32(&p_reg->rxfgmask, 0);             /* 全局mask */

    /* 判断是否需要滤波, 长度为0表示不进行滤波 */
    if (length == 0) {

        /* 退出冻结模式 */
        if (__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT) {
            __imx10xx_can_reset_haltfrz(p_serv);
        }
        return AW_CAN_ERR_NONE;
    }

    rximaskreg = (uint32_t)(&p_reg->mb[6]);
    /* 配置滤波ID, 最多可配置__IMX_TABLE_ID_CNT个 */
    for (i = 0; i < filcnt; i++) {
        __IMX_CAN_REG_WRITE32(rximaskreg, *p_filbuf++);
        rximaskreg += 4;
    }

    /* 从__IMX_TABLE_ID_CNT到__IMX_TABLE_ID_MAXIMUM填充滤波ID */
    p_filbuf--;
    for (i = filcnt; i < __IMX_TABLE_ID_MAXIMUM; i++) {
        __IMX_CAN_REG_WRITE32(rximaskreg, *p_filbuf);
        rximaskreg += 4;
    }

    /* 配置独立滤波MASK, 最多可配置__IMX_TABLE_ID_CNT个 */
    p_filbuf++;
    for (i = 0; i < filcnt; i++) {
        __IMX_CAN_REG_WRITE32(&p_reg->rximr[i], *p_filbuf++);
    }

    /* 填充剩下的独立滤波MASK */
    p_filbuf--;
    for (i = filcnt; i < __IMX_TABLE_ID_CNT; i++) {
        __IMX_CAN_REG_WRITE32(&p_reg->rximr[i], *p_filbuf);
    }

    /* 配置全局滤波MASK */
    __IMX_CAN_REG_WRITE32(&p_reg->rxfgmask, *p_filbuf);

    /* 退出冻结模式 */
    if (__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT) {
        __imx10xx_can_reset_haltfrz(p_serv);
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 验收滤波相关函数 */
aw_local aw_can_err_t __imx10xx_can_filter_table_get (
        struct awbl_can_service *p_serv,
        uint8_t                 *p_filterbuff,
        size_t                  *p_length)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);

    uint32_t i;
    uint32_t filcnt = p_this->filtercnt;
    uint32_t *p_filbuf       = NULL;
    uint32_t  rximaskreg     = (uint32_t)(&p_reg->mb[6]);   /* 获取滤波表的首地址*/

    /* 参数检查 */
    if (p_filterbuff == NULL) {
        return -AW_CAN_ERR_INVALID_PARAMETER;
    }

    p_filbuf = (uint32_t *)p_filterbuff;

    /* 进入冻结模式 */
    if (!(__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
        __imx10xx_can_set_haltfrz(p_serv);
    }

    /* 获取ID */
    for (i = 0; i < filcnt; i++) {
        *p_filbuf++ = __IMX_CAN_REG_READ32(rximaskreg);
        rximaskreg   += 4;
    }

    /* 获取MASK */
    for (i = 0; i < filcnt; i++) {
        *p_filbuf = __IMX_CAN_REG_READ32(&p_reg->rximr[i]);
         p_filbuf++;
    }

    /* 退出冻结模式 */
    if ((__IMX_CAN_REG_READ32(&p_reg->mcr) & __FLEXCAN_HALT)) {
        __imx10xx_can_reset_haltfrz(p_serv);
    }

    /* 获取滤波表长度 */
    *p_length = (p_this->filtercnt * 2 * 4);

    return AW_CAN_ERR_NONE;
}

/**
 * \brief CAN bus 中断标志位获取
 * 读完中断位之后 接下来进行判断中断类型，并清除对应中断标志位
 **/
aw_local aw_can_err_t __imx10xx_can_intstatus_get (
        struct awbl_can_service *p_serv,
        awbl_can_int_type_t     *p_int_type,
        aw_can_bus_err_t        *p_bus_err)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));
    imx10xx_can_regs_t          *p_reg  = (imx10xx_can_regs_t *)(p_info->reg_base);
    int                          can_no = -1;

    awbl_can_int_type_t esr1;
    uint32_t iflag1;
#if (__IMX_MBN_SEND_CHN >= 32)
    uint32_t iflag2;
#endif

    *p_int_type = AWBL_CAN_INT_NONE;
    *p_bus_err  = AW_CAN_BUS_ERR_NONE;

    esr1 = __IMX_CAN_REG_READ32(&p_reg->esr1);
    iflag1 = __IMX_CAN_REG_READ32(&p_reg->iflag1);
#if (__IMX_MBN_SEND_CHN >= 32)
    iflag2 = __IMX_CAN_REG_READ32(&p_reg->iflag2);
#endif

    /* 读完中断位之后 接下来进行判断中断类型，并清除对应中断标志位 */
    if (esr1 & __FLEXCAN_ESR1_BOFF) {
        *p_int_type |= AWBL_CAN_INT_BUS_OFF;
        writel((1 << __CAN_ESR1_BOFF), &p_reg->esr1);
    }

    if (esr1 & __FLEXCAN_ESR1_ERR) {
        *p_int_type |= AWBL_CAN_INT_ERROR;
        writel((1 << __CAN_ESR1_ERR), &p_reg->esr1);

        if (((esr1 & __FLEXCAN_ESR1_BIT1) == __FLEXCAN_ESR1_BIT1) ||
            ((esr1 & __FLEXCAN_ESR1_BIT0) == __FLEXCAN_ESR1_BIT0)) {
            *p_bus_err |= AW_CAN_BUS_ERR_BIT;
        }
        if ((esr1 & __FLEXCAN_ESR1_ACK) == __FLEXCAN_ESR1_ACK) {
            *p_bus_err |= AW_CAN_BUS_ERR_ACK;
        }
        if ((esr1 & __FLEXCAN_ESR1_CRC) == __FLEXCAN_ESR1_CRC) {
            *p_bus_err |= AW_CAN_BUS_ERR_CRC;
        }
        if ((esr1 & __FLEXCAN_ESR1_FRM) == __FLEXCAN_ESR1_FRM) {
            *p_bus_err |= AW_CAN_BUS_ERR_FORM;
        }
        if ((esr1 & __FLEXCAN_ESR1_STF) == __FLEXCAN_ESR1_STF) {
            *p_bus_err |= AW_CAN_BUS_ERR_STUFF;
        }
    }

    if (((esr1 & __FLEXCAN_ESR1_TWRN_INT) == __FLEXCAN_ESR1_TWRN_INT) ||
        ((esr1 & __FLEXCAN_ESR1_RWRN_INT) == __FLEXCAN_ESR1_RWRN_INT)) {
        *p_int_type |= AWBL_CAN_INT_WARN;
        writel(((1 << __CAN_ESR1_TWRN) | (1 << __CAN_ESR1_RWRN)), &p_reg->esr1);
    }

    if (((esr1 & __FLEXCAN_ESR1_TX_WRN) == __FLEXCAN_ESR1_TX_WRN) ||
        ((esr1 & __FLEXCAN_ESR1_RX_WRN) == __FLEXCAN_ESR1_RX_WRN)) {
//        writel(((1 << __CAN_ESR1_TW_C) | (1 << __CAN_ESR1_RW_C)), &p_reg->esr1);
    }

    if (esr1 & __FLEXCAN_ESR1_WAK) {
        *p_int_type |= AWBL_CAN_INT_WAKE_UP;
        writel((1 << __CAN_ESR1_WAK), &p_reg->esr1);

        /* 产生唤醒中断后，需要手动退出stop模式 */
        can_no = __imx10xx_can_no(p_info->reg_base);
        if (can_no == 1) {
            AW_REG_BIT_CLR32(IOMUXC_GPR_GPR4, CAN1_STOP_REQ);
            while (AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN1_STOP_ACK));
        } else if (can_no == 2) {
            AW_REG_BIT_CLR32(IOMUXC_GPR_GPR4, CAN2_STOP_REQ);
            while (AW_REG_BIT_ISSET32(IOMUXC_GPR_GPR4, CAN2_STOP_ACK));
        }
    }

    if (iflag1 & (0x01 << __IMX_RX_FIFO)) {
        *p_int_type |= AWBL_CAN_INT_RX;
    }
#if (__IMX_MBN_SEND_CHN < 32)
    if ((__IMX_CAN_REG_READ32(&p_reg->imask1) & ((0x01 << __IMX_MBN_SEND_CHN)))) {
        if (iflag1 & (0x01 << __IMX_MBN_SEND_CHN)) {
            *p_int_type |= AWBL_CAN_INT_TX;
            writel((1 << __IMX_MBN_SEND_CHN), &p_reg->iflag1);
        }
    }
#else
    if ((__IMX_CAN_REG_READ32(&p_reg->imask2) & ((0x01 << (__IMX_MBN_SEND_CHN - 32))))) {
        if (iflag2 & (0x01 << (__IMX_MBN_SEND_CHN - 32))) {
            *p_int_type |= AWBL_CAN_INT_TX;
            writel((1 << (__IMX_MBN_SEND_CHN - 32)), &p_reg->iflag2);
        }
    }
#endif
    return AW_CAN_ERR_NONE;
}


/** \brief 操作寄存器函数接口，特殊的CAN控制器用到 写操作 */
aw_local aw_can_err_t __imx10xx_can_reg_write (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));

    uint32_t regbase  = p_info->reg_base;
    uint32_t i;

    if (offset >= __FLEXCAN_REG_MAX_OFFSET) {
        if ((offset + length) > __FLEXCAN_BUF_MAX_OFFSET) {
            return -AW_CAN_ERR_ILLEGAL_OFFSET;
        }
    } else {
        if ((offset + length) > __FLEXCAN_REG_MAX_OFFSET) {
            return -AW_CAN_ERR_ILLEGAL_OFFSET;
        }
    }

    for (i = 0; i < length; i++) {
        __IMX_CAN_REG_WRITE8((regbase + offset + i), p_data[i]);
    }

    return AW_CAN_ERR_NONE;
}

/** \brief 操作寄存器函数接口，特殊的CAN控制器用到 读操作 */
aw_local aw_can_err_t __imx10xx_can_reg_read (
        struct awbl_can_service *p_serv,
        uint32_t                 offset,
        uint8_t                 *p_data,
        size_t                   length)
{
    awbl_imx10xx_can_dev_t      *p_this = __IMX10xx_CAN_GET_THIS(p_serv);
    awbl_imx10xx_can_dev_info_t *p_info = __IMX10xx_CAN_GET_INFO(&(p_this->dev));

    uint32_t regbase  = p_info->reg_base;
    uint32_t i;

    if (offset >= __FLEXCAN_REG_MAX_OFFSET) {
        if ((offset + length) > __FLEXCAN_BUF_MAX_OFFSET) {
            return -AW_CAN_ERR_ILLEGAL_OFFSET;
        }
    } else {
        if ((offset + length) > __FLEXCAN_REG_MAX_OFFSET) {
            return -AW_CAN_ERR_ILLEGAL_OFFSET;
        }
    }

    for (i = 0; i < length; i++) {
        p_data[i] = __IMX_CAN_REG_READ8((regbase + offset + i));
    }

    return AW_CAN_ERR_NONE;
}
/**
 * \brief register imx10xx can driver
 *
 * This routine registers the imx10xx can driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_imx10xx_flexcan_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_can_drv_registration);
}

/* end of file */

