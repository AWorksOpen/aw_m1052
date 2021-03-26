
/******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx Quadrature Decoder driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2018-03-20  mex, first implementation.
 * \endinternal
 */
#include "aworks.h"
#include "aw_int.h"
#include "driver/enc/awbl_imx10xx_enc.h"
#include "driver/enc/imx10xx_enc_regs.h"

/*******************************************************************************
  macro operate
*******************************************************************************/

/** \brief 从p_serv获得enc_dev宏操作 */
#define __IMX10xx_ENC_GET_THIS(p_this, p_serv) \
    struct awbl_imx10xx_enc_dev *p_this = \
        AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_enc_dev, enc_serv)

/** \brief 从p_dev获得enc_dev宏操作 */
#define __IMX10xx_ENC_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_enc_dev *p_this = \
        (struct awbl_imx10xx_enc_dev *)(p_dev)

/** \brief 从p_dev获得enc_devinfo宏操作 */
#define __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_enc_devinfo *p_devinfo = \
        (struct awbl_imx10xx_enc_devinfo *)AWBL_DEVINFO_GET(p_dev)

/** \brief 从enc_devinfo获得寄存器信息 */
#define __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo) \
        imx10xx_enc_regs_t *p_hw_enc = \
        ((imx10xx_enc_regs_t *)((p_devinfo)->regbase))


/******************************************************************************/

aw_local void __imx10xx_enc_isr (void *p_arg)
{
    __IMX10xx_ENC_DEV_DECL(p_this, p_arg);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo);

    /* 写1清除index中断标志位 */
    uint16_t state = 0;
    state = readw(&p_hw_enc->CTRL);
    writew(state, &p_hw_enc->CTRL);

    if (state && __ENC_CTRL_XIRQ_MASK) {

        /* 执行用户回调函数 */
        if (NULL != p_this->enc_serv.p_func) {
            p_this->enc_serv.p_func(p_this->enc_serv.p_arg);
        }
    }
}


/* 硬件初始化 */
aw_local void  __enc_hard_init(struct awbl_imx10xx_enc_dev *p_dev)
{
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_dev);
    __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo);

    uint16_t reg_val = 0;

    /* 先清除各数据寄存器值 */
    writew(0x00, &p_hw_enc->CTRL);
    writew(0x00, &p_hw_enc->FILT);
    writew(0x00, &p_hw_enc->REV);
    writew(0x00, &p_hw_enc->UPOS);
    writew(0x00, &p_hw_enc->LPOS);

    if (enc_index_trigger_disable != p_devinfo->trigger_mode) {

        reg_val |= __ENC_CTRL_XIP_MASK;
        if (enc_index_trigger_falling_edge == p_devinfo->trigger_mode) {
            reg_val |= __ENC_CTRL_XNE_MASK;
        }

        /* 使能Index中断 */
        reg_val |= __ENC_CTRL_XIE_MASK | __ENC_CTRL_XIRQ_MASK;
    }

    writew(reg_val, &p_hw_enc->CTRL);

    /* 设置滤波器 */
    reg_val = __ENC_FILT_CNT(p_devinfo->filter_count) |
              __ENC_FILT_PER(p_devinfo->filter_sample_priod);

    writew(reg_val, &p_hw_enc->FILT);

    writew(0x00, &p_hw_enc->CTRL2);

    writew(__ENC_UCOMP_COMP(0xFFFF),&p_hw_enc->UCOMP);
    writew(__ENC_LCOMP_COMP(0xFFFF),&p_hw_enc->LCOMP);

    writew(__ENC_UINIT_INIT(0x00),&p_hw_enc->UINIT);
    writew(__ENC_LINIT_INIT(0x00),&p_hw_enc->LINIT);

    /*  Load the initial position value to position counter  */
    reg_val = readw(&p_hw_enc->CTRL);

    reg_val &=  ((uint16_t)~(__ENC_CTRL_HIRQ_MASK |
                  __ENC_CTRL_XIRQ_MASK |
                  __ENC_CTRL_DIRQ_MASK |
                  __ENC_CTRL_CMPIRQ_MASK));
    reg_val |= __ENC_CTRL_SWIP_MASK;

    writew(reg_val, &p_hw_enc->CTRL);
}


