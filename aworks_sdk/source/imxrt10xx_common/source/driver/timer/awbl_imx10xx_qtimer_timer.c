/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief iMX RT10XX QTimer定时器驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 19-07-01  ral, first implemetation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_int.h"
#include "aw_bitops.h"
#include "aw_hwtimer.h"
#include "driver/timer/awbl_imx10xx_qtimer_timer.h"

/**
 * \brief TMR - Register Layout Typedef
 */
typedef struct {
    struct {                            /* offset: 0x0, array step: 0x20 */
        volatile uint16_t COMP1;        /**< \brief Timer Channel Compare Register 1, array offset: 0x0, array step: 0x20 */
        volatile uint16_t COMP2;        /**< \brief Timer Channel Compare Register 2, array offset: 0x2, array step: 0x20 */
        volatile uint16_t CAPT;         /**< \brief Timer Channel Capture Register, array offset: 0x4, array step: 0x20 */
        volatile uint16_t LOAD;         /**< \brief Timer Channel Load Register, array offset: 0x6, array step: 0x20 */
        volatile uint16_t HOLD;         /**< \brief Timer Channel Hold Register, array offset: 0x8, array step: 0x20 */
        volatile uint16_t CNTR;         /**< \brief Timer Channel Counter Register, array offset: 0xA, array step: 0x20 */
        volatile uint16_t CTRL;         /**< \brief Timer Channel Control Register, array offset: 0xC, array step: 0x20 */
        volatile uint16_t SCTRL;        /**< \brief Timer Channel Status and Control Register, array offset: 0xE, array step: 0x20 */
        volatile uint16_t CMPLD1;       /**< \brief Timer Channel Comparator Load Register 1, array offset: 0x10, array step: 0x20 */
        volatile uint16_t CMPLD2;       /**< \brief Timer Channel Comparator Load Register 2, array offset: 0x12, array step: 0x20 */
        volatile uint16_t CSCTRL;       /**< \brief Timer Channel Comparator Status and Control Register, array offset: 0x14, array step: 0x20 */
        volatile uint16_t FILT;         /**< \brief Timer Channel Input Filter Register, array offset: 0x16, array step: 0x20 */
        volatile uint16_t DMA;          /**< \brief Timer Channel DMA Enable Register, array offset: 0x18, array step: 0x20 */
                 uint8_t RESERVED_0[4];
        volatile uint16_t ENBL;         /**< \brief Timer Channel Enable Register, array offset: 0x1E, array step: 0x20, this item is not available for all array instances */
    } CHANNEL[4];

} imx10xx_qtimer_regs_t;


/* declare TIMER device instance */
#define __TIMER_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_qtimer_timer_dev *p_this = \
        (struct awbl_imx10xx_qtimer_timer_dev *)(p_dev)

