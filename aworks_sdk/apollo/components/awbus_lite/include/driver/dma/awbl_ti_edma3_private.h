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
 * \brief AWBus Ti EDMA3 driver private file
 *
 * \internal
 * \par modification history:
 * - 1.00 13-09-30  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_TI_EDMA3_PRIVATE_H
#define __AWBL_TI_EDMA3_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*******************************************************************************
  register overlay structure for dra
*******************************************************************************/
typedef struct __edma3_ccrl_draregs {
    volatile uint32_t drae;
    volatile uint32_t draeh;
} __edma3_ccrl_dra_regs_t;

/*******************************************************************************
  register overlay structure for queevtentry
*******************************************************************************/
typedef struct __edma3_ccrl_queevtentry_regs {
    volatile uint32_t queevt_entry;
} __edma3_ccrl_queevtentry_regs_t;

/*******************************************************************************
  register overlay structure for shadow
*******************************************************************************/
typedef struct __edma3_ccrl_shadow_regs {
    volatile uint32_t er;
    volatile uint32_t erh;
    volatile uint32_t ecr;
    volatile uint32_t ecrh;
    volatile uint32_t esr;
    volatile uint32_t esrh;
    volatile uint32_t cer;
    volatile uint32_t cerh;
    volatile uint32_t eer;
    volatile uint32_t eerh;
    volatile uint32_t eecr;
    volatile uint32_t eecrh;
    volatile uint32_t eesr;
    volatile uint32_t eesrh;
    volatile uint32_t ser;
    volatile uint32_t serh;
    volatile uint32_t secr;
    volatile uint32_t secrh;
    volatile uint8_t rsvd0[8];
    volatile uint32_t ier;
    volatile uint32_t ierh;
    volatile uint32_t iecr;
    volatile uint32_t iecrh;
    volatile uint32_t iesr;
    volatile uint32_t iesrh;
    volatile uint32_t ipr;
    volatile uint32_t iprh;
    volatile uint32_t icr;
    volatile uint32_t icrh;
    volatile uint32_t ieval;
    volatile uint8_t rsvd1[4];
    volatile uint32_t qer;
    volatile uint32_t qeer;
    volatile uint32_t qeecr;
    volatile uint32_t qeesr;
    volatile uint32_t qser;
    volatile uint32_t qsecr;
    volatile uint8_t rsvd2[360];
} __edma3_ccrl_shadow_regs_t;

/*******************************************************************************
  register overlay structure for paramentry
*******************************************************************************/
typedef struct __edma3_ccrl_paramentry_regs {
    volatile uint32_t opt;
    volatile uint32_t src;
    volatile uint32_t a_b_cnt;
    volatile uint32_t dst;
    volatile uint32_t src_dst_bidx;
    volatile uint32_t link_bcntrld;
    volatile uint32_t src_dst_cidx;
    volatile uint32_t ccnt;
} __edma3_ccrl_paramentry_regs_t;

/*******************************************************************************
 register overlay structure
*******************************************************************************/
typedef struct __edma3_ccrl_regs {
    volatile uint32_t rev;
    volatile uint32_t cccfg;
    volatile uint8_t rsvd0[248];
    volatile uint32_t dchmap[64];
    volatile uint32_t qchmap[8];
    volatile uint8_t rsvd1[32];
    volatile uint32_t dmaqnum[8];
    volatile uint32_t qdmaqnum;
    volatile uint8_t rsvd2[28];
    volatile uint32_t quetcmap;
    volatile uint32_t quepri;
    volatile uint8_t rsvd3[120];
    volatile uint32_t emr;
    volatile uint32_t emrh;
    volatile uint32_t emcr;
    volatile uint32_t emcrh;
    volatile uint32_t qemr;
    volatile uint32_t qemcr;
    volatile uint32_t ccerr;
    volatile uint32_t ccerrclr;
    volatile uint32_t eeval;
    volatile uint8_t rsvd4[28];
    __edma3_ccrl_dra_regs_t dra[8];
    volatile uint32_t qrae[8];
    volatile uint8_t rsvd5[96];
    __edma3_ccrl_queevtentry_regs_t queevtentry[8][16];
    volatile uint32_t qstat[8];
    volatile uint32_t qwmthra;
    volatile uint32_t qwmthrb;
    volatile uint8_t rsvd6[24];
    volatile uint32_t ccstat;
    volatile uint8_t rsvd7[188];
    volatile uint32_t aetctl;
    volatile uint32_t aetstat;
    volatile uint32_t aetcmd;
    volatile uint8_t rsvd8[244];
    volatile uint32_t mpfar;
    volatile uint32_t mpfsr;
    volatile uint32_t mpfcr;
    volatile uint32_t mppag;
    volatile uint32_t mppa[8];
    volatile uint8_t rsvd9[2000];
    volatile uint32_t er;
    volatile uint32_t erh;
    volatile uint32_t ecr;
    volatile uint32_t ecrh;
    volatile uint32_t esr;
    volatile uint32_t esrh;
    volatile uint32_t cer;
    volatile uint32_t cerh;
    volatile uint32_t eer;
    volatile uint32_t eerh;
    volatile uint32_t eecr;
    volatile uint32_t eecrh;
    volatile uint32_t eesr;
    volatile uint32_t eesrh;
    volatile uint32_t ser;
    volatile uint32_t serh;
    volatile uint32_t secr;
    volatile uint32_t secrh;
    volatile uint8_t rsvd10[8];
    volatile uint32_t ier;
    volatile uint32_t ierh;
    volatile uint32_t iecr;
    volatile uint32_t iecrh;
    volatile uint32_t iesr;
    volatile uint32_t iesrh;
    volatile uint32_t ipr;
    volatile uint32_t iprh;
    volatile uint32_t icr;
    volatile uint32_t icrh;
    volatile uint32_t ieval;
    volatile uint8_t rsvd11[4];
    volatile uint32_t qer;
    volatile uint32_t qeer;
    volatile uint32_t qeecr;
    volatile uint32_t qeesr;
    volatile uint32_t qser;
    volatile uint32_t qsecr;
    volatile uint8_t rsvd12[3944];
    __edma3_ccrl_shadow_regs_t shadow[8];
    volatile uint8_t rsvd13[4096];
    __edma3_ccrl_paramentry_regs_t paramentry[512];
} __edma3_ccrl_regs_t;

/**************************************************************************\
* Register Overlay Structure for DFIREG
\**************************************************************************/
typedef struct  {
    volatile uint32_t dfopt;
    volatile uint32_t dfsrc;
    volatile uint32_t dfcnt;
    volatile uint32_t dfdst;
    volatile uint32_t dfbidx;
    volatile uint32_t dfmpprxy;
    volatile uint8_t rsvd0[40];
} __edma3_tcrl_dfireg_regs_t;

typedef struct  {
    volatile uint32_t rev;
    volatile uint32_t tccfg;
    volatile uint8_t rsvd0[248];
    volatile uint32_t tcstat;
    volatile uint32_t intstat;
    volatile uint32_t inten;
    volatile uint32_t intclr;
    volatile uint32_t intcmd;
    volatile uint8_t rsvd1[12];
    volatile uint32_t errstat;
    volatile uint32_t erren;
    volatile uint32_t errclr;
    volatile uint32_t errdet;
    volatile uint32_t errcmd;
    volatile uint8_t rsvd2[12];
    volatile uint32_t rdrate;
    volatile uint8_t rsvd3[188];
    volatile uint32_t popt;
    volatile uint32_t psrc;
    volatile uint32_t pcnt;
    volatile uint32_t pdst;
    volatile uint32_t pbidx;
    volatile uint32_t pmpprxy;
    volatile uint8_t rsvd4[40];
    volatile uint32_t saopt;
    volatile uint32_t sasrc;
    volatile uint32_t sacnt;
    volatile uint32_t sadst;
    volatile uint32_t sabidx;
    volatile uint32_t sampprxy;
    volatile uint32_t sacntrld;
    volatile uint32_t sasrcbref;
    volatile uint32_t sadstbref;
    volatile uint8_t rsvd5[28];
    volatile uint32_t dfcntrld;
    volatile uint32_t dfsrcbref;
    volatile uint32_t dfdstbref;
    volatile uint8_t rsvd6[116];
    __edma3_tcrl_dfireg_regs_t dfireg[4];
} __edma3_tcrl_regs_t;

/**
 * This define is used to specify that a DMA channel is NOT tied to any PaRAM
 * Set and hence any available PaRAM Set could be used for that DMA channel.
 * It could be used in dmaChannelPaRAMMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific PaRAM Sets.
 */
#define EDMA3_DRV_CH_NO_PARAM_MAP           EDMA3_RM_CH_NO_PARAM_MAP

/**
 * This define is used to specify that the DMA/QDMA channel is not tied to any
 * TCC and hence any available TCC could be used for that DMA/QDMA channel.
 * It could be used in dmaChannelTccMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific TCCs.
 */
#define EDMA3_DRV_CH_NO_TCC_MAP             EDMA3_RM_CH_NO_TCC_MAP


/**
 * \brief QDMA Channel defines
 *
 * They should be used while requesting a specific QDMA channel in API
 * EDMA3_DRV_requestChannel() as the argument (*pLch). Please note that
 * these defines should ONLY be used in the API EDMA3_DRV_requestChannel() and
 * not in any other API to perform further operations. They are only provided
 * to allow user allocate specific QDMA channels.
 */
/** QDMA Channel 0 */
#define EDMA3_DRV_QDMA_CHANNEL_0    (EDMA3_MAX_DMA_CH + EDMA3_MAX_PARAM_SETS)
/** QDMA Channel 1 */
#define EDMA3_DRV_QDMA_CHANNEL_1    (EDMA3_DRV_QDMA_CHANNEL_0+1u)
/** QDMA Channel 2 */
#define EDMA3_DRV_QDMA_CHANNEL_2    (EDMA3_DRV_QDMA_CHANNEL_0+2u)
/** QDMA Channel 3 */
#define EDMA3_DRV_QDMA_CHANNEL_3    (EDMA3_DRV_QDMA_CHANNEL_0+3u)
/** QDMA Channel 4 */
#define EDMA3_DRV_QDMA_CHANNEL_4    (EDMA3_DRV_QDMA_CHANNEL_0+4u)
/** QDMA Channel 5 */
#define EDMA3_DRV_QDMA_CHANNEL_5    (EDMA3_DRV_QDMA_CHANNEL_0+5u)
/** QDMA Channel 6 */
#define EDMA3_DRV_QDMA_CHANNEL_6    (EDMA3_DRV_QDMA_CHANNEL_0+6u)
/** QDMA Channel 7 */
#define EDMA3_DRV_QDMA_CHANNEL_7    (EDMA3_DRV_QDMA_CHANNEL_0+7u)


/**
 * \brief Defines used to support the maximum resources supported
 *        by the EDMA3 controller. These are used to allocate the maximum
 *        memory for different data structures of the EDMA3 Driver and Resource
 *        Manager.
 */
/** \brief Maximum EDMA3 Controllers on the SoC */
#define EDMA3_MAX_EDMA3_INSTANCES               (2u)
/** \brief Maximum DMA channels supported by the EDMA3 Controller */
#define EDMA3_MAX_DMA_CH                        (64u)
/** \brief Maximum QDMA channels supported by the EDMA3 Controller */
#define EDMA3_MAX_QDMA_CH                       (8u)
/** \brief Maximum PaRAM Sets supported by the EDMA3 Controller */
#define EDMA3_MAX_PARAM_SETS                    (512u)
/** \brief Maximum Logical channels supported by the EDMA3 Package */
#define EDMA3_MAX_LOGICAL_CH                    (EDMA3_MAX_DMA_CH + \
                                                 EDMA3_MAX_PARAM_SETS + \
                                                 EDMA3_MAX_QDMA_CH)
/** \brief Maximum TCCs (Interrupt Channels) supported by the EDMA3 Controller */
#define EDMA3_MAX_TCC                           (64u)
/** \brief Maximum Event Queues supported by the EDMA3 Controller */
#define EDMA3_MAX_EVT_QUE                       (8u)
/** \brief Maximum Transfer Controllers supported by the EDMA3 Controller */
#define EDMA3_MAX_TC                            (8u)
/** \brief Maximum Shadow Regions supported by the EDMA3 Controller */
#define EDMA3_MAX_REGIONS                       (8u)

/**
 * Maximum Words (4-bytes region) required for the book-keeping information
 * specific to the maximum possible DMA channels.
 */
#define EDMA3_MAX_DMA_CHAN_DWRDS                (EDMA3_MAX_DMA_CH / 32u)

/**
 * \brief Maximum Words (4-bytes region) required for the book-keeping information
 *        specific to the maximum possible QDMA channels.
 */
#define EDMA3_MAX_QDMA_CHAN_DWRDS               (1u)

/**
 * \brief Maximum Words (4-bytes region) required for the book-keeping information
 *        specific to the maximum possible PaRAM Sets.
 */
#define EDMA3_MAX_PARAM_DWRDS                   (EDMA3_MAX_PARAM_SETS / 32u)

/**
 * \brief Maximum Words (4-bytes region) required for the book-keeping information
 *        specific to the maximum possible TCCs.
 */
#define EDMA3_MAX_TCC_DWRDS                     (EDMA3_MAX_TCC / 32u)




/** \brief Define for setting all bits of the EDMA3 Controller Registers */
#define EDMA3_RM_SET_ALL_BITS              (0xFFFFFFFFu)


/** \brief Defines for Logical Channel Values */
/******************************************************************************/
/**
 * \brief Used to specify any available DMA Channel while requesting
 *        one. Used in the API EDMA3_DRV_requestChannel().
 *        DMA channel from the pool of (owned && non_reserved && available_right_now)
 *        DMA channels will be chosen and returned.
 */
#define EDMA3_DRV_DMA_CHANNEL_ANY                   1002u

/**
 * \brief Used to specify any available QDMA Channel while requesting
 *        one. Used in the API EDMA3_DRV_requestChannel().
 *        QDMA channel from the pool of (owned && non_reserved && available_right_now)
 *        QDMA channels will be chosen and returned.
 */
#define EDMA3_DRV_QDMA_CHANNEL_ANY                  1003u

/**
 * \brief Used to specify any available TCC while requesting
 *        one. Used in the API EDMA3_DRV_requestChannel(), for
 *        both DMA and QDMA channels.
 *        TCC from the pool of (owned && non_reserved && available_right_now)
 *        TCCs will be chosen and returned.
 */
#define EDMA3_DRV_TCC_ANY                           1004u

/*
 * Used to specify any available PaRAM Set while requesting
 * one. Used in the API EDMA3_DRV_requestChannel(), for Link channels.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_DRV_LINK_CHANNEL                      1005u

/*
 * Used to specify any available PaRAM Set while requesting one. Used in the
 * API EDMA3_DRV_requestChannel(), for Link channels.
 * TCC code should also be specified and it will be used to populate the LINK
 * field of the PaRAM Set. Without TCC code, the call will fail.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_DRV_LINK_CHANNEL_WITH_TCC                1006u


/*---------------------------------------------------------------------------*/
/*
 * Used to specify any available DMA Channel while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel ().
 * DMA channel from the pool of (owned && non_reserved && available_right_now)
 * DMA channels will be chosen and returned.
 */
#define EDMA3_RM_DMA_CHANNEL_ANY            (1011u)

/*
 * Used to specify any available QDMA Channel while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel().
 * QDMA channel from the pool of (owned && non_reserved && available_right_now)
 * QDMA channels will be chosen and returned.
 */
#define EDMA3_RM_QDMA_CHANNEL_ANY           (1012u)

/*
 * Used to specify any available TCC while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel(), for
 * both DMA and QDMA channels.
 * TCC from the pool of (owned && non_reserved && available_right_now)
 * TCCs will be chosen and returned.
 */
#define EDMA3_RM_TCC_ANY                    (1013u)

/*
 * Used to specify any available PaRAM Set while requesting
 * one. Used in the API EDMA3_RM_allocLogicalChannel(), for
 * both DMA/QDMA and Link channels.
 * PaRAM Set from the pool of (owned && non_reserved && available_right_now)
 * PaRAM Sets will be chosen and returned.
 */
#define EDMA3_RM_PARAM_ANY                  (1014u)

/*
 * This define is used to specify that a DMA channel is NOT tied to any PaRAM
 * Set and hence any available PaRAM Set could be used for that DMA channel.
 * It could be used in dmaChannelPaRAMMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific PaRAM Sets.
 */
#define EDMA3_RM_CH_NO_PARAM_MAP           (1015u)

/*
 * This define is used to specify that the DMA/QDMA channel is not tied to any
 * TCC and hence any available TCC could be used for that DMA/QDMA channel.
 * It could be used in dmaChannelTccMap [EDMA3_MAX_DMA_CH], in global
 * configuration structure EDMA3_RM_GblConfigParams.
 *
 * This value should mandatorily be used to mark DMA channels with no initial
 * mapping to specific TCCs.
 */
#define EDMA3_RM_CH_NO_TCC_MAP             (1016u)


/**
 * \brief EDMA3 Channel Type
 */
typedef enum
{
    /** \brief Invalid Channel */
    EDMA3_DRV_CHANNEL_TYPE_NONE        = 0,

    /** \brief DMA Channel */
    EDMA3_DRV_CHANNEL_TYPE_DMA      = 1,

    /** \brief QDMA Channel */
    EDMA3_DRV_CHANNEL_TYPE_QDMA     = 2,

    /** \brief LINK Channel */
    EDMA3_DRV_CHANNEL_TYPE_LINK     = 3,

    /** \brief LINK Channel with TCC */
    EDMA3_DRV_CHANNEL_TYPE_LINK_TCC    = 4
} EDMA3_DRV_ChannelType;

/**
 * \brief   Used to specify any available Resource Id (EDMA3_RM_ResDesc.resId)
 */
#define EDMA3_RM_RES_ANY            (1010u)

/**
 * \brief   EDMA3 Resource Type
 */
typedef enum
{
    /** \brief  DMA Channel resource */
    EDMA3_RM_RES_DMA_CHANNEL        = 1,

    /** \brief QDMA Channel resource*/
    EDMA3_RM_RES_QDMA_CHANNEL       = 2,

    /** \brief TCC resource*/
    EDMA3_RM_RES_TCC                = 3,

    /** \brief Parameter RAM Set resource*/
    EDMA3_RM_RES_PARAM_SET          = 4

} EDMA3_RM_ResType;

/**
 * \brief   Handle to a Resource.
 */
typedef struct
{
    /** Resource Id */
    /**
     * Range of resId values :
     * As an example, for resource Type = EDMA3_RM_RES_DMA_CHANNEL,
     * resId can take values from 0 to EDMA3_MAX_DMA_CH
     * Or
     * resId can take the value EDMA3_RM_RES_ANY.
     */
    uint32_t res_id;

    /** Resource Type */
    EDMA3_RM_ResType type;
} EDMA3_RM_ResDesc;

/**
 * \brief   QDMA Trigger Word
 *
 * Use this enum to set the QDMA trigger word to any of the
 * 8 DWords(uint32_t) within a Parameter RAM set
 */
typedef enum
{
        /*
         * Set the OPT field (Offset Address 0h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_OPT                  = 0,
        /*
         * Set the SRC field (Offset Address 4h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC                  = 1,
        /*
         * Set the (ACNT + BCNT) field (Offset Address 8h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_ACNT_BCNT            = 2,
        /*
         * Set the DST field (Offset Address Ch Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_DST                  = 3,
        /*
         * Set the (SRCBIDX + DSTBIDX) field (Offset Address 10h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC_DST_BIDX         = 4,
        /*
         * Set the (LINK + BCNTRLD) field (Offset Address 14h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_LINK_BCNTRLD         = 5,
        /*
         * Set the (SRCCIDX + DSTCIDX) field (Offset Address 18h Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_SRC_DST_CIDX         = 6,
        /*
         * Set the (CCNT + RSVD) field (Offset Address 1Ch Bytes)
         * as the QDMA trigger word
         */
        EDMA3_RM_QDMA_TRIG_CCNT                 = 7,
        /** \brief Default Trigger Word */
        EDMA3_RM_QDMA_TRIG_DEFAULT              = 7
} EDMA3_RM_QdmaTrigWord;

/**
 * \brief EDMA Transfer Synchronization type.
 *
 * Two types of Synchronization of transfers are possible
 *     -# A Synchronized
 *     -# AB Syncronized
 * - A Sync
 *     -# Each Array is submitted as one TR
 *     -# (BCNT*CCNT) number of sync events are needed to completely service
 *        a PaRAM set. (Where BCNT = Num of Arrays in a Frame;
 *                            CCNT = Num of Frames in a Block)
 *     -# (S/D)CIDX = (Addr of First array in next frame)
 *              minus (Addr of Last array in present frame)
 *     (Where CIDX is the Inter-Frame index)
 *
 * - AB Sync
 *     -# Each Frame is submitted as one TR
 *     -# Only CCNT number of sync events are needed to completely service
 *        a PaRAM set
 *     -# (S/D)CIDX = (Addr of First array in next frame)
 *              minus (Addr of First array of present frame)
 *
 * \note ABC sync transfers can be achieved logically by chaining multiple
 *       AB sync transfers
 *
 */
typedef enum
{
        /*
         * A-synchronized.
         * Each event triggers the transfer of a single array of ACNT bytes
         */
        EDMA3_DRV_SYNC_A = 0 ,

        /*
         * AB-synchronized.
         * Each event triggers the transfer of BCNT arrays of ACNT bytes
         */
    EDMA3_DRV_SYNC_AB = 1

} EDMA3_DRV_SyncType;

/**************************************************************************\
* Field Definition Macros
\**************************************************************************/
/* Mask defines */
/** \brief Parameter RAM Set field OPT bit-field defines */
/** \brief OPT-SAM bit Clear */
#define EDMA3_DRV_OPT_SAM_CLR_MASK                  (~EDMA3_CCRL_OPT_SAM_MASK)
/** \brief OPT-SAM bit Set */
#define EDMA3_DRV_OPT_SAM_SET_MASK(mode)            (((EDMA3_CCRL_OPT_SAM_MASK \
                                                      >> EDMA3_CCRL_OPT_SAM_SHIFT) \
                                                      & (mode)) << \
                                                      EDMA3_CCRL_OPT_SAM_SHIFT)

/** \brief OPT-DAM bit Clear */
#define EDMA3_DRV_OPT_DAM_CLR_MASK                  (~EDMA3_CCRL_OPT_DAM_MASK)
/** \brief OPT-DAM bit Set */
#define EDMA3_DRV_OPT_DAM_SET_MASK(mode)            (((EDMA3_CCRL_OPT_DAM_MASK >> \
                                                      EDMA3_CCRL_OPT_DAM_SHIFT) & \
                                                      (mode)) << \
                                                      EDMA3_CCRL_OPT_DAM_SHIFT)

/** \brief OPT-SYNCDIM bit Clear */
#define EDMA3_DRV_OPT_SYNCDIM_CLR_MASK              (~EDMA3_CCRL_OPT_SYNCDIM_MASK)
/** \brief OPT-SYNCDIM bit Set */
#define EDMA3_DRV_OPT_SYNCDIM_SET_MASK(synctype)    (((EDMA3_CCRL_OPT_SYNCDIM_MASK >> \
                                                      EDMA3_CCRL_OPT_SYNCDIM_SHIFT) & \
                                                      (synctype)) << \
                                                      EDMA3_CCRL_OPT_SYNCDIM_SHIFT)

/** \brief OPT-STATIC bit Clear */
#define EDMA3_DRV_OPT_STATIC_CLR_MASK               (~EDMA3_CCRL_OPT_STATIC_MASK)
/** \brief OPT-STATIC bit Set */
#define EDMA3_DRV_OPT_STATIC_SET_MASK(en)           (((EDMA3_CCRL_OPT_STATIC_MASK \
                                                      >> EDMA3_CCRL_OPT_STATIC_SHIFT) \
                                                      & (en)) << \
                                                      EDMA3_CCRL_OPT_STATIC_SHIFT)

/** \brief OPT-FWID bitfield Clear */
#define EDMA3_DRV_OPT_FWID_CLR_MASK                 (~EDMA3_CCRL_OPT_FWID_MASK)
/** \brief OPT-FWID bitfield Set */
#define EDMA3_DRV_OPT_FWID_SET_MASK(width)          (((EDMA3_CCRL_OPT_FWID_MASK \
                                                     >> EDMA3_CCRL_OPT_FWID_SHIFT) \
                                                     & (width)) \
                                                     << EDMA3_CCRL_OPT_FWID_SHIFT)

/** \brief OPT-TCCMODE bit Clear */
#define EDMA3_DRV_OPT_TCCMODE_CLR_MASK              (~EDMA3_CCRL_OPT_TCCMODE_MASK)
/** \brief OPT-TCCMODE bit Set */
#define EDMA3_DRV_OPT_TCCMODE_SET_MASK(early)       (((EDMA3_CCRL_OPT_TCCMODE_MASK \
                                                      >> EDMA3_CCRL_OPT_TCCMODE_SHIFT) \
                                                      & (early)) << \
                                                      EDMA3_CCRL_OPT_TCCMODE_SHIFT)

/** \brief OPT-TCC bitfield Clear */
#define EDMA3_DRV_OPT_TCC_CLR_MASK                  (~EDMA3_CCRL_OPT_TCC_MASK)
/** \brief OPT-TCC bitfield Set */
#define EDMA3_DRV_OPT_TCC_SET_MASK(tcc)             (((EDMA3_CCRL_OPT_TCC_MASK >> \
                                                      EDMA3_CCRL_OPT_TCC_SHIFT) \
                                                      & (tcc)) << \
                                                      EDMA3_CCRL_OPT_TCC_SHIFT)

/** \brief OPT-TCINTEN bit Clear */
#define EDMA3_DRV_OPT_TCINTEN_CLR_MASK              (~EDMA3_CCRL_OPT_TCINTEN_MASK)
/** \brief OPT-TCINTEN bit Set */
#define EDMA3_DRV_OPT_TCINTEN_SET_MASK(tcinten)     (((EDMA3_CCRL_OPT_TCINTEN_MASK \
                                                      >> EDMA3_CCRL_OPT_TCINTEN_SHIFT) \
                                                      & (tcinten)) << \
                                                      EDMA3_CCRL_OPT_TCINTEN_SHIFT)

/** \brief OPT-ITCINTEN bit Clear */
#define EDMA3_DRV_OPT_ITCINTEN_CLR_MASK             (~EDMA3_CCRL_OPT_ITCINTEN_MASK)
/** \brief OPT-ITCINTEN bit Set */
#define EDMA3_DRV_OPT_ITCINTEN_SET_MASK(itcinten)   (((EDMA3_CCRL_OPT_ITCINTEN_MASK \
                                                      >> EDMA3_CCRL_OPT_ITCINTEN_SHIFT)\
                                                      & (itcinten)) << \
                                                      EDMA3_CCRL_OPT_ITCINTEN_SHIFT)

/** \brief OPT-TCCHEN bit Clear */
#define EDMA3_DRV_OPT_TCCHEN_CLR_MASK               (~EDMA3_CCRL_OPT_TCCHEN_MASK)
/** \brief OPT-TCCHEN bit Set */
#define EDMA3_DRV_OPT_TCCHEN_SET_MASK(tcchen)       (((EDMA3_CCRL_OPT_TCCHEN_MASK \
                                                      >> EDMA3_CCRL_OPT_TCCHEN_SHIFT) \
                                                      & (tcchen)) << \
                                                      EDMA3_CCRL_OPT_TCCHEN_SHIFT)

/** \brief OPT-ITCCHEN bit Clear */
#define EDMA3_DRV_OPT_ITCCHEN_CLR_MASK              (~EDMA3_CCRL_OPT_ITCCHEN_MASK)
/** \brief OPT-ITCCHEN bit Set */
#define EDMA3_DRV_OPT_ITCCHEN_SET_MASK(itcchen)     (((EDMA3_CCRL_OPT_ITCCHEN_MASK \
                                                      >> EDMA3_CCRL_OPT_ITCCHEN_SHIFT) \
                                                      & (itcchen)) << \
                                                      EDMA3_CCRL_OPT_ITCCHEN_SHIFT)

/** \brief OPT-SAM bit Get */
#define EDMA3_DRV_OPT_SAM_GET_MASK(mode)            ((mode)&1u)
/** \brief OPT-DAM bit Get */
#define EDMA3_DRV_OPT_DAM_GET_MASK(mode)            (((mode)&(1u<<1u))>>1u)
/** \brief OPT-SYNCDIM bit Get */
#define EDMA3_DRV_OPT_SYNCDIM_GET_MASK(synctype)    (((synctype)&(1u<<2u))>>2u)
/** \brief OPT-STATIC bit Get */
#define EDMA3_DRV_OPT_STATIC_GET_MASK(en)           (((en)&(1u<<3u))>>3u)
/** \brief OPT-FWID bitfield Get */
#define EDMA3_DRV_OPT_FWID_GET_MASK(width)          (((width)&(0x7u<<8u))>>8u)
/** \brief OPT-TCCMODE bit Get */
#define EDMA3_DRV_OPT_TCCMODE_GET_MASK(early)       (((early)&(1u<<11u))>>11u)
/** \brief OPT-TCC bitfield Get */
#define EDMA3_DRV_OPT_TCC_GET_MASK(tcc)             (((tcc)&(0x3fu<<12u))>>12u)
/** \brief OPT-TCINTEN bit Get */
#define EDMA3_DRV_OPT_TCINTEN_GET_MASK(tcinten)     (((tcinten)&(1u<<20u))>>20u)
/** \brief OPT-ITCINTEN bit Get */
#define EDMA3_DRV_OPT_ITCINTEN_GET_MASK(itcinten)   (((itcinten)&(1u<<21u))>>21u)
/** \brief OPT-TCCHEN bit Get */
#define EDMA3_DRV_OPT_TCCHEN_GET_MASK(tcchen)       (((tcchen)&(1u<<22u))>>22u)
/** \brief OPT-ITCCHEN bit Get */
#define EDMA3_DRV_OPT_ITCCHEN_GET_MASK(itcchen)     (((itcchen)&(1u<<23u))>>23u)

/** \brief DMAQNUM bits Clear */
#define EDMA3_DRV_DMAQNUM_CLR_MASK(chNum)           (~(0x7u<<(((chNum)%8u)*4u)))
/** \brief DMAQNUM bits Set */
#define EDMA3_DRV_DMAQNUM_SET_MASK(chNum,queNum)    ((0x7u & (queNum)) << (((chNum)%8u)*4u))
/** \brief QDMAQNUM bits Clear */
#define EDMA3_DRV_QDMAQNUM_CLR_MASK(chNum)          (~(0x7u<<((chNum)*4u)))
/** \brief QDMAQNUM bits Set */
#define EDMA3_DRV_QDMAQNUM_SET_MASK(chNum,queNum)   ((0x7u & (queNum)) << ((chNum)*4u))


/** \brief Other Mask defines */
/** \brief DCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_DRV_DCH_PARAM_CLR_MASK                (~EDMA3_CCRL_DCHMAP_PAENTRY_MASK)
/** \brief DCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_DRV_DCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_DCHMAP_PAENTRY_MASK \
                                                      >> EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT) \
                                                      & (paRAMId)) << \
                                                      EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)

/** \brief QCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_DRV_QCH_PARAM_CLR_MASK                (~EDMA3_CCRL_QCHMAP_PAENTRY_MASK)
/** \brief QCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_DRV_QCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_QCHMAP_PAENTRY_MASK \
                                                      >> EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT) \
                                                      & (paRAMId)) << \
                                                      EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)
/** \brief QCHMAP-TrigWord bitfield Clear */
#define EDMA3_DRV_QCH_TRWORD_CLR_MASK               (~EDMA3_CCRL_QCHMAP_TRWORD_MASK)
/** \brief QCHMAP-TrigWord bitfield Set */
#define EDMA3_DRV_QCH_TRWORD_SET_MASK(trWord)      (((EDMA3_CCRL_QCHMAP_TRWORD_MASK >> \
                                                    EDMA3_CCRL_QCHMAP_TRWORD_SHIFT) & \
                                                    (trWord)) << \
                                                    EDMA3_CCRL_QCHMAP_TRWORD_SHIFT)
/** \brief QCHMAP-PaRAMEntry & TrigWord bitfield Clear */
#define EDMA3_DRV_QCH_PARAM_TRWORD_CLR_MASK         (EDMA3_DRV_QCH_PARAM_CLR_MASK \
                                                     | EDMA3_DRV_QCH_TRWORD_CLR_MASK)

/** \brief Max value of ACnt */
#define EDMA3_DRV_ACNT_MAX_VAL              (0xFFFFu)
/** \brief Max value of BCnt */
#define EDMA3_DRV_BCNT_MAX_VAL              (0xFFFFu)
/** \brief Max value of CCnt */
#define EDMA3_DRV_CCNT_MAX_VAL              (0xFFFFu)
/** \brief Max value of BCntReld */
#define EDMA3_DRV_BCNTRELD_MAX_VAL          (0xFFFFu)
/** \brief Max value of SrcBIdx */
#define EDMA3_DRV_SRCBIDX_MAX_VAL           (0x7FFF)
/** \brief Min value of SrcBIdx */
#define EDMA3_DRV_SRCBIDX_MIN_VAL           (-32768)
/** \brief Max value of SrcCIdx */
#define EDMA3_DRV_SRCCIDX_MAX_VAL           (0x7FFF)
/** \brief Min value of SrcCIdx */
#define EDMA3_DRV_SRCCIDX_MIN_VAL           (-32768)
/** \brief Max value of DestBIdx */
#define EDMA3_DRV_DSTBIDX_MAX_VAL           (0x7FFF)
/** \brief Min value of DestBIdx */
#define EDMA3_DRV_DSTBIDX_MIN_VAL           (-32768)
/** \brief Max value of DestCIdx */
#define EDMA3_DRV_DSTCIDX_MAX_VAL           (0x7FFF)
/** \brief Min value of DestCIdx */
#define EDMA3_DRV_DSTCIDX_MIN_VAL           (-32768)
/** \brief Max value of Queue Priority */
#define EDMA3_DRV_QPRIORITY_MAX_VAL         (7u)
/** \brief Min value of Queue Priority */
#define EDMA3_DRV_QPRIORITY_MIN_VAL         (0u)

/* REV */

#define EDMA3_CCRL_REV_TYPE_MASK         (0x00FF0000u)
#define EDMA3_CCRL_REV_TYPE_SHIFT        (0x00000010u)
#define EDMA3_CCRL_REV_TYPE_RESETVAL     (0x00000007u)

#define EDMA3_CCRL_REV_CLASS_MASK        (0x0000FF00u)
#define EDMA3_CCRL_REV_CLASS_SHIFT       (0x00000008u)
#define EDMA3_CCRL_REV_CLASS_RESETVAL    (0x00000004u)

