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
 * \brief ͨ�ö�����BCH����/�����
 *
 * \internal
 * \par modification history:
 * - 1.00 2016-04-01  cyl, first porting.
 * \endinternal
 */


#ifndef __AW_SOFT_BCH_H
#define __AW_SOFT_BCH_H

#include "apollo.h"


/** \brief ���ڱ�ʾ����ʽ�Ľṹ */
typedef struct aw_soft_bch_gf_poly {

    /** \brief polynomial degree */
    unsigned int deg;

    /** \brief polynomial terms */
    unsigned int c[1];

} aw_soft_bch_gf_poly_t;


/** \brief 1�ζ���ʽ */
typedef struct aw_soft_bch_gf_poly_deg1 {

    /** \brief polynomial structure var*/
    struct aw_soft_bch_gf_poly poly;

    /** \brief polynomial terms */
    unsigned int          c[2];

} aw_soft_bch_gf_poly_deg1_t;


/**
 * \brief BCH���ƽṹ
 */
typedef struct aw_soft_bch_ctrl {

    /** \brief ٤�޻���Ľ�GF(2^m) */
    unsigned int    m;

    /** \brief ������ִ�С(��λbits), 2^m - 1 */
    unsigned int    n;

    /** brief У��ǿ�ȣ���BCH�ܹ�У��t bits */
    unsigned int    t;

    /** \brief ECC��С(��λbits), m * t / 8 */
    unsigned int    ecc_bits;

    /** \brief ECC��С(��λbytes), m * t */
    unsigned int    ecc_bytes;

    /** \brief ٤�޻���GF(2^m)ָ�����ұ� */
    uint16_t       *a_pow_tab;

    /** \brief ٤�޻���GF(2^m)�������ұ� */
    uint16_t       *a_log_tab;

    /** \brief �������ɶ���ʽ���ұ� */
    uint32_t       *mod8_tab;

    /** \brief ECCУ���ֻ��� */
    uint32_t       *ecc_buf;

    /** \brief ECCУ���ֻ��� */
    uint32_t       *ecc_buf2;

    /** \brief 2��������ʽ�� */
    unsigned int   *xi_tab;

    /** \brief ����ʽ */
    unsigned int   *syn;

    /** \brief ����ʽ���� */
    int            *cache;

    /** \brief ����λ�ö���ʽ */
    struct aw_soft_bch_gf_poly *elp;

    /** \brief ��ʱ2t�ζ���ʽ */
    struct aw_soft_bch_gf_poly *poly_2t[4];

} aw_soft_bch_ctrl_t;


/**
 * \breif BCH��ʼ��
 *
 * \param[in] m          2^m - 1 > У�����ݳ���(��512�ֽڵ�) * 8
 * \param[in] t          У��ǿ��
 * \param[in] prim_poly  ��ԭ����ʽ�����Ϊ0��ʹ���ڲ�Ĭ�ϵı�ԭ����ʽ
 *
 * \retval ��NULL  BCH������
 * \retval   NULL  ��ʼ��BCH������ʧ��
 *
 * \note ����: 5 <= m <= 15
 */
struct aw_soft_bch_ctrl *aw_soft_bch_init (int m, int t, unsigned int prim_poly);


/**
 * \brief BCHȥ��ʼ��
 * \param[in] bch  BCH������
 * \return ��
 */
void aw_soft_bch_deinit (struct aw_soft_bch_ctrl *bch);


/**
 * \brief BCH����
 *
 * \param[in]  bch  BCH������
 * \param[in]  data Ҫ���������
 * \param[in]  len  ���ݳ���
 * \param[in]  ecc  ������ECC�ֽ�����
 *
 * \return ��
 *
 * \note 1. ��Ų�����ECC�ֽ����ݵĻ�����ecc�ڵ��øú���ǰ������0
 *       2. ecc��������С������bch->ecc_bytes��Сһ��
 */
void aw_soft_bch_encode (struct aw_soft_bch_ctrl *bch,
                         const uint8_t           *data,
                         unsigned int             len,
                         uint8_t                 *ecc);


/**
 * \brief BCH����
 *
 * \param[in]  bch       BCH������
 * \param[in]  data      Ҫ���������
 * \param[in]  len       ���ݳ���
 * \param[in]  recv_ecc  ���յ���ECC�ֽڣ���ԭ���洢��ECC�ֽ�
 * \param[in]  calc_ecc  ����õ���ECC�ֽڣ����ڶ�ȡ�����ݺ����õ���ECC�ֽ�
 * \param[in]  syn       ����ʽ
 * \param[out] errloc    ��������λ�õĻ�����
 *
 * \retval 0      �޴���
 * \retval ����0  ��ҪУ����λ����
 * \retval С��0  �޷�У���Ĵ���
 *
 * \par һЩ���÷�����˵��
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