/* declare TIMER device infomation */
#define __TIMER_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_qtimer_timer_devinfo *p_devinfo = \
        (struct awbl_imx10xx_qtimer_timer_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get TIMER hardware */
#define __TIMER_HW_DECL(p_hw_timer, p_devinfo) \
        imx10xx_qtimer_regs_t *(p_hw_timer) =  \
        ((imx10xx_qtimer_regs_t *)((p_devinfo)->regbase))

#define __QTIMER_REG_READ16(addr) \
        readw((volatile void __iomem *)(addr))

#define __QTIMER_REG_WRITE16(addr, val) \
        writew(val, (volatile void __iomem *)(addr))

#define __QTIMER_REG_WRITE_MASK16(addr, mask)   \
    do { \
        uint16_t __val__ = readw((volatile void __iomem *)(addr)); \
        AW_BIT_SET_MASK(__val__, mask); \
        writew(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

#define __QTIMER_REG_CLR_MASK16(addr, mask) \
    do { \
        uint16_t __val__ = readw((volatile void __iomem *)(addr)); \
        AW_BIT_CLR_MASK(__val__, mask); \
        writew(__val__, (volatile void __iomem *)(addr)); \
    } while (0)

#define TMR_CTRL_DIR_MASK                    (0x10U)
#define TMR_CTRL_DIR_SHIFT                   (4U)
#define TMR_CTRL_DIR(x)                      (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_DIR_SHIFT)) & TMR_CTRL_DIR_MASK)

#define TMR_CTRL_LENGTH_MASK                 (0x20U)
#define TMR_CTRL_LENGTH_SHIFT                (5U)
#define TMR_CTRL_LENGTH(x)                   (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_LENGTH_SHIFT)) & TMR_CTRL_LENGTH_MASK)

#define TMR_CTRL_SCS_MASK                    (0x180U)
#define TMR_CTRL_SCS_SHIFT                   (7U)
#define TMR_CTRL_SCS(x)                      (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_SCS_SHIFT)) & TMR_CTRL_SCS_MASK)

#define TMR_CTRL_PCS_MASK                    (0x1E00U)
#define TMR_CTRL_PCS_SHIFT                   (9U)
#define TMR_CTRL_PCS(x)                      (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_PCS_SHIFT)) & TMR_CTRL_PCS_MASK)

#define TMR_CTRL_CM_MASK                     (0xE000U)
#define TMR_CTRL_CM_SHIFT                    (13U)
#define TMR_CTRL_CM(x)                       (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_CM_SHIFT)) & TMR_CTRL_CM_MASK)

#define TMR_FILT_FILT_CNT_MASK               (0x700U)
#define TMR_FILT_FILT_CNT_SHIFT              (8U)
#define TMR_FILT_FILT_CNT(x)                 (((uint16_t)(((uint16_t)(x)) << TMR_FILT_FILT_CNT_SHIFT)) & TMR_FILT_FILT_CNT_MASK)

#define TMR_FILT_FILT_PER_MASK               (0xFFU)
#define TMR_FILT_FILT_PER_SHIFT              (0U)
#define TMR_FILT_FILT_PER(x)                 (((uint16_t)(((uint16_t)(x)) << TMR_FILT_FILT_PER_SHIFT)) & TMR_FILT_FILT_PER_MASK)

#define TMR_SCTRL_EEOF_MASK                  (0x10U)
#define TMR_SCTRL_EEOF_SHIFT                 (4U)
#define TMR_SCTRL_EEOF(x)                    (((uint16_t)(((uint16_t)(x)) << TMR_SCTRL_EEOF_SHIFT)) & TMR_SCTRL_EEOF_MASK)

#define TMR_SCTRL_MSTR_MASK                  (0x20U)
#define TMR_SCTRL_MSTR_SHIFT                 (5U)
#define TMR_SCTRL_MSTR(x)                    (((uint16_t)(((uint16_t)(x)) << TMR_SCTRL_MSTR_SHIFT)) & TMR_SCTRL_MSTR_MASK)

#define TMR_CSCTRL_DBG_EN_MASK               (0xC000U)
#define TMR_CSCTRL_DBG_EN_SHIFT              (14U)
#define TMR_CSCTRL_DBG_EN(x)                 (((uint16_t)(((uint16_t)(x)) << TMR_CSCTRL_DBG_EN_SHIFT)) & TMR_CSCTRL_DBG_EN_MASK)

#define TMR_SCTRL_TCF_MASK                   (0x8000U)
#define TMR_SCTRL_TCF_SHIFT                  (15U)
#define TMR_SCTRL_TCF(x)                     (((uint16_t)(((uint16_t)(x)) << TMR_SCTRL_TCF_SHIFT)) & TMR_SCTRL_TCF_MASK)

#define TMR_SCTRL_TCFIE_MASK                 (0x4000U)
#define TMR_SCTRL_TCFIE_SHIFT                (14U)
#define TMR_SCTRL_TCFIE(x)                   (((uint16_t)(((uint16_t)(x)) << TMR_SCTRL_TCFIE_SHIFT)) & TMR_SCTRL_TCFIE_MASK)

#define TMR_CTRL_CM_MASK                     (0xE000U)
#define TMR_CTRL_CM_SHIFT                    (13U)
#define TMR_CTRL_CM(x)                       (((uint16_t)(((uint16_t)(x)) << TMR_CTRL_CM_SHIFT)) & TMR_CTRL_CM_MASK)

/*******************************************************************************
  local function
*******************************************************************************/
/** \brief 为 QTimer 分配资源 */
aw_local aw_err_t __qtimer_timer_allocate (struct awbl_dev *p_dev,
                                           uint32_t         flags,
                                           void           **pp_drv,
                                           int              timer_no);

 /** \brief 为 QTimer 释放资源 */
 aw_local aw_err_t __qtimer_timer_release (void *p_drv);

 /** \brief 获取计数器溢出值 */
 aw_local aw_err_t __qtimer_timer_rollover_get (void *p_drv, uint32_t *p_count);

 /** \brief 获取当前计数值 */
 aw_local aw_err_t __qtimer_timer_count_get (void *p_drv, uint32_t *p_count);

 /** \brief 禁能 QTimer */
 aw_local aw_err_t __qtimer_timer_disable (void *p_drv);

 /** \brief 使能 QTimer */
 aw_local aw_err_t __qtimer_timer_enable (void *p_drv, uint32_t max_timer_count);

 /** \brief 设置中断回调函数 */
 aw_local aw_err_t __qtimer_timer_isr_set (void  *p_drv,
                                           void (*pfn_isr) (void *),
                                           void  *p_arg);

 /** \brief METHOD: 获取指向标准定时器结构的指针 */
 aw_local aw_err_t __qtimer_timer_func_get (struct awbl_dev    *p_dev,
                                            struct awbl_timer **pp_awbl_tmr,
                                            int                 timer_no);

/** \brief 驱动入口点函数 */
aw_local void __qtimer_timer_inst_init1 (struct awbl_dev *p_dev);
aw_local void __qtimer_timer_inst_init2 (struct awbl_dev *p_dev);
aw_local void __qtimer_timer_inst_connect (struct awbl_dev *p_dev);

/*******************************************************************************
  local function define
*******************************************************************************/
/**
 * \brief 定时器中断处理函数
 */
aw_local void __qtimer_timer_isr (struct awbl_imx10xx_qtimer_timer_dev *p_this)
{
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);
    __TIMER_HW_DECL(p_hw_timer, p_devinfo);
    void (*pfn_isr) (void *);

    uint8_t ch = p_devinfo->channel;

    /* clear interrupt flag */
    __QTIMER_REG_CLR_MASK16(&p_hw_timer->CHANNEL[ch].SCTRL, TMR_SCTRL_TCF_MASK);

    /* 执行用户回调函数 */
    pfn_isr = p_this->pfn_isr;
    if (pfn_isr != NULL) {
        (*pfn_isr)(p_this->p_arg);
    }
}

/**
 * \brief 禁能 Timer
 */
aw_local aw_err_t __qtimer_timer_disable (void *p_drv)
{
    __TIMER_DEV_DECL(p_this, p_drv);
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);
    __TIMER_HW_DECL(p_hw_timer, p_devinfo);

    uint8_t ch = p_devinfo->channel;

    if (p_this->enabled) {

        __QTIMER_REG_CLR_MASK16(&p_hw_timer->CHANNEL[ch].CTRL, TMR_CTRL_CM_MASK);

        /* 禁能 QTimer 时钟 */
        aw_clk_disable(p_devinfo->clk_id);

        p_this->enabled = 0;
    }

    return AW_OK;
}

/**
 * \brief 获取计数器溢出值
 */
aw_local aw_err_t __qtimer_timer_rollover_get (void *p_drv, uint32_t *p_count)
{
    __TIMER_DEV_DECL(p_this, p_drv);

    if (NULL == p_count) {
        return -AW_EINVAL;
    }

    *p_count = p_this->rollover_period;

    return AW_OK;
}

/**
 * \brief 获取当前计数值
 */
aw_local aw_err_t __qtimer_timer_count_get (void *p_drv, uint32_t *p_count)
{
    __TIMER_DEV_DECL(p_this, p_drv);
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);
    __TIMER_HW_DECL(p_hw_timer, p_devinfo);

    uint8_t ch = p_devinfo->channel;

    if (NULL == p_count) {
        return -AW_EINVAL;
    }

    *p_count = __QTIMER_REG_READ16(&p_hw_timer->CHANNEL[ch].CNTR);

    return AW_OK;
}

