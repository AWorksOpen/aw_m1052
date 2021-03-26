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
 * \brief The IMX1050 Synchronous Audio Interface driver
 *
 * \internal
 * \par modification history:
 * - 1.00 16-06-13, tee, first implementation.
 * \endinternal
 */
#ifndef __AWBL_IMX1050_SAI_H
#define __AWBL_IMX1050_SAI_H

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


#define AWBL_IMX1050_SAI_NAME   "awbl_imx1050_sai"


/* IMX1050 support 3 SAI, SAI1 ~ SAI3 */
#define AWBL_IMX1050_SAI_NUM     3

#define AW_BL_IMX1050_SAI_MCLK_SEL_0   0
#define AW_BL_IMX1050_SAI_MCLK_SEL_1   1
#define AW_BL_IMX1050_SAI_MCLK_SEL_2   2
#define AW_BL_IMX1050_SAI_MCLK_SEL_3   3


/* 采样频率 */
#define SAI_SAMPLE_RATE     44100


/**
 * \brief imx1050 SAI 设备信息
 */
struct awbl_imx1050_sai_devinfo {

    uint32_t        regbase;              /**< \brief 寄存器基地址        */
    int             inum;                 /**< \brief 中断号       */
    aw_clk_id_t     ref_clk_id;           /**< \brief refence clock id */
    uint8_t         mclk_sel;             /**< \brief clk_sel:0 ~ 3 */

    const char     *p_plfm_name[2];       /**< \brief soc platform name */
    const char     *p_dais_name[1];       /**< \brief soc platform dai name */
    uint32_t        dai_fmt;              /**< \brief soc platform dai fmt */

    void          (*pfn_plfm_init)(void); /**< \brief 平台相关初始化   */
    int             tx_dma_prep_type;     /**< \brief dma 外设的请求类型 */
    int             rx_dma_prep_type;     /**< \brief dma 外设的请求类型 */
};

/**
 * \brief imx1050 SAI 流信息
 */
struct awbl_imx1050_sai_stream {
    struct awsa_soc_platform   soc_plfm;
    struct awsa_soc_dai        plfm_dai;
    void                      *p_dma_chan;
    uint32_t                   dma_period;
    uint32_t                   dma_size;
    awsa_pcm_format_t          format;
    awsa_pcm_stream_t          stream;
    uint32_t                  *p_dma_area;
    uint32_t                   app_ptr;
    aw_bool_t                  enable;
    void                      *p_sai_dev;
};

/**
 * \brief imx1050 SAI 设备实例
 */
struct awbl_imx1050_sai_dev {

    struct awbl_dev                 dev;
    struct awbl_imx1050_sai_stream  stream[2];

    AW_MUTEX_DECL(mutex_dev);
};

/**
 * \brief imx1050 SAI 驱动注册
 */
void awbl_imx1050_sai_drv_register (void);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif

