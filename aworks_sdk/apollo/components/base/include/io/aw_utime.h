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
 * \brief access and modification times structure
 *
 * \internal
 * \par modification history:
 * - 1.00 15-04-20 deo, first implementation.
 * \endinternal
 */

#ifndef AW_UTIME_H_
#define AW_UTIME_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_iosys_utime
 * @{
 */

#include "aworks.h"
#include <aw_types.h>

/**
 * \brief utime结构体
 */
struct aw_utimbuf {
    time_t    actime;    /**< \brief 访问时间. */
    time_t    modtime;   /**< \brief 修改时间. */
};


/**
 * \brief 设置文件的访问和修改时间
 *
 * \param[in]         path     文件路径
 * \param[in]         times    结构体指针
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_utime(const char *path, struct aw_utimbuf *times);

/** @} grp_iosys_utime */

#ifdef __cplusplus
}
#endif

#endif /* AW_UTIME_H_ */