#define EDMA3_CCRL_REV_RESERVED_MASK     (0x000000FFu)
#define EDMA3_CCRL_REV_RESERVED_SHIFT    (0x00000000u)
#define EDMA3_CCRL_REV_RESERVED_RESETVAL (0x00000000u)

#define EDMA3_CCRL_REV_RESETVAL          (0x00070400u)

/* CCCFG */

#define EDMA3_CCRL_CCCFG_MP_EXIST_MASK   (0x02000000u)
#define EDMA3_CCRL_CCCFG_MP_EXIST_SHIFT  (0x00000019u)
#define EDMA3_CCRL_CCCFG_MP_EXIST_RESETVAL (0x00000000u)

/*----MP_EXIST Tokens----*/
#define EDMA3_CCRL_CCCFG_MP_EXIST_NONE   (0x00000000u)
#define EDMA3_CCRL_CCCFG_MP_EXIST_INCLUDED (0x00000001u)

#define EDMA3_CCRL_CCCFG_CHMAP_EXIST_MASK (0x01000000u)
#define EDMA3_CCRL_CCCFG_CHMAP_EXIST_SHIFT (0x00000018u)
#define EDMA3_CCRL_CCCFG_CHMAP_EXIST_RESETVAL (0x00000000u)

/*----CHMAP_EXIST Tokens----*/
#define EDMA3_CCRL_CCCFG_CHMAP_EXIST_NONE (0x00000000u)
#define EDMA3_CCRL_CCCFG_CHMAP_EXIST_INCLUDED (0x00000001u)

#define EDMA3_CCRL_CCCFG_NUM_REGN_MASK   (0x00300000u)
#define EDMA3_CCRL_CCCFG_NUM_REGN_SHIFT  (0x00000014u)
#define EDMA3_CCRL_CCCFG_NUM_REGN_RESETVAL (0x00000000u)

/*----NUM_REGN Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_REGN_0      (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_REGN_2      (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_REGN_4      (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_REGN_8      (0x00000003u)

#define EDMA3_CCRL_CCCFG_NUM_TC_MASK     (0x00070000u)
#define EDMA3_CCRL_CCCFG_NUM_TC_SHIFT    (0x00000010u)
#define EDMA3_CCRL_CCCFG_NUM_TC_RESETVAL (0x00000000u)

/*----NUM_TC Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_TC_1        (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_TC_2        (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_TC_3        (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_TC_4        (0x00000003u)
#define EDMA3_CCRL_CCCFG_NUM_TC_5        (0x00000004u)
#define EDMA3_CCRL_CCCFG_NUM_TC_6        (0x00000005u)
#define EDMA3_CCRL_CCCFG_NUM_TC_7        (0x00000006u)
#define EDMA3_CCRL_CCCFG_NUM_TC_8        (0x00000007u)

#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_MASK (0x00007000u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_SHIFT (0x0000000Cu)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_RESETVAL (0x00000000u)

/*----NUM_PAENTRY Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_16  (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_32  (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_64  (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_128 (0x00000003u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_256 (0x00000004u)
#define EDMA3_CCRL_CCCFG_NUM_PAENTRY_512 (0x00000005u)

#define EDMA3_CCRL_CCCFG_NUM_INTCH_MASK  (0x00000700u)
#define EDMA3_CCRL_CCCFG_NUM_INTCH_SHIFT (0x00000008u)
#define EDMA3_CCRL_CCCFG_NUM_INTCH_RESETVAL (0x00000000u)

/*----NUM_INTCH Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_INTCH_8     (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_INTCH_16    (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_INTCH_32    (0x00000003u)
#define EDMA3_CCRL_CCCFG_NUM_INTCH_64    (0x00000004u)

#define EDMA3_CCRL_CCCFG_NUM_QDMACH_MASK (0x00000070u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_SHIFT (0x00000004u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_RESETVAL (0x00000000u)

/*----NUM_QDMACH Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_NONE (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_2    (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_4    (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_6    (0x00000003u)
#define EDMA3_CCRL_CCCFG_NUM_QDMACH_8    (0x00000004u)

#define EDMA3_CCRL_CCCFG_NUM_DMACH_MASK  (0x00000007u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_SHIFT (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_RESETVAL (0x00000000u)

/*----NUM_DMACH Tokens----*/
#define EDMA3_CCRL_CCCFG_NUM_DMACH_NONE  (0x00000000u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_4     (0x00000001u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_8     (0x00000002u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_16    (0x00000003u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_32    (0x00000004u)
#define EDMA3_CCRL_CCCFG_NUM_DMACH_64    (0x00000005u)

#define EDMA3_CCRL_CCCFG_RESETVAL        (0x00000000u)

/* DCHMAP */

#define EDMA3_CCRL_DCHMAP_PAENTRY_MASK   (0x00003FE0u)
#define EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT  (0x00000005u)
#define EDMA3_CCRL_DCHMAP_PAENTRY_RESETVAL (0x00000000u)

#define EDMA3_CCRL_DCHMAP_RESETVAL       (0x00000000u)

/* QCHMAP */

#define EDMA3_CCRL_QCHMAP_PAENTRY_MASK   (0x00003FE0u)
#define EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT  (0x00000005u)
#define EDMA3_CCRL_QCHMAP_PAENTRY_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QCHMAP_TRWORD_MASK    (0x0000001Cu)
#define EDMA3_CCRL_QCHMAP_TRWORD_SHIFT   (0x00000002u)
#define EDMA3_CCRL_QCHMAP_TRWORD_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QCHMAP_RESETVAL       (0x00000000u)

/* DMAQNUM */

#define EDMA3_CCRL_DMAQNUM_E7_MASK       (0x70000000u)
#define EDMA3_CCRL_DMAQNUM_E7_SHIFT      (0x0000001Cu)
#define EDMA3_CCRL_DMAQNUM_E7_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E6_MASK       (0x07000000u)
#define EDMA3_CCRL_DMAQNUM_E6_SHIFT      (0x00000018u)
#define EDMA3_CCRL_DMAQNUM_E6_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E5_MASK       (0x00700000u)
#define EDMA3_CCRL_DMAQNUM_E5_SHIFT      (0x00000014u)
#define EDMA3_CCRL_DMAQNUM_E5_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E4_MASK       (0x00070000u)
#define EDMA3_CCRL_DMAQNUM_E4_SHIFT      (0x00000010u)
#define EDMA3_CCRL_DMAQNUM_E4_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E3_MASK       (0x00007000u)
#define EDMA3_CCRL_DMAQNUM_E3_SHIFT      (0x0000000Cu)
#define EDMA3_CCRL_DMAQNUM_E3_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E2_MASK       (0x00000700u)
#define EDMA3_CCRL_DMAQNUM_E2_SHIFT      (0x00000008u)
#define EDMA3_CCRL_DMAQNUM_E2_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E1_MASK       (0x00000070u)
#define EDMA3_CCRL_DMAQNUM_E1_SHIFT      (0x00000004u)
#define EDMA3_CCRL_DMAQNUM_E1_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_E0_MASK       (0x00000007u)
#define EDMA3_CCRL_DMAQNUM_E0_SHIFT      (0x00000000u)
#define EDMA3_CCRL_DMAQNUM_E0_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_DMAQNUM_RESETVAL      (0x00000000u)

/* QDMAQNUM */

#define EDMA3_CCRL_QDMAQNUM_E7_MASK      (0x70000000u)
#define EDMA3_CCRL_QDMAQNUM_E7_SHIFT     (0x0000001Cu)
#define EDMA3_CCRL_QDMAQNUM_E7_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E6_MASK      (0x07000000u)
#define EDMA3_CCRL_QDMAQNUM_E6_SHIFT     (0x00000018u)
#define EDMA3_CCRL_QDMAQNUM_E6_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E5_MASK      (0x00700000u)
#define EDMA3_CCRL_QDMAQNUM_E5_SHIFT     (0x00000014u)
#define EDMA3_CCRL_QDMAQNUM_E5_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E4_MASK      (0x00070000u)
#define EDMA3_CCRL_QDMAQNUM_E4_SHIFT     (0x00000010u)
#define EDMA3_CCRL_QDMAQNUM_E4_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E3_MASK      (0x00007000u)
#define EDMA3_CCRL_QDMAQNUM_E3_SHIFT     (0x0000000Cu)
#define EDMA3_CCRL_QDMAQNUM_E3_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E2_MASK      (0x00000700u)
#define EDMA3_CCRL_QDMAQNUM_E2_SHIFT     (0x00000008u)
#define EDMA3_CCRL_QDMAQNUM_E2_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E1_MASK      (0x00000070u)
#define EDMA3_CCRL_QDMAQNUM_E1_SHIFT     (0x00000004u)
#define EDMA3_CCRL_QDMAQNUM_E1_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_E0_MASK      (0x00000007u)
#define EDMA3_CCRL_QDMAQNUM_E0_SHIFT     (0x00000000u)
#define EDMA3_CCRL_QDMAQNUM_E0_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QDMAQNUM_RESETVAL     (0x00000000u)

/* QUETCMAP */

#define EDMA3_CCRL_QUETCMAP_TCNUMQ7_MASK (0x70000000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ7_SHIFT (0x0000001Cu)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ7_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ6_MASK (0x07000000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ6_SHIFT (0x00000018u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ6_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ5_MASK (0x00700000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ5_SHIFT (0x00000014u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ5_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ4_MASK (0x00070000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ4_SHIFT (0x00000010u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ4_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ3_MASK (0x00007000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ3_SHIFT (0x0000000Cu)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ3_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ2_MASK (0x00000700u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ2_SHIFT (0x00000008u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ2_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ1_MASK (0x00000070u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ1_SHIFT (0x00000004u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ1_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_TCNUMQ0_MASK (0x00000007u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ0_SHIFT (0x00000000u)
#define EDMA3_CCRL_QUETCMAP_TCNUMQ0_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUETCMAP_RESETVAL     (0x00000000u)

/* QUEPRI */

#define EDMA3_CCRL_QUEPRI_PRIQ7_MASK     (0x70000000u)
#define EDMA3_CCRL_QUEPRI_PRIQ7_SHIFT    (0x0000001Cu)
#define EDMA3_CCRL_QUEPRI_PRIQ7_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ6_MASK     (0x07000000u)
#define EDMA3_CCRL_QUEPRI_PRIQ6_SHIFT    (0x00000018u)
#define EDMA3_CCRL_QUEPRI_PRIQ6_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ5_MASK     (0x00700000u)
#define EDMA3_CCRL_QUEPRI_PRIQ5_SHIFT    (0x00000014u)
#define EDMA3_CCRL_QUEPRI_PRIQ5_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ4_MASK     (0x00070000u)
#define EDMA3_CCRL_QUEPRI_PRIQ4_SHIFT    (0x00000010u)
#define EDMA3_CCRL_QUEPRI_PRIQ4_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ3_MASK     (0x00007000u)
#define EDMA3_CCRL_QUEPRI_PRIQ3_SHIFT    (0x0000000Cu)
#define EDMA3_CCRL_QUEPRI_PRIQ3_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ2_MASK     (0x00000700u)
#define EDMA3_CCRL_QUEPRI_PRIQ2_SHIFT    (0x00000008u)
#define EDMA3_CCRL_QUEPRI_PRIQ2_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ1_MASK     (0x00000070u)
#define EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT    (0x00000004u)
#define EDMA3_CCRL_QUEPRI_PRIQ1_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_PRIQ0_MASK     (0x00000007u)
#define EDMA3_CCRL_QUEPRI_PRIQ0_SHIFT    (0x00000000u)
#define EDMA3_CCRL_QUEPRI_PRIQ0_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEPRI_RESETVAL       (0x00000000u)

/* EMR */

#define EDMA3_CCRL_EMR_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_EMR_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_EMR_E31_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_EMR_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_EMR_E30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_EMR_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_EMR_E29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_EMR_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_EMR_E28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_EMR_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_EMR_E27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_EMR_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_EMR_E26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_EMR_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_EMR_E25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_EMR_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_EMR_E24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_EMR_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_EMR_E23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_EMR_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_EMR_E22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_EMR_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_EMR_E21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_EMR_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_EMR_E20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_EMR_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_EMR_E19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_EMR_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_EMR_E18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_EMR_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_EMR_E17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_EMR_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_EMR_E16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_EMR_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_EMR_E15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_EMR_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_EMR_E14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_EMR_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_EMR_E13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_EMR_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_EMR_E12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_EMR_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_EMR_E11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_EMR_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_EMR_E10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMR_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_EMR_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_EMR_E9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_EMR_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_EMR_E8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_EMR_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_EMR_E7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_EMR_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_EMR_E6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_EMR_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_EMR_E5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_EMR_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_EMR_E4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_EMR_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_EMR_E3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_EMR_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_EMR_E2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_EMR_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_EMR_E1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_EMR_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_EMR_E0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EMR_RESETVAL          (0x00000000u)

/* EMRH */

#define EDMA3_CCRL_EMRH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_EMRH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_EMRH_E63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_EMRH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_EMRH_E62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_EMRH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_EMRH_E61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_EMRH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_EMRH_E60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_EMRH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_EMRH_E59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_EMRH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_EMRH_E58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_EMRH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_EMRH_E57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_EMRH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_EMRH_E56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_EMRH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_EMRH_E55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_EMRH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_EMRH_E54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_EMRH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_EMRH_E53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_EMRH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_EMRH_E52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_EMRH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_EMRH_E51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_EMRH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_EMRH_E50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_EMRH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_EMRH_E49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_EMRH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_EMRH_E48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_EMRH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_EMRH_E47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_EMRH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_EMRH_E46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_EMRH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_EMRH_E45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_EMRH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_EMRH_E44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_EMRH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_EMRH_E43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_EMRH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_EMRH_E42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_EMRH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_EMRH_E41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_EMRH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_EMRH_E40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_EMRH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_EMRH_E39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_EMRH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_EMRH_E38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_EMRH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_EMRH_E37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_EMRH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_EMRH_E36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_EMRH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_EMRH_E35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_EMRH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_EMRH_E34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_EMRH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_EMRH_E33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_EMRH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_EMRH_E32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMRH_RESETVAL         (0x00000000u)

/* EMCR */

#define EDMA3_CCRL_EMCR_E31_MASK         (0x80000000u)
#define EDMA3_CCRL_EMCR_E31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_EMCR_E31_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E30_MASK         (0x40000000u)
#define EDMA3_CCRL_EMCR_E30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_EMCR_E30_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E29_MASK         (0x20000000u)
#define EDMA3_CCRL_EMCR_E29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_EMCR_E29_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E28_MASK         (0x10000000u)
#define EDMA3_CCRL_EMCR_E28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_EMCR_E28_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E27_MASK         (0x08000000u)
#define EDMA3_CCRL_EMCR_E27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_EMCR_E27_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E26_MASK         (0x04000000u)
#define EDMA3_CCRL_EMCR_E26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_EMCR_E26_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E25_MASK         (0x02000000u)
#define EDMA3_CCRL_EMCR_E25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_EMCR_E25_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E24_MASK         (0x01000000u)
#define EDMA3_CCRL_EMCR_E24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_EMCR_E24_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E23_MASK         (0x00800000u)
#define EDMA3_CCRL_EMCR_E23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_EMCR_E23_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E22_MASK         (0x00400000u)
#define EDMA3_CCRL_EMCR_E22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_EMCR_E22_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E21_MASK         (0x00200000u)
#define EDMA3_CCRL_EMCR_E21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_EMCR_E21_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E20_MASK         (0x00100000u)
#define EDMA3_CCRL_EMCR_E20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_EMCR_E20_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E19_MASK         (0x00080000u)
#define EDMA3_CCRL_EMCR_E19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_EMCR_E19_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E18_MASK         (0x00040000u)
#define EDMA3_CCRL_EMCR_E18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_EMCR_E18_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E17_MASK         (0x00020000u)
#define EDMA3_CCRL_EMCR_E17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_EMCR_E17_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E16_MASK         (0x00010000u)
#define EDMA3_CCRL_EMCR_E16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_EMCR_E16_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E15_MASK         (0x00008000u)
#define EDMA3_CCRL_EMCR_E15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_EMCR_E15_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E14_MASK         (0x00004000u)
#define EDMA3_CCRL_EMCR_E14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_EMCR_E14_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E13_MASK         (0x00002000u)
#define EDMA3_CCRL_EMCR_E13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_EMCR_E13_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E12_MASK         (0x00001000u)
#define EDMA3_CCRL_EMCR_E12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_EMCR_E12_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E11_MASK         (0x00000800u)
#define EDMA3_CCRL_EMCR_E11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_EMCR_E11_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E10_MASK         (0x00000400u)
#define EDMA3_CCRL_EMCR_E10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_EMCR_E10_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EMCR_E9_MASK          (0x00000200u)
#define EDMA3_CCRL_EMCR_E9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_EMCR_E9_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E8_MASK          (0x00000100u)
#define EDMA3_CCRL_EMCR_E8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_EMCR_E8_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_EMCR_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_EMCR_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_EMCR_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_EMCR_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_EMCR_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_EMCR_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_EMCR_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_EMCR_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_EMCR_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_EMCR_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_EMCR_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_EMCR_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_EMCR_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_EMCR_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_EMCR_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_EMCR_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EMCR_RESETVAL         (0x00000000u)

/* EMCRH */

#define EDMA3_CCRL_EMCRH_E63_MASK        (0x80000000u)
#define EDMA3_CCRL_EMCRH_E63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_EMCRH_E63_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E62_MASK        (0x40000000u)
#define EDMA3_CCRL_EMCRH_E62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_EMCRH_E62_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E61_MASK        (0x20000000u)
#define EDMA3_CCRL_EMCRH_E61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_EMCRH_E61_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E60_MASK        (0x10000000u)
#define EDMA3_CCRL_EMCRH_E60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_EMCRH_E60_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E59_MASK        (0x08000000u)
#define EDMA3_CCRL_EMCRH_E59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_EMCRH_E59_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E58_MASK        (0x04000000u)
#define EDMA3_CCRL_EMCRH_E58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_EMCRH_E58_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E57_MASK        (0x02000000u)
#define EDMA3_CCRL_EMCRH_E57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_EMCRH_E57_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E56_MASK        (0x01000000u)
#define EDMA3_CCRL_EMCRH_E56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_EMCRH_E56_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E55_MASK        (0x00800000u)
#define EDMA3_CCRL_EMCRH_E55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_EMCRH_E55_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E54_MASK        (0x00400000u)
#define EDMA3_CCRL_EMCRH_E54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_EMCRH_E54_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E53_MASK        (0x00200000u)
#define EDMA3_CCRL_EMCRH_E53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_EMCRH_E53_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E52_MASK        (0x00100000u)
#define EDMA3_CCRL_EMCRH_E52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_EMCRH_E52_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E51_MASK        (0x00080000u)
#define EDMA3_CCRL_EMCRH_E51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_EMCRH_E51_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E50_MASK        (0x00040000u)
#define EDMA3_CCRL_EMCRH_E50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_EMCRH_E50_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E49_MASK        (0x00020000u)
#define EDMA3_CCRL_EMCRH_E49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_EMCRH_E49_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E48_MASK        (0x00010000u)
#define EDMA3_CCRL_EMCRH_E48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_EMCRH_E48_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E47_MASK        (0x00008000u)
#define EDMA3_CCRL_EMCRH_E47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_EMCRH_E47_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E46_MASK        (0x00004000u)
#define EDMA3_CCRL_EMCRH_E46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_EMCRH_E46_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E45_MASK        (0x00002000u)
#define EDMA3_CCRL_EMCRH_E45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_EMCRH_E45_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E44_MASK        (0x00001000u)
#define EDMA3_CCRL_EMCRH_E44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_EMCRH_E44_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E43_MASK        (0x00000800u)
#define EDMA3_CCRL_EMCRH_E43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_EMCRH_E43_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E42_MASK        (0x00000400u)
#define EDMA3_CCRL_EMCRH_E42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_EMCRH_E42_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E41_MASK        (0x00000200u)
#define EDMA3_CCRL_EMCRH_E41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_EMCRH_E41_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E40_MASK        (0x00000100u)
#define EDMA3_CCRL_EMCRH_E40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_EMCRH_E40_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E39_MASK        (0x00000080u)
#define EDMA3_CCRL_EMCRH_E39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_EMCRH_E39_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E38_MASK        (0x00000040u)
#define EDMA3_CCRL_EMCRH_E38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_EMCRH_E38_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E37_MASK        (0x00000020u)
#define EDMA3_CCRL_EMCRH_E37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_EMCRH_E37_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E36_MASK        (0x00000010u)
#define EDMA3_CCRL_EMCRH_E36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_EMCRH_E36_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E35_MASK        (0x00000008u)
#define EDMA3_CCRL_EMCRH_E35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_EMCRH_E35_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E34_MASK        (0x00000004u)
#define EDMA3_CCRL_EMCRH_E34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_EMCRH_E34_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E33_MASK        (0x00000002u)
#define EDMA3_CCRL_EMCRH_E33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_EMCRH_E33_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_E32_MASK        (0x00000001u)
#define EDMA3_CCRL_EMCRH_E32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_EMCRH_E32_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EMCRH_RESETVAL        (0x00000000u)

/* QEMR */

#define EDMA3_CCRL_QEMR_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_QEMR_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QEMR_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_QEMR_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_QEMR_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_QEMR_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_QEMR_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_QEMR_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_QEMR_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_QEMR_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_QEMR_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_QEMR_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_QEMR_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_QEMR_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_QEMR_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_QEMR_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_QEMR_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEMR_RESETVAL         (0x00000000u)

/* QEMCR */

#define EDMA3_CCRL_QEMCR_E7_MASK         (0x00000080u)
#define EDMA3_CCRL_QEMCR_E7_SHIFT        (0x00000007u)
#define EDMA3_CCRL_QEMCR_E7_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E6_MASK         (0x00000040u)
#define EDMA3_CCRL_QEMCR_E6_SHIFT        (0x00000006u)
#define EDMA3_CCRL_QEMCR_E6_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E5_MASK         (0x00000020u)
#define EDMA3_CCRL_QEMCR_E5_SHIFT        (0x00000005u)
#define EDMA3_CCRL_QEMCR_E5_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E4_MASK         (0x00000010u)
#define EDMA3_CCRL_QEMCR_E4_SHIFT        (0x00000004u)
#define EDMA3_CCRL_QEMCR_E4_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E3_MASK         (0x00000008u)
#define EDMA3_CCRL_QEMCR_E3_SHIFT        (0x00000003u)
#define EDMA3_CCRL_QEMCR_E3_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E2_MASK         (0x00000004u)
#define EDMA3_CCRL_QEMCR_E2_SHIFT        (0x00000002u)
#define EDMA3_CCRL_QEMCR_E2_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E1_MASK         (0x00000002u)
#define EDMA3_CCRL_QEMCR_E1_SHIFT        (0x00000001u)
#define EDMA3_CCRL_QEMCR_E1_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_E0_MASK         (0x00000001u)
#define EDMA3_CCRL_QEMCR_E0_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QEMCR_E0_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEMCR_RESETVAL        (0x00000000u)

/* CCERR */

#define EDMA3_CCRL_CCERR_TCCERR_MASK     (0x00010000u)
#define EDMA3_CCRL_CCERR_TCCERR_SHIFT    (0x00000010u)
#define EDMA3_CCRL_CCERR_TCCERR_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD7_MASK   (0x00000080u)
#define EDMA3_CCRL_CCERR_QTHRXCD7_SHIFT  (0x00000007u)
#define EDMA3_CCRL_CCERR_QTHRXCD7_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD6_MASK   (0x00000040u)
#define EDMA3_CCRL_CCERR_QTHRXCD6_SHIFT  (0x00000006u)
#define EDMA3_CCRL_CCERR_QTHRXCD6_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD5_MASK   (0x00000020u)
#define EDMA3_CCRL_CCERR_QTHRXCD5_SHIFT  (0x00000005u)
#define EDMA3_CCRL_CCERR_QTHRXCD5_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD4_MASK   (0x00000010u)
#define EDMA3_CCRL_CCERR_QTHRXCD4_SHIFT  (0x00000004u)
#define EDMA3_CCRL_CCERR_QTHRXCD4_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD3_MASK   (0x00000008u)
#define EDMA3_CCRL_CCERR_QTHRXCD3_SHIFT  (0x00000003u)
#define EDMA3_CCRL_CCERR_QTHRXCD3_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD2_MASK   (0x00000004u)
#define EDMA3_CCRL_CCERR_QTHRXCD2_SHIFT  (0x00000002u)
#define EDMA3_CCRL_CCERR_QTHRXCD2_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD1_MASK   (0x00000002u)
#define EDMA3_CCRL_CCERR_QTHRXCD1_SHIFT  (0x00000001u)
#define EDMA3_CCRL_CCERR_QTHRXCD1_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_QTHRXCD0_MASK   (0x00000001u)
#define EDMA3_CCRL_CCERR_QTHRXCD0_SHIFT  (0x00000000u)
#define EDMA3_CCRL_CCERR_QTHRXCD0_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERR_RESETVAL        (0x00000000u)

/* CCERRCLR */

#define EDMA3_CCRL_CCERRCLR_TCCERR_MASK  (0x00010000u)
#define EDMA3_CCRL_CCERRCLR_TCCERR_SHIFT (0x00000010u)
#define EDMA3_CCRL_CCERRCLR_TCCERR_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD7_MASK (0x00000080u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD7_SHIFT (0x00000007u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD7_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD6_MASK (0x00000040u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD6_SHIFT (0x00000006u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD6_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD5_MASK (0x00000020u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD5_SHIFT (0x00000005u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD5_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD4_MASK (0x00000010u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD4_SHIFT (0x00000004u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD4_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD3_MASK (0x00000008u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD3_SHIFT (0x00000003u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD3_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD2_MASK (0x00000004u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD2_SHIFT (0x00000002u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD2_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD1_MASK (0x00000002u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD1_SHIFT (0x00000001u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD1_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_QTHRXCD0_MASK (0x00000001u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD0_SHIFT (0x00000000u)
#define EDMA3_CCRL_CCERRCLR_QTHRXCD0_RESETVAL (0x00000000u)

#define EDMA3_CCRL_CCERRCLR_RESETVAL     (0x00000000u)

/* EEVAL */

#define EDMA3_CCRL_EEVAL_SET_MASK        (0x00000002u)
#define EDMA3_CCRL_EEVAL_SET_SHIFT       (0x00000001u)
#define EDMA3_CCRL_EEVAL_SET_RESETVAL    (0x00000000u)

/*----SET Tokens----*/
#define EDMA3_CCRL_EEVAL_SET_SET         (0x00000001u)

#define EDMA3_CCRL_EEVAL_EVAL_MASK       (0x00000001u)
#define EDMA3_CCRL_EEVAL_EVAL_SHIFT      (0x00000000u)
#define EDMA3_CCRL_EEVAL_EVAL_RESETVAL   (0x00000000u)

/*----EVAL Tokens----*/
#define EDMA3_CCRL_EEVAL_EVAL_EVAL       (0x00000001u)

#define EDMA3_CCRL_EEVAL_RESETVAL        (0x00000000u)

/* DRAE */

#define EDMA3_CCRL_DRAE_E31_MASK         (0x80000000u)
#define EDMA3_CCRL_DRAE_E31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_DRAE_E31_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E30_MASK         (0x40000000u)
#define EDMA3_CCRL_DRAE_E30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_DRAE_E30_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E29_MASK         (0x20000000u)
#define EDMA3_CCRL_DRAE_E29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_DRAE_E29_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E28_MASK         (0x10000000u)
#define EDMA3_CCRL_DRAE_E28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_DRAE_E28_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E27_MASK         (0x08000000u)
#define EDMA3_CCRL_DRAE_E27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_DRAE_E27_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E26_MASK         (0x04000000u)
#define EDMA3_CCRL_DRAE_E26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_DRAE_E26_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E25_MASK         (0x02000000u)
#define EDMA3_CCRL_DRAE_E25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_DRAE_E25_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E24_MASK         (0x01000000u)
#define EDMA3_CCRL_DRAE_E24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_DRAE_E24_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E23_MASK         (0x00800000u)
#define EDMA3_CCRL_DRAE_E23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_DRAE_E23_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E22_MASK         (0x00400000u)
#define EDMA3_CCRL_DRAE_E22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_DRAE_E22_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E21_MASK         (0x00200000u)
#define EDMA3_CCRL_DRAE_E21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_DRAE_E21_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E20_MASK         (0x00100000u)
#define EDMA3_CCRL_DRAE_E20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_DRAE_E20_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E19_MASK         (0x00080000u)
#define EDMA3_CCRL_DRAE_E19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_DRAE_E19_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E18_MASK         (0x00040000u)
#define EDMA3_CCRL_DRAE_E18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_DRAE_E18_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E17_MASK         (0x00020000u)
#define EDMA3_CCRL_DRAE_E17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_DRAE_E17_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E16_MASK         (0x00010000u)
#define EDMA3_CCRL_DRAE_E16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_DRAE_E16_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E15_MASK         (0x00008000u)
#define EDMA3_CCRL_DRAE_E15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_DRAE_E15_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E14_MASK         (0x00004000u)
#define EDMA3_CCRL_DRAE_E14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_DRAE_E14_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E13_MASK         (0x00002000u)
#define EDMA3_CCRL_DRAE_E13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_DRAE_E13_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E12_MASK         (0x00001000u)
#define EDMA3_CCRL_DRAE_E12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_DRAE_E12_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E11_MASK         (0x00000800u)
#define EDMA3_CCRL_DRAE_E11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_DRAE_E11_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E10_MASK         (0x00000400u)
#define EDMA3_CCRL_DRAE_E10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_DRAE_E10_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_DRAE_E9_MASK          (0x00000200u)
#define EDMA3_CCRL_DRAE_E9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_DRAE_E9_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E8_MASK          (0x00000100u)
#define EDMA3_CCRL_DRAE_E8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_DRAE_E8_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_DRAE_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_DRAE_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_DRAE_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_DRAE_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_DRAE_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_DRAE_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_DRAE_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_DRAE_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_DRAE_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_DRAE_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_DRAE_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_DRAE_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_DRAE_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_DRAE_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_DRAE_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_DRAE_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DRAE_RESETVAL         (0x00000000u)

/* DRAEH */

#define EDMA3_CCRL_DRAEH_E63_MASK        (0x80000000u)
#define EDMA3_CCRL_DRAEH_E63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_DRAEH_E63_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E62_MASK        (0x40000000u)
#define EDMA3_CCRL_DRAEH_E62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_DRAEH_E62_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E61_MASK        (0x20000000u)
#define EDMA3_CCRL_DRAEH_E61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_DRAEH_E61_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E60_MASK        (0x10000000u)
#define EDMA3_CCRL_DRAEH_E60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_DRAEH_E60_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E59_MASK        (0x08000000u)
#define EDMA3_CCRL_DRAEH_E59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_DRAEH_E59_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E58_MASK        (0x04000000u)
#define EDMA3_CCRL_DRAEH_E58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_DRAEH_E58_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E57_MASK        (0x02000000u)
#define EDMA3_CCRL_DRAEH_E57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_DRAEH_E57_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E56_MASK        (0x01000000u)
#define EDMA3_CCRL_DRAEH_E56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_DRAEH_E56_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E55_MASK        (0x00800000u)
#define EDMA3_CCRL_DRAEH_E55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_DRAEH_E55_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E54_MASK        (0x00400000u)
#define EDMA3_CCRL_DRAEH_E54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_DRAEH_E54_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E53_MASK        (0x00200000u)
#define EDMA3_CCRL_DRAEH_E53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_DRAEH_E53_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E52_MASK        (0x00100000u)
#define EDMA3_CCRL_DRAEH_E52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_DRAEH_E52_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E51_MASK        (0x00080000u)
#define EDMA3_CCRL_DRAEH_E51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_DRAEH_E51_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E50_MASK        (0x00040000u)
#define EDMA3_CCRL_DRAEH_E50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_DRAEH_E50_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E49_MASK        (0x00020000u)
#define EDMA3_CCRL_DRAEH_E49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_DRAEH_E49_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E48_MASK        (0x00010000u)
#define EDMA3_CCRL_DRAEH_E48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_DRAEH_E48_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E47_MASK        (0x00008000u)
#define EDMA3_CCRL_DRAEH_E47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_DRAEH_E47_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E46_MASK        (0x00004000u)
#define EDMA3_CCRL_DRAEH_E46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_DRAEH_E46_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E45_MASK        (0x00002000u)
#define EDMA3_CCRL_DRAEH_E45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_DRAEH_E45_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E44_MASK        (0x00001000u)
#define EDMA3_CCRL_DRAEH_E44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_DRAEH_E44_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E43_MASK        (0x00000800u)
#define EDMA3_CCRL_DRAEH_E43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_DRAEH_E43_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E42_MASK        (0x00000400u)
#define EDMA3_CCRL_DRAEH_E42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_DRAEH_E42_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E41_MASK        (0x00000200u)
#define EDMA3_CCRL_DRAEH_E41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_DRAEH_E41_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E40_MASK        (0x00000100u)
#define EDMA3_CCRL_DRAEH_E40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_DRAEH_E40_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E39_MASK        (0x00000080u)
#define EDMA3_CCRL_DRAEH_E39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_DRAEH_E39_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E38_MASK        (0x00000040u)
#define EDMA3_CCRL_DRAEH_E38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_DRAEH_E38_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E37_MASK        (0x00000020u)
#define EDMA3_CCRL_DRAEH_E37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_DRAEH_E37_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E36_MASK        (0x00000010u)
#define EDMA3_CCRL_DRAEH_E36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_DRAEH_E36_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E35_MASK        (0x00000008u)
#define EDMA3_CCRL_DRAEH_E35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_DRAEH_E35_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E34_MASK        (0x00000004u)
#define EDMA3_CCRL_DRAEH_E34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_DRAEH_E34_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E33_MASK        (0x00000002u)
#define EDMA3_CCRL_DRAEH_E33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_DRAEH_E33_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_E32_MASK        (0x00000001u)
#define EDMA3_CCRL_DRAEH_E32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_DRAEH_E32_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_DRAEH_RESETVAL        (0x00000000u)

/* QRAE */

#define EDMA3_CCRL_QRAE_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E6_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E6_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E5_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E5_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E4_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E4_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E3_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E3_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E2_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E2_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E1_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E1_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_E0_MASK          (0x00000080u)
#define EDMA3_CCRL_QRAE_E0_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QRAE_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QRAE_RESERVED_MASK    (0x0000007Fu)
#define EDMA3_CCRL_QRAE_RESERVED_SHIFT   (0x00000000u)
#define EDMA3_CCRL_QRAE_RESERVED_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QRAE_RESETVAL         (0x00000000u)

