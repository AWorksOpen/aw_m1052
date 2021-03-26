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
 * \brief FLASH translation layer��
 *        �ò����Ҫ�����Ƕ�flash�豸�ṩĥ����⡢��ַӳ�䡢�������ȹ��ܣ�
 *        ʹ�øò�Ķ�д�ӿ�������Ҫ����flash�Ĳ���������������
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-4-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_H
#define __AW_FTL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_ftl
 * \copydoc aw_ftl.h
 * @{
 */

#include "aworks.h"

/**
 * \brief IO���Ƶ�ѡ��
 */
typedef enum aw_ftl_ioctl_req {

    /** \brief ����ϵͳ��Ϣ�ĸ���ʱ�䣬��λms, Ĭ��Ϊ0���ر��Զ����£� */
    AW_FTL_SYS_INFO_UPDATE_TIME,

    /** \brief ������д����������ʱ��������ϵͳ��Ϣ���µ���ʱʱ�� ��Ĭ��Ϊ�ر� */
    AW_FTL_SYS_INFO_TARENEW,

    /** \brief ��ȡ�豸������ */
    AW_FTL_GET_DEV_NAME,

    /** \brief ��ȡ�豸�������� */
    AW_FTL_GET_SECTOR_COUNT,

    /** \brief ��ȡ�豸������С  */
    AW_FTL_GET_SECTOR_SIZE,

    /** \brief ��ȡ�豸�洢����С  */
    AW_FTL_GET_STORAGE_SIZE,

    /**
     * \brief �����߼�������ķ�ֵ��
     * ���߼���������������ﵽ�÷�ֵʱ�� ��ϲ����߼������ݡ�
     * ȡֵ�� 0~100 ����ٷ�ֵ��Ϊ0 ��һ������������������Ϊ
     * 100�򲻻��Զ����� Ĭ��Ϊ100
     */
    AW_FTL_LOGIC_THRESHOLD,

    /**
     * \brief ��ֻ��־������ķ�ֵ��
     * ��������־�������ʹ�������ﵽ�÷�ֵʱ�� ��ϲ�������־�����ݡ�
     * ȡֵ�� 0~100 ����ٷ�ֵ��Ϊ0 ��һ������������������  Ϊ100��
     * �����Զ����� Ĭ��Ϊ85
     */
    AW_FTL_LOG_THRESHOLD,

    /** \brief ɾ���߼���������   */
    AW_FTL_SECTOR_DAT_DEL,

    /** \brief ��ʽ��FTL�豸   */
    AW_FTL_DEV_FORMAT,

} aw_ftl_ioctl_req_t;


/**
 * \brief ����һ��FTL�豸����Ը��豸���г�ʼ��
 *
 * \param[in] p_ftl_name : Ҫ������FTL�豸����
 * \param[in] p_mtd_name : Ҫʹ�õ�MTD�豸����
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_dev_create (const char *p_ftl_name,
                            const char *p_mtd_name);

/**
 * \brief ����һ��FTL�豸
 *
 * \param[in] p_ftl_name : FTL�豸��
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_dev_destroy (const char *p_ftl_name);

/**
 * \brief ��ȡһ��FTL�豸
 *
 * \param[in] p_ftl_name : Ҫ������FTL�豸����
 *
 * \return ��ʼ���ɹ�:���� ftl�豸ָ�룬���򷵻�NULL
 */
void * aw_ftl_dev_get (const char *p_ftl_name);

/**
 * \brief �ͷ�һ��FTL�豸
 *
 * \param[in] p_hd : FTL�豸�ṹ��ָ��
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_dev_put (void *p_hd);

/**
 * \brief ��ȡָ��λ�õ�����
 *
 * \param[in]  p_hd         : FTL�豸�ṹ��ָ��
 * \param[in]  start_addr   : ��ȡ���ݵ���ʼ��ַ
 * \param[out] data         : ������ݵ�buf
 * \param[in]  dsize        : ������ݵ�buf�Ŀռ��С
 *
 * \retval > 0              : ���������ݳ���
 * \retval -AW_EINVAL       : ������Ч
 * \retval -AW_ENODATA      : û������
 * \retval -AW_ENOMEM       : ����������������Ч��Χ
 * \retval -AW_EIO          : IO����
 */
aw_err_t aw_ftl_data_read  (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize);

/**
 * \brief ��ָ��λ����д������
 *
 * \param[in]  p_hd         : FTL�豸�ṹ��ָ��
 * \param[in]  start_addr   : ��ȡ���ݵ���ʼ��ַ
 * \param[in]  data         : ����buf
 * \param[in]  dsize        : ����buf�Ŀռ��С
 *
 * \retval > 0              : д������ݳ���
 * \retval -AW_EINVAL       : ������Ч
 * \retval -AW_ENODATA      : û������
 * \retval -AW_ENOMEM       : ����������������Ч��Χ
 * \retval -AW_EIO          : IO����
 */
aw_err_t aw_ftl_data_write (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize);

/**
 * \brief �߼�������ȡ
 *
 * \param[in]  p_hd   : FTL�豸�ṹ��ָ��
 * \param[in]  lsn    : �߼��������
 * \param[out] data   : ������ݵ�buf��ע���buf����С��һ�������Ŀռ�
 *
 * \retval AW_OK        :  �ɹ�
 * \retval -AW_EINVAL   :  ������Ч
 * \retval -AW_ENODATA  :  û������
 * \retval -AW_ENOMEM   :  ����������������Ч��Χ
 * \retval -AW_EIO      :  IO����
 */
aw_err_t aw_ftl_sector_read (void              *p_hd,
                             uint32_t           lsn,
                             uint8_t           *data);

/**
 * \brief �߼�����д��
 *
 * \param[in] p_hd   : FTL�豸�ṹ��ָ��
 * \param[in] lsn    : �߼��������
 * \param[in] data   : Ҫд������ݣ� ע���buf����С��һ�������Ŀռ�
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_sector_write (void              *p_hd,
                              uint32_t           lsn,
                              uint8_t           *data);

/**
 * \brief ��ftl�豸ע��ɿ��豸
 *
 * \param[in] p_ftl_path  : ftl�豸����ע�������豸Ҳʹ�ø���
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_blk_register (const char *p_ftl_path);

/**
 * \brief ��ע��ftlע��ɵĿ��豸
 *
 * \param[in] p_ftl_path  : ftl�豸����ע�������豸Ҳʹ�ø���
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 */
aw_err_t aw_ftl_blk_unregister (const char *p_ftl_path);

/**
 * \brief io����
 *
 * \param[in]     p_hd    : FTL�豸�ṹ��
 * \param[in]     req     : io����
 * \param[in,out] p_arg   : ����
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t��������
 *
 * \par ����
 * \code
 *
 *  uint32_t arg = 0;
 *  char     name[AW_BD_NAME_MAX];
 *
 *  //����ϵͳ��Ϣ�ı�����Ϊ60000ms, ʱ��Ϊ0���Զ�����
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SYS_INFO_UPDATE_TIME, (void *)60000);
 *
 *  //����ϵͳ��Ϣ�ı�����Ϊ60000ms, ʱ��Ϊ0���Զ�����
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SYS_INFO_TARENEW, (void *)TURE);
 *
 *  //��ȡ�豸������
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_DEV_NAME, (void *)name);
 *
 *  //��ȡ�豸��������С
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_SECTOR_SIZE, (void *)&arg);
 *
 *  //��ȡ�豸����������
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_GET_SECTOR_COUNT, (void *)&arg);
 *
 *  //���������߼���ķ�ֵ�����߼����dirty�����ﵽ���߼�������������79%ʱ��
 *  //����������
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_LOGIC_THRESHOLD, (void *)79);
 *
 *  //����������־��ķ�ֵ����������־���ʹ�������ﵽ����־������������87%ʱ��
 *  //����������
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_LOG_THRESHOLD, (void *)87);
 *
 *  //ɾ���߼����� 200 ������
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_SECTOR_DAT_DEL, (void *)200);
 *
 *  // �첽��ʽ�� ftl
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_DEV_FORMAT, NULL);
 *  // ͬ����ʽ��
 *  aw_ftl_dev_ioctl(p_ftl, AW_FTL_DEV_FORMAT, (void *)1);
 *
 * \endcode
 */
aw_err_t aw_ftl_dev_ioctl (void                *p_hd,
                           aw_ftl_ioctl_req_t   req,
                           void                *p_arg);

/**
 * \brief FTL������������սӿ�
 */
void aw_ftl_garbage_collect (void);


/** @}  grp_aw_if_ftl */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AW_FTL_H */

/* end of file */
