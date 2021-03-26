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

/* ��ȡUVCʵ������*/
#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)

/* UVC�ӿ��������*/
#define VSC_UNDEFINED                    0x00         /* δ��������*/
#define VSC_VIDEOCONTROL                 0x01         /* ��Ƶ��������*/
#define VSC_VIDEOSTREAMING               0x02         /* ��Ƶ������*/
#define VSC_VIDEO_INTERFACE_COLLECTION   0x03         /* ��Ƶ�ӿڼ�����*/



/* A.5 �ض���Ƶ��VC�ӿ������������� */
#define UVC_VC_DESCRIPTOR_UNDEFINED         0x00           /* UVCδ��������������*/
#define UVC_VC_HEADER                       0x01           /* UVCͷ������*/
#define UVC_VC_INPUT_TERMINAL               0x02           /* UVC�����ն�������*/
#define UVC_VC_OUTPUT_TERMINAL              0x03           /* UVC����ն�������*/
#define UVC_VC_SELECTOR_UNIT                0x04           /* UVCѡ������Ԫ������*/
#define UVC_VC_PROCESSING_UNIT              0x05           /* UVC����Ԫ������*/
#define UVC_VC_EXTENSION_UNIT               0x06           /* UVC��չ��Ԫ������*/
/* A.6. �ض���Ƶ��VC�ӿ������������� */
#define UVC_VS_UNDEFINED                    0x00       /* UVCδ����������*/
#define UVC_VS_INPUT_HEADER                 0x01       /* UVC����ͷ������*/
#define UVC_VS_OUTPUT_HEADER                0x02       /* UVC���ͷ������*/
#define UVC_VS_STILL_IMAGE_FRAME            0x03
#define UVC_VS_FORMAT_UNCOMPRESSED          0x04       /* ��ѹ����Ƶ��ʽ*/
#define UVC_VS_FRAME_UNCOMPRESSED           0x05       /* ��ѹ��֡*/
#define UVC_VS_FORMAT_MJPEG                 0x06       /* MJPEG��ʽ*/
#define UVC_VS_FRAME_MJPEG                  0x07       /* MJPEG֡*/
#define UVC_VS_FORMAT_MPEG2TS               0x0a       /* MPEG2-TS��ʽ*/
#define UVC_VS_FORMAT_DV                    0x0c       /* DV��ʽ*/
#define UVC_VS_COLORFORMAT                  0x0d
#define UVC_VS_FORMAT_FRAME_BASED           0x10       /* ��Ӧ���Զ����ʽ*/
#define UVC_VS_FRAME_FRAME_BASED            0x11       /* ͨ�û���֡*/
#define UVC_VS_FORMAT_STREAM_BASED          0x12       /* MPEG1-SS/MPEG2-PS��ʽ*/
/* A.8.��Ƶ�ض����������*/
#define UVC_RC_UNDEFINED                0x00
#define UVC_SET_CUR                 0x01
#define UVC_GET_CUR                 0x81          /* ����ҪЭ�̵��ֶε�Ĭ��ֵ*/
#define UVC_GET_MIN                 0x82          /* ����ҪЭ�̵��ֶε���Сֵ*/
#define UVC_GET_MAX                 0x83          /* ����ҪЭ�̵��ֶε����ֵ*/
#define UVC_GET_RES                 0x84
#define UVC_GET_LEN                 0x85
#define UVC_GET_INFO                    0x86
#define UVC_GET_DEF                 0x87
/* A.9.4. ����ͷ�ն˿���ѡ����*/
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

/* A.9.5. ����Ԫ����ѡ����*/
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

