
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
#include "driver/acmp/awbl_imx10xx_acmp.h"
#include "driver/acmp/imx10xx_acmp_regs.h"
#include "driver/acmp/awbl_acmp.h"

/*******************************************************************************
  macro operate
*******************************************************************************/

/** \brief 从p_serv获得acmp_dev宏操作 */
#define __IMX10xx_ACMP_GET_THIS(p_this, p_serv) \
    struct awbl_imx10xx_acmp_dev *p_this = \
        AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_acmp_dev, acmp_serv)

/** \brief 从p_dev获得acmp_dev宏操作 */
#define __IMX10xx_ACMP_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_acmp_dev *p_this = \
        (struct awbl_imx10xx_acmp_dev *)(p_dev)

/** \brief 从p_dev获得acmp_devinfo宏操作 */
#define __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_acmp_devinfo *p_devinfo = \
        (struct awbl_imx10xx_acmp_devinfo *)AWBL_DEVINFO_GET(p_dev)

/** \brief 从acmp_devinfo获得寄存器信息 */
#define __IMX10xx_ACMP_HW_DECL(p_hw_acmp, p_devinfo) \
        imx10xx_acmp_regs_t *p_hw_acmp = \
        ((imx10xx_acmp_regs_t *)((p_devinfo)->regbase))


/******************************************************************************/

aw_local void __imx10xx_acmp_isr (void *p_arg)
{
    __IMX10xx_ACMP_DEV_DECL(p_this, p_arg);
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ACMP_HW_DECL(p_hw_acmp, p_devinfo);

    uint8_t status = 0;
    cmp_status_flags_t temp;

    status = readb(&p_hw_acmp->SCR);
    writeb(status, &p_hw_acmp->SCR);

    if (status & __CMP_SCR_CFR_MASK) {

        temp = kCMP_OutputRisingEventFlag;
    } else if (status & __CMP_SCR_CFF_MASK) {

        temp = kCMP_OutputFallingEventFlag;
    }

    /* 执行用户回调函数 */
    if (NULL != p_this->acmp_serv.p_func) {
        p_this->acmp_serv.p_func(&temp);
    }
}


/* 硬件初始化 */
aw_local void  __acmp_hard_init(struct awbl_imx10xx_acmp_dev *p_dev)
{
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_dev);
    __IMX10xx_ACMP_HW_DECL(p_hw_acmp, p_devinfo);

    uint8_t reg_val = 0;

    /* disable acmp */
    writeb(0x0, &p_hw_acmp->CR1);

    /* Hysteresis level 0 */
    reg_val = __CMP_CR0_HYSTCTR(0);
    writeb(reg_val, &p_hw_acmp->CR0);

    /* High-Speed (HS) Comparison mode selected. */
    reg_val = __CMP_CR1_PMODE_MASK |
              __CMP_CR1_EN_MASK;
    writeb(reg_val, &p_hw_acmp->CR1);

    /* Disable dac */
    writeb(0x0, &p_hw_acmp->DACCR);

    /* 使能Rising和Falling中断 */
    writeb(__CMP_SCR_IEF_MASK | __CMP_SCR_IER_MASK, &p_hw_acmp->SCR);

}


/* 配置DAC */
aw_err_t __acmp_config_dac (struct awbl_acmp_service *p_serv,
                            uint8_t                   vin,
                            uint16_t                  vin_mv,
                            uint16_t                  vref_mv)
{
    __IMX10xx_ACMP_GET_THIS(p_this, p_serv);
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ACMP_HW_DECL(p_hw_acmp, p_devinfo);

    uint8_t  reg_val = 0;
    uint16_t v_temp  = 0;
    int      i       = 0;

    /* CMPx_DACCR. */
    reg_val |= __CMP_DACCR_DACEN_MASK;  /* Enable the internal DAC. */

    if (VREF_SOURCE_VIN2 == vin) {

        reg_val |= __CMP_DACCR_VRSEL_MASK;
    }

    v_temp = vin_mv / 64;

    for (i = 0; i < 64; i++) {
        if (i * v_temp >= vref_mv)  {
            break;
        }
    }

    reg_val |= __CMP_DACCR_VOSEL(i);
    writeb(reg_val, &p_hw_acmp->DACCR);

    return AW_OK;
}


/* 配置比较通道 */
aw_err_t __acmp_set_input_chan (struct awbl_acmp_service *p_serv,
                                 uint32_t                  positive_chan,
                                 uint16_t                  negative_chan)
{
    __IMX10xx_ACMP_GET_THIS(p_this, p_serv);
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_this);
    __IMX10xx_ACMP_HW_DECL(p_hw_acmp, p_devinfo);

    uint8_t reg_val = 0;

    reg_val = __CMP_MUXCR_PSEL(positive_chan) | __CMP_MUXCR_MSEL(negative_chan);

    writeb(reg_val, &p_hw_acmp->MUXCR);

    return AW_OK;
}


aw_local aw_err_t __acmp_init (struct awbl_acmp_service *p_serv)
{
    __IMX10xx_ACMP_GET_THIS(p_this, p_serv);
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_this);


    /* 硬件初始化 */
    __acmp_hard_init(p_this);

    if (NULL != p_serv->p_func) {

        /* 连接中断 */
        aw_int_connect(p_devinfo->int_num,
                      (aw_pfuncvoid_t)__imx10xx_acmp_isr,
                      (void *)p_this);

        /* 使能中断 */
        aw_int_enable(p_devinfo->int_num);
    }

    return AW_OK;
}



/** \brief sio driver functions */
static awbl_acmp_drv_funcs_t __g_imx10xx_acmp_drv_funcs = {
    __acmp_init,
    __acmp_config_dac,
    __acmp_set_input_chan,

};

/**
 * \brief initialize imx10xx can device
 */
aw_local void __imx10xx_acmp_inst_init (awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief initialize imx10xx can device
 */
aw_local void __imx10xx_acmp_inst_init2 (awbl_dev_t *p_dev)
{
    __IMX10xx_ACMP_DEV_DECL(p_this, p_dev);
    __IMX10xx_ACMP_DEVINFO_DECL(p_devinfo, p_dev);

    if (NULL != p_devinfo->pfn_plfm_init) {
        p_devinfo->pfn_plfm_init();
    }

    awbl_acmp_service_init(&(p_this->acmp_serv),
                           &__g_imx10xx_acmp_drv_funcs,
                           p_devinfo->acmp_id);

    awbl_acmp_service_register(&p_this->acmp_serv);

    /* 先禁能中断 */
    aw_int_disable(p_devinfo->int_num);

    return;
}

/**
 * \brief connect imx10xx acmp device to system
 */
aw_local void __imx10xx_acmp_inst_connect (awbl_dev_t *p_dev)
{
    return;
}

aw_const struct awbl_drvfuncs g_imx10xx_acmp_awbl_drvfuncs = {
    __imx10xx_acmp_inst_init,        /* devInstanceInit */
    __imx10xx_acmp_inst_init2,       /* devInstanceInit2 */
    __imx10xx_acmp_inst_connect      /* devConnect */
};


/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_acmp_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_ACMP_NAME,             /* p_drvname*/
    &g_imx10xx_acmp_awbl_drvfuncs,      /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};


/**
 * \brief register imx10xx acmp driver
 */
void awbl_imx10xx_acmp_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_acmp_drv_registration);
}

/* end of file */
