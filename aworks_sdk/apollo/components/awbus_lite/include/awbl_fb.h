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
 * \brief AWBus FB �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_fb.h"
 * \endcode
 *
 * �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 19-05-24  cat, first implementation
 * \endinternal
 */
 
#ifndef __AWBL_FB_H
#define __AWBL_FB_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_fb.h"

struct awbl_fb_info;

/**
 * \brief ֡�����豸�����ӿ�
 */
typedef struct awbl_fb_ops {

    /** \brief ��ʼ��֡�����豸 */
    aw_err_t (*fb_init)             (struct awbl_fb_info *p_fb);

    /** \brief ����֡�����豸 */
    aw_err_t (*fb_start)            (struct awbl_fb_info *p_fb);

    /** \brief ֹͣ֡�����豸 */
    aw_err_t (*fb_stop)             (struct awbl_fb_info *p_fb);

    /** \brief ����֡�����豸 */
    aw_err_t (*fb_reset)            (struct awbl_fb_info *p_fb);

    /** \breif ��ȡǰ�û�������ַ */
    uint32_t (*fb_get_online_buf)   (struct awbl_fb_info *p_fb);

    /** \brief ��ȡ����֡��������ַ */
    uint32_t (*fb_get_offline_buf)  (struct awbl_fb_info *p_fb);

    /** \brief ����֡������ */
    aw_err_t (*fb_swap_buf)         (struct awbl_fb_info *p_fb);

    /** \brief ���Խ���֡������ */
    aw_err_t (*fb_try_swap_buf)     (struct awbl_fb_info *p_fb);

    /** \brief ����֡�����豸��Ӱ */
    aw_err_t (*fb_blank)            (struct awbl_fb_info *p_fb, 
                                     aw_fb_black_t        blank);

    /** \brief ����֡�����豸���� */
    aw_err_t (*fb_backlight)        (struct awbl_fb_info *p_fb, int level);

    /** \brief ����֡�����豸ʱ�����Ϣ */
    aw_err_t (*fb_config)     (struct awbl_fb_info *p_fb);
	
    /** \brief ����/��ȡ֡�����豸��Ϣ */
    aw_err_t (*fb_ioctl)            (struct awbl_fb_info *p_fb, 
                                     aw_fb_iocmd_t        cmd, 
                                     void                *p_arg);

}awbl_fb_ops_t;

/**
 * \brief ֡������״̬
 */
typedef struct awbl_fb_status {

    int16_t                 online_buffer;  /**< \brief ǰ�û��������� */
    int16_t                 offline_buffer; /**< \brief ���û��������� */

}awbl_fb_status_t;

/**
 * \brief ֡�����豸��Ϣ
 */
typedef struct awbl_fb_info {

    aw_fb_var_info_t        var_info;       /**< \brief ֡�����豸�ɱ���Ϣ */
    aw_fb_fix_info_t        fix_info;       /**< \brief ֡�����豸�̶���Ϣ */

    const awbl_fb_ops_t    *p_fb_ops;       /**< \brief ֡�����豸�����ӿ� */

    awbl_fb_status_t        fb_status;      /**< \brief ֡������״̬ */

    void                   *p_cookie;

} awbl_fb_info_t;

#ifdef __cplusplus
}
#endif

#endif  /* __AWBL_FB_H */

/* end of file */