/* QUEEVT_ENTRY */

#define EDMA3_CCRL_QUEEVT_ENTRY_RESV_MASK (0xFFFFFF00u)
#define EDMA3_CCRL_QUEEVT_ENTRY_RESV_SHIFT (0x00000008u)
#define EDMA3_CCRL_QUEEVT_ENTRY_RESV_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_SRC_MASK (0x000000C0u)
#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_SRC_SHIFT (0x00000006u)
#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_SRC_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_MASK (0x0000003Fu)
#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_SHIFT (0x00000000u)
#define EDMA3_CCRL_QUEEVT_ENTRY_EVT_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QUEEVT_ENTRY_RESETVAL (0x00000000u)

/* QSTAT */

#define EDMA3_CCRL_QSTAT_THRXD_MASK      (0x01000000u)
#define EDMA3_CCRL_QSTAT_THRXD_SHIFT     (0x00000018u)
#define EDMA3_CCRL_QSTAT_THRXD_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_QSTAT_RESERVED_MASK   (0x00600000u)
#define EDMA3_CCRL_QSTAT_RESERVED_SHIFT  (0x00000015u)
#define EDMA3_CCRL_QSTAT_RESERVED_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QSTAT_WM_MASK         (0x001F0000u)
#define EDMA3_CCRL_QSTAT_WM_SHIFT        (0x00000010u)
#define EDMA3_CCRL_QSTAT_WM_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QSTAT_NUMVAL_MASK     (0x00001F00u)
#define EDMA3_CCRL_QSTAT_NUMVAL_SHIFT    (0x00000008u)
#define EDMA3_CCRL_QSTAT_NUMVAL_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QSTAT_STRTPTR_MASK    (0x0000000Fu)
#define EDMA3_CCRL_QSTAT_STRTPTR_SHIFT   (0x00000000u)
#define EDMA3_CCRL_QSTAT_STRTPTR_RESETVAL (0x00000000u)

#define EDMA3_CCRL_QSTAT_RESETVAL        (0x00000000u)

/* QWMTHRA */

#define EDMA3_CCRL_QWMTHRA_Q3_MASK       (0x1F000000u)
#define EDMA3_CCRL_QWMTHRA_Q3_SHIFT      (0x00000018u)
#define EDMA3_CCRL_QWMTHRA_Q3_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRA_Q2_MASK       (0x001F0000u)
#define EDMA3_CCRL_QWMTHRA_Q2_SHIFT      (0x00000010u)
#define EDMA3_CCRL_QWMTHRA_Q2_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRA_Q1_MASK       (0x00001F00u)
#define EDMA3_CCRL_QWMTHRA_Q1_SHIFT      (0x00000008u)
#define EDMA3_CCRL_QWMTHRA_Q1_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRA_Q0_MASK       (0x0000001Fu)
#define EDMA3_CCRL_QWMTHRA_Q0_SHIFT      (0x00000000u)
#define EDMA3_CCRL_QWMTHRA_Q0_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRA_RESETVAL      (0x10101010u)

/* QWMTHRB */

#define EDMA3_CCRL_QWMTHRB_Q7_MASK       (0x1F000000u)
#define EDMA3_CCRL_QWMTHRB_Q7_SHIFT      (0x00000018u)
#define EDMA3_CCRL_QWMTHRB_Q7_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRB_Q6_MASK       (0x001F0000u)
#define EDMA3_CCRL_QWMTHRB_Q6_SHIFT      (0x00000010u)
#define EDMA3_CCRL_QWMTHRB_Q6_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRB_Q5_MASK       (0x00001F00u)
#define EDMA3_CCRL_QWMTHRB_Q5_SHIFT      (0x00000008u)
#define EDMA3_CCRL_QWMTHRB_Q5_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRB_Q4_MASK       (0x0000001Fu)
#define EDMA3_CCRL_QWMTHRB_Q4_SHIFT      (0x00000000u)
#define EDMA3_CCRL_QWMTHRB_Q4_RESETVAL   (0x00000010u)

#define EDMA3_CCRL_QWMTHRB_RESETVAL      (0x10101010u)

/* CCSTAT */

#define EDMA3_CCRL_CCSTAT_QUEACTV7_MASK  (0x00800000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV7_SHIFT (0x00000017u)
#define EDMA3_CCRL_CCSTAT_QUEACTV7_RESETVAL (0x00000000u)

/*----QUEACTV7 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV7_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV7_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV6_MASK  (0x00400000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV6_SHIFT (0x00000016u)
#define EDMA3_CCRL_CCSTAT_QUEACTV6_RESETVAL (0x00000000u)

/*----QUEACTV6 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV6_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV6_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV5_MASK  (0x00200000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV5_SHIFT (0x00000015u)
#define EDMA3_CCRL_CCSTAT_QUEACTV5_RESETVAL (0x00000000u)

/*----QUEACTV5 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV5_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV5_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV4_MASK  (0x00100000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV4_SHIFT (0x00000014u)
#define EDMA3_CCRL_CCSTAT_QUEACTV4_RESETVAL (0x00000000u)

/*----QUEACTV4 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV4_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV4_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV3_MASK  (0x00080000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV3_SHIFT (0x00000013u)
#define EDMA3_CCRL_CCSTAT_QUEACTV3_RESETVAL (0x00000000u)

/*----QUEACTV3 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV3_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV3_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV2_MASK  (0x00040000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV2_SHIFT (0x00000012u)
#define EDMA3_CCRL_CCSTAT_QUEACTV2_RESETVAL (0x00000000u)

/*----QUEACTV2 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV2_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV2_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV1_MASK  (0x00020000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV1_SHIFT (0x00000011u)
#define EDMA3_CCRL_CCSTAT_QUEACTV1_RESETVAL (0x00000000u)

/*----QUEACTV1 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV1_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV1_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QUEACTV0_MASK  (0x00010000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV0_SHIFT (0x00000010u)
#define EDMA3_CCRL_CCSTAT_QUEACTV0_RESETVAL (0x00000000u)

/*----QUEACTV0 Tokens----*/
#define EDMA3_CCRL_CCSTAT_QUEACTV0_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QUEACTV0_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_COMPACT_MASK   (0x00003F00u)
#define EDMA3_CCRL_CCSTAT_COMPACT_SHIFT  (0x00000008u)
#define EDMA3_CCRL_CCSTAT_COMPACT_RESETVAL (0x00000000u)

/*----COMPACT Tokens----*/
#define EDMA3_CCRL_CCSTAT_COMPACT_NONE   (0x00000000u)

#define EDMA3_CCRL_CCSTAT_ACTV_MASK      (0x00000010u)
#define EDMA3_CCRL_CCSTAT_ACTV_SHIFT     (0x00000004u)
#define EDMA3_CCRL_CCSTAT_ACTV_RESETVAL  (0x00000000u)

/*----ACTV Tokens----*/
#define EDMA3_CCRL_CCSTAT_ACTV_IDLE      (0x00000000u)
#define EDMA3_CCRL_CCSTAT_ACTV_BUSY      (0x00000001u)

#define EDMA3_CCRL_CCSTAT_TRACTV_MASK    (0x00000004u)
#define EDMA3_CCRL_CCSTAT_TRACTV_SHIFT   (0x00000002u)
#define EDMA3_CCRL_CCSTAT_TRACTV_RESETVAL (0x00000000u)

/*----TRACTV Tokens----*/
#define EDMA3_CCRL_CCSTAT_TRACTV_NONE    (0x00000000u)
#define EDMA3_CCRL_CCSTAT_TRACTV_ACTIVE  (0x00000001u)

#define EDMA3_CCRL_CCSTAT_QEVTACTV_MASK  (0x00000002u)
#define EDMA3_CCRL_CCSTAT_QEVTACTV_SHIFT (0x00000001u)
#define EDMA3_CCRL_CCSTAT_QEVTACTV_RESETVAL (0x00000000u)

/*----QEVTACTV Tokens----*/
#define EDMA3_CCRL_CCSTAT_QEVTACTV_NONE  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_QEVTACTV_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_EVTACTV_MASK   (0x00000001u)
#define EDMA3_CCRL_CCSTAT_EVTACTV_SHIFT  (0x00000000u)
#define EDMA3_CCRL_CCSTAT_EVTACTV_RESETVAL (0x00000000u)

/*----EVTACTV Tokens----*/
#define EDMA3_CCRL_CCSTAT_EVTACTV_NONE   (0x00000000u)
#define EDMA3_CCRL_CCSTAT_EVTACTV_ACTIVE (0x00000001u)

#define EDMA3_CCRL_CCSTAT_RESETVAL       (0x00000000u)

/* AETCTL */

#define EDMA3_CCRL_AETCTL_EN_MASK        (0x80000000u)
#define EDMA3_CCRL_AETCTL_EN_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_AETCTL_EN_RESETVAL    (0x00000000u)

/*----EN Tokens----*/
#define EDMA3_CCRL_AETCTL_EN_DISABLE     (0x00000000u)
#define EDMA3_CCRL_AETCTL_EN_ENABLE      (0x00000001u)

#define EDMA3_CCRL_AETCTL_ENDINT_MASK    (0x00003F00u)
#define EDMA3_CCRL_AETCTL_ENDINT_SHIFT   (0x00000008u)
#define EDMA3_CCRL_AETCTL_ENDINT_RESETVAL (0x00000000u)

#define EDMA3_CCRL_AETCTL_TYPE_MASK      (0x00000040u)
#define EDMA3_CCRL_AETCTL_TYPE_SHIFT     (0x00000006u)
#define EDMA3_CCRL_AETCTL_TYPE_RESETVAL  (0x00000000u)

/*----TYPE Tokens----*/
#define EDMA3_CCRL_AETCTL_TYPE_DMA       (0x00000000u)
#define EDMA3_CCRL_AETCTL_TYPE_QDMA      (0x00000001u)

#define EDMA3_CCRL_AETCTL_STRTEVT_MASK   (0x0000003Fu)
#define EDMA3_CCRL_AETCTL_STRTEVT_SHIFT  (0x00000000u)
#define EDMA3_CCRL_AETCTL_STRTEVT_RESETVAL (0x00000000u)

#define EDMA3_CCRL_AETCTL_RESETVAL       (0x00000000u)

/* AETSTAT */

#define EDMA3_CCRL_AETSTAT_STAT_MASK     (0x00000001u)
#define EDMA3_CCRL_AETSTAT_STAT_SHIFT    (0x00000000u)
#define EDMA3_CCRL_AETSTAT_STAT_RESETVAL (0x00000000u)

/*----STAT Tokens----*/
#define EDMA3_CCRL_AETSTAT_STAT_LOW      (0x00000000u)
#define EDMA3_CCRL_AETSTAT_STAT_HIGH     (0x00000001u)

#define EDMA3_CCRL_AETSTAT_RESETVAL      (0x00000000u)

/* AETCMD */

#define EDMA3_CCRL_AETCMD_CLR_MASK       (0x00000001u)
#define EDMA3_CCRL_AETCMD_CLR_SHIFT      (0x00000000u)
#define EDMA3_CCRL_AETCMD_CLR_RESETVAL   (0x00000000u)

/*----CLR Tokens----*/
#define EDMA3_CCRL_AETCMD_CLR_CLEAR      (0x00000001u)

#define EDMA3_CCRL_AETCMD_RESETVAL       (0x00000000u)

/* MPFAR */

#define EDMA3_CCRL_MPFAR_FADDR_MASK      (0xFFFFFFFFu)
#define EDMA3_CCRL_MPFAR_FADDR_SHIFT     (0x00000000u)
#define EDMA3_CCRL_MPFAR_FADDR_RESETVAL  (0x00000000u)

#define EDMA3_CCRL_MPFAR_RESETVAL        (0x00000000u)

/* MPFSR */

#define EDMA3_CCRL_MPFSR_FID_MASK        (0x00001E00u)
#define EDMA3_CCRL_MPFSR_FID_SHIFT       (0x00000009u)
#define EDMA3_CCRL_MPFSR_FID_RESETVAL    (0x00000009u)

#define EDMA3_CCRL_MPFSR_SECE_MASK       (0x00000080u)
#define EDMA3_CCRL_MPFSR_SECE_SHIFT      (0x00000007u)
#define EDMA3_CCRL_MPFSR_SECE_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_MPFSR_SRE_MASK        (0x00000020u)
#define EDMA3_CCRL_MPFSR_SRE_SHIFT       (0x00000005u)
#define EDMA3_CCRL_MPFSR_SRE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_SWE_MASK        (0x00000010u)
#define EDMA3_CCRL_MPFSR_SWE_SHIFT       (0x00000004u)
#define EDMA3_CCRL_MPFSR_SWE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_SXE_MASK        (0x00000008u)
#define EDMA3_CCRL_MPFSR_SXE_SHIFT       (0x00000003u)
#define EDMA3_CCRL_MPFSR_SXE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_URE_MASK        (0x00000004u)
#define EDMA3_CCRL_MPFSR_URE_SHIFT       (0x00000002u)
#define EDMA3_CCRL_MPFSR_URE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_UWE_MASK        (0x00000002u)
#define EDMA3_CCRL_MPFSR_UWE_SHIFT       (0x00000001u)
#define EDMA3_CCRL_MPFSR_UWE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_UXE_MASK        (0x00000001u)
#define EDMA3_CCRL_MPFSR_UXE_SHIFT       (0x00000000u)
#define EDMA3_CCRL_MPFSR_UXE_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPFSR_RESETVAL        (0x00001200u)

/* MPFCR */

#define EDMA3_CCRL_MPFCR_MPFCLR_MASK     (0x00000001u)
#define EDMA3_CCRL_MPFCR_MPFCLR_SHIFT    (0x00000000u)
#define EDMA3_CCRL_MPFCR_MPFCLR_RESETVAL (0x00000000u)

#define EDMA3_CCRL_MPFCR_RESETVAL        (0x00000000u)

/* MPPAG */

#define EDMA3_CCRL_MPPAG_AID5_MASK       (0x00008000u)
#define EDMA3_CCRL_MPPAG_AID5_SHIFT      (0x0000000Fu)
#define EDMA3_CCRL_MPPAG_AID5_RESETVAL   (0x00000000u)

/*----AID5 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID5_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID5_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_AID4_MASK       (0x00004000u)
#define EDMA3_CCRL_MPPAG_AID4_SHIFT      (0x0000000Eu)
#define EDMA3_CCRL_MPPAG_AID4_RESETVAL   (0x00000000u)

/*----AID4 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID4_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID4_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_AID3_MASK       (0x00002000u)
#define EDMA3_CCRL_MPPAG_AID3_SHIFT      (0x0000000Du)
#define EDMA3_CCRL_MPPAG_AID3_RESETVAL   (0x00000000u)

/*----AID3 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID3_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID3_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_AID2_MASK       (0x00001000u)
#define EDMA3_CCRL_MPPAG_AID2_SHIFT      (0x0000000Cu)
#define EDMA3_CCRL_MPPAG_AID2_RESETVAL   (0x00000000u)

/*----AID2 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID2_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID2_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_AID1_MASK       (0x00000800u)
#define EDMA3_CCRL_MPPAG_AID1_SHIFT      (0x0000000Bu)
#define EDMA3_CCRL_MPPAG_AID1_RESETVAL   (0x00000000u)

/*----AID1 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID1_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID1_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_AID0_MASK       (0x00000400u)
#define EDMA3_CCRL_MPPAG_AID0_SHIFT      (0x0000000Au)
#define EDMA3_CCRL_MPPAG_AID0_RESETVAL   (0x00000000u)

/*----AID0 Tokens----*/
#define EDMA3_CCRL_MPPAG_AID0_BLOCK      (0x00000000u)
#define EDMA3_CCRL_MPPAG_AID0_PERMIT     (0x00000001u)

#define EDMA3_CCRL_MPPAG_EXT_MASK        (0x00000200u)
#define EDMA3_CCRL_MPPAG_EXT_SHIFT       (0x00000009u)
#define EDMA3_CCRL_MPPAG_EXT_RESETVAL    (0x00000000u)

/*----EXT Tokens----*/
#define EDMA3_CCRL_MPPAG_EXT_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPAG_EXT_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPAG_LCL_MASK        (0x00000100u)
#define EDMA3_CCRL_MPPAG_LCL_SHIFT       (0x00000008u)
#define EDMA3_CCRL_MPPAG_LCL_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_MPPAG_NS_MASK         (0x00000080u)
#define EDMA3_CCRL_MPPAG_NS_SHIFT        (0x00000007u)
#define EDMA3_CCRL_MPPAG_NS_RESETVAL     (0x00000000u)

/*----NS Tokens----*/
#define EDMA3_CCRL_MPPAG_NS_SECURE       (0x00000000u)
#define EDMA3_CCRL_MPPAG_NS_NONSECURE    (0x00000001u)

#define EDMA3_CCRL_MPPAG_EMU_MASK        (0x00000040u)
#define EDMA3_CCRL_MPPAG_EMU_SHIFT       (0x00000006u)
#define EDMA3_CCRL_MPPAG_EMU_RESETVAL    (0x00000000u)

/*----EMU Tokens----*/
#define EDMA3_CCRL_MPPAG_EMU_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPAG_EMU_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPAG_SR_MASK         (0x00000020u)
#define EDMA3_CCRL_MPPAG_SR_SHIFT        (0x00000005u)
#define EDMA3_CCRL_MPPAG_SR_RESETVAL     (0x00000000u)

/*----SR Tokens----*/
#define EDMA3_CCRL_MPPAG_SR_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_SR_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_SW_MASK         (0x00000010u)
#define EDMA3_CCRL_MPPAG_SW_SHIFT        (0x00000004u)
#define EDMA3_CCRL_MPPAG_SW_RESETVAL     (0x00000000u)

/*----SW Tokens----*/
#define EDMA3_CCRL_MPPAG_SW_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_SW_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_SX_MASK         (0x00000008u)
#define EDMA3_CCRL_MPPAG_SX_SHIFT        (0x00000003u)
#define EDMA3_CCRL_MPPAG_SX_RESETVAL     (0x00000000u)

/*----SX Tokens----*/
#define EDMA3_CCRL_MPPAG_SX_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_SX_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_UR_MASK         (0x00000004u)
#define EDMA3_CCRL_MPPAG_UR_SHIFT        (0x00000002u)
#define EDMA3_CCRL_MPPAG_UR_RESETVAL     (0x00000000u)

/*----UR Tokens----*/
#define EDMA3_CCRL_MPPAG_UR_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_UR_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_UW_MASK         (0x00000002u)
#define EDMA3_CCRL_MPPAG_UW_SHIFT        (0x00000001u)
#define EDMA3_CCRL_MPPAG_UW_RESETVAL     (0x00000000u)

/*----UW Tokens----*/
#define EDMA3_CCRL_MPPAG_UW_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_UW_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_UX_MASK         (0x00000001u)
#define EDMA3_CCRL_MPPAG_UX_SHIFT        (0x00000000u)
#define EDMA3_CCRL_MPPAG_UX_RESETVAL     (0x00000000u)

/*----UX Tokens----*/
#define EDMA3_CCRL_MPPAG_UX_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPAG_UX_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPAG_RESETVAL        (0x00000000u)

/* MPPA */

#define EDMA3_CCRL_MPPA_AID5_MASK        (0x00008000u)
#define EDMA3_CCRL_MPPA_AID5_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_MPPA_AID5_RESETVAL    (0x00000000u)

/*----AID5 Tokens----*/
#define EDMA3_CCRL_MPPA_AID5_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID5_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_AID4_MASK        (0x00004000u)
#define EDMA3_CCRL_MPPA_AID4_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_MPPA_AID4_RESETVAL    (0x00000000u)

/*----AID4 Tokens----*/
#define EDMA3_CCRL_MPPA_AID4_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID4_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_AID3_MASK        (0x00002000u)
#define EDMA3_CCRL_MPPA_AID3_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_MPPA_AID3_RESETVAL    (0x00000000u)

/*----AID3 Tokens----*/
#define EDMA3_CCRL_MPPA_AID3_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID3_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_AID2_MASK        (0x00001000u)
#define EDMA3_CCRL_MPPA_AID2_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_MPPA_AID2_RESETVAL    (0x00000000u)

/*----AID2 Tokens----*/
#define EDMA3_CCRL_MPPA_AID2_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID2_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_AID1_MASK        (0x00000800u)
#define EDMA3_CCRL_MPPA_AID1_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_MPPA_AID1_RESETVAL    (0x00000000u)

/*----AID1 Tokens----*/
#define EDMA3_CCRL_MPPA_AID1_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID1_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_AID0_MASK        (0x00000400u)
#define EDMA3_CCRL_MPPA_AID0_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_MPPA_AID0_RESETVAL    (0x00000000u)

/*----AID0 Tokens----*/
#define EDMA3_CCRL_MPPA_AID0_BLOCK       (0x00000000u)
#define EDMA3_CCRL_MPPA_AID0_PERMIT      (0x00000001u)

#define EDMA3_CCRL_MPPA_EXT_MASK         (0x00000200u)
#define EDMA3_CCRL_MPPA_EXT_SHIFT        (0x00000009u)
#define EDMA3_CCRL_MPPA_EXT_RESETVAL     (0x00000000u)

/*----EXT Tokens----*/
#define EDMA3_CCRL_MPPA_EXT_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPA_EXT_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPA_LCL_MASK         (0x00000100u)
#define EDMA3_CCRL_MPPA_LCL_SHIFT        (0x00000008u)
#define EDMA3_CCRL_MPPA_LCL_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_MPPA_NS_MASK          (0x00000080u)
#define EDMA3_CCRL_MPPA_NS_SHIFT         (0x00000007u)
#define EDMA3_CCRL_MPPA_NS_RESETVAL      (0x00000000u)

/*----NS Tokens----*/
#define EDMA3_CCRL_MPPA_NS_SECURE        (0x00000000u)
#define EDMA3_CCRL_MPPA_NS_NONSECURE     (0x00000001u)

#define EDMA3_CCRL_MPPA_EMU_MASK         (0x00000040u)
#define EDMA3_CCRL_MPPA_EMU_SHIFT        (0x00000006u)
#define EDMA3_CCRL_MPPA_EMU_RESETVAL     (0x00000000u)

/*----EMU Tokens----*/
#define EDMA3_CCRL_MPPA_EMU_BLOCK        (0x00000000u)
#define EDMA3_CCRL_MPPA_EMU_PERMIT       (0x00000001u)

#define EDMA3_CCRL_MPPA_SR_MASK          (0x00000020u)
#define EDMA3_CCRL_MPPA_SR_SHIFT         (0x00000005u)
#define EDMA3_CCRL_MPPA_SR_RESETVAL      (0x00000000u)

/*----SR Tokens----*/
#define EDMA3_CCRL_MPPA_SR_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_SR_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_SW_MASK          (0x00000010u)
#define EDMA3_CCRL_MPPA_SW_SHIFT         (0x00000004u)
#define EDMA3_CCRL_MPPA_SW_RESETVAL      (0x00000000u)

/*----SW Tokens----*/
#define EDMA3_CCRL_MPPA_SW_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_SW_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_SX_MASK          (0x00000008u)
#define EDMA3_CCRL_MPPA_SX_SHIFT         (0x00000003u)
#define EDMA3_CCRL_MPPA_SX_RESETVAL      (0x00000000u)

/*----SX Tokens----*/
#define EDMA3_CCRL_MPPA_SX_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_SX_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_UR_MASK          (0x00000004u)
#define EDMA3_CCRL_MPPA_UR_SHIFT         (0x00000002u)
#define EDMA3_CCRL_MPPA_UR_RESETVAL      (0x00000000u)

/*----UR Tokens----*/
#define EDMA3_CCRL_MPPA_UR_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_UR_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_UW_MASK          (0x00000002u)
#define EDMA3_CCRL_MPPA_UW_SHIFT         (0x00000001u)
#define EDMA3_CCRL_MPPA_UW_RESETVAL      (0x00000000u)

/*----UW Tokens----*/
#define EDMA3_CCRL_MPPA_UW_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_UW_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_UX_MASK          (0x00000001u)
#define EDMA3_CCRL_MPPA_UX_SHIFT         (0x00000000u)
#define EDMA3_CCRL_MPPA_UX_RESETVAL      (0x00000000u)

/*----UX Tokens----*/
#define EDMA3_CCRL_MPPA_UX_BLOCK         (0x00000000u)
#define EDMA3_CCRL_MPPA_UX_PERMIT        (0x00000001u)

#define EDMA3_CCRL_MPPA_RESETVAL         (0x00000000u)

/* ER */

#define EDMA3_CCRL_ER_E31_MASK           (0x80000000u)
#define EDMA3_CCRL_ER_E31_SHIFT          (0x0000001Fu)
#define EDMA3_CCRL_ER_E31_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E30_MASK           (0x40000000u)
#define EDMA3_CCRL_ER_E30_SHIFT          (0x0000001Eu)
#define EDMA3_CCRL_ER_E30_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E29_MASK           (0x20000000u)
#define EDMA3_CCRL_ER_E29_SHIFT          (0x0000001Du)
#define EDMA3_CCRL_ER_E29_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E28_MASK           (0x10000000u)
#define EDMA3_CCRL_ER_E28_SHIFT          (0x0000001Cu)
#define EDMA3_CCRL_ER_E28_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E27_MASK           (0x08000000u)
#define EDMA3_CCRL_ER_E27_SHIFT          (0x0000001Bu)
#define EDMA3_CCRL_ER_E27_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E26_MASK           (0x04000000u)
#define EDMA3_CCRL_ER_E26_SHIFT          (0x0000001Au)
#define EDMA3_CCRL_ER_E26_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E25_MASK           (0x02000000u)
#define EDMA3_CCRL_ER_E25_SHIFT          (0x00000019u)
#define EDMA3_CCRL_ER_E25_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E24_MASK           (0x01000000u)
#define EDMA3_CCRL_ER_E24_SHIFT          (0x00000018u)
#define EDMA3_CCRL_ER_E24_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E23_MASK           (0x00800000u)
#define EDMA3_CCRL_ER_E23_SHIFT          (0x00000017u)
#define EDMA3_CCRL_ER_E23_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E22_MASK           (0x00400000u)
#define EDMA3_CCRL_ER_E22_SHIFT          (0x00000016u)
#define EDMA3_CCRL_ER_E22_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E21_MASK           (0x00200000u)
#define EDMA3_CCRL_ER_E21_SHIFT          (0x00000015u)
#define EDMA3_CCRL_ER_E21_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E20_MASK           (0x00100000u)
#define EDMA3_CCRL_ER_E20_SHIFT          (0x00000014u)
#define EDMA3_CCRL_ER_E20_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E19_MASK           (0x00080000u)
#define EDMA3_CCRL_ER_E19_SHIFT          (0x00000013u)
#define EDMA3_CCRL_ER_E19_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E18_MASK           (0x00040000u)
#define EDMA3_CCRL_ER_E18_SHIFT          (0x00000012u)
#define EDMA3_CCRL_ER_E18_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E17_MASK           (0x00020000u)
#define EDMA3_CCRL_ER_E17_SHIFT          (0x00000011u)
#define EDMA3_CCRL_ER_E17_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E16_MASK           (0x00010000u)
#define EDMA3_CCRL_ER_E16_SHIFT          (0x00000010u)
#define EDMA3_CCRL_ER_E16_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E15_MASK           (0x00008000u)
#define EDMA3_CCRL_ER_E15_SHIFT          (0x0000000Fu)
#define EDMA3_CCRL_ER_E15_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E14_MASK           (0x00004000u)
#define EDMA3_CCRL_ER_E14_SHIFT          (0x0000000Eu)
#define EDMA3_CCRL_ER_E14_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E13_MASK           (0x00002000u)
#define EDMA3_CCRL_ER_E13_SHIFT          (0x0000000Du)
#define EDMA3_CCRL_ER_E13_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E12_MASK           (0x00001000u)
#define EDMA3_CCRL_ER_E12_SHIFT          (0x0000000Cu)
#define EDMA3_CCRL_ER_E12_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E11_MASK           (0x00000800u)
#define EDMA3_CCRL_ER_E11_SHIFT          (0x0000000Bu)
#define EDMA3_CCRL_ER_E11_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E10_MASK           (0x00000400u)
#define EDMA3_CCRL_ER_E10_SHIFT          (0x0000000Au)
#define EDMA3_CCRL_ER_E10_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_E9_MASK            (0x00000200u)
#define EDMA3_CCRL_ER_E9_SHIFT           (0x00000009u)
#define EDMA3_CCRL_ER_E9_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E8_MASK            (0x00000100u)
#define EDMA3_CCRL_ER_E8_SHIFT           (0x00000008u)
#define EDMA3_CCRL_ER_E8_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E7_MASK            (0x00000080u)
#define EDMA3_CCRL_ER_E7_SHIFT           (0x00000007u)
#define EDMA3_CCRL_ER_E7_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E6_MASK            (0x00000040u)
#define EDMA3_CCRL_ER_E6_SHIFT           (0x00000006u)
#define EDMA3_CCRL_ER_E6_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E5_MASK            (0x00000020u)
#define EDMA3_CCRL_ER_E5_SHIFT           (0x00000005u)
#define EDMA3_CCRL_ER_E5_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E4_MASK            (0x00000010u)
#define EDMA3_CCRL_ER_E4_SHIFT           (0x00000004u)
#define EDMA3_CCRL_ER_E4_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E3_MASK            (0x00000008u)
#define EDMA3_CCRL_ER_E3_SHIFT           (0x00000003u)
#define EDMA3_CCRL_ER_E3_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E2_MASK            (0x00000004u)
#define EDMA3_CCRL_ER_E2_SHIFT           (0x00000002u)
#define EDMA3_CCRL_ER_E2_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E1_MASK            (0x00000002u)
#define EDMA3_CCRL_ER_E1_SHIFT           (0x00000001u)
#define EDMA3_CCRL_ER_E1_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_E0_MASK            (0x00000001u)
#define EDMA3_CCRL_ER_E0_SHIFT           (0x00000000u)
#define EDMA3_CCRL_ER_E0_RESETVAL        (0x00000000u)

#define EDMA3_CCRL_ER_RESETVAL           (0x00000000u)

/* ERH */

#define EDMA3_CCRL_ERH_E63_MASK          (0x80000000u)
#define EDMA3_CCRL_ERH_E63_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_ERH_E63_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E62_MASK          (0x40000000u)
#define EDMA3_CCRL_ERH_E62_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_ERH_E62_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E61_MASK          (0x20000000u)
#define EDMA3_CCRL_ERH_E61_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_ERH_E61_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E60_MASK          (0x10000000u)
#define EDMA3_CCRL_ERH_E60_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_ERH_E60_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E59_MASK          (0x08000000u)
#define EDMA3_CCRL_ERH_E59_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_ERH_E59_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E58_MASK          (0x04000000u)
#define EDMA3_CCRL_ERH_E58_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_ERH_E58_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E57_MASK          (0x02000000u)
#define EDMA3_CCRL_ERH_E57_SHIFT         (0x00000019u)
#define EDMA3_CCRL_ERH_E57_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E56_MASK          (0x01000000u)
#define EDMA3_CCRL_ERH_E56_SHIFT         (0x00000018u)
#define EDMA3_CCRL_ERH_E56_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E55_MASK          (0x00800000u)
#define EDMA3_CCRL_ERH_E55_SHIFT         (0x00000017u)
#define EDMA3_CCRL_ERH_E55_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E54_MASK          (0x00400000u)
#define EDMA3_CCRL_ERH_E54_SHIFT         (0x00000016u)
#define EDMA3_CCRL_ERH_E54_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E53_MASK          (0x00200000u)
#define EDMA3_CCRL_ERH_E53_SHIFT         (0x00000015u)
#define EDMA3_CCRL_ERH_E53_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E52_MASK          (0x00100000u)
#define EDMA3_CCRL_ERH_E52_SHIFT         (0x00000014u)
#define EDMA3_CCRL_ERH_E52_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E51_MASK          (0x00080000u)
#define EDMA3_CCRL_ERH_E51_SHIFT         (0x00000013u)
#define EDMA3_CCRL_ERH_E51_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E50_MASK          (0x00040000u)
#define EDMA3_CCRL_ERH_E50_SHIFT         (0x00000012u)
#define EDMA3_CCRL_ERH_E50_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E49_MASK          (0x00020000u)
#define EDMA3_CCRL_ERH_E49_SHIFT         (0x00000011u)
#define EDMA3_CCRL_ERH_E49_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E48_MASK          (0x00010000u)
#define EDMA3_CCRL_ERH_E48_SHIFT         (0x00000010u)
#define EDMA3_CCRL_ERH_E48_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E47_MASK          (0x00008000u)
#define EDMA3_CCRL_ERH_E47_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_ERH_E47_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E46_MASK          (0x00004000u)
#define EDMA3_CCRL_ERH_E46_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_ERH_E46_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E45_MASK          (0x00002000u)
#define EDMA3_CCRL_ERH_E45_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_ERH_E45_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E44_MASK          (0x00001000u)
#define EDMA3_CCRL_ERH_E44_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_ERH_E44_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E43_MASK          (0x00000800u)
#define EDMA3_CCRL_ERH_E43_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_ERH_E43_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E42_MASK          (0x00000400u)
#define EDMA3_CCRL_ERH_E42_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_ERH_E42_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E41_MASK          (0x00000200u)
#define EDMA3_CCRL_ERH_E41_SHIFT         (0x00000009u)
#define EDMA3_CCRL_ERH_E41_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E40_MASK          (0x00000100u)
#define EDMA3_CCRL_ERH_E40_SHIFT         (0x00000008u)
#define EDMA3_CCRL_ERH_E40_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E39_MASK          (0x00000080u)
#define EDMA3_CCRL_ERH_E39_SHIFT         (0x00000007u)
#define EDMA3_CCRL_ERH_E39_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E38_MASK          (0x00000040u)
#define EDMA3_CCRL_ERH_E38_SHIFT         (0x00000006u)
#define EDMA3_CCRL_ERH_E38_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E37_MASK          (0x00000020u)
#define EDMA3_CCRL_ERH_E37_SHIFT         (0x00000005u)
#define EDMA3_CCRL_ERH_E37_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E36_MASK          (0x00000010u)
#define EDMA3_CCRL_ERH_E36_SHIFT         (0x00000004u)
#define EDMA3_CCRL_ERH_E36_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E35_MASK          (0x00000008u)
#define EDMA3_CCRL_ERH_E35_SHIFT         (0x00000003u)
#define EDMA3_CCRL_ERH_E35_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E34_MASK          (0x00000004u)
#define EDMA3_CCRL_ERH_E34_SHIFT         (0x00000002u)
#define EDMA3_CCRL_ERH_E34_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E33_MASK          (0x00000002u)
#define EDMA3_CCRL_ERH_E33_SHIFT         (0x00000001u)
#define EDMA3_CCRL_ERH_E33_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_E32_MASK          (0x00000001u)
#define EDMA3_CCRL_ERH_E32_SHIFT         (0x00000000u)
#define EDMA3_CCRL_ERH_E32_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_RESETVAL          (0x00000000u)

