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
 * \brief iMX RT10xx PIT ����
 *
 * ������ʵ���˶�ʱ���������жϹ��ܡ�
 *
 * \internal
 * \par modification history
 * - 1.00 17-11-13  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
*******************************************************************************/
#include "aworks.h"
#include "aw_bitops.h"
#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "driver/timer/awbl_imx10xx_pit.h"

/*******************************************************************************
  �궨��
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
  ���غ�������
*******************************************************************************/

/** \brief Ϊ PIT ������Դ */
aw_local aw_err_t __imx10xx_pit_allocate (struct awbl_dev *p_dev,
                                          uint32_t         flags,
                                          void           **pp_drv,
                                          int              timer_no);

 /** \brief Ϊ PIT �ͷ���Դ */
 aw_local aw_err_t __imx10xx_pit_release (void *p_drv);

 /** \brief ��ȡ���������ֵ */
 aw_local aw_err_t __imx10xx_pit_rollover_get (void *p_drv, uint32_t *p_count);

 /** \brief ��ȡ��ǰ����ֵ */
 aw_local aw_err_t __imx10xx_pit_count_get (void *p_drv, uint32_t *p_count);

 /** \brief ʧ�� PIT */
 aw_local aw_err_t __imx10xx_pit_disable (void *p_drv);

 /** \brief ʹ�� PIT */
 aw_local aw_err_t __imx10xx_pit_enable (void *p_drv, uint32_t max_timer_count);

 /** \brief �����жϻص����� */
 aw_local aw_err_t __imx10xx_pit_isr_set (void  *p_drv,
                                          void (*pfn_isr) (void *),
                                          void  *p_arg);

 /** \brief METHOD: ��ȡָ���׼��ʱ���ṹ��ָ�� */
 aw_local aw_err_t __imx10xx_pit_func_get (struct awbl_dev    *p_dev,
                                           struct awbl_timer **pp_awbl_tmr,
                                           int                 timer_no);

/** \brief ������ڵ㺯�� */
aw_local void __imx10xx_pit_inst_init1 (struct awbl_dev *p_dev);
aw_local void __imx10xx_pit_inst_init2 (struct awbl_dev *p_dev);
aw_local void __imx10xx_pit_inst_connect (struct awbl_dev *p_dev);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief �����׼��ʱ���ӿڵĽṹ�� */
aw_local aw_const struct awbl_timer_functions __g_imx10xx_timer_functions = {
    __imx10xx_pit_allocate,     /* Ϊ PIT ������Դ */
    __imx10xx_pit_release,      /* Ϊ PIT �ͷ���Դ */
    __imx10xx_pit_rollover_get, /* ��ȡ���������ֵ */
    __imx10xx_pit_count_get,    /* ��ȡ��ǰ����ֵ */
    __imx10xx_pit_disable,      /* ʧ�� PIT */
    __imx10xx_pit_enable,       /* ʹ�� PIT */
    __imx10xx_pit_isr_set,      /* �����жϻص����� */
    NULL,                       /* ʹ�� 64 λ��ʱ�� */
    NULL,                       /* ��ȡ 64 λ���������ֵ */
    NULL                        /* ��ȡ 64 λ��ǰ����ֵ */
};

/** \brief ������ڵ� */
aw_const struct awbl_drvfuncs __g_imx10xx_pit_drvfuncs = {
    __imx10xx_pit_inst_init1,   /**< \brief ��һ�׶γ�ʼ�� */
    __imx10xx_pit_inst_init2,   /**< \brief �ڶ��׶γ�ʼ�� */
    __imx10xx_pit_inst_connect  /**< \brief �����׶γ�ʼ�� */
};

/** \brief �豸���� */
aw_local aw_const struct awbl_dev_method __g_imx10xx_pit_methods[] = {
    AWBL_METHOD(awbl_timer_func_get, __imx10xx_pit_func_get),
    AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_pit_drvinfo = {
    AWBL_VER_1,                       /**< \brief AWBus �汾�� */
    AWBL_BUSID_PLB,                   /**< \brief ����ID */
    AWBL_IMX10XX_PIT_DRV_NAME,        /**< \brief ������ */
    &__g_imx10xx_pit_drvfuncs,        /**< \brief ������ڵ� */
    &__g_imx10xx_pit_methods[0],      /**< \brief �����ṩ�ķ��� */
    NULL                              /**< \brief ����̽�⺯�� */
};

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief ��ʱ���жϴ����� */
aw_local void __imx10xx_pit_isr (struct awbl_imx10xx_pit_dev *p_this)
{
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);
    void   (*pfn_isr)(void *);
    uint32_t int_stat;

    aw_assert(p_this != NULL);
    aw_assert(p_devinfo != NULL);
    aw_assert(p_reg != NULL);

    /* �鿴�ж�״̬(������) */
    int_stat = AW_REG_READ32(&p_reg->channel[0].tflg);

    /* ����жϱ�־λ */
    AW_REG_WRITE32(&p_reg->channel[0].tflg, __PIT_TFLG_TIF_MASK);

    pfn_isr = p_this->pfn_isr;
    if (pfn_isr != NULL) {
        (*pfn_isr)(p_this->p_arg);
    }
}

