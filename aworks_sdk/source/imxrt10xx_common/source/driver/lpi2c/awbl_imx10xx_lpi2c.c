/*******************************************************************************
*                                 AWorks
*
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
 * \brief imx10xx lpi2c driver
 *
 * \internal
 * \par modification history:
 * - 2.00 19-03-18  lgg, according to new i2c frame, rewrite lpi2c driver
 * - 1.00 17-10-18  sow, first implemetation
 * \endinternal
 */

#include "aworks.h"
#include "aw_exc.h"
#include "aw_errno.h"
#include "aw_vdebug.h"
#include "aw_system.h"
#include "aw_psp_delay.h"
#include "aw_psp_task.h"
#include "aw_sem.h"
#include "aw_clk.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "aw_common.h"
#include "aw_int.h"
#include "string.h"
#include "awbl_plb.h"
#include "aw_i2c.h"
#include "aw_bitops.h"
#include "aw_spinlock.h"
#include "aw_types.h"
#include "driver/lpi2c/awbl_imx10xx_lpi2c.h"
#include "driver/lpi2c/imx10xx_lpi2c_regs.h"


/** \brief  lpi2c debug mode macro */
//#define IMX10XX_I2C_DEBUG

#define __I2C_MASTER_RESET_TIME         1       /* ms */
#define __IMXI2C_WAIT_IDEL_TIMEOUT_MS   1000    /* ms */

/** \macro flag */
#define __LPI2C_FIFO_SIZE               0x04 /* imx10xx lpi2c FIFO size */
#define __WR_DATA_FIFO_FULL             0x01 /* FIFO is full when write data*/
#define __WR_DATA_NO_OVER               0x08 /* write data not over */
#define __RD_DATA_FIFO_EMPTY            0x02 /* FIFO is empty when read data */

/*******************************************************************************
  macro operate
*******************************************************************************/
/* write register */
#define __LPI2C_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __LPI2C_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* get lpi2c master instance */
#define __LPI2C_MASTER_DEV_DECL(p_master, p_dev) \
    struct awbl_i2c_master *p_master = (struct awbl_i2c_master *)(p_dev)

/* get imx10xx lpi2c device instance */
#define __LPI2C_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_lpi2c_dev *p_this = \
        (struct awbl_imx10xx_lpi2c_dev *)(p_dev)

/* get imx10xx lpi2c device infomation */
#define __LPI2C_DEVINFO_DECL(p_this_dvif, p_dev) \
    struct awbl_imx10xx_lpi2c_devinfo *p_this_dvif = \
        (struct awbl_imx10xx_lpi2c_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get imx10xx lpi2c hardware infomation */
#define __LPI2C_HW_DECL(p_hw_lpi2c, p_devinfo)    \
        __imx10xx_lpi2c_reg_t *p_hw_lpi2c =            \
            ((__imx10xx_lpi2c_reg_t *)((p_devinfo)->regbase))

/** \brief MCR - Master Control Register */
#define LPI2C_MCR_MEN                   (0x01)
#define LPI2C_MCR_RST                   (0x01 << 1)
#define LPI2C_MCR_DEBUG                 (0x01 << 3)

/*! @name MCCR0 - Master Clock Configuration Register 0 */
#define LPI2C_MCCR0_CLKLO_MASK          (0x3FU)
#define LPI2C_MCCR0_CLKLO_SHIFT         (0U)
#define LPI2C_MCCR0_CLKLO(x)            (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCCR0_CLKLO_SHIFT)) &     \
                                        LPI2C_MCCR0_CLKLO_MASK)
#define LPI2C_MCCR0_CLKHI_MASK          (0x3F00U)
#define LPI2C_MCCR0_CLKHI_SHIFT         (8U)
#define LPI2C_MCCR0_CLKHI(x)            (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCCR0_CLKHI_SHIFT)) &     \
                                        LPI2C_MCCR0_CLKHI_MASK)
#define LPI2C_MCCR0_SETHOLD_MASK        (0x3F0000U)
#define LPI2C_MCCR0_SETHOLD_SHIFT       (16U)
#define LPI2C_MCCR0_SETHOLD(x)          (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCCR0_SETHOLD_SHIFT)) &   \
                                        LPI2C_MCCR0_SETHOLD_MASK)
#define LPI2C_MCCR0_DATAVD_MASK         (0x3F000000U)
#define LPI2C_MCCR0_DATAVD_SHIFT        (24U)
#define LPI2C_MCCR0_DATAVD(x)           (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCCR0_DATAVD_SHIFT)) &    \
                                        LPI2C_MCCR0_DATAVD_MASK)
/*! @name MCFGR1 - Master Configuration Register 1 */
#define LPI2C_MCFGR1_PRESCALE_MASK      (0x7U)
#define LPI2C_MCFGR1_PRESCALE_SHIFT     (0U)
#define LPI2C_MCFGR1_PRESCALE(x)        (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCFGR1_PRESCALE_SHIFT)) & \
                                        LPI2C_MCFGR1_PRESCALE_MASK)
/*! @name MCFGR2 - Master Configuration Register 2 */
#define LPI2C_MCFGR2_BUSIDLE_MASK       (0xFFFU)
#define LPI2C_MCFGR2_BUSIDLE_SHIFT      (0U)
#define LPI2C_MCFGR2_BUSIDLE(x)         (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCFGR2_BUSIDLE_SHIFT)) &  \
                                        LPI2C_MCFGR2_BUSIDLE_MASK)
#define LPI2C_MCFGR2_FILTSCL_MASK       (0xF0000U)
#define LPI2C_MCFGR2_FILTSCL_SHIFT      (16U)
#define LPI2C_MCFGR2_FILTSCL(x)         (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCFGR2_FILTSCL_SHIFT)) &  \
                                        LPI2C_MCFGR2_FILTSCL_MASK)
