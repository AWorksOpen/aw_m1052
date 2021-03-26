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
 * \brief iMX RT10xx GPT ����
 *
 * ������ʵ���˶�ʱ���������жϹ��ܣ��Լ�ƥ��Ͳ����ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-09  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/
#include "aworks.h"
#include "aw_bitops.h"
#include "aw_assert.h"
#include "aw_gpio.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_gpt.h"

/*******************************************************************************
  �궨��
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
  ���غ�������
*******************************************************************************/

/** \brief Ϊ GPT ������Դ */
aw_local aw_err_t __imx10xx_gpt_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no);

 /** \brief Ϊ GPT �ͷ���Դ */
 aw_local aw_err_t __imx10xx_gpt_release (void *p_drv);

 /** \brief ��ȡ���������ֵ */
 aw_local aw_err_t __imx10xx_gpt_rollover_get (void *p_drv, uint32_t *p_count);

 /** \brief ��ȡ��ǰ����ֵ */
 aw_local aw_err_t __imx10xx_gpt_count_get (void *p_drv, uint32_t *p_count);

 /** \brief ʧ�� GPT */
 aw_local aw_err_t __imx10xx_gpt_disable (void *p_drv);

 /** \brief ʹ�� GPT */
 aw_local aw_err_t __imx10xx_gpt_enable (void *p_drv, uint32_t max_timer_count);

 /** \brief �����жϻص����� */
 aw_local aw_err_t __imx10xx_gpt_isr_set (void  *p_drv,
                                          void (*pfn_isr) (void *),
                                          void  *p_arg);

 /** \brief METHOD: ��ȡָ���׼��ʱ���ṹ��ָ�� */
 aw_local aw_err_t __imx10xx_gpt_func_get (struct awbl_dev    *p_dev,
                                           struct awbl_timer **pp_awbl_tmr,
                                           int                 timer_no);

/** \brief ������ڵ㺯�� */
aw_local void __imx10xx_gpt_inst_init1 (struct awbl_dev *p_dev);
aw_local void __imx10xx_gpt_inst_init2 (struct awbl_dev *p_dev);
aw_local void __imx10xx_gpt_inst_connect (struct awbl_dev *p_dev);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief �����׼��ʱ���ӿڵĽṹ�� */
aw_local aw_const struct awbl_timer_functions __g_imx10xx_timer_functions = {
    __imx10xx_gpt_allocate,     /* Ϊ GPT ������Դ */
    __imx10xx_gpt_release,      /* Ϊ GPT �ͷ���Դ */
    __imx10xx_gpt_rollover_get, /* ��ȡ���������ֵ */
    __imx10xx_gpt_count_get,    /* ��ȡ��ǰ����ֵ */
    __imx10xx_gpt_disable,      /* ʧ�� GPT */
    __imx10xx_gpt_enable,       /* ʹ�� GPT */
    __imx10xx_gpt_isr_set,      /* �����жϻص����� */
    NULL,                       /* ʹ�� 64 λ��ʱ�� */
    NULL,                       /* ��ȡ 64 λ���������ֵ */
    NULL                        /* ��ȡ 64 λ��ǰ����ֵ */
};

/** \brief ������ڵ� */
aw_const struct awbl_drvfuncs __g_imx10xx_gpt_drvfuncs = {
    __imx10xx_gpt_inst_init1,   /**< \brief ��һ�׶γ�ʼ�� */
    __imx10xx_gpt_inst_init2,   /**< \brief �ڶ��׶γ�ʼ�� */
    __imx10xx_gpt_inst_connect  /**< \brief �����׶γ�ʼ�� */
};

/** \brief �豸���� */
aw_local aw_const struct awbl_dev_method __g_imx10xx_gpt_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx10xx_gpt_func_get),
    AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_gpt_drvinfo = {
    AWBL_VER_1,                       /**< \brief AWBus �汾�� */
    AWBL_BUSID_PLB,                   /**< \brief ����ID */
    AWBL_IMX10XX_GPT_DRV_NAME,        /**< \brief ������ */
    &__g_imx10xx_gpt_drvfuncs,        /**< \brief ������ڵ� */
    &__g_imx10xx_gpt_methods[0],      /**< \brief �����ṩ�ķ��� */
    NULL                              /**< \brief ����̽�⺯�� */
};

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief ��ʱ���жϴ����� */
aw_local void __imx10xx_gpt_isr (struct awbl_imx10xx_gpt_dev *p_tmr)
{
    void (*pfn_isr) (void *);
    uint32_t int_stat;

    int_stat = AW_REG_READ32(&p_tmr->p_reg->sr);

    /* ����жϱ�־λ */
    AW_REG_WRITE32(&p_tmr->p_reg->sr, int_stat);

    pfn_isr = p_tmr->pfn_isr;
    if (pfn_isr != NULL) {
        (*pfn_isr)(p_tmr->p_arg);
    }
}

/** \brief Ϊ GPT ������Դ */
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

    /* ����������Ϣ */
    *pp_drv = (void *)p_tmr;

    ret = aw_int_enable(awbldev_get_imx10xx_gpt_param(p_dev)->inum);

