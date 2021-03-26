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
 * \brief AWBus TI UPP 通用并行接口
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-06  dcf, first implementation
 * \endinternal
 */

#include <stdarg.h>
#include <stdint.h>
#include "apollo.h"
#include "awbl_upp.h"

#ifndef AWBL_TI_UPP_H
#define AWBL_TI_UPP_H


#define AWBL_TI_UPP_DRV_NAME "awbl_ti_upp"

/**
 * \brief TI UPP 寄存器定义
 */
typedef struct
{
    volatile uint32_t UPPID;        /**< \brief 0x0000 */
    volatile uint32_t UPPCR;        /**< \brief 0x0004 */
    volatile uint32_t UPDLB;        /**< \brief 0x0008 */
    volatile uint32_t NA1;          /**< \brief 0x000c not accessable */
    volatile uint32_t UPCTL;        /**< \brief 0x0010 */
    volatile uint32_t UPICR;        /**< \brief 0x0014 */
    volatile uint32_t UPIVR;        /**< \brief 0x0018 */
    volatile uint32_t UPTCR;        /**< \brief 0x001c */
    volatile uint32_t UPISR;        /**< \brief 0x0020 */
    volatile uint32_t UPIER;        /**< \brief 0x0024 */
    volatile uint32_t UPIES;        /**< \brief 0x0028 */
    volatile uint32_t UPIEC;        /**< \brief 0x002c */
    volatile uint32_t UPEOI;        /**< \brief 0x0030 */
    volatile uint32_t NA2;          /**< \brief 0x0034 not accessable */
    volatile uint32_t NA3;          /**< \brief 0x0038 not accessable */
    volatile uint32_t NA4;          /**< \brief 0x003c not accessable */
    volatile uint32_t UPID0;        /**< \brief 0x0040 */
    volatile uint32_t UPID1;        /**< \brief 0x0044 */
    volatile uint32_t UPID2;        /**< \brief 0x0048 */
    volatile uint32_t NA5;          /**< \brief 0x004c not accessable */
    volatile uint32_t UPIS0;        /**< \brief 0x0050 */
    volatile uint32_t UPIS1;        /**< \brief 0x0054 */
    volatile uint32_t UPIS2;        /**< \brief 0x0058 */
    volatile uint32_t NA6;          /**< \brief 0x005c not accessable */
    volatile uint32_t UPQD0;        /**< \brief 0x0060 */
    volatile uint32_t UPQD1;        /**< \brief 0x0064 */
    volatile uint32_t UPQD2;        /**< \brief 0x0068 */
    volatile uint32_t NA7;          /**< \brief 0x007c not accessable */
    volatile uint32_t UPQS0;        /**< \brief 0x0070 */
    volatile uint32_t UPQS1;        /**< \brief 0x0074 */
    volatile uint32_t UPQS2;        /**< \brief 0x0078 */
} upp_regs_t;

typedef union
{
  struct
  {
    unsigned DPEI           :1;
    unsigned UORI           :1;
    unsigned ERRI           :1;
    unsigned EOWI           :1;
    unsigned EOLI           :1;
    unsigned RESERVED2      :3;
    unsigned DPEQ           :1;
    unsigned UORQ           :1;
    unsigned ERRQ           :1;
    unsigned EOWQ           :1;
    unsigned EOLQ           :1;
    unsigned RESERVED1      :19;
  } bits;
  uint32_t value;
}UPISR_t;

typedef union
{
    struct
    {
        unsigned ACT        :1;
        unsigned PEND       :1;
        unsigned RESERVED2  :2;
        unsigned WM         :4;
        unsigned RESERVED1  :24;
    }bits;
    uint32_t value;
}UPXS2_t;

