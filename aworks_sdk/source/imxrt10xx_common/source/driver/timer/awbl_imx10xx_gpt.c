/*******************************************************************************
*                                  AWorks
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
 * \brief iMX RT10xx GPT 驱动
 *
 * 本驱动实现了定时器的周期中断功能，以及匹配和捕获功能。
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-09  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/
#include "aworks.h"
#include "aw_bitops.h"
#include "aw_assert.h"
#include "aw_gpio.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_gpt.h"

/*******************************************************************************
  宏定义
*******************************************************************************/
#define __CTRL_EN           0
#define __CTRL_ENMOD        1
#define __CTRL_DEBUGEN      2
#define __CTRL_WAITEN       3
#define __CTRL_DOZEEN       4
#define __CTRL_STOPEN       5
#define __CTRL_CLKSRC       6
#define __CTRL_CLKSRC_LEN   3
#define __CTRL_CLKSRC_NCLK  0
#define __CTRL_CLKSRC_PCLK  1
#define __CTRL_CLKSRC_HCLK  2
#define __CTRL_CLKSRC_ECLK  3
#define __CTRL_CLKSRC_LCLK  4
#define __CTRL_FRR          9
#define __CTRL_EN_24M       10
#define __CTRL_SWR          15
#define __CTRL_IM1          16
#define __CTRL_IM2          18
#define __CTRL_IM_LEN       2
#define __CTRL_IM_DIS       0
#define __CTRL_IM_RISE      1
#define __CTRL_IM_FALL      2
#define __CTRL_IM_BOTH      3
#define __CTRL_OM1          20
#define __CTRL_OM2          23
#define __CTRL_OM3          26
#define __CTRL_OM_LEN       3
#define __CTRL_OM_DIS       0
#define __CTRL_OM_TOG       1
#define __CTRL_OM_CLR       2
#define __CTRL_OM_SET       3
#define __CTRL_OM_LOW_PULSE 4
#define __CTRL_FO1          29
#define __CTRL_FO2          30
#define __CTRL_FO3          31

#define __PRES_24M          12

#define __STAT_OF1          0
#define __STAT_OF2          1
#define __STAT_OF3          2
#define __STAT_IF1          3
#define __STAT_IF2          4
#define __STAT_ROV          5

#define __INT_OF1           0
#define __INT_OF2           1
#define __INT_OF3           2
#define __INT_IF1           3
#define __INT_IF2           4
#define __INT_ROV           5

/** \brief macro to convert a p_drv to gpt pointer */
#define __pdrv_to_imx10xx_gpt(p_drv)    ((struct awbl_imx10xx_gpt_dev *)p_drv)

/** \brief convert AWBus timer device info to imx10xx timer parameter */
#define awbldev_get_imx10xx_gpt_param(p_dev) \
    ((struct awbl_imx10xx_gpt_devinfo *)AWBL_DEVINFO_GET(p_dev))

/** \brief Convert a awbl_timer pointer to a awbl_imx10xx_gpt pointer */
#define awbl_timer_to_imx10xx_gpt(p_timer) \
    ((struct awbl_imx10xx_gpt_dev *)(p_timer))

/** \brief convert a awbl_dev pointer to awbl_imx10xx_gpt pointer */
#define awbldev_to_imx10xx_gpt(p_dev) \
    ((struct awbl_imx10xx_gpt_dev *)(p_dev))

/** \brief get IMX10xx gpt parameter */
#define imx10xx_gpt_get_param(p_imxtimer) ((struct awbl_imx10xx_gpt_devinfo *) \
    AWBL_DEVINFO_GET(&((p_imxtimer)->tmr.dev)))

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/** \brief 为 GPT 分配资源 */
aw_local aw_err_t __imx10xx_gpt_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no);

 /** \brief 为 GPT 释放资源 */
 aw_local aw_err_t __imx10xx_gpt_release (void *p_drv);

 /** \brief 获取计数器溢出值 */
 aw_local aw_err_t __imx10xx_gpt_rollover_get (void *p_drv, uint32_t *p_count);

 /** \brief 获取当前计数值 */
 aw_local aw_err_t __imx10xx_gpt_count_get (void *p_drv, uint32_t *p_count);

 /** \brief 失能 GPT */
 aw_local aw_err_t __imx10xx_gpt_disable (void *p_drv);

 /** \brief 使能 GPT */
 aw_local aw_err_t __imx10xx_gpt_enable (void *p_drv, uint32_t max_timer_count);

 /** \brief 设置中断回调函数 */
 aw_local aw_err_t __imx10xx_gpt_isr_set (void  *p_drv,
                                          void (*pfn_isr) (void *),
                                          void  *p_arg);

 /** \brief METHOD: 获取指向标准定时器结构的指针 */
 aw_local aw_err_t __imx10xx_gpt_func_get (struct awbl_dev    *p_dev,
                                           struct awbl_timer **pp_awbl_tmr,
                                           int                 timer_no);

