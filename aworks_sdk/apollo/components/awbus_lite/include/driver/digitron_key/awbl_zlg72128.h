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
 * \brief ZLG72128 �ӿ�ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-11  pea, first implementation
 * \endinternal
 */

#ifndef __AWBL_ZLG72128_H
#define __AWBL_ZLG72128_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "aw_sem.h"
#include "aw_i2c.h"
#include "aw_pool.h"
#include "aw_input.h"
#include "awbus_lite.h"
#include "awbl_scanner.h"
#include "awbl_digitron.h"

#define AWBL_ZLG72128_NAME      "awbl_zlg72128"

/**
 * \brief ʵ��ʹ�õ����к궨��
 *
 * �豸��Ϣ�е� key_use_row_flags �� key_use_col_flags �������ʵ�����
 * ���ã�����ʵ��ʹ���˵� 0 �к͵� 3 �У��� key_use_row_flags ��ֵӦ������Ϊ
 * AWBL_ZLG72128_KEY_ROW_0 | AWBL_ZLG72128_KEY_ROW_3
 */
#define AWBL_ZLG72128_KEY_ROW_0  (1 << 0) /**< \brief ʹ�õ� 0 �� */
#define AWBL_ZLG72128_KEY_ROW_1  (1 << 1) /**< \brief ʹ�õ� 1 �� */
#define AWBL_ZLG72128_KEY_ROW_2  (1 << 2) /**< \brief ʹ�õ� 2 �� */
#define AWBL_ZLG72128_KEY_ROW_3  (1 << 3) /**< \brief ʹ�õ� 3 �У����ܰ����У�*/

#define AWBL_ZLG72128_KEY_COL_0  (1 << 0) /**< \brief ʹ�õ� 0 �� */
#define AWBL_ZLG72128_KEY_COL_1  (1 << 1) /**< \brief ʹ�õ� 1 �� */
#define AWBL_ZLG72128_KEY_COL_2  (1 << 2) /**< \brief ʹ�õ� 2 �� */
#define AWBL_ZLG72128_KEY_COL_3  (1 << 3) /**< \brief ʹ�õ� 3 �� */
#define AWBL_ZLG72128_KEY_COL_4  (1 << 4) /**< \brief ʹ�õ� 4 �� */
#define AWBL_ZLG72128_KEY_COL_5  (1 << 5) /**< \brief ʹ�õ� 5 �� */
#define AWBL_ZLG72128_KEY_COL_6  (1 << 6) /**< \brief ʹ�õ� 6 �� */
#define AWBL_ZLG72128_KEY_COL_7  (1 << 7) /**< \brief ʹ�õ� 7 �� */

/**
 * \brief ��ͨ�������� 1 �� ~ �� 3 �����а�������ֵ�궨��
 *
 * �оٳ��� awbl_zlg72128_key_cb_t �ص������е� key_val ���ܵ�һϵ��ֵ������
 * ʹ�� key_val ֵ����һϵ�к�ֵ����Ӧ�Աȣ����ж�����һ�������¡��������������
 * AWBL_ZLG72128_KEY_x_y�����У�x ��ʾ��һ�У�1 ~ 3����y ��ʾ��һ�У�1 ~ 8)
 */
#define  AWBL_ZLG72128_KEY_1_1     1     /**< \brief �� 1 �ŵ� 1 ������ */
#define  AWBL_ZLG72128_KEY_1_2     2     /**< \brief �� 1 �ŵ� 2 ������ */
#define  AWBL_ZLG72128_KEY_1_3     3     /**< \brief �� 1 �ŵ� 3 ������ */
#define  AWBL_ZLG72128_KEY_1_4     4     /**< \brief �� 1 �ŵ� 4 ������ */
#define  AWBL_ZLG72128_KEY_1_5     5     /**< \brief �� 1 �ŵ� 5 ������ */
#define  AWBL_ZLG72128_KEY_1_6     6     /**< \brief �� 1 �ŵ� 6 ������ */
#define  AWBL_ZLG72128_KEY_1_7     7     /**< \brief �� 1 �ŵ� 7 ������ */
#define  AWBL_ZLG72128_KEY_1_8     8     /**< \brief �� 1 �ŵ� 8 ������ */

