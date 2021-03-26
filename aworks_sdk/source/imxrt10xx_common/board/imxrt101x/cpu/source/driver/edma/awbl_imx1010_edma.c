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
*******************************************************************************/

/**
 * \file
 * \brief imx1010 eDMA 驱动源文件
 *
 * \internal
 * \par Modification History
 * - 1.00 17-11-07  hsg, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_cache.h"
#include "aw_mem.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include <string.h>
#include "aw_assert.h"
#include "aw_bitops.h"
#include "driver/edma/awbl_imx1010_edma.h"
#include "driver/edma/imx1010_edma_regs.h"


/** \brief get awbl_imx1010_edma_chan_dev from awbl_dev */
#define __TO_GET_EDMA_DEV(p_dev)   \
       (awbl_imx1010_edma_dev_t *)(p_dev)

/** \brief get awbl_imx1010_edma_devinfo_t from awbl_dev */
#define __TO_GET_EDMA_DEVINFO(p_dev)   \
       (awbl_imx1010_edma_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

/**
 * \brief 通道申请
 */
aw_local void *__edma_alloc_chan_resources (
    awbl_dev_t *p_dev, struct imx_dma_data *data)
{
    uint32_t                     i;
    awbl_imx1010_edma_dev_t     *edma           = __TO_GET_EDMA_DEV(p_dev);
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(p_dev);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    rt1010_dmamux_regs_t        *p_hw_dmamux    = (rt1010_dmamux_regs_t *)p_edma_devinfo->dmamux_regbase;
    struct edma_channel         *edmac          = NULL;

    /* 判断是否需要一个指定的优先级 */
    if (data->priority == DMA_PRIO_ANY) {   /* 任意优先级 */

        /* 查找一个没有分配的通道 */
        for (i = 0; i < MAX_EDMA_CHANNELS; i++) {
            if (!(edma->channel[i].alloced)) {
                edmac = &(edma->channel[i]);
                break;
            }
        }
    } else {                                /* 指定优先级 */
        if (!(edma->channel[data->priority].alloced)) {
            edmac = &(edma->channel[data->priority]);
        } else if (!(edma->channel[data->priority + 15].alloced)) {
            edmac = &(edma->channel[data->priority + 15]);
        }
    }

    /* 未申请到通道  */
    if (edmac == NULL) {
        return NULL;
    }

    /* 在设置DMAMUX 前将寄存器清零 */
    AW_REG_WRITE32(&(p_hw_dmamux->CHCFG[edmac->channel]), 0);

    /* 根据外设类型设置DMA请求 */
    if (data->peripheral_type == IMX_DMATYPE_MEM) {
        /* 打开DMA请求(enable always on) */
        AW_REG_BIT_SET32(&(p_hw_dmamux->CHCFG[edmac->channel]), 29);
    } else {
        AW_REG_BITS_SET32(&(p_hw_dmamux->CHCFG[edmac->channel]),
                          0,
                          7,
                          data->peripheral_type);
    }

    /* 使能 DMAMUX 通道 */
    AW_REG_BIT_SET32(&(p_hw_dmamux->CHCFG[edmac->channel]), 31);

    /* 清零所有寄存器 */
    p_hw_edma->TCD[edmac->channel].SADDR            = 0;
    p_hw_edma->TCD[edmac->channel].SOFF             = 0;
    p_hw_edma->TCD[edmac->channel].ATTR             = 0;
    p_hw_edma->TCD[edmac->channel].NBYTES_MLNO      = 0;
    p_hw_edma->TCD[edmac->channel].SLAST            = 0;
    p_hw_edma->TCD[edmac->channel].DADDR            = 0;
    p_hw_edma->TCD[edmac->channel].DOFF             = 0;
    p_hw_edma->TCD[edmac->channel].CITER_ELINKNO    = 0;
    p_hw_edma->TCD[edmac->channel].DLAST_SGA        = 0;
    p_hw_edma->TCD[edmac->channel].CSR              = 0;
    p_hw_edma->TCD[edmac->channel].BITER_ELINKNO    = 0;

    edmac->alloced          = 1;     /* 标志该通道已经被申请  */
    edmac->peripheral_type  = data->peripheral_type;

    return edmac;
}

/**
 * \brief 配置已经申请了的通道
 */
aw_local int __edma_channel_cfg (void *chan, struct dma_slave_config *cfg)
{
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    uint32_t                    dst_width, src_width;

    /* check if edma is busy */
    if ((p_hw_edma->TCD[edmac->channel].CSR != 0) &&
        (p_hw_edma->TCD[edmac->channel].CSR & 0x80) == 0) {
        return AW_EBUSY;
    }

    /* 目标地址宽度 */
    switch (cfg->dst_addr_width)
    {

    case DMA_SLAVE_BUSWIDTH_1_BYTE:
        p_hw_edma->TCD[edmac->channel].ATTR |= 0;
        dst_width = 1;
        break;

    case DMA_SLAVE_BUSWIDTH_2_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= 1;
        dst_width = 2;
        break;

    case DMA_SLAVE_BUSWIDTH_4_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= 2;
        dst_width = 4;
        break;

    case DMA_SLAVE_BUSWIDTH_16_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= 4;
        dst_width = 16;
        break;

    case DMA_SLAVE_BUSWIDTH_32_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= 5;
        dst_width = 32;
        break;

    default:
        break;
    }

    /* 源地址宽度 */
    switch (cfg->src_addr_width)
    {

    case DMA_SLAVE_BUSWIDTH_1_BYTE:
        p_hw_edma->TCD[edmac->channel].ATTR |= (0 << 8);
        src_width = 1;
        break;

    case DMA_SLAVE_BUSWIDTH_2_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= (1 << 8);
        src_width = 2;
        break;

    case DMA_SLAVE_BUSWIDTH_4_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= (2 << 8);
        src_width = 4;
        break;

    case DMA_SLAVE_BUSWIDTH_16_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= (4 << 8);
        src_width = 16;
        break;

    case DMA_SLAVE_BUSWIDTH_32_BYTES:
        p_hw_edma->TCD[edmac->channel].ATTR |= (5 << 8);
        src_width = 32;
        break;

    default:
        break;
    }

    /* 根据传输的方向配置地址偏移  */
    if (cfg->direction == DMA_MEM_TO_MEM) {
        p_hw_edma->TCD[edmac->channel].SOFF = src_width;
        p_hw_edma->TCD[edmac->channel].DOFF = dst_width;
    } else if (cfg->direction == DMA_MEM_TO_DEV) {
        p_hw_edma->TCD[edmac->channel].SOFF = src_width;
        p_hw_edma->TCD[edmac->channel].DOFF = 0;
    } else if (cfg->direction == DMA_DEV_TO_MEM) {
        p_hw_edma->TCD[edmac->channel].SOFF = 0;
        p_hw_edma->TCD[edmac->channel].DOFF = dst_width;
    }

    /* 配置源地址 */
    p_hw_edma->TCD[edmac->channel].SADDR = cfg->src_addr;

    /* 配置目标地址 */
    p_hw_edma->TCD[edmac->channel].DADDR = cfg->dst_addr;

    /* 保存地址宽度和传输方向  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->direction = cfg->direction;
    edmac->dst_width = dst_width;
    edmac->src_width = src_width;

    if (edmac->cfg == CHAN_CFG_0) {
        edmac->cfg = CHAN_CFG_1;
    }

    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return AW_OK;

}

/**
 * \brief 通道控制
 */
aw_local int __edma_control (
    void *chan, enum dma_ctrl_cmd cmd, unsigned long arg)
{
    int                      ret   = -AW_EINVAL;
    struct dma_slave_config *cfg   = NULL;

    if (cmd == DMA_SLAVE_CONFIG) {
        cfg = (struct dma_slave_config *)arg;
        ret = __edma_channel_cfg(chan, cfg);
    }

    return ret;
}

/**
 * \brief 内存拷贝准备
 */
aw_local int __edma_prep_memcpy (
    void           *chan,
    dma_addr_t      dma_dst,
    dma_addr_t      dma_src,
    size_t          len,
    unsigned long   flags)
{

    int                          ret            = -AW_EINVAL;
    uint32_t                     max_addr_width;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    /* 计算最大的地址宽度 */
    max_addr_width = (edmac->src_width > edmac->dst_width) ? \
                        edmac->src_width : edmac->dst_width;

    /* 参数检查  */
    if (
        (edmac == NULL) ||
        (len == 0) ||
        (edmac->cfg == CHAN_CFG_0) ||
        (((len % edmac->dst_width) != 0) || ((len % edmac->src_width) != 0))
       ) {
        return ret;
    }

    /* check if edma is busy */
    if ((p_hw_edma->TCD[edmac->channel].CSR != 0) &&
        (p_hw_edma->TCD[edmac->channel].CSR & 0x80) == 0) {
        return AW_EBUSY;
    }

    /* 配置目标地址和源地址 */
    p_hw_edma->TCD[edmac->channel].DADDR = dma_dst;
    p_hw_edma->TCD[edmac->channel].SADDR = dma_src;

    /* 配置地址宽度 */
    p_hw_edma->TCD[edmac->channel].DOFF = edmac->dst_width;
    p_hw_edma->TCD[edmac->channel].SOFF = edmac->src_width;

    /* 设置目标地址自增, 源地址自增 */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      30,
                      2,
                      3);

    /* 配置传输的数目 */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      0,
                      30,
                      max_addr_width);
    p_hw_edma->TCD[edmac->channel].BITER_ELINKNO = len / max_addr_width;
    p_hw_edma->TCD[edmac->channel].CITER_ELINKNO = len / max_addr_width;

    /* 将attr的smod和dmod清空(不使用循环模式) */
    p_hw_edma->TCD[edmac->channel].ATTR &= ~(0xF8F8);

    /* 配置CSR寄存器 */
    p_hw_edma->TCD[edmac->channel].CSR = 0;
    p_hw_edma->TCD[edmac->channel].CSR |= 0x8;
    p_hw_edma->TCD[edmac->channel].CSR |= 0x2;

    /* 保存传输的数目  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->direction    = DMA_MEM_TO_MEM;
    edmac->count        = len;
    edmac->cfg          = CHAN_CFG_2;
    edmac->circle       = 0;
    edmac->base_addr    = dma_dst;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    ret = AW_OK;
    return ret;

}

/**
 * \brief 设置一个通道的 scatter/gather 链接
 */
aw_local int __edm_channel_sg_link_cfg (
    void                *chan,
    struct scatterlist  *dst_sg,
    unsigned int         dst_count,
    struct scatterlist  *src_sg,
    unsigned int         src_count)
{

    int                          ret            = -AW_EINVAL;
    uint32_t                     i              = 0;
    uint32_t                     max_addr_width;
    uint32_t                     max_count;
    __edma_tcd_t                *p_tcd            = NULL;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    uint16_t                     data_temp;

    /* 计算最大的地址宽度 */
    max_addr_width = (edmac->src_width > edmac->dst_width) ? \
                        edmac->src_width : edmac->dst_width;

    /* 最大的s/g块个数 */
    max_count = (src_count > dst_count) ? (src_count - 1) : (dst_count - 1);

    /* 参数检查 */
    if ((NULL == chan) ||
        (((dst_count != 0) && (src_count != 0)) && (dst_count != src_count))) {
        return ret;
    }

    /* 保证源和目的的每个块的长度相同 */
    if ((dst_count != 0) && (src_count != 0)) {
        for (i = 0; i < dst_count; i++) {
            if (dst_sg[i].length != src_sg[i].length) {
                return ret;
            }
        }
    }

    /* 如果上一个申请的TCD空间没有free，先释放  */
    if (edmac->p_tcd != NULL) {
        aw_cache_dma_free(edmac->p_tcd);
    }

    /* 申请内存保存TCD  */
    p_tcd = (__edma_tcd_t *)aw_cache_dma_malloc(sizeof(__edma_tcd_t) * max_count);
    if (NULL == p_tcd) {
        return -AW_ENOMEM;
    }

    memset(p_tcd, 0, sizeof(__edma_tcd_t) * max_count);

    /* 第一个传输块链接到p_tcd[0]的地址 */
    p_hw_edma->TCD[edmac->channel].DLAST_SGA = (uint32_t)&p_tcd[0];

    for (i = 0; i < max_count; i++) {

        p_tcd[i].ATTR = p_hw_edma->TCD[edmac->channel].ATTR;
        p_tcd[i].DOFF = p_hw_edma->TCD[edmac->channel].DOFF;
        p_tcd[i].SOFF = p_hw_edma->TCD[edmac->channel].SOFF;
        p_tcd[i].CSR  = p_hw_edma->TCD[edmac->channel].CSR;

        /* 保存源或目的地址偏移使能 */
        p_tcd[i].NBYTES = p_hw_edma->TCD[edmac->channel].NBYTES_MLNO;

        /* 配置传输的数目 */
        AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                          0,
                          30,
                          max_addr_width);

        p_tcd[i].BITER = src_sg[i + 1].length / max_addr_width;
        p_tcd[i].CITER = src_sg[i + 1].length / max_addr_width;

        if (dst_count != 0) {
            p_tcd[i].DADDR = dst_sg[i + 1].dma_address;
        } else {
            p_tcd[i].DADDR = p_hw_edma->TCD[edmac->channel].DADDR;
        }

        if (src_count != 0) {
            p_tcd[i].SADDR      = src_sg[i + 1].dma_address;
        } else {
            p_tcd[i].SADDR      = p_hw_edma->TCD[edmac->channel].SADDR;
        }

        /* 链接到下一个传输块 */
        p_tcd[i].DLAST_SGA  = (uint32_t)&p_tcd[i + 1];

    }

    /* 修改最后一个块的CSR */
    data_temp = p_tcd[max_count - 1].CSR;
    AW_BIT_SET(data_temp, 1);       /* 使能传输完成中断  */
    AW_BIT_SET(data_temp, 3);       /* 传输完成后禁能通道请求  */
    AW_BIT_CLR(data_temp, 4);       /* 禁能S/G  */
    p_tcd[max_count - 1].CSR = data_temp;

    /* 保存p_tcd[]的基址  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->p_tcd    = (void *)p_tcd;
    edmac->tcd_usd  = 1;        /* 申请了TCD空间 */
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return AW_OK;
}


