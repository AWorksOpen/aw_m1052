/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-11  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_CORE_DEBUG_H
#define __AWSA_CORE_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/*******************************************************************************
  Debug Info Format Ctrl
*******************************************************************************/

static inline
int awsa_core_kprintf (const char *fmt, ...)
{
    char     buf[128];
    int      len = 0;
    va_list  vlist;

    aw_kprintf("awsa core: ");

    va_start(vlist, fmt);
    len = aw_vsnprintf(buf, sizeof(buf), fmt, vlist);
    va_end(vlist);

    aw_kprintf("%s\r\n", buf);

    return len;
}

#define AWSA_CORE_DEBUG_FMT_HEAD       "\033["
#define AWSA_CORE_DEBUG_PRINTF         awsa_core_kprintf

#define AWSA_CORE_DEBUG_FMT_NONE       AWSA_CORE_DEBUG_FMT_HEAD "0m"
#define AWSA_CORE_DEBUG_FMT_CLR        AWSA_CORE_DEBUG_FMT_HEAD "2J"
#define AWSA_CORE_DEBUG_FMT_BLACK      AWSA_CORE_DEBUG_FMT_HEAD "30m"
#define AWSA_CORE_DEBUG_FMT_RED        AWSA_CORE_DEBUG_FMT_HEAD "31m"
#define AWSA_CORE_DEBUG_FMT_GREEN      AWSA_CORE_DEBUG_FMT_HEAD "32m"
#define AWSA_CORE_DEBUG_FMT_YELLOW     AWSA_CORE_DEBUG_FMT_HEAD "33m"
#define AWSA_CORE_DEBUG_FMT_BLUE       AWSA_CORE_DEBUG_FMT_HEAD "34m"
#define AWSA_CORE_DEBUG_FMT_PURPLE     AWSA_CORE_DEBUG_FMT_HEAD "35m"
#define AWSA_CORE_DEBUG_FMT_CYAN       AWSA_CORE_DEBUG_FMT_HEAD "36m"
#define AWSA_CORE_DEBUG_FMT_WHITE      AWSA_CORE_DEBUG_FMT_HEAD "37m"

#define AWSA_CORE_DEBUG_FMT_SET(__n)   AWSA_CORE_DEBUG_PRINTF(__n)

/*******************************************************************************
  Debug Info Level
*******************************************************************************/
#define __UNUSED_MACRO_PRINTF(__NAME)  \
            static inline              \
            void __NAME (const char *unused, ...) { (void) unused; }

/**
 * \brief debug level (from low to high)
 */
typedef enum {
    AWSA_CORE_DEBUG_LV_NONE   = 0,  /**< \brief no debug level */
    AWSA_CORE_DEBUG_LV_ERROR,       /**< \brief debug level：error */
    AWSA_CORE_DEBUG_LV_WARN,        /**< \brief debug level：warning */
    AWSA_CORE_DEBUG_LV_INFO,        /**< \brief debug level：information */
    AWSA_CORE_DEBUG_LV_VERBOSE,     /**< \brief debug level：verbose */
} awsa_core_level_t;

#ifndef AWSA_CORE_DEBUG_LV
#define AWSA_CORE_DEBUG_LV       AWSA_CORE_DEBUG_LV_VERBOSE
#endif

#if (AWSA_CORE_DEBUG_LV > AWSA_CORE_DEBUG_LV_VERBOSE)
#error "awsa error debug level."
#endif

#if (AWSA_CORE_SRCFG_DEBUG && (AWSA_CORE_DEBUG_LV >= AWSA_CORE_DEBUG_LV_ERROR))
#define AWSA_CORE_DEBUG_ERROR    AWSA_CORE_DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AWSA_CORE_DEBUG_ERROR)
#endif

#if (AWSA_CORE_SRCFG_DEBUG && (AWSA_CORE_DEBUG_LV >= AWSA_CORE_DEBUG_LV_WARN))
#define AWSA_CORE_DEBUG_WARN     AWSA_CORE_DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AWSA_CORE_DEBUG_WARN)
#endif

#if (AWSA_CORE_SRCFG_DEBUG && (AWSA_CORE_DEBUG_LV >= AWSA_CORE_DEBUG_LV_INFO))
#define AWSA_CORE_DEBUG_INFO     AWSA_CORE_DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AWSA_CORE_DEBUG_INFO)
#endif

#if (AWSA_CORE_SRCFG_DEBUG && (AWSA_CORE_DEBUG_LV >= AWSA_CORE_DEBUG_LV_VERBOSE))
#define AWSA_CORE_DEBUG_VERBOSE  AWSA_CORE_DEBUG_PRINTF
#else
__UNUSED_MACRO_PRINTF(AWSA_CORE_DEBUG_VERBOSE)
#endif

/*******************************************************************************
  Assert
*******************************************************************************/
#if (AWSA_CORE_SRCFG_DEBUG && AWSA_CORE_SRCFG_DEBUG_ASSERT)
#define __ASSERT_VTOS(n)     #n
#define __ASSERT_SVAL(m)     __ASSERT_VTOS(m)
#define awsa_core_assert(e_)    do {                           \
    if (!(e_)) {                                               \
        AWSA_CORE_DEBUG_PRINTF(                                \
            __FILE__":"__ASSERT_SVAL(__LINE__)":("#e_")\n");   \
        while(1);                                              \
    }                                                          \
} while(0)

#else
static inline
void awsa_core_assert (int unused)
{
    (void) unused;
}
#endif

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_DEBUG_H */

/* end of file */

