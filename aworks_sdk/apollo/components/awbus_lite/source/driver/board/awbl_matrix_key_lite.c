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


#include "driver/board/awbl_matrix_key_lite.h"

/******************************************************************************/

void awbl_matrix_key_dev_ctor(struct awbl_matrix_key_lite_dev  *p_this,
                              struct awbl_matrix_key_lite_par  *p_par)
{
    int i = 0;

    p_this->p_par       = p_par;

    if (p_par->col_gpios != NULL) {
        /*
         * 列输出配置引脚
         */
        for (i = 0; i < p_par->num_col; i++) {
            aw_gpio_pin_cfg(p_par->col_gpios[i], AW_GPIO_OUTPUT);
        }
    }

    /* 行输入配置引脚 */
    for (i = 0; i < p_par->num_row; i++) {
        aw_gpio_pin_cfg(p_par->row_gpios[i], AW_GPIO_INPUT | AW_GPIO_PULL_DOWN);
    }

}
/* end of file */
