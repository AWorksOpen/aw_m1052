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
 * \brief 抽象数码管接口管理层
 *
 * \internal
 * \par modification history
 * - 1.00 14-11-22  ops, first implementation.
 * \endinternal
 */

#ifndef AWBL_DIGITRON_IF_H_
#define AWBL_DIGITRON_IF_H_

#include "awbl_digitron.h"

#ifdef __cplusplus
extern "C" {
#endif

void awbl_digitron_dev_list_init (void);
void awbl_digitron_dev_list_add  (struct awbl_digitron_dev *p_dd);
void awbl_digitron_dev_list_del  (struct awbl_digitron_dev *p_dd);

void awbl_digitron_disp_register   (struct awbl_digitron_dev        *p_dd, 
                                    struct awbl_digitron_disp_param *p_par);
void awbl_digitron_disp_unregister (struct awbl_digitron_dev  *p_dd);


#ifdef __cplusplus
}
#endif


#endif /* AWBL_DIGITRON_IF_H_ */