#define LPI2C_MCFGR2_FILTSDA_MASK       (0xF000000U)
#define LPI2C_MCFGR2_FILTSDA_SHIFT      (24U)
#define LPI2C_MCFGR2_FILTSDA(x)         (((uint32_t)(((uint32_t)(x)) << \
                                        LPI2C_MCFGR2_FILTSDA_SHIFT)) &  \
                                        LPI2C_MCFGR2_FILTSDA_MASK)
/*! @name MCFGR3 - Master Configuration Register 3 */
#define LPI2C_MCFGR3_PINLOW_MASK        (0xFFF00U)
#define LPI2C_MCFGR3_PINLOW_SHIFT       (8U)
#define LPI2C_MCFGR3_PINLOW(x)          (((uint32_t)(((uint32_t)(x)) << \
                                         LPI2C_MCFGR3_PINLOW_SHIFT)) &   \
                                         LPI2C_MCFGR3_PINLOW_MASK)
/*! @name MSR - Master Status Register */
#define LPI2C_MSR_TDF_MASK              (1U << 0)   /* Transmit data flag */
#define LPI2C_MSR_RDF_MASK              (1U << 1)   /* Receive data flag */
#define LPI2C_MSR_EPF_MASK              (1U << 8)   /* End Packet flag */
#define LPI2C_MSR_SDF_MASK              (1U << 9)   /* Stop detect flag */
#define LPI2C_MSR_NDF_MASK              (1U << 10)  /* NACK detect flag */
#define LPI2C_MSR_ALF_MASK              (1U << 11)  /* Arbitration lost flag */
#define LPI2C_MSR_FEF_MASK              (1U << 12)  /* FIFO error flag */
#define LPI2C_MSR_PLTF_MASK             (1U << 13)  /* Pin low timeout flag */
#define LPI2C_MSR_DMF_MASK              (1U << 14)  /* Data match flag */
#define LPI2C_MSR_MBF_MASK              (1U << 24)  /* Master busy flag */
#define LPI2C_MSR_BBF_MASK              (1U << 25)  /* Bus busy flag */
#define LPI2C_MSR_ABNM_MASK             (0xf << 10) /* MSR abnormal bits mask*/

/*! @name MTDR - Master Transmit Data Register */
#define LPI2C_CMD_TRANSMIT              (0x0ff)     /* transmit data */

#define __IMX10XX_LPI2C_DISABLE_IT(p_hw_lpi2c, flag) {             \
        uint32_t reg = __LPI2C_REG_READ(&(p_hw_lpi2c->mier));      \
        reg &= ~(flag);                                            \
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mier), reg);               \
}

#define __IMX10XX_LPI2C_ENABLE_IT(p_hw_lpi2c, flag) {             \
        uint32_t reg = __LPI2C_REG_READ(&(p_hw_lpi2c->mier));     \
        reg |= (flag);                                            \
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mier), reg);              \
}

/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __lpi2c_inst_init1(awbl_dev_t *p_dev);
aw_local void __lpi2c_inst_init2(awbl_dev_t *p_dev);
aw_local void __lpi2c_inst_connect(awbl_dev_t *p_dev);
aw_local void __lpi2c_isr(void *p_arg);
aw_local void __lpi2c_hard_init(struct awbl_imx10xx_lpi2c_dev  *p_this);

aw_local aw_err_t  __imx10xx_lpi2c_write(struct awbl_i2c_master  *p_master,
                                         uint16_t                 chip_addr,
                                         struct awbl_trans_buf   *p_trans_buf,
                                         uint32_t                 trans_buf_num,
                                         uint8_t                  stop_ctl,
                                         uint16_t                 dr_flag);

aw_local aw_err_t  __imx10xx_lpi2c_read(struct awbl_i2c_master *p_master,
                                        uint16_t                chip_addr,
                                        struct awbl_trans_buf  *p_trans_buf,
                                        uint8_t                 start_ctl,
                                        uint16_t                dr_flag);

aw_local aw_err_t  __imx10xx_lpi2c_set_cfg(struct awbl_i2c_master *p_master,
                                           struct aw_i2c_config   *p_cfg);

aw_local aw_err_t  __imx10xx_lpi2c_get_cfg(struct awbl_i2c_master *p_master,
                                           struct aw_i2c_config   *p_cfg);

aw_local aw_err_t __check_usr_cfg_par(struct aw_i2c_config *p_cfg);

