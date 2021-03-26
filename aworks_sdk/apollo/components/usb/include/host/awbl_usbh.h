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

#ifndef __AWBL_USBH_H
#define __AWBL_USBH_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "aw_refcnt.h"
#include "aw_list.h"
#include "aw_delayed_work.h"
#include "aw_usb_common.h"
#include "aw_usb_os.h"



#define __AWBL_USBH_DECL(p_hc, p_awdev) \
    struct awbl_usbh *p_hc = \
        (struct awbl_usbh *)AW_CONTAINER_OF( \
            p_awdev, struct awbl_usbh, super)

#define __AWBL_USBH_INFO_DECL(p_info, p_awdev) \
    struct awbl_usbh_info *p_info = \
        (struct awbl_usbh_info *)AWBL_DEVINFO_GET(p_awdev)

#define __AWBL_USBH_FUN_DECL(p_fun, p_awdev) \
    struct awbl_usbh_function *p_fun = \
        (struct awbl_usbh_function *)AW_CONTAINER_OF( \
            p_awdev, struct awbl_usbh_function, awdev)

#define __AWBL_USBH_FUN_DRV(p_drv, p_awdrv) \
    struct awbl_usbh_fundrv_info *p_drv = \
        (struct awbl_usbh_fundrv_info *)AW_CONTAINER_OF( \
            p_awdrv, struct awbl_usbh_fundrv_info, awdrv)


/* USB������������*/
#define AWBL_USBH_ROOT_HUB_NUM     2
/* USB���������˿�����*/
#define AWBL_USBH_HUB_MAX_PORTS    15
/* USB�豸���ӿ�����*/
#define AWBL_USBH_MAX_INTERFACES   32
/* USB�豸�����������*/
#define AWBL_USBH_MAX_CONFIG       5
/* USB�豸���˵���Ŀ*/
#define AWBL_USBH_MAX_EP_NUM       32


#define AWBL_USBH_TRP_SHORT_NOT_OK  0x00000001    /* Short packets are errors */
#define AWBL_USBH_TRP_ISO_ASAP      0x00000002    /* Start ISO transfer at the earliest */
#define AWBL_USBH_TRP_ZERO_PACKET   0x00000004    /* ��0��������������*/
#define AWBL_USBH_TRP_NO_INTERRUPT  0x00000008    /* no interrupt needed,except for error */



#define AWBL_USBH_FUNDRV_INFO_END  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}




struct awbl_usbh_hub;
struct awbl_usbh;
struct awbl_usbh_config;
struct awbl_usbh_interface;
struct awbl_usbh_device;
struct awbl_usbh_endpoint;
struct awbl_usbh_function;



/** USB��ʱ��������*/
struct usb_iso_packet_descriptor {
    uint32_t offset;         /* ƫ��*/
    uint32_t  length;        /* ��������ĳ���*/
    uint32_t actual_length;  /* ʵ�ʴ���ĳ���*/
    int status;              /* ״̬*/
};


/** USB �������������*/
struct awbl_usbh_trp {
    struct awbl_usbh_endpoint        *p_ep;                    /* �����������ض˵�*/
    struct aw_usb_ctrlreq            *p_ctrl;                  /* SETUP��*/
    void                             *p_data;                  /* ���ݻ���*/
    size_t                            len;                     /* Ҫ����ĳ���*/
    void                             *p_ctrl_dma;              /* SETUP��ӳ���DMA�ڴ�*/
    void                             *p_data_dma;              /* ���ݹ���ӳ���DMA�ڴ�*/
    void                            (*pfn_done) (void *p_arg); /* ������ɻص�����*/
    void                             *p_arg;                   /* ������ɻص���������*/
    size_t                            act_len;                 /* ʵ�ʴ��䳤��*/
    int                               status;                  /* ����״̬*/
    int                               flag;                    /* �����־*/
    void                             *priv;                    /* �û�˽������*/
    int                               interval;                /* ����ʱ������(�ж�/��ʱ)*/
    void                             *hcpriv;                  /* ������������˽������*/
    int                               error_count;             /* (����)��ʱ����Ĵ�������*/
    int                               start_frame;             /* ��ʱ��ʼ֡*/
    int                               number_of_packets;       /* (����)ͬ����������(add by CYX at 9/17-2019)*/
    struct usb_iso_packet_descriptor *iso_frame_desc;          /* ��ʱ��������(add by CYX at 9/17-2019)*/
    struct aw_list_head               node;                    /* ��ǰUSB����������Ľڵ�*/
};

