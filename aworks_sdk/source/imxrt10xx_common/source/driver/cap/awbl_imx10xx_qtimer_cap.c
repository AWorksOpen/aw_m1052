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
 * \brief iMX RT10xx QTimer输入捕获驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-03  mex, first implemetation
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
#include "driver/cap/awbl_imx10xx_qtimer_cap.h"

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


/**
 * \brief Quad Timer input sources selection
 */
typedef enum _qtmr_input_source
{
    kQTMR_Counter0InputPin = 0, /**< \brief Use counter 0 input pin */
    kQTMR_Counter1InputPin,     /**< \brief Use counter 1 input pin */
    kQTMR_Counter2InputPin,     /**< \brief Use counter 2 input pin */
    kQTMR_Counter3InputPin      /**< \brief Use counter 3 input pin */
} qtmr_input_source_t;


/**
 * \brief Quad Timer input capture edge mode, rising edge, or falling edge
 */
typedef enum _qtmr_input_capture_edge
{
    kQTMR_NoCapture = 0,       /**< \brief Capture is disabled */
    kQTMR_RisingEdge,          /**< \brief Capture on rising edge (IPS=0) or falling edge (IPS=1)*/
    kQTMR_FallingEdge,         /**< \brief Capture on falling edge (IPS=0) or rising edge (IPS=1)*/
    kQTMR_RisingAndFallingEdge  /**< \brief Capture on both edges */
} qtmr_input_capture_edge_t;


/* declare CAP device instance */
#define __CAP_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_qtimer_cap_dev *p_this = \
        (struct awbl_imx10xx_qtimer_cap_dev *)(p_dev)

/* declare CAP device infomation */
#define __CAP_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_qtimer_cap_devinfo *p_devinfo = \
        (struct awbl_imx10xx_qtimer_cap_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get CAP hardware */
#define __CAP_HW_DECL(p_hw_cap, p_devinfo) \
        imx10xx_qtimer_regs_t *(p_hw_cap) =  \
        ((imx10xx_qtimer_regs_t *)((p_devinfo)->regbase))

/* get CAP device config */
#define __CAP_DEV_CONFIG(p_devcfg, p_devinfo) \
        struct awbl_imx10xx_qtimer_cap_devcfg  *p_devcfg = \
            (struct awbl_imx10xx_qtimer_cap_devcfg *)p_devinfo->cap_devcfg;


#define __TMR_CTRL_SCS_MASK           (0x180UL)
#define __TMR_CTRL_PCS(x)             (((x) & 0xF) << 9)   /* Primary Count Source */
#define __TMR_CTRL_SCS(x)             (((x) & 0x3) << 7)   /* Secondary Count Source */
#define __TMR_CTRL_CM_MASK            (0xE000UL)
#define __TMR_CTRL_CM(x)              (((x) & 0x7) << 13)

#define __TMR_SCTRL_IPS_MASK          (0x200UL)
#define __TMR_SCTRL_CAPTURE_MODE_MASK (0xC0UL)
#define __TMR_SCTRL_OEN_MASK          (0x1UL)
#define __TMR_SCTRL_CAPTURE_MODE(x)   (((x) & 0x3) << 6)   /* Input Capture Mode */
#define __TMR_SCTRL_IEFIE_MASK        (0x400UL)
#define __TMR_SCTRL_IEF_MASK          (0x800U)

#define __TMR_CSCTRL_ROC_MASK         (0x800UL)


/**
 * \brief QTimer硬件初始化
 */
aw_local void __imx10xx_qtimer_hard_init (awbl_imx10xx_qtimer_cap_dev_t *p_this,
                                          uint8_t                        channel)
{
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);

    uint16_t reg = 0;

    /* Set Primary Count Source and Secondary Count Source */
    reg = (__TMR_CTRL_PCS(p_devinfo->prescaler_div) |
           __TMR_CTRL_SCS(kQTMR_Counter0InputPin));
    writew(reg, &p_hw_cap->CHANNEL[channel].CTRL);

    /* Disable master mode */
    writew(0x00, &p_hw_cap->CHANNEL[channel].SCTRL);

    /* Continue with normal operation during debug mode */
    writew(0x00, &p_hw_cap->CHANNEL[channel].CSCTRL);

    /* Clear the input filter Register*/
    writew(0x00, &p_hw_cap->CHANNEL[channel].FILT);

}


