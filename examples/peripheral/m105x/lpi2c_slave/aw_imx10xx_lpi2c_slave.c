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
 * \brief imx10xx lpi2c slave driver
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-14  zq, first implemetation
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


#include "aw_imx10xx_lpi2c_slave.h"

/*******************************************************************************
  macro operate
*******************************************************************************/
/* write register */
#define __LPI2C_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __LPI2C_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)


/* get i2c instance */
__imx10xx_lpi2c_reg_t *p_hw_lpi2c = (__imx10xx_lpi2c_reg_t*) I2C_BASE;


typedef int32_t status_t;

typedef enum _lpi2c_slave_address_match {

    /*!< Match only address 0. */
    kLPI2C_MatchAddress0 = 0U,

    /*!< Match either address 0 or address 1. */
    kLPI2C_MatchAddress0OrAddress1 = 2U,

    /*!< Match a range of slave addresses from address 0 through address 1.*/
    kLPI2C_MatchAddress0ThroughAddress1 = 6U,

} lpi2c_slave_address_match_t;


typedef struct _lpi2c_slave_config {

    aw_bool_t                   enable_slave;                   /*!< Enable slave mode. */
    uint8_t                     address0;                       /*!< Slave's 7-bit address. */
    uint8_t                     address1;                       /*!< Alternate slave 7-bit address. */
    lpi2c_slave_address_match_t address_match_mode;
    aw_bool_t                   filter_doze_enable;
    aw_bool_t                   filter_enable;
    aw_bool_t                   enable_general_call;

    struct
    {
        aw_bool_t enable_ack;
        aw_bool_t enable_tx;
        aw_bool_t enable_rx;
        aw_bool_t enable_address;
    } scl_stall;

    aw_bool_t                   ignore_ack;

    aw_bool_t                   enable_received_address_read;   /*Enable reading the address received address as the first byte of data.*/
    uint32_t                    sda_glitch_filterwidth_ns;
    uint32_t                    scl_glitch_filterwidth_ns;
    uint32_t                    data_validdelay_ns;
    uint32_t                    clkholdtime_ns;

} lpi2c_slave_config_t;



#define LPI2C_SAMR_ADDR0_MASK            (0x7FEU)
#define LPI2C_SAMR_ADDR0_SHIFT           (1U)
#define LPI2C_SAMR_ADDR0(x)              (((uint32_t)(((uint32_t)(x)) << \
                LPI2C_SAMR_ADDR0_SHIFT)) & LPI2C_SAMR_ADDR0_MASK)
#define LPI2C_SAMR_ADDR1_MASK            (0x7FE0000U)
#define LPI2C_SAMR_ADDR1_SHIFT           (17U)
#define LPI2C_SAMR_ADDR1(x)              (((uint32_t)(((uint32_t)(x)) << \
                LPI2C_SAMR_ADDR1_SHIFT)) & LPI2C_SAMR_ADDR1_MASK)

/*! @name SCFGR1 - Slave Configuration Register 1 */
/*! @{ */
#define LPI2C_SCFGR1_ADRSTALL_MASK       (0x1U)
#define LPI2C_SCFGR1_ADRSTALL_SHIFT      (0U)
/*! ADRSTALL - Address SCL Stall
 *  0b0..Clock stretching is disabled
 *  0b1..Clock stretching is enabled
 */
#define LPI2C_SCFGR1_ADRSTALL(x)         (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_ADRSTALL_SHIFT)) & LPI2C_SCFGR1_ADRSTALL_MASK)


#define LPI2C_SCFGR1_RXSTALL_MASK        (0x2U)
#define LPI2C_SCFGR1_RXSTALL_SHIFT       (1U)
/*! RXSTALL - RX SCL Stall
 *  0b0..Clock stretching is disabled
 *  0b1..Clock stretching is enabled
 */
#define LPI2C_SCFGR1_RXSTALL(x)          (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_RXSTALL_SHIFT)) & LPI2C_SCFGR1_RXSTALL_MASK)


#define LPI2C_SCFGR1_TXDSTALL_MASK       (0x4U)
#define LPI2C_SCFGR1_TXDSTALL_SHIFT      (2U)
/*! TXDSTALL - TX Data SCL Stall
 *  0b0..Clock stretching is disabled
 *  0b1..Clock stretching is enabled
 */
#define LPI2C_SCFGR1_TXDSTALL(x)         (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_TXDSTALL_SHIFT)) & LPI2C_SCFGR1_TXDSTALL_MASK)