/*******************************************************************************
  locals
*******************************************************************************/
/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs g_lpi2c_awbl_drvfuncs = {
        __lpi2c_inst_init1,
        __lpi2c_inst_init2,
        __lpi2c_inst_connect
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_lpi2c_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL ,   /* bus_id */
        AWBL_IMX10XX_LPI2C_NAME,                /* p_drvname */
        &g_lpi2c_awbl_drvfuncs,                 /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const \
    struct awbl_i2c_master_devinfo2 __g_imx10xx_lpi2c_master_devinfo2 = {
        /* i2c slave device address features */
        AWBL_FEATURE_I2C_7BIT | AWBL_FEATURE_I2C_10BIT,
        __imx10xx_lpi2c_write,
        __imx10xx_lpi2c_read,
        __imx10xx_lpi2c_set_cfg,
        __imx10xx_lpi2c_get_cfg,
    };

/*******************************************************************************
  implementation
*******************************************************************************/
/**
 * \brief   first level initialization routine
 *
 * \par[in] p_dev     device instance
 */
aw_local void __lpi2c_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief   check the initial configure infomation
 */
aw_local void __check_init_cfg(awbl_dev_t *p_dev)
{
    __LPI2C_DEV_DECL(p_this, p_dev);
    __LPI2C_DEVINFO_DECL(p_devinfo, p_this);

    /* initialize controller hardware */
    p_this->i2c_cfg_par.bus_idle_time = p_devinfo->bus_idle_timeout / 1000;
    p_this->i2c_cfg_par.timeout = p_devinfo->lpi2c_master_devinfo.timeout / 1000;
    p_this->i2c_cfg_par.speed = p_devinfo->lpi2c_master_devinfo.speed;

    if (__check_usr_cfg_par(&p_this->i2c_cfg_par)) {
        p_this->i2c_cfg_par.speed         = 200000;     /* default 200KHZ */
        p_this->i2c_cfg_par.timeout       = 1000000;    /* default 1ms */
        p_this->i2c_cfg_par.bus_idle_time = 60000;      /* default 60us */
    } else {
        p_this->i2c_cfg_par.bus_idle_time = p_devinfo->bus_idle_timeout;
        p_this->i2c_cfg_par.timeout = p_devinfo->lpi2c_master_devinfo.timeout;
    }
}

/**
 * \brief   second level initialization routine
 *
 * \par[in] p_dev     device instance
 */
aw_local void __lpi2c_inst_init2(awbl_dev_t *p_dev)
{
    __LPI2C_MASTER_DEV_DECL(p_master, p_dev);
    __LPI2C_DEV_DECL(p_this, p_dev);
    __LPI2C_DEVINFO_DECL(p_devinfo, p_this);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* AWBus lpi2c master general init2 */
    p_master->p_devinfo2 = &__g_imx10xx_lpi2c_master_devinfo2;

    /* initialize the binary semaphore */
    AW_SEMB_INIT(p_this->i2c_sem, 0, AW_SEM_Q_FIFO);

    __check_init_cfg(p_dev);
    __lpi2c_hard_init(p_this);

    /* initialize the err count */
    p_this->err_count = 0;

    /* connect and enable ISR */
    aw_int_connect(p_devinfo->inum,
                  (aw_pfuncvoid_t)__lpi2c_isr,
                  (void *)p_dev);
    aw_int_enable(p_devinfo->inum);

    /* create lpi2c bus */
    awbl_i2cbus_create((struct awbl_i2c_master *)p_dev);
}

/**
 * \brief   third level initialization routine
 *
 * \par[in] p_dev     device instance
 */
aw_local void __lpi2c_inst_connect(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief   obtain cycles by for-width
 *
 * \par[in] source_clock_hz: lpi2c module clock
 * \par[in] width_ns:        width_ns value
 * \par[in] max_cycles:      max cycles value
 * \par[in] prescaler:       prescaler value
 *
 * \ret     cycles           get by width_ns
 */
aw_local uint32_t __lpi2c_get_cycles_forwidth(uint32_t source_clock_hz,
                                              uint32_t width_ns,
                                              uint32_t max_cycles,
                                              uint32_t prescaler)
{
    uint32_t cycles = 0;
    uint32_t busCycle_ns =0;

    if (!prescaler) {
        prescaler = 1;
    }

    busCycle_ns = 1000000 / (source_clock_hz / prescaler / 1000);

    /* Search for the cycle count just below the desired glitch width. */
    while ((((cycles + 1) * busCycle_ns) < width_ns) &&
             (cycles + 1 < max_cycles))
    {
        ++cycles;
    }

    /* If we end up with zero cycles, then set the filter to
     * a single cycle unless the                               */
    /* bus clock is greater than 10x the desired glitch width. */
    if ((cycles == 0) && (busCycle_ns <= (width_ns * 10)))
    {
        cycles = 1;
    }

    return cycles;
}

/**
 * \brief   set speed
 *
 * \par[in] lpi2c register base address
 * \par[in] lpi2c module clock
 * \par[in] lpi2c send and receive data clock
 */
aw_local void __lpi2c_master_set_baudrate(__imx10xx_lpi2c_reg_t *p_base,
                                          uint32_t               source_clock_hz,
                                          uint32_t               baud_rate_hz)
{
    int      i             = 0;
    uint8_t  was_enabled   = 0;
    uint32_t value         = 0;
    uint32_t prescale      = 0;
    uint32_t best_pre      = 0;
    uint32_t abs_error     = 0;
    uint32_t best_error    = 0xffffffffu;
    uint32_t best_clk_hi   = 0;
    uint32_t clk_hi_cycle  = 0;
    uint32_t computed_rate = 0;

    /* Disable master mode. */
    value = __LPI2C_REG_READ(&(p_base->mcr));
    was_enabled = (value & 0x01);
    value &= ~(LPI2C_MCR_MEN);

    /* Master disable */
    __LPI2C_REG_WRITE(&(p_base->mcr), value);

    /*
     * Baud rate = (source_clock_hz/2^prescale) /
     * (CLKLO+1+CLKHI+1 + ROUNDDOWN((2+FILTSCL)/2^prescale)
     */
    /* Assume CLKLO = 2*CLKHI, SETHOLD = CLKHI, DATAVD = CLKHI/2. */
    for (prescale = 1;
        (prescale <= 128) && (best_error != 0);
         prescale = 2 * prescale) {
        for (clk_hi_cycle = 1; clk_hi_cycle < 32; clk_hi_cycle++) {
            if (clk_hi_cycle == 1) {
                computed_rate = (source_clock_hz / prescale) /
                                (1 + 3 + 2 + 2 / prescale);
            }
            else {
                computed_rate = (source_clock_hz / prescale) /
                                (3 * clk_hi_cycle + 2 + 2 / prescale);
            }
            abs_error = baud_rate_hz > computed_rate ?
                       baud_rate_hz - computed_rate :
                       computed_rate - baud_rate_hz;
            if (abs_error < best_error)
            {
                best_pre = prescale;
                best_clk_hi = clk_hi_cycle;
                best_error = abs_error;
                /* If the error is 0, then we can stop searching
                 * because we won't find a better match.          */
                if (abs_error == 0)
                {
                    break;
                }
            }
        }
    }
    /* Standard, fast, fast mode plus and ultra-fast transfers. */
    value = LPI2C_MCCR0_CLKHI(best_clk_hi);
    if (best_clk_hi < 2)
    {
        value |= LPI2C_MCCR0_CLKLO(3)   |
                 LPI2C_MCCR0_SETHOLD(2) |
                 LPI2C_MCCR0_DATAVD(1);
    }
    else
    {
        value |= LPI2C_MCCR0_CLKLO(2 * best_clk_hi) |
                 LPI2C_MCCR0_SETHOLD(best_clk_hi)   |
                 LPI2C_MCCR0_DATAVD(best_clk_hi / 2);
    }
    __LPI2C_REG_WRITE(&(p_base->mccr0), value);
    for (i = 0; i < 8; i++)
    {
        if (best_pre == (1U << i))
        {
            best_pre = i;
            break;
        }
    }
    value = __LPI2C_REG_READ(&(p_base->mcfgr1));
    value = (value & ~0x7U) |
             LPI2C_MCFGR1_PRESCALE(best_pre);
    __LPI2C_REG_WRITE(&(p_base->mcfgr1), value);

    /* Restore master mode. */
    if (was_enabled)
    {
        value = __LPI2C_REG_READ(&(p_base->mcr));
        value |= (LPI2C_MCR_MEN);
        __LPI2C_REG_WRITE(&(p_base->mcr), value);
    }
}

/**
 * \brief   Configure MCFGRX -- Master Configuration Register
 *          glitch filters and bus idle and pin low timeouts.
 */
aw_local void __lpi2c_cfg_features(struct awbl_imx10xx_lpi2c_dev *p_this)
{
    uint32_t value     = 0;
    uint32_t cfgr2     = 0;
    uint32_t cfgr3     = 0;
    uint32_t cycles    = 0;
    uint32_t clk_in    = 0;
    uint32_t prescaler = 0;
    uint8_t  was_enabled = 0;

    __LPI2C_DEVINFO_DECL(p_devinfo, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_devinfo);

    /* Disable master mode. */
    value = __LPI2C_REG_READ(&(p_hw_lpi2c->mcr));
    was_enabled = (value & 0x01);
    value &= ~(LPI2C_MCR_MEN);

    /* Master disable */
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), value);

    /* Configure glitch filters and bus idle and pin low timeouts. */
    prescaler = __LPI2C_REG_READ(&(p_hw_lpi2c->mcfgr1));
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcfgr1),
                      prescaler |     \
                      (1 << 10) );
    prescaler = (prescaler & LPI2C_MCFGR1_PRESCALE_MASK) >>
                 LPI2C_MCFGR1_PRESCALE_SHIFT;
    cfgr2 = __LPI2C_REG_READ(&(p_hw_lpi2c->mcfgr2));
    clk_in = aw_clk_rate_get(p_devinfo->ref_clk_id);

    if (p_this->i2c_cfg_par.bus_idle_time) {
        cycles = __lpi2c_get_cycles_forwidth(clk_in,
                                             p_this->i2c_cfg_par.bus_idle_time,
                                             (LPI2C_MCFGR2_BUSIDLE_MASK >>
                                             LPI2C_MCFGR2_BUSIDLE_SHIFT),
                                             prescaler);

        cfgr2 &= ~LPI2C_MCFGR2_BUSIDLE_MASK;
        cfgr2 |= LPI2C_MCFGR2_BUSIDLE(cycles);
    }

    if (p_devinfo->sda_filter_wide) {
        cycles = __lpi2c_get_cycles_forwidth(clk_in,
                                             p_devinfo->sda_filter_wide,
                                            (LPI2C_MCFGR2_FILTSDA_MASK >>
                                             LPI2C_MCFGR2_FILTSDA_SHIFT),
                                             1);
        cfgr2 &= ~LPI2C_MCFGR2_FILTSDA_MASK;
        cfgr2 |= LPI2C_MCFGR2_FILTSDA(cycles);
    }

    if (p_devinfo->scl_filter_wide) {
        cycles = __lpi2c_get_cycles_forwidth(clk_in,
                                             p_devinfo->scl_filter_wide,
                                            (LPI2C_MCFGR2_FILTSCL_MASK >>
                                             LPI2C_MCFGR2_FILTSCL_SHIFT),
                                             1);
        cfgr2 &= ~LPI2C_MCFGR2_FILTSCL_MASK;
        cfgr2 |= LPI2C_MCFGR2_FILTSCL(cycles);
    }
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcfgr2), cfgr2);

    if (p_this->i2c_cfg_par.timeout) {
        cycles = __lpi2c_get_cycles_forwidth(clk_in,
                                             p_this->i2c_cfg_par.timeout / 256,
                                             (LPI2C_MCFGR2_BUSIDLE_MASK >>
                                             LPI2C_MCFGR2_BUSIDLE_SHIFT),
                                             prescaler);
        cfgr3 = __LPI2C_REG_READ(&(p_hw_lpi2c->mcfgr3));
        cfgr3 = (cfgr3 & ~LPI2C_MCFGR3_PINLOW_MASK) |
                 LPI2C_MCFGR3_PINLOW(cycles);
    }
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcfgr3), cfgr3);

    /* Restore master mode. */
    if (was_enabled) {
        value = __LPI2C_REG_READ(&(p_hw_lpi2c->mcr));
        value |= (LPI2C_MCR_MEN);
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), value);
    }
}

