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
 * \brief ͨ��������ඨ��
 *
 * \internal
 * \par modification history
 * - 1.02 15-03-17  ops, redefine the aw_digitron_disp_str.
 * - 1.01 14-12-06  ops, the abstraction of common operation interface.
 * - 1.00 14-11-06  ops, first implementation.
 * \endinternal
 */

#ifndef __AWBL_DIGITRON_H
#define __AWBL_DIGITRON_H

#include "apollo.h"
#include "aw_compiler.h"
#include "aw_common.h"
#include "awbus_lite.h"
#include "aw_types.h"
#include "aw_list.h"
#include "aw_digitron_disp.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ����ܾ���������Ϣ
 */
struct awbl_digitron_disp_param {

    uint8_t   num_segment;            /**< \brief ����ܶ��� */

    uint8_t   num_rows;               /**< \brief ����ܾ�������� */
    uint8_t   num_cols;               /**< \brief ����ܾ�������� */

    uint8_t   seg_active_low;         /**< \brief ����ܼ���*/
    uint8_t   com_active_low;         /**< \brief ����ܹ����˼��� */

    uint16_t  blink_cnt_max;          /**< \brief �������˸���� */

    uint16_t       *p_disp_buf;       /**< \brief ������Դ� */
       const uint8_t  *p_disp_idx_tbl;   /**< \brief ����������� */

};

struct awbl_digitron_dev;

/**
 * \brief ͨ������ܲ����ӿ�
 */
struct awbl_digitron_common_ops {

    void (*pfn_digitron_disp_blink_set)(struct awbl_digitron_dev *p_dd, 
                                        int                       index,
                                        aw_bool_t                 blink);

    void (*pfn_digitron_disp_at)(struct awbl_digitron_dev *p_dd, 
                                 int                       index, 
                                 uint16_t                  seg);

    void (*pfn_digitron_disp_char_at)(struct awbl_digitron_dev *p_dd, 
                                      int                       index, 
                                      const char                ch);

    void (*pfn_digitron_disp_str)(struct awbl_digitron_dev *p_dd, 
                                  int                       index,
                                  int                       len, 
                                  const char               *p_str);

    void (*pfn_digitron_disp_clr)(struct awbl_digitron_dev *p_dd);

    /**
     * \brief �ú�������Ϩ���������ʾ.
     *
     * \note ʹ���봦����Ч״̬, ���Ǹı�buf�е�����, ��clr�������ڱ�������
     */
    void (*pfn_digitron_disp_seg_deactive_set)(struct awbl_digitron_dev *p_dd);

    void (*pfn_digitron_disp_enable)(struct awbl_digitron_dev *p_dd);

    void (*pfn_digitron_disp_disable)(struct awbl_digitron_dev *p_dd);

    uint16_t (*pfn_digitron_disp_decode)(uint16_t code);

};

/**
 * \brief ͨ��������豸
 */
struct awbl_digitron_dev {

    uint32_t  blink_flags;         /**< \brief ��˸���(bit map) */
    uint16_t  blink_cnt_max;       /**< \brief max count of blinking timer */
    uint16_t  blink_cnt;           /**< \brief blinking timer counter */
    uint8_t   blinking;            /**< \brief �Ƿ���˸ */

    struct awbl_digitron_disp_param *pdd_param;

    struct aw_list_head     list_elm;

    uint8_t  disp_dev_id;

    aw_bool_t   disp_enable;

    struct awbl_digitron_common_ops  digitron_ops;

};

/**
 * \brief Ϊ������豸���ý��뺯��
 *
 * \param[in] p_dd ͨ��������豸
 * \param[in] pfn  ���뺯��
 */
aw_local aw_inline void awbl_digitron_disp_decode_set (struct awbl_digitron_dev *p_dd,
                                                       uint16_t (*pfn) (uint16_t code))
{
    p_dd->digitron_ops.pfn_digitron_disp_decode = pfn;
}

void awbl_digitron_dev_ctor(struct awbl_digitron_dev         *p_dd,
                            struct awbl_digitron_disp_param  *p_par);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_DIGITRON_H */

/* end of file */