#define LPI2C_SCFGR1_ACKSTALL_MASK       (0x8U)
#define LPI2C_SCFGR1_ACKSTALL_SHIFT      (3U)
/*! ACKSTALL - ACK SCL Stall
 *  0b0..Clock stretching is disabled
 *  0b1..Clock stretching is enabled
 */
#define LPI2C_SCFGR1_ACKSTALL(x)         (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_ACKSTALL_SHIFT)) & LPI2C_SCFGR1_ACKSTALL_MASK)


#define LPI2C_SCFGR1_IGNACK_MASK         (0x1000U)
#define LPI2C_SCFGR1_IGNACK_SHIFT        (12U)
/*! IGNACK - Ignore NACK
 *  0b0..Slave will end transfer when NACK is detected
 *  0b1..Slave will not end transfer when NACK detected
 */
#define LPI2C_SCFGR1_IGNACK(x)           (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_IGNACK_SHIFT)) & LPI2C_SCFGR1_IGNACK_MASK)


#define LPI2C_SCFGR1_RXCFG_MASK          (0x800U)
#define LPI2C_SCFGR1_RXCFG_SHIFT         (11U)
/*! RXCFG - Receive Data Configuration
 *  0b0..Reading the Receive Data register will return received data and clear
 *       the Receive Data flag (MSR[RDF]).
 *  0b1..Reading the Receive Data register when the Address Valid flag
 *       (SSR[AVF])is set, will return the Address
 *       Status register and clear the Address Valid flag. Reading the Receive
 *       Data register when the Address Valid flag is clear,
 *       will return received data and clear the Receive Data flag (MSR[RDF]).
 */
#define LPI2C_SCFGR1_RXCFG(x)            (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_RXCFG_SHIFT)) & LPI2C_SCFGR1_RXCFG_MASK)


#define LPI2C_SCFGR1_GCEN_MASK           (0x100U)
#define LPI2C_SCFGR1_GCEN_SHIFT          (8U)
/*! GCEN - General Call Enable
 *  0b0..General Call address is disabled
 *  0b1..General Call address is enabled
 */
#define LPI2C_SCFGR1_GCEN(x)             (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_GCEN_SHIFT)) & LPI2C_SCFGR1_GCEN_MASK)


#define LPI2C_SCFGR1_ADDRCFG_MASK        (0x70000U)
#define LPI2C_SCFGR1_ADDRCFG_SHIFT       (16U)
/*! ADDRCFG - Address Configuration
 *  0b000..Address match 0 (7-bit)
 *  0b001..Address match 0 (10-bit)
 *  0b010..Address match 0 (7-bit) or Address match 1 (7-bit)
 *  0b011..Address match 0 (10-bit) or Address match 1 (10-bit)
 *  0b100..Address match 0 (7-bit) or Address match 1 (10-bit)
 *  0b101..Address match 0 (10-bit) or Address match 1 (7-bit)
 *  0b110..From Address match 0 (7-bit) to Address match 1 (7-bit)
 *  0b111..From Address match 0 (10-bit) to Address match 1 (10-bit)
 */
#define LPI2C_SCFGR1_ADDRCFG(x)          (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_ADDRCFG_SHIFT)) & LPI2C_SCFGR1_ADDRCFG_MASK)


/*! TXCFG - Transmit Flag Configuration
 *  0b0..Transmit Data Flag will only assert during a slave-transmit transfer
 *       when the Transmit Data register is empty
 *  0b1..Transmit Data Flag will assert whenever the Transmit Data register
 *       is empty
 */
#define LPI2C_SCFGR1_TXCFG(x)            (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_TXCFG_SHIFT)) & LPI2C_SCFGR1_TXCFG_MASK)

#define LPI2C_SCFGR1_HSMEN(x)            (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR1_HSMEN_SHIFT)) & LPI2C_SCFGR1_HSMEN_MASK)


