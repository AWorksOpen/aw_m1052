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
 * \brief iMX RT10xx GPT CAP����
 *
 * ������ʵ����GPT��ʱ���Ĳ����ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 20-09-09  licl, first implementation
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/
#include "apollo.h"
#include "aw_bitops.h"
#include "aw_gpio.h"
#include "aw_int.h"
#include "driver/cap/awbl_imx10xx_gpt_cap.h"

/*******************************************************************************
  �궨��
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
  ���غ�������
*******************************************************************************/

/** \brief ��ʱ���жϴ����� */
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

            /* ���״̬ */
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
 * \brief GPTӲ����ʼ��
 */
aw_local void __imx10xx_gpt_hard_init (awbl_imx10xx_gpt_cap_dev_t *p_this,
                                       uint8_t                     channel)
{
    __CAP_DEVINFO_DECL(p_devinfo, p_this);
    __CAP_HW_DECL(p_hw_cap, p_devinfo);

    /* ʹ��GPTʱ�� */
    aw_clk_enable(p_devinfo->clk_id);

    /* ����Ҫ���ô˶�ʱ�� */
    AW_REG_WRITE32(&p_hw_cap->cr, 0x0);

    /* �����ж� */
    AW_REG_WRITE32(&p_hw_cap->ir, 0x0);

    /* ����Ԥ��Ƶ */
    AW_REG_WRITE32(&p_hw_cap->pr, 0x0);

    /* ѡ��ipg_clkΪԴʱ�� */
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_CLKSRC, __CTRL_CLKSRC_LEN, __CTRL_CLKSRC_PCLK);

    /* ����������� */
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_OM1, __CTRL_OM_LEN, __CTRL_OM_DIS);

    /* ֹͣ�͵ȴ�״̬��GPT�Ƿ�ʹ�� */
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_STOPEN);
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_WAITEN);

    /* �������¼���ģʽ */
    AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_FRR);
}

/**
 * \brief GPT CAP����
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

    /* ����pid��Ӧ��CAPͨ�� */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* ���ô������� */
    if ((options & AW_CAP_TRIGGER_RISE) == AW_CAP_TRIGGER_RISE) {
        reg = __CTRL_IM_RISE;
    } else if ((options & AW_CAP_TRIGGER_FALL) == AW_CAP_TRIGGER_FALL) {
        reg = __CTRL_IM_FALL;
    } else if ((options & AW_CAP_TRIGGER_BOTH_EDGES) == AW_CAP_TRIGGER_BOTH_EDGES) {
        reg = __CTRL_IM_BOTH;
    }
    AW_REG_BITS_SET32(&p_hw_cap->cr, __CTRL_IM1 + ch * 2, __CTRL_IM_LEN, reg);

    /* �����жϻص����� */
    p_this->callback_info[ch].callback_func = pfunc_callback;
    p_this->callback_info[ch].p_arg = p_arg;

    return AW_OK;
}

/**
 * \brief GPT CAPʹ��
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

    /* ����pid��Ӧ��CAPͨ�� */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* �����ж� */
    AW_REG_BIT_SET32(&p_hw_cap->ir, __INT_IF1 + ch);

    /* ���ü������ֵ(���Ƚ�ֵ) */
    AW_REG_WRITE32(&p_hw_cap->ocr[0], 0xffffffff);

    /* �����־(д1��0) */
    AW_REG_BIT_SET32(&p_hw_cap->sr, __STAT_IF1 + ch);

    /* ��ռ���ֵ */
    AW_REG_BIT_SET32(&p_hw_cap->cr, __CTRL_ENMOD);

    /* ʹ�� */
    AW_REG_BIT_SET32(&p_hw_cap->cr, __CTRL_EN);

    return AW_OK;
}

/**
 * \brief GPT CAP����
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

    /* ����pid��Ӧ��CAPͨ�� */
    for (i = 0; i < p_devinfo->pnum; i++) {
        if (pid == p_devcfg[i].pid) {
            ch = p_devcfg[i].ch;
            break;
        }
    }

    if (i >= p_devinfo->pnum) {
        return AW_ERROR;
    }

    /* �����ж� */
    AW_REG_BIT_CLR32(&p_hw_cap->ir, __INT_IF1 + ch);

    if (AW_REG_READ32(&p_hw_cap->ir) & (0x18) == 0) {
        AW_REG_BIT_CLR32(&p_hw_cap->cr, __CTRL_EN);
    }

    return AW_OK;
}

/**
 * \brief GPT CAP��λ
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

    /* ����pid��Ӧ��CAPͨ�� */
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
 * \brief GPT CAPʱ�任��
 */
aw_local aw_err_t __imx10xx_cap_count_to_time(void         *p_cookie,
                                              int           pid,
                                              unsigned int  count1,     /* ��һ�β���õ���ֵ */
                                              unsigned int  count2,     /* �ڶ��β���õ���ֵ */
                                              unsigned int *p_time_ns)  /* ����õ����β���֮���ʱ�� */
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

/** \brief ��һ�׶γ�ʼ�� */
aw_local void __cap_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}

/** \brief �ڶ��׶γ�ʼ�� */
aw_local void __cap_inst_init2 (struct awbl_dev *p_dev)
{
    __CAP_DEV_DECL(p_this, p_dev);
    __CAP_DEVINFO_DECL(p_devinfo, p_this);

    int     i  = 0;
    uint8_t ch = 0;

    /* ����ƽ̨��ʼ������ */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    /* ��ʼ��ʹ�õ���ͨ�� */
    for (i = 0; i < p_devinfo->pnum; i++) {
        ch = p_devinfo->cap_devcfg[i].ch;
        p_this->callback_info[ch].callback_func = NULL;
        p_this->callback_info[ch].p_arg = NULL;
        __imx10xx_gpt_hard_init(p_this, ch);
    }

    /* �����жϷ����� */
    aw_int_connect(p_devinfo->inum,
                  (aw_pfuncvoid_t)__gpt_cap_isr,
                  (void *)p_dev);

    aw_int_enable(p_devinfo->inum);
}

/** \brief �����׶γ�ʼ�� */
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

     /* ��ȡcap����ʵ�� */
    p_serv = &p_this->cap_serv;

    /* ��ʼ��cap����ʵ�� */
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


/** �������ṩ��һ�ַ������÷������ڵõ�cap���񣨷���������3������ָ���ϣ� */
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
  �ⲿ��������
*******************************************************************************/

/**
 * \brief �� IMX10xx GPT CAP ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_gpt_cap_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_cap_drv_registration);
}

/* end of file */
