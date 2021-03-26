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
 * \brief iMX RT10xx GPT PWM驱动
 *
 * 本驱动实现了GPT定时器的捕获功能。
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-09  licl, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/
#include "apollo.h"
#include "aw_bitops.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include "driver/pwm/awbl_imx10xx_gpt_pwm.h"

/*******************************************************************************
  宏定义
*******************************************************************************/
#define __CTRL_EN           0
#define __CTRL_ENMOD        1
#define __CTRL_WAITEN       3
#define __CTRL_STOPEN       5
#define __CTRL_CLKSRC       6
#define __CTRL_CLKSRC_LEN   3
#define __CTRL_CLKSRC_PCLK  1
#define __CTRL_FRR          9

#define __CTRL_OM1          20
#define __CTRL_OM_LEN       3
#define __CTRL_OM_DIS       0
#define __CTRL_OM_TOG       1
#define __CTRL_OM_CLR       2
#define __CTRL_OM_SET       3
#define __CTRL_OM_LOW_PULSE 4
#define __STAT_OF1          0
#define __INT_OF1           0

/* declare PWM device instance */
#define __PWM_DEV_DECL(p_this, p_dev)           \
    struct awbl_imx10xx_gpt_pwm_dev *p_this =   \
        (struct awbl_imx10xx_gpt_pwm_dev *)(p_dev)

/* declare PWM device infomation */
#define __PWM_DEVINFO_DECL(p_devinfo, p_dev)            \
    struct awbl_imx10xx_gpt_pwm_devinfo *p_devinfo =    \
        (struct awbl_imx10xx_gpt_pwm_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_PARAM(p_dev) \
    (awbl_imx10xx_gpt_pwm_devinfo_t *)AWBL_DEVINFO_GET(p_dev)

#define __PWM_DEV_TO_THIS(p_dev) \
    (awbl_imx10xx_gpt_pwm_dev_t *)(p_dev)

/* get iMX10xx PWM hardware */
#define __PWM_HW_DECL(p_hw_pwm, p_devinfo)          \
    imx10xx_gpt_regs_t *(p_hw_pwm) =                \
        ((imx10xx_gpt_regs_t *)((p_devinfo)->regbase))

/* get pwm device config */
#define __PWM_DEV_CONFIG(p_devcfg, p_devinfo)           \
    struct awbl_imx10xx_gpt_pwm_devcfg *p_devcfg =      \
        (struct awbl_imx10xx_gpt_pwm_devcfg *)p_devinfo->pwm_devcfg;

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 定时器中断处理函数 */
aw_local void __imx10xx_pwm_isr (void *p_arg)
{
    __PWM_DEV_DECL(p_this, p_arg);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_pwm, p_devinfo);

    uint32_t     reg  = 0;
    volatile int i    = 0;

    for (i = 0; i < p_devinfo->pnum; i++) {
        reg = AW_REG_READ32(&p_hw_pwm->sr);
        if (reg & (1 << (__STAT_OF1 + i))) {

            /* 清除状态 */
            AW_REG_WRITE32(&p_hw_pwm->sr, reg);
            if (--p_this->channel[i].period_num == 0) {

                /* 禁用中断 */
                AW_REG_BIT_CLR32(&p_hw_pwm->ir, __INT_OF1 + i);
                AW_REG_BITS_SET32(&p_hw_pwm->cr,
                                  __CTRL_OM1 + i * __CTRL_OM_LEN,
                                  __CTRL_OM_LEN,
                                  __CTRL_OM_DIS);
                AW_SEMB_GIVE(p_this->channel[i].sync_sem);
            }
        }
    }
}

