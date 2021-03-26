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
 * \brief ����ɨ�����ඨ��
 *
 *    ����ɨ������scan_interval��ʱ��ִ��һ��ɨ����Ϊ, ��һ��ɨ����Ϊ��, 
 *    ɨ��scan_cnt��, ÿ��ɨ��, ���ε��ûص�����������������ע��Ļص�����.
 *
 *    ÿ��ɨ���������ڲ���ά��һ���ص�������������, ʹ����ͨ��
 *    awbl_scan_cb_list_add/awbl_scan_cb_list_del������ɾ.
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-04  ops, first implementation.
 * \endinternal
 */
#ifndef AWBL_SCANNER_H_
#define AWBL_SCANNER_H_

#include "aw_delayed_work.h"
#include "aw_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ɨ�����ص�����ԭ��
 */
typedef void scan_cb_func_t (void *p_arg);


/**
 * \brief ɨ�����ص��������
 */
struct awbl_scan_cb_handler {

    scan_cb_func_t      *pfn_scan_cb;

    struct aw_list_head  list_elm;
};

/**
 * \brief ɨ�������ò���
 */
struct awbl_scanner_param {

    /**
     * \brief ɨ����
     */
    uint16_t  scan_interval;

    /**
     * \brief ��һ��ɨ��������, ɨ��Ĵ���
     */
    uint16_t  scan_cnt;
};

/**
 * \brief ɨ�����豸
 */
struct awbl_scanner_dev {

    /**
     * \brief ɨ���������ò���
     */
    struct awbl_scanner_param *p_par;

    /**
     * \brief ��ʱ��ҵ
     */
    struct aw_delayed_work  work;

    /**
     * \brief ɨ������
     */
    uint8_t   scan_index;

    /**
     * \brief ɨ��ʹ�ܱ��, ʹ��: AW_TRUE, ����: AW_FALSE
     */
    aw_bool_t    scan_enable;

    /**
     * \brief ��ͣʱ��(ms)
     */
    uint16_t  pause_time;

    /**
     * \brief �ص���������ͷ
     */
    struct aw_list_head  cb_list_head;

    /**
     * \brief ����ɨ��
     *
     * \param  p_scan  ɨ��������.
     */
    void (*pfn_scanner_start)(struct awbl_scanner_dev *p_scan);

    /**
     * \brief ��ͣɨ��(��ָͣ��ʱ����, ������ԭɨ����ɨ��)
     *
     * \param  p_scan      ɨ��������.
     * \param  pause_time  ��ͣʱ��
     */
    void (*pfn_scanner_pause)(struct awbl_scanner_dev *p_scan, 
                              uint16_t                 pause_time);

    /**
     * \brief ֹͣɨ��
     *
     * \param  p_scan  ɨ��������.
     */
    void (*pfn_scanner_stop)(struct awbl_scanner_dev *p_scan);

};

aw_local aw_inline void awbl_scan_cb_list_add(
    struct awbl_scanner_dev     *p_scan,
    struct awbl_scan_cb_handler *p_cb_handler)
{
    aw_list_add_tail(&p_cb_handler->list_elm, &p_scan->cb_list_head);
}

aw_local aw_inline void awbl_scan_cb_list_del(
    struct awbl_scan_cb_handler *p_cb_handler)
{
    aw_list_del(&p_cb_handler->list_elm);
}

void awbl_scanner_start(struct awbl_scanner_dev *p_scan);

void awbl_scanner_pause(struct awbl_scanner_dev *p_scan, uint16_t pause_time);

void awbl_scanner_stop(struct awbl_scanner_dev *p_scan);

void awbl_scanner_dev_ctor(struct awbl_scanner_dev   *p_scan, 
                           struct awbl_scanner_param *p_par);

#ifdef __cplusplus
}
#endif

#endif /* AWBL_HC595_SCANNER_H_ */

/* end of file */
