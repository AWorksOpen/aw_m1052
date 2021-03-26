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


/* USB根集线器数量*/
#define AWBL_USBH_ROOT_HUB_NUM     2
/* USB集线器最大端口数量*/
#define AWBL_USBH_HUB_MAX_PORTS    15
/* USB设备最大接口数量*/
#define AWBL_USBH_MAX_INTERFACES   32
/* USB设备最大配置数量*/
#define AWBL_USBH_MAX_CONFIG       5
/* USB设备最大端点数目*/
#define AWBL_USBH_MAX_EP_NUM       32


#define AWBL_USBH_TRP_SHORT_NOT_OK  0x00000001    /* Short packets are errors */
#define AWBL_USBH_TRP_ISO_ASAP      0x00000002    /* Start ISO transfer at the earliest */
#define AWBL_USBH_TRP_ZERO_PACKET   0x00000004    /* 用0包结束批量传输*/
#define AWBL_USBH_TRP_NO_INTERRUPT  0x00000008    /* no interrupt needed,except for error */



#define AWBL_USBH_FUNDRV_INFO_END  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}




struct awbl_usbh_hub;
struct awbl_usbh;
struct awbl_usbh_config;
struct awbl_usbh_interface;
struct awbl_usbh_device;
struct awbl_usbh_endpoint;
struct awbl_usbh_function;



/** USB等时包描述符*/
struct usb_iso_packet_descriptor {
    uint32_t offset;         /* 偏移*/
    uint32_t  length;        /* 期望传输的长度*/
    uint32_t actual_length;  /* 实际传输的长度*/
    int status;              /* 状态*/
};


/** USB 主机传输请求包*/
struct awbl_usbh_trp {
    struct awbl_usbh_endpoint        *p_ep;                    /* 传输请求包相关端点*/
    struct aw_usb_ctrlreq            *p_ctrl;                  /* SETUP包*/
    void                             *p_data;                  /* 数据缓存*/
    size_t                            len;                     /* 要传输的长度*/
    void                             *p_ctrl_dma;              /* SETUP包映射的DMA内存*/
    void                             *p_data_dma;              /* 数据过程映射的DMA内存*/
    void                            (*pfn_done) (void *p_arg); /* 传输完成回调函数*/
    void                             *p_arg;                   /* 传输完成回调函数参数*/
    size_t                            act_len;                 /* 实际传输长度*/
    int                               status;                  /* 传输状态*/
    int                               flag;                    /* 传输标志*/
    void                             *priv;                    /* 用户私有数据*/
    int                               interval;                /* 传输时间周期(中断/等时)*/
    void                             *hcpriv;                  /* 主机控制器的私有数据*/
    int                               error_count;             /* (返回)等时传输的错误数量*/
    int                               start_frame;             /* 等时起始帧*/
    int                               number_of_packets;       /* (输入)同步包的数量(add by CYX at 9/17-2019)*/
    struct usb_iso_packet_descriptor *iso_frame_desc;          /* 等时包描述符(add by CYX at 9/17-2019)*/
    struct aw_list_head               node;                    /* 当前USB传输请求包的节点*/
};

#define AWBL_USBDEV_INJECT 1         /* 插入标志*/
#define AWBL_USBDEV_EJECT  2         /* 拔出标志*/
#define AWBL_USBDEV_IGONRE_PID  0    /* 忽略使用PID*/
#define AWBL_USBDEV_IGONRE_VID  0    /* 忽略使用VID*/

typedef void* awbl_usbh_monitor_handle;

/** USB设备监视结构体*/
struct awbl_usbh_dev_monitor {
    uint16_t pid;                                          /* 监控的设备的PID*/
    uint16_t vid;                                          /* 监控的设备的VID*/
    uint8_t type;                                          /* 监控类型*/
    void (*user_callback) (void *p_arg, uint8_t evt_type); /* 用户回调函数*/
    void *p_arg;                                           /* 回调函数参数*/
    struct aw_list_head node;                              /* 节点*/
    uint8_t dev_type;                                      /* 设备类型*/
    struct aw_delayed_work dly_work;                       /* 延时工作*/
    uint8_t state;                                         /* 状态*/
};

/** USB 集线器结构体*/
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






/** USB主机驱动回调函数*/
struct awbl_usbh_drv {
    /* 请求传输*/
    aw_err_t (*pfn_request) (struct awbl_usbh     *p_hc,
                             struct awbl_usbh_trp *p_trp);

    /* 取消传输*/
    aw_err_t (*pfn_cancel) (struct awbl_usbh     *p_hc,
                            struct awbl_usbh_trp *p_trp);

    /* 使能端点*/
    aw_err_t (*pfn_ep_en) (struct awbl_usbh          *p_hc,
                           struct awbl_usbh_endpoint *p_ep);

