/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2013 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT1050 gpio driver
 *
 * this driver is the implementation of aw_gpio.h for iMX RT1050
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-12, mex, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "string.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_gpio.h"
#include "aw_gpio_util.h"
#include "aw_cpu_clk.h"
#include "aw_clk.h"
#include "aw_gpio.h"
#include "imx1050_pin.h"
#include "imx1050_iomuxc_regs.h"
#include "imx1050_reg_base.h"
#include "imx1050_inum.h"
#include "aw_errno.h"
#include "driver/gpio/awbl_imx1050_gpio.h"


/* write register */
#define __GPIO_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, data)

/* read register */
#define __GPIO_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* get bit */
#define __GPIO_REG_BIT_GET(reg_addr, bit) \
        AW_REG_BIT_GET32(reg_addr, bit)

/* set bit */
#define __GPIO_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __GPIO_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __GPIO_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* set register bits */
#define __GPIO_REG_BITS_SET(reg_addr, start, len, value) \
        AW_REG_BITS_SET32(reg_addr, start, len, value)

/* declare sim device instance */
#define __GPIO_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1050_gpio_dev *p_this = \
        (struct awbl_imx1050_gpio_dev *)(p_dev)

/* declare sim device infomation */
#define __GPIO_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1050_gpio_devinfo *p_devinfo = \
        (struct awbl_imx1050_gpio_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* devlock lock */
#define __GPIO_DEV_LOCK(p_dev) \
    aw_spinlock_isr_take(&p_dev->dev_lock)

/* devlock unlock */
#define __GPIO_DEV_UNLOCK(p_dev) \
    aw_spinlock_isr_give(&p_dev->dev_lock)

/* devlock initialize */
#define __GPIO_DEV_LOCK_INIT(p_dev) \
    aw_spinlock_isr_init(&p_dev->dev_lock, 0)

/* is pin valid */
#define __GPIO_PIN_IS_VALID(pin) \
    (((pin) >= 0) && ((pin) < (IMX1050_GPIO_PINS)))


/* 获取端口的寄存器基地址*/
#define IMX1050_GET_PORT_REG_BASE(p_port, port, p_devinfo)    \
    imx1050_gpio_port_regs_t  *p_port =                       \
                     (imx1050_gpio_port_regs_t  *)p_devinfo->gpio_regs_base[port];


/**
 * \brief imx1050 GPIO每端口寄存器
 */
typedef struct __tag_imx1050_gpio_port_regs
{
    volatile uint32_t data;         /**< \brief GPIO data register */
    volatile uint32_t dir;          /**< \brief GPIO direction register */
    volatile uint32_t psr;          /**< \brief GPIO pad status register */
    volatile uint32_t icr1;         /**< \brief GPIO interrupt configuration register1 */
    volatile uint32_t icr2;         /**< \brief GPIO interrupt configuration register2 */
    volatile uint32_t imr;          /**< \brief GPIO interrupt mask register */
    volatile uint32_t isr;          /**< \brief GPIO interrupt status register */
    volatile uint32_t edge_sel;     /**< \brief GPIO edge select register */
             uint32_t reserved[25];
    volatile uint32_t dr_set;       /**< \brief The SET register of GPIO_DR. */
    volatile uint32_t dr_clear;     /**< \brief The CLEAR register of GPIO_DR. */
    volatile uint32_t dr_toggle;    /**< \brief The TOGGLE register of GPIO_DR */
} imx1050_gpio_port_regs_t;


/**
 * \brief imx1050 GPIO引脚配置映射表基础结构
 */
typedef struct __tag_imx1050_gpio_pin_cfg_map {

    uint16_t mux_ctl_offset;
    uint16_t pad_ctl_offset;
    uint16_t select_input_offset[8];
    uint16_t select_input_val[8];
} imx1050_pin_cfg_map_t;


aw_local const imx1050_pin_cfg_map_t g_pin_cfg_map[IMX1050_GPIO_PINS] = {

    /* GPIO1_0 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_00),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[0] = 0x02,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN14_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG2_ID_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SCK_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO1_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_01),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[0] = 0x02,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN15_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG1_ID_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SDO_SELECT_INPUT),
    .select_input_val[7] = 0x00,
    },

    /* GPIO1_2 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN16_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SDI_SELECT_INPUT),
    .select_input_val[7] = 0x00,
    },

    /* GPIO1_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_03),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG1_OC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_PCS0_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO1_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_5 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA08_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[6] = 0x02,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_6 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA07_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN18_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_7 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA06_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN19_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA05_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN20_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x03,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA04_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN21_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_10 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_MCLK2_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA03_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN22_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_11 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[1] = 0x03,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA02_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN23_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, ENET0_TIMER_SELECT_INPUT),
    .select_input_val[7] = 0x01,
    },

    /* GPIO1_12 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_12),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SCL_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, NMI_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO1_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_13),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SDA_SELECT_INPU),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_14 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_14),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG2_OC_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN24_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_VSYNC_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_15 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_15),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN25_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET0_TIMER_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_HSYNC_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[6] = 0x02,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_00),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG2_ID_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SCL_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[6] = 0x02,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_17 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SDA_SELECT_INPU),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_18 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_02),
    .select_input_offset[0] =  AW_OFFSET(hw_iomuxc_regs_t, USB_OTG1_ID_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_19 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_03),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG1_OC_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SPDIF_IN_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CD_B_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_04),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA3_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_CTS_B_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_PIXCLK_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA0_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_21 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_05),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA2_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA1_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_06),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA1_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SDA_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_VSYNC_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA2_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_07),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA0_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SCL_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_HSYNC_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA3_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[3] = 0x03,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA09_SELECT_INPUT),
    .select_input_val[4] = 0x00,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CMD_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_25 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_09),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DQS_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK2_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA08_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CLK_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_10),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA3_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA07_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_WP_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_27 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_11),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA2_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA06_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_28 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_12),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA1_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_PCS0_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA05_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA4_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_29 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_13),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA0_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SDI_SELECT_INPUT),
    .select_input_val[2] = 1,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA04_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA5_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_30 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_14),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_SCK_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SDO_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA03_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA6_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_31 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_15),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI3_SCK_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, CSI_DATA02_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA7_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /******************************************************************************/

    /* GPIO2_0 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_PCS0_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDI_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_2 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDO_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[2] = 0x03,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SCK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SCL_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_5 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SDA_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_6 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER0_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA0_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_7 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER1_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB0_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER2_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA1_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_TX_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB1_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_RX_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_10 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA2_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA1_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_11 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB2_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA2_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_12 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_12),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA3_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK2_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_14 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_14),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_15 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B0_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B0_15),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN14_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[6] = 0x04,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_17 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN15_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[6] = 0x04,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_18 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN16_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[6] = 0x04,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_19 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[1] = 0x03,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[6] = 0x03,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_PCS0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET0_RXDATA_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_21 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDI_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET1_RXDATA_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDO_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RXEN_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SCK_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_25 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[6] = 0x03,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER3_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_TXCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, ENET_IPG_CLK_RMII_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_27 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RXERR_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_28 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_12),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_TX_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, CSI_PIXCLK_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET0_TIMER_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[6] = 0x02,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_29 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_RX_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, CSI_VSYNC_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[6] = 0x03,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_30 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_14),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, CSI_HSYNC_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN02_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_31 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_B1_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_B1_15),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[0] = 0x02,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA3_SELECT_INPU),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN03_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /******************************************************************************/

    /* GPIO3_0 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_00),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA3_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA3_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA1_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_01),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA2_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA2_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_2 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_02),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA1_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA3_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_03),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA0_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIB_DATA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK2_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_04),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CLK_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SCL_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_5 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_05),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CMD_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DQS_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SDA_SELECT_INPU),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_6 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_7 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_SCK_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SCK_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_08),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA4_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDO_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_09),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA5_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDI_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_10 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_10),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA6_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SDA_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_11 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_11),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_DATA7_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPIA_DATA3_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SCL_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_12 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SCL_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN04_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SCK_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SDA_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN05_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_14 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN06_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDO_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_15 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN07_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDI_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN08_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_17 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN09_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_18 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_32),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_32),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[3] = 0x04,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_19 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_33),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_33),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_34),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_34),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_21 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_35),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_35),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN18_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_36),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_36),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN22_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_37),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_37),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN23_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_WP_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_38),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_38),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_TX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_25 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_39),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_39),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_RX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CD_B_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_40),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_40),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG2_OC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_27 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_41),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_41),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_28 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_29 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_30 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_31 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_0 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SCK_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN02_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_1 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_PCS0_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN03_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_2 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDO_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN04_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_3 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDI_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN05_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_4 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN06_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_5 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_SYNC_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN07_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_6 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_BCLK_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN08_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_7 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_MCLK2_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN09_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_8 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_DATA0_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_9 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_SYNC_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_10 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_BCLK_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_11 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SDA_SELECT_INPU),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_12 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_12),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN24_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SCL_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_13 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN25_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_14 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_14),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN19_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_15 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_15),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN20_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_CTS_B_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_16 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_16),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_16),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN21_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SPDIF_IN_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER1_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_17 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_17),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_17),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM4_PWMA3_SELECT_INPU),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER2_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_18 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_18),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_18),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER3_TIMER3_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_19 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_19),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_19),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET1_RXDATA_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_20 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_20),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_20),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET0_RXDATA_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER1_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_21 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_21),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_21),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SDA_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER2_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_22 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_22),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_22),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SCL_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER3_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_23 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_23),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_23),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RXEN_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_24 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_24),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_24),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_25 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_25),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_25),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_TXCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, ENET_IPG_CLK_RMII_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_26 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_26),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_26),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RXERR_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_27 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_27),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_27),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SCK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_28 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_28),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_28),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDO_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_29 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_29),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_29),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDI_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_30 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_30),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_30),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_PCS0_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO4_31 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_31),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_31),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

/******************************************************************************/

    /* GPIO5_0 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_MUX_CTL_PAD_WAKEUP),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_PAD_CTL_PAD_WAKEUP),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO5_1 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_MUX_CTL_PAD_PMIC_ON_REQ),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_PAD_CTL_PAD_PMIC_ON_REQ),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO5_2 */
    {
    .mux_ctl_offset  = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_MUX_CTL_PAD_PMIC_STBY_REQ),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_snvs_regs_t, SNVS_SW_PAD_CTL_PAD_PMIC_STBY_REQ),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

};