/**
 * \brief 使能 QTimer
 */
aw_local aw_err_t __qtimer_timer_enable (void *p_drv, uint32_t max_timer_count)
{
    __TIMER_DEV_DECL(p_this, p_drv);
    __TIMER_DEVINFO_DECL(p_devinfo, p_drv);
    __TIMER_HW_DECL(p_hw_timer, p_devinfo);
    uint16_t reg;

    uint8_t ch = p_devinfo->channel;

    if (p_this->enabled) {
        return -AW_EBUSY;
    }

    /* 使能QTimer时钟 */
    aw_clk_enable(p_devinfo->clk_id);

    /* 设置定时器为重载模式  */
    __QTIMER_REG_WRITE_MASK16(&p_hw_timer->CHANNEL[ch].CTRL, TMR_CTRL_LENGTH_MASK);

    reg = __QTIMER_REG_READ16(&p_hw_timer->CHANNEL[ch].CTRL);
    if (reg & TMR_CTRL_DIR_MASK)
    {
        /* Counting down */
        __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[ch].COMP2, max_timer_count);
    }
    else
    {
        /* Counting up */
        __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[ch].COMP1, max_timer_count);
    }

    /* 使能中断*/
    __QTIMER_REG_WRITE_MASK16(&p_hw_timer->CHANNEL[ch].SCTRL, TMR_SCTRL_TCFIE_MASK);

    /* 使能 QTimer */
    reg = __QTIMER_REG_READ16(&p_hw_timer->CHANNEL[ch].CTRL);

    reg &= ~(TMR_CTRL_CM_MASK);
    reg |= TMR_CTRL_CM(0x01);

    __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[ch].CTRL, reg);

    p_this->enabled = 1;

    return AW_OK;
}

