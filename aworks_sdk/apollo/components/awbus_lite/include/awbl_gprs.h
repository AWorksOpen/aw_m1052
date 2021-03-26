/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief gprs�ӿ�
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-11-29   vih, first implementation.
 * \endinternal
 */

#ifndef __AWBL_GPRS_H
#define __AWBL_GPRS_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_awbl_if_gprs
 * \copydoc aw_gprs.h
 * @{
 */

#include "aworks.h"

#include "aw_gprs.h"
#include "aw_sem.h"
#include "ppp/sio_mgn.h"

#ifdef LWIP_2_X
#define AWBL_GPRS_USE_LWIP_1_4     0
#define AWBL_GPRS_USE_LWIP_2_X     1
#else
#define AWBL_GPRS_USE_LWIP_1_4     1
#define AWBL_GPRS_USE_LWIP_2_X     0
#endif

/**
 * \brief ʹ�� eth ģʽ����ʹ����Ĭ��ֻ֧�� ppp ģʽ
 */
#define AWBL_GPRS_LL_TYPE_ETH_ENABLE       0


/**
 * \brief ʹ��ʹ��  usbh_serial �ӿڣ����� Ϊ usbh_pipe �ӿ�
 */
#define AWBL_GPRS_USB_USBH_SERIAL     1


#if AWBL_GPRS_USE_LWIP_1_4
#include "ppp/ppp.h"
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

#if AWBL_GPRS_USE_LWIP_2_X
#include "netif/ppp/pppos.h"
#include "lwip/sio.h"
#endif /* AWBL_GPRS_USE_LWIP_2_X */

struct awbl_gprs_dev;

enum {
    AWBL_GPRS_SIM800_ID_BASE    = 0,
    AWBL_GPRS_U9300C_ID_BASE    = 10,
    AWBL_GPRS_EC20_ID_BASE      = 20,
    AWBL_GPRS_ME909S_ID_BASE    = 30,
    AWBL_GPRS_N58_ID_BASE       = 40,
};

/** \brief GPRS�豸���� link layer ����  */
typedef enum {
    AWBL_GPRS_LL_TYPE_PPP = 1,   /* PPPģʽ(��Ե�Э��)*/
    AWBL_GPRS_LL_TYPE_ETH,       /* ETHERNETģʽ(��̫��ģʽ)*/
} awbl_gprs_ll_type_t;


/** \brief GPRSģ��Ĳ�����Ϣ  */
struct awbl_gprs_info
{
    char     *p_dial_num;             /**< \brief ����ҵ�񲦺ź���*/
    char     *p_apn;                  /**< \brief APN����*/
    char     *p_user;                 /**< \brief APN�û���*/
    char     *p_password;             /**< \brief APN����*/
    char     *p_center_num;           /**< \brief ����Ϣ���ĺ���*/
    char     *p_at_err;               /**< \brief AT �������ʱ���յ����ַ���*/

    void    (*pfn_reset) (void);
    void    (*pfn_power_on) (void);
    void    (*pfn_power_off) (void);
    void    (*pfn_write_en) (void);      /**< \brief дʹ�� */
};

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE

/* ��̫���׶�״̬*/
typedef enum {
    AWBL_GPRS_ETHERNET_PHASE_UNINIT  = 0,
    AWBL_GPRS_ETHERNET_PHASE_INIT    = 1,
    AWBL_GPRS_ETHERNET_PHASE_DIAL_UP = 2,   /* ��̫������*/
} awbl_gprs_ethernet_phase_t;

/* ��¼��̫��ģʽ�ĵ�ַ*/
struct ethernet_addrs {
    struct ip_addr our_ipaddr;        /* IP��ַ*/
    struct ip_addr his_ipaddr;
    struct ip_addr netmask;           /* ��������*/
    struct ip_addr gateway;           /* ����*/
    struct ip_addr dns1;
    struct ip_addr dns2;
};


/* GPRS�豸��̫���ṹ��*/
struct awbl_gprs_ethernet {
    struct ethernet_addrs             ethernetaddr;     /* ��̫����ص�ַ */
    awbl_gprs_ethernet_phase_t        phase;              /* ��̫��ģʽ�׶�*/
    /* gprs�豸���Žӿ�  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);
    /* gprs�豸�رղ��Žӿ�  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};

#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

/**
 * \brief  ppp �׶�״̬
 */
typedef enum {
    AWBL_GPRS_PPP_PHASE_UNINIT = 0,
    AWBL_GPRS_PPP_PHASE_INIT,
    AWBL_GPRS_PPP_PHASE_DIAL_UP,
    AWBL_GPRS_PPP_PHASE_CREATE,
    AWBL_GPRS_PPP_PHASE_CONNECT,
    AWBL_GPRS_PPP_PHASE_RUNNING,
} awbl_gprs_ppp_phase_t;


struct awbl_gprs_ppp {
    struct sio_mgn               sio_mgn;       /**< \brief sio���� */

#if AWBL_GPRS_USE_LWIP_1_4
    int                          ppp_fd;        /**< \brief PPP��� */
    struct ppp_addrs            *p_pppaddr;     /**< \brief PPP��ȡ���ĵ�ַ */

    /** \brief ����ppp���ӹرյ��ź��� */
    AW_SEMB_DECL(link_close);
#endif /* AWBL_GPRS_USE_LWIP_1_4 */

    awbl_gprs_ppp_phase_t        phase;

#if AWBL_GPRS_USE_LWIP_2_X
    /** \brief ppp���ƿ�ָ�� */
    ppp_pcb     *p_pcb;

    /** \brief ppp���� */
    struct netif netif;

    /** \brief ����buf,����������ݴ�Ŵ�������� PPP_MRU + PPP_HDRLEN */
    u_char       rxbuf[2048];

    /** \brief ����ppp�߳� */
    AW_TASK_DECL(thread, PPP_THREAD_STACKSIZE);

    /** \brief ����ppp�̵߳Ĺر��ź��� */
    AW_SEMB_DECL(thread_close);
#endif /* AWBL_GPRS_USE_LWIP_2_X */

    /** \brief gprs�豸���Žӿ�  */
    aw_err_t (*pfn_dial_up)   (struct awbl_gprs_dev *p_this);

    /** \brief gprs�豸�رղ��Žӿ�  */
    aw_err_t (*pfn_dial_down) (struct awbl_gprs_dev *p_this);
};

/** \brief GPRS�豸�ķ���ӿ�   */
struct awbl_gprs_service {

    /** \brief GPRSģ�������ӿ� */
    aw_err_t (*pfn_gprs_power_on)(struct awbl_gprs_dev *p_this);

    /** \brief GPRSģ��رսӿ� */
    aw_err_t (*pfn_gprs_power_off)(struct awbl_gprs_dev *p_this);

    aw_err_t (*pfn_gprs_ioctl)(struct awbl_gprs_dev *p_this,
                               aw_gprs_ioctl_req_t   req,
                               aw_gprs_ioctl_args_t *p_arg);

    /** \brief ���ŷ��ͽӿ�    */
    aw_err_t (*pfn_gprs_sms_send)(struct awbl_gprs_dev *p_this,
                                  uint8_t              *p_sms,
                                  uint8_t               len,
                                  char                 *p_num);

    /** \brief HEX���ŷ��ͽӿ�    */
    aw_err_t (*pfn_gprs_sms_hex_send)(struct awbl_gprs_dev *p_this,
                                      uint8_t              *p_sms,
                                      uint8_t               len,
                                      char                 *p_num);

    /** \brief ���Ž��սӿ�   */
    aw_err_t (*pfn_gprs_sms_recv)(struct awbl_gprs_dev      *p_this,
                                  struct aw_gprs_sms_buf    *p_sms,
                                  uint32_t                   timeout);

    /** \brief ɾ��ȫ������     */
    aw_err_t (*pfn_gprs_all_sms_del)(struct awbl_gprs_dev *p_this);


    /** \brief ����GPRS����  */
    int (*pfn_gprs_start)(struct awbl_gprs_dev *p_this,
                          uint8_t               mode,
                          char                 *p_ip,
                          uint16_t              port,
                          uint32_t              timeout);

    /** \brief �ر�GPRS����  */
    aw_err_t (*pfn_gprs_close)(struct awbl_gprs_dev *p_this, int   hd);

    /** \brief ����GPRS����  */
    ssize_t (*pfn_gprs_send)(struct awbl_gprs_dev *p_this,
                             int                   hd,
                             void                 *p_data,
                             size_t                len,
                             uint32_t              timeout);

    /** \brief ����GPRS����   */
    ssize_t (*pfn_gprs_recv)(struct awbl_gprs_dev *p_this,
                             int                   hd,
                             void                 *p_buf,
                             size_t                len);

    /** \brief ̽����豸�Ƿ���ã�����ʵ��ʱҪ������   */
    aw_bool_t (*pfn_probe)(struct awbl_gprs_dev *p_this);

    /** \brief ʹ�� at �ܵ��շ�����   */
    ssize_t  (*pfn_at_cmd_send) (struct awbl_gprs_dev *p_this, char *p_buf);
    ssize_t  (*pfn_at_cmd_recv) (struct awbl_gprs_dev *p_this,
                                 char                 *p_buf,
                                 size_t                buf_size);

    /** \brief ʹ�� gps ����  */
    aw_err_t (*pfn_gnss_enable) (struct awbl_gprs_dev   *p_this,
                                 aw_gprs_gnss_cfg_t     *p_gnss_cfg);

    /** \brief �ر� gps ���� */
    aw_err_t (*pfn_gnss_disable) (struct awbl_gprs_dev   *p_this);

    /** \brief ��ȡ gnss Դ���� */
    aw_err_t (*pfn_gnss_nmeasrc_get) (struct awbl_gprs_dev  *p_this,
                                      uint8_t               *p_buf,
                                      size_t                 bufsize,
                                      uint32_t               timeout);
};


/** \brief GPRS�豸�ṹ��   */
struct awbl_gprs_dev {

    struct awbl_gprs_dev                 *p_next;

    /** \brief GPRS�豸�ķ���ӿ�   */
    aw_const struct awbl_gprs_service    *p_serv;

    /** \brief gprsģ��Ĳ��� */
    struct awbl_gprs_info       *p_info;

    /** \brief ���ŵ����ݺ���Ϣ */
    struct aw_gprs_sms_buf      *p_sms;

    /** \brief GPRS link layer ����*/
    awbl_gprs_ll_type_t          ll_type;

    struct awbl_gprs_ppp         ppp;

#if AWBL_GPRS_LL_TYPE_ETH_ENABLE
    /** \brief GPRS�豸��̫���ṹ��*/
    struct awbl_gprs_ethernet    ethernet;
#endif /* AWBL_GPRS_LL_TYPE_ETH_ENABLE */

    /** \brief gprs�豸��ΨһID�����ڸ��м���ȡ�豸 */
    int                          dev_id;

    /** \brief gprs��׼�ӿ�ʹ�õĻ����� */
    AW_MUTEX_DECL(               mutex);

    aw_bool_t                    mutex_inited;

    /** \brief �²���gprs�豸��ָ��  */
    void                        *p_gprs_dev;

    /** \brief socktet���ӵ����� */
    uint8_t                      socket_num;

    /** \brief �Ƿ�ʹ���ⲿЭ��ջ��־ */
    aw_bool_t                    use_external_protocol;

    /** \brief �Ƿ��Ѿ���ʼ�� */
    aw_bool_t                    already_init;

    /** \brief �Ѿ��ϵ��־ */
    aw_bool_t                    power_on;

    /** \brief gnss ������ʹ�ܱ�־ */
    aw_bool_t                    gnss_en;

    aw_gprs_sts_t                status;

    aw_tick_t                    last_status_tick;

    /** \brief ��¼��һ�ε� evt�������¼��ص�������ʱ���ϱ���һ�ε� evt */
    aw_gprs_evt_t                last_evt;

    /** \brief �ź���С��Чֵ�����ڸ�ֵ�޷�����ʹ���繦��  */
    int                          csq_min;

#define AWBL_GPRS_EVT_CB_MAX    6
    /** \brief gprs �¼��ص����飬��������  */
    aw_gprs_evt_cb_t             pfn_evt_cb_array[AWBL_GPRS_EVT_CB_MAX];

    void                       (*pfn_log_out) ( void         *p_color,
                                                const char   *func_name,
                                                int           line,
                                                const char   *fmt, ...);
};

/** \brief GPRS ��Ϣ�ṹ��*/
typedef struct {
    uint32_t    vid;
    uint32_t    pid;

    /** \brief com ֵΪ -1 ʱ��Ч  */
    int8_t      at_com;
    int8_t      ppp_com;
    int8_t      dm_com;
    int8_t      gps_com;

} awbl_gprs_usbh_info;

/**
 * \brief ��ppp��sio_mgn��ͨ����Դ�ĳ�ʼ��
 *
 * \param[in]  p_gprs         gprs�豸ָ��
 *
 * \return  ��
 */
aw_err_t awbl_gprs_init_tail (struct awbl_gprs_dev *p_gprs);

/**
 * \brief ���� AT ����
 *
 * \param[in]  p_this         gprs�豸ָ��
 * \param[in]  p_str          ���͵�AT����
 *
 * \return  ��
 */
void awbl_gprs_at_send (struct awbl_gprs_dev  *p_this, char *p_str);


/**
 * \brief ָ������ AT ����صĽ��
 *
 * \param[in]  this           gprs�豸ָ��
 * \param[in]  p_buf          �������ݵĻ���buf
 * \param[in]  buf_size       �����С
 * \param[in]  p_cond         ָ�����ص��ַ�������
 * \param[in]  arg_num        �ж����ֿ��ܵķ������ݣ�ָ��ָ���������м���Ԫ�أ�
 * \param[in]  wait_ms        ���ĵȴ�ʱ�䣬��λS��Ϊ0�򲻵ȴ�
 *
 * \retval   >= 0            : ���յ���p_cond ������ݣ����Է���ֵ��Ϊ�±���Ǹ�����
 * \retval   -AW_ETIMEDOUT   : ��ʱδ���յ��ڴ�����
 * \retval   -AW_ENOMSG      : ���յ� ERROR
 */
int awbl_gprs_at_recv (struct awbl_gprs_dev     *p_this,
                       char                     *p_buf,
                       uint32_t                  buf_size,
                       char                     *p_cond[],
                       uint32_t                  arg_num,
                       uint32_t                  wait_ms);

aw_err_t awbl_gprs_pdu_pack_ucs32(char    *message,
                                  size_t   msg_size,
                                  char    *p_sca,
                                  char    *p_sms,
                                  uint8_t  len,
                                  char    *p_num);

aw_err_t awbl_gprs_pdu_unpack_ucs32 (char                   *p_pdu_data,
                                     struct aw_gprs_sms_buf *p_to_sms);

uint32_t utf8_to_ucs2(const char *utf8, int utf8_length,
        char *ucs2, int ucs2_length);

uint32_t ucs2_to_utf8(const char *ucs2, int ucs2_length,
        char *utf8, int utf8_length);

/**
 * \brief �����ݴ����PDU��protocol data unit����
 *
 * \param[in]  message   ��Ŵ�������ݵ�buf
 * \param[in]  msg_size  buf��С
 * \param[in]  p_sca     ����Ϣ�������ĵ�ַ
 * \param[in]  p_sms     ��������
 * \param[in]  len       ���ų���
 * \param[in]  p_num     ���շ�����
 * \param[in]  code      ָ�� sms �ı����ʽ���� AW_GPRS_SMS_CODE_UCS2
 *
 * \retval AW_OK        :  �ɹ�
 */
aw_err_t awbl_gprs_pdu_pack (char     *message,
                             size_t    msg_size,
                             char     *p_sca,
                             uint8_t  *p_sms,
                             uint8_t   len,
                             char     *p_num,
                             uint8_t   code);

/**
 * \brief ����PDU��protocol data unit����
 *
 * \param[in]  p_pdu_data     pdu����
 * \param[in]  p_to_sms       ��Ž��͵���pdu���ݵĽṹ��
 *
 * \retval AW_OK        :  �ɹ�
 * \retval -AW_EBADMSG  :  ������ȷ�����ݰ�
 */
aw_err_t awbl_gprs_pdu_unpack (char                   *p_pdu_data,
                               struct aw_gprs_sms_buf *p_to_sms);

void awbl_gprs_init (void);

/** @}  grp_awbl_if_gprs */

#ifdef __cplusplus
}
#endif /* __cplusplus   */

#endif /* __AWBL_GPRS_H */

/* end of file */
