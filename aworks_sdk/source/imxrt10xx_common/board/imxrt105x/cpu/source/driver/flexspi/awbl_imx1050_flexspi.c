/*******************************************************************************
*                                 AWorks
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
 * \brief iMX RT1050 FlexSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 17-10-26  mex, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_spinlock.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_list.h"
#include "aw_assert.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_spibus.h"
#include "aw_clk.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_cache.h"
#include "string.h"
#include "awbl_wdt.h"
#include "driver/flexspi/awbl_imx1050_flexspi.h"
#include "driver/flexspi/awbl_imx1050_flexspi_mtd.h"

/*******************************************************************************
  SPI State and Event define
*******************************************************************************/
#if defined (__CC_ARM)
extern unsigned int Image$$ER_FLEXSPI_DRV_START$$Base;   /*  __flexspi_drv_start__  */
#define __FLEXSPI_DRV_START        (&Image$$ER_FLEXSPI_DRV_START$$Base)
#elif defined (__GNUC__)
extern int __flexspi_drv_start__;
#define __FLEXSPI_DRV_START        (&__flexspi_drv_start__)
#endif

#define __FLEXSPI_IR_SIZE            256
#define __FLEXSPI_TXFIFO_SIZE        128
#define __FLEXSPI_RXFIFO_SIZE        128
#define __FLEXSPI_LUT_NUM            64

/** \brief LUT KEY */
#define __FLEXSPI_LUT_KEY_VAL        0x5AF05AF0UL

/** \brief AHB buffer的个数 */
#define __FLEXSPI_AHB_BUFFER_COUNT   4

#define __FLEXSPI_SEQID_READ         0   /**< \brief 读数据 */
#define __FLEXSPI_SEQID_RDSR         1   /**< \brief 读状态寄存器 */

#define __FLEXSPI_SEQID_WREN         3   /**< \brief 写使能 */

#define __FLEXSPI_SEQID_SE           5   /**< \brief 扇区擦除 */
#define __FLEXSPI_SEQID_CHIP_ERASE   6   /**< \brief 整片擦除 */
#define __FLEXSPI_SEQID_RDID         7   /**< \brief 读ID */
#define __FLEXSPI_SEQID_WRSR         8   /**< \brief 写状态寄存器 */
#define __FLEXSPI_SEQID_PP           9   /**< \brief 编程 */
#define __FLEXSPI_SEQID_EN4B         10  /**< \brief 进入4Byte模式 */
#define __FLEXSPI_SEQID_IRRD         11  /**< \brief 读保留扇区数据 */
#define __FLEXSPI_SEQID_IRP          12  /**< \brief 写保留扇区数据 */
#define __FLEXSPI_SEQID_RDFR         13  /**< \brief 读function寄存器 */
#define __FLEXSPI_SEQID_WRFR         14  /**< \brief 写function寄存器 */
#define __FLEXSPI_SEQID_IRE          15  /**< \brief 擦除保留扇区数据 */


/** \brief CMD definition of FLEXSPI, use to form LUT instructio*/
#define __FLEXSPI_CMD_STOP           0x00U    /**< \brief Stop execution, deassert CS. */
#define __FLEXSPI_CMD_SDR            0x01U    /**< \brief Transmit Command code to Flash, using SDR mode. */
#define __FLEXSPI_CMD_RADDR_SDR      0x02U    /**< \brief Transmit Row Address to Flash, using SDR mode. */
#define __FLEXSPI_CMD_CADDR_SDR      0x03U    /**< \brief Transmit Column Address to Flash, using SDR mode. */
#define __FLEXSPI_CMD_MODE1_SDR      0x04U    /**< \brief Transmit 1-bit Mode bits to Flash, using SDR mode. */
#define __FLEXSPI_CMD_MODE2_SDR      0x05U    /**< \brief Transmit 2-bit Mode bits to Flash, using SDR mode. */
#define __FLEXSPI_CMD_MODE4_SDR      0x06U    /**< \brief Transmit 4-bit Mode bits to Flash, using SDR mode. */
#define __FLEXSPI_CMD_MODE8_SDR      0x07U    /**< \brief Transmit 8-bit Mode bits to Flash, using SDR mode. */
#define __FLEXSPI_CMD_WRITE_SDR      0x08U    /**< \brief Transmit Programming Data to Flash, using SDR mode. */
#define __FLEXSPI_CMD_READ_SDR       0x09U    /**< \brief Receive Read Data from Flash, using SDR mode. */
#define __FLEXSPI_CMD_LEARN_SDR      0x0AU    /**< \brief Receive Read Data or Preamble bit from Flash, SDR mode. */
#define __FLEXSPI_CMD_DATSZ_SDR      0x0BU    /**< \brief Transmit Read/Program Data size (byte) to Flash, SDR mode. */
#define __FLEXSPI_CMD_DUMMY_SDR      0x0CU    /**< \brief Leave data lines undriven by FlexSPI controller.*/
#define __FLEXSPI_CMD_DUMMY_RWDS_SDR 0x0DU    /**< \brief Leave data lines undriven by FlexSPI controller,
                                                        dummy cycles decided by RWDS. */
#define __FLEXSPI_CMD_DDR            0x21U    /**< \brief Transmit Command code to Flash, using DDR mode. */
#define __FLEXSPI_CMD_RADDR_DDR      0x22U    /**< \brief Transmit Row Address to Flash, using DDR mode. */
#define __FLEXSPI_CMD_CADDR_DDR      0x23U    /**< \brief Transmit Column Address to Flash, using DDR mode. */
#define __FLEXSPI_CMD_MODE1_DDR      0x24U    /**< \brief Transmit 1-bit Mode bits to Flash, using DDR mode. */
#define __FLEXSPI_CMD_MODE2_DDR      0x25U    /**< \brief Transmit 2-bit Mode bits to Flash, using DDR mode. */
#define __FLEXSPI_CMD_MODE4_DDR      0x26U    /**< \brief Transmit 4-bit Mode bits to Flash, using DDR mode. */
#define __FLEXSPI_CMD_MODE8_DDR      0x27U    /**< \brief Transmit 8-bit Mode bits to Flash, using DDR mode. */
#define __FLEXSPI_CMD_WRITE_DDR      0x28U    /**< \brief Transmit Programming Data to Flash, using DDR mode. */
#define __FLEXSPI_CMD_READ_DDR       0x29U    /**< \brief Receive Read Data from Flash, using DDR mode. */
#define __FLEXSPI_CMD_LEARN_DDR      0x2AU    /**< \brief Receive Read Data or Preamble bit from Flash, DDR mode. */
#define __FLEXSPI_CMD_DATSZ_DDR      0x2BU    /**< \brief Transmit Read/Program Data size (byte) to Flash, DDR mode. */
#define __FLEXSPI_CMD_DUMMY_DDR      0x2CU    /**< \brief Leave data lines undriven by FlexSPI controller.*/
#define __FLEXSPI_CMD_DUMMY_RWDS_DDR 0x2DU    /**< \brief Leave data lines undriven by FlexSPI controller,
                                                        dummy cycles decided by RWDS. */
#define __FLEXSPI_CMD_JUMP_ON_CS     0x1FU    /**< \brief Stop execution, deassert CS and save operand[7:0] as the
                                                        instruction start pointer for next sequence */

