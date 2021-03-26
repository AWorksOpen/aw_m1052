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
 * \brief IMX10xx FlexIO UART Driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-02  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/
#include "aworks.h"
#include "aw_int.h"
#include "aw_serial.h"
#include "aw_assert.h"
#include "aw_bitops.h"
#include "driver/flexio/imx10xx_flexio_regs.h"
#include "driver/flexio/awbl_imx10xx_flexio_uart.h"

/*******************************************************************************
  宏定义
*******************************************************************************/
#define __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev)   \
          struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo = \
          (struct awbl_imx10xx_flexio_uart_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev)   \
          struct awbl_imx10xx_flexio_uart_chan *p_chan = \
          &(((struct awbl_imx10xx_flexio_uart_dev *)p_dev)->chan)

#define __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan) \
          struct awbl_dev *p_dev =                     \
          &AW_CONTAINER_OF(p_chan, struct awbl_imx10xx_flexio_uart_dev, chan)->dev;

#define __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan) \
          struct awbl_imx10xx_flexio_uart_chan *p_chan =         \
          AW_CONTAINER_OF(p_sio_chan, struct awbl_imx10xx_flexio_uart_chan, sio_chan);

#define __IMX10xx_CONSOLE_BAUD_RATE        115200

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/** \brief 驱动入口点函数 */
aw_local void __flexio_uart_inst_init1 (struct awbl_dev *p_dev);
aw_local void __flexio_uart_inst_init2 (struct awbl_dev *p_dev);
aw_local void __flexio_uart_inst_connect (struct awbl_dev *p_dev);

/** \brief 设备控制函数 */
aw_local aw_err_t __flexio_uart_ioctl (struct aw_sio_chan *p_sio_chan,
                                       int                 cmd,
                                       void               *p_arg);

/** \brief 配置发送中断 */
aw_local aw_err_t __flexio_uart_tx_startup (struct aw_sio_chan *p_chan);

/** \brief 设置中断中调用的接收、发送字符回调函数 */
aw_local aw_err_t __flexio_uart_callback_install (
    struct aw_sio_chan *p_sio_chan,
    int                 callback_type,
    aw_err_t          (*pfn_callback)(void *),
    void               *p_callback_arg);

/** \brief 串口轮询接收 */
aw_local aw_err_t __flexio_uart_poll_input (struct aw_sio_chan *p_sio_chan,
                                            char               *p_inchar);

/** \brief 串口轮询发送 */
aw_local aw_err_t __flexio_uart_poll_output (struct aw_sio_chan *p_sio_chan,
                                             char                outchar);

/** \brief FlexIO UART 中断服务函数 */
aw_local void __flexio_uart_isr (struct awbl_imx10xx_flexio_uart_chan *p_chan);

/** \brief FlexIO UART 波特率设置 */
aw_local aw_err_t __flexio_uart_baudrate_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              baudtare);

/** \brief FlexIO UART 模式设置 */
aw_local aw_err_t __flexio_uart_mode_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   new_mode);

/** \brief FlexIO UART 选项设置 */
aw_local aw_err_t __flexio_uart_opt_set (
    struct awbl_imx10xx_flexio_uart_chan  *p_chan,
    int                                    options);

/** \brief METHOD: 获取指向 p_chan 结构体的指针 */
void imx10xx_flexio_uart_sio_chan_get (awbl_dev_t *p_dev, void *p_arg);

/** \brief METHOD: 连接指定通道到 SIO 通道 */
void imx10xx_flexio_uart_sio_chan_connect (awbl_dev_t *p_dev, void *p_arg);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 驱动入口点 */
aw_const struct awbl_drvfuncs g_flexio_uart_awbl_drvfuncs = {
    __flexio_uart_inst_init1,       /**< \brief 第一阶段初始化 */
    __flexio_uart_inst_init2,       /**< \brief 第二阶段初始化 */
    __flexio_uart_inst_connect      /**< \brief 第三阶段初始化 */
};

