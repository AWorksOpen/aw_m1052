/*******************************************************************************
*                                 AWorks
*                       ----------------------------
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
 * \brief AW imx1010 eDMA 接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "aw_imx1010_edma.h"
 * \endcode
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-07  hsg, first implementation
 * \endinternal
 */


#ifndef __AW_IMX1010_EDMA_H
#define __AW_IMX1010_EDMA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "awbus_lite.h"

/* 类型定义 */
typedef uint32_t            dma_addr_t;

#define MAX_EDMA_CHANNELS 16
#define MXC_SDMA_DEFAULT_PRIORITY 1
#define MXC_SDMA_MIN_PRIORITY 1
#define MXC_SDMA_MAX_PRIORITY 7


/** \brief DMA 驱动名 */
#define AWBL_IMX1010_EDMA_NAME                  "imx1010_edma"

/**
 * enum dma_transfer_direction - dma transfer mode and direction indicator
 * @DMA_MEM_TO_MEM: Async/Memcpy mode
 * @DMA_MEM_TO_DEV: Slave mode & From Memory to Device
 * @DMA_DEV_TO_MEM: Slave mode & From Device to Memory
 * @DMA_DEV_TO_DEV: Slave mode & From Device to Device
 */
enum dma_transfer_direction {
    DMA_MEM_TO_MEM,
    DMA_MEM_TO_DEV,
    DMA_DEV_TO_MEM,
    DMA_DEV_TO_DEV,
    DMA_TRANS_NONE,
};

/**
 * enum dma_slave_buswidth - defines bus with of the DMA slave
 * device, source or target buses
 */
enum dma_slave_buswidth {
    DMA_SLAVE_BUSWIDTH_1_BYTE = 0,
    DMA_SLAVE_BUSWIDTH_2_BYTES = 1,
    DMA_SLAVE_BUSWIDTH_4_BYTES = 2,
    DMA_SLAVE_BUSWIDTH_16_BYTES = 4,
    DMA_SLAVE_BUSWIDTH_32_BYTES = 5
};


/**
 * struct dma_slave_config - dma slave channel runtime config
 * @direction: whether the data shall go in or out on this slave
 * channel, right now. DMA_MEM_TO_DEV and DMA_DEV_TO_MEM are
 * legal values.
 * @src_addr: this is the physical address where DMA slave data
 * should be read (RX), if the source is memory this argument is
 * ignored.
 * @dst_addr: this is the physical address where DMA slave data
 * should be written (TX), if the source is memory this argument
 * is ignored.
 * @src_addr_width: this is the width in bytes of the source (RX)
 * register where DMA data shall be read. If the source
 * is memory this may be ignored depending on architecture.
 * Legal values: 1, 2, 4, 8.
 * @dst_addr_width: same as src_addr_width but for destination
 * target (TX) mutatis mutandis.
 * @src_maxburst: the maximum number of words (note: words, as in
 * units of the src_addr_width member, not bytes) that can be sent
 * in one burst to the device. Typically something like half the
 * FIFO depth on I/O peripherals so you don't overflow it. This
 * may or may not be applicable on memory sources.
 * @dst_maxburst: same as src_maxburst but for destination target
 * mutatis mutandis.
 * @device_fc: Flow Controller Settings. Only valid for slave channels. Fill
 * with 'true' if peripheral should be flow controller. Direction will be
 * selected at Runtime.
 * @slave_id: Slave requester id. Only valid for slave channels. The dma
 * slave peripheral will have unique id as dma requester which need to be
 * pass as slave config.
 *
 * This struct is passed in as configuration data to a DMA engine
 * in order to set up a certain channel for DMA transport at runtime.
 * The DMA device/engine has to provide support for an additional
 * command in the channel config interface, DMA_SLAVE_CONFIG
 * and this struct will then be passed in as an argument to the
 * DMA engine device_control() function.
 *
 * The rationale for adding configuration information to this struct
 * is as follows: if it is likely that most DMA slave controllers in
 * the world will support the configuration option, then make it
 * generic. If not: if it is fixed so that it be sent in static from
 * the platform data, then prefer to do that. Else, if it is neither
 * fixed at runtime, nor generic enough (such as bus mastership on
 * some CPU family and whatnot) then create a custom slave config
 * struct and pass that, then make this config a member of that
 * struct, if applicable.
 */
struct dma_slave_config {
    enum dma_transfer_direction direction;
    dma_addr_t src_addr;
    dma_addr_t dst_addr;
    enum dma_slave_buswidth src_addr_width;
    enum dma_slave_buswidth dst_addr_width;
    uint32_t src_maxburst;
    uint32_t dst_maxburst;
    aw_bool_t device_fc;
    unsigned int slave_id;
};