/**
 * \brief GPIO引脚配置
 *
 * aw_gpio_pin_cfg()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         要配值得引脚编号
 * \param   flags       要配置的特性标志，参考aw gpio模块来获取有效的参数值
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_pin_cfg (void *p_cookie, int pin, uint32_t flags)
{
    //todo:
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  func    = AW_GPIO_FUNCBITS_GET(flags);
    uint32_t  mode    = AW_GPIO_MODEBITS_GET(flags);

    uint32_t  mux     = IMX1050_PIN_MUX_GET(flags);
    uint32_t  pad_ctl = IMX1050_PAD_CTL_GET(flags);

    uint32_t  port    = pin / IMX1050_PORT_PINS;
    uint32_t  index   = pin % IMX1050_PORT_PINS;
    aw_err_t  err     = AW_OK;
    uint32_t  *p_iomuxc_regs = NULL;
    uint32_t  iomuxc_base = 0;

    IMX1050_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }
    __GPIO_DEV_LOCK(p_gpio_dev);

    if (p_devinfo->p_pin_cfg != NULL) {

        /* 配置引脚前需要请求引脚 */
        if (!(p_devinfo->p_pin_cfg[pin / 8] & AW_BIT(pin % 8))) {
            __GPIO_DEV_UNLOCK(p_gpio_dev);
            return -AW_EPERM;
        }
    }

    if (4 == port) {
        iomuxc_base = p_devinfo->iomuxc_snvs_base;
    } else {
        iomuxc_base = p_devinfo->iomuxc_base;
    }

    /* 首先设置引脚复用功能 */
    if (IS_PIN_MUX_SET_ENABLE(flags)) {

        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].mux_ctl_offset + iomuxc_base);

        __GPIO_REG_WRITE(p_iomuxc_regs, mux);

        mux &= (~PIN_MUX_SI_ON);

        /* 修改引脚复用之后，可能需要修改SELECT_INPUT */
        if(g_pin_cfg_map[pin].select_input_offset[mux] != 0) {

            p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].select_input_offset[mux]
                          + iomuxc_base);
            __GPIO_REG_WRITE(p_iomuxc_regs, g_pin_cfg_map[pin].select_input_val[mux]);
        }
    }

    /* 设置GPIO的属性，如上下拉、速率、滤波... */
    if (IS_PAD_CTL_SET_ENABLE(flags)) {
        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].pad_ctl_offset + iomuxc_base);

        __GPIO_REG_WRITE(p_iomuxc_regs, pad_ctl);
    }

    /* 设置GPIO通用的功能，如输入、输出... */
    if (AW_GPIO_FUNC_INVALID_VAL != func) {

            p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].mux_ctl_offset + iomuxc_base);

            /* 将GPIO设置为GPIO功能 */
            __GPIO_REG_BITS_SET(p_iomuxc_regs, 0, 3, PIN_MUX_ALT5 | PIN_MUX_SI_ON);

            switch (func) {

            case AW_GPIO_INPUT_VAL:
                __GPIO_REG_BIT_CLR(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_VAL:
                __GPIO_REG_BIT_SET(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_INIT_HIGH_VAL:
                __GPIO_REG_BIT_SET(&p_port->data, index);
                __GPIO_REG_BIT_SET(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_INIT_LOW_VAL:
                __GPIO_REG_BIT_CLR(&p_port->data, index);
                __GPIO_REG_BIT_SET(&p_port->dir, index);
                break;
            default:
                break;
            }
        }

    /* GPIO通用属性的配置，上拉、下拉、浮空、开漏 */
    if (AW_GPIO_MODE_INVALID_VAL != mode) {

        pad_ctl = 0;
        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].pad_ctl_offset + iomuxc_base);

        pad_ctl = __GPIO_REG_READ(p_iomuxc_regs);

        if (AW_GPIO_PULL_UP_VAL == mode) {

            pad_ctl |= PKE_1_Pull_Keeper_Enabled | PUE_1_Pull;
            pad_ctl &= ~(PUS_3_22K_Ohm_Pull_Up);

            /* The default pull up is 47K ohm */
            pad_ctl |= PUS_1_47K_Ohm_Pull_Up;

        } else if (AW_GPIO_PULL_DOWN_VAL == mode) {

            pad_ctl |= PKE_1_Pull_Keeper_Enabled | PUE_1_Pull;
            pad_ctl &= ~(PUS_3_22K_Ohm_Pull_Up);
            pad_ctl |= PUS_0_100K_Ohm_Pull_Down;

        } else if (AW_GPIO_FLOAT_VAL == mode) {

            pad_ctl &= ~(PKE_1_Pull_Keeper_Enabled);

        } else if (AW_GPIO_OPEN_DRAIN_VAL == mode) {

            pad_ctl |= Open_Drain_Enabled;
        }

        __GPIO_REG_WRITE(p_iomuxc_regs, pad_ctl);
    }

    __GPIO_DEV_UNLOCK(p_gpio_dev);