/** \brief 设备方法 */
aw_local aw_const struct awbl_dev_method g_flexio_uart_plb_awbl_methods[] = {
    AWBL_METHOD(aw_sio_chan_get,        imx10xx_flexio_uart_sio_chan_get),
    AWBL_METHOD(aw_sio_chan_connect,    imx10xx_flexio_uart_sio_chan_connect),
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_flexio_uart_drv_registration = {
    AWBL_VER_1,                         /**< \brief AWBus 版本号 */
    AWBL_BUSID_PLB,                     /**< \brief 总线ID */
    AWBL_IMX10XX_FLEXIO_UART_DRV_NAME,  /**< \brief 驱动名 */
   &g_flexio_uart_awbl_drvfuncs,        /**< \brief 驱动入口点 */
    g_flexio_uart_plb_awbl_methods,     /**< \brief 驱动提供的方法 */
    NULL                                /**< \brief 驱动探测函数 */
};

/** \brief SIO 驱动函数 */
aw_local aw_const struct aw_sio_drv_funcs __g_imx10xx_flexio_uart_sio_drv_funcs = {
    __flexio_uart_ioctl,
    __flexio_uart_tx_startup,
    __flexio_uart_callback_install,
    __flexio_uart_poll_input,
    __flexio_uart_poll_output
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief FlexIO UART 空闲回调函数 */
aw_local aw_err_t __flexio_uart_dummy_callback (void)
{
    return AW_ERROR;
}

/** \brief FlexIO UART 失能 */
aw_local void __flexio_uart_disable (
    struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo)
{
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2, 0);
    while (0 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2));

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2, 0);
    while (0 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2));
}

/** \brief FlexIO UART 使能 */
aw_local void __flexio_uart_enable (
    struct awbl_imx10xx_flexio_uart_devinfo *p_devinfo)
{
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2, 1);
    while (1 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0, 2));

    AW_REG_BITS_SET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2, 1);
    while (1 != AW_REG_BITS_GET32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0, 2));
}

/** \brief FlexIO UART 发送中断使能 */
aw_local void __flexio_uart_tx_interrupt_enable (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   enable)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    p_chan->is_tx_int_en = enable;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (enable) {

        /* 使能发送中断 */
        AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }
    else {

        /* 失能发送中断 */
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/**
 * \brief FlexIO UART 硬件去初始化
 *
 * \param[in] p_chan 指向设备通道信息结构的指针
 */
aw_local void __flexio_uart_hardware_deinit (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    aw_spinlock_isr_take(&p_chan->dev_lock);

    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[0]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[0]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[1]], 0);
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], 0);
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], 0);
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], 0);

    /* 清除 Shifter 标志位(写 1 清0) */
    AW_REG_BIT_SET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[0]);
    AW_REG_BIT_SET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1]);

    /* 清除 Timer 标志位(写 1 清0) */
    AW_REG_BIT_SET32(&p_reg->TIMSTAT, p_devinfo->timer_index[0]);
    AW_REG_BIT_SET32(&p_reg->TIMSTAT, p_devinfo->timer_index[1]);

    /* 失能 Shifter DMA */
    AW_REG_BIT_CLR32(&p_reg->SHIFTSDEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTSDEN, p_devinfo->shifter_index[1]);

    /* 失能 Timer 中断 */
    AW_REG_BIT_CLR32(&p_reg->TIMIEN, p_devinfo->timer_index[0]);
    AW_REG_BIT_CLR32(&p_reg->TIMIEN, p_devinfo->timer_index[1]);

    /* 失能 Shifter 错误中断 */
    AW_REG_BIT_CLR32(&p_reg->SHIFTEIEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTEIEN, p_devinfo->shifter_index[1]);

    /* 失能 Shifter 状态中断 */
    AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);

    /* 清除 Shifter 错误标志位(写 1 清0) */
    AW_REG_BIT_SET32(&p_reg->SHIFTERR, p_devinfo->shifter_index[0]);
    AW_REG_BIT_SET32(&p_reg->SHIFTERR, p_devinfo->shifter_index[1]);

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/**
 * \brief FlexIO UART 硬件初始化
 *
 * \param[in] p_chan 指向设备通道信息结构的指针
 */
