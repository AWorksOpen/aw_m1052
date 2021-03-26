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
 * \brief Freescale i.MXRT10xx eFlexPWMÍâÉèÇý¶¯
 *
 * \internal
 * \par modification history:
 * - 1.00 18-01-09  mex, first implemetation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "aw_system.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_sem.h"
#include "aw_int.h"
#include "driver/pwm/awbl_imx10xx_eflex_pwm.h"
#include "driver/pwm/imx10xx_eflex_pwm_regs.h"

/*******************************************************************************
  macro operate
*******************************************************************************/

/* declare PWM device instance */
#define __PWM_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_eflex_pwm_dev *p_this = \
        (struct awbl_imx10xx_eflex_pwm_dev *)(p_dev)

/* declare PWM device infomation */
#define __PWM_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_eflex_pwm_devinfo *p_devinfo = \
        (struct awbl_imx10xx_eflex_pwm_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_PARAM(p_dev) \
    (awbl_imx10xx_eflex_pwm_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_THIS(p_dev) \
    (awbl_imx10xx_eflex_pwm_dev_t *)(p_dev)

/* get iMX10xx PWM hardware */
#define __EFLEX_PWM_HW_DECL(p_hw_pwm, p_devinfo)       \
        imx10xx_eflex_pwm_regs_t *(p_hw_pwm) =         \
        ((imx10xx_eflex_pwm_regs_t *)((p_devinfo)->regbase))

/* get pwm device config */
#define __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_devinfo)        \
        struct awbl_imx10xx_eflex_pwm_devcfg  *p_devcfg =  \
            (struct awbl_imx10xx_eflex_pwm_devcfg *)p_dev_info->pwm_devcfg;

#define __IMX10XX_EFLEXPWM_ISR_DECL(index) \
    aw_local void __imx10xx_eflexpwm_isr##index(void *p_arg) \
    { \
        __imx10xx_eflexpwm_gen_isr(p_arg, index); \
    }

#define __IMX10XX_EFLEXPWM_ISR(index)    __imx10xx_eflexpwm_isr##index

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
aw_local void __imx10xx_eflexpwm_gen_isr(void *p_arg, uint32_t index)
{
    __PWM_DEV_DECL(p_this, p_arg);
    __PWM_DEVINFO_DECL(p_dev_info, p_arg);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);

    uint16_t                                        reg;
    uint8_t                                         submodle;

    submodle = p_dev_info->pwm_devcfg[index].sub_modedle;

    reg = readw(&p_hw_pwm->SM[submodle].STS);
    if (reg & (1 << 0)) {
        reg |= 1 << 0;
        writew(reg, &p_hw_pwm->SM[submodle].STS);

        if (--p_this->channel[submodle].period_num == 0) {
            reg = readw(&p_hw_pwm->MCTRL);
            reg &= ~(__PWM_MCTRL_RUN(1 << submodle));
            writew(reg, &p_hw_pwm->MCTRL);

            reg = readw(&p_hw_pwm->SM[submodle].INTEN);
            reg &= ~(1 << 1);
            writew(reg, &p_hw_pwm->SM[submodle].INTEN);

            AW_SEMB_GIVE(p_this->channel[submodle].sync_sem);
        }
    }
}

/* ÖÐ¶Ï·þÎñº¯Êý¶¨Òå */
__IMX10XX_EFLEXPWM_ISR_DECL(0)
__IMX10XX_EFLEXPWM_ISR_DECL(1)
__IMX10XX_EFLEXPWM_ISR_DECL(2)
__IMX10XX_EFLEXPWM_ISR_DECL(3)


/* PWMÅäÖÃ */
aw_local aw_err_t __imx10xx_pwm_config (void          *p_cookie,
                                        int            pid,
                                        unsigned long  duty_ns,
                                        unsigned long  period_ns)
{

    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint16_t reg = 0;
    uint32_t clk_div = 1;
    uint32_t clkfreq = 0;
    uint32_t period_c, low_c;
    int16_t  modulo = 0;
    uint8_t polarityShift = 0;
    uint8_t outputEnableShift = 0;
    uint8_t index = 0;
    uint8_t channels = 0;
    uint8_t mode     = 0;
    uint8_t submodle = 0;
    uint8_t pid_num  = p_dev_info->pnum;
    uint8_t shift = 0;
    uint64_t c = 0;

    if ((period_ns == 0) || (duty_ns > 4294967295UL) ||
         (period_ns > 4294967295UL) || (duty_ns > period_ns)) {
         return -AW_EINVAL;
    }

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* ²éÕÒpidÊÇ·ñÔÚÅäÖÃÐÅÏ¢ÖÐ */
    for (index = 0; index < pid_num; index++) {
        if (pid == p_devcfg[index].pid) {
            break;
        }
    }

    if (index >= pid_num) {
        return AW_ERROR;
    }

    channels = p_dev_info->pwm_devcfg[index].pwm_channel;
    mode     = p_dev_info->pwm_devcfg[index].pwm_mode;
    submodle = p_dev_info->pwm_devcfg[index].sub_modedle;

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

    low_c = (uint64_t)(duty_ns)   * (clkfreq) / (uint64_t)1000000000;

    /* ÉèÖÃÔ¤·ÖÆµ */
    reg = readw(&p_hw_pwm->SM[submodle].CTRL);
    reg &= ~__PWM_CTRL_PRSC_MASK;
    reg |=  __PWM_CTRL_PRSC(shift);
    writew(reg, &p_hw_pwm->SM[submodle].CTRL);

    switch (mode) {

    case kPWM_SignedCenterAligned:

        /* Setup the PWM period for a signed center aligned signal */
        modulo =  period_c >> 1;

        /* Indicates the start of the PWM period */
        writew((-modulo), &p_hw_pwm->SM[submodle].INIT);

        /* Indicates the center value */
        writew(0, &p_hw_pwm->SM[submodle].VAL0);

        /* Indicates the end of the PWM period */
        writew(modulo, &p_hw_pwm->SM[submodle].VAL1);

        /* Setup the PWM dutycycle */
        if (channels == kPWM_PwmA) {
            writew((-(low_c / 2)), &p_hw_pwm->SM[submodle].VAL2);
            writew((low_c / 2), &p_hw_pwm->SM[submodle].VAL3);
        } else {
            writew((-(low_c / 2)), &p_hw_pwm->SM[submodle].VAL4);
            writew((low_c / 2), &p_hw_pwm->SM[submodle].VAL5);
        }
        break;

    case kPWM_CenterAligned:

        /* Indicates the start of the PWM period */
        writew(0, &p_hw_pwm->SM[submodle].INIT);

        /* Indicates the center value */
        writew((period_c / 2), &p_hw_pwm->SM[submodle].VAL0);

        /* Indicates the end of the PWM period */
        writew(period_c, &p_hw_pwm->SM[submodle].VAL1);

        /* Setup the PWM dutycycle */
        if (channels == kPWM_PwmA) {
            writew(((period_c - low_c) / 2), &p_hw_pwm->SM[submodle].VAL2);
            writew(((period_c + low_c) / 2), &p_hw_pwm->SM[submodle].VAL3);
        } else {
            writew(((period_c - low_c) / 2), &p_hw_pwm->SM[submodle].VAL4);
            writew(((period_c + low_c) / 2), &p_hw_pwm->SM[submodle].VAL5);
        }

        break;

    case kPWM_SignedEdgeAligned:

        /* Setup the PWM period for a signed edge aligned signal */
        modulo = period_c >> 1;

        /* Indicates the start of the PWM period */
        writew((-modulo), &p_hw_pwm->SM[submodle].INIT);

        /* Indicates the center value */
        writew(0, &p_hw_pwm->SM[submodle].VAL0);

        /* Indicates the end of the PWM period */
        writew(modulo, &p_hw_pwm->SM[submodle].VAL1);

        /* Setup the PWM dutycycle */
        if (channels == kPWM_PwmA) {
            p_hw_pwm->SM[submodle].VAL2 = (-modulo);
            p_hw_pwm->SM[submodle].VAL3 = (-modulo + low_c);
        } else {
            p_hw_pwm->SM[submodle].VAL4 = (-modulo);
            p_hw_pwm->SM[submodle].VAL5 = (-modulo + low_c);
        }
        break;

    case kPWM_EdgeAligned:

        /* Indicates the start of the PWM period */
        p_hw_pwm->SM[submodle].INIT = 0;

        /* Indicates the center value */
        p_hw_pwm->SM[submodle].VAL0 = (period_c / 2);

        /* Indicates the end of the PWM period */
        p_hw_pwm->SM[submodle].VAL1 = period_c;

        /* Setup the PWM dutycycle */
        if (channels == kPWM_PwmA) {
            writew(0, &p_hw_pwm->SM[submodle].VAL2);
            writew(low_c, &p_hw_pwm->SM[submodle].VAL3);
        } else {
            writew(0, &p_hw_pwm->SM[submodle].VAL4);
            writew(low_c, &p_hw_pwm->SM[submodle].VAL5);
        }
        break;
    default:
        break;
    }

    if (channels == kPWM_PwmA) {

        polarityShift = __PWM_OCTRL_POLA_SHIFT;
        outputEnableShift = __PWM_OUTEN_PWMA_EN_SHIFT;
        writew(__PWM_DTCNT0(0xff), &p_hw_pwm->SM[submodle].DTCNT0);
    } else {

        polarityShift = __PWM_OCTRL_POLB_SHIFT;
        outputEnableShift = __PWM_OUTEN_PWMB_EN_SHIFT;
        writew(__PWM_DTCNT0(0xff), &p_hw_pwm->SM[submodle].DTCNT1);
    }

    /* Setup signal active level */
    /* High level represents "on" or "active" state */
    reg = readw(&p_hw_pwm->SM[submodle].OCTRL);
    reg &= ~(1U << polarityShift);
    writew(reg, &p_hw_pwm->SM[submodle].OCTRL);

    /* Enable PWM output */
    reg = readw(&p_hw_pwm->OUTEN);
    reg |= (1U << (outputEnableShift + submodle));
    writew(reg, &p_hw_pwm->OUTEN);

    return AW_OK;
}


/* PWMÊ¹ÄÜ */
aw_local aw_err_t __imx10xx_pwm_enable (void *p_cookie, int pid)
{

    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    uint8_t  submodle = 0;
    uint8_t  index    = 0;
    uint16_t reg      = 0;
    uint8_t  pid_num  = p_dev_info->pnum;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* ²éÕÒpidÊÇ·ñÔÚÅäÖÃÐÅÏ¢ÖÐ */
    for (index = 0; index < pid_num; index++) {
        if (pid == p_devcfg[index].pid) {
            break;
        }
    }

    if (index >= pid_num) {
        return AW_ERROR;
    }

    submodle = p_dev_info->pwm_devcfg[index].sub_modedle;

    /* ÅäÖÃÒý½Å¸´ÓÃÎªPWMÊä³ö */
    aw_gpio_pin_cfg(p_devcfg[index].gpio, p_devcfg[index].func);

    /* Set the load okay bit for submodules to load registers from their buffer */
    reg = readw(&p_hw_pwm->MCTRL);
    reg |= __PWM_MCTRL_LDOK(1 << submodle);
    writew(reg, &p_hw_pwm->MCTRL);

    /* Starts the PWM counter */
    reg = readw(&p_hw_pwm->MCTRL);
    reg |= __PWM_MCTRL_RUN(1 << submodle);
    writew(reg, &p_hw_pwm->MCTRL);

    return AW_OK;
}


/* PWM½ûÄÜ */
aw_local aw_err_t __imx10xx_pwm_disable (void *p_cookie, int pid)
{
    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);

    uint16_t reg    = 0;
    uint8_t  index  = 0;
    uint8_t  submodle = 0;
    uint8_t  pid_num  = p_dev_info->pnum;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* ²éÕÒpidÊÇ·ñÔÚÅäÖÃÐÅÏ¢ÖÐ */
    for (index = 0; index < pid_num; index++) {
        if (pid == p_devcfg[index].pid) {
            break;
        }
    }

    if (index >= pid_num) {
        return AW_ERROR;
    }

    /* ÅäÖÃÒý½ÅÎªÄ¬ÈÏÒý½ÅÄ£Ê½ */
    aw_gpio_pin_cfg(p_devcfg[index].gpio, p_devcfg[index].dfunc);

    submodle = p_dev_info->pwm_devcfg[index].sub_modedle;

    reg = readw(&p_hw_pwm->MCTRL);
    reg &= ~(__PWM_MCTRL_RUN(1 << submodle));
    writew(reg, &p_hw_pwm->MCTRL);
    return AW_OK;
}


