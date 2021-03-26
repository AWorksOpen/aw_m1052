/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief 网络状态管理内部使用头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 20-03-16  vih, first implementation
 * \endinternal
 */

#ifndef __NET_STS_INTERNAL_H__
#define __NET_STS_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/******************************************************************************/
#define __LOG_FCOLOR_RED        "\x1B[31m"
#define __LOG_FCOLOR_GREEN      "\x1B[32m"

#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask) && _pfn_log_out_) {\
         _pfn_log_out_(color, PFX, __LINE__, fmt, ##__VA_ARGS__);\
    }

#define __LOG_PFX(mask, fmt, ...) \
    __LOG_BASE(mask, NULL, "[%s:%4d] " fmt "\n", \
               PFX, __LINE__, ##__VA_ARGS__)

#define __LOG_RES(is_true, mask, fmt, ...) \
    if (is_true) {\
        __LOG_BASE(mask, __LOG_FCOLOR_GREEN, "[%s:%4d] " "[OK] " fmt "\n", \
                     PFX, __LINE__, ##__VA_ARGS__);\
    } else {\
        __LOG_BASE(1, __LOG_FCOLOR_RED,\
                   "[%s:%4d] " "[ER] " fmt "\n", \
                     PFX, __LINE__,  ##__VA_ARGS__);\
    }

#define __CHECK(mask, cond, exe, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE(mask, __LOG_FCOLOR_RED, \
                   "[ CHECK Failed ][%s:%d] " #cond "\n- " fmt " \n",\
                   __func__, __LINE__, ##__VA_ARGS__); \
        exe; \
    }

#define __ASSERT(cond, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE(1, __LOG_FCOLOR_RED,\
               "[ ASSERT Failed ] \n- file: %s : %d \n- func: %s() \n- cond: "\
               #cond "\n", __FILE__, __LINE__, __func__); \
        if (fmt != NULL && strlen((char *)fmt) != 0) {\
            __LOG_BASE(1, __LOG_FCOLOR_RED,"- \"" fmt "\"\n", ##__VA_ARGS__);\
        } \
        aw_assert(cond); \
    }

#define __CHECK_D(mask, a, cond, b, exe)  \
        __CHECK(mask, (a cond b), exe, "%d %d", a, b)
#define __ASSERT_D(a, cond, b)        __ASSERT((a cond b), "%d %d", a, b)

aw_net_sts_dev_t * aw_net_sts_4g_dev_create (aw_net_sts_dev_init_t *p_init);
aw_net_sts_dev_t * aw_net_sts_eth_dev_create (aw_net_sts_dev_init_t *p_init);
aw_net_sts_dev_t * aw_net_sts_wifi_dev_create (aw_net_sts_dev_init_t *p_init);

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __NET_STS_INTERNAL_H__ */

/* end of file */