aw_local void __flexio_uart_hardware_init (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    /* 配置 CTRL 寄存器 */
    reg_temp = AW_REG_READ32(&p_reg->CTRL);
    AW_BIT_SET(reg_temp, 0);
    AW_BIT_MODIFY(reg_temp, 2, p_devinfo->is_fast_access_en);
    AW_BIT_MODIFY(reg_temp, 30, p_devinfo->is_debug_en);
    AW_BIT_MODIFY(reg_temp, 31, !p_devinfo->is_doze_en);
    AW_REG_WRITE32(&p_reg->CTRL, reg_temp);

    /* 复位相关 Shifter 和 Timer */
    __flexio_uart_hardware_deinit(p_chan);

    /* 配置 UART 发送器 */
    reg_temp = (0x02 << 0) |   /* 起始位为低电平 */
               (0x03 << 4);    /* 停止位为高电平 */
    AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[0]], reg_temp);

    reg_temp = (0x02 << 0)                       |   /* 发送模式 */
               (p_devinfo->tx_pin_index << 8)    |   /* 引脚选择 */
               (0x03 << 16)                      |   /* 引脚配置为输出 */
               (p_devinfo->timer_index[0] << 24);    /* 选择定时器 */
    AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[0]], reg_temp);

    reg_temp = (0x01 << 1) |  /* 定时器启动时自动插入起始位 */
               (0x02 << 4) |  /* 定时器停止时自动插入停止位 */
               (0x02 << 8) |  /* 触发电平为高时定时器启动 */
               (0x02 << 12);  /* 定时器匹配时自动停止 */
    AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[0]], reg_temp);

    reg_temp = (0x01 << 0)                                         | /* 波特率发生模式 */
               (p_devinfo->tx_pin_index << 8)                      | /* 引脚选择 */
               (0x01 << 22)                                        | /* 选择内部触发源 */
               (0x01 << 23)                                        | /* 低电平触发 */
               (((p_devinfo->shifter_index[0] << 2) | 0x01) << 24);  /* 触发源选择 */
    AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[0]], reg_temp);

    /* 如果使能接收 */
    if (p_chan->options & CREAD) {

        /* 配置 UART 接收器 */
        reg_temp = (0x02 << 0) |  /* 起始位为低电平 */
                   (0x03 << 4);   /* 停止位为高电平 */
        AW_REG_WRITE32(&p_reg->SHIFTCFG[p_devinfo->shifter_index[1]], reg_temp);

        reg_temp = (0x01 << 0)                       |  /* 接收模式 */
                   (p_devinfo->rx_pin_index << 8)    |  /* 引脚选择 */
                   (0x01 << 23)                      |  /* 定时器下降沿移位 */
                   (p_devinfo->timer_index[1] << 24);   /* 选择定时器 */
        AW_REG_WRITE32(&p_reg->SHIFTCTL[p_devinfo->shifter_index[1]], reg_temp);

        reg_temp = (0x01 << 1)  |  /* 定时器启动时自动插入起始位 */
                   (0x02 << 4)  |  /* 定时器停止时自动插入停止位 */
                   (0x04 << 8)  |  /* 引脚上升沿时定时器启动 */
                   (0x02 << 12) |  /* 定时器匹配时自动停止 */
                   (0x04 << 16) |  /* 引脚上升沿时定时器复位(低八位) */
                   (0x02 << 24);   /* 定时器复位时输出逻辑高电平 */
        AW_REG_WRITE32(&p_reg->TIMCFG[p_devinfo->timer_index[1]], reg_temp);

        reg_temp = (0x01 << 0)                            |  /* 波特率发生模式 */
                   (0x01 << 7)                            |  /* 引脚低电平有效 */
                   (p_devinfo->rx_pin_index << 8)         |  /* 引脚选择 */
                   ((p_devinfo->rx_pin_index << 1) << 24);   /* 触发源选择 */
        AW_REG_WRITE32(&p_reg->TIMCTL[p_devinfo->timer_index[1]], reg_temp);
    }

    /* 配置 UART 波特率 */
    __flexio_uart_baudrate_set(p_chan, p_chan->baudrate);

    /* UART 选项设置 */
    __flexio_uart_opt_set(p_chan, p_chan->options);
}

/**
 * \brief FlexIO UART 波特率设置
 *
 * \param[in] p_chan   指向设备通道信息结构的指针
 * \param[in] baudtare 波特率
 *
 * \retval  AW_OK     完成
 * \retval -AW_EINVAL 不支持的波特率
 */