#define AWBL_USBDEV_INJECT 1         /* �����־*/
#define AWBL_USBDEV_EJECT  2         /* �γ���־*/
#define AWBL_USBDEV_IGONRE_PID  0    /* ����ʹ��PID*/
#define AWBL_USBDEV_IGONRE_VID  0    /* ����ʹ��VID*/

typedef void* awbl_usbh_monitor_handle;

/** USB�豸���ӽṹ��*/
struct awbl_usbh_dev_monitor {
    uint16_t pid;                                          /* ��ص��豸��PID*/
    uint16_t vid;                                          /* ��ص��豸��VID*/
    uint8_t type;                                          /* �������*/
    void (*user_callback) (void *p_arg, uint8_t evt_type); /* �û��ص�����*/
    void *p_arg;                                           /* �ص���������*/
    struct aw_list_head node;                              /* �ڵ�*/
    uint8_t dev_type;                                      /* �豸����*/
    struct aw_delayed_work dly_work;                       /* ��ʱ����*/
    uint8_t state;                                         /* ״̬*/
};

/** USB �������ṹ��*/
struct awbl_usbh_hub {
    struct awbl_usbh_function   *p_fun;
    struct aw_list_head          evt;
    struct aw_list_head          node;
    struct awbl_usbh_trp         trp;
    struct awbl_usbh_device     *ports[AWBL_USBH_HUB_MAX_PORTS];
    uint32_t                    *p_sta;
    uint8_t                      err_cnt;
    uint8_t                      nports;
    uint8_t                      pwrtim;
};






/** USB���������ص�����*/
struct awbl_usbh_drv {
    /* ������*/
    aw_err_t (*pfn_request) (struct awbl_usbh     *p_hc,
                             struct awbl_usbh_trp *p_trp);

    /* ȡ������*/
    aw_err_t (*pfn_cancel) (struct awbl_usbh     *p_hc,
                            struct awbl_usbh_trp *p_trp);

    /* ʹ�ܶ˵�*/
    aw_err_t (*pfn_ep_en) (struct awbl_usbh          *p_hc,
                           struct awbl_usbh_endpoint *p_ep);

    /* ���ܶ˵�*/
    aw_err_t (*pfn_ep_dis) (struct awbl_usbh          *p_hc,
                            struct awbl_usbh_endpoint *p_ep);

    /* ��ȡ��������״̬*/
    aw_err_t (*pfn_rh_check) (struct awbl_usbh *p_hc,
                              uint16_t         *p_change);

    /* ������������*/
    aw_err_t (*pfn_rh_ctrl) (struct awbl_usbh        *p_hc,
                             uint8_t                  type,
                             uint8_t                  req,
                             uint16_t                 val,
                             uint16_t                 idx,
                             uint16_t                 len,
                             void                    *p_data);
    /* ��ȡ��ǰ֡���*/
    int (*get_current_frame_number)(struct awbl_usbh *p_hc);

    void (*disable_periodic)(struct awbl_usbh *p_hc);

    void (*enable_periodic)(struct awbl_usbh *p_hc);
};




/** \brief USB host */
struct awbl_usbh {
    struct awbl_busctlr         super;
    aw_usb_mutex_handle_t       lock;
    uint32_t                    map[4];
    struct awbl_usbh_hub        rh;
    struct aw_delayed_work      rh_work;
    const struct awbl_usbh_drv *p_drv;
    struct awbl_usbh           *p_next;
};