/** \brief 驱动入口点函数 */
aw_local void __imx10xx_gpt_inst_init1 (struct awbl_dev *p_dev);
aw_local void __imx10xx_gpt_inst_init2 (struct awbl_dev *p_dev);
aw_local void __imx10xx_gpt_inst_connect (struct awbl_dev *p_dev);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 适配标准定时器接口的结构体 */
aw_local aw_const struct awbl_timer_functions __g_imx10xx_timer_functions = {
    __imx10xx_gpt_allocate,     /* 为 GPT 分配资源 */
    __imx10xx_gpt_release,      /* 为 GPT 释放资源 */
    __imx10xx_gpt_rollover_get, /* 获取计数器溢出值 */
    __imx10xx_gpt_count_get,    /* 获取当前计数值 */
    __imx10xx_gpt_disable,      /* 失能 GPT */
    __imx10xx_gpt_enable,       /* 使能 GPT */
    __imx10xx_gpt_isr_set,      /* 设置中断回调函数 */
    NULL,                       /* 使能 64 位定时器 */
    NULL,                       /* 获取 64 位计数器溢出值 */
    NULL                        /* 获取 64 位当前计数值 */
};

/** \brief 驱动入口点 */
aw_const struct awbl_drvfuncs __g_imx10xx_gpt_drvfuncs = {
    __imx10xx_gpt_inst_init1,   /**< \brief 第一阶段初始化 */
    __imx10xx_gpt_inst_init2,   /**< \brief 第二阶段初始化 */
    __imx10xx_gpt_inst_connect  /**< \brief 第三阶段初始化 */
};

/** \brief 设备方法 */
aw_local aw_const struct awbl_dev_method __g_imx10xx_gpt_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx10xx_gpt_func_get),
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_gpt_drvinfo = {
    AWBL_VER_1,                       /**< \brief AWBus 版本号 */
    AWBL_BUSID_PLB,                   /**< \brief 总线ID */
    AWBL_IMX10XX_GPT_DRV_NAME,        /**< \brief 驱动名 */
    &__g_imx10xx_gpt_drvfuncs,        /**< \brief 驱动入口点 */
    &__g_imx10xx_gpt_methods[0],      /**< \brief 驱动提供的方法 */
    NULL                              /**< \brief 驱动探测函数 */
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 定时器中断处理函数 */
aw_local void __imx10xx_gpt_isr (struct awbl_imx10xx_gpt_dev *p_tmr)
{
    void (*pfn_isr) (void *);
    uint32_t int_stat;

    int_stat = AW_REG_READ32(&p_tmr->p_reg->sr);

    /* 清除中断标志位 */
    AW_REG_WRITE32(&p_tmr->p_reg->sr, int_stat);

    pfn_isr = p_tmr->pfn_isr;
    if (pfn_isr != NULL) {
        (*pfn_isr)(p_tmr->p_arg);
    }
}

/** \brief 为 GPT 分配资源 */
aw_local aw_err_t __imx10xx_gpt_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no)
{
    struct awbl_imx10xx_gpt_dev *p_tmr;
    int                          ret = AW_ERROR;

    aw_assert((p_dev != NULL) && (pp_drv != NULL));
    if (timer_no != 0) {
        return ret;
    }

    p_tmr = awbldev_to_imx10xx_gpt(p_dev);

    if (AW_FALSE != p_tmr->tmr.allocated) {
        goto cleanup;
    }

    p_tmr->tmr.allocated = AW_TRUE;

    /* 更新驱动信息 */
    *pp_drv = (void *)p_tmr;

    ret = aw_int_enable(awbldev_get_imx10xx_gpt_param(p_dev)->inum);

cleanup:
    return  ret;
}

