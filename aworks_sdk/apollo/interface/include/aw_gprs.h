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
 * \brief ͨ��GPRS����
 *
 * \par ʹ��ʾ��
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.04 17-12-09  vih, redefine the architecture
 * - 1.03 15-03-06  ops, add sms if and aw_gprs_sms_data.
 * - 1.02 15-01-23  ops, add signal strength if.
 * - 1.01 15-01-09  ops, create the gprs abstract class, and redefine the std if.
 * - 1.00 14-12-15  afm, first implementation.
 * \endinternal
 */

#ifndef __AW_GPRS_H_
#define __AW_GPRS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_gprs
 * \copydoc aw_gprs.h
 * @{
 */

#include "aworks.h"
#include "aw_types.h"
#include "aw_netif.h"

//#include "arpa/inet.h"

/**
 * \brief gprs �豸״̬
 */
typedef enum {
    AW_GPRS_STS_AT_ERROR,           /**< \brief AT ����ͨ���쳣 */
    AW_GPRS_STS_CSQ_ERROR,          /**< \brief �ź�ǿ���쳣 */
    AW_GPRS_STS_APN_ERROR,          /**< \brief δ��ѯ��������Ӫ�̣����޿���*/
    AW_GPRS_STS_PIN_ERROR,          /**< \brief ��Ҫ���� PIN �� PUK ���� */
    AW_GPRS_STS_CEREG_ERROR,        /**< \brief ����ע��״̬����   */
    AW_GPRS_STS_PPP_ERROR,          /**< \brief ppp �����쳣 */
    AW_GPRS_STS_ETH_ERROR,          /**< \brief eth ģʽ�����쳣 */
    AW_GPRS_STS_PING_ERROR,         /**< \brief ping ����ʧ�� */
    AW_GPRS_STS_OK,                 /**< \brief �豸���� */
} aw_gprs_sts_t;


struct aw_gprs_sms_buf {
    char     *p_sms_buff;                   /**< \brief �������ݻ��� */
    uint16_t  sms_buff_len;                 /**< \brief �������ݻ��泤�� */

    char     *p_num_buff;                   /**< \brief �ֻ��������ݻ��� */
    uint16_t  num_buff_len;                 /**< \brief �ֻ��������ݻ��泤�� */

    char     *p_time_buff;                  /**< \brief ʱ�����ݻ���   */
    uint16_t  time_buff_len;                /**< \brief ʱ�����ݻ��泤�� */

/** \brief ���յĶ��ű�������  */
#define AW_GPRS_SMS_CODE_7BIT        (0)     /**< \brief 7bit ������� */
#define AW_GPRS_SMS_CODE_8BIT         (1)    /**< \brief 8bit ������� */
#define AW_GPRS_SMS_CODE_UCS2        (2)     /**< \brief UCS2������� */
#define AW_GPRS_SMS_CODE_GB          (3)     /**< \brief GB������� */
#define AW_GPRS_SMS_CODE_UTF8        (4)     /**< \brief UTF8������� */
    uint8_t   sms_code;                      /**< \brief ���ű��� */
};


/**
 * \brief GPRS �¼�����
 */
typedef enum {
    AW_GPRS_EVT_TYPE_NONE = 0,          /**< \brief ��Ч�¼��������Ͳ�����֪ͨ  */
    AW_GPRS_EVT_TYPE_PWR_OFF_PRE,       /**< \brief �豸׼������  */
    AW_GPRS_EVT_TYPE_PWR_OFF,           /**< \brief �豸�ѵ���  */
    AW_GPRS_EVT_TYPE_PWR_ON,            /**< \brief �豸���ϵ�  */

    AW_GPRS_EVT_TYPE_RESET_PRE,     /**< \brief �豸׼����λ  */
    AW_GPRS_EVT_TYPE_RESET_COMPLETED,   /**< \brief �豸��λ�ɹ�   */

    /** \brief PPP ���ӳɹ��¼� */
    AW_GPRS_EVT_TYPE_PPP_LINK_UP,

    /** \brief PPP ���ӶϿ��¼�  */
    AW_GPRS_EVT_TYPE_PPP_LINK_DOWN,

    /** \brief eth ���ӳɹ��¼�  */
    AW_GPRS_EVT_TYPE_ETH_LINK_UP,

    /** \brief eth ���ӶϿ��¼� */
    AW_GPRS_EVT_TYPE_ETH_LINK_DOWN,

} aw_gprs_evt_type_t;

/**
 * \brief GPRS �¼��ṹ��
 */
typedef struct {
    aw_gprs_evt_type_t type;

    union {
        struct {
            /**< \brief �ɻص��������ã�=0 ����׼���øò�����<0 ��δ׼���� */
            aw_err_t retval;
        } pwr_off_pre, pwr_off, reset_pre;

        /** \brief AW_GPRS_EVT_TYPE_ETH_LINK_UP �¼�����  */
        struct {
            aw_netif_t  *p_if;

            ip_addr_t    ip;
            ip_addr_t    netmask;
            ip_addr_t    gateway;
            ip_addr_t    dns;
        } eth_link_up;
    } u;

} aw_gprs_evt_t;

typedef void (*aw_gprs_evt_cb_t) (int devid, aw_gprs_evt_t *p_evt);

/**
 * \brief IO���Ƶ�ѡ��
 */
typedef enum aw_gprs_ioctl_req {

    /** \brief ����ʹ�õ�Э��ջ����Ϊ0��Ϊ�ⲿЭ��ջ��Ĭ�ϣ���Ϊ1��ʹ���ڲ�Э��ջ   */
    AW_GPRS_REQ_PROTOCOL_SET,

    /**
     * \brief ��������������
     *
     * ���ӳɹ����ϱ� AW_GPRS_EVT_TYPE_PPP_LINK_UP ��
     *  AW_GPRS_EVT_TYPE_ETH_LINK_UP �¼�����Ҫ�������¼��ص�������
     *
     * ֻ����ʹ���ⲿЭ��ʱ���ܽ��иò�����
     * ���ͨ��ioctl ���� NET_LINK_UP ���������Ӧ�ò����socket�Ľӿڽ������Ӻ�ͨ�ţ�
     * ��ʱ���ٵ��� aw_gprs_ip_start ��aw_gprs_ip_send��aw_gprs_ip_receive
     * �� aw_gprs_ip_close �ӿڣ�
     */
    AW_GPRS_REQ_NET_LINK_UP,

    /** \brief ����Ͽ���������    */
    AW_GPRS_REQ_NET_LINK_DOWN,

    /** \brief �豸��λ��������0��Ϊ�����λ����1��ΪӲ����λ    */
    AW_GPRS_REQ_RESET,

    /** \brief ���� GPRSģ���ź���С��Чֵ�����ڸ�ֵ�޷�����ʹ���繦��   */
    AW_GPRS_REQ_CSQ_MIN_SET,

    /** \brief GPRSģ���ź�ǿ�Ȼ�ȡ   */
    AW_GPRS_REQ_CSQ_GET,

    /** \brief ��ȡGPRSģ��CCID   */
    AW_GPRS_REQ_CCID_GET,

    /** \brief ��ȡ APN  */
    AW_GPRS_REQ_APN_GET,

    /** \brief ���� APN  */
    AW_GPRS_REQ_APN_SET,

    /** \brief PPP���ŵ�IP��ַ��ȡ    */
    AW_GPRS_REQ_IP_GET,

    /** \brief ��ȡGPRSģ��İ汾��      */
    AW_GPRS_REQ_VERSION_GET,

    /** \brief ��ȡGPRSģ���IMEI��   */
    AW_GPRS_REQ_IMEI_GET,

    /** \brief ��ȡGPRSģ���IMSI��    */
    AW_GPRS_REQ_IMSI_GET,

    /** \brief ��ȡGPRSģ��Ķ���Ϣ���ĺ���   */
    AW_GPRS_REQ_SCA_GET,

    /** \brief ��ȡ����ģʽ   */
    AW_GPRS_REQ_NET_MODE_GET,

    /** \brief ��ȡ��վ��Ϣ   */
    AW_GPRS_REQ_BS_GET,

    /** \brief ���� AT ����ͨ��  */
    AW_GPRS_REQ_AT_TRANS,

    /** \brief ping����   */
    AW_GPRS_REQ_PING,

    /** \brief ��ѯ�豸״̬   */
    AW_GPRS_REQ_STS,

    /**
     * \brief
     * ģ���쳣����ģ�鲻����������ʱ�����쳣�ϵ硢AT�����շ�����Ӧ��
     * Ӧ�ȵ��ø� ioctl���ٽ��к��� close �� power off ����
     */
    AW_GPRS_EXCEPTION,

    /**\brief ������� log �Ļص� */
    AW_GPRS_LOG_SET,

    /** \brief ���� gprs �¼��ص� */
    AW_GPRS_EVT_CB_SET,

    /** \brief ��� gprs �¼��ص� */
    AW_GPRS_EVT_CB_CLR,

    /** \brief ��ȡ��4gģ�����Ķ�λ���ݣ���Ҫ  aw_gprs_gnss_enable() ʹ�� gnss ���� */
    AW_GPRS_GNSS_LOC,

} aw_gprs_ioctl_req_t;

typedef union {

    /** \brief AW_GPRS_REQ_PROTOCOL_SET ѡ��Ĳ��� */
    int             proto_opt;

    /** \brief AW_GPRS_REQ_RESET ѡ��Ĳ��� */
    struct {
        aw_bool_t       flag_hw_rst;    /**< \brief Ӳ����λ��־   */
        uint32_t        delay_ms;       /**< \brief �豸��λ���ʼ��֮�����ʱʱ��   */
    } reset;

    /** \brief AW_GPRS_REQ_CSQ_GET��AW_GPRS_REQ_CSQ_MIN_SET ѡ��Ĳ��� */
    int             csq;

    /** \brief AW_GPRS_REQ_CCID_GET ѡ��Ĳ��� */
    char            ccid[64];

    /** \brief AW_GPRS_REQ_APN_GET��AW_GPRS_REQ_APN_GET ѡ��Ĳ���,��Ϊ��̬������ַ */
    char           *p_apn;

    /** \brief AW_GPRS_REQ_PING */
    char           *p_ping_dst;

    /** \brief AW_GPRS_REQ_IP_GET ѡ��Ĳ��� */
    ip_addr_t       ip_addr;

    /** \brief AW_GPRS_REQ_VERSION_GET ѡ��Ĳ��� */
    char            version[64];

    /** \brief AW_GPRS_REQ_IMEI_GET ѡ��Ĳ��� */
    char            imei[64];

    /** \brief AW_GPRS_REQ_IMSI_GET ѡ��Ĳ��� */
    char            imsi[64];

    /** \brief AW_GPRS_REQ_SCA_GET ѡ��Ĳ��� */
    char            sca[32];

    /** \brief AW_GPRS_REQ_NET_MODE_GET ѡ��Ĳ�����Ϊ�ַ��� */
    char            net_mode[64];

    /**
     * \brief AW_GPRS_REQ_BS_GET ѡ��Ĳ���,
     *        bs_info[0] Ϊ��վ�����ţ� bs_info[1] ΪС��ID
     */
    uint32_t        bs_info[2];

    /** \brief AW_GPRS_REQ_AT_TRANS ѡ��Ĳ��� */
    struct {
        /**< \brief ���͵� AT �����ַ�����Ϊ NULL ��ֻ�������  */
        char       *p_at_cmd;

        /** \brief �ڴ����յ����ݣ����յ����������أ�Ϊ NULL ���յ��������ݻ�ʱ�򷵻�  */
        char       *p_recv_expect;
        char       *p_recv_buf;     /**< \brief ���� buf��Ϊ NULL �򲻽�������  */
        size_t      bufsize;        /**< \brief ���� buf ��С  */
        uint32_t    recvlen;        /**< \brief ���յ������ݳ���  */
        uint32_t    recv_tm;        /**< \brief ���ճ�ʱ��ms  */
    } at_trans;

    /** \brief AW_GPRS_REQ_STS ѡ��Ĳ��� */
    aw_gprs_sts_t   sts;

    /** \brief AW_GPRS_LOG_SET ѡ��Ĳ��� */
    void          (*pfn_log_out) (  void         *p_color,
                                    const char   *func_name,
                                    int           line,
                                    const char   *fmt, ...);

    /** \brief AW_GPRS_EVT_CB_SET ѡ��Ĳ��� */
    aw_gprs_evt_cb_t pfn_evt_handler;

    /** \brief AW_GPRS_GNSS_LOC ѡ����� */
    struct {
        char data[12];      /**< \brief Format: ddmmyy */
        char utc[12];       /**< \brief UTC time. Format: hhmmss.sss */
        char latitude[12];  /**< \brief Latitude. Format: ddmm.mmmm N/S */
        char longitude[12]; /**< \brief Longitude. Format: dddmm.mmmm E/W */
        char spkm[12];      /**< \brief Speed over ground. Format: xxxx.x, unit: Km/h */
    } gnss_loc;

} aw_gprs_ioctl_args_t;


/**
 * \brief ֧�ֵ� nmea ����
 */
#define AW_GPRS_GNSS_NMEA_AUTO      0
#define AW_GPRS_GNSS_NMEA_GGA       1
#define AW_GPRS_GNSS_NMEA_RMC       2
#define AW_GPRS_GNSS_NMEA_GSV       4
#define AW_GPRS_GNSS_NMEA_GSA       8
#define AW_GPRS_GNSS_NMEA_VTG       16

/**
 * \brief gnss ��������ʽ
 */
typedef enum {
    AW_GPRS_GNSS_STARTUP_HOT = 0,      /**< \brief ʹ�ø������ݣ�����ʱ��� */
    AW_GPRS_GNSS_STARTUP_WARM,         /**< \brief ʹ�ò��ָ������ݣ�����ʱ�����е� */
    AW_GPRS_GNSS_STARTUP_COLD,         /**< \brief ɾ�����и������ݣ�����ʱ������ */
    AW_GPRS_GNSS_STARTUP_AUTO = 0xFF,  /**< \brief ��ģ���Զ�����  */
} aw_gprs_gnss_startup_t;

/**
 * \brief gnss �������ò���
 */
typedef struct {
    aw_gprs_gnss_startup_t  startup_mode;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;      /** \brief Ϊ 0 ��ʹ��Ĭ��ֵ */
    } gps;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;
    } bd;

    struct {
        aw_bool_t   flag_enable;
        uint32_t    nmea_type;
    } glonass;

    /** \brief gprs �¼��ص�����������  */
    aw_gprs_evt_cb_t  pfn_evt_cb;
} aw_gprs_gnss_cfg_t;