/** \brief USB host information */
struct awbl_usbh_info {
    uint8_t     bus_index;
};




/* �����˵�ṹ��*/
struct awbl_usbh_endpoint {
    struct awbl_usbh_device     *p_udev;    /* �����豸�ṹ��*/
    struct aw_usb_endpoint_desc *p_desc;    /* USB�˵�����������*/
    aw_bool_t                    enabled;   /* ʹ�ܱ�־λ*/
    int                          bandwidth; /* �˵��������*/
    int                          interval;  /* ����ʱ������(�ж�/��ʱ)*/
    struct aw_list_head          node;      /* ���˵�ṹ��ڵ�*/
    aw_usb_sem_handle_t          sem;       /* �ź���*/
    void                        *priv;      /* ˽��������*/
    int                          extralen;  /* ������������ĳ���*/
    uint8_t                     *extra;     /* �����������(���磬�ض������������ض�����������) */
};



/* USB���ýṹ��*/
struct awbl_usbh_config {
    struct aw_usb_config_desc       *p_desc;    /* ����������*/
    char                            *string;    /* ��������*/
    struct awbl_usbh_function       *funs;      /* �ӿڹ��ܽṹ��*/
    int                              nfuns;     /* �ӿڹ�����*/
    int                              extralen;  /* ������������ĳ���*/
    uint8_t                         *extra;     /* �����������(���磬�ض������������ض�����������) */
};



/* USB�豸���������ṹ��*/
struct awbl_usbh_quirk_id {
    uint16_t pid;
    uint16_t vid;
    uint16_t driver_info;
};

struct awbl_usbh_dev_spec_config_info{
    uint16_t pid;
    uint16_t vid;
    uint16_t config_num;
};

/** �豸����*/
enum{
    AWBL_USBH_DEV_UNKNOWN = 0,   /* δ֪����*/
    AWBL_USBH_DEV_UMS     = 1,   /* USB�������洢�豸*/
    AWBL_USBH_DEV_UVC     = 2,   /* USB����ͷ�豸*/
    AWBL_USBH_DEV_USERIAL = 3,   /* USB�����豸*/
    AWBL_USBH_DEV_UNIC    = 4,   /* USB���翨*/
    AWBL_USBH_DEV_UHID    = 5,   /* USB HID�豸*/
};

/** USB�豸�ṹ��*/
struct awbl_usbh_device {
    struct awbl_usbh               *p_hc;                     /* USB�����ṹ��*/
    char                            name[32];                 /* �豸����*/
    struct aw_refcnt                ref;                      /* �豸���ü���*/
    aw_usb_mutex_handle_t           lock;                     /* �豸������*/
    struct aw_list_head             node;                     /* ��ǰ�豸�ṹ��ڵ�*/
    uint8_t                         addr;                     /* �豸��ַ*/
    uint8_t                         speed;                    /* �豸�ٶ�*/
    struct awbl_usbh_hub           *p_hub;                    /* ����������*/
    uint8_t                         port;                     /* �����������˿ں�*/
    uint8_t                         status;                   /* �豸״̬*/
#define AWBL_USBH_DEV_STA_CONN       0x01                      /* �豸������״̬*/
#define AWBL_USBH_DEV_STA_INJECT     0x02                      /* �豸�Ѳ���״̬*/
#define AWBL_USBH_DEV_STA_CFG        0x04                      /* �豸������״̬*/
#define AWBL_USBH_DEV_STA_CHANGE_CFG 0x08                      /* �豸��ı�����*/
/* �ж��豸�Ƿ�������״̬*/
#define AWBL_USBH_DEV_CONN(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_CONN)
/* �ж��豸�Ƿ��ǲ���״̬*/
#define AWBL_USBH_DEV_INJECT(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_INJECT)
/* �ж��豸�Ƿ�������״̬*/
#define AWBL_USBH_DEV_CFG(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_CFG)
/* �ж��豸״̬�Ƿ���Ч*/
#define AWBL_USBH_DEV_VALID(p_dev) \
            ((p_dev)->status == (AWBL_USBH_DEV_STA_CONN | \
                                 AWBL_USBH_DEV_STA_CFG | \
                                 AWBL_USBH_DEV_STA_INJECT))
    struct aw_usb_ctrlreq          *p_ctrl;                   /* ��������(SETUP)*/
    struct aw_usb_device_desc      *p_desc;                   /* USB�豸����������*/
    char                           *p_str_mft;                /* �豸������*/
    char                           *p_str_pdt;                /* �豸����*/
    char                           *p_str_snum;               /* �豸���к�*/
    struct awbl_usbh_config         cfg;                      /* USB�豸�����ýṹ��*/
    struct awbl_usbh_endpoint       ep0;                      /* �˵�0*/
    struct aw_usb_endpoint_desc     ep0_desc;                 /* �˵�0����������*/
    struct awbl_usbh_endpoint      *ep_in[16];                /* �豸����˵�*/
    struct awbl_usbh_endpoint      *ep_out[16];               /* �豸����˵�*/
    uint16_t                        langid;                   /* ����ID*/
    uint32_t                        quirks;                   /* �豸����*/
    uint8_t                         dev_type;                 /* �豸����*/
};