#define  AWBL_ZLG72128_KEY_2_1     9     /**< \brief �� 2 �ŵ� 1 ������ */
#define  AWBL_ZLG72128_KEY_2_2     10    /**< \brief �� 2 �ŵ� 2 ������ */
#define  AWBL_ZLG72128_KEY_2_3     11    /**< \brief �� 2 �ŵ� 3 ������ */
#define  AWBL_ZLG72128_KEY_2_4     12    /**< \brief �� 2 �ŵ� 4 ������ */
#define  AWBL_ZLG72128_KEY_2_5     13    /**< \brief �� 2 �ŵ� 5 ������ */
#define  AWBL_ZLG72128_KEY_2_6     14    /**< \brief �� 2 �ŵ� 6 ������ */
#define  AWBL_ZLG72128_KEY_2_7     15    /**< \brief �� 2 �ŵ� 7 ������ */
#define  AWBL_ZLG72128_KEY_2_8     16    /**< \brief �� 2 �ŵ� 8 ������ */

#define  AWBL_ZLG72128_KEY_3_1     17    /**< \brief �� 3 �ŵ� 1 ������ */
#define  AWBL_ZLG72128_KEY_3_2     18    /**< \brief �� 3 �ŵ� 2 ������ */
#define  AWBL_ZLG72128_KEY_3_3     19    /**< \brief �� 3 �ŵ� 3 ������ */
#define  AWBL_ZLG72128_KEY_3_4     20    /**< \brief �� 3 �ŵ� 4 ������ */
#define  AWBL_ZLG72128_KEY_3_5     21    /**< \brief �� 3 �ŵ� 5 ������ */
#define  AWBL_ZLG72128_KEY_3_6     22    /**< \brief �� 3 �ŵ� 6 ������ */
#define  AWBL_ZLG72128_KEY_3_7     23    /**< \brief �� 3 �ŵ� 7 ������ */
#define  AWBL_ZLG72128_KEY_3_8     24    /**< \brief �� 3 �ŵ� 8 ������ */

/**
 * \brief ���ܰ������� 4 �����а�������ź궨��
 *
 * ���ܰ����ӵ� 4 �п�ʼ���������ң����ζ���Ϊ F0 ~ F7����״̬�ֱ��밴���ص�����
 * �е� funkey_val ֵ�� bit0 ~ bit7 ��Ӧ��0 Ϊ���£�1 Ϊ�ͷš����� funkey_val �� 8 ��
 * ���ܼ������ֵ������ֱ��ʹ����� 8 λ��ֵʵ�ֽ�Ϊ���ӵİ������ơ���ֻ��Ҫ��
 * �ļ������ĳ�����ܼ���״̬������ʹ�� #AWBL_ZLG72128_FUNKEY_CHECK() ����ĳһ��
 * �ܼ���״̬��#AWBL_ZLG72128_FUNKEY_CHECK() ��� funkey_num ����Ϊ 0 ~ 7������ʹ��
 * �˴�����ĺ�ֵ��#AWBL_ZLG72128_FUNKEY_0 ~ #AWBL_ZLG72128_FUNKEY_7
 */
#define  AWBL_ZLG72128_FUNKEY_0     0     /**< \brief ���ܼ� 1 */
#define  AWBL_ZLG72128_FUNKEY_1     1     /**< \brief ���ܼ� 2 */
#define  AWBL_ZLG72128_FUNKEY_2     2     /**< \brief ���ܼ� 3 */
#define  AWBL_ZLG72128_FUNKEY_3     3     /**< \brief ���ܼ� 4 */
#define  AWBL_ZLG72128_FUNKEY_4     4     /**< \brief ���ܼ� 4 */
#define  AWBL_ZLG72128_FUNKEY_5     5     /**< \brief ���ܼ� 5 */
#define  AWBL_ZLG72128_FUNKEY_6     6     /**< \brief ���ܼ� 6 */
#define  AWBL_ZLG72128_FUNKEY_7     7     /**< \brief ���ܼ� 7 */

