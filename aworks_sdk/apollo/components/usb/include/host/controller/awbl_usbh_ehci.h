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

#ifndef __AWBL_USBH_EHCI_H
#define __AWBL_USBH_EHCI_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_pool.h"
#include "aw_timer.h"
#include "host/awbl_usbh.h"
#include "host/controller/awbl_usbh_ehci_iso_sched.h"


//#define AWBL_USBH_EHCI_FRAME_LIST_SIZE  (1024)  /* 帧列表大小：1024(4K), 512(2K), 256(1K) */
#define AWBL_USBH_EHCI_TASK_STACK_SIZE  (8192)  /* EHCI任务棧大小*/
#define AWBL_USBH_EHCI_FRAME_LIST_SIZE  (256)  /* 帧列表大小：1024(4K), 512(2K), 256(1K) */
//#define AWBL_USBH_EHCI_TASK_STACK_SIZE  (4096)  /* EHCI任务棧大小*/
/* 高速中断传输不得占用超过80%的微帧时间，全速和低速不得超过90%帧时间*/
/* 全速同步传输不得占用超过90%的帧时间，高速同步传输不得占用超过90%的微帧时间*/
#define AWBL_USBH_EHCI_UFRAME_BANDWIDTH (100)   /* 一个微帧的带宽，125*80% us */
#define AWBL_USBH_EHCI_BANDWIDTH_SIZE   (80)    /* 存储微帧带宽的数量，同时也是最小的周期 */
#define AWBL_USBH_EHCI_BANDWIDTH_FRAMES (10)    /* 带宽帧数量，带宽微帧数量除以8*/
#define AWBL_USBH_EHCI_TUNE_RL_HS       4       /* nak throttle; see 4.9 */
#define AWBL_USBH_EHCI_TUNE_RL_TT        0
#define AWBL_USBH_EHCI_TUNE_MULT_HS     1       /* 1-3事务每微帧 */
#define AWBL_USBH_EHCI_TUNE_MULT_TT     1       /* 端点每一微帧产生一个事务*/

/* 类型标记来自 {qh,itd,sitd,fstn}->hw_next */
#define Q_NEXT_TYPE(dma)   ((dma) & AW_USB_CPU_TO_LE32(3 << 1))


union __ehci_ptr;
/** EHCI控制器结构体*/
struct awbl_usbh_ehci {
//    struct awbl_usbh          hc;

    struct aw_pool            qhs;                   /* QH(队列头)内存池*/
    struct aw_pool            qtds;                  /* qtd(队列传输描述符)内存池*/
    struct aw_pool            itds;                  /* itd(同步传输描述符)内存池*/
    struct aw_pool            sitds;                 /* sitd(分割事务同步描述符)内存池*/

    uint8_t                   evt;                   /* EHCI事件*/

    /* 等时传输相关*//////////////////////////////////////////////////////////////////////////
    //struct aw_list_head       tt_list;              /* 事务转换器链表*/
    uint32_t                  i_thresh;
    uint32_t                  isoc_count;            /* 等时调度激活的次数*/
    int                       bandwidth_allocated;
    int                       next_uframe;           /* 下一微帧索引*/
    //uint8_t                   iso_on;
    ///////////////////////////////////////////////////////////////////////////////////

    aw_usb_sem_handle_t       sem;                   /* EHCI信号量*/
    aw_usb_mutex_handle_t     lock;                  /* EHCI互斥锁*/
    aw_usb_task_handle_t      task;                  /* EHCI任务*/
    aw_spinlock_isr_t         spinlock;

    uint32_t                  capregs;               /* USB性能寄存器*/
    uint32_t                  optregs;               /* EHCI操作寄存器*/

    uint32_t                  hcs;                   /* 主机控制器结构体参数*/
    aw_bool_t                 ppcd;                  /* 每一个端口变化检测*/
    uint8_t                   ps_bit;                /* 端口速度位*/
    uint8_t                   nports;                /* 端口数量*/
    uint32_t                  status;                /* EHCI状态*/


    struct awbl_usbh_ehci_qh  *async;                /* 异步调度*/
    uint32_t                  *periodic;             /* 周期调度*/
    union __ehci_ptr          *pshadow;              /* 硬件周期表镜像，用来存放周期帧列表的数据结构*/
    uint32_t                  fls;                   /* 帧列表大小*/
    uint8_t                   bdwidth[AWBL_USBH_EHCI_BANDWIDTH_SIZE];   /* 总线带宽*/
    int                       random;
    struct aw_list_head       intr_qhs;              /* 中断QH(队列头)*/

    aw_timer_t                guarder;               /* 守护定时器*/