cleanup:
    return  ret;
}

/** \brief Ϊ GPT �ͷ���Դ */
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

/** \brief ��ȡ���������ֵ */
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

/** \brief ��ȡ��ǰ����ֵ */
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

/** \brief ʧ�� GPT */
aw_local aw_err_t __imx10xx_gpt_disable (void *p_drv)
{
    awbl_imx10xx_gpt_dev_t           *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);
    const awbl_imx10xx_gpt_devinfo_t *p_param;

    p_param = awbldev_get_imx10xx_gpt_param(p_tmr);

    if (p_tmr->enabled) {

        /* ������ƼĴ��� */
        AW_REG_WRITE32(&p_tmr->p_reg->cr, 0x0);

        /* ����GPTʱ�� */
        aw_clk_disable(p_param->clk_id);

        p_tmr->enabled = 0;
    }

    return AW_OK;
}

/** \brief ʹ�� GPT */
aw_local aw_err_t __imx10xx_gpt_enable (void *p_drv, uint32_t max_timer_count)
{
    awbl_imx10xx_gpt_dev_t     *p_tmr = __pdrv_to_imx10xx_gpt(p_drv);
    awbl_imx10xx_gpt_devinfo_t *p_param;

    p_param = awbldev_get_imx10xx_gpt_param(p_tmr);

    if (p_tmr->enabled) {
        return -AW_EBUSY;
    }

    p_tmr->enabled = 1;

    /* ʹ��GPTʱ�� */
    aw_clk_enable(p_param->clk_id);

    p_tmr->count = max_timer_count;

    /* ����Ҫ���ô˶�ʱ�� */
    AW_REG_WRITE32(&p_tmr->p_reg->cr, 0x0);

    /* �����ж� */
    AW_REG_WRITE32(&p_tmr->p_reg->ir, 0x0);

    /* ����Ԥ��Ƶ */
    AW_REG_WRITE32(&p_tmr->p_reg->pr, 0x0);

    /* ѡ��ipg_clkΪԴʱ�� */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_CLKSRC, __CTRL_CLKSRC_LEN, __CTRL_CLKSRC_PCLK);

    /* ����������� */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_OM1, __CTRL_OM_LEN, __CTRL_OM_DIS);

    /* ֹͣ�͵ȴ�״̬��GPT�Ƿ�ʹ�� */
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_STOPEN);
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_WAITEN);

    /* �������¼���ģʽ */
    AW_REG_BIT_CLR32(&p_tmr->p_reg->cr, __CTRL_FRR);

    /* �����ж� */
    AW_REG_BIT_SET32(&p_tmr->p_reg->ir, __INT_OF1);

    /* ���ü������ֵ(���Ƚ�ֵ) */
    AW_REG_WRITE32(&p_tmr->p_reg->ocr[0], max_timer_count);

    /* �����־(д1��0) */
    AW_REG_BIT_SET32(&p_tmr->p_reg->sr, __STAT_OF1);

    /* ��ռ���ֵ */
    AW_REG_BIT_SET32(&p_tmr->p_reg->cr, __CTRL_ENMOD);

    /* ���Զ�ʱ����׼�� */
    /* �������ģʽ */
    AW_REG_BITS_SET32(&p_tmr->p_reg->cr, __CTRL_OM1, __CTRL_OM_LEN, 0);

    /* ʹ�� */
    AW_REG_BIT_SET32(&p_tmr->p_reg->cr, __CTRL_EN);

    return AW_OK;
}

/** \brief �����жϻص����� */
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

/** \brief METHOD: ��ȡָ���׼��ʱ���ṹ��ָ�� */
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

/** \brief ��һ�׶γ�ʼ�� */
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
    
    /* ʹ��GPTʱ�� */
    aw_clk_enable(p_param->clk_id);

    p_tmr->param.clk_frequency = aw_clk_rate_get(p_param->clk_id);

    /* ����GPTʱ�� */
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
    
    /* ��ʱ��ͨ���������ã���ͨ��ֻ��Ҫ�ж�bit0Ϊ�Ƿ���Ϊ���� */
    if (AW_BIT_ISSET(p_param->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }
}

/** \brief �ڶ��׶γ�ʼ�� */
aw_local void __imx10xx_gpt_inst_init2 (struct awbl_dev *p_dev)
{
    const struct awbl_imx10xx_gpt_devinfo *p_param;       /* timer parameter */

    p_param = awbldev_get_imx10xx_gpt_param(p_dev);

    /* �����ж� */
    while (AW_OK != aw_int_connect(p_param->inum,
                                   (aw_pfuncvoid_t)__imx10xx_gpt_isr,
                                   (void *)awbldev_to_imx10xx_gpt(p_dev))) {
    }
}

/** \brief �����׶γ�ʼ�� */
aw_local void __imx10xx_gpt_inst_connect (struct awbl_dev *p_dev)
{
    return;
}

/*******************************************************************************
  �ⲿ��������
*******************************************************************************/

/**
 * \brief �� IMX10xx GPT ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_gpt_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_gpt_drvinfo);
}

/* end of file */