/**
 * \brief �������ʾ��λ�ķ���
 *
 * ���� awbl_zlg72128_digitron_shift() ������ dir ������
 */
#define  AWBL_ZLG72128_DIGITRON_SHIFT_RIGHT   0   /**< \brief ���� */
#define  AWBL_ZLG72128_DIGITRON_SHIFT_LEFT    1   /**< \brief ���� */

/**
 * \brief ���ܰ������� 4 �����а�����״̬����
 *
 * ���ܰ����ӵ� 4 �п�ʼ���������ң����ζ���Ϊ F0 ~ F7����״̬�ֱ��밴���ص�����
 * �е� funkey_val ֵ�� bit0 ~ bit7 ��Ӧ��0 Ϊ���£�1 Ϊ�ͷš����� funkey_val �� 8 ��
 * ���ܼ������ֵ������ֱ��ʹ����� 8 λ��ֵʵ�ֽ�Ϊ���ӵİ������ơ���ֻ��Ҫ��
 * �ļ������ĳ�����ܼ���״̬������ʹ�øú���ĳһ���ܼ���״̬��
 *
 * \param[in] funkey_val �����ص������л�ȡ���Ĺ��ܼ���ֵ
 * \param[in] funkey_num ���ܼ���ţ���Чֵ��0 ~ 7���ֱ��Ӧ F0 ~ F7������ʹ��
 *                       ��ֵ #AWBL_ZLG72128_FUNKEY_0 ~ #AWBL_ZLG72128_FUNKEY_7
 *
 * \retval AW_TRUE  ��Ӧ���ܼ���ǰ�ǰ���״̬
 * \retval AW_FALSE ��Ӧ���ܼ���ǰ���ͷ�״̬
 */
#define  AWBL_ZLG72128_FUNKEY_CHECK(funkey_val, funkey_num)  \
               (((funkey_val) & (1 << ((funkey_num) & 0x07))) ? AW_FALSE : AW_TRUE)

/**
 * \brief ����ܶ�
 *
 * ���� awbl_zlg72128_digitron_seg_ctrl() �ο��ƺ����� seg ������
 */
#define  AWBL_ZLG72128_DIGITRON_SEG_A    0        /**< \brief a  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_B    1        /**< \brief b  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_C    2        /**< \brief c  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_D    3        /**< \brief d  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_E    4        /**< \brief e  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_F    5        /**< \brief f  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_G    6        /**< \brief g  �� */
#define  AWBL_ZLG72128_DIGITRON_SEG_DP   7        /**< \brief dp �� */