/*! @name SCFGR2 - Slave Configuration Register 2 */
/*! @{ */
#define LPI2C_SCFGR2_CLKHOLD_MASK        (0xFU)
#define LPI2C_SCFGR2_CLKHOLD_SHIFT       (0U)
#define LPI2C_SCFGR2_CLKHOLD(x)          (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR2_CLKHOLD_SHIFT)) & LPI2C_SCFGR2_CLKHOLD_MASK)
#define LPI2C_SCFGR2_DATAVD_MASK         (0x3F00U)
#define LPI2C_SCFGR2_DATAVD_SHIFT        (8U)
#define LPI2C_SCFGR2_DATAVD(x)           (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR2_DATAVD_SHIFT)) & LPI2C_SCFGR2_DATAVD_MASK)
#define LPI2C_SCFGR2_FILTSCL_MASK        (0xF0000U)
#define LPI2C_SCFGR2_FILTSCL_SHIFT       (16U)
#define LPI2C_SCFGR2_FILTSCL(x)          (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR2_FILTSCL_SHIFT)) & LPI2C_SCFGR2_FILTSCL_MASK)
#define LPI2C_SCFGR2_FILTSDA_MASK        (0xF000000U)
#define LPI2C_SCFGR2_FILTSDA_SHIFT       (24U)
#define LPI2C_SCFGR2_FILTSDA(x)          (((uint32_t)(((uint32_t)(x)) << \
        LPI2C_SCFGR2_FILTSDA_SHIFT)) & LPI2C_SCFGR2_FILTSDA_MASK)



/*! @name SCR - Slave Control Register */
/*! @{ */
#define LPI2C_SCR_SEN_MASK               (0x1U)
#define LPI2C_SCR_SEN_SHIFT              (0U)
/*! SEN - Slave Enable
 *  0b0..I2C Slave mode is disabled
 *  0b1..I2C Slave mode is enabled
 */
#define LPI2C_SCR_SEN(x)                 (((uint32_t)(((uint32_t)(x)) << \
                              LPI2C_SCR_SEN_SHIFT)) & LPI2C_SCR_SEN_MASK)


#define LPI2C_SCR_RST_MASK               (0x2U)
#define LPI2C_SCR_RST_SHIFT              (1U)
/*! RST - Software Reset
 *  0b0..Slave mode logic is not reset
 *  0b1..Slave mode logic is reset
 */
#define LPI2C_SCR_RST(x)                 (((uint32_t)(((uint32_t)(x)) << \
                              LPI2C_SCR_RST_SHIFT)) & LPI2C_SCR_RST_MASK)


#define LPI2C_SCR_FILTEN_MASK            (0x10U)
#define LPI2C_SCR_FILTEN_SHIFT           (4U)
/*! FILTEN - Filter Enable
 *  0b0..Disable digital filter and output delay counter for slave mode
 *  0b1..Enable digital filter and output delay counter for slave mode
 */
#define LPI2C_SCR_FILTEN(x)              (((uint32_t)(((uint32_t)(x)) << \
                              LPI2C_SCR_FILTEN_SHIFT)) & LPI2C_SCR_FILTEN_MASK)


#define LPI2C_SCR_FILTDZ_MASK            (0x20U)
#define LPI2C_SCR_FILTDZ_SHIFT           (5U)
#define LPI2C_SCR_FILTDZ(x)              (((uint32_t)(((uint32_t)(x)) << \
                              LPI2C_SCR_FILTDZ_SHIFT)) & LPI2C_SCR_FILTDZ_MASK)


#define LPI2C_SCR_RTF_MASK               (0x100U)
#define LPI2C_SCR_RTF_SHIFT              (8U)
/*! RTF - Reset Transmit FIFO
 *  0b0..No effect
 *  0b1..Transmit Data Register is now empty
 */
#define LPI2C_SCR_RTF(x)                 (((uint32_t)(((uint32_t)(x)) << \
                              LPI2C_SCR_RTF_SHIFT)) & LPI2C_SCR_RTF_MASK)


#define LPI2C_SCR_RRF_MASK               (0x200U)
#define LPI2C_SCR_RRF_SHIFT              (9U)

/*! @name STAR - Slave Transmit ACK Register */
/*! @{ */
#define LPI2C_STAR_TXNACK_MASK           (0x1U)
#define LPI2C_STAR_TXNACK_SHIFT          (0U)






aw_local void __imx1050_lpi2c1_pinmux_init (void) {

    int lpi2c1_gpios[] = {GPIO1_17, GPIO1_16};

    if (aw_gpio_pin_request("lpi2c1_gpios",
                            lpi2c1_gpios,
                            AW_NELEMENTS(lpi2c1_gpios)) == AW_OK) {
        aw_gpio_pin_cfg(GPIO1_17, GPIO1_17_LPI2C1_SDA | AW_GPIO_OPEN_DRAIN);
        aw_gpio_pin_cfg(GPIO1_16, GPIO1_16_LPI2C1_SCL | AW_GPIO_OPEN_DRAIN);
    }
}