/**
 * enum dma_status - DMA transaction status
 * @DMA_COMPLETE: transaction completed
 * @DMA_IN_PROGRESS: transaction not yet processed
 * @DMA_PAUSED: transaction is paused
 * @DMA_ERROR: transaction failed
 */
enum dma_status {
    DMA_COMPLETE,
    DMA_IN_PROGRESS,
    DMA_PAUSED,
    DMA_ERROR,
};

/**
 * enum dma_ctrl_cmd - DMA operations that can optionally be exercised
 * on a running channel.
 * @DMA_TERMINATE_ALL: terminate all ongoing transfers
 * @DMA_PAUSE: pause ongoing transfers
 * @DMA_RESUME: resume paused transfer
 * @DMA_SLAVE_CONFIG: this command is only implemented by DMA controllers
 * that need to runtime reconfigure the slave channels (as opposed to passing
 * configuration data in statically from the platform). An additional
 * argument of struct dma_slave_config must be passed in with this
 * command.
 * @FSLDMA_EXTERNAL_START: this command will put the Freescale DMA controller
 * into external start mode.
 */
enum dma_ctrl_cmd {
    DMA_TERMINATE_ALL,
    DMA_PAUSE,
    DMA_RESUME,
    DMA_SLAVE_CONFIG,
    FSLDMA_EXTERNAL_START,
};

/**
 * \brief 通道的配置状态
 */
enum dma_chan_cfg_status {
    CHAN_CFG_0 = 0,  /* 未配置状态  */
    CHAN_CFG_1 = 1,  /* 调用imx1010_edma_control(,DMA_SLAVE_CONFIG,,)进行了基础的配置 */
    CHAN_CFG_2 = 2   /* 配置了源和目标的地址，可以开始传输了  */

};


/*
 * This enumerates peripheral types. Used for EDMA.
 */