#define __FLEXSPI_LUT_PAD1           0x0      /**< \brief Single mode：SIO0发送数据，SIO1接收数据 */
#define __FLEXSPI_LUT_PAD2           0x1      /**< \brief Dual   mode：SIO0~SIO1发送数据和接收数据 */
#define __FLEXSPI_LUT_PAD4           0x2      /**< \brief Quad   mode：SIO0~SIO3发送数据和接收数据 */
#define __FLEXSPI_LUT_PAD8           0x3      /**< \brief Single mode：SIO0~SIO7发送数据和接收数据 */

#define __FLEXSPI_ADDR_24BIT         0x18     /**< \brief 地址为24位 */
#define __FLEXSPI_ADDR_32BIT         0x20     /**< \brief 地址为32位 */

/** \brief Controller State */
#define __FLEXSPI_ST_IDLE            0        /* idle state */
#define __FLEXSPI_ST_READ            1        /* read state */
#define __FLEXSPI_ST_WRITE           2        /* write start */

/*******************************************************************************
  macro operate
*******************************************************************************/

/* write register */
#define __FLEXSPI_REG_WRITE(reg_addr, data) \
        AW_REG_WRITE32(reg_addr, (data))

/* read register */
#define __FLEXSPI_REG_READ(reg_addr) \
        AW_REG_READ32(reg_addr)

/* bit is set */
#define __FLEXSPI_REG_BIT_ISSET(reg_addr, bit) \
        AW_REG_BIT_ISSET32(reg_addr, bit)

/* set bit */
#define __FLEXSPI_REG_BIT_SET(reg_addr, bit) \
        AW_REG_BIT_SET32(reg_addr, bit)

/* clear bit */
#define __FLEXSPI_REG_BIT_CLR(reg_addr, bit) \
        AW_REG_BIT_CLR32(reg_addr, bit)

/* get register bits */
#define __FLEXSPI_REG_BITS_GET(reg_addr, start, len) \
        AW_REG_BITS_GET32(reg_addr, start, len)

/* declare FLEXSPI device instance */
#define __FLEXSPI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1050_flexspi_dev *p_this = \
        (struct awbl_imx1050_flexspi_dev *)(p_dev)

/* declare FLEXSPI device infomation */
#define __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1050_flexspi_devinfo *p_devinfo = \
        (struct awbl_imx1050_flexspi_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get imx1050 spi hardware infomation */
#define __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo)      \
        imx1050_flexspi_regs_t *p_hw_flexspi =          \
        ((imx1050_flexspi_regs_t *)((p_devinfo)->regbase))

#define __IS_XIP_MODE  \
        (((uint32_t)&__flexspi_inst_init2 > FLEXSPI_AMBA_BASE) &&  \
        ((uint32_t)&__flexspi_inst_init2 < FLEXSPI_AMBA_BASE + 0x4000000))

#define __FLEXSPI_ASSERT(condition, run) \
        if (condition) { \
            run;\
        }

/* FlexSPI enable */
#define flexspi_enable(p_hw_flexspi)  \
        __FLEXSPI_REG_BIT_CLR(&p_hw_flexspi->MCR0, __FLEXSPI_MCR0_MDIS_SHIFT);

/* FlexSPI disable */
#define flexspi_disable(p_hw_flexspi) \
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->MCR0, __FLEXSPI_MCR0_MDIS_SHIFT);

/* Formula to form FLEXSPI instructions in LUT table */
#define FLEXSPI_LUT0_SEQ(ins, pad, opr) \
       (__FLEXSPI_LUT_OPERAND0(opr) |   \
        __FLEXSPI_LUT_NUM_PADS0(__FLEXSPI_LUT_##pad) |\
        __FLEXSPI_LUT_OPCODE0(__FLEXSPI_##ins))

#define FLEXSPI_LUT1_SEQ(ins, pad, opr) \
       (__FLEXSPI_LUT_OPERAND1(opr) |   \
        __FLEXSPI_LUT_NUM_PADS1(__FLEXSPI_LUT_##pad) |\
        __FLEXSPI_LUT_OPCODE1(__FLEXSPI_##ins))

/*******************************************************************************
  forward declarations
*******************************************************************************/

aw_local void __flexspi_inst_init1 (awbl_dev_t *p_dev);
aw_local void __flexspi_inst_init2 (awbl_dev_t *p_dev);
aw_local void __flexspi_inst_connect (awbl_dev_t *p_dev);

/** \brief flexspi controller hardware initialize */
aw_local void __flexspi_hard_init (struct awbl_imx1050_flexspi_dev  *p_this);

aw_local aw_err_t __flexsp_check_and_clear_error(imx1050_flexspi_regs_t *p_hw_flexspi,
                                                 uint32_t                status);

/*******************************************************************************
  locals
*******************************************************************************/

/**
 * \brief FlexSPI interrupt service routine
 *
 * \param[in]   p_arg
 */
aw_local aw_err_t __flexspi_isr (void *p_arg)
{
    __FLEXSPI_DEV_DECL(p_this, p_arg);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_arg);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    aw_err_t ret    = AW_OK;
    uint32_t status = 0;
    uint8_t rx_watermark = 0;
    uint8_t tx_watermark = 0;
    int i = 0;

    status =  __FLEXSPI_REG_READ(&p_hw_flexspi->INTR);
    ret = __flexsp_check_and_clear_error(p_hw_flexspi, status);
    if (ret != AW_OK) {
        p_this->state =  __FLEXSPI_ST_IDLE;
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTEN, 0X00);
        return AW_ERROR;
    }

    /* 如果是接收中断 */
    if (AW_BIT_ISSET(status, __FLEXSPI_INTR_IPRXWA_SHIFT) &&
                    (p_this->state == __FLEXSPI_ST_READ)) {

        rx_watermark = __FLEXSPI_REG_BITS_GET(&p_hw_flexspi->IPRXFCR,
                       __FLEXSPI_IPRXFCR_RXWMRK_SHIFT,
                       4) + 1;

        /* Read watermark level data from rx fifo . */
        if (p_this->data_size >= 8 * rx_watermark) {
            /* Read watermark level data from rx fifo . */
            for (i = 0; i < 2 * rx_watermark; i++) {
                *p_this->p_data++  = __FLEXSPI_REG_READ(&p_hw_flexspi->RFDR[i]);
            }
            p_this->data_size -=  8 * rx_watermark;

        } else {
            for (i = 0; i < (p_this->data_size / 4 + 1); i++) {
                *p_this->p_data++ = __FLEXSPI_REG_READ(&p_hw_flexspi->RFDR[i]);
            }
            p_this->data_size = 0;
        }

        /* 将已经读取到数据弹出FIFO */
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPRXWA_SHIFT);
    }

    /* 如果是IP命令序列完成中断 */
    if (AW_BIT_ISSET(status, __FLEXSPI_INTR_IPCMDDONE_SHIFT)) {
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPCMDDONE_SHIFT);

        /* 释放信号量 */
        AW_SEMB_GIVE(p_this->flexspi_trans_sync);

        /* 此时禁能所有中断 */
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTEN, 0x00);
    }

    /* 如果是数据发送中断 */
    if (AW_BIT_ISSET(status, __FLEXSPI_INTR_IPTXWE_SHIFT) &&
                    (p_this->state == __FLEXSPI_ST_WRITE)) {

        tx_watermark = __FLEXSPI_REG_BITS_GET(&p_hw_flexspi->IPTXFCR,
                   __FLEXSPI_IPTXFCR_TXWMRK_SHIFT,
                   4) + 1;

        if (p_this->data_size >= 8 * tx_watermark) {
            for (i = 0; i < 2 * tx_watermark; i++) {
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i],  *p_this->p_data++);
            }

            p_this->data_size = p_this->data_size - 8 * tx_watermark;
        } else {

            for (i = 0; i < p_this->data_size / 4; i++) {
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i], *p_this->p_data++);
            }
            if (p_this->data_size % 4) {
                uint32_t write_buf = 0;
                memcpy(&write_buf, p_this->p_data, p_this->data_size % 4);
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i + 1], write_buf);
            }

            p_this->data_size = 0;

            /* 释放信号量 */
            AW_SEMB_GIVE(p_this->flexspi_trans_sync);

            /* 禁能所有中断 */
            __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTEN, 0x00);
        }

        /* Push a watermark level datas into IP TX FIFO. */
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPTXWE_SHIFT);
    }

    return AW_OK;
}