/**
 * \brief LPI2C controller hardware initialize
 */
aw_local void __lpi2c_hard_init (struct awbl_imx10xx_lpi2c_dev *p_this)
{
    uint32_t value;

    __LPI2C_DEVINFO_DECL(p_devinfo, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_devinfo);


    /* Software Reset and reset FIFO*/
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), LPI2C_MCR_RST |
                                          (1 << 8)      |
                                          (1 << 9));
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), 0x00);

#ifdef IMX10XX_I2C_DEBUG
    /* Enable master mode and debug mode*/
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), LPI2C_MCR_MEN | LPI2C_MCR_DEBUG);
#else
    /* Enable master mode */
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), LPI2C_MCR_MEN);
#endif

    /* Host Request  */
    value =0x00;
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcfgr0), value);

    /* configure lpi2c clock */
    __lpi2c_master_set_baudrate(p_hw_lpi2c,
                                aw_clk_rate_get(p_devinfo->ref_clk_id),
                                p_this->i2c_cfg_par.speed);

    /* FIFO water mark */
    value = 0x00;
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mfcr), value);

    /* Configure glitch filters and bus idle and pin low timeouts. */
    __lpi2c_cfg_features(p_this);

    /* Enable master mode */
    value = __LPI2C_REG_READ(&(p_hw_lpi2c->mcr));
    value |= (LPI2C_MCR_MEN);
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mcr), value);
}

