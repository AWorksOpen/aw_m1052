/*******************************************************************************
*                                 AWorks
*                       ---------------------------
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
 * \brief iMX RT1020 GPIO driver
 *
 * this driver is the implementation of aw_gpio.h for iMX RT1020
 *
 * \internal
 * \par modification history:
 * - 1.00 18-09-25, mex, first implementation.
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
#include "imx1020_pin.h"
#include "imx1020_iomuxc_regs.h"
#include "imx1020_reg_base.h"
#include "imx1020_inum.h"

#include "driver/gpio/awbl_imx1020_gpio.h"

#ifdef CRP_ENABLE
#include "awbl_stack_overflow_check.h"
#endif


/* declare sim device instance */
#define __GPIO_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1020_gpio_dev *p_this = \
        (struct awbl_imx1020_gpio_dev *)(p_dev)

/* declare sim device infomation */
#define __GPIO_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1020_gpio_devinfo *p_devinfo = \
        (struct awbl_imx1020_gpio_devinfo *)AWBL_DEVINFO_GET(p_dev)

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
    (((pin) >= 0) && ((pin) < (IMX1020_GPIO_PINS)))


/* 获取端口的寄存器基地址*/
#define IMX1020_GET_PORT_REG_BASE(p_port, port, p_devinfo)    \
    imx1020_gpio_port_regs_t  *p_port =                       \
                     (imx1020_gpio_port_regs_t  *)p_devinfo->gpio_regs_base[port];


/**
 * \brief IMX1020 GPIO每端口寄存器
 */
typedef struct __tag_imx1020_gpio_port_regs
{
    uint32_t data;          /**< \brief GPIO data register */
    uint32_t dir;           /**< \brief GPIO direction register */
    uint32_t psr;           /**< \brief GPIO pad status register */
    uint32_t icr1;          /**< \brief GPIO interrupt configuration register1 */
    uint32_t icr2;          /**< \brief GPIO interrupt configuration register2 */
    uint32_t imr;           /**< \brief GPIO interrupt mask register */
    uint32_t isr;           /**< \brief GPIO interrupt status register */
    uint32_t edge_sel;      /**< \brief GPIO edge select register */
    uint32_t reserved[25];
    uint32_t data_set;
    uint32_t data_clr;
    uint32_t data_toggle;
} imx1020_gpio_port_regs_t;


/**
 * \brief IMX1020 GPIO引脚配置映射表基础结构
 */
typedef struct __tag_imx1020_gpio_pin_cfg_map {

    uint16_t mux_ctl_offset;
    uint16_t pad_ctl_offset;
    uint16_t select_input_offset[8];
    uint16_t select_input_val[8];
} imx1020_pin_cfg_map_t;


