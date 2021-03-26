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
 * \brief 非易失性存储器接口头文件
 *
 * 使用本模块需要包含头文件 aw_nvram.h
 *
 * \par 简单示例
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4] = {1, 1, 1, 1};
 *  char data[4];
 *
 *  aw_nvram_set("ip", 0, &ip[0], 0, 4);    // 设置非易失性数据"ip"
 *  aw_nvram_get("ip", 0, &data[0], 0, 4);  // 读取非易失性数据"ip"
 * \endcode
 *
 * // 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-30  zen, first implementation
 * \endinternal
 */

#ifndef __AW_NVRAM_H
#define __AW_NVRAM_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

/**
 * \addtogroup grp_aw_if_nvram
 * \copydoc aw_nvram.h
 * @{
 */

/**
 * \brief 读取一条非易失性存储信息。
 *
 * 本函数从非易失性存储设备中读取非易失性信息并保存到调用者提供的缓冲区中。任何
 * 需要使用NVRAM服务的模块都可以使用本函数。
 *
 * 调用者使用\a p_name 和\a unit 来标识一条非易失性信息。
 *
 * 拷贝的起始位置由\a offset 指定，拷贝数据总量由\a len 指定。如果指定的大小超出
 * 了该非易失性信息的大小，则其行为未定义。
 *
 * \param[in]  p_name   非易失性存储信息的名字
 * \param[in]  unit     非易失性存储信息的单元号 
 *                      (相同名字的非易失性信息用单元号区分)
 * \param[out] p_buf    数据返回的缓冲区
 * \param[in]  offset   在非易失性存储信息中的偏移
 * \param[in]  len      要读取的长度
 *
 * \retval  AW_OK     : 成功
 * \retval -AW_EFAULT : \a p_name 或 \a p_buf 为NULL
 * \retval -AW_EXIO   : 指定的非易失性存储信息不存在或操作失败
 *
 * \par 示例
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4];
 *  aw_nvram_get("ip", 0, &ip[0], 0, 4);    // 读取非易失性数据"ip"
 * \endcode
 *
 * \sa aw_nvram_set()
 */
aw_err_t aw_nvram_get (char *p_name, int unit, char *p_buf, int offset, int len);

/**
 * \brief 写入一条非易失性存储信息。
 *
 * 本函数从调用者提供的缓冲区中读取信息，并将其存储到非易失性存储设备中。任何
 * 需要使用NVRAM服务的模块都可以使用本函数。
 *
 * 调用者使用\a p_name 和\a unit 来标识一条非易失性信息。
 *
 * 存储的起始位置由\a offset 指定，存储数据总量由\a len 指定。如果指定的大小超出
 * 了该非易失性信息的大小，则其行为未定义。
 *
 * \param[in] p_name   非易失性存储信息的名字
 * \param[in] unit     非易失性存储信息的单元号 
 *                     (相同名字的非易失性信息用单元号区分)
 * \param[in] p_buf    写入数据缓冲区
 * \param[in] offset   在非易失性存储信息中的偏移
 * \param[in] len      要存储的长度
 *
 * \retval  AW_OK     : 成功
 * \retval -AW_EFAULT : \a p_name 或 \a p_buf 为NULL
 * \retval -AW_EXIO   : 指定的非易失性存储信息不存在或操作失败
 *
 * \par 示例
 * \code
 *  #include "aw_nvram.h"
 *
 *  char ip[4] = {1, 1, 1, 1};
 *  aw_nvram_set("ip", 0, &ip[0], 0, 4);    // 设置非易失性数据"ip"
 * \endcode
 *
 * \sa aw_nvram_get()
 */
aw_err_t aw_nvram_set (char *p_name, int unit, char *p_buf, int offset, int len);


/** @} grp_aw_if_nvram */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_NVRAM_H */

/* end of file */
