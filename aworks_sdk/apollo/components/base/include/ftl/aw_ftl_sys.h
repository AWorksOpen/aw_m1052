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
 * \brief system information
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_SYS_H
#define __AW_FTL_SYS_H

/**
 * \addtogroup grp_aw_if_ftl_sys
 * \copydoc aw_ftl_sys.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef struct aw_ftl_sys_info {
    /** \brief ϵͳ��Ϣ������ */
    uint16_t *array;

    /** \brief ÿ��ϵͳ��Ϣ�����Ч�� */
    aw_bool_t  *blk_valid;

    /** \brief ϵͳ��Ϣ���µļ��ʱ��(ms),Ϊ0�򲻻��Զ����� */
    uint32_t update_time;

    /** \brief ���ϵͳ��Ϣ��������������ʹ�ü��� */
    uint16_t sector_use_cnt;

    /** \brief ���һ��ϵͳ��Ϣ������Ҫ���������� */
    uint16_t need_sectors;

    /** \brief ��Ҫ��ϵͳ��Ϣ����������Ǵ����ݿ����ȡ */
    uint8_t  nsys_blk;

    /** \brief ���ϵͳ��Ϣ�������ŵ���Ч�� */
    aw_bool_t   pbn_valid;

    /** \brief ϵͳ��Ϣ��Ҫ���� */
    aw_bool_t   need_update;

    /** \brief ϵͳ��Ϣ���ڵȴ����� */
    aw_bool_t   waiting_update;

    /** \brief ��д��������ʱ������ϵͳ��Ϣ�ĵȴ�ʱ�� */
    aw_bool_t   time_auto_renew;

}aw_ftl_sys_info_t;

struct aw_ftl_dev;

/**
 * \brief ��ʼ��
 *
 * \param[in] p_ftl           : FTL�豸�ṹ��
 *
 * \retval  AW_OK  : �����ɹ�
 * \retval  -ENOMEM: �ڴ治��
 */
aw_err_t aw_ftl_sys_info_init (struct aw_ftl_dev *p_ftl);
aw_err_t aw_ftl_sys_info_deinit (struct aw_ftl_dev *p_ftl);

/** \brief ��ϵͳ������Ϣ�洢��flash�� */
int aw_ftl_sys_info_update (struct aw_ftl_dev *p_ftl);

/**
 * \brief ��flahs������ϵͳ������Ϣ
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 *
 * \retval  AW_OK  : �����ɹ�
 */
int aw_ftl_sys_info_download (struct aw_ftl_dev *p_ftl);

int aw_ftl_sys_info_clear (struct aw_ftl_dev *p_ftl);




/** @}  grp_aw_if_ftl_sys */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_SYS_H */



/* end of file */