/**
 * \brief 设置中断回调函数
 */
aw_local aw_err_t __qtimer_timer_isr_set (void  *p_drv,
                                          void (*pfn_isr) (void *),
                                          void  *p_arg)
{
    __TIMER_DEV_DECL(p_this, p_drv);

    p_this->pfn_isr = pfn_isr;
    p_this->p_arg   = p_arg;

    return AW_OK;
}

/**
 * \brief 为 timer 分配资源
*/
aw_local aw_err_t __qtimer_timer_allocate (struct awbl_dev *p_dev,
                                           uint32_t         flags,
                                           void           **pp_drv,
                                           int              timer_no)
{
    __TIMER_DEV_DECL(p_this, p_dev);
    __TIMER_DEVINFO_DECL(p_devinfo, p_dev);

    int ret = AW_ERROR;

    if ((timer_no != 0) || (p_this->tmr.allocated)) {
        goto __exit;
    }

    p_this->tmr.allocated = AW_TRUE;

    /* 更新驱动信息 */
    *pp_drv = (void *)p_this;

    ret = aw_int_enable(p_devinfo->inum);

__exit:
    return  ret;
}

/**
 * \brief 为 timer 释放资源
 */
aw_local aw_err_t __qtimer_timer_release (void *p_drv)
{
    __TIMER_DEV_DECL(p_this, p_drv);
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);

    if (!p_this->tmr.allocated) {
        return AW_ERROR;
    }

    __qtimer_timer_disable(p_drv);

    p_this->pfn_isr = NULL;
    p_this->p_arg   = NULL;

    p_this->tmr.allocated = AW_FALSE;

    return aw_int_disable(p_devinfo->inum);
}

/**
 * \brief 获取指向标准定时器结构的指针
 */
aw_local aw_err_t __qtimer_timer_func_get (struct awbl_dev    *p_dev,
                                           struct awbl_timer **pp_awbl_tmr,
                                           int                 timer_no)
{
    __TIMER_DEV_DECL(p_this, p_dev);

    if (timer_no != 0) {
        return AW_ERROR;
    }

    *pp_awbl_tmr = &(p_this->tmr);

    return AW_OK;
}

/**
 * \brief QTimer硬件初始化
 */
aw_local void __imx10xx_qtimer_hard_init (
        awbl_imx10xx_qtimer_timer_dev_t *p_this,
        uint8_t                          channel)
{
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);
    __TIMER_HW_DECL(p_hw_timer, p_devinfo);

    uint32_t reg;

    /** IP bus clock divide by 128 prescaler
     *  Secondary Count Source is Counter 0 input pin
     */
    if (p_devinfo->count_mode == AW_TRUE) {
        reg = TMR_CTRL_PCS(p_devinfo->channel) |
              TMR_CTRL_SCS(0x00);
    } else {
        reg = TMR_CTRL_PCS(0x0f) |
              TMR_CTRL_SCS(0x00);
    }

    __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[channel].CTRL, reg);

    /* set master mode */
    reg = TMR_SCTRL_EEOF(0) | TMR_SCTRL_MSTR(0);
    __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[channel].SCTRL, reg);

    /* set debug mode */
    reg = TMR_CSCTRL_DBG_EN(0);
    __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[channel].CSCTRL, reg);

    /* set input filter */
    reg = TMR_FILT_FILT_CNT_MASK | TMR_FILT_FILT_PER_MASK;
    __QTIMER_REG_CLR_MASK16(&p_hw_timer->CHANNEL[channel].FILT, reg);

    reg = TMR_FILT_FILT_CNT(0) | TMR_FILT_FILT_PER(0);
    __QTIMER_REG_WRITE16(&p_hw_timer->CHANNEL[channel].FILT, reg);
}

