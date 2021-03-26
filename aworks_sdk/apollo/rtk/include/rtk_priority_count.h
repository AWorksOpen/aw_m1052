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
 * \file  rtk_priority_count.h
 * \brief 定义rtk 优先级数量相关结构和接口函数
 *
 * \internal
 * \par modification history:
 * - 1.00 18-05-15  sni, first implement
 * \endinternal
 */

#ifndef         __RTK_PRIORITY_COUNT_H__
#define         __RTK_PRIORITY_COUNT_H__

unsigned int rtk_max_priority( void );

#define rtk_get_realtime_priority_count() (CONFIG_RTK_REALTIME_PRIORITY_COUNT)
#define rtk_get_sys_priority_count() (CONFIG_RTK_SYS_PRIORITY_COUNT)
#define rtk_get_usr_priority_count() (CONFIG_RTK_USR_PRIORITY_COUNT)
#define rtk_get_sys_low_priority_count() (CONFIG_RTK_SYS_LOW_PRIORITY_COUNT)


#define RTK_MAX_PRIORITY            rtk_max_priority()

#endif          /*__RTK_PRIORITY_COUNT_H__*/