/* A.9.7. ��Ƶ���ӿڿؼ�ѡ����*/
#define UVC_VS_CONTROL_UNDEFINED            0x00             /* δ����ؼ�*/
#define UVC_VS_PROBE_CONTROL                0x01             /* ̽��ؼ�*/
#define UVC_VS_COMMIT_CONTROL               0x02             /* �ύ�ؼ�*/
#define UVC_VS_STILL_PROBE_CONTROL          0x03
#define UVC_VS_STILL_COMMIT_CONTROL         0x04
#define UVC_VS_STILL_IMAGE_TRIGGER_CONTROL      0x05
#define UVC_VS_STREAM_ERROR_CODE_CONTROL        0x06
#define UVC_VS_GENERATE_KEY_FRAME_CONTROL       0x07
#define UVC_VS_UPDATE_FRAME_SEGMENT_CONTROL     0x08
#define UVC_VS_SYNC_DELAY_CONTROL           0x09

/* B.1. USB�ն�����*/
#define UVC_TT_VENDOR_SPECIFIC              0x0100    /* �ض���Ӧ���ն�����*/
#define UVC_TT_STREAMING                0x0101        /* ���ն�����*/

/* B.2. �����ն�����*/
#define UVC_ITT_VENDOR_SPECIFIC             0x0200   /* ��Ӧ���ض������ն�*/
#define UVC_ITT_CAMERA                      0x0201   /* ����ͷ������������������ͷ�ն�������*/
#define UVC_ITT_MEDIA_TRANSPORT_INPUT       0x0202   /* ����ý�塣������ý�崫���ն�������*/

/* B.3. ����ն�����*/
#define UVC_OTT_VENDOR_SPECIFIC             0x0300       /* �ض���Ӧ������ն�*/
#define UVC_OTT_DISPLAY                 0x0301           /* ��ʾ����ն�*/
#define UVC_OTT_MEDIA_TRANSPORT_OUTPUT          0x0302   /* ý������ն�*/

/* 2.4.2.2. ״̬������*/
#define UVC_STATUS_TYPE_CONTROL             1
#define UVC_STATUS_TYPE_STREAMING           2

/* 2.4.3.3. ����ͷ��Ϣ*/
#define UVC_STREAM_EOH                  (1 << 7)
#define UVC_STREAM_ERR                  (1 << 6)
#define UVC_STREAM_STI                  (1 << 5)
#define UVC_STREAM_RES                  (1 << 4)
#define UVC_STREAM_SCR                  (1 << 3)
#define UVC_STREAM_PTS                  (1 << 2)
#define UVC_STREAM_EOF                  (1 << 1)
#define UVC_STREAM_FID                  (1 << 0)       /* ֡ID*/
/* --------------------------------------------------------------------------
 * UVC constants
 */

#define UVC_TERM_INPUT          0x0000                                  /* �����ն�*/
#define UVC_TERM_OUTPUT         0x8000                                  /* ����ն�*/
#define UVC_TERM_DIRECTION(term)    ((term)->type & 0x8000)             /* ����ն˷���*/

#define UVC_ENTITY_TYPE(entity)     ((entity)->type & 0x7fff)           /* ���UVCʵ������*/
#define UVC_ENTITY_IS_UNIT(entity)  (((entity)->type & 0xff00) == 0)    /* ���UVCʵ���ǲ���һ����Ԫ*/
#define UVC_ENTITY_IS_TERM(entity)  (((entity)->type & 0xff00) != 0)    /* ���UVCʵ���ǲ���һ���ն�*/
/* �ж�ʵ���ǲ��������ն�����*/
#define UVC_ENTITY_IS_ITERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_INPUT)
/* �ж�ʵ���ǲ�������ն�����*/
#define UVC_ENTITY_IS_OTERM(entity) \
    (UVC_ENTITY_IS_TERM(entity) && \
    ((entity)->type & 0x8000) == UVC_TERM_OUTPUT)

/* ------------------------------------------------------------------------
 * ȫ��Ψһ��ʶ��(Globally Unique Identifier,GUIDs)
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


/* �豸����*/
#define UVC_QUIRK_FIX_BANDWIDTH     0x00000080
#define UVC_QUIRK_STREAM_NO_FID     0x00000010