/**
 * \brief 适配标准定时器接口的结构体
 */
aw_local aw_const struct awbl_timer_functions __g_qtimer_timer_functions = {
    __qtimer_timer_allocate,     /* 为 timer分配资源 */
    __qtimer_timer_release,      /* 为 timer释放资源 */
    __qtimer_timer_rollover_get, /* 获取计数器溢出值 */
    __qtimer_timer_count_get,    /* 获取当前计数值 */
    __qtimer_timer_disable,      /* 禁能timer*/
    __qtimer_timer_enable,       /* 使能timer*/
    __qtimer_timer_isr_set,      /* 设置中断回调函数 */
    NULL,                        /* 使能 64 位定时器 */
    NULL,                        /* 获取 64 位计数器溢出值 */
    NULL                         /* 获取 64 位当前计数值 */
};

/**
 * \brief first level initialization routine
 */
aw_local void __qtimer_timer_inst_init1 (awbl_dev_t *p_dev)
{
    __TIMER_DEV_DECL(p_this, p_dev);
    __TIMER_DEVINFO_DECL(p_devinfo, p_this);

    struct awbl_timer *p_awbl_tmr = &(p_this->tmr);

    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    p_this->count = 0;
    p_this->enabled = 0;

    /* 初始化QTimer */
    __imx10xx_qtimer_hard_init(p_this, p_devinfo->channel);

    p_this->param.clk_frequency =
            (aw_clk_rate_get(p_devinfo->clk_id) / p_devinfo->prescaler_div);

    p_this->param.max_frequency = p_this->param.clk_frequency / 2;
    p_this->param.min_frequency = 16;

    p_this->param.features = AW_HWTIMER_CAN_INTERRUPT      |
                             AW_HWTIMER_INTERMEDIATE_COUNT |
                             AW_HWTIMER_AUTO_RELOAD        |
                             AW_HWTIMER_SIZE(16);

    p_this->rollover_period = 0xFFFF;

    p_awbl_tmr->p_param = &p_this->param;
    p_awbl_tmr->p_func = &__g_qtimer_timer_functions;
    p_awbl_tmr->allocated = AW_FALSE;
    p_awbl_tmr->ticks_per_second = 60u;

    /* 定时器通道属性配置，单通道只需要判断bit0为是否置为即可 */
    if (AW_BIT_ISSET(p_devinfo->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }

    return;
}

/** \brief second level initialization routine */
aw_local void __qtimer_timer_inst_init2 (awbl_dev_t *p_dev)
{
    __TIMER_DEV_DECL(p_this, p_dev);
    __TIMER_DEVINFO_DECL(p_devinfo, p_dev);

    /* 连接中断 */
    aw_int_connect(p_devinfo->inum,
                   (aw_pfuncvoid_t)__qtimer_timer_isr,
                   (void *)p_this);
}

/** \brief third level initialization routine */
aw_local void __qtimer_timer_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

aw_local aw_const struct awbl_drvfuncs __g_qtimer_timer_drvfuncs = {
    __qtimer_timer_inst_init1,
    __qtimer_timer_inst_init2,
    __qtimer_timer_inst_connect
};

/** \brief 设备方法 */
aw_local aw_const struct awbl_dev_method __g_qtimer_timer_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __qtimer_timer_func_get),
    AWBL_METHOD_END
};

/** \brief 驱动注册信息 */
aw_local aw_const awbl_plb_drvinfo_t __g_qtimer_timer_registration = {
    {
        AWBL_VER_1,                        /* awb_ver */
        AWBL_BUSID_PLB,                    /* bus_id */
        AWBL_IMX10XX_QTIMER_TIMER_NAME,    /* p_drvname */
        &__g_qtimer_timer_drvfuncs,        /* p_busfuncs */
        &__g_qtimer_timer_methods[0],      /* p_methods */
        NULL                               /* pfunc_drv_probe */
    }
};

/**
 * \brief 驱动注册函数
 */
void awbl_imx10xx_qtimer_timer_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_qtimer_timer_registration);
}

/* end of file */
