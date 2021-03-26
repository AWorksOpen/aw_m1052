/*******************************************************************************
*                                 AWorks
*                       ----------------------------
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
 * \brief Freescale i.MXRT10xx QTimer PWM外设驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-12  mex, first implemetation
 * \endinternal
 */

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "aw_system.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_int.h"
#include "driver/pwm/awbl_imx10xx_qtimer_pwm.h"

/**
 * \brief TMR - Register Layout Typedef
 */
typedef struct {
  struct {                              /* offset: 0x0, array step: 0x20 */
    volatile uint16_t COMP1;            /**< \brief Timer Channel Compare Register 1, array offset: 0x0, array step: 0x20 */
    volatile uint16_t COMP2;            /**< \brief Timer Channel Compare Register 2, array offset: 0x2, array step: 0x20 */
    volatile uint16_t CAPT;             /**< \brief Timer Channel Capture Register, array offset: 0x4, array step: 0x20 */
    volatile uint16_t LOAD;             /**< \brief Timer Channel Load Register, array offset: 0x6, array step: 0x20 */
    volatile uint16_t HOLD;             /**< \brief Timer Channel Hold Register, array offset: 0x8, array step: 0x20 */
    volatile uint16_t CNTR;             /**< \brief Timer Channel Counter Register, array offset: 0xA, array step: 0x20 */
    volatile uint16_t CTRL;             /**< \brief Timer Channel Control Register, array offset: 0xC, array step: 0x20 */
    volatile uint16_t SCTRL;            /**< \brief Timer Channel Status and Control Register, array offset: 0xE, array step: 0x20 */
    volatile uint16_t CMPLD1;           /**< \brief Timer Channel Comparator Load Register 1, array offset: 0x10, array step: 0x20 */
    volatile uint16_t CMPLD2;           /**< \brief Timer Channel Comparator Load Register 2, array offset: 0x12, array step: 0x20 */
    volatile uint16_t CSCTRL;           /**< \brief Timer Channel Comparator Status and Control Register, array offset: 0x14, array step: 0x20 */
    volatile uint16_t FILT;             /**< \brief Timer Channel Input Filter Register, array offset: 0x16, array step: 0x20 */
    volatile uint16_t DMA;              /**< \brief Timer Channel DMA Enable Register, array offset: 0x18, array step: 0x20 */
             uint8_t RESERVED_0[4];
    volatile uint16_t ENBL;             /**< \brief Timer Channel Enable Register, array offset: 0x1E, array step: 0x20, this item is not available for all array instances */
  } CHANNEL[4];
} imx10xx_qtimer_pwm_regs_t;


/*******************************************************************************
  macro operate
*******************************************************************************/

/* declare PWM device instance */
#define __PWM_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_qtimer_pwm_dev *p_this = \
        (struct awbl_imx10xx_qtimer_pwm_dev *)(p_dev)