cleanup:
    return  err;
}


/**
 * \brief 获取GPIO引脚的当前输入
 *
 * aw_gpio_get()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \return  输入为高返回1，低返回0，失败返回负的错误码
 */
aw_local aw_err_t __gpio_get (void *p_cookie, int pin)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  val   = 0;
    uint32_t  dir   = 0;
    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;

    IMX1050_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return  -AW_EINVAL;
    }

    dir = __GPIO_REG_READ(&p_port->dir);
    dir = ((dir >> index) & 0x01);

    if (dir == 0x01) {
        val = __GPIO_REG_READ(&p_port->data);
    }
    else {
        val = __GPIO_REG_READ(&p_port->psr);
    }

    val = ((val >> index) & 0x01);

    return val;
}


/**
 * \brief 设置某个gpio引脚的输出值
 *
 * aw_gpio_set()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \param   value       0输出低，否则输出高
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_set (void *p_cookie, int pin, int value)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;
    IMX1050_GET_PORT_REG_BASE(p_port, port, p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    if (value) {
        __GPIO_REG_WRITE(&p_port->dr_set, (0x1 << index));
    } else {
        __GPIO_REG_WRITE(&p_port->dr_clear, (0x1 << index));
    }

    return AW_OK;
}


/**
 * \brief 申请一个GPIO引脚
 *
 * aw_gpio_pin_request()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   p_name      使用该引脚的外设名
 * \param   pin         引脚编号
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_pin_request (void       *p_cookie,
                                      const char *p_name,
                                      int         pin)
{
    __GPIO_DEV_DECL(p_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_dev);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_dev);

    if (p_devinfo->p_pin_cfg != NULL) {

        /* 该引脚已经被占用 */
        if (p_devinfo->p_pin_cfg[pin / 8] & AW_BIT(pin % 8)) {
            __GPIO_DEV_UNLOCK(p_dev);
            return -AW_EPERM;
        }

        p_devinfo->p_pin_cfg[pin / 8] |= AW_BIT(pin % 8);
    }

    __GPIO_DEV_UNLOCK(p_dev);

    return AW_OK;
}


