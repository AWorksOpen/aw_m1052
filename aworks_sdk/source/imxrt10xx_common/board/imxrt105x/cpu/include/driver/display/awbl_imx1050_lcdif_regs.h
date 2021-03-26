/*******************************************************************************
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
 * \brief iMX RT1050 eLCDIF¼Ä´æÆ÷¶¨Òå
 *
 * \internal
 * \par Modification History
 * - 1.00  17-11-21  mex, first implementation.
 * \endinternal
 */

#ifndef __AWBL_IMX1050_LCDIF_REGS_H
#define __AWBL_IMX1050_LCDIF_REGS_H

#define   __I     volatile const          /**< \brief \brief Defines 'read only' permissions */
#define   __O     volatile                /**< \brief \brief Defines 'write only' permissions */
#define   __IO    volatile                /**< \brief \brief Defines 'read / write' permissions */

/**
 * \brief LCDIF - Register Layout Typedef
 */
typedef struct {

    __IO uint32_t CTRL;                     /**< \brief eLCDIF General Control Register, offset: 0x0 */
    __IO uint32_t CTRL_SET;                 /**< \brief eLCDIF General Control Register, offset: 0x4 */
    __IO uint32_t CTRL_CLR;                 /**< \brief eLCDIF General Control Register, offset: 0x8 */
    __IO uint32_t CTRL_TOG;                 /**< \brief eLCDIF General Control Register, offset: 0xC */
    __IO uint32_t CTRL1;                    /**< \brief eLCDIF General Control1 Register, offset: 0x10 */
    __IO uint32_t CTRL1_SET;                /**< \brief eLCDIF General Control1 Register, offset: 0x14 */
    __IO uint32_t CTRL1_CLR;                /**< \brief eLCDIF General Control1 Register, offset: 0x18 */
    __IO uint32_t CTRL1_TOG;                /**< \brief eLCDIF General Control1 Register, offset: 0x1C */
    __IO uint32_t CTRL2;                    /**< \brief eLCDIF General Control2 Register, offset: 0x20 */
    __IO uint32_t CTRL2_SET;                /**< \brief eLCDIF General Control2 Register, offset: 0x24 */
    __IO uint32_t CTRL2_CLR;                /**< \brief eLCDIF General Control2 Register, offset: 0x28 */
    __IO uint32_t CTRL2_TOG;                /**< \brief eLCDIF General Control2 Register, offset: 0x2C */
    __IO uint32_t TRANSFER_COUNT;           /**< \brief eLCDIF Horizontal and Vertical Valid Data Count Register, offset: 0x30 */
         uint8_t RESERVED_0[12];
    __IO uint32_t CUR_BUF;                  /**< \brief LCD Interface Current Buffer Address Register, offset: 0x40 */
         uint8_t RESERVED_1[12];
    __IO uint32_t NEXT_BUF;                 /**< \brief LCD Interface Next Buffer Address Register, offset: 0x50 */
         uint8_t RESERVED_2[28];
    __IO uint32_t VDCTRL0;                  /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x70 */
    __IO uint32_t VDCTRL0_SET;              /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x74 */
    __IO uint32_t VDCTRL0_CLR;              /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x78 */
    __IO uint32_t VDCTRL0_TOG;              /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register0, offset: 0x7C */
    __IO uint32_t VDCTRL1;                  /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register1, offset: 0x80 */
         uint8_t RESERVED_3[12];
    __IO uint32_t VDCTRL2;                  /**< \brief LCDIF VSYNC Mode and Dotclk Mode Control Register2, offset: 0x90 */
         uint8_t RESERVED_4[12];
    __IO uint32_t VDCTRL3;                  /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register3, offset: 0xA0 */
         uint8_t RESERVED_5[12];
    __IO uint32_t VDCTRL4;                  /**< \brief eLCDIF VSYNC Mode and Dotclk Mode Control Register4, offset: 0xB0 */
         uint8_t RESERVED_6[220];
    __IO uint32_t BM_ERROR_STAT;            /**< \brief Bus Master Error Status Register, offset: 0x190 */
         uint8_t RESERVED_7[12];
    __IO uint32_t CRC_STAT;                 /**< \brief CRC Status Register, offset: 0x1A0 */
         uint8_t RESERVED_8[12];
    __I  uint32_t STAT;                     /**< \brief LCD Interface Status Register, offset: 0x1B0 */
         uint8_t RESERVED_9[76];
    __IO uint32_t THRES;                    /**< \brief eLCDIF Threshold Register, offset: 0x200 */
         uint8_t RESERVED_10[380];
    __IO uint32_t PIGEONCTRL0;              /**< \brief LCDIF Pigeon Mode Control0 Register, offset: 0x380 */
    __IO uint32_t PIGEONCTRL0_SET;          /**< \brief LCDIF Pigeon Mode Control0 Register, offset: 0x384 */
    __IO uint32_t PIGEONCTRL0_CLR;          /**< \brief LCDIF Pigeon Mode Control0 Register, offset: 0x388 */
    __IO uint32_t PIGEONCTRL0_TOG;          /**< \brief LCDIF Pigeon Mode Control0 Register, offset: 0x38C */
    __IO uint32_t PIGEONCTRL1;              /**< \brief LCDIF Pigeon Mode Control1 Register, offset: 0x390 */
    __IO uint32_t PIGEONCTRL1_SET;          /**< \brief LCDIF Pigeon Mode Control1 Register, offset: 0x394 */
    __IO uint32_t PIGEONCTRL1_CLR;          /**< \brief LCDIF Pigeon Mode Control1 Register, offset: 0x398 */
    __IO uint32_t PIGEONCTRL1_TOG;          /**< \brief LCDIF Pigeon Mode Control1 Register, offset: 0x39C */
    __IO uint32_t PIGEONCTRL2;              /**< \brief LCDIF Pigeon Mode Control2 Register, offset: 0x3A0 */
    __IO uint32_t PIGEONCTRL2_SET;          /**< \brief LCDIF Pigeon Mode Control2 Register, offset: 0x3A4 */
    __IO uint32_t PIGEONCTRL2_CLR;          /**< \brief LCDIF Pigeon Mode Control2 Register, offset: 0x3A8 */
    __IO uint32_t PIGEONCTRL2_TOG;          /**< \brief LCDIF Pigeon Mode Control2 Register, offset: 0x3AC */
         uint8_t RESERVED_11[1104];
    struct {                                /* offset: 0x800, array step: 0x40 */
        __IO uint32_t PIGEON_0;             /**< \brief Panel Interface Signal Generator Register, array offset: 0x800, array step: 0x40 */
             uint8_t RESERVED_0[12];
        __IO uint32_t PIGEON_1;             /**< \brief Panel Interface Signal Generator Register, array offset: 0x810, array step: 0x40 */
             uint8_t RESERVED_1[12];
        __IO uint32_t PIGEON_2;             /**< \brief Panel Interface Signal Generator Register, array offset: 0x820, array step: 0x40 */
             uint8_t RESERVED_2[28];
    } PIGEON[12];
    __IO uint32_t LUT_CTRL;                 /**< \brief Lookup Table Data Register., offset: 0xB00 */
         uint8_t RESERVED_12[12];
    __IO uint32_t LUT0_ADDR;                /**< \brief Lookup Table Control Register., offset: 0xB10 */
         uint8_t RESERVED_13[12];
    __IO uint32_t LUT0_DATA;                /**< \brief Lookup Table Data Register., offset: 0xB20 */
         uint8_t RESERVED_14[12];
    __IO uint32_t LUT1_ADDR;                /**< \brief Lookup Table Control Register., offset: 0xB30 */
         uint8_t RESERVED_15[12];
    __IO uint32_t LUT1_DATA;                /**< \brief Lookup Table Data Register., offset: 0xB40 */
} imx1050_lcdif_regs_t;


