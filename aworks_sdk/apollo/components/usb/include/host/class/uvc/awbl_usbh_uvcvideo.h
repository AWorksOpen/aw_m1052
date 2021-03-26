/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/
#ifndef __AWBL_USBH_UVCVIDEO_H
#define __AWBL_USBH_UVCVIDEO_H

#include "host/awbl_usbh.h"
#include "aw_usb_os.h"
#include "usb_video.h"
#include "videodev2.h"
#include "awbl_camera.h"
/* 支持UVC shell命令*/
#define UVC_SHELL_CMD
/* UVC调试信息*/
//#define UVC_DEBUG

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
/* n/d的值向上取整*/
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))
/* UVC视频缓存个数*/
#define UVC_BUFFER_NUM 3

#define CAMERA_USB_BUS_ID 2

/* 数据转换*/
#if (AW_CPU_ENDIAN == AW_BIG_ENDIAN)
/* 小端转换成大端*/
#define AW_UVC_CPU_TO_LE16(x) \
            (uint16_t) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))

#define AW_UVC_CPU_TO_LE32(x) \
             ((((x) & 0xff000000) >> 24) | \
             (((x) & 0x00ff0000) >> 8) | \
             (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))
#else

#define AW_UVC_CPU_TO_LE32(x) (x)
#define AW_UVC_CPU_TO_LE16(x) (x)

#endif

#define USB_UVC_DEVICE_MATCH_DEVICE \
            (AWBL_USBH_FUNDRV_MATCH_VENDOR | \
             AWBL_USBH_FUNDRV_MATCH_PRODUCT)

#define USB_UVC_DEVICE_MATCH_INT_INFO \
            (AWBL_USBH_FUNDRV_MATCH_FUN_CLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS | \
            AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL)

