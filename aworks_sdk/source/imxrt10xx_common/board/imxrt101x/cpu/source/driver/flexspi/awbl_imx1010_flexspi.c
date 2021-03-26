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
 * \brief iMX RT1010 FlexSPI driver
 *
 * \internal
 * \par modification history:
 * - 1.00 19-05-21  ral, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_int.h"
#include "string.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_clk.h"
#include "aw_vdebug.h"
#include "aw_barrier.h"
#include "rtk_int.h"
#include "driver/flexspi/awbl_imx1010_flexspi.h"
#include "driver/flexspi/awbl_imx1010_flexspi_mtd.h"

/*******************************************************************************
  SPI State and Event define
*******************************************************************************/
#define __FLEXSPI_LUT_NUM            64

/** \brief LUT KEY */
#define __FLEXSPI_LUT_KEY_VAL        0x5AF05AF0UL

/** \brief SEQ ID */
#define __FLEXSPI_SEQID_READ         0       /**< \brief read flash */
#define __FLEXSPI_SEQID_RDSR         1       /**< \brief read status register*/
#define __FLEXSPI_SEQID_WRSR         2       /**< \brief write status register */
#define __FLEXSPI_SEQID_RDFR         3       /**< \brief read function register */
#define __FLEXSPI_SEQID_WRFR         4       /**< \brief write function register */
#define __FLEXSPI_SEQID_WREN         5       /**< \brief write enable */
#define __FLEXSPI_SEQID_SE           6       /**< \brief erase section */
#define __FLEXSPI_SEQID_PP           7       /**< \brief page program */
#define __FLEXSPI_SEQID_IRRD         9      /**< \brief infomation row read */
#define __FLEXSPI_SEQID_IRER         10      /**< \brief infomation row erase */
#define __FLEXSPI_SEQID_IRP          11      /**< \brief infomation row program */

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

/*******************************************************************************
  macro operate
*******************************************************************************/

/* read register */
#define __FLEXSPI_REG_READ(reg_addr)  \
        *(volatile uint32_t *)reg_addr

/* write register */
#define __FLEXSPI_REG_WRITE(reg_addr, data) \
        rtk_arm_barrier_dsb();   \
        *(volatile uint32_t *)reg_addr = (uint32_t)data;  \
        rtk_arm_barrier_dsb();

/* set register bit */
#define __FLEXSPI_REG_BIT_SET(reg_addr, bit) \
        do { \
            volatile uint32_t __val__ = __FLEXSPI_REG_READ(reg_addr);   \
            AW_BIT_SET(__val__, bit);   \
            __FLEXSPI_REG_WRITE(reg_addr, __val__) \
        } while(0)

/*write register*/
#define __FLEXSPI_REG_BIT_WRITE(reg_addr, data) \
        do { \
            volatile uint32_t __val__ = __FLEXSPI_REG_READ(reg_addr);   \
            __val__ |= data;   \
            __FLEXSPI_REG_WRITE(reg_addr, __val__) \
        } while(0)

/* get register bits */
#define __FLEXSPI_REG_BITS_GET(reg_addr, start, len) \
        AW_BITS_GET(__FLEXSPI_REG_READ(reg_addr), start, len)

/* bit is set */
#define __FLEXSPI_REG_BIT_ISSET(reg_addr, bit) \
        AW_BIT_ISSET(__FLEXSPI_REG_READ(reg_addr), bit)

/* memcpy */
#define __FLEXSPI_MEMCPY(buf1, buf2, size) \
        do { \
            volatile uint8_t *p_dst8 = (uint8_t *)buf1; \
            volatile uint8_t *p_src8 = (uint8_t *)buf2; \
            volatile size_t cpy_size = size; \
            while(cpy_size--) { \
                *p_dst8++ = *p_src8++; \
            } \
        } while(0)

/* declare FLEXSPI device instance */
#define __FLEXSPI_DEV_DECL(p_this, p_dev) \
    struct awbl_imx1010_flexspi_dev *p_this = \
        (struct awbl_imx1010_flexspi_dev *)(p_dev)