    /* 根集线器，第31位是复位变化状态位，第30位是挂起变化状态位，29~0位和USB端口状态控制寄存器值一样*/
    uint32_t                  psts[15];              /* 端口状态*/
    /* 根集线器变化回调函数*/
    void (*pfn_rh_change) (struct awbl_usbh_ehci *p_ehci, uint16_t change);
};



/** EHCI qTD(队列传输描述符) 结构体*/
struct awbl_usbh_ehci_qtd {
    /* EHCI qTD数据结构规范*/
    uint32_t                hw_next;       /* 指向下一个qTD的指针*/
    uint32_t                hw_alt_next;   /*  Aletmate Next qTD Pointer*/
    uint32_t                hw_token;      /* 令牌*/
    uint32_t                hw_buf[5];     /* qTD缓冲区页指针*/
    uint32_t                hw_buf_hi[5];

    /* USB主机控制器驱动所需*/
    struct aw_list_head     node;          /* 当前qTD节点*/
    struct awbl_usbh_trp   *p_trp;         /* 传输请求包 */
    size_t                  len;           /* 当前qTD数据长度*/
};

/* EHCI类型指针联合体*/
union __ehci_ptr {
    struct awbl_usbh_ehci_qh    *p_qh;        /* QH(队列头)结构，Q_TYPE_QH */
    struct awbl_usbh_ehci_itd   *p_itd;       /* 等时传输描述符结构，Q_TYPE_ITD */
    struct awbl_usbh_ehci_sitd  *p_sitd;      /* 分割等时传输描述符结构， Q_TYPE_SITD */
    struct awbl_usbh_ehci_fstn  *p_fstn;      /* FSTN结构，Q_TYPE_FSTN */
    uint32_t                    *hw_next;     /* 所有类型适用*/
    void                        *ptr;
};




/** \brief EHCI QH */
struct awbl_usbh_ehci_qh {
    /* for HC */
    uint32_t                    hw_next;
    uint32_t                    hw_info1;
    uint32_t                    hw_info2;
    uint32_t                    hw_cur_qtd;
    uint32_t                    hw_next_qtd;
    uint32_t                    hw_alt_next_qtd;
    uint32_t                    hw_token;
    uint32_t                    hw_buf [5];
    uint32_t                    hw_buf_hi [5];

    /* for HCD */
    union __ehci_ptr            p_next;        /* next QH or others*/
    struct aw_list_head         qtds;          /* QTDs on this QH */
    struct awbl_usbh_ehci_qtd  *dummy;         /* dummy */
    struct awbl_usbh_ehci      *p_ehci;        /* controller */
    uint8_t                     state;         /* state */
    uint8_t                     retry;          /* XactErr retry count */
    aw_bool_t                      unlink;
    /* for interrupt transfer */
    uint16_t                    f_phase;        /* frame index */
    uint8_t                     u_phase;        /* uframe index */
    uint16_t                    f_period;       /* frame period */
    uint8_t                     usecs;              /* 中断端点带宽*/
    uint8_t                     c_usecs;            /* 分割完成带宽*/
    struct aw_list_head         intr_node;
    struct awbl_usbh_endpoint  *p_ep;

};

///* EHCI事务转换器结构体*/
//struct awbl_usbh_ehci_tt {
//    //todo
//};


/** EHCI等时传输描述符结构体*/
struct awbl_usbh_ehci_itd {
    uint32_t            hw_next;                             /* 下一个链接指针(EHCI 3.3.1)*/
    uint32_t            hw_transaction[8];                   /* 等时传输描述符事务状态和控制列表(EHCI 3.3.2)*/
    uint32_t            hw_bufp[7];                          /* 等时传输描述符缓存页指针链表(EHCI 3.3.3)*/
    uint32_t            hw_bufp_hi[7];                       /* 64位版本的数据结构(附录 B)*/

#define AW_EHCI_ISOC_ACTIVE        (1<<31)                   /* 这个插槽激活传输中 */
#define AW_EHCI_ISOC_BUF_ERR       (1<<30)                   /* 数据缓存错误*/
#define AW_EHCI_ISOC_BABBLE        (1<<29)                   /* babble 检测*/
#define AW_EHCI_ISOC_XACTERR       (1<<28)                   /* XactErr - 传输错误*/
#define AW_EHCI_ITD_LENGTH(tok)    (((tok)>>16) & 0x0fff)
#define AW_EHCI_ITD_IOC            (1 << 15)                 /* 完成中断*/

#define AW_ITD_ACTIVE    AW_USB_CPU_TO_LE32(AW_EHCI_ISOC_ACTIVE)
    union __ehci_ptr    p_next;                               /* ptr to periodic q entry */
    uint32_t frame;                                           /* 从哪里调度*/
    struct aw_list_head itd_list;                             /* 数据流的等时传输描述符链表*/
    struct awbl_usbh_trp *trp;                                /* USB传输请求包*/
    struct ehci_iso_stream  *stream;                          /* 端点队列*/
    uint32_t        index[8];                                 /* 在 trp->iso_frame_descde的索引*/
    uint32_t        pg;                                       /* 页选择*/
    uint8_t         usecs[8];
} __attribute__ ((aligned (32)));