/* ECR */

#define EDMA3_CCRL_ECR_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_ECR_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_ECR_E31_RESETVAL      (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_ECR_E31_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_ECR_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_ECR_E30_RESETVAL      (0x00000000u)

/*----E30 Tokens----*/
#define EDMA3_CCRL_ECR_E30_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_ECR_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_ECR_E29_RESETVAL      (0x00000000u)

/*----E29 Tokens----*/
#define EDMA3_CCRL_ECR_E29_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_ECR_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_ECR_E28_RESETVAL      (0x00000000u)

/*----E28 Tokens----*/
#define EDMA3_CCRL_ECR_E28_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_ECR_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_ECR_E27_RESETVAL      (0x00000000u)

/*----E27 Tokens----*/
#define EDMA3_CCRL_ECR_E27_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_ECR_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_ECR_E26_RESETVAL      (0x00000000u)

/*----E26 Tokens----*/
#define EDMA3_CCRL_ECR_E26_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_ECR_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_ECR_E25_RESETVAL      (0x00000000u)

/*----E25 Tokens----*/
#define EDMA3_CCRL_ECR_E25_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_ECR_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_ECR_E24_RESETVAL      (0x00000000u)

/*----E24 Tokens----*/
#define EDMA3_CCRL_ECR_E24_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_ECR_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_ECR_E23_RESETVAL      (0x00000000u)

/*----E23 Tokens----*/
#define EDMA3_CCRL_ECR_E23_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_ECR_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_ECR_E22_RESETVAL      (0x00000000u)

/*----E22 Tokens----*/
#define EDMA3_CCRL_ECR_E22_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_ECR_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_ECR_E21_RESETVAL      (0x00000000u)

/*----E21 Tokens----*/
#define EDMA3_CCRL_ECR_E21_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_ECR_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_ECR_E20_RESETVAL      (0x00000000u)

/*----E20 Tokens----*/
#define EDMA3_CCRL_ECR_E20_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_ECR_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_ECR_E19_RESETVAL      (0x00000000u)

/*----E19 Tokens----*/
#define EDMA3_CCRL_ECR_E19_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_ECR_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_ECR_E18_RESETVAL      (0x00000000u)

/*----E18 Tokens----*/
#define EDMA3_CCRL_ECR_E18_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_ECR_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_ECR_E17_RESETVAL      (0x00000000u)

/*----E17 Tokens----*/
#define EDMA3_CCRL_ECR_E17_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_ECR_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_ECR_E16_RESETVAL      (0x00000000u)

/*----E16 Tokens----*/
#define EDMA3_CCRL_ECR_E16_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_ECR_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_ECR_E15_RESETVAL      (0x00000000u)

/*----E15 Tokens----*/
#define EDMA3_CCRL_ECR_E15_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_ECR_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_ECR_E14_RESETVAL      (0x00000000u)

/*----E14 Tokens----*/
#define EDMA3_CCRL_ECR_E14_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_ECR_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_ECR_E13_RESETVAL      (0x00000000u)

/*----E13 Tokens----*/
#define EDMA3_CCRL_ECR_E13_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_ECR_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_ECR_E12_RESETVAL      (0x00000000u)

/*----E12 Tokens----*/
#define EDMA3_CCRL_ECR_E12_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_ECR_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_ECR_E11_RESETVAL      (0x00000000u)

/*----E11 Tokens----*/
#define EDMA3_CCRL_ECR_E11_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_ECR_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_ECR_E10_RESETVAL      (0x00000000u)

/*----E10 Tokens----*/
#define EDMA3_CCRL_ECR_E10_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ECR_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_ECR_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_ECR_E9_RESETVAL       (0x00000000u)

/*----E9 Tokens----*/
#define EDMA3_CCRL_ECR_E9_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_ECR_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_ECR_E8_RESETVAL       (0x00000000u)

/*----E8 Tokens----*/
#define EDMA3_CCRL_ECR_E8_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_ECR_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_ECR_E7_RESETVAL       (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_ECR_E7_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_ECR_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_ECR_E6_RESETVAL       (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_ECR_E6_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_ECR_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_ECR_E5_RESETVAL       (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_ECR_E5_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_ECR_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_ECR_E4_RESETVAL       (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_ECR_E4_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_ECR_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_ECR_E3_RESETVAL       (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_ECR_E3_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_ECR_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_ECR_E2_RESETVAL       (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_ECR_E2_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_ECR_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_ECR_E1_RESETVAL       (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_ECR_E1_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_ECR_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_ECR_E0_RESETVAL       (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_ECR_E0_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ECR_RESETVAL          (0x00000000u)

/* ECRH */

#define EDMA3_CCRL_ECRH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_ECRH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_ECRH_E63_RESETVAL     (0x00000000u)

/*----E63 Tokens----*/
#define EDMA3_CCRL_ECRH_E63_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_ECRH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_ECRH_E62_RESETVAL     (0x00000000u)

/*----E62 Tokens----*/
#define EDMA3_CCRL_ECRH_E62_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_ECRH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_ECRH_E61_RESETVAL     (0x00000000u)

/*----E61 Tokens----*/
#define EDMA3_CCRL_ECRH_E61_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_ECRH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_ECRH_E60_RESETVAL     (0x00000000u)

/*----E60 Tokens----*/
#define EDMA3_CCRL_ECRH_E60_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_ECRH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_ECRH_E59_RESETVAL     (0x00000000u)

/*----E59 Tokens----*/
#define EDMA3_CCRL_ECRH_E59_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_ECRH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_ECRH_E58_RESETVAL     (0x00000000u)

/*----E58 Tokens----*/
#define EDMA3_CCRL_ECRH_E58_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_ECRH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_ECRH_E57_RESETVAL     (0x00000000u)

/*----E57 Tokens----*/
#define EDMA3_CCRL_ECRH_E57_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_ECRH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_ECRH_E56_RESETVAL     (0x00000000u)

/*----E56 Tokens----*/
#define EDMA3_CCRL_ECRH_E56_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_ECRH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_ECRH_E55_RESETVAL     (0x00000000u)

/*----E55 Tokens----*/
#define EDMA3_CCRL_ECRH_E55_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_ECRH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_ECRH_E54_RESETVAL     (0x00000000u)

/*----E54 Tokens----*/
#define EDMA3_CCRL_ECRH_E54_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_ECRH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_ECRH_E53_RESETVAL     (0x00000000u)

/*----E53 Tokens----*/
#define EDMA3_CCRL_ECRH_E53_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_ECRH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_ECRH_E52_RESETVAL     (0x00000000u)

/*----E52 Tokens----*/
#define EDMA3_CCRL_ECRH_E52_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_ECRH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_ECRH_E51_RESETVAL     (0x00000000u)

/*----E51 Tokens----*/
#define EDMA3_CCRL_ECRH_E51_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_ECRH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_ECRH_E50_RESETVAL     (0x00000000u)

/*----E50 Tokens----*/
#define EDMA3_CCRL_ECRH_E50_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_ECRH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_ECRH_E49_RESETVAL     (0x00000000u)

/*----E49 Tokens----*/
#define EDMA3_CCRL_ECRH_E49_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_ECRH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_ECRH_E48_RESETVAL     (0x00000000u)

/*----E48 Tokens----*/
#define EDMA3_CCRL_ECRH_E48_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_ECRH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_ECRH_E47_RESETVAL     (0x00000000u)

/*----E47 Tokens----*/
#define EDMA3_CCRL_ECRH_E47_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_ECRH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_ECRH_E46_RESETVAL     (0x00000000u)

/*----E46 Tokens----*/
#define EDMA3_CCRL_ECRH_E46_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_ECRH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_ECRH_E45_RESETVAL     (0x00000000u)

/*----E45 Tokens----*/
#define EDMA3_CCRL_ECRH_E45_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_ECRH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_ECRH_E44_RESETVAL     (0x00000000u)

/*----E44 Tokens----*/
#define EDMA3_CCRL_ECRH_E44_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_ECRH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_ECRH_E43_RESETVAL     (0x00000000u)

/*----E43 Tokens----*/
#define EDMA3_CCRL_ECRH_E43_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_ECRH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_ECRH_E42_RESETVAL     (0x00000000u)

/*----E42 Tokens----*/
#define EDMA3_CCRL_ECRH_E42_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_ECRH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_ECRH_E41_RESETVAL     (0x00000000u)

/*----E41 Tokens----*/
#define EDMA3_CCRL_ECRH_E41_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_ECRH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_ECRH_E40_RESETVAL     (0x00000000u)

/*----E40 Tokens----*/
#define EDMA3_CCRL_ECRH_E40_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_ECRH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_ECRH_E39_RESETVAL     (0x00000000u)

/*----E39 Tokens----*/
#define EDMA3_CCRL_ECRH_E39_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_ECRH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_ECRH_E38_RESETVAL     (0x00000000u)

/*----E38 Tokens----*/
#define EDMA3_CCRL_ECRH_E38_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_ECRH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_ECRH_E37_RESETVAL     (0x00000000u)

/*----E37 Tokens----*/
#define EDMA3_CCRL_ECRH_E37_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_ECRH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_ECRH_E36_RESETVAL     (0x00000000u)

/*----E36 Tokens----*/
#define EDMA3_CCRL_ECRH_E36_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_ECRH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_ECRH_E35_RESETVAL     (0x00000000u)

/*----E35 Tokens----*/
#define EDMA3_CCRL_ECRH_E35_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_ECRH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_ECRH_E34_RESETVAL     (0x00000000u)

/*----E34 Tokens----*/
#define EDMA3_CCRL_ECRH_E34_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_ECRH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_ECRH_E33_RESETVAL     (0x00000000u)

/*----E33 Tokens----*/
#define EDMA3_CCRL_ECRH_E33_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_ECRH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ECRH_E32_RESETVAL     (0x00000000u)

/*----E32 Tokens----*/
#define EDMA3_CCRL_ECRH_E32_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ECRH_RESETVAL         (0x00000000u)

/* ESR */

#define EDMA3_CCRL_ESR_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_ESR_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_ESR_E31_RESETVAL      (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_ESR_E31_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_ESR_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_ESR_E30_RESETVAL      (0x00000000u)

/*----E30 Tokens----*/
#define EDMA3_CCRL_ESR_E30_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_ESR_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_ESR_E29_RESETVAL      (0x00000000u)

/*----E29 Tokens----*/
#define EDMA3_CCRL_ESR_E29_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_ESR_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_ESR_E28_RESETVAL      (0x00000000u)

/*----E28 Tokens----*/
#define EDMA3_CCRL_ESR_E28_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_ESR_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_ESR_E27_RESETVAL      (0x00000000u)

/*----E27 Tokens----*/
#define EDMA3_CCRL_ESR_E27_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_ESR_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_ESR_E26_RESETVAL      (0x00000000u)

/*----E26 Tokens----*/
#define EDMA3_CCRL_ESR_E26_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_ESR_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_ESR_E25_RESETVAL      (0x00000000u)

/*----E25 Tokens----*/
#define EDMA3_CCRL_ESR_E25_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_ESR_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_ESR_E24_RESETVAL      (0x00000000u)

/*----E24 Tokens----*/
#define EDMA3_CCRL_ESR_E24_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_ESR_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_ESR_E23_RESETVAL      (0x00000000u)

/*----E23 Tokens----*/
#define EDMA3_CCRL_ESR_E23_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_ESR_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_ESR_E22_RESETVAL      (0x00000000u)

/*----E22 Tokens----*/
#define EDMA3_CCRL_ESR_E22_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_ESR_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_ESR_E21_RESETVAL      (0x00000000u)

/*----E21 Tokens----*/
#define EDMA3_CCRL_ESR_E21_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_ESR_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_ESR_E20_RESETVAL      (0x00000000u)

/*----E20 Tokens----*/
#define EDMA3_CCRL_ESR_E20_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_ESR_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_ESR_E19_RESETVAL      (0x00000000u)

/*----E19 Tokens----*/
#define EDMA3_CCRL_ESR_E19_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_ESR_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_ESR_E18_RESETVAL      (0x00000000u)

/*----E18 Tokens----*/
#define EDMA3_CCRL_ESR_E18_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_ESR_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_ESR_E17_RESETVAL      (0x00000000u)

/*----E17 Tokens----*/
#define EDMA3_CCRL_ESR_E17_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_ESR_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_ESR_E16_RESETVAL      (0x00000000u)

/*----E16 Tokens----*/
#define EDMA3_CCRL_ESR_E16_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_ESR_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_ESR_E15_RESETVAL      (0x00000000u)

/*----E15 Tokens----*/
#define EDMA3_CCRL_ESR_E15_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_ESR_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_ESR_E14_RESETVAL      (0x00000000u)

/*----E14 Tokens----*/
#define EDMA3_CCRL_ESR_E14_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_ESR_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_ESR_E13_RESETVAL      (0x00000000u)

/*----E13 Tokens----*/
#define EDMA3_CCRL_ESR_E13_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_ESR_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_ESR_E12_RESETVAL      (0x00000000u)

/*----E12 Tokens----*/
#define EDMA3_CCRL_ESR_E12_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_ESR_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_ESR_E11_RESETVAL      (0x00000000u)

/*----E11 Tokens----*/
#define EDMA3_CCRL_ESR_E11_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_ESR_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_ESR_E10_RESETVAL      (0x00000000u)

/*----E10 Tokens----*/
#define EDMA3_CCRL_ESR_E10_SET           (0x00000001u)

#define EDMA3_CCRL_ESR_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_ESR_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_ESR_E9_RESETVAL       (0x00000000u)

/*----E9 Tokens----*/
#define EDMA3_CCRL_ESR_E9_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_ESR_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_ESR_E8_RESETVAL       (0x00000000u)

/*----E8 Tokens----*/
#define EDMA3_CCRL_ESR_E8_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_ESR_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_ESR_E7_RESETVAL       (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_ESR_E7_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_ESR_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_ESR_E6_RESETVAL       (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_ESR_E6_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_ESR_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_ESR_E5_RESETVAL       (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_ESR_E5_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_ESR_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_ESR_E4_RESETVAL       (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_ESR_E4_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_ESR_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_ESR_E3_RESETVAL       (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_ESR_E3_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_ESR_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_ESR_E2_RESETVAL       (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_ESR_E2_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_ESR_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_ESR_E1_RESETVAL       (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_ESR_E1_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_ESR_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_ESR_E0_RESETVAL       (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_ESR_E0_SET            (0x00000001u)

#define EDMA3_CCRL_ESR_RESETVAL          (0x00000000u)

/* ESRH */

#define EDMA3_CCRL_ESRH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_ESRH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_ESRH_E63_RESETVAL     (0x00000000u)

/*----E63 Tokens----*/
#define EDMA3_CCRL_ESRH_E63_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_ESRH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_ESRH_E62_RESETVAL     (0x00000000u)

/*----E62 Tokens----*/
#define EDMA3_CCRL_ESRH_E62_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_ESRH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_ESRH_E61_RESETVAL     (0x00000000u)

/*----E61 Tokens----*/
#define EDMA3_CCRL_ESRH_E61_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_ESRH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_ESRH_E60_RESETVAL     (0x00000000u)

/*----E60 Tokens----*/
#define EDMA3_CCRL_ESRH_E60_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_ESRH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_ESRH_E59_RESETVAL     (0x00000000u)

/*----E59 Tokens----*/
#define EDMA3_CCRL_ESRH_E59_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_ESRH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_ESRH_E58_RESETVAL     (0x00000000u)

/*----E58 Tokens----*/
#define EDMA3_CCRL_ESRH_E58_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_ESRH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_ESRH_E57_RESETVAL     (0x00000000u)

/*----E57 Tokens----*/
#define EDMA3_CCRL_ESRH_E57_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_ESRH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_ESRH_E56_RESETVAL     (0x00000000u)

/*----E56 Tokens----*/
#define EDMA3_CCRL_ESRH_E56_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_ESRH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_ESRH_E55_RESETVAL     (0x00000000u)

/*----E55 Tokens----*/
#define EDMA3_CCRL_ESRH_E55_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_ESRH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_ESRH_E54_RESETVAL     (0x00000000u)

/*----E54 Tokens----*/
#define EDMA3_CCRL_ESRH_E54_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_ESRH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_ESRH_E53_RESETVAL     (0x00000000u)

/*----E53 Tokens----*/
#define EDMA3_CCRL_ESRH_E53_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_ESRH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_ESRH_E52_RESETVAL     (0x00000000u)

/*----E52 Tokens----*/
#define EDMA3_CCRL_ESRH_E52_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_ESRH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_ESRH_E51_RESETVAL     (0x00000000u)

/*----E51 Tokens----*/
#define EDMA3_CCRL_ESRH_E51_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_ESRH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_ESRH_E50_RESETVAL     (0x00000000u)

/*----E50 Tokens----*/
#define EDMA3_CCRL_ESRH_E50_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_ESRH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_ESRH_E49_RESETVAL     (0x00000000u)

/*----E49 Tokens----*/
#define EDMA3_CCRL_ESRH_E49_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_ESRH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_ESRH_E48_RESETVAL     (0x00000000u)

/*----E48 Tokens----*/
#define EDMA3_CCRL_ESRH_E48_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_ESRH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_ESRH_E47_RESETVAL     (0x00000000u)

/*----E47 Tokens----*/
#define EDMA3_CCRL_ESRH_E47_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_ESRH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_ESRH_E46_RESETVAL     (0x00000000u)

/*----E46 Tokens----*/
#define EDMA3_CCRL_ESRH_E46_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_ESRH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_ESRH_E45_RESETVAL     (0x00000000u)

/*----E45 Tokens----*/
#define EDMA3_CCRL_ESRH_E45_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_ESRH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_ESRH_E44_RESETVAL     (0x00000000u)

/*----E44 Tokens----*/
#define EDMA3_CCRL_ESRH_E44_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_ESRH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_ESRH_E43_RESETVAL     (0x00000000u)

/*----E43 Tokens----*/
#define EDMA3_CCRL_ESRH_E43_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_ESRH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_ESRH_E42_RESETVAL     (0x00000000u)

/*----E42 Tokens----*/
#define EDMA3_CCRL_ESRH_E42_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_ESRH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_ESRH_E41_RESETVAL     (0x00000000u)

/*----E41 Tokens----*/
#define EDMA3_CCRL_ESRH_E41_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_ESRH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_ESRH_E40_RESETVAL     (0x00000000u)

/*----E40 Tokens----*/
#define EDMA3_CCRL_ESRH_E40_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_ESRH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_ESRH_E39_RESETVAL     (0x00000000u)

/*----E39 Tokens----*/
#define EDMA3_CCRL_ESRH_E39_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_ESRH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_ESRH_E38_RESETVAL     (0x00000000u)

/*----E38 Tokens----*/
#define EDMA3_CCRL_ESRH_E38_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_ESRH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_ESRH_E37_RESETVAL     (0x00000000u)

/*----E37 Tokens----*/
#define EDMA3_CCRL_ESRH_E37_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_ESRH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_ESRH_E36_RESETVAL     (0x00000000u)

/*----E36 Tokens----*/
#define EDMA3_CCRL_ESRH_E36_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_ESRH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_ESRH_E35_RESETVAL     (0x00000000u)

/*----E35 Tokens----*/
#define EDMA3_CCRL_ESRH_E35_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_ESRH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_ESRH_E34_RESETVAL     (0x00000000u)

/*----E34 Tokens----*/
#define EDMA3_CCRL_ESRH_E34_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_ESRH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_ESRH_E33_RESETVAL     (0x00000000u)

/*----E33 Tokens----*/
#define EDMA3_CCRL_ESRH_E33_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_ESRH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ESRH_E32_RESETVAL     (0x00000000u)

/*----E32 Tokens----*/
#define EDMA3_CCRL_ESRH_E32_SET          (0x00000001u)

#define EDMA3_CCRL_ESRH_RESETVAL         (0x00000000u)

/* CER */

#define EDMA3_CCRL_CER_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_CER_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_CER_E31_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_CER_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_CER_E30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_CER_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_CER_E29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_CER_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_CER_E28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_CER_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_CER_E27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_CER_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_CER_E26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_CER_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_CER_E25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_CER_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_CER_E24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_CER_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_CER_E23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_CER_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_CER_E22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_CER_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_CER_E21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_CER_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_CER_E20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_CER_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_CER_E19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_CER_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_CER_E18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_CER_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_CER_E17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_CER_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_CER_E16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_CER_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_CER_E15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_CER_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_CER_E14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_CER_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_CER_E13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_CER_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_CER_E12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_CER_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_CER_E11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_CER_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_CER_E10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_CER_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_CER_E9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_CER_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_CER_E8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_CER_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_CER_E7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_CER_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_CER_E6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_CER_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_CER_E5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_CER_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_CER_E4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_CER_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_CER_E3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_CER_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_CER_E2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_CER_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_CER_E1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_CER_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_CER_E0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_CER_RESETVAL          (0x00000000u)

/* CERH */

#define EDMA3_CCRL_CERH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_CERH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_CERH_E63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_CERH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_CERH_E62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_CERH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_CERH_E61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_CERH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_CERH_E60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_CERH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_CERH_E59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_CERH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_CERH_E58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_CERH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_CERH_E57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_CERH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_CERH_E56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_CERH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_CERH_E55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_CERH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_CERH_E54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_CERH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_CERH_E53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_CERH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_CERH_E52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_CERH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_CERH_E51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_CERH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_CERH_E50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_CERH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_CERH_E49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_CERH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_CERH_E48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_CERH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_CERH_E47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_CERH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_CERH_E46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_CERH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_CERH_E45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_CERH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_CERH_E44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_CERH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_CERH_E43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_CERH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_CERH_E42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_CERH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_CERH_E41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_CERH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_CERH_E40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_CERH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_CERH_E39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_CERH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_CERH_E38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_CERH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_CERH_E37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_CERH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_CERH_E36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_CERH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_CERH_E35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_CERH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_CERH_E34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_CERH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_CERH_E33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_CERH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_CERH_E32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_RESETVAL         (0x00000000u)

/* EER */

#define EDMA3_CCRL_EER_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_EER_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_EER_E31_RESETVAL      (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_EER_E31_              (0x00000001u)

#define EDMA3_CCRL_EER_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_EER_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_EER_E30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_EER_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_EER_E29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_EER_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_EER_E28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_EER_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_EER_E27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_EER_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_EER_E26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_EER_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_EER_E25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_EER_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_EER_E24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_EER_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_EER_E23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_EER_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_EER_E22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_EER_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_EER_E21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_EER_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_EER_E20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_EER_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_EER_E19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_EER_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_EER_E18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_EER_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_EER_E17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_EER_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_EER_E16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_EER_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_EER_E15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_EER_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_EER_E14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_EER_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_EER_E13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_EER_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_EER_E12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_EER_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_EER_E11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_EER_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_EER_E10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_EER_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_EER_E9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_EER_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_EER_E8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_EER_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_EER_E7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_EER_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_EER_E6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_EER_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_EER_E5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_EER_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_EER_E4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_EER_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_EER_E3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_EER_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_EER_E2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_EER_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_EER_E1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_EER_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_EER_E0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_EER_RESETVAL          (0x00000000u)

/* EERH */

#define EDMA3_CCRL_EERH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_EERH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_EERH_E63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_EERH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_EERH_E62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_EERH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_EERH_E61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_EERH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_EERH_E60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_EERH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_EERH_E59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_EERH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_EERH_E58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_EERH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_EERH_E57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_EERH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_EERH_E56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_EERH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_EERH_E55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_EERH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_EERH_E54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_EERH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_EERH_E53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_EERH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_EERH_E52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_EERH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_EERH_E51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_EERH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_EERH_E50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_EERH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_EERH_E49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_EERH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_EERH_E48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_EERH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_EERH_E47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_EERH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_EERH_E46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_EERH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_EERH_E45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_EERH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_EERH_E44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_EERH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_EERH_E43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_EERH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_EERH_E42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_EERH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_EERH_E41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_EERH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_EERH_E40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_EERH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_EERH_E39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_EERH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_EERH_E38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_EERH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_EERH_E37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_EERH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_EERH_E36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_EERH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_EERH_E35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_EERH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_EERH_E34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_EERH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_EERH_E33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_EERH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_EERH_E32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_RESETVAL         (0x00000000u)

/* EECR */

#define EDMA3_CCRL_EECR_E31_MASK         (0x80000000u)
#define EDMA3_CCRL_EECR_E31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_EECR_E31_RESETVAL     (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_EECR_E31_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E30_MASK         (0x40000000u)
#define EDMA3_CCRL_EECR_E30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_EECR_E30_RESETVAL     (0x00000000u)

/*----E30 Tokens----*/
#define EDMA3_CCRL_EECR_E30_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E29_MASK         (0x20000000u)
#define EDMA3_CCRL_EECR_E29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_EECR_E29_RESETVAL     (0x00000000u)

/*----E29 Tokens----*/
#define EDMA3_CCRL_EECR_E29_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E28_MASK         (0x10000000u)
#define EDMA3_CCRL_EECR_E28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_EECR_E28_RESETVAL     (0x00000000u)

/*----E28 Tokens----*/
#define EDMA3_CCRL_EECR_E28_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E27_MASK         (0x08000000u)
#define EDMA3_CCRL_EECR_E27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_EECR_E27_RESETVAL     (0x00000000u)

/*----E27 Tokens----*/
#define EDMA3_CCRL_EECR_E27_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E26_MASK         (0x04000000u)
#define EDMA3_CCRL_EECR_E26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_EECR_E26_RESETVAL     (0x00000000u)

/*----E26 Tokens----*/
#define EDMA3_CCRL_EECR_E26_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E25_MASK         (0x02000000u)
#define EDMA3_CCRL_EECR_E25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_EECR_E25_RESETVAL     (0x00000000u)

/*----E25 Tokens----*/
#define EDMA3_CCRL_EECR_E25_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E24_MASK         (0x01000000u)
#define EDMA3_CCRL_EECR_E24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_EECR_E24_RESETVAL     (0x00000000u)

/*----E24 Tokens----*/
#define EDMA3_CCRL_EECR_E24_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E23_MASK         (0x00800000u)
#define EDMA3_CCRL_EECR_E23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_EECR_E23_RESETVAL     (0x00000000u)

/*----E23 Tokens----*/
#define EDMA3_CCRL_EECR_E23_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E22_MASK         (0x00400000u)
#define EDMA3_CCRL_EECR_E22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_EECR_E22_RESETVAL     (0x00000000u)

/*----E22 Tokens----*/
#define EDMA3_CCRL_EECR_E22_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E21_MASK         (0x00200000u)
#define EDMA3_CCRL_EECR_E21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_EECR_E21_RESETVAL     (0x00000000u)

/*----E21 Tokens----*/
#define EDMA3_CCRL_EECR_E21_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E20_MASK         (0x00100000u)
#define EDMA3_CCRL_EECR_E20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_EECR_E20_RESETVAL     (0x00000000u)

/*----E20 Tokens----*/
#define EDMA3_CCRL_EECR_E20_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E19_MASK         (0x00080000u)
#define EDMA3_CCRL_EECR_E19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_EECR_E19_RESETVAL     (0x00000000u)

/*----E19 Tokens----*/
#define EDMA3_CCRL_EECR_E19_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E18_MASK         (0x00040000u)
#define EDMA3_CCRL_EECR_E18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_EECR_E18_RESETVAL     (0x00000000u)

/*----E18 Tokens----*/
#define EDMA3_CCRL_EECR_E18_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E17_MASK         (0x00020000u)
#define EDMA3_CCRL_EECR_E17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_EECR_E17_RESETVAL     (0x00000000u)

/*----E17 Tokens----*/
#define EDMA3_CCRL_EECR_E17_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E16_MASK         (0x00010000u)
#define EDMA3_CCRL_EECR_E16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_EECR_E16_RESETVAL     (0x00000000u)

/*----E16 Tokens----*/
#define EDMA3_CCRL_EECR_E16_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E15_MASK         (0x00008000u)
#define EDMA3_CCRL_EECR_E15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_EECR_E15_RESETVAL     (0x00000000u)

/*----E15 Tokens----*/
#define EDMA3_CCRL_EECR_E15_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E14_MASK         (0x00004000u)
#define EDMA3_CCRL_EECR_E14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_EECR_E14_RESETVAL     (0x00000000u)

/*----E14 Tokens----*/
#define EDMA3_CCRL_EECR_E14_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E13_MASK         (0x00002000u)
#define EDMA3_CCRL_EECR_E13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_EECR_E13_RESETVAL     (0x00000000u)

/*----E13 Tokens----*/
#define EDMA3_CCRL_EECR_E13_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E12_MASK         (0x00001000u)
#define EDMA3_CCRL_EECR_E12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_EECR_E12_RESETVAL     (0x00000000u)

/*----E12 Tokens----*/
#define EDMA3_CCRL_EECR_E12_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E11_MASK         (0x00000800u)
#define EDMA3_CCRL_EECR_E11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_EECR_E11_RESETVAL     (0x00000000u)

/*----E11 Tokens----*/
#define EDMA3_CCRL_EECR_E11_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E10_MASK         (0x00000400u)
#define EDMA3_CCRL_EECR_E10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_EECR_E10_RESETVAL     (0x00000000u)

/*----E10 Tokens----*/
#define EDMA3_CCRL_EECR_E10_CLEAR        (0x00000001u)

#define EDMA3_CCRL_EECR_E9_MASK          (0x00000200u)
#define EDMA3_CCRL_EECR_E9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_EECR_E9_RESETVAL      (0x00000000u)

/*----E9 Tokens----*/
#define EDMA3_CCRL_EECR_E9_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E8_MASK          (0x00000100u)
#define EDMA3_CCRL_EECR_E8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_EECR_E8_RESETVAL      (0x00000000u)

/*----E8 Tokens----*/
#define EDMA3_CCRL_EECR_E8_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_EECR_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_EECR_E7_RESETVAL      (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_EECR_E7_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_EECR_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_EECR_E6_RESETVAL      (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_EECR_E6_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_EECR_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_EECR_E5_RESETVAL      (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_EECR_E5_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_EECR_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_EECR_E4_RESETVAL      (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_EECR_E4_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_EECR_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_EECR_E3_RESETVAL      (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_EECR_E3_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_EECR_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_EECR_E2_RESETVAL      (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_EECR_E2_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_EECR_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_EECR_E1_RESETVAL      (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_EECR_E1_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_EECR_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_EECR_E0_RESETVAL      (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_EECR_E0_CLEAR         (0x00000001u)

#define EDMA3_CCRL_EECR_RESETVAL         (0x00000000u)

/* EECRH */

#define EDMA3_CCRL_EECRH_E63_MASK        (0x80000000u)
#define EDMA3_CCRL_EECRH_E63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_EECRH_E63_RESETVAL    (0x00000000u)

/*----E63 Tokens----*/
#define EDMA3_CCRL_EECRH_E63_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E62_MASK        (0x40000000u)
#define EDMA3_CCRL_EECRH_E62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_EECRH_E62_RESETVAL    (0x00000000u)

/*----E62 Tokens----*/
#define EDMA3_CCRL_EECRH_E62_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E61_MASK        (0x20000000u)
#define EDMA3_CCRL_EECRH_E61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_EECRH_E61_RESETVAL    (0x00000000u)

/*----E61 Tokens----*/
#define EDMA3_CCRL_EECRH_E61_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E60_MASK        (0x10000000u)
#define EDMA3_CCRL_EECRH_E60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_EECRH_E60_RESETVAL    (0x00000000u)

/*----E60 Tokens----*/
#define EDMA3_CCRL_EECRH_E60_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E59_MASK        (0x08000000u)
#define EDMA3_CCRL_EECRH_E59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_EECRH_E59_RESETVAL    (0x00000000u)

/*----E59 Tokens----*/
#define EDMA3_CCRL_EECRH_E59_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E58_MASK        (0x04000000u)
#define EDMA3_CCRL_EECRH_E58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_EECRH_E58_RESETVAL    (0x00000000u)

/*----E58 Tokens----*/
#define EDMA3_CCRL_EECRH_E58_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E57_MASK        (0x02000000u)
#define EDMA3_CCRL_EECRH_E57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_EECRH_E57_RESETVAL    (0x00000000u)

/*----E57 Tokens----*/
#define EDMA3_CCRL_EECRH_E57_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E56_MASK        (0x01000000u)
#define EDMA3_CCRL_EECRH_E56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_EECRH_E56_RESETVAL    (0x00000000u)

/*----E56 Tokens----*/
#define EDMA3_CCRL_EECRH_E56_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E55_MASK        (0x00800000u)
#define EDMA3_CCRL_EECRH_E55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_EECRH_E55_RESETVAL    (0x00000000u)

/*----E55 Tokens----*/
#define EDMA3_CCRL_EECRH_E55_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E54_MASK        (0x00400000u)
#define EDMA3_CCRL_EECRH_E54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_EECRH_E54_RESETVAL    (0x00000000u)

/*----E54 Tokens----*/
#define EDMA3_CCRL_EECRH_E54_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E53_MASK        (0x00200000u)
#define EDMA3_CCRL_EECRH_E53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_EECRH_E53_RESETVAL    (0x00000000u)

/*----E53 Tokens----*/
#define EDMA3_CCRL_EECRH_E53_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E52_MASK        (0x00100000u)
#define EDMA3_CCRL_EECRH_E52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_EECRH_E52_RESETVAL    (0x00000000u)

/*----E52 Tokens----*/
#define EDMA3_CCRL_EECRH_E52_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E51_MASK        (0x00080000u)
#define EDMA3_CCRL_EECRH_E51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_EECRH_E51_RESETVAL    (0x00000000u)

/*----E51 Tokens----*/
#define EDMA3_CCRL_EECRH_E51_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E50_MASK        (0x00040000u)
#define EDMA3_CCRL_EECRH_E50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_EECRH_E50_RESETVAL    (0x00000000u)

/*----E50 Tokens----*/
#define EDMA3_CCRL_EECRH_E50_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E49_MASK        (0x00020000u)
#define EDMA3_CCRL_EECRH_E49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_EECRH_E49_RESETVAL    (0x00000000u)

/*----E49 Tokens----*/
#define EDMA3_CCRL_EECRH_E49_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E48_MASK        (0x00010000u)
#define EDMA3_CCRL_EECRH_E48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_EECRH_E48_RESETVAL    (0x00000000u)

/*----E48 Tokens----*/
#define EDMA3_CCRL_EECRH_E48_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E47_MASK        (0x00008000u)
#define EDMA3_CCRL_EECRH_E47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_EECRH_E47_RESETVAL    (0x00000000u)

