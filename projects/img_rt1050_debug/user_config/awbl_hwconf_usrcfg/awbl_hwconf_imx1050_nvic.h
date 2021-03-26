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
  中断控制器配置信息
*******************************************************************************/
#include "driver/intctlr/awbl_armcm_nvic.h"

/*******************************************************************************
  ARM Cortex-M NVIC 配置信息
*******************************************************************************/

/* ISR 信息内存静态分配 */
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

/* NVIC 设备信息 */
aw_local aw_const struct awbl_armcm_nvic_devinfo __g_armcm_nvic_devinfo = {

    {
        0,                /* 支持的起始中断号 */
        159,              /* 支持的结束中断号 */
    },

    160,
    &__g_armcm_nvic_isrinfos[0],      /* ISR 信息槽内存(大小等于isrinfo_cnt) */
    NULL,
    NULL,
};

/* NVIC 设备实例内存静态分配*/
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