/**
 * \brief ��ȡһ�����õ��豸
 *
 * \param[in]  timeout_sec     ��ʱʱ�䣬��λ second
 *
 * \retval > 0        :  �豸 id
 * \retval -1         :  �޿����豸
 */
int aw_gprs_any_get (uint32_t timeout_sec);

/**
 * \brief gprs�豸�ϵ粢��ʼ��
 *
 * \param[in]  id     �豸id
 *
 * \retval AW_OK        :  �ɹ�
 * \retval -AW_EINVAL   :  ������Ч
 * \retval -AW_EALREADY :  ���豸�Ѿ��ϵ�
 */
aw_err_t aw_gprs_power_on (int   id);

/**
 * \brief gprs�豸����
 *
 * \param[in]  id     �豸id
 *
 * \retval AW_OK        :  �ɹ�
 * \retval -AW_EINVAL   :  ������Ч
 * \retval -AW_EALREADY :  ���豸�Ѿ�����
 * \retval -AW_EBUSY    :  ���豸��æ���Ժ�����
 */
aw_err_t aw_gprs_power_off (int   id);

/**
 * \brief ����GPRS����
 * ʹ�øýӿڽ�������ʱ�������ʹ�õ��ⲿЭ�飬����Զ��򿪽���ppp����
 *
 * \param[in]  id       �豸id
 * \param[in]  p_mode   ģʽ: "TCP" ��  "UDP"
 * \param[in]  p_ip     ���ӷ�������IP��ַ������
 * \param[in]  port     ���ӷ������Ķ˿ں�
 * \param[in]  timeout  ��ʱʱ�䣬��λΪ��
 *
 * \retval  >= 0            : gprs���ӳɹ������ظ����ӵľ��
 * \retval  -AW_EINVAL      : ������Ч
 * \retval  -AW_ENOMSG      : GPRS�豸���ź�
 * \retval  -AW_ENOTSUP     : �� p_mode ģʽ��֧��
 */