/* declare FLEXSPI device infomation */
#define __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_imx1010_flexspi_devinfo *p_devinfo = \
        (struct awbl_imx1010_flexspi_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get imx1010 spi hardware infomation */
#define __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo)      \
        imx1010_flexspi_regs_t *p_hw_flexspi =          \
        ((imx1010_flexspi_regs_t *)((p_devinfo)->regbase))

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

aw_local aw_err_t __flexspi_do_transfer (
        struct awbl_imx1010_flexspi_devinfo *p_devinfo,
        struct flexspi_transfer              *p_trans);

/*******************************************************************************
  locals
*******************************************************************************/
/**
 * \brief Returns whether the length is more than program size,
 *        return 0 less than program size
 *        return 1 more than program size
 */
__attribute__((used, section(".ram.func")))
aw_local uint8_t __flexspi_check_length(
        const struct flexspi_device_config *p_cfg,
        uint32_t                            offset)
{
    return ((( p_cfg->pgm_start_addr - p_cfg->pgm_end_addr) > offset) ? 0 : 1 );
}


/**
 * \brief FLEXSPI lut table lock
 */
aw_local void __flexspi_lut_lock (imx1010_flexspi_regs_t *p_hw_flexspi)
{
    /* Lock LUT. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x01);
}


/**
 * \brief FLEXSPI lut table unlock
 */
aw_local void __flexspi_lut_unlock (imx1010_flexspi_regs_t *p_hw_flexspi)
{
    /* Unlock LUT for update. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x02);
}


/**
 * \brief 检查并清除错误状态标志
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexsp_check_and_clear_error(
        imx1010_flexspi_regs_t *p_hw_flexspi,
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
 * \brief flexspi wait flash idle
 */
__attribute__((used, section(".ram.func")))
aw_local void __flexspi_wait_idle(struct awbl_imx1010_flexspi_devinfo *p_devinfo)
{
    uint8_t status;
    flexspi_transfer_t transfer;

    transfer.cmd_type = flexspi_read;
    transfer.device_address = 0;
    transfer.seq_index = __FLEXSPI_SEQID_RDSR;
    transfer.seq_num = 1;
    transfer.p_buf = &status;
    transfer.data_size = 1;

    do {
        __flexspi_do_transfer(p_devinfo, &transfer);
    } while(status & 0x01);
}


/**
 * \brief 轮询方式写数据
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_do_write(imx1010_flexspi_regs_t *p_hw_flexspi,
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
                __FLEXSPI_MEMCPY(&write_buf, p_buf32, size % 4);
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
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_do_read(imx1010_flexspi_regs_t *p_hw_flexspi,
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
                 if (ret != AW_OK) {
                     return ret;
                 }
             }
         } else {

             /* Wait fill level. This also checks for errors. */
            while (size > ((((__FLEXSPI_REG_READ(&p_hw_flexspi->IPRXFSTS)) &
                   __FLEXSPI_IPRXFSTS_FILL_MASK) >> __FLEXSPI_IPRXFSTS_FILL_SHIFT) * 8U))
            {
                 ret = __flexsp_check_and_clear_error(p_hw_flexspi,
                                        __FLEXSPI_REG_READ(&p_hw_flexspi->INTR));
                 if (ret != AW_OK) {
                     return ret;
                 }
            }
         }

         ret = __flexsp_check_and_clear_error(p_hw_flexspi,
                                        __FLEXSPI_REG_READ(&p_hw_flexspi->INTR));
         if (ret != AW_OK) {
                return ret;
         }

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
                __FLEXSPI_MEMCPY(p_buf32, &read_buf, size % 4);

            }
            size = 0;
         }

         /* 将已经读取到数据弹出FIFO */
         __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->INTR, __FLEXSPI_INTR_IPRXWA_SHIFT);
     }
     return ret;
}

/**
 * \brief 处理一次传输
 */
__attribute__((used, section(".ram.func")))
aw_err_t __flexspi_do_transfer (struct awbl_imx1010_flexspi_devinfo *p_devinfo,
                                struct flexspi_transfer              *p_trans)
{
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    uint32_t reg = 0;
    int ret = AW_OK;

