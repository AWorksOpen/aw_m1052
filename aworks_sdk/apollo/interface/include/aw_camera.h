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

/** \brief ����ͷ��־λ���ò��� */
#define AW_CAMERA_RESOLUTION    (0x0001U)
#define AW_CAMERA_WINDOW        (0x0002U)
#define AW_CAMERA_EXPOSURE      (0x0004U)
#define AW_CAMERA_GAIN          (0x0008U)

#define AW_CAMERA_FORMAT        (0x0010U)
#define AW_CAMERA_CONVERSION    (0x0020U)
#define AW_CAMERA_USB_TYPE      (0x0040U)
#define AW_CAMERA_STANDBY       (0x0080U)

#define AW_CAMERA_PRIVATE       (0x80000000U)


/** \brief ����ʹ��ѡ�� */
#define AW_CAMERA_OPT_ENABLE    1
#define AW_CAMERA_OPT_DISABLE   0

/** \brief ����ͷ��ǰ֧�ֵ�������ظ�ʽ   */
typedef enum __pix_format {
    RGB565,
    RGB555,
    RGB444,
    RGB888,
    YUV420,
    YUV422,
    YUV444,
    JPEG,
    MJPEG,
    BAYER_RAW,
} pix_format_t;

//AW_CAMERA_CONVERSION ��ѡ��
#define MIRROR                  0x01
#define UPSIDE_DOWN             0x02

/* �ֱ������� */
#define AW_RESOLUTION_SET(width, height)   (width | (height << 16u))

/** \brief ͨ���ֱ�����ȡ��͸�  */
#define VIDEO_EXTRACT_WIDTH(resolution) ((uint16_t)((resolution)&0xFFFFU))
#define VIDEO_EXTRACT_HEIGHT(resolution) ((uint16_t)((resolution) >> 16U))

typedef struct aw_camera_cfg {

    /* AW_CAMERA_RESOLUTION ���� �ֱ������� */
    uint32_t     photo_resolution;        /* ��Ƭ�ķֱ���  */
    uint32_t     video_resolution;        /* ��Ƶ�ķֱ���  */

    /* AW_CAMERA_WINDOW ���� ���ڲ������� */
    uint8_t      win_fix;                 /* ����/��Ұ�̶�ʹ�ܣ����ݲ�ͬ�ͺŵ�����ͷҪ��
                                                                                                                ʹ�ܴ˱�־�轫���ڿ�ȼ��߶���Ϊ�ֱ��ʵ�����/ż���� */
    uint16_t     win_width;               /* ����ͷ���ڿ�� */
    uint16_t     win_height;              /* ����ͷ���ڸ߶� */

    /* AW_CAMERA_EXPOSURE ���� �ع�������� */
    uint8_t      aec_enable;              /* �Զ��ع�ʹ�� */
    uint8_t      brightness;              /* ����Ŀ��ֵ��ʹ���Զ��ع�ʱ������Ч�� */
    uint32_t     exposure;                /* �ع�ֵ�������Զ��ع�ʱ������Ч�� */

    /* AW_CAMERA_GAIN ���� ����������� */
    uint8_t      awb_enable;              /* �Զ�����ʹ�� */
    uint16_t     red_gain;                /* ��ɫ���棨�����Զ�����ʱ������Ч�� */
    uint16_t     blue_gain;               /* ��ɫ���棨�����Զ�����ʱ������Ч�� */
    uint16_t     green_gain;              /* ��ɫ���棨�����Զ�����ʱ������Ч�� */

    /* AW_CAMERA_FORMAT ���� �����ʽ���� */
    pix_format_t format;                  /* ����ͷ�����ʽ���� */

    /* AW_CAMERA_CONVERSION ���� ͼ��任���� */
    uint16_t     conversion;              /* �任�������� */

    /* AW_CAMERA_USB_TYPE ���� USB����ͷ��type���� */
    uint32_t     usb_camera_type;         /* ��ʹ��usb����ͷʱ�����ô˲�����
                                                                                                                   ����������ο�uvc/videodev2.h */

    /* AW_CAMERA_STANDBY ���� ����ͷ����ģʽ���� */
    uint8_t      standby;                 /* ����ģʽʹ�� */
	
    /* AW_CAMERA_PRIVATE ���� �����������  */
    void        *private_data;

}aw_camera_cfg_t;


