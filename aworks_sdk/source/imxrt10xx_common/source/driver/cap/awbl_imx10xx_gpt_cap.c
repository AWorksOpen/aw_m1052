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
 * \brief iMX RT10xx GPT CAP驱动
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
#include "driver/cap/awbl_imx10xx_gpt_cap.h"

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
#define __CTRL_IM1          16
#define __CTRL_IM_LEN       2
#define __CTRL_IM_RISE      1
#define __CTRL_IM_FALL      2
#define __CTRL_IM_BOTH      3
#define __CTRL_OM1          20
#define __CTRL_OM_LEN       3
#define __CTRL_OM_DIS       0
#define __STAT_IF1          3
#define __INT_IF1           3

/* declare CAP device instance */
#define __CAP_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_gpt_cap_dev *p_this = \
        (struct awbl_imx10xx_gpt_cap_dev *)(p_dev)

/* declare CAP device infomation */
#define __CAP_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_gpt_cap_devinfo *p_devinfo = \
        (struct awbl_imx10xx_gpt_cap_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get CAP hardware */
#define __CAP_HW_DECL(p_hw_cap, p_devinfo) \
        imx10xx_gpt_regs_t *(p_hw_cap) =  \
            ((imx10xx_gpt_regs_t *)((p_devinfo)->regbase))

/* get CAP device config */
#define __CAP_DEV_CONFIG(p_devcfg, p_devinfo) \
        struct awbl_imx10xx_gpt_cap_devcfg  *p_devcfg = \
            (struct awbl_imx10xx_gpt_cap_devcfg *)p_devinfo->cap_devcfg;

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 定时器中断处理函数 */
aw_local void __gpt_cap_isr (void *p_arg)
{
    __CAP_DEV_DECL(p_this, p_arg);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);

    uint8_t      ch   = 0;
    uint32_t     reg  = 0;
    uint32_t     vale = 0;
    volatile int i    = 0;

    for (i = 0; i < p_devinfo->pnum; i++) {
        ch = p_devinfo->cap_devcfg[i].ch;
        reg = AW_REG_READ32(&p_hw_cap->sr);
        if (reg & (1 << (__STAT_IF1 + i))) {

            /* 清除状态 */
            AW_REG_WRITE32(&p_hw_cap->sr, reg);

            if (p_this->callback_info[ch].callback_func != NULL) {
                vale = AW_REG_READ32(&p_hw_cap->icr[ch]);
                p_this->callback_info[ch].callback_func(
                        p_this->callback_info[ch].p_arg, vale);
            }
        }
    }
}

/**
 * \brief GPT硬件初始化
 */
aw_local void __imx10xx_gpt_hard_init (awbl_imx10xx_gpt_cap_dev_t *p_this,
                                       uint8_t                     channel)
{
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);

    /* 使能GPT时钟 */
    aw_clk_enable(p_devinfo->clk_id);

    /* 首先要禁用此定时器 */
    AW_REG_WRITE32(&p_hw_cap->cr, 0x0);

    /* 禁能中断 */
    AW_REG_WRITE32(&p_hw_cap->ir, 0x0);

    /* 设置预分频 */
    AW_REG_WRITE32(&p_hw_cap->pr, 0x0);

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

/**
 * \brief GPT CAP配置
 */
aw_local aw_err_t __imx10xx_cap_config(void           *p_cookie,
                                       int             pid,
                                       unsigned int    options,
                                       cap_callback_t  pfunc_callback,
                                       void           *p_arg)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t  i, ch;
    uint32_t reg = 0;

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

    /* 设置触发条件 */
    if ((options & AW_CAP_TRIGGER_RISE) == AW_CAP_TRIGGER_RISE) {
        reg = __CTRL_IM_RISE;
    } else if ((options & AW_CAP_TRIGGER_FALL) == AW_CAP_TRIGGER_FALL) {
        reg = __CTRL_IM_FALL;
    } else if ((options & AW_CAP_TRIGGER_BOTH_EDGES) == AW_CAP_TRIGGER_BOTH_EDGES) {
        reg = __CTRL_IM_BOTH;
    }
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_IM1 + ch * 2, __CTRL_IM_LEN, reg);

    /* 设置中断回掉函数 */
    p_this->callback_info[ch].callback_func = pfunc_callback;
    p_this->callback_info[ch].p_arg = p_arg;

    return AW_OK;
}

/**
 * \brief GPT CAP使能
 */