    /* Clear sequence pointer before sending data to external devices */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->FLSHCR2[p_devinfo->port], 31);

    /* Clear former pending status */
    __FLEXSPI_REG_BIT_WRITE(&p_hw_flexspi->INTR, __FLEXSPI_INTR_AHBCMDERR_SHIFT |
                                                 __FLEXSPI_INTR_IPCMDERR_SHIFT |
                                                 __FLEXSPI_INTR_AHBCMDGE_SHIFT |
                                                 __FLEXSPI_INTR_IPCMDGE_SHIFT);

    /* Reset tx and rx fifos */
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPTXFCR, __FLEXSPI_IPTXFCR_CLRIPTXF_SHIFT);
    __FLEXSPI_REG_BIT_SET(&p_hw_flexspi->IPRXFCR, __FLEXSPI_IPRXFCR_CLRIPRXF_SHIFT);

    /* Configure base addresss */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCR0, p_trans->device_address);

    /* Configure sequence info */
    reg |= __FLEXSPI_IPCR1_IDATSZ(p_trans->data_size) |
           __FLEXSPI_IPCR1_ISEQID(p_trans->seq_index) |
           __FLEXSPI_IPCR1_ISEQNUM(p_trans->seq_num - 1);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCR1, reg);

    /* trigger an IP Command */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPCMD, 0x01);

    /* write information row */
    if (p_trans->seq_index == __FLEXSPI_SEQID_IRP) {
        ret = __flexspi_do_write(p_hw_flexspi,
                                 p_trans->p_buf,
                                 p_trans->data_size);
        if (ret != AW_OK) {
            goto __exit;
        }
    }

    /* read register or information row */
    if (p_trans->cmd_type == flexspi_read) {
        ret = __flexspi_do_read(p_hw_flexspi,
                                p_trans->p_buf,
                                p_trans->data_size);
        if (ret != AW_OK) {
            goto __exit;
        }
    }

    /* write command */
    if (p_trans->cmd_type == flexspi_write) {
        /* Wait for bus idle */
        while(!( __FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->STS0, 0) &&
                 __FLEXSPI_REG_BIT_ISSET(&p_hw_flexspi->STS0, 1)));

        /* 等待flash空闲 */
        __flexspi_wait_idle(p_devinfo);
    }

    /* check error */
    ret = __flexsp_check_and_clear_error(
            p_hw_flexspi, __FLEXSPI_REG_READ(&p_hw_flexspi->INTR));

__exit:
    return ret;
}


/**
 * \brief flexspi write enable
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_write_enable(void *p_dev, uint32_t addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;

    transfer.cmd_type = flexspi_write;
    transfer.device_address = addr;
    transfer.seq_index = __FLEXSPI_SEQID_WREN;
    transfer.seq_num = 1;

    return __flexspi_do_transfer(p_devinfo, &transfer);
}


/**
 * \brief flexspi read status register
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_rdsr(void    *p_dev,
                                           uint8_t *p_buf)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;

    transfer.cmd_type = flexspi_read;
    transfer.device_address = 0;
    transfer.seq_index = __FLEXSPI_SEQID_RDSR;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = 1;

    return __flexspi_do_transfer(p_devinfo, &transfer);
}


/**
 * \brief flexspi read function register
 */
__attribute__((used, section(".ram.func")))
aw_err_t __flexspi_nor_flash_rdfr(void       *p_dev,
                                  uint8_t    *p_buf)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;

    transfer.cmd_type = flexspi_read;
    transfer.device_address = 0;
    transfer.seq_index = __FLEXSPI_SEQID_RDFR;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = 1;

    return __flexspi_do_transfer(p_devinfo, &transfer);
}


/**
 * \brief flexspi write status register
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_wrsr(void       *p_dev,
                                           uint8_t    *p_buf)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    flexspi_transfer_t transfer;
    uint32_t lut_base = 0;
    aw_err_t ret = AW_OK;

    /* 写使能 */
    ret = __flexspi_write_enable(p_dev, 0x00);
    if (ret != AW_OK) {
        goto __exit;
    }

    /* Unlock LUT for update. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x02);

    /* Write status Register */
    lut_base = __FLEXSPI_SEQID_WRSR * 4;
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRSR) |
                        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1,  *p_buf));

    /* Lock LUT. */
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTKEY, __FLEXSPI_LUT_KEY_VAL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUTCR, 0x01);

    transfer.device_address = 0;
    transfer.cmd_type = flexspi_write;
    transfer.seq_index = __FLEXSPI_SEQID_WRSR;
    transfer.seq_num = 1;
    transfer.data_size = 0;

    ret = __flexspi_do_transfer(p_devinfo, &transfer);

__exit:
    return ret;
}


