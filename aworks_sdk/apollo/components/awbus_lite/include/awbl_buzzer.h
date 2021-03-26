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
 * \brief Buzzer driver.
 *
 * \internal
 * \par modification history
 * - 1.01 14-11-01  ops, redefine the abstract buzzer by using new specification.
 * - 1.00 13-03-08  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_BUZZER_H
#define __AWBL_BUZZER_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup awbl_buzzer
 * @{
 */

/** \brief buzzer device */
struct awbl_buzzer_dev {

    /**
     * \brief 打开或关闭蜂鸣器鸣叫
     *
     * \param[in] on ： 为TRUE时，打开蜂鸣器；为FALSE时，关闭蜂鸣器
     * 
     * \return 无
     */
    void (*beep) (struct awbl_buzzer_dev *p_dev, aw_bool_t on);

    /**
     * \brief 响度设置函数
     *
     * 若不支持，可以将该值设置为NULL
     *
     * \param[in] p_dev : 蜂鸣器设备
     * \param[in] level : 响度，0 ~ 100，响度为0时表示静音，100时响度最大
     *
     * \retval AW_OK        设置成功
     * \retval -AW_ENOTSUP  设备不支持该操作
     */
    aw_err_t (*loud_set) (struct awbl_buzzer_dev *p_dev, int level);
};

/**
 * \brief 注册蜂鸣器设备
 *
 * \param[in] p_dev : 蜂鸣器设备
 *
 * \return 无
 *
 * \note 一个系统中只能注册一个蜂鸣器
 */
void awbl_buzzer_register_device (struct awbl_buzzer_dev *p_dev);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_BUZZER_H */

/* end of file */
