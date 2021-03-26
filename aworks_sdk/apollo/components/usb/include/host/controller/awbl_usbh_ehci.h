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


//#define AWBL_USBH_EHCI_FRAME_LIST_SIZE  (1024)  /* ֡�б��С��1024(4K), 512(2K), 256(1K) */
#define AWBL_USBH_EHCI_TASK_STACK_SIZE  (8192)  /* EHCI���񗣴�С*/
#define AWBL_USBH_EHCI_FRAME_LIST_SIZE  (256)  /* ֡�б��С��1024(4K), 512(2K), 256(1K) */
//#define AWBL_USBH_EHCI_TASK_STACK_SIZE  (4096)  /* EHCI���񗣴�С*/
/* �����жϴ��䲻��ռ�ó���80%��΢֡ʱ�䣬ȫ�ٺ͵��ٲ��ó���90%֡ʱ��*/
/* ȫ��ͬ�����䲻��ռ�ó���90%��֡ʱ�䣬����ͬ�����䲻��ռ�ó���90%��΢֡ʱ��*/
#define AWBL_USBH_EHCI_UFRAME_BANDWIDTH (100)   /* һ��΢֡�Ĵ���125*80% us */
#define AWBL_USBH_EHCI_BANDWIDTH_SIZE   (80)    /* �洢΢֡�����������ͬʱҲ����С������ */
#define AWBL_USBH_EHCI_BANDWIDTH_FRAMES (10)    /* ����֡����������΢֡��������8*/
#define AWBL_USBH_EHCI_TUNE_RL_HS       4       /* nak throttle; see 4.9 */
#define AWBL_USBH_EHCI_TUNE_RL_TT        0
#define AWBL_USBH_EHCI_TUNE_MULT_HS     1       /* 1-3����ÿ΢֡ */
#define AWBL_USBH_EHCI_TUNE_MULT_TT     1       /* �˵�ÿһ΢֡����һ������*/

/* ���ͱ������ {qh,itd,sitd,fstn}->hw_next */
#define Q_NEXT_TYPE(dma)   ((dma) & AW_USB_CPU_TO_LE32(3 << 1))


union __ehci_ptr;
/** EHCI�������ṹ��*/
struct awbl_usbh_ehci {
//    struct awbl_usbh          hc;

    struct aw_pool            qhs;                   /* QH(����ͷ)�ڴ��*/
    struct aw_pool            qtds;                  /* qtd(���д���������)�ڴ��*/
    struct aw_pool            itds;                  /* itd(ͬ������������)�ڴ��*/
    struct aw_pool            sitds;                 /* sitd(�ָ�����ͬ��������)�ڴ��*/

    uint8_t                   evt;                   /* EHCI�¼�*/

    /* ��ʱ�������*//////////////////////////////////////////////////////////////////////////
    //struct aw_list_head       tt_list;              /* ����ת��������*/
    uint32_t                  i_thresh;
    uint32_t                  isoc_count;            /* ��ʱ���ȼ���Ĵ���*/
    int                       bandwidth_allocated;
    int                       next_uframe;           /* ��һ΢֡����*/
    //uint8_t                   iso_on;
    ///////////////////////////////////////////////////////////////////////////////////

    aw_usb_sem_handle_t       sem;                   /* EHCI�ź���*/
    aw_usb_mutex_handle_t     lock;                  /* EHCI������*/
    aw_usb_task_handle_t      task;                  /* EHCI����*/
    aw_spinlock_isr_t         spinlock;

    uint32_t                  capregs;               /* USB���ܼĴ���*/
    uint32_t                  optregs;               /* EHCI�����Ĵ���*/

    uint32_t                  hcs;                   /* �����������ṹ�����*/
    aw_bool_t                 ppcd;                  /* ÿһ���˿ڱ仯���*/
    uint8_t                   ps_bit;                /* �˿��ٶ�λ*/
    uint8_t                   nports;                /* �˿�����*/
    uint32_t                  status;                /* EHCI״̬*/


    struct awbl_usbh_ehci_qh  *async;                /* �첽����*/
    uint32_t                  *periodic;             /* ���ڵ���*/
    union __ehci_ptr          *pshadow;              /* Ӳ�����ڱ��������������֡�б�����ݽṹ*/
    uint32_t                  fls;                   /* ֡�б��С*/
    uint8_t                   bdwidth[AWBL_USBH_EHCI_BANDWIDTH_SIZE];   /* ���ߴ���*/
    int                       random;
    struct aw_list_head       intr_qhs;              /* �ж�QH(����ͷ)*/

    aw_timer_t                guarder;               /* �ػ���ʱ��*/

    /* ������������31λ�Ǹ�λ�仯״̬λ����30λ�ǹ���仯״̬λ��29~0λ��USB�˿�״̬���ƼĴ���ֵһ��*/
    uint32_t                  psts[15];              /* �˿�״̬*/
    /* ���������仯�ص�����*/
    void (*pfn_rh_change) (struct awbl_usbh_ehci *p_ehci, uint16_t change);
};



/** EHCI qTD(���д���������) �ṹ��*/
struct awbl_usbh_ehci_qtd {
    /* EHCI qTD���ݽṹ�淶*/
    uint32_t                hw_next;       /* ָ����һ��qTD��ָ��*/
    uint32_t                hw_alt_next;   /*  Aletmate Next qTD Pointer*/
    uint32_t                hw_token;      /* ����*/
    uint32_t                hw_buf[5];     /* qTD������ҳָ��*/
    uint32_t                hw_buf_hi[5];

    /* USB������������������*/
    struct aw_list_head     node;          /* ��ǰqTD�ڵ�*/
    struct awbl_usbh_trp   *p_trp;         /* ��������� */
    size_t                  len;           /* ��ǰqTD���ݳ���*/
};