/**
 * \brief ���尴���ص���������
 *
 * ����ʹ�� awbl_zlg72128_key_cb_set() �������øûص�������ϵͳ֮�У����а���
 * �¼�����ʱ�����ûص�����
 *
 * \param[in] p_arg      ���ûص�����ʱ�趨���û��Զ������
 *
 * \param[in] key_val    ��ֵ���ӵ� 1 �п�ʼ����������˳���š��� 1 �е� 1 ������
 *                       ��ֵΪ 1���� 1 �е� 2 ��������ֵΪ 2���Դ����ơ�����ʹ��
 *                       ��ֱֵ���� AWBL_ZLG72128_KEY_*_*(#AWBL_ZLG72128_KEY_1_1)
 *                       �Ƚϣ����ж�����һ�������¡�����أ�0 ��ʾ���κ���ͨ
 *                       ��������
 *
 * \param[in] repeat_cnt ����������������ͨ����֧����������ֵ�����ڸ�������
 *                       key_val0 ��Ӧ����ͨ������������������ funkey_val ����
 *                       �Ĺ��ܼ�û�й�ϵ��0 ��ʾ�״ΰ�����δ����������ֵ��
 *                       ʾʶ�𵽵�������������������סʱ�䳬�� 2s �󣬿�ʼ��
 *                       ����Ч��������Ч��ʱ����Լ 200ms����ÿ�� 200ms��
 *                       repeat_cnt �� 1��������һ�θú�����ֱ�������ͷš���ֵ
 *                       ���Ϊ 255���������������� 255 ���򽫽�����������׶Σ�
 *                       ����û��ʱ���������ûص�����
 *
 * \param[in] funkey_val ���ܼ���ֵ������ 8 λ���ֱ���� 4 �и���������Ӧ�����λ
 *                       ��� 4 �е� 1 ��������Ӧ���θ�λ��� 4 �е� 2 ��������Ӧ����
 *                       ��λ��� 4 �е� 8 ��������Ӧ��λֵΪ 0 ������Ӧ�����£�λ
 *                       ֵΪ 1 ������Ӧ��δ���¡�ֻҪ��ֵ�����ı䣬�ͻ��������
 *                       �Ļص�����������ʹ�ú꣺#AWBL_ZLG72128_FUNKEY_CHECK() ��
 *                       �ж϶�Ӧ�Ĺ��ܼ��Ƿ���
 *
 * \note ���ڹ��ܼ�����ͨ����������ʵ����ϼ�Ӧ�ã����� PC ���ϵ� Alt��Shift �Ȱ�������
 *       �����õ���һ�����ܣ��༭����ʱ��TAB �������ƣ������ס Shift ������ TAB ��
 *       ��Ϊ���ơ����Ժ�����ʵ�����ƵĹ��ܣ��繦�ܼ� 0 ������ Shift ������ͨ���� 0 ����
 *       TAB �����ڻص������еĴ���ʾ�������磺
 *
 * \code
 *  // ����Ӧʹ�� awbl_zlg72128_key_cb_set() �������ú���ע���ϵͳ���Ա��ں��ʵ�ʱ
 *  // ����øûص�����
 *  static void zlg72128_key_callback (void    *p_arg,
 *                                     uint8_t  key_val,
 *                                     uint8_t  repeat_cnt,
 *                                     uint8_t  funkey_val)
 *
 *  {
 *       if (keyval == AWBL_ZLG72128_KEY_1_1) {
 *
 *           // ���ܼ����£�����
 *           if (AWBL_ZLG72128_FUNKEY_CHECK(fun_keyval, AWBL_ZLG72128_FUNKEY_0)) {
 *
 *           } else {  // ���ܼ�δ���£���������
 *
 *           }
 *      }
 *  }
 * \endcode
 */
typedef void (*awbl_zlg72128_key_cb_t) (void    *p_arg,
                                        uint8_t  key_val,
                                        uint8_t  repeat_cnt,
                                        uint8_t  funkey_val);

/**
 * \brief ZLG72128 ��Ϣ
 */
typedef struct awbl_zlg72128_msg {
    aw_pool_id_t      pool_id;   /**< \brief �ڴ�ؾ�� */
    aw_i2c_message_t  i2c_msg;   /**< \brief I2C ��Ϣ */
    aw_i2c_transfer_t trans[2];  /**< \brief I2C ���� */
    uint8_t           buf[13];   /**< \brief ������ */
} awbl_zlg72128_msg_t;

/**
 * \brief ZLG72128 �豸��Ϣ�ṹ��
 */
typedef struct awbl_zlg72128_devinfo {

    /** \brief �������ʾ�� ID */
    int                             digitron_disp_id;

    /** \brief ZLG72128 ��λ���ţ���ʹ��ʱ���磬�̶�Ϊ�ߵ�ƽʱ������Ϊ -1 */
    int                             rst_pin;

    /** \brief ZLG72128 �ж����ţ���ʹ��ʱ������Ϊ -1��ʹ����ѯ��ʽ��ȡ����״̬ */
    int                             int_pin;

    /** \brief ���� int_pin Ϊ -1 ʱ���ò�����Ч��ָ������״̬��ȡ��� */
    uint16_t                        interval_ms;

    /** \brief ʵ��ʹ�õ�����ܸ��� */
    uint8_t                         digitron_num;

    /** \brief һ����˸�����ڣ�������ʱ�䣬�� 500ms */
    uint16_t                        blink_on_time;

    /** \brief һ����˸�����ڣ�Ϩ���ʱ�䣬�� 500ms */
    uint16_t                        blink_off_time;

    /** \brief ʵ��ʹ�õ��б�־ */
    uint8_t                         key_use_row_flags;

    /** \brief ʵ��ʹ�õ��б�־ */
    uint8_t                         key_use_col_flags;

    /** \brief �������룬 ���СΪ key_rows * key_cols */
    const int                      *p_key_codes;

} awbl_zlg72128_devinfo_t;