/*******************************************************************************
    eLCDIF General Control Register
*******************************************************************************/

/*
 *  When this bit is set by software, the eLCDIF will begin transferring data
 *  between the SoC and the display
 */
#define __LCDIF_CTRL_RUN_SHIFT                0
#define __LCDIF_CTRL_RUN_MASK                (0x01 << __LCDIF_CTRL_RUN_SHIFT)

/* Used only when WORD_LENGTH = 3, i.e. 24-bit */
#define __LCDIF_CTRL_DATA_FORMAT_24BIT_SHIFT  1
#define __LCDIF_CTRL_DATA_FORMAT_24BIT_MASK  (0x01 << __LCDIF_CTRL_DATA_FORMAT_24BIT_SHIFT)

/* Used only when WORD_LENGTH = 2, i.e. 18-bit */
#define __LCDIF_CTRL_DATA_FORMAT_18BIT_SHIFT  2
#define __LCDIF_CTRL_DATA_FORMAT_18BIT_MASK  (0x01 << __LCDIF_CTRL_DATA_FORMAT_18BIT_SHIFT)

/* When this bit is 1 and WORD_LENGTH = 0, it implies that the 16-bit data is in ARGB555 format */
#define __LCDIF_CTRL_DATA_FORMAT_16BIT_SHIFT  3
#define __LCDIF_CTRL_DATA_FORMAT_16BIT_MASK  (0x01 << __LCDIF_CTRL_DATA_FORMAT_16BIT_SHIFT)

