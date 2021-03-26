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
 * \brief iMX RT10xx PIT 驱动
 *
 * 本驱动实现了定时器的周期中断功能。
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-13  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/
#include "aworks.h"
#include "aw_bitops.h"
#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_pit.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

#define __PIT_DEV_DECL(p_this, p_dev)           \
          struct awbl_imx10xx_pit_dev *p_this = \
          (struct awbl_imx10xx_pit_dev *)p_dev

#define __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_dev)   \
          struct awbl_imx10xx_pit_devinfo *p_devinfo = \
          (struct awbl_imx10xx_pit_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo) \
          imx10xx_pit_regs_t *p_reg = (imx10xx_pit_regs_t *)p_devinfo->regbase

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/** \brief 为 PIT 分配资源 */
aw_local aw_err_t __imx10xx_pit_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no);

 /** \brief 为 PIT 释放资源 */
 aw_local aw_err_t __imx10xx_pit_release (void *p_drv);

 /** \brief 获取计数器溢出值 */
 aw_local aw_err_t __imx10xx_pit_rollover_get (void *p_drv, uint32_t *p_count);

 /** \brief 获取当前计数值 */
 aw_local aw_err_t __imx10xx_pit_count_get (void *p_drv, uint32_t *p_count);

 /** \brief 失能 PIT */
 aw_local aw_err_t __imx10xx_pit_disable (void *p_drv);

 /** \brief 使能 PIT */
 aw_local aw_err_t __imx10xx_pit_enable (void *p_drv, uint32_t max_timer_count);

 /** \brief 设置中断回调函数 */
 aw_local aw_err_t __imx10xx_pit_isr_set (void  *p_drv,
                                          void (*pfn_isr) (void *),
                                          void  *p_arg);

 /** \brief METHOD: 获取指向标准定时器结构的指针 */
 aw_local aw_err_t __imx10xx_pit_func_get (struct awbl_dev    *p_dev,
                                           struct awbl_timer **pp_awbl_tmr,
                                           int                 timer_no);

/** \brief 驱动入口点函数 */
aw_local void __imx10xx_pit_inst_init1 (struct awbl_dev *p_dev);
aw_local void __imx10xx_pit_inst_init2 (struct awbl_dev *p_dev);
aw_local void __imx10xx_pit_inst_connect (struct awbl_dev *p_dev);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 适配标准定时器接口的结构体 */
aw_local aw_const struct awbl_timer_functions __g_imx10xx_timer_functions = {
    __imx10xx_pit_allocate,     /* 为 PIT 分配资源 */
    __imx10xx_pit_release,      /* 为 PIT 释放资源 */
    __imx10xx_pit_rollover_get, /* 获取计数器溢出值 */
    __imx10xx_pit_count_get,    /* 获取当前计数值 */
    __imx10xx_pit_disable,      /* 失能 PIT */
    __imx10xx_pit_enable,       /* 使能 PIT */
    __imx10xx_pit_isr_set,      /* 设置中断回调函数 */
    NULL,                       /* 使能 64 位定时器 */
    NULL,                       /* 获取 64 位计数器溢出值 */
    NULL                        /* 获取 64 位当前计数值 */
};

/** \brief 驱动入口点 */
aw_const struct awbl_drvfuncs __g_imx10xx_pit_drvfuncs = {
    __imx10xx_pit_inst_init1,   /**< \brief 第一阶段初始化 */
    __imx10xx_pit_inst_init2,   /**< \brief 第二阶段初始化 */
    __imx10xx_pit_inst_connect  /**< \brief 第三阶段初始化 */
};

