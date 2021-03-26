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
 * Generic binary BCH encoding/decoding library
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 51
 * Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Copyright @ 2011 Parrot S.A.
 *
 * Author: Ivan Djelic <ivan.djelic@parrot.com>
 *
 * Description:
 *
 * This library provides runtime configurable encoding/decoding of binary
 * Bose-Chaudhuri-Hocquenghem (BCH) codes.
*/


/**
 * \file
 * \brief 通用二进制BCH编码/解码库
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-04-01  cyl, first porting.
 * \endinternal
 */


#ifndef __AW_SOFT_BCH_H
#define __AW_SOFT_BCH_H

#include "apollo.h"


/** \brief 用于表示多项式的结构 */
typedef struct aw_soft_bch_gf_poly {

    /** \brief polynomial degree */
    unsigned int deg;

    /** \brief polynomial terms */
    unsigned int c[1];

} aw_soft_bch_gf_poly_t;


/** \brief 1次多项式 */
typedef struct aw_soft_bch_gf_poly_deg1 {

    /** \brief polynomial structure var*/
    struct aw_soft_bch_gf_poly poly;

    /** \brief polynomial terms */
    unsigned int          c[2];

} aw_soft_bch_gf_poly_deg1_t;


/**
 * \brief BCH控制结构
 */
typedef struct aw_soft_bch_ctrl {

    /** \brief 伽罗华域的阶GF(2^m) */
    unsigned int    m;

    /** \brief 最大码字大小(单位bits), 2^m - 1 */
    unsigned int    n;

    /** brief 校验强度，即BCH能够校验t bits */
    unsigned int    t;

    /** \brief ECC大小(单位bits), m * t / 8 */
    unsigned int    ecc_bits;

    /** \brief ECC大小(单位bytes), m * t */
    unsigned int    ecc_bytes;

    /** \brief 伽罗华域GF(2^m)指数查找表 */
    uint16_t       *a_pow_tab;

    /** \brief 伽罗华域GF(2^m)对数查找表 */
    uint16_t       *a_log_tab;

    /** \brief 余数生成多项式查找表 */
    uint32_t       *mod8_tab;

    /** \brief ECC校验字缓存 */
    uint32_t       *ecc_buf;

    /** \brief ECC校验字缓存 */
    uint32_t       *ecc_buf2;

    /** \brief 2次求解多项式根 */
    unsigned int   *xi_tab;

    /** \brief 伴随式 */
    unsigned int   *syn;

    /** \brief 多项式缓存 */
    int            *cache;

    /** \brief 错误位置多项式 */
    struct aw_soft_bch_gf_poly *elp;

    /** \brief 临时2t次多项式 */
    struct aw_soft_bch_gf_poly *poly_2t[4];

} aw_soft_bch_ctrl_t;


/**
 * \breif BCH初始化
 *
 * \param[in] m          2^m - 1 > 校验数据长度(如512字节等) * 8
 * \param[in] t          校验强度
 * \param[in] prim_poly  本原多项式，如果为0则使用内部默认的本原多项式
 *
 * \retval 非NULL  BCH控制器
 * \retval   NULL  初始化BCH控制器失败
 *
 * \note 限制: 5 <= m <= 15
 */
struct aw_soft_bch_ctrl *aw_soft_bch_init (int m, int t, unsigned int prim_poly);


/**
 * \brief BCH去初始化
 * \param[in] bch  BCH控制器
 * \return 无
 */
void aw_soft_bch_deinit (struct aw_soft_bch_ctrl *bch);


/**
 * \brief BCH编码
 *
 * \param[in]  bch  BCH控制器
 * \param[in]  data 要编码的数据
 * \param[in]  len  数据长度
 * \param[in]  ecc  产生的ECC字节数据
 *
 * \return 无
 *
 * \note 1. 存放产生的ECC字节数据的缓冲区ecc在调用该函数前必须清0
 *       2. ecc缓冲区大小必须与bch->ecc_bytes大小一致
 */
void aw_soft_bch_encode (struct aw_soft_bch_ctrl *bch,
                         const uint8_t           *data,
                         unsigned int             len,
                         uint8_t                 *ecc);


/**
 * \brief BCH解码
 *
 * \param[in]  bch       BCH控制器
 * \param[in]  data      要解码的数据
 * \param[in]  len       数据长度
 * \param[in]  recv_ecc  接收到的ECC字节，即原来存储的ECC字节
 * \param[in]  calc_ecc  计算得到的ECC字节，即在读取到数据后计算得到的ECC字节
 * \param[in]  syn       伴随式
 * \param[out] errloc    描述错误位置的缓冲区
 *
 * \retval 0      无错误
 * \retval 大于0  需要校正的位个数
 * \retval 小于0  无法校正的错误
 *
 * \par 一些调用方法的说明
 *
 * \code
 *
 * aw_soft_bch_decode(bch, @data, @len, @recv_ecc, NULL, NULL, @errloc)
 *
 * aw_soft_bch_decode(@bch, NULL, @len, @recv_ecc, @calc_ecc, NULL, @errloc)
 *
 * aw_soft_bch_decode(@bch, NULL, @len, NULL, ecc, NULL, @errloc)
 *
 * aw_soft_bch_decode(@bch, NULL, @len, NULL, NULL, @syn, @errloc)
 *
 * \endcode
 */
int aw_soft_bch_decode (struct aw_soft_bch_ctrl *bch,
                        const uint8_t           *data,
                        unsigned int             len,
                        const uint8_t           *recv_ecc,
                        const uint8_t           *calc_ecc,
                        const unsigned int      *syn,
                        unsigned int            *errloc);

#endif /* __AW_SOFT_BCH_H */

/* end of file */