/**
 * \brief 分散/聚集传输准备
 */
aw_local int __edma_prep_sg (
    void                       *chan,
    struct scatterlist         *dst_sg,
    unsigned int                dst_nents,
    struct scatterlist         *src_sg,
    unsigned int                src_nents,
    enum dma_transfer_direction direction)
{
    /* 函数暂时未实现  */
    return AW_ERROR;
}

/**
 * \brief 内存拷贝分散/聚集传输准备
 */
aw_local int __edma_prep_memcpy_sg (
    void               *chan,
    struct scatterlist *dst_sg,
    unsigned int        dst_count,
    struct scatterlist *src_sg,
    unsigned int        src_count,
    unsigned long       flags)
{
    int                          ret            = -AW_EINVAL;
    uint32_t                     max_addr_width;
    uint32_t                     i;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    uint16_t                     data_temp;

    /* 计算最大的地址宽度 */
    max_addr_width = (edmac->src_width > edmac->dst_width) ? \
                        edmac->src_width : edmac->dst_width;

    /* 参数检查 */
    if ((chan == NULL) ||
        ((src_count < 2) || (dst_count < 2)) ||
        (src_count != dst_count) ||
        (edmac->cfg == CHAN_CFG_0)) {
        return ret;
    }
    for (i = 0; i < src_count; i++) {
        if(((dst_sg[i].length % edmac->dst_width) != 0) ||
           ((src_sg[i].length % edmac->src_width) != 0) ||
           (dst_sg[i].length != src_sg[i].length)) {
            return ret;
        }
    }

    /* check if edma is busy */
    if ((p_hw_edma->TCD[edmac->channel].CSR != 0) &&
        (p_hw_edma->TCD[edmac->channel].CSR & 0x80) == 0) {
        return AW_EBUSY;
    }

    /* 配置目标地址和源地址 */
    p_hw_edma->TCD[edmac->channel].DADDR = dst_sg[0].dma_address;
    p_hw_edma->TCD[edmac->channel].SADDR = src_sg[0].dma_address;

    /* 配置地址宽度 */
    p_hw_edma->TCD[edmac->channel].DOFF = edmac->dst_width;
    p_hw_edma->TCD[edmac->channel].SOFF = edmac->src_width;

    /* 设置目标地址自增, 源地址自增 */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      30,
                      2,
                      3);

    /* 配置传输的数目 */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      0,
                      30,
                      max_addr_width);
    p_hw_edma->TCD[edmac->channel].BITER_ELINKNO = src_sg[0].length / max_addr_width;
    p_hw_edma->TCD[edmac->channel].CITER_ELINKNO = src_sg[0].length / max_addr_width;

    /* 将attr的smod和dmod清空(不使用循环模式) */
    p_hw_edma->TCD[edmac->channel].ATTR &= ~(0xF8F8);

    /* 配置CSR寄存器 */
    p_hw_edma->TCD[edmac->channel].CSR = 0;
    data_temp = p_hw_edma->TCD[edmac->channel].CSR;
    AW_BIT_CLR(data_temp, 1);       /* 禁能传输完成中断  */
    AW_BIT_CLR(data_temp, 3);       /* 传输完成后不禁能通道请求  */
    AW_BIT_SET(data_temp, 4);       /* 使能 S/G  */
    p_hw_edma->TCD[edmac->channel].CSR = data_temp;

    /* 调用sg_link函数 填充传输块  */
    ret = __edm_channel_sg_link_cfg(chan, dst_sg, dst_count, src_sg, src_count);
    if (ret != AW_OK) {
        return ret;
    }

    /* 保存传输的数目  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->direction    = DMA_MEM_TO_MEM;
    edmac->count        = dst_sg[0].length;
    edmac->cfg          = CHAN_CFG_2;
    edmac->circle       = 0;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return AW_OK;
}

/**
 * \brief 从机分散/聚集传输准备
 */