/*----E47 Tokens----*/
#define EDMA3_CCRL_EECRH_E47_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E46_MASK        (0x00004000u)
#define EDMA3_CCRL_EECRH_E46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_EECRH_E46_RESETVAL    (0x00000000u)

/*----E46 Tokens----*/
#define EDMA3_CCRL_EECRH_E46_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E45_MASK        (0x00002000u)
#define EDMA3_CCRL_EECRH_E45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_EECRH_E45_RESETVAL    (0x00000000u)

/*----E45 Tokens----*/
#define EDMA3_CCRL_EECRH_E45_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E44_MASK        (0x00001000u)
#define EDMA3_CCRL_EECRH_E44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_EECRH_E44_RESETVAL    (0x00000000u)

/*----E44 Tokens----*/
#define EDMA3_CCRL_EECRH_E44_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E43_MASK        (0x00000800u)
#define EDMA3_CCRL_EECRH_E43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_EECRH_E43_RESETVAL    (0x00000000u)

/*----E43 Tokens----*/
#define EDMA3_CCRL_EECRH_E43_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E42_MASK        (0x00000400u)
#define EDMA3_CCRL_EECRH_E42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_EECRH_E42_RESETVAL    (0x00000000u)

/*----E42 Tokens----*/
#define EDMA3_CCRL_EECRH_E42_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E41_MASK        (0x00000200u)
#define EDMA3_CCRL_EECRH_E41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_EECRH_E41_RESETVAL    (0x00000000u)

/*----E41 Tokens----*/
#define EDMA3_CCRL_EECRH_E41_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E40_MASK        (0x00000100u)
#define EDMA3_CCRL_EECRH_E40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_EECRH_E40_RESETVAL    (0x00000000u)

/*----E40 Tokens----*/
#define EDMA3_CCRL_EECRH_E40_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E39_MASK        (0x00000080u)
#define EDMA3_CCRL_EECRH_E39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_EECRH_E39_RESETVAL    (0x00000000u)

/*----E39 Tokens----*/
#define EDMA3_CCRL_EECRH_E39_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E38_MASK        (0x00000040u)
#define EDMA3_CCRL_EECRH_E38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_EECRH_E38_RESETVAL    (0x00000000u)

/*----E38 Tokens----*/
#define EDMA3_CCRL_EECRH_E38_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E37_MASK        (0x00000020u)
#define EDMA3_CCRL_EECRH_E37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_EECRH_E37_RESETVAL    (0x00000000u)

/*----E37 Tokens----*/
#define EDMA3_CCRL_EECRH_E37_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E36_MASK        (0x00000010u)
#define EDMA3_CCRL_EECRH_E36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_EECRH_E36_RESETVAL    (0x00000000u)

/*----E36 Tokens----*/
#define EDMA3_CCRL_EECRH_E36_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E35_MASK        (0x00000008u)
#define EDMA3_CCRL_EECRH_E35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_EECRH_E35_RESETVAL    (0x00000000u)

/*----E35 Tokens----*/
#define EDMA3_CCRL_EECRH_E35_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E34_MASK        (0x00000004u)
#define EDMA3_CCRL_EECRH_E34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_EECRH_E34_RESETVAL    (0x00000000u)

/*----E34 Tokens----*/
#define EDMA3_CCRL_EECRH_E34_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E33_MASK        (0x00000002u)
#define EDMA3_CCRL_EECRH_E33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_EECRH_E33_RESETVAL    (0x00000000u)

/*----E33 Tokens----*/
#define EDMA3_CCRL_EECRH_E33_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_E32_MASK        (0x00000001u)
#define EDMA3_CCRL_EECRH_E32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_EECRH_E32_RESETVAL    (0x00000000u)

/*----E32 Tokens----*/
#define EDMA3_CCRL_EECRH_E32_CLEAR       (0x00000001u)

#define EDMA3_CCRL_EECRH_RESETVAL        (0x00000000u)

/* EESR */

#define EDMA3_CCRL_EESR_E31_MASK         (0x80000000u)
#define EDMA3_CCRL_EESR_E31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_EESR_E31_RESETVAL     (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_EESR_E31_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E30_MASK         (0x40000000u)
#define EDMA3_CCRL_EESR_E30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_EESR_E30_RESETVAL     (0x00000000u)

/*----E30 Tokens----*/
#define EDMA3_CCRL_EESR_E30_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E29_MASK         (0x20000000u)
#define EDMA3_CCRL_EESR_E29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_EESR_E29_RESETVAL     (0x00000000u)

/*----E29 Tokens----*/
#define EDMA3_CCRL_EESR_E29_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E28_MASK         (0x10000000u)
#define EDMA3_CCRL_EESR_E28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_EESR_E28_RESETVAL     (0x00000000u)

/*----E28 Tokens----*/
#define EDMA3_CCRL_EESR_E28_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E27_MASK         (0x08000000u)
#define EDMA3_CCRL_EESR_E27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_EESR_E27_RESETVAL     (0x00000000u)

/*----E27 Tokens----*/
#define EDMA3_CCRL_EESR_E27_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E26_MASK         (0x04000000u)
#define EDMA3_CCRL_EESR_E26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_EESR_E26_RESETVAL     (0x00000000u)

/*----E26 Tokens----*/
#define EDMA3_CCRL_EESR_E26_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E25_MASK         (0x02000000u)
#define EDMA3_CCRL_EESR_E25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_EESR_E25_RESETVAL     (0x00000000u)

/*----E25 Tokens----*/
#define EDMA3_CCRL_EESR_E25_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E24_MASK         (0x01000000u)
#define EDMA3_CCRL_EESR_E24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_EESR_E24_RESETVAL     (0x00000000u)

/*----E24 Tokens----*/
#define EDMA3_CCRL_EESR_E24_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E23_MASK         (0x00800000u)
#define EDMA3_CCRL_EESR_E23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_EESR_E23_RESETVAL     (0x00000000u)

/*----E23 Tokens----*/
#define EDMA3_CCRL_EESR_E23_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E22_MASK         (0x00400000u)
#define EDMA3_CCRL_EESR_E22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_EESR_E22_RESETVAL     (0x00000000u)

/*----E22 Tokens----*/
#define EDMA3_CCRL_EESR_E22_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E21_MASK         (0x00200000u)
#define EDMA3_CCRL_EESR_E21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_EESR_E21_RESETVAL     (0x00000000u)

/*----E21 Tokens----*/
#define EDMA3_CCRL_EESR_E21_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E20_MASK         (0x00100000u)
#define EDMA3_CCRL_EESR_E20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_EESR_E20_RESETVAL     (0x00000000u)

/*----E20 Tokens----*/
#define EDMA3_CCRL_EESR_E20_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E19_MASK         (0x00080000u)
#define EDMA3_CCRL_EESR_E19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_EESR_E19_RESETVAL     (0x00000000u)

/*----E19 Tokens----*/
#define EDMA3_CCRL_EESR_E19_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E18_MASK         (0x00040000u)
#define EDMA3_CCRL_EESR_E18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_EESR_E18_RESETVAL     (0x00000000u)

/*----E18 Tokens----*/
#define EDMA3_CCRL_EESR_E18_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E17_MASK         (0x00020000u)
#define EDMA3_CCRL_EESR_E17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_EESR_E17_RESETVAL     (0x00000000u)

/*----E17 Tokens----*/
#define EDMA3_CCRL_EESR_E17_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E16_MASK         (0x00010000u)
#define EDMA3_CCRL_EESR_E16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_EESR_E16_RESETVAL     (0x00000000u)

/*----E16 Tokens----*/
#define EDMA3_CCRL_EESR_E16_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E15_MASK         (0x00008000u)
#define EDMA3_CCRL_EESR_E15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_EESR_E15_RESETVAL     (0x00000000u)

/*----E15 Tokens----*/
#define EDMA3_CCRL_EESR_E15_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E14_MASK         (0x00004000u)
#define EDMA3_CCRL_EESR_E14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_EESR_E14_RESETVAL     (0x00000000u)

/*----E14 Tokens----*/
#define EDMA3_CCRL_EESR_E14_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E13_MASK         (0x00002000u)
#define EDMA3_CCRL_EESR_E13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_EESR_E13_RESETVAL     (0x00000000u)

/*----E13 Tokens----*/
#define EDMA3_CCRL_EESR_E13_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E12_MASK         (0x00001000u)
#define EDMA3_CCRL_EESR_E12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_EESR_E12_RESETVAL     (0x00000000u)

/*----E12 Tokens----*/
#define EDMA3_CCRL_EESR_E12_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E11_MASK         (0x00000800u)
#define EDMA3_CCRL_EESR_E11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_EESR_E11_RESETVAL     (0x00000000u)

/*----E11 Tokens----*/
#define EDMA3_CCRL_EESR_E11_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E10_MASK         (0x00000400u)
#define EDMA3_CCRL_EESR_E10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_EESR_E10_RESETVAL     (0x00000000u)

/*----E10 Tokens----*/
#define EDMA3_CCRL_EESR_E10_SET          (0x00000001u)

#define EDMA3_CCRL_EESR_E9_MASK          (0x00000200u)
#define EDMA3_CCRL_EESR_E9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_EESR_E9_RESETVAL      (0x00000000u)

/*----E9 Tokens----*/
#define EDMA3_CCRL_EESR_E9_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E8_MASK          (0x00000100u)
#define EDMA3_CCRL_EESR_E8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_EESR_E8_RESETVAL      (0x00000000u)

/*----E8 Tokens----*/
#define EDMA3_CCRL_EESR_E8_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_EESR_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_EESR_E7_RESETVAL      (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_EESR_E7_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_EESR_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_EESR_E6_RESETVAL      (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_EESR_E6_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_EESR_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_EESR_E5_RESETVAL      (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_EESR_E5_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_EESR_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_EESR_E4_RESETVAL      (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_EESR_E4_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_EESR_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_EESR_E3_RESETVAL      (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_EESR_E3_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_EESR_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_EESR_E2_RESETVAL      (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_EESR_E2_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_EESR_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_EESR_E1_RESETVAL      (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_EESR_E1_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_EESR_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_EESR_E0_RESETVAL      (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_EESR_E0_SET           (0x00000001u)

#define EDMA3_CCRL_EESR_RESETVAL         (0x00000000u)

/* EESRH */

#define EDMA3_CCRL_EESRH_E63_MASK        (0x80000000u)
#define EDMA3_CCRL_EESRH_E63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_EESRH_E63_RESETVAL    (0x00000000u)

/*----E63 Tokens----*/
#define EDMA3_CCRL_EESRH_E63_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E62_MASK        (0x40000000u)
#define EDMA3_CCRL_EESRH_E62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_EESRH_E62_RESETVAL    (0x00000000u)

/*----E62 Tokens----*/
#define EDMA3_CCRL_EESRH_E62_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E61_MASK        (0x20000000u)
#define EDMA3_CCRL_EESRH_E61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_EESRH_E61_RESETVAL    (0x00000000u)

/*----E61 Tokens----*/
#define EDMA3_CCRL_EESRH_E61_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E60_MASK        (0x10000000u)
#define EDMA3_CCRL_EESRH_E60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_EESRH_E60_RESETVAL    (0x00000000u)

/*----E60 Tokens----*/
#define EDMA3_CCRL_EESRH_E60_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E59_MASK        (0x08000000u)
#define EDMA3_CCRL_EESRH_E59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_EESRH_E59_RESETVAL    (0x00000000u)

/*----E59 Tokens----*/
#define EDMA3_CCRL_EESRH_E59_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E58_MASK        (0x04000000u)
#define EDMA3_CCRL_EESRH_E58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_EESRH_E58_RESETVAL    (0x00000000u)

/*----E58 Tokens----*/
#define EDMA3_CCRL_EESRH_E58_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E57_MASK        (0x02000000u)
#define EDMA3_CCRL_EESRH_E57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_EESRH_E57_RESETVAL    (0x00000000u)

/*----E57 Tokens----*/
#define EDMA3_CCRL_EESRH_E57_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E56_MASK        (0x01000000u)
#define EDMA3_CCRL_EESRH_E56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_EESRH_E56_RESETVAL    (0x00000000u)

/*----E56 Tokens----*/
#define EDMA3_CCRL_EESRH_E56_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E55_MASK        (0x00800000u)
#define EDMA3_CCRL_EESRH_E55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_EESRH_E55_RESETVAL    (0x00000000u)

/*----E55 Tokens----*/
#define EDMA3_CCRL_EESRH_E55_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E54_MASK        (0x00400000u)
#define EDMA3_CCRL_EESRH_E54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_EESRH_E54_RESETVAL    (0x00000000u)

/*----E54 Tokens----*/
#define EDMA3_CCRL_EESRH_E54_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E53_MASK        (0x00200000u)
#define EDMA3_CCRL_EESRH_E53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_EESRH_E53_RESETVAL    (0x00000000u)

/*----E53 Tokens----*/
#define EDMA3_CCRL_EESRH_E53_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E52_MASK        (0x00100000u)
#define EDMA3_CCRL_EESRH_E52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_EESRH_E52_RESETVAL    (0x00000000u)

/*----E52 Tokens----*/
#define EDMA3_CCRL_EESRH_E52_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E51_MASK        (0x00080000u)
#define EDMA3_CCRL_EESRH_E51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_EESRH_E51_RESETVAL    (0x00000000u)

/*----E51 Tokens----*/
#define EDMA3_CCRL_EESRH_E51_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E50_MASK        (0x00040000u)
#define EDMA3_CCRL_EESRH_E50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_EESRH_E50_RESETVAL    (0x00000000u)

/*----E50 Tokens----*/
#define EDMA3_CCRL_EESRH_E50_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E49_MASK        (0x00020000u)
#define EDMA3_CCRL_EESRH_E49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_EESRH_E49_RESETVAL    (0x00000000u)

/*----E49 Tokens----*/
#define EDMA3_CCRL_EESRH_E49_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E48_MASK        (0x00010000u)
#define EDMA3_CCRL_EESRH_E48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_EESRH_E48_RESETVAL    (0x00000000u)

/*----E48 Tokens----*/
#define EDMA3_CCRL_EESRH_E48_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E47_MASK        (0x00008000u)
#define EDMA3_CCRL_EESRH_E47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_EESRH_E47_RESETVAL    (0x00000000u)

/*----E47 Tokens----*/
#define EDMA3_CCRL_EESRH_E47_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E46_MASK        (0x00004000u)
#define EDMA3_CCRL_EESRH_E46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_EESRH_E46_RESETVAL    (0x00000000u)

/*----E46 Tokens----*/
#define EDMA3_CCRL_EESRH_E46_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E45_MASK        (0x00002000u)
#define EDMA3_CCRL_EESRH_E45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_EESRH_E45_RESETVAL    (0x00000000u)

/*----E45 Tokens----*/
#define EDMA3_CCRL_EESRH_E45_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E44_MASK        (0x00001000u)
#define EDMA3_CCRL_EESRH_E44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_EESRH_E44_RESETVAL    (0x00000000u)

/*----E44 Tokens----*/
#define EDMA3_CCRL_EESRH_E44_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E43_MASK        (0x00000800u)
#define EDMA3_CCRL_EESRH_E43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_EESRH_E43_RESETVAL    (0x00000000u)

/*----E43 Tokens----*/
#define EDMA3_CCRL_EESRH_E43_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E42_MASK        (0x00000400u)
#define EDMA3_CCRL_EESRH_E42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_EESRH_E42_RESETVAL    (0x00000000u)

/*----E42 Tokens----*/
#define EDMA3_CCRL_EESRH_E42_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E41_MASK        (0x00000200u)
#define EDMA3_CCRL_EESRH_E41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_EESRH_E41_RESETVAL    (0x00000000u)

/*----E41 Tokens----*/
#define EDMA3_CCRL_EESRH_E41_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E40_MASK        (0x00000100u)
#define EDMA3_CCRL_EESRH_E40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_EESRH_E40_RESETVAL    (0x00000000u)

/*----E40 Tokens----*/
#define EDMA3_CCRL_EESRH_E40_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E39_MASK        (0x00000080u)
#define EDMA3_CCRL_EESRH_E39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_EESRH_E39_RESETVAL    (0x00000000u)

/*----E39 Tokens----*/
#define EDMA3_CCRL_EESRH_E39_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E38_MASK        (0x00000040u)
#define EDMA3_CCRL_EESRH_E38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_EESRH_E38_RESETVAL    (0x00000000u)

/*----E38 Tokens----*/
#define EDMA3_CCRL_EESRH_E38_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E37_MASK        (0x00000020u)
#define EDMA3_CCRL_EESRH_E37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_EESRH_E37_RESETVAL    (0x00000000u)

/*----E37 Tokens----*/
#define EDMA3_CCRL_EESRH_E37_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E36_MASK        (0x00000010u)
#define EDMA3_CCRL_EESRH_E36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_EESRH_E36_RESETVAL    (0x00000000u)

/*----E36 Tokens----*/
#define EDMA3_CCRL_EESRH_E36_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E35_MASK        (0x00000008u)
#define EDMA3_CCRL_EESRH_E35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_EESRH_E35_RESETVAL    (0x00000000u)

/*----E35 Tokens----*/
#define EDMA3_CCRL_EESRH_E35_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E34_MASK        (0x00000004u)
#define EDMA3_CCRL_EESRH_E34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_EESRH_E34_RESETVAL    (0x00000000u)

/*----E34 Tokens----*/
#define EDMA3_CCRL_EESRH_E34_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E33_MASK        (0x00000002u)
#define EDMA3_CCRL_EESRH_E33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_EESRH_E33_RESETVAL    (0x00000000u)

/*----E33 Tokens----*/
#define EDMA3_CCRL_EESRH_E33_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_E32_MASK        (0x00000001u)
#define EDMA3_CCRL_EESRH_E32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_EESRH_E32_RESETVAL    (0x00000000u)

/*----E32 Tokens----*/
#define EDMA3_CCRL_EESRH_E32_SET         (0x00000001u)

#define EDMA3_CCRL_EESRH_RESETVAL        (0x00000000u)

/* SER */

#define EDMA3_CCRL_SER_E31_MASK          (0x80000000u)
#define EDMA3_CCRL_SER_E31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_SER_E31_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E30_MASK          (0x40000000u)
#define EDMA3_CCRL_SER_E30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_SER_E30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E29_MASK          (0x20000000u)
#define EDMA3_CCRL_SER_E29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_SER_E29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E28_MASK          (0x10000000u)
#define EDMA3_CCRL_SER_E28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_SER_E28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E27_MASK          (0x08000000u)
#define EDMA3_CCRL_SER_E27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_SER_E27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E26_MASK          (0x04000000u)
#define EDMA3_CCRL_SER_E26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_SER_E26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E25_MASK          (0x02000000u)
#define EDMA3_CCRL_SER_E25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_SER_E25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E24_MASK          (0x01000000u)
#define EDMA3_CCRL_SER_E24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_SER_E24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E23_MASK          (0x00800000u)
#define EDMA3_CCRL_SER_E23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_SER_E23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E22_MASK          (0x00400000u)
#define EDMA3_CCRL_SER_E22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_SER_E22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E21_MASK          (0x00200000u)
#define EDMA3_CCRL_SER_E21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_SER_E21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E20_MASK          (0x00100000u)
#define EDMA3_CCRL_SER_E20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_SER_E20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E19_MASK          (0x00080000u)
#define EDMA3_CCRL_SER_E19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_SER_E19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E18_MASK          (0x00040000u)
#define EDMA3_CCRL_SER_E18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_SER_E18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E17_MASK          (0x00020000u)
#define EDMA3_CCRL_SER_E17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_SER_E17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E16_MASK          (0x00010000u)
#define EDMA3_CCRL_SER_E16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_SER_E16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E15_MASK          (0x00008000u)
#define EDMA3_CCRL_SER_E15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_SER_E15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E14_MASK          (0x00004000u)
#define EDMA3_CCRL_SER_E14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_SER_E14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E13_MASK          (0x00002000u)
#define EDMA3_CCRL_SER_E13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_SER_E13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E12_MASK          (0x00001000u)
#define EDMA3_CCRL_SER_E12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_SER_E12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E11_MASK          (0x00000800u)
#define EDMA3_CCRL_SER_E11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_SER_E11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E10_MASK          (0x00000400u)
#define EDMA3_CCRL_SER_E10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_SER_E10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_E9_MASK           (0x00000200u)
#define EDMA3_CCRL_SER_E9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_SER_E9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E8_MASK           (0x00000100u)
#define EDMA3_CCRL_SER_E8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_SER_E8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_SER_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_SER_E7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_SER_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_SER_E6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_SER_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_SER_E5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_SER_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_SER_E4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_SER_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_SER_E3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_SER_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_SER_E2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_SER_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_SER_E1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_SER_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_SER_E0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_SER_RESETVAL          (0x00000000u)

/* SERH */

#define EDMA3_CCRL_SERH_E63_MASK         (0x80000000u)
#define EDMA3_CCRL_SERH_E63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_SERH_E63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E62_MASK         (0x40000000u)
#define EDMA3_CCRL_SERH_E62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_SERH_E62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E61_MASK         (0x20000000u)
#define EDMA3_CCRL_SERH_E61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_SERH_E61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E60_MASK         (0x10000000u)
#define EDMA3_CCRL_SERH_E60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_SERH_E60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E59_MASK         (0x08000000u)
#define EDMA3_CCRL_SERH_E59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_SERH_E59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E58_MASK         (0x04000000u)
#define EDMA3_CCRL_SERH_E58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_SERH_E58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E57_MASK         (0x02000000u)
#define EDMA3_CCRL_SERH_E57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_SERH_E57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E56_MASK         (0x01000000u)
#define EDMA3_CCRL_SERH_E56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_SERH_E56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E55_MASK         (0x00800000u)
#define EDMA3_CCRL_SERH_E55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_SERH_E55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E54_MASK         (0x00400000u)
#define EDMA3_CCRL_SERH_E54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_SERH_E54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E53_MASK         (0x00200000u)
#define EDMA3_CCRL_SERH_E53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_SERH_E53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E52_MASK         (0x00100000u)
#define EDMA3_CCRL_SERH_E52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_SERH_E52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E51_MASK         (0x00080000u)
#define EDMA3_CCRL_SERH_E51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_SERH_E51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E50_MASK         (0x00040000u)
#define EDMA3_CCRL_SERH_E50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_SERH_E50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E49_MASK         (0x00020000u)
#define EDMA3_CCRL_SERH_E49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_SERH_E49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E48_MASK         (0x00010000u)
#define EDMA3_CCRL_SERH_E48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_SERH_E48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E47_MASK         (0x00008000u)
#define EDMA3_CCRL_SERH_E47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_SERH_E47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E46_MASK         (0x00004000u)
#define EDMA3_CCRL_SERH_E46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_SERH_E46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E45_MASK         (0x00002000u)
#define EDMA3_CCRL_SERH_E45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_SERH_E45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E44_MASK         (0x00001000u)
#define EDMA3_CCRL_SERH_E44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_SERH_E44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E43_MASK         (0x00000800u)
#define EDMA3_CCRL_SERH_E43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_SERH_E43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E42_MASK         (0x00000400u)
#define EDMA3_CCRL_SERH_E42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_SERH_E42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E41_MASK         (0x00000200u)
#define EDMA3_CCRL_SERH_E41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_SERH_E41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E40_MASK         (0x00000100u)
#define EDMA3_CCRL_SERH_E40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_SERH_E40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E39_MASK         (0x00000080u)
#define EDMA3_CCRL_SERH_E39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_SERH_E39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E38_MASK         (0x00000040u)
#define EDMA3_CCRL_SERH_E38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_SERH_E38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E37_MASK         (0x00000020u)
#define EDMA3_CCRL_SERH_E37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_SERH_E37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E36_MASK         (0x00000010u)
#define EDMA3_CCRL_SERH_E36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_SERH_E36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E35_MASK         (0x00000008u)
#define EDMA3_CCRL_SERH_E35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_SERH_E35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E34_MASK         (0x00000004u)
#define EDMA3_CCRL_SERH_E34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_SERH_E34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E33_MASK         (0x00000002u)
#define EDMA3_CCRL_SERH_E33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_SERH_E33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_E32_MASK         (0x00000001u)
#define EDMA3_CCRL_SERH_E32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_SERH_E32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_RESETVAL         (0x00000000u)

/* SECR */

#define EDMA3_CCRL_SECR_E31_MASK         (0x80000000u)
#define EDMA3_CCRL_SECR_E31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_SECR_E31_RESETVAL     (0x00000000u)

/*----E31 Tokens----*/
#define EDMA3_CCRL_SECR_E31_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E30_MASK         (0x40000000u)
#define EDMA3_CCRL_SECR_E30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_SECR_E30_RESETVAL     (0x00000000u)

/*----E30 Tokens----*/
#define EDMA3_CCRL_SECR_E30_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E29_MASK         (0x20000000u)
#define EDMA3_CCRL_SECR_E29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_SECR_E29_RESETVAL     (0x00000000u)

/*----E29 Tokens----*/
#define EDMA3_CCRL_SECR_E29_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E28_MASK         (0x10000000u)
#define EDMA3_CCRL_SECR_E28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_SECR_E28_RESETVAL     (0x00000000u)

/*----E28 Tokens----*/
#define EDMA3_CCRL_SECR_E28_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E27_MASK         (0x08000000u)
#define EDMA3_CCRL_SECR_E27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_SECR_E27_RESETVAL     (0x00000000u)

/*----E27 Tokens----*/
#define EDMA3_CCRL_SECR_E27_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E26_MASK         (0x04000000u)
#define EDMA3_CCRL_SECR_E26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_SECR_E26_RESETVAL     (0x00000000u)

/*----E26 Tokens----*/
#define EDMA3_CCRL_SECR_E26_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E25_MASK         (0x02000000u)
#define EDMA3_CCRL_SECR_E25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_SECR_E25_RESETVAL     (0x00000000u)

/*----E25 Tokens----*/
#define EDMA3_CCRL_SECR_E25_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E24_MASK         (0x01000000u)
#define EDMA3_CCRL_SECR_E24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_SECR_E24_RESETVAL     (0x00000000u)

/*----E24 Tokens----*/
#define EDMA3_CCRL_SECR_E24_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E23_MASK         (0x00800000u)
#define EDMA3_CCRL_SECR_E23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_SECR_E23_RESETVAL     (0x00000000u)

/*----E23 Tokens----*/
#define EDMA3_CCRL_SECR_E23_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E22_MASK         (0x00400000u)
#define EDMA3_CCRL_SECR_E22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_SECR_E22_RESETVAL     (0x00000000u)

/*----E22 Tokens----*/
#define EDMA3_CCRL_SECR_E22_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E21_MASK         (0x00200000u)
#define EDMA3_CCRL_SECR_E21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_SECR_E21_RESETVAL     (0x00000000u)

/*----E21 Tokens----*/
#define EDMA3_CCRL_SECR_E21_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E20_MASK         (0x00100000u)
#define EDMA3_CCRL_SECR_E20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_SECR_E20_RESETVAL     (0x00000000u)

/*----E20 Tokens----*/
#define EDMA3_CCRL_SECR_E20_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E19_MASK         (0x00080000u)
#define EDMA3_CCRL_SECR_E19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_SECR_E19_RESETVAL     (0x00000000u)

/*----E19 Tokens----*/
#define EDMA3_CCRL_SECR_E19_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E18_MASK         (0x00040000u)
#define EDMA3_CCRL_SECR_E18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_SECR_E18_RESETVAL     (0x00000000u)

/*----E18 Tokens----*/
#define EDMA3_CCRL_SECR_E18_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E17_MASK         (0x00020000u)
#define EDMA3_CCRL_SECR_E17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_SECR_E17_RESETVAL     (0x00000000u)

/*----E17 Tokens----*/
#define EDMA3_CCRL_SECR_E17_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E16_MASK         (0x00010000u)
#define EDMA3_CCRL_SECR_E16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_SECR_E16_RESETVAL     (0x00000000u)

/*----E16 Tokens----*/
#define EDMA3_CCRL_SECR_E16_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E15_MASK         (0x00008000u)
#define EDMA3_CCRL_SECR_E15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_SECR_E15_RESETVAL     (0x00000000u)

/*----E15 Tokens----*/
#define EDMA3_CCRL_SECR_E15_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E14_MASK         (0x00004000u)
#define EDMA3_CCRL_SECR_E14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_SECR_E14_RESETVAL     (0x00000000u)

/*----E14 Tokens----*/
#define EDMA3_CCRL_SECR_E14_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E13_MASK         (0x00002000u)
#define EDMA3_CCRL_SECR_E13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_SECR_E13_RESETVAL     (0x00000000u)

/*----E13 Tokens----*/
#define EDMA3_CCRL_SECR_E13_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E12_MASK         (0x00001000u)
#define EDMA3_CCRL_SECR_E12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_SECR_E12_RESETVAL     (0x00000000u)

/*----E12 Tokens----*/
#define EDMA3_CCRL_SECR_E12_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E11_MASK         (0x00000800u)
#define EDMA3_CCRL_SECR_E11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_SECR_E11_RESETVAL     (0x00000000u)

/*----E11 Tokens----*/
#define EDMA3_CCRL_SECR_E11_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E10_MASK         (0x00000400u)
#define EDMA3_CCRL_SECR_E10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_SECR_E10_RESETVAL     (0x00000000u)

/*----E10 Tokens----*/
#define EDMA3_CCRL_SECR_E10_CLEAR        (0x00000001u)

#define EDMA3_CCRL_SECR_E9_MASK          (0x00000200u)
#define EDMA3_CCRL_SECR_E9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_SECR_E9_RESETVAL      (0x00000000u)

/*----E9 Tokens----*/
#define EDMA3_CCRL_SECR_E9_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E8_MASK          (0x00000100u)
#define EDMA3_CCRL_SECR_E8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_SECR_E8_RESETVAL      (0x00000000u)

/*----E8 Tokens----*/
#define EDMA3_CCRL_SECR_E8_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_SECR_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_SECR_E7_RESETVAL      (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_SECR_E7_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_SECR_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_SECR_E6_RESETVAL      (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_SECR_E6_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_SECR_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_SECR_E5_RESETVAL      (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_SECR_E5_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_SECR_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_SECR_E4_RESETVAL      (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_SECR_E4_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_SECR_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_SECR_E3_RESETVAL      (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_SECR_E3_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_SECR_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_SECR_E2_RESETVAL      (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_SECR_E2_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_SECR_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_SECR_E1_RESETVAL      (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_SECR_E1_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_SECR_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_SECR_E0_RESETVAL      (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_SECR_E0_CLEAR         (0x00000001u)

#define EDMA3_CCRL_SECR_RESETVAL         (0x00000000u)

/* SECRH */

#define EDMA3_CCRL_SECRH_E63_MASK        (0x80000000u)
#define EDMA3_CCRL_SECRH_E63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_SECRH_E63_RESETVAL    (0x00000000u)

/*----E63 Tokens----*/
#define EDMA3_CCRL_SECRH_E63_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E62_MASK        (0x40000000u)
#define EDMA3_CCRL_SECRH_E62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_SECRH_E62_RESETVAL    (0x00000000u)

/*----E62 Tokens----*/
#define EDMA3_CCRL_SECRH_E62_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E61_MASK        (0x20000000u)
#define EDMA3_CCRL_SECRH_E61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_SECRH_E61_RESETVAL    (0x00000000u)

/*----E61 Tokens----*/
#define EDMA3_CCRL_SECRH_E61_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E60_MASK        (0x10000000u)
#define EDMA3_CCRL_SECRH_E60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_SECRH_E60_RESETVAL    (0x00000000u)

/*----E60 Tokens----*/
#define EDMA3_CCRL_SECRH_E60_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E59_MASK        (0x08000000u)
#define EDMA3_CCRL_SECRH_E59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_SECRH_E59_RESETVAL    (0x00000000u)

/*----E59 Tokens----*/
#define EDMA3_CCRL_SECRH_E59_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E58_MASK        (0x04000000u)
#define EDMA3_CCRL_SECRH_E58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_SECRH_E58_RESETVAL    (0x00000000u)

/*----E58 Tokens----*/
#define EDMA3_CCRL_SECRH_E58_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E57_MASK        (0x02000000u)
#define EDMA3_CCRL_SECRH_E57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_SECRH_E57_RESETVAL    (0x00000000u)

/*----E57 Tokens----*/
#define EDMA3_CCRL_SECRH_E57_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E56_MASK        (0x01000000u)
#define EDMA3_CCRL_SECRH_E56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_SECRH_E56_RESETVAL    (0x00000000u)

/*----E56 Tokens----*/
#define EDMA3_CCRL_SECRH_E56_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E55_MASK        (0x00800000u)
#define EDMA3_CCRL_SECRH_E55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_SECRH_E55_RESETVAL    (0x00000000u)

/*----E55 Tokens----*/
#define EDMA3_CCRL_SECRH_E55_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E54_MASK        (0x00400000u)
#define EDMA3_CCRL_SECRH_E54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_SECRH_E54_RESETVAL    (0x00000000u)

/*----E54 Tokens----*/
#define EDMA3_CCRL_SECRH_E54_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E53_MASK        (0x00200000u)
#define EDMA3_CCRL_SECRH_E53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_SECRH_E53_RESETVAL    (0x00000000u)

/*----E53 Tokens----*/
#define EDMA3_CCRL_SECRH_E53_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E52_MASK        (0x00100000u)
#define EDMA3_CCRL_SECRH_E52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_SECRH_E52_RESETVAL    (0x00000000u)

/*----E52 Tokens----*/
#define EDMA3_CCRL_SECRH_E52_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E51_MASK        (0x00080000u)
#define EDMA3_CCRL_SECRH_E51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_SECRH_E51_RESETVAL    (0x00000000u)

/*----E51 Tokens----*/
#define EDMA3_CCRL_SECRH_E51_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E50_MASK        (0x00040000u)
#define EDMA3_CCRL_SECRH_E50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_SECRH_E50_RESETVAL    (0x00000000u)

/*----E50 Tokens----*/
#define EDMA3_CCRL_SECRH_E50_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E49_MASK        (0x00020000u)
#define EDMA3_CCRL_SECRH_E49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_SECRH_E49_RESETVAL    (0x00000000u)

/*----E49 Tokens----*/
#define EDMA3_CCRL_SECRH_E49_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E48_MASK        (0x00010000u)
#define EDMA3_CCRL_SECRH_E48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_SECRH_E48_RESETVAL    (0x00000000u)

/*----E48 Tokens----*/
#define EDMA3_CCRL_SECRH_E48_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E47_MASK        (0x00008000u)
#define EDMA3_CCRL_SECRH_E47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_SECRH_E47_RESETVAL    (0x00000000u)

