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

/*
 ***********************************************************************
 ** md5.h -- header file for implementation of MD5                    **
 ** RSA Data Security, Inc. MD5 Message-Digest Algorithm              **
 ** Created: 2/17/90 RLR                                              **
 ** Revised: 12/27/90 SRD,AJ,BSK,JT Reference C version               **
 ** Revised (for MD5): RLR 4/27/91                                    **
 **   -- G modified to have y&~z instead of y&z                       **
 **   -- FF, GG, HH modified to add in last register done             **
 **   -- Access pattern: round 2 works mod 5, round 3 works mod 3     **
 **   -- distinct additive constant for each step                     **
 **   -- round 4 added, working mod 7                                 **
 ***********************************************************************
 */

/*
 ***********************************************************************
 ** Copyright (C) 1990, RSA Data Security, Inc. All rights reserved.  **
 **                                                                   **
 ** License to copy and use this software is granted provided that    **
 ** it is identified as the "RSA Data Security, Inc. MD5 Message-     **
 ** Digest Algorithm" in all material mentioning or referencing this  **
 ** software or this function.                                        **
 **                                                                   **
 ** License is also granted to make and use derivative works          **
 ** provided that such works are identified as "derived from the RSA  **
 ** Data Security, Inc. MD5 Message-Digest Algorithm" in all          **
 ** material mentioning or referencing the derived work.              **
 **                                                                   **
 ** RSA Data Security, Inc. makes no representations concerning       **
 ** either the merchantability of this software or the suitability    **
 ** of this software for any particular purpose.  It is provided "as  **
 ** is" without express or implied warranty of any kind.              **
 **                                                                   **
 ** These notices must be retained in any copies of any part of this  **
 ** documentation and/or software.                                    **
 ***********************************************************************
 */

/**
 * \file
 * \brief MD5通用标准接口
 *
 * \internal
 * \par modification history
 * - 1.00 14-07-18  dcf, rename function name to aw_xx.
 * \endinternal
 */

#ifndef __AW_MD5_H
#define __AW_MD5_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_md5
 * \copydoc aw_md5.h
 * @{
 */

/** \brief MD5(消息摘要算法第五版)数据结构  */
typedef struct {

  /** \brief 原始信息的bits数长度，不包括填充的bits，最长为2^64bits */
  uint32_t i[2];           
  
  /**
   * \brief 最终计算得到的消息摘要，当消息长度>512bits时，
   *        也用于存放每个512bits的中间结果 
   */
  uint32_t buf[4];  
  unsigned char in[64];     /**< \brief 输入信息缓冲区 */
  unsigned char digest[16]; /**< \brief 最终的消息摘要计算值 */
} aw_md5_t;

/**
 * \brief md5 算法初始化
 *
 * \param[in] md5obj  md5 数据句柄
 *
 * \return 无
 */
void aw_md5_init (aw_md5_t *md5obj);

/**
 * \brief 更新输入值
 *
 * \param[in,out] md5obj  md5 数据
 * \param[in]     in_buf  数据缓冲区地址
 * \param[in]     in_len  数据个数
 *
 * \return 无
 */
void aw_md5_update(aw_md5_t      *md5obj,
                   unsigned char *in_buf,
                   unsigned int   in_len);

/**
 * \brief 输出计算值-MD5值
 *
 * \param[out] hash    最终计算得到的md值，hash指向的空间至少为16字节
 * \param[in]  md5obj  md5数据句柄值
 *
 * \return 无
 */
void aw_md5_final (unsigned char hash[], aw_md5_t *md5obj);

/** @} grp_aw_if_md5 */

#ifdef __cplusplus
}
#endif

#endif /* __AW_MD5_H */

 /* end of file */
 