/** \brief 为 GPT 释放资源 */
aw_local aw_err_t __imx10xx_gpt_release (void *p_drv)
{
    struct awbl_imx10xx_gpt_dev *p_tmr;

    aw_assert(p_drv != NULL);

    p_tmr = __pdrv_to_imx10xx_gpt(p_drv);

    if (AW_FALSE == p_tmr->tmr.allocated) {
        return AW_ERROR;
    }

    __imx10xx_gpt_disable(p_drv);

    p_tmr->pfn_isr = NULL;
    p_tmr->p_arg   = NULL;

    p_tmr->tmr.allocated = AW_FALSE;

    return aw_int_disable(awbldev_get_imx10xx_gpt_param(&(p_tmr->dev))->inum);
}

/** \brief 获取计数器溢出值 */
aw_local aw_err_t __imx10xx_gpt_rollover_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx10xx_gpt_dev *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }
    *p_count = p_tmr->rollover_period;

    return AW_OK;
}

/** \brief 获取当前计数值 */
aw_local aw_err_t __imx10xx_gpt_count_get (void *p_drv, uint32_t *p_count)
{
    struct awbl_imx10xx_gpt_dev *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);

    aw_assert(NULL != p_tmr);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }

    *p_count = AW_REG_READ32(&p_tmr->p_reg->cnt);

    return AW_OK;
}

/** \brief 失能 GPT */
aw_local aw_err_t __imx10xx_gpt_disable (void *p_drv)
{
    awbl_imx10xx_gpt_dev_t           *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);
    const awbl_imx10xx_gpt_devinfo_t *p_param;

    p_param = awbldev_get_imx10xx_gpt_param(p_tmr);

    if (p_tmr->enabled) {

        /* 清零控制寄存器 */
        AW_REG_WRITE32(&p_tmr->p_reg->cr, 0x0);

        /* 禁能GPT时钟 */
        aw_clk_disable(p_param->clk_id);

        p_tmr->enabled = 0;
    }

    return AW_OK;
}

/** \brief 使能 GPT */
aw_local aw_err_t __imx10xx_gpt_enable (void *p_drv, uint32_t max_timer_count)
{
    awbl_imx10xx_gpt_dev_t     *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);
    awbl_imx10xx_gpt_devinfo_t *p_param;

    p_param = awbldev_get_imx10xx_gpt_param(p_tmr);

    if (p_tmr->enabled) {
        return -AW_EBUSY;
    }

    p_tmr->enabled = 1;

    /* 使能GPT时钟 */
    aw_clk_enable(p_param->clk_id);

    p_tmr->count = max_timer_count;

    /* 首先要禁用此定时器 */
    AW_REG_WRITE32(&p_tmr->p_reg->cr, 0x0);

    /* 禁能中断 */
    AW_REG_WRITE32(&p_tmr->p_reg->ir, 0x0);

    /* 设置预分频 */
    AW_REG_WRITE32(&p_tmr->p_reg->pr, 0x0);

    /* 选择ipg_clk为源时钟 */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_CLKSRC, __CTRL_CLKSRC_LEN, __CTRL_CLKSRC_PCLK);

    /* 不输出到引脚 */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_OM1, __CTRL_OM_LEN, __CTRL_OM_DIS);

    /* 停止和等待状态下GPT是否使能 */
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_STOPEN);
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_WAITEN);

    /* 设置重新加载模式 */
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_FRR);

    /* 启用中断 */
    AW_REG_BIT_SET32(&p_tmr->p_reg->ir, __INT_OF1);

    /* 设置计数最大值(即比较值) */
    AW_REG_WRITE32(&p_tmr->p_reg->ocr[0], max_timer_count);

    /* 清除标志(写1清0) */
    AW_REG_BIT_SET32(&p_tmr->p_reg->sr, __STAT_OF1);

    /* 清空计数值 */
    AW_REG_BIT_SET32(&p_tmr->p_reg->cr, __CTRL_ENMOD);

    /* 测试定时器精准度 */
    /* 配置输出模式 */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_OM1, __CTRL_OM_LEN, 0);

    /* 使能 */
    AW_REG_BIT_SET32(&p_tmr->p_reg->cr, __CTRL_EN);

    return AW_OK;
}