aw_local aw_err_t __flexio_uart_baudrate_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              baudtare)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;
    uint32_t               f_clk;
    uint16_t               timer_div;
    aw_err_t               result = AW_OK;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    do {
        f_clk = aw_clk_rate_get(p_devinfo->async_parent_clk_id);

        timer_div = f_clk / baudtare;
        timer_div = timer_div / 2 - 1;

        if (timer_div > 0xFFu) {
            result = -AW_EINVAL;
            break;
        }

        reg_temp = AW_REG_READ32(&p_reg->TIMCMP[p_devinfo->timer_index[0]]) & 0xFF00;
        reg_temp |= timer_div & 0x00FF;

        aw_spinlock_isr_take(&p_chan->dev_lock);
        __flexio_uart_disable(p_devinfo);

        AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], reg_temp);

        /* 如果使能接收 */
        if (p_chan->options & CREAD) {
            AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], reg_temp);
        }

        __flexio_uart_enable(p_devinfo);
        aw_spinlock_isr_give(&p_chan->dev_lock);

        p_chan->baudrate = baudtare;
    } while(0);

    return result;
}

/**
 * \brief FlexIO UART 数据位宽设置
 *
 * \param[in] p_chan   指向设备通道信息结构的指针
 * \param[in] baudtare 数据位宽
 *
 * \retval  AW_OK     完成
 * \retval -AW_EINVAL 不支持的数据位宽
 */
aw_local aw_err_t __flexio_uart_data_bits_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    uint32_t                              data_bits)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    uint32_t               reg_temp;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    if ((data_bits < 7) || (data_bits > 9)) {
        return -AW_EINVAL;
    }

    reg_temp = AW_REG_READ32(&p_reg->TIMCMP[p_devinfo->timer_index[0]]) & 0xFF;
    reg_temp |= ((uint32_t)(data_bits * 2 - 1)) << 8u;

    __flexio_uart_disable(p_devinfo);

    AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[0]], reg_temp);

    /* 如果使能接收 */
    if (p_chan->options & CREAD) {
        AW_REG_WRITE32(&p_reg->TIMCMP[p_devinfo->timer_index[1]], reg_temp);
    }

    __flexio_uart_enable(p_devinfo);

    return AW_OK;
}

/**
 * \brief FlexIO UART 模式设置
 *
 * 配置为中断模式或者轮询模式
 *
 * \param[in] p_chan   指向设备通道信息结构的指针
 * \param[in] new_mode SIO_MODE_INT 或者 SIO_MODE_POLL
 *
 * \retval  AW_OK     完成
 * \retval -AW_EINVAL 无效参数
 */
aw_local aw_err_t __flexio_uart_mode_set (
    struct awbl_imx10xx_flexio_uart_chan *p_chan,
    int                                   new_mode)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    if ((new_mode != SIO_MODE_POLL) && new_mode != SIO_MODE_INT) {
        return -AW_EINVAL;
    }

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (new_mode == SIO_MODE_INT) {

        if (p_chan->options & CREAD) {
            AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        } else {
            AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        }

        if (p_chan->is_tx_int_en) {
            AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
        } else {
            AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
        }

    } else {
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]);
    }

    p_chan->channel_mode = new_mode;

    aw_spinlock_isr_give(&p_chan->dev_lock);

    return AW_OK;
}

/**
 * \brief FlexIO UART 选项设置
 *
 * 配置通道选项，分别为 CLOCAL, HUPCL, CREAD, CSIZE, PARENB 和 PARODD。
 * 当 HUPCL 选项使能时，一个连接将在最后一次调用 close() 时关闭，但是每次调用 open()
 * 都会打开。
 *
 * \param[in] p_chan  指向设备通道信息结构的指针
 * \param[in] options 选项
 *
 * \retval  AW_OK      完成
 * \retval -AW_ENOTSUP 不支持
 */
aw_local aw_err_t __flexio_uart_opt_set (
    struct awbl_imx10xx_flexio_uart_chan  *p_chan,
    int                                    options)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               state = AW_OK;

    aw_assert(p_chan != NULL);
    aw_assert(p_dev != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    aw_spinlock_isr_take(&p_chan->dev_lock);

    switch (options & CSIZE) {

    case CS5:
    case CS6:
        state = -AW_ENOTSUP;
        break;

    case CS7:
        __flexio_uart_data_bits_set(p_chan, 7);
        break;

    case CS8:
        __flexio_uart_data_bits_set(p_chan, 8);
        break;

    default:
        break;
    }

    /* 如果使能接收且为中断模式 */
    if ((options & CREAD) && (SIO_MODE_INT == p_chan->channel_mode)) {

        /* 使能接收中断 */
        AW_REG_BIT_SET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
    } else {

        /* 失能接收中断 */
        AW_REG_BIT_CLR32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]);
    }

    /* 不支持奇偶校验 */
    if (options & (PARENB | PARODD)) {
        state = -AW_ENOTSUP;
    }

    /* 不支持 2 位停止信号 */
    if (options & STOPB) {
        state = -AW_ENOTSUP;
    }

    /* 不支持硬件流控 */
    if (!(options & CLOCAL)) {
        state = -AW_ENOTSUP;
    }

    p_chan->options = options;

    aw_spinlock_isr_give(&p_chan->dev_lock);

    return state;
}

