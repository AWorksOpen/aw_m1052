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
 * \brief 基于GPIO驱动的矩阵键盘驱动(lite版本)
 *
 * \internal
 * \par modification history
 * - 1.00 17-06-26  nwt, first implementation
 * \endinternal
 */

#ifndef __AWBL_MATRIX_KEY_LITE_H
#define __AWBL_MATRIX_KEY_LITE_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "aw_gpio.h"
#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif


/** \brief max matrix keyboard rows and columns */
#define MATRIX_KEY_LITE_COLS         16
#define MATRIX_KEY_LITE_ROWS         16

/** \brief get matrix keyboard scan code */
#define MATRIX_KEY_LITE_SCAN_CODE(row, col, num_col) (((row) * (num_col)) + (col))

/******************************************************************************/

/**
 * \brief matrix keyboard platform parameters
 */
struct awbl_matrix_key_lite_par {
    const uint16_t  *col_gpios; /**< \brief col GPIO number table */
    const uint16_t  *row_gpios; /**< \brief row GPIO number table */
    const uint16_t  *keymap;    /**< \brief key map */

    uint8_t num_row;            /**< \brief number of keyboard rows */
    uint8_t num_col;            /**< \brief number of keyboard columns */
    uint8_t active_low;         /**< \brief gpio polarity */

    /** \brief last state of keys buf point,the buf num must equal the num_col */
    uint16_t *p_last_key_state;

    /** \brief current state of keys buf point,the buf num must equal the num_col */
    uint16_t *p_current_key_state;
};





/** \brief matrix keyboard structure */
struct awbl_matrix_key_lite_dev{

    /**
     * \brief 矩阵键盘信息
     */
    struct awbl_matrix_key_lite_par *p_par;

};

void awbl_matrix_key_dev_ctor(struct awbl_matrix_key_lite_dev  *p_this,
                              struct awbl_matrix_key_lite_par  *p_par);






#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MINIPORT_MATRIX_KEY_H */

/* end of file */