/**
 * \brief QTimer CAP中断服务函数
 */
aw_local void __qtimer_cap_isr (void *p_arg)
{
    __CAP_DEV_DECL(p_this, p_arg);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);

    uint8_t      ch   = 0;
    uint16_t     reg  = 0;
    uint16_t     vale = 0;
    volatile int i    = 0;

    for (i = 0; i < p_devinfo->pnum; i++) {
        ch = p_devinfo->cap_devcfg[i].ch;
        reg = readw(&p_hw_cap->CHANNEL[ch].SCTRL);
        if (reg & __TMR_SCTRL_IEF_MASK) {

            /* 清除状态 */
            reg &=~__TMR_SCTRL_IEF_MASK;
            writew(reg, &p_hw_cap->CHANNEL[ch].SCTRL);

            if (p_this->callback_info[ch].callback_func != NULL) {

                vale = readw(&p_hw_cap->CHANNEL[ch].CAPT);
                p_this->callback_info[ch].callback_func(p_this->callback_info[ch].p_arg,
                                                        vale);
            }
        }
    }
}


/**
 * \brief QTimer CAP配置
 */
aw_local aw_err_t __imx10xx_cap_config(void           *p_cookie,
                                       int             pid,
                                       uint32_t        options,
                                       cap_callback_t  pfunc_callback,
                                       void           *p_arg)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;
    uint16_t reg = 0;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    /* 查找pid对应的CAP通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 设置通道对应的输入源 */
    reg = readw(&p_hw_cap->CHANNEL[ch].CTRL) & (~__TMR_CTRL_SCS_MASK);
    reg |= __TMR_CTRL_SCS(ch);
    writew(reg, &p_hw_cap->CHANNEL[ch].CTRL);

    /* Clear the prior values forcinput polarity, capture mode. Set the external pin as input */
    reg = readw(&p_hw_cap->CHANNEL[ch].SCTRL);
    reg &=  (~(__TMR_SCTRL_IPS_MASK | __TMR_SCTRL_CAPTURE_MODE_MASK | __TMR_SCTRL_OEN_MASK));

    /* 设置触发条件 */
    if ((options & AW_CAP_TRIGGER_RISE) == AW_CAP_TRIGGER_RISE) {
        reg |= __TMR_SCTRL_CAPTURE_MODE(kQTMR_RisingEdge);
    } else if ((options & AW_CAP_TRIGGER_FALL) == AW_CAP_TRIGGER_FALL) {
        reg |= __TMR_SCTRL_CAPTURE_MODE(kQTMR_FallingEdge);
    } else if ((options & AW_CAP_TRIGGER_BOTH_EDGES) == AW_CAP_TRIGGER_BOTH_EDGES) {
        reg |= __TMR_SCTRL_CAPTURE_MODE(kQTMR_RisingAndFallingEdge);
    }
    writew(reg, &p_hw_cap->CHANNEL[ch].SCTRL);

    /* reload the counter on a capture event */
    reg = readw(&p_hw_cap->CHANNEL[ch].CSCTRL);
    reg |=__TMR_CSCTRL_ROC_MASK;
    writew(reg, &p_hw_cap->CHANNEL[ch].CSCTRL);

    /* 设置中断回掉函数 */
    p_this->callback_info[ch].callback_func = pfunc_callback;
    p_this->callback_info[ch].p_arg = p_arg;

    return AW_OK;
}


/**
 * \brief QTimer CAP使能
 */