/*----E47 Tokens----*/
#define EDMA3_CCRL_SECRH_E47_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E46_MASK        (0x00004000u)
#define EDMA3_CCRL_SECRH_E46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_SECRH_E46_RESETVAL    (0x00000000u)

/*----E46 Tokens----*/
#define EDMA3_CCRL_SECRH_E46_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E45_MASK        (0x00002000u)
#define EDMA3_CCRL_SECRH_E45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_SECRH_E45_RESETVAL    (0x00000000u)

/*----E45 Tokens----*/
#define EDMA3_CCRL_SECRH_E45_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E44_MASK        (0x00001000u)
#define EDMA3_CCRL_SECRH_E44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_SECRH_E44_RESETVAL    (0x00000000u)

/*----E44 Tokens----*/
#define EDMA3_CCRL_SECRH_E44_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E43_MASK        (0x00000800u)
#define EDMA3_CCRL_SECRH_E43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_SECRH_E43_RESETVAL    (0x00000000u)

/*----E43 Tokens----*/
#define EDMA3_CCRL_SECRH_E43_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E42_MASK        (0x00000400u)
#define EDMA3_CCRL_SECRH_E42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_SECRH_E42_RESETVAL    (0x00000000u)

/*----E42 Tokens----*/
#define EDMA3_CCRL_SECRH_E42_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E41_MASK        (0x00000200u)
#define EDMA3_CCRL_SECRH_E41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_SECRH_E41_RESETVAL    (0x00000000u)

/*----E41 Tokens----*/
#define EDMA3_CCRL_SECRH_E41_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E40_MASK        (0x00000100u)
#define EDMA3_CCRL_SECRH_E40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_SECRH_E40_RESETVAL    (0x00000000u)

/*----E40 Tokens----*/
#define EDMA3_CCRL_SECRH_E40_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E39_MASK        (0x00000080u)
#define EDMA3_CCRL_SECRH_E39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_SECRH_E39_RESETVAL    (0x00000000u)

/*----E39 Tokens----*/
#define EDMA3_CCRL_SECRH_E39_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E38_MASK        (0x00000040u)
#define EDMA3_CCRL_SECRH_E38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_SECRH_E38_RESETVAL    (0x00000000u)

/*----E38 Tokens----*/
#define EDMA3_CCRL_SECRH_E38_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E37_MASK        (0x00000020u)
#define EDMA3_CCRL_SECRH_E37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_SECRH_E37_RESETVAL    (0x00000000u)

/*----E37 Tokens----*/
#define EDMA3_CCRL_SECRH_E37_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E36_MASK        (0x00000010u)
#define EDMA3_CCRL_SECRH_E36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_SECRH_E36_RESETVAL    (0x00000000u)

/*----E36 Tokens----*/
#define EDMA3_CCRL_SECRH_E36_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E35_MASK        (0x00000008u)
#define EDMA3_CCRL_SECRH_E35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_SECRH_E35_RESETVAL    (0x00000000u)

/*----E35 Tokens----*/
#define EDMA3_CCRL_SECRH_E35_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E34_MASK        (0x00000004u)
#define EDMA3_CCRL_SECRH_E34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_SECRH_E34_RESETVAL    (0x00000000u)

/*----E34 Tokens----*/
#define EDMA3_CCRL_SECRH_E34_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E33_MASK        (0x00000002u)
#define EDMA3_CCRL_SECRH_E33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_SECRH_E33_RESETVAL    (0x00000000u)

/*----E33 Tokens----*/
#define EDMA3_CCRL_SECRH_E33_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_E32_MASK        (0x00000001u)
#define EDMA3_CCRL_SECRH_E32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_SECRH_E32_RESETVAL    (0x00000000u)

/*----E32 Tokens----*/
#define EDMA3_CCRL_SECRH_E32_CLEAR       (0x00000001u)

#define EDMA3_CCRL_SECRH_RESETVAL        (0x00000000u)

/* IER */

#define EDMA3_CCRL_IER_I31_MASK          (0x80000000u)
#define EDMA3_CCRL_IER_I31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_IER_I31_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I30_MASK          (0x40000000u)
#define EDMA3_CCRL_IER_I30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_IER_I30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I29_MASK          (0x20000000u)
#define EDMA3_CCRL_IER_I29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_IER_I29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I28_MASK          (0x10000000u)
#define EDMA3_CCRL_IER_I28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_IER_I28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I27_MASK          (0x08000000u)
#define EDMA3_CCRL_IER_I27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_IER_I27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I26_MASK          (0x04000000u)
#define EDMA3_CCRL_IER_I26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_IER_I26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I25_MASK          (0x02000000u)
#define EDMA3_CCRL_IER_I25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_IER_I25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I24_MASK          (0x01000000u)
#define EDMA3_CCRL_IER_I24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_IER_I24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I23_MASK          (0x00800000u)
#define EDMA3_CCRL_IER_I23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_IER_I23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I22_MASK          (0x00400000u)
#define EDMA3_CCRL_IER_I22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_IER_I22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I21_MASK          (0x00200000u)
#define EDMA3_CCRL_IER_I21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_IER_I21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I20_MASK          (0x00100000u)
#define EDMA3_CCRL_IER_I20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_IER_I20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I19_MASK          (0x00080000u)
#define EDMA3_CCRL_IER_I19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_IER_I19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I18_MASK          (0x00040000u)
#define EDMA3_CCRL_IER_I18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_IER_I18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I17_MASK          (0x00020000u)
#define EDMA3_CCRL_IER_I17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_IER_I17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I16_MASK          (0x00010000u)
#define EDMA3_CCRL_IER_I16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_IER_I16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I15_MASK          (0x00008000u)
#define EDMA3_CCRL_IER_I15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_IER_I15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I14_MASK          (0x00004000u)
#define EDMA3_CCRL_IER_I14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_IER_I14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I13_MASK          (0x00002000u)
#define EDMA3_CCRL_IER_I13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_IER_I13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I12_MASK          (0x00001000u)
#define EDMA3_CCRL_IER_I12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_IER_I12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I11_MASK          (0x00000800u)
#define EDMA3_CCRL_IER_I11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_IER_I11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I10_MASK          (0x00000400u)
#define EDMA3_CCRL_IER_I10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_IER_I10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_I9_MASK           (0x00000200u)
#define EDMA3_CCRL_IER_I9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_IER_I9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I8_MASK           (0x00000100u)
#define EDMA3_CCRL_IER_I8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_IER_I8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I7_MASK           (0x00000080u)
#define EDMA3_CCRL_IER_I7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_IER_I7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I6_MASK           (0x00000040u)
#define EDMA3_CCRL_IER_I6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_IER_I6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I5_MASK           (0x00000020u)
#define EDMA3_CCRL_IER_I5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_IER_I5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I4_MASK           (0x00000010u)
#define EDMA3_CCRL_IER_I4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_IER_I4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I3_MASK           (0x00000008u)
#define EDMA3_CCRL_IER_I3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_IER_I3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I2_MASK           (0x00000004u)
#define EDMA3_CCRL_IER_I2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_IER_I2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I1_MASK           (0x00000002u)
#define EDMA3_CCRL_IER_I1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_IER_I1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_I0_MASK           (0x00000001u)
#define EDMA3_CCRL_IER_I0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_IER_I0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IER_RESETVAL          (0x00000000u)

/* IERH */

#define EDMA3_CCRL_IERH_I63_MASK         (0x80000000u)
#define EDMA3_CCRL_IERH_I63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_IERH_I63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I62_MASK         (0x40000000u)
#define EDMA3_CCRL_IERH_I62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_IERH_I62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I61_MASK         (0x20000000u)
#define EDMA3_CCRL_IERH_I61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_IERH_I61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I60_MASK         (0x10000000u)
#define EDMA3_CCRL_IERH_I60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_IERH_I60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I59_MASK         (0x08000000u)
#define EDMA3_CCRL_IERH_I59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_IERH_I59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I58_MASK         (0x04000000u)
#define EDMA3_CCRL_IERH_I58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_IERH_I58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I57_MASK         (0x02000000u)
#define EDMA3_CCRL_IERH_I57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_IERH_I57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I56_MASK         (0x01000000u)
#define EDMA3_CCRL_IERH_I56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_IERH_I56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I55_MASK         (0x00800000u)
#define EDMA3_CCRL_IERH_I55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_IERH_I55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I54_MASK         (0x00400000u)
#define EDMA3_CCRL_IERH_I54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_IERH_I54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I53_MASK         (0x00200000u)
#define EDMA3_CCRL_IERH_I53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_IERH_I53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I52_MASK         (0x00100000u)
#define EDMA3_CCRL_IERH_I52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_IERH_I52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I51_MASK         (0x00080000u)
#define EDMA3_CCRL_IERH_I51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_IERH_I51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I50_MASK         (0x00040000u)
#define EDMA3_CCRL_IERH_I50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_IERH_I50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I49_MASK         (0x00020000u)
#define EDMA3_CCRL_IERH_I49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_IERH_I49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I48_MASK         (0x00010000u)
#define EDMA3_CCRL_IERH_I48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_IERH_I48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I47_MASK         (0x00008000u)
#define EDMA3_CCRL_IERH_I47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_IERH_I47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I46_MASK         (0x00004000u)
#define EDMA3_CCRL_IERH_I46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_IERH_I46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I45_MASK         (0x00002000u)
#define EDMA3_CCRL_IERH_I45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_IERH_I45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I44_MASK         (0x00001000u)
#define EDMA3_CCRL_IERH_I44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_IERH_I44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I43_MASK         (0x00000800u)
#define EDMA3_CCRL_IERH_I43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_IERH_I43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I42_MASK         (0x00000400u)
#define EDMA3_CCRL_IERH_I42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_IERH_I42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I41_MASK         (0x00000200u)
#define EDMA3_CCRL_IERH_I41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_IERH_I41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I40_MASK         (0x00000100u)
#define EDMA3_CCRL_IERH_I40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_IERH_I40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I39_MASK         (0x00000080u)
#define EDMA3_CCRL_IERH_I39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_IERH_I39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I38_MASK         (0x00000040u)
#define EDMA3_CCRL_IERH_I38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_IERH_I38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I37_MASK         (0x00000020u)
#define EDMA3_CCRL_IERH_I37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_IERH_I37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I36_MASK         (0x00000010u)
#define EDMA3_CCRL_IERH_I36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_IERH_I36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I35_MASK         (0x00000008u)
#define EDMA3_CCRL_IERH_I35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_IERH_I35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I34_MASK         (0x00000004u)
#define EDMA3_CCRL_IERH_I34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_IERH_I34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I33_MASK         (0x00000002u)
#define EDMA3_CCRL_IERH_I33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_IERH_I33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_I32_MASK         (0x00000001u)
#define EDMA3_CCRL_IERH_I32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IERH_I32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_RESETVAL         (0x00000000u)

/* IECR */

#define EDMA3_CCRL_IECR_I31_MASK         (0x80000000u)
#define EDMA3_CCRL_IECR_I31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_IECR_I31_RESETVAL     (0x00000000u)

/*----I31 Tokens----*/
#define EDMA3_CCRL_IECR_I31_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I30_MASK         (0x40000000u)
#define EDMA3_CCRL_IECR_I30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_IECR_I30_RESETVAL     (0x00000000u)

/*----I30 Tokens----*/
#define EDMA3_CCRL_IECR_I30_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I29_MASK         (0x20000000u)
#define EDMA3_CCRL_IECR_I29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_IECR_I29_RESETVAL     (0x00000000u)

/*----I29 Tokens----*/
#define EDMA3_CCRL_IECR_I29_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I28_MASK         (0x10000000u)
#define EDMA3_CCRL_IECR_I28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_IECR_I28_RESETVAL     (0x00000000u)

/*----I28 Tokens----*/
#define EDMA3_CCRL_IECR_I28_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I27_MASK         (0x08000000u)
#define EDMA3_CCRL_IECR_I27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_IECR_I27_RESETVAL     (0x00000000u)

/*----I27 Tokens----*/
#define EDMA3_CCRL_IECR_I27_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I26_MASK         (0x04000000u)
#define EDMA3_CCRL_IECR_I26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_IECR_I26_RESETVAL     (0x00000000u)

/*----I26 Tokens----*/
#define EDMA3_CCRL_IECR_I26_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I25_MASK         (0x02000000u)
#define EDMA3_CCRL_IECR_I25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_IECR_I25_RESETVAL     (0x00000000u)

/*----I25 Tokens----*/
#define EDMA3_CCRL_IECR_I25_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I24_MASK         (0x01000000u)
#define EDMA3_CCRL_IECR_I24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_IECR_I24_RESETVAL     (0x00000000u)

/*----I24 Tokens----*/
#define EDMA3_CCRL_IECR_I24_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I23_MASK         (0x00800000u)
#define EDMA3_CCRL_IECR_I23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_IECR_I23_RESETVAL     (0x00000000u)

/*----I23 Tokens----*/
#define EDMA3_CCRL_IECR_I23_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I22_MASK         (0x00400000u)
#define EDMA3_CCRL_IECR_I22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_IECR_I22_RESETVAL     (0x00000000u)

/*----I22 Tokens----*/
#define EDMA3_CCRL_IECR_I22_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I21_MASK         (0x00200000u)
#define EDMA3_CCRL_IECR_I21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_IECR_I21_RESETVAL     (0x00000000u)

/*----I21 Tokens----*/
#define EDMA3_CCRL_IECR_I21_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I20_MASK         (0x00100000u)
#define EDMA3_CCRL_IECR_I20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_IECR_I20_RESETVAL     (0x00000000u)

/*----I20 Tokens----*/
#define EDMA3_CCRL_IECR_I20_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I19_MASK         (0x00080000u)
#define EDMA3_CCRL_IECR_I19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_IECR_I19_RESETVAL     (0x00000000u)

/*----I19 Tokens----*/
#define EDMA3_CCRL_IECR_I19_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I18_MASK         (0x00040000u)
#define EDMA3_CCRL_IECR_I18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_IECR_I18_RESETVAL     (0x00000000u)

/*----I18 Tokens----*/
#define EDMA3_CCRL_IECR_I18_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I17_MASK         (0x00020000u)
#define EDMA3_CCRL_IECR_I17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_IECR_I17_RESETVAL     (0x00000000u)

/*----I17 Tokens----*/
#define EDMA3_CCRL_IECR_I17_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I16_MASK         (0x00010000u)
#define EDMA3_CCRL_IECR_I16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_IECR_I16_RESETVAL     (0x00000000u)

/*----I16 Tokens----*/
#define EDMA3_CCRL_IECR_I16_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I15_MASK         (0x00008000u)
#define EDMA3_CCRL_IECR_I15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_IECR_I15_RESETVAL     (0x00000000u)

/*----I15 Tokens----*/
#define EDMA3_CCRL_IECR_I15_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I14_MASK         (0x00004000u)
#define EDMA3_CCRL_IECR_I14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_IECR_I14_RESETVAL     (0x00000000u)

/*----I14 Tokens----*/
#define EDMA3_CCRL_IECR_I14_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I13_MASK         (0x00002000u)
#define EDMA3_CCRL_IECR_I13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_IECR_I13_RESETVAL     (0x00000000u)

/*----I13 Tokens----*/
#define EDMA3_CCRL_IECR_I13_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I12_MASK         (0x00001000u)
#define EDMA3_CCRL_IECR_I12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_IECR_I12_RESETVAL     (0x00000000u)

/*----I12 Tokens----*/
#define EDMA3_CCRL_IECR_I12_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I11_MASK         (0x00000800u)
#define EDMA3_CCRL_IECR_I11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_IECR_I11_RESETVAL     (0x00000000u)

/*----I11 Tokens----*/
#define EDMA3_CCRL_IECR_I11_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I10_MASK         (0x00000400u)
#define EDMA3_CCRL_IECR_I10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_IECR_I10_RESETVAL     (0x00000000u)

/*----I10 Tokens----*/
#define EDMA3_CCRL_IECR_I10_CLEAR        (0x00000001u)

#define EDMA3_CCRL_IECR_I9_MASK          (0x00000200u)
#define EDMA3_CCRL_IECR_I9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_IECR_I9_RESETVAL      (0x00000000u)

/*----I9 Tokens----*/
#define EDMA3_CCRL_IECR_I9_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I8_MASK          (0x00000100u)
#define EDMA3_CCRL_IECR_I8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_IECR_I8_RESETVAL      (0x00000000u)

/*----I8 Tokens----*/
#define EDMA3_CCRL_IECR_I8_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I7_MASK          (0x00000080u)
#define EDMA3_CCRL_IECR_I7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_IECR_I7_RESETVAL      (0x00000000u)

/*----I7 Tokens----*/
#define EDMA3_CCRL_IECR_I7_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I6_MASK          (0x00000040u)
#define EDMA3_CCRL_IECR_I6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_IECR_I6_RESETVAL      (0x00000000u)

/*----I6 Tokens----*/
#define EDMA3_CCRL_IECR_I6_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I5_MASK          (0x00000020u)
#define EDMA3_CCRL_IECR_I5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_IECR_I5_RESETVAL      (0x00000000u)

/*----I5 Tokens----*/
#define EDMA3_CCRL_IECR_I5_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I4_MASK          (0x00000010u)
#define EDMA3_CCRL_IECR_I4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_IECR_I4_RESETVAL      (0x00000000u)

/*----I4 Tokens----*/
#define EDMA3_CCRL_IECR_I4_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I3_MASK          (0x00000008u)
#define EDMA3_CCRL_IECR_I3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_IECR_I3_RESETVAL      (0x00000000u)

/*----I3 Tokens----*/
#define EDMA3_CCRL_IECR_I3_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I2_MASK          (0x00000004u)
#define EDMA3_CCRL_IECR_I2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_IECR_I2_RESETVAL      (0x00000000u)

/*----I2 Tokens----*/
#define EDMA3_CCRL_IECR_I2_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I1_MASK          (0x00000002u)
#define EDMA3_CCRL_IECR_I1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_IECR_I1_RESETVAL      (0x00000000u)

/*----I1 Tokens----*/
#define EDMA3_CCRL_IECR_I1_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_I0_MASK          (0x00000001u)
#define EDMA3_CCRL_IECR_I0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_IECR_I0_RESETVAL      (0x00000000u)

/*----I0 Tokens----*/
#define EDMA3_CCRL_IECR_I0_CLEAR         (0x00000001u)

#define EDMA3_CCRL_IECR_RESETVAL         (0x00000000u)

/* IECRH */

#define EDMA3_CCRL_IECRH_I63_MASK        (0x80000000u)
#define EDMA3_CCRL_IECRH_I63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_IECRH_I63_RESETVAL    (0x00000000u)

/*----I63 Tokens----*/
#define EDMA3_CCRL_IECRH_I63_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I62_MASK        (0x40000000u)
#define EDMA3_CCRL_IECRH_I62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_IECRH_I62_RESETVAL    (0x00000000u)

/*----I62 Tokens----*/
#define EDMA3_CCRL_IECRH_I62_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I61_MASK        (0x20000000u)
#define EDMA3_CCRL_IECRH_I61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_IECRH_I61_RESETVAL    (0x00000000u)

/*----I61 Tokens----*/
#define EDMA3_CCRL_IECRH_I61_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I60_MASK        (0x10000000u)
#define EDMA3_CCRL_IECRH_I60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_IECRH_I60_RESETVAL    (0x00000000u)

/*----I60 Tokens----*/
#define EDMA3_CCRL_IECRH_I60_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I59_MASK        (0x08000000u)
#define EDMA3_CCRL_IECRH_I59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_IECRH_I59_RESETVAL    (0x00000000u)

/*----I59 Tokens----*/
#define EDMA3_CCRL_IECRH_I59_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I58_MASK        (0x04000000u)
#define EDMA3_CCRL_IECRH_I58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_IECRH_I58_RESETVAL    (0x00000000u)

/*----I58 Tokens----*/
#define EDMA3_CCRL_IECRH_I58_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I57_MASK        (0x02000000u)
#define EDMA3_CCRL_IECRH_I57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_IECRH_I57_RESETVAL    (0x00000000u)

/*----I57 Tokens----*/
#define EDMA3_CCRL_IECRH_I57_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I56_MASK        (0x01000000u)
#define EDMA3_CCRL_IECRH_I56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_IECRH_I56_RESETVAL    (0x00000000u)

/*----I56 Tokens----*/
#define EDMA3_CCRL_IECRH_I56_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I55_MASK        (0x00800000u)
#define EDMA3_CCRL_IECRH_I55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_IECRH_I55_RESETVAL    (0x00000000u)

/*----I55 Tokens----*/
#define EDMA3_CCRL_IECRH_I55_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I54_MASK        (0x00400000u)
#define EDMA3_CCRL_IECRH_I54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_IECRH_I54_RESETVAL    (0x00000000u)

/*----I54 Tokens----*/
#define EDMA3_CCRL_IECRH_I54_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I53_MASK        (0x00200000u)
#define EDMA3_CCRL_IECRH_I53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_IECRH_I53_RESETVAL    (0x00000000u)

/*----I53 Tokens----*/
#define EDMA3_CCRL_IECRH_I53_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I52_MASK        (0x00100000u)
#define EDMA3_CCRL_IECRH_I52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_IECRH_I52_RESETVAL    (0x00000000u)

/*----I52 Tokens----*/
#define EDMA3_CCRL_IECRH_I52_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I51_MASK        (0x00080000u)
#define EDMA3_CCRL_IECRH_I51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_IECRH_I51_RESETVAL    (0x00000000u)

/*----I51 Tokens----*/
#define EDMA3_CCRL_IECRH_I51_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I50_MASK        (0x00040000u)
#define EDMA3_CCRL_IECRH_I50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_IECRH_I50_RESETVAL    (0x00000000u)

/*----I50 Tokens----*/
#define EDMA3_CCRL_IECRH_I50_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I49_MASK        (0x00020000u)
#define EDMA3_CCRL_IECRH_I49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_IECRH_I49_RESETVAL    (0x00000000u)

/*----I49 Tokens----*/
#define EDMA3_CCRL_IECRH_I49_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I48_MASK        (0x00010000u)
#define EDMA3_CCRL_IECRH_I48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_IECRH_I48_RESETVAL    (0x00000000u)

/*----I48 Tokens----*/
#define EDMA3_CCRL_IECRH_I48_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I47_MASK        (0x00008000u)
#define EDMA3_CCRL_IECRH_I47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_IECRH_I47_RESETVAL    (0x00000000u)

/*----I47 Tokens----*/
#define EDMA3_CCRL_IECRH_I47_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I46_MASK        (0x00004000u)
#define EDMA3_CCRL_IECRH_I46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_IECRH_I46_RESETVAL    (0x00000000u)

/*----I46 Tokens----*/
#define EDMA3_CCRL_IECRH_I46_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I45_MASK        (0x00002000u)
#define EDMA3_CCRL_IECRH_I45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_IECRH_I45_RESETVAL    (0x00000000u)

/*----I45 Tokens----*/
#define EDMA3_CCRL_IECRH_I45_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I44_MASK        (0x00001000u)
#define EDMA3_CCRL_IECRH_I44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_IECRH_I44_RESETVAL    (0x00000000u)

/*----I44 Tokens----*/
#define EDMA3_CCRL_IECRH_I44_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I43_MASK        (0x00000800u)
#define EDMA3_CCRL_IECRH_I43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_IECRH_I43_RESETVAL    (0x00000000u)

/*----I43 Tokens----*/
#define EDMA3_CCRL_IECRH_I43_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I42_MASK        (0x00000400u)
#define EDMA3_CCRL_IECRH_I42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_IECRH_I42_RESETVAL    (0x00000000u)

/*----I42 Tokens----*/
#define EDMA3_CCRL_IECRH_I42_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I41_MASK        (0x00000200u)
#define EDMA3_CCRL_IECRH_I41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_IECRH_I41_RESETVAL    (0x00000000u)

/*----I41 Tokens----*/
#define EDMA3_CCRL_IECRH_I41_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I40_MASK        (0x00000100u)
#define EDMA3_CCRL_IECRH_I40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_IECRH_I40_RESETVAL    (0x00000000u)

/*----I40 Tokens----*/
#define EDMA3_CCRL_IECRH_I40_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I39_MASK        (0x00000080u)
#define EDMA3_CCRL_IECRH_I39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_IECRH_I39_RESETVAL    (0x00000000u)

/*----I39 Tokens----*/
#define EDMA3_CCRL_IECRH_I39_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I38_MASK        (0x00000040u)
#define EDMA3_CCRL_IECRH_I38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_IECRH_I38_RESETVAL    (0x00000000u)

/*----I38 Tokens----*/
#define EDMA3_CCRL_IECRH_I38_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I37_MASK        (0x00000020u)
#define EDMA3_CCRL_IECRH_I37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_IECRH_I37_RESETVAL    (0x00000000u)

/*----I37 Tokens----*/
#define EDMA3_CCRL_IECRH_I37_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I36_MASK        (0x00000010u)
#define EDMA3_CCRL_IECRH_I36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_IECRH_I36_RESETVAL    (0x00000000u)

/*----I36 Tokens----*/
#define EDMA3_CCRL_IECRH_I36_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I35_MASK        (0x00000008u)
#define EDMA3_CCRL_IECRH_I35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_IECRH_I35_RESETVAL    (0x00000000u)

/*----I35 Tokens----*/
#define EDMA3_CCRL_IECRH_I35_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I34_MASK        (0x00000004u)
#define EDMA3_CCRL_IECRH_I34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_IECRH_I34_RESETVAL    (0x00000000u)

/*----I34 Tokens----*/
#define EDMA3_CCRL_IECRH_I34_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I33_MASK        (0x00000002u)
#define EDMA3_CCRL_IECRH_I33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_IECRH_I33_RESETVAL    (0x00000000u)

/*----I33 Tokens----*/
#define EDMA3_CCRL_IECRH_I33_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_I32_MASK        (0x00000001u)
#define EDMA3_CCRL_IECRH_I32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_IECRH_I32_RESETVAL    (0x00000000u)

/*----I32 Tokens----*/
#define EDMA3_CCRL_IECRH_I32_CLEAR       (0x00000001u)

#define EDMA3_CCRL_IECRH_RESETVAL        (0x00000000u)

/* IESR */

#define EDMA3_CCRL_IESR_I31_MASK         (0x80000000u)
#define EDMA3_CCRL_IESR_I31_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_IESR_I31_RESETVAL     (0x00000000u)

/*----I31 Tokens----*/
#define EDMA3_CCRL_IESR_I31_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I30_MASK         (0x40000000u)
#define EDMA3_CCRL_IESR_I30_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_IESR_I30_RESETVAL     (0x00000000u)

/*----I30 Tokens----*/
#define EDMA3_CCRL_IESR_I30_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I29_MASK         (0x20000000u)
#define EDMA3_CCRL_IESR_I29_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_IESR_I29_RESETVAL     (0x00000000u)

/*----I29 Tokens----*/
#define EDMA3_CCRL_IESR_I29_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I28_MASK         (0x10000000u)
#define EDMA3_CCRL_IESR_I28_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_IESR_I28_RESETVAL     (0x00000000u)

/*----I28 Tokens----*/
#define EDMA3_CCRL_IESR_I28_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I27_MASK         (0x08000000u)
#define EDMA3_CCRL_IESR_I27_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_IESR_I27_RESETVAL     (0x00000000u)

/*----I27 Tokens----*/
#define EDMA3_CCRL_IESR_I27_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I26_MASK         (0x04000000u)
#define EDMA3_CCRL_IESR_I26_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_IESR_I26_RESETVAL     (0x00000000u)

/*----I26 Tokens----*/
#define EDMA3_CCRL_IESR_I26_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I25_MASK         (0x02000000u)
#define EDMA3_CCRL_IESR_I25_SHIFT        (0x00000019u)
#define EDMA3_CCRL_IESR_I25_RESETVAL     (0x00000000u)

/*----I25 Tokens----*/
#define EDMA3_CCRL_IESR_I25_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I24_MASK         (0x01000000u)
#define EDMA3_CCRL_IESR_I24_SHIFT        (0x00000018u)
#define EDMA3_CCRL_IESR_I24_RESETVAL     (0x00000000u)

/*----I24 Tokens----*/
#define EDMA3_CCRL_IESR_I24_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I23_MASK         (0x00800000u)
#define EDMA3_CCRL_IESR_I23_SHIFT        (0x00000017u)
#define EDMA3_CCRL_IESR_I23_RESETVAL     (0x00000000u)

/*----I23 Tokens----*/
#define EDMA3_CCRL_IESR_I23_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I22_MASK         (0x00400000u)
#define EDMA3_CCRL_IESR_I22_SHIFT        (0x00000016u)
#define EDMA3_CCRL_IESR_I22_RESETVAL     (0x00000000u)

/*----I22 Tokens----*/
#define EDMA3_CCRL_IESR_I22_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I21_MASK         (0x00200000u)
#define EDMA3_CCRL_IESR_I21_SHIFT        (0x00000015u)
#define EDMA3_CCRL_IESR_I21_RESETVAL     (0x00000000u)

/*----I21 Tokens----*/
#define EDMA3_CCRL_IESR_I21_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I20_MASK         (0x00100000u)
#define EDMA3_CCRL_IESR_I20_SHIFT        (0x00000014u)
#define EDMA3_CCRL_IESR_I20_RESETVAL     (0x00000000u)

/*----I20 Tokens----*/
#define EDMA3_CCRL_IESR_I20_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I19_MASK         (0x00080000u)
#define EDMA3_CCRL_IESR_I19_SHIFT        (0x00000013u)
#define EDMA3_CCRL_IESR_I19_RESETVAL     (0x00000000u)

/*----I19 Tokens----*/
#define EDMA3_CCRL_IESR_I19_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I18_MASK         (0x00040000u)
#define EDMA3_CCRL_IESR_I18_SHIFT        (0x00000012u)
#define EDMA3_CCRL_IESR_I18_RESETVAL     (0x00000000u)

/*----I18 Tokens----*/
#define EDMA3_CCRL_IESR_I18_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I17_MASK         (0x00020000u)
#define EDMA3_CCRL_IESR_I17_SHIFT        (0x00000011u)
#define EDMA3_CCRL_IESR_I17_RESETVAL     (0x00000000u)

/*----I17 Tokens----*/
#define EDMA3_CCRL_IESR_I17_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I16_MASK         (0x00010000u)
#define EDMA3_CCRL_IESR_I16_SHIFT        (0x00000010u)
#define EDMA3_CCRL_IESR_I16_RESETVAL     (0x00000000u)

/*----I16 Tokens----*/
#define EDMA3_CCRL_IESR_I16_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I15_MASK         (0x00008000u)
#define EDMA3_CCRL_IESR_I15_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_IESR_I15_RESETVAL     (0x00000000u)

/*----I15 Tokens----*/
#define EDMA3_CCRL_IESR_I15_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I14_MASK         (0x00004000u)
#define EDMA3_CCRL_IESR_I14_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_IESR_I14_RESETVAL     (0x00000000u)

/*----I14 Tokens----*/
#define EDMA3_CCRL_IESR_I14_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I13_MASK         (0x00002000u)
#define EDMA3_CCRL_IESR_I13_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_IESR_I13_RESETVAL     (0x00000000u)

/*----I13 Tokens----*/
#define EDMA3_CCRL_IESR_I13_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I12_MASK         (0x00001000u)
#define EDMA3_CCRL_IESR_I12_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_IESR_I12_RESETVAL     (0x00000000u)

/*----I12 Tokens----*/
#define EDMA3_CCRL_IESR_I12_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I11_MASK         (0x00000800u)
#define EDMA3_CCRL_IESR_I11_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_IESR_I11_RESETVAL     (0x00000000u)

/*----I11 Tokens----*/
#define EDMA3_CCRL_IESR_I11_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I10_MASK         (0x00000400u)
#define EDMA3_CCRL_IESR_I10_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_IESR_I10_RESETVAL     (0x00000000u)

/*----I10 Tokens----*/
#define EDMA3_CCRL_IESR_I10_SET          (0x00000001u)

#define EDMA3_CCRL_IESR_I9_MASK          (0x00000200u)
#define EDMA3_CCRL_IESR_I9_SHIFT         (0x00000009u)
#define EDMA3_CCRL_IESR_I9_RESETVAL      (0x00000000u)

/*----I9 Tokens----*/
#define EDMA3_CCRL_IESR_I9_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I8_MASK          (0x00000100u)
#define EDMA3_CCRL_IESR_I8_SHIFT         (0x00000008u)
#define EDMA3_CCRL_IESR_I8_RESETVAL      (0x00000000u)

/*----I8 Tokens----*/
#define EDMA3_CCRL_IESR_I8_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I7_MASK          (0x00000080u)
#define EDMA3_CCRL_IESR_I7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_IESR_I7_RESETVAL      (0x00000000u)

/*----I7 Tokens----*/
#define EDMA3_CCRL_IESR_I7_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I6_MASK          (0x00000040u)
#define EDMA3_CCRL_IESR_I6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_IESR_I6_RESETVAL      (0x00000000u)

/*----I6 Tokens----*/
#define EDMA3_CCRL_IESR_I6_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I5_MASK          (0x00000020u)
#define EDMA3_CCRL_IESR_I5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_IESR_I5_RESETVAL      (0x00000000u)

/*----I5 Tokens----*/
#define EDMA3_CCRL_IESR_I5_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I4_MASK          (0x00000010u)
#define EDMA3_CCRL_IESR_I4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_IESR_I4_RESETVAL      (0x00000000u)

/*----I4 Tokens----*/
#define EDMA3_CCRL_IESR_I4_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I3_MASK          (0x00000008u)
#define EDMA3_CCRL_IESR_I3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_IESR_I3_RESETVAL      (0x00000000u)

/*----I3 Tokens----*/
#define EDMA3_CCRL_IESR_I3_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I2_MASK          (0x00000004u)
#define EDMA3_CCRL_IESR_I2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_IESR_I2_RESETVAL      (0x00000000u)

/*----I2 Tokens----*/
#define EDMA3_CCRL_IESR_I2_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I1_MASK          (0x00000002u)
#define EDMA3_CCRL_IESR_I1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_IESR_I1_RESETVAL      (0x00000000u)

/*----I1 Tokens----*/
#define EDMA3_CCRL_IESR_I1_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_I0_MASK          (0x00000001u)
#define EDMA3_CCRL_IESR_I0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_IESR_I0_RESETVAL      (0x00000000u)

/*----I0 Tokens----*/
#define EDMA3_CCRL_IESR_I0_SET           (0x00000001u)

#define EDMA3_CCRL_IESR_RESETVAL         (0x00000000u)

/* IESRH */

#define EDMA3_CCRL_IESRH_I63_MASK        (0x80000000u)
#define EDMA3_CCRL_IESRH_I63_SHIFT       (0x0000001Fu)
#define EDMA3_CCRL_IESRH_I63_RESETVAL    (0x00000000u)