/* EHCI����ָ��������*/
union __ehci_ptr {
    struct awbl_usbh_ehci_qh    *p_qh;        /* QH(����ͷ)�ṹ��Q_TYPE_QH */
    struct awbl_usbh_ehci_itd   *p_itd;       /* ��ʱ�����������ṹ��Q_TYPE_ITD */
    struct awbl_usbh_ehci_sitd  *p_sitd;      /* �ָ��ʱ�����������ṹ�� Q_TYPE_SITD */
    struct awbl_usbh_ehci_fstn  *p_fstn;      /* FSTN�ṹ��Q_TYPE_FSTN */
    uint32_t                    *hw_next;     /* ������������*/
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
    uint8_t                     usecs;              /* �ж϶˵����*/
    uint8_t                     c_usecs;            /* �ָ���ɴ���*/
    struct aw_list_head         intr_node;
    struct awbl_usbh_endpoint  *p_ep;

};

///* EHCI����ת�����ṹ��*/
//struct awbl_usbh_ehci_tt {
//    //todo
//};


/** EHCI��ʱ�����������ṹ��*/
struct awbl_usbh_ehci_itd {
    uint32_t            hw_next;                             /* ��һ������ָ��(EHCI 3.3.1)*/
    uint32_t            hw_transaction[8];                   /* ��ʱ��������������״̬�Ϳ����б�(EHCI 3.3.2)*/
    uint32_t            hw_bufp[7];                          /* ��ʱ��������������ҳָ������(EHCI 3.3.3)*/
    uint32_t            hw_bufp_hi[7];                       /* 64λ�汾�����ݽṹ(��¼ B)*/

#define AW_EHCI_ISOC_ACTIVE        (1<<31)                   /* �����ۼ������ */
#define AW_EHCI_ISOC_BUF_ERR       (1<<30)                   /* ���ݻ������*/
#define AW_EHCI_ISOC_BABBLE        (1<<29)                   /* babble ���*/
#define AW_EHCI_ISOC_XACTERR       (1<<28)                   /* XactErr - �������*/
#define AW_EHCI_ITD_LENGTH(tok)    (((tok)>>16) & 0x0fff)
#define AW_EHCI_ITD_IOC            (1 << 15)                 /* ����ж�*/

#define AW_ITD_ACTIVE    AW_USB_CPU_TO_LE32(AW_EHCI_ISOC_ACTIVE)
    union __ehci_ptr    p_next;                               /* ptr to periodic q entry */
    uint32_t frame;                                           /* ���������*/
    struct aw_list_head itd_list;                             /* �������ĵ�ʱ��������������*/
    struct awbl_usbh_trp *trp;                                /* USB���������*/
    struct ehci_iso_stream  *stream;                          /* �˵����*/
    uint32_t        index[8];                                 /* �� trp->iso_frame_descde������*/
    uint32_t        pg;                                       /* ҳѡ��*/
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



/* USB����EHCI��ʼ��*/
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
/* USB����EHCI�жϺ���*/
void awbl_usbh_ehci_irq (struct awbl_usbh_ehci *p_ehci);
/* USB����EHCI�˵�ʹ�ܺ���*/
aw_err_t awbl_usbh_ehci_ep_enable (struct awbl_usbh_ehci     *p_ehci,
                                   struct awbl_usbh_endpoint *p_ep);
/* USB����EHCI�˵���ܺ���*/
aw_err_t awbl_usbh_ehci_ep_disable (struct awbl_usbh_ehci     *p_ehci,
                                    struct awbl_usbh_endpoint *p_ep);
/* USB����EHCI�����亯��*/
aw_err_t awbl_usbh_ehci_request (struct awbl_usbh_ehci *p_ehci,
                                 struct awbl_usbh_trp  *p_trp);
/* USB����EHCIȡ�����亯��*/
aw_err_t awbl_usbh_ehci_cancel (struct awbl_usbh_ehci *p_ehci,
                                struct awbl_usbh_trp  *p_trp);
/* USB����EHCI�����������ɺ���*/
aw_err_t awbl_usbh_ehci_trp_done (struct awbl_usbh_ehci *p_ehci,
                                  struct awbl_usbh_trp  *p_trp);
/* USB����EHCI����������״̬����*/
aw_err_t awbl_usbh_ehci_rh_change_check (struct awbl_usbh_ehci *p_ehci,
                                         uint16_t              *p_change);
/* USB����EHCI�����������ƺ���*/
aw_err_t awbl_usbh_ehci_rh_ctrl (struct awbl_usbh_ehci *p_ehci,
                                 uint8_t                type,
                                 uint8_t                req,
                                 uint16_t               val,
                                 uint16_t               idx,
                                 uint16_t               len,
                                 void                  *p_buf);
uint16_t periodic_usecs (struct awbl_usbh_ehci *p_ehci, uint32_t frame, uint32_t uframe);
/* USB����EHCI��ȡ΢֡��������*/
uint32_t awbl_usbh_ehci_read_frame_index(struct awbl_usbh_ehci *p_ehci);
/* USB����EHCI��ȡ֡��������*/
int awbl_usbh_ehci_get_frame(struct awbl_usbh_ehci *p_ehci);
/* USB����EHCI�������ڵ��Ⱥ���*/
void awbl_usbh_ehci_enable_periodic(struct awbl_usbh_ehci *p_ehci, uint8_t is_iso);
/* USB����EHCI�ر����ڵ��Ⱥ���*/
void awbl_usbh_ehci_disable_periodic(struct awbl_usbh_ehci *p_ehci);

union __ehci_ptr __ehci_next_struct (union __ehci_ptr pstr, uint32_t type);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_USBH_EHCI_H */
