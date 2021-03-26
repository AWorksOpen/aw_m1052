/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief iMX RT10xx lpspi 从机例程驱动
 *
 * \internal
 * \par modification history:
 * - 1.00 20-04-16  licl, first implementation
 * \endinternal
 */

#include "aw_int.h"
#include "aw_rt10xx_lpspi_slv.h"

aw_local void __lpspi_mode_set (imx10xx_lpspi_slv_regs_t *p_hw_lpspi,
                                uint8_t                   bpw,
                                uint8_t                   mode)
{
    uint32_t reg = 0;

    reg = ((((uint32_t)(bpw - 1)) << 0) & 0xFFFUL);

    /* 设置后第2个时钟沿进行采样 */
    if (mode & AW_SPI_CPHA) {
        reg |= (0x1UL << 30);
    }
	/* 设置后空闲时钟是高电平 */
    if (mode & AW_SPI_CPOL) {
        reg |= (0x1UL << 31);
    }

    /* 设置后按低位先出的方式发送数据帧 Data is transferred LSB first */
    if (mode & AW_SPI_LSB_FIRST) {
        reg |= (0x1UL << 23);
    }

    AW_REG_WRITE32(&p_hw_lpspi->tcr, reg);
}

/* 中断回调函数  */
aw_local void __lpspi_isr (void *p_arg)
{
    imx10xx_lpspi_slv_dev_t     *p_dev      = (imx10xx_lpspi_slv_dev_t *)p_arg;
    imx10xx_lpspi_slv_regs_t    *p_hw_lpspi = p_dev->p_hw_lpspi;

    /* 清除所有标志 */
    lpspi_int_status_clr(p_hw_lpspi, 0x1003f03);

    if (p_dev->pfn_isr_active != NULL) {
        p_dev->pfn_isr_active(p_dev->p_arg);
    }
}

/**
 * \brief 中断使能
 *
 * \param[in] mask  spi设备中断标志
 */
void lpspi_interrupts_enable (imx10xx_lpspi_slv_regs_t  *p_hw_lpspi,
                              uint32_t                   mask)
{
    uint32_t reg = 0;

    reg = AW_REG_READ32(&p_hw_lpspi->ier);
    reg |= mask;
    AW_REG_WRITE32(&p_hw_lpspi->ier, reg);
}

/**
 * \brief 中断禁能
 *
 * \param[in] mask  spi设备中断标志
 */
void lpspi_interrupts_disable (imx10xx_lpspi_slv_regs_t *p_hw_lpspi,
                               uint32_t                  mask)
{
    uint32_t reg = 0;

    reg = AW_REG_READ32(&p_hw_lpspi->ier);
    reg &= ~mask;
    AW_REG_WRITE32(&p_hw_lpspi->ier, reg);
}

/**
 * \brief 清除标志位
 *
 * \param[in] flags spi设备状态标志
 */
void lpspi_int_status_clr (imx10xx_lpspi_slv_regs_t *p_hw_lpspi,
                           uint32_t                  flags)
{
    uint32_t reg = 0;

    reg = AW_REG_READ32(&p_hw_lpspi->sr);
    reg |= flags;
    AW_REG_WRITE32(&p_hw_lpspi->sr, reg);
}

/**
 * \brief spi设备配置
 *
 * \param[in] p_dev  spi设备实例
 * \param[in] p_dev  spi设备基地址
 * \param[in] p_dev  spi设备中断号
 * \param[in] p_dev  spi传输帧宽度
 * \param[in] p_dev  spi模式
 */
