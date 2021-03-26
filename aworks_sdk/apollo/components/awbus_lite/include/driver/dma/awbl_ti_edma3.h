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
 * \brief AWBus Ti EDMA3 ����ͷ�ļ�
 *
 * EDMA3 Ϊ Ti DSP �г����� DMA ������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "ti_edma3"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct awbl_ti_edma3_devinfo
 *
 * \par 2.�����豸
 *
 *  - Ti C674X ϵ��MCU
 *  - ������C674X EDMA3 ���ݵ��豸
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_ti_edma3_hwconf
 *
 * \par 4.�û��ӿ�
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
 * \defgroup  grp_awbl_ti_edma3_hwconf �豸����/����
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
 * \brief  TCC �ص�����
 *
 * \param tcc       TCC ��
 * \param status    ����״̬
 * \param cb_data   ���ݸ��ص������Ĳ���(ע��ص�����ʱ����)
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
 * \brief EDMA3 ͨ����Դ
 *
 * ����ά��EDMA3ͨ����ص���Դ��Ϣ(����������TCC)�Լ���������ģʽ(�ֶ���Ӳ���¼�
 * �ȵ�)
 */
typedef struct awbl_ti_edma3_chan_resource {

    /** \brief ͨ��������Ĳ������� */
    int param_set_id;

    /** \brief ͨ���������TCC */
    unsigned int tcc;

    /** \brief ͨ���Ĵ���ģʽ */
    aw_ti_edma3_trigger_mode_t trig_mode;

} awbl_ti_edma3_chan_resource_t;


/**
 * \brief EDMA3�������ĳ�ʼ�����ã��ṩȫ�ֵ�SoC������Ϣ
 *
 * �����ýṹ������ָ��EDMA3��������ȫ�����ã������SoC��ء����磬DMA/QMAͨ����
 * PaRAM ������TCC�������¼����������жϴ�������жϺţ�CC�����¼��������ȼ���
 * ˮӡ����(watermark threshold level) �ȴ���
 */
typedef struct awbl_ti_edma3_gblcfg_params {

    /** 
     * \brief �ڶ�����ϵͳ��(e.g. ARM + DSP)����ѡ���������������ʹӻ���
     *        ֻ��������������ȫ�ּĴ��� (���磬�������ȼ�������ˮӡ���ޡ�
     *        ����Ĵ����ȵ�)
     */
    /* aw_bool_t                is_master; */

    /** \brief EDMA3 ������֧�ֵ�DMAͨ���� */
    unsigned int        dma_chan_num;

    /** \brief EDMA3 ������֧�ֵ�QDMAͨ���� */
    unsigned int        qdma_chan_num;

    /** \brief EDMA3 ������֧�ֵ��ж�ͨ���� */
    unsigned int        tcc_num;

    /** \brief EDMA3 ������֧�ֵ�PaRAM���� */
    unsigned int        param_set_num;

    /** \brief EDMA3 ������֧�ֵ��¼��������� */
    unsigned int        evt_queue_num;

    /** \brief EDMA3 ������֧�ֵĴ������������ */
    unsigned int        tc_num;

    /** \brief EDMA3 ������֧�ֵ���(region)���� */
    unsigned int        region_num;

    /**
     * \brief ͨ��ӳ���Ƿ����
     *
     * false    ��ͨ��ӳ�䣬DMAͨ����PaRAM֮���ǹ̶���ϵ��Ҳ����˵��DMAͨ��nֻ��
     *          ʹ�� PaRAM n ���ڴ���;
     *
     * true     ����DMA/QMDA ͨ������ʹ������ PaRAM ����
     */
    aw_bool_t                dma_ch_param_map_exists;

    /** \brief �ڴ汣�������Ƿ���� */
    aw_bool_t                mem_protection_exists;

    /** \brief EDMA3 CC �ڴ�ӳ��Ĵ�������ַ */
    void               *global_regs_base;

    /** \brief EDMA3 TC �ڴ�ӳ�����ַ(ÿ��TCһ��) */
    void               *tc_regs_base[EDMA3_MAX_TC];

    /** \brief EDMA3 ��������жϺ� */
    int                 inum_xfer_complete;

    /** \brief EDMA CC �����жϺ� (ARM �� DSP �����ǲ�ͬ��) */
    int                 inum_cc_err;

    /** EDMA3 TC �����жϺ� (ÿ��TCһ��) (ARM �� DSP �����ǲ�ͬ��) */
    int                 inum_tc_err[EDMA3_MAX_TC];

    /**
     * \brief EDMA3 TC ���ȼ�����
     *
     * �û���������ϵͳ��Χ�ڵ��¼��������ȼ�������ζ���û�����������TC
     * (���������)����Ļ������豸(ARM,DSP,USB,etc)���������������IO��
     */
    unsigned int        evtq_pri[EDMA3_MAX_EVT_QUE];

    /**
     * \brief �¼�����ˮӡ����
     *
     * ���ÿ����Ŷӵ����е�ʱ�������ޡ�EDMA3CC ����Ĵ���(CCERR)����ָʾ�Ŷӵ�
     * �����е��¼��Ƿ񳬹���������ޣ�����û���¼���
     */
    unsigned int        evtq_water_mark_lvl[EDMA3_MAX_EVT_QUE];

    /**
     * \brief TC ��Ĭ��ͻ����С(Default Burst Size, DBS)
     *
     * ��ͬ��TC�����в�ͬ��DBS���ֽڵ�λ
     */
    unsigned int        tc_dbs[EDMA3_MAX_TC];

    /**
     * \brief DMA ͨ����PaRAM��ӳ��
     *
     * ���ͨ��ӳ����ڣ���������ÿ��DMAͨ��ӳ���PaRAM���û�����ʹ���ض���
     * PaRAM ���� AWBL_TI_EDMA3_CH_NO_PARAM_MAP ����ʼ��ÿ�������Ա��
     *
     * ���ͨ��ӳ�䲻���ڣ���������Ч��ֻ��ӳ�����ʱ����������Ч��
     */
    unsigned int        dma_chan_param_map[EDMA3_MAX_DMA_CH];

    /**
     * \brief DMAͨ����TCC��ӳ��
     *
     * ��������ÿ��DMAͨ����Ӧ��TCC(�ж�ͨ��)���û�ʹ��ָ����TCC��
     * AWBL_TI_EDMA3_CH_NO_TCC_MAP ����ʼ��ÿ����Ա����ӳ���DMAͨ���������ʱ��
     * TCC���뽫�᷵�ء�
     */
    unsigned int       dma_chan_tcc_map[EDMA3_MAX_DMA_CH];

    /**
     * \brief DMAͨ����Ӳ���¼���ӳ��
     *
     * �������е�ÿ�� bit ��Ӧ1��DMAͨ������ʾ��DMAͨ���Ƿ�󶨵��κ����衣����
     * ���κ������Ƿ���Է���ͬ���¼�����DMAͨ����
     *
     * 1 ��ʾͨ�����󶨵�ĳ�������ϣ�0 ���ʾû�С�
     *
     * ���󶨵�ĳ�������ϵ�DMAͨ��������Ϊֻ��������ʹ�á����û�����"ANY"DMAͨ��
     * ʱ����ͨ�����ᱻ���䡣
     */
    uint32_t            dma_chan_hw_evt_map[EDMA3_MAX_DMA_CHAN_DWRDS];
} awbl_ti_edma3_gblcfg_params_t;