/**
 * \brief Returns whether the bus is idle,
 *        return 0 Bus is busy
 *        return 1 Bus is idle
 */
aw_static_inline
uint8_t  __flexspi_get_bus_idle_status(imx1050_flexspi_regs_t  *p_hw_flexspi)
{
    return ( __FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->STS0, 0) &&
             __FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->STS0, 1));
}


/**
 * \brief 软件复位
 */
aw_static_inline
void __flexspi_software_rest (struct awbl_imx1050_flexspi_dev *p_this)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    /* Reset peripheral before configuring it */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->MCR0, __FLEXSPI_MCR0_SWRESET_SHIFT);
    while(__FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->MCR0, __FLEXSPI_MCR0_SWRESET_SHIFT));
}


/**
 * \brief DLL配置
 */
aw_local uint32_t __flexspi_dll_config(struct awbl_imx1050_flexspi_dev *p_this)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    uint8_t  is_unified_cfg = 1;
    uint32_t flexspi_dll_val = 0;
    uint32_t dll_val = 0;
    uint32_t temp = 0;

    uint32_t flexspi_root_clk = aw_clk_rate_get(p_devinfo->ref_clk_id);
    uint8_t  rx_sample_clk = (__FLEXSPI_REG_READ(&p_hw_flexspi->MCR0) & 0x30UL) >> 4;

    switch (rx_sample_clk) {

    case 0x0:
    case 0x1:
    case 0x2:
        is_unified_cfg = 1;
        break;
    case 0x3:
        is_unified_cfg = 0;
        break;
    default:
        break;
    }

    if (is_unified_cfg) {
        /* 1 fixed delay cells in DLL delay chain */
        flexspi_dll_val = 0x100UL;
    } else {
        if(flexspi_root_clk > 100000000) {

            /* DLLEN = 1, SLVDLYTARGET = 0xF */
            flexspi_dll_val = __FLEXSPI_DLLCR_DLLEN_MASK | __FLEXSPI_DLLCR_SLVDLYTARGET(0x0F);
        } else {

            /* Convert data valid time in ns to ps */
            temp = p_devinfo->dev_cfg.data_valid_time * 1000;
            dll_val = temp / 75;
            if (dll_val * 75 < temp) {
                dll_val++;
            }
            flexspi_dll_val = __FLEXSPI_DLLCR_OVRDEN_MASK | __FLEXSPI_DLLCR_OVRDVAL(dll_val);
        }
    }
    return flexspi_dll_val;
}


/**
 * \brief Flash参数配置
 */
aw_local void __flexspi_flash_config (struct awbl_imx1050_flexspi_dev *p_this)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    uint32_t cfg = 0;
    uint8_t  port = p_devinfo->port;

    /* Wait for bus idle before change flash configuration. */
    while (!__flexspi_get_bus_idle_status(p_hw_flexspi));

    /* Flash大小(单位是KByte),最大支持4G */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->FLSHCR0[port],
                         p_devinfo->dev_cfg.flash_size / 1024);

    /* 配置Flash的参数 */
    cfg = __FLEXSPI_FLSHCR1_CSINTERVAL(p_devinfo->dev_cfg.cs_interval) |
          __FLEXSPI_FLSHCR1_CSINTERVALUNIT(0) |
          __FLEXSPI_FLSHCR1_TCSH(p_devinfo->dev_cfg.cs_hold_time) |
          __FLEXSPI_FLSHCR1_TCSS(p_devinfo->dev_cfg.cs_setup_time) |
          __FLEXSPI_FLSHCR1_CAS(p_devinfo->dev_cfg.columnspace) |
          __FLEXSPI_FLSHCR1_WA(0);

    __FLEXSPI_REG_WRITE(&p_hw_flexspi->FLSHCR1[port], cfg);

    /* 配置AHB读写命令序列 */
    cfg = __FLEXSPI_FLSHCR2_AWRWAITUNIT(p_devinfo->dev_cfg.ahb_write_wait_unit)|
          __FLEXSPI_FLSHCR2_AWRWAIT(p_devinfo->dev_cfg.ahb_write_wait_interval)|
          __FLEXSPI_FLSHCR2_ARDSEQID(__FLEXSPI_SEQID_READ) |
          __FLEXSPI_FLSHCR2_ARDSEQNUM(0) |
          __FLEXSPI_FLSHCR2_AWRSEQID(__FLEXSPI_SEQID_PP)|
          __FLEXSPI_FLSHCR2_AWRSEQNUM(0);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->FLSHCR2[port], cfg);

    /* Configure DLL. */
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->DLLCR[port >> 1], __flexspi_dll_config(p_this));

    /* Exit stop mode. */
    __FLEXSPI_REG_BIT_CLR(&p_hw_flexspi->MCR0, 1);
}


/**
 * \brief FlexSPI初始化
 */
aw_local void __flexspi_hard_init (struct awbl_imx1050_flexspi_dev *p_this)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    uint32_t cfg = 0;
    int i = 0;

    /* Module enable */
    flexspi_enable(p_hw_flexspi);

    /* sofrware rest */
    __flexspi_software_rest(p_this);

    /* Configure MCR0 configuration items. */
    cfg = __FLEXSPI_MCR0_RXCLKSRC(0x0U) |
          __FLEXSPI_MCR0_DOZEEN(0)            | /* Disable Doze mode */
          __FLEXSPI_MCR0_IPGRANTWAIT(0xFFUL)  | /* Timeout wait cycle for AHB command grant */
          __FLEXSPI_MCR0_AHBGRANTWAIT(0xFFUL) | /* Time out wait cycle for IP command grant */
          __FLEXSPI_MCR0_SCKFREERUNEN(0)      | /* Disable force SCK output free-running */
          __FLEXSPI_MCR0_HSEN(0)              | /* Half Speed Serial Flash access disable */
          __FLEXSPI_MCR0_COMBINATIONEN(0)     | /* Disable combining Port A and B Data pins*/
          __FLEXSPI_MCR0_ATDFEN(0)            | /* IP TX FIFO should be written by IP Bus. */
          __FLEXSPI_MCR0_ARDFEN(0)            | /* IP RX FIFO should be read by IP Bus */
          __FLEXSPI_MCR0_MDIS_MASK;             /* Module Disable */

    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR0, cfg);

    /* Configure MCR1 configurations */
    cfg = __FLEXSPI_MCR1_AHBBUSWAIT(0xFFFFUL) |
          __FLEXSPI_MCR1_SEQWAIT(0xFFFFUL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR1, cfg);

    /* Configure MCR2 configurations */
    cfg = __FLEXSPI_MCR2_RESUMEWAIT(0x20UL) |
          __FLEXSPI_MCR2_SCKBDIFFOPT(__SCKB_USED_AS_A_SCK) |
          __FLEXSPI_MCR2_SAMEDEVICEEN(0) |
          __FLEXSPI_MCR2_CLRAHBBUFOPT(0); /* AHB RX/TX Buffer will not be cleaned automatically */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR2, 0x200801f7);

    /* Configure AHB control items. */
    cfg = __FLEXSPI_AHBCR_PREFETCHEN(1)   | /* AHB Read Prefetch Enable */
          __FLEXSPI_AHBCR_BUFFERABLEEN(0) | /* Disable AHB bus bufferable write access support */
          __FLEXSPI_AHBCR_CACHABLEEN(0);    /* Disable AHB bus cachable read access support */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->AHBCR, cfg);

    /* Configure AHB rx buffers */
    for (i = 0; i < __FLEXSPI_AHB_BUFFER_COUNT ; i++) {

        cfg = __FLEXSPI_AHBRXBUFCR0_PRIORITY(0) |
              __FLEXSPI_AHBRXBUFCR0_MSTRID(0)   |
              __FLEXSPI_AHBRXBUFCR0_BUFSZ(0);

        __FLEXSPI_REG_WRITE(&p_hw_flexspi->AHBRXBUFCR0[i], cfg);
    }

    /* Configure IP Fifo watermarks */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPRXFCR, __FLEXSPI_IPRXFCR_RXWMRK(15));
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPTXFCR, __FLEXSPI_IPTXFCR_TXWMRK(15));

    return;
}