/**
 * \brief 释放一个已申请的GPIO引脚
 *
 * aw_gpio_pin_release()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_pin_release (void *p_cookie, int pin)
{
    __GPIO_DEV_DECL(p_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_dev);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_dev);
    if (p_devinfo->p_pin_cfg != NULL) {
        p_devinfo->p_pin_cfg[pin / 8] &= (~AW_BIT(pin % 8));
    }
    __GPIO_DEV_UNLOCK(p_dev);

    return AW_OK;
}


/**
 * \brief 反转某个gpio引脚的输出值
 *
 * aw_gpio_toggle()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_toggle (void *p_cookie, int pin)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  val = 0;
    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;
    IMX1050_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    __GPIO_REG_WRITE(&p_port->dr_toggle, (0x1<<index));

    return AW_OK;
}


/**
 * \brief 配置GPIO引脚的触发(中断)
 *
 * aw_gpio_trigger_cfg()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \param   flags       中断触发的标志，参考aw_gpio.h获取有效的标志
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_trigger_cfg(void *p_cookie, int pin, uint32_t flags)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    aw_err_t  err   = AW_OK;
    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;
    IMX1050_GET_PORT_REG_BASE(p_port, port, p_devinfo);
    volatile  uint32_t *p_icr = NULL;
    uint32_t  val_table[4] = {0x1,   /* 高电平触发 */
                              0x0,   /* 低电平触发 */
                              0x2,   /* 上升沿触发 */
                              0x3};  /* 下降沿触发 */

    if (!__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if ((flags != AW_GPIO_TRIGGER_FALL)  &&
        (flags != AW_GPIO_TRIGGER_RISE)  &&
        (flags != AW_GPIO_TRIGGER_HIGH)  &&
        (flags != AW_GPIO_TRIGGER_LOW)   &&
        (flags != AW_GPIO_TRIGGER_BOTH_EDGES) ) {

        err  = -AW_ENOTSUP;
        goto cleanup;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    __GPIO_REG_BIT_CLR(&p_port->edge_sel, index);

    /* 如果是双边沿触发 */
    if (flags == AW_GPIO_TRIGGER_BOTH_EDGES) {
        __GPIO_REG_BIT_SET(&p_port->edge_sel, index);
    } else {

        if (index <= 15) {
            p_icr = &p_port->icr1;
        } else {
            p_icr = &p_port->icr2;
        }
        index %= 16;

        /* set pin's trigger type */
        __GPIO_REG_BITS_SET(p_icr, index * 2, 2, val_table[flags - 1]);
    }

    __GPIO_DEV_UNLOCK(p_gpio_dev);
cleanup:
    return err;
}


/**
 * \brief 打开GPIO引脚的触发功能
 *
 * aw_gpio_trigger_on()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_trigger_on (void *p_cookie, int pin)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;
    IMX1050_GET_PORT_REG_BASE(p_port, port, p_devinfo);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    /* clear the trigger status */
    __GPIO_REG_BIT_SET(&p_port->isr, index);

    /* pin's trigger on */
    __GPIO_REG_BIT_SET(&p_port->imr, index);

    __GPIO_DEV_UNLOCK(p_gpio_dev);

    return AW_OK;
}