/* Set this bit to make the eLCDIF act as a bus master */
#define __LCDIF_CTRL_MASTER_SHIFT             5
#define __LCDIF_CTRL_MASTER_MASK             (0x01 << __LCDIF_CTRL_MASTER_SHIFT)

/*
 * If this bit is set and LCDIF_MASTER bit is set, the eLCDIF will act as bus
 * master and the handshake mechanism between eLCDIF and PXP will be turned on
 */
#define __LCDIF_CTRL_EN_PXP_HANDSHAKE_SHIFT   6
#define __LCDIF_CTRL_EN_PXP_HANDSHAKE_MASK   (0x01 << __LCDIF_CTRL_EN_PXP_HANDSHAKE_SHIFT)

/* Input data format */
#define __LCDIF_CTRL_WORD_LENGTH_SHIFT        8
#define __LCDIF_CTRL_WORD_LENGTH_MASK        (0x03 << __LCDIF_CTRL_WORD_LENGTH_SHIFT)
#define __LCDIF_CTRL_WORD_LENGTH(x)          (((x) << __LCDIF_CTRL_WORD_LENGTH_SHIFT) & \
                                                      __LCDIF_CTRL_WORD_LENGTH_MASK)

/* LCD Data bus transfer width */
#define __LCDIF_CTRL_DATABUS_WIDTH_SHIFT      10
#define __LCDIF_CTRL_DATABUS_WIDTH_MASK      (0x03 << __LCDIF_CTRL_DATABUS_WIDTH_SHIFT)
#define __LCDIF_CTRL_DATABUS_WIDTH(x)        (((x) << __LCDIF_CTRL_DATABUS_WIDTH_SHIFT) & \
                                                      __LCDIF_CTRL_DATABUS_WIDTH_MASK)
/*
 * This field specifies how to swap the bytes after the data has been converted
 * into an internal representation of 24 bits per pixel and before it is transmitted
 * over the LCD interface bus.
 */
#define __LCDIF_CTRL_CSC_DATA_SWIZZLE_SHIFT   12
#define __LCDIF_CTRL_CSC_DATA_SWIZZLE_MASK   (0x02 << __LCDIF_CTRL_CSC_DATA_SWIZZLE_SHIFT)
#define __LCDIF_CTRL_CSC_DATA_SWIZZLE(x)     (((x) << __LCDIF_CTRL_CSC_DATA_SWIZZLE_SHIFT) & \
                                                         __LCDIF_CTRL_CSC_DATA_SWIZZLE_MASK)

