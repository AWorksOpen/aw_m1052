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
 * \brief iMX RT10xx lpspi �ӻ���������
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
 * \brief iMX RT105X LPSPI �Ĵ����鶨��
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
 * \brief iMX RT10XX  SPI ���豸ʵ��
 */
typedef struct imx10xx_lpspi_slv_dev {

    imx10xx_lpspi_slv_regs_t   *p_hw_lpspi;     /**< /brief SPI ���豸����ַ */
    uint32_t                    tx_index;       /**< /brief �ѷ��͵��ֽ��� */
    uint32_t                    rx_nbytes;      /**< /brief �ѽ��յ��ֽ��� */
    uint32_t                    tx_nbytes;      /**< /brief Ҫ���͵��ֽ��� */
    const void                 *p_tx_buf;       /**< /brief ָ���ͻ����ָ�� */
    void                       *p_rx_buf;       /**< /brief ָ����ջ����ָ�� */
    aw_timer_t                  spi_slave_timer;/**< /brief �����ʱ��ʱ�� */
    uint8_t                     bpw;            /**< /brief spi����֡λ��*/
    int (*pfn_isr_active)(void *p_arg);         /**< /brief �жϻص����� */
    void                       *p_arg;          /**< /brief �жϻص��������� */

} imx10xx_lpspi_slv_dev_t;


/** \brief ���ú��2��ʱ���ؽ��в���,�����ڵ�1��ʱ���ؽ��в��� */
#define AW_SPI_CPHA         0x01

/** \brief ���ú����ʱ���Ǹߵ�ƽ, ����Ϊ�͵�ƽ */
#define AW_SPI_CPOL         0x02

/**< \brief ����λ�ȳ��ķ�ʽ��������֡ */
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
 * \brief �����־λ
 *
 * \param[in] flags spi�豸״̬��־
 */
void  lpspi_int_status_clr (imx10xx_lpspi_slv_regs_t   *p_hw_lpspi,
                            uint32_t                    flags);

/**
 * \brief �ж�ʹ��
 *
 * \param[in] mask  spi�豸�жϱ�־
 */
void  lpspi_interrupts_enable (imx10xx_lpspi_slv_regs_t *p_hw_lpspi,
                               uint32_t                  mask);

/**
 * \brief �жϽ���
 *
 * \param[in] mask  spi�豸�жϱ�־
 */
void  lpspi_interrupts_disable (imx10xx_lpspi_slv_regs_t   *p_hw_lpspi,
                                uint32_t                    mask);

/**
 * \brief spi�豸����
 *
 * \param[in] p_dev  spi�豸ʵ��
 * \param[in] p_dev  spi�豸����ַ
 * \param[in] p_dev  spi�豸�жϺ�
 * \param[in] p_dev  spi����֡���
 * \param[in] p_dev  spiģʽ
 */
aw_err_t lpspi_slv_configuration (imx10xx_lpspi_slv_dev_t  *p_dev,
                                  uint32_t                 *p_hw,
                                  uint32_t                  inum,
                                  uint8_t                   bpw,
                                  uint8_t                   mode);

/**
 * \brief spi�ӻ����ͺ���
 *
 * \param[in] p_dev  spi�豸ʵ��
 * \param[in] buf    Ҫ�������ݵ��׵�ַ
 * \param[in] size   Ҫ�������ݵĴ�С
 */

aw_err_t lpspi_slv_send ( imx10xx_lpspi_slv_dev_t  *p_dev,
                          uint8_t                  *buf,
                          uint32_t                  size);

/**
 * \brief spi��������
 */
aw_err_t imx1050_lpspi3_slv_plfm_init (void);

#endif  /* __AW_RT10XX_LPSPI_SLV_H */

/*end of file */