/** USB���ܽṹ��*/
struct awbl_usbh_function {
    struct awbl_dev                      awdev;       /* �����豸��*/
    struct awbl_devhcf                   devhcf;      /* �豸Ӳ����Ϣ*/
    char                                 name[16];    /* �豸����*/
    struct awbl_usbh_device             *p_udev;      /* ��ص�USB�豸�ṹ��*/
    struct aw_list_head                  node;        /* �����ܽڵ�*/

    uint8_t                              ifirst;      /* ��һ���ӿڵı��*/
    uint8_t                              nitfs;       /* �ӿڵ�����*/
    uint8_t                              nitfs_type;  /* �ӿ����͵�����(��ͨ�ӿڸ�ֵΪ1�����Ͻӿڸ�ֵΪ���Ͻӿڰ����Ľӿ�����)*/
    uint8_t                              clss;        /* ��*/
    uint8_t                              sclss;       /* ����*/
    uint8_t                              proto;       /* Э��*/
    uint8_t                              type;        /* �ӿ�����(��ͨ�ӿڻ����Ͻӿ�)*/

    struct aw_list_head                  itfs;        /* �ӿ�����*/
    void                                *prive;       /* ����˽����Ϣ*/
};




/** USB�ӿڽṹ��*/
struct awbl_usbh_interface {
    struct aw_list_head              node;      /* ��ǰ�ӿڽڵ�*/
    struct aw_usb_interface_desc    *p_desc;    /* �ӿ�������*/
    struct awbl_usbh_endpoint       *eps;       /* �ӿ��µĶ˵�*/
    int                              extralen;  /* ������������ĳ���*/
    uint8_t                         *extra;     /* �����������(���磬�ض������������ض�����������) */
};


/* USB����������Ϣƥ���*/
struct awbl_usbh_fundrv_info_tab {
    uint16_t                    flags;                 /* ƥ���־*/
#define AWBL_USBH_FUNDRV_MATCH_VENDOR          0x0001  /* ƥ��VID*/
#define AWBL_USBH_FUNDRV_MATCH_PRODUCT         0x0002  /* ƥ��PID*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_CLASS       0x0010  /* ƥ���豸�����*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS    0x0020  /* ƥ���豸�������*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL    0x0040  /* ƥ���豸Э�����*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_CLASS       0x0080  /* ƥ��ӿ������*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS    0x0100  /* ƥ��ӿ��������*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL    0x0200  /* ƥ��ӿ�Э�����*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER      0x0400  /* ƥ���豸�ӿ�����*/
    uint16_t                    vid;
    uint16_t                    pid;

    uint8_t                     dclss;
    uint8_t                     dsclss;
    uint8_t                     dproto;

    uint8_t                     iclss;
    uint8_t                     isclss;
    uint8_t                     iproto;
    uint8_t                     inum;
};