/**
 * \brief flexspi write function register
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_wrfr(void       *p_dev,
                                           uint8_t    *p_buf)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    flexspi_transfer_t transfer;
    uint32_t lut_base = 0;
    aw_err_t ret = AW_OK;

    /* 写使能 */
    ret = __flexspi_write_enable(p_dev, 0x00);
    if (ret != AW_OK) {
        goto __exit;
    }

    __flexspi_lut_unlock(p_hw_flexspi);

    /* Write status Register */
    lut_base = __FLEXSPI_SEQID_WRFR * 4;
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[lut_base],
                        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRFR) |
                        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1,  *p_buf));
    __flexspi_lut_lock(p_hw_flexspi);

    transfer.device_address = 0;
    transfer.cmd_type = flexspi_write;
    transfer.seq_index = __FLEXSPI_SEQID_WRFR;
    transfer.seq_num = 1;
    transfer.data_size = 0;

    ret = __flexspi_do_transfer(p_devinfo, &transfer);

__exit:
    return ret;
}


/**
 * \brief nor flash读数据
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_read(void       *p_dev,
                                           uint32_t    offset,
                                           uint8_t    *p_buf,
                                           uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    uint32_t *p_ahbaddr = NULL;

    /* 判断是否超出可编程区域的长度  */
    if (__flexspi_check_length(p_devinfo->p_cfg, (offset + bytes))) {
        return -AW_EINVAL;
    }

    p_ahbaddr = (uint32_t *)(p_devinfo->p_cfg->pgm_start_addr + offset);

    __FLEXSPI_MEMCPY(p_buf, p_ahbaddr, bytes);

    return AW_OK;
}

/**
 * \brief flash page program
 *
 * \param[in] p_dev     device instance
 * \param[in] offset    program flash offset form start_addr to end_addr
 * \param[in] p_buf     write data
 * \param[in] bytes     write size
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_program(void       *p_dev,
                                              uint32_t    offset,
                                              uint8_t    *p_buf,
                                              uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    volatile uint8_t *p_ahbaddr = NULL;
    aw_err_t ret;
    uint8_t status;

    /* 判断是否超出可编程区域的长度  */
    if (__flexspi_check_length(p_devinfo->p_cfg, (offset + bytes))) {
        return -AW_EINVAL;
    }

    /* 每次只允许写一页数据 */
    if (bytes > p_devinfo->p_cfg->page_size) {
        return -AW_EINVAL;
    }

    p_ahbaddr = (uint8_t *)(p_devinfo->p_cfg->pgm_start_addr + offset);

    __FLEXSPI_MEMCPY(p_ahbaddr, p_buf, bytes);

    /* 等待Flash编程完成 */
    do {
        ret = __flexspi_nor_flash_rdsr(p_dev, &status);
    } while(status & 0x01);

    return ret;
}


/**
 * \brief 擦除扇区
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_nor_flash_erase_sector(void      *p_dev,
                                                   uint32_t   addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;
    uint32_t real_addr;
    aw_err_t ret;

    real_addr = (p_devinfo->p_cfg->pgm_start_addr - 0x60000000) + addr;

    /* 写使能 */
    ret = __flexspi_write_enable(p_dev, 0x00);
    if (ret != AW_OK) {
        goto __exit;
    }

    transfer.cmd_type = flexspi_write;
    transfer.device_address = real_addr;
    transfer.seq_index = __FLEXSPI_SEQID_SE;
    transfer.seq_num = 1;
    transfer.p_buf = NULL;
    transfer.data_size = 0;
    ret = __flexspi_do_transfer(p_devinfo, &transfer);
    if (ret != AW_OK) {
        goto __exit;
    }

__exit :
    return ret;
}


/**
 * \brief flexspi read information row
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_information_row_read(void       *p_dev,
                                                 uint32_t    addr,
                                                 uint8_t    *p_buf,
                                                 uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;
    aw_err_t ret;

    transfer.cmd_type = flexspi_read;
    transfer.device_address = addr;
    transfer.seq_index = __FLEXSPI_SEQID_IRRD;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    return __flexspi_do_transfer(p_devinfo, &transfer);
}


/**
 * \brief flexspi erase information rows
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_information_row_erase(void *p_dev, uint32_t addr)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;
    aw_err_t ret;

    /* 写使能 */
    ret = __flexspi_write_enable(p_dev, 0x00);
    if (ret != AW_OK) {
        goto __exit;
    }

    transfer.cmd_type = flexspi_write;
    transfer.device_address = addr;
    transfer.seq_index = __FLEXSPI_SEQID_IRER;
    transfer.seq_num = 1;
    transfer.data_size = 0;
    ret = __flexspi_do_transfer(p_devinfo, &transfer);