/** \brief 挂起 UART, 设置 RTS 为无效并清除发送、接受缓冲区 */
aw_local aw_err_t __flexio_uart_hup (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    return -AW_ENOTSUP;
}

/** \brief 设置 RTS 为有效并清除发送、接受缓冲区  */
aw_local aw_err_t __flexio_uart_open (
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    return -AW_ENOTSUP;
}

/**
 * \brief 连接指定通道到中断
 *
 * \param[in] p_dev  指向设备实例指针
 * \param[in] p_chan 指向设备通道信息结构的指针
 */
aw_local void __flexio_uart_chan_int_connect (
    awbl_dev_t                           *p_dev,
    struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);

    aw_assert(p_dev != NULL);
    aw_assert(p_chan != NULL);
    aw_assert(p_devinfo != NULL);

    /* 连接与使能中断 */
    aw_int_connect(p_devinfo->inum, (void ( *)(void *))__flexio_uart_isr, p_chan);
    aw_int_enable(p_devinfo->inum);
}

/** \brief 读取串口接收到的数据并调用回调函数 */
aw_local void __flexio_uart_int_rd (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    char                   inchar;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1])) {
        inchar = (char)readl(&p_reg->SHIFTBUFBYS[p_devinfo->shifter_index[1]]);
        (*p_chan->pfn_rxchar_put)(p_chan->rxput_arg, inchar);
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/** \brief 获取需要发送的数据并通过串口发送 */
aw_local int __flexio_uart_int_wr (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    char                   outchar;
    int                    count;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (p_chan->pfn_txchar_get(p_chan->txget_arg, &outchar) == AW_OK) {
        writel(outchar, &p_reg->SHIFTBUF[p_devinfo->shifter_index[0]]);
        count = 1;
    } else {
        count = 0;
    }
    aw_spinlock_isr_give(&p_chan->dev_lock);

    return count;
}

/** \brief FlexIO UART 中断服务函数 */
aw_local void __flexio_uart_isr (struct awbl_imx10xx_flexio_uart_chan *p_chan)
{
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    register uint32_t      sr;
    int                    send_count = 0;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    sr = readl(&p_reg->SHIFTSTAT);

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[1]) &&
        (sr & (1 << p_devinfo->shifter_index[1]))) {
        __flexio_uart_int_rd(p_chan);
    }

    if (AW_REG_BIT_ISSET32(&p_reg->SHIFTSIEN, p_devinfo->shifter_index[0]) &&
        (sr & (1 << p_devinfo->shifter_index[0]))) {
        send_count = __flexio_uart_int_wr(p_chan);
        if (0 == send_count) {
            __flexio_uart_tx_interrupt_enable(p_chan, 0);
        }
    }

    aw_spinlock_isr_give(&p_chan->dev_lock);
}

/*******************************************************************************
  SIO 驱动函数
*******************************************************************************/

/**
 * \brief 设备控制函数
 *
 * \param[in] p_sio_chan 指向 aw_sio_chan 结构的指针
 * \param[in] cmd        包含波特率获取/设置、模式设置、硬件设置以及流控设置
 * \param[in] p_arg      根据不同的命令传入不同的参数
 *
 * \note 当进行最后一次 close() 函数调用时，SIO_HUP 命令由 ttydrv 发送，
 *       同理，第一次调用 open() 函数时，SIO_OPEN 命令被 ttydrv 发送。
 */