aw_local int __edma_prep_slave_sg (
    void                       *chan,
    struct scatterlist         *src_sg,
    unsigned int                src_count,
    enum dma_transfer_direction direction)
{

    int                          ret            = -AW_EINVAL;
    uint32_t                     max_addr_width;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    uint16_t                     data_temp;

    /* 计算最大的地址宽度 */
    max_addr_width = (edmac->src_width > edmac->dst_width) ? \
                        edmac->src_width : edmac->dst_width;

    /* 参数检查 */
    if ((chan == NULL) ||
        (src_count == 0) ||
        ((src_sg->length == 0) || (src_sg->length > 0x3FFFFFFF)) ||
        ((src_sg->length % max_addr_width) != 0) ||
        (edmac->cfg == CHAN_CFG_0) ||
        ((direction != DMA_MEM_TO_DEV) && (direction != DMA_DEV_TO_MEM))) {
        return ret;
    }

    /* 根据传输方向配置:地址、地址宽度、地址自增 */
    if (direction == DMA_MEM_TO_DEV) {
        p_hw_edma->TCD[edmac->channel].SOFF     = edmac->src_width;
        p_hw_edma->TCD[edmac->channel].SADDR    = src_sg->dma_address;
        p_hw_edma->TCD[edmac->channel].DOFF     = 0;

        /* 设置源地址自增, 目标地址不自增 */
        AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                          30,
                          2,
                          2);
    } else if (direction == DMA_DEV_TO_MEM) {
        p_hw_edma->TCD[edmac->channel].SOFF     = 0;
        p_hw_edma->TCD[edmac->channel].DADDR    = src_sg->dma_address;
        p_hw_edma->TCD[edmac->channel].DOFF     = edmac->dst_width;

        /* 设置目标地址自增, 源地址不自增 */
        AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                          30,
                          2,
                          1);
    }

    /* 将attr的smod和dmod清空(不使用循环模式) */
    p_hw_edma->TCD[edmac->channel].ATTR &= ~(0xF8F8);

    /* 配置传输的数目 */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      0,
                      30,
                      max_addr_width);
    p_hw_edma->TCD[edmac->channel].BITER_ELINKNO = src_sg->length / max_addr_width;
    p_hw_edma->TCD[edmac->channel].CITER_ELINKNO = src_sg->length / max_addr_width;

    /* 配置CSR寄存器 */
    p_hw_edma->TCD[edmac->channel].CSR = 0;
    data_temp = p_hw_edma->TCD[edmac->channel].CSR;
    if (src_count == 1) {   /* 如果只有一个传输块 */
        AW_BIT_SET(data_temp, 1);       /* 使能传输完成中断  */
        AW_BIT_SET(data_temp, 3);       /* 传输完成后禁能通道请求  */
        AW_BIT_CLR(data_temp, 4);       /* 禁能S/G  */
        p_hw_edma->TCD[edmac->channel].CSR = data_temp;
    } else {                /* 有多个传输块 */
        AW_BIT_CLR(data_temp, 1);       /* 禁能传输完成中断  */
        AW_BIT_CLR(data_temp, 3);       /* 传输完成后不禁能通道请求  */
        AW_BIT_SET(data_temp, 4);       /* 使能 S/G  */
        p_hw_edma->TCD[edmac->channel].CSR = data_temp;

        /* 调用sg_link函数 填充传输块  */
        ret = __edm_channel_sg_link_cfg(chan, NULL, 0, src_sg, src_count);
        if (ret != AW_OK) {
            return ret;
        }
    }


    /* 保存传输的数目  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->direction    = direction;
    edmac->count        = src_sg->length;
    edmac->cfg          = CHAN_CFG_2;
    edmac->circle       = 0;
    edmac->base_addr    = src_sg->dma_address;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return AW_OK;
}

/**
 * \brief 循环传输准备
 */
