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
 * \brief ������ts��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ��������ͷ�ļ�:
 * \code
 * #include "aw_ts.h"
 * \endcode
 *
 * \note
 *
 *  (1) �������豸�����Ĵ����������޹�, ��ΪGUI�ṩͨ�õĴ�������, ��ֱ����
 *      ��GUI�Ĵ���ϵͳ.
 *
 *      ����, ʹ�� AWorks �������豸�滻emwin�Ĵ���ϵͳ
 *
 *      GUI_PID_STATE        th;             // emwinָ�������豸״̬�ṹ��
 *      struct aw_ts_state   ts_state;       // apollo����״̬�ṹ��
 *
 *      int ts_id = 0;
 *
 *      // �ýӿڱ������ȵ��ã���ȡ���������id
 *      ts_id = aw_ts_serv_id_get("480x272", 0x12345678, 0x12345678);
 *
 *      AW_FOREVER {
 *
 *          aw_ts_exec(ts_id, &ts_state, 1);  // ��ȡһ�������������(���㴥��)
 *
 *          th.x = ts_state.x;
 *          th.y = ts_state.y;
 *          th.Pressed = ts_state.pressed;
 *
 *          GUI_TOUCH_StoreStateEx(&th);      // �ѽ���洢��emwin
 *
 *          aw_mdelay(10);
 *      }
 *
 *  (2) ֱ�ӿ���Touch Screen Controller�ı�׼�ӿ�
 *      aw_ts_lock(aw_bool_t lock)  �� ���������� (AW_TRUE: ����, AW_FALSE: ����)
 *
 *
 * \internal
 * \par modification history
 * - 1.01 17-02-08  anu, add MultiTouch.
 * - 1.01 14-09-22  ops, add the implementation of abstract ts device.
 * - 1.00 14-07-25  ops, first implementation.
 * \endinternal
 */