aw_local aw_err_t __imx10xx_pwm_output(
        void *p_cookie, int pid, uint32_t period_num)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_dev_info, p_cookie);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);
    __EFLEX_PWM_DEV_CONFIG(p_devcfg, p_dev_info);

    aw_err_t ret = AW_OK;
    uint8_t  index  = 0;
    uint8_t  submodle = 0;
    uint16_t reg      = 0;

    if ((pid < p_dev_info->pwm_servinfo.pwm_min_id) ||
        (pid > p_dev_info->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* ²éÕÒpidÊÇ·ñÔÚÅäÖÃÐÅÏ¢ÖÐ */
    for (index = 0; index < p_dev_info->pnum; index++) {
        if (pid == p_devcfg[index].pid) {
            break;
        }
    }

    if (index >= p_dev_info->pnum) {
        return AW_ERROR;
    }

    submodle = p_dev_info->pwm_devcfg[index].sub_modedle;

    if (period_num == 0) {
        return -AW_EINVAL;
    }

    aw_gpio_pin_cfg(p_devcfg[index].gpio, p_devcfg[index].func);

    reg = readw(&p_hw_pwm->SM[submodle].STS);
    reg |= 1 << 0;
    writew(reg, &p_hw_pwm->SM[submodle].STS);

    reg = readw(&p_hw_pwm->SM[submodle].INTEN);
    reg |= 1 << 0;
    writew(reg, &p_hw_pwm->SM[submodle].INTEN);

    p_this->channel[submodle].period_num = period_num;

    /* Set the load okay bit for submodules to load registers from their buffer */
    reg = readw(&p_hw_pwm->MCTRL);
    reg |= __PWM_MCTRL_LDOK(1 << submodle);
    writew(reg, &p_hw_pwm->MCTRL);

    /* Starts the PWM counter */
    reg = readw(&p_hw_pwm->MCTRL);
    reg |= __PWM_MCTRL_RUN(1 << submodle);
    writew(reg, &p_hw_pwm->MCTRL);

    ret = AW_SEMB_TAKE(p_this->channel[submodle].sync_sem, AW_WAIT_FOREVER);

    aw_gpio_pin_cfg(p_devcfg[index].gpio, p_devcfg[index].dfunc);

    return ret;
}


/* Ó²¼þ³õÊ¼»¯ */
aw_local void __imx10xx_pwm_hw_init(imx10xx_eflex_pwm_regs_t *p_hw_pwm,
                                    uint8_t                  submodule)
{
    uint16_t reg = 0;

    /* Clear the fault status flags */
    reg = readw(&p_hw_pwm->FSTS);
    reg |= __PWM_FSTS_FFLAG_MASK;
    writew(reg, &p_hw_pwm->FSTS);

    reg = __PWM_CTRL2_CLK_SEL(0)   |   /* Use IPBus clock */
          __PWM_CTRL2_FORCE_SEL(0) |   /* local force signal */
          __PWM_CTRL2_INIT_SEL(0)  |   /* Local sync (PWM_X) causes initialization */
          __PWM_CTRL2_DBGEN_MASK;      /* Enable debug mode */

    /* Setup PWM A & PWM B operate as 2 independent channels */
    reg |=  __PWM_CTRL2_INDEP_MASK;

    writew(reg, &p_hw_pwm->SM[submodule].CTRL2);

    /* Setup register reload logic */
    reg = readw(&p_hw_pwm->SM[submodule].CTRL);

    reg &= ~(__PWM_CTRL_PRSC_MASK | __PWM_CTRL_LDFQ_MASK | __PWM_CTRL_LDMOD_MASK);

    reg |=  __PWM_CTRL_PRSC(0) |    /* PWM clock frequency = fclk / 1 */
            __PWM_CTRL_LDFQ(0) |    /* Every PWM opportunity */
            __PWM_CTRL_LDMOD_MASK;  /* uffered-registers get loaded with new values as soon as LDOK bit is set */

    reg &= ~__PWM_CTRL_HALF_MASK;
    reg |= __PWM_CTRL_FULL_MASK;

    writew(reg, &p_hw_pwm->SM[submodule].CTRL);

    /* Setup the fault filter */
    reg = readw(&p_hw_pwm->FFILT);

    if (reg & __PWM_FFILT_FILT_PER_MASK) {

        /* When changing values for fault period from a non-zero value, first write a value of 0
         * to clear the filter
         */
        reg &= ~(__PWM_FFILT_FILT_PER_MASK);
        writew(reg, &p_hw_pwm->FFILT);
    }

    /* Issue a Force trigger event when configured to trigger locally */
    p_hw_pwm->SM[submodule].CTRL2 |= __PWM_CTRL2_FORCE_MASK;

    /* �ع����ж� */
    p_hw_pwm->SM[submodule].DISMAP[0] = 0;
    p_hw_pwm->SM[submodule].DISMAP[1] = 0;
}


/** \brief first level initialization routine */
aw_local void __pwm_inst_init1 (awbl_dev_t *p_dev)
{
    __PWM_DEV_DECL(p_this, p_dev);
    __PWM_DEVINFO_DECL(p_dev_info, p_dev);
    __EFLEX_PWM_HW_DECL(p_hw_pwm, p_dev_info);

    int i = 0;
    int sub_index = -1;     /* ×ÓÄ£¿éË÷Òý */

    /* Æ½Ì¨³õÊ¼»¯ */
    if (p_dev_info->pfunc_plfm_init) {
        p_dev_info->pfunc_plfm_init();
    }

    aw_pfuncvoid_t p_func;

    /* ³õÊ¼»¯Ó²¼þ */
    for (i = 0; i < p_dev_info->pnum; i++) {
        if (p_dev_info->pwm_devcfg[i].sub_modedle != sub_index) {
            sub_index = p_dev_info->pwm_devcfg[i].sub_modedle;
            __imx10xx_pwm_hw_init(p_hw_pwm, sub_index);
        }

        switch (sub_index) {
        case kPWM_Module_0:
            p_func = __IMX10XX_EFLEXPWM_ISR(0);
            break;
        case kPWM_Module_1:
            p_func = __IMX10XX_EFLEXPWM_ISR(1);
            break;
        case kPWM_Module_2:
            p_func = __IMX10XX_EFLEXPWM_ISR(2);
            break;
        case kPWM_Module_3:
            p_func = __IMX10XX_EFLEXPWM_ISR(3);
            break;
        }

        aw_int_connect(p_dev_info->pwm_devcfg[i].inum, p_func,  p_dev);
        aw_int_enable(p_dev_info->pwm_devcfg[i].inum);

        AW_SEMB_INIT(p_this->channel[i].sync_sem, 0, AW_SEM_Q_PRIORITY);
    }
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

aw_local aw_const struct awbl_drvfuncs __g_eflex_pwm_drvfuncs = {
        __pwm_inst_init1,
        __pwm_inst_init2,
        __pwm_inst_connect
};

/** \brief method "awbl_pwm_func_get" handler */
aw_local aw_err_t __imx10xx_pwmserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_imx10xx_eflex_pwm_dev_t  *p_this  =  __PWM_DEV_TO_THIS(p_dev);
    awbl_imx10xx_eflex_pwm_devinfo_t *p_devinfo = __PWM_DEV_TO_PARAM(p_dev);

    struct awbl_pwm_service *p_serv = NULL;

    /* »ñÈ¡PWM·þÎñÊµÀý */
    p_serv = &p_this->pwm_serv;

    /* ³õÊ¼»¯PWM·þÎñÊµÀý */
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
aw_local aw_const struct awbl_dev_method __g_imx10xx_eflex_pwm_methods[] = {
    AWBL_METHOD(awbl_pwmserv_get, __imx10xx_pwmserv_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_eflex_pwm_drv_registration = {
    {
        AWBL_VER_1,                          /* awb_ver */
        AWBL_BUSID_PLB,                      /* bus_id */
        AWBL_IMX10XX_EFLEX_PWM_NAME,         /* p_drvname */
        &__g_eflex_pwm_drvfuncs,             /* p_busfuncs */
        &__g_imx10xx_eflex_pwm_methods[0],   /* p_methods */
        NULL                                 /* pfunc_drv_probe */
    }
};

void awbl_imx10xx_eflex_pwm_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_eflex_pwm_drv_registration);
}

/* end of file */