/* 读取位置数据 */
static aw_err_t __enc_get_position (struct awbl_enc_service *p_serv,
                                    int                     *p_val)
{
    uint32_t val = 0;
    __IMX10xx_ENC_GET_THIS(p_this, p_serv);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo);

    /* Get upper 16 bits and make a snapshot. */
    val = readw(&p_hw_enc->UPOS);
    val <<= 16;

    /* Get lower 16 bits from hold register. */
    val |= readw(&p_hw_enc->LPOS);
    *p_val = val;

    return AW_OK;
}


/* 读取微分数据 */
static aw_err_t __enc_get_position_difference (struct awbl_enc_service *p_serv,
                                               int16_t                 *p_val)
{
    __IMX10xx_ENC_GET_THIS(p_this, p_serv);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo);

    *p_val  = readw(&p_hw_enc->POSD);

    return AW_OK;
}


/* 读取旋转速度 */
static aw_err_t __enc_get_revolution (struct awbl_enc_service *p_serv,
                                      int16_t                 *p_val)
{
    __IMX10xx_ENC_GET_THIS(p_this, p_serv);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ENC_HW_DECL(p_hw_enc, p_devinfo);

    *p_val = readw(&p_hw_enc->REV);

    return AW_OK;
}

aw_local aw_err_t __enc_init (struct awbl_enc_service *p_serv)
{
    __IMX10xx_ENC_GET_THIS(p_this, p_serv);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_this);

    /* 硬件初始化 */
    __enc_hard_init(p_this);

    if (NULL != p_serv->p_func) {

        /* 使能中断 */
        aw_int_connect(p_devinfo->int_num,
                      (aw_pfuncvoid_t)__imx10xx_enc_isr,
                      (void *)p_this);
        aw_int_enable(p_devinfo->int_num);
    }

    return AW_OK;
}



/** \brief sio driver functions */
static awbl_enc_drv_funcs_t __g_imx10xx_enc_drv_funcs = {
    __enc_init,
    __enc_get_position,
    __enc_get_position_difference,
    __enc_get_revolution,
};

/**
 * \brief initialize imx10xx can device
 */
aw_local void __imx10xx_enc_inst_init (awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief initialize imx10xx can device
 */
aw_local void __imx10xx_enc_inst_init2 (awbl_dev_t *p_dev)
{
    __IMX10xx_ENC_DEV_DECL(p_this, p_dev);
    __IMX10xx_ENC_DEVINFO_DECL(p_devinfo, p_dev);

    if (NULL != p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    awbl_enc_service_init(&(p_this->enc_serv),
                           &__g_imx10xx_enc_drv_funcs,
                           p_devinfo->chan);

    awbl_enc_service_register(&p_this->enc_serv);

    aw_int_disable(p_devinfo->int_num);

    return;
}

/**
 * \brief connect imx10xx enc device to system
 */
aw_local void __imx10xx_enc_inst_connect (awbl_dev_t *p_dev)
{

    return;
}

aw_const struct awbl_drvfuncs g_imx10xx_enc_awbl_drvfuncs = {
    __imx10xx_enc_inst_init,        /* devInstanceInit */
    __imx10xx_enc_inst_init2,       /* devInstanceInit2 */
    __imx10xx_enc_inst_connect      /* devConnect */
};


/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_enc_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_ENC_NAME,              /* p_drvname*/
    &g_imx10xx_enc_awbl_drvfuncs,       /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};


/**
 * \brief register imx10xx enc driver
 */
void awbl_imx10xx_enc_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_enc_drv_registration);
}


/* end of file */
