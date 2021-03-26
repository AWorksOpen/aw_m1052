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


/** \brief 摄像头的当前状态    */
typedef enum __stat {
    CLOSED,
    OPENED,
}stat_t;

typedef enum __current_mode {
    VIDEO,
    PHOTO,
}current_mode_t;

/** \brief 存储buffer的节点  */
typedef struct __buffer_node {
    struct aw_list_head node;
    void         *p_buf;
}buffer_node_t;


/** \brief  摄像头服务函数  */
typedef struct awbl_camera_servfuncs {

    /** \brief 打开摄像头 */
    aw_err_t (*pfunc_camera_open)(void *p_cookie);

    /** \brief 关闭摄像头  */
    aw_err_t (*pfunc_camera_close)(void *p_cookie);

    /** \brief 获取照片的地址  */
    aw_err_t (*pfunc_camera_get_photo_buf)(void *p_cookie, uint32_t **p_buf);

    /** \brief 获取预览的地址  */
    aw_err_t (*pfunc_camera_get_video_buf)(void *p_cookie, uint32_t **p_buf);

    /** \brief 释放照片的地址，回收给照片继续使用 */
    aw_err_t (*pfunc_camera_release_photo_buf)(void *p_cookie, uint32_t addr);

    /** \brief 释放预览地址，回收给预览继续使用 */
    aw_err_t (*pfunc_camera_release_video_buf)(void *p_cookie, uint32_t  addr);

    aw_err_t (*pfunc_camera_config_set)(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);

    aw_err_t (*pfunc_camera_config_get)(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);

}awbl_camera_servfuncs_t;


/** \brief  摄像头控制器配制信息  */
typedef struct awbl_camera_controller_cfg_info {
       uint8_t          bytes_per_pix;       /**< \brief 每个像素多少个字节  */

       uint32_t         control_flags;       /**< \brief 控制标志位   */

       uint32_t         resolution;          /**< \brief 分辨率 */

       uint8_t           buf_num;            /**< \brief 预览缓冲区个数    */

       current_mode_t    current_mode;       /**< \brief 当前是拍照模式，还是预览模式 */

       uint32_t      *p_video_buf_addr;      /**< \brief 存储分配的video buf的首地址 */

       uint32_t      *p_photo_buf_addr;      /**< \brief 存储分配的photo buf的首地址 */

       uint32_t      *p_buff_nodes_addr;     /**< \brief 存储分配的buffer个数链表节点的首地址 */
} awbl_camera_controller_cfg_info_t;


/** \brief 摄像头控制器的服务函数接口, 如CSI接口  */
typedef struct awbl_camera_controller_servfuncs {

    /* 打开 */
    aw_err_t (*open)(void *p_cookie, awbl_camera_controller_cfg_info_t *p_cfg);

    /* 关闭  */
    aw_err_t (*close)(void *p_cookie );

    /* 获取照片buf */
    aw_err_t (*get_photo_buf)(void *p_cookie, uint32_t **p_buf);

    /* 获取video buf */
    aw_err_t (*get_video_buf)(void *p_cookie, uint32_t **p_buf);

    /* 释放photo buf */
    aw_err_t (*release_photo_buf)(void *p_cookie, uint32_t buf);

    /* 释放 video buf */
    aw_err_t (*release_video_buf)(void *p_cookie, uint32_t buf);
} awbl_camera_controller_servfuncs_t;


/**
 * \brief AWBus GPIO 服务实例
 */
struct awbl_camera_controller_service {

    /** \brief 指向下一个摄像头控制器 服务 */
    struct awbl_camera_controller_service *p_next;

    /** \brief 摄像头控制器ID   */
    uint8_t    id;

    /** \brief 摄像头控制器服务的相关函数 */
    const struct awbl_camera_controller_servfuncs *p_servfuncs;

    /**
     * \brief 摄像头控制器 服务自定义的数据
     *
     * 此参数在摄像头控制器接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;
};


/**
 * \brief AWBus camera 服务实例
 */
struct awbl_camera_service {

    /** \brief 指向下一个 摄像头 服务 */
    struct awbl_camera_service *p_next;

    /** \brief 摄像头的名字, 如OV7725,通过名字来检索相应的服务 */
    char *p_name;

    /** \brief 摄像头 服务的相关函数 */
    const struct awbl_camera_servfuncs *p_servfuncs;

    /**
     * \brief 摄像头服务自定义的数据
     *        此参数在摄像头接口模块搜索服务时由驱动设置，在调用服务函数时作为第一个参数。
     */
    void *p_cookie;

    /** \brief 摄像头标志位 */
    uint32_t flag;

    /** \brief 引用计数 */
    uint32_t used_count;

    /** \brief 信号量 */
    AW_SEMC_DECL(sem);
};


/**
 * \brief  摄像头服务初始化
 *
 * \param   无
 * \return  无
 */
void aw_camera_serve_init (void);

/**
 * \brief  创建摄像头服务
 *
 * \param   无
 * \return  无
 */
aw_err_t aw_camera_serve_create(char *p_name, void *p_cookie, struct awbl_camera_servfuncs *serv_fun);

/**
 * \brief  移除摄像头服务
 *
 * \param   无
 * \return  无
 */
aw_err_t aw_camera_serve_delete(char *name);
 
 
#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_CAMERA_H */

/* end of file */