/**
 * \brief �����������Ϣ
 *
 * �����ýṹ������ָ����Щ EDMA3 ��Դ�ɴ��豸ʵ��ӵ�кͱ����������ýṹ����Ӱ��
 * ��(shadow region)���壬���û�������豸ʵ��ʱ�ṩ��Щ��Դ��
 *
 * ӵ�е���Դ:
 * ****************
 *
 * EDMA3 �豸ʵ�����󶨵���ͬ��Ӱ�����Լ��������������:
 *
 * a) ARM,
 * b) DSP,
 * c) IMCOP (Imaging Co-processor) etc.
 *
 * �û�����ʹ�ô˽ṹ�彫ÿ��EDMA3��Դ���䵽һ��Ӱ�����ϡ��û�ָ���� EDMA3 �豸
 * ʵ��ӵ����Щ��Դ��
 *
 * ����ķ���Ӧ��ͬ����֤��ͬ����Դ���ܱ���������Ӱ����(������ϣ�����)���κ�
 * �������������ķ�����ܻᵼ�������Եĺ����
 *
 * ��������Դ:
 * *******************
 *
 * �� EDMA3 �豸ʵ����ʼ��ʱ��ͨ��ָ����Щ��Դ���������û����Ա���ĳЩEDMA3��Դ
 * ��������֮�á�
 *
 * �û�����ʹ���������ַ�ʽ������ͬ��EDMA3��Դ
 *
 * a) ������Դ���ͺ�ʵ�ʵ���ԴID
 * b) ������Դ���ͺ� ANY ��Ϊ��ԴID
 *
 * e.g. Ϊ������ DMA ͨ��31���û���Ҫ����31��Ϊ��ԴID ����������DMA�ϵ��������
 * ��ͨ��(��Ҫ�����ڴ浽�ڴ�Ĵ���)���û���Ҫ���� AW_DMA_CHAN_ANY ��Ϊ��ԴID ��
 *
 * �ڳ�ʼ����ʱ���û����Ա���ĳЩDMAͨ�������ض�Ŀ��(��Ҫ������ʹ��EDMA)��
 * �û�ʹ�� ANY ��Ϊ��ԴID ������ͨ��ʱ����Щ��������DMAͨ�������᷵�ء�
 *
 * �����������߼�ͬ�������� QDMA ͨ���� TCC��
 *
 * ���� PaRAM ��������һ����ͬ�ĵط������ DMA ͨ����1��1�ذ󶨵� PaRAM (�û���
 * ��ΪDMAͨ��ѡ���ض��� PaRAM)��EDMA3 �����Զ�����������Щ���󶨵� PaRAM ��
 * ���û�ʹ�� ANY ������PaRAMʱ(��������(linking)��Ŀ��)����Щ PaRAM���ᱻ���ء�
 * ��������Ʋ����ڣ�DMAͨ�����ܸ������ܱ�ʹ�ã���Ϊ������ PaRAM ���ܲ����á�
 */
