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

/* 分辨率设置 */
#define AW_RESOLUTION_SET(width, height)   (width | (height << 16u))

typedef struct aw_camera_cfg {

    uint32_t     photo_resolution;        /* 照片的分辨率  */
    uint32_t     video_resolution;        /* 视频的分辨率  */

}aw_camera_cfg_t;

/**
 * \brief   打开摄像头
 *
 * \param   p_name  摄像头名
 * \return  成功返回摄像头服务的指针, 否则返回NULL
 */
void * aw_camera_open (char *p_name);


/**
 * \brief 关闭摄像头
 *
 * \param   p_serv  摄像头的服务指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_close (void *p_serv);


/**
 * \brief 释放摄像头的一帧拍照地址
 *
 * \param   p_serv  摄像头的服务指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_release_photo_buf(void *p_serv, void *buf);


/**
 * \brief 释放摄像头的一帧预览地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   addr    待释放的地址
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_release_video_buf(void *p_serv, void *buf);


/**
 * \brief 获取摄像头的一帧拍照地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_buf   获取的地址存储在p_buf中
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_get_photo_buf (void *p_serv, void **p_buf);


/**
 * \brief 获取摄像头的一帧预览地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_buf   获取的地址存储在p_buf中
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_get_video_buf ( void *p_serv, void **p_buf);


/**
 * \brief  摄像头配制接口
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_cfg   配制内容的指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_cfg (void *p_serv, struct aw_camera_cfg *p_cfg);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_CAMERA_H */

/* end of file */