/**
 * \brief FLEXSPI lut table lock
 */
aw_static_inline void __flexspi_lut_lock (imx1050_flexspi_regs_t  *p_hw_flexspi)
{
    /* Lock LUT. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x01);
}


/**
 * \brief FLEXSPI lut table unlock
 */
aw_static_inline void __flexspi_lut_unlock (imx1050_flexspi_regs_t *p_hw_flexspi)
{
    /* Unlock LUT for update. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x02);
}


/**
 * \brief FLEXSPI initialize lut table
 */
aw_local void __flexspi_lut_init (struct awbl_imx1050_flexspi_dev *p_this)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    volatile int  i = 0;

    uint32_t lut_base;
    uint8_t  op, dm;
    uint8_t  addr_len;
    int      rxfifo = __FLEXSPI_RXFIFO_SIZE;

    /* Wait for bus idle before change flash configuration. */
    while(!__flexspi_get_bus_idle_status(p_hw_flexspi));

    /* lut table unlock */
    __flexspi_lut_unlock(p_hw_flexspi);

    /* Clear all the LUT table */
    for (i = 0; i < __FLEXSPI_LUT_NUM; i++) {
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[i], 0x00);
    }

    /* Quad Read and DDR Quad Read*/
     lut_base = __FLEXSPI_SEQID_READ * 4;
     op       = p_devinfo->read_opcode;
     dm       = p_devinfo->read_dummy;
     addr_len = p_devinfo->addr_width == 3 ?
                __FLEXSPI_ADDR_24BIT : __FLEXSPI_ADDR_32BIT;

     switch (p_devinfo->read_mode) {
     case AWBL_FLEXSPI_NOR_SDR_QUAD :

         if (op == AWBL_FLEXSPI_NOR_OP_READ_1_1_4 ||
             op == AWBL_FLEXSPI_NOR_OP_READ4_1_1_4) {

             /* read mode : 1-1-4 */
             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                 FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, op) |
                                 FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                 FLEXSPI_LUT0_SEQ(CMD_DUMMY_SDR, PAD4, dm) |
                                 FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD4, rxfifo));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 2],
                                 FLEXSPI_LUT0_SEQ(CMD_STOP, PAD1, 0));

         } else {
             AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
         }
         break;

     case AWBL_FLEXSPI_NOR_DDR_QUAD :
         if (op == AWBL_FLEXSPI_NOR_OP_READ_1_4_4_D ||
             op == AWBL_FLEXSPI_NOR_OP_READ4_1_4_4_D) {

             /* read mode : 1-4-4, such as Spansion s25fl128s. */
             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                  FLEXSPI_LUT0_SEQ(CMD_DDR, PAD1, op) |
                                  FLEXSPI_LUT1_SEQ(CMD_RADDR_DDR, PAD4, addr_len));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                  FLEXSPI_LUT0_SEQ(CMD_MODE8_DDR, PAD4, 0xff) |
                                  FLEXSPI_LUT1_SEQ(CMD_DUMMY_DDR, PAD4, dm));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 2],
                                  FLEXSPI_LUT0_SEQ(CMD_LEARN_DDR, PAD4, 0X1) |
                                  FLEXSPI_LUT1_SEQ(CMD_READ_DDR, PAD4, rxfifo));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 3],
                                  FLEXSPI_LUT0_SEQ(CMD_STOP, PAD1, 0));

          } else if (op == AWBL_FLEXSPI_NOR_OP_READ_1_1_4_D) {

              /* read mode : 1-1-4, such as Micron N25Q256A. */
              __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                   FLEXSPI_LUT0_SEQ(CMD_DDR, PAD1, op) |
                                   FLEXSPI_LUT1_SEQ(CMD_RADDR_DDR, PAD1, addr_len));

              __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                   FLEXSPI_LUT0_SEQ(CMD_DUMMY_DDR, PAD1, dm) |
                                   FLEXSPI_LUT1_SEQ(CMD_READ_DDR, PAD4, rxfifo));

              __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 2],
                                   FLEXSPI_LUT0_SEQ(CMD_STOP, PAD1, 0));

         } else {
             AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
         }
         break;

     case AWBL_FLEXSPI_NOR_DUAL :
         if (op == AWBL_FLEXSPI_NOR_OP_READ_1_1_2 ||
             op == AWBL_FLEXSPI_NOR_OP_READ4_1_1_2) {

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                 FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, op) |
                                 FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                 FLEXSPI_LUT0_SEQ(CMD_DUMMY_SDR, PAD1, dm) |
                                 FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD2, rxfifo));

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 2],
                                 FLEXSPI_LUT0_SEQ(CMD_STOP, PAD1, 0));

         } else {
             AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
         }
         break;

     case AWBL_FLEXSPI_NOR_FAST :
         if (op == AWBL_FLEXSPI_NOR_OP_READ_FAST ||
             op == AWBL_FLEXSPI_NOR_OP_READ4_FAST) {

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                 FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, op) |
                                 FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                 FLEXSPI_LUT0_SEQ(CMD_DUMMY_SDR, PAD1, dm) |
                                 FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, rxfifo));

            __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 2],
                                 FLEXSPI_LUT0_SEQ(CMD_STOP, PAD1, 0));
         } else {
             AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
         }
         break;

     case AWBL_FLEXSPI_NOR_NORMAL :
     default :
         if (op == AWBL_FLEXSPI_NOR_OP_READ ||
             op == AWBL_FLEXSPI_NOR_OP_READ4) {

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                                 FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, op) |
                                 FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

             __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                                 FLEXSPI_LUT0_SEQ(CMD_READ_SDR, PAD1, rxfifo) |
                                 FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0));
         } else {
             AW_DBGF(("Unsupported opcode : 0x%.2x\n", op));
         }
         break;
     }

     /* Write enable */
     lut_base = __FLEXSPI_SEQID_WREN * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WREN));

     /* Page Program */
     lut_base = __FLEXSPI_SEQID_PP * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, p_devinfo->program_opcode) |
                          FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

     if (p_devinfo->program_opcode == AWBL_FLEXSPI_NOR_OP_PP_4) {
         __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                              FLEXSPI_LUT0_SEQ(CMD_WRITE_SDR, PAD4, p_devinfo->dev_cfg.page_size) |
                              FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0));
     } else {
         __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                              FLEXSPI_LUT0_SEQ(CMD_WRITE_SDR, PAD1, p_devinfo->dev_cfg.page_size) |
                              FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0));
     }

     /* Read Status */
     lut_base = __FLEXSPI_SEQID_RDSR * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_RDSR) |
                         FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, 0X1));
     /* Erase a sector */
     lut_base = __FLEXSPI_SEQID_SE * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, p_devinfo->erase_opcode) |
                         FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len));

     /* Erase the whole chip */
     lut_base = __FLEXSPI_SEQID_CHIP_ERASE * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_CHIP_ERASE));

     /* READ ID */
     lut_base = __FLEXSPI_SEQID_RDID * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_RDID) |
                         FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, 0x08));

     /* Write status Register */
     lut_base = __FLEXSPI_SEQID_WRSR * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRSR) |
                         FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, 0x00));

     /* Enter 4 Byte Mode (ISSI) */
     lut_base = __FLEXSPI_SEQID_EN4B * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_EN4B));

     /* read function register */
     lut_base = __FLEXSPI_SEQID_RDFR * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_RDFR) |
                          FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, 0x01));

     /* write function register */
     lut_base = __FLEXSPI_SEQID_WRFR * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRFR) |
                          FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, 0x00));

     /* Information Row Erase */
     lut_base = __FLEXSPI_SEQID_IRE * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                         FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRE) |
                         FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, __FLEXSPI_ADDR_24BIT));

     /* Information Row Program */
     lut_base = __FLEXSPI_SEQID_IRP * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRP) |
                          FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, __FLEXSPI_ADDR_24BIT));

     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                          FLEXSPI_LUT0_SEQ(CMD_WRITE_SDR, PAD1, __FLEXSPI_IR_SIZE) |
                          FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0));

     /* Information Row Read */
     lut_base = __FLEXSPI_SEQID_IRRD * 4;
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                          FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRRD) |
                          FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, __FLEXSPI_ADDR_24BIT));

    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base + 1],
                         FLEXSPI_LUT0_SEQ(CMD_DUMMY_SDR, PAD1, dm) |
                         FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, rxfifo));

     /* lut table lock */
    __flexspi_lut_lock(p_hw_flexspi);
}


