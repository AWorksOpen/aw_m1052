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
#ifndef __USB_VIDEO_H
#define __USB_VIDEO_H
#include "media-entity.h"
#include "aw_time.h"
#include "awbl_usbh_uvcvideo.h"
#include "aw_spinlock.h"
#include "videodev2.h"

/* 获取UVC实体类型*/
#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)

/* UVC接口子类代码*/
#define VSC_UNDEFINED                    0x00         /* 未定义子类*/
#define VSC_VIDEOCONTROL                 0x01         /* 视频控制子类*/
#define VSC_VIDEOSTREAMING               0x02         /* 视频流子类*/
#define VSC_VIDEO_INTERFACE_COLLECTION   0x03         /* 视频接口集子类*/



/* A.5 特定视频类VC接口描述符子类型 */
#define UVC_VC_DESCRIPTOR_UNDEFINED         0x00           /* UVC未定义描述符类型*/
#define UVC_VC_HEADER                       0x01           /* UVC头描述符*/
#define UVC_VC_INPUT_TERMINAL               0x02           /* UVC输入终端描述符*/
#define UVC_VC_OUTPUT_TERMINAL              0x03           /* UVC输出终端描述符*/
#define UVC_VC_SELECTOR_UNIT                0x04           /* UVC选择器单元描述符*/
#define UVC_VC_PROCESSING_UNIT              0x05           /* UVC处理单元描述符*/
#define UVC_VC_EXTENSION_UNIT               0x06           /* UVC扩展单元描述符*/
/* A.6. 特定视频类VC接口描述符子类型 */
#define UVC_VS_UNDEFINED                    0x00       /* UVC未定义描述符*/
#define UVC_VS_INPUT_HEADER                 0x01       /* UVC输入头描述符*/
#define UVC_VS_OUTPUT_HEADER                0x02       /* UVC输出头描述符*/
#define UVC_VS_STILL_IMAGE_FRAME            0x03
#define UVC_VS_FORMAT_UNCOMPRESSED          0x04       /* 无压缩视频格式*/
#define UVC_VS_FRAME_UNCOMPRESSED           0x05       /* 无压缩帧*/
#define UVC_VS_FORMAT_MJPEG                 0x06       /* MJPEG格式*/
#define UVC_VS_FRAME_MJPEG                  0x07       /* MJPEG帧*/
#define UVC_VS_FORMAT_MPEG2TS               0x0a       /* MPEG2-TS格式*/
#define UVC_VS_FORMAT_DV                    0x0c       /* DV格式*/
#define UVC_VS_COLORFORMAT                  0x0d
#define UVC_VS_FORMAT_FRAME_BASED           0x10       /* 供应商自定义格式*/
#define UVC_VS_FRAME_FRAME_BASED            0x11       /* 通用基本帧*/
#define UVC_VS_FORMAT_STREAM_BASED          0x12       /* MPEG1-SS/MPEG2-PS格式*/
/* A.8.视频特定类请求代码*/
#define UVC_RC_UNDEFINED                0x00
#define UVC_SET_CUR                 0x01
#define UVC_GET_CUR                 0x81          /* 返回要协商的字段的默认值*/
#define UVC_GET_MIN                 0x82          /* 返回要协商的字段的最小值*/
#define UVC_GET_MAX                 0x83          /* 返回要协商的字段的最大值*/
#define UVC_GET_RES                 0x84
#define UVC_GET_LEN                 0x85
#define UVC_GET_INFO                    0x86
#define UVC_GET_DEF                 0x87
/* A.9.4. 摄像头终端控制选择器*/
#define UVC_CT_CONTROL_UNDEFINED            0x00
#define UVC_CT_SCANNING_MODE_CONTROL            0x01
#define UVC_CT_AE_MODE_CONTROL              0x02
#define UVC_CT_AE_PRIORITY_CONTROL          0x03
#define UVC_CT_EXPOSURE_TIME_ABSOLUTE_CONTROL       0x04
#define UVC_CT_EXPOSURE_TIME_RELATIVE_CONTROL       0x05
#define UVC_CT_FOCUS_ABSOLUTE_CONTROL           0x06
#define UVC_CT_FOCUS_RELATIVE_CONTROL           0x07
#define UVC_CT_FOCUS_AUTO_CONTROL           0x08
#define UVC_CT_IRIS_ABSOLUTE_CONTROL            0x09
#define UVC_CT_IRIS_RELATIVE_CONTROL            0x0a
#define UVC_CT_ZOOM_ABSOLUTE_CONTROL            0x0b
#define UVC_CT_ZOOM_RELATIVE_CONTROL            0x0c
#define UVC_CT_PANTILT_ABSOLUTE_CONTROL         0x0d
#define UVC_CT_PANTILT_RELATIVE_CONTROL         0x0e
#define UVC_CT_ROLL_ABSOLUTE_CONTROL            0x0f
#define UVC_CT_ROLL_RELATIVE_CONTROL            0x10
#define UVC_CT_PRIVACY_CONTROL              0x11

