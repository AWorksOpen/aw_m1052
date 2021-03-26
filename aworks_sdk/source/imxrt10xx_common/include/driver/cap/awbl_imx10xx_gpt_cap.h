/*******************************************************************************
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
 * \brief iMX RT10xx GPT CAP驱动
 *
 * 本驱动实现了定时器的捕获功能。
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-09  licl, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10xx_GPT_CAP_H
#define __AWBL_IMX10xx_GPT_CAP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "apollo.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "awbl_cap.h"
#include "driver/timer/imx10xx_gpt_regs.h"

#define AWBL_IMX10XX_GPT_CAP_NAME   "imx10xx_gpt_cap"

/** \brief CAP 通道选择列表 */
typedef enum gpt_cap_channel_sel
{
    GPT_CAP_Channel_0 = 0,    /**< \brief GPT CAP Channel 0 */
    GPT_CAP_Channel_1,        /**< \brief GPT CAP Channel 1 */
    GPT_CAP_Channel_all,
} gpt_cap_channel_sel_t;

/** \brief iMX RT10xx GPT CAP 设备配置 */
typedef struct awbl_imx10xx_gpt_cap_devcfg {

    uint8_t                 pid;        /**< \brief CAP ID */
    gpt_cap_channel_sel_t   ch;         /**< \brief CAP对应的通道 */
    int                     gpio;       /**< \brief CAP对应的引脚 */
    uint32_t                func;       /**< \brief CAP引脚功能 */
} awbl_imx10xx_gpt_cap_devcfg_t;

/** \brief 设备配置信息 */
typedef struct awbl_imx10xx_gpt_cap_devinfo {
    uint32_t                    regbase;        /**< \brief 寄存器基地址 */
    const aw_clk_id_t           clk_id;         /**< \brief 时钟 ID */
    int                         inum;           /**< \brief 中断号 */

    /** \brief CAP 配置信息 */
    aw_const struct awbl_imx10xx_gpt_cap_devcfg *cap_devcfg;
    uint8_t                     pnum;           /**< \brief 使用到的CAP通道数*/

    struct awbl_cap_servinfo    cap_servinfo;

    /** \brief 指向平台初始化函数的指针 */
    void                        (*pfn_plfm_init) (void);
} awbl_imx10xx_gpt_cap_devinfo_t;

/** \brief 设备实例 */
typedef struct awbl_imx10xx_gpt_cap_dev {
    awbl_dev_t                  dev;            /**< \brief AWBus 设备实例结构体 */
    struct awbl_cap_service     cap_serv;       /**< \brief 每个设备提供一个CAP服务给上层 */

    struct {
        cap_callback_t          callback_func;  /**< \brief callback function  */
        void                   *p_arg;          /**< \brief The parameter for callback function */
    } callback_info[2];                         /**< \brief Most support 2 channel */
} awbl_imx10xx_gpt_cap_dev_t;

/**
 * \brief 将 IMX10xx GPT CAP 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_gpt_cap_drv_register (void);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_IMX10xx_GPT_CAP_H */

/* end of file */