/** \brief USB device driver info */
struct awbl_usbh_fundrv_info {
    struct awbl_drvinfo                     awdrv;
    const struct awbl_usbh_fundrv_info_tab *p_tab;
};



#define AWBL_USBH_EP_TYPE(p_ep)   ((p_ep)->p_desc->bmAttributes & 0x03)
#define AWBL_USBH_EP_DIR(p_ep)    ((p_ep)->p_desc->bEndpointAddress & 0x80)
#define AWBL_USBH_EP_INT(p_ep)    (((p_ep)->p_desc->bmAttributes & AW_USB_EP_TYPE_INT) == AW_USB_EP_TYPE_INT)
#define AWBL_USBH_EP_NUM(p_ep)    ((p_ep)->p_desc->bEndpointAddress & 0x0F)
#define AWBL_USBH_EP_MPS(p_ep)    (AW_USB_CPU_TO_LE16((p_ep)->p_desc->wMaxPacketSize))
#define AWBL_USBH_EP_INTVAL(p_ep) ((p_ep)->p_desc->bInterval)

#define AWBL_USBH_DEV_VID(p_udev)           (AW_USB_CPU_TO_LE16((p_udev)->p_desc->idVendor))
#define AWBL_USBH_DEV_PID(p_udev)           (AW_USB_CPU_TO_LE16((p_udev)->p_desc->idProduct))
#define AWBL_USBH_DEV_CLASS(p_udev)         ((p_udev)->p_desc->bDeviceClass)
#define AWBL_USBH_DEV_SUB_CLASS(p_udev)     ((p_udev)->p_desc->bDeviceSubClass)
#define AWBL_USBH_DEV_PROTOCOL(p_udev)      ((p_udev)->p_desc->bDeviceProtocol)
#define AWBL_USBH_DEV_NCFGS(p_udev)         ((p_udev)->p_desc->bNumConfigurations)

static inline int AWBL_USBH_EP_IS_INT_IN( const struct awbl_usbh_endpoint *ep)
{
    return AWBL_USBH_EP_DIR(ep) && AWBL_USBH_EP_INT(ep);
}



/**
 * \brief USB �������ߺ���
 */

/** \brief USB �������߳�ʼ�� */
void awbl_usbh_bus_init (void);

/** \brief USB ������Դ��ʼ�� */
aw_err_t awbl_usbh_lib_init (int hub_stack_size, int hub_task_prio);

/** \brief Ѱ����һ��������*/
int find_next_descriptor(unsigned char *buffer, int size, int dt1, int dt2, int *num_skipped);

/**
 * \brief USB host API for host driver
 */
/** \brief create a USB host */
aw_err_t awbl_usbh_host_create (struct awbl_usbh           *p_hc,
                                uint8_t                     nports,
                                const struct awbl_usbh_drv *p_drv);

/** \brief root hub status change */
void awbl_usbh_host_rh_change (struct awbl_usbh *p_hc, uint16_t change);

/** \brief finish */
void awbl_usbh_host_trp_done (struct awbl_usbh_trp *p_trp, int status);




/**
 * \brief USB device API for all driver
 */
/** \brief Ѱ��USB�豸*/
void *awbl_usbh_dev_find(uint16_t vid, uint16_t pid, uint8_t dev_type, aw_bool_t is_print);
/** \brief ģ���Ȳ��*/
aw_err_t awbl_usbh_hubplug(void *udev);
/** \brief ��ȡ�����豸�����ñ��*/
int awbl_usbh_dev_get_confignum(uint16_t vid, uint16_t pid);
/** \brief ����һ��USB�豸*/
aw_err_t awbl_usbh_dev_get (struct awbl_usbh_device *p_udev);
/** \brief ȡ����USB�豸������*/
aw_err_t awbl_usbh_dev_put (struct awbl_usbh_device *p_udev);
/** \brief ����豸�Ƿ�����*/
aw_bool_t awbl_usbh_dev_is_connect (struct awbl_usbh_device *p_udev);
/** \brief ��λһ���˵� */
aw_err_t awbl_usbh_ep_reset(struct awbl_usbh_device *p_udev, unsigned int epaddr);
/** \brief ����˵�ֹͣ״̬*/
aw_err_t awbl_usbh_ep_halt_clr (struct awbl_usbh_endpoint *p_ep);
/** \brief ע���豸��غ���*/
awbl_usbh_monitor_handle awbl_usbh_dev_monitor_register(uint16_t vid, uint16_t pid,
                                                        uint8_t dev_type, uint8_t monitor_type,
                                                        void (*user_callback) (void *p_arg, uint8_t type),
                                                        void *p_arg);
