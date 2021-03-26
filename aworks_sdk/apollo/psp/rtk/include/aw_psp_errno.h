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
 * \brief aworks1.0  errno 标准接口头文件
 *
 * \internal
 * \par modification history:
 * - 1.00 13-03-05  zen, first implementation
 * \endinternal
 */

#ifndef __AW_PSP_ERRNO_H
#define __AW_PSP_ERRNO_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \name 错误编码数据类型
 * @{
 */
typedef int aw_psp_error_t;    /* 兼容POSIX错误类型 */
/** @} */





/******************************************************************************/
#define AW_PSP_ERRNO_GET(err)  do {(err) = (*rtk_task_errno_addr());}while(0)
#define AW_PSP_ERRNO_SET(err)  do {(*rtk_task_errno_addr()) = (err);}while(0)



/** @} grp_aw_if_posix_err */

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AW_PSP_ERRNO_H */

/* end of file */