aw_local void __imx10xx_gpt_hard_init (awbl_imx10xx_gpt_pwm_dev_t *p_this)
{
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_cap, p_devinfo);

    /* 使能GPT时钟 */
    aw_clk_enable(p_devinfo->ref_clk_id);

    /* 首先要禁用此定时器 */
    AW_REG_WRITE32(&p_hw_cap->cr, 0x0);

    /* 禁能中断 */
    AW_REG_WRITE32(&p_hw_cap->ir, 0x0);

    /* 设置预分频 */
    AW_REG_WRITE32(&p_hw_cap->pr, 0x0);

    /* 清空计数值 */
    AW_REG_BIT_SET32(&p_hw_cap->cr, __CTRL_ENMOD);

    /* 选择ipg_clk为源时钟 */
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_CLKSRC, __CTRL_CLKSRC_LEN, __CTRL_CLKSRC_PCLK);

    /* 不输出到引脚 */
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_OM1, __CTRL_OM_LEN, __CTRL_OM_DIS);

    /* 停止和等待状态下GPT是否使能 */
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_STOPEN);
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_WAITEN);

    /* 设置重新加载模式 */
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_FRR);
}

aw_local aw_err_t __imx10xx_pwm_config (void          *p_cookie,
                                        int            pid,
                                        unsigned long  duty_ns, /* 此参数无效 */
                                        unsigned long  period_ns)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_pwm, p_devinfo);
    __PWM_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  i, ch;
    uint32_t count;
    uint32_t clkfreq;

    if ((pid < p_devinfo->pwm_servinfo.pwm_min_id) ||
        (pid > p_devinfo->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid对应的PWM通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    clkfreq = aw_clk_rate_get(p_devinfo->ref_clk_id);
    count = (uint64_t)(period_ns / 2) * (uint64_t)clkfreq/ (uint64_t)1000000000;

    /* 设置比较值 */
    AW_REG_WRITE32(&p_hw_pwm->ocr[ch], count);

    return AW_OK;
}

aw_local aw_err_t __imx10xx_pwm_enable(void *p_cookie, int pid)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_pwm, p_devinfo);
    __PWM_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;

    if ((pid < p_devinfo->pwm_servinfo.pwm_min_id) ||
        (pid > p_devinfo->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid对应的PWM通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 配置输出模式 */
    AW_REG_BITS_SET32(&p_hw_pwm->cr,
                      __CTRL_OM1 + ch * __CTRL_OM_LEN,
                      __CTRL_OM_LEN,
                      __CTRL_OM_TOG);

    /* 启用中断 */
    AW_REG_BIT_SET32(&p_hw_pwm->ir, __INT_OF1 + ch);

    /* 清除标志(写1清0) */
    AW_REG_BIT_SET32(&p_hw_pwm->sr, __STAT_OF1 + ch);

    /* 使能 */
    AW_REG_BIT_SET32(&p_hw_pwm->cr, __CTRL_EN);

    return AW_OK;
}

aw_local aw_err_t __imx10xx_pwm_disable(void *p_cookie, int pid)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_pwm, p_devinfo);
    __PWM_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;

    if ((pid < p_devinfo->pwm_servinfo.pwm_min_id) ||
        (pid > p_devinfo->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid对应的PWM通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* 禁用中断 */
    AW_REG_BIT_CLR32(&p_hw_pwm->ir, __INT_OF1 + ch);

    /* 配置输出模式 */
    AW_REG_BITS_SET32(&p_hw_pwm->cr,
                      __CTRL_OM1 + ch * __CTRL_OM_LEN,
                      __CTRL_OM_LEN,
                      __CTRL_OM_DIS);

    if ((AW_REG_READ32(&p_hw_pwm->ir) & 0x7) == 0) {
        AW_REG_BIT_CLR32(&p_hw_pwm->cr, __CTRL_EN);
    }

    return AW_OK;
}

aw_local aw_err_t __imx10xx_pwm_output(
        void *p_cookie, int pid, uint32_t period_num)
{
    __PWM_DEV_DECL(p_this, p_cookie);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);
    __PWM_HW_DECL(p_hw_pwm, p_devinfo);
    __PWM_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  i, ch;
    aw_err_t ret = AW_OK;

    if ((pid < p_devinfo->pwm_servinfo.pwm_min_id) ||
        (pid > p_devinfo->pwm_servinfo.pwm_max_id)) {
        return -AW_EINVAL;
    }

    /* 查找pid对应的PWM通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    p_this->channel[ch].period_num = period_num * 2;

    /* 配置输出模式 */
    AW_REG_BITS_SET32(&p_hw_pwm->cr,
                      __CTRL_OM1 + ch * __CTRL_OM_LEN,
                      __CTRL_OM_LEN,
                      __CTRL_OM_TOG);

    /* 启用中断 */
    AW_REG_BIT_SET32(&p_hw_pwm->ir, __INT_OF1 + ch);

    /* 清除标志(写1清0) */
    AW_REG_BIT_SET32(&p_hw_pwm->sr, __STAT_OF1 + ch);

    /* 使能 */
    AW_REG_BIT_SET32(&p_hw_pwm->cr, __CTRL_EN);

    ret = AW_SEMB_TAKE(p_this->channel[ch].sync_sem, AW_WAIT_FOREVER);
    return ret;
}