/**
 * \brief 关闭GPIO引脚的触发功能
 *
 * aw_gpio_trigger_off()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_trigger_off (void *p_cookie, int pin)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;
    IMX1050_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return  -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    /* pin's trigger off */
    __GPIO_REG_BIT_CLR(&p_port->imr, index);

    /* clear the trigger status */
    __GPIO_REG_BIT_SET(&p_port->isr, index);

    __GPIO_DEV_UNLOCK(p_gpio_dev);

    return AW_OK;
}


/**
 * \brief 连接GPIO引脚的触发功能
 *
 * aw_gpio_trigger_connect()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \param   pfunc_callback 发生中断时的回调函数
 * \param   p_arg        调用中断回调函数的参数
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_trigger_connect (void           *p_cookie,
                                          int             pin,
                                          aw_pfuncvoid_t  pfunc_callback,
                                          void           *p_arg)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);

    aw_err_t  err   = AW_OK;
    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;

    __GPIO_DEV_LOCK(p_gpio_dev);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if (NULL != p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].pfunc_callback) {

        err = -AW_ENOSPC;  /* no isr present */
        goto cleanup;
    }

    p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].pfunc_callback = pfunc_callback;
    p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].p_arg          = p_arg;

cleanup:
    __GPIO_DEV_UNLOCK(p_gpio_dev);
    return err;
}