__exit :
    return ret;
}


/**
 * \brief flexspi program information row
 */
__attribute__((used, section(".ram.func")))
aw_local aw_err_t __flexspi_information_row_program(void      *p_dev,
                                                    uint32_t    addr,
                                                    uint8_t    *p_buf,
                                                    uint32_t    bytes)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_this);
    flexspi_transfer_t transfer;
    aw_err_t ret;

    /* 写使能 */
    ret = __flexspi_write_enable(p_dev, 0x00);
    if (ret != AW_OK) {
        goto clean_up;
    }

    transfer.device_address = addr;
    transfer.cmd_type = flexspi_write;
    transfer.seq_index = __FLEXSPI_SEQID_IRP;
    transfer.seq_num = 1;
    transfer.p_buf = p_buf;
    transfer.data_size = bytes;

    ret = __flexspi_do_transfer(p_devinfo, &transfer);

clean_up:
    return ret;
}


/**
 * \brief DLL配置
 */
aw_local uint32_t __flexspi_dll_config(struct awbl_imx1010_flexspi_dev *p_this)
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
            temp = p_devinfo->p_cfg->data_valid_time * 1000;
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
 * \brief  flexspi hard init
 *
 * \param[in] p_devinfo device information
 */
__attribute__((used, section(".ram.func")))
aw_local void __flexspi_hard_init (awbl_dev_t *p_dev)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev);
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    uint32_t cfg;

    /* Configure MCR0 configuration items. */
    cfg = __FLEXSPI_REG_READ(&p_hw_flexspi->MCR0);

    cfg |= __FLEXSPI_MCR0_RXCLKSRC(0x0U)       |
           __FLEXSPI_MCR0_DOZEEN(1)            |
           __FLEXSPI_MCR0_IPGRANTWAIT(0xFFUL)  |
           __FLEXSPI_MCR0_AHBGRANTWAIT(0xFFUL) |
           __FLEXSPI_MCR0_SCKFREERUNEN(0)      |
           __FLEXSPI_MCR0_HSEN(0)              |
           __FLEXSPI_MCR0_COMBINATIONEN(0)     |
           __FLEXSPI_MCR0_ATDFEN(0)            |
           __FLEXSPI_MCR0_ARDFEN(0);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR0, cfg);

    /* Configure MCR1 configurations */
    cfg = __FLEXSPI_MCR1_AHBBUSWAIT(0xFFFFUL) |
          __FLEXSPI_MCR1_SEQWAIT(0xFFFFUL);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR1, cfg);

    /* Configure MCR2 configurations */