/* ��ʽ��־ */
#define UVC_FMT_FLAG_COMPRESSED     0x00000001    /* ѹ����־*/
#define UVC_FMT_FLAG_STREAM         0x00000002    /* ����־*/

#define UVC_CTRL_STREAMING_TIMEOUT  5000      /* UVC��������ʱʱ��*/

/* ��ʱUSB����������*/
#define UVC_TRPS        2
/* ÿ��USB��������������*/
#define UVC_MAX_PACKETS     32
/* �ж�URB�����״̬�����ֽڴ�С*/
#define UVC_MAX_STATUS_SIZE 16
/* UVC��ʽ������*/
struct uvc_format_desc {
    char *name;           /* ��ʽ����*/
    uint8_t guid[16];     /* ȫ��Ψһ��ʶ��*/
    uint32_t fcc;
};

/* UVC֡�ṹ��*/
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

/* UVC��ʽ�ṹ��*/
struct uvc_format {
    uint8_t type;            /* ��ʽ����*/
    uint8_t index;           /* ��ʽ����*/
    uint8_t bpp;             /* �������*/
    uint8_t colorspace;      /* ɫ��*/
    uint32_t fcc;
    uint32_t flags;          /* ��ʽ��־*/

    char name[32];           /* ��ʽ����*/

    unsigned int nframes;    /* ֡�ṹ������*/
    struct uvc_frame *frame; /* UVC֡�ṹ��*/
};

/* UVC��Ƶ��ͷ�ṹ��*/
struct uvc_streaming_header {
    uint8_t bNumFormats;
    uint8_t bEndpointAddress;
    uint8_t bTerminalLink;
    uint8_t bControlSize;
    uint8_t *bmaControls;
    /* �����ֶ�ֻ��������ͷ������*/
    uint8_t bmInfo;
    uint8_t bStillCaptureMethod;
    uint8_t bTriggerSupport;
    uint8_t bTriggerUsage;
};

/* UVC�ؼ���Ϣ�ṹ��*/
struct uvc_control_info {
    struct aw_list_head mappings;   /* �ؼ�ͼ����*/

    uint8_t entity[16];             /* UVCʵ��GUID*/
    uint8_t index;                  /* bmControls��λ����(Bit index in bmControls) */
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

/* UVCʵ��ṹ��*/
struct uvc_entity {
    struct aw_list_head list;                    /* ��ǰUVCʵ��ڵ�*/
    struct aw_list_head chain;                   /* ��Ƶ�豸���ڵ�*/

    uint32_t flags;                              /* UVCʵ���־*/

    uint8_t id;                                  /* UVCʵ��ID*/
    uint16_t type;                               /* UVCʵ������*/
    char name[64];                               /* UVCʵ������*/

    uint32_t num_pads;
    uint32_t num_links;
    struct media_pad *pads;