int  aw_gprs_start (int                 id,
                    const char         *p_mode,
                    char               *p_ip,
                    uint16_t            port,
                    uint32_t            timeout);

/**
 * \brief �ر�һ��GPRS����
 * ʹ�øýӿڶϿ�����ʱ�������ʹ�õ��ⲿЭ�飬����Զ��ر�ppp����
 *
 * \param[in]  id       �豸id
 * \param[in]  hd       gprs���ӵľ��
 *
 * \retval AW_OK    �ɹ�
 */
aw_err_t aw_gprs_close (int   id, int hd);

/**
 * \brief ����GPRS����
 *
 * \note  ʹ���ڲ�Э��ջʱ
 *
 * \param[in]  id       �豸id
 * \param[in]  hd       gprs���ӵľ��
 * \param[in]  p_data   ���͵�����
 * \param[in]  len      �������ݳ���
 * \param[in]  timeout  ���ͳ�ʱ
 *
 * \retval AW_OK    �ɹ�
 */
ssize_t aw_gprs_send (int                 id,
                      int                 hd,
                      void               *p_data,
                      size_t              len,
                      uint32_t            timeout);

/**
 * \brief ����GPRS����
 *
 * \param[in]   id       �豸id
 * \param[in]   hd       gprs���ӵľ��
 * \param[out]  p_buf    �������ݻ���
 * \param[in]   len      �������ݻ����С
 *
 * \retval AW_OK    �ɹ�
 */