aw_local int __edma_prep_dma_cyclic (
    void                       *chan,
    dma_addr_t                  dma_addr,
    size_t                      buf_len,
    size_t                      period_len,
    enum dma_transfer_direction direction)
{

    int                          ret            = -AW_EINVAL;
    size_t                       temp;
    uint32_t                     max_addr_width;
    uint32_t                     modulo;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    /* 计算最大的地址宽度 */
    max_addr_width = (edmac->src_width > edmac->dst_width) ? \
                        edmac->src_width : edmac->dst_width;

    /* 参数检查 */
    if ((chan == NULL) ||
        ((period_len == 0) || (period_len > 0x3FFFFFFF)) ||
        (buf_len > (0x1 << 31)) ||
        (edmac->cfg == CHAN_CFG_0) ||
        ((dma_addr % buf_len) != 0) ||
        ((period_len % max_addr_width) != 0) ||
        ((buf_len % period_len) != 0) ||
        ((direction != DMA_MEM_TO_DEV) && (direction != DMA_DEV_TO_MEM))) {
        return ret;
    }

    /* 检查buf_len是否为2的幂 */
    temp = buf_len;
    modulo = 0;
    while ((temp > 1) && ((temp % 2) == 0)) {
        temp = (temp >> 1);
        modulo++;
    }
    if (temp != 1) {     /* 不是2的幂 */
        return ret;
    }