    union {
        /* ����ͷ��Ϣ*/
        struct {
            uint16_t wObjectiveFocalLengthMin;    /* L(min)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint16_t wObjectiveFocalLengthMax;    /* L(max)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint16_t wOcularFocalLength;          /* L(ocular)��ֵ�������֧�ֹ�ѧ�佹����Ϊ0*/
            uint8_t  bControlSize;                /* λͼ��С*/
            uint8_t  *bmControls;                 /* bmControls ��һ��λͼ����ʾ��Ƶ����ĳЩ����ͷ�ؼ��Ŀ�����*/
        } camera;
        /* ý��*/
        struct {
            uint8_t  bControlSize;
            uint8_t  *bmControls;
            uint8_t  bTransportModeSize;
            uint8_t  *bmTransportModes;
        } media;
        /* ����ն�*/
        struct {
        } output;
        /* ����Ԫ*/
        struct {
            uint16_t wMaxMultiplier;               /* ���ֱ���(���ֶ�ָʾ������ֱ����ʳ���100)*/
            uint8_t  bControlSize;                 /* �ؼ�λͼ�Ĵ�С*/
            uint8_t  *bmControls;                  /* �ؼ�λͼ��ַ*/
            uint8_t  bmVideoStandards;             /* ����Ԫ֧�ֵ�����ģ����Ƶ��׼��λͼ*/
        } processing;
        /* ѡ������Ԫ*/
        struct {
        } selector;
        /* ��չ��Ԫ*/
        struct {
            uint8_t  guidExtensionCode[16];        /* �ض���Ӧ�̵Ĵ���*/
            uint8_t  bNumControls;                 /* ��չ��Ԫ�Ŀؼ�����*/
            uint8_t  bControlSize;                 /* �ؼ�λͼ�Ĵ�С*/
            uint8_t  *bmControls;                  /* �ؼ�λͼ�ĵ�ַ*/
            uint8_t  *bmControlsType;
        } extension;
    };
    uint8_t bNrInPins;                             /* �������Ÿ���*/
    uint8_t *baSourceID;                           /* ���ӵ�ID*/

    uint32_t ncontrols;                            /* �ؼ�����*/
    struct uvc_control *controls;                  /* UVC�ؼ��ṹ��*/
};

/* UVC�ؼ��ṹ��*/
struct uvc_control {
    struct uvc_entity *entity;    /* ������UVCʵ��*/
    struct uvc_control_info info; /* �ؼ���Ϣ�ṹ��*/

    uint8_t index;                /* �� ��ƥ�� uvc_control ��ں� uvc_control_info*/
    uint8_t dirty:1,
         loaded:1,
         modified:1,
         cached:1,
         initialized:1;           /* ��ʼ����ɱ�־*/

    uint8_t *uvc_data;
};

#define UVC_QUEUE_DISCONNECTED      (1 << 0)
#define UVC_QUEUE_DROP_CORRUPTED    (1 << 1)

/* UVC��Ƶ���нṹ��*/
struct uvc_video_queue {
    aw_usb_mutex_handle_t mutex; /* ��������*/

    uint32_t flags;
    uint32_t buf_used;

    uint32_t num_buffers;         /* ������/ʹ�õĻ��������*/
    aw_spinlock_isr_t irqlock;    /* �����ж϶���*/
    struct aw_list_head irqqueue; /* �ж϶�������*/
};

/* uvc��Ƶ���ṹ��*/
struct uvc_video_chain {
    struct awbl_usbh_uvc_device *dev;  /* ������uvc�豸�ṹ��*/
    struct aw_list_head list;          /* ��ǰ��Ƶ���ڵ�*/

    struct aw_list_head entities;      /* ����ʵ�� */
    struct uvc_entity *processing;     /* ����Ԫ*/
    struct uvc_entity *selector;       /* ѡ��Ԫ*/

