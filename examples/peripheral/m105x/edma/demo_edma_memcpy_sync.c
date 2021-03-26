/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief edma例程(内存到内存同步传输测试)
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_DEV_IMX1050_EDMA
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_edma_memcpy_sync.c src_edma_memcpy_sync
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_edma_memcpy_sync EDMA(同步传输)
 * \copydoc demo_edma_memcpy_sync.c
 */

/** [src_edma_memcpy_sync] */

#include "aworks.h"
#include "aw_cache.h"
#include "string.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_vdebug.h"

/*
 * /brief 例程入口
 */
void demo_edma_memcpy_sync_entry (void)
{
    void                       *chan = NULL;
    struct imx_dma_data         data = {0};
    char                       *p_dst, *p_src;
    struct dma_slave_config     cfg  = {0};
    int                         len  = 1024;
    int                         ret;

    AW_INFOF(("demo_edma_memcpy_sync_entry start...\r\n"));

    /* 申请目标缓冲区 */
    p_dst = (char *)aw_cache_dma_align(len, 4);
    if (p_dst == NULL) {
        return ;
    }

    /* 申请源缓冲区 */
    p_src = (char *)aw_cache_dma_align(len, 4);
    if (p_src == NULL) {
        AW_INFOF(("申请缓存失败\r\n"));
        return ;
    }

    memset(p_dst, 'c', 1024);
    memset(p_src, '7', 1024);

    ret = memcmp(p_src, p_dst, len);
    if (ret == 0) {
        AW_INFOF(("数据处理失败\r\n"));
        return ;
    }

    /* 根据优先级申请一个通道 */
    data.peripheral_type = IMX_DMATYPE_MEM;
    data.priority        = DMA_PRIO_1;
    chan = imx10xx_edma_alloc_chan(&data);
    if (chan == NULL) {
        AW_INFOF(("分配通道失败\r\n"));
        return ;
    }

    /* 设置通道的属性 */
    cfg.direction       = DMA_MEM_TO_MEM;
    cfg.dst_addr_width  = DMA_SLAVE_BUSWIDTH_4_BYTES;
    cfg.src_addr_width  = DMA_SLAVE_BUSWIDTH_4_BYTES;
    cfg.dst_addr        = (dma_addr_t)p_dst;
    cfg.src_addr        = (dma_addr_t)p_src;
    ret = imx10xx_edma_control(chan, DMA_SLAVE_CONFIG, (unsigned long)&cfg);
    if (ret != AW_OK) {
        return ;
    }

    /* 准备一个传输 */
    ret = imx10xx_edma_prep_memcpy(chan, (dma_addr_t)p_dst,
                (dma_addr_t)p_src, len, 0);
    if (ret != AW_OK) {
        return ;
    }

    /* 开始传输 */
    ret = imx10xx_edma_chan_start_sync(chan, 20);
    if (ret != AW_OK) {
        return ;
    }

    ret = memcmp(p_src, p_dst, len);
    if (ret != 0) {
        AW_INFOF(("数据校验失败\r\n"));
        return ;
    }

    /* 释放通道 */
    ret = imx10xx_edma_free_chan(chan);

    aw_cache_dma_free(p_dst);
    aw_cache_dma_free(p_src);
    if (ret != AW_OK) {
        AW_INFOF(("释放通道失败\r\n"));
        return ;
    }

    AW_INFOF(("demo_edma_memcpy_sync_entry test OK!\r\n"));
    return ;
}

/** [src_edma_memcpy_sync] */

/* end of file */