typedef struct
{
    /** \brief UPCTL */
    union
    {
    struct
    {
      uint32_t MODE       :2;
      uint32_t CHN        :1;
      uint32_t SDRTXIL    :1;
      uint32_t DDRDEMUX   :1;
      uint32_t RESERVED3  :11;
      uint32_t DRA        :1;
      uint32_t IWA        :1;
      uint32_t DPWA       :3;
      uint32_t DPFA       :2;
      uint32_t RESERVED2  :1;
      uint32_t DRB        :1;
      uint32_t IWB        :1;
      uint32_t DPWB       :3;
      uint32_t DPFB       :2;
      uint32_t RESERVED1  :1;
    }bits;
    uint32_t value;
    }UPCTL;

    /** \brief UPPCR */
    union
    {
      struct
      {

        unsigned FREE         :1;
        unsigned SOFT         :1;
        unsigned RTEMU        :1;
        unsigned EN           :1;
        unsigned SWRST        :1;
        unsigned RESERVED2    :2;
        unsigned DB           :1;
        unsigned RESERVED1    :24;
      }bits;
      uint32_t value;
    }UPPCR;

    /** \brief UPDLB */
    union
    {
    struct
    {
      unsigned RESERVED2      :12;
      unsigned AB             :1;
      unsigned BA             :1;
      unsigned RESERVED1      :18;
    }bits;
    uint32_t value;
    }UPDLB;

    /** \brief UPICR */
    union
    {
    struct
    {
      unsigned STARTPOLA      :1;
      unsigned ENAPOLA        :1;
      unsigned WAITPOLA       :1;
      unsigned STARTA         :1;
      unsigned ENAA           :1;
      unsigned WAITA          :1;
      unsigned RESERVED4      :2;
      unsigned CLKDIVA        :4;
      unsigned CLKINVA        :1;
      unsigned TRISA          :1;
      unsigned RESERVED3      :2;
      unsigned STARTPOLB       :1;
      unsigned ENAPOLB         :1;
      unsigned WAITPOLB       :1;
      unsigned STARTB         :1;
      unsigned ENAB           :1;
      unsigned WAITB          :1;
      unsigned RESERVED2      :2;
      unsigned CLKDIVB        :4;
      unsigned CLKINVB        :1;
      unsigned TRISB          :1;
      unsigned RESERVED1      :2;
    }bits;
    uint32_t value;
    }UPICR;

    /** \brief UPIVR */
    union
    {
    struct
    {
      unsigned VALA            :16;
      unsigned VALB            :16;
    }bits;
    uint32_t value;
    }UPIVR;

    /** \brief UPTCR */
    union
    {
        struct
        {
          unsigned RDSIZEI        :2;
          unsigned RESERVED4      :6;
          unsigned RDSIZEQ        :2;
          unsigned RESERVED3      :6;
          unsigned TXSIZEA        :2;
          unsigned RESERVED2      :6;
          unsigned TXSIZEB        :2;
          unsigned RESERVED1      :6;
        }bits;
        uint32_t value;
    }UPTCR;

    /** \brief UPIES */
    union
    {
        struct
        {
          unsigned DPEI           :1;
          unsigned UORI           :1;
          unsigned ERRI           :1;
          unsigned EOWI           :1;
          unsigned EOLI           :1;
          unsigned RESERVED2      :3;
          unsigned DPEQ           :1;
          unsigned UORQ           :1;
          unsigned ERRQ           :1;
          unsigned EOWQ           :1;
          unsigned EOLQ           :1;
          unsigned RESERVED1      :19;
        } bits;
        uint32_t value;
    }UPIES;
} upp_config_t;


struct awbl_ti_upp;

/**
 * \brief 设备配置信息
 */
struct awbl_ti_upp_devinfo
{
    upp_regs_t   *base;     /**< \brief 基地址   */
    upp_config_t *cfg_in;   /**< \brief 基本配置(输入)，为0时有默认值 */
    
    /** \brief 基本配置(输出), 为0时无默认值，不支持输出 */
    upp_config_t *cfg_out;  

    int vector;              /**< \brief INUM */

    void (*pfunc_platform_init)( struct awbl_ti_upp *thiz );
};

/**
 * \bief TI UPP设备定义
 *
 */
struct awbl_ti_upp
{
    struct awbl_upp super;/**< \brief 属于awbl_upp设备 */

    upp_regs_t *reg_upp;
    upp_config_t *cfg;

    int vector;/**< \brief INUM */
    upp_config_t cfg_default;

    int channel;          

    void *reserved;
    int is_configured;
    AW_MSGQ_DECL(msgq, 512, sizeof(void*));
};


void awbl_ti_upp_drv_register(void);

#endif

/* end of file */
