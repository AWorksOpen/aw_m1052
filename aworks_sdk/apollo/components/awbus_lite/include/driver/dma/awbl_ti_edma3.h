/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus Ti EDMA3 驱动头文件
 *
 * EDMA3 为 Ti DSP 中常见的 DMA 控制器
 *
 * \par 1.驱动信息
 *
 *  - 驱动名:   "ti_edma3"
 *  - 总线类型: AWBL_BUSID_PLB
 *  - 设备信息: struct awbl_ti_edma3_devinfo
 *
 * \par 2.兼容设备
 *
 *  - Ti C674X 系列MCU
 *  - 其它与C674X EDMA3 兼容的设备
 *
 * \par 3.设备定义/配置
 *
 *  - \ref grp_awbl_ti_edma3_hwconf
 *
 * \par 4.用户接口
 *
 *  - \ref grp_aw_serv_dma
 *  - \ref grp_aw_serv_edma3
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-30  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_TI_EDMA3_H
#define __AWBL_TI_EDMA3_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_drv_ti_edma3
 * \copydetails awbl_ti_edma3.h
 * @{
 */

/**
 * \defgroup  grp_awbl_ti_edma3_hwconf 设备定义/配置
 * \todo:
 */
/** @} grp_awbl_drv_ti_edma3 */

#include "aw_spinlock.h"
#include "aw_sem.h"
#include "aw_ti_edma3.h"
#include "driver/dma/awbl_ti_edma3_private.h"

#define AWBL_TI_EDMA3_NAME   "ti_edma3"

#define AW_EDMA3_DMA_CHAN_ANY   0
#define AW_EDMA3_QDMA_CHAN_ANY  0
#define AW_EDMA3_LINK_CHAN_ANY  0

/**
 * \brief  TCC 回调函数
 *
 * \param tcc       TCC 号
 * \param status    传输状态
 * \param cb_data   传递给回调函数的参数(注册回调函数时设置)
 */
typedef void (* awbl_ti_edma3_tcc_callback_t)(uint32_t                 tcc,
                                              aw_ti_edma3_tcc_status_t status,
                                              void                    *cb_data);

/**
 * \brief TCC Callback - Caters to channel specific status reporting.
 */
typedef struct {
    /** Callback function */
    awbl_ti_edma3_tcc_callback_t tcc_cb;

    /** Callback data, passed to the Callback function */
    void *p_data;
} awbl_ti_edma3_tcc_callback_params_t;

/**
 * \brief EDMA3 通道资源
 *
 * 用于维护EDMA3通道相关的资源信息(参数数集和TCC)以及触发传输模式(手动、硬件事件
 * 等等)
 */
typedef struct awbl_ti_edma3_chan_resource {

    /** \brief 通道相关联的参数集号 */
    int param_set_id;

    /** \brief 通道相关联的TCC */
    unsigned int tcc;

    /** \brief 通道的触发模式 */
    aw_ti_edma3_trigger_mode_t trig_mode;

} awbl_ti_edma3_chan_resource_t;


/**
 * \brief EDMA3控制器的初始化配置，提供全局的SoC定义信息
 *
 * 此配置结构体用于指定EDMA3控制器的全局设置，与具体SoC相关。例如，DMA/QMA通道，
 * PaRAM 数量，TCC数量，事件队列数，中断传输完成中断号，CC错误，事件队列优先级，
 * 水印门限(watermark threshold level) 等待。
 */
typedef struct awbl_ti_edma3_gblcfg_params {

    /** 
     * \brief 在多主机系统中(e.g. ARM + DSP)，本选项用于区分主机和从机。
     *        只有主机被允许编程全局寄存器 (例如，队列优先级、队列水印门限、
     *        错误寄存器等等)
     */
    /* aw_bool_t                is_master; */

    /** \brief EDMA3 控制器支持的DMA通道数 */
    unsigned int        dma_chan_num;

    /** \brief EDMA3 控制器支持的QDMA通道数 */
    unsigned int        qdma_chan_num;

    /** \brief EDMA3 控制器支持的中断通道数 */
    unsigned int        tcc_num;

    /** \brief EDMA3 控制器支持的PaRAM数量 */
    unsigned int        param_set_num;

    /** \brief EDMA3 控制器支持的事件队列数量 */
    unsigned int        evt_queue_num;

    /** \brief EDMA3 控制器支持的传输控制器数量 */
    unsigned int        tc_num;

    /** \brief EDMA3 控制器支持的域(region)数量 */
    unsigned int        region_num;

    /**
     * \brief 通道映射是否存在
     *
     * false    无通道映射，DMA通道和PaRAM之间是固定联系，也就是说，DMA通道n只能
     *          使用 PaRAM n 用于传输;
     *
     * true     任意DMA/QMDA 通道可以使用任意 PaRAM 集。
     */
    aw_bool_t                dma_ch_param_map_exists;

    /** \brief 内存保存特性是否存在 */
    aw_bool_t                mem_protection_exists;

    /** \brief EDMA3 CC 内存映射寄存器基地址 */
    void               *global_regs_base;

    /** \brief EDMA3 TC 内存映射基地址(每个TC一个) */
    void               *tc_regs_base[EDMA3_MAX_TC];

    /** \brief EDMA3 传输完成中断号 */
    int                 inum_xfer_complete;

    /** \brief EDMA CC 错误中断号 (ARM 和 DSP 可能是不同的) */
    int                 inum_cc_err;

    /** EDMA3 TC 错误中断号 (每个TC一个) (ARM 和 DSP 可能是不同的) */
    int                 inum_tc_err[EDMA3_MAX_TC];

    /**
     * \brief EDMA3 TC 优先级设置
     *
     * 用户可以设置系统范围内的事件队列优先级。这意味着用户可以设置由TC
     * (传输控制器)发起的或其它设备(ARM,DSP,USB,etc)的总线主机发起的IO。
     */
    unsigned int        evtq_pri[EDMA3_MAX_EVT_QUE];

    /**
     * \brief 事件队列水印门限
     *
     * 配置可以排队到队列的时间数门限。EDMA3CC 错误寄存器(CCERR)将会指示排队到
     * 队列中的事件是否超过或等于门限，或者没有事件。
     */
    unsigned int        evtq_water_mark_lvl[EDMA3_MAX_EVT_QUE];

    /**
     * \brief TC 的默认突发大小(Default Burst Size, DBS)
     *
     * 不同的TC可以有不同的DBS，字节单位
     */
    unsigned int        tc_dbs[EDMA3_MAX_TC];

    /**
     * \brief DMA 通道与PaRAM的映射
     *
     * 如果通道映射存在，本数组存放每个DMA通道映射的PaRAM。用户可以使用特定的
     * PaRAM 集或 AWBL_TI_EDMA3_CH_NO_PARAM_MAP 来初始化每个数组成员。
     *
     * 如果通道映射不存在，此数组无效。只有映射存在时，此数组有效。
     */
    unsigned int        dma_chan_param_map[EDMA3_MAX_DMA_CH];

    /**
     * \brief DMA通道与TCC的映射
     *
     * 本数组存放每个DMA通道对应的TCC(中断通道)。用户使用指定的TCC或
     * AWBL_TI_EDMA3_CH_NO_TCC_MAP 来初始化每个成员。当映射的DMA通道传输完成时，
     * TCC编码将会返回。
     */
    unsigned int       dma_chan_tcc_map[EDMA3_MAX_DMA_CH];

    /**
     * \brief DMA通道与硬件事件的映射
     *
     * 此数组中的每个 bit 对应1个DMA通道，表示此DMA通道是否绑定到任何外设。决定
     * 了任何外设是否可以发送同步事件到此DMA通道。
     *
     * 1 表示通道被绑定到某个外设上；0 则表示没有。
     *
     * 被绑定到某个外设上的DMA通道被保留为只供此外设使用。当用户请求"ANY"DMA通道
     * 时，此通道不会被分配。
     */
    uint32_t            dma_chan_hw_evt_map[EDMA3_MAX_DMA_CHAN_DWRDS];
} awbl_ti_edma3_gblcfg_params_t;


/**
 * \brief 域相关配置信息
 *
 * 本配置结构体用于指定哪些 EDMA3 资源由此设备实例拥有和保留。本配置结构体由影子
 * 域(shadow region)定义，当用户请求此设备实例时提供这些资源。
 *
 * 拥有的资源:
 * ****************
 *
 * EDMA3 设备实例被绑定到不同的影子域以及主机。域可以是:
 *
 * a) ARM,
 * b) DSP,
 * c) IMCOP (Imaging Co-processor) etc.
 *
 * 用户可以使用此结构体将每个EDMA3资源分配到一个影子域上。用户指定此 EDMA3 设备
 * 实例拥有那些资源。
 *
 * 这里的分配应该同样保证相同的资源不能被分配给多个影子域(除非你希望如此)。任何
 * 不按这里描述的分配可能会导致灾难性的后果。
 *
 * 保留的资源:
 * *******************
 *
 * 在 EDMA3 设备实例初始化时，通过指定哪些资源被保留，用户可以保留某些EDMA3资源
 * 以作将来之用。
 *
 * 用户可以使用以下两种方式来请求不同的EDMA3资源
 *
 * a) 传递资源类型和实际的资源ID
 * b) 传递资源类型和 ANY 作为资源ID
 *
 * e.g. 为了请求 DMA 通道31，用户需要传递31作为资源ID 。但是请求DMA上的任意的任
 * 意通道(主要用于内存到内存的传输)，用户需要传递 AW_DMA_CHAN_ANY 作为资源ID 。
 *
 * 在初始化的时候，用户可以保留某些DMA通道用作特定目的(主要是外设使用EDMA)。
 * 用户使用 ANY 作为资源ID 来请求通道时，这些被保留的DMA通道将不会返回。
 *
 * 以上描述的逻辑同样适用于 QDMA 通道和 TCC。
 *
 * 对于 PaRAM 集，存在一个不同的地方。如果 DMA 通道是1对1地绑定到 PaRAM (用户不
 * 能为DMA通道选择特定的 PaRAM)，EDMA3 驱动自动保留所有这些被绑定的 PaRAM 。
 * 当用户使用 ANY 来请求PaRAM时(用于链接(linking)的目的)，这些 PaRAM不会被返回。
 * 如果此限制不存在，DMA通道可能根本不能被使用，因为期望的 PaRAM 可能不可用。
 */
typedef struct awbl_ti_edma3_rgncfg_params {

    /**
     * \brief 此设备实例是否为主控, 只有与主控实例相关联的影子区域(region)将会
     * 接收到 EDMA3 中断
     */
    /* aw_bool_t        is_master; */

    /** \brief 事件队列号 (0, 1) */
    /* unsigned    queue_num;*/

    /** \brief 此设备实例拥有的 PaRAM 集，每个bit对应1个 PaRAM */
    unsigned int        own_param_sets[EDMA3_MAX_PARAM_DWRDS];

    /** \brief 此设备实例拥有的 DMA 通道 */
    unsigned int        own_dma_channels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /** \brief 此设备实例拥有的 QDMA 通道 */
    unsigned int        own_qdma_channels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /** \brief 此设备实例拥有的 */
    unsigned int        own_tccs[EDMA3_MAX_TCC_DWRDS];

    /**
     * \brief  保留的 PaRAM Sets  Reserved PaRAM Sets
     *
     * 被保留的 PaRAM Sets 以作将来之用。当用户请求 ANY 通道时，这些PaRAM Sets
     * 将不会被分配。
     */
    unsigned int        resvd_param_sets[EDMA3_MAX_PARAM_DWRDS];

    /**
     * \brief 保留的DMA通道
     *
     * 被保留的 DMA 通道以作将来之用。当用户请求 ANY DMA通道时，这些通道将不会被
     * 分配
     */
    unsigned int        resvd_dma_channels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /**
     * \brief 保留的QDMA通道
     *
     * 被保留的 QDMA 通道以作将来之用。当用户请求 ANY QDMA通道时，这些通道将不会
     * 被分配
     */
    unsigned int        resvd_qdma_channels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /**
     * \brief 保留的 TCCs
     *
     * 被保留的 TCC 以作将来之用。当用户请求 ANY TCC 时，这些通道将不会被
     * 分配
     */
    unsigned int        resvd_tccs[EDMA3_MAX_TCC_DWRDS];
} awbl_ti_edma3_rgncfg_params_t;


/**
 * \brief 域对象
 */
typedef struct awbl_ti_edma3_region {

    /** \brief 域ID */
    unsigned    region_id;

    /** \brief 是否是Master */
    aw_bool_t        is_master;

    /** \brief 域保护锁 */
    AW_MUTEX_DECL(lock);

    /** \brief 域保护锁ID */
    aw_mutex_id_t lock_id;

    /** \brief 对应的 Shadow Register region of CC Registers */
    volatile __edma3_ccrl_shadow_regs_t *p_shadow_regs;

    /** \brief 全局的SoC定义 */
    awbl_ti_edma3_gblcfg_params_t *p_gblcfg_params;

    /** \brief 域初始化参数 */
    awbl_ti_edma3_rgncfg_params_t rgncfg_params;

    /** \brief Available DMA Channels to the RM Instance */
    uint32_t        avlbl_dma_channels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /** \brief Available QDMA Channels to the RM Instance */
    uint32_t        avlbl_qdma_channels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /** \brief Available PaRAM Sets to the RM Instance */
    uint32_t        avlbl_param_sets[EDMA3_MAX_PARAM_DWRDS];

    /** \brief Available TCCs to the RM Instance */
    uint32_t        avlbl_tccs[EDMA3_MAX_TCC_DWRDS];

} awbl_ti_edma3_region_t;


/**
 * \brief Ti EDMA3 设备实例
 */
typedef struct awbl_ti_edma3_devinfo {

    /** \brief 全局的SoC定义 */
    aw_const awbl_ti_edma3_gblcfg_params_t *p_gblcfg_params;

    /** \brief 域配置 (成员个数 = region_num) */
    aw_const awbl_ti_edma3_rgncfg_params_t *p_rgncfg_params;

    /** \brief 域对象内存静态分配 (成员个数 = region_num) */
    awbl_ti_edma3_region_t        *p_regions;

    /** \brief 域个数 */
    size_t  region_num;

    /** \brief 决定当前CPU使用哪个域(作为 Master域) */
    int (* pfunc_region_decide) (void);

    /** \brief 当前CPU是否为 Master (只有Master才能初始化全局寄存器) */
    aw_bool_t (* pfunc_is_master) (void);

} awbl_ti_edma3_devinfo_t;

typedef struct awbl_ti_edma3_tcerr_handler_arg {
    struct awbl_ti_edma3_dev *p_this;
    uint32_t                     tcc_no;
} awbl_ti_edma3_tcerr_handler_arg_t;

/**
 * \brief Ti EDMA3 设备实例
 */
typedef struct awbl_ti_edma3_dev {

    /** \brief 继承自 AWBus 设备 */
    struct awbl_dev super;

    /** \brief 域个数 */
    unsigned region_num;

    /** \brief 指向 Region 对象 */
    awbl_ti_edma3_region_t *p_regions;

    /** \brief 主 region */
    awbl_ti_edma3_region_t *p_master_region;

    /** \brief EDMA3 全局配置 */
    struct awbl_ti_edma3_gblcfg_params   gblcfg;

    size_t param_sets_num;

    /** \brief Min of DMA Channels */
    size_t dma_chan_min_val;

    /** \brief Max of DMA Channels */
    size_t dma_chan_max_val;

    /** \brief Min of Link Channels */
    size_t link_chan_min_val;

    /** \brief Max of Link Channels */
    size_t link_chan_max_val;

    /** \brief Min of QDMA Channels */
    size_t qdma_chan_min_val;

    /** \brief Max of QDMA Channels */
    size_t qdma_chan_max_val;

    /** \brief Max of Logical Channels */
    size_t log_chan_max_val;

    /**
     * mapping between DMA channels and Interrupt channels i.e. TCCs.
     * DMA channel X can use any TCC Y. Transfer completion
     * interrupt will occur on the TCC Y (IPR/IPRH Register, bit Y), but error
     * interrupt will occur on DMA channel X (EMR/EMRH register, bit X). In that
     * scenario, this DMA channel <-> TCC mapping will be used to point to
     * the correct callback function.
     */
    uint32_t dma_chan_tcc_map[EDMA3_MAX_DMA_CH];

    /**
     * mapping between QDMA channels and Interrupt channels i.e. TCCs.
     * QDMA channel X can use any TCC Y. Transfer completion
     * interrupt will occur on the TCC Y (IPR/IPRH Register, bit Y), but error
     * interrupt will occur on QDMA channel X (QEMR register, bit X). In that
     * scenario, this QDMA channel <-> TCC mapping will be used to point to
     * the correct callback function.
     */
    uint32_t qdma_chan_tcc_map[EDMA3_MAX_QDMA_CH];


    /**
     * \brief Resources bound to a Channel
     *
     * When a request for a channel is made, the resources PaRAM Set and TCC
     * get bound to that channel. This information is needed internally by the
     * resource manager, when a request is made to free the channel,
     * to free up the channel-associated resources.
     */
    awbl_ti_edma3_chan_resource_t    chan_res[EDMA3_MAX_LOGICAL_CH];

    /**
     * The list of Interrupt Channels which get allocated while requesting the
     * TCC. It will be used while checking the IPR/IPRH bits in the RM ISR.
     */
    uint32_t allocated_tccs[2u];


    awbl_ti_edma3_tcc_callback_params_t intr_params [EDMA3_MAX_TCC];


    awbl_ti_edma3_tcerr_handler_arg_t tcerr_handler_arg[EDMA3_MAX_TC];

    struct awbl_ti_edma3_dev *p_next;

} awbl_ti_edma3_dev_t;

/**
 * \brief Ti EDMA3 driver register
 */
void awbl_ti_edma3_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TI_EDMA3_H */

/* end of file */