aw_local const imx1020_pin_cfg_map_t g_pin_cfg_map[IMX1020_GPIO_PINS] = {

    /* GPIO1_0 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO1_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO1_2 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO1_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CD_B_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG_OC_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[7] = 0x02,
    },

    /* GPIO1_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER0_INPUT_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[4] = 0x01,
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
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER1_INPUT_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, ANATOP_USB_OTG_ID_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, NMI_SELECT_INPUT),
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
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER2_INPUT_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
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
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER3_INPUT_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_08),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_TX_CLK_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SCL_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RMII_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_09),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_DATA1_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SDA_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
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

    /* GPIO1_10 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_10),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_DATA0_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SCK_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA2_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_11 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_11),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_EN_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_PCS0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB2_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_12 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_12),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_ERR_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDO_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA1_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B0_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B0_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDI_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB1_SELECT_INPUT),
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
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA0_SELECT_INPUT),
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
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[1] = 0x02,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_00),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, SEMC_READY_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK_SELECT_INPUT),
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

    /* GPIO1_17 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_SCLK_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[2] = 0x03,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
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

    /* GPIO1_18 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SCK_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
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

    /* GPIO1_19 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_PCS0_SELECT_INPUT),
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

    /* GPIO1_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDO_SELECT_INPUT),
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

    /* GPIO1_21 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_05),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[0] = 0x02,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDI_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
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

    /* GPIO1_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_CTS_B_SELECT_INPUT),
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

    /* GPIO1_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO1_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_08),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SCL_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA2_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN12_SELECT_INPUT),
    .select_input_val[7] = 0x01,
    },

    /* GPIO1_25 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_09),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SDA_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA1_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN13_SELECT_INPUT),
    .select_input_val[7] = 0x01,
    },

    /* GPIO1_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
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

    /* GPIO1_27 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_11),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, ANATOP_USB_OTG_ID_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[3] = 0x03,
    .select_input_offset[4] = 0,
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
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG_OC_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CD_B_SELECT_INPUT),
    .select_input_val[3] = 0x02,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_29 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_WP_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO1_30 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_14),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SCL_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO1_31 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_AD_B1_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_AD_B1_15),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SDA_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

/**************************************************************************/

    /* GPIO2_0 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER0_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_CTS_B_SELECT_INPUT),
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

    /* GPIO2_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER1_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_2 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER2_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDO_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SCL_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER2_TIMER3_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDI_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C1_SDA_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_BCLK_SELECT_INPUT),
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

    /* GPIO2_5 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SPDIF_IN_SELECT_INPUT),
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

    /* GPIO2_6 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_06),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_TX_SELECT_INPUT),
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

    /* GPIO2_7 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART3_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_SYNC_SELECT_INPUT),
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

    /* GPIO2_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_DATA0_SELECT_INPUT),
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

    /* GPIO2_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_BCLK_SELECT_INPUT),
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

    /* GPIO2_10 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN10_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SDA_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SCK_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_11 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SCL_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_12 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_12),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_12),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN12_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDO_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_13),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_13),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN13_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDI_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO2_14 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_14),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_14),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN14_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_BCLK_SELECT_INPUT),
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

    /* GPIO2_15 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_15),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_15),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN15_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[6] = 0x03,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO2_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_16),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_16),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_MCLK_SELECT_INPUT),
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

    /* GPIO2_17 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_17),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_17),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_MCLK_SELECT_INPUT),
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

    /* GPIO2_18 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_18),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_18),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN16_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SDA_SELECT_INPUT),
    .select_input_val[2] = 0x01,
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

    /* GPIO2_19 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_19),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_19),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C2_SCL_SELECT_INPUT),
    .select_input_val[2] = 0x01,
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

    /* GPIO2_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_20),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_20),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_CTS_B_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK_SELECT_INPUT),
    .select_input_val[3] = 0x03,
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_21),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_21),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_RX_DATA0_SELECT_INPUT),
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

    /* GPIO2_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_22),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_22),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_TX_SELECT_INPUT),
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

    /* GPIO2_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_23),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_23),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART2_RX_SELECT_INPUT),
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

    /* GPIO2_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_24),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_24),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
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

    /* GPIO2_25 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_25),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_25),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
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

    /* GPIO2_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_26),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_26),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_BCLK_SELECT_INPUT),
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

    /* GPIO2_27 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_27),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_27),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM1_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_TX_SYNC_SELECT_INPUT),
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

    /* GPIO2_28 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_28),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_28),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN18_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_MCLK_SELECT_INPUT),
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

    /* GPIO2_29 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t,SW_MUX_CTL_PAD_GPIO_EMC_29),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t,SW_PAD_CTL_PAD_GPIO_EMC_29),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB3_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN19_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_BCLK_SELECT_INPUT),
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

    /* GPIO2_30 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_30),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_30),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_CTS_B_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_SYNC_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_31),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_31),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB2_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_DATA0_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_32),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_32),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER0_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_TX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SCK_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_1 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_33),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_33),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER1_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART4_RX_SELECT_INPUT),
    .select_input_val[2] = 0x02,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_2 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_34),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_34),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER2_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDO_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_3 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_35),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_35),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER3_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_WP_SELECT_INPUT),
    .select_input_val[3] = 0x01,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI4_SDI_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_4 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_36),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_36),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[3] = 0x03,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_WP_SELECT_INPUT),
    .select_input_val[7] = 0x04,
    },

    /* GPIO3_5 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_37),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_37),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB1_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
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

    /* GPIO3_6 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_38),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_38),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMA0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[7] = 0x03,
    },

    /* GPIO3_7 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_39),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_39),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXPWM2_PWMB0_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART5_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, USB_OTG_OC_SELECT_INPUT),
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

    /* GPIO3_8 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_40),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_40),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN18_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ANATOP_USB_OTG_ID_SELECT_INPUT),
    .select_input_val[3] = 0x2,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_9 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_EMC_41),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_EMC_41),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, SEMC_READY_SELECT_INPUT),
    .select_input_val[0] = 0x01,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN19_SELECT_INPUT),
    .select_input_val[1] = 0x01,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SPDIF_IN_SELECT_INPUT),
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

    /* GPIO3_10 此引脚不存在，仅作占位 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO3_11 此引脚不存在，仅作占位 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO3_12  此引脚不存在，仅作占位 */
    {
    .mux_ctl_offset = 0,
    .pad_ctl_offset = 0,
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
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

    /* GPIO3_13 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER0_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, SAI1_MCLK_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_MCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SCL_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN14_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO3_14 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER1_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C3_SDA_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN15_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO3_15 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER2_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SCK_SELECT_INPUT),
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, ENET_MDIO_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN16_SELECT_INPUT),
    .select_input_val[7] = 0,
    },

    /* GPIO3_16 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, QTIMER1_TIMER3_INPUT_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_RX_DATA0_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_PCS0_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDO_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN2_RX_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART7_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI1_SDI_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B0_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B0_06),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC1_CD_B_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = 0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI2_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN17_SELECT_INPUT),
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_20 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_00),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_00),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_TX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, XBAR1_IN10_SELECT_INPUT),
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

    /* GPIO3_21 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_01),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_01),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART6_RX_SELECT_INPUT),
    .select_input_val[2] = 0x01,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, FLEXCAN1_RX_SELECT_INPUT),
    .select_input_val[4] = 0x01,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_22 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_02),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_02),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_TX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SCL_SELECT_INPUT),
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

    /* GPIO3_23 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_03),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_03),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, LPUART8_RX_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, LPI2C4_SDA_SELECT_INPUT),
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

    /* GPIO3_24 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_04),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_04),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, ENET_TX_CLK_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RMII_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_05),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_05),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_DATA1_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_MCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = 0,
    .select_input_val[4] = 0,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = AW_OFFSET(hw_iomuxc_regs_t, CCM_PMIC_READY_SELECT_INPUT),
    .select_input_val[6] = 0x01,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_26 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_06),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_06),
    .select_input_offset[0] = AW_OFFSET(hw_iomuxc_regs_t, USDHC2_CD_B_SELECT_INPUT),
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA3_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_DATA0_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_TX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_PCS0_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_27 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_07),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_07),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_SCLK_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_EN_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_TX_SYNC_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SCK_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_28 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_08),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_08),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA0_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] = AW_OFFSET(hw_iomuxc_regs_t, ENET_RX_ERR_SELECT_INPUT),
    .select_input_val[2] = 0,
    .select_input_offset[3] = 0,
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDO_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_29 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_09),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_09),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA2_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_BCLK_SELECT_INPUT),
    .select_input_val[3] = 0,
    .select_input_offset[4] = AW_OFFSET(hw_iomuxc_regs_t, LPSPI2_SDI_SELECT_INPUT),
    .select_input_val[4] = 0x02,
    .select_input_offset[5] = 0,
    .select_input_val[5] = 0,
    .select_input_offset[6] = 0,
    .select_input_val[6] = 0,
    .select_input_offset[7] = 0,
    .select_input_val[7] = 0,
    },

    /* GPIO3_30 */
    {
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_10),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_10),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = AW_OFFSET(hw_iomuxc_regs_t, FLEXSPI_A_DATA1_SELECT_INPUT),
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_SYNC_SELECT_INPUT),
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
    .mux_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_MUX_CTL_PAD_GPIO_SD_B1_11),
    .pad_ctl_offset = AW_OFFSET(hw_iomuxc_regs_t, SW_PAD_CTL_PAD_GPIO_SD_B1_11),
    .select_input_offset[0] = 0,
    .select_input_val[0] = 0,
    .select_input_offset[1] = 0,
    .select_input_val[1] = 0,
    .select_input_offset[2] =0,
    .select_input_val[2] = 0,
    .select_input_offset[3] = AW_OFFSET(hw_iomuxc_regs_t, SAI3_RX_DATA0_SELECT_INPUT),
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
    .select_input_offset[7] = AW_OFFSET(hw_iomuxc_regs_t, NMI_SELECT_INPUT),
    .select_input_val[7] = 0x01,
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
    __GPIO_DEV_DECL(p_gpio_dev, p_cookie);
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);

    uint32_t  func    = AW_GPIO_FUNCBITS_GET(flags);
    uint32_t  mode    = AW_GPIO_MODEBITS_GET(flags);

    uint32_t  mux     = IMX1020_PIN_MUX_GET(flags);
    uint32_t  pad_ctl = IMX1020_PAD_CTL_GET(flags);

    uint32_t  port    = pin / IMX1020_PORT_PINS;
    uint32_t  index   = pin % IMX1020_PORT_PINS;
    aw_err_t  err     = AW_OK;
    uint32_t  *p_iomuxc_regs = NULL;
    uint32_t  iomuxc_base = 0;

    IMX1020_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    if (p_devinfo->p_pin_cfg != NULL) {

        /* 配置引脚前需要请求引脚 */
        if (!(p_devinfo->p_pin_cfg[pin / 8] & AW_BIT(pin % 8))) {
            while (1);
        }
    }

    if (3 == port) {
        iomuxc_base = p_devinfo->iomuxc_snvs_base;
    } else {
        iomuxc_base = p_devinfo->iomuxc_base;
    }

    /* 首先设置引脚复用功能 */
    if (IS_PIN_MUX_SET_ENABLE(flags)) {

        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].mux_ctl_offset + iomuxc_base);

        writel(mux,p_iomuxc_regs);

        mux &= (~PIN_MUX_SI_ON);

        /* 修改引脚复用之后，可能需要修改SELECT_INPUT */
        if(g_pin_cfg_map[pin].select_input_offset[mux] != 0) {

            p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].select_input_offset[mux]
                          + iomuxc_base);
            writel(g_pin_cfg_map[pin].select_input_val[mux],p_iomuxc_regs);
        }
    }

    /* 设置GPIO的属性，如上下拉、速率、滤波... */
    if (IS_PAD_CTL_SET_ENABLE(flags)) {
        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].pad_ctl_offset + iomuxc_base);
        writel(pad_ctl,p_iomuxc_regs);
    }

    /* 设置GPIO通用的功能，如输入、输出... */
    if (AW_GPIO_FUNC_INVALID_VAL != func) {

            p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].mux_ctl_offset + iomuxc_base);

            /* 将GPIO设置为GPIO功能 */
            writel(PIN_MUX_ALT5,p_iomuxc_regs);

            switch (func) {

            case AW_GPIO_INPUT_VAL:
                AW_REG_BIT_CLR32(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_VAL:
                AW_REG_BIT_SET32(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_INIT_HIGH_VAL:
                AW_REG_BIT_SET32(&p_port->data, index);
                AW_REG_BIT_SET32(&p_port->dir, index);
                break;
            case AW_GPIO_OUTPUT_INIT_LOW_VAL:
                AW_REG_BIT_CLR32(&p_port->data, index);
                AW_REG_BIT_SET32(&p_port->dir, index);
                break;
            default:
                break;
            }
        }

    /* GPIO通用属性的配置，上拉、下拉、浮空、开漏 */
    if (AW_GPIO_MODE_INVALID_VAL != mode) {

        pad_ctl = 0;
        p_iomuxc_regs = (uint32_t *)(g_pin_cfg_map[pin].pad_ctl_offset + iomuxc_base);

        pad_ctl = readl(p_iomuxc_regs);

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

        writel(pad_ctl,p_iomuxc_regs);
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
    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;

    IMX1020_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return  -AW_EINVAL;
    }

    val = readl(&p_port->psr);
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

    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;
    IMX1020_GET_PORT_REG_BASE(p_port, port, p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }


    if (value) {
        writel(AW_BIT(index),&p_port->data_set);
    } else {
        writel(AW_BIT(index),&p_port->data_clr);
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
            while (1);
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

    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;

    IMX1020_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    writel(AW_BIT(index),&p_port->data_toggle);

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
    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;
    IMX1020_GET_PORT_REG_BASE(p_port, port, p_devinfo);
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

    AW_REG_BIT_CLR32(&p_port->edge_sel, index);

    /* 如果是双边沿触发 */
    if (flags == AW_GPIO_TRIGGER_BOTH_EDGES) {
        AW_REG_BIT_SET32(&p_port->edge_sel, index);
    } else {

        if (index <= 15) {
            p_icr = &p_port->icr1;
        } else {
            p_icr = &p_port->icr2;
        }
        index %= 16;

        /* set pin's trigger type */
        AW_REG_BITS_SET32(p_icr, index * 2, 2, val_table[flags - 1]);
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

    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;
    IMX1020_GET_PORT_REG_BASE(p_port, port, p_devinfo);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        return -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    /* clear the trigger status */
    AW_REG_BIT_SET32(&p_port->isr, index);

    /* pin's trigger on */
    AW_REG_BIT_SET32(&p_port->imr, index);

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

    uint32_t  port  = pin / IMX1020_PORT_PINS;
    uint32_t  index = pin % IMX1020_PORT_PINS;
    IMX1020_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    if ( !__GPIO_PIN_IS_VALID(pin)) {
        return  -AW_EINVAL;
    }

    __GPIO_DEV_LOCK(p_gpio_dev);

    /* pin's trigger off */
    AW_REG_BIT_CLR32(&p_port->imr, index);

    /* clear the trigger status */
    AW_REG_BIT_CLR32(&p_port->isr, index);

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

    __GPIO_DEV_LOCK(p_gpio_dev);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if (NULL != p_gpio_dev->trig_info[pin].pfunc_callback) {

        err = -AW_ENOSPC;  /* no isr present */
        goto cleanup;
    }

    p_gpio_dev->trig_info[pin].pfunc_callback = pfunc_callback;
    p_gpio_dev->trig_info[pin].p_arg          = p_arg;

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

    __GPIO_DEV_LOCK(p_gpio_dev);

    if (!__GPIO_PIN_IS_VALID(pin)) {
        err = -AW_EINVAL;
        goto cleanup;
    }

    if (NULL == p_gpio_dev->trig_info[pin].pfunc_callback) {

        err = -AW_ENOSPC;  /* no isr present */
        goto cleanup;
    }

    p_gpio_dev->trig_info[pin].pfunc_callback = NULL;
    p_gpio_dev->trig_info[pin].p_arg          = NULL;

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
aw_local void __gpio_process_int(struct awbl_imx1020_gpio_dev *p_gpio_dev,
                                 uint32_t port,
                                 uint32_t min,
                                 uint32_t max)
{
    __GPIO_DEVINFO_DECL(p_devinfo, p_gpio_dev);
    IMX1020_GET_PORT_REG_BASE(p_port, port ,p_devinfo);

    uint32_t                                pin_index;
    register uint32_t                       mask;
    uint32_t                                status;
    uint32_t                                int_en;
    uint32_t                                pin;
    struct awbl_imx1020_gpio_trigger_info  *p_trig_info;


    status = readl(&p_port->isr);
    int_en = readl(&p_port->imr);
    for (pin_index = min; pin_index <= max; pin_index++) {
        mask = AW_BIT(pin_index);
        if (status & mask) {
            writel(mask,&p_port->isr);
            if (int_en & mask) {
                pin = port * IMX1020_PORT_PINS + pin_index;
                p_trig_info = &p_gpio_dev->trig_info[pin];
                if (NULL !=  p_trig_info->pfunc_callback ) {
                    p_trig_info->pfunc_callback(p_trig_info->p_arg);
                }
                // 处理了一个中断就退出，防止中断过多影响高优先级中断
                break;
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
    aw_clk_enable(IMX1020_CLK_CG_GPIO1);
    aw_clk_enable(IMX1020_CLK_CG_GPIO2);
    aw_clk_enable(IMX1020_CLK_CG_GPIO3);

    aw_clk_enable(IMX1020_CLK_CG_IOMUXC_SNVS_GPR);
    aw_clk_enable(IMX1020_CLK_CG_IOMUXC_GPR);
    aw_clk_enable(IMX1020_CLK_CG_IOMUXC_SNVS);
    aw_clk_enable(IMX1020_CLK_CG_IOMUXC);

    /* 平台初始化 */
    if (p_devinfo->plat_init != NULL) {
        p_devinfo->plat_init();
    }

    awbl_gpio_init();
#ifdef CRP_ENABLE
    //此代码请放在awbl_wdt_init中，在加密烧写代码之后，调用
    awbl_stack_overflow_check_init(NULL);

    if (awbl_stack_overflow_check() != 0) {
        volatile int    *p = NULL;
        volatile int    div0 = 0;
        *p = 0;
        aw_kprintf("the stack overflow check failed,%d\n",(*p)/div0);
    }

#endif
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
    aw_int_connect(INUM_GPIO5_0_15, __gpio_port4_0_15_eint_isr, p_dev);
    aw_int_connect(INUM_GPIO5_16_31, __gpio_port4_16_31_eint_isr, p_dev);

    aw_int_enable(INUM_GPIO1_0_15);
    aw_int_enable(INUM_GPIO1_16_31);
    aw_int_enable(INUM_GPIO2_0_15);
    aw_int_enable(INUM_GPIO2_16_31);
    aw_int_enable(INUM_GPIO3_0_15);
    aw_int_enable(INUM_GPIO3_16_31);
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
#ifdef CRP_ENABLE
    if (awbl_stack_overflow_check() != 0) {
        volatile int    *p = NULL;
        volatile int    div0 = 0;
        *p = 0;
        aw_kprintf("the stack overflow check failed,%d\n",(*p)/div0);
    }

#endif
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

/**
 * \brief GPIO驱动设备专有函数
 */
aw_local aw_const struct awbl_dev_method __g_gpio_dev_methods[] = {
    AWBL_METHOD(awbl_gpioserv_get, __gpio_gpioserv_get),
    AWBL_METHOD_END
};


/**
 * \brief GPIO驱动注册信息
 */
aw_local aw_const awbl_plb_drvinfo_t __g_gpio_drv_registration = {
    {
        AWBL_VER_1,                 /* awb_ver */
        AWBL_BUSID_PLB,             /* bus_id */
        AWBL_IMX1020_GPIO_NAME,     /* p_drvname */
        &__g_gpio_drvfuncs,         /* p_busfuncs */
        &__g_gpio_dev_methods[0],   /* p_methods */
        NULL                        /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx1020_gpio_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gpio_drv_registration);
}


/* end of file */