/** \brief EHCI SITD */
struct awbl_usbh_ehci_sitd {
    uint32_t            hw_next;
    uint32_t            hw_fullspeed_ep;    /* EHCI table 3-9 */
    uint32_t            hw_uframe;      /* EHCI table 3-10 */
    uint32_t            hw_results;     /* EHCI table 3-11 */
    uint32_t            hw_buf[2];         /* EHCI table 3-12 */
    uint32_t            hw_backpointer;     /* EHCI table 3-13 */
    uint32_t            hw_buf_hi[2];      /* Appendix B */

    /* the rest is HCD-private */
    union __ehci_ptr    p_next;     /* ptr to periodic q entry */
    struct aw_list_head sitds;      /* list of stream's sitds */

};




/** \brief EHCI FSTN */
struct awbl_usbh_ehci_fstn {
    uint32_t            hw_next;
    union __ehci_ptr    p_next;
};



/* USB主机EHCI初始化*/
aw_err_t awbl_usbh_ehci_init (struct awbl_usbh_ehci *p_ehci,
                              uint32_t               reg_base,
                              int                    task_prio,
                              void (*pfn_rh_change) (
                                      struct awbl_usbh_ehci *p_ehci,
                                      uint16_t               change),
                              uint8_t                ps_bit,
                              int                    nqhs,
                              int                    nqtds,
                              int                    nitds,
                              int                    nsitds);
/* USB主机EHCI中断函数*/
void awbl_usbh_ehci_irq (struct awbl_usbh_ehci *p_ehci);
/* USB主机EHCI端点使能函数*/
aw_err_t awbl_usbh_ehci_ep_enable (struct awbl_usbh_ehci     *p_ehci,
                                   struct awbl_usbh_endpoint *p_ep);
/* USB主机EHCI端点禁能函数*/
aw_err_t awbl_usbh_ehci_ep_disable (struct awbl_usbh_ehci     *p_ehci,
                                    struct awbl_usbh_endpoint *p_ep);
/* USB主机EHCI请求传输函数*/
aw_err_t awbl_usbh_ehci_request (struct awbl_usbh_ehci *p_ehci,
                                 struct awbl_usbh_trp  *p_trp);
/* USB主机EHCI取消传输函数*/
aw_err_t awbl_usbh_ehci_cancel (struct awbl_usbh_ehci *p_ehci,
                                struct awbl_usbh_trp  *p_trp);
/* USB主机EHCI传输请求包完成函数*/
aw_err_t awbl_usbh_ehci_trp_done (struct awbl_usbh_ehci *p_ehci,
                                  struct awbl_usbh_trp  *p_trp);
/* USB主机EHCI检查根集线器状态函数*/
aw_err_t awbl_usbh_ehci_rh_change_check (struct awbl_usbh_ehci *p_ehci,
                                         uint16_t              *p_change);
/* USB主机EHCI根集线器控制函数*/
aw_err_t awbl_usbh_ehci_rh_ctrl (struct awbl_usbh_ehci *p_ehci,
                                 uint8_t                type,
                                 uint8_t                req,
                                 uint16_t               val,
                                 uint16_t               idx,
                                 uint16_t               len,
                                 void                  *p_buf);
uint16_t periodic_usecs (struct awbl_usbh_ehci *p_ehci, uint32_t frame, uint32_t uframe);
/* USB主机EHCI获取微帧索引函数*/
uint32_t awbl_usbh_ehci_read_frame_index(struct awbl_usbh_ehci *p_ehci);
/* USB主机EHCI获取帧索引函数*/
int awbl_usbh_ehci_get_frame(struct awbl_usbh_ehci *p_ehci);
/* USB主机EHCI启动周期调度函数*/
void awbl_usbh_ehci_enable_periodic(struct awbl_usbh_ehci *p_ehci, uint8_t is_iso);
/* USB主机EHCI关闭周期调度函数*/
void awbl_usbh_ehci_disable_periodic(struct awbl_usbh_ehci *p_ehci);

union __ehci_ptr __ehci_next_struct (union __ehci_ptr pstr, uint32_t type);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_EHCI_H */
