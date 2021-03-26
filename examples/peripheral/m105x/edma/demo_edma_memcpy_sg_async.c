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
 * \brief edma例程,内存到内存的异步传输测试（分散/聚集传输）
 *
 * - 操作步骤：
 *   1. 需要在aw_prj_params.h头文件里使能
 *      - AW_DEV_IMX1050_EDMA
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_edma_memcpy_sg_async.c src_edma_memcpy_sg_async
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_edma_memcpy_sg_async EDMA(异步传输)
 * \copydoc demo_edma_memcpy_sg_async.c
 */

/** [src_edma_memcpy_sg_async] */

#include "aworks.h"
#include "aw_cache.h"
#include "aw_delay.h"
#include "aw_sem.h"
#include "string.h"
#include "driver/edma/aw_imx10xx_edma.h"
#include "aw_vdebug.h"


/* 异步传输的回调函数 */
static void __edma_chan_start_sync_callback (void *pVoid, unsigned int len)
{
    aw_semb_id_t            sem_id;

    sem_id = (aw_semb_id_t)pVoid;

    aw_semb_give(sem_id);
}



/* 内存到内存的异步传输测试（分散/聚集传输） */
void demo_edma_memcpy_sg_async_entry (void)
{
    void                    *chan       = NULL;
    struct imx_dma_data      data       = {0};
    struct dma_slave_config  cfg        = {0};
    char                    *p_dst, *p_src;
    int                      ret;
    struct scatterlist       dst_list[4];
    struct scatterlist       src_list[4];
    const int                len        = 1024;
    aw_semb_id_t             sem_id;
    AW_SEMB_DECL(sem);

    AW_INFOF(("demo_edma_memcpy_sg_async_entry start...\r\n"));

    /* 申请目标缓冲区 */
    p_dst = (char *)aw_cache_dma_align(len, 4);
    if (p_dst == NULL) {
        AW_INFOF(("aw_cache_dma_align error\r\n"));
        return ;
    }

    /* 申请源缓冲区 */
    p_src = (char *)aw_cache_dma_align(len, 4);
    if (p_src == NULL) {
        return ;
    }

    memset(p_dst,'3', 1024);
    memset(p_src,'6', 1024);

    ret = memcmp(p_src, p_dst, len);
    if (ret == 0) {
        AW_INFOF(("p_src and p_dst equal!\r\n"));
        return ;
    }

    /* 根据优先级申请一个通道 */
    data.peripheral_type = IMX_DMATYPE_MEM;
    data.priority        = DMA_PRIO_1;
    chan = imx10xx_edma_alloc_chan(&data);
    if (chan == NULL) {
        AW_INFOF(("imx10xx_edma_alloc_chan error!\r\n"));
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
        AW_INFOF(("imx10xx_edma_control error!\r\n"));
        return ;
    }

    dst_list[0].dma_address = (dma_addr_t)p_dst;
    dst_list[0].length      = len / 4;
    dst_list[1].dma_address = (dma_addr_t)((uint32_t)p_dst + 256);
    dst_list[1].length      = len / 4;
    dst_list[2].dma_address = (dma_addr_t)((uint32_t)p_dst + 512);
    dst_list[2].length      = len / 4;
    dst_list[3].dma_address = (dma_addr_t)((uint32_t)p_dst + 768);
    dst_list[3].length      = len / 4;

    src_list[0].dma_address = (dma_addr_t)p_src;
    src_list[0].length      = len / 4;
    src_list[1].dma_address = (dma_addr_t)((uint32_t)p_src + 256);
    src_list[1].length      = len / 4;
    src_list[2].dma_address = (dma_addr_t)((uint32_t)p_src + 512);
    src_list[2].length      = len / 4;
    src_list[3].dma_address = (dma_addr_t)((uint32_t)p_src + 768);
    src_list[3].length      = len / 4;

    /* 准备一个传输 */
    ret = imx10xx_edma_prep_memcpy_sg(chan, dst_list, 4, src_list, 4, 0);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_prep_memcpy_sg error!\r\n"));
        return ;
    }

    sem_id = AW_SEMB_INIT(sem,AW_SEM_EMPTY,AW_SEM_Q_FIFO);

    /* 开始传输 */
    ret = imx10xx_edma_chan_start_async(chan, \
                    __edma_chan_start_sync_callback, (void *)sem_id);
    if (ret != AW_OK) {
        AW_INFOF(("imx10xx_edma_chan_start_async error!\r\n"));
        return ;
    }

    /* 等待信号量 */
    ret = aw_semb_take(sem_id, 100);
    if (ret != 0) {
        return ;
    }

    ret = memcmp(p_src, p_dst, len);

    /* 释放通道 */
    imx10xx_edma_free_chan(chan);

    aw_cache_dma_free(p_dst);
    aw_cache_dma_free(p_src);

    if (ret != 0) {
        AW_INFOF(("data verify fail!\r\n"));
    }

    AW_INFOF(("demo_edma_memcpy_sg_async_entry test OK!\r\n"));
    return ;
}

/** [src_edma_memcpy_sg_async] */

/* end of file */