aw_local aw_err_t __imx10xx_cap_enable(void *p_cookie, int pid)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  ch  = 0;
    uint8_t  i   = 0;
    uint16_t reg = 0;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    /* 查找pid对应的CAP通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 清空计数器值 */
    writew(0x00, &p_hw_cap->CHANNEL[ch].CAPT);
    writew(0x00, &p_hw_cap->CHANNEL[ch].HOLD);
    writew(0x00, &p_hw_cap->CHANNEL[ch].CNTR);

    /* 使能通道 */
    reg = readw(&p_hw_cap->CHANNEL[ch].ENBL);
    reg |= (1 << ch);
    writew(reg, &p_hw_cap->CHANNEL[ch].ENBL);

    //清除中断状态
    p_hw_cap->CHANNEL[ch].SCTRL &=~ __TMR_SCTRL_IEF_MASK;

    /* 使能对应通道的中断 */
    reg = readw(&p_hw_cap->CHANNEL[ch].DMA);
    reg &= ~0x01;
    writew(reg, &p_hw_cap->CHANNEL[ch].DMA);
    reg = readw(&p_hw_cap->CHANNEL[ch].SCTRL);
    reg |= __TMR_SCTRL_IEFIE_MASK;
    writew(reg, &p_hw_cap->CHANNEL[ch].SCTRL);

    /* 启动捕获 */
    reg = readw(&p_hw_cap->CHANNEL[ch].CTRL);
    reg &= ~__TMR_CTRL_CM_MASK;
    reg |=  __TMR_CTRL_CM(0x01);  /* Count rising edges of primary source1 */
    writew(reg, &p_hw_cap->CHANNEL[ch].CTRL);

    return AW_OK;
}

/**
 * \brief QTimer CAP禁能
 */
aw_local aw_err_t __imx10xx_cap_disable(void *p_cookie, int pid)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  ch  = 0;
    uint8_t  i   = 0;
    uint16_t reg = 0;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    /* 查找pid对应的CAP通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 停止捕获 */
    reg = readw(&p_hw_cap->CHANNEL[ch].CTRL);
    reg &= ~__TMR_CTRL_CM_MASK;
    writew(reg, &p_hw_cap->CHANNEL[ch].CTRL);

    p_hw_cap->CHANNEL[ch].CAPT = 0;

    /* 关闭中断 */
    reg = readw(&p_hw_cap->CHANNEL[ch].SCTRL);
    reg &= ~__TMR_SCTRL_IEFIE_MASK;
    writew(reg, &p_hw_cap->CHANNEL[ch].SCTRL);

    /* 禁能通道 */
    reg = readw(&p_hw_cap->CHANNEL[ch].ENBL);
    reg &= ~(1 << ch);
    writew(reg, &p_hw_cap->CHANNEL[ch].ENBL);

    return AW_OK;
}


/**
 * \brief QTimer CAP复位
 */
aw_local aw_err_t __imx10xx_cap_reset(void *p_cookie, int pid)
{

    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  ch  = 0;
    uint8_t  i   = 0;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    /* 查找pid对应的CAP通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    p_this->callback_info[ch].callback_func = NULL;
    p_this->callback_info[ch].p_arg = NULL;
    __imx10xx_qtimer_hard_init(p_this, p_devinfo->cap_devcfg[ch].ch);

    return AW_OK;
}

/**
 * \brief QTimer CAP时间换算
 */
aw_local aw_err_t __imx10xx_cap_count_to_time( void        *p_cookie,
                                              int           pid,
                                              unsigned int  count1,     /* 第一次捕获得到的值 */
                                              unsigned int  count2,     /* 第二次捕获得到的值 */
                                              unsigned int *p_time_ns)  /* 计算得到两次捕获之间的时间 */
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint32_t  count_err = 0;
    uint8_t   ch        = 0;
    uint8_t   i         = 0;
    uint32_t clkfreq    = aw_clk_rate_get(p_devinfo->ref_clk_id);

    uint16_t reg = 0;
    uint8_t  div = 1;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    /* 查找pid对应的CAP通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 计算预分频 */
    reg = (readw(&p_hw_cap->CHANNEL[ch].CTRL) >> 9) & 0xF;
    div <<= (reg - 8);
    clkfreq /= div;

    count_err = count2 - count1;
    uint64_t tmp = (uint64_t)1000000000 * (uint64_t)count_err;

    /* convert the count err value to time  */
    *p_time_ns = (uint64_t)tmp / (uint64_t)clkfreq;

    return AW_OK;
}