ssize_t aw_gprs_recv (int                 id,
                      int                 hd,
                      void               *p_buf,
                      size_t              len);


/**
 * \brief ���ŷ���
 *
 * \param[in]  id       �豸id
 * \param[in]  p_sms    ��������
 * \param[in]  len      �������ݳ���
 * \param[in]  p_num    �ֻ�����
 *
 * \retval AW_OK    �ɹ�
 */
aw_err_t aw_gprs_sms_send (int                 id,
                           uint8_t            *p_sms,
                           uint8_t             len,
                           char               *p_num);

/**
 * \brief HEX���ŷ���
 *
 * \param[in]  id       �豸id
 * \param[in]  p_sms    ��������
 * \param[in]  len      �������ݳ���
 * \param[in]  p_num    �ֻ�����
 *
 * \retval AW_OK    �ɹ�
 */
aw_err_t aw_gprs_sms_hex_send (int                 id,
                               uint8_t            *p_sms,
                               uint8_t             len,
                               char               *p_num);

/**
 * \brief ���Ž���
 *
 * \param[in]   id       �豸id
 * \param[out]  p_sms    ��������
 * \param[in]   timeout  ���յ����ŵĳ�ʱʱ��
 *
 * \return > 0     ����ʣ����ſ��Լ�������
 * \retval = 0     ���Ž������
 * \retval = -1    ���Ž���ʧ��
 */