aw_local aw_err_t __flexio_uart_ioctl (struct aw_sio_chan *p_sio_chan,
                                       int                 cmd,
                                       void               *p_arg)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    aw_err_t status = AW_OK;

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    switch (cmd) {

    case SIO_BAUD_SET:
        status = __flexio_uart_baudrate_set(p_chan, (uint32_t)p_arg);
        break;

    case SIO_BAUD_GET:
        *(int *)p_arg = p_chan->baudrate;
        break;

    case SIO_MODE_SET:
        status = (__flexio_uart_mode_set(p_chan, (int)p_arg) == AW_OK)
                 ? AW_OK : -AW_EIO;
        break;

    case SIO_MODE_GET:
        *(int *)p_arg = p_chan->channel_mode;
        break;

    case SIO_AVAIL_MODES_GET:
        *(int *)p_arg = SIO_MODE_INT | SIO_MODE_POLL;
        break;

    case SIO_HW_OPTS_SET:
        status = (__flexio_uart_opt_set(p_chan, (int)p_arg) == AW_OK) ? AW_OK : AW_EIO;
        break;

    case SIO_HW_OPTS_GET:
        *(int *)p_arg = p_chan->options;
        break;

    case SIO_HUP:
        if (p_chan->options & HUPCL) {
            status = __flexio_uart_hup(p_chan);
        }
        break;

    case SIO_OPEN:
        if (p_chan->options & HUPCL) {
            status = __flexio_uart_open(p_chan);
        }
        break;

    default:
        status = -AW_ENOSYS;
        break;
    }

    return status;
}

/**
 * \brief 配置发送中断
 *
 * \details 调用中断级别的字符发送程序，如果处于中断模式且无硬件流控，则启用中断
 *          如果启用硬件流控，并且 CTS 设置为 TRUE，则启用 TX 中断。
 */
aw_local aw_err_t __flexio_uart_tx_startup (struct aw_sio_chan *p_sio_chan)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    if (p_chan->channel_mode == SIO_MODE_INT) {
         if (__flexio_uart_int_wr(p_chan) > 0 ) {
             __flexio_uart_tx_interrupt_enable(p_chan, 1);
         }
         return AW_OK;
     }

     return -AW_ENOSYS;
}

/**
 * \brief 设置中断中调用的接收、发送字符回调函数
 *
 * \param p_sio_chan     指向 aw_sio_chan 结构的指针
 * \param callback_type  回调函数类型(发送或者接收)
 * \param pfn_callback   指向回调函数的指针
 * \param p_callback_arg 传入回调函数的参数
 *
 * \retval  AW_OK  完成
 * \retval -AW_ENOSYS 不支持的回调函数类型
 */
aw_local aw_err_t __flexio_uart_callback_install (
    struct aw_sio_chan *p_sio_chan,
    int                 callback_type,
    aw_err_t          (*pfn_callback)(void *),
    void               *p_callback_arg)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);

    aw_assert(p_sio_chan != NULL);
    aw_assert(p_chan != NULL);

    switch (callback_type) {

    case AW_SIO_CALLBACK_GET_TX_CHAR:
        p_chan->pfn_txchar_get = (pfunc_sio_txchar_get_t)pfn_callback;
        p_chan->txget_arg      = p_callback_arg;
        return AW_OK;

    case AW_SIO_CALLBACK_PUT_RCV_CHAR:
        p_chan->pfn_rxchar_put = (pfunc_sio_rxchar_put_t)pfn_callback;
        p_chan->rxput_arg      = p_callback_arg;
        return AW_OK;

    default:
        return -AW_ENOSYS;
    }
}

/** \brief 串口轮询接收 */
aw_local aw_err_t __flexio_uart_poll_input (struct aw_sio_chan *p_sio_chan,
                                            char               *p_inchar)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               err = AW_OK;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (!AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[1])) {
        err = -AW_EAGAIN;
        goto cleanup;
    }

    *p_inchar = (char)readl(&p_reg->SHIFTBUFBYS[p_devinfo->shifter_index[1]]);

cleanup:
    aw_spinlock_isr_give(&p_chan->dev_lock);
    return err;
}

/** \brief 串口轮询发送 */
aw_local aw_err_t __flexio_uart_poll_output (struct aw_sio_chan *p_sio_chan,
                                             char                outchar)
{
    __SIO_CHAN_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_sio_chan);
    __FLEXIO_UART_CHAN_GET_DEV_DECL(p_dev, p_chan);
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    imx10xx_flexio_regs_t *p_reg = (imx10xx_flexio_regs_t *)p_devinfo->regbase;
    aw_err_t               err = AW_OK;

    aw_spinlock_isr_take(&p_chan->dev_lock);

    if (!AW_REG_BIT_ISSET32(&p_reg->SHIFTSTAT, p_devinfo->shifter_index[0])) {
        err =  -AW_EAGAIN;
        goto cleanup;
    }

    writel(outchar, &p_reg->SHIFTBUF[p_devinfo->shifter_index[0]]);