/* declare PWM device infomation */
#define __PWM_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_qtimer_pwm_devinfo *p_devinfo = \
        (struct awbl_imx10xx_qtimer_pwm_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_PARAM(p_dev) \
    (awbl_imx10xx_qtimer_pwm_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_THIS(p_dev) \
    (awbl_imx10xx_qtimer_pwm_dev_t *)(p_dev)

/* get iMX10xx PWM hardware */
#define __EFLEX_PWM_HW_DECL(p_hw_pwm, p_devinfo)       \
        imx10xx_qtimer_pwm_regs_t *(p_hw_pwm) =         \
        ((imx10xx_qtimer_pwm_regs_t *)((p_devinfo)->regbase))

/* get pwm device config */
#define __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_devinfo)        \
        struct awbl_imx10xx_qtimer_pwm_devcfg  *p_devcfg =  \
            (struct awbl_imx10xx_qtimer_pwm_devcfg *)p_dev_info->pwm_devcfg;

/*******************************************************************************
  Timer Channel Control Register
*******************************************************************************/

/* Primary Count Source */
#define __TMR_CTRL_SCS(x)           (((x) & 0x3) << 7)

/* Secondary Count Source */
#define __TMR_CTRL_PCS_SHIFT         9
#define __TMR_CTRL_PCS_MASK         (0xFu << __TMR_CTRL_PCS_SHIFT)
#define __TMR_CTRL_PCS(x)           (((x) & 0xFu) << __TMR_CTRL_PCS_SHIFT)

/* Output Mode */
#define __TMR_CTRL_OUTMODE_MASK     (0x7u)
#define __TMR_CTRL_OUTMODE(x)       ((x) & 0x7u)

/* Count Mode */
#define __TMR_CTRL_CM_MASK          (0x7u << 13)
#define __TMR_CTRL_CM(x)            (((x) & 0x7u) << 13)

/* Count Length */
#define __TMR_CTRL_LENGTH_MASK      (0x1u << 5)

/*******************************************************************************
  Input Filter Register
*******************************************************************************/

/* Input Filter Sample Count */
#define __TMR_FILT_FILT_CNT(x)      (((x) & 0xFu) << 8)

/* Input Filter Sample Period */
#define __TMR_FILT_FILT_PER(x)      ((x) & 0xFFu)

/*******************************************************************************
  Timer Channel Status and Control Register
*******************************************************************************/

/* Output Enable */
#define __TMR_SCTRL_OEN_MASK        (0x1u << 0)

/* Output Polarity Select */
#define __TMR_SCTRL_OPS_MASK        (0x1u << 1)

/* Force OFLAG Output */
#define __TMR_SCTRL_FORCE_MASK      (0x1u << 2)

/*******************************************************************************
  Timer Channel Comparator Status and Control Register
*******************************************************************************/

/* Compare Load Control 1 */
#define __TMR_CSCTRL_CL1_MASK       (0x3u << 0)
#define __TMR_CSCTRL_CL1(x)         ((x) & 0x3u)

/* Compare Load Control 2 */
#define __TMR_CSCTRL_CL2_MASK       (0x3u << 2)
#define __TMR_CSCTRL_CL2(x)         (((x) & 0x3u) << 2)

/* Prescaler select */
#define __CLK_DIV_1                 (8)
#define __CLK_DIV_2                 (9)
#define __CLK_DIV_4                 (10)
#define __CLK_DIV_8                 (11)
#define __CLK_DIV_16                (12)
#define __CLK_DIV_32                (13)
#define __CLK_DIV_64                (14)
#define __CLK_DIV_128               (15)

/*******************************************************************************
  forward declarations
*******************************************************************************/

aw_local void __pwm_inst_init1 (awbl_dev_t *p_dev);
aw_local void __pwm_inst_init2 (awbl_dev_t *p_dev);
aw_local void __pwm_inst_connect (awbl_dev_t *p_dev);

aw_local aw_err_t __imx10xx_pwm_config (void          *p_cookie,
                                        int            pid,
                                        unsigned long  duty_ns,
                                        unsigned long  period_ns);

aw_local aw_err_t __imx10xx_pwm_enable (void *p_cookie, int pid);

aw_local aw_err_t __imx10xx_pwm_disable (void *p_cookie, int pid);

/*******************************************************************************
  implementation
*******************************************************************************/
aw_local void __imx10xx_qtimer_pwm_isr(void *p_arg)
{
    __PWM_DEV_DECL(p_this, p_arg);
    __PWM_DEVINFO_DECL(p_dev_info, p_arg);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);

    uint16_t reg;
    uint32_t i;
    uint32_t ch;

    for (i = 0; i < p_dev_info->pnum; i++) {
        ch = p_dev_info->pwm_devcfg[i].ch;
        reg = readw(&p_hw_pwm->CHANNEL[0].ENBL);

        if (reg & (0x1 << ch)) {
            reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
            reg &= ~(1 << 15);
            writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

            if (--p_this->channel[i].period_num == 0) {
                /* Stop the timer */
                reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
                reg &= ~(__TMR_CTRL_CM_MASK);
                writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

                /* Disable Timer Channel */
                reg = readw(&p_hw_pwm->CHANNEL[ch].ENBL);
                reg &= ~(1 << ch);
                writew(reg, &p_hw_pwm->CHANNEL[ch].ENBL);

                /*关闭中断 */
                reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
                reg &= ~(1 << 14);
                writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

                AW_SEMB_GIVE(p_this->channel[i].sync_sem);
            }
        }
    }
}


/* PWM配置 */
aw_local aw_err_t __imx10xx_pwm_config (void          *p_cookie,
                                        int            pid,
                                        unsigned long  duty_ns,
                                        unsigned long  period_ns)
{

    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint16_t reg = 0;
    uint32_t clkfreq = 0;
    uint32_t period_c, low_c, high_c;
    uint64_t c = 0;
    uint8_t  i = 0;
    uint8_t  ch = 0;
    uint32_t clk_div = 1;
    uint8_t  shift = 0;

    if ((period_ns == 0) || (duty_ns > 4294967295UL) ||
         (period_ns > 4294967295UL) || (duty_ns > period_ns)) {
         return -AW_EINVAL;
    }

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid对应的pwm通道 */
    for (i = 0; i < p_dev_info->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_dev_info->pnum) {
        return AW_ERROR;
    }

    /* Set OFLAG pin for output mode and force out a low on the pin */
    reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
    reg |=  (__TMR_SCTRL_FORCE_MASK | __TMR_SCTRL_OEN_MASK);
    writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

    clkfreq  = aw_clk_rate_get(p_dev_info->ref_clk_id) / clk_div;

    if (period_ns < 1000000000UL / clkfreq) {
        AW_INFOF(("awbl_imx10xx_qtimer_pwm: period_ns is invalid, as it is too smaller.\n"));
        return -AW_EINVAL;
    }

    /* Calculate the division factor and the cycle cout */
     while (1) {

        clkfreq  =  aw_clk_rate_get(p_dev_info->ref_clk_id) / clk_div;
        c = (uint64_t)period_ns * clkfreq;
        period_c = c / 1000000000UL;

         if ((period_c == 0) || (period_c > 0XFFFFu)) {
             shift++;
             clk_div  = (1 << shift);

             if (clk_div > 128) {

                 AW_INFOF(("awbl_imx10xx_pwm: period_ns is invalid, as it is too larger.\n"));
                 return -AW_EINVAL;
             }
         } else {
             break;
         }
     }

    high_c = (uint64_t)(duty_ns)   * (clkfreq) / (uint64_t)1000000000UL;
    low_c  = period_c - high_c;

    /* 设置预分频 */
    reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
    reg &= ~__TMR_CTRL_PCS_MASK;
    reg |=  __TMR_CTRL_PCS(shift + __CLK_DIV_1);
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    /* Setup the compare registers for PWM output */
    writew(low_c, &p_hw_pwm->CHANNEL[ch].COMP1);
    writew(high_c, &p_hw_pwm->CHANNEL[ch].COMP2);

    /* Setup the pre-load registers for PWM output */
    writew(low_c, &p_hw_pwm->CHANNEL[ch].CMPLD1);
    writew(high_c, &p_hw_pwm->CHANNEL[ch].CMPLD2);

    /*
     * Setup the compare load control for COMP1 and COMP2.
     * Load COMP1 when CSCTRL[TCF2] is asserted, load COMP2 when CSCTRL[TCF1] is asserted
     */
    reg = readw(&p_hw_pwm->CHANNEL[ch].CSCTRL);
    reg &= ~(__TMR_CSCTRL_CL1_MASK | __TMR_CSCTRL_CL2_MASK);
    reg |= (__TMR_CSCTRL_CL1(2) | __TMR_CSCTRL_CL2(1));
    writew(reg, &p_hw_pwm->CHANNEL[ch].CSCTRL);

    /* True polarity, no inversion */
    reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
    reg &= ~__TMR_SCTRL_OPS_MASK;
    writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

    reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
    reg &= ~(__TMR_CTRL_OUTMODE_MASK);
    /* Count until compare value is  reached and re-initialize the counter, toggle OFLAG output
     * using alternating compare register
     */
    reg |=  (__TMR_CTRL_LENGTH_MASK | __TMR_CTRL_OUTMODE(4));
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    return AW_OK;
}


/* PWM使能 */
aw_local aw_err_t __imx10xx_pwm_enable (void *p_cookie, int pid)
{

    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint8_t  i   = 0;
    uint16_t reg = 0;
    uint8_t  ch  = 0;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid所对应的通道 */
    for (i = 0; i < p_dev_info->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_dev_info->pnum) {
        return AW_ERROR;
    }

    /* 配置引脚复用为PWM输出 */
    aw_gpio_pin_cfg(p_devcfg[i].gpio, p_devcfg[i].func);

    /* 清空计数器 */
    writew(0x00, &p_hw_pwm->CHANNEL[ch].CNTR);

    /* Enable Timer Channel */
    reg = readw(&p_hw_pwm->CHANNEL[ch].ENBL);
    reg |= (1 << ch);
    writew(reg, &p_hw_pwm->CHANNEL[ch].ENBL);

    /* Start the timer */
    reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
    reg &= ~(__TMR_CTRL_CM_MASK);
    reg |= __TMR_CTRL_CM(1);       /* Count rising edges or primary source */
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    return AW_OK;
}


/* PWM禁能 */
aw_local aw_err_t __imx10xx_pwm_disable (void *p_cookie, int pid)
{
    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint8_t  i   = 0;
    uint16_t reg = 0;
    uint8_t  ch  = 0;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid所对应的通道 */
    for (i = 0; i < p_dev_info->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_dev_info->pnum) {
        return AW_ERROR;
    }

    /* 配置引脚为默认引脚模式 */
    aw_gpio_pin_cfg(p_devcfg[i].gpio, p_devcfg[i].dfunc);

    /* Stop the timer */
    reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
    reg &= ~(__TMR_CTRL_CM_MASK);
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    /* Disable Timer Channel */
    reg = readw(&p_hw_pwm->CHANNEL[ch].ENBL);
    reg &= ~(1 << ch);
    writew(reg, &p_hw_pwm->CHANNEL[ch].ENBL);

    return AW_OK;
}


aw_local aw_err_t __imx10xx_pwm_output(
        void *p_cookie, int pid, uint32_t period_num)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint8_t  i   = 0;
    uint16_t reg = 0;
    uint8_t  ch  = 0;
    aw_err_t ret = AW_OK;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid所对应的通道 */
    for (i = 0; i < p_dev_info->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_dev_info->pnum) {
        return AW_ERROR;
    }

    /* 配置引脚为默认引脚模式 */
    aw_gpio_pin_cfg(p_devcfg[i].gpio, p_devcfg[i].func);

    /* 清除中断标志位 */
    reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
    reg &= ~(1 << 15);
    writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

    /* 打开比较中断 */
    reg = readw(&p_hw_pwm->CHANNEL[ch].SCTRL);
    reg |= (1 << 14);
    writew(reg, &p_hw_pwm->CHANNEL[ch].SCTRL);

    p_this->channel[ch].period_num = period_num * 2;

    /* Set the load okay bit for submodules to load registers from their buffer */
    /* Enable Timer Channel */
    reg = readw(&p_hw_pwm->CHANNEL[ch].ENBL);
    reg |= (1 << ch);
    writew(reg, &p_hw_pwm->CHANNEL[ch].ENBL);

    /* Start the timer */
    reg = readw(&p_hw_pwm->CHANNEL[ch].CTRL);
    reg &= ~(__TMR_CTRL_CM_MASK);
    reg |= __TMR_CTRL_CM(1);       /* Count rising edges or primary source */
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    ret = AW_SEMB_TAKE(p_this->channel[ch].sync_sem, AW_WAIT_FOREVER);

    aw_gpio_pin_cfg(p_devcfg[i].gpio, p_devcfg[i].dfunc);
    return ret;
}