#ifndef __AW_TS_H
#define __AW_TS_H

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \addtogroup grp_aw_if_ts
 * \copydoc aw_ts.h
 * @{
 */

#include "aworks.h"
#include "aw_ts_lib.h"

/** \brief ����У׼�㷨���ͣ�����У׼  */
#define AW_TS_LIB_TWO_POINT    0

/** \brief ����У׼�㷨���ͣ����У׼  */
#define AW_TS_LIB_FIVE_POINT   1


/** \brief ����X��Y���������� */
#define AW_TS_SWAP_XY         (1 << 2)

/** \brief ���֧�ֶ��ٵ�Ĵ��� */
#define AW_MULTI_TS_CNT        5

#define AW_TS_DEFAULT_NAME   "default_tc"


/** \brief  ������ָ�����. */
typedef  void *aw_ts_id;

/**
 * \brief ������״̬
 */
struct aw_ts_state {
    int32_t x;         /**< \brief  x����ֵ */
    int32_t y;         /**< \brief  y����ֵ */
    int32_t slot;      /**< \brief  �ڵ�ǰ�����д���ڼ���������  */
    uint8_t pressed;   /**< \brief  �Ƿ���, 1 : ����, 0 : δ���� */
};

/**
 * \brief ��ȡ���������id
 *
 * \param[in]  name     ��ǰ�������������
 * \param[in]  vendor   ��ǰ�������������ʾ���ĳ���id(û��ʹ��)
 *
 * \param[in]  product  ��ǰ�������������ʾ���Ĳ�Ʒid(û��ʹ��)
 *
 * \return ���������id
 *
 * \note ʹ�ô��������ṩ�Ľӿڣ��ýӿڱ������ȱ�����
 *
 * \Remarks change by CYX at 2019.1.4
 */
aw_ts_id aw_ts_serv_id_get (char *name, int vendor, int product);

/**
 * \brief ����У׼
 *
 * \param[in]  ts_id      :  ��������id
 * \param[in]  p_calc_data:  ��ȡ�õ������ݣ�����У׼
 *
 * \return  AW_OK         : У׼�ɹ�
 *         -AW_EINVAL     : ��������
 *         -AW_ENOTSUP    : ��֧�ֵ�У���㷨
 *
 */
int aw_ts_calibrate (aw_ts_id ts_id, aw_ts_lib_calibration_t *p_calc_data);

/**
 * \brief ���津��У׼��Ϣ(��)
 * \param[in]  ts_id      : ��������id
 *
 *
 * \return  AW_OK         : У׼��Ϣ����ɹ�
 *         -AW_EINVAL     : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_ENOTSUP    : ��֧��У���㷨
 *         -AW_EIO        : д�봥��У׼��Ϣʧ��
 */
int aw_ts_calc_data_write (aw_ts_id ts_id);

/**
 * \brief ���津��У׼��Ϣ(�Ƽ�ʹ��)
 * \param[in]  ts_id      : ��������id
 *
 *
 * \return  AW_OK         : У׼��Ϣ����ɹ�
 *         -AW_EINVAL     : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_ENOTSUP    : ��֧��У���㷨
 *         -AW_EIO        : д�봥��У׼��Ϣʧ��
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_data_save (aw_ts_id ts_id);


/**
 * \brief ��ȡ����У׼��Ϣ(��)
 * \param[in] ts_id      : ��������id
 *
 * \return  AW_OK        : У׼��Ϣ��ȡ�ɹ�
 *         -AW_EINVAL    : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_EIO       : д�봥��У׼��Ϣʧ��
 */
int aw_ts_calc_data_read (aw_ts_id ts_id);

/**
 * \brief ���ش���У׼��Ϣ(�Ƽ�ʹ��)
 * \param[in] ts_id      : ��������id
 *
 * \return  AW_OK        : У׼��Ϣ��ȡ�ɹ�
 *         -AW_EINVAL    : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_EIO       : д�봥��У׼��Ϣʧ��
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_data_load (aw_ts_id ts_id);


/**
 * \brief ���ô��������᷽��
 *
 * \param[in] ts_id : ��������id
 * \param[in] flags : ��ʾ���
 *            AW_TS_LIB_SWAP_XY : ����������
 *
 * \return ��
 *
 * \note   ���������᷽��Ĭ��Ϊ������ʾ
 */
void aw_ts_set_orientation (aw_ts_id ts_id, int flags);

/**
 * \brief ��ȡ���������᷽��
 *
 * \param[in] ts_id : ��������id
 *
 * \return AW_TS_LIB_SWAP_XY : ����������
 *
 * \note   ���������᷽��Ĭ��Ϊ������ʾ
 *
 * \Remarks add by CYX at 2019.1.4
 */
int aw_ts_get_orientation (aw_ts_id ts_id);


/**
 * \brief ��������������ֹ����
 * \param[in] ts_id: ��������id
 * \param[in] lock : AW_TRUE : ���� / AW_FALSE: ����
 *
 * \return ��
 */
void aw_ts_lock (aw_ts_id ts_id, aw_bool_t lock);

/**
 * \brief �������Ƿ��а���
 *
 * \param[in] ts_id: ��������id
 *
 * \return -AW_EINVAL: ��������
 *          0        : ������û�а���;
 *          1        : ����������
 */
int aw_ts_is_touch (aw_ts_id ts_id);

/**
 * \brief �������Ƿ���ҪУ׼(�Ƽ�ʹ��)
 *
 * \param[in] ts_id: ��������id
 *
 * \return -AW_EINVAL: ��������
 *          0        : ����ҪУ׼;
 *          1        : ��ҪУ׼;
 *
 * \Remarks change by CYX at 2019.1.4
 */
int aw_ts_cal_attribute_get (aw_ts_id ts_id);

/**
 * \brief �������Ƿ���ҪУ׼(��)
 *
 * \param[in] ts_id: ��������id
 *
 * \return -AW_EINVAL: ��������
 *          0        : ����ҪУ׼;
 *          1        : ��ҪУ׼;
 */
int aw_ts_calc_flag_get (aw_ts_id ts_id);


/**
 * \brief ���´���״̬��Ϣ�����д����㰴�£����ش����������ֵ
 *
 * \param[in] ts_id    : ��������id
 * \param[in] ts_point : ����״̬������ָ��
 * \param[in] max_slots: ����״̬�������ĸ�������ֵ������Ҫ��ȡ���ٸ�����������
 *
 * \return -AW_EINVAL  : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_ENOTSUP : ��ǰ��������֧�ֻ�ȡmax_slots����㴥��������
 *         others      : ��ǰ����ʵ�ʰ��µĴ�������
 *
 * \note �ýӿ���Ҫ���ڵ���, �Դ�����������ѯ, �Ա㲻�ϸ��´�����״̬
 *       �û����봫�봥��״̬�������ĸ��������max_slots
 */
int aw_ts_exec (aw_ts_id ts_id, struct aw_ts_state *ts_point, int max_slots);

/**
 * \brief ���ذ��´��������˲�֮��Ĳ���ֵ
 *
 * \param[in] ts_id     : ��������id
 * \param[in] ts_sample : ����״̬������ָ��
 * \param[in] max_slots : ����״̬�������ĸ�������ֵ������Ҫ��ȡ���ٸ�����������
 *
 * \return -AW_EINVAL   : ��������δ�ҵ���Ӧ�Ĵ�������
 *         -AW_ENOTSUP  : ��ǰ��������֧�ֻ�ȡmax_slots����㴥��������
 *         others       : ��ǰ����ʵ�ʰ��µĴ�������
 */
int aw_ts_get_phys (aw_ts_id ts_id, struct aw_ts_state *ts_sample, int max_slots);


/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_TS_H */

/* end of file */