/** \brief 第一阶段初始化 */
aw_local void __pwm_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}

/** \brief 第二阶段初始化 */
aw_local void __pwm_inst_init2 (struct awbl_dev *p_dev)
{
    __PWM_DEV_DECL(p_this, p_dev);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);

    int     i  = 0;

    /* 调用平台初始化函数 */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    __imx10xx_gpt_hard_init(p_this);

    /* 初始化使用到的通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        AW_SEMB_INIT(p_this->channel[p_devinfo->pwm_devcfg[i].ch].sync_sem,
                     0,
                     AW_SEM_Q_PRIORITY);
    }

    aw_int_connect(p_devinfo->inum, __imx10xx_pwm_isr, p_this);
    aw_int_enable(p_devinfo->inum);
}

/** \brief 第三阶段初始化 */
aw_local void __pwm_inst_connect (struct awbl_dev *p_dev)
{
    return;
}


/*******************************************************************************
    pwm driver functions
*******************************************************************************/
aw_local aw_const struct awbl_pwm_servopts __g_imx10xx_pwm_servopts = {
    __imx10xx_pwm_config,         /* pfunc_pwm_config */
    __imx10xx_pwm_enable,         /* pfunc_pwm_enable */
    __imx10xx_pwm_disable,        /* pfunc_pwm_disable */
    __imx10xx_pwm_output,          /* pfunc_pwm_reset */
};

aw_local aw_const struct awbl_drvfuncs __g_pwm_std_drvfuncs = {
    __pwm_inst_init1,
    __pwm_inst_init2,
    __pwm_inst_connect
};

/** \brief method "awbl_pwm_func_get" handler */
aw_local aw_err_t __imx10xx_pwm_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __PWM_DEV_DECL(p_this, p_dev);
    __PWM_DEVINFO_DECL(p_devinfo, p_this);

    struct awbl_pwm_service *p_serv = NULL;

     /* 获取pwm服务实例 */
    p_serv = &p_this->pwm_serv;

    /* 初始化pwm服务实例 */
    p_serv->p_next = NULL;
    p_serv->p_servinfo = &p_devinfo->pwm_servinfo;
    p_serv->p_servfuncs = &__g_imx10xx_pwm_servopts;
    p_serv->p_cookie = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_pwm_service **)p_arg = p_serv;

     return AW_OK;
}

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_pwmserv_get);

/* methods for this driver */
aw_local aw_const struct awbl_dev_method __g_imx10xx_pwm_methods[] = {
    AWBL_METHOD(awbl_pwmserv_get, __imx10xx_pwm_serv_get),
    AWBL_METHOD_END
};


/** 本驱动提供了一种方法，该方法用于得到pwm服务（服务体现在3个函数指针上） */
aw_local aw_const awbl_plb_drvinfo_t __g_pwm_drv_registration = {
    {
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_PLB,                 /* bus_id */
        AWBL_IMX10XX_GPT_PWM_NAME,  /* p_drvname */
        &__g_pwm_std_drvfuncs,          /* p_busfuncs */
        &__g_imx10xx_pwm_methods[0],    /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 IMX10xx GPT PWM 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_gpt_pwm_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_pwm_drv_registration);
}

/* end of file */