/** \brief Ϊ PIT ������Դ */
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

    /* ����������Ϣ */
    *pp_drv = (void *)p_this;

    ret = aw_int_enable(p_devinfo->inum);

cleanup:
    return  ret;
}

/** \brief Ϊ PIT �ͷ���Դ */
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

/** \brief ��ȡ���������ֵ */
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

/** \brief ��ȡ��ǰ����ֵ */
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

/** \brief ʧ�� PIT */
aw_local aw_err_t __imx10xx_pit_disable (void *p_drv)
{
    __PIT_DEV_DECL(p_this, p_drv);
    __DEV_GET_PIT_DEVINFO_DECL(p_devinfo, p_this);
    __DEVINFO_GET_PIT_REG_DECL(p_reg, p_devinfo);

    aw_assert(NULL != p_this);
    aw_assert(NULL != p_devinfo);
    aw_assert(NULL != p_reg);

    if (p_this->enabled) {

        /* ������ƼĴ��� */
        AW_REG_WRITE32(&p_reg->channel[0].tctrl, 0);

        /* ʧ�� PIT */
        AW_REG_BIT_SET_MASK32(&p_reg->mcr, __PIT_MCR_MDIS_MASK);

        /* ���� PIT ʱ�� */
        aw_clk_disable(p_devinfo->clk_id);

        p_this->enabled = 0;
    }

    return AW_OK;
}

/** \brief ʹ�� PIT */
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

    /* ʹ��PITʱ�� */
    aw_clk_enable(p_devinfo->clk_id);

    /* ʹ�� PIT */
    AW_REG_BIT_CLR_MASK32(&p_reg->mcr, __PIT_MCR_MDIS_MASK);

    writel(0, &p_reg->channel[0].tctrl);

    /* ���ü������ֵ(����װ�ؼ���ֵ) */
    AW_REG_WRITE32(&p_reg->channel[0].ldval, max_timer_count);

    /* �����־(д1��0) */
    AW_REG_WRITE32(&p_reg->channel[0].tflg, __PIT_TFLG_TIF_MASK);

    /* ʹ���жϡ���ʼ�ݼ����� */
    AW_REG_WRITE32(&p_reg->channel[0].tctrl, __PIT_TCTRL_TEN_MASK |
                                             __PIT_TCTRL_TIE_MASK);

    p_this->enabled = 1;

    return AW_OK;
}

/** \brief �����жϻص����� */
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

/** \brief METHOD: ��ȡָ���׼��ʱ���ṹ��ָ�� */
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

/** \brief ��һ�׶γ�ʼ�� */
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
    
    /* ʹ��PITʱ�� */
    aw_clk_enable(p_devinfo->clk_id);

    p_this->param.clk_frequency = aw_clk_rate_get(p_devinfo->clk_id);

    /* ����PITʱ�� */
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
    
    /* ��ʱ��ͨ���������ã���ͨ��ֻ��Ҫ�ж�bit0Ϊ�Ƿ���Ϊ���� */
    if (AW_BIT_ISSET(p_devinfo->alloc_byname, 0) != 0) {
        p_awbl_tmr->alloc_by_name = AW_TRUE;
    } else {
        p_awbl_tmr->alloc_by_name = AW_FALSE;
    }

    /* �����ж� */
    while (AW_OK != aw_int_connect(p_devinfo->inum,
                                   (aw_pfuncvoid_t)__imx10xx_pit_isr,
                                   (void *)p_this)) {
    }
}

/** \brief �ڶ��׶γ�ʼ�� */
aw_local void __imx10xx_pit_inst_init2 (struct awbl_dev *p_dev)
{

}

/** \brief �����׶γ�ʼ�� */
aw_local void __imx10xx_pit_inst_connect (struct awbl_dev *p_dev)
{
    return;
}

/*******************************************************************************
  �ⲿ��������
*******************************************************************************/

/**
 * \brief �� IMX10xx PIT ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note ������Ӧ�� aw_prj_early_init() �г�ʼ�����������н�ֹ���ò���ϵͳ��غ���
 */
void awbl_imx10xx_pit_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_pit_drvinfo);
}

/* end of file */