/**
 * \brief 断开GPIO引脚的触发
 *
 * aw_gpio_trigger_disconnect()的运行时实现
 * \param   p_cookie    运行时传下来的表征当前设备的值
 * \param   pin         引脚编号
 * \param   pfunc_callback 发生中断时的回调函数
 * \param   p_arg        调用中断回调函数的参数
 * \return  失败返回负的错误码，否则返回AW_OK
 */
aw_local aw_err_t __gpio_trigger_disconnect(void            *p_cookie,
                                            int              pin,
                                            aw_pfuncvoid_t   pfunc_isr,
                                            void            *p_arg)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);

    aw_err_t  err   = AW_OK;
    uint32_t  port  = pin / IMX1050_PORT_PINS;
    uint32_t  index = pin % IMX1050_PORT_PINS;

    __GPIO_DEV_LOCK(p_gpio_dev);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if (NULL == p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].pfunc_callback) {

        err = -AW_ENOSPC;  /* no isr present */
        goto cleanup;
    }

    p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].pfunc_callback = NULL;
    p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + index].p_arg          = NULL;

cleanup:
    __GPIO_DEV_UNLOCK(p_gpio_dev);
    return err;
}


/**
 * \brief GPIO中断处理核心函数
 *
 * 此函数通过传入的发生中断的端口和引脚索引返回来调用对应的中断回调函数
 * \param   p_gpio_dev  当前GPIO设备
 * \param   port        发生中断的端口
 * \param   min         发生中断引脚范围最小值，有效值为0和16
 * \param   max         发生中断引脚范围最大值，有效值为15和31
 * \return  无
 */
aw_local void __gpio_process_int(struct awbl_imx1050_gpio_dev *p_gpio_dev,
                                 uint32_t port,
                                 uint32_t min,
                                 uint32_t max)
{
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);
    IMX1050_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    uint32_t                        pin  = 0;
    register uint32_t               temp = 0;
    for (pin = min; pin <= max; pin++) {
        if (__GPIO_REG_BIT_GET(&p_port->isr, pin)) {

            /* 对应位写1清除中断标志 */
            __GPIO_REG_WRITE(&p_port->isr, AW_BIT(pin));
            temp = __GPIO_REG_READ(&p_port->imr);
            if((temp & AW_BIT(pin)) &&
                    (NULL != p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + pin].pfunc_callback)) {
                p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + pin].pfunc_callback (
                    p_gpio_dev->trig_info[port * IMX1050_PORT_PINS + pin].p_arg);
            }
        }
    }
}