/* 硬件初始化 */
aw_local void __imx10xx_pwm_hw_init(imx10xx_qtimer_pwm_regs_t *p_hw_pwm,
                                    uint8_t                    ch)
{
    uint16_t reg = 0;

    reg = __TMR_CTRL_PCS(__CLK_DIV_1) |  /* IP bus clock divide by 1 prescaler */
          __TMR_CTRL_SCS(0);             /* Secondary Count Source is Counter 0 input pin */
    writew(reg, &p_hw_pwm->CHANNEL[ch].CTRL);

    reg = (__TMR_FILT_FILT_CNT(0) | __TMR_FILT_FILT_PER(0));
    writew(reg, &p_hw_pwm->CHANNEL[ch].FILT);
}


/** \brief first level initialization routine */
aw_local void __pwm_inst_init1 (awbl_dev_t *p_dev)
{
    __PWM_DEV_DECL(p_this, p_dev);
    __PWM_DEVINFO_DECL(p_dev_info, p_dev);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);

    int i = 0;

    /* 平台初始化 */
    if (p_dev_info->pfunc_plfm_init) {
        p_dev_info->pfunc_plfm_init();
    }

    /* 初始化硬件 */
    for (i = 0; i < p_dev_info->pnum; i++) {
        __imx10xx_pwm_hw_init(p_hw_pwm, p_dev_info->pwm_devcfg[i].ch);
        AW_SEMB_INIT(p_this->channel[p_dev_info->pwm_devcfg[i].ch].sync_sem,
                     0,
                     AW_SEM_Q_PRIORITY);
    }

    aw_int_connect(p_dev_info->inum, __imx10xx_qtimer_pwm_isr, p_this);
    aw_int_enable(p_dev_info->inum);
}

