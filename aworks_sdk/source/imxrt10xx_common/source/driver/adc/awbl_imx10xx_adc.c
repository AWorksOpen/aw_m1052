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
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx adc driver
 *
 * \internal
 * \par Modification History
 * - 1.00 2017-11-02  mex, first implementation.
 * \endinternal
 */

#include "aw_common.h"
#include "aw_int.h"
#include "aw_adc.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_adc.h"
#include "aw_vdebug.h"
#include "aw_clk.h"
#include "aw_delay.h"
#include "driver/adc/awbl_imx10xx_adc.h"

/**
 * \brief IMX10xx ADC 寄存器块定义
 */
typedef struct __imx10xx_adc_regs {

    volatile uint32_t HC[8];           /**< \brief Control register for hardware triggers */
    volatile const uint32_t HS;        /**< \brief  Status register for HW triggers */
    volatile const uint32_t R[8];      /**< \brief  Data result register for HW triggers */
    volatile uint32_t CFG;             /**< \brief Configuration register */
    volatile uint32_t GC;              /**< \brief  General control register */
    volatile uint32_t GS;              /**< \brief General status register */
    volatile uint32_t CV;              /**< \brief Compare value register */
    volatile uint32_t OFS;             /**< \brief  Offset correction value register */
    volatile uint32_t CAL;             /**< \brief Calibration value register */

} imx10xx_adc_regs_t;


/*******************************************************************************
  local defines
*******************************************************************************/

/* write register */
#define __ADC_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, data)

/* read register */
#define __ADC_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* bit is set */
#define __ADC_REG_BIT_ISSET(reg_addr, bit) \
        AW_REG_BIT_ISSET32(reg_addr, bit)

/* set bit */
#define __ADC_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __ADC_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __ADC_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* set register bits */
#define __ADC_REG_BITS_SET(reg_addr, start, len, value) \
        AW_REG_BITS_SET32(reg_addr, start, len, value)

/* declare imx10xx adc device instance */
#define __ADC_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_adc_dev *p_this = \
        (struct awbl_imx10xx_adc_dev *)(p_dev)