aw_err_t aw_gprs_sms_recv (int                      id,
                           struct aw_gprs_sms_buf  *p_sms,
                           uint32_t                 timeout);

/**
 * \brief ɾ��ȫ������
 *
 * \param[in]  id       �豸id
 *
 * \retval AW_OK    �ɹ�
 */
aw_err_t aw_gprs_all_sms_del (int       id);

/**
 * \brief io����
 *
 * \param[in]     id      : �豸id
 * \param[in]     req     : io����
 * \param[in,out] p_arg   : ����
 *
 * \retval  AW_OK        : �����ɹ�
 * \retval  -AW_EINVAL   : ������Ч
 * \retval  -AW_ENOTSUP  : ��ǰ�豸��֧�ָ�req����
 * \retval  -AW_EPERM    : ��������иò���������ʹ���ڲ�Э��ջʱ���������ppp���Ų���
 * \retval  -AW_ENOMSG   : AT ��������Ӧ
 * \retval  -AW_ENOMEM   : ��Դ����
 * \retval  -AW_EEXIST   : �Ѵ���
 *
 * \return  ELSE         : ��������
 *
 * \par ����
 * \code
 *
 *  // ���� AT ���������Ӧʾ��
 *  aw_err_t ret;
 *  aw_gprs_ioctl_args_t arg;
 *  int  dev_id = 0;   //ʹ��idΪ0��gprs�豸����id�� awbl_hwconf_gprs_xxx.h �����
 *
 *  //����AT������
 *  char  rec_buf[256];
 *
 *  memset(rec_buf, 0, 256);
 *
 *  arg.at_trans.p_at_cmd        = "AT+CSQ\r\n";
 *  arg.at_trans.p_recv_expect   = "CSQ:";
 *  arg.at_trans.p_recv_buf      = rec_buf;
 *  arg.at_trans.bufsize         = sizeof(rec_buf);
 *  arg.at_trans.recv_tm         = 1500;
 *
 *  ret = aw_gprs_ioctl(dev_id, AW_GPRS_REQ_AT_TRANS, &arg);
 *  if (ret == AW_OK) {
 *      AW_INFOF(("recv : %s.\n", rec_buf));
 *  }
 *
 * \endcode
 */