/**
 * \brief GPIO PORT0 第0至15位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port0_0_15_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 0, 0, 15);
}

/**
 * \brief GPIO PORT0 第16至31位中断处理函数
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port0_16_31_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 0, 16, 31);
}
/**
 * \brief GPIO PORT1 第0至15位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port1_0_15_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 1, 0, 15);
}

/**
 * \brief GPIO PORT1 第16至31位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port1_16_31_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 1, 16, 31);
}

/**
 * \brief GPIO PORT2 第0至15位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port2_0_15_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 2, 0, 15);
}

/**
 * \brief GPIO PORT2第16至31位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port2_16_31_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 2, 16, 31);
}

/**
 * \brief GPIO PORT3 第0至15位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port3_0_15_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 3, 0, 15);
}

/**
 * \brief GPIO PORT3 第16至31位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port3_16_31_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 3, 16, 31);
}

/**
 * \brief GPIO PORT4 第0至15位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port4_0_15_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 4, 0, 15);
}

/**
 * \brief GPIO PORT4 第16至31位中断处理函数
 *
 * \param   p_cookie    中断子系统传下来的标志当前失败的值
 * \return  无
 */
aw_local void __gpio_port4_16_31_eint_isr(void *p_cookie)
{
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __gpio_process_int(p_gpio_dev, 4, 16, 31);
}

/******************************************************************************/
void awbl_stack_overflow_check_init(char *devicetype);

/**
 * \brief GPIO驱动初始化步骤1
 *
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __gpio_inst_init1 (awbl_dev_t *p_dev)
{
//todo:
    __GPIO_DEV_DECL(p_this, p_dev);
    __GPIO_DEVINFO_DECL(p_devinfo, p_dev);

    /* 设备锁初始化 */
    __GPIO_DEV_LOCK_INIT(p_this);

    /* 打开各端口时钟 */
    aw_clk_enable(IMX1050_CLK_CG_GPIO1);
    aw_clk_enable(IMX1050_CLK_CG_GPIO2);
    aw_clk_enable(IMX1050_CLK_CG_GPIO3);
    aw_clk_enable(IMX1050_CLK_CG_GPIO4);
    aw_clk_enable(IMX1050_CLK_CG_GPIO5);

    aw_clk_enable(IMX1050_CLK_CG_IOMUXC_SNVS_GPR);
    aw_clk_enable(IMX1050_CLK_CG_IOMUXC_GPR);
    aw_clk_enable(IMX1050_CLK_CG_IOMUXC_SNVS);
    aw_clk_enable(IMX1050_CLK_CG_IOMUXC);

    /* 平台初始化 */
    if (p_devinfo->plat_init != NULL) {
        p_devinfo->plat_init();
    }

    awbl_gpio_init();
}

/**
 * \brief GPIO驱动初始化阶段2
 *
 * 此阶段主要是使能并注册了GPIO端口中断
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __gpio_inst_init2(awbl_dev_t *p_dev)
{

    aw_int_connect(INUM_GPIO1_0_15, __gpio_port0_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO1_16_31, __gpio_port0_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO2_0_15, __gpio_port1_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO2_16_31, __gpio_port1_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO3_0_15, __gpio_port2_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO3_16_31, __gpio_port2_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO4_0_15, __gpio_port3_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO4_16_31, __gpio_port3_16_31_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO5_0_15, __gpio_port4_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO5_16_31, __gpio_port4_16_31_eint_isr, p_dev);

    aw_int_enable(INUM_GPIO1_0_15);
    aw_int_enable(INUM_GPIO1_16_31);
    aw_int_enable(INUM_GPIO2_0_15);
    aw_int_enable(INUM_GPIO2_16_31);
    aw_int_enable(INUM_GPIO3_0_15);
    aw_int_enable(INUM_GPIO3_16_31);
    aw_int_enable(INUM_GPIO4_0_15);
    aw_int_enable(INUM_GPIO4_16_31);
    aw_int_enable(INUM_GPIO5_0_15);
    aw_int_enable(INUM_GPIO5_16_31);

}

/**
 * \brief GPIO驱动初始化阶段3
 *
 * \param   p_dev   AWbus传下来表征当前设备
 * \return  无
 */