/* UVC打印函数*/
#define __UVC_TRACE(info, ...) \
            do { \
                AW_INFOF(("USBH-UVC:"info, ##__VA_ARGS__)); \
            } while (0)
#ifdef UVC_DEBUG
    #define __UVC_TRACE_DEBUG  __UVC_TRACE
    #define uvc_printf  aw_kprintf
#else
    #define __UVC_TRACE_DEBUG(info) \
            do { \
            } while (0)
#endif



/* UVC控件数据的数据类型*/
#define UVC_CTRL_DATA_TYPE_RAW      0
#define UVC_CTRL_DATA_TYPE_SIGNED   1
#define UVC_CTRL_DATA_TYPE_UNSIGNED 2
#define UVC_CTRL_DATA_TYPE_BOOLEAN  3
#define UVC_CTRL_DATA_TYPE_ENUM     4
#define UVC_CTRL_DATA_TYPE_BITMASK  5

/* 控件标志*/
#define UVC_CTRL_FLAG_SET_CUR       (1 << 0)
#define UVC_CTRL_FLAG_GET_CUR       (1 << 1)
#define UVC_CTRL_FLAG_GET_MIN       (1 << 2)
#define UVC_CTRL_FLAG_GET_MAX       (1 << 3)
#define UVC_CTRL_FLAG_GET_RES       (1 << 4)
#define UVC_CTRL_FLAG_GET_DEF       (1 << 5)
/* 控件应该在挂起时保存，在恢复时还原*/
#define UVC_CTRL_FLAG_RESTORE       (1 << 6)
/* 控件可以通过摄像头更新 */
#define UVC_CTRL_FLAG_AUTO_UPDATE   (1 << 7)

#define UVC_CTRL_FLAG_GET_RANGE \
    (UVC_CTRL_FLAG_GET_CUR | UVC_CTRL_FLAG_GET_MIN | \
     UVC_CTRL_FLAG_GET_MAX | UVC_CTRL_FLAG_GET_RES | \
     UVC_CTRL_FLAG_GET_DEF)

struct usb_device_id {
    uint16_t match_flags;
    uint16_t idVendor;
    uint16_t idProduct;
};

struct uvc_menu_info {
    uint32_t value;
    uint8_t name[32];
};

#define USB_DEVICE(vend, prod) \
    .match_flags = USB_UVC_DEVICE_MATCH_DEVICE, \
    .idVendor = (vend), \
    .idProduct = (prod)

/* UVC设备结构体*/
struct awbl_usbh_uvc_device{
    struct awbl_usbh_function *uvc_fun;               /* 相关的UBS功能结构体(描述一个USB设备功能)*/
    struct awbl_usbh_interface *uvc_controlitf;       /* UVC控制接口链表(根据UVC规范1.5，每个视频功能只有一个控制接口)*/
    AW_MUTEX_DECL(               mutex);              /* 互斥锁*/
    uint16_t uvc_version;                             /* UVC规范版本*/
    uint32_t clock_frequency;                         /* 时钟频率*/
    uint32_t itfstreaming_altsetting_num;             /* UVC流接口备用设置的数量*/
    struct aw_list_head streams;                      /* 视频流链表*/
    struct aw_list_head entities;                     /* UVC实体链表(描述单元和终端)*/
    struct aw_list_head chains;                       /* 视频设备链表*/
    int nstreams;                                     /* 视频流数量*/
    struct awbl_usbh_trp  int_trp;                    /* 中断传输请求包*/
    struct awbl_usbh_endpoint *int_ep;                /* 中断端点*/
    uint8_t *status;                                  /* UVC设备状态缓存*/
    uint32_t quirks;                                  /* 设备兼容*/
};


/* UVC视频流结构体*/
struct awbl_usbh_uvc_streaming {
    struct aw_list_head list;                         /* UVC流结构体链表*/
    struct awbl_usbh_uvc_device *dev;                 /* UVC设备结构体*/
    struct awbl_usbh_interface *intf;                 /* USB接口结构体*/
    int intfnum;                                      /* 接口编号*/
    struct uvc_streaming_header header;               /* UVC视频流头结构体*/
    aw_usb_mutex_handle_t lock;                       /* 设备互斥锁*/
    uint32_t nformats;                                /* 格式的数量*/
    struct uvc_format *format;                        /* UVC格式结构体*/
    enum v4aw_buf_type type;                          /* video4AWorks 类型*/
    uint16_t maxpsize;                                /* 端点最大包大小*/
    struct uvc_video_chain *chain;                    /* 视频链链表*/
    struct uvc_streaming_control ctrl;                /* UVC流控制结构体*/
    struct uvc_format *def_format;                    /* 默认格式*/
    struct uvc_format *cur_format;                    /* 当前格式*/
    struct uvc_frame *cur_frame;                      /* 当前帧*/
    char *camera_serv_name;                           /* 摄像头服务名字*/
    struct uvc_video_queue queue;                     /* UVC视频队列*/
    uint32_t sequence;                                /* 序列号*/
    uint8_t last_fid;                                 /* 最后帧ID*/
    uint8_t frozen;                                   /* 挂起标志位*/
    struct awbl_usbh_trp *trp[UVC_TRPS];              /* USB请求块*/
    uint32_t trp_size;                                /* USB请求块大小*/
    char *trp_buffer[UVC_TRPS];                       /* USB请求块缓存*/
    /* 编码回调函数*/
    void (*decode) (struct awbl_usbh_trp *trp, struct awbl_usbh_uvc_streaming *video,
            struct uvc_buffer *buf);

    struct {
        struct uvc_stats_frame frame;
        struct uvc_stats_stream stream;
    } stats;

    /* 批量完成处理程序使用的上下文数据*/
    struct {
        uint8_t header[256];
        uint32_t header_size;
        int skip_payload;
        uint32_t payload_size;
        uint32_t max_payload_size;
    } bulk;

    /* 时间戳支持*/
    struct uvc_clock {                                /* UVC时钟结构体*/
        struct uvc_clock_sample {                     /* UVC时钟采样结构体*/
            uint32_t dev_stc;
            uint16_t dev_sof;
            struct aw_timespec host_ts;
            uint16_t host_sof;
        } *samples;

        uint32_t head;
        uint32_t count;

        int16_t last_sof;
        int16_t sof_offset;
        uint32_t size;                                /* */
        aw_spinlock_isr_t lock;                       /* UVC时钟自旋锁*/
    } clock;
};

/* 常用函数声明*/
uint16_t get_unaligned_le16(uint8_t *p);
uint32_t get_unaligned_le32(uint8_t *p);
/* UVC控件相关函数声明*/
int uvc_ctrl_init_device(struct awbl_usbh_uvc_device *dev);
void uvc_ctrl_cleanup_device(struct awbl_usbh_uvc_device *dev);
int uvc_set_video_ctrl(struct awbl_usbh_uvc_streaming *stream,
    struct uvc_streaming_control *ctrl, int probe);
int uvc_get_video_ctrl(struct awbl_usbh_uvc_streaming *stream,
    struct uvc_streaming_control *ctrl, int probe, uint8_t query);

int uvc_video_init(struct awbl_usbh_uvc_streaming *stream);
int uvc_status_init(struct awbl_usbh_uvc_device *dev);
void uvc_cancel_status_trp(struct awbl_usbh_uvc_device *dev);
struct awbl_usbh_endpoint *uvc_find_endpoint(struct awbl_usbh_interface *uvc_itf, uint8_t epaddr);

/* 摄像头服务回调函数*/
aw_err_t __uvc_open(void *p_cookie);
aw_err_t __uvc_close(void *p_cookie);
aw_err_t __uvc_get_photo_buf(void *p_cookie, uint32_t **p_buf);
aw_err_t __uvc_op_get_video_buf(void *p_cookie, uint32_t **p_buf);
aw_err_t __uvc_op_release_video_buf(void *p_cookie, uint32_t addr);
aw_err_t __uvc_release_photo_buf(void *p_cookie, uint32_t addr);
aw_err_t __uvc_cfg_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);
aw_err_t __uvc_cfg_get(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);
/* UVC队列相关函数声明*/
int uvc_queue_init(struct uvc_video_queue *queue, enum v4aw_buf_type type);
int uvc_buffer_init(struct uvc_video_queue *queue, uint32_t buffer_size, uint8_t buffer_num);
int uvc_queue_allocated(struct uvc_video_queue *queue);
int uvc_video_enable(struct awbl_usbh_uvc_streaming *stream, int enable);
int uvc_queue_streamon(struct uvc_video_queue *queue, enum v4aw_buf_type type);
#endif

