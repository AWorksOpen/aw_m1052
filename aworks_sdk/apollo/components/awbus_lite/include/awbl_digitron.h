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
 * \brief 通用数码管类定义
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
 * \brief 数码管矩阵配置信息
 */
struct awbl_digitron_disp_param {

    uint8_t   num_segment;            /**< \brief 数码管段数 */

    uint8_t   num_rows;               /**< \brief 数码管矩阵的行数 */
    uint8_t   num_cols;               /**< \brief 数码管矩阵的列数 */

    uint8_t   seg_active_low;         /**< \brief 数码管极性*/
    uint8_t   com_active_low;         /**< \brief 数码管公共端极性 */

    uint16_t  blink_cnt_max;          /**< \brief 数码管闪烁计数 */

    uint16_t       *p_disp_buf;       /**< \brief 数码管显存 */
       const uint8_t  *p_disp_idx_tbl;   /**< \brief 数码管索引表 */

};

struct awbl_digitron_dev;

/**
 * \brief 通用数码管操作接口
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
     * \brief 该函数用于熄灭数码管显示.
     *
     * \note 使段码处于无效状态, 并非改变buf中的内容, 与clr函数存在本质区别
     */
    void (*pfn_digitron_disp_seg_deactive_set)(struct awbl_digitron_dev *p_dd);

    void (*pfn_digitron_disp_enable)(struct awbl_digitron_dev *p_dd);

    void (*pfn_digitron_disp_disable)(struct awbl_digitron_dev *p_dd);

    uint16_t (*pfn_digitron_disp_decode)(uint16_t code);

};

/**
 * \brief 通用数码管设备
 */
struct awbl_digitron_dev {

    uint32_t  blink_flags;         /**< \brief 闪烁标记(bit map) */
    uint16_t  blink_cnt_max;       /**< \brief max count of blinking timer */
    uint16_t  blink_cnt;           /**< \brief blinking timer counter */
    uint8_t   blinking;            /**< \brief 是否闪烁 */

    struct awbl_digitron_disp_param *pdd_param;

    struct aw_list_head     list_elm;

    uint8_t  disp_dev_id;

    aw_bool_t   disp_enable;

    struct awbl_digitron_common_ops  digitron_ops;

};

/**
 * \brief 为数码管设备设置解码函数
 *
 * \param[in] p_dd 通用数码管设备
 * \param[in] pfn  解码函数
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