/** \brief ע���豸��غ���*/
aw_err_t awbl_usbh_dev_monitor_unregister(awbl_usbh_monitor_handle handle);
int awbl_usbh_host_get_current_frame_number(struct awbl_usbh *p_hc);


/** \brief submit a TRP */
aw_err_t awbl_usbh_trp_submit (struct awbl_usbh_trp *p_trp);

/** \brief cancel a TRP */
aw_err_t awbl_usbh_trp_cancel (struct awbl_usbh_trp *p_trp);

/** \brief submit a TRP and wait for it */
aw_err_t awbl_usbh_trp_sync (struct awbl_usbh_endpoint *p_ep,
                             struct aw_usb_ctrlreq     *p_ctrl,
                             void                      *p_data,
                             int                        len,
                             int                        timeout,
                             int                        flag);

/** \brief submit a control TRP and wait for it */
aw_err_t awbl_usbh_ctrl_sync (struct awbl_usbh_endpoint *p_ep,
                              uint8_t                    type,
                              uint8_t                    req,
                              uint16_t                   val,
                              uint16_t                   idx,
                              uint16_t                   len,
                              void                      *p_data,
                              int                        timeout,
                              int                        flag);


/** \brief get descriptor from device */
aw_err_t awbl_usbh_desc_get (struct awbl_usbh_device *p_dev,
                             uint16_t                 type,
                             uint16_t                 value,
                             uint16_t                 index,
                             uint16_t                 len,
                             void                    *p_buf);

/** \brief set device address */
aw_err_t awbl_usbh_addr_set (struct awbl_usbh_device *p_dev);

/** \brief get string */
char *awbl_usbh_string_get (struct awbl_usbh_device *p_udev,
                            uint8_t                  index);

/** \brief set a new config for device */
aw_err_t awbl_usbh_dev_cfg_set (struct awbl_usbh_device *p_dev,
                                uint8_t                  val);
/** \brief ����USB�豸�ӿ� */
int awbl_usbh_set_interface(struct awbl_usbh_function *ufun ,int intfnum, int altnum);

/**
 * \brief USB host hub driver
 */
/** \brief USB host hub driver register */
void awbl_usbh_hub_drv_register (void);





aw_usb_task_handle_t aw_usb_task_create (const char  *name,
                                         int          prio,
                                         size_t       stk_s,
                                         void       (*pfnc) (void *p_arg),
                                         void        *p_arg);

aw_err_t aw_usb_task_delete (aw_usb_task_handle_t tsk);

aw_err_t aw_usb_task_startup (aw_usb_task_handle_t tsk);

aw_usb_sem_handle_t aw_usb_sem_create (void);

void aw_usb_sem_delete (aw_usb_sem_handle_t sem);

aw_err_t aw_usb_sem_take (aw_usb_sem_handle_t sem,
                          int                 timeout);

aw_err_t aw_usb_sem_give (aw_usb_sem_handle_t sem);

aw_usb_mutex_handle_t aw_usb_mutex_create (void);

void aw_usb_mutex_delete (aw_usb_mutex_handle_t mutex);

aw_err_t aw_usb_mutex_lock (aw_usb_mutex_handle_t mutex,
                            int                   timeout);

aw_err_t aw_usb_mutex_unlock (aw_usb_mutex_handle_t mutex);


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_H */
