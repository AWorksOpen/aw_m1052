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
 * \brief ͨ���������ʵ��
 *
 * \internal
 * \par modification history:
 * - 1.01 15-03-17  ops, redefine the aw_digitron_disp_str.
 * - 1.00 14-11-12  ops, first implementation.
 * \endinternal
 */

#include "awbl_digitron.h"
#include "aw_compiler.h"
#include "aw_gpio.h"
#include "aw_assert.h"
#include "aw_vdebug.h"

#include "string.h"

/**
 * \brief ����ܽ��뺯��.
 *
 * \param p_dd        ͨ������ܶ���.
 * \param ascii_char  �������ascii_char.
 *
 * \return ���ؽ����Ķ�����, �粻�ܽ���, ��0x00.
 */
aw_local uint16_t __digitron_decode (struct awbl_digitron_dev *p_dd, 
                                     uint16_t                  code)
{

    if (NULL != p_dd->digitron_ops.pfn_digitron_disp_decode) {

        return p_dd->digitron_ops.pfn_digitron_disp_decode(code);
    }

    return code;
}

/**
 * \brief ����������Դ�.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param index  �Դ�����.
 * \param data   ������������.
 */
aw_local void __digitron_disp_buf_set (struct awbl_digitron_dev *p_dd,
                                       int                       index,
                                       uint16_t                  data)
{
    if (p_dd == NULL) {

        return;
    }

    if (index < (p_dd->pdd_param->num_cols * p_dd->pdd_param->num_rows)) {

        if (p_dd->pdd_param->seg_active_low) {

            data ^= 0xFFFFu;
        }
        p_dd->pdd_param->p_disp_buf[index] = data;
    }
}

/**
 * \brief ���Դ��е����ݽ���'OR'����.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param index  �Դ�����.
 * \param data   ������������.
 *
 */
aw_local void __digitron_disp_buf_xor (struct awbl_digitron_dev *p_dd,
                                       int                      index,
                                       uint16_t                 data)
{
    if (p_dd == NULL) {

        return;
    }

    if (index < (p_dd->pdd_param->num_cols * p_dd->pdd_param->num_rows)) {

        if (p_dd->pdd_param->seg_active_low) {

            data ^= 0xFFFFu;
            p_dd->pdd_param->p_disp_buf[index] &= data;

        } else {

            p_dd->pdd_param->p_disp_buf[index] |= data;
        }
    }
}

/*******************************************************************************
  ͨ������ܲ����ӿ�ʵ��
*******************************************************************************/

/**
 * \brief �����������˸.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param index  �Դ�����.
 * \param blink  ��˸(AW_TRUE) or ����(AW_FALSE).
 */
aw_local void __digitron_disp_blink_set (struct awbl_digitron_dev *p_dd, 
                                         int                       index,
                                         aw_bool_t                 blink)
{
    if (p_dd == NULL) {

        return;
    }

    AW_BIT_MODIFY(p_dd->blink_flags, index, blink);
}

/**
 * \brief ���������Դ�.
 *
 * \param p_dd   ͨ������ܶ���.
 */
aw_local void __digitron_disp_clr (struct awbl_digitron_dev *p_dd)
{
    int i = 0;

    if (p_dd == NULL) {

        return;
    }

    for (i = 0; i < (p_dd->pdd_param->num_cols * p_dd->pdd_param->num_rows); i++) {

        __digitron_disp_buf_set(p_dd, i, 0);
    }
}

/**
 * \brief ֱ����������Դ�д������.
 * \param p_dd   ͨ������ܶ���.
 * \param index  �Դ�����.
 * \param seg    ��д���Դ�Ķ�����.
 */
aw_local void __digitron_disp_at (struct awbl_digitron_dev *p_dd, 
                                  int                       index, 
                                  uint16_t                  seg)
{
    if (p_dd == NULL) {

        return;
    }

    __digitron_disp_buf_set(p_dd, index, seg);
}

/**
 * \brief ��ָ���������, ��ʾһ��ASCII�ַ�.
 * \param p_dd   ͨ������ܶ���.
 * \param index  �Դ�����.
 * \param ch     ����ʾASCII char.
 */