/** \brief second level initialization routine */
aw_local void __pwm_inst_init2 (awbl_dev_t *p_dev)
{

}

/** \brief third level initialization routine */
aw_local void __pwm_inst_connect(awbl_dev_t *p_dev)
{

}

/*******************************************************************************
    pwm driver functions
*******************************************************************************/
aw_local aw_const struct awbl_pwm_servopts __g_imx10xx_pwm_servopts = {
        __imx10xx_pwm_config,      /* pfunc_pwm_config */
        __imx10xx_pwm_enable,      /* pfunc_pwm_enable */
        __imx10xx_pwm_disable,     /* pfunc_pwm_disable */
        __imx10xx_pwm_output       /* pfunc_pwm_output */
};

aw_local aw_const struct awbl_drvfuncs __g_qtimer_pwm_drvfuncs = {
        __pwm_inst_init1,
        __pwm_inst_init2,
        __pwm_inst_connect
};

/** \brief method "awbl_pwm_func_get" handler */
aw_local aw_err_t __imx10xx_pwmserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_imx10xx_qtimer_pwm_dev_t  *p_this  =  __PWM_DEV_TO_THIS(p_dev);
    awbl_imx10xx_qtimer_pwm_devinfo_t *p_devinfo = __PWM_DEV_TO_PARAM(p_dev);

    struct awbl_pwm_service *p_serv = NULL;

    /* 获取PWM服务实例 */
    p_serv = &p_this->pwm_serv;

    /* 初始化PWM服务实例 */
    p_serv->p_next = NULL;
    p_serv->p_servinfo = &p_devinfo->pwm_servinfo;
    p_serv->p_servfuncs = &__g_imx10xx_pwm_servopts;
    p_serv->p_cookie = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_pwm_service **)p_arg = p_serv;

    return AW_OK;
}

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_pwmserv_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx10xx_qtimer_pwm_methods[] = {
    AWBL_METHOD(awbl_pwmserv_get, __imx10xx_pwmserv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_qtimer_pwm_drv_registration = {
    {
        AWBL_VER_1,                          /* awb_ver */
        AWBL_BUSID_PLB,                      /* bus_id */
        AWBL_IMX10XX_QTIMER_PWM_NAME,        /* p_drvname */
        &__g_qtimer_pwm_drvfuncs,            /* p_busfuncs */
        &__g_imx10xx_qtimer_pwm_methods[0],  /* p_methods */
        NULL                                 /* pfunc_drv_probe */
    }
};

void awbl_imx10xx_qtimer_pwm_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_qtimer_pwm_drv_registration);
}

/* end of file */