/**
 * \brief ZLG72128 �豸�ṹ��
 */
typedef struct awbl_zlg72128_dev {

    /** \brief �̳��� AWBus �豸 */
    struct awbl_dev                      dev;

    /** \brief ������豸 */
    struct awbl_digitron_dev             digitron_dev;

    /** \brief ����ܾ���������Ϣ */
    struct awbl_digitron_disp_param      disp_param;

    /** \brief ���ܼ�ֵ�������ܼ�ת��Ϊͨ�ð���ʹ�� */
    volatile uint8_t                     f_key;

    /** \brief ʵ��ʹ�õİ������� */
    uint8_t                              num_rows;

    /** \brief ʵ��ʹ�ð��������� */
    uint8_t                              num_cols;

    /** \brief I2C �ӻ��豸 */
    aw_i2c_device_t                      i2c_dev;

    /** \brief ���� I2C ���ݴ��� */
    aw_i2c_transfer_t                    trans[2];

    /** \brief ��Ϣ���ڲ�ʹ�� */
    aw_i2c_message_t                     msg;

    /** \brief ״̬ */
    volatile uint8_t                     state;

    /** \brief ������ͨ���ص����� */
    awbl_zlg72128_key_cb_t               pfn_key_cb;

    /** \brief ����ص��������� */
    void                                *pfn_key_arg;

    /** \brief ��ֵ��ص� 4 ���Ĵ��� */
    uint8_t                              key_buf[4];

    /** \brief ɨ�����豸 */
    struct awbl_scanner_dev              scan_dev;

    /** \brief ɨ�������ò��� */
    struct awbl_scanner_param            scan_param;

    /** \brief ɨ�����ص�������� */
    struct awbl_scan_cb_handler          scan_cb_handler;

    /** \brief �ڴ�ض��� */
    aw_pool_t                            pool;

    /** \brief �ڴ�ؾ�� */
    aw_pool_id_t                         pool_id;

    /** \brief �ڴ�ؾ�� */
    awbl_zlg72128_msg_t                  bulk[10];

} awbl_zlg72128_dev_t;

/**
 * \brief ZLG72128 �������
 */
typedef awbl_zlg72128_dev_t *awbl_zlg72128_handle_t;

/**
 * \brief �����ص�����ע��
 *
 * ���а����¼�����ʱ�������ô˴����õĻص������������¼��������� 3 �������
 *    1. ����ͨ��������
 *    2. ��ͨ����һֱ���£���������״̬
 *    3. ��һ���ܰ���״̬�����仯���ͷ�->���¡�����->�ͷ�
 *
 * \param[in] id         �豸��
 * \param[in] pfn_key_cb �����ص�����
 * \param[in] p_arg      �ص������Ĳ���
 *
 * \retval  AW_OK     ���ûص������ɹ�
 * \retval -AW_EINVAL ��������
 */
aw_err_t awbl_zlg72128_key_cb_set (int                     id,
                                   awbl_zlg72128_key_cb_t  pfn_key_cb,
                                   void                   *p_arg);