//    cfg = p_hw_flexspi->MCR2;
//    cfg |= __FLEXSPI_MCR2_RESUMEWAIT(0x20UL) |
//           __FLEXSPI_MCR2_SCKBDIFFOPT(0) |
//           __FLEXSPI_MCR2_SAMEDEVICEEN(0) |
//           __FLEXSPI_MCR2_CLRAHBBUFOPT(0);
//    __FLEXSPI_REG_WRITE(&p_hw_flexspi->MCR2, cfg);

    /* Configure AHB control items. */
    cfg = __FLEXSPI_REG_READ(&p_hw_flexspi->AHBCR);

    cfg |= __FLEXSPI_AHBCR_READADDROPT(1)  |
           __FLEXSPI_AHBCR_PREFETCHEN(1)   |
           __FLEXSPI_AHBCR_BUFFERABLEEN(0) |
           __FLEXSPI_AHBCR_CACHABLEEN(1);
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->AHBCR, cfg);

    /* 配置Flash的参数 */
    cfg = __FLEXSPI_REG_READ(&p_hw_flexspi->FLSHCR1[p_devinfo->port]);

    cfg |= __FLEXSPI_FLSHCR1_CSINTERVAL(p_devinfo->p_cfg->cs_interval) |
           __FLEXSPI_FLSHCR1_CSINTERVALUNIT(0) |
           __FLEXSPI_FLSHCR1_TCSH(p_devinfo->p_cfg->cs_hold_time) |
           __FLEXSPI_FLSHCR1_TCSS(p_devinfo->p_cfg->cs_setup_time) |
           __FLEXSPI_FLSHCR1_CAS(p_devinfo->p_cfg->columnspace) |
           __FLEXSPI_FLSHCR1_WA(0);
    __FLEXSPI_REG_WRITE(&p_hw_flexspi->FLSHCR1[p_devinfo->port], cfg);

    /* 配置AHB读写命令序列  */
    cfg = __FLEXSPI_REG_READ(&p_hw_flexspi->FLSHCR2[p_devinfo->port]);

    cfg = __FLEXSPI_FLSHCR2_AWRWAITUNIT(p_devinfo->p_cfg->ahb_write_wait_unit)|
          __FLEXSPI_FLSHCR2_AWRWAIT(p_devinfo->p_cfg->ahb_write_wait_interval)|
          __FLEXSPI_FLSHCR2_ARDSEQID(__FLEXSPI_SEQID_READ) |
          __FLEXSPI_FLSHCR2_ARDSEQNUM(0) |
          __FLEXSPI_FLSHCR2_AWRSEQID(__FLEXSPI_SEQID_PP)|
          __FLEXSPI_FLSHCR2_AWRSEQNUM(1);
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->FLSHCR2[p_devinfo->port], cfg);

     /* Configure IP Fifo watermarks */
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPRXFCR, __FLEXSPI_IPRXFCR_RXWMRK(15));
     __FLEXSPI_REG_WRITE(&p_hw_flexspi->IPTXFCR, __FLEXSPI_IPTXFCR_TXWMRK(15));

     /* Configure DLL. */
     cfg = __flexspi_dll_config(p_this);

      __FLEXSPI_REG_WRITE(&p_hw_flexspi->DLLCR[p_devinfo->port], cfg);
}


/**
 * \brief config lookup table again
 *
 * \param[in] p_devinfo device information
 */