    /* 禁能端点*/
    aw_err_t (*pfn_ep_dis) (struct awbl_usbh          *p_hc,
                            struct awbl_usbh_endpoint *p_ep);

    /* 获取根集线器状态*/
    aw_err_t (*pfn_rh_check) (struct awbl_usbh *p_hc,
                              uint16_t         *p_change);

    /* 根集线器控制*/
    aw_err_t (*pfn_rh_ctrl) (struct awbl_usbh        *p_hc,
                             uint8_t                  type,
                             uint8_t                  req,
                             uint16_t                 val,
                             uint16_t                 idx,
                             uint16_t                 len,
                             void                    *p_data);
    /* 获取当前帧编号*/
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




/* 主机端点结构体*/
struct awbl_usbh_endpoint {
    struct awbl_usbh_device     *p_udev;    /* 所属设备结构体*/
    struct aw_usb_endpoint_desc *p_desc;    /* USB端点描述符定义*/
    aw_bool_t                    enabled;   /* 使能标志位*/
    int                          bandwidth; /* 端点所需带宽*/
    int                          interval;  /* 传输时间周期(中断/等时)*/
    struct aw_list_head          node;      /* 本端点结构体节点*/
    aw_usb_sem_handle_t          sem;       /* 信号量*/
    void                        *priv;      /* 私有数据域*/
    int                          extralen;  /* 额外的描述符的长度*/
    uint8_t                     *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};



/* USB配置结构体*/
struct awbl_usbh_config {
    struct aw_usb_config_desc       *p_desc;    /* 配置描述符*/
    char                            *string;    /* 配置描述*/
    struct awbl_usbh_function       *funs;      /* 接口功能结构体*/
    int                              nfuns;     /* 接口功能数*/
    int                              extralen;  /* 额外的描述符的长度*/
    uint8_t                         *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};



/* USB设备兼容描述结构体*/
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

/** 设备类型*/
enum{
    AWBL_USBH_DEV_UNKNOWN = 0,   /* 未知类型*/
    AWBL_USBH_DEV_UMS     = 1,   /* USB大容量存储设备*/
    AWBL_USBH_DEV_UVC     = 2,   /* USB摄像头设备*/
    AWBL_USBH_DEV_USERIAL = 3,   /* USB串口设备*/
    AWBL_USBH_DEV_UNIC    = 4,   /* USB网络卡*/
    AWBL_USBH_DEV_UHID    = 5,   /* USB HID设备*/
};

/** USB设备结构体*/
struct awbl_usbh_device {
    struct awbl_usbh               *p_hc;                     /* USB主机结构体*/
    char                            name[32];                 /* 设备名字*/
    struct aw_refcnt                ref;                      /* 设备引用计数*/
    aw_usb_mutex_handle_t           lock;                     /* 设备互斥锁*/
    struct aw_list_head             node;                     /* 当前设备结构体节点*/
    uint8_t                         addr;                     /* 设备地址*/
    uint8_t                         speed;                    /* 设备速度*/
    struct awbl_usbh_hub           *p_hub;                    /* 所属集线器*/
    uint8_t                         port;                     /* 所属集线器端口号*/
    uint8_t                         status;                   /* 设备状态*/
#define AWBL_USBH_DEV_STA_CONN       0x01                      /* 设备已连接状态*/
#define AWBL_USBH_DEV_STA_INJECT     0x02                      /* 设备已插入状态*/
#define AWBL_USBH_DEV_STA_CFG        0x04                      /* 设备已配置状态*/
#define AWBL_USBH_DEV_STA_CHANGE_CFG 0x08                      /* 设备想改变配置*/
/* 判断设备是否是连接状态*/
#define AWBL_USBH_DEV_CONN(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_CONN)
/* 判断设备是否是插入状态*/
#define AWBL_USBH_DEV_INJECT(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_INJECT)
/* 判断设备是否是配置状态*/
#define AWBL_USBH_DEV_CFG(p_dev) \
            ((p_dev)->status & AWBL_USBH_DEV_STA_CFG)
/* 判断设备状态是否有效*/
#define AWBL_USBH_DEV_VALID(p_dev) \
            ((p_dev)->status == (AWBL_USBH_DEV_STA_CONN | \
                                 AWBL_USBH_DEV_STA_CFG | \
                                 AWBL_USBH_DEV_STA_INJECT))
    struct aw_usb_ctrlreq          *p_ctrl;                   /* 控制请求(SETUP)*/
    struct aw_usb_device_desc      *p_desc;                   /* USB设备描述符定义*/
    char                           *p_str_mft;                /* 设备制造商*/
    char                           *p_str_pdt;                /* 设备产商*/
    char                           *p_str_snum;               /* 设备序列号*/
    struct awbl_usbh_config         cfg;                      /* USB设备的配置结构体*/
    struct awbl_usbh_endpoint       ep0;                      /* 端点0*/
    struct aw_usb_endpoint_desc     ep0_desc;                 /* 端点0描述符定义*/
    struct awbl_usbh_endpoint      *ep_in[16];                /* 设备输入端点*/
    struct awbl_usbh_endpoint      *ep_out[16];               /* 设备输出端点*/
    uint16_t                        langid;                   /* 语言ID*/
    uint32_t                        quirks;                   /* 设备兼容*/
    uint8_t                         dev_type;                 /* 设备类型*/
};




/** USB功能结构体*/
struct awbl_usbh_function {
    struct awbl_dev                      awdev;       /* 基础设备类*/
    struct awbl_devhcf                   devhcf;      /* 设备硬件信息*/
    char                                 name[16];    /* 设备名字*/
    struct awbl_usbh_device             *p_udev;      /* 相关的USB设备结构体*/
    struct aw_list_head                  node;        /* 本功能节点*/