/* This field specifies how to swap the bytes fetched by the bus master interface */
#define __LCDIF_CTRL_INPUT_DATA_SWIZZLE_SHIFT 14
#define __LCDIF_CTRL_INPUT_DATA_SWIZZLE_MASK (0x02 << __LCDIF_CTRL_INPUT_DATA_SWIZZLE_SHIFT)
#define __LCDIF_CTRL_INPUT_DATA_SWIZZLE(x)   (((x) << __LCDIF_CTRL_INPUT_DATA_SWIZZLE_SHIFT) & \
                                                         __LCDIF_CTRL_INPUT_DATA_SWIZZLE_MASK)

/* Set this bit to 1 to make the hardware go into the DOTCLK mode */
#define __LCDIF_CTRL_DOTCLK_MODE_SHIFT        17
#define __LCDIF_CTRL_DOTCLK_MODE_MASK        (0x01 << __LCDIF_CTRL_DOTCLK_MODE_SHIFT)

/*
 * When this bit is 0, it means that eLCDIF will stop the block operation and
 * turn off the RUN bit after the amount of data indicated by the LCDIF_TRANSFER_
 * COUNT register has been transferred out.
 */
#define __LCDIF_CTRL_BYPASS_COUNT_SHIFT      19
#define __LCDIF_CTRL_BYPASS_COUNT_MASK      (0x01 << __LCDIF_CTRL_BYPASS_COUNT_SHIFT)

/* The data to be transmitted is shifted left or right by this number of bits */
#define __LCDIF_CTRL_SHIFT_NUM_BITS_SHIFT    21
#define __LCDIF_CTRL_SHIFT_NUM_BITS_MASK    (0x1F << __LCDIF_CTRL_SHIFT_NUM_BITS_SHIFT)
#define __LCDIF_CTRL_SHIFT_NUM_BITS(x)      (((x) << __LCDIF_CTRL_SHIFT_NUM_BITS_SHIFT) & \
                                                     __LCDIF_CTRL_SHIFT_NUM_BITS_MASK)

/* Use this bit to determine the direction of shift of transmit data */
#define __LCDIF_CTRL_DATA_SHIFT_DIR_SHIFT    26
#define __LCDIF_CTRL_DATA_SHIFT_DIR_MASK    (0x01 << __LCDIF_CTRL_DATA_SHIFT_DIR_SHIFT)

/* When set to one it gates off the clocks to the block */
#define __LCDIF_CTRL_CLKGATE_SHIFT           30
#define __LCDIF_CTRL_CLKGATE_MASK           (0x01 << __LCDIF_CTRL_CLKGATE_SHIFT)

/* When set to one, it forces a block level reset */
#define __LCDIF_CTRL_SFTRST_SHIFT            31
#define __LCDIF_CTRL_SFTRST_MASK            (0x01 << __LCDIF_CTRL_SFTRST_SHIFT)


/*******************************************************************************
    eLCDIF General Control1 Register
*******************************************************************************/

/* This bitfield is used to show which data bytes in a 32-bit word are valid */
#define __LCDIF_CTRL1_BYTE_PACKING_FORMAT_SHIFT    16
#define __LCDIF_CTRL1_BYTE_PACKING_FORMAT_MASK    (0x0F << __LCDIF_CTRL1_BYTE_PACKING_FORMAT_SHIFT)
#define __LCDIF_CTRL1_BYTE_PACKING_FORMAT(x)      (((x) << __LCDIF_CTRL1_BYTE_PACKING_FORMAT_SHIFT) & \
                                                           __LCDIF_CTRL1_BYTE_PACKING_FORMAT_MASK)

/*******************************************************************************
    eLCDIF Horizontal and Vertical Valid Data Count Register
*******************************************************************************/

/* Total valid data (pixels) in each horizontal line */
#define __LCDIF_TRANSFER_COUNT_H_SHIFT    0
#define __LCDIF_TRANSFER_COUNT_H_MASK    (0xFFFF << __LCDIF_TRANSFER_COUNT_H_SHIFT)
#define __LCDIF_TRANSFER_COUNT_H(x)      (((x)   << __LCDIF_TRANSFER_COUNT_H_SHIFT) & \
                                                    __LCDIF_TRANSFER_COUNT_H_MASK)

