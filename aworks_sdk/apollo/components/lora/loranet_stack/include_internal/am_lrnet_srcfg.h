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
 * \brief   LoRaNet SRC Configuration
 *
 * \internal
 * \par modification history:
 * - 2017-05-23 ebi, first implementation.
 * \endinternal
 */

#ifndef __AM_LRNET_SRCFG_H
#define __AM_LRNET_SRCFG_H

#ifdef __cplusplus
extern "C" {
#endif

/*******************************************************************************
  Debug Configure
*******************************************************************************/

/** \brief Debug���� */
#ifndef AM_LRNET_SRCFG_DEBUG
#define AM_LRNET_SRCFG_DEBUG                       0
#endif

/** \brief ���Եȼ������� */
#ifndef AM_LRNET_SRCFG_DEBUG_INFO_LV
#define AM_LRNET_SRCFG_DEBUG_INFO_LV               3
#endif

/** \brief �ڲ�Debug���� */
#ifndef AM_LRNET_SRCFG_INTERNAL_DEBUG
#define AM_LRNET_SRCFG_INTERNAL_DEBUG              0
#endif

/** \brief ���Կ��� */
#ifndef AM_LRNET_SRCFG_DEBUG_ASSERT
#define AM_LRNET_SRCFG_DEBUG_ASSERT                0
#endif

/*******************************************************************************
  Function Optional
*******************************************************************************/

/** \brief Payload�ü��� */
#define AM_LRNET_SRCFG_PAYLOAD_MAX                  210     /* todo: */

/** \brief Detect���ܲü� */
#ifndef AM_LRNET_SRCFG_DETECT_ENABLE
#define AM_LRNET_SRCFG_DETECT_ENABLE                1
#endif

/** \brief Echo���ܲü� */
#ifndef AM_LRNET_SRCFG_ECHO_ENABLE
#define AM_LRNET_SRCFG_ECHO_ENABLE                  1
#endif

/** \brief Timebc���ܲü� */
#ifndef AM_LRNET_SRCFG_TIMEBC_ENABLE
#define AM_LRNET_SRCFG_TIMEBC_ENABLE                1
#endif

/** \brief GFetch���ܲü� */
#ifndef AM_LRNET_SRCFG_GFETCH_ENABLE
#define AM_LRNET_SRCFG_GFETCH_ENABLE                1
#endif

/** \brief Error Report���ܲü� */
#ifndef AM_LRNET_SRCFG_EREPORT_ENABLE
#define AM_LRNET_SRCFG_EREPORT_ENABLE               1
#endif

/** \brief Upstream���ܲü� */
#ifndef AM_LRNET_SRCFG_UPSTREAM_ENABLE
#define AM_LRNET_SRCFG_UPSTREAM_ENABLE              1
#endif

/** \brief Listen Before Tx���ܼ��� Ӱ��API��Burst Report */
#ifndef AM_LRNET_SRCFG_LBT_ENABLE
#define AM_LRNET_SRCFG_LBT_ENABLE                   1
#endif

/*******************************************************************************
  Configure
*******************************************************************************/

/** \brief ����packet ���Ի���ǰ���ȳ��Եķ������Դ��� */
#define AM_LRNET_SRCFG_TX_TRY_RETRY                2

#define AM_LRNET_SRCFG_TX_AFTER_WAKEUP_RETRY       0

#define AM_LRNET_SRCFG_PREAMBLE                    10

/** \brief ��������·�ɷ���req����������Դ��� */
#define AM_LRNET_SRCFG_ROUTER_RETRY_MAX            3

#define AM_LRNET_SRCFG_RELIABILITY_RETRY_MAX       8
#define AM_LRNET_SRCFG_RELIABILITY_RETRY_MIN       2

#ifdef __cplusplus
}
#endif

#endif /* __AM_LRNET_SRCFG_H */

/* end of file */