/*----I63 Tokens----*/
#define EDMA3_CCRL_IESRH_I63_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I62_MASK        (0x40000000u)
#define EDMA3_CCRL_IESRH_I62_SHIFT       (0x0000001Eu)
#define EDMA3_CCRL_IESRH_I62_RESETVAL    (0x00000000u)

/*----I62 Tokens----*/
#define EDMA3_CCRL_IESRH_I62_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I61_MASK        (0x20000000u)
#define EDMA3_CCRL_IESRH_I61_SHIFT       (0x0000001Du)
#define EDMA3_CCRL_IESRH_I61_RESETVAL    (0x00000000u)

/*----I61 Tokens----*/
#define EDMA3_CCRL_IESRH_I61_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I60_MASK        (0x10000000u)
#define EDMA3_CCRL_IESRH_I60_SHIFT       (0x0000001Cu)
#define EDMA3_CCRL_IESRH_I60_RESETVAL    (0x00000000u)

/*----I60 Tokens----*/
#define EDMA3_CCRL_IESRH_I60_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I59_MASK        (0x08000000u)
#define EDMA3_CCRL_IESRH_I59_SHIFT       (0x0000001Bu)
#define EDMA3_CCRL_IESRH_I59_RESETVAL    (0x00000000u)

/*----I59 Tokens----*/
#define EDMA3_CCRL_IESRH_I59_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I58_MASK        (0x04000000u)
#define EDMA3_CCRL_IESRH_I58_SHIFT       (0x0000001Au)
#define EDMA3_CCRL_IESRH_I58_RESETVAL    (0x00000000u)

/*----I58 Tokens----*/
#define EDMA3_CCRL_IESRH_I58_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I57_MASK        (0x02000000u)
#define EDMA3_CCRL_IESRH_I57_SHIFT       (0x00000019u)
#define EDMA3_CCRL_IESRH_I57_RESETVAL    (0x00000000u)

/*----I57 Tokens----*/
#define EDMA3_CCRL_IESRH_I57_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I56_MASK        (0x01000000u)
#define EDMA3_CCRL_IESRH_I56_SHIFT       (0x00000018u)
#define EDMA3_CCRL_IESRH_I56_RESETVAL    (0x00000000u)

/*----I56 Tokens----*/
#define EDMA3_CCRL_IESRH_I56_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I55_MASK        (0x00800000u)
#define EDMA3_CCRL_IESRH_I55_SHIFT       (0x00000017u)
#define EDMA3_CCRL_IESRH_I55_RESETVAL    (0x00000000u)

/*----I55 Tokens----*/
#define EDMA3_CCRL_IESRH_I55_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I54_MASK        (0x00400000u)
#define EDMA3_CCRL_IESRH_I54_SHIFT       (0x00000016u)
#define EDMA3_CCRL_IESRH_I54_RESETVAL    (0x00000000u)

/*----I54 Tokens----*/
#define EDMA3_CCRL_IESRH_I54_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I53_MASK        (0x00200000u)
#define EDMA3_CCRL_IESRH_I53_SHIFT       (0x00000015u)
#define EDMA3_CCRL_IESRH_I53_RESETVAL    (0x00000000u)

/*----I53 Tokens----*/
#define EDMA3_CCRL_IESRH_I53_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I52_MASK        (0x00100000u)
#define EDMA3_CCRL_IESRH_I52_SHIFT       (0x00000014u)
#define EDMA3_CCRL_IESRH_I52_RESETVAL    (0x00000000u)

/*----I52 Tokens----*/
#define EDMA3_CCRL_IESRH_I52_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I51_MASK        (0x00080000u)
#define EDMA3_CCRL_IESRH_I51_SHIFT       (0x00000013u)
#define EDMA3_CCRL_IESRH_I51_RESETVAL    (0x00000000u)

/*----I51 Tokens----*/
#define EDMA3_CCRL_IESRH_I51_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I50_MASK        (0x00040000u)
#define EDMA3_CCRL_IESRH_I50_SHIFT       (0x00000012u)
#define EDMA3_CCRL_IESRH_I50_RESETVAL    (0x00000000u)

/*----I50 Tokens----*/
#define EDMA3_CCRL_IESRH_I50_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I49_MASK        (0x00020000u)
#define EDMA3_CCRL_IESRH_I49_SHIFT       (0x00000011u)
#define EDMA3_CCRL_IESRH_I49_RESETVAL    (0x00000000u)

/*----I49 Tokens----*/
#define EDMA3_CCRL_IESRH_I49_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I48_MASK        (0x00010000u)
#define EDMA3_CCRL_IESRH_I48_SHIFT       (0x00000010u)
#define EDMA3_CCRL_IESRH_I48_RESETVAL    (0x00000000u)

/*----I48 Tokens----*/
#define EDMA3_CCRL_IESRH_I48_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I47_MASK        (0x00008000u)
#define EDMA3_CCRL_IESRH_I47_SHIFT       (0x0000000Fu)
#define EDMA3_CCRL_IESRH_I47_RESETVAL    (0x00000000u)

/*----I47 Tokens----*/
#define EDMA3_CCRL_IESRH_I47_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I46_MASK        (0x00004000u)
#define EDMA3_CCRL_IESRH_I46_SHIFT       (0x0000000Eu)
#define EDMA3_CCRL_IESRH_I46_RESETVAL    (0x00000000u)

/*----I46 Tokens----*/
#define EDMA3_CCRL_IESRH_I46_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I45_MASK        (0x00002000u)
#define EDMA3_CCRL_IESRH_I45_SHIFT       (0x0000000Du)
#define EDMA3_CCRL_IESRH_I45_RESETVAL    (0x00000000u)

/*----I45 Tokens----*/
#define EDMA3_CCRL_IESRH_I45_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I44_MASK        (0x00001000u)
#define EDMA3_CCRL_IESRH_I44_SHIFT       (0x0000000Cu)
#define EDMA3_CCRL_IESRH_I44_RESETVAL    (0x00000000u)

/*----I44 Tokens----*/
#define EDMA3_CCRL_IESRH_I44_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I43_MASK        (0x00000800u)
#define EDMA3_CCRL_IESRH_I43_SHIFT       (0x0000000Bu)
#define EDMA3_CCRL_IESRH_I43_RESETVAL    (0x00000000u)

/*----I43 Tokens----*/
#define EDMA3_CCRL_IESRH_I43_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I42_MASK        (0x00000400u)
#define EDMA3_CCRL_IESRH_I42_SHIFT       (0x0000000Au)
#define EDMA3_CCRL_IESRH_I42_RESETVAL    (0x00000000u)

/*----I42 Tokens----*/
#define EDMA3_CCRL_IESRH_I42_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I41_MASK        (0x00000200u)
#define EDMA3_CCRL_IESRH_I41_SHIFT       (0x00000009u)
#define EDMA3_CCRL_IESRH_I41_RESETVAL    (0x00000000u)

/*----I41 Tokens----*/
#define EDMA3_CCRL_IESRH_I41_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I40_MASK        (0x00000100u)
#define EDMA3_CCRL_IESRH_I40_SHIFT       (0x00000008u)
#define EDMA3_CCRL_IESRH_I40_RESETVAL    (0x00000000u)

/*----I40 Tokens----*/
#define EDMA3_CCRL_IESRH_I40_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I39_MASK        (0x00000080u)
#define EDMA3_CCRL_IESRH_I39_SHIFT       (0x00000007u)
#define EDMA3_CCRL_IESRH_I39_RESETVAL    (0x00000000u)

/*----I39 Tokens----*/
#define EDMA3_CCRL_IESRH_I39_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I38_MASK        (0x00000040u)
#define EDMA3_CCRL_IESRH_I38_SHIFT       (0x00000006u)
#define EDMA3_CCRL_IESRH_I38_RESETVAL    (0x00000000u)

/*----I38 Tokens----*/
#define EDMA3_CCRL_IESRH_I38_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I37_MASK        (0x00000020u)
#define EDMA3_CCRL_IESRH_I37_SHIFT       (0x00000005u)
#define EDMA3_CCRL_IESRH_I37_RESETVAL    (0x00000000u)

/*----I37 Tokens----*/
#define EDMA3_CCRL_IESRH_I37_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I36_MASK        (0x00000010u)
#define EDMA3_CCRL_IESRH_I36_SHIFT       (0x00000004u)
#define EDMA3_CCRL_IESRH_I36_RESETVAL    (0x00000000u)

/*----I36 Tokens----*/
#define EDMA3_CCRL_IESRH_I36_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I35_MASK        (0x00000008u)
#define EDMA3_CCRL_IESRH_I35_SHIFT       (0x00000003u)
#define EDMA3_CCRL_IESRH_I35_RESETVAL    (0x00000000u)

/*----I35 Tokens----*/
#define EDMA3_CCRL_IESRH_I35_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I34_MASK        (0x00000004u)
#define EDMA3_CCRL_IESRH_I34_SHIFT       (0x00000002u)
#define EDMA3_CCRL_IESRH_I34_RESETVAL    (0x00000000u)

/*----I34 Tokens----*/
#define EDMA3_CCRL_IESRH_I34_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I33_MASK        (0x00000002u)
#define EDMA3_CCRL_IESRH_I33_SHIFT       (0x00000001u)
#define EDMA3_CCRL_IESRH_I33_RESETVAL    (0x00000000u)

/*----I33 Tokens----*/
#define EDMA3_CCRL_IESRH_I33_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_I32_MASK        (0x00000001u)
#define EDMA3_CCRL_IESRH_I32_SHIFT       (0x00000000u)
#define EDMA3_CCRL_IESRH_I32_RESETVAL    (0x00000000u)

/*----I32 Tokens----*/
#define EDMA3_CCRL_IESRH_I32_SET         (0x00000001u)

#define EDMA3_CCRL_IESRH_RESETVAL        (0x00000000u)

/* IPR */

#define EDMA3_CCRL_IPR_I31_MASK          (0x80000000u)
#define EDMA3_CCRL_IPR_I31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_IPR_I31_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I30_MASK          (0x40000000u)
#define EDMA3_CCRL_IPR_I30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_IPR_I30_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I29_MASK          (0x20000000u)
#define EDMA3_CCRL_IPR_I29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_IPR_I29_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I28_MASK          (0x10000000u)
#define EDMA3_CCRL_IPR_I28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_IPR_I28_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I27_MASK          (0x08000000u)
#define EDMA3_CCRL_IPR_I27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_IPR_I27_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I26_MASK          (0x04000000u)
#define EDMA3_CCRL_IPR_I26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_IPR_I26_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I25_MASK          (0x02000000u)
#define EDMA3_CCRL_IPR_I25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_IPR_I25_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I24_MASK          (0x01000000u)
#define EDMA3_CCRL_IPR_I24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_IPR_I24_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I23_MASK          (0x00800000u)
#define EDMA3_CCRL_IPR_I23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_IPR_I23_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I22_MASK          (0x00400000u)
#define EDMA3_CCRL_IPR_I22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_IPR_I22_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I21_MASK          (0x00200000u)
#define EDMA3_CCRL_IPR_I21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_IPR_I21_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I20_MASK          (0x00100000u)
#define EDMA3_CCRL_IPR_I20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_IPR_I20_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I19_MASK          (0x00080000u)
#define EDMA3_CCRL_IPR_I19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_IPR_I19_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I18_MASK          (0x00040000u)
#define EDMA3_CCRL_IPR_I18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_IPR_I18_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I17_MASK          (0x00020000u)
#define EDMA3_CCRL_IPR_I17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_IPR_I17_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I16_MASK          (0x00010000u)
#define EDMA3_CCRL_IPR_I16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_IPR_I16_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I15_MASK          (0x00008000u)
#define EDMA3_CCRL_IPR_I15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_IPR_I15_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I14_MASK          (0x00004000u)
#define EDMA3_CCRL_IPR_I14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_IPR_I14_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I13_MASK          (0x00002000u)
#define EDMA3_CCRL_IPR_I13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_IPR_I13_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I12_MASK          (0x00001000u)
#define EDMA3_CCRL_IPR_I12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_IPR_I12_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I11_MASK          (0x00000800u)
#define EDMA3_CCRL_IPR_I11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_IPR_I11_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I10_MASK          (0x00000400u)
#define EDMA3_CCRL_IPR_I10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_IPR_I10_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_I9_MASK           (0x00000200u)
#define EDMA3_CCRL_IPR_I9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_IPR_I9_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I8_MASK           (0x00000100u)
#define EDMA3_CCRL_IPR_I8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_IPR_I8_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I7_MASK           (0x00000080u)
#define EDMA3_CCRL_IPR_I7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_IPR_I7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I6_MASK           (0x00000040u)
#define EDMA3_CCRL_IPR_I6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_IPR_I6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I5_MASK           (0x00000020u)
#define EDMA3_CCRL_IPR_I5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_IPR_I5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I4_MASK           (0x00000010u)
#define EDMA3_CCRL_IPR_I4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_IPR_I4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I3_MASK           (0x00000008u)
#define EDMA3_CCRL_IPR_I3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_IPR_I3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I2_MASK           (0x00000004u)
#define EDMA3_CCRL_IPR_I2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_IPR_I2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I1_MASK           (0x00000002u)
#define EDMA3_CCRL_IPR_I1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_IPR_I1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_I0_MASK           (0x00000001u)
#define EDMA3_CCRL_IPR_I0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_IPR_I0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_IPR_RESETVAL          (0x00000000u)

/* IPRH */

#define EDMA3_CCRL_IPRH_I63_MASK         (0x80000000u)
#define EDMA3_CCRL_IPRH_I63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_IPRH_I63_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I62_MASK         (0x40000000u)
#define EDMA3_CCRL_IPRH_I62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_IPRH_I62_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I61_MASK         (0x20000000u)
#define EDMA3_CCRL_IPRH_I61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_IPRH_I61_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I60_MASK         (0x10000000u)
#define EDMA3_CCRL_IPRH_I60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_IPRH_I60_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I59_MASK         (0x08000000u)
#define EDMA3_CCRL_IPRH_I59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_IPRH_I59_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I58_MASK         (0x04000000u)
#define EDMA3_CCRL_IPRH_I58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_IPRH_I58_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I57_MASK         (0x02000000u)
#define EDMA3_CCRL_IPRH_I57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_IPRH_I57_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I56_MASK         (0x01000000u)
#define EDMA3_CCRL_IPRH_I56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_IPRH_I56_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I55_MASK         (0x00800000u)
#define EDMA3_CCRL_IPRH_I55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_IPRH_I55_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I54_MASK         (0x00400000u)
#define EDMA3_CCRL_IPRH_I54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_IPRH_I54_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I53_MASK         (0x00200000u)
#define EDMA3_CCRL_IPRH_I53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_IPRH_I53_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I52_MASK         (0x00100000u)
#define EDMA3_CCRL_IPRH_I52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_IPRH_I52_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I51_MASK         (0x00080000u)
#define EDMA3_CCRL_IPRH_I51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_IPRH_I51_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I50_MASK         (0x00040000u)
#define EDMA3_CCRL_IPRH_I50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_IPRH_I50_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I49_MASK         (0x00020000u)
#define EDMA3_CCRL_IPRH_I49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_IPRH_I49_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I48_MASK         (0x00010000u)
#define EDMA3_CCRL_IPRH_I48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_IPRH_I48_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I47_MASK         (0x00008000u)
#define EDMA3_CCRL_IPRH_I47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_IPRH_I47_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I46_MASK         (0x00004000u)
#define EDMA3_CCRL_IPRH_I46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_IPRH_I46_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I45_MASK         (0x00002000u)
#define EDMA3_CCRL_IPRH_I45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_IPRH_I45_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I44_MASK         (0x00001000u)
#define EDMA3_CCRL_IPRH_I44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_IPRH_I44_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I43_MASK         (0x00000800u)
#define EDMA3_CCRL_IPRH_I43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_IPRH_I43_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I42_MASK         (0x00000400u)
#define EDMA3_CCRL_IPRH_I42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_IPRH_I42_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I41_MASK         (0x00000200u)
#define EDMA3_CCRL_IPRH_I41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_IPRH_I41_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I40_MASK         (0x00000100u)
#define EDMA3_CCRL_IPRH_I40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_IPRH_I40_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I39_MASK         (0x00000080u)
#define EDMA3_CCRL_IPRH_I39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_IPRH_I39_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I38_MASK         (0x00000040u)
#define EDMA3_CCRL_IPRH_I38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_IPRH_I38_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I37_MASK         (0x00000020u)
#define EDMA3_CCRL_IPRH_I37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_IPRH_I37_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I36_MASK         (0x00000010u)
#define EDMA3_CCRL_IPRH_I36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_IPRH_I36_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I35_MASK         (0x00000008u)
#define EDMA3_CCRL_IPRH_I35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_IPRH_I35_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I34_MASK         (0x00000004u)
#define EDMA3_CCRL_IPRH_I34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_IPRH_I34_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I33_MASK         (0x00000002u)
#define EDMA3_CCRL_IPRH_I33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_IPRH_I33_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_I32_MASK         (0x00000001u)
#define EDMA3_CCRL_IPRH_I32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IPRH_I32_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_RESETVAL         (0x00000000u)

/* ICR */

#define EDMA3_CCRL_ICR_I31_MASK          (0x80000000u)
#define EDMA3_CCRL_ICR_I31_SHIFT         (0x0000001Fu)
#define EDMA3_CCRL_ICR_I31_RESETVAL      (0x00000000u)

/*----I31 Tokens----*/
#define EDMA3_CCRL_ICR_I31_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I30_MASK          (0x40000000u)
#define EDMA3_CCRL_ICR_I30_SHIFT         (0x0000001Eu)
#define EDMA3_CCRL_ICR_I30_RESETVAL      (0x00000000u)

/*----I30 Tokens----*/
#define EDMA3_CCRL_ICR_I30_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I29_MASK          (0x20000000u)
#define EDMA3_CCRL_ICR_I29_SHIFT         (0x0000001Du)
#define EDMA3_CCRL_ICR_I29_RESETVAL      (0x00000000u)

/*----I29 Tokens----*/
#define EDMA3_CCRL_ICR_I29_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I28_MASK          (0x10000000u)
#define EDMA3_CCRL_ICR_I28_SHIFT         (0x0000001Cu)
#define EDMA3_CCRL_ICR_I28_RESETVAL      (0x00000000u)

/*----I28 Tokens----*/
#define EDMA3_CCRL_ICR_I28_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I27_MASK          (0x08000000u)
#define EDMA3_CCRL_ICR_I27_SHIFT         (0x0000001Bu)
#define EDMA3_CCRL_ICR_I27_RESETVAL      (0x00000000u)

/*----I27 Tokens----*/
#define EDMA3_CCRL_ICR_I27_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I26_MASK          (0x04000000u)
#define EDMA3_CCRL_ICR_I26_SHIFT         (0x0000001Au)
#define EDMA3_CCRL_ICR_I26_RESETVAL      (0x00000000u)

/*----I26 Tokens----*/
#define EDMA3_CCRL_ICR_I26_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I25_MASK          (0x02000000u)
#define EDMA3_CCRL_ICR_I25_SHIFT         (0x00000019u)
#define EDMA3_CCRL_ICR_I25_RESETVAL      (0x00000000u)

/*----I25 Tokens----*/
#define EDMA3_CCRL_ICR_I25_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I24_MASK          (0x01000000u)
#define EDMA3_CCRL_ICR_I24_SHIFT         (0x00000018u)
#define EDMA3_CCRL_ICR_I24_RESETVAL      (0x00000000u)

/*----I24 Tokens----*/
#define EDMA3_CCRL_ICR_I24_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I23_MASK          (0x00800000u)
#define EDMA3_CCRL_ICR_I23_SHIFT         (0x00000017u)
#define EDMA3_CCRL_ICR_I23_RESETVAL      (0x00000000u)

/*----I23 Tokens----*/
#define EDMA3_CCRL_ICR_I23_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I22_MASK          (0x00400000u)
#define EDMA3_CCRL_ICR_I22_SHIFT         (0x00000016u)
#define EDMA3_CCRL_ICR_I22_RESETVAL      (0x00000000u)

/*----I22 Tokens----*/
#define EDMA3_CCRL_ICR_I22_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I21_MASK          (0x00200000u)
#define EDMA3_CCRL_ICR_I21_SHIFT         (0x00000015u)
#define EDMA3_CCRL_ICR_I21_RESETVAL      (0x00000000u)

/*----I21 Tokens----*/
#define EDMA3_CCRL_ICR_I21_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I20_MASK          (0x00100000u)
#define EDMA3_CCRL_ICR_I20_SHIFT         (0x00000014u)
#define EDMA3_CCRL_ICR_I20_RESETVAL      (0x00000000u)

/*----I20 Tokens----*/
#define EDMA3_CCRL_ICR_I20_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I19_MASK          (0x00080000u)
#define EDMA3_CCRL_ICR_I19_SHIFT         (0x00000013u)
#define EDMA3_CCRL_ICR_I19_RESETVAL      (0x00000000u)

/*----I19 Tokens----*/
#define EDMA3_CCRL_ICR_I19_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I18_MASK          (0x00040000u)
#define EDMA3_CCRL_ICR_I18_SHIFT         (0x00000012u)
#define EDMA3_CCRL_ICR_I18_RESETVAL      (0x00000000u)

/*----I18 Tokens----*/
#define EDMA3_CCRL_ICR_I18_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I17_MASK          (0x00020000u)
#define EDMA3_CCRL_ICR_I17_SHIFT         (0x00000011u)
#define EDMA3_CCRL_ICR_I17_RESETVAL      (0x00000000u)

/*----I17 Tokens----*/
#define EDMA3_CCRL_ICR_I17_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I16_MASK          (0x00010000u)
#define EDMA3_CCRL_ICR_I16_SHIFT         (0x00000010u)
#define EDMA3_CCRL_ICR_I16_RESETVAL      (0x00000000u)

/*----I16 Tokens----*/
#define EDMA3_CCRL_ICR_I16_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I15_MASK          (0x00008000u)
#define EDMA3_CCRL_ICR_I15_SHIFT         (0x0000000Fu)
#define EDMA3_CCRL_ICR_I15_RESETVAL      (0x00000000u)

/*----I15 Tokens----*/
#define EDMA3_CCRL_ICR_I15_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I14_MASK          (0x00004000u)
#define EDMA3_CCRL_ICR_I14_SHIFT         (0x0000000Eu)
#define EDMA3_CCRL_ICR_I14_RESETVAL      (0x00000000u)

/*----I14 Tokens----*/
#define EDMA3_CCRL_ICR_I14_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I13_MASK          (0x00002000u)
#define EDMA3_CCRL_ICR_I13_SHIFT         (0x0000000Du)
#define EDMA3_CCRL_ICR_I13_RESETVAL      (0x00000000u)

/*----I13 Tokens----*/
#define EDMA3_CCRL_ICR_I13_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I12_MASK          (0x00001000u)
#define EDMA3_CCRL_ICR_I12_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_ICR_I12_RESETVAL      (0x00000000u)

/*----I12 Tokens----*/
#define EDMA3_CCRL_ICR_I12_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I11_MASK          (0x00000800u)
#define EDMA3_CCRL_ICR_I11_SHIFT         (0x0000000Bu)
#define EDMA3_CCRL_ICR_I11_RESETVAL      (0x00000000u)

/*----I11 Tokens----*/
#define EDMA3_CCRL_ICR_I11_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I10_MASK          (0x00000400u)
#define EDMA3_CCRL_ICR_I10_SHIFT         (0x0000000Au)
#define EDMA3_CCRL_ICR_I10_RESETVAL      (0x00000000u)

/*----I10 Tokens----*/
#define EDMA3_CCRL_ICR_I10_CLEAR         (0x00000001u)

#define EDMA3_CCRL_ICR_I9_MASK           (0x00000200u)
#define EDMA3_CCRL_ICR_I9_SHIFT          (0x00000009u)
#define EDMA3_CCRL_ICR_I9_RESETVAL       (0x00000000u)

/*----I9 Tokens----*/
#define EDMA3_CCRL_ICR_I9_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I8_MASK           (0x00000100u)
#define EDMA3_CCRL_ICR_I8_SHIFT          (0x00000008u)
#define EDMA3_CCRL_ICR_I8_RESETVAL       (0x00000000u)

/*----I8 Tokens----*/
#define EDMA3_CCRL_ICR_I8_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I7_MASK           (0x00000080u)
#define EDMA3_CCRL_ICR_I7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_ICR_I7_RESETVAL       (0x00000000u)

/*----I7 Tokens----*/
#define EDMA3_CCRL_ICR_I7_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I6_MASK           (0x00000040u)
#define EDMA3_CCRL_ICR_I6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_ICR_I6_RESETVAL       (0x00000000u)

/*----I6 Tokens----*/
#define EDMA3_CCRL_ICR_I6_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I5_MASK           (0x00000020u)
#define EDMA3_CCRL_ICR_I5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_ICR_I5_RESETVAL       (0x00000000u)

/*----I5 Tokens----*/
#define EDMA3_CCRL_ICR_I5_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I4_MASK           (0x00000010u)
#define EDMA3_CCRL_ICR_I4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_ICR_I4_RESETVAL       (0x00000000u)

/*----I4 Tokens----*/
#define EDMA3_CCRL_ICR_I4_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I3_MASK           (0x00000008u)
#define EDMA3_CCRL_ICR_I3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_ICR_I3_RESETVAL       (0x00000000u)

/*----I3 Tokens----*/
#define EDMA3_CCRL_ICR_I3_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I2_MASK           (0x00000004u)
#define EDMA3_CCRL_ICR_I2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_ICR_I2_RESETVAL       (0x00000000u)

/*----I2 Tokens----*/
#define EDMA3_CCRL_ICR_I2_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I1_MASK           (0x00000002u)
#define EDMA3_CCRL_ICR_I1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_ICR_I1_RESETVAL       (0x00000000u)

/*----I1 Tokens----*/
#define EDMA3_CCRL_ICR_I1_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_I0_MASK           (0x00000001u)
#define EDMA3_CCRL_ICR_I0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_ICR_I0_RESETVAL       (0x00000000u)

/*----I0 Tokens----*/
#define EDMA3_CCRL_ICR_I0_CLEAR          (0x00000001u)

#define EDMA3_CCRL_ICR_RESETVAL          (0x00000000u)

/* ICRH */

#define EDMA3_CCRL_ICRH_I63_MASK         (0x80000000u)
#define EDMA3_CCRL_ICRH_I63_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_ICRH_I63_RESETVAL     (0x00000000u)

/*----I63 Tokens----*/
#define EDMA3_CCRL_ICRH_I63_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I62_MASK         (0x40000000u)
#define EDMA3_CCRL_ICRH_I62_SHIFT        (0x0000001Eu)
#define EDMA3_CCRL_ICRH_I62_RESETVAL     (0x00000000u)

/*----I62 Tokens----*/
#define EDMA3_CCRL_ICRH_I62_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I61_MASK         (0x20000000u)
#define EDMA3_CCRL_ICRH_I61_SHIFT        (0x0000001Du)
#define EDMA3_CCRL_ICRH_I61_RESETVAL     (0x00000000u)

/*----I61 Tokens----*/
#define EDMA3_CCRL_ICRH_I61_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I60_MASK         (0x10000000u)
#define EDMA3_CCRL_ICRH_I60_SHIFT        (0x0000001Cu)
#define EDMA3_CCRL_ICRH_I60_RESETVAL     (0x00000000u)

/*----I60 Tokens----*/
#define EDMA3_CCRL_ICRH_I60_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I59_MASK         (0x08000000u)
#define EDMA3_CCRL_ICRH_I59_SHIFT        (0x0000001Bu)
#define EDMA3_CCRL_ICRH_I59_RESETVAL     (0x00000000u)

/*----I59 Tokens----*/
#define EDMA3_CCRL_ICRH_I59_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I58_MASK         (0x04000000u)
#define EDMA3_CCRL_ICRH_I58_SHIFT        (0x0000001Au)
#define EDMA3_CCRL_ICRH_I58_RESETVAL     (0x00000000u)

/*----I58 Tokens----*/
#define EDMA3_CCRL_ICRH_I58_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I57_MASK         (0x02000000u)
#define EDMA3_CCRL_ICRH_I57_SHIFT        (0x00000019u)
#define EDMA3_CCRL_ICRH_I57_RESETVAL     (0x00000000u)

/*----I57 Tokens----*/
#define EDMA3_CCRL_ICRH_I57_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I56_MASK         (0x01000000u)
#define EDMA3_CCRL_ICRH_I56_SHIFT        (0x00000018u)
#define EDMA3_CCRL_ICRH_I56_RESETVAL     (0x00000000u)

/*----I56 Tokens----*/
#define EDMA3_CCRL_ICRH_I56_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I55_MASK         (0x00800000u)
#define EDMA3_CCRL_ICRH_I55_SHIFT        (0x00000017u)
#define EDMA3_CCRL_ICRH_I55_RESETVAL     (0x00000000u)

/*----I55 Tokens----*/
#define EDMA3_CCRL_ICRH_I55_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I54_MASK         (0x00400000u)
#define EDMA3_CCRL_ICRH_I54_SHIFT        (0x00000016u)
#define EDMA3_CCRL_ICRH_I54_RESETVAL     (0x00000000u)

/*----I54 Tokens----*/
#define EDMA3_CCRL_ICRH_I54_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I53_MASK         (0x00200000u)
#define EDMA3_CCRL_ICRH_I53_SHIFT        (0x00000015u)
#define EDMA3_CCRL_ICRH_I53_RESETVAL     (0x00000000u)

/*----I53 Tokens----*/
#define EDMA3_CCRL_ICRH_I53_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I52_MASK         (0x00100000u)
#define EDMA3_CCRL_ICRH_I52_SHIFT        (0x00000014u)
#define EDMA3_CCRL_ICRH_I52_RESETVAL     (0x00000000u)

/*----I52 Tokens----*/
#define EDMA3_CCRL_ICRH_I52_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I51_MASK         (0x00080000u)
#define EDMA3_CCRL_ICRH_I51_SHIFT        (0x00000013u)
#define EDMA3_CCRL_ICRH_I51_RESETVAL     (0x00000000u)

/*----I51 Tokens----*/
#define EDMA3_CCRL_ICRH_I51_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I50_MASK         (0x00040000u)
#define EDMA3_CCRL_ICRH_I50_SHIFT        (0x00000012u)
#define EDMA3_CCRL_ICRH_I50_RESETVAL     (0x00000000u)

/*----I50 Tokens----*/
#define EDMA3_CCRL_ICRH_I50_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I49_MASK         (0x00020000u)
#define EDMA3_CCRL_ICRH_I49_SHIFT        (0x00000011u)
#define EDMA3_CCRL_ICRH_I49_RESETVAL     (0x00000000u)

/*----I49 Tokens----*/
#define EDMA3_CCRL_ICRH_I49_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I48_MASK         (0x00010000u)
#define EDMA3_CCRL_ICRH_I48_SHIFT        (0x00000010u)
#define EDMA3_CCRL_ICRH_I48_RESETVAL     (0x00000000u)

/*----I48 Tokens----*/
#define EDMA3_CCRL_ICRH_I48_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I47_MASK         (0x00008000u)
#define EDMA3_CCRL_ICRH_I47_SHIFT        (0x0000000Fu)
#define EDMA3_CCRL_ICRH_I47_RESETVAL     (0x00000000u)

/*----I47 Tokens----*/
#define EDMA3_CCRL_ICRH_I47_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I46_MASK         (0x00004000u)
#define EDMA3_CCRL_ICRH_I46_SHIFT        (0x0000000Eu)
#define EDMA3_CCRL_ICRH_I46_RESETVAL     (0x00000000u)

/*----I46 Tokens----*/
#define EDMA3_CCRL_ICRH_I46_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I45_MASK         (0x00002000u)
#define EDMA3_CCRL_ICRH_I45_SHIFT        (0x0000000Du)
#define EDMA3_CCRL_ICRH_I45_RESETVAL     (0x00000000u)

/*----I45 Tokens----*/
#define EDMA3_CCRL_ICRH_I45_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I44_MASK         (0x00001000u)
#define EDMA3_CCRL_ICRH_I44_SHIFT        (0x0000000Cu)
#define EDMA3_CCRL_ICRH_I44_RESETVAL     (0x00000000u)

/*----I44 Tokens----*/
#define EDMA3_CCRL_ICRH_I44_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I43_MASK         (0x00000800u)
#define EDMA3_CCRL_ICRH_I43_SHIFT        (0x0000000Bu)
#define EDMA3_CCRL_ICRH_I43_RESETVAL     (0x00000000u)

/*----I43 Tokens----*/
#define EDMA3_CCRL_ICRH_I43_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I42_MASK         (0x00000400u)
#define EDMA3_CCRL_ICRH_I42_SHIFT        (0x0000000Au)
#define EDMA3_CCRL_ICRH_I42_RESETVAL     (0x00000000u)

/*----I42 Tokens----*/
#define EDMA3_CCRL_ICRH_I42_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I41_MASK         (0x00000200u)
#define EDMA3_CCRL_ICRH_I41_SHIFT        (0x00000009u)
#define EDMA3_CCRL_ICRH_I41_RESETVAL     (0x00000000u)

/*----I41 Tokens----*/
#define EDMA3_CCRL_ICRH_I41_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I40_MASK         (0x00000100u)
#define EDMA3_CCRL_ICRH_I40_SHIFT        (0x00000008u)
#define EDMA3_CCRL_ICRH_I40_RESETVAL     (0x00000000u)

/*----I40 Tokens----*/
#define EDMA3_CCRL_ICRH_I40_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I39_MASK         (0x00000080u)
#define EDMA3_CCRL_ICRH_I39_SHIFT        (0x00000007u)
#define EDMA3_CCRL_ICRH_I39_RESETVAL     (0x00000000u)

/*----I39 Tokens----*/
#define EDMA3_CCRL_ICRH_I39_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I38_MASK         (0x00000040u)
#define EDMA3_CCRL_ICRH_I38_SHIFT        (0x00000006u)
#define EDMA3_CCRL_ICRH_I38_RESETVAL     (0x00000000u)

/*----I38 Tokens----*/
#define EDMA3_CCRL_ICRH_I38_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I37_MASK         (0x00000020u)
#define EDMA3_CCRL_ICRH_I37_SHIFT        (0x00000005u)
#define EDMA3_CCRL_ICRH_I37_RESETVAL     (0x00000000u)

/*----I37 Tokens----*/
#define EDMA3_CCRL_ICRH_I37_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I36_MASK         (0x00000010u)
#define EDMA3_CCRL_ICRH_I36_SHIFT        (0x00000004u)
#define EDMA3_CCRL_ICRH_I36_RESETVAL     (0x00000000u)

