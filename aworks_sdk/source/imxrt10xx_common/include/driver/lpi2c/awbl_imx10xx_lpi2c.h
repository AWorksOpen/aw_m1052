/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-present Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief imx10xx lpi2c driver  header file
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-18  like, first implementation
 * \endinternal
 */

#ifndef AWBL_IMX10XX_LPI2C_H
#define AWBL_IMX10XX_LPI2C_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbl_i2cbus.h"
#include "aw_i2c.h"
#include "aw_sem.h"
#include "aw_cpu_clk.h"

#define AWBL_IMX10XX_LPI2C_NAME               "imx10xx_lpi2c"

/**
 * \brief imx10xx LPI2C 设备信息
 */
struct awbl_imx10xx_lpi2c_devinfo {

    /** \brief 继承自 AWBus LPI2C 控制器设备信息 */
    struct awbl_i2c_master_devinfo lpi2c_master_devinfo;

    uint32_t    sda_filter_wide;        /**< SDA滤波宽度  (ns) */
    uint32_t    scl_filter_wide;        /**< SCL滤波宽度  (ns) */
    uint32_t    bus_idle_timeout;       /**< 总线空闲时间 */

    uint32_t    regbase;                /**< 寄存器基地址 */
    int         inum;                   /**< 中断号 */
    aw_clk_id_t ref_clk_id;             /**< 输入时钟ID */
    void (*pfunc_plfm_init)(void);      /**< 平台相关初始化：开启时钟、初始化引脚 */
    void (*pfunc_plfm_rst)(void);       /**< i2c死锁处理 */
};

/**
 * \brief imx10xx I2C 设备实例
 */
struct awbl_imx10xx_lpi2c_dev {

    /** \brief 继承自 AWBus I2C 主机控制器 */
    struct awbl_i2c_master  super;

    /** \brief 传输数据计数 */
    size_t      data_ptr;

    /** \brief 用于同步传输任务的信号量 */
    AW_SEMB_DECL(i2c_sem);

    /** \brief 停止信号控制标记  */
    uint32_t    stop_ctrl_flg;

    /** \brief  当前 trans 索引值 */
    uint32_t    cur_trans_index;

    /** \brief imxrt1050 lpi2c 配置信息结构体变量 */
    struct aw_i2c_config i2c_cfg_par;

    /* 传递给中断的写buffer数组  */
    struct awbl_trans_buf  *p_trans_buf;

    /* 传递给中断的写buffer个数 */
    uint32_t                trans_buf_num;
	
    /* 出错计数  */
    uint32_t err_count;
};

/**
 * \brief imx10xx LPI2C master driver register
 */
void awbl_imx10xx_lpi2c_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */


#endif /* AWBL_IMX10XX_LPI2C_H */