aw_err_t lpspi_slv_configuration (imx10xx_lpspi_slv_dev_t  *p_dev,
                                  uint32_t                 *p_hw,
                                  uint32_t                  inum,
                                  uint8_t                   bpw,
                                  uint8_t                   mode)
{
    imx10xx_lpspi_slv_regs_t *p_hw_lpspi = (imx10xx_lpspi_slv_regs_t *)p_hw;
    aw_err_t ret = AW_OK;
    uint32_t reg = 0;

    /* 设置数据位大小 */
    if ((bpw < 8) || (bpw > 32)) {
        return -AW_ENOTSUP;
    }

    p_dev->p_hw_lpspi  = p_hw_lpspi;
    p_dev->rx_nbytes   = 0;
    p_dev->tx_index    = 0;

    if (ret != AW_OK) {
        return ret;
    }

    /* reset the lpspi */
    reg = AW_REG_READ32(&p_hw_lpspi->cr);
    reg |= (0x1UL << 1) |    /* Software Reset */
           (0x1UL << 9) |    /* Reset Receive FIFO */
           (0x1UL << 8);     /* Reset Transmit FIFO */

    AW_REG_WRITE32(&p_hw_lpspi->cr, reg);
    AW_REG_WRITE32(&p_hw_lpspi->cr, 0x00);

    /* enable lpspi */
    AW_REG_BIT_SET32(&p_hw_lpspi->cr, 0);

    reg = (0x0UL)                                   |   /* Slave mode */
          ((((uint32_t)(3)) << 24) & 0x3000000UL)   |   /* SIN is used for input data and SOUT for output data */
          ((((uint32_t)(0)) << 26) & 0x4000000UL);      /* Output data retains last value when chip select is negated */

    AW_REG_WRITE32(&p_hw_lpspi->cfgr1, reg);

    /* 设置FIFO中断 */
    AW_REG_WRITE32(&p_hw_lpspi->fcr,
                  ((1) & 0xFUL) | ((0 << 16) & 0xF0000UL));

    /* disable lpspi */
    AW_REG_BIT_CLR32(&p_hw_lpspi->cr, 0);

    /* 清空FIFO */
    reg = AW_REG_READ32(&p_hw_lpspi->cr);
    reg |= ((1 << 8UL) | (1 << 9UL));
    AW_REG_WRITE32(&p_hw_lpspi->cr, reg);

    /* 清除所有的中断标志和所有的中断 */
    lpspi_int_status_clr(p_hw_lpspi, 0x1003f03);
    lpspi_interrupts_disable(p_hw_lpspi, 0x3F03);

    __lpspi_mode_set(p_hw_lpspi, bpw, mode);

    /* enable lpspi */
    AW_REG_BIT_SET32(&p_hw_lpspi->cr, 0);

    aw_int_connect(inum, (aw_pfuncvoid_t)__lpspi_isr, p_dev->p_arg);
    aw_int_enable(inum);

    return AW_OK;
}

/**
 * \brief spi从机发送函数
 *
 * \param[in] p_dev  spi设备实例
 * \param[in] buf    要发送数据的首地址
 * \param[in] size   要发送数据的大小
 */
aw_err_t lpspi_slv_send ( imx10xx_lpspi_slv_dev_t  *p_dev,
                          uint8_t                  *buf,
                          uint32_t                  size)
{
    imx10xx_lpspi_slv_regs_t *p_hw_lpspi = p_dev->p_hw_lpspi;
    uint8_t                  *p_tx_buf   = p_dev->p_tx_buf;

    p_dev->tx_index  = 0;
    p_dev->tx_nbytes = size;

    if (p_dev->bpw == 8) {
        /* 复制用户要传输的数据 */
        memcpy(p_tx_buf,buf,size);
        /* 向管道中添加数据 */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, p_tx_buf[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else if (p_dev->bpw == 16) {
        /* 复制用户要传输的数据 */
        memcpy(p_tx_buf,buf,size*2);
        /* 向管道中添加数据 */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint16_t*)p_tx_buf)[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    } else {
        /* 复制用户要传输的数据 */
        memcpy(p_tx_buf,buf,size*4);
        /* 向管道中添加数据 */
        while(((AW_REG_READ32(&p_hw_lpspi->fsr) & 0x1f) < 15) && \
               (p_dev->tx_index < p_dev->tx_nbytes)) {

            AW_REG_WRITE32(&p_hw_lpspi->tdr, ((uint32_t*)p_tx_buf)[p_dev->tx_index]);
            p_dev->tx_index++;
        }
    }
    
    return AW_OK;
}

/*end of file */