typedef struct awbl_ti_edma3_rgncfg_params {

    /**
     * \brief ���豸ʵ���Ƿ�Ϊ����, ֻ��������ʵ���������Ӱ������(region)����
     * ���յ� EDMA3 �ж�
     */
    /* aw_bool_t        is_master; */

    /** \brief �¼����к� (0, 1) */
    /* unsigned    queue_num;*/

    /** \brief ���豸ʵ��ӵ�е� PaRAM ����ÿ��bit��Ӧ1�� PaRAM */
    unsigned int        own_param_sets[EDMA3_MAX_PARAM_DWRDS];

    /** \brief ���豸ʵ��ӵ�е� DMA ͨ�� */
    unsigned int        own_dma_channels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /** \brief ���豸ʵ��ӵ�е� QDMA ͨ�� */
    unsigned int        own_qdma_channels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /** \brief ���豸ʵ��ӵ�е� */
    unsigned int        own_tccs[EDMA3_MAX_TCC_DWRDS];

    /**
     * \brief  ������ PaRAM Sets  Reserved PaRAM Sets
     *
     * �������� PaRAM Sets ��������֮�á����û����� ANY ͨ��ʱ����ЩPaRAM Sets
     * �����ᱻ���䡣
     */
    unsigned int        resvd_param_sets[EDMA3_MAX_PARAM_DWRDS];

    /**
     * \brief ������DMAͨ��
     *
     * �������� DMA ͨ����������֮�á����û����� ANY DMAͨ��ʱ����Щͨ�������ᱻ
     * ����
     */
    unsigned int        resvd_dma_channels[EDMA3_MAX_DMA_CHAN_DWRDS];

    /**
     * \brief ������QDMAͨ��
     *
     * �������� QDMA ͨ����������֮�á����û����� ANY QDMAͨ��ʱ����Щͨ��������
     * ������
     */
    unsigned int        resvd_qdma_channels[EDMA3_MAX_QDMA_CHAN_DWRDS];

    /**
     * \brief ������ TCCs
     *
     * �������� TCC ��������֮�á����û����� ANY TCC ʱ����Щͨ�������ᱻ
     * ����
     */
    unsigned int        resvd_tccs[EDMA3_MAX_TCC_DWRDS];
} awbl_ti_edma3_rgncfg_params_t;


/**
 * \brief �����
 */
typedef struct awbl_ti_edma3_region {

    /** \brief ��ID */
    unsigned    region_id;

    /** \brief �Ƿ���Master */
    aw_bool_t        is_master;

    /** \brief �򱣻��� */
    AW_MUTEX_DECL(lock);

    /** \brief �򱣻���ID */
    aw_mutex_id_t lock_id;

    /** \brief ��Ӧ�� Shadow Register region of CC Registers */
    volatile __edma3_ccrl_shadow_regs_t *p_shadow_regs;

    /** \brief ȫ�ֵ�SoC���� */
    awbl_ti_edma3_gblcfg_params_t *p_gblcfg_params;

    /** \brief ���ʼ������ */
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
 * \brief Ti EDMA3 �豸ʵ��
 */
typedef struct awbl_ti_edma3_devinfo {

    /** \brief ȫ�ֵ�SoC���� */
    aw_const awbl_ti_edma3_gblcfg_params_t *p_gblcfg_params;

    /** \brief ������ (��Ա���� = region_num) */
    aw_const awbl_ti_edma3_rgncfg_params_t *p_rgncfg_params;

    /** \brief ������ڴ澲̬���� (��Ա���� = region_num) */
    awbl_ti_edma3_region_t        *p_regions;

    /** \brief ����� */
    size_t  region_num;

    /** \brief ������ǰCPUʹ���ĸ���(��Ϊ Master��) */
    int (* pfunc_region_decide) (void);

    /** \brief ��ǰCPU�Ƿ�Ϊ Master (ֻ��Master���ܳ�ʼ��ȫ�ּĴ���) */
    aw_bool_t (* pfunc_is_master) (void);

} awbl_ti_edma3_devinfo_t;

typedef struct awbl_ti_edma3_tcerr_handler_arg {
    struct awbl_ti_edma3_dev *p_this;
    uint32_t                     tcc_no;
} awbl_ti_edma3_tcerr_handler_arg_t;

/**
 * \brief Ti EDMA3 �豸ʵ��
 */
typedef struct awbl_ti_edma3_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev super;

    /** \brief ����� */
    unsigned region_num;

    /** \brief ָ�� Region ���� */
    awbl_ti_edma3_region_t *p_regions;

    /** \brief �� region */
    awbl_ti_edma3_region_t *p_master_region;

    /** \brief EDMA3 ȫ������ */
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

