/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

#ifndef __AW_CAMERA_H
#define __AW_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/* �ֱ������� */
#define AW_RESOLUTION_SET(width, height)   (width | (height << 16u))

typedef struct aw_camera_cfg {

    uint32_t     photo_resolution;        /* ��Ƭ�ķֱ���  */
    uint32_t     video_resolution;        /* ��Ƶ�ķֱ���  */

}aw_camera_cfg_t;

/**
 * \brief   ������ͷ
 *
 * \param   p_name  ����ͷ��
 * \return  �ɹ���������ͷ�����ָ��, ���򷵻�NULL
 */
void * aw_camera_open (char *p_name);


/**
 * \brief �ر�����ͷ
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_close (void *p_serv);


/**
 * \brief �ͷ�����ͷ��һ֡���յ�ַ
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_release_photo_buf(void *p_serv, void *buf);


/**
 * \brief �ͷ�����ͷ��һ֡Ԥ����ַ
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \param   addr    ���ͷŵĵ�ַ
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_release_video_buf(void *p_serv, void *buf);


/**
 * \brief ��ȡ����ͷ��һ֡���յ�ַ
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \param   p_buf   ��ȡ�ĵ�ַ�洢��p_buf��
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_get_photo_buf (void *p_serv, void **p_buf);


/**
 * \brief ��ȡ����ͷ��һ֡Ԥ����ַ
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \param   p_buf   ��ȡ�ĵ�ַ�洢��p_buf��
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_get_video_buf ( void *p_serv, void **p_buf);


/**
 * \brief  ����ͷ���ƽӿ�
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \param   p_cfg   �������ݵ�ָ��
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_cfg (void *p_serv, struct aw_camera_cfg *p_cfg);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_CAMERA_H */

/* end of file */

