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
*******************************************************************************/

/**
 * \file
 * \brief Freescale iMX RT1050 LCD控制器驱动
 *
 * \internal
 * \par modification history:
 * - 1.01  19-04-09  cat, add triple-buffer drivers.
 * - 1.00  17-11-21  mex, firstn implementation.
 * \endinternal
 */

/*******************************************************************************
    includes
*******************************************************************************/
#include "aw_common.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "aw_delay.h"

#include "aw_assert.h"
#include "aw_hwtimer.h"
#include "aw_int.h"
#include "aw_intensity.h"
#include "aw_fb.h"

#include "driver/display/awbl_imx1050_lcdif_regs.h"
#include "driver/display/awbl_imx1050_lcdif.h"
#include "driver/display/awbl_imx1050_fb.h"

/*******************************************************************************
    local defines
*******************************************************************************/

/* write register */
#define __LCDIF_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __LCDIF_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* bit is set */
#define __LCDIF_REG_BIT_ISSET(reg_addr, bit) \
        AW_REG_BIT_ISSET32(reg_addr, bit)

/* set bit */
#define __LCDIF_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __LCDIF_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __LCDIF_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* set register bits */
#define __LCDIF_REG_BITS_SET(reg_addr, start, len, value) \
        AW_REG_BITS_SET32(reg_addr, start, len, value)

/* get lcdif hw register */
#define __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif) \
        imx1050_lcdif_regs_t * p_hw_lcdif = \
       (imx1050_lcdif_regs_t *)((p_lcdif)->p_devinfo->regbase)

#define __IMX1050_LCDIF_DEV_DECL(p_this, p_dev) \
    awbl_imx1050_lcdif_t *p_this = \
           (awbl_imx1050_lcdif_t *)(&(((struct awbl_imx1050_fb *)(p_dev))->lcdif))

/* 每一个像素点数据字节数 */
#define __LCDIF_BPP_8_BIT        8
#define __LCDIF_BPP_16_BIT       16
#define __LCDIF_BPP_24_BIT       24
#define __LCDIF_BPP_18_BIT       18
#define __LCDIF_BPP_32_BIT       32

/*******************************************************************************
    implementation
*******************************************************************************/

aw_local void __imx1050_lcdif_rset(imx1050_lcdif_regs_t * p_hw_lcdif)
{

    /* Disable the clock gate */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR, __LCDIF_CTRL_CLKGATE_MASK);

    /* Confirm the clock gate is disabled. */
    while (__LCDIF_REG_READ(&p_hw_lcdif->CTRL) &__LCDIF_CTRL_CLKGATE_MASK) {

    }
    /* Reset the block */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_SET, __LCDIF_CTRL_SFTRST_MASK);

    /* Confirm the reset bit is set */
    while (!(__LCDIF_REG_READ(&p_hw_lcdif->CTRL) & __LCDIF_CTRL_SFTRST_MASK)) {

    }

    /* Delay for the reset */
    aw_udelay(500);

    /* Bring the module out of reset */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR, __LCDIF_CTRL_SFTRST_MASK);

    /* Disable the clock gate */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR, __LCDIF_CTRL_CLKGATE_MASK);
}