/* declare imx10xx adc device infomation */
#define __ADC_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx10xx_adc_devinfo *p_devinfo = \
        (struct awbl_imx10xx_adc_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* declare imx10xx adc hardware instance */
#define __ADC_HW_DECL(p_hw_adc, p_devinfo) \
    imx10xx_adc_regs_t *p_hw_adc =         \
        ((imx10xx_adc_regs_t *)((p_devinfo)->regbase))

/* devlock lock */
#define __ADC_DEV_LOCK(p_dev) \
    AW_MUTEX_LOCK(p_dev->dev_lock, AW_SEM_WAIT_FOREVER)

/* devlock unlock */
#define __ADC_DEV_UNLOCK(p_dev) \
    AW_MUTEX_UNLOCK(p_dev->dev_lock)

/* devlock initialize */
#define __ADC_DEV_LOCK_INIT(p_dev) \
    AW_MUTEX_INIT(p_dev->dev_lock, AW_SEM_Q_PRIORITY)


/*******************************************************************************
   ADC Configuration register define
*******************************************************************************/

/* Data Overwrite Enable */
#define __ADC_CFG_OVWREN_MASK      (0x1UL << 16)

/* Hardware Average select */
#define __ADC_CFG_AVGS_MASK        (0x3UL << 14)

/* Conversion Trigger Select */
#define __ADC_CFG_ADTRG_MASK       (0x1UL << 13)

/* High Speed Configuration */
#define __ADC_CFG_ADHSC_MASK       (0x1UL << 10)

/* Voltage Reference Selection */
#define __ADC_CFG_REFSEL(x)        (((x) & 0x3) << 11)

/* Defines the sample time duration */
#define __ADC_CFG_ADSTS(x)         (((x) & 0x3) << 8)

/* Clock Divide Select */
#define __ADC_CFG_ADIV_SHIFT        5
#define __ADC_CFG_ADIV(x)          (((x) & 0x3) << __ADC_CFG_ADIV_SHIFT)

/* Long Sample Time Configuration */
#define __ADC_CFG_ADLSMP_MASK      (0x1UL << 4)

/* Conversion Mode Selection */
#define __ADC_CFG_MODE(x)          (((x) & 0x3) << 2)

/* Input Clock Select */
#define __ADC_CFG_ADICLK_MASK      (0x3UL)
#define __ADC_CFG_ADICLK(x)        (((x) & 0x3) << 0)


/*******************************************************************************
   ADC General control register define
*******************************************************************************/

/* Calibration */
#define __ADC_GC_CAL_SHIFT          7
#define __ADC_GC_CAL_MASK(x)       (0x1UL << __ADC_GC_CAL_SHIFT)

/* Continuous Conversion Enable */
#define __ADC_GC_ADCO_MASK(x)      (0x1UL << 6)

/* Hardware average enable */
#define __ADC_GC_AVGE_MASK(x)      (0x1UL << 5)

/* Compare Function Enable */
#define __ADC_GC_ACFE_MASK(x)      (0x1UL << 4)

/* Compare Function Greater Than Enable */
#define __ADC_GC_ACFGT_MASK(x)     (0x1UL << 3)

/* Compare Function Range Enable */
#define __ADC_GC_ACREN_MASK(x)     (0x1UL << 2)

/* DMA Enable */
#define __ADC_GC_DMAEN_MASK(x)     (0x1UL << 1)

/* Asynchronous clock output enable */
#define __ADC_GC_ADACKEN_MASK(x)   (0x1UL << 0)


/*******************************************************************************
   ADC General status register define
*******************************************************************************/

/* Asynchronous wakeup interrupt status */
#define __ADC_GS_AWKST_SHIFT        2
#define __ADC_GS_AWKST_MASK        (0x1 << __ADC_GS_AWKST_SHIFT)

/* Calibration Failed Flag */
#define __ADC_GS_CALF_SHIFT         1
#define __ADC_GS_CALF_MASK         (0x1 << __ADC_GS_CALF_SHIFT)

/* Conversion Active */
#define __ADC_GS_ADACT_SHIFT        0
#define __ADC_GS_ADACT_MASK        (0x1 << __ADC_GS_CALF_SHIFT)

/*******************************************************************************
   ADC Status register for HW triggers define
*******************************************************************************/

/* Conversion Complete Flag */
#define __ADC_HS_COCO0_SHIFT        0
#define __ADC_HS_COCO0_MASK        (0x1 << __ADC_HS_COCO0_SHIFT)

/*******************************************************************************
   ADC Control register for hardware triggers define
*******************************************************************************/

/* Input Channel Select */
#define __ADC_HC_ADCH_SHIFT         0
#define __ADC_HC_ADCH_MASK         (0x1FUL)
#define __ADC_HC_ADCH(x)           (((x) & __ADC_HC_ADCH_MASK) << __ADC_HC_ADCH_SHIFT)

/* Conversion Complete Interrupt Enable/Disable Control */
#define __ADC_HC_AIEN_SHIFT         7
#define __ADC_HC_AIEN_MASK         (0x1UL << __ADC_HC_AIEN_SHIFT)


/** \brief ADC转换时间周期 */
#define __ADC_CT_RATE_8BIT          25
#define __ADC_CT_RATE_10BIT         29
#define __ADC_CT_RATE_12BIT         33


/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __imx10xx_adc_inst_init2 (awbl_dev_t *p_dev);

aw_local aw_err_t __imx10xx_adc_serv_get (struct awbl_dev *p_dev, void *p_arg);

aw_local void __imx10xx_adc_isr (void *p_cookie);

aw_local aw_err_t __imx10xx_adc_start (void                 *p_cookie,
                                       aw_adc_channel_t      channel,
                                       aw_adc_buf_desc_t    *p_desc,
                                       int                   desc_num,
                                       uint32_t              count,
                                       pfn_adc_complete_t    pfn_seq_complete,
                                       void                 *p_arg);

aw_local aw_err_t __imx10xx_adc_stop (void                  *p_cookie,
                                      aw_adc_channel_t       channel);

aw_local aw_err_t __imx10xx_adc_bits_get (void *p_cookie, aw_adc_channel_t channel);
aw_local aw_err_t __imx10xx_adc_vref_get (void *p_cookie, aw_adc_channel_t channel);

aw_local aw_err_t __imx10xx_adc_rate_get (void               *p_cookie,
                                          aw_adc_channel_t    channel,
                                          uint32_t           *p_rate);

aw_local aw_err_t __imx10xx_adc_rate_set (void               *p_cookie,
                                          aw_adc_channel_t    channel,
                                          uint32_t            rate);

/*******************************************************************************
  loccals
*******************************************************************************/

/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_imx10xx_adc_drvfuncs = {
    NULL,
    __imx10xx_adc_inst_init2,
    NULL
};


/** \brief adc service functions (must defined as aw_const) */
aw_local aw_const struct awbl_adc_servfuncs __g_imx10xx_adc_servfuncs = {
    __imx10xx_adc_start,
    __imx10xx_adc_stop,
    __imx10xx_adc_bits_get,
    __imx10xx_adc_vref_get,
    __imx10xx_adc_rate_get,
    __imx10xx_adc_rate_set
};

/** \brief driver methods (must defined as aw_const) */
aw_local aw_const struct awbl_dev_method __g_imx10xx_adc_dev_methods[] = {
    AWBL_METHOD(awbl_adcserv_get, __imx10xx_adc_serv_get),
    AWBL_METHOD_END
};
/******************************************************************************/

/** \brief driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_imx10xx_adc_drv_registration = {
    {
        AWBL_VER_1,                        /** \brief awb_ver */
        AWBL_BUSID_PLB,                    /** \brief bus_id */
        AWBL_IMX10XX_ADC_NAME,             /** \brief p_drvname */
        &__g_imx10xx_adc_drvfuncs,         /** \brief p_busfuncs */
        &__g_imx10xx_adc_dev_methods[0],   /** \brief p_methods */
        NULL                               /** \brief pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief ADC self calibration
 */
aw_local aw_err_t __imx10xx_adc_calibration (imx10xx_adc_regs_t *p_hw_adc)
{

    /*  Clear the calibration failed flag */
    __ADC_REG_BIT_SET(&p_hw_adc->GS, __ADC_GS_CALF_SHIFT);

    /*  Launch the calibration */
    __ADC_REG_BIT_SET(&p_hw_adc->GC, __ADC_GC_CAL_SHIFT);

    /* Check the status of CALF bit in ADC_GS and the CAL bit in ADC_GC */
    while (__ADC_REG_BIT_ISSET(&p_hw_adc->GC, __ADC_GC_CAL_SHIFT)) {

        /*  Check the CALF when the calibration is active */
        if (__ADC_REG_BIT_ISSET(&p_hw_adc->GS, __ADC_GS_CALF_SHIFT)) {
            return AW_ERROR;
        }
    }

    /* When CAL bit becomes '0' then check the CALF status and COCO[0] bit status */
    if (!(__ADC_REG_READ(&p_hw_adc->HS) & __ADC_HS_COCO0_MASK)) {
        return AW_ERROR;
    }

    /* Check the CALF status */
    if (__ADC_REG_BIT_ISSET(&p_hw_adc->GS, __ADC_GS_CALF_SHIFT)) {
        return AW_ERROR;
    }

    /* Clear conversion done flag */
    __ADC_REG_READ(&p_hw_adc->R[0]);

    return AW_OK;
}


/* ADC初始化 */
aw_local void __imx10xx_adc_hard_init (awbl_imx10xx_adc_dev_t *p_dev)
{
    __ADC_DEVINFO_DECL(p_devinfo, p_dev);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    volatile int i = 0;
    uint32_t cfg   = 0;

    cfg = __ADC_REG_READ(&p_hw_adc->CFG) & (__ADC_CFG_AVGS_MASK | __ADC_CFG_ADTRG_MASK);
    cfg |= __ADC_CFG_REFSEL(0)  |  /* Selects VREFH/VREFL as reference voltage */
           __ADC_CFG_ADSTS(0)   |  /* Long sample 12 clocks or short sample 2 clocks */
           __ADC_CFG_ADICLK(1)  |  /* Input Clock Select is IPG clock */
           __ADC_CFG_ADIV(3)    |  /* Input clock / 8 */
           __ADC_CFG_MODE((p_devinfo->bits - 8) / 2);
    __ADC_REG_WRITE(&p_hw_adc->CFG, cfg);

    __ADC_REG_WRITE(&p_hw_adc->GC, 0X00);

    for (i = 0; i < 10; i++) {
        /* ADC self calibration */
        if(AW_OK == __imx10xx_adc_calibration(p_hw_adc)) {
            break;
        }
    }
}


/**
 * \brief ADC 第2阶段初始化
 */
aw_local void __imx10xx_adc_inst_init2 (awbl_dev_t *p_dev)
{
    __ADC_DEV_DECL(p_this, p_dev);
    __ADC_DEVINFO_DECL(p_devinfo, p_this);

    /* 平台相关初始化  */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    p_this->bufdesc_num      = 0;
    p_this->cur_bufdesc_id   = 0;
    p_this->phy_chan         = 0;
    p_this->is_stopped       = AW_TRUE;  /* 未启动，处于停止态*/
    p_this->pfn_seq_complete = NULL;
    p_this->p_arg            = NULL;
    p_this->p_bufdesc        = NULL;
    p_this->remain_count     = 0;
    p_this->samples          = 0;

    /* ADC初始化 */
    __imx10xx_adc_hard_init(p_this);

    /* 连接并使能中断 */
    aw_int_connect(p_devinfo->inum, __imx10xx_adc_isr, p_dev);
    aw_int_enable(p_devinfo->inum);
}

/**
 * \brief "awbl_adcserv_get" 方法实现
 */
aw_local aw_err_t __imx10xx_adc_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __ADC_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_adc_service *p_serv = NULL;

    /* get adc service instance */
    p_serv = &((awbl_imx10xx_adc_dev_t *)p_dev)->adc_serv;

    /* initialize the adc service instance */
    p_serv->p_servinfo  = &p_devinfo->adc_servinfo;
    p_serv->p_servfuncs = &__g_imx10xx_adc_servfuncs;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_adc_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief ADC 启动通道转换
 */
aw_local void __imx10xx_adc_start_convert (struct awbl_imx10xx_adc_dev *p_dev,
                                           uint8_t                      channel)
{
    __ADC_DEVINFO_DECL(p_devinfo, p_dev);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    uint32_t reg = 0;

    /* 配置转换通道 */
    reg = __ADC_HC_ADCH(channel) |
          __ADC_HC_AIEN_MASK;    /* Conversion complete interrupt enabled */

    __ADC_REG_WRITE(&p_hw_adc->HC[0], reg);
}


/**
 * \brief ADC 转换结果数据填充
 */
aw_local void __imx10xx_adc_data_fill (struct awbl_imx10xx_adc_dev *p_this,
                                        uint16_t                      data)
{
    aw_adc_buf_desc_t  *p_desc = &(p_this->p_bufdesc[p_this->cur_bufdesc_id]);

    aw_bool_t is_continue = AW_FALSE;
    uint16_t *p_buf       = (uint16_t *)p_desc->p_buf;

    p_buf[p_this->samples++] = data;
    if (4095 != data) {
        p_buf = (uint16_t *)p_desc->p_buf;
    }

    /* 当前缓冲区转换完毕 */
    if (p_this->samples == p_desc->length) {
        pfn_adc_complete_t pfn_tmp = p_desc->pfn_complete;

        if (pfn_tmp) {
            pfn_tmp(p_desc->p_arg, AW_OK);
        }

        /* 序列中最后一个缓冲区,序列正常结束 */
        if (p_this->cur_bufdesc_id == p_this->bufdesc_num - 1) {

            if (p_this->remain_count == 0) {     /* 持续转换 */
                is_continue = AW_TRUE;
            } else {
                if (p_this->remain_count == 1) { /* 最后一个的count=1,转换完毕 */
                    is_continue = AW_FALSE;

                } else {
                    p_this->remain_count--;
                    is_continue = AW_TRUE;
                }
            }

            /* 序列转换完，send msg OK */
            if (p_this->pfn_seq_complete) {
                p_this->pfn_seq_complete(p_this->p_arg, AW_OK);
            }

        } else {
            is_continue = AW_TRUE;
        }

        /* 当前描述缓冲区传输完成，进行下一个描述缓冲区的传输 */
        p_this->samples = 0;
        p_this->cur_bufdesc_id++;
        if (p_this->cur_bufdesc_id == p_this->bufdesc_num) {
            p_this->cur_bufdesc_id = 0;
        }

    } else {
        is_continue = AW_TRUE;
    }

    if (is_continue == AW_TRUE) {
        __imx10xx_adc_start_convert(p_this, p_this->phy_chan);
    }
}


/**
 * \brief ADC 转换完成中断服务
 */
aw_local void __imx10xx_adc_isr (void *p_cookie)
{
    __ADC_DEV_DECL(p_this, p_cookie);
    __ADC_DEVINFO_DECL(p_devinfo, p_this);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    volatile uint32_t data = 0;

    /* 转换已被停止 */
    if (p_this->is_stopped == AW_TRUE) {
        return;
    }

    /* 参数无效 */
    if (p_this->bufdesc_num == 0 || p_this->p_bufdesc == NULL) {
        return;
    }

    /* 转换完成*/
    if ((__ADC_REG_READ(&p_hw_adc->HS) & __ADC_HS_COCO0_MASK)) {

        /* 禁能转换完成中断 */
        __ADC_REG_BIT_CLR(&p_hw_adc->HC[0], __ADC_HC_AIEN_SHIFT);

        /* 读取数据数据 */
        data = __ADC_REG_READ(&p_hw_adc->R[0]);
        __imx10xx_adc_data_fill(p_this, data);
    }
}



/**
 * \brief ADC 转换开始
 */
aw_local aw_err_t __imx10xx_adc_start (void                 *p_cookie,
                                       aw_adc_channel_t      channel,
                                       aw_adc_buf_desc_t    *p_bufdesc,
                                       int                   bufdesc_num,
                                       uint32_t              count,
                                       pfn_adc_complete_t    pfn_seq_complete,
                                       void                 *p_arg)
{
    __ADC_DEV_DECL(p_this, p_cookie);

    /* 最大通道号为15 */
    if (channel > IMX10xx_ADC_MAX_CH) {
        return -AW_ENXIO;
    }

    /* 参数有效性检测 */
    if (p_bufdesc == NULL || bufdesc_num == 0 || p_cookie == NULL) {
        return -AW_EINVAL;
    }

    p_this->phy_chan         = channel;
    p_this->samples          = 0;
    p_this->p_bufdesc        = p_bufdesc;
    p_this->bufdesc_num      = bufdesc_num;
    p_this->remain_count     = count;
    p_this->is_stopped       = AW_FALSE;
    p_this->cur_bufdesc_id   = 0;
    p_this->pfn_seq_complete = pfn_seq_complete;
    p_this->p_arg            = p_arg;

    /* 启动通道转换 */
    __imx10xx_adc_start_convert(p_this, channel);

    return AW_OK;
}

/**
 * \brief ADC 转换停止
 */
aw_local aw_err_t __imx10xx_adc_stop (void               *p_cookie,
                                      aw_adc_channel_t    channel)
{
    __ADC_DEV_DECL(p_this, p_cookie);
    __ADC_DEVINFO_DECL(p_devinfo, p_this);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    aw_adc_channel_t  phy_ch = channel - p_devinfo->adc_servinfo.ch_min;

    /* 检查通道号 */
    if (phy_ch > IMX10xx_ADC_MAX_CH ) {
        return -AW_ENXIO;
    }

    /* 当前转换的通道不为需要停止的通道 */
    if (p_this->phy_chan != phy_ch) {
        return -AW_EPERM;
    }

    /* 停止转换 */
    __ADC_REG_BIT_CLR(&p_hw_adc->HC[0], __ADC_HC_ADCH(0x1F));

    /* 标识转换被停止 */
    p_this->is_stopped = AW_TRUE;
    return AW_OK;
}

/**
 * \brief 获取ADC 通道位宽 bits
 */
aw_local aw_err_t __imx10xx_adc_bits_get (void *p_cookie, aw_adc_channel_t channel)
{
    __ADC_DEVINFO_DECL(p_devinfo, p_cookie);

    return p_devinfo->bits;
}

/**
 * \brief 获取ADC采样率
 */
aw_local aw_err_t __imx10xx_adc_rate_get (void               *p_cookie,
                                          aw_adc_channel_t    channel,
                                          uint32_t           *p_rate)
{
    __ADC_DEV_DECL(p_this, p_cookie);
    __ADC_DEVINFO_DECL(p_devinfo, p_this);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    uint32_t  div      = 0;
    uint32_t  clk_in   = 0;
    uint32_t  multiple = 0;

    if (NULL == p_cookie) {
        return -AW_EINVAL;
    }

    clk_in = aw_clk_rate_get(p_devinfo->clk_id);
    div = __ADC_REG_BITS_GET(&p_hw_adc->CFG, __ADC_CFG_ADIV_SHIFT, 2);
    div = (1 << div);

    /* clk source use IPG /2 ? */
    if ((p_hw_adc->CFG & __ADC_CFG_ADICLK_MASK) == 0x01) {
        div = div * 2;
    }

    /* 查找采样倍率 */
    switch(__imx10xx_adc_bits_get(p_cookie, channel)) {

    case 8:
        multiple = __ADC_CT_RATE_8BIT;
        break;

    case 10:
        multiple = __ADC_CT_RATE_10BIT;
        break;

    case 12:
        multiple = __ADC_CT_RATE_12BIT;
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    *p_rate =  (clk_in / div) / multiple;

    return AW_OK;
}


/**
 * \brief 设置ADC采样率
 *
 *  |--SFCAdder--|-------BCT------|------LSTAddr-------|
 *
 *    SFCAdder   = 4ADCK
 *    AverageNum = 1 （不使用硬件平均）
 *    BCT(8bit)  = 17ADCK ** BCT(10bit) = 21ADCK ** BCT(12bit) = 25ADCK
 *    LSTAddr    = 3 ADCK （选择短采样模式 ADLSMP = 0, ADSTS = 00）
 *
 *    conversion Timer = SFCAdder + AverageNum * (BCT + LSTAddr)
 *    = 25(8bit) 29(10bit) 33(12bit)
 */
aw_local aw_err_t __imx10xx_adc_rate_set (void               *p_cookie,
                                          aw_adc_channel_t    channel,
                                          uint32_t            rate)
{
    __ADC_DEV_DECL(p_this, p_cookie);
    __ADC_DEVINFO_DECL(p_devinfo, p_this);
    __ADC_HW_DECL(p_hw_adc, p_devinfo);

    uint32_t  clk_in    = 0;
    uint8_t   div       = 0;
    uint8_t   adiv      = 0;
    uint8_t   multiple  = 0;
    uint32_t  last_rate = 0;

    if (NULL == p_cookie) {
        return -AW_EINVAL;
    }

    /* 最大采样率为1Msps */
    if (rate >= 1000000) {
        rate = 1000000;
    }

    clk_in = aw_clk_rate_get(p_devinfo->clk_id);

    /* clk source use IPG /2 ? */
    if ((p_hw_adc->CFG & __ADC_CFG_ADICLK_MASK) == 0x01) {
        clk_in /= 2;
    }

    /* 查找采样倍率 */
    switch(__imx10xx_adc_bits_get(p_cookie, channel)) {

    case 8:
        multiple = __ADC_CT_RATE_8BIT;
        break;

    case 10:
        multiple = __ADC_CT_RATE_10BIT;
        break;

    case 12:
        multiple = __ADC_CT_RATE_12BIT;
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    __imx10xx_adc_rate_get(p_cookie, channel, &last_rate);

    /* 不用再重复设置相同的采样率 */
    if (rate == last_rate) {
        return AW_OK;
    }

    /* 可实现输入时钟 1 2 4 8 分频*/
    div = clk_in / (rate * multiple);

    if (div >= 8) {
        adiv = 0x03;
    } else if (div >= 4) {
        adiv = 0x02;
    } else if (div >= 2) {
        adiv = 0x01;
    } else {
        adiv = 0;
    }

    __ADC_REG_BITS_SET(&p_hw_adc->CFG, __ADC_CFG_ADIV_SHIFT, 2, adiv);

    return AW_OK;
}


/**
 * \brief 获取ADC 参考电压
 */
aw_local aw_err_t __imx10xx_adc_vref_get (void *p_cookie, aw_adc_channel_t channel)
{
    __ADC_DEVINFO_DECL(p_devinfo, p_cookie);

    return p_devinfo->refmv;
}

/******************************************************************************/

/**
 * \brief ADC 驱动注册
 */
void awbl_imx10xx_adc_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_imx10xx_adc_drv_registration);
}


/* end of file */