void i2c_slaver_reset (void)
{
    p_hw_lpi2c->scr = 0x2U;
    p_hw_lpi2c->scr = 0;
}


void __i2c_slaver_interrupt_clr (__imx10xx_lpi2c_reg_t *p_hw_lpi2c,
                                 uint32_t               flags)
{
    uint32_t value;

    value   = __LPI2C_REG_READ(&(p_hw_lpi2c->ssr));
    value  &= ~flags;
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->ssr), value);
}

void __i2c_slaver_interrupt_enable (__imx10xx_lpi2c_reg_t      *p_hw_lpi2c,
                                    uint32_t                    mask)
{
    uint32_t value;

    value = __LPI2C_REG_READ(&(p_hw_lpi2c->sier));
    value |= mask;
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->sier), value);
}

void __i2c_slaver_interrupt_disable (__imx10xx_lpi2c_reg_t     *p_hw_lpi2c,
                                     uint32_t                   mask)
{
    uint32_t value;

    value = __LPI2C_REG_READ(&(p_hw_lpi2c->sier));
    value &= ~mask;
    __LPI2C_REG_WRITE(&(p_hw_lpi2c->sier), value);
}

i2c_slave_handle_t __g_handle;

void __i2c_isr (void *p_arg)
{
    __imx10xx_lpi2c_reg_t *p_hw = (__imx10xx_lpi2c_reg_t*)p_arg;
    uint8_t res;
    /* 接收中断 */
    if (p_hw->ssr & 0x02) {
        res = p_hw->srdr & 0xff;
        __g_handle.callback(SlaveReceiveEvent, &res);

        __i2c_slaver_interrupt_clr(p_hw ,0x02);
    }

    /* 发送中断 */
    if (p_hw->ssr & 0x01) {
        __g_handle.callback(SlaveTransmitEvent, &res);
        p_hw->stdr = res;
        __i2c_slaver_interrupt_clr(p_hw ,0x01);
    }

    /* 停止中断 */
    if (p_hw->ssr & 0x200) {
        __g_handle.callback(SlaveCompletionEvent, &res);
        __i2c_slaver_interrupt_clr(p_hw ,0x200);
    }

}


static uint32_t __i2c_getcycles_forwidth (uint32_t src_clk_hz,
                                          uint32_t width_ns,
                                          uint32_t max_cycles,
                                          uint32_t prescaler)
{

    uint32_t buscycle_ns = 1000000 / (src_clk_hz / prescaler / 1000);
    uint32_t cycles = 0;

    /* Search for the cycle count just below the desired glitch width. */
    while ((((cycles + 1) * buscycle_ns) < width_ns) &&
           (cycles + 1 < max_cycles)) {
        ++cycles;
    }

    /* If we end up with zero cycles,
     * then set the filter to a single cycle unless the */
    /* bus clock is greater than 10x the desired glitch width. */
    if ((cycles == 0) && (buscycle_ns <= (width_ns * 10))) {
        cycles = 1;
    }

    return cycles;
}