    /* 将attr的smod和dmod清空 */
    p_hw_edma->TCD[edmac->channel].ATTR &= ~(0xF8F8);

    /* 根据传输方向配置:地址、地址宽度、地址自增 */
    if (direction == DMA_MEM_TO_DEV) {
        p_hw_edma->TCD[edmac->channel].SOFF     = edmac->src_width;
        p_hw_edma->TCD[edmac->channel].SADDR    = dma_addr;
        p_hw_edma->TCD[edmac->channel].DOFF     = 0;

        /* 设置源地址自增, 目标地址不自增 */
        AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                          30,
                          2,
                          2);

        /* 配置tcd.attr(配置循环模式地址范围限制) */
        p_hw_edma->TCD[edmac->channel].ATTR |= ((modulo << 11) & 0xF800);
    } else if (direction == DMA_DEV_TO_MEM) {
        p_hw_edma->TCD[edmac->channel].SOFF     = 0;
        p_hw_edma->TCD[edmac->channel].DADDR    = dma_addr;
        p_hw_edma->TCD[edmac->channel].DOFF     = edmac->dst_width;

        /* 设置目标地址自增, 源地址不自增 */
        AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                          30,
                          2,
                          1);

        /* 配置tcd.attr(配置循环模式地址范围限制) */
        p_hw_edma->TCD[edmac->channel].ATTR |= ((modulo << 3) & 0xF8);
    }

    /* 配置传输的数目(内层循环传输的数目按最大的地址宽度来传输) */
    AW_REG_BITS_SET32(&(p_hw_edma->TCD[edmac->channel].NBYTES_MLNO),
                      0,
                      30,
                      max_addr_width);
    p_hw_edma->TCD[edmac->channel].BITER_ELINKNO = period_len / max_addr_width;
    p_hw_edma->TCD[edmac->channel].CITER_ELINKNO = period_len / max_addr_width;

    /* 配置CSR寄存器 */
    p_hw_edma->TCD[edmac->channel].CSR = 0;
    p_hw_edma->TCD[edmac->channel].CSR &= ~0x8;    /* 不自动关闭DAM请求  */
    p_hw_edma->TCD[edmac->channel].CSR |= 0x2;

    /* 保存传输的数目  */
    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->direction    = direction;
    edmac->count        = period_len;
    edmac->cfg          = CHAN_CFG_2;
    edmac->circle       = 1;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return AW_OK;

}