/**
 * \brief   just send data to i2c slave device, not send stop signal.
 */
aw_local int __send_data_isr(void *p_arg)
{
    __LPI2C_DEV_DECL(p_this, p_arg);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_arg);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    uint8_t     tmp_data  = 0x00;
    uint32_t    fifo_stat = 0U;

    while (p_this->data_ptr < \
           p_this->p_trans_buf[p_this->cur_trans_index].buf_size) {

        fifo_stat = __LPI2C_REG_READ(&(p_hw_lpi2c->mfsr)) & 0x7U;
        if (fifo_stat < __LPI2C_FIFO_SIZE) {
            tmp_data = \
            p_this->p_trans_buf[p_this->cur_trans_index].p_buf[p_this->data_ptr++];
            __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), (tmp_data & LPI2C_CMD_TRANSMIT));
        } else {
            return __WR_DATA_FIFO_FULL;
        }
    }

    /* transmit data over? */
    if (p_this->cur_trans_index < p_this->trans_buf_num -1) {
        p_this->data_ptr = 0;
        p_this->cur_trans_index++;
        return __WR_DATA_NO_OVER;
    }

    /*
     * if a stop signal is sent.
     *
     * if the FIFO is full, return __WR_DATA_FIFO_FULL, wait for the next
     * interrupt, if the next interrupt is not available, an exception must
     * have occurred.
     */
    if (p_this->stop_ctrl_flg) {
        fifo_stat = __LPI2C_REG_READ(&(p_hw_lpi2c->mfsr)) & 0x7U;
        if (fifo_stat >= __LPI2C_FIFO_SIZE) {
            return __WR_DATA_FIFO_FULL;
        }
        /* manually send stop signal */
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), (0x02ul << 8));
    }

    p_this->p_trans_buf = NULL;
    p_this->cur_trans_index = 0;
    p_this->data_ptr = 0;

    return AW_OK;
}

/**
 * \brief   receive data from i2c slave device
 */
aw_local int __rcv_data_isr(void *p_arg)
{
    __LPI2C_DEV_DECL(p_this, p_arg);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_arg);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    uint32_t  data = 0;
    uint8_t  *p_tmp_buf = p_this->p_trans_buf->p_buf;

    while (p_this->data_ptr < p_this->p_trans_buf->buf_size) {

        /* read MRDR, check if FIFO is empty */
        data = __LPI2C_REG_READ(&(p_hw_lpi2c->mrdr));
        if (data & (1 << 14)) {
            return __RD_DATA_FIFO_EMPTY;
        }
        p_tmp_buf[p_this->data_ptr] = data & 0xff;        /* get data */
        p_this->data_ptr++;
    }

    /* manually send stop signal */
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), (0x02ul << 8));

    p_this->data_ptr = 0;
    p_this->p_trans_buf = NULL;

    return AW_OK;
}

/**
 * \brief   LPI2C interrupt
 */
aw_local void __lpi2c_isr (void *p_arg)
{
    __LPI2C_DEV_DECL(p_this, p_arg);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_arg);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    uint32_t intr_stat   = 0;
    uint32_t intr_enable = 0;


    intr_stat   = __LPI2C_REG_READ(&(p_hw_lpi2c->msr));
    intr_enable = __LPI2C_REG_READ(&(p_hw_lpi2c->mier));

    /* detect abnormality first */
    if ((intr_stat & LPI2C_MSR_ALF_MASK)  ||  \
        (intr_stat & LPI2C_MSR_PLTF_MASK) ||  \
        (intr_stat & LPI2C_MSR_FEF_MASK)  ||  \
        (intr_stat & LPI2C_MSR_NDF_MASK)) {
        aw_int_disable(p_this_dvif->inum);
        p_this->data_ptr = 0;
        p_this->cur_trans_index = 0;
        AW_SEMB_GIVE(p_this->i2c_sem);              /* give semaphore */
        return;
    }

    /* send data */
    if ((intr_stat   & LPI2C_MSR_TDF_MASK) &&
        (intr_enable & LPI2C_MSR_TDF_MASK)) {   /* send data */
        if (__send_data_isr(p_arg) != AW_OK) {
            return ;
        }
        __IMX10XX_LPI2C_DISABLE_IT(p_hw_lpi2c, LPI2C_MSR_TDF_MASK);
        AW_SEMB_GIVE(p_this->i2c_sem);              /* give semaphore */
        return;
    }

    /* receive data */
    if ((intr_stat   & LPI2C_MSR_RDF_MASK) &&
        (intr_enable & LPI2C_MSR_RDF_MASK)) {   /* receive data */
        if (__rcv_data_isr(p_arg) == __RD_DATA_FIFO_EMPTY) {
            return;
        }
        __IMX10XX_LPI2C_DISABLE_IT(p_hw_lpi2c, LPI2C_MSR_RDF_MASK);
        AW_SEMB_GIVE(p_this->i2c_sem);              /* give semaphore */;
        return;
    }

    /* go there, empty interrupt */
}