aw_local void __flexspi_lut_init(struct awbl_imx1010_flexspi_devinfo *p_devinfo)
{
    __FLEXSPI_HW_DECL(p_hw_flexspi, p_devinfo);
    uint32_t lut_base;
    int      i;

    uint8_t  addr_len = __FLEXSPI_ADDR_24BIT;

    /* lut table unlock */
    __flexspi_lut_unlock(p_hw_flexspi);

    /* Clear all the LUT table */
    for (i = 4; i < __FLEXSPI_LUT_NUM; i++) {
        __FLEXSPI_REG_WRITE(&p_hw_flexspi->LUT[i], 0x00);
    }

    if (p_devinfo->addr_width == 3) {
        addr_len = __FLEXSPI_ADDR_24BIT;

    } else if (p_devinfo->addr_width == 4) {
        addr_len = __FLEXSPI_ADDR_32BIT;
    }

    /* write enable */
    lut_base = __FLEXSPI_SEQID_WREN * 4;
    p_hw_flexspi->LUT[lut_base] =
            FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WREN);

    /* erase section */
    lut_base = __FLEXSPI_SEQID_SE * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, p_devinfo->erase_opcode) |
        FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len);

    /* AHB page program */
    lut_base = __FLEXSPI_SEQID_PP * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WREN);

    p_hw_flexspi->LUT[lut_base + 4] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, p_devinfo->program_opcode) |
        FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len);

    p_hw_flexspi->LUT[lut_base + 5] =
        FLEXSPI_LUT0_SEQ(CMD_WRITE_SDR, PAD4, 0x01) |
        FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0);

    /* read function register */
    lut_base = __FLEXSPI_SEQID_RDFR * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_RDFR) |
        FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, 0x01);

    /* write function register */
    lut_base = __FLEXSPI_SEQID_WRFR * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRFR) |
        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, 0x00);

    /* read status register */
    lut_base = __FLEXSPI_SEQID_RDSR * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_RDSR) |
        FLEXSPI_LUT1_SEQ(CMD_READ_SDR, PAD1, 0x01);

    /* write status register */
    lut_base = __FLEXSPI_SEQID_WRSR * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_WRSR) |
        FLEXSPI_LUT1_SEQ(CMD_SDR, PAD1, 0x40);

    /* security infomation row read */
    lut_base = __FLEXSPI_SEQID_IRRD * 4;
    p_hw_flexspi->LUT [lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRRD) |
        FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len);

    p_hw_flexspi->LUT[lut_base + 1] =
        FLEXSPI_LUT0_SEQ(CMD_DUMMY_SDR, PAD1, 0x08) |
        FLEXSPI_LUT1_SEQ(CMD_READ_SDR,  PAD1, 0x80);

    /* security infomation row erase */
    lut_base = __FLEXSPI_SEQID_IRER * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRER) |
        FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len);

    /* security infomation row program */
    lut_base = __FLEXSPI_SEQID_IRP * 4;
    p_hw_flexspi->LUT[lut_base] =
        FLEXSPI_LUT0_SEQ(CMD_SDR, PAD1, AWBL_FLEXSPI_NOR_OP_IRP) |
        FLEXSPI_LUT1_SEQ(CMD_RADDR_SDR, PAD1, addr_len);

    p_hw_flexspi->LUT[lut_base + 1] =
        FLEXSPI_LUT0_SEQ(CMD_WRITE_SDR, PAD1, 0x80) |
        FLEXSPI_LUT1_SEQ(CMD_STOP, PAD1, 0);

    __flexspi_lut_lock(p_hw_flexspi);
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
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __flexspi_inst_init2 (awbl_dev_t *p_dev)
{
    __FLEXSPI_DEV_DECL(p_this, p_dev);
    __FLEXSPI_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    /* configuration common register*/
    __flexspi_hard_init(p_dev);

    /* reset LUT for AHB command */
    __flexspi_lut_init(p_devinfo);

    p_this->serv_func.pfunc_flexspi_nor_flash_read = __flexspi_nor_flash_read;
    p_this->serv_func.pfunc_flexspi_nor_flash_erase = __flexspi_nor_flash_erase_sector;
    p_this->serv_func.pfunc_flexspi_nor_flash_program = __flexspi_nor_flash_program;
    p_this->serv_func.pfunc_flexspi_nor_flash_rdfr = __flexspi_nor_flash_rdfr;
    p_this->serv_func.pfunc_flexspi_nor_flash_rdsr = __flexspi_nor_flash_rdsr;
    p_this->serv_func.pfunc_flexspi_nor_flash_wrfr = __flexspi_nor_flash_wrfr;
    p_this->serv_func.pfunc_flexspi_nor_flash_wrsr = __flexspi_nor_flash_wrsr;
    p_this->serv_func.pfunc_flexspi_nor_flash_irrd = __flexspi_information_row_read;
    p_this->serv_func.pfunc_flexspi_nor_flash_irer = __flexspi_information_row_erase;
    p_this->serv_func.pfunc_flexspi_nor_flash_irp = __flexspi_information_row_program;

    /* 注册MTD设备 */
    if ((p_devinfo->p_mtd != NULL) && (p_devinfo->p_mtd_info != NULL)) {
        awbl_flexspi_flash_mtd_init(p_this, p_devinfo->p_mtd, p_devinfo->p_mtd_info);
    }

    awbl_imx1010_flexspi_init();
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __flexspi_inst_connect(awbl_dev_t *p_dev)
{
}

aw_local aw_err_t __imx1010_flexspi_serv_func_get (struct awbl_dev *p_dev,
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
AWBL_METHOD_IMPORT(awbl_imx1010_flexspi_serv_func_get);

aw_local aw_const struct awbl_dev_method __g_flexspi_dev_methods[] = {
    AWBL_METHOD(awbl_imx1010_flexspi_serv_func_get,
                __imx1010_flexspi_serv_func_get),
    AWBL_METHOD(awbl_nvram_get, awbl_flexspi_nor_flash_nvram_get),
    AWBL_METHOD(awbl_nvram_set, awbl_flexspi_nor_flash_nvram_set),

    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_flexspi_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB,                         /* bus_id */
        AWBL_IMX1010_FLEXSPI_NAME,              /* p_drvname */
        &__g_flexspi_drvfuncs,                  /* p_busfuncs */
        &__g_flexspi_dev_methods[0],            /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};

/******************************************************************************/
void awbl_imx1010_flexspi_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_flexspi_drv_registration);
}

/* end of file */