    uint8_t                              ifirst;      /* 第一个接口的编号*/
    uint8_t                              nitfs;       /* 接口的数量*/
    uint8_t                              nitfs_type;  /* 接口类型的数量(普通接口该值为1，联合接口该值为联合接口包含的接口数量)*/
    uint8_t                              clss;        /* 类*/
    uint8_t                              sclss;       /* 子类*/
    uint8_t                              proto;       /* 协议*/
    uint8_t                              type;        /* 接口类型(普通接口或联合接口)*/

    struct aw_list_head                  itfs;        /* 接口链表*/
    void                                *prive;       /* 驱动私有信息*/
};




/** USB接口结构体*/
struct awbl_usbh_interface {
    struct aw_list_head              node;      /* 当前接口节点*/
    struct aw_usb_interface_desc    *p_desc;    /* 接口描述符*/
    struct awbl_usbh_endpoint       *eps;       /* 接口下的端点*/
    int                              extralen;  /* 额外的描述符的长度*/
    uint8_t                         *extra;     /* 额外的描述符(例如，特定类描述符或特定产商描述符) */
};


/* USB功能驱动信息匹配表*/
struct awbl_usbh_fundrv_info_tab {
    uint16_t                    flags;                 /* 匹配标志*/
#define AWBL_USBH_FUNDRV_MATCH_VENDOR          0x0001  /* 匹配VID*/
#define AWBL_USBH_FUNDRV_MATCH_PRODUCT         0x0002  /* 匹配PID*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_CLASS       0x0010  /* 匹配设备类代码*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_SUBCLASS    0x0020  /* 匹配设备子类代码*/
#define AWBL_USBH_FUNDRV_MATCH_DEV_PROTOCOL    0x0040  /* 匹配设备协议代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_CLASS       0x0080  /* 匹配接口类代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_SUBCLASS    0x0100  /* 匹配接口子类代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_PROTOCOL    0x0200  /* 匹配接口协议代码*/
#define AWBL_USBH_FUNDRV_MATCH_FUN_NUMBER      0x0400  /* 匹配设备接口数量*/
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
 * \brief USB 主机总线函数
 */

/** \brief USB 主机总线初始化 */
void awbl_usbh_bus_init (void);

/** \brief USB 主机资源初始化 */
aw_err_t awbl_usbh_lib_init (int hub_stack_size, int hub_task_prio);

/** \brief 寻找下一个描述符*/
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
/** \brief 寻找USB设备*/
void *awbl_usbh_dev_find(uint16_t vid, uint16_t pid, uint8_t dev_type, aw_bool_t is_print);
/** \brief 模拟热插拔*/
aw_err_t awbl_usbh_hubplug(void *udev);
/** \brief 获取存在设备的配置编号*/
int awbl_usbh_dev_get_confignum(uint16_t vid, uint16_t pid);
/** \brief 引用一个USB设备*/
aw_err_t awbl_usbh_dev_get (struct awbl_usbh_device *p_udev);
/** \brief 取消对USB设备的引用*/
aw_err_t awbl_usbh_dev_put (struct awbl_usbh_device *p_udev);
/** \brief 检查设备是否连接*/
aw_bool_t awbl_usbh_dev_is_connect (struct awbl_usbh_device *p_udev);
/** \brief 复位一个端点 */
aw_err_t awbl_usbh_ep_reset(struct awbl_usbh_device *p_udev, unsigned int epaddr);
/** \brief 清除端点停止状态*/
aw_err_t awbl_usbh_ep_halt_clr (struct awbl_usbh_endpoint *p_ep);
/** \brief 注册设备监控函数*/
awbl_usbh_monitor_handle awbl_usbh_dev_monitor_register(uint16_t vid, uint16_t pid,
                                                        uint8_t dev_type, uint8_t monitor_type,
                                                        void (*user_callback) (void *p_arg, uint8_t type),
                                                        void *p_arg);
/** \brief 注销设备监控函数*/
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
/** \brief 设置USB设备接口 */
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