/**
 * \brief   reset lpi2c master, notice bus idle time, pin low time.
 */
aw_local void __lpi2c_master_reset(struct awbl_i2c_master *p_master)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);

    /* first reset lpi2c master */
    p_this->i2c_cfg_par.timeout = p_this_dvif->lpi2c_master_devinfo.timeout;
    p_this->i2c_cfg_par.bus_idle_time = p_this_dvif->bus_idle_timeout;
    __lpi2c_hard_init(p_this);

    /* then enable interrupt */
    aw_int_enable(p_this_dvif->inum);

    aw_task_delay(aw_ms_to_ticks(__I2C_MASTER_RESET_TIME));
}

/**
 * \brief   send start signal or restart signal
 */
aw_local aw_err_t __lpi2c_send_start(struct awbl_i2c_master *p_master,
                                     uint16_t                chip_addr,
                                     uint16_t                dr_flags,
                                     aw_bool_t               send_flg)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    /* get i2c slave device address */
    uint32_t   mtdr   = 0;
    uint8_t    l_addr = (chip_addr & 0x00ff);

    if (send_flg) {
        mtdr = (0x4 << 8) | (l_addr << 1);
    } else {
        mtdr = (0x4 << 8) | (l_addr << 1) | 0x01;
    }

    /* send start signal */
    if (dr_flags == AW_I2C_M_7BIT) {
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), mtdr);
    } else {
        return -AW_ENOTSUP;      /* not support others bits-device-address */
    }

    // TODO send 10-bit address

    return AW_OK;
}

/**
 * \brief   match exception error val
 */
aw_local aw_err_t __match_excep_state_err_val(uint32_t reg_val)
{
    if (reg_val & LPI2C_MSR_ALF_MASK) {
        return -AW_EIO;
    } else if (reg_val & LPI2C_MSR_PLTF_MASK) {
        return -AW_ETIMEDOUT;
    } else if (reg_val & LPI2C_MSR_FEF_MASK) {
        return -AW_EIO;
    } else if (reg_val & LPI2C_MSR_NDF_MASK) {
        return -AW_ENODEV;
    } else {
        return -AW_EIO;           /* not go there */
    }
}

/**
 * \brief   check if the i2c bus is busy
 */
aw_local aw_err_t __check_bus_busy(struct awbl_i2c_master *p_master)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    uint32_t        timeout_ms  = 0;
    aw_tick_t       start_ticks = 0;
    aw_tick_t       end_ticks   = 0;

    start_ticks = aw_sys_tick_get();

    while ((timeout_ms < __IMXI2C_WAIT_IDEL_TIMEOUT_MS) && \
           (__LPI2C_REG_READ(&(p_hw_lpi2c->msr)) & LPI2C_MSR_BBF_MASK)) {
        aw_task_delay(aw_ms_to_ticks(5));    /* give up CPU */
        end_ticks = aw_sys_tick_get();
        timeout_ms = aw_ticks_to_ms(end_ticks - start_ticks);
    }

    /* bus busy */
    if (timeout_ms >= __IMXI2C_WAIT_IDEL_TIMEOUT_MS) {
        __lpi2c_master_reset(p_master);
        return (-AW_EIO);
    }

    return AW_OK;
}

/**
 * \brief   check the initial state before send start signal
 */
aw_local aw_err_t __check_init_state(struct awbl_i2c_master *p_master)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    uint32_t  tmp_reg_val = 0x00;

    tmp_reg_val = __LPI2C_REG_READ(&p_hw_lpi2c->msr);

    /* check if the bus is normal */
    if ((tmp_reg_val & LPI2C_MSR_PLTF_MASK) ||
        (tmp_reg_val & LPI2C_MSR_BBF_MASK)) {

        aw_task_delay(aw_ms_to_ticks(10));
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->msr), LPI2C_MSR_PLTF_MASK);

        /* if true, the bus state is abnormal */
        if ((p_hw_lpi2c->msr & LPI2C_MSR_PLTF_MASK) ||
            (p_hw_lpi2c->msr & LPI2C_MSR_BBF_MASK)) {
            return -AW_EBUSY;
        }
    }

    /* check if lpi2c has detected other abnormal conditions */
    if ((tmp_reg_val & LPI2C_MSR_NDF_MASK)  ||
        (tmp_reg_val & LPI2C_MSR_ALF_MASK)) {
        return -AW_EBUSY;
    }

    /*
     * check if lpi2c detects FIFO err,
     * imxrt1050 lpi2c FIFO error detection isn't real time
     */
    if (tmp_reg_val & LPI2C_MSR_FEF_MASK) {
        __lpi2c_master_reset(p_master);
    }

    return AW_OK;
}

/**
 * \brief   reset the bus when a deadlock occurs
 */