aw_local void __gpio_inst_connect(awbl_dev_t *p_dev)
{

}

/**
 * \brief GPIO服务函数表
 */
aw_local aw_const struct awbl_gpio_servfuncs __g_gpio_servfuncs = {
    __gpio_pin_cfg,                 /* pfunc_gpio_pin_cfg */
    __gpio_get,                     /* pfunc_gpio_get */
    __gpio_set,                     /* pfunc_gpio_set */
    __gpio_toggle,                  /* pfunc_gpio_toggle */
    __gpio_trigger_cfg,             /* pfunc_gpio_trigger_cfg */
    __gpio_trigger_connect,         /* pfunc_gpio_trigger_connect */
    __gpio_trigger_disconnect,      /* pfunc_gpio_trigger_disconnect */
    __gpio_trigger_on,              /* pfunc_gpio_trigger_on */
    __gpio_trigger_off,             /* pfunc_gpio_trigger_off */
    __gpio_pin_request,             /* pfunc_gpio_pin_request */
    __gpio_pin_release,             /* pfunc_gpio_pin_release */
};

/**
 * \brief GPIO中断服务函数表
 */
aw_local aw_const struct awbl_intctlr_servfuncs __g_intctlr_servfuncs = {
    __gpio_trigger_connect,         /* pfunc_int_connect */
    __gpio_trigger_disconnect,      /* pfunc_int_disconnect */
    __gpio_trigger_on,              /* pfunc_int_enable */
    __gpio_trigger_off              /* pfunc_int_disable */
};

/**
 * \brief 获取GPIO服务的函数列表
 *
 * awbl_gpioserv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出GPIO服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __gpio_gpioserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GPIO_DEV_DECL(p_this, p_dev);
    __GPIO_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_gpio_service   *p_serv = &p_this->gpio_serv;

    /* initialize the gpio service instance */
    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->gpio_info;
    p_serv->p_servfuncs = &__g_gpio_servfuncs;
    p_serv->p_cookie    = (void *)p_this;

    /* send back the service instance */
    *(struct awbl_gpio_service **)p_arg = p_serv;

    return AW_OK;
}


/**
 * \brief 获取GPIO中断服务的函数列表
 *
 * awbl_icserv_get的运行时实现
 * \param   p_dev   AWbus传下来表征当前设备
 * \param   p_arg   用于输出GPIO中断服务函数列表的指针
 * \return  无
 */
aw_local aw_err_t __gpio_icserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __GPIO_DEV_DECL(p_this, p_dev);
    __GPIO_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_intctlr_service            *p_serv = &p_this->intctlr_serv;

    /* get intcltr service instance */

    p_serv = &p_this->intctlr_serv;

    /* initialize the gpio service instance */

    p_serv->p_next      = NULL;
    p_serv->p_servinfo  = &p_devinfo->int_info;
    p_serv->p_servfuncs = &__g_intctlr_servfuncs;
    p_serv->p_cookie    = (void *)p_dev;

    /* send back the service instance */

    *(struct awbl_intctlr_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief GPIO驱动AWbus驱动函数
 */
aw_local aw_const struct awbl_drvfuncs __g_gpio_drvfuncs = {
    __gpio_inst_init1,
    __gpio_inst_init2,
    __gpio_inst_connect
};

/******************************************************************************/

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gpioserv_get);
AWBL_METHOD_IMPORT(awbl_icserv_get);

/**
 * \brief GPIO驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_gpio_dev_methods[] = {
    AWBL_METHOD(awbl_gpioserv_get, __gpio_gpioserv_get),
    AWBL_METHOD(awbl_icserv_get, __gpio_icserv_get),
    AWBL_METHOD_END
};


/**
 * \brief GPIO驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_gpio_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_IMX1050_GPIO_NAME,     /* p_drvname */
        &__g_gpio_drvfuncs,         /* p_busfuncs */
        &__g_gpio_dev_methods[0],   /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx1050_gpio_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gpio_drv_registration);
}


/* end of file */