aw_err_t aw_gprs_ioctl (int                   id,
                        aw_gprs_ioctl_req_t   req,
                        aw_gprs_ioctl_args_t *p_arg);

/**
 * \brief ʹ�� gps ����
 *
 * \param dev_id            4g ģ�� id
 * \param p_gnss_cfg        gnss ����
 *
 * \retval  AW_OK           �����ɹ�
 * \retval  -AW_EINVAL      �����쳣
 * \retval  -AW_ENODEV      ���豸
 */
aw_err_t aw_gprs_gnss_enable (int dev_id, aw_gprs_gnss_cfg_t *p_gnss_cfg);

/**
 * \brief �ر� gps ����
 *
 * \param dev_id            4g ģ�� id
 *
 * \retval  AW_OK           �����ɹ�
 * \retval  -AW_EINVAL      �����쳣
 * \retval  -AW_ENODEV      ���豸
 */
aw_err_t aw_gprs_gnss_disable (int dev_id);

/**
 * \brief ��ȡ gnss Դ����
 *
 * \param dev_id            4g ģ�� id
 * \param p_buf             ���buf
 * \param bufsize           buf��С
 * \param timeout           ms ��ʱ
 *
 * \retval  >=0             ���ػ�ȡ���ݳ���
 * \retval  -AW_EINVAL      �����쳣
 * \retval  -AW_ENODEV      ���豸
 */
aw_err_t aw_gprs_gnss_nmeasrc_get (int      dev_id,
                                   uint8_t *p_buf,
                                   size_t   bufsize,
                                   uint32_t timeout);

/** @} grp_aw_if_gprs */

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPRS_H_ */

/* end of file */