/**
 * \brief �����������˸ʱ�䣬�����趨ĳλ��˸ʱ����������ʱ���Ϩ�������ʱ��
 *
 *     ��������Ϊ���������ʱ���Ϊ500ms��ʱ�䵥λΪms����Ч��ʱ��Ϊ��
 *  150��200��250��������800��850��900���� 150ms ~ 900ms������50msΪ��ࡣ
 *  ��ʱ�䲻��ǡ��Ϊ��Щֵ���������һ����ӽ���ֵ��
 *
 * \param[in] id     �豸��
 * \param[in] on_ms  ����������ʱ�䣬��ЧֵΪ 150ms ~ 900ms������50msΪ���
 * \param[in] off_ms Ϩ�������ʱ�䣬��ЧֵΪ 150ms ~ 900ms������50msΪ���
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 *
 * \note ��������˸ʱ�䲢��������������˸���������ĳλ����˸���غ��ܿ���
 * ��˸����awbl_zlg72128_digitron_flash_ctrl()
 */
aw_err_t awbl_zlg72128_digitron_flash_time_cfg (int      id,
                                                uint16_t on_ms,
                                                uint16_t off_ms);

/**
 * \brief ���ƣ��򿪻�رգ��������˸��Ĭ�Ͼ�����˸
 *
 * \param[in] id       �豸��
 * \param[in] ctrl_val ����ֵ��λ0 ~ 11�ֱ��Ӧ�������ҵ�����ܡ�ֵΪ1ʱ��˸��
 *                     ֵΪ0ʱ����˸��
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_flash_ctrl (int id, uint16_t ctrl_val);

/**
 * \brief ��������ܵ���ʾ���ԣ�����ʾ��ر���ʾ��
 *
 *     Ĭ������£���������ܾ����ڴ���ʾ״̬���򽫰���12λ�����ɨ�裬ʵ���У�
 *  ������Ҫ��ʾ��λ��������12λ����ʱ������ʹ�øú����ر�ĳЩλ����ʾ��
 *
 * \param[in] id       �豸��
 * \param[in] ctrl_val ����ֵ��λ0 ~ λ11�ֱ��Ӧ�������ҵ�����ܡ�
 *                     ֵΪ1�ر���ʾ��ֵΪ0����ʾ
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 *
 * \note �������ö���ΪϨ����벻ͬ�����ö���ΪϨ�����ʱ�������ͬ����ɨ�裬ͬ
 * ����Ҫ��ÿ��ɨ��ʱ�����Ӧ�Ķ��롣ʹ�øú����ر�ĳЩλ����ʾ���ǴӸ����ϲ�
 * ��ɨ���λ��
 */
aw_err_t awbl_zlg72128_digitron_disp_ctrl (int id, uint16_t ctrl_val);