aw_local void __digitron_disp_char_at (struct awbl_digitron_dev *p_dd, 
                                       int                       index,
                                       const char                ch)
{
    uint16_t seg = __digitron_decode(p_dd, ch);

    if (p_dd == NULL) {

        return;
    }

    if ('.' == ch) {

        __digitron_disp_buf_xor(p_dd, index, seg);

    } else {

        __digitron_disp_buf_set(p_dd, index, seg);
    }
}

/**
 * \brief ���ö���Ϊ��Ч״̬.
 *
 * \param p_dd   ͨ������ܶ���.
 */
void __digitron_disp_set_seg_deactive (struct awbl_digitron_dev *p_dd)
{

	uint16_t data = 0;

    int i = 0;

    if (p_dd == NULL) {

    	return;
    }

    p_dd->pdd_param->seg_active_low ? (data = 0xFF) : (data = 0x00);

    for (i = 0; i < (p_dd->pdd_param->num_cols * p_dd->pdd_param->num_rows); i++) {

    	p_dd->pdd_param->p_disp_buf[i] = data;
    }

}

/**
 * \brief ���������ʾ�豸����ʾ ASCII String.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param p_str  ����ʾ��ASCII String.
 */
aw_local void __digitron_disp_str (struct awbl_digitron_dev *p_dd, 
                                   int                       index,
                                   int                       len,
                                   const char               *p_str)
{
    uint16_t last_ch = 0, ch;
    int idx = index - 1;
    uint8_t str_len = 0, i = 0;

    if (p_str == NULL) {

        return;
    }

    str_len = strlen(p_str);

    if (len < str_len) {

        i = len;

        while(i) {

            ch = *p_str++;

            if (('.' == ch) && ('.' != last_ch)) {

                __digitron_disp_char_at(p_dd, (idx < 0) ? ++idx : idx, ch);

            }else {

                __digitron_disp_char_at(p_dd, ++idx, ch);

                i--;
            }
            last_ch = ch;
        }
    }
    else {

        while ((ch = *p_str++) != '\0') {

            if (('.' == ch) && ('.' != last_ch)) {

                __digitron_disp_char_at(p_dd, (idx < 0) ? ++idx : idx, ch);

            }else {

                __digitron_disp_char_at(p_dd, ++idx, ch);

            }

            last_ch = ch;
        }
    }
}

/**
 * \brief ���������ʾ�豸����ʾ ASCII String.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param p_str  ����ʾ��ASCII String.
 */
aw_local void __digitron_disp_enable (struct awbl_digitron_dev *p_dd)
{
    p_dd->disp_enable = AW_TRUE;
}

/**
 * \brief ���������ʾ�豸����ʾ ASCII String.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param p_str  ����ʾ��ASCII String.
 */
aw_local void __digitron_disp_disable (struct awbl_digitron_dev *p_dd)
{
    p_dd->disp_enable = AW_FALSE;
}

/******************************************************************************/

/**
 * \brief ͨ������ܹ��캯��.
 *
 * \param p_dd   ͨ������ܶ���.
 * \param p_par  ��ʼ������.
 */
void awbl_digitron_dev_ctor(struct awbl_digitron_dev         *p_dd,
                            struct awbl_digitron_disp_param  *p_par)
{

    if (p_dd == NULL || p_par == NULL) {

        return;
    }

    p_dd->blink_cnt_max = p_par->blink_cnt_max;
    p_dd->blink_flags   = 0;
    p_dd->blink_cnt     = 0;
    p_dd->blinking      = AW_FALSE;

    p_dd->disp_enable = AW_TRUE;
    p_dd->pdd_param = p_par;

    p_dd->digitron_ops.pfn_digitron_disp_blink_set        = __digitron_disp_blink_set;
    p_dd->digitron_ops.pfn_digitron_disp_at               = __digitron_disp_at;
    p_dd->digitron_ops.pfn_digitron_disp_char_at          = __digitron_disp_char_at;
    p_dd->digitron_ops.pfn_digitron_disp_clr              = __digitron_disp_clr;
    p_dd->digitron_ops.pfn_digitron_disp_str              = __digitron_disp_str;
    p_dd->digitron_ops.pfn_digitron_disp_seg_deactive_set = __digitron_disp_set_seg_deactive;
    p_dd->digitron_ops.pfn_digitron_disp_enable           = __digitron_disp_enable;
    p_dd->digitron_ops.pfn_digitron_disp_disable          = __digitron_disp_disable;
    p_dd->digitron_ops.pfn_digitron_disp_decode           = NULL;
}

/* end of file */
