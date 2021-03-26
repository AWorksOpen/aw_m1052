/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10XX LPSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-17  mex, first implementation
 * \endinternal
 */

#ifndef __AWBL_IMX10XX_LPSPI_H
#define __AWBL_IMX10XX_LPSPI_H

#include "awbl_spibus.h"
#include "aw_task.h"
#include "aw_sem.h"
#include "aw_cpu_clk.h"


#define AWBL_IMX10XX_LPSPI_NAME   "imx10xx_lpspi"


/**
 * \brief iMX RT10XX LPSPI 设备信息
 */
typedef struct awbl_imx10xx_lpspi_devinfo {

    /** \brief 继承自 AWBus SPI 控制器设备信息 */
    struct awbl_spi_master_devinfo spi_master_devinfo;

    uint32_t         regbase;            /**< \brief 寄存器基地址 */
    int              inum;               /**< \brief 中断号 */
    aw_clk_id_t      ref_clk_id;         /**< \brief the clock which the counter use */
    aw_bool_t        hard_cs;            /**< \brief 硬件CS模式 */
    aw_bool_t        cs_active_H;        /**< \brief 硬件CS高电平有效 */
    uint8_t          cs_sck;             /**< \brief cs有效到第一个sck edge的delay cycle */
    uint8_t          sck_cs;             /**< \brief 最后一个sck edge到cs无效的delay cycle */
    /** \brief 平台相关初始化：开启时钟、初始化引脚等操作 */
    void (*pfunc_plfm_init)(void);

} awbl_imx10xx_lpspi_devinfo_t;

/**
 * \brief iMX RT10XX  SPI 设备实例
 */
typedef struct awbl_imx10xx_lpspi_dev {

    /** \brief 继承自 AWBus SPI 主机控制器 */
    struct awbl_spi_master  super;

    uint32_t    write_index;
    uint32_t    read_nbytes;
    uint32_t    nbytes_to_recv;   /**< /brief 待接收的字节数 */
    const void  *p_write_buf;
    void        *p_read_buf;
    void       (*bpw_func) (void* p_arg);
    uint32_t    cur_speed;
    uint8_t     cur_cs_sck_t;
    uint8_t     cur_bpw;
    aw_bool_t   continuous_flg;

    /** \brief 用于同步传输任务的信号量 */
    AW_SEMB_DECL(lpspi_sem_sync);

} awbl_imx10xx_lpspi_dev_t;


/**
 * \brief iMX RT10XX LPSPI master driver register
 */
void awbl_imx10xx_lpspi_drv_register (void);


#endif /* __AWBL_IMX10XX_LPSPI_H */