/**
 * \brief 同步传输的回调函数
 */
aw_local void __edma_channel_start_sync_callback (void *p_void, unsigned int len)
{
    aw_semb_id_t            sem_id;

    sem_id = (aw_semb_id_t)p_void;

    aw_semb_give(sem_id);
}


/**
 * \brief 启动同步传输
 */
aw_local int __edma_chan_start_sync (void *chan, uint32_t timeout)
{
    int                          ret            = -AW_EINVAL;
    aw_semb_id_t                 sem_id;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    AW_SEMB_DECL(sem);

    /* 检查参数 */
    if ((edmac == NULL) ||
        (edmac->cfg != CHAN_CFG_2)) {
        return ret;
    }

    if (edmac->fn_callback != NULL) {
        ret = -AW_EDEADLK;
        return ret;
    }

    if (0 == timeout) {
        timeout = 1;
    }

    sem_id = AW_SEMB_INIT(sem, AW_SEM_EMPTY,AW_SEM_Q_FIFO);

    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->fn_callback = __edma_channel_start_sync_callback;
    edmac->param       = (void *)sem_id;
    edmac->status      = DMA_IN_PROGRESS;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    /* 开启传输 */
    p_hw_edma->SERQ = edmac->channel;

    /* 等待信号量  */
    ret = aw_semb_take(sem_id, timeout);

    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->fn_callback   = NULL;
    edmac->param         = 0;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return ret;
}

/**
 * \brief 启动异步传输
 */
aw_local int __edma_chan_start_async (
    void *chan, pfn_dma_callback_t callback, void *param)
{
    int                          ret            = -AW_EINVAL;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    /* 检查参数 */
    if ((edmac == NULL) ||
        (edmac->cfg != CHAN_CFG_2)) {
        return ret;
    }

    if (edmac->fn_callback != NULL) {
        ret = -AW_EDEADLK;
        return ret;
    }

    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    edmac->fn_callback   = callback;
    edmac->param         = param;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    /* 开启传输 */
    p_hw_edma->SERQ = edmac->channel;

    return AW_OK;
}

/**
 * \brief 获取传输数目
 */
aw_local int __edma_get_transfer_count (void *chan)
{
    /* 函数暂时未实现  */
    return AW_ERROR;
}

/**
 * \brief 停止传输
 */
aw_local int __edma_chan_stop(void *chan)
{
    int                          ret            = -AW_EINVAL;
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    /* 参数检查  */
    if (edmac == NULL) {
        return ret;
    }

    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);
    if (edmac->fn_callback) {           /* 停止时删除回调  */
        edmac->fn_callback = NULL;
    }
    edmac->status = DMA_PAUSED;
    AW_MUTEX_UNLOCK(edmac->chan_lock);

    /* 取消传输  */
    p_hw_edma->CERQ = edmac->channel;

    return AW_OK;

}

/**
 * \brief 释放通道
 */