/**
 * \brief 检查并清除错误状态标志
 */
aw_err_t  __flexsp_check_and_clear_error(imx1050_flexspi_regs_t *p_hw_flexspi,
                                         uint32_t                status)
{
    uint32_t reg = 0;

    /* Check for error. */
    status &= (AW_BIT(__FLEXSPI_INTR_SEQTIMEOUT_SHIFT) |
               AW_BIT(__FLEXSPI_INTR_IPCMDERR_SHIFT)   |
               AW_BIT(__FLEXSPI_INTR_IPCMDGE_SHIFT));
    if (status) {

        /* Clear the flags. */
        reg = __FLEXSPI_REG_READ(&p_hw_flexspi->INTR);
        reg |= status;
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTR, reg);

        /* Reset fifos. These flags clear automatically. */
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPTXFCR, __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT);
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPRXFCR, __FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT);
        return AW_ERROR;
    }

    return AW_OK;
}


/**
 * \brief 轮询方式写数据
 */
aw_local aw_err_t __flexspi_do_write(imx1050_flexspi_regs_t *p_hw_flexspi,
                                     uint8_t                *p_buf,
                                     size_t                  size)
{
    aw_err_t  ret = AW_OK;
    uint32_t  tx_water_mark = 0;
    uint32_t  i = 0;
    uint32_t  status = 0;
    uint32_t  write_buf = 0;
    uint32_t *p_buf32 = (uint32_t *)(p_buf);

    tx_water_mark = __FLEXSPI_REG_BITS_GET(&p_hw_flexspi->IPTXFCR,
                                           __FLEXSPI_IPTXFCR_TXWMRK_SHIFT,
                                           4) + 1;
    while (size) {

        /* IP TX FIFO watermark empty interrupt */
        while (!((status = __FLEXSPI_REG_READ(&p_hw_flexspi->INTR)) & 0x40UL)){

            if (__FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->INTR, 0)) {
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTR, 0x01);
            }
        }

        /* 清除错误标志 */
        if (__flexsp_check_and_clear_error(p_hw_flexspi, status)) {
            /* Reset fifos. These flags clear automatically. */
            return AW_ERROR;
        }

        /* Write watermark level data into tx fifo . */
        if (size >= 8 * tx_water_mark) {
            for (i = 0; i < 2 * tx_water_mark; i++) {
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i], *p_buf32++);
            }
            size = size - 8 * tx_water_mark;
        } else {
            for (i = 0; i < size / 4; i++) {
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i], *p_buf32++);
            }
            if (size % 4) {
                memcpy(&write_buf, p_buf32, size % 4);
                __FLEXSPI_REG_WRITE(&p_hw_flexspi->TFDR[i + 1], write_buf);
            }
            size = 0;
        }

        /* Push a watermark level datas into IP TX FIFO. */
        __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPTXWE_SHIFT);
    }

    return ret;
}


/**
 * \brief 轮询方式读数据
 */
aw_local aw_err_t __flexspi_do_read(imx1050_flexspi_regs_t *p_hw_flexspi,
                                    uint8_t                *p_buf,
                                    size_t                  size)
{
    aw_err_t  ret = AW_OK;
    uint32_t  rx_water_mark = 0;
    uint32_t  i = 0;
    uint32_t  status   = 0;
    uint32_t  read_buf = 0;
    uint32_t *p_buf32 = (uint32_t *)(p_buf);
    rx_water_mark = __FLEXSPI_REG_BITS_GET(&p_hw_flexspi->IPRXFCR,
                                            __FLEXSPI_IPRXFCR_RXWMRK_SHIFT,
                                            4) + 1;
     /* read data buffer */
     while (size) {
         if (size >= 8 * rx_water_mark) {

             /* Wait until there is room in the fifo. This also checks for errors. */
             while (!((status = __FLEXSPI_REG_READ(&p_hw_flexspi->INTR)) & 0x20)) {

                 ret = __flexsp_check_and_clear_error(p_hw_flexspi, status);
                 __FLEXSPI_ASSERT(ret != AW_OK, return ret)
             }
         } else {

             /* Wait fill level. This also checks for errors. */
            while (size > ((((__FLEXSPI_REG_READ(&p_hw_flexspi->IPRXFSTS)) &
                   __FLEXSPI_IPRXFSTS_FILL_MASK) >> __FLEXSPI_IPRXFSTS_FILL_SHIFT) * 8U))
            {
                 ret = __flexsp_check_and_clear_error(p_hw_flexspi,
                                        __FLEXSPI_REG_READ(&p_hw_flexspi->INTR));
                 __FLEXSPI_ASSERT(ret != AW_OK, return ret)
            }
         }

         ret = __flexsp_check_and_clear_error(p_hw_flexspi,
                                        __FLEXSPI_REG_READ(&p_hw_flexspi->INTR));
         __FLEXSPI_ASSERT(ret != AW_OK, return ret)

         /* Read watermark level data from rx fifo . */
         if (size >= 8 * rx_water_mark) {
             for (i = 0; i < 2 * rx_water_mark; i++) {
                 *p_buf32++ = __FLEXSPI_REG_READ(&p_hw_flexspi->RFDR[i]);
             }
             size = size - 8 * rx_water_mark;
         } else {

            for (i = 0; i < size / 4; i++) {
                *p_buf32++ = __FLEXSPI_REG_READ(&p_hw_flexspi->RFDR[i]);
            }
            if (size % 4) {
                read_buf =__FLEXSPI_REG_READ(&p_hw_flexspi->RFDR[i]);
                memcpy(p_buf32, &read_buf, size % 4);
            }
            size = 0;
         }

         /* 将已经读取到数据弹出FIFO */
         __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPRXWA_SHIFT);
     }
     return ret;
}