cleanup:
    aw_spinlock_isr_give(&p_chan->dev_lock);
    return err;
}

/*******************************************************************************
  METHOD
*******************************************************************************/

/**
 * \brief METHOD: 获取指向 p_chan 结构体的指针
 *
 * \param[in] p_dev 指向设备实例结构的指针
 * \param[in] p_arg 参数
 */
void imx10xx_flexio_uart_sio_chan_get (awbl_dev_t *p_dev, void *p_arg)
{
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);
    struct aw_sio_chan_info *p_info = (struct aw_sio_chan_info *)p_arg;

    while (p_chan != NULL ) {

        /* 在这个驱动中， channel_no 就是 dev unit */
        if (p_chan->channel_no == p_info->chan_no) {
            p_info->p_chan = p_chan;
            return;
        }
        p_chan = p_chan->p_next;
    }
}

/** \brief METHOD: 连接指定通道到 SIO 通道 */
void imx10xx_flexio_uart_sio_chan_connect (awbl_dev_t *p_dev, void *p_arg)
{
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);
    struct aw_sio_chan_info *p_info = (struct aw_sio_chan_info *)p_arg;

    if (p_info->chan_no == -1) {
        /* 连接所有通道 */

        while (NULL != p_chan) {

            /* 连接并使能中断 */
            __flexio_uart_chan_int_connect(p_dev, p_chan);
            p_chan = p_chan->p_next;
        }

    } else {
        /* 连接指定通道 */

        if (p_info->p_chan != NULL) {
            return;
        }

        /* 查找通道 */
        while (p_chan && (p_chan->channel_no != p_info->chan_no)) {
            p_chan = p_chan->p_next;
        }

        if (p_chan == NULL) {
            return;
        }

        if ((p_info->chan_no != -1)
            && (p_chan->channel_no != p_info->chan_no)) {
            return;
        }

        /* 连接并使能中断 */
        __flexio_uart_chan_int_connect(p_dev, p_chan);

        p_info->p_chan = p_chan;
    }
}

/** \brief 第一阶段初始化 */
aw_local void __flexio_uart_inst_init1 (struct awbl_dev *p_dev)
{
    __DEV_GET_FLEXIO_UART_DEVINFO_DECL(p_devinfo, p_dev);
    __DEV_GET_FLEXIO_UART_CHAN_DECL(p_chan, p_dev);

    /* 调用平台初始化函数 */
    if (NULL != p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    /* 初始化自旋锁 */
    aw_spinlock_isr_init(&p_chan->dev_lock, 0);

    /* 初始化设备通道结构 */
    p_chan->channel_no = p_dev->p_devhcf->unit;

    p_chan->sio_chan.p_drv_funcs = (struct aw_sio_drv_funcs *)&__g_imx10xx_flexio_uart_sio_drv_funcs;
    p_chan->channel_mode = SIO_MODE_INT; /* 中断模式 */
    p_chan->options      = (CLOCAL |     /* 本地设备(没有调制解调器) */
                            CREAD  |     /* 使能接收 */
                            CS8);        /* 数据长度为 8 位 */

    p_chan->is_tx_int_en = 0;
    p_chan->baudrate = __IMX10xx_CONSOLE_BAUD_RATE;

    p_chan->pfn_txchar_get = (int ( *)(void *, char *))__flexio_uart_dummy_callback;
    p_chan->pfn_rxchar_put = (int ( *)(void *, char ))__flexio_uart_dummy_callback;

    p_chan->p_next = NULL;

    /* 将FlexIO配置为UART */
    __flexio_uart_hardware_init(p_chan);

}

/** \brief 第二阶段初始化 */
aw_local void __flexio_uart_inst_init2 (struct awbl_dev *p_dev)
{

}

/** \brief 第三阶段初始化 */
aw_local void __flexio_uart_inst_connect (struct awbl_dev *p_dev)
{

}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 IMX10xx FlexIO UART 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_flexio_uart_drv_register (void)
{
    awbl_drv_register(&__g_flexio_uart_drv_registration);
}

/* end of file */
