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

#ifndef __AW_RT10XX_LPSPI_SLV_H
#define __AW_RT10XX_LPSPI_SLV_H

#include "apollo.h"
#include "aw_bitops.h"
#include "aw_timer.h"

#define LPSPI1_BASE_ADDR    0x40394000
#define LPSPI2_BASE_ADDR    0x40398000
#define LPSPI3_BASE_ADDR    0x4039C000
#define LPSPI4_BASE_ADDR    0x403A0000

/**
 * \brief iMX RT105X LPSPI 寄存器块定义
 */
typedef struct imx10xx_lpspi_slv_regs{

    uint32_t verid;                     /**< \brief Version ID Register, offset: 0x0 */
    uint32_t param;                     /**< \brief Parameter Register, offset: 0x4 */
    uint8_t RESERVED_0[8];
    uint32_t cr;                        /**< \brief Control Register, offset: 0x10 */
    uint32_t sr;                        /**< \brief Status Register, offset: 0x14 */
    uint32_t ier;                       /**< \brief Interrupt Enable Register, offset: 0x18 */
    uint32_t der;                       /**< \brief DMA Enable Register, offset: 0x1C */
    uint32_t cfgr0;                     /**< \brief Configuration Register 0, offset: 0x20 */
    uint32_t cfgr1;                     /**< \brief Configuration Register 1, offset: 0x24 */
    uint8_t RESERVED_1[8];
    uint32_t dmr0;                      /**< \brief Data Match Register 0, offset: 0x30 */
    uint32_t dmr1;                      /**< \brief Data Match Register 1, offset: 0x34 */
    uint8_t RESERVED_2[8];
    uint32_t ccr;                       /**< \brief Clock Configuration Register, offset: 0x40 */
    uint8_t RESERVED_3[20];
    uint32_t fcr;                       /**< \brief FIFO Control Register, offset: 0x58 */
    uint32_t fsr;                       /**< \brief FIFO Status Register, offset: 0x5C */
    uint32_t tcr;                       /**< \brief Transmit Command Register, offset: 0x60 */
    uint32_t tdr;                       /**< \brief Transmit Data Register, offset: 0x64 */
    uint8_t RESERVED_4[8];
    uint32_t rsr;                       /**< \brief Receive Status Register, offset: 0x70 */
    uint32_t rdr;                       /**< \brief Receive Data Register, offset: 0x74 */

} imx10xx_lpspi_slv_regs_t;

/**
 * \brief iMX RT10XX  SPI 从设备实例
 */
typedef struct imx10xx_lpspi_slv_dev {

    imx10xx_lpspi_slv_regs_t   *p_hw_lpspi;     /**< /brief SPI 从设备基地址 */
    uint32_t                    tx_index;       /**< /brief 已发送的字节数 */
    uint32_t                    rx_nbytes;      /**< /brief 已接收的字节数 */
    uint32_t                    tx_nbytes;      /**< /brief 要发送的字节数 */
    const void                 *p_tx_buf;       /**< /brief 指向发送缓存的指针 */
    void                       *p_rx_buf;       /**< /brief 指向接收缓存的指针 */
    aw_timer_t                  spi_slave_timer;/**< /brief 软件超时定时器 */
    uint8_t                     bpw;            /**< /brief spi传输帧位数*/
    int (*pfn_isr_active)(void *p_arg);         /**< /brief 中断回调函数 */
    void                       *p_arg;          /**< /brief 中断回调函数参数 */

} imx10xx_lpspi_slv_dev_t;


/** \brief 设置后第2个时钟沿进行采样,否则在第1个时钟沿进行采样 */
#define AW_SPI_CPHA         0x01

/** \brief 设置后空闲时钟是高电平, 否则为低电平 */
#define AW_SPI_CPOL         0x02

/**< \brief 按低位先出的方式发送数据帧 */
#define AW_SPI_LSB_FIRST    0x08

/* write 8-bit width repeatly */
#define LPSPI_DATA_WRITE_DUMP_REP(p_hw_lpspi, len) {\
        size_t i = len; \
        while (i--) {   \
            writel(0, (void *)&p_hw_lpspi->tdr);\
        } \
    }

/* write 8-bit width repeatly */
#define LPSPI_DATA_READ_DUMP_REP(p_hw_lpspi, len) {\
        size_t i = len; \
        while (i--) {   \
            readl((void *)&p_hw_lpspi->rdr);\
        } \
    }

#define LPSPI_DATA_READ_REP(p_hw_lpspi, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf);  \
        while (i--) { \
            *ptr++ = readl((void *)&p_hw_lpspi->rdr);   \
        } \
}

#define LPSPI_DATA_READ(p_hw_lpspi, p_buf, offset, len, bpw)    {   \
             if (p_buf == NULL) {                                   \
                 LPSPI_DATA_READ_DUMP_REP(p_hw_lpspi, len);         \
             } else {                                               \
                 LPSPI_DATA_READ_REP(p_hw_lpspi,                    \
                         ((uint##bpw##_t *)p_buf) + offset,         \
                         len,                                       \
                         bpw)                                       \
             }                                                      \
}

#define LPSPI_DATA_WRITE_REP(p_hw_lpspi, p_buf, len, bpw) { \
        size_t i = len; \
        uint##bpw##_t *ptr = (uint##bpw##_t *)(p_buf);  \
        while (i--) { \
            AW_REG_WRITE32(&p_hw_lpspi->tdr, *ptr++);   \
        } \
    }

#define LPSPI_DATA_WRITE(p_hw_lpspi, p_buf, offset, len, bpw)    {  \
             if (p_buf == NULL) {                                   \
                 LPSPI_DATA_WRITE_DUMP_REP(p_hw_lpspi, len);        \
             } else {                                               \
                 LPSPI_DATA_WRITE_REP(p_hw_lpspi,                   \
                                ((uint##bpw##_t*)p_buf) + offset,   \
                                 len ,                              \
                                 bpw)                               \
             }                                                      \
}

/**
 * \brief 清除标志位
 *
 * \param[in] flags spi设备状态标志
 */
void  lpspi_int_status_clr (imx10xx_lpspi_slv_regs_t   *p_hw_lpspi,
                            uint32_t                    flags);

/**
 * \brief 中断使能
 *
 * \param[in] mask  spi设备中断标志
 */
void  lpspi_interrupts_enable (imx10xx_lpspi_slv_regs_t *p_hw_lpspi,
                               uint32_t                  mask);

/**
 * \brief 中断禁能
 *
 * \param[in] mask  spi设备中断标志
 */
void  lpspi_interrupts_disable (imx10xx_lpspi_slv_regs_t   *p_hw_lpspi,
                                uint32_t                    mask);

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
                                  uint8_t                   mode);

/**
 * \brief spi从机发送函数
 *
 * \param[in] p_dev  spi设备实例
 * \param[in] buf    要发送数据的首地址
 * \param[in] size   要发送数据的大小
 */

aw_err_t lpspi_slv_send ( imx10xx_lpspi_slv_dev_t  *p_dev,
                          uint8_t                  *buf,
                          uint32_t                  size);

/**
 * \brief spi引脚配置
 */
aw_err_t imx1050_lpspi3_slv_plfm_init (void);

#endif  /* __AW_RT10XX_LPSPI_SLV_H */

/*end of file */

