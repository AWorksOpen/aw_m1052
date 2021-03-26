/*******************************************************************************
*                                 AnyWhere
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      anywhere.support@zlg.cn
*******************************************************************************/

/*******************************************************************************
  �жϿ�����������Ϣ
*******************************************************************************/
#include "driver/intctlr/awbl_armcm_nvic.h"

/*******************************************************************************
  ARM Cortex-M NVIC ������Ϣ
*******************************************************************************/

/* ISR ��Ϣ�ڴ澲̬���� */
aw_local struct awbl_armcm_nvic_isrinfo __g_armcm_nvic_isrinfos[160];
#if defined (__CC_ARM)

extern  unsigned int  const Image$$ER_VEC_SCT$$ZI$$Base;
extern  unsigned int  const Image$$ER_VEC_SCT$$ZI$$Limit;

#define RAM_VECTOR_START    ((uint32_t *)(&Image$$ER_VEC_SCT$$ZI$$Base))
#define RAM_VECTOR_END      ((uint32_t *)(&Image$$ER_VEC_SCT$$ZI$$Limit))

#else

#if defined(__GNUC__)

extern char __ram_vector_start__;
extern char __ram_vector_end__;

#define RAM_VECTOR_START    ((uint32_t *)(&__ram_vector_start__))
#define RAM_VECTOR_END      (((uint32_t *)&__ram_vector_end__) - 1 )

#endif

#endif

/* NVIC �豸��Ϣ */
aw_local aw_const struct awbl_armcm_nvic_devinfo __g_armcm_nvic_devinfo = {

    {
        0,                /* ֧�ֵ���ʼ�жϺ� */
        159,              /* ֧�ֵĽ����жϺ� */
    },

    160,
    &__g_armcm_nvic_isrinfos[0],      /* ISR ��Ϣ���ڴ�(��С����isrinfo_cnt) */
    NULL,
    NULL,
};

/* NVIC �豸ʵ���ڴ澲̬����*/
aw_local struct awbl_armcm_nvic_dev __g_armcm_nvic_dev;

#define AWBL_HWCONF_IMX1050_NVIC   \
    {                              \
        AWBL_ARMCM_NVIC_NAME,      \
        0,                         \
        AWBL_BUSID_PLB,            \
        0,                         \
        (struct awbl_dev *)&__g_armcm_nvic_dev, \
        &__g_armcm_nvic_devinfo    \
    },



/* end of file */