/**
 * \brief  使能接收中断
 */
aw_local aw_inline
void __flexspi_enable_rx_int(imx1050_flexspi_regs_t *p_hw_flexspi)
{
    uint32_t reg  = 0;

    reg = __FLEXSPI_INTEN_IPRXWAEN_MASK     |
          __FLEXSPI_INTEN_SEQTIMEOUTEN_MASK |
          __FLEXSPI_INTEN_IPCMDERREN_MASK   |
          __FLEXSPI_INTEN_IPCMDGEEN_MASK    |
          __FLEXSPI_INTEN_IPCMDDONEEN_MASK;

   __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTEN, reg);
}


/**
 * \brief  使能发送中断
 */
aw_local aw_inline
void __flexspi_enable_tx_int(imx1050_flexspi_regs_t *p_hw_flexspi)
{
    uint32_t reg  = 0;

   reg =  __FLEXSPI_INTEN_IPTXWEEN_MASK     |
          __FLEXSPI_INTEN_SEQTIMEOUTEN_MASK |
          __FLEXSPI_INTEN_IPCMDERREN_MASK   |
          __FLEXSPI_INTEN_IPCMDGEEN_MASK    |
          __FLEXSPI_INTEN_IPCMDDONEEN_MASK;

   __FLEXSPI_REG_WRITE(&p_hw_flexspi->INTEN, reg);
}


/**
 * \brief 处理一次传输
 */
aw_local aw_err_t __flexspi_do_transfer(awbl_imx1050_flexspi_dev_t *p_this,
                                        __flexspi_transfer_t       *p_trans)
{
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    aw_err_t ret = AW_OK;
    uint32_t cfg_val = 0;
    uint32_t old = 0;

    RTK_ENTER_CRITICAL(old);

    /* 释放信号量 */
    AW_SEMB_GIVE(p_this->flexspi_trans_sync);

    p_this->state = (p_trans->cmd_type == flexspi_read) ? __FLEXSPI_ST_READ : __FLEXSPI_ST_WRITE;
    p_this->p_data = (uint32_t *)p_trans->p_buf;
    p_this->data_size = p_trans->data_size;

    /* Clear sequence pointer before sending data to external devices */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->FLSHCR2[p_trans->port], 31);

    /* Clear former pending status before start this tranfer */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_AHBCMDERR_SHIFT);
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPCMDERR_SHIFT);
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_AHBCMDGE_SHIFT);
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPCMDGE_SHIFT);

    /* Configure base addresss */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCR0, p_trans->device_address);

    /* Reset tx and rx fifos */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPTXFCR, __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT);
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPRXFCR, __FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT);

    if ((p_trans->cmd_type == flexspi_read)  ||
        (p_trans->cmd_type == flexspi_write) ||
        (p_trans->cmd_type == flexspi_config)) {

        /* Configure data size */
        cfg_val = __FLEXSPI_IPCR1_IDATSZ(p_trans->data_size);
    }

    /* Configure sequence ID */
    cfg_val |= __FLEXSPI_IPCR1_ISEQID(p_trans->seq_index) |
               __FLEXSPI_IPCR1_ISEQNUM(p_trans->seq_num - 1);


    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCR1, cfg_val);

    /* trigger an IP Command */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCMD, 0x01);

    if ((p_trans->cmd_type == flexspi_write) ||
        (p_trans->cmd_type == flexspi_config)) {

#if 0   /* 中断方式 */
        /* 开发送中断 */
        __flexspi_enable_tx_int(p_hw_flexspi);

        /* 等待传输完成 */
        AW_SEMB_TAKE(p_this->flexspi_trans_sync, AW_SEM_WAIT_FOREVER);
#else  /* 轮询方式 */
        __flexspi_do_write(p_hw_flexspi, p_trans->p_buf, p_trans->data_size);
#endif

    } else if (p_trans->cmd_type == flexspi_read) {

        /* 轮询方式读数据，经验证，轮询方式比中断方式读速度更快 */
       ret = __flexspi_do_read(p_hw_flexspi, p_trans->p_buf, p_trans->data_size);
       __FLEXSPI_ASSERT(ret != AW_OK, goto __exit)
    }

    /* Wait for bus idle */
    while(!__flexspi_get_bus_idle_status(p_hw_flexspi));

    p_this->state = __FLEXSPI_ST_IDLE;

    if (p_trans->cmd_type == flexspi_command) {
        ret = __flexsp_check_and_clear_error(p_hw_flexspi, p_hw_flexspi->INTR);
        __FLEXSPI_ASSERT(ret != AW_OK, goto __exit)
    }

__exit:
    RTK_EXIT_CRITICAL(old);
    return ret;
}

/**
 * \brief nor flash读数据
 */
aw_local aw_err_t __flexspi_nor_flash_read(void       *p_dev,
                                           uint32_t    addr,
                                           uint8_t    *p_buf,
                                           uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    memcpy(p_buf, (const void *)(FLEXSPI_AMBA_BASE + addr), bytes);

    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);

    return AW_OK;
}

/**
 * \brief nor flash读ID
 */
aw_local aw_err_t __flexspi_nor_flash_read_id(void      *p_dev,
                                              uint8_t   *p_buf,
                                              uint32_t   bytes)
{
    aw_err_t ret = AW_OK;
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_read;
    transfer.seq_index = __FLEXSPI_SEQID_RDID;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);

    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);

    return ret;
}

/**
 * \brief nor flash读状态寄存器
 */
aw_local aw_err_t __flexspi_nor_flash_read_status (void       *p_dev,
                                                   uint8_t    *p_buf,
                                                   uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret = AW_OK;

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_read;
    transfer.seq_index = __FLEXSPI_SEQID_RDSR;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);

    return ret;
}


/**
 * \brief nor flash读function寄存器
 */
aw_local aw_err_t __flexspi_nor_flash_read_function (void       *p_dev,
                                                     uint8_t    *p_buf,
                                                     uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret = AW_OK;

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_read;
    transfer.seq_index = __FLEXSPI_SEQID_RDFR;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);

    return ret;
}


/**
 * \brief nor flash写使能
 */
aw_local aw_err_t __flexspi_nor_flash_write_enable(void      *p_dev,
                                                   uint32_t   addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret;

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_WREN;
    transfer.seq_num = 1;

    ret = __flexspi_do_transfer(p_this, &transfer);

    return ret;
}


