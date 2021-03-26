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
 * - 1.01 15-03-17  ops, redefine the aw_digitron_disp_str.
 * - 1.00 14-11-22  ops, first implementation.
 * \endinternal
 */

/**
 * \addtogroup aw_digitron
 * @{
 */
#include "awbl_digitron_if.h"
#include "aw_vdebug.h"

aw_local struct aw_list_head __g_digitron_dev_list_head;

aw_local struct awbl_digitron_dev* __digitron_dev_list_iterator(int id)
{
    struct awbl_digitron_dev *p_cur = NULL;
    struct aw_list_head *p_pos = NULL;

    if (aw_list_empty(&__g_digitron_dev_list_head)) {

        AW_INFOF(("The digitron display device list is empty. \n"));
        return NULL;
    }

    aw_list_for_each(p_pos, &__g_digitron_dev_list_head) {


        p_cur = aw_list_entry(p_pos, struct awbl_digitron_dev, list_elm);

        if (p_cur->disp_dev_id == id) {

            return p_cur;
        }
    }

    return NULL;
}

void awbl_digitron_dev_list_init (void)
{
    aw_list_head_init(&__g_digitron_dev_list_head);
}

void awbl_digitron_dev_list_add (struct awbl_digitron_dev *p_dd)
{
    aw_list_add_tail(&p_dd->list_elm, &__g_digitron_dev_list_head);
}

void awbl_digitron_dev_list_del (struct awbl_digitron_dev *p_dd)
{
    aw_list_del (&p_dd->list_elm);
}

/**
 * \brief 数码管组注册函数, 将该数码管组注册进数码管子系统.
 */
void awbl_digitron_disp_register (struct awbl_digitron_dev        *p_dd,
                                  struct awbl_digitron_disp_param *p_par)
{
    awbl_digitron_dev_ctor(p_dd, p_par);

    p_dd->digitron_ops.pfn_digitron_disp_clr(p_dd);

    awbl_digitron_dev_list_add(p_dd);
}

/**
 * \brief 数码管组注销函数, 将该组数码管从数码管子系统中移除.
 */
void awbl_digitron_disp_unregister (struct awbl_digitron_dev  *p_dd)
{

    awbl_digitron_dev_list_del(p_dd);
}

/******************************************************************************/

aw_err_t aw_digitron_disp_decode_set (int id, uint16_t (*pfn_decode)(uint16_t code))
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        awbl_digitron_disp_decode_set(p_dd, pfn_decode);

        return AW_OK;
    }

    return -AW_ENODEV;
}

/******************************************************************************/

aw_err_t aw_digitron_disp_blink_set (int id, int index, aw_bool_t blink)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        if ((index >= 0) &&
            (index < (p_dd->pdd_param->num_rows * p_dd->pdd_param->num_cols))) {

            p_dd->digitron_ops.pfn_digitron_disp_blink_set(p_dd, index, blink);

            return AW_OK;
        }
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t aw_digitron_disp_at (int id, int index, uint16_t seg)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        if ((index >= 0) &&
            (index < (p_dd->pdd_param->num_rows * p_dd->pdd_param->num_cols))) {

            p_dd->digitron_ops.pfn_digitron_disp_at(p_dd, index, seg);

            return AW_OK;
        }
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t aw_digitron_disp_char_at (int id, int index, const char ch)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        if ((index >= 0) &&
            (index < (p_dd->pdd_param->num_rows * p_dd->pdd_param->num_cols))) {

            p_dd->digitron_ops.pfn_digitron_disp_char_at(p_dd, index, ch);

            return AW_OK;
        }
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t aw_digitron_disp_str (int id, int index, int len, const char *p_str)
{

    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        p_dd->digitron_ops.pfn_digitron_disp_str(p_dd, index, len, p_str);

        return AW_OK;
    }

    return -AW_ENODEV;
}

/******************************************************************************/
aw_err_t aw_digitron_disp_clr (int id)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        p_dd->digitron_ops.pfn_digitron_disp_clr(p_dd);

        return AW_OK;
    }

    return -AW_ENODEV;
}

aw_err_t aw_digitron_disp_enable (int id)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        p_dd->digitron_ops.pfn_digitron_disp_enable(p_dd);

        return AW_OK;
    }

    return -AW_ENODEV;
}

aw_err_t aw_digitron_disp_disable (int id)
{
    struct awbl_digitron_dev *p_dd = NULL;

    if ((p_dd = __digitron_dev_list_iterator(id)) != NULL) {

        p_dd->digitron_ops.pfn_digitron_disp_disable(p_dd);

        return AW_OK;
    }

    return -AW_ENODEV;
}

/** @} */

/* end of file */