/**
 * \brief   ������ͷ
 *
 * \param       p_name  ����ͷ��
 * \param[in]   timeout ���ȴ�ʱ��(��ʱʱ��), ��λ��ϵͳ����, ϵͳʱ�ӽ���Ƶ��
 *                      �ɺ��� aw_sys_clkrate_get()���, ��ʹ�� aw_ms_to_ticks()
 *                      ������ת��Ϊ������; #AW_SEM_NO_WAIT Ϊ������;
 *                      #AW_SEM_WAIT_FOREVER Ϊһֱ�ȴ�, �޳�ʱ
 * \return  �ɹ���������ͷ�����ָ��, ���򷵻�NULL
 */
void *aw_camera_wait_open(char *p_name, int time_out);

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
 * \param   option  �������ñ�־
 *
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 *
 * \per ����
 * \code
 *
 * #include "Aworks.h"
 * #include "aw_vdebug.h"
 * #include "aw_camera.h"
 *
 * void demo_camera(void)
 * {
 *      aw_err_t ret;
 *      void *fd = NULL;
 *      uint32_t opt;
 *      void * buf = NULL;
 *      struct aw_camera_cfg cfg;
 *
 *      //������ͷ
 *      fd = aw_camera_open("gc0308");
 *      if (fd == NULL) {
 *          aw_kprintf("camera_open error! \n\r");
 *          return ;
 *      }
 *
 *      //���÷ֱ���
 *      cfg.photo_resolution = AW_RESOLUTION_SET(320, 240);
 *      cfg.video_resolution = AW_RESOLUTION_SET(320, 240);
 *
 *      //�̶�����ͷ��Ұ
 *      cfg.win_fix    = AW_CAMERA_OPT_ENABLE;
 *      cfg.win_height = 480;
 *      cfg.win_width  = 640;
 *
 *      //�����Զ��ع�
 *      cfg.aec_enable = AW_CAMERA_OPT_ENABLE;
 *      cfg.brightness = 64;
 *
 *      //ѡ��Ҫ���õı�־λ
 *      opt = AW_CAMERA_RESOLUTION | AW_CAMERA_WINDOW | AW_CAMERA_EXPOSURE;
 *      ret = aw_camera_config_set(fd, &cfg, &opt);
 *      if (ret != AW_OK) {
 *          aw_kprintf("setting camera error,code:%d\r\n",ret);
 *      }
 *      aw_kprintf("camera config:\n\rresolution set %s! \n"
 *                 "window set %s! \nexposure set %s! \n",
 *                 (opt & AW_CAMERA_RESOLUTION)?"successful":"failed",
 *                 (opt & AW_CAMERA_WINDOW)?"successful":"failed",
 *                 (opt & AW_CAMERA_EXPOSURE)?"successful":"failed");
 *
 *      ret = aw_camera_get_video_buf(fd, &buf);
 *      if (ret == AW_OK) {
 *          //��ȡ��ǰ�ع�ֵ
 *          opt = AW_CAMERA_EXPOSURE;
 *          ret = aw_camera_config_get(fd, &cfg, &opt);
 *          if (ret != AW_OK) {
 *              aw_kprintf("getting camera error,code:%d\r\n",ret);
 *              return;
 *          }
 *          if (opt & AW_CAMERA_EXPOSURE) {
 *              aw_kprintf("exposure value is %d\n", cfg.exposure);
 *          } else {
 *              aw_kprintf("can not get exposure value\n");
 *          }
 *
 *          //todo
 *
 *          ret = aw_camera_release_video_buf(fd, buf);
 *      }
 *
 *      aw_camera_close(fd);
 *      return;
 * }
 */
aw_err_t aw_camera_config_set (void *p_serv, struct aw_camera_cfg *p_cfg, uint32_t *option);


/**
 * \brief  ��ȡ����ͷ���ƽӿ�
 *
 * \param   p_serv  ����ͷ�ķ���ָ��
 * \param   p_cfg   �������ݵ�ָ��
 * \param   option  ������ȡ��־
 *
 * \return  �ɹ�����AW_OK, ���򷵻���Ӧ�Ĵ������
 */
aw_err_t aw_camera_config_get (void *p_serv, struct aw_camera_cfg *p_cfg, uint32_t *option);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_CAMERA_H */

/* end of file */