aw_local void __lpi2c_bus_reset(struct awbl_i2c_master *p_master)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);

    if (p_this->err_count == 10 && p_this_dvif->pfunc_plfm_rst != NULL) {

        p_this_dvif->pfunc_plfm_rst();
        p_this->err_count = 0;
        return;
    }
    p_this->err_count += 1;
}

/*
 * \brief   write data to i2c slave device
 */
aw_local aw_err_t  __imx10xx_lpi2c_write(struct awbl_i2c_master  *p_master,
                                         uint16_t                 chip_addr,
                                         struct awbl_trans_buf   *p_trans_buf,
                                         uint32_t                 trans_buf_num,
                                         uint8_t                  stop_ctl,
                                         uint16_t                 dr_flag)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    int ret = AW_OK;

    uint32_t  tmp_reg_val = 0x00;


    p_this->data_ptr = 0;
    p_this->p_trans_buf   = p_trans_buf;
    p_this->stop_ctrl_flg = stop_ctl;
    p_this->trans_buf_num = trans_buf_num;
    p_this->cur_trans_index = 0;

    /* consuming the semaphore released by an abnormal situation */
    AW_SEMB_RESET(p_this->i2c_sem, 0);

    /* check if the initial state is normal */
    if ((ret = __check_init_state(p_master))) {
        __lpi2c_master_reset(p_master);
        __lpi2c_bus_reset(p_master);
        return ret;
    } else {
        p_this->err_count = 0;
    }

    /* set the interrupt value to open */
    tmp_reg_val = (LPI2C_MSR_TDF_MASK | LPI2C_MSR_PLTF_MASK |  \
                   LPI2C_MSR_ALF_MASK | LPI2C_MSR_FEF_MASK  |  \
                   LPI2C_MSR_NDF_MASK );

    /* send start */
    if ((ret = __lpi2c_send_start(p_master, chip_addr, \
                                  dr_flag,  AW_TRUE))) {
       return ret;
    }
    __IMX10XX_LPI2C_ENABLE_IT(p_hw_lpi2c, tmp_reg_val); /* enable interrupt */

    /* take semaphore, take semaphore timeout, not go to ISR */
    if ((ret = AW_SEMB_TAKE(p_this->i2c_sem, aw_ms_to_ticks(1000)))) {
        __lpi2c_master_reset(p_master);
        return ret;
    }

    /*
     * deal with abnormality
     * 1st: store abnormal bits
     * 2nd: reset lpi2c master, then enable interrupt
     * 3rd: detect specific error value and return error value
     */
    tmp_reg_val = __LPI2C_REG_READ(&p_hw_lpi2c->msr);
    if (tmp_reg_val & LPI2C_MSR_ABNM_MASK) {
        __lpi2c_master_reset(p_master);
        return __match_excep_state_err_val(tmp_reg_val);
    }

    if (stop_ctl) {
        if ((ret = __check_bus_busy(p_master))) {   /* check bus busy */
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief   read data from i2c slave device
 */
aw_local aw_err_t  __imx10xx_lpi2c_read(struct awbl_i2c_master *p_master,
                                        uint16_t                chip_addr,
                                        struct awbl_trans_buf  *p_trans_buf,
                                        uint8_t                 start_ctl,
                                        uint16_t                dr_flag)

{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    int     ret = AW_OK;

    uint32_t tmp_reg_val = 0;

    p_this->data_ptr = 0;
    p_this->cur_trans_index = 0;
    p_this->p_trans_buf = p_trans_buf;

    /*
     * if stop control flag is set, a start signal is sent,
     * otherwise a restart signal is sent
     */
    if (start_ctl == AWBL_I2C_READ_RESTART) {    /* send restart */

        /*
         * check lpi2c whether detect FIFO err,
         * imxrt1050 lpi2c FIFO error detection isn't real time
         */
        tmp_reg_val = __LPI2C_REG_READ(&p_hw_lpi2c->msr);
        if (tmp_reg_val & LPI2C_MSR_ABNM_MASK) {
            __lpi2c_master_reset(p_master);
            __lpi2c_bus_reset(p_master);
            return __match_excep_state_err_val(tmp_reg_val);
        } else {
            p_this->err_count = 0;
        }

        if ((ret = __lpi2c_send_start(p_master, chip_addr,  \
                                      dr_flag, AW_FALSE))) {
            return ret;
        }
    } else {

        /* consuming the semaphore released by an abnormal situation */
        AW_SEMB_RESET(p_this->i2c_sem, 0);

        /* check if the initial state is normal */
        if ((ret = __check_init_state(p_master))) {
            __lpi2c_master_reset(p_master);
            __lpi2c_bus_reset(p_master);
            return ret;
        } else {
            p_this->err_count = 0;
        }

        /* set the interrupt value to open */
        tmp_reg_val = (LPI2C_MSR_ALF_MASK  | LPI2C_MSR_FEF_MASK | \
                       LPI2C_MSR_PLTF_MASK | LPI2C_MSR_PLTF_MASK) ;

        /* send start */
        if ((ret = __lpi2c_send_start(p_master, chip_addr,  \
                                      dr_flag, AW_FALSE))) {
            return ret;
        }

        __IMX10XX_LPI2C_ENABLE_IT(p_hw_lpi2c, tmp_reg_val);
    }

    /*
     * too much data received,
     * lpi2c only supports receiving 256 bytes at a time
     */
    if (p_this->p_trans_buf->buf_size >= 256) {
        __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), (0x02ul << 8));
        return -AW_EMSGSIZE;
    }

    __LPI2C_REG_WRITE(&(p_hw_lpi2c->mtdr), \
                      ((0x1u) << 8) | (p_this->p_trans_buf->buf_size - 1));
    __IMX10XX_LPI2C_ENABLE_IT(p_hw_lpi2c, LPI2C_MSR_RDF_MASK);

    /* take semaphore, take semaphore timeout, not go to ISR */
    if ((ret = AW_SEMB_TAKE(p_this->i2c_sem, aw_ms_to_ticks(1000)))) {
        __lpi2c_master_reset(p_master);
        return ret;
    }

    /*
     * deal with abnormality
     * 1st: store abnormal bits
     * 2nd: reset lpi2c master, then enable interrupt, will not go ISR
     * 3rd: detect specific error value and return error value
     */
    tmp_reg_val = __LPI2C_REG_READ(&p_hw_lpi2c->msr);
    if (tmp_reg_val & LPI2C_MSR_ABNM_MASK) {
        __lpi2c_master_reset(p_master);
        return __match_excep_state_err_val(tmp_reg_val);
    }

    /* check bus busy */
    if ((ret = __check_bus_busy(p_master))) {
        return ret;
    }

    return AW_OK;
}