    aw_usb_mutex_handle_t mutex;       /* �����ؼ���Ϣ*/
    uint32_t caps;                     /* V4AW chain-wide caps */
};

/* ��Ƶ̽����ύ�ؼ�*/
struct uvc_streaming_control {
    uint16_t bmHint;                   /* ����ֻ����Щ�ֶ�Ӧ���̶ֹ���λ�ֶο���*/
    uint8_t  bFormatIndex;             /* ���Դ���Ƶ�ӿڵĸ�ʽ����������Ƶ��ʽ����*/
    uint8_t  bFrameIndex;              /* ����֡����������Ƶ֡����*/
    uint32_t dwFrameInterval;          /* ���ֶ�Ϊѡ������Ƶ����֡���������������Ƶ֡���(100nsΪ��λ��֡���)*/
    uint16_t wKeyFrameRate;            /* ÿ��Ƶ֡��λ�Ĺؼ�֡����*/
    uint16_t wPFrameRate;              /* ��֡/�ؼ�֡��λ��ʾ��֡����*/
    uint16_t wCompQuality;             /* ����Ԫѹ����������1(���)��10000(���)*/
    uint16_t wCompWindowSize;          /* ƽ�������ʿ��ƵĴ��ڴ�С*/
    uint16_t wDelay;                   /* �ڲ���Ƶ���ӿ��ӳ�(����)*/
    uint32_t dwMaxVideoFrameSize;      /* ��Ƶ֡���������ض������δ�С(�ֽ�)*/
    uint32_t dwMaxPayloadTransferSize; /* ָ���豸�ڵ�����Ч���ش����п��Է��ͻ���ܵ�����ֽ���*/
    uint32_t dwClockFrequency;         /* ָ�����ʵ��豸ʱ��Ƶ��*/
    uint8_t  bmFramingInfo;            /* �˿ؼ�����ָʾ��Ч���ش����Ƿ�����Ƶ��Ч����ͷ�а������֡��Ϣ*/
    uint8_t  bPreferedVersion;         /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ���ѡ���ظ�ʽ�汾*/
    uint8_t  bMinVersion;              /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ���С���ظ�ʽ�汾*/
    uint8_t  bMaxVersion;              /* �������豸Ϊָ����bFormatIndexֵ֧�ֵ�����ظ�ʽ�汾*/
} __attribute__((__packed__));

enum uvc_buffer_state {
    UVC_BUF_STATE_IDLE  = 0,
    UVC_BUF_STATE_QUEUED    = 1,
    UVC_BUF_STATE_ACTIVE    = 2,
    UVC_BUF_STATE_READY = 3,
    UVC_BUF_STATE_DONE  = 4,
    UVC_BUF_STATE_ERROR = 5,
};

/* USB����ṹ��*/
struct uvc_buffer {
    struct aw_list_head queue;      /* ����ڵ�*/

    enum uvc_buffer_state state;    /* ����״̬*/
    uint32_t error;                 /* �������*/

    void *mem;                      /* ����ָ��*/
    uint32_t length;                /* ���泤��*/
    uint32_t bytesused;             /* ����������ֽ���*/
    uint8_t is_used;                /* �Ƿ�ʹ�ñ�־*/

    uint32_t pts;
};

/* UVC֡״̬�ṹ��*/
struct uvc_stats_frame {
    uint32_t size;            /* ���񵽵��ֽ���*/
    uint32_t first_data;      /* ��һ���ǿհ�������*/

    uint32_t nb_packets;      /* ��������*/
    uint32_t nb_empty;        /* �հ�������*/
    uint32_t nb_invalid;      /* ������ͷ����������*/
    uint32_t nb_errors;       /* �����˴���λ�İ�������*/

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

/* UVC��״̬�ṹ��*/
struct uvc_stats_stream {
    struct aw_timespec start_ts;   /* ����ʼʱ���*/
    struct aw_timespec stop_ts;    /* ��ֹͣʱ���*/

    uint32_t nb_frames;            /* ֡������*/

    uint32_t nb_packets;           /* ��������*/
    uint32_t nb_empty;             /* �հ�������*/
    uint32_t nb_invalid;           /* ������ͷ����������*/
    uint32_t nb_errors;            /* �����˴���λ�İ�������*/

    uint32_t nb_pts_constant;      /* ������ʾʱ�����֡������*/
    uint32_t nb_pts_early;         /* Number of frames with early PTS */
    uint32_t nb_pts_initial;       /* Number of frames with initial PTS */

    uint32_t nb_scr_count_ok;   /* Number of frames with at least one SCR per non empty packet */
    uint32_t nb_scr_diffs_ok;   /* Number of frames with varying SCR.STC */
    uint32_t scr_sof_count; /* STC.SOF counter accumulated since stream start */
    uint32_t scr_sof;       /* STC.SOF of the last packet */
    uint32_t min_sof;           /* ��С STC.SOF ֵ*/
    uint32_t max_sof;           /* ��� STC.SOF ֵ */
};


#endif
