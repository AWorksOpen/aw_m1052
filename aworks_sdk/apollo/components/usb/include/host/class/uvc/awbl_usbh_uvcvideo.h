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
/* ֧��UVC shell����*/
#define UVC_SHELL_CMD
/* UVC������Ϣ*/
//#define UVC_DEBUG

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
/* n/d��ֵ����ȡ��*/
#define DIV_ROUND_UP(n,d) (((n) + (d) - 1) / (d))
#define ALIGN(x, a)    (((x) + ((a) - 1)) & ~((a) - 1))
/* UVC��Ƶ�������*/
#define UVC_BUFFER_NUM 3

#define CAMERA_USB_BUS_ID 2

/* ����ת��*/
#if (AW_CPU_ENDIAN == AW_BIG_ENDIAN)
/* С��ת���ɴ��*/
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

/* UVC��ӡ����*/
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



/* UVC�ؼ����ݵ���������*/
#define UVC_CTRL_DATA_TYPE_RAW      0
#define UVC_CTRL_DATA_TYPE_SIGNED   1
#define UVC_CTRL_DATA_TYPE_UNSIGNED 2
#define UVC_CTRL_DATA_TYPE_BOOLEAN  3
#define UVC_CTRL_DATA_TYPE_ENUM     4
#define UVC_CTRL_DATA_TYPE_BITMASK  5

/* �ؼ���־*/
#define UVC_CTRL_FLAG_SET_CUR       (1 << 0)
#define UVC_CTRL_FLAG_GET_CUR       (1 << 1)
#define UVC_CTRL_FLAG_GET_MIN       (1 << 2)
#define UVC_CTRL_FLAG_GET_MAX       (1 << 3)
#define UVC_CTRL_FLAG_GET_RES       (1 << 4)
#define UVC_CTRL_FLAG_GET_DEF       (1 << 5)
/* �ؼ�Ӧ���ڹ���ʱ���棬�ڻָ�ʱ��ԭ*/
#define UVC_CTRL_FLAG_RESTORE       (1 << 6)
/* �ؼ�����ͨ������ͷ���� */
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

/* UVC�豸�ṹ��*/
struct awbl_usbh_uvc_device{
    struct awbl_usbh_function *uvc_fun;               /* ��ص�UBS���ܽṹ��(����һ��USB�豸����)*/
    struct awbl_usbh_interface *uvc_controlitf;       /* UVC���ƽӿ�����(����UVC�淶1.5��ÿ����Ƶ����ֻ��һ�����ƽӿ�)*/
    AW_MUTEX_DECL(               mutex);              /* ������*/
    uint16_t uvc_version;                             /* UVC�淶�汾*/
    uint32_t clock_frequency;                         /* ʱ��Ƶ��*/
    uint32_t itfstreaming_altsetting_num;             /* UVC���ӿڱ������õ�����*/
    struct aw_list_head streams;                      /* ��Ƶ������*/
    struct aw_list_head entities;                     /* UVCʵ������(������Ԫ���ն�)*/
    struct aw_list_head chains;                       /* ��Ƶ�豸����*/
    int nstreams;                                     /* ��Ƶ������*/
    struct awbl_usbh_trp  int_trp;                    /* �жϴ��������*/
    struct awbl_usbh_endpoint *int_ep;                /* �ж϶˵�*/
    uint8_t *status;                                  /* UVC�豸״̬����*/
    uint32_t quirks;                                  /* �豸����*/
};


/* UVC��Ƶ���ṹ��*/
struct awbl_usbh_uvc_streaming {
    struct aw_list_head list;                         /* UVC���ṹ������*/
    struct awbl_usbh_uvc_device *dev;                 /* UVC�豸�ṹ��*/
    struct awbl_usbh_interface *intf;                 /* USB�ӿڽṹ��*/
    int intfnum;                                      /* �ӿڱ��*/
    struct uvc_streaming_header header;               /* UVC��Ƶ��ͷ�ṹ��*/
    aw_usb_mutex_handle_t lock;                       /* �豸������*/
    uint32_t nformats;                                /* ��ʽ������*/
    struct uvc_format *format;                        /* UVC��ʽ�ṹ��*/
    enum v4aw_buf_type type;                          /* video4AWorks ����*/
    uint16_t maxpsize;                                /* �˵�������С*/
    struct uvc_video_chain *chain;                    /* ��Ƶ������*/
    struct uvc_streaming_control ctrl;                /* UVC�����ƽṹ��*/
    struct uvc_format *def_format;                    /* Ĭ�ϸ�ʽ*/
    struct uvc_format *cur_format;                    /* ��ǰ��ʽ*/
    struct uvc_frame *cur_frame;                      /* ��ǰ֡*/
    char *camera_serv_name;                           /* ����ͷ��������*/
    struct uvc_video_queue queue;                     /* UVC��Ƶ����*/
    uint32_t sequence;                                /* ���к�*/
    uint8_t last_fid;                                 /* ���֡ID*/
    uint8_t frozen;                                   /* �����־λ*/
    struct awbl_usbh_trp *trp[UVC_TRPS];              /* USB�����*/
    uint32_t trp_size;                                /* USB������С*/
    char *trp_buffer[UVC_TRPS];                       /* USB����黺��*/
    /* ����ص�����*/
    void (*decode) (struct awbl_usbh_trp *trp, struct awbl_usbh_uvc_streaming *video,
            struct uvc_buffer *buf);

    struct {
        struct uvc_stats_frame frame;
        struct uvc_stats_stream stream;
    } stats;

    /* ������ɴ������ʹ�õ�����������*/
    struct {
        uint8_t header[256];
        uint32_t header_size;
        int skip_payload;
        uint32_t payload_size;
        uint32_t max_payload_size;
    } bulk;

    /* ʱ���֧��*/
    struct uvc_clock {                                /* UVCʱ�ӽṹ��*/
        struct uvc_clock_sample {                     /* UVCʱ�Ӳ����ṹ��*/
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
        aw_spinlock_isr_t lock;                       /* UVCʱ��������*/
    } clock;
};

/* ���ú�������*/
uint16_t get_unaligned_le16(uint8_t *p);
uint32_t get_unaligned_le32(uint8_t *p);
/* UVC�ؼ���غ�������*/
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

/* ����ͷ����ص�����*/
aw_err_t __uvc_open(void *p_cookie);
aw_err_t __uvc_close(void *p_cookie);
aw_err_t __uvc_get_photo_buf(void *p_cookie, uint32_t **p_buf);
aw_err_t __uvc_op_get_video_buf(void *p_cookie, uint32_t **p_buf);
aw_err_t __uvc_op_release_video_buf(void *p_cookie, uint32_t addr);
aw_err_t __uvc_release_photo_buf(void *p_cookie, uint32_t addr);
aw_err_t __uvc_cfg_set(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);
aw_err_t __uvc_cfg_get(void *p_cookie, aw_camera_cfg_t *p_cfg, uint32_t *option);
/* UVC������غ�������*/
int uvc_queue_init(struct uvc_video_queue *queue, enum v4aw_buf_type type);
int uvc_buffer_init(struct uvc_video_queue *queue, uint32_t buffer_size, uint8_t buffer_num);
int uvc_queue_allocated(struct uvc_video_queue *queue);
int uvc_video_enable(struct awbl_usbh_uvc_streaming *stream, int enable);
int uvc_queue_streamon(struct uvc_video_queue *queue, enum v4aw_buf_type type);
#endif