/** \brief 设备方法 */
aw_local aw_const struct awbl_dev_method __g_imx10xx_pit_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx10xx_pit_func_get),
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_pit_drvinfo = {
    AWBL_VER_1,                       /**< \brief AWBus 版本号 */
    AWBL_BUSID_PLB,                   /**< \brief 总线ID */
    AWBL_IMX10XX_PIT_DRV_NAME,        /**< \brief 驱动名 */
    &__g_imx10xx_pit_drvfuncs,        /**< \brief 驱动入口点 */
    &__g_imx10xx_pit_methods[0],      /**< \brief 驱动提供的方法 */
    NULL                              /**< \brief 驱动探测函数 */
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 定时器中断处理函数 */
aw_local void __imx10xx_pit_isr (struct awbl_imx10xx_pit_dev *p_this)
{
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);
    void   (*pfn_isr)(void *);
    uint32_t int_stat;

    aw_assert(p_this != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    /* 查看中断状态(调试用) */
    int_stat = AW_REG_READ32(&p_reg->channel[0].tflg);

    /* 清除中断标志位 */
    AW_REG_WRITE32(&p_reg->channel[0].tflg, __PIT_TFLG_TIF_MASK);

    pfn_isr = p_this->pfn_isr;
    if (pfn_isr != NULL) {
        (*pfn_isr)(p_this->p_arg);
    }
}

/** \brief 为 PIT 分配资源 */
aw_local aw_err_t __imx10xx_pit_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no)
{
    __PIT_DEV_DECL(p_this, p_dev);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_dev);
    int ret = AW_ERROR;

    aw_assert(p_this != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(pp_drv != NULL);

    if (timer_no != 0) {
        return ret;
    }

    if (AW_FALSE != p_this->tmr.allocated) {
        goto cleanup;
    }

    p_this->tmr.allocated = AW_TRUE;

    /* 更新驱动信息 */
    *pp_drv = (void *)p_this;

    ret = aw_int_enable(p_devinfo->inum);

cleanup:
    return  ret;
}

/** \brief 为 PIT 释放资源 */
aw_local aw_err_t __imx10xx_pit_release (void *p_drv)
{
    __PIT_DEV_DECL(p_this, p_drv);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);

    aw_assert(p_this != NULL);
    aw_assert(p_devinfo != NULL);

    if (AW_FALSE == p_this->tmr.allocated) {
        return AW_ERROR;
    }

    __imx10xx_pit_disable(p_drv);

    p_this->pfn_isr = NULL;
    p_this->p_arg   = NULL;

    p_this->tmr.allocated = AW_FALSE;

    return aw_int_disable(p_devinfo->inum);
}

/** \brief 获取计数器溢出值 */
aw_local aw_err_t __imx10xx_pit_rollover_get (void *p_drv, uint32_t *p_count)
{
    __PIT_DEV_DECL(p_this, p_drv);

    aw_assert(NULL != p_this);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }
    *p_count = p_this->rollover_period;

    return AW_OK;
}

/** \brief 获取当前计数值 */
aw_local aw_err_t __imx10xx_pit_count_get (void *p_drv, uint32_t *p_count)
{
    __PIT_DEV_DECL(p_this, p_drv);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);

    aw_assert(NULL != p_this);
    aw_assert(NULL != p_devinfo);
    aw_assert(NULL != p_reg);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }

    *p_count = AW_REG_READ32(&p_reg->channel[0].cval);

    return AW_OK;
}

/** \brief 失能 PIT */
aw_local aw_err_t __imx10xx_pit_disable (void *p_drv)
{
    __PIT_DEV_DECL(p_this, p_drv);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);

    aw_assert(NULL != p_this);
    aw_assert(NULL != p_devinfo);
    aw_assert(NULL != p_reg);

    if (p_this->enabled) {

        /* 清零控制寄存器 */
        AW_REG_WRITE32(&p_reg->channel[0].tctrl, 0);

        /* 失能 PIT */
        AW_REG_BIT_SET_MASK32(&p_reg->mcr, __PIT_MCR_MDIS_MASK);

        /* 禁能 PIT 时钟 */
        aw_clk_disable(p_devinfo->clk_id);

        p_this->enabled = 0;
    }

    return AW_OK;
}