/**
 * \brief nor flash进入四字节地址模式
 */
aw_local aw_err_t __flexspi_nor_flash_enter_4byte(void *p_dev)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret;

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_EN4B;
    transfer.seq_num = 1;

    ret = __flexspi_do_transfer(p_this, &transfer);

    return ret;
}


/**
 * \brief nor flash写function寄存器
 */
aw_local aw_err_t __flexspi_nor_flash_write_function (void *p_dev, uint8_t cfg)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    __flexspi_transfer_t transfer;
    uint32_t lut_base = 0;
    aw_err_t ret      = AW_OK;

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }

    __flexspi_lut_unlock(p_hw_flexspi);

    /* Write status Register */
    lut_base = __FLEXSPI_SEQID_WRFR * 4;
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRFR) |
                        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, cfg));
    __flexspi_lut_lock(p_hw_flexspi);

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_WRFR;
    transfer.seq_num = 1;
    transfer.p_buf = NULL;
    transfer.data_size = 0;

    ret = __flexspi_do_transfer(p_this, &transfer);

    return ret;
}


/**
 * \brief nor flash设置状态寄存器
 */
aw_local aw_err_t __flexspi_nor_flash_write_status (void *p_dev, uint8_t cfg)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    __flexspi_transfer_t transfer;
    uint32_t lut_base = 0;
    aw_err_t ret      = AW_OK;

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0);
    if (ret != AW_OK) {
        return ret;
    }

    __flexspi_lut_unlock(p_hw_flexspi);

    /* Write status Register */
    lut_base = __FLEXSPI_SEQID_WRSR * 4;
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRSR) |
                        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, cfg));
    __flexspi_lut_lock(p_hw_flexspi);

    transfer.device_address = 0;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_WRSR;
    transfer.seq_num = 1;
    transfer.p_buf = NULL;
    transfer.data_size = 0;

    ret = __flexspi_do_transfer(p_this, &transfer);

    if (ret == AW_OK) {
        do {
            __flexspi_nor_flash_read_status(p_this, &cfg, 1);
        } while(cfg & 0x01);
    }

    return ret;
}


/**
 * \brief nor flash写数据
 */
aw_local aw_err_t __flexspi_nor_flash_program(void       *p_dev,
                                              uint32_t    addr,
                                              uint8_t    *p_buf,
                                              uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret    = AW_OK;
    uint8_t  status = 1;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0);
    if (ret != AW_OK) {
        goto clean_up;
    }

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_write;
    transfer.seq_index = __FLEXSPI_SEQID_PP;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);
    if (ret != AW_OK) {
        goto clean_up;
    }

    /* 等待Flash编程完成 */
    while (status & 0x01) {
        ret = __flexspi_nor_flash_read_status(p_this, &status, 1);
    }

clean_up:
    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);
    return ret;
}


/**
 * \brief 擦除扇区
 */
aw_local aw_err_t __flexspi_nor_flash_erase_sector(void      *p_dev,
                                                   uint32_t   addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret    = AW_OK;
    uint8_t  status = 1;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0);
    if (ret != AW_OK) {
        goto clean_up;
    }

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_SE;
    transfer.seq_num = 1;

    ret = __flexspi_do_transfer(p_this, &transfer);
    if (ret != AW_OK) {
        goto clean_up;
    }

    /* 等待Flash擦除完成 */
    while (status & 0x01) {
        ret = __flexspi_nor_flash_read_status(p_this, &status, 1);
    }

clean_up:
    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);

    return ret;
}


/**
 * \brief 读取保留扇区数据
 */
aw_local aw_err_t __flexspi_nor_flash_ir_read_sector (void       *p_dev,
                                                      uint32_t    addr,
                                                      uint8_t    *p_buf,
                                                      uint32_t    bytes)
{
    __flexspi_transfer_t transfer;
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    aw_err_t ret = AW_OK;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_read;
    transfer.seq_index = __FLEXSPI_SEQID_IRRD;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);

    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);

    return ret;
}


/**
 * \brief 擦除保留扇区
 */
aw_local aw_err_t __flexspi_nor_flash_ir_erase_sector (void      *p_dev,
                                                       uint32_t   addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret    = AW_OK;
    uint8_t  status = 1;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0x00);
    if (ret != AW_OK) {
        goto end;
    }

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_command;
    transfer.seq_index = __FLEXSPI_SEQID_IRE;
    transfer.seq_num = 1;
    transfer.data_size = 0;

    ret = __flexspi_do_transfer(p_this, &transfer);
    if (ret != AW_OK) {
        goto end;
    }

    /* 等待Flash擦除完成 */
    while (status & 0x01) {
        ret = __flexspi_nor_flash_read_status(p_this, &status, 1);
    }

end:
    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);
    return ret;
}


/**
 * \brief 写保留扇区
 */
aw_local aw_err_t __flexspi_nor_flash_ir_program_sector(void       *p_dev,
                                                        uint32_t    addr,
                                                        uint8_t    *p_buf,
                                                        uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __flexspi_transfer_t transfer;
    aw_err_t ret    = AW_OK;
    uint8_t  status = 1;

    AW_MUTEX_LOCK(p_this->flexspi_trans_lock, AW_WAIT_FOREVER);

    /* 写使能 */
    ret = __flexspi_nor_flash_write_enable(p_this, 0x00);
    if (ret != AW_OK) {
        goto end;
    }

    transfer.device_address = addr;
    transfer.port = p_devinfo->port;
    transfer.cmd_type = flexspi_write;
    transfer.seq_index = __FLEXSPI_SEQID_IRP;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_this, &transfer);
    if (ret != AW_OK) {
        goto end;
    }

    /* 等待Flash编程完成 */
    while (status & 0x01) {
        ret = __flexspi_nor_flash_read_status(p_this, &status, 1);
    }

end:
    AW_MUTEX_UNLOCK(p_this->flexspi_trans_lock);
    return ret;
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __flexspi_inst_init1 (awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief 将Flexspi的读写驱动拷贝到内存，XIP模式下读写函数需要在内存中执行
 */
aw_static_inline void __copy_flexspi_drv_img_to_ram (void)
{
    extern int flexspi_drv_ram;
    extern int end_of_drv;

    memcpy((void *)__FLEXSPI_DRV_START,
           (const void *)&flexspi_drv_ram,
           ((uint32_t)&end_of_drv - (uint32_t)&flexspi_drv_ram));
}


/**
 * \brief xip模式下重新映射lut
 */
aw_static_inline void __flexspi_init_in_xip (imx1050_flexspi_regs_t *p_hw_flexspi)
{
    int i;

    /* lut table unlock */
    __flexspi_lut_unlock(p_hw_flexspi);

    /* Clear all the LUT table */
    for (i = 4; i < __FLEXSPI_LUT_NUM; i++) {
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[i], 0x00);
    }

    p_hw_flexspi->LUT[__FLEXSPI_SEQID_RDSR * 4]      = 0x24040405;     /* Read Status */
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_WREN * 4]      = 0x406;          /* Write Enable */
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_SE * 4]        = 0x8180420;      /* Erase Sector  */
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_WRSR * 4]      = 0x4010401;      /* write status */
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_PP * 4]        = 0x8180438;      /* program page */
    p_hw_flexspi->LUT[(__FLEXSPI_SEQID_PP * 4) + 1]  = 0x2204;
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_RDFR * 4]      = 0x24010448;     /* read function */
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_WRFR * 4]      = 0x4010442;      /* write function */

    p_hw_flexspi->LUT[__FLEXSPI_SEQID_IRRD * 4]      = 0x8180468;      /* read reserved line */
    p_hw_flexspi->LUT[(__FLEXSPI_SEQID_IRRD * 4) + 1]= 0x24803008;
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_IRP * 4]       = 0x8180462;      /* write reserved line */
    p_hw_flexspi->LUT[(__FLEXSPI_SEQID_IRP * 4) + 1] = 0x2000;
    p_hw_flexspi->LUT[__FLEXSPI_SEQID_IRE * 4]       = 0x8180464;      /* erase reserved line */

    __flexspi_lut_lock(p_hw_flexspi);
}


