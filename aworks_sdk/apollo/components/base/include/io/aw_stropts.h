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
 * \brief STREAMS interface.
 *
 * \internal
 * \par modification history:
 * - 1.00 14-08-22 orz, first implementation.
 * \endinternal
 */

#ifndef __IO_STROPTS_H /* { */
#define __IO_STROPTS_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_ios_stropts
 * @{
 */

#include "aworks.h"
#include "aw_ioctl.h"

#define I_FLUSH     AW_FIOFLUSH
#define I_NREAD     AW_FIONREAD


/**
 * \brief 发送io控制命令
 *
 * \param[in]         filedes     文件描述符
 * \param[in]         request     命令
 * \param[out]        argument    参数
 *
 * \return 成功返回AW_OK，否则返回值为错误编码，见 aw_errno.h 。
 */
int aw_ioctl (int filedes, int request, void *argument);

/** @} grp_ios_stropts */

#ifdef __cplusplus
}
#endif

#endif /* } __IO_STROPTS_H */

/* end of file */
