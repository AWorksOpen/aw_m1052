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
 * \brief ���ڱ�׼GPIO�����ľ����������
 *
 * \par 1.������Ϣ
 *
 *  - ������:   "matrix-kp"
 *  - ��������: AWBL_BUSID_PLB
 *  - �豸��Ϣ: struct matrix_keypad_par
 *
 * \par 2.�����豸
 *
 *  - �κο���ʹ�ñ�׼GPIO�ӿڷ��ʵ�ͨ�þ������
 *
 * \par 3.�豸����/����
 *
 *  - \ref grp_awbl_drv_input_matrix_keypad_hwconf
 *
 * \par 4.�û��ӿ�
 *
 *  - \ref grp_aw_serv_input_key
 *
 * \internal
 * \par modification history
 * - 1.00 13-02-26  orz, first implementation.
 * \endinternal
 */

#ifndef __AWBL_MATRIX_KEYPAD_H
#define __AWBL_MATRIX_KEYPAD_H

#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_input.h"
#include "awbl_digitron.h"
#include "aw_types.h"
#include "aw_delayed_work.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_awbl_drv_input_matrix_keypad
 * \copydetails awbl_matrix_keypad.h
 * @{
 */

/**
 * \defgroup  grp_awbl_drv_input_matrix_keypad_hwconf �豸����/����
 *
 * ͨ�þ�������豸�Ķ���������Ĵ�����ʾ���û��ڶ���Ŀ��ϵͳ��Ӳ����Դʱ��
 * ����ֱ�ӽ���δ���Ƕ�뵽 awbus_lite_hwconf_usrcfg.c �ж�Ӧ��λ�ã�Ȼ����豸
 * ��Ϣ���б�Ҫ������(�����õ���Ŀ�á�x����ʶ)��
 *
 * \note �ж��ٸ���������豸���ͽ�����Ĵ���Ƕ����ٴΣ�ע�⽫�����޸�Ϊ��ͬ
 *       �����֡�
 *
 * \include  hwconf_usrcfg_matrix_keypad.c
 * \note �����������д����û����á���ʶ�ģ���ʾ�û��ڿ���Ӧ��ʱ�������ô˲�����
 *
 * - ��1��������̵���GPIO��ű� \n
 * ���������������������̵ġ��С���������GPIO�ı��
 *
 * - ��2��������̵���GPIO��ű� \n
 * ���������������������̵ġ��С���������GPIO�ı��
 *
 * - ��3��������̼�ֵӳ��� \n
 *      - ����������������ÿ��������Ӧ�ļ�ֵ����ֵ��ʹ���Զ����ֵ
 *        ����ASCII�룩�� aw_input_key.h ͷ�ļ��ж���ı�׼��ֵ���Ƽ�ʹ�ú���
 *      - ��Ӧ��ϵΪ�����ң����ϵ��¡���2x2�ļ��̣�0��0�ж�Ӧӳ����Ԫ��0��
 *        0��1�ж�Ӧӳ����Ԫ��1��1��0�ж�Ӧӳ����Ԫ��2����������
 *
 * - ��4���������ƽ̨��ص�һЩ��ʼ�� \n
 * ��Щ��ʼ����Ҫ�������������������������GPIO��������������ر���Ƶĵ�·��
 * �����ʼ���ȵȡ� \n
 * ��͵�ƽ��Ч�ļ�����Ҫ�����������裬�����Ӳ��������������裬����Ҫ����
 *
 * - ��5��������̵ġ��С��� \n
 * �����������GPIO��ű�1����Ԫ�ظ�����ͬ
 *
 * - ��6��������̵ġ��С��� \n
 * �����������GPIO��ű�2����Ԫ�ظ�����ͬ
 *
 * - ��7�������Ƿ�Ϊ�͵�ƽ��Ч \n
 * ����������Ϊ�͵�ƽ��Ч��������true��������false
 *
 * - ��8���ж�ģʽ��ȥ������ʱʱ�� \n
 * ���ʱ�����ڿ��жϵļ������������������жϺ���ʱ��ɨ����̣�һ��ȡ10~20����
 *
 * - ��9����ʱɨ����ʱ�䣬Ϊ0���ʾʹ���жϷ�ʽ����Ϊ0��ʹ�ö�ʱɨ�跽ʽ \n
 * ���ʱ�����ڲ����жϵļ��̣����ܲ����жϵļ���ʹ�ö�ʱɨ��ķ�ʽɨ�谴����
 * ���ʱ�伴Ϊ��ʱɨ��ļ��ʱ�䣬����Ϊ10~20����
 *
 * - ��10��GPIO�ġ��ء��жϺ� \n
 * ��ЩоƬ�Ķ��GPIO������ͬһ���жϺţ���ʹ�������ַ�ʽ�����������ǵĹ���
 * �жϺţ�������0
 *
 * - ��11���;�������豸���á��С�GPIO���������ʾ�豸��GPIO��Ч���� \n
 * ������������������豸���������ʾ�豸������GPIO������ָ������Ч����
 *
 * - ��12���;�������豸���á��С�GPIO���������ʾ�豸��GPIO��Ч���� \n
 * ������������������豸���������ʾ�豸������GPIO������ָ������Ч����
 *
 * - ��13���;�������豸����GPIO���������ʾ�豸���豸ʵ�� \n
 * ��û�и���GPIO������NULL
 */

/** @} */

/** @brief name of matrix keyboard driver */
#define MATRIX_KEYPAD_NAME      "matrix-kp"

/** @brief max matrix keyboard rows and columns */
#define MATRIX_MAX_COLS         16
#define MATRIX_MAX_ROWS         16

/** @brief get matrix keyboard scan code */
#define MATRIX_SCAN_CODE(row, col, num_col) (((row) * (num_col)) + (col))

/******************************************************************************/

/**
 * \brief matrix keyboard platform parameters
 * \note -  if scan_interval is not 0, the keypad driver will use a timer to
 *          scan the keypad in period, otherwise will use GPIO interrupt.
 *       - if <digitron> is set, the key pad is scanning in a digitron display
 *         device's scanning black time.
 */
struct matrix_keypad_par {
    void (*plat_init)(void);    /**< \brief platform initialization */

    const uint16_t  *row_gpios; /**< \brief row GPIO number table */
    const uint16_t  *col_gpios; /**< \brief column GPIO number table */
    const uint16_t  *keymap;    /**< \brief key map */

    uint8_t num_row;        /**< \brief number of keyboard rows */
    uint8_t num_col;        /**< \brief number of keyboard columns */
    uint8_t active_low;     /**< \brief gpio polarity */
    uint8_t debounce_ms;    /**< \brief key debounce interval in ms */
    uint8_t scan_interval;  /**< \brief keypad scanning interval in ms */
    uint8_t clustered_irq;  /**< \brief irq that all row/column GPIOs bundled */

    uint8_t dd_row_active_low; /**< \brief polarity of complex row GPIOs */
    uint8_t dd_col_active_low; /**< \brief polarity of complex column GPIOs */

    struct awbl_digitron_dev *digitron; /** \brief the digitron display device */
};

/** \brief get keyboard ops from device */
#define dev_get_matrix_keypad_par(p_dev)    ((void *)AWBL_DEVINFO_GET(p_dev))

/** \brief matrix keyboard structure */
struct matrix_keypad {
    struct awbl_dev        dev;     /**< \brief AWBus device, always go first */
    struct aw_delayed_work work;    /**< \brief timer for scan keypad */

    uint16_t dd_scan_count_max;     /**< \brief scan count maxim in digitron */
    uint16_t dd_scan_count;         /**< \brief scan counter in digitron */

    uint16_t last_key_state[MATRIX_MAX_COLS]; /**< \brief last state of keys */
};

/** \brief convert a awbl_dev pointer to matrix_keypad pointer */
#define dev_to_matrix_keypad(p_dev)     ((struct matrix_keypad *)(p_dev))

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_MATRIX_KEYPAD_H */

/* end of file */
