/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief imx10xx eDMA driver header file
 *
 *   eEMA 可以实现外围I/O设备与内部、外部储存的数据传输
 * \internal
 * \par Modification History
 * - 1.00 17-11-07  hsg, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_EDMA_H
#define __AWBL_IMX10xx_EDMA_H

#include "aworks.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "driver/edma/awbl_imx10xx_edma_service.h"
#include "driver/edma/imx10xx_edma_regs.h"

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

struct edma_engine;

struct edma_channel {
    struct edma_engine             *edma;              /* 指向eDMA设备结构体  */
    uint32_t                        alloced;            /* 该通道是否被申请标志 */
    uint32_t                        channel;            /* 通道号 */
    enum dma_transfer_direction     direction;          /* 传输方向 */
    enum edma_peripheral_type       peripheral_type;    /* 外设类型 */
    dma_addr_t                      base_addr;          /* 保存一个地址用于计算传输的数目(用于非循环模式) */
    enum dma_slave_buswidth         src_width;          /* 源地址宽度 */
    enum dma_slave_buswidth         dst_width;          /* 目标地址宽度 */
    enum dma_status                 status;             /* 传输的状态 */

    uint32_t                        tcd_usd;            /* 申请了TCD存储空间  */
    void                            *p_tcd;              /* TCD存储空间的指针 */

    uint32_t                        circle;             /* 是否为循环传输  */

    uint32_t                        count;              /* 传输的数目  */

    enum dma_chan_cfg_status        cfg;                /* 通道配置状态  */

    pfn_dma_callback_t              fn_callback;        /* 回调函数 */
    void                           *param;              /* 调用回调函数的参数 */

    AW_MUTEX_DECL(chan_lock);

};

#define     DMA_INTR_TASK_STACK_SIZE    512

/**
 * \brief EDMA 设备结构体
 */
typedef struct edma_engine {
    struct awbl_dev                 dev;
    struct edma_channel             channel[MAX_EDMA_CHANNELS];

    AW_TASK_DECL(dma_intr_task, DMA_INTR_TASK_STACK_SIZE);   /* DMA中断任务 */
    AW_SEMB_DECL(dma_intr_semb);                            /* 表征是否有dma中断产生 */

    AW_MUTEX_DECL(dev_lock);
    struct awbl_imx10xx_edma_service edma_serv;

} awbl_imx10xx_edma_dev_t;

/**
 * \brief imx10xx eDMA  设备信息
 */
typedef struct awbl_imx10xx_edma_devinfo {

    /** \brief edma寄存器基地址 */
    uint32_t regbase;

    /** \brief dmamux寄存器基地址  */
    uint32_t dmamux_regbase;

    /** \brief eEMA 中断号入口 */
    const uint32_t *inum;

    /** \brief 中断号长度 */
    uint32_t inum_len;

    uint32_t err_inum;

    /** \brief 平台相关初始化 */
    void (*pfunc_plfm_init)(void);
} awbl_imx10xx_edma_devinfo_t;


void awbl_imx10xx_edma_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

/* end of file */