/**
 * \brief   fix the lpi2c speed user will configure
 */
aw_local aw_err_t __fix_usr_cfg_speed(uint32_t *p_speed)
{
    uint32_t tmp_speed  = *p_speed;
    uint32_t tmp_up_lim = 0;

    tmp_speed  = (tmp_speed / 100000) ;     /* fix: multiple of 100KHZ */
    tmp_up_lim = tmp_speed / 10;            /* upper limit speed factor */

    if (tmp_up_lim) {                       /* exceeding the 1,000,000 HZ*/
        *p_speed = 1000000;                 /* upper limit speed: 1,000,000HZ */
    } else {
        if (tmp_speed) {                    /* 100,000 ~ 900,000 */
            *p_speed = tmp_speed * 100000;
        } else {                            /* speed <= 100,000 */
            *p_speed = 100000;
        }
    }

    return AW_OK;
}

/**
 * \brief   check user configure i2c par
 */
aw_local aw_err_t __check_usr_cfg_par(struct aw_i2c_config *p_cfg)
{
    int ret = AW_OK;

    /* check user configure speed */
    if ((ret = __fix_usr_cfg_speed(&p_cfg->speed))) {
        return ret;
    }

    /*
     * ((1/speed) * 15)us ~ 16000us
     *
     * each bit is pulled down for (1/speed)us, considering that the sda_data
     * may be 0x00, 9 bits time pulled down should be configured.
     *
     * because there may be some errors(Îó²î) in the calculation process, it is
     * configured into 20(18 round up 20) bits.
     *
     * 100KHZ: 10us/bit     20bits: 200us       busCycle_ns: > 16ns
     * 200KHZ: 5us/bit      20bits: 100us       busCycle_ns: > 16ns
     * 300KHZ: 3.3us/bit    20bits: 66us        busCycle_ns: 16ns
     * 400KHZ: 2.5us/bit    20bits: 50us        busCycle_ns: 16ns
     * 500KHZ: 2us/bit      20bits: 40us        busCycle_ns: 16ns
     * ......
     * 1MHZ:   1us/bit      20bits: 20us        busCycle_ns: 16ns
     *
     * (2^12 * 256 * 16)ns -> 16ms ->16000us
     */
    if (p_cfg->timeout > 16000 || p_cfg->timeout < 200) {
        return -AW_ENOTSUP;
    }

    /* 2^12 * 16ns -> 60000ns -> 60us */
    if (p_cfg->bus_idle_time > 60 || p_cfg->bus_idle_time < 0) {
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief   configure lpi2c master par
 */
aw_local aw_err_t  __imx10xx_lpi2c_set_cfg(struct awbl_i2c_master *p_master,
                                           struct aw_i2c_config   *p_cfg)
{
    __LPI2C_DEV_DECL(p_this, p_master);
    __LPI2C_DEVINFO_DECL(p_this_dvif, p_this);
    __LPI2C_HW_DECL(p_hw_lpi2c, p_this_dvif);

    int ret = AW_OK;

    if((ret = __check_usr_cfg_par(p_cfg))) {
        return ret;
    }

    /* configure i2c master speed */
    __lpi2c_master_set_baudrate(p_hw_lpi2c,
                                aw_clk_rate_get(p_this_dvif->ref_clk_id),
                                p_cfg->speed);

    /* store speed that user set */
    p_this->i2c_cfg_par.speed         = p_cfg->speed;
    p_this->i2c_cfg_par.timeout       = p_cfg->timeout * 1000; /* us -> ns */
    p_this->i2c_cfg_par.bus_idle_time = p_cfg->bus_idle_time * 1000;

    /* configure i2c bus idle time */
    __lpi2c_cfg_features(p_this);

    aw_udelay(50);                   /* wait clock stable */

    return AW_OK;
}

/**
 * \brief   get lpi2c master par
 */
aw_local aw_err_t __imx10xx_lpi2c_get_cfg(struct awbl_i2c_master *p_master,
                                          struct aw_i2c_config   *p_cfg)
{
    __LPI2C_DEV_DECL(p_this, p_master);

    p_cfg->bus_idle_time = p_this->i2c_cfg_par.bus_idle_time / 1000;
    p_cfg->timeout       = p_this->i2c_cfg_par.timeout / 1000;  /* ns -> us */
    p_cfg->speed         = p_this->i2c_cfg_par.speed;

    return AW_OK;
}

/**
 * \brief register imx10xx lpi2c driver
 *
 * This routine registers the imx10xx lpi2c driver and device recognition
 * data with the AWBus subsystem.
 *
 * NOTE: This routine is called early during system initialization, and
 * *MUST NOT* make calls to OS facilities such as memory allocation
 * and I/O.
 */
void awbl_imx10xx_lpi2c_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_lpi2c_drv_registration);
}
