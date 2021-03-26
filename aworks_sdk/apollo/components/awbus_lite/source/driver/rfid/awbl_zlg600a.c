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
 * \brief AWBus ZLG600A interface implementation (lite)
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_zlg600a.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-05-04  win, first implementation
 * \endinternal
 */

#include "apollo.h"
#include "aw_gpio.h"

#include "awbus_lite.h"
#include "driver/rfid/awbl_zlg600a.h"

#include <string.h>

/*******************************************************************************
  locals
*******************************************************************************/

AWBL_METHOD_DEF(awbl_zlg600aserv_get, "awbl_zlg600aserv_get");

/* pointer to first rtc service */
struct awbl_zlg600a_service *__gp_zlg600a_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief allocate zlg600a service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __zlg600a_serv_alloc_helper (struct awbl_dev *p_dev, 
                                               void            *p_arg)
{
    awbl_method_handler_t         pfunc_zlg600a_serv = NULL;
    struct awbl_zlg600a_service  *p_zlg600a_serv     = NULL;

    /* find handler */
    pfunc_zlg600a_serv = awbl_dev_method_get(
                            p_dev,
                            AWBL_METHOD_CALL(awbl_zlg600aserv_get));

    /* call method handler to allocate ZLG600A service */
    if (pfunc_zlg600a_serv != NULL) {

        pfunc_zlg600a_serv(p_dev, &p_zlg600a_serv);

        /* found a ZLG600A service, insert it to the service list */
        if (p_zlg600a_serv != NULL) {
            struct awbl_zlg600a_service **pp_serv_cur = &__gp_zlg600a_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_zlg600a_serv;
            p_zlg600a_serv->p_next = NULL;

            AW_MUTEX_INIT(p_zlg600a_serv->devlock, AW_SEM_Q_PRIORITY);
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate zlg600a services from instance tree
 */
aw_local aw_err_t __zlg600a_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every zlg600a device will be find */
    __gp_zlg600a_serv_head = NULL;

    awbl_dev_iterate(__zlg600a_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the id
 */
aw_local struct awbl_zlg600a_service * __zlg600a_id_to_serv (int id)
{
    struct awbl_zlg600a_service *p_serv_cur = __gp_zlg600a_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_servinfo->dev_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/**
 * \brief AWBus ZLG600A �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init2() ֮�����
 */
void awbl_zlg600a_init (void)
{
    /* allocate rtc services from instance tree */
    __zlg600a_serv_alloc();
}

/******************************************************************************/

/**
 * \brief �����豸�е�frame_fmt����һ��֡
 */
aw_local aw_err_t __zlg600a_frame_proc (
                     int                           id,
                     uint8_t                       cmd_class,
                     uint16_t                      cmd,
                     awbl_zlg600a_info_buf_desc_t *p_cmd_desc,
                     uint8_t                       cmd_buf_num,
                     awbl_zlg600a_info_buf_desc_t *p_res_desc,
                     uint8_t                       res_buf_num,
                     uint16_t                     *p_res_info_len)

{
    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    aw_err_t ret = -AW_ENOTSUP;

    awbl_zlg600a_frame_base_t frame;

    aw_err_t (*pfn_temp)(void *, awbl_zlg600a_frame_base_t *);

    int      i;
    uint16_t total_len;

    if (p_serv == NULL) {
        return -AW_ENXIO;  /* ZLG600A[id] is invalid */
    }

    if (cmd_buf_num != 0 && p_cmd_desc == NULL) {
        return -AW_EINVAL;
    }

    if (res_buf_num != 0 && p_res_desc == NULL) {
        return -AW_EINVAL;
    }

    total_len = 0;
    for (i = 0; i < cmd_buf_num; i++) {
        total_len += p_cmd_desc[i].len;
    }

    p_serv->frame_seq = (p_serv->frame_seq + 1) & 0xF;

    frame.frame_seq   = p_serv->frame_seq;
    frame.cmd_class   = cmd_class;
    frame.cmd_status  = cmd;
    frame.info_length = total_len;
    frame.p_info_desc = p_cmd_desc;
    frame.buf_num     = cmd_buf_num;

    pfn_temp = (aw_err_t (*)(void *, awbl_zlg600a_frame_base_t *))
                p_serv->p_servopts->pfn_frame_send;
    if (pfn_temp == NULL) {
        return -AW_ENOTSUP;
    }

    AW_MUTEX_LOCK(p_serv->devlock, AW_SEM_WAIT_FOREVER);

    ret = pfn_temp(p_serv->p_cookie, &frame);
    if (ret != AW_OK) {
        return ret;
    }

    frame.p_info_desc = p_res_desc;
    frame.buf_num     = res_buf_num;

    pfn_temp = (aw_err_t (*)(void *, awbl_zlg600a_frame_base_t *))
                p_serv->p_servopts->pfn_frame_recv;
    if (pfn_temp == NULL) {
        return -AW_ENOTSUP;
    }

    /* ���ջ�Ӧ֡�����ж��Ƿ�ɹ� */
    ret = pfn_temp(p_serv->p_cookie, &frame);

    AW_MUTEX_UNLOCK(p_serv->devlock);

    if (p_res_info_len != NULL) {
        *p_res_info_len = frame.info_length;
    }

    return ret;
}

/**
 * \brief ��ZLG600A�Ĳ�Ʒ���汾��Ϣ���磺��ZLG600A V1.00��
 */
aw_err_t awbl_zlg600a_info_get (int id, uint8_t *p_info)
{
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    rx_desc[0].p_buf = p_info;
    rx_desc[0].len   = 0x14;          /* �̶����ճ���0x14 */

    return __zlg600a_frame_proc(id,
                                1,
                                'A',
                                NULL,
                                0,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief ����IC���ӿڣ�ִ�гɹ�������IC���ӿڣ�������Ĭ��֧��TypeA��
 */
aw_err_t awbl_zlg600a_ic_port_config (int id)
{
    return __zlg600a_frame_proc(id,
                                1,
                                'B',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief �ر�IC���ӿ�
 */
aw_err_t awbl_zlg600a_ic_port_close (int id)
{
    return __zlg600a_frame_proc(id,
                                1,
                                'C',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ����IC���ӿ�Э�飨����ģʽ��TypeA ���� TypeB
 */
aw_err_t awbl_zlg600a_ic_isotype_set (int id, uint8_t isotype)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &isotype;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'D',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief װ��IC����Կ
 *
 * ���������Կ������ģ���ڲ���ģ���������Կ����ʧ��ZLG600Aģ�鹲�ܱ���
 * A��Կ16�顢B��Կ16�顣
 *
 */
aw_err_t awbl_zlg600a_ic_key_load (int      id,
                                   uint8_t  key_type,
                                   uint8_t  key_sec,
                                   uint8_t *p_key,
                                   uint8_t  key_length)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[3];

    if (p_key == NULL) {
        return -AW_EINVAL;
    }

    /* ��Կ���� */
    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    /* ��Կ���� */
    tx_desc[1].p_buf = &key_sec;
    tx_desc[1].len   = 1;

    /* ��Կ    */
    tx_desc[2].p_buf = p_key;
    tx_desc[2].len   = key_length;

    return __zlg600a_frame_proc(id,
                                1,
                                'E',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ����IC���ӿڵļĴ���ֵ
 *
 * ��������ģ���϶�д��оƬ�ڲ��ļĴ���ֵ��ͨ�����������ʵ�ֺܶ���������
 * ������ɵĹ�����
 */
aw_err_t awbl_zlg600a_ic_reg_set (int id, uint8_t reg_addr, uint8_t reg_val)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    /* �Ĵ�����ַ */
    tx_desc[0].p_buf = &reg_addr;
    tx_desc[0].len   = 1;

    /* �Ĵ���ֵ  */
    tx_desc[1].p_buf = &reg_val;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'F',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ��ȡIC���ӿڵļĴ���ֵ
 *
 * ��������������ģ���϶�д��оƬ�ڲ��ļĴ���ֵ��
 * ͨ�����������ʵ�ֺܶ������������ɵĹ�����
 */
aw_err_t awbl_zlg600a_ic_reg_get (int id, uint8_t reg_addr, uint8_t *p_val)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �Ĵ�����ַ     */
    tx_desc[0].p_buf = &reg_addr;
    tx_desc[0].len   = 1;

    /* ��ȡ�ļĴ���ֵ */
    rx_desc[0].p_buf = p_val;
    rx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief ���ò�����
 *
 * ������UARTͨ�Ź����иı�ͨ�ŵĲ����ʣ�����������ֵ������
 */
aw_err_t awbl_zlg600a_baudrate_set (int id, uint32_t baudrate_flag)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];
    uint8_t                       num = 0;     /* �����ʱ��룬��Ӧ0 ~ 7 */

    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    switch(baudrate_flag) {

    case AWBL_ZLG600A_BAUDRATE_9600:
        num = 0;
        break;

    case AWBL_ZLG600A_BAUDRATE_19200:
        num = 1;
        break;

    case AWBL_ZLG600A_BAUDRATE_28800:
        num = 2;
        break;

    case AWBL_ZLG600A_BAUDRATE_38400:
        num = 3;
        break;
    case AWBL_ZLG600A_BAUDRATE_57600:
        num = 4;
        break;

    case AWBL_ZLG600A_BAUDRATE_115200:
        num = 5;
        break;

    case AWBL_ZLG600A_BAUDRATE_172800:
        num = 6;
        break;

    case AWBL_ZLG600A_BAUDRATE_230400:
        num = 7;
        break;

    default:
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &num;
    tx_desc[0].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'H',
                               tx_desc,
                               1,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_BAUD_SET,
                                                (void *)baudrate_flag);
    }

    return ret;
}

/**
 * \brief ��������������ʽ����������򿪡��ر�ĳ��������������
 */
aw_err_t awbl_zlg600a_ant_mode_set (int id, uint8_t ant_mode)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    tx_desc[0].p_buf = &ant_mode;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'I',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ����֡��ʽ�������л��¡���֡��ʽ�����óɹ�����粻��ʧ
 */
aw_err_t awbl_zlg600a_frame_fmt_set (int id, uint8_t frame_type)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    aw_err_t  ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    tx_desc[0].p_buf = &frame_type;
    tx_desc[0].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'K',
                               tx_desc,
                               1,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET,
                                                (void *)(int)frame_type);
    }

    return ret;
}

/**
 * \brief �����豸����ģʽ�ʹӻ���ַ
 */
aw_err_t awbl_zlg600a_mode_addr_set (int id, uint8_t mode, uint8_t new_addr)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];

    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &new_addr;
    tx_desc[1].len   = 1;

    ret = __zlg600a_frame_proc(id,
                               1,
                               'U',
                               tx_desc,
                               2,
                               NULL,
                               0,
                               NULL);

    if (ret != AW_OK) {
        return ret;
    }

    if (p_serv->p_servopts->pfn_dev_ioctl != NULL) {
        ret = p_serv->p_servopts->pfn_dev_ioctl(p_serv->p_cookie,
                                                AWBL_ZLG600A_DEV_IOCTL_ADDR_SET,
                                                (void *)(int)new_addr);
    }

    return AW_OK;
}

/**
 * \brief ��ȡ�豸����ģʽ�ʹӻ���ַ
 */
aw_err_t awbl_zlg600a_mode_addr_get (int id, uint8_t *p_mode, uint8_t *p_addr)
{
    awbl_zlg600a_info_buf_desc_t  rx_desc[2];

    rx_desc[0].p_buf = p_mode;
    rx_desc[0].len   = 1;

    rx_desc[1].p_buf = p_addr;
    rx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                1,
                                'V',
                                NULL,
                                0,
                                rx_desc,
                                2,
                                NULL);
}

/**
 * \brief װ���û���Կ��ģ�������ṩ��2��16�ֽڵĴ洢�ռ����ڱ����û���Կ��
 */
aw_err_t awbl_zlg600a_user_key_load (int id, uint8_t key_sec, uint8_t *p_key)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];

    if (p_key == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_sec;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;              /* ���ȹ̶�Ϊ16�ֽ�  */

    return __zlg600a_frame_proc(id,
                                1,
                                'a',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief дEEPROM��ģ���ڲ�ӵ��һ��256Byte��EEPROM
 */
aw_err_t awbl_zlg600a_eeprom_write (int      id,
                                    uint8_t  eeprom_addr,
                                    uint8_t *p_buf,
                                    uint8_t  nbytes)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &eeprom_addr;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nbytes;
    tx_desc[1].len   = 1;              /* ���ȹ̶�Ϊ16�ֽ�  */

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = nbytes;

    return __zlg600a_frame_proc(id,
                                1,
                                'c',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief ��EEPROM��ģ���ڲ�ӵ��һ��256Byte��EEPROM
 */
aw_err_t awbl_zlg600a_eeprom_read (int      id,
                                   uint8_t  eeprom_addr,
                                   uint8_t *p_buf,
                                   uint8_t  nbytes)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[3];

    tx_desc[0].p_buf = &eeprom_addr;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nbytes;
    tx_desc[1].len   = 1;              /* ���ȹ̶�Ϊ16�ֽ�  */

    tx_desc[2].p_buf = p_buf;
    tx_desc[2].len   = nbytes;

    return __zlg600a_frame_proc(id,
                                1,
                                'b',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ��ȡ����оƬ��λ����
 */
aw_err_t awbl_zlg600a_ic_reset_count_get (int id, uint32_t *p_count)
{
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    rx_desc[0].p_buf = (uint8_t *)p_count;
    rx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                1,
                                'd',
                                NULL,
                                0,
                                rx_desc,
                                1,
                                NULL);
}

/*******************************************************************************
   ISO14443ͨ�ú���������ʹ�ã�
*******************************************************************************/

/*
 *     ���������������������ISO14443Э��涨�õģ�Mifare �� �� PICC������һ����
 * ����,�����ֿ����������Ͳ�ͬ��
 */
aw_local aw_err_t __zlg600a_iso14443_request (int       id,
                                              uint8_t   cmd_type,
                                              uint8_t   req_mode,
                                              uint16_t *p_atq)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[1];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_atq;
    rx_desc[0].len   = 2;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'A',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/* ����ײ */
aw_local aw_err_t __zlg600a_iso14443_anticoll (int       id,
                                               uint8_t   cmd_type,
                                               uint8_t   anticoll_level,
                                               uint32_t  know_uid,
                                               uint8_t   nbit_cnt,
                                               uint32_t *p_uid)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[3];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &anticoll_level;
    tx_desc[0].len   = 1;

    /* ��֪���кŵĳ��� */
    tx_desc[1].p_buf = &nbit_cnt;
    tx_desc[1].len   = 1;

    /* ��֪UID����֪UID��λ���� nbit_cnt ���� */
    tx_desc[2].p_buf = (uint8_t *)&know_uid;
    tx_desc[2].len   = 4;

    /* ���յ�4�ֽ�UID */
    rx_desc[0].p_buf = (uint8_t *)p_uid;
    rx_desc[0].len   = 4;

    if (nbit_cnt == 0) {

        return __zlg600a_frame_proc(id,
                                    cmd_type,
                                    'B',
                                    tx_desc,
                                    2,
                                    rx_desc,
                                    1,
                                    NULL);
    } else {

        return __zlg600a_frame_proc(id,
                                    cmd_type,
                                    'B',
                                    tx_desc,
                                    3,
                                    rx_desc,
                                    1,
                                    NULL);
    }
}

/* ��ѡ�� */
aw_local aw_err_t __zlg600a_iso14443_select (int       id,
                                             uint8_t   cmd_type,
                                             uint8_t   anticoll_level,
                                             uint32_t  uid,
                                             uint8_t  *p_sak)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[1];

    tx_desc[0].p_buf = &anticoll_level;
    tx_desc[0].len   = 1;

    /* ��֪���к�     */
    tx_desc[1].p_buf = (uint8_t *)&uid;
    tx_desc[1].len   = 4;

    /* ���յ�1�ֽ�SAK */
    rx_desc[0].p_buf = p_sak;
    rx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'C',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/* ������ */
aw_local aw_err_t __zlg600a_iso14443_halt (int id, uint8_t cmd_type)
{
    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'D',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/* ����λ */
aw_local aw_err_t __zlg600a_iso14443_reset (int     id,
                                            uint8_t cmd_type,
                                            uint8_t time_ms)
{

    awbl_zlg600a_info_buf_desc_t  tx_desc[1];

    tx_desc[0].p_buf = &time_ms;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'L',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/* ������*/
aw_local aw_err_t __zlg600a_iso14443_active (int       id,
                                             uint8_t   cmd_type,
                                             uint8_t   req_mode,
                                             uint16_t *p_atq,
                                             uint8_t  *p_sak,
                                             uint8_t  *p_len,
                                             uint8_t  *p_uid,
                                             uint8_t   uid_len)
{
    awbl_zlg600a_info_buf_desc_t  tx_desc[2];
    awbl_zlg600a_info_buf_desc_t  rx_desc[4];

    uint8_t data = 0x00;                  /* �����ֽڣ�����Ϊ0x00 */

    /* ��һ�ֽ�Ϊ���� */
    data = 0x00;
    tx_desc[0].p_buf = &data;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &req_mode;
    tx_desc[1].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_atq; /* ����Ӧ��2�ֽڣ�          */
    rx_desc[0].len   = 2;

    rx_desc[1].p_buf = p_sak;            /* ���һ��ѡ��Ӧ��SAK��1�ֽڣ�*/
    rx_desc[1].len   = 1;

    rx_desc[2].p_buf = p_len;            /* ���кų��ȣ�1�ֽڣ�        */
    rx_desc[2].len   = 1;

    rx_desc[3].p_buf = p_uid;            /* ���кţ�N�ֽڣ������кų��Ⱦ����� */
    rx_desc[3].len   = uid_len;          /* ���10�ֽڣ�ֱ�Ӱ���10�ֽڶ�ȡ    */

    return __zlg600a_frame_proc(id,
                                cmd_type,
                                'M',
                                tx_desc,
                                2,
                                rx_desc,
                                4,
                                NULL);
}

/*******************************************************************************
  Mifare������
*******************************************************************************/

/**
 * \brief Mifare���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 * \note ���������ߺ󣬴���Ƶ���л�ȡ�������Ӷ��õ縴λ����λ�󿨴���IDLEģʽ��
 * ����������ģʽ����һ������ʱ����ʱ�Ŀ�������Ӧ������ĳһ�ſ��ɹ������˹���
 * ������Halt�����DeSelect������������Haltģʽ����ʱ�Ŀ�ֻ��ӦALL��0x52��
 * ģʽ�����󣬳��ǽ����뿪���߸�Ӧ�����ٽ��롣
 */
aw_err_t awbl_zlg600a_mifare_card_request (int       id,
                                           uint8_t   req_mode,
                                           uint16_t *p_atq)
{
    return __zlg600a_iso14443_request(id,
                                      0x02,
                                      req_mode,
                                      p_atq);
}

/**
 * \brief Mifare���ķ���ײ����
 *
 *  ��Ҫ�ɹ�ִ��һ�������������������ɹ������ܽ��з���ײ���������򷵻ش���
 *
 * \note  ����ISO14443A��׼�������кŶ���ȫ��Ψһ�ģ���������Ψһ�ԣ�����ʵ�ַ�
 * ��ײ���㷨�߼������������ſ�ͬʱ�����߸�Ӧ��������������ܹ��ҵ�һ�����кŽ�
 * ��Ŀ���������
 */
aw_err_t awbl_zlg600a_mifare_card_anticoll (int       id,
                                            uint8_t   anticoll_level,
                                            uint32_t  know_uid,
                                            uint8_t   nbit_cnt,
                                            uint32_t *p_uid)
{
    return __zlg600a_iso14443_anticoll(id,
                                       0x02,
                                       anticoll_level,
                                       know_uid,
                                       nbit_cnt,
                                       p_uid);
}

/**
 * \brief Mifare����ѡ�����
 *
 *  ��Ҫ�ɹ�ִ��һ�η���ײ��������سɹ������ܽ��п�ѡ����������򷵻ش���
 *
 * \note  �������кų��������֣�4�ֽڡ�7�ֽں�10�ֽڡ� 4�ֽڵ�ֻҪ��һ��ѡ�񼴿�
 * �õ����������кţ���Mifare1 S50/S70�ȣ�7�ֽڵ�Ҫ�ö���ѡ����ܵõ�����������
 * �ţ�ǰһ�����õ������кŵ�����ֽ�Ϊ������־0x88�������к���ֻ�к�3�ֽڿ��ã�
 * ��һ��ѡ���ܵõ�4�ֽ����кţ����߰�˳�����Ӽ�Ϊ7�ֽ����кţ���UltraLight
 * ��DesFire�ȣ�10�ֽڵ��Դ����ƣ�������δ���ִ��࿨��
 */
aw_err_t awbl_zlg600a_mifare_card_select (int       id,
                                          uint8_t   anticoll_level,
                                          uint32_t  uid,
                                          uint8_t  *p_sak)
{
    return __zlg600a_iso14443_select(id,
                                     0x02,
                                     anticoll_level,
                                     uid,
                                     p_sak);
}

/**
 * \brief Mifare���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *  ��HALT״̬�£���������Ӧ������������IDLEģʽ�����󣬳��ǽ�����λ���뿪���߸�
 *  Ӧ�����ٽ��롣��������Ӧ������������ALL����
 */
aw_err_t awbl_zlg600a_mifare_card_halt (int id)
{
    return __zlg600a_iso14443_halt(id, 0x02);
}


/**
 * \brief Mifare��E2��Կ��֤
 *
 *     ��ģ���ڲ��Ѵ������Կ�뿨����Կ������֤��ʹ�ø�����ǰӦ���á�װ��IC����Կ��
 * ��������Կ�ɹ�����ģ���ڡ����⣬��Ҫ��֤�Ŀ��������Ų�����ģ������Կ������ȡ�
 *
 * \note PLUS CPUϵ�еĿ��Ŀ�����4�ֽں�7�ֽ�֮�֣�����7�ֽڿ��ŵĿ���
 *       ֻ��Ҫ�����ŵĸ�4�ֽڣ��ȼ�2����ײ�õ��Ŀ��ţ���Ϊ��֤�Ŀ��ż��ɡ�
 */
aw_err_t awbl_zlg600a_mifare_card_e2_authent (int      id,
                                              uint8_t  key_type,
                                              uint8_t *p_uid,
                                              uint8_t  key_sec,
                                              uint8_t  nblock)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if (key_sec > 7) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_uid;
    tx_desc[1].len   = 4;

    tx_desc[2].p_buf = &key_sec;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = &nblock;
    tx_desc[3].len   = 1;

    return __zlg600a_frame_proc(id,
                                2,
                                'E',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare��ֱ����Կ��֤
 *
 *    �����������Ϊ�������ݣ�����ڴ�֮ǰ�����á�װ��IC����Կ���������ǰ��
 * ΪPLUS CPU���ĵȼ�2��ȼ�3�������������ֻ��6�ֽڣ���ģ���Զ������������
 * ����2�Σ�ȡǰ16�ֽ���Ϊ��ǰ��֤��Կ��
 *
 * \note PLUS CPUϵ�еĿ��Ŀ�����4�ֽں�7�ֽ�֮�֣�����7�ֽڿ��ŵĿ���
 *       ֻ��Ҫ�����ŵĸ�4�ֽڣ��ȼ�2����ײ�õ��Ŀ��ţ���Ϊ��֤�Ŀ��ż��ɡ�
 */
aw_err_t awbl_zlg600a_mifare_card_direct_authent (int      id,
                                                  uint8_t  key_type,
                                                  uint8_t *p_uid,
                                                  uint8_t *p_key,
                                                  uint8_t  key_len,
                                                  uint8_t  nblock)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if (((key_len != 6) && (key_len != 16))) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &key_type;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_uid;
    tx_desc[1].len   = 4;

    tx_desc[2].p_buf = p_key;
    tx_desc[2].len   = key_len;

    tx_desc[3].p_buf = &nblock;
    tx_desc[3].len   = 1;

    return __zlg600a_frame_proc(id,
                                2,
                                'F',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare�����ݶ�ȡ
 *
 *     ����֤�ɹ�֮�󣬲��ܶ���Ӧ�Ŀ����ݣ�����֤�Ŀ�������ű�����ͬһ�������ڣ�
 * Mifare1���ӿ��0��ʼ��˳��ÿ4����1����������Ҫ��һ�ſ��еĶ���������в�������
 * ��ĳһ����������Ϻ󣬱������һ����������ܶ���һ������ֱ�ӽ�����֤�������
 * ���������ʼ����������PLUS CPU��������һ������������Կ�͵�ǰ��������Կ��ͬ��
 * ����Ҫ�ٴ���֤��Կ��ֱ�Ӷ����ɡ�
 *
 * \param[in] p_dev       : ָ��ZLG600A�豸��ָ��
 * \param[in] nblock      : ��ȡ���ݵĿ��
 *                         - S50��0 ~ 63
 *                         - S70��0 ~ 255
 *                         - PLUS CPU 2K��0 ~ 127
 *                         - PLUS CPU 4K��0 ~ 255
 * \param[in] p_buf      : ��Ŷ�ȡ�����ݣ���СΪ 16
 *
 * \retval AW_OK      : ��ȡ�ɹ�
 * \retval -AW_EINVAL : ��ȡʧ�ܣ���������
 * \retval -AW_EIO    : ��ȡʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_read (int      id,
                                        uint8_t  nblock,
                                        uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = 16;

    return __zlg600a_frame_proc(id,
                                2,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief Mifare��д����,д֮ǰ����ɹ�������Կ��֤��
 *
 *  �Կ���ĳһ�������֤�ɹ��󣬼��ɶ�ͬһ�����ĸ�������д������ֻҪ������������
 *  �������а���λ������β������飬���Ǹ��������Ψһ����������PLUS CPU���ȼ�
 *  2��3��AES��Կ����������λ���޸���Կ��
 */
aw_err_t awbl_zlg600a_mifare_card_write (int      id,
                                         uint8_t  nblock,
                                         uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                2,
                                'H',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare��д����
 *
 * ������ֻ��UltraLight����Ч����UltraLight�����ж�������Mifare1��һ����
 */
aw_err_t awbl_zlg600a_ultralight_card_write (int      id,
                                             uint8_t  nblock,
                                             uint8_t *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if (p_buf == NULL) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'I',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifareֵ��������Mifare����ֵ����мӼ�����
 *
 * \note Ҫ���д�������������ݱ���Ҫ��ֵ��ĸ�ʽ���ɲο�NXP������ĵ���������
 * ���봫������ͬ���򽫲�����Ľ��д��ԭ���Ŀ��ڣ���������봫���Ų���ͬ��
 * �򽫲�����Ľ��д�봫����ڣ����������ڵ����ݱ����ǣ�ԭ���ڵ�ֵ���䡣
 * ���ڵȼ�2��PLUS CPU����֧��ֵ��������ȼ�1��3֧�֡�
 */
aw_err_t awbl_zlg600a_mifare_card_val_operate (int     id,
                                               uint8_t mode,
                                               uint8_t nblock,
                                               uint8_t ntransblk,
                                               int32_t value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &nblock;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = (uint8_t *)&value;
    tx_desc[2].len   = 4;

    tx_desc[3].p_buf = &ntransblk;
    tx_desc[3].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'J',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifare����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \param[in] p_dev      : ָ��ZLG600A�豸��ָ��
 * \param[in] time_ms    : �رյ�ʱ�䣨��λ:ms����0Ϊһֱ�ر�
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 *
 * \note �ú����������źŹر������룬��һֱ�رգ���ȵ�ִ��һ����������ʱ�򿪡�
 */
aw_err_t awbl_zlg600a_mifare_card_reset (int     id,
                                         uint8_t time_ms)
{
    return __zlg600a_iso14443_reset(id, 0x02, time_ms);
}

/**
 * \brief Mifare������ú������ڼ��Ƭ�������󡢷���ײ��ѡ�������������ϡ�
 */
aw_err_t awbl_zlg600a_mifare_card_active (int       id,
                                          uint8_t   req_mode,
                                          uint16_t *p_atq,
                                          uint8_t  *p_sak,
                                          uint8_t  *p_len,
                                          uint8_t  *p_uid,
                                          uint8_t   uid_len)
{
    return __zlg600a_iso14443_active(id,
                                     0x02,
                                     req_mode,
                                     p_atq,
                                     p_sak,
                                     p_len,
                                     p_uid,
                                     uid_len);
}

#if 0
/**
 * \brief Mifare���Զ��������һ���ص�����
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_connect (
    int             id,
    aw_pfuncvoid_t  pfn_callback,
    void           *p_arg)
{
    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    p_serv->pfn_callback = pfn_callback;
    p_serv->p_arg        = p_arg;

    return AW_OK;
}

/**
 * \brief ����Mifare���Զ����
 * \note ���Զ�����ڼ䣬�����������κγ����Զ����������ģ������ݳ���С��3
 *       ��������˳��Զ����ģʽ��������awbl_zlg600a_mifare_card_active()
 *       ����ڴ��ڼ䣬ģ�齫���������ݳ��ȴ���2������
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_start (
    int                              id,
    awbl_zlg600a_auto_detect_info_t *p_info)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[7];

    uint8_t  tx_desc_num;
    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    if ((p_serv->mode & 0xF0) == AWBL_ZLG600A_MODE_MASTER) {
        return AW_OK;
    }

    tx_desc[0].p_buf = &p_info->ad_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &p_info->tx_mode;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &p_info->req_mode;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = &p_info->auth_mode;
    tx_desc[3].len   = 1;

    tx_desc[4].p_buf = &p_info->key_type;
    tx_desc[4].len   = 1;

    switch (p_info->auth_mode) {

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_DIRECT :
        tx_desc[5].p_buf = p_info->key;
        tx_desc[5].len   = p_info->key_len;

        tx_desc[6].p_buf = &p_info->nblock;
        tx_desc[6].len   = 1;

        tx_desc_num = 7;
        break;

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_E2 :
        tx_desc[5].p_buf = &p_info->key[0];
        tx_desc[5].len   = 1;

        tx_desc[6].p_buf = &p_info->nblock;
        tx_desc[6].len   = 1;

        tx_desc_num = 7;
        break;

    case AWBL_ZLG600A_MIFARE_CARD_AUTH_NO :
        tx_desc_num = 4;
        break;

    default :
        return -AW_EINVAL;
    }

    ret = __zlg600a_frame_proc(id,
                               2,
                               'N',
                               tx_desc,
                               tx_desc_num,
                               NULL,
                               0,
                               NULL);

    if (ret == AW_OK) {
        p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_MASTER;
        p_serv->ad_read = AW_TRUE;

        AW_SEMB_GIVE(p_serv->semb_ad);
    }

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}

/**
 * \brief �ر�Mifare���Զ����
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_stop (int id)
{
    //uint8_t  mode, addr;
    aw_err_t ret;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    /* ��ʱ�˳�����ģʽ */
    p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_SLAVE;
    p_serv->ad_read = AW_FALSE;

    ret = awbl_zlg600a_mode_addr_get(id, NULL, NULL);

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}

/**
 * \brief Mifare����ȡ�Զ������������
 */
aw_err_t awbl_zlg600a_mifare_card_auto_detect_read (
    int                              id,
    awbl_zlg600a_mifare_card_info_t *p_card_info)
{
    awbl_zlg600a_info_buf_desc_t rx_desc[4];

    aw_err_t ret;
    uint16_t real_len;
    uint8_t  i, j, k;

    struct awbl_zlg600a_service *p_serv = __zlg600a_id_to_serv(id);
    if (p_serv == NULL) {
        return -AW_ENXIO;
    }

    __ZLG600A_AD_LOCK(p_serv);

    rx_desc[0].p_buf = (uint8_t *)&p_card_info->tx_mode;
    rx_desc[0].len   = 1;

    rx_desc[1].p_buf = NULL;
    rx_desc[1].len   = 3;

    rx_desc[2].p_buf = (uint8_t *)&p_card_info->uid_len;
    rx_desc[2].len   = 1;

    rx_desc[3].p_buf = (uint8_t *)p_card_info->uid;
    rx_desc[3].len   = 26;

    ret = __zlg600a_frame_proc(id,
                               2,
                               'O',
                               NULL,
                               0,
                               rx_desc,
                               4,
                               &real_len);

    if (ret != AW_OK) {
        return ret;
    }

    i = p_card_info->uid_len + 2;
    j = real_len - 3;

    if (i != j) {
        i = real_len - 3 - 1;
        j = sizeof(awbl_zlg600a_mifare_card_info_t) - 1;
        for (k = 0; k < sizeof(p_card_info->card_data); k++) {
            ((uint8_t *)p_card_info)[j] = ((uint8_t *)p_card_info)[i];
            j--;
            i--;
        }

        for (i = p_card_info->uid_len; i < sizeof(p_card_info->uid); i++) {
            p_card_info->uid[i] = 0;
        }
    }

    p_serv->mode    = (p_serv->mode & 0x0F) | AWBL_ZLG600A_MODE_SLAVE;
    p_serv->ad_read = AW_FALSE;

    __ZLG600A_AD_UNLOCK(p_serv);

    return ret;
}
#endif

/**
 * \brief Mifareֵ����������ֵ���ֵ
 */
aw_err_t awbl_zlg600a_mifare_card_val_set (int     id, 
                                           uint8_t nblock, 
                                           int32_t value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&value;
    tx_desc[1].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'P',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief Mifareֵ��������ȡֵ���ֵ
 */
aw_err_t awbl_zlg600a_mifare_card_val_get (int     id, 
                                           uint8_t nblock,
                                           int32_t *p_value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &nblock;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = (uint8_t *)p_value;
    rx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                2,
                                'Q',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief �����
 *
 *     �ú�������ģ����չ���ܣ�����ģ����Ƭ�������ⳤ����ϵ����ݴ����������
 * NXP���Ƴ���NTAG213F������Ultralight Cϵ�п�Ƭ�����Ǹÿ�Ƭ�����������������
 * ��д��Կ�������ܡ��������Կ��֤��������ô��������亯����ʵ�֡�
 *
 */
aw_err_t awbl_zlg600a_mifare_card_cmd_trans (int      id,
                                             uint8_t *p_tx_buf,
                                             uint8_t  tx_nbytes,
                                             uint8_t *p_rx_buf,
                                             uint8_t  rx_nbytes)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    /* ʵ�ʴ�����������ݴ� */
    tx_desc[1].p_buf = p_tx_buf;
    tx_desc[1].len   = tx_nbytes;

    /* ��һ�ֽ�Ϊʵ�����ݳ��ȣ�Ӧ���ǣ�ʵ����Ϣ���� + 1��*/
    tx_nbytes++;
    tx_desc[0].p_buf = &tx_nbytes;
    tx_desc[0].len   = 1;

    /* ʵ�ʽ��յ�����Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_nbytes;

    return __zlg600a_frame_proc(id,
                                2,
                                'S',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}


/**
 * \brief ���ݽ���
 *
 * �������ö�д���뿨Ƭ�����ݽ�����ͨ���ú�������ʵ�ֶ�д���������й��ܡ�
 *
 * \param[in] p_dev      : ָ��ZLG600A�豸��ָ��
 * \param[in] p_data_buf : ��Ҫ�������͵�����
 * \param[in] len        : �������ݵĳ���
 * \param[in] wtxm_crc   : b7~b2,wtxm��b1,RFU(0); b0,0,CRC��ֹ�ܣ�1,CRCʹ�ܡ�
 * \param[in] fwi        : ��ʱ�ȴ�ʱ�����, ��ʱʱ��=����0x01 << fwi*302us��
 * \param[in] p_rx_buf   : ���ݽ�����Ӧ֡��Ϣ
 * \param[in] buf_size   : ���ջ�Ӧ��Ϣ�Ļ�������С
 * \param[in] p_real_len : ���ڻ�ȡʵ�ʽ��յ���Ϣ���ȣ������������СС��ʵ����
 *                         Ϣ���ȣ�����ಿ�ֽ�����
 *
 * \retval AW_OK      : �ɹ�
 * \retval -AW_EINVAL : ʧ�ܣ���������
 * \retval -AW_EIO    : ʧ�ܣ�����ͨ�ų���
 */
aw_err_t awbl_zlg600a_mifare_card_data_exchange (int       id,
                                                 uint8_t  *p_data_buf,
                                                 uint8_t   len,
                                                 uint8_t   wtxm_crc,
                                                 uint8_t   fwi,
                                                 uint8_t  *p_rx_buf,
                                                 uint8_t   buf_size,
                                                 uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �������� */
    tx_desc[0].p_buf = p_data_buf;
    tx_desc[0].len   = len;

    tx_desc[1].p_buf = &wtxm_crc;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &fwi;
    tx_desc[2].len   = 1;

    /* ʵ�ʽ��յ�����Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = buf_size;

    return __zlg600a_frame_proc(id,
                                2,
                                'X',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief �Ӵ�ʽIC����λ���Զ�����PPS��
 *
 *     �ú������临λ��ִ�гɹ�����Զ�����IC���ĸ�λ��Ϣ��ִ��PPS���Ȼ����
 * ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1����
 *
 */
aw_err_t awbl_zlg600a_cicc_reset (int       id,
                                  uint8_t   baudrate_flag,
                                  uint8_t  *p_rx_buf,
                                  uint8_t   rx_bufsize,
                                  uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    /* ������Ϣ16�ֽڶ��� */
    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    /* ʵ����Ϣ */
    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'A',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief �Ӵ�ʽIC������Э��
 *
 *     ��������ݽӴ�ʽIC���ĸ�λ��Ϣ���Զ�ѡ��T = 0��T = 1����Э�飬��������
 * ����Ҫʹ���߸�Ԥ�����������ڴ���APDU��������
 *
 */
aw_err_t awbl_zlg600a_cicc_tpdu(int       id,
                                uint8_t  *p_tx_buf,
                                uint8_t   tx_bufsize,
                                uint8_t  *p_rx_buf,
                                uint8_t   rx_bufsize,
                                uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �������� */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* ������Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'B',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief �Ӵ�ʽIC���临λ
 *
 * �ú������临λ��ִ���˽Ӵ�ʽIC���ϵ�ʱ��ִ�гɹ�����Զ�����IC���ĸ�λ��Ϣ
 * ��ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1�����ú���û��
 * �Զ�ִ��PPS�����Ҫ�û����ݸ�λ��Ϣ���ж��Ƿ�ʹ��awbl_zlg600a_cicc_pps()��
 * �޸�Э��Ͳ�����
 *
 */
aw_err_t awbl_zlg600a_cicc_coldreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'C',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief �Ӵ�ʽIC���ȸ�λ
 *
 *    �ú������ȸ�λ��û��ִ���˽Ӵ�ʽIC���ϵ�ʱ��ִ�гɹ�����Զ�����IC����
 * ��λ��Ϣ��ѡ��awbl_zlg600a_cicc_tpdu()����ʹ�õĴ���Э�飨T = 0 ��T = 1����
 * �ȸ�λ���临λΨһ��������û��ִ��IC���ϵ��������Ҫ�û����ݸ�λ��Ϣ���ж�
 * �Ƿ�ʹ��awbl_zlg600a_cicc_pps()���޸�Э��Ͳ������ú���������IC��ʱ�Ӻ͵�Դ��
 * ��Ч������²���ִ�С�
 *
 */
aw_err_t awbl_zlg600a_cicc_warmreset (int       id,
                                      uint8_t   baudrate_flag,
                                      uint8_t  *p_rx_buf,
                                      uint8_t   rx_bufsize,
                                      uint16_t *p_real_len)
{
    aw_err_t ret;

    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[2];

    tx_desc[0].p_buf = &baudrate_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = NULL;
    rx_desc[0].len   = 16;

    rx_desc[1].p_buf = p_rx_buf;
    rx_desc[1].len   = rx_bufsize;

    ret = __zlg600a_frame_proc(id,
                               5,
                               'D',
                               tx_desc,
                               1,
                               rx_desc,
                               2,
                               p_real_len);

    if (ret == AW_OK) {
        *p_real_len -= 16;
    }

    return ret;
}

/**
 * \brief �Ӵ�ʽIC��ͣ��رսӴ�ʽIC���ĵ�Դ��ʱ��
 */
aw_err_t awbl_zlg600a_cicc_deactive (int id)
{
    return __zlg600a_frame_proc(id,
                                5,
                                'E',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief �Ӵ�ʽIC��Э��Ͳ���ѡ��
 *
 *     �ú������临λ���ȸ�λ֮���ұ�������ִ�У�Э��ģʽ����Ҫִ�У�ר��ģʽ
 * ����Ҫִ�У������ԽӴ�ʽIC�����˽⣬����ʹ��awbl_zlg600a_cicc_reset()���и�λ��
 *
 */
aw_err_t awbl_zlg600a_cicc_pps (int      id,
                                uint8_t *p_pps)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = p_pps;
    tx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                5,
                                'F',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief �Ӵ�ʽIC������Э�飨T = 0��
 *
 *     ����������T = 0����Э�顣���Ӵ�ʽIC���Ĵ���Э��ΪT = 0���ú�����ͬ��
 * awbl_zlg600a_cicc_tpdu()��
 *
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp0 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �������� */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* ������Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'G',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief �Ӵ�ʽIC������Э�飨T = 1��
 *
 *     ����������T = 1����Э�顣���Ӵ�ʽIC���Ĵ���Э��ΪT = 1���ú�����ͬ��
 * awbl_zlg600a_cicc_tpdu()��
 */
aw_err_t awbl_zlg600a_cicc_tpdu_tp1 (int       id,
                                     uint8_t  *p_tx_buf,
                                     uint8_t   tx_bufsize,
                                     uint8_t  *p_rx_buf,
                                     uint8_t   rx_bufsize,
                                     uint16_t  *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �������� */
    tx_desc[0].p_buf = p_tx_buf;
    tx_desc[0].len   = tx_bufsize;

    /* ������Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = rx_bufsize;

    return __zlg600a_frame_proc(id,
                                5,
                                'H',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICCA���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 * \note ���������ߺ󣬴���Ƶ���л�ȡ�������Ӷ��õ縴λ����λ�󿨴���IDLEģʽ��
 * ����������ģʽ����һ������ʱ����ʱ�Ŀ�������Ӧ������ĳһ�ſ��ɹ������˹���
 * ������Halt�����DeSelect������������Haltģʽ����ʱ�Ŀ�ֻ��ӦALL��0x52��
 * ģʽ�����󣬳��ǽ����뿪���߸�Ӧ�����ٽ��롣
 */
aw_err_t awbl_zlg600a_picca_request (int       id,
                                     uint8_t   req_mode,
                                     uint16_t *p_atq)
{
    return __zlg600a_iso14443_request(id, 6, req_mode, p_atq);
}

/**
 * \brief PICCA���ķ���ײ����
 *
 *  ��Ҫ�ɹ�ִ��һ�������������������ɹ������ܽ��з���ײ���������򷵻ش���
 *
 * \note  ����ISO14443A��׼�������кŶ���ȫ��Ψһ�ģ���������Ψһ�ԣ�����ʵ�ַ�
 * ��ײ���㷨�߼������������ſ�ͬʱ�����߸�Ӧ��������������ܹ��ҵ�һ�����кŽ�
 * ��Ŀ���������
 */
aw_err_t awbl_zlg600a_picca_anticoll (int       id,
                                      uint8_t   anticoll_level,
                                      uint32_t  know_uid,
                                      uint8_t   nbit_cnt,
                                      uint32_t *p_uid)
{
    return __zlg600a_iso14443_anticoll(id,
                                       0x06,
                                       anticoll_level,
                                       know_uid,
                                       nbit_cnt,
                                       p_uid);
}

/**
 * \brief PICC A����ѡ�����
 *
 *  ��Ҫ�ɹ�ִ��һ�η���ײ��������سɹ������ܽ��п�ѡ����������򷵻ش���
 *
 * \note  �������кų��������֣�4�ֽڡ�7�ֽں�10�ֽڡ� 4�ֽڵ�ֻҪ��һ��ѡ�񼴿�
 * �õ����������кţ���Mifare1 S50/S70�ȣ�7�ֽڵ�Ҫ�ö���ѡ����ܵõ�����������
 * �ţ�ǰһ�����õ������кŵ�����ֽ�Ϊ������־0x88�������к���ֻ�к�3�ֽڿ��ã�
 * ��һ��ѡ���ܵõ�4�ֽ����кţ����߰�˳�����Ӽ�Ϊ7�ֽ����кţ���UltraLight
 * ��DesFire�ȣ�10�ֽڵ��Դ����ƣ�������δ���ִ��࿨��
 */
aw_err_t awbl_zlg600a_picca_select (int       id,
                                    uint8_t   anticoll_level,
                                    uint32_t  uid,
                                    uint8_t  *p_sak)
{
    return __zlg600a_iso14443_select(id,
                                     0x06,
                                     anticoll_level,
                                     uid,
                                     p_sak);
}

/**
 * \brief PICCA���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *  ��HALT״̬�£���������Ӧ������������IDLEģʽ�����󣬳��ǽ�����λ���뿪���߸�
 *  Ӧ�����ٽ��롣��������Ӧ������������ALL����
 *
 */
aw_err_t awbl_zlg600a_picca_halt (int id)
{
    return __zlg600a_iso14443_halt(id, 0x06);
}

/**
 * \brief PICC TypeA��RATS��request for answer to select��
 *
 *     RATS��request for answer to select����ISO14443-4Э������ģ�鷢��RATS��
 * ��Ƭ����ATS��answer to select����ΪRATS��Ӧ����ִ�и�����ǰ�������Ƚ���һ��
 * ��ѡ���������ִ�й�һ��RATS��������ٴ�ִ��RATS��������Ƚ�����
 *
 */
aw_err_t awbl_zlg600a_picca_rats (int       id,
                                  uint8_t   cid,
                                  uint8_t  *p_ats_buf,
                                  uint8_t   bufsize,
                                  uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &cid;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_ats_buf;
    rx_desc[0].len   = bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'E',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICC TypeA��PPS��request for answer to select��
 *
 *     PPS��protocal and parameter selection����ISO14443-4Э���������ڸı���
 * �ص�ר��Э�������������Ǳ���ģ�����ֻ֧��Ĭ�ϲ�������������Ĳ�������Ϊ
 * 0���ɡ���ִ�и�����ǰ�������ȳɹ�ִ��һ��RATS���
 *
 */
aw_err_t awbl_zlg600a_picca_pps (int id, uint8_t dsi_dri)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &dsi_dri;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'F',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICC TypeA���������
 *
 *   ��������ISO14443-4Э���������ڽ���Ƭ��Ϊ����HALT��״̬�����ڹ���
 *��HALT��״̬�Ŀ������á���������������ΪALL�������¼����ֻ��ִ�С�RATS��
 * ����Ŀ����ø����
 *
 */
aw_err_t awbl_zlg600a_picca_deselect (int id)
{
    return __zlg600a_frame_proc(id,
                                6,
                                'G',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICC��T=CL
 *
 *     T=CL�ǰ�˫�����鴫��Э�飬ISO14443-4Э��������ڶ�д���뿨Ƭ֮�������
 * ������һ�����ISO14443Э���CPU�����ø�Э�����д��ͨ�š����ø�����ʱֻ��Ҫ
 * ��CPU��COS�����������Ϊ���뼴�ɣ���������������͡�����ʶ��CID��֡�ȴ�ʱ��
 * FWT���ȴ�ʱ����չ��������WTXM��waiting time extensionmultiplier�����ȵ��ɸ�
 * �����Զ���ɡ�
 *
 */
aw_err_t awbl_zlg600a_picc_tpcl (int       id,
                                 uint8_t  *p_cos_buf,
                                 uint8_t   cos_bufsize,
                                 uint8_t  *p_res_buf,
                                 uint8_t   res_bufsize,
                                 uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = p_cos_buf;
    tx_desc[0].len   = cos_bufsize;

    rx_desc[0].p_buf = p_res_buf;
    rx_desc[0].len   = res_bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'H',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief ���ݽ���
 *
 * �������ö�д���뿨Ƭ�����ݽ�����ͨ���ú�������ʵ�ֶ�д���������й��ܡ�
 *
 */
aw_err_t awbl_zlg600a_picc_data_exchange (int       id,
                                          uint8_t  *p_data_buf,
                                          uint8_t   len,
                                          uint8_t   wtxm_crc,
                                          uint8_t   fwi,
                                          uint8_t  *p_rx_buf,
                                          uint8_t   bufsize,
                                          uint16_t *p_real_len)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    /* �������� */
    tx_desc[0].p_buf = p_data_buf;
    tx_desc[0].len   = len;

    tx_desc[1].p_buf = &wtxm_crc;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &fwi;
    tx_desc[2].len   = 1;

    /* ʵ�ʽ��յ�����Ϣ */
    rx_desc[0].p_buf = p_rx_buf;
    rx_desc[0].len   = bufsize;

    return __zlg600a_frame_proc(id,
                                6,
                                'J',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                p_real_len);
}

/**
 * \brief PICC A����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 */
aw_err_t awbl_zlg600a_picca_reset (int id, uint8_t time_ms)
{
    return __zlg600a_iso14443_reset(id, 0x06, time_ms);
}

/**
 * \brief PICC A������ú������ڼ��Ƭ�������󡢷���ײ��ѡ�������������ϡ�
 */
aw_err_t awbl_zlg600a_picca_active (int       id,
                                    uint8_t   req_mode,
                                    uint16_t *p_atq,
                                    uint8_t  *p_saq,
                                    uint8_t  *p_len,
                                    uint8_t  *p_uid,
                                    uint8_t   uid_len)
{
    return __zlg600a_iso14443_active(id,
                                     0x06,
                                     req_mode,
                                     p_atq,
                                     p_saq,
                                     p_len,
                                     p_uid,
                                     uid_len);
}

/**
 * \brief PICC B������
 *
 *    �ڵ��øú���ǰ����Ҫȷ��IC����ģʽ�Ѿ�����ΪTypeBģʽ��
 *
 */
aw_err_t awbl_zlg600a_piccb_active (int id, uint8_t req_mode, uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    uint8_t  data = 0x00;  /* Ӧ�ñ�ʶ��1�ֽڣ���Ĭ��Ϊ0x00 */

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &data;
    tx_desc[1].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'N',
                                tx_desc,
                                2,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PICC B����λ��ͨ�����ز��źŹر�ָ����ʱ�䣬�ٿ�����ʵ�ֿ�Ƭ��λ��
 *
 * \note �ú����������źŹر������룬��һֱ�رգ���ȵ�ִ��һ����������ʱ�򿪡�
 */
aw_err_t awbl_zlg600a_piccb_reset (int id, uint8_t time_ms)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = &time_ms;
    tx_desc[0].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'O',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PICCA���������
 *
 *  �ú�����Ϊ�������������ֻҪ����ISO14443A��׼�Ŀ���Ӧ�ܷ�����Ӧ��
 *
 */
aw_err_t awbl_zlg600a_piccb_request (int      id,
                                     uint8_t  req_mode,
                                     uint8_t  slot_time,
                                     uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    uint8_t data = 0x00;  /* Ӧ�ñ�ʶ��1�ֽڣ���Ĭ��Ϊ0x00 */

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = &data;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &slot_time;
    tx_desc[2].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'P',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PICC B ���ķ���ײ����
 */
aw_err_t awbl_zlg600a_piccb_anticoll (int      id,
                                      uint8_t  slot_flag,
                                      uint8_t *p_uid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &slot_flag;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_uid;
    rx_desc[0].len   = 12;

    return __zlg600a_frame_proc(id,
                                6,
                                'Q',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}


/**
 * \brief PICC B���Ĺ��������ʹ��ѡ��Ŀ�����HALT״̬
 *
 *     �ú�������B�Ϳ�������ִ�й�������ǰ��������ִ�гɹ���һ���������ִ��
 * ��������ɹ��󣬿�Ƭ���ڹ���״̬��ģ�����ͨ��ALL��ʽ����Ƭ����������IDLE��
 * ʽ����
 *
 */
aw_err_t awbl_zlg600a_piccb_halt (int id, uint8_t *p_pupi)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];

    tx_desc[0].p_buf = p_pupi;
    tx_desc[0].len   = 4;

    return __zlg600a_frame_proc(id,
                                6,
                                'S',
                                tx_desc,
                                1,
                                NULL,
                                0,
                                NULL);
}


/**
 * \brief �޸�B���Ĵ�������
 *
 *     �ú�������B�Ϳ�������ִ�й�������ǰ��������ִ�гɹ���һ���������ִ��
 * ��������ɹ��󣬿�Ƭ���ڹ���״̬��ģ�����ͨ��ALL��ʽ����Ƭ����������IDLE��
 * ʽ����
 *
 */
aw_err_t awbl_zlg600a_piccb_attrib_set (int      id,
                                        uint8_t *p_pupi,
                                        uint8_t  cid,
                                        uint8_t  protype)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];

    tx_desc[0].p_buf = p_pupi;
    tx_desc[0].len   = 4;

    tx_desc[1].p_buf = &cid;
    tx_desc[1].len   = 1;

    tx_desc[2].p_buf = &protype;
    tx_desc[2].len   = 1;

    return __zlg600a_frame_proc(id,
                                6,
                                'R',
                                tx_desc,
                                3,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief ��ȡ8�ֽ����֤ID
 *
 *   �ú������ڻ�ȡ�������֤ID����ִ������ǰ�������ȵ���
 * awbl_zlg600a_ic_isotype_set()����Э��ΪISO14443B��
 */
aw_err_t awbl_zlg600a_sid_get (int id, uint8_t req_mode, uint8_t *p_sid)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[1];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    tx_desc[0].p_buf = &req_mode;
    tx_desc[0].len   = 1;

    rx_desc[0].p_buf = p_sid;
    rx_desc[0].len   = 8;

    return __zlg600a_frame_proc(id,
                                6,
                                'T',
                                tx_desc,
                                1,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PLUS CPU�����˻���������
 *
 *     ����������SL0��Security Level 0����ȫ�ȼ�0����PLUS CPU�����˻���PLUS CPU
 * ������ʱ�İ�ȫ�ȼ�ΪSL0���õȼ��£�����Ҫ�κ���֤�Ϳ�������д���ݣ�д�����
 * ������Ϊ������ȫ�ȼ��ĳ�ʼֵ�����磺��SL0��0x0003��д�룺0xA0 0xA1 0xA2 0xA3
 * 0xA4 0xA5 0xFF 0x07 0x80 0x69 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF������Ƭ������SL1
 * ������0��A��ԿΪ0xA0 0xA1 0xA2 0xA3 0xA4 0xA5��������Ĭ�ϵ�0xFF 0xFF 0xFF
 * 0xFF 0xFF 0xFF����������SL0�޸Ŀ�Ƭ��Ĭ�����ݺ���Կ��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_write (int       id,
                                              uint16_t  sector_addr,
                                              uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&sector_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_buf;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'B',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU���ύ���˻�
 *
 *  awbl_zlg600a_pluscpu_persotcl_write()����ֻ�Ǹ��¿��е����ݣ������ݻ�δ��Ч��
 * �ύ�����ݲ���Ч��ִ�и������PLUS CPU���İ�ȫ�ȼ���ߵ�SL1����SL3
 * ������֧��SL1�Ŀ�����ִ�и������Ƭ��ȫ�ȼ���ߵ�SL1������ֻ֧��SL0��SL3��
 * ������ִ�и������Ƭ��ȫ�ȼ���ߵ�SL3����
 * ע�⣺��SL0�У�ֻ���޸������µ�ַ����ִ���ύ������
 *   - 0x9000��������Կ��
 *   - 0x9001�����ÿ���Կ��
 *   - 0x9002��SL2������Կ��ֻ��֧��SL2�Ŀ����и���Կ��
 *   - 0x9003��SL3������Կ��ֻ��֧��SL3�Ŀ����и���Կ��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_persotcl_commit (int id)
{
    return __zlg600a_frame_proc(id,
                                7,
                                'C',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}


/**
 * \brief PLUS CPU���״���֤��ֱ����Կ��֤��
 *
 * \note PLUS CPU������ԿA/B��ͨ����ַ����ż�������֣�AES����Կ��ַ�����ݿ��
 * ������ϵ��Ӧ����:
 * ��ԿA��ַ=0x4000 + ���� �� 2
 * ��ԿB��ַ=0x4000 + ���� �� 2 + 1
 * ��ˣ�����֤1��������ԿA������Կ��ַΪ��0x4002
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_direct_authent (int       id,
                                                        uint16_t  authent_addr,
                                                        uint8_t  *p_key)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_key == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'J',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU���״���֤��E2��Կ��֤������֤����Կ����ģ���ڲ������粻��ʧ
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_first_e2_authent (int      id,
                                                    uint16_t authent_addr,
                                                    uint8_t  key_sector)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = &key_sector;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                7,
                                'K',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU��������֤��ֱ����Կ��֤��
 *
 *     �ú�������SL3 PLUS CPU���ĸ�����Կ��֤����֤����Կ�������������ֻ��ִ��
 * �����״���֤������ɹ������ʹ�ø����������֤����������ʹ�õ�ʱ�䲻ͬ��
 * ���״���֤������Ҫ��ʱ��ȡ�������֤����ʱ��Ҫ��Щ��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_direct_authent (int       id,
                                                         uint16_t  authent_addr,
                                                         uint8_t  *p_key)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    if ((p_key == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = p_key;
    tx_desc[1].len   = 16;

    return __zlg600a_frame_proc(id,
                                    7,
                                    'L',
                                    tx_desc,
                                    2,
                                    NULL,
                                    0,
                                    NULL);
}

/**
 * \brief PLUS CPU��������֤��E2��Կ��֤��,��֤����Կ����ģ���ڲ������粻��ʧ
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_follow_e2_authent (int      id,
                                                     uint16_t authent_addr,
                                                     uint8_t  key_sector)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[2];

    tx_desc[0].p_buf = (uint8_t *)&authent_addr;
    tx_desc[0].len   = 2;

    tx_desc[1].p_buf = &key_sector;
    tx_desc[1].len   = 1;

    return __zlg600a_frame_proc(id,
                                7,
                                'M',
                                tx_desc,
                                2,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU��SL3��λ��֤��E2��Կ��֤��,��֤����Կ����ģ���ڲ������粻��ʧ
 *
 *  ����������PLUS CPU��ͨ���״���֤���ʹ�ù����У���λ��д����������֤����Ϣ��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_reset_authent (int id)
{
    return __zlg600a_frame_proc(id,
                                7,
                                'N',
                                NULL,
                                0,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU��SL3�����ݿ�
 *
 *  �ú������ڶ�ȡSL3�����ݿ飬�ڶ����ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_read (int       id,
                                        uint8_t   read_mode,
                                        uint16_t  start_block,
                                        uint8_t   block_num,
                                        uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[3];
    awbl_zlg600a_info_buf_desc_t rx_desc[1];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &read_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&start_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = &block_num;
    tx_desc[2].len   = 1;

    rx_desc[0].p_buf = p_buf;
    rx_desc[0].len   = block_num * 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'O',
                                tx_desc,
                                3,
                                rx_desc,
                                1,
                                NULL);
}

/**
 * \brief PLUS CPU��SL3д���ݿ�
 *
 *  �ú�������дSL3�����ݿ飬��д���ݿ�֮ǰ����ɹ�ִ��һ����Կ��֤��
 *
 */
aw_err_t awbl_zlg600a_pluscpu_sl3_write (int       id,
                                         uint8_t   write_mode,
                                         uint16_t  start_block,
                                         uint8_t   block_num,
                                         uint8_t  *p_buf)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    if ((p_buf == NULL)) {
        return -AW_EINVAL;
    }

    tx_desc[0].p_buf = &write_mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&start_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = &block_num;
    tx_desc[2].len   = 1;

    tx_desc[3].p_buf = p_buf;
    tx_desc[3].len   = block_num * 16;

    return __zlg600a_frame_proc(id,
                                7,
                                'P',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}

/**
 * \brief PLUS CPU��ֵ�����
 *
 * \note Ҫ���д�������������ݱ���Ҫ��ֵ��ĸ�ʽ���ɲο�NXP������ĵ���������
 * ���봫������ͬ���򽫲�����Ľ��д��ԭ���Ŀ��ڣ���������봫���Ų���ͬ��
 * �򽫲�����Ľ��д�봫����ڣ����������ڵ����ݱ����ǣ�ԭ���ڵ�ֵ���䡣
 * ���ڵȼ�2��PLUS CPU����֧��ֵ��������ȼ�1��3֧�֡�
 */
aw_err_t awbl_zlg600a_pluscpu_val_operate (int      id,
                                           uint8_t  mode,
                                           uint16_t src_block,
                                           uint16_t dst_block,
                                           int32_t  value)
{
    awbl_zlg600a_info_buf_desc_t tx_desc[4];

    tx_desc[0].p_buf = &mode;
    tx_desc[0].len   = 1;

    tx_desc[1].p_buf = (uint8_t *)&src_block;
    tx_desc[1].len   = 2;

    tx_desc[2].p_buf = (uint8_t *)&dst_block;
    tx_desc[2].len   = 2;

    tx_desc[3].p_buf = (uint8_t *)&value;
    tx_desc[3].len   = 4;

    return __zlg600a_frame_proc(id,
                                7,
                                'S',
                                tx_desc,
                                4,
                                NULL,
                                0,
                                NULL);
}


/* end of file */