/* A.9.5. 处理单元控制选择器*/
#define UVC_PU_CONTROL_UNDEFINED            0x00
#define UVC_PU_BACKLIGHT_COMPENSATION_CONTROL       0x01
#define UVC_PU_BRIGHTNESS_CONTROL           0x02
#define UVC_PU_CONTRAST_CONTROL             0x03
#define UVC_PU_GAIN_CONTROL             0x04
#define UVC_PU_POWER_LINE_FREQUENCY_CONTROL     0x05
#define UVC_PU_HUE_CONTROL              0x06
#define UVC_PU_SATURATION_CONTROL           0x07
#define UVC_PU_SHARPNESS_CONTROL            0x08
#define UVC_PU_GAMMA_CONTROL                0x09
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_CONTROL    0x0a
#define UVC_PU_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL   0x0b
#define UVC_PU_WHITE_BALANCE_COMPONENT_CONTROL      0x0c
#define UVC_PU_WHITE_BALANCE_COMPONENT_AUTO_CONTROL 0x0d
#define UVC_PU_DIGITAL_MULTIPLIER_CONTROL       0x0e
#define UVC_PU_DIGITAL_MULTIPLIER_LIMIT_CONTROL     0x0f
#define UVC_PU_HUE_AUTO_CONTROL             0x10
#define UVC_PU_ANALOG_VIDEO_STANDARD_CONTROL        0x11
#define UVC_PU_ANALOG_LOCK_STATUS_CONTROL       0x12

/* A.9.7. 视频流接口控件选择器*/
#define UVC_VS_CONTROL_UNDEFINED            0x00             /* 未定义控件*/
#define UVC_VS_PROBE_CONTROL                0x01             /* 探测控件*/
#define UVC_VS_COMMIT_CONTROL               0x02             /* 提交控件*/
#define UVC_VS_STILL_PROBE_CONTROL          0x03
#define UVC_VS_STILL_COMMIT_CONTROL         0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL      0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL        0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL       0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL     0x08
#define UVC_VS_SYNC_DELAY_CONTROL           0x09

/* B.1. USB终端类型*/
#define UVC_TT_VENDOR_SPECIFIC              0x0100    /* 特定供应商终端类型*/
#define UVC_TT_STREAMING                0x0101        /* 流终端类型*/

/* B.2. 输入终端类型*/
#define UVC_ITT_VENDOR_SPECIFIC             0x0200   /* 供应商特定输入终端*/
#define UVC_ITT_CAMERA                      0x0201   /* 摄像头传感器。仅用于摄像头终端描述符*/
#define UVC_ITT_MEDIA_TRANSPORT_INPUT       0x0202   /* 序列媒体。仅用于媒体传输终端描述符*/

/* B.3. 输出终端类型*/
#define UVC_OTT_VENDOR_SPECIFIC             0x0300       /* 特定供应商输出终端*/
#define UVC_OTT_DISPLAY                 0x0301           /* 显示输出终端*/
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT          0x0302   /* 媒体输出终端*/

/* 2.4.2.2. 状态包类型*/
#define UVC_STATUS_TYPE_CONTROL             1
#define UVC_STATUS_TYPE_STREAMING           2

