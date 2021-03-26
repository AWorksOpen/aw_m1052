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

/** \brief 摄像头标志位配置参数 */
#define AW_CAMERA_RESOLUTION    (0x0001U)
#define AW_CAMERA_WINDOW        (0x0002U)
#define AW_CAMERA_EXPOSURE      (0x0004U)
#define AW_CAMERA_GAIN          (0x0008U)

#define AW_CAMERA_FORMAT        (0x0010U)
#define AW_CAMERA_CONVERSION    (0x0020U)
#define AW_CAMERA_USB_TYPE      (0x0040U)
#define AW_CAMERA_STANDBY       (0x0080U)

#define AW_CAMERA_PRIVATE       (0x80000000U)


/** \brief 参数使能选项 */
#define AW_CAMERA_OPT_ENABLE    1
#define AW_CAMERA_OPT_DISABLE   0

/** \brief 摄像头当前支持的输出像素格式   */
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

//AW_CAMERA_CONVERSION 可选项
#define MIRROR                  0x01
#define UPSIDE_DOWN             0x02

/* 分辨率设置 */
#define AW_RESOLUTION_SET(width, height)   (width | (height << 16u))

/** \brief 通过分辨率提取宽和高  */
#define VIDEO_EXTRACT_WIDTH(resolution) ((uint16_t)((resolution)&0xFFFFU))
#define VIDEO_EXTRACT_HEIGHT(resolution) ((uint16_t)((resolution) >> 16U))

typedef struct aw_camera_cfg {

    /* AW_CAMERA_RESOLUTION —— 分辨率设置 */
    uint32_t     photo_resolution;        /* 照片的分辨率  */
    uint32_t     video_resolution;        /* 视频的分辨率  */

    /* AW_CAMERA_WINDOW —— 窗口参数设置 */
    uint8_t      win_fix;                 /* 窗口/视野固定使能，根据不同型号的摄像头要求，
                                                                                                                使能此标志需将窗口宽度及高度设为分辨率的整数/偶数倍 */
    uint16_t     win_width;               /* 摄像头窗口宽度 */
    uint16_t     win_height;              /* 摄像头窗口高度 */

    /* AW_CAMERA_EXPOSURE —— 曝光参数设置 */
    uint8_t      aec_enable;              /* 自动曝光使能 */
    uint8_t      brightness;              /* 亮度目标值（使能自动曝光时设置有效） */
    uint32_t     exposure;                /* 曝光值（禁能自动曝光时设置有效） */

    /* AW_CAMERA_GAIN —— 增益参数设置 */
    uint8_t      awb_enable;              /* 自动增益使能 */
    uint16_t     red_gain;                /* 红色增益（禁能自动增益时设置有效） */
    uint16_t     blue_gain;               /* 蓝色增益（禁能自动增益时设置有效） */
    uint16_t     green_gain;              /* 绿色增益（禁能自动增益时设置有效） */

    /* AW_CAMERA_FORMAT —— 输出格式设置 */
    pix_format_t format;                  /* 摄像头输出格式设置 */

    /* AW_CAMERA_CONVERSION —— 图像变换设置 */
    uint16_t     conversion;              /* 变换参数设置 */

    /* AW_CAMERA_USB_TYPE —— USB摄像头的type设置 */
    uint32_t     usb_camera_type;         /* 在使用usb摄像头时需设置此参数，
                                                                                                                   具体设置请参考uvc/videodev2.h */

    /* AW_CAMERA_STANDBY —— 摄像头待机模式设置 */
    uint8_t      standby;                 /* 待机模式使能 */
	
    /* AW_CAMERA_PRIVATE —— 特殊参数设置  */
    void        *private_data;

}aw_camera_cfg_t;


/**
 * \brief   打开摄像头
 *
 * \param       p_name  摄像头名
 * \param[in]   timeout 最大等待时间(超时时间), 单位是系统节拍, 系统时钟节拍频率
 *                      由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                      将毫秒转换为节拍数; #AW_SEM_NO_WAIT 为无阻塞;
 *                      #AW_SEM_WAIT_FOREVER 为一直等待, 无超时
 * \return  成功返回摄像头服务的指针, 否则返回NULL
 */
void *aw_camera_wait_open(char *p_name, int time_out);

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
 * \param   option  参数设置标志
 *
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 *
 * \per 范例
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
 *      //打开摄像头
 *      fd = aw_camera_open("gc0308");
 *      if (fd == NULL) {
 *          aw_kprintf("camera_open error! \n\r");
 *          return ;
 *      }
 *
 *      //配置分辨率
 *      cfg.photo_resolution = AW_RESOLUTION_SET(320, 240);
 *      cfg.video_resolution = AW_RESOLUTION_SET(320, 240);
 *
 *      //固定摄像头视野
 *      cfg.win_fix    = AW_CAMERA_OPT_ENABLE;
 *      cfg.win_height = 480;
 *      cfg.win_width  = 640;
 *
 *      //配置自动曝光
 *      cfg.aec_enable = AW_CAMERA_OPT_ENABLE;
 *      cfg.brightness = 64;
 *
 *      //选定要配置的标志位
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
 *          //获取当前曝光值
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
 * \brief  获取摄像头配制接口
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_cfg   配制内容的指针
 * \param   option  参数获取标志
 *
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_config_get (void *p_serv, struct aw_camera_cfg *p_cfg, uint32_t *option);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_CAMERA_H */

/* end of file */

