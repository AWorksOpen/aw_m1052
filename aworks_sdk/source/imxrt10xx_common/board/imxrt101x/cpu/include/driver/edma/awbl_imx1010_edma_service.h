/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief AWBus imx1010 eDMA 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_imx1010_edma_service.h"
 * \endcode
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-07  hsg, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX1010_EDMA_SERVICE_H
#define __AWBL_IMX1010_EDMA_SERVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/edma/aw_imx1010_edma.h"
#include "awbus_lite.h"

typedef void * (*pfn_imx1010_edma_alloc_chan_t) (awbl_dev_t *p_dev,
        struct imx_dma_data *data);

typedef int (* pfn_imx1010_edma_control_t)(void *chan, enum dma_ctrl_cmd cmd,
        unsigned long arg);

typedef int (* pfn_imx1010_edma_prep_memcpy_t)(void *chan,
        dma_addr_t dma_dst,
        dma_addr_t dma_src,
        size_t len,
        unsigned long flags);
typedef int (*pfn_imx1010_edma_prep_sg_t)(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

typedef int (*pfn_imx1010_edma_prep_memcpy_sg_t)(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        unsigned long flags);

typedef int (*pfn_imx1010_edma_preq_slave_sg_t)(void *chan,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

typedef int (* pfn_imx1010_edma_chan_start_sync_t)(void *chan,uint32_t timeout);
typedef int (*pfn_imx1010_edma_chan_start_async_t)(void *chan,
        pfn_dma_callback_t callback,void *param);
typedef int (*pfn_imx1010_edma_chan_stop_t)(void *chan);
typedef int (*pfn_imx1010_edma_free_chan_t) (void *chan);
typedef int (*pfn_imx1010_edma_prep_dma_cyclic_t)(
        void *chan, dma_addr_t dma_addr, size_t buf_len,
        size_t period_len, enum dma_transfer_direction direction);
typedef int (*pfn_imx1010_edma_get_transfer_count_t)(void *chan);

typedef struct imx1010_edma_serv_func {
    pfn_imx1010_edma_alloc_chan_t           pfn_alloc_chan;
    pfn_imx1010_edma_control_t              pfn_control;
    pfn_imx1010_edma_prep_memcpy_t          pfn_prep_memcpy;
    pfn_imx1010_edma_prep_sg_t              pfn_preq_sg;
    pfn_imx1010_edma_prep_memcpy_sg_t       pfn_preq_memcpy_sg;
    pfn_imx1010_edma_preq_slave_sg_t        pfn_preq_slave_sg;
    pfn_imx1010_edma_prep_dma_cyclic_t      pfn_preq_dma_cyclic;
    pfn_imx1010_edma_chan_start_sync_t      pfn_chan_start_sync;
    pfn_imx1010_edma_chan_start_async_t     pfn_chan_start_async;
    pfn_imx1010_edma_get_transfer_count_t   pfn_get_transfer_count;
    pfn_imx1010_edma_chan_stop_t            pfn_chan_stop;
    pfn_imx1010_edma_free_chan_t            pfn_chan_free;

}imx1010_edma_serv_func_t;

/** \brief AWBus imx1010 edma service instance */
struct awbl_imx1010_edma_service {
    struct  awbl_dev                   *p_dev;
    const imx1010_edma_serv_func_t      *p_serv_func;
};


/** \brief AWBus method, used to get a AWBus ADC service
 *
 * a driver can implement the method 'awbl_adcserv_get'
 * to provide a AWBus ADC service. \see awbl_adc_methods.c
 */
AWBL_METHOD_IMPORT(awbl_imx1010_edma_serv_func_get);

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_IMX1010_EDMA_SERVICE_H */

/* end of file */