/* 2.4.3.3. 负载头信息*/
#define UVC_STREAM_EOH                  (1 << 7)
#define UVC_STREAM_ERR                  (1 << 6)
#define UVC_STREAM_STI                  (1 << 5)
#define UVC_STREAM_RES                  (1 << 4)
#define UVC_STREAM_SCR                  (1 << 3)
#define UVC_STREAM_PTS                  (1 << 2)
#define UVC_STREAM_EOF                  (1 << 1)
#define UVC_STREAM_FID                  (1 << 0)       /* 帧ID*/
/* --------------------------------------------------------------------------
 * UVC constants
 */

#define UVC_TERM_INPUT          0x0000                                  /* 输入终端*/
#define UVC_TERM_OUTPUT         0x8000                                  /* 输出终端*/
#define UVC_TERM_DIRECTION(term)    ((term)->type & 0x8000)             /* 检查终端方向*/

#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)           /* 检查UVC实体类型*/
#define UVC_ENTITY_IS_UNIT(entity)  (((entity)->type & 0xff00) == 0)    /* 检查UVC实体是不是一个单元*/
#define UVC_ENTITY_IS_TERM(entity)  (((entity)->type & 0xff00) != 0)    /* 检查UVC实体是不是一个终端*/
/* 判断实体是不是输入终端类型*/
#define UVC_ENTITY_IS_ITERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_INPUT)
/* 判断实体是不是输出终端类型*/
#define UVC_ENTITY_IS_OTERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_OUTPUT)

/* ------------------------------------------------------------------------
 * 全局唯一标识符(Globally Unique Identifier,GUIDs)
 */
#define UVC_GUID_UVC_CAMERA \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}
#define UVC_GUID_UVC_OUTPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02}
#define UVC_GUID_UVC_MEDIA_TRANSPORT_INPUT \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03}
#define UVC_GUID_UVC_PROCESSING \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01}
#define UVC_GUID_UVC_SELECTOR \
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \
     0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02}