/**
 * \brief ��������ܵ�ɨ��λ��
 *
 *     ��ʹ�ù����У��������Ҫ 12 λ�������ʾ�������λ��ʼ�ü���ͬʱ������ɨ��λ
 * �����ó���Ӧ�������λ����
 *
 * \param[in] id       �豸��
 * \param[in] ctrl_val ����ֵ��λ0 ~ λ11�ֱ��Ӧ�������ҵ�����ܡ�
 *                     ֵΪ1�ر���ʾ��ֵΪ0����ʾ
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_scan_ctrl (int id, uint8_t ctrl_val);

/**
 * \brief ֱ�������������ʾ�����ݣ����룩
 *
 *      ���û���Ҫ��ʾһЩ�Զ��������ͼ��ʱ������ʹ�øú�����ֱ�����ö��롣һ
 *  ��������ZLG72128 �Ѿ��ṩ�˳�����10�����ֺ�21����ĸ��ֱ����ʾ���û�����ʹ��
 *  �˺���ֱ�����ö��룬����ֱ��ʹ����غ�����ʾ���ֻ���ĸ
 *      Ϊ����������ö�������λ��ʾ�Ķ��룬����������һ������д���������
 *  ��ʾ�Ķ��롣
 *
 * \param[in] id        �豸��
 * \param[in] start_pos �������ö�����ʼλ�ã���Чֵ 0 ~ 11
 * \param[in] p_buf     �����ŵĻ�����
 * \param[in] num       �������õ�����ܶ���ĸ���
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 *
 * \note �� start_pos + num ��ֵ���� 12 �������Ļ��������ݱ�������
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_set (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num);

/**
 * \brief ��ȡ�������ʾ�����ݣ����룩
 *
 * \param[in] id        �豸��
 * \param[in] start_pos �������ö�����ʼλ�ã���Чֵ 0 ~ 11
 * \param[in] p_buf     �����ŵĻ�����
 * \param[in] num       �������õ�����ܶ���ĸ���
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_get (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num);

/**
 * \brief �����������ʾ���ַ�
 *
 *      ZLG72128�Ѿ��ṩ�˳�����10�����ֺ�21����ĸ��ֱ����ʾ������Ҫ��ʾ��֧�ֵ�
 *  ͼ�Σ�����ֱ��ʹ�� awbl_zlg72128_digitron_dispbuf_set() ����ֱ�����ö��롣
 *
 * \param[in] id         �豸��
 * \param[in] pos        ������ʾ��λ�ã���Чֵ 0 ~ 11
 * \param[in] ch         ��ʾ���ַ���֧���ַ� 0123456789AbCdEFGHiJLopqrtUychT
 *                       �Լ��ո�����ʾ��
 * \param[in] is_dp_disp �Ƿ���ʾС���㣬TRUE:��ʾ; AW_FALSE:����ʾ
 * \param[in] is_flash   ��λ�Ƿ���˸��TRUE:��˸; AW_FALSE:����˸
 *
 * \retval  AW_OK      ���óɹ�
 * \retval -AW_EINVAL  ��Ч����
 * \retval -AW_ENOTSUP ��֧�ֵ��ַ�
 * \retval -AW_EIO     ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_disp_char (int        id,
                                           uint8_t    pos,
                                           char       ch,
                                           aw_bool_t  is_dp_disp,
                                           aw_bool_t  is_flash);

/**
 * \brief ��ָ��λ�ÿ�ʼ��ʾһ���ַ���
 *
 * \param[in] id          �豸��
 * \param[in] start_pos   �ַ�����ʼλ��
 * \param[in] p_str       ��ʾ���ַ���
 * \param[in] flash_flags �ַ�����Ӧ���ַ��Ƿ���˸���� 0 λ�����ַ����еĵ�
 *                        һ���ַ��Ƿ���˸������ 1 λ�����ַ����еĵڶ�����
 *                        ���Ƿ���˸���Դ�����
 *
 * \retval  AW_OK      ���óɹ�
 * \retval -AW_EINVAL  ��Ч����
 * \retval -AW_ENOTSUP ��֧�ֵ��ַ�
 * \retval -AW_EIO     ����ʧ�ܣ����ݴ������
 *
 * \note ��������֧�ֵ��ַ������ַ�λ������ʾ
 */
aw_err_t awbl_zlg72128_digitron_disp_str (int         id,
                                          uint8_t     start_pos,
                                          const char *p_str,
                                          uint8_t     flash_flags);