enum edma_peripheral_type {
    IMX_DMATYPE_FLEXIO1_CHNANNEL0,
    IMX_DMATYPE_FLEXIO2_CHANNNEL1,
    IMX_DMATYPE_LPUART1_TX,
    IMX_DMATYPE_LPUART1_RX,
    IMX_DMATYPE_LPUART3_TX,
    IMX_DMATYPE_LPUART3_RX,
    IMX_DMATYPE_LPUART5_TX,
    IMX_DMATYPE_LPUART5_RX,
    IMX_DMATYPE_LPUART7_TX,
    IMX_DMATYPE_LPUART7_RX,
    IMX_DMATYPE_RESERVED_10,
    IMX_DMATYPE_RESERVED_11,
    IMX_DMATYPE_CSI,
    IMX_DMATYPE_LPSPI1_RX,
    IMX_DMATYPE_LPSPI1_TX,
    IMX_DMATYPE_LPSPI3_RX,
    IMX_DMATYPE_LPSPI3_TX,
    IMX_DMATYPE_LPI2C1,
    IMX_DMATYPE_LPI2C3,
    IMX_DMATYPE_SAI1_RX,
    IMX_DMATYPE_SAI1_TX,
    IMX_DMATYPE_SAI2_RX,
    IMX_DMATYPE_SAI2_TX,
    IMX_DMATYPE_ADC_ETC,
    IMX_DMATYPE_ADC1,
    IMX_DMATYPE_ACMP1,
    IMX_DMATYPE_ACMP2,
    IMX_DMATYPE_RESERVED_27,
    IMX_DMATYPE_FLEXSPI_RX,
    IMX_DMATYPE_FLEXSPI_TX,
    IMX_DMATYPE_XBAR1_0,
    IMX_DMATYPE_XBAR1_1,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE0_READ,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE1_READ,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE2_READ,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE3_READ,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE0_WRITE,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE1_WRITE,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE2_WRITE,
    IMX_DMATYPE_FLEXPWM1_SUB_MODULE3_WRITE,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE0_READ,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE1_READ,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE2_READ,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE3_READ,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE0_WRITE,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE1_WRITE,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE2_WRITE,
    IMX_DMATYPE_FLEXPWM3_SUB_MODULE3_WRITE,
    IMX_DMATYPE_QTIMER1_CAPT_0_READ,
    IMX_DMATYPE_QTIMER1_CAPT_1_READ,
    IMX_DMATYPE_QTIMER1_CAPT_2_READ,
    IMX_DMATYPE_QTIMER1_CAPT_3_READ,
    IMX_DMATYPE_QTIMER1_CMPLD1_0_OR_CMALD2_1_WRITE,
    IMX_DMATYPE_QTIMER1_CMPLD1_1_OR_CMPLD2_0_WRITE,
    IMX_DMATYPE_QTIMER1_CMPLD1_2_OR_CMPLD2_3_WRITE,
    IMX_DMATYPE_QTIMER1_CMPLD1_3_OR_CMPLD2_2_WRITE,
    IMX_DMATYPE_QTIMER3_CAPT_0_READ_OR_CMPLD1_0_CMPLD2_1_WRITE,
    IMX_DMATYPE_QTIMER3_CAPT_1_READ_OR_CMPLD1_1_CMPLD2_0_WRITE,
    IMX_DMATYPE_QTIMER3_CAPT_2_READ_OR_CMPLD1_2_CMPLD2_3_WRITE,
    IMX_DMATYPE_QTIMER3_CAPT_3_READ_OR_CMPLD1_3_CMPLD2_2_WRITE,
    IMX_DMATYPE_RESERVED_60,
    IMX_DMATYPE_RESERVED_61,
    IMX_DMATYPE_RESERVED_62,
    IMX_DMATYPE_RESERVED_63,
    IMX_DMATYPE_FLEXIO1,
    IMX_DMATYPE_FLEXIO2,
    IMX_DMATYPE_LPUART2_TX,
    IMX_DMATYPE_LPUART2_RX,
    IMX_DMATYPE_LPUART4_TX,
    IMX_DMATYPE_LPUART4_RX,
    IMX_DMATYPE_LPUART6_TX,
    IMX_DMATYPE_LPUART6_RX,
    IMX_DMATYPE_LPUART8_TX,
    IMX_DMATYPE_LPUART8_RX,
    IMX_DMATYPE_RESERVED_74,
    IMX_DMATYPE_PXP,
    IMX_DMATYPE_LCDIF,
    IMX_DMATYPE_LPSPI2_RX,
    IMX_DMATYPE_LPSPI2_TX,
    IMX_DMATYPE_LPSPI4_RX,
    IMX_DMATYPE_LPSPI4_TX,
    IMX_DMATYPE_LPI2C2,
    IMX_DMATYPE_LPI2C4,
    IMX_DMATYPE_SAI3_RX,
    IMX_DMATYPE_SAI3_TX,
    IMX_DMATYPE_SPDIF_RX,
    IMX_DMATYPE_SPDIF_TX,
    IMX_DMATYPE_RESERVED_87,
    IMX_DMATYPE_ADC2,
    IMX_DMATYPE_ACMP3,
    IMX_DMATYPE_ACMP4,
    IMX_DMATYPE_RESERVED,
    IMX_DMATYPE_ENET_TIMER_0,
    IMX_DMATYPE_ENET_TIMER_1,
    IMX_DMATYPE_XBAR1_2,
    IMX_DMATYPE_XBAR1_3,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE0_READ,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE1_READ,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE2_READ,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE3_READ,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE0_WRITE,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE1_WRITE,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE2_WRITE,
    IMX_DMATYPE_FLEXPWM2_SUB_MODULE3_WRITE,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE0_READ,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE1_READ,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE2_READ,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE3_READ,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE0_WRITE,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE1_WRITE,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE2_WRITE,
    IMX_DMATYPE_FLEXPWM4_SUB_MODULE3_WRITE,
    IMX_DMATYPE_QTIMER2_CAPT_0_READ,
    IMX_DMATYPE_QTIMER2_CAPT_1_READ,
    IMX_DMATYPE_QTIMER2_CAPT_2_READ,
    IMX_DMATYPE_QTIMER2_CAPT_3_READ,
    IMX_DMATYPE_QTIMER2_CMPLD1_0_OR_CMALD2_1_WRITE,
    IMX_DMATYPE_QTIMER2_CMPLD1_1_OR_CMPLD2_0_WRITE,
    IMX_DMATYPE_QTIMER2_CMPLD1_2_OR_CMPLD2_3_WRITE,
    IMX_DMATYPE_QTIMER2_CMPLD1_3_OR_CMPLD2_2_WRITE,
    IMX_DMATYPE_QTIMER4_CAPT_0_READ_OR_CMPLD1_0_CMPLD2_1_WRITE,
    IMX_DMATYPE_QTIMER4_CAPT_1_READ_OR_CMPLD1_1_CMPLD2_0_WRITE,
    IMX_DMATYPE_QTIMER4_CAPT_2_READ_OR_CMPLD1_2_CMPLD2_3_WRITE,
    IMX_DMATYPE_QTIMER4_CAPT_3_READ_OR_CMPLD1_3_CMPLD2_2_WRITE,
    IMX_DMATYPE_MEM
};