/*  Number of horizontal lines per frame which contain valid data*/
#define __LCDIF_TRANSFER_COUNT_V_SHIFT    16
#define __LCDIF_TRANSFER_COUNT_V_MASK    (0xFFFF << __LCDIF_TRANSFER_COUNT_V_SHIFT)
#define __LCDIF_TRANSFER_COUNT_V(x)      (((x)   << __LCDIF_TRANSFER_COUNT_V_SHIFT) & \
                                                    __LCDIF_TRANSFER_COUNT_V_MASK)

/*******************************************************************************
    eLCDIF VSYNC Mode and Dotclk Mode Control Register0
*******************************************************************************/

/* Number of units for which VSYNC signal is active */
#define __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_SHIFT    0
#define __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_MASK    (0x3FFFF << __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_SHIFT)
#define __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH(x)      (((x)    << __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_SHIFT) & \
                                                              __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_MASK)
/*
 * When this bit is 0, the first field (VSYNC period) will end in half a horizontal
 * line and the second field will begin with half a horizontal line. When this
 * bit is 1, all fields will end with half a horizontal line, and none will begin
 * with half a horizontal line
 */
#define __LCDIF_VDCTRL0_HALF_LINE_MODE_SHIFT       18
#define __LCDIF_VDCTRL0_HALF_LINE_MODE_MASK       (0X01 << __LCDIF_VDCTRL0_HALF_LINE_MODE_SHIFT)

/*
 * Setting this bit to 1 will make the total VSYNC period equal to the VSYNC_PERIOD
 * field plus half the HORIZONTAL_PERIOD field
 */
#define __LCDIF_VDCTRL0_HALF_LINE_SHIFT            19
#define __LCDIF_VDCTRL0_HALF_LINE_MASK            (0X01 << __LCDIF_VDCTRL0_HALF_LINE_SHIFT)

/*
 * Default 0 for counting VSYNC_PULSE_WIDTH in terms of DISPLAY CLOCK (pix_clk)
 * cycles. Set it to 1 to count in terms of complete horizontal lines.
 */
#define __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT_SHIFT   20
#define __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT_MASK   (0X01 << __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT_SHIFT)

/* Default 0 for counting VSYNC_PERIOD in terms of DISPLAY CLOCK (pix_clk) cycles */
#define __LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT_SHIFT    21
#define __LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT_MASK    (0X01 << __LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT_SHIFT)

/* Default 0 active low during valid data transfer on each horizontal line */
#define __LCDIF_VDCTRL0_ENABLE_POL_SHIFT           24
#define __LCDIF_VDCTRL0_ENABLE_POL_MASK           (0X01 << __LCDIF_VDCTRL0_ENABLE_POL_SHIFT)

/* Default is data launched at negative edge of DOTCLK and captured at positive edge */
#define __LCDIF_VDCTRL0_DOTCLK_POL_SHIFT           25
#define __LCDIF_VDCTRL0_DOTCLK_POL_MASK           (0X01 << __LCDIF_VDCTRL0_DOTCLK_POL_SHIFT)

/*
 * Default 0 active low during HSYNC_PULSE_WIDTH time and will be high during
 * the rest of the HSYNC period. Set it to 1 to invert the polarity
 */
#define __LCDIF_VDCTRL0_HSYNC_POL_SHIFT            26
#define __LCDIF_VDCTRL0_HSYNC_POL_MASK            (0X01 << __LCDIF_VDCTRL0_HSYNC_POL_SHIFT)

/*
 * Default 0 active low during VSYNC_PULSE_WIDTH time and will be high during
 * the rest of the VSYNC period. Set it to 1 to invert the polarity
 */
#define __LCDIF_VDCTRL0_VSYNC_POL_SHIFT            27
#define __LCDIF_VDCTRL0_VSYNC_POL_MASK            (0X01 << __LCDIF_VDCTRL0_VSYNC_POL_SHIFT)