aw_err_t awbl_imx1050_lcdif_setup_dotclk_panel (awbl_imx1050_lcdif_t *p_lcdif,
                                                aw_fb_var_info_t     *p_vinfo)
{
    uint32_t val;
    uint16_t v_period;
    uint16_t h_period;
    uint16_t v_wait_cnt;
    uint16_t h_wait_cnt;

    h_period = p_vinfo->hsync.h_pulse_width
              + p_vinfo->hsync.h_bporch
              + p_vinfo->hsync.h_active
              + p_vinfo->hsync.h_fporch;
    v_period = p_vinfo->vsync.v_pulse_width
              + p_vinfo->vsync.v_bporch
              + p_vinfo->vsync.v_active
              + p_vinfo->vsync.v_fporch;
    h_wait_cnt = p_vinfo->hsync.h_pulse_width + p_vinfo->hsync.h_bporch;
    v_wait_cnt = p_vinfo->vsync.v_pulse_width + p_vinfo->vsync.v_bporch;

    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    /* 设置屏幕的长宽像数 */
    val = __LCDIF_TRANSFER_COUNT_H(p_vinfo->hsync.h_active) |
          __LCDIF_TRANSFER_COUNT_V(p_vinfo->vsync.v_active);

    __LCDIF_REG_WRITE(&p_hw_lcdif->TRANSFER_COUNT, val);

    val = __LCDIF_VDCTRL0_ENABLE_PRESENT_MASK         |   /* Data enable signal. */
          __LCDIF_VDCTRL0_VSYNC_PERIOD_UNIT_MASK      |   /* VSYNC period in the unit of display clock. */
          __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH_UNIT_MASK |   /* VSYNC pulse width in the unit of display clock. */
          __LCDIF_VDCTRL0_ENABLE_POL_MASK |
          __LCDIF_VDCTRL0_DOTCLK_POL_MASK |
          __LCDIF_VDCTRL0_VSYNC_PULSE_WIDTH(p_vinfo->vsync.v_pulse_width);

    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL0, val);

    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL1, v_period);

    val = __LCDIF_VDCTRL2_HSYNC_PERIOD(h_period) |
          __LCDIF_VDCTRL2_HSYNC_PULSE_WIDTH(p_vinfo->hsync.h_pulse_width);

    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL2, val);

    val = __LCDIF_VDCTRL3_VERTICAL_WAIT_CNT(v_wait_cnt) |
          __LCDIF_VDCTRL3_HORIZONTAL_WAIT_CNT(h_wait_cnt);

    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL3, val);

    val = __LCDIF_VDCTRL4_SYNC_SIGNALS_ON_MASK |
          __LCDIF_VDCTRL4_DOTCLK_H_VALID_DATA_CNT(p_vinfo->hsync.h_active);

     __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL4, val);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_release_dotclk_panel(awbl_imx1050_lcdif_t *p_lcdif)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    __imx1050_lcdif_rset(p_hw_lcdif);

    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL0, 0x00);
    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL1, 0x00);
    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL2, 0x00);
    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL3, 0x00);
    __LCDIF_REG_WRITE(&p_hw_lcdif->VDCTRL4, 0x00);

    return AW_OK;
}


/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_init(awbl_imx1050_lcdif_t *p_lcdif,
                                 uint8_t               bus_bpp,
                                 uint8_t               word_lbpp)
{
    uint32_t ctrl     = 0;
    uint32_t bus_val  = 0;
    uint32_t word_val = 0;

    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    /* 复位lcdif控制器 */
    __imx1050_lcdif_rset(p_hw_lcdif);

    /* 屏幕硬件总线宽度 */
    if (__LCDIF_BPP_16_BIT == bus_bpp) {
        bus_val = 0x00;
    } else if (__LCDIF_BPP_8_BIT == bus_bpp) {
        bus_val = 0x01;
    } else if (__LCDIF_BPP_18_BIT == bus_bpp) {
        bus_val = 0x02;
    } else if (__LCDIF_BPP_24_BIT == bus_bpp) {
        bus_val = 0x03;
    }

    /* 一个像素的长度，位为单位*/
    if (__LCDIF_BPP_16_BIT == word_lbpp) {
        word_val = 0x00;
    } else if (__LCDIF_BPP_8_BIT == word_lbpp) {
        word_val = 0x01;
    } else if (__LCDIF_BPP_18_BIT == word_lbpp) {
        word_val = 0x02;
    } else if (__LCDIF_BPP_24_BIT == word_lbpp) {
        word_val = 0x03;
    }

    ctrl = __LCDIF_CTRL_WORD_LENGTH(word_val)   |
           __LCDIF_CTRL_DATABUS_WIDTH(bus_val)  |
           __LCDIF_CTRL_DOTCLK_MODE_MASK        |   /* RGB mode. */
           __LCDIF_CTRL_BYPASS_COUNT_MASK       |   /* Keep RUN bit set. */
           __LCDIF_CTRL_MASTER_MASK;

    /* 18位时，每8个字节高两位数据位无效 */
    if (__LCDIF_BPP_18_BIT == word_lbpp) {
        ctrl |= __LCDIF_CTRL_DATA_FORMAT_24BIT_MASK;
    }
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL, ctrl);

    if (__LCDIF_BPP_18_BIT == word_lbpp) {
        __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL1, __LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x07));
    } else {
        __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL1, __LCDIF_CTRL1_BYTE_PACKING_FORMAT(0x0F));
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_run(awbl_imx1050_lcdif_t *p_lcdif)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    /* 开启中断  */
    __LCDIF_REG_BIT_SET(&p_hw_lcdif->CTRL1, 13);

    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_SET,
                       __LCDIF_CTRL_RUN_MASK | __LCDIF_CTRL_DOTCLK_MODE_MASK);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_stop(awbl_imx1050_lcdif_t *p_lcdif)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    /* 关闭中断  */
    __LCDIF_REG_BIT_CLR(&p_hw_lcdif->CTRL1, 13);

    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR,
                       __LCDIF_CTRL_RUN_MASK | __LCDIF_CTRL_DOTCLK_MODE_MASK);

    /* Wait for data transfer finished. */