#define UVC_GUID_FORMAT_MJPEG \
    { 'M',  'J',  'P',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2 \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YUY2_ISIGHT \
    { 'Y',  'U',  'Y',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0x00, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_NV12 \
    { 'N',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_YV12 \
    { 'Y',  'V',  '1',  '2', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_I420 \
    { 'I',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_UYVY \
    { 'U',  'Y',  'V',  'Y', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y800 \
    { 'Y',  '8',  '0',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y8 \
    { 'Y',  '8',  ' ',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y10 \
    { 'Y',  '1',  '0',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y12 \
    { 'Y',  '1',  '2',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_Y16 \
    { 'Y',  '1',  '6',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BY8 \
    { 'B',  'Y',  '8',  ' ', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BA81 \
    { 'B',  'A',  '8',  '1', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GBRG \
    { 'G',  'B',  'R',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_GRBG \
    { 'G',  'R',  'B',  'G', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGGB \
    { 'R',  'G',  'G',  'B', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_RGBP \
    { 'R',  'G',  'B',  'P', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}
#define UVC_GUID_FORMAT_BGR3 \
    { 0x7d, 0xeb, 0x36, 0xe4, 0x4f, 0x52, 0xce, 0x11, \
     0x9f, 0x53, 0x00, 0x20, 0xaf, 0x0b, 0xa7, 0x70}
#define UVC_GUID_FORMAT_M420 \
    { 'M',  '4',  '2',  '0', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}

#define UVC_GUID_FORMAT_H264 \
    { 'H',  '2',  '6',  '4', 0x00, 0x00, 0x10, 0x00, \
     0x80, 0x00, 0x00, 0xaa, 0x00, 0x38, 0x9b, 0x71}


/* 设备兼容*/
#define UVC_QUIRK_FIX_BANDWIDTH     0x00000080
#define UVC_QUIRK_STREAM_NO_FID     0x00000010

/* 格式标志 */
#define UVC_FMT_FLAG_COMPRESSED     0x00000001    /* 压缩标志*/
#define UVC_FMT_FLAG_STREAM         0x00000002    /* 流标志*/

#define UVC_CTRL_STREAMING_TIMEOUT  5000      /* UVC控制流超时时间*/

/* 等时USB请求块的数量*/
#define UVC_TRPS        2
/* 每个USB请求块的最大包数量*/
#define UVC_MAX_PACKETS     32
/* 中断URB的最大状态缓存字节大小*/
#define UVC_MAX_STATUS_SIZE 16
/* UVC格式描述符*/
struct uvc_format_desc {
    char *name;           /* 格式名字*/
    uint8_t guid[16];     /* 全局唯一标识符*/
    uint32_t fcc;
};

/* UVC帧结构体*/
struct uvc_frame {
    uint8_t  bFrameIndex;
    uint8_t  bmCapabilities;
    uint16_t wWidth;
    uint16_t wHeight;
    uint32_t dwMinBitRate;
    uint32_t dwMaxBitRate;
    uint32_t dwMaxVideoFrameBufferSize;
    uint8_t  bFrameIntervalType;
    uint32_t dwDefaultFrameInterval;
    uint32_t *dwFrameInterval;
};

/* UVC格式结构体*/
struct uvc_format {
    uint8_t type;            /* 格式类型*/
    uint8_t index;           /* 格式索引*/
    uint8_t bpp;             /* 像素深度*/
    uint8_t colorspace;      /* 色域*/
    uint32_t fcc;
    uint32_t flags;          /* 格式标志*/

    char name[32];           /* 格式名字*/

    unsigned int nframes;    /* 帧结构体数量*/
    struct uvc_frame *frame; /* UVC帧结构体*/
};

/* UVC视频流头结构体*/
struct uvc_streaming_header {
    uint8_t bNumFormats;
    uint8_t bEndpointAddress;
    uint8_t bTerminalLink;
    uint8_t bControlSize;
    uint8_t *bmaControls;
    /* 以下字段只用于输入头描述符*/
    uint8_t bmInfo;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
};

/* UVC控件信息结构体*/
struct uvc_control_info {
    struct aw_list_head mappings;   /* 控件图链表*/

    uint8_t entity[16];             /* UVC实体GUID*/
    uint8_t index;                  /* bmControls的位索引(Bit index in bmControls) */
    uint8_t selector;

    uint16_t size;
    uint32_t flags;
};

struct uvc_control_mapping {
    struct aw_list_head list;
    struct aw_list_head ev_subs;

    uint32_t id;
    uint8_t name[32];
    uint8_t entity[16];
    uint8_t selector;

    uint8_t size;
    uint8_t offset;
    enum v4aw_ctrl_type v4aw_type;
    uint32_t data_type;

    struct uvc_menu_info *menu_info;
    uint32_t menu_count;

    uint32_t master_id;
    int master_manual;
    uint32_t slave_ids[2];

    int (*get) (struct uvc_control_mapping *mapping, uint8_t query,
              const uint8_t *data);
    void (*set) (struct uvc_control_mapping *mapping, int value,
            uint8_t *data);
};

#define UVC_ENTITY_FLAG_DEFAULT     (1 << 0)

/* UVC实体结构体*/
struct uvc_entity {
    struct aw_list_head list;                    /* 当前UVC实体节点*/
    struct aw_list_head chain;                   /* 视频设备链节点*/

    uint32_t flags;                              /* UVC实体标志*/

    uint8_t id;                                  /* UVC实体ID*/
    uint16_t type;                               /* UVC实体类型*/
    char name[64];                               /* UVC实体名字*/

    uint32_t num_pads;
    uint32_t num_links;
    struct media_pad *pads;

    union {
        /* 摄像头信息*/
        struct {
            uint16_t wObjectiveFocalLengthMin;    /* L(min)的值，如果不支持光学变焦，则为0*/
            uint16_t wObjectiveFocalLengthMax;    /* L(max)的值，如果不支持光学变焦，则为0*/
            uint16_t wOcularFocalLength;          /* L(ocular)的值，如果不支持光学变焦，则为0*/
            uint8_t  bControlSize;                /* 位图大小*/
            uint8_t  *bmControls;                 /* bmControls 是一个位图，表示视频流的某些摄像头控件的可用性*/
        } camera;
        /* 媒体*/
        struct {
            uint8_t  bControlSize;
            uint8_t  *bmControls;
            uint8_t  bTransportModeSize;
            uint8_t  *bmTransportModes;
        } media;
        /* 输出终端*/
        struct {
        } output;
        /* 处理单元*/
        struct {
            uint16_t wMaxMultiplier;               /* 数字倍增(此字段指示最大数字倍增率乘以100)*/
            uint8_t  bControlSize;                 /* 控件位图的大小*/
            uint8_t  *bmControls;                  /* 控件位图地址*/
            uint8_t  bmVideoStandards;             /* 处理单元支持的所有模拟视频标准的位图*/
        } processing;
        /* 选择器单元*/
        struct {
        } selector;
        /* 扩展单元*/
        struct {
            uint8_t  guidExtensionCode[16];        /* 特定供应商的代码*/
            uint8_t  bNumControls;                 /* 扩展单元的控件数量*/
            uint8_t  bControlSize;                 /* 控件位图的大小*/
            uint8_t  *bmControls;                  /* 控件位图的地址*/
            uint8_t  *bmControlsType;
        } extension;
    };
    uint8_t bNrInPins;                             /* 输入引脚个数*/
    uint8_t *baSourceID;                           /* 连接点ID*/

    uint32_t ncontrols;                            /* 控件数量*/
    struct uvc_control *controls;                  /* UVC控件结构体*/
};

/* UVC控件结构体*/
struct uvc_control {
    struct uvc_entity *entity;    /* 所属的UVC实体*/
    struct uvc_control_info info; /* 控件信息结构体*/

    uint8_t index;                /* 用 来匹配 uvc_control 入口和 uvc_control_info*/
    uint8_t dirty:1,
         loaded:1,
         modified:1,
         cached:1,
         initialized:1;           /* 初始化完成标志*/

    uint8_t *uvc_data;
};

#define UVC_QUEUE_DISCONNECTED      (1 << 0)
#define UVC_QUEUE_DROP_CORRUPTED    (1 << 1)

/* UVC视频队列结构体*/
struct uvc_video_queue {
    aw_usb_mutex_handle_t mutex; /* 保护队列*/

    uint32_t flags;
    uint32_t buf_used;

    uint32_t num_buffers;         /* 被分配/使用的缓存的数量*/
    aw_spinlock_isr_t irqlock;    /* 保护中断队列*/
    struct aw_list_head irqqueue; /* 中断队列链表*/
};

/* uvc视频链结构体*/
struct uvc_video_chain {
    struct awbl_usbh_uvc_device *dev;  /* 所属的uvc设备结构体*/
    struct aw_list_head list;          /* 当前视频链节点*/

    struct aw_list_head entities;      /* 所有实体 */
    struct uvc_entity *processing;     /* 处理单元*/
    struct uvc_entity *selector;       /* 选择单元*/

    aw_usb_mutex_handle_t mutex;       /* 保护控件信息*/
    uint32_t caps;                     /* V4AW chain-wide caps */
};

/* 视频探测和提交控件*/
struct uvc_streaming_control {
    uint16_t bmHint;                   /* 向功能只是哪些字段应保持固定的位字段控制*/
    uint8_t  bFormatIndex;             /* 来自此视频接口的格式描述符的视频格式索引*/
    uint8_t  bFrameIndex;              /* 来自帧描述符的视频帧索引*/
    uint32_t dwFrameInterval;          /* 此字段为选定的视频流和帧索引设置所需的视频帧间隔(100ns为单位的帧间隔)*/
    uint16_t wKeyFrameRate;            /* 每视频帧单位的关键帧速率*/
    uint16_t wPFrameRate;              /* 以帧/关键帧单位表示的帧速率*/
    uint16_t wCompQuality;             /* 抽象单元压缩质量控制1(最低)到10000(最高)*/
    uint16_t wCompWindowSize;          /* 平均比特率控制的窗口大小*/
    uint16_t wDelay;                   /* 内部视频流接口延迟(毫秒)*/
    uint32_t dwMaxVideoFrameSize;      /* 视频帧或编解码其特定的最大段大小(字节)*/
    uint32_t dwMaxPayloadTransferSize; /* 指定设备在单个有效负载传输中可以发送或接受的最大字节数*/
    uint32_t dwClockFrequency;         /* 指定合适的设备时钟频率*/
    uint8_t  bmFramingInfo;            /* 此控件向功能指示有效负载传输是否将在视频有效负载头中包含外带帧信息*/
    uint8_t  bPreferedVersion;         /* 主机或设备为指定的bFormatIndex值支持的首选负载格式版本*/
    uint8_t  bMinVersion;              /* 主机或设备为指定的bFormatIndex值支持的最小负载格式版本*/
    uint8_t  bMaxVersion;              /* 主机或设备为指定的bFormatIndex值支持的最大负载格式版本*/
} __attribute__((__packed__));

enum uvc_buffer_state {
    UVC_BUF_STATE_IDLE  = 0,
    UVC_BUF_STATE_QUEUED    = 1,
    UVC_BUF_STATE_ACTIVE    = 2,
    UVC_BUF_STATE_READY = 3,
    UVC_BUF_STATE_DONE  = 4,
    UVC_BUF_STATE_ERROR = 5,
};

/* USB缓存结构体*/
struct uvc_buffer {
    struct aw_list_head queue;      /* 缓存节点*/

    enum uvc_buffer_state state;    /* 缓存状态*/
    uint32_t error;                 /* 错误代码*/

    void *mem;                      /* 缓存指针*/
    uint32_t length;                /* 缓存长度*/
    uint32_t bytesused;             /* 缓存已填充字节数*/
    uint8_t is_used;                /* 是否被使用标志*/

    uint32_t pts;
};

/* UVC帧状态结构体*/
struct uvc_stats_frame {
    uint32_t size;            /* 捕获到的字节数*/
    uint32_t first_data;      /* 第一个非空包的索引*/

    uint32_t nb_packets;      /* 包的数量*/
    uint32_t nb_empty;        /* 空包的数量*/
    uint32_t nb_invalid;      /* 不可用头部包的数量*/
    uint32_t nb_errors;       /* 设置了错误位的包的数量*/

    uint32_t nb_pts;        /* Number of packets with a PTS timestamp */
    uint32_t nb_pts_diffs;  /* Number of PTS differences inside a frame */
    uint32_t last_pts_diff; /* Index of the last PTS difference */
    aw_bool_t has_initial_pts;       /* Whether the first non-empty packet has a PTS */
    aw_bool_t has_early_pts;     /* Whether a PTS is present before the first non-empty packet */
    uint32_t pts;            /* PTS of the last packet */

    uint32_t nb_scr;        /* Number of packets with a SCR timestamp */
    uint32_t nb_scr_diffs;  /* Number of SCR.STC differences inside a frame */
    uint16_t scr_sof;            /* SCR.SOF of the last packet */
    uint32_t scr_stc;            /* SCR.STC of the last packet */
};

/* UVC流状态结构体*/
struct uvc_stats_stream {
    struct aw_timespec start_ts;   /* 流起始时间戳*/
    struct aw_timespec stop_ts;    /* 流停止时间戳*/

    uint32_t nb_frames;            /* 帧的数量*/

    uint32_t nb_packets;           /* 包的数量*/
    uint32_t nb_empty;             /* 空包的数量*/
    uint32_t nb_invalid;           /* 不可用头部包的数量*/
    uint32_t nb_errors;            /* 设置了错误位的包的数量*/

    uint32_t nb_pts_constant;      /* 包含显示时间戳的帧的数量*/
    uint32_t nb_pts_early;         /* Number of frames with early PTS */
    uint32_t nb_pts_initial;       /* Number of frames with initial PTS */

    uint32_t nb_scr_count_ok;   /* Number of frames with at least one SCR per non empty packet */
    uint32_t nb_scr_diffs_ok;   /* Number of frames with varying SCR.STC */
    uint32_t scr_sof_count; /* STC.SOF counter accumulated since stream start */
    uint32_t scr_sof;       /* STC.SOF of the last packet */
    uint32_t min_sof;           /* 最小 STC.SOF 值*/
    uint32_t max_sof;           /* 最大 STC.SOF 值 */
};


#endif