aw_local int __edma_free_chan_resources(void *chan)
{
    struct edma_channel         *edmac          = (struct edma_channel *)chan;
    awbl_imx1010_edma_dev_t     *edma           = edmac->edma;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(edma);
    rt1010_dmamux_regs_t        *p_hw_dmamux    = (rt1010_dmamux_regs_t *)p_edma_devinfo->dmamux_regbase;

    /* 在释放通道时将DMAMUX寄存器复位 */
    AW_REG_WRITE32(&(p_hw_dmamux->CHCFG[edmac->channel]), 0);

    /* 释放申请到的TCD空间  */
    if ((edmac->tcd_usd) && (edmac->p_tcd != NULL)) {
        aw_cache_dma_free(edmac->p_tcd);
    }

    AW_MUTEX_LOCK(edmac->chan_lock, AW_WAIT_FOREVER);

    edmac->fn_callback = NULL;
    edmac->param       = 0;
    edmac->direction   = DMA_TRANS_NONE;
    edmac->cfg         = CHAN_CFG_0;
    edmac->count       = 0;
    edmac->status      = DMA_PAUSED;
    edmac->circle      = 0;
    edmac->tcd_usd     = 0;
    edmac->p_tcd       = NULL;
    edmac->alloced     = 0;             /* 通道被释放  */

    AW_MUTEX_UNLOCK(edmac->chan_lock);

    return 0;
}


static void edma_intr_task_fn(void *p)
{
    uint32_t                     i;
    uint32_t                    ret;
    awbl_imx1010_edma_dev_t     *edma = (awbl_imx1010_edma_dev_t *)p;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(p);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    while (1) {
        AW_SEMB_TAKE(edma->dma_intr_semb, AW_WAIT_FOREVER);
        for (i = 0; i < MAX_EDMA_CHANNELS; i++) {

            /* 读取DONE bit and ERROR bit */
            ret |= ((p_hw_edma->TCD[i].CSR & 0x80) >> 7);
            ret |= (((p_hw_edma->ERR >> i) & 0x01) << 1);

            /* 判断是否传输完成  */
            if (AW_REG_BIT_ISSET32(&p_hw_edma->INT, i)) {

                /* 清除中断 */
                p_hw_edma->CINT = i;

                if (edma->channel[i].circle) {  /* 循环模式  */
                    /* 调用回调  */
                    if (edma->channel[i].fn_callback) {
                        edma->channel[i].fn_callback(edma->channel[i].param,
                                                 edma->channel[i].count);
                    }
                } else {                        /* 其他模式  */
                    /* 调用回调  */
                    if (edma->channel[i].fn_callback) {
                        edma->channel[i].fn_callback(edma->channel[i].param,
                                                 edma->channel[i].count);
                        edma->channel[i].fn_callback = NULL;  /* 调用回调后将回调函数删除 */
                        edma->channel[i].status      = DMA_COMPLETE;
                    }
                }
            }
        }

        /* 打开中断  */
        for (i = 0; i < p_edma_devinfo->inum_len; i++) {
            aw_int_enable(p_edma_devinfo->inum[i]);
        }
    }



}

/**
 * \brief ISR for imx1010 eDMA interrupts
 *
 * This routine handles the imx1010 eDMA interrupts. This function
 * is hooked as the ISR in the second level initialization routine.
 */
aw_local void __imx1010_edma_isr (void *p)
{
    uint32_t i;
    awbl_imx1010_edma_dev_t     *edma = (awbl_imx1010_edma_dev_t *)p;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(p);

    for (i = 0; i < p_edma_devinfo->inum_len; i++) {
        aw_int_disable(p_edma_devinfo->inum[i]);
    }

    AW_SEMB_GIVE(edma->dma_intr_semb);
}

/* 错误中断处理函数 */
aw_local void __imx1010_edma_err_isr (void *p)
{
    uint32_t                    i;
    uint32_t                    es, err;
    uint32_t                    count;
    awbl_imx1010_edma_dev_t     *edma           = (awbl_imx1010_edma_dev_t *)p;
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(p);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;

    err = p_hw_edma->ERR;
    if (err) {
        for (i = 0; i < 32; i++) {
            if (AW_BIT_ISSET(err, i)) {
                break;
            }
        }

        /* 清除错误 */
        p_hw_edma->CERR = i;

        /* 停止传输 */
        __edma_chan_stop(&edma->channel[i]);

        /* 设置错误状态  */
        edma->channel[i].status = DMA_ERROR;

        if (edma->channel[i].tcd_usd == 0) {    /* 没有使用s/g传输 */
            /* 计算传输的数目  */
            if (edma->channel[i].direction == DMA_MEM_TO_DEV) {
                count = (uint32_t)p_hw_edma->TCD[i].SADDR - (uint32_t)edma->channel[i].base_addr;
            } else {
                count = (uint32_t)p_hw_edma->TCD[i].DADDR - (uint32_t)edma->channel[i].base_addr;
            }
        }

        /* 调用回调函数  */
        if (edma->channel[i].fn_callback) {
            edma->channel[i].fn_callback(edma->channel[i].param,
                                         count);
        }

        es = p_hw_edma->ES;

    }




}

/**
 * \brief 第二阶段初始化
 */