/* Setting this bit to 1 will make the hardware generate the ENABLE signal in the DOTCLK mode, */
#define __LCDIF_VDCTRL0_ENABLE_PRESENT_SHIFT       28
#define __LCDIF_VDCTRL0_ENABLE_PRESENT_MASK       (0X01 << __LCDIF_VDCTRL0_ENABLE_PRESENT_SHIFT)

/*******************************************************************************
    LCDIF VSYNC Mode and Dotclk Mode Control Register2
*******************************************************************************/

/*
 * Total number of DISPLAY CLOCK (pix_clk) cycles between two positive or two
 * negative edges of the HSYNC signal
 */
#define __LCDIF_VDCTRL2_HSYNC_PERIOD_SHIFT    0
#define __LCDIF_VDCTRL2_HSYNC_PERIOD_MASK    (0x3FFFF << __LCDIF_VDCTRL2_HSYNC_PERIOD_SHIFT)
#define __LCDIF_VDCTRL2_HSYNC_PERIOD(x)      (((x)    << __LCDIF_VDCTRL2_HSYNC_PERIOD_SHIFT) & \
                                                         __LCDIF_VDCTRL2_HSYNC_PERIOD_MASK)

/* Number of DISPLAY CLOCK (pix_clk) cycles for which HSYNC signal is active */
#define __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_SHIFT  18
#define __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_MASK  (0x3FFF << __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_SHIFT)
#define __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH(x)    (((x)   << __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_SHIFT) & \
                                                            __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH_MASK)

/*******************************************************************************
    eLCDIF VSYNC Mode and Dotclk Mode Control Register3
*******************************************************************************/

#define __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_SHIFT    0
#define __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_MASK    (0xFFFF << __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_SHIFT)
#define __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT(x)      (((x)   << __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_SHIFT) & \
                                                             __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT_MASK)

#define __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_SHIFT  16
#define __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_MASK  (0xFFF << __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_SHIFT)
#define __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT(x)    (((x)  << __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_SHIFT) & \
                                                            __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT_MASK)
/*******************************************************************************
    eLCDIF VSYNC Mode and Dotclk Mode Control Register4
*******************************************************************************/

/*
 * Total number of DISPLAY CLOCK (pix_clk) cycles on each horizontal line that
 * carry valid data in DOTCLK mode.
 */
#define __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_SHIFT    0
#define __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_MASK    (0x3FFFF << __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_SHIFT)
#define __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT(x)      (((x)    << __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_SHIFT) & \
                                                                    __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT_MASK)
/*
 * Set this field to 1 if the LCD controller requires that the VSYNC or
 * VSYNC/HSYNC/DOTCLK control signals should be active at least one frame before
 * the data transfers actually start and remain active at least one frame after
 * the data transfers end.
 */
#define __LCDIF_VDCTRL4_SYNC_SIGNALS_ON_SHIFT    18
#define __LCDIF_VDCTRL4_SYNC_SIGNALS_ON_MASK    (0x01 << __LCDIF_VDCTRL4_SYNC_SIGNALS_ON_SHIFT)

/*
 * This bitfield selects the amount of time by which the DOTCLK signal should be
 * delayed before coming out of the LCD_DOTCK pin. 0 = 2ns; 1=4ns;2=6ns;3=8ns.
 * Remaining values are reserved
 */
#define __LCDIF_VDCTRL4_DOTCLK_DLY_SEL_SHIFT    29
#define __LCDIF_VDCTRL4_DOTCLK_DLY_SEL_MASK    (0x07 << __LCDIF_VDCTRL4_DOTCLK_DLY_SEL_SHIFT)
#define __LCDIF_VDCTRL4_DOTCLK_DLY_SEL(x)      (((x) << __LCDIF_VDCTRL4_DOTCLK_DLY_SEL_SHIFT) & \
                                                        __LCDIF_VDCTRL4_DOTCLK_DLY_SEL_MASK)

#endif /* __AWBL_IMX1050_LCDIF_REGS_H */

/* end of file */
