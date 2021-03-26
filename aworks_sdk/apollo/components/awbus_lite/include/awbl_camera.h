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

#ifndef __AWBL_CAMERA_H
#define __AWBL_CAMERA_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_camera.h"
#include "aw_sem.h"
#include "aw_list.h"

#define AW_CAM_HREF_ACTIVE_LOW              0x0000u     /**< \brief is active low. */
#define AW_CAM_HREF_ACTIVE_HIGH             (1u << 0u)  /**< \brief is active high. */
#define AW_CAM_DATA_LATCH_ON_FALLING_EDGE   0x0000u     /**< \brief Pixel data latched at falling edge of pixel clock. */
#define AW_CAM_DATA_LATCH_ON_RISING_EDGE    (1u << 1u)  /**< \brief Pixel data latched at rising edge of pixel clock. */
#define AW_CAM_VSYNC_ACTIVE_LOW             0x0000u     /**< \brief VSYNC is active low. */
#define AW_CAM_VSYNC_ACTIVE_HIGH            (1u << 2u)  /**< \brief VSYNC is active high. */


#define AW_CAMERA_FLAG_ALIVE            0x01U
#define AW_CAMERA_FLAG_DYNAMIC          0x02U


/** \brief ����ͷ�ĵ�ǰ״̬    */
typedef enum __stat {
    CLOSED,
    OPENED,
}stat_t;

typedef enum __current_mode {
    VIDEO,
    PHOTO,
}current_mode_t;

/** \brief �洢buffer�Ľڵ�  */
typedef struct __buffer_node {
    struct aw_list_head node;
    void         *p_buf;
}buffer_node_t;


/** \brief  ����ͷ������  */
typedef struct awbl_camera_servfuncs {

    /** \brief ������ͷ */
    aw_err_t (*pfunc_camera_open)(void *p_cookie);

    /** \brief �ر�����ͷ  */
    aw_err_t (*pfunc_camera_close)(void *p_cookie);

    /** \brief ��ȡ��Ƭ�ĵ�ַ  */
    aw_err_t (*pfunc_camera_get_photo_buf)(void *p_cookie, uint32_t **p_buf);

    /** \brief ��ȡԤ���ĵ�ַ  */
    aw_err_t (*pfunc_camera_get_video_buf)(void *p_cookie, uint32_t **p_buf);

    /** \brief �ͷ���Ƭ�ĵ�ַ�����ո���Ƭ����ʹ�� */
    aw_err_t (*pfunc_camera_release_photo_buf)(void *p_cookie, uint32_t addr);

    /** \brief �ͷ�Ԥ����ַ�����ո�Ԥ������ʹ�� */
    aw_err_t (*pfunc_camera_release_video_buf)(void *p_cookie, uint32_t  addr);

    aw_err_t (*pfunc_camera_config_set)(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);

    aw_err_t (*pfunc_camera_config_get)(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);

}awbl_camera_servfuncs_t;


/** \brief  ����ͷ������������Ϣ  */
typedef struct awbl_camera_controller_cfg_info {
       uint8_t          bytes_per_pix;       /**< \brief ÿ�����ض��ٸ��ֽ�  */

       uint32_t         control_flags;       /**< \brief ���Ʊ�־λ   */

       uint32_t         resolution;          /**< \brief �ֱ��� */

       uint8_t           buf_num;            /**< \brief Ԥ������������    */

       current_mode_t    current_mode;       /**< \brief ��ǰ������ģʽ������Ԥ��ģʽ */

       uint32_t      *p_video_buf_addr;      /**< \brief �洢�����video buf���׵�ַ */

       uint32_t      *p_photo_buf_addr;      /**< \brief �洢�����photo buf���׵�ַ */

       uint32_t      *p_buff_nodes_addr;     /**< \brief �洢�����buffer��������ڵ���׵�ַ */
} awbl_camera_controller_cfg_info_t;


/** \brief ����ͷ�������ķ������ӿ�, ��CSI�ӿ�  */
typedef struct awbl_camera_controller_servfuncs {

    /* �� */
    aw_err_t (*open)(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg);

    /* �ر�  */
    aw_err_t (*close)(void *p_cookie );

    /* ��ȡ��Ƭbuf */
    aw_err_t (*get_photo_buf)(void *p_cookie, uint32_t **p_buf);

    /* ��ȡvideo buf */
    aw_err_t (*get_video_buf)(void *p_cookie, uint32_t **p_buf);

    /* �ͷ�photo buf */
    aw_err_t (*release_photo_buf)(void *p_cookie, uint32_t buf);

    /* �ͷ� video buf */
    aw_err_t (*release_video_buf)(void *p_cookie, uint32_t buf);
} awbl_camera_controller_servfuncs_t;


/**
 * \brief AWBus GPIO ����ʵ��
 */
struct awbl_camera_controller_service {

    /** \brief ָ����һ������ͷ������ ���� */
    struct awbl_camera_controller_service *p_next;

    /** \brief ����ͷ������ID   */
    uint8_t    id;

    /** \brief ����ͷ�������������غ��� */
    const struct awbl_camera_controller_servfuncs *p_servfuncs;

    /**
     * \brief ����ͷ������ �����Զ��������
     *
     * �˲���������ͷ�������ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
};


/**
 * \brief AWBus camera ����ʵ��
 */
struct awbl_camera_service {

    /** \brief ָ����һ�� ����ͷ ���� */
    struct awbl_camera_service *p_next;

    /** \brief ����ͷ������, ��OV7725,ͨ��������������Ӧ�ķ��� */
    char *p_name;

    /** \brief ����ͷ �������غ��� */
    const struct awbl_camera_servfuncs *p_servfuncs;

    /**
     * \brief ����ͷ�����Զ��������
     *        �˲���������ͷ�ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;

    /** \brief ����ͷ��־λ */
    uint32_t flag;

    /** \brief ���ü��� */
    uint32_t used_count;

    /** \brief �ź��� */
    AW_SEMC_DECL(sem);
};


/**
 * \brief  ����ͷ�����ʼ��
 *
 * \param   ��
 * \return  ��
 */
void aw_camera_serve_init (void);

/**
 * \brief  ��������ͷ����
 *
 * \param   ��
 * \return  ��
 */
aw_err_t aw_camera_serve_create(char *p_name, void *p_cookie, struct awbl_camera_servfuncs *serv_fun);

/**
 * \brief  �Ƴ�����ͷ����
 *
 * \param   ��
 * \return  ��
 */
aw_err_t aw_camera_serve_delete(char *name);
 
 
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_CAMERA_H */

/* end of file */