/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __flexspi_inst_init2 (awbl_dev_t *p_dev)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);

    /* 判断是否是Flexspi XIP 启动模式 */
   if (__IS_XIP_MODE) {

       p_this->xip_en = 1;
       __copy_flexspi_drv_img_to_ram();

       /* 指向Flexspi 服务函数  */
       memcpy(&p_this->serv_func, (const void *)(*(uint32_t *)__FLEXSPI_DRV_START),
              sizeof(struct awbl_flexspi_servfuncs));

       /* xip模式下只需要重新建立lut映射  */
       __flexspi_init_in_xip(p_hw_flexspi);

    } else {

        /* platform initialization */
        if (p_devinfo->pfunc_plfm_init != NULL) {
            p_devinfo->pfunc_plfm_init();
        }

        AW_SEMB_INIT(p_this->flexspi_trans_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);
        AW_MUTEX_INIT(p_this->flexspi_trans_lock, AW_SEM_Q_PRIORITY);

        /* init controller hardware */
        __flexspi_hard_init(p_this);

        /* 配置Flash的参数 */
        __flexspi_flash_config(p_this);

        /* 初始化命令查找表  */
        __flexspi_lut_init(p_this);

        /* 软件复位 */
        __flexspi_software_rest(p_this);

        /* connect and enable isr */
        aw_int_connect(p_devinfo->inum, (aw_pfuncvoid_t)__flexspi_isr, (void *)p_dev);
        aw_int_enable(p_devinfo->inum);

        p_this->serv_func.pfunc_flexspi_nor_flash_read = __flexspi_nor_flash_read;
        p_this->serv_func.pfunc_flexspi_nor_flash_read_status = __flexspi_nor_flash_read_status;
        p_this->serv_func.pfunc_flexspi_nor_flash_write_status = __flexspi_nor_flash_write_status;
        p_this->serv_func.pfunc_flexspi_nor_flash_write_enable = __flexspi_nor_flash_write_enable;
        p_this->serv_func.pfunc_flexspi_nor_flash_erase = __flexspi_nor_flash_erase_sector;
        p_this->serv_func.pfunc_flexspi_nor_flash_program = __flexspi_nor_flash_program;
        p_this->serv_func.pfunc_flexspi_nor_flash_read_function = __flexspi_nor_flash_read_function;
        p_this->serv_func.pfunc_flexspi_nor_flash_write_function = __flexspi_nor_flash_write_function;
        p_this->serv_func.pfunc_flexspi_nor_flash_read_ir =__flexspi_nor_flash_ir_read_sector;
        p_this->serv_func.pfunc_flexspi_nor_flash_write_ir =__flexspi_nor_flash_ir_program_sector;
        p_this->serv_func.pfunc_flexspi_nor_flash_erase_ir =__flexspi_nor_flash_ir_erase_sector;

    }

    /* 注册MTD设备 */
    if ((p_devinfo->p_mtd != NULL) && (p_devinfo->p_info != NULL)) {

        awbl_flexspi_flash_mtd_init(p_this, p_devinfo->p_mtd, p_devinfo->p_info);
    }

    awbl_imx1050_flexspi_init();
}


/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __flexspi_inst_connect(awbl_dev_t *p_dev)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev);

    uint8_t temp = 0;
    uint8_t status = 0;
    uint8_t times = 255;

    /* 如果操作命令使用到了四线模式，则需要将Flash配置为四线模式  */
    if ((p_this->xip_en) ||
        (p_devinfo->read_opcode == AWBL_FLEXSPI_NOR_OP_READ_1_1_4  ||
         p_devinfo->read_opcode == AWBL_FLEXSPI_NOR_OP_READ4_1_1_4 ||
         p_devinfo->program_opcode == AWBL_FLEXSPI_NOR_OP_PP_4))  {

        status |= FLASH_STATUS_REG_QE_MASK;
    }

    /* 默认写保护从地址0开始计算  */
    p_this->serv_func.pfunc_flexspi_nor_flash_read_function(p_dev, &temp, 1);
    if (!(temp & 0x02)) {
        temp |= 0x02;
        p_this->serv_func.pfunc_flexspi_nor_flash_write_function(p_dev, temp);
    }
    
    temp = 0;
    p_this->serv_func.pfunc_flexspi_nor_flash_read_status(p_dev, &temp, 1);

    /* 设置flash Block Protection */
    if (p_devinfo->bp_flag <= 8) {
        status |= (p_devinfo->bp_flag << 2);
    } else {
        status |= (temp & 0x3c);
    }

    while ((status != temp) && (times-- > 0)) {
        /* 增加喂狗操作，可能在某些不正常的复位情况下，会导致该循环跳出的时间大于看门狗喂狗时间,
         * 进而导致在此处发生看门狗复位，但此时norflash处于异常工作状态，
         * 会导致整个程序卡死在内部boot代码段(0x0020xxx 地址)。
         */
        awbl_hwwdt_feed();
        p_this->serv_func.pfunc_flexspi_nor_flash_write_status(p_dev, status);
        p_this->serv_func.pfunc_flexspi_nor_flash_read_status(p_dev, &temp, 1);
    }

   /* 进入四字节地址模式 */
   if (p_devinfo->dev_cfg.flash_size >= 16 *1024*1024) {
       __flexspi_nor_flash_enter_4byte(p_dev);
   }
}


aw_local aw_err_t __imx1050_flexspi_serv_func_get (struct awbl_dev *p_dev,
                                                   void            *p_arg)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);

    *((const struct awbl_flexspi_servfuncs **)p_arg) =  &p_this->serv_func;

    return AW_OK;
}


/** \brief driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_flexspi_drvfuncs = {
    __flexspi_inst_init1,
    __flexspi_inst_init2,
    __flexspi_inst_connect
};

/** \brief driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);
AWBL_METHOD_IMPORT(awbl_imx1050_flexspi_serv_func_get);

aw_local aw_const struct awbl_dev_method __g_flexspi_dev_methods[] = {
    AWBL_METHOD(awbl_imx1050_flexspi_serv_func_get,
                __imx1050_flexspi_serv_func_get),
    AWBL_METHOD(awbl_nvram_get, awbl_flexspi_nor_flash_nvram_get),
    AWBL_METHOD(awbl_nvram_set, awbl_flexspi_nor_flash_nvram_set),

    AWBL_METHOD_END
};


/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_flexspi_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB,                         /* bus_id */
        AWBL_IMX1050_FLEXSPI_NAME,              /* p_drvname */
        &__g_flexspi_drvfuncs,                  /* p_busfuncs */
        &__g_flexspi_dev_methods[0],            /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx1050_flexspi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_flexspi_drv_registration);
}

/* end of file */