/** \brief 使能 PIT */
aw_local aw_err_t __imx10xx_pit_enable (void *p_drv, uint32_t max_timer_count)
{
    __PIT_DEV_DECL(p_this, p_drv);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);

    aw_assert(NULL != p_this);
    aw_assert(NULL != p_devinfo);
    aw_assert(NULL != p_reg);

    if (p_this->enabled) {
        return -AW_EBUSY;
    }

    /* 使能PIT时钟 */
    aw_clk_enable(p_devinfo->clk_id);

    /* 使能 PIT */
    AW_REG_BIT_CLR_MASK32(&p_reg->mcr, __PIT_MCR_MDIS_MASK);

    writel(0, &p_reg->channel[0].tctrl);

    /* 设置计数最大值(即重装载计数值) */
    AW_REG_WRITE32(&p_reg->channel[0].ldval, max_timer_count);

    /* 清除标志(写1清0) */
    AW_REG_WRITE32(&p_reg->channel[0].tflg, __PIT_TFLG_TIF_MASK);

    /* 使能中断、开始递减计数 */
    AW_REG_WRITE32(&p_reg->channel[0].tctrl, __PIT_TCTRL_TEN_MASK |
                                             __PIT_TCTRL_TIE_MASK);

    p_this->enabled = 1;

    return AW_OK;
}

/** \brief 设置中断回调函数 */
aw_local aw_err_t __imx10xx_pit_isr_set (void  *p_drv,
                                         void (*pfn_isr) (void *),
                                         void  *p_arg)
{
    __PIT_DEV_DECL(p_this, p_drv);

    aw_assert(p_this != NULL);

    p_this->pfn_isr = pfn_isr;
    p_this->p_arg   = p_arg;

    return AW_OK;
}

/** \brief METHOD: 获取指向标准定时器结构的指针 */
aw_local aw_err_t __imx10xx_pit_func_get (struct awbl_dev    *p_dev,
                                          struct awbl_timer **pp_awbl_tmr,
                                          int                 timer_no)
{
    __PIT_DEV_DECL(p_this, p_dev);

    aw_assert((p_this != NULL) && (pp_awbl_tmr != NULL));

    if (timer_no != 0) {
        return AW_ERROR;
    }

    *pp_awbl_tmr = &(p_this->tmr);

    return AW_OK;
}

/** \brief 第一阶段初始化 */
aw_local void __imx10xx_pit_inst_init1 (struct awbl_dev *p_dev)
{
    __PIT_DEV_DECL(p_this, p_dev);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_dev);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);
    struct awbl_timer *p_awbl_tmr = &(p_this->tmr);
    int                i;

    aw_assert(NULL != p_this);
    aw_assert(NULL != p_devinfo);
    aw_assert(NULL != p_reg);

    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    p_this->count = 0;
    p_this->enabled = 0;
    
    /* 使能PIT时钟 */
    aw_clk_enable(p_devinfo->clk_id);

    p_this->param.clk_frequency = aw_clk_rate_get(p_devinfo->clk_id);

    /* 禁能PIT时钟 */
    aw_clk_disable(p_devinfo->clk_id);

    p_this->param.max_frequency = p_this->param.clk_frequency / 2;
    p_this->param.min_frequency = 1;
    
    p_this->param.features = AW_HWTIMER_CAN_INTERRUPT      |
                             AW_HWTIMER_INTERMEDIATE_COUNT |
                             AW_HWTIMER_AUTO_RELOAD        |
                             AW_HWTIMER_SIZE(32);

    p_this->rollover_period =  0xFFFFFFFF / (p_this->param.clk_frequency);
    
    p_awbl_tmr->p_param = &p_this->param;
    p_awbl_tmr->p_func = &__g_imx10xx_timer_functions;
    p_awbl_tmr->allocated = AW_FALSE;
    p_awbl_tmr->ticks_per_second = 60u;
    
    /* 定时器通道属性配置，单通道只需要判断bit0为是否置为即可 */
    if (AW_BIT_ISSET(p_devinfo->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }

    /* 连接中断 */
    while (AW_OK != aw_int_connect(p_devinfo->inum,
                                   (aw_pfuncvoid_t)__imx10xx_pit_isr,
                                   (void *)p_this)) {
    }
}

/** \brief 第二阶段初始化 */
aw_local void __imx10xx_pit_inst_init2 (struct awbl_dev *p_dev)
{

}

/** \brief 第三阶段初始化 */
aw_local void __imx10xx_pit_inst_connect (struct awbl_dev *p_dev)
{
    return;
}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 IMX10xx PIT 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_pit_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_pit_drvinfo);
}

/* end of file */