/*----I36 Tokens----*/
#define EDMA3_CCRL_ICRH_I36_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I35_MASK         (0x00000008u)
#define EDMA3_CCRL_ICRH_I35_SHIFT        (0x00000003u)
#define EDMA3_CCRL_ICRH_I35_RESETVAL     (0x00000000u)

/*----I35 Tokens----*/
#define EDMA3_CCRL_ICRH_I35_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I34_MASK         (0x00000004u)
#define EDMA3_CCRL_ICRH_I34_SHIFT        (0x00000002u)
#define EDMA3_CCRL_ICRH_I34_RESETVAL     (0x00000000u)

/*----I34 Tokens----*/
#define EDMA3_CCRL_ICRH_I34_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I33_MASK         (0x00000002u)
#define EDMA3_CCRL_ICRH_I33_SHIFT        (0x00000001u)
#define EDMA3_CCRL_ICRH_I33_RESETVAL     (0x00000000u)

/*----I33 Tokens----*/
#define EDMA3_CCRL_ICRH_I33_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_I32_MASK         (0x00000001u)
#define EDMA3_CCRL_ICRH_I32_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ICRH_I32_RESETVAL     (0x00000000u)

/*----I32 Tokens----*/
#define EDMA3_CCRL_ICRH_I32_CLEAR        (0x00000001u)

#define EDMA3_CCRL_ICRH_RESETVAL         (0x00000000u)

/* IEVAL */

#define EDMA3_CCRL_IEVAL_SET_MASK        (0x00000002u)
#define EDMA3_CCRL_IEVAL_SET_SHIFT       (0x00000001u)
#define EDMA3_CCRL_IEVAL_SET_RESETVAL    (0x00000000u)

/*----SET Tokens----*/
#define EDMA3_CCRL_IEVAL_SET_SET         (0x00000001u)

#define EDMA3_CCRL_IEVAL_EVAL_MASK       (0x00000001u)
#define EDMA3_CCRL_IEVAL_EVAL_SHIFT      (0x00000000u)
#define EDMA3_CCRL_IEVAL_EVAL_RESETVAL   (0x00000000u)

/*----EVAL Tokens----*/
#define EDMA3_CCRL_IEVAL_EVAL_EVAL       (0x00000001u)

#define EDMA3_CCRL_IEVAL_RESETVAL        (0x00000000u)

/* QER */

#define EDMA3_CCRL_QER_E7_MASK           (0x00000080u)
#define EDMA3_CCRL_QER_E7_SHIFT          (0x00000007u)
#define EDMA3_CCRL_QER_E7_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E6_MASK           (0x00000040u)
#define EDMA3_CCRL_QER_E6_SHIFT          (0x00000006u)
#define EDMA3_CCRL_QER_E6_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E5_MASK           (0x00000020u)
#define EDMA3_CCRL_QER_E5_SHIFT          (0x00000005u)
#define EDMA3_CCRL_QER_E5_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E4_MASK           (0x00000010u)
#define EDMA3_CCRL_QER_E4_SHIFT          (0x00000004u)
#define EDMA3_CCRL_QER_E4_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E3_MASK           (0x00000008u)
#define EDMA3_CCRL_QER_E3_SHIFT          (0x00000003u)
#define EDMA3_CCRL_QER_E3_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E2_MASK           (0x00000004u)
#define EDMA3_CCRL_QER_E2_SHIFT          (0x00000002u)
#define EDMA3_CCRL_QER_E2_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E1_MASK           (0x00000002u)
#define EDMA3_CCRL_QER_E1_SHIFT          (0x00000001u)
#define EDMA3_CCRL_QER_E1_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_E0_MASK           (0x00000001u)
#define EDMA3_CCRL_QER_E0_SHIFT          (0x00000000u)
#define EDMA3_CCRL_QER_E0_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_QER_RESETVAL          (0x00000000u)

/* QEER */

#define EDMA3_CCRL_QEER_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_QEER_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QEER_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_QEER_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_QEER_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_QEER_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_QEER_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_QEER_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_QEER_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_QEER_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_QEER_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_QEER_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_QEER_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_QEER_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_QEER_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_QEER_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_QEER_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QEER_RESETVAL         (0x00000000u)

/* QEECR */

#define EDMA3_CCRL_QEECR_E7_MASK         (0x00000080u)
#define EDMA3_CCRL_QEECR_E7_SHIFT        (0x00000007u)
#define EDMA3_CCRL_QEECR_E7_RESETVAL     (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_QEECR_E7_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E6_MASK         (0x00000040u)
#define EDMA3_CCRL_QEECR_E6_SHIFT        (0x00000006u)
#define EDMA3_CCRL_QEECR_E6_RESETVAL     (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_QEECR_E6_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E5_MASK         (0x00000020u)
#define EDMA3_CCRL_QEECR_E5_SHIFT        (0x00000005u)
#define EDMA3_CCRL_QEECR_E5_RESETVAL     (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_QEECR_E5_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E4_MASK         (0x00000010u)
#define EDMA3_CCRL_QEECR_E4_SHIFT        (0x00000004u)
#define EDMA3_CCRL_QEECR_E4_RESETVAL     (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_QEECR_E4_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E3_MASK         (0x00000008u)
#define EDMA3_CCRL_QEECR_E3_SHIFT        (0x00000003u)
#define EDMA3_CCRL_QEECR_E3_RESETVAL     (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_QEECR_E3_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E2_MASK         (0x00000004u)
#define EDMA3_CCRL_QEECR_E2_SHIFT        (0x00000002u)
#define EDMA3_CCRL_QEECR_E2_RESETVAL     (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_QEECR_E2_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E1_MASK         (0x00000002u)
#define EDMA3_CCRL_QEECR_E1_SHIFT        (0x00000001u)
#define EDMA3_CCRL_QEECR_E1_RESETVAL     (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_QEECR_E1_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_E0_MASK         (0x00000001u)
#define EDMA3_CCRL_QEECR_E0_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QEECR_E0_RESETVAL     (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_QEECR_E0_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QEECR_RESETVAL        (0x00000000u)

/* QEESR */

#define EDMA3_CCRL_QEESR_E7_MASK         (0x00000080u)
#define EDMA3_CCRL_QEESR_E7_SHIFT        (0x00000007u)
#define EDMA3_CCRL_QEESR_E7_RESETVAL     (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_QEESR_E7_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E6_MASK         (0x00000040u)
#define EDMA3_CCRL_QEESR_E6_SHIFT        (0x00000006u)
#define EDMA3_CCRL_QEESR_E6_RESETVAL     (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_QEESR_E6_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E5_MASK         (0x00000020u)
#define EDMA3_CCRL_QEESR_E5_SHIFT        (0x00000005u)
#define EDMA3_CCRL_QEESR_E5_RESETVAL     (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_QEESR_E5_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E4_MASK         (0x00000010u)
#define EDMA3_CCRL_QEESR_E4_SHIFT        (0x00000004u)
#define EDMA3_CCRL_QEESR_E4_RESETVAL     (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_QEESR_E4_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E3_MASK         (0x00000008u)
#define EDMA3_CCRL_QEESR_E3_SHIFT        (0x00000003u)
#define EDMA3_CCRL_QEESR_E3_RESETVAL     (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_QEESR_E3_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E2_MASK         (0x00000004u)
#define EDMA3_CCRL_QEESR_E2_SHIFT        (0x00000002u)
#define EDMA3_CCRL_QEESR_E2_RESETVAL     (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_QEESR_E2_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E1_MASK         (0x00000002u)
#define EDMA3_CCRL_QEESR_E1_SHIFT        (0x00000001u)
#define EDMA3_CCRL_QEESR_E1_RESETVAL     (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_QEESR_E1_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_E0_MASK         (0x00000001u)
#define EDMA3_CCRL_QEESR_E0_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QEESR_E0_RESETVAL     (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_QEESR_E0_SET          (0x00000001u)

#define EDMA3_CCRL_QEESR_RESETVAL        (0x00000000u)

/* QSER */

#define EDMA3_CCRL_QSER_E7_MASK          (0x00000080u)
#define EDMA3_CCRL_QSER_E7_SHIFT         (0x00000007u)
#define EDMA3_CCRL_QSER_E7_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E6_MASK          (0x00000040u)
#define EDMA3_CCRL_QSER_E6_SHIFT         (0x00000006u)
#define EDMA3_CCRL_QSER_E6_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E5_MASK          (0x00000020u)
#define EDMA3_CCRL_QSER_E5_SHIFT         (0x00000005u)
#define EDMA3_CCRL_QSER_E5_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E4_MASK          (0x00000010u)
#define EDMA3_CCRL_QSER_E4_SHIFT         (0x00000004u)
#define EDMA3_CCRL_QSER_E4_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E3_MASK          (0x00000008u)
#define EDMA3_CCRL_QSER_E3_SHIFT         (0x00000003u)
#define EDMA3_CCRL_QSER_E3_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E2_MASK          (0x00000004u)
#define EDMA3_CCRL_QSER_E2_SHIFT         (0x00000002u)
#define EDMA3_CCRL_QSER_E2_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E1_MASK          (0x00000002u)
#define EDMA3_CCRL_QSER_E1_SHIFT         (0x00000001u)
#define EDMA3_CCRL_QSER_E1_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_E0_MASK          (0x00000001u)
#define EDMA3_CCRL_QSER_E0_SHIFT         (0x00000000u)
#define EDMA3_CCRL_QSER_E0_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QSER_RESETVAL         (0x00000000u)

/* QSECR */

#define EDMA3_CCRL_QSECR_E7_MASK         (0x00000080u)
#define EDMA3_CCRL_QSECR_E7_SHIFT        (0x00000007u)
#define EDMA3_CCRL_QSECR_E7_RESETVAL     (0x00000000u)

/*----E7 Tokens----*/
#define EDMA3_CCRL_QSECR_E7_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E6_MASK         (0x00000040u)
#define EDMA3_CCRL_QSECR_E6_SHIFT        (0x00000006u)
#define EDMA3_CCRL_QSECR_E6_RESETVAL     (0x00000000u)

/*----E6 Tokens----*/
#define EDMA3_CCRL_QSECR_E6_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E5_MASK         (0x00000020u)
#define EDMA3_CCRL_QSECR_E5_SHIFT        (0x00000005u)
#define EDMA3_CCRL_QSECR_E5_RESETVAL     (0x00000000u)

/*----E5 Tokens----*/
#define EDMA3_CCRL_QSECR_E5_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E4_MASK         (0x00000010u)
#define EDMA3_CCRL_QSECR_E4_SHIFT        (0x00000004u)
#define EDMA3_CCRL_QSECR_E4_RESETVAL     (0x00000000u)

/*----E4 Tokens----*/
#define EDMA3_CCRL_QSECR_E4_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E3_MASK         (0x00000008u)
#define EDMA3_CCRL_QSECR_E3_SHIFT        (0x00000003u)
#define EDMA3_CCRL_QSECR_E3_RESETVAL     (0x00000000u)

/*----E3 Tokens----*/
#define EDMA3_CCRL_QSECR_E3_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E2_MASK         (0x00000004u)
#define EDMA3_CCRL_QSECR_E2_SHIFT        (0x00000002u)
#define EDMA3_CCRL_QSECR_E2_RESETVAL     (0x00000000u)

/*----E2 Tokens----*/
#define EDMA3_CCRL_QSECR_E2_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E1_MASK         (0x00000002u)
#define EDMA3_CCRL_QSECR_E1_SHIFT        (0x00000001u)
#define EDMA3_CCRL_QSECR_E1_RESETVAL     (0x00000000u)

/*----E1 Tokens----*/
#define EDMA3_CCRL_QSECR_E1_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_E0_MASK         (0x00000001u)
#define EDMA3_CCRL_QSECR_E0_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QSECR_E0_RESETVAL     (0x00000000u)

/*----E0 Tokens----*/
#define EDMA3_CCRL_QSECR_E0_CLEAR        (0x00000001u)

#define EDMA3_CCRL_QSECR_RESETVAL        (0x00000000u)

/* OPT */

#define EDMA3_CCRL_OPT_PRIV_MASK         (0x80000000u)
#define EDMA3_CCRL_OPT_PRIV_SHIFT        (0x0000001Fu)
#define EDMA3_CCRL_OPT_PRIV_RESETVAL     (0x00000000u)

/*----PRIV Tokens----*/
#define EDMA3_CCRL_OPT_PRIV_USER         (0x00000000u)
#define EDMA3_CCRL_OPT_PRIV_SUPERVISOR   (0x00000001u)

#define EDMA3_CCRL_OPT_SECURE_MASK       (0x40000000u)
#define EDMA3_CCRL_OPT_SECURE_SHIFT      (0x0000001Eu)
#define EDMA3_CCRL_OPT_SECURE_RESETVAL   (0x00000000u)

/*----SECURE Tokens----*/
#define EDMA3_CCRL_OPT_SECURE_SECURE     (0x00000000u)
#define EDMA3_CCRL_OPT_SECURE_NONSECURE  (0x00000001u)

#define EDMA3_CCRL_OPT_PRIVID_MASK       (0x0F000000u)
#define EDMA3_CCRL_OPT_PRIVID_SHIFT      (0x00000018u)
#define EDMA3_CCRL_OPT_PRIVID_RESETVAL   (0x00000000u)

#define EDMA3_CCRL_OPT_ITCCHEN_MASK      (0x00800000u)
#define EDMA3_CCRL_OPT_ITCCHEN_SHIFT     (0x00000017u)
#define EDMA3_CCRL_OPT_ITCCHEN_RESETVAL  (0x00000000u)

/*----ITCCHEN Tokens----*/
#define EDMA3_CCRL_OPT_ITCCHEN_DISABLE   (0x00000000u)
#define EDMA3_CCRL_OPT_ITCCHEN_ENABLE    (0x00000001u)

#define EDMA3_CCRL_OPT_TCCHEN_MASK       (0x00400000u)
#define EDMA3_CCRL_OPT_TCCHEN_SHIFT      (0x00000016u)
#define EDMA3_CCRL_OPT_TCCHEN_RESETVAL   (0x00000000u)

/*----TCCHEN Tokens----*/
#define EDMA3_CCRL_OPT_TCCHEN_DISABLE    (0x00000000u)
#define EDMA3_CCRL_OPT_TCCHEN_ENABLE     (0x00000001u)

#define EDMA3_CCRL_OPT_ITCINTEN_MASK     (0x00200000u)
#define EDMA3_CCRL_OPT_ITCINTEN_SHIFT    (0x00000015u)
#define EDMA3_CCRL_OPT_ITCINTEN_RESETVAL (0x00000000u)

/*----ITCINTEN Tokens----*/
#define EDMA3_CCRL_OPT_ITCINTEN_DISABLE  (0x00000000u)
#define EDMA3_CCRL_OPT_ITCINTEN_ENABLE   (0x00000001u)

#define EDMA3_CCRL_OPT_TCINTEN_MASK      (0x00100000u)
#define EDMA3_CCRL_OPT_TCINTEN_SHIFT     (0x00000014u)
#define EDMA3_CCRL_OPT_TCINTEN_RESETVAL  (0x00000000u)

/*----TCINTEN Tokens----*/
#define EDMA3_CCRL_OPT_TCINTEN_DISABLE   (0x00000000u)
#define EDMA3_CCRL_OPT_TCINTEN_ENABLE    (0x00000001u)

#define EDMA3_CCRL_OPT_WIMODE_MASK       (0x00080000u)
#define EDMA3_CCRL_OPT_WIMODE_SHIFT      (0x00000013u)
#define EDMA3_CCRL_OPT_WIMODE_RESETVAL   (0x00000000u)

/*----WIMODE Tokens----*/
#define EDMA3_CCRL_OPT_WIMODE_NORMAL     (0x00000000u)
#define EDMA3_CCRL_OPT_WIMODE_WI         (0x00000001u)

#define EDMA3_CCRL_OPT_TCC_MASK          (0x0003F000u)
#define EDMA3_CCRL_OPT_TCC_SHIFT         (0x0000000Cu)
#define EDMA3_CCRL_OPT_TCC_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_OPT_TCCMODE_MASK      (0x00000800u)
#define EDMA3_CCRL_OPT_TCCMODE_SHIFT     (0x0000000Bu)
#define EDMA3_CCRL_OPT_TCCMODE_RESETVAL  (0x00000000u)

/*----TCCMODE Tokens----*/
#define EDMA3_CCRL_OPT_TCCMODE_NORMAL    (0x00000000u)
#define EDMA3_CCRL_OPT_TCCMODE_EARLY     (0x00000001u)

#define EDMA3_CCRL_OPT_FWID_MASK         (0x00000700u)
#define EDMA3_CCRL_OPT_FWID_SHIFT        (0x00000008u)
#define EDMA3_CCRL_OPT_FWID_RESETVAL     (0x00000000u)

/*----FWID Tokens----*/
#define EDMA3_CCRL_OPT_FWID_8            (0x00000000u)
#define EDMA3_CCRL_OPT_FWID_16           (0x00000001u)
#define EDMA3_CCRL_OPT_FWID_32           (0x00000002u)
#define EDMA3_CCRL_OPT_FWID_64           (0x00000003u)
#define EDMA3_CCRL_OPT_FWID_128          (0x00000004u)
#define EDMA3_CCRL_OPT_FWID_256          (0x00000005u)

#define EDMA3_CCRL_OPT_STATIC_MASK       (0x00000008u)
#define EDMA3_CCRL_OPT_STATIC_SHIFT      (0x00000003u)
#define EDMA3_CCRL_OPT_STATIC_RESETVAL   (0x00000000u)

/*----STATIC Tokens----*/
#define EDMA3_CCRL_OPT_STATIC_NORMAL     (0x00000000u)
#define EDMA3_CCRL_OPT_STATIC_STATIC     (0x00000001u)

#define EDMA3_CCRL_OPT_SYNCDIM_MASK      (0x00000004u)
#define EDMA3_CCRL_OPT_SYNCDIM_SHIFT     (0x00000002u)
#define EDMA3_CCRL_OPT_SYNCDIM_RESETVAL  (0x00000000u)

/*----SYNCDIM Tokens----*/
#define EDMA3_CCRL_OPT_SYNCDIM_ASYNC     (0x00000000u)
#define EDMA3_CCRL_OPT_SYNCDIM_ABSYNC    (0x00000001u)

#define EDMA3_CCRL_OPT_DAM_MASK          (0x00000002u)
#define EDMA3_CCRL_OPT_DAM_SHIFT         (0x00000001u)
#define EDMA3_CCRL_OPT_DAM_RESETVAL      (0x00000000u)

/*----DAM Tokens----*/
#define EDMA3_CCRL_OPT_DAM_INCR          (0x00000000u)
#define EDMA3_CCRL_OPT_DAM_FIFO          (0x00000001u)

#define EDMA3_CCRL_OPT_SAM_MASK          (0x00000001u)
#define EDMA3_CCRL_OPT_SAM_SHIFT         (0x00000000u)
#define EDMA3_CCRL_OPT_SAM_RESETVAL      (0x00000000u)

/*----SAM Tokens----*/
#define EDMA3_CCRL_OPT_SAM_INCR          (0x00000000u)
#define EDMA3_CCRL_OPT_SAM_FIFO          (0x00000001u)

#define EDMA3_CCRL_OPT_RESETVAL          (0x00000000u)

/* SRC */

#define EDMA3_CCRL_SRC_SRC_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_SRC_SRC_SHIFT         (0x00000000u)
#define EDMA3_CCRL_SRC_SRC_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SRC_RESETVAL          (0x00000000u)

/* A_B_CNT */

#define EDMA3_CCRL_A_B_CNT_BCNT_MASK     (0xFFFF0000u)
#define EDMA3_CCRL_A_B_CNT_BCNT_SHIFT    (0x00000010u)
#define EDMA3_CCRL_A_B_CNT_BCNT_RESETVAL (0x00000000u)

#define EDMA3_CCRL_A_B_CNT_ACNT_MASK     (0x0000FFFFu)
#define EDMA3_CCRL_A_B_CNT_ACNT_SHIFT    (0x00000000u)
#define EDMA3_CCRL_A_B_CNT_ACNT_RESETVAL (0x00000000u)

#define EDMA3_CCRL_A_B_CNT_RESETVAL      (0x00000000u)

/* DST */

#define EDMA3_CCRL_DST_DST_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_DST_DST_SHIFT         (0x00000000u)
#define EDMA3_CCRL_DST_DST_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_DST_RESETVAL          (0x00000000u)

/* SRC_DST_BIDX */

#define EDMA3_CCRL_SRC_DST_BIDX_DSTBIDX_MASK (0xFFFF0000u)
#define EDMA3_CCRL_SRC_DST_BIDX_DSTBIDX_SHIFT (0x00000010u)
#define EDMA3_CCRL_SRC_DST_BIDX_DSTBIDX_RESETVAL (0x00000000u)

#define EDMA3_CCRL_SRC_DST_BIDX_SRCBIDX_MASK (0x0000FFFFu)
#define EDMA3_CCRL_SRC_DST_BIDX_SRCBIDX_SHIFT (0x00000000u)
#define EDMA3_CCRL_SRC_DST_BIDX_SRCBIDX_RESETVAL (0x00000000u)

#define EDMA3_CCRL_SRC_DST_BIDX_RESETVAL (0x00000000u)

/* LINK_BCNTRLD */

#define EDMA3_CCRL_LINK_BCNTRLD_BCNTRLD_MASK (0xFFFF0000u)
#define EDMA3_CCRL_LINK_BCNTRLD_BCNTRLD_SHIFT (0x00000010u)
#define EDMA3_CCRL_LINK_BCNTRLD_BCNTRLD_RESETVAL (0x00000000u)

#define EDMA3_CCRL_LINK_BCNTRLD_LINK_MASK (0x0000FFFFu)
#define EDMA3_CCRL_LINK_BCNTRLD_LINK_SHIFT (0x00000000u)
#define EDMA3_CCRL_LINK_BCNTRLD_LINK_RESETVAL (0x00000000u)

#define EDMA3_CCRL_LINK_BCNTRLD_RESETVAL (0x00000000u)

/* SRC_DST_CIDX */

#define EDMA3_CCRL_SRC_DST_CIDX_DSTCIDX_MASK (0xFFFF0000u)
#define EDMA3_CCRL_SRC_DST_CIDX_DSTCIDX_SHIFT (0x00000010u)
#define EDMA3_CCRL_SRC_DST_CIDX_DSTCIDX_RESETVAL (0x00000000u)

#define EDMA3_CCRL_SRC_DST_CIDX_SRCCIDX_MASK (0x0000FFFFu)
#define EDMA3_CCRL_SRC_DST_CIDX_SRCCIDX_SHIFT (0x00000000u)
#define EDMA3_CCRL_SRC_DST_CIDX_SRCCIDX_RESETVAL (0x00000000u)

#define EDMA3_CCRL_SRC_DST_CIDX_RESETVAL (0x00000000u)

/* CCNT */

#define EDMA3_CCRL_CCNT_CCNT_MASK        (0x0000FFFFu)
#define EDMA3_CCRL_CCNT_CCNT_SHIFT       (0x00000000u)
#define EDMA3_CCRL_CCNT_CCNT_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_CCNT_RESETVAL         (0x00000000u)

/* ER */

#define EDMA3_CCRL_ER_REG_MASK           (0xFFFFFFFFu)
#define EDMA3_CCRL_ER_REG_SHIFT          (0x00000000u)
#define EDMA3_CCRL_ER_REG_RESETVAL       (0x00000000u)

#define EDMA3_CCRL_ER_RESETVAL           (0x00000000u)

/* ERH */

#define EDMA3_CCRL_ERH_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_ERH_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_ERH_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ERH_RESETVAL          (0x00000000u)

/* ECR */

#define EDMA3_CCRL_ECR_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_ECR_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_ECR_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ECR_RESETVAL          (0x00000000u)

/* ECRH */

#define EDMA3_CCRL_ECRH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_ECRH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ECRH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_ECRH_RESETVAL         (0x00000000u)

/* ESR */

#define EDMA3_CCRL_ESR_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_ESR_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_ESR_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ESR_RESETVAL          (0x00000000u)

/* ESRH */

#define EDMA3_CCRL_ESRH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_ESRH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ESRH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_ESRH_RESETVAL         (0x00000000u)

/* CER */

#define EDMA3_CCRL_CER_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_CER_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_CER_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_CER_RESETVAL          (0x00000000u)

/* CERH */

#define EDMA3_CCRL_CERH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_CERH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_CERH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_CERH_RESETVAL         (0x00000000u)

/* EER */

#define EDMA3_CCRL_EER_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_EER_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_EER_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_EER_RESETVAL          (0x00000000u)

/* EERH */

#define EDMA3_CCRL_EERH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_EERH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_EERH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EERH_RESETVAL         (0x00000000u)

/* EECR */

#define EDMA3_CCRL_EECR_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_EECR_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_EECR_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EECR_RESETVAL         (0x00000000u)

/* EECRH */

#define EDMA3_CCRL_EECRH_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_EECRH_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_EECRH_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EECRH_RESETVAL        (0x00000000u)

/* EESR */

#define EDMA3_CCRL_EESR_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_EESR_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_EESR_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_EESR_RESETVAL         (0x00000000u)

/* EESRH */

#define EDMA3_CCRL_EESRH_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_EESRH_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_EESRH_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_EESRH_RESETVAL        (0x00000000u)

/* SER */

#define EDMA3_CCRL_SER_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_SER_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_SER_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_SER_RESETVAL          (0x00000000u)

/* SERH */

#define EDMA3_CCRL_SERH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_SERH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_SERH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SERH_RESETVAL         (0x00000000u)

/* SECR */

#define EDMA3_CCRL_SECR_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_SECR_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_SECR_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_SECR_RESETVAL         (0x00000000u)

/* SECRH */

#define EDMA3_CCRL_SECRH_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_SECRH_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_SECRH_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_SECRH_RESETVAL        (0x00000000u)

/* IER */

#define EDMA3_CCRL_IER_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_IER_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_IER_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IER_RESETVAL          (0x00000000u)

/* IERH */

#define EDMA3_CCRL_IERH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_IERH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IERH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IERH_RESETVAL         (0x00000000u)

/* IECR */

#define EDMA3_CCRL_IECR_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_IECR_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IECR_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IECR_RESETVAL         (0x00000000u)

/* IECRH */

#define EDMA3_CCRL_IECRH_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_IECRH_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_IECRH_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_IECRH_RESETVAL        (0x00000000u)

/* IESR */

#define EDMA3_CCRL_IESR_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_IESR_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IESR_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IESR_RESETVAL         (0x00000000u)

/* IESRH */

#define EDMA3_CCRL_IESRH_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_IESRH_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_IESRH_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_IESRH_RESETVAL        (0x00000000u)

/* IPR */

#define EDMA3_CCRL_IPR_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_IPR_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_IPR_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_IPR_RESETVAL          (0x00000000u)

/* IPRH */

#define EDMA3_CCRL_IPRH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_IPRH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_IPRH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_IPRH_RESETVAL         (0x00000000u)

/* ICR */

#define EDMA3_CCRL_ICR_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_ICR_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_ICR_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_ICR_RESETVAL          (0x00000000u)

/* ICRH */

#define EDMA3_CCRL_ICRH_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_ICRH_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_ICRH_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_ICRH_RESETVAL         (0x00000000u)

/* IEVAL */

#define EDMA3_CCRL_IEVAL_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_IEVAL_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_IEVAL_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_IEVAL_RESETVAL        (0x00000000u)

/* QER */

#define EDMA3_CCRL_QER_REG_MASK          (0xFFFFFFFFu)
#define EDMA3_CCRL_QER_REG_SHIFT         (0x00000000u)
#define EDMA3_CCRL_QER_REG_RESETVAL      (0x00000000u)

#define EDMA3_CCRL_QER_RESETVAL          (0x00000000u)

/* QEER */

#define EDMA3_CCRL_QEER_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_QEER_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QEER_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QEER_RESETVAL         (0x00000000u)

/* QEECR */

#define EDMA3_CCRL_QEECR_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_QEECR_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_QEECR_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_QEECR_RESETVAL        (0x00000000u)

/* QEESR */

#define EDMA3_CCRL_QEESR_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_QEESR_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_QEESR_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_QEESR_RESETVAL        (0x00000000u)

/* QSER */

#define EDMA3_CCRL_QSER_REG_MASK         (0xFFFFFFFFu)
#define EDMA3_CCRL_QSER_REG_SHIFT        (0x00000000u)
#define EDMA3_CCRL_QSER_REG_RESETVAL     (0x00000000u)

#define EDMA3_CCRL_QSER_RESETVAL         (0x00000000u)

/* QSECR */

#define EDMA3_CCRL_QSECR_REG_MASK        (0xFFFFFFFFu)
#define EDMA3_CCRL_QSECR_REG_SHIFT       (0x00000000u)
#define EDMA3_CCRL_QSECR_REG_RESETVAL    (0x00000000u)

#define EDMA3_CCRL_QSECR_RESETVAL        (0x00000000u)

/******************************************************************************/

/* Other Mask defines */
/** DCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_DCH_PARAM_CLR_MASK                (~EDMA3_CCRL_DCHMAP_PAENTRY_MASK)
/** DCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_DCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_DCHMAP_PAENTRY_MASK >> EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT) & (paRAMId)) << EDMA3_CCRL_DCHMAP_PAENTRY_SHIFT)
/** QCHMAP-PaRAMEntry bitfield Clear */
#define EDMA3_RM_QCH_PARAM_CLR_MASK                (~EDMA3_CCRL_QCHMAP_PAENTRY_MASK)
/** QCHMAP-PaRAMEntry bitfield Set */
#define EDMA3_RM_QCH_PARAM_SET_MASK(paRAMId)       (((EDMA3_CCRL_QCHMAP_PAENTRY_MASK >> EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT) & (paRAMId)) << EDMA3_CCRL_QCHMAP_PAENTRY_SHIFT)
/** QCHMAP-TrigWord bitfield Clear */
#define EDMA3_RM_QCH_TRWORD_CLR_MASK               (~EDMA3_CCRL_QCHMAP_TRWORD_MASK)
/** QCHMAP-TrigWord bitfield Set */
#define EDMA3_RM_QCH_TRWORD_SET_MASK(paRAMId)      (((EDMA3_CCRL_QCHMAP_TRWORD_MASK >> EDMA3_CCRL_QCHMAP_TRWORD_SHIFT) & (paRAMId)) << EDMA3_CCRL_QCHMAP_TRWORD_SHIFT)
/** QUEPRI bits Clear */
#define EDMA3_RM_QUEPRI_CLR_MASK(queNum)           (~(EDMA3_CCRL_QUEPRI_PRIQ0_MASK << ((queNum) * EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT)))
/** QUEPRI bits Set */
#define EDMA3_RM_QUEPRI_SET_MASK(queNum,quePri)    ((EDMA3_CCRL_QUEPRI_PRIQ0_MASK & (quePri)) << ((queNum) * EDMA3_CCRL_QUEPRI_PRIQ1_SHIFT))
/** QUEWMTHR bits Clear */
#define EDMA3_RM_QUEWMTHR_CLR_MASK(queNum)         (~(EDMA3_CCRL_QWMTHRA_Q0_MASK << ((queNum) * EDMA3_CCRL_QWMTHRA_Q1_SHIFT)))
/** QUEWMTHR bits Set */
#define EDMA3_RM_QUEWMTHR_SET_MASK(queNum,queThr)  ((EDMA3_CCRL_QWMTHRA_Q0_MASK & (queThr)) << ((queNum) * EDMA3_CCRL_QWMTHRA_Q1_SHIFT))

/** OPT-TCC bitfield Clear */
#define EDMA3_RM_OPT_TCC_CLR_MASK                  (~EDMA3_CCRL_OPT_TCC_MASK)
/** OPT-TCC bitfield Set */
#define EDMA3_RM_OPT_TCC_SET_MASK(tcc)             (((EDMA3_CCRL_OPT_TCC_MASK >> EDMA3_CCRL_OPT_TCC_SHIFT) & (tcc)) << EDMA3_CCRL_OPT_TCC_SHIFT)

/** PaRAM Set Entry for Link and B count Reload field */
#define EDMA3_RM_PARAM_ENTRY_LINK_BCNTRLD   (5u)





/* ERRSTAT */

#define EDMA3_TCRL_ERRSTAT_MMRAERR_MASK  (0x00000008u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_SHIFT (0x00000003u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_RESETVAL (0x00000000u)

/*----MMRAERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_MMRAERR_NONE  (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_MMRAERR_ERROR (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_TRERR_MASK    (0x00000004u)
#define EDMA3_TCRL_ERRSTAT_TRERR_SHIFT   (0x00000002u)
#define EDMA3_TCRL_ERRSTAT_TRERR_RESETVAL (0x00000000u)

/*----TRERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_TRERR_NONE    (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_TRERR_ERROR   (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_BUSERR_MASK   (0x00000001u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_SHIFT  (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_RESETVAL (0x00000000u)

/*----BUSERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_BUSERR_NONE   (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_ERROR  (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_RESETVAL      (0x00000000u)



/*----TRERR Tokens----*/
#define EDMA3_TCRL_ERRSTAT_TRERR_NONE    (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_TRERR_ERROR   (0x00000001u)

#define EDMA3_TCRL_ERRSTAT_BUSERR_MASK   (0x00000001u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_SHIFT  (0x00000000u)
#define EDMA3_TCRL_ERRSTAT_BUSERR_RESETVAL (0x00000000u)


/* ERRDET */

#define EDMA3_TCRL_ERRDET_TCCHEN_MASK    (0x00020000u)
#define EDMA3_TCRL_ERRDET_TCCHEN_SHIFT   (0x00000011u)
#define EDMA3_TCRL_ERRDET_TCCHEN_RESETVAL (0x00000000u)

#define EDMA3_TCRL_ERRDET_TCINTEN_MASK   (0x00010000u)
#define EDMA3_TCRL_ERRDET_TCINTEN_SHIFT  (0x00000010u)
#define EDMA3_TCRL_ERRDET_TCINTEN_RESETVAL (0x00000000u)

#define EDMA3_TCRL_ERRDET_TCC_MASK       (0x00003F00u)
#define EDMA3_TCRL_ERRDET_TCC_SHIFT      (0x00000008u)
#define EDMA3_TCRL_ERRDET_TCC_RESETVAL   (0x00000000u)

#define EDMA3_TCRL_ERRDET_STAT_MASK      (0x0000000Fu)
#define EDMA3_TCRL_ERRDET_STAT_SHIFT     (0x00000000u)
#define EDMA3_TCRL_ERRDET_STAT_RESETVAL  (0x00000000u)



#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_TI_EDMA3_PRIVATE_H */

/* end of file */