//    while (__LCDIF_REG_READ(p_hw_lcdif->CTRL) & __LCDIF_CTRL_DOTCLK_MODE_MASK) {
//
//    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_dma_init(awbl_imx1050_lcdif_t *p_lcdif, uint32_t phys, size_t memsize)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_SET, __LCDIF_CTRL_MASTER_MASK);

    __LCDIF_REG_WRITE(&p_hw_lcdif->CUR_BUF, phys);
    __LCDIF_REG_WRITE(&p_hw_lcdif->NEXT_BUF, phys);

    return AW_OK;
}

/******************************************************************************/
aw_err_t awbl_imx1050_lcdif_dma_release(awbl_imx1050_lcdif_t *p_lcdif)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR, __LCDIF_CTRL_MASTER_MASK);

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief 使用双缓存或单缓存时显存地址设置函数
 */
aw_err_t awbl_imx1050_lcdif_pan_common_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);
    __LCDIF_REG_WRITE(&p_hw_lcdif->NEXT_BUF, addr);

    /* 先复位信号量，可能中断先于此函数的到来，导致数据不同步 */
    AW_SEMB_RESET(p_lcdif->lcd_sem_sync, 0);
    AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, AW_SEM_WAIT_FOREVER);

    return AW_OK;
}

/**
 * \brief 使用三缓存时的显存地址设置函数
 */
aw_err_t awbl_imx1050_lcdif_pan_triple_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    /* 等待NEXT缓冲区可写 */
    AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, AW_SEM_WAIT_FOREVER);

    __LCDIF_REG_WRITE(&p_hw_lcdif->NEXT_BUF, addr);

    return AW_OK;
}

/**
 * \brief 尝试设置显存地址函数
 */
aw_err_t awbl_imx1050_lcdif_pan_try_vram_addr_set(awbl_imx1050_lcdif_t *p_lcdif, uint32_t addr) {
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);
    aw_err_t    ret;

    ret = AW_SEMB_TAKE(p_lcdif->lcd_sem_sync, 0);

    if (AW_OK != ret) {
        return -AW_EAGAIN;
    }

    __LCDIF_REG_WRITE(&p_hw_lcdif->NEXT_BUF, addr);

    return ret;
}

aw_err_t __imx_lcdif_common_buf_isr(void *p_dev)
{
    __IMX1050_LCDIF_DEV_DECL(p_this, p_dev);
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_this);

    /* 清除LCDIF_CTRL1_CUR_FRAME_DONE_IRQ 中断标志位  */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL1_CLR, 1 << 9);

    /* 释放信号量  */
    AW_SEMB_GIVE(p_this->lcd_sem_sync);

    return AW_OK;
}

aw_err_t __imx_lcdif_triple_buf_isr(void *p_dev)
{
    volatile uint32_t reg = 0;
    static uint32_t last_cur_buffer = 0;
    __IMX1050_LCDIF_DEV_DECL(p_this, p_dev);
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_this);

    /* 当前cur-buffer的值 */
    reg = __LCDIF_REG_READ(&p_hw_lcdif->CUR_BUF);

    /* 清除LCDIF_CTRL1_CUR_FRAME_DONE_IRQ 中断标志位  */
    __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL1_CLR, 1 << 9);

    /* 如果进行了缓冲区交换就释放信号量 */
    if (last_cur_buffer != reg) {

        /* 释放同步信号量  */
        AW_SEMB_GIVE(p_this->lcd_sem_sync);
        /* 保存当前cur-buffer的地址 */
        last_cur_buffer = reg;
    }

    return AW_OK;
}

aw_err_t awbl_imx1050_lcdif_pan_blank(awbl_imx1050_lcdif_t *p_lcdif, int blank)
{
    __IMX1050_LCDIF_HW_GET(p_hw_lcdif, p_lcdif);

    int ret = 0, count = 0;

    switch (blank) {

    case AW_FB_BLANK_NORMAL:
    case AW_FB_BLANK_VSYNC_SUSPEND:
    case AW_FB_BLANK_HSYNC_SUSPEND:
    case AW_FB_BLANK_POWERDOWN:

        __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_CLR, __LCDIF_CTRL_BYPASS_COUNT_MASK);

        for (count = 10000; count; count--) {

            if (__LCDIF_REG_BIT_ISSET(&p_hw_lcdif->STAT, 26)) {
                break;
            }
            aw_udelay(1);
        }
        break;

    case AW_FB_BLANK_UNBLANK:

        __LCDIF_REG_WRITE(&p_hw_lcdif->CTRL_SET, __LCDIF_CTRL_BYPASS_COUNT_MASK);
        break;

    default:
        ret = -AW_EINVAL;
        break;
    }

    return ret;
}

/* end of file */