/* 通道优先级  */
enum imx_dma_prio {
    DMA_PRIO_0 = 0,
    DMA_PRIO_1 = 1,
    DMA_PRIO_2 = 2,
    DMA_PRIO_3 = 3,
    DMA_PRIO_4 = 4,
    DMA_PRIO_5 = 5,
    DMA_PRIO_6 = 6,
    DMA_PRIO_7 = 7,
    DMA_PRIO_8 = 8,
    DMA_PRIO_9 = 9,
    DMA_PRIO_10 = 10,
    DMA_PRIO_11 = 11,
    DMA_PRIO_12 = 12,
    DMA_PRIO_13 = 13,
    DMA_PRIO_14 = 14,
    DMA_PRIO_15 = 15,
    DMA_PRIO_ANY            /* 0 - 15 中任意的优先级*/
};

struct imx_dma_data {
    int                         dma_request; /* DMA request line */
    int                         dma_request2; /* secondary DMA request line */
    enum edma_peripheral_type   peripheral_type;
    enum imx_dma_prio           priority;
};

struct scatterlist {
    unsigned int    length;
    dma_addr_t      dma_address;
};

/**
 * \brief initialize AWBus imx1010 sdma service
 *
 * \attention the function should be called
 * after awbl_dev_init1() and before awbl_dev_init2()
 */
void awbl_imx1010_edma_init (void);

typedef void (*pfn_dma_callback_t)(void *param,unsigned int len);

void * imx1010_edma_alloc_chan(struct imx_dma_data *data);
int imx1010_edma_control(void *chan, enum dma_ctrl_cmd cmd,unsigned long arg);

int imx1010_edma_prep_memcpy(void *chan,
        dma_addr_t dma_dst,
        dma_addr_t dma_src,
        size_t len,
        unsigned long flags);

int imx1010_edma_prep_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

/**
 * \brief   为一个通道准备一个分散/聚集传输
 *
 * \param[in] chan       通道指针
 * \param[in] dst_sg     分散/聚集传输块目标首地址
 * \param[in] dst_count  分散/聚集传输块目标数目
 * \param[in] src_sg     分散/聚集传输块源首地址
 * \param[in] src_count  分散/聚集传输块源数目
 * \param[in] flags      标志(未使用到这个参数，传0即可)
 *
 * \return    AW_OK     :   操作成功
 * \return    -ENODEV   :   设备不存在
 * \return    -EINVAL   :   无效参数
 * \return    -ENOMEM   :   空间（内存）不足
 *
 */
int imx1010_edma_prep_memcpy_sg(void *chan,
        struct scatterlist *dst_sg,
        unsigned int dst_count,
        struct scatterlist *src_sg,
        unsigned int src_count,
        unsigned long flags);

int imx1010_edma_prep_slave_sg(void *chan,
        struct scatterlist *src_sg,
        unsigned int src_count,
        enum dma_transfer_direction direction);

/**
 * \brief   为一个通道准备一个循环传输
 *
 * \param[in] chan       通道指针
 * \param[in] dma_addr   内存缓冲区首地址
 * \param[in] buf_len    内存缓冲区大小
 * \param[in] period_len 一次传输的大小
 * \param[in] direction  传输方向
 *
 * \return    AW_OK     :   操作成功
 * \return    -ENODEV   :   设备不存在
 * \return    -EINVAL   :   无效参数
 *
 * \note: dma_addr，这个地址必需按内存缓冲区大小（buf_len）对齐，例如：内存缓冲区大小为128字节，
 *        dma_addr就必需按128字节对齐。
 * \note: buf_len，缓冲区大小只能为2的幂次方（范围为：2^1 ~ 2^31），如：2，4，8，16，……，0x80000000
 *
 */
int imx1010_edma_prep_dma_cyclic(void *chan,
        dma_addr_t dma_addr, size_t buf_len,
        size_t period_len, enum dma_transfer_direction direction);

int imx1010_edma_chan_start_sync(void *chan,uint32_t timeout);
int imx1010_edma_chan_start_async(void *chan,
        pfn_dma_callback_t callback,void *param);

int imx1010_edma_get_transfer_count(void *chan);
int imx1010_edma_chan_stop(void *chan);
int imx1010_edma_free_chan(void *chan);


#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_ADC_H */

/* end of file */