aw_local aw_err_t __imx10xx_cap_enable(void *p_cookie, int pid)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;

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

    /* 启用中断 */
    AW_REG_BIT_SET32(&p_hw_cap->ir, __INT_IF1 + ch);

    /* 设置计数最大值(即比较值) */
    AW_REG_WRITE32(&p_hw_cap->ocr[0], 0xffffffff);

    /* 清除标志(写1清0) */
    AW_REG_BIT_SET32(&p_hw_cap->sr, __STAT_IF1 + ch);

    /* 清空计数值 */
    AW_REG_BIT_SET32(&p_hw_cap->cr, __CTRL_ENMOD);

    /* 使能 */
    AW_REG_BIT_SET32(&p_hw_cap->cr, __CTRL_EN);

    return AW_OK;
}

/**
 * \brief GPT CAP禁能
 */
aw_local aw_err_t __imx10xx_cap_disable(void *p_cookie, int pid)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;

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

    /* 禁用中断 */
    AW_REG_BIT_CLR32(&p_hw_cap->ir, __INT_IF1 + ch);

    if (AW_REG_READ32(&p_hw_cap->ir) & (0x18) == 0) {
        AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_EN);
    }

    return AW_OK;
}

/**
 * \brief GPT CAP复位
 */
aw_local aw_err_t __imx10xx_cap_reset(void *p_cookie, int pid)
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_DEV_CONFIG(p_devcfg, p_devinfo);

    uint8_t i, ch;

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
    __imx10xx_gpt_hard_init(p_this, p_devinfo->cap_devcfg[ch].ch);

    return AW_OK;
}

/**
 * \brief GPT CAP时间换算
 */
aw_local aw_err_t __imx10xx_cap_count_to_time(void         *p_cookie,
                                              int           pid,
                                              unsigned int  count1,     /* 第一次捕获得到的值 */
                                              unsigned int  count2,     /* 第二次捕获得到的值 */
                                              unsigned int *p_time_ns)  /* 计算得到两次捕获之间的时间 */
{
    __CAP_DEV_DECL(p_this, p_cookie);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);

    uint32_t count_err = 0;
    uint32_t clkfreq   = aw_clk_rate_get(p_devinfo->clk_id);
    uint64_t tmp;

    if ((pid < p_devinfo->cap_servinfo.cap_min_id) ||
        (pid > p_devinfo->cap_servinfo.cap_max_id)) {

        return -AW_EINVAL;
    }

    count_err = count2 - count1;
    tmp = (uint64_t)1000000000 * (uint64_t)count_err;

    /* convert the count err value to time  */
    *p_time_ns = (uint64_t)tmp / (uint64_t)clkfreq;

    return AW_OK;
}

/** \brief 第一阶段初始化 */
aw_local void __cap_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}

/** \brief 第二阶段初始化 */
aw_local void __cap_inst_init2 (struct awbl_dev *p_dev)
{
    __CAP_DEV_DECL(p_this, p_dev);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);

    int     i  = 0;
    uint8_t ch = 0;

    /* 调用平台初始化函数 */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* 初始化使用到的通道 */
    for (i = 0; i < p_devinfo->pnum; i++) {
        ch = p_devinfo->cap_devcfg[i].ch;
        p_this->callback_info[ch].callback_func = NULL;
        p_this->callback_info[ch].p_arg = NULL;
        __imx10xx_gpt_hard_init(p_this, ch);
    }

    /* 连接中断服务函数 */
    aw_int_connect(p_devinfo->inum,
                  (aw_pfuncvoid_t)__gpt_cap_isr,
                  (void *)p_dev);

    aw_int_enable(p_devinfo->inum);
}

/** \brief 第三阶段初始化 */
aw_local void __cap_inst_connect (struct awbl_dev *p_dev)
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
        AWBL_VER_1,                     /* awb_ver */
        AWBL_BUSID_PLB,                 /* bus_id */
        AWBL_IMX10XX_GPT_CAP_NAME,  /* p_drvname */
        &__g_cap_std_drvfuncs,          /* p_busfuncs */
        &__g_imx10xx_cap_methods[0],    /* p_methods */
        NULL                            /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 IMX10xx GPT CAP 驱动注册到 AWBus 子系统中
 *
 * \note 本函数应在 aw_prj_early_init() 中初始化，本函数中禁止调用操作系统相关函数
 */
void awbl_imx10xx_gpt_cap_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_cap_drv_registration);
}

/* end of file */