aw_local void __i2c_slave_init (const lpi2c_slave_config_t     *cfg,
                                uint32_t                        src_clk_hz)
{
    /* enable i2c clk */
    aw_clk_enable(I2C_CLK_ID);

    /* config gpio */
    __imx1050_lpi2c1_pinmux_init();

    /* reset slaver */
    i2c_slaver_reset();

    p_hw_lpi2c->samr = LPI2C_SAMR_ADDR0(cfg->address0) |
                       LPI2C_SAMR_ADDR1(cfg->address1);

    p_hw_lpi2c->scfgr1 =
            LPI2C_SCFGR1_ADDRCFG(
                    cfg->address_match_mode) |
                    LPI2C_SCFGR1_IGNACK(cfg->ignore_ack) |
            LPI2C_SCFGR1_RXCFG(
                    cfg->enable_received_address_read) |
                    LPI2C_SCFGR1_GCEN(cfg->enable_general_call) |
            LPI2C_SCFGR1_ACKSTALL(
                    cfg->scl_stall.enable_ack) |
                    LPI2C_SCFGR1_TXDSTALL(cfg->scl_stall.enable_tx) |
            LPI2C_SCFGR1_RXSTALL(cfg->scl_stall.enable_rx) |
            LPI2C_SCFGR1_ADRSTALL(cfg->scl_stall.enable_address);


    p_hw_lpi2c->scfgr2 =
            LPI2C_SCFGR2_FILTSDA(
                    __i2c_getcycles_forwidth(
                            src_clk_hz,
                            cfg->sda_glitch_filterwidth_ns,
                            (LPI2C_SCFGR2_FILTSDA_MASK >>
                            LPI2C_SCFGR2_FILTSDA_SHIFT),
                            1)) |
            LPI2C_SCFGR2_FILTSCL(
                    __i2c_getcycles_forwidth(
                            src_clk_hz,
                            cfg->scl_glitch_filterwidth_ns,
                            (LPI2C_SCFGR2_FILTSCL_MASK >>
                            LPI2C_SCFGR2_FILTSCL_SHIFT),
                            1)) |
            LPI2C_SCFGR2_DATAVD(
                    __i2c_getcycles_forwidth(
                            src_clk_hz,
                            cfg->data_validdelay_ns,
                            (LPI2C_SCFGR2_DATAVD_MASK >>
                            LPI2C_SCFGR2_DATAVD_SHIFT),
                            1)) |
            LPI2C_SCFGR2_CLKHOLD(
                    __i2c_getcycles_forwidth(
                            src_clk_hz,
                            cfg->clkholdtime_ns,
                            (LPI2C_SCFGR2_CLKHOLD_MASK >>
                            LPI2C_SCFGR2_CLKHOLD_SHIFT),
                            1));

    /* Save SCR to last so we don't enable slave until it is configured */
    p_hw_lpi2c->scr = LPI2C_SCR_FILTDZ(cfg->filter_doze_enable) |
                      LPI2C_SCR_FILTEN(cfg->filter_enable) |
                      LPI2C_SCR_SEN(cfg->enable_slave);


    __i2c_slaver_interrupt_disable(p_hw_lpi2c, 0xff);

    /* connect and enable ISR */
    aw_int_connect(I2C_INT_NUM,
                  (aw_pfuncvoid_t)__i2c_isr,
                  (void *)p_hw_lpi2c);
    aw_int_enable(I2C_INT_NUM);


    __i2c_slaver_interrupt_enable(p_hw_lpi2c, 0x02 | 0x01 | 0x200);

}




void __i2c_slave_get_cfg_def (lpi2c_slave_config_t *p_cfg)
{
    /* Initializes the configure structure to zero. */
    memset(p_cfg, 0, sizeof(*p_cfg));

    p_cfg->enable_slave = AW_TRUE;
    p_cfg->address0 = 0U;
    p_cfg->address1 = 0U;
    p_cfg->address_match_mode = kLPI2C_MatchAddress0;
    p_cfg->filter_doze_enable = AW_TRUE;
    p_cfg->filter_enable = AW_TRUE;
    p_cfg->enable_general_call = AW_FALSE;
    p_cfg->scl_stall.enable_ack = AW_FALSE;
    p_cfg->scl_stall.enable_tx = AW_TRUE;
    p_cfg->scl_stall.enable_rx = AW_TRUE;
    p_cfg->scl_stall.enable_address = AW_FALSE;
    p_cfg->ignore_ack = AW_FALSE;
    p_cfg->enable_received_address_read = AW_FALSE;
    p_cfg->sda_glitch_filterwidth_ns = 0;
    p_cfg->scl_glitch_filterwidth_ns = 0;
    p_cfg->data_validdelay_ns = 0;
    p_cfg->clkholdtime_ns = 0;
}

typedef enum i2c_slave_flag {

    SlaveTxReadyFlag = 0x01U,
    SlaveRepeatedStartDetectFlag = 0x100U,
    SlaveStopDetectFlag = 0x200,

} i2c_slave_flag_t;


void aw_i2c_slave_register_callback (i2c_slave_callback_t callback)
{
    __g_handle.callback = callback;
}



void aw_i2c_slave_init (uint16_t addr, i2c_slave_callback_t cb)
{
    aw_clk_rate_t clk;
    lpi2c_slave_config_t slaveconfig;

    /* get i2c clk */
    clk = aw_clk_rate_get( I2C_CLK_ID);

    /* get i2c cfg default */
    __i2c_slave_get_cfg_def(&slaveconfig);

    slaveconfig.address0 = (uint8_t)addr;

    aw_i2c_slave_register_callback(cb);

    __i2c_slave_init(&slaveconfig, clk);
}