/** \brief 设置中断回调函数 */
aw_local aw_err_t __imx10xx_gpt_isr_set (void  *p_drv,
                                         void (*pfn_isr) (void *),
                                         void  *p_arg)
{
    struct awbl_imx10xx_gpt_dev *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);

    aw_assert(p_tmr != NULL);

    p_tmr->pfn_isr = pfn_isr;
    p_tmr->p_arg   = p_arg;

    return AW_OK;
}

/** \brief METHOD: 获取指向标准定时器结构的指针 */
aw_local aw_err_t __imx10xx_gpt_func_get (struct awbl_dev    *p_dev,
                                          struct awbl_timer **pp_awbl_tmr,
                                          int                 timer_no)
{
    aw_assert((p_dev != NULL) && (pp_awbl_tmr != NULL));

    if (timer_no != 0) {
        return AW_ERROR;
    }

    *pp_awbl_tmr = &(awbldev_to_imx10xx_gpt(p_dev)->tmr);

    return AW_OK;
}

/** \brief 第一阶段初始化 */
aw_local void __imx10xx_gpt_inst_init1 (struct awbl_dev *p_dev)
{
    struct awbl_timer                     *p_awbl_tmr;
    struct awbl_imx10xx_gpt_dev           *p_tmr;
    const struct awbl_imx10xx_gpt_devinfo *p_param;

    p_param     = awbldev_get_imx10xx_gpt_param(p_dev);
    p_tmr       = awbldev_to_imx10xx_gpt(p_dev);
    
    p_awbl_tmr  = &(p_tmr->tmr);

    /* TODO: do timer register mapping here */
    /* awbl_reg_map(p_dev, p_param->p_reg, &p_tmr->handle); */
		
    if (p_param->pfn_plfm_init != NULL) {
        p_param->pfn_plfm_init();
    }

    p_tmr->p_reg = (imx10xx_gpt_regs_t *)(p_param->reg_addr);
    p_tmr->count = 0;
    p_tmr->enabled = 0;
    
    /* 使能GPT时钟 */
    aw_clk_enable(p_param->clk_id);

    p_tmr->param.clk_frequency = aw_clk_rate_get(p_param->clk_id);

    /* 禁能GPT时钟 */
    aw_clk_disable(p_param->clk_id);

    p_tmr->param.max_frequency = p_tmr->param.clk_frequency / 2;
    p_tmr->param.min_frequency = 1;
    
    p_tmr->param.features = AW_HWTIMER_CAN_INTERRUPT      |
                            AW_HWTIMER_INTERMEDIATE_COUNT |
                            AW_HWTIMER_AUTO_RELOAD        |
                            AW_HWTIMER_SIZE(32);

    p_tmr->rollover_period =  0xFFFFFFFF / (p_tmr->param.clk_frequency);
    
    p_awbl_tmr->p_param = &p_tmr->param;
    p_awbl_tmr->p_func = &__g_imx10xx_timer_functions;
    p_awbl_tmr->allocated = AW_FALSE;
    p_awbl_tmr->ticks_per_second = 60u;
    
    /* 定时器通道属性配置，单通道只需要判断bit0为是否置为即可 */
    if (AW_BIT_ISSET(p_param->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }
}

/** \brief 第二阶段初始化 */
aw_local void __imx10xx_gpt_inst_init2 (struct awbl_dev *p_dev)
{
    const struct awbl_imx10xx_gpt_devinfo *p_param;       /* timer parameter */

    p_param = awbldev_get_imx10xx_gpt_param(p_dev);

    /* 连接中断 */
    while (AW_OK != aw_int_connect(p_param->inum,
                                   (aw_pfuncvoid_t)__imx10xx_gpt_isr,
                                   (void *)awbldev_to_imx10xx_gpt(p_dev))) {
    }
}

/** \brief 第三阶段初始化 */
aw_local void __imx10xx_gpt_inst_connect (struct awbl_dev *p_dev)
{
    return;
}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 IMX10xx GPT 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_gpt_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_gpt_drvinfo);
}

/* end of file */