/**
 * \brief �����������ʾ�����֣�0 ~ 9��
 *
 * \param[in] id         �豸��
 * \param[in] pos        ������ʾ��λ�ã���Чֵ 0 ~ 11
 * \param[in] num        ��ʾ�����֣�0 ~ 9
 * \param[in] is_dp_disp �Ƿ���ʾС���㣬TRUE:��ʾ; AW_FALSE:����ʾ
 * \param[in] is_flash   ��λ�Ƿ���˸��TRUE:��˸; AW_FALSE:����˸
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_disp_num (int        id,
                                          uint8_t    pos,
                                          uint8_t    num,
                                          aw_bool_t  is_dp_disp,
                                          aw_bool_t  is_flash);

/**
 * \brief ֱ�ӿ��ƶεĵ�����Ϩ��
 *
 *     Ϊ�˸������Ŀ����������ʾ���ṩ��ֱ�ӿ���ĳһ�ε����ԣ������𣩡�
 *
 * \param[in] id    �豸��
 * \param[in] pos   ���ƶ�������λ����Чֵ 0 ~ 11
 * \param[in] seg   �Σ���Чֵ 0 ~ 7���ֱ��Ӧ��a,b,c,d,e,f,g,dp
 *                  ����ʹ�����к꣺
 *                    - AWBL_ZLG72128_DIGITRON_SEG_A
 *                    - AWBL_ZLG72128_DIGITRON_SEG_B
 *                    - AWBL_ZLG72128_DIGITRON_SEG_C
 *                    - AWBL_ZLG72128_DIGITRON_SEG_D
 *                    - AWBL_ZLG72128_DIGITRON_SEG_E
 *                    - AWBL_ZLG72128_DIGITRON_SEG_F
 *                    - AWBL_ZLG72128_DIGITRON_SEG_G
 *                    - AWBL_ZLG72128_DIGITRON_SEG_DP
 * \param[in] is_on �Ƿ�����öΣ�TRUE:����; AW_FALSE:Ϩ��
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_seg_ctrl (int        id,
                                          uint8_t    pos,
                                          char       seg,
                                          aw_bool_t  is_on);

/**
 * \brief ��ʾ��λ����
 *
 *      ʹ��ǰ��������ܵ���ʾ���ƻ����ƣ�����ָ���Ƿ���ѭ���ƶ����������ѭ��
 *   ��λ������λ���ұ߿ճ���λ�����ƣ�����߿ճ���λ�����ƣ�������ʾ�κ����ݡ�
 *   ����ѭ���ƶ�����ճ���λ������ʾ���Ƴ�λ�����ݡ�
 *
 * \param[in] id        �豸��
 * \param[in] dir       ��λ����
 *                        - AWBL_ZLG72128_DIGITRON_SHIFT_LEFT   ����
 *                        - AWBL_ZLG72128_DIGITRON_SHIFT_RIGHT  ����
 *
 * \param[in] is_cyclic �Ƿ���ѭ����λ��TRUE��ѭ����λ��FALSE����ѭ����λ
 * \param[in] num       ������λ��λ������Чֵ 0�����ƶ��� ~ 11������ֵ��Ч
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 *
 * \note ������λ��˵��
 *
 *   ʵ���У����ܻᷢ����λ�����෴���������ڿ��ܴ�������Ӳ����ƣ�
 *   1�� ���ұ�Ϊ0������ܣ���������Ϊ��11��10��9��8��7��6��5��4��3��2�� 1�� 0
 *   2�� �����Ϊ0������ܣ���������Ϊ�� 0�� 1��2��3��4��5��6��7��8��9��10��11
 *   ����Ҫȡ����Ӳ�����ʱ COM0 ~ COM11 ��������Ӧ���������������λ�á�
 *
 *       �˴����ƺ����Ƶĸ������ԡ������ֲᡷ�е���Ӧ�õ�·Ϊ�ο��ģ������ұ�
 *   Ϊ0������ܡ���ô���ƺ����Ƶĸ���ֱ�Ϊ��
 *   ���ƣ������0��ʾ�л���1�������1��ʾ�л���2�������������10��ʾ�л���11
 *   ���ƣ������11��ʾ�л���10�������1��ʾ�л���2�������������10��ʾ�л���11
 *
 *   �ɼ�����Ӳ����·��������λ�����෴�ģ�����λЧ������Ҳǡǡ���෴�ģ��˴�
 * ֻ��Ҫ��΢ע�⼴�ɡ�
 */
aw_err_t awbl_zlg72128_digitron_shift (int        id,
                                       uint8_t    dir,
                                       aw_bool_t  is_cyclic,
                                       uint8_t    num);

/**
 * \brief ��λ���������ܵ����� LED ��Ϩ��
 *
 * \param[in] id �豸��
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_disp_reset (int id);

/**
 * \brief ����������� LED ���� 0.5S ��������˸�����ڲ����������ʾ�Ƿ�����
 *
 * \param[in] id �豸��
 *
 * \retval  AW_OK     ���óɹ�
 * \retval -AW_EINVAL ��Ч����
 * \retval -AW_EIO    ����ʧ�ܣ����ݴ������
 */
aw_err_t awbl_zlg72128_digitron_disp_test (int id);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_ZLG72128_H */

/* end of file */