/** \brief first level initialization routine */
aw_local void __cap_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}


/** \brief second level initialization routine */
aw_local void __cap_inst_init2 (awbl_dev_t *p_dev)
{
    __CAP_DEV_DECL(p_this, p_dev);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);

    int     i  = 0;
    uint8_t ch = 0;

    /* 调用平台初始化函数 */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* 初始化QTimer使用到的通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {

        ch = p_devinfo->cap_devcfg[i].ch;
        p_this->callback_info[ch].callback_func = NULL;
        p_this->callback_info[ch].p_arg = NULL;
        __imx10xx_qtimer_hard_init(p_this, ch);
    }

    /* 连接中断服务函数 */
    aw_int_connect(p_devinfo->inum,
                  (aw_pfuncvoid_t)__qtimer_cap_isr,
                  (void *)p_dev);


    aw_int_enable(p_devinfo->inum);
}

/** \brief third level initialization routine */
aw_local void __cap_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/*******************************************************************************
    cap driver functions
*******************************************************************************/
aw_local aw_const struct awbl_cap_servopts __g_imx10xx_cap_servopts = {
    __imx10xx_cap_config,         /* pfunc_cap_config */
    __imx10xx_cap_enable,         /* pfunc_cap_enable */
    __imx10xx_cap_disable,        /* pfunc_cap_disable */
    __imx10xx_cap_reset,          /* pfunc_cap_reset */
    __imx10xx_cap_count_to_time,  /* pfunc_cap_count_to_time */
};

aw_local aw_const struct awbl_drvfuncs __g_cap_std_drvfuncs = {
    __cap_inst_init1,
    __cap_inst_init2,
    __cap_inst_connect
};

/** \brief method "awbl_cap_func_get" handler */
aw_local aw_err_t __imx10xx_cap_serv_get (struct awbl_dev *p_dev, void *p_arg)
{

    __CAP_DEV_DECL(p_this, p_dev);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);

    struct awbl_cap_service *p_serv = NULL;

     /* 获取cap服务实例 */
    p_serv = &p_this->cap_serv;

    /* 初始化cap服务实例 */
    p_serv->p_next = NULL;
    p_serv->p_servinfo = &p_devinfo->cap_servinfo;
    p_serv->p_servfuncs = &__g_imx10xx_cap_servopts;
    p_serv->p_cookie = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_cap_service **)p_arg = p_serv;

     return AW_OK;
}

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_capserv_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx10xx_cap_methods[] = {
    AWBL_METHOD(awbl_capserv_get, __imx10xx_cap_serv_get),
    AWBL_METHOD_END
};


/** 本驱动提供了一种方法，该方法用于得到cap服务（服务体现在3个函数指针上） */
aw_local aw_const awbl_plb_drvinfo_t __g_cap_drv_registration = {
    {
        AWBL_VER_1,                   /* awb_ver */
        AWBL_BUSID_PLB,               /* bus_id */
        AWBL_IMX10XX_QTIMER_CAP_NAME, /* p_drvname */
        &__g_cap_std_drvfuncs,        /* p_busfuncs */
        &__g_imx10xx_cap_methods[0],  /* p_methods */
        NULL                          /* pfunc_drv_probe */
    }
};

void awbl_imx10xx_qtimer_cap_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_cap_drv_registration);
}

/* end of file */