aw_local void __imx1010_edma_inst_init2 (awbl_dev_t *p_dev)
{

    uint8_t                      i;
    uint32_t                     temp;
    awbl_imx1010_edma_dev_t     *edma           = __TO_GET_EDMA_DEV(p_dev);
    awbl_imx1010_edma_devinfo_t *p_edma_devinfo = __TO_GET_EDMA_DEVINFO(p_dev);
    rt1010_edma_regs_t          *p_hw_edma      = (rt1010_edma_regs_t *)p_edma_devinfo->regbase;
    struct edma_channel         *edmac          = NULL;

    if (p_edma_devinfo->pfunc_plfm_init) {
        p_edma_devinfo->pfunc_plfm_init();
    }

    /* 初始化设备锁 */
    AW_MUTEX_INIT(edma->dev_lock,AW_SEM_Q_FIFO);

    /* initialize channel parameters */
    for (i = 0; i < MAX_EDMA_CHANNELS; i++) {
       edmac = &edma->channel[i];

       edmac->edma          = edma;
       edmac->fn_callback   = NULL;
       edmac->channel       = i;
       edmac->cfg           = CHAN_CFG_0;
       edmac->count         = 0;
       edmac->status        = DMA_PAUSED;
       edmac->circle        = 0;
       edmac->tcd_usd       = 0;
       edmac->p_tcd         = NULL;
       edmac->alloced       = 0;    /* 标志通道未被申请 */
       AW_MUTEX_INIT(edmac->chan_lock,AW_SEM_Q_FIFO);
    }

    /* edma init */
    temp = AW_REG_READ32(&p_hw_edma->CR);
    AW_BIT_CLR(temp, 2);        /* channel fix priority arbitration */
    AW_BIT_SET(temp, 4);        /* HOE bit set */
    AW_BIT_CLR(temp, 6);        /* CLM bit reset */
    AW_BIT_CLR(temp, 1);        /* EDBG bit reset */
    AW_BIT_SET(temp, 7);        /* EMLM bit set */
    AW_REG_WRITE32(&p_hw_edma->CR, temp);

    /* 初始化一个任务来处理DMA中断 */
    AW_SEMB_INIT(edma->dma_intr_semb,AW_SEM_EMPTY,AW_SEM_Q_FIFO);
    AW_TASK_INIT(edma->dma_intr_task,"edma_intr_task",
            AW_TASK_SYS_PRIORITY(1),DMA_INTR_TASK_STACK_SIZE,
            edma_intr_task_fn,edma);
    AW_TASK_STARTUP(edma->dma_intr_task);

    /* 使能中断 */
    for(i = 0; i < p_edma_devinfo->inum_len; i++) {
        aw_int_connect(p_edma_devinfo->inum[i], __imx1010_edma_isr, edma);
        aw_int_enable(p_edma_devinfo->inum[i]);
    }

    /* 使能错误中断   */
    p_hw_edma->SEEI = (0x01 << 6);
    aw_int_connect(p_edma_devinfo->err_inum, __imx1010_edma_err_isr, edma);
    aw_int_enable(p_edma_devinfo->err_inum);

}
/**
 * \brief driver functions (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvfuncs __g_edma_drvfuncs = {
    NULL,                       /**< \brief 1st inst init */
    __imx1010_edma_inst_init2,  /**< \brief 2ed inst init */
    NULL                        /**< \brief inst int connect */
};


aw_local const imx1010_edma_serv_func_t __g_edma_serv_func = {
    __edma_alloc_chan_resources,
    __edma_control,
    __edma_prep_memcpy,
    __edma_prep_sg,
    __edma_prep_memcpy_sg,
    __edma_prep_slave_sg,
    __edma_prep_dma_cyclic,
    __edma_chan_start_sync,
    __edma_chan_start_async,
    __edma_get_transfer_count,
    __edma_chan_stop,
    __edma_free_chan_resources
};


/**
 * \brief method "awbl_adcserv_get" handler
 */
aw_local aw_err_t __imx1010_edma_serv_get (
        struct awbl_dev *p_dev, void *p_arg)
{
     awbl_imx1010_edma_dev_t            *edma = __TO_GET_EDMA_DEV(p_dev);
     struct awbl_imx1010_edma_service   *p;

     p = &edma->edma_serv;
     p->p_dev = p_dev;
     p->p_serv_func = &__g_edma_serv_func;

     *(struct awbl_imx1010_edma_service **)p_arg = p;
    return AW_OK;
}


/* driver methods (must defined as aw_const) */
aw_local aw_const struct awbl_dev_method __g_imx1010_edma_dev_methods[] = {
    AWBL_METHOD(awbl_imx1010_edma_serv_func_get, __imx1010_edma_serv_get),
    AWBL_METHOD_END
};

/**
 * \brief driver registration (must defined as aw_const)
 */
aw_local aw_const awbl_plb_drvinfo_t __g_edma_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_IMX1010_EDMA_NAME,                 /* p_drvname */
        &__g_edma_drvfuncs,                     /* p_busfuncs */
        __g_imx1010_edma_dev_methods,           /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};


/******************************************************************************/
/**
 * \brief EDMA 驱动注册
 */
void awbl_imx1010_edma_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_edma_drv_registration);
}

/* end of file */



