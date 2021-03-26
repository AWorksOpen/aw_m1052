/*******************************************************************************
*                                 AWorks
*                       ---------------------------
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
 * \brief The IMX1010 Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX1010_SAI_H
#define __AWBL_IMX1010_SAI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aworks.h"
#include "aw_cpu_clk.h"
#include "aw_sem.h"
#include "aw_task.h"
#include "awbus_lite.h"
#include "aw_pool.h"

#include "soc/awsa_soc_internal.h"


#define AWBL_IMX1010_SAI_NAME   "awbl_imx1010_sai"


/* IMX1010 support 3 SAI, SAI1 ~ SAI3 */
#define AWBL_IMX1010_SAI_NUM     3

#define AW_BL_IMX1010_SAI_MCLK_SEL_0   0
#define AW_BL_IMX1010_SAI_MCLK_SEL_1   1
#define AW_BL_IMX1010_SAI_MCLK_SEL_2   2
#define AW_BL_IMX1010_SAI_MCLK_SEL_3   3


/* 采样频率 */
#define SAI_SAMPLE_RATE     44100


/**
 * \brief imx1010 SAI 设备信息
 */
struct awbl_imx1010_sai_devinfo {

    uint32_t        regbase;            /**< \brief 寄存器基地址        */
    int             inum;               /**< \brief 中断号       */
    aw_clk_id_t     ref_clk_id;         /**< \brief refence clock id */
    uint8_t         mclk_sel;           /**< \brief clk_sel:0 ~ 3 */
    void (*pfn_plfm_init)(void);        /**< \brief 平台相关初始化   */
    int             tx_dma_prep_type;   /**< \brief dma 外设的请求类型 */
};

/**
 * \brief imx1010 SAI 设备实例
 */
struct awbl_imx1010_sai_dev {

    struct awbl_dev           dev;
    void                     *p_tx_dma_chan;
    uint32_t                  dma_period;
    uint32_t                  dma_size;
    int                       format;
    uint32_t                 *p_dma_area;
    uint32_t                  app_ptr;
    awsa_pcm_stream_t         stream;
    struct awsa_soc_platform  soc_plfm;
    struct awsa_soc_dai       plfm_dai;
    struct awsa_soc_pcm      *p_soc_pcm;
    AW_MUTEX_DECL(mutex_dev);
};

/**
 * \brief imx1010 SAI 驱动注册
 */
void awbl_imx1010_sai_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

