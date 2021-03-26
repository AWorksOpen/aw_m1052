/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief modbus�ӻ�Э��ջ�û��ӿ�ͷ�ļ�
 *
 * \internal
 * \par modification history
 * - 1.05 2019-03-26  cml, use serial interface replace int serial.
 * - 1.04 2016-09-02  cod, support muti-tcp.
 * - 1.03 2016-04-27  cod, support muti-instance.
 * - 1.02 2015-05-22  cod, fix code style and comments, a little improve.
 * - 1.01 2013-03-29  liuweiyun, fix code style and comments, a little improve.
 * - 1.00 2012-05-03  ygy, first implementation
 * \endinternal
 */

#ifndef __AW_MB_SLAVE_H /* { */
#define __AW_MB_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_mb_slave
 * \copydoc aw_mb_slave.h
 * @{
 */

#include "modbus/aw_mb_config.h"
#include "modbus/aw_mb_err.h"
#include "modbus/aw_mb_comm.h"

/** \brief ������ص��������� */
enum aw_mb_func_cb_type {
    AW_MB_FUNC_COILS_CALLBACK,                /**< \brief ��д��Ȧ�����ص� */
    AW_MB_FUNC_DISCRETEINPUTS_CALLBACK,       /**< \brief ����ɢ�������ص� */
    AW_MB_FUNC_HOLDREGISTERS_CALLBACK,        /**< \brief ��д����Ĵ����ص� */
    AW_MB_FUNC_INPUTREGISTERS_CALLBACK,       /**< \brief ������Ĵ����ص� */
};

/** \brief ������ص������������� */
enum aw_mb_func_cb_op {
    AW_MB_FUNC_CALLBACK_READ,   /**< \brief ������ */
    AW_MB_FUNC_CALLBACK_WRITE,  /**< \brief д���� */
};

/** \brief Modbus��վ������� */
typedef void *aw_mb_slave_t;

/******************************************************************************/

/**
 * \file
 *
 * callback function
 *
 * Read the ON/OFF status of discrete outputs(DO)in the slave. The coils status
 * is packed as per bit of data field. Status is indicated as: 1=ON, 0=OFF.
 * The LSB of the first data byte contains the coil addressed in the query.
 * If the coil quantity is not a multiple of eight, the remaining bits in the
 * final data byte will be padded with zeros.
 *
 * For example. addr = 0x0013  num = 10,
 * status of coils 20 ~ 27 is ON-ON-OFF-OFF-ON-OFF-ON-OFF 28~29 ON-ON,
 * so the buf[0] = 01010011(0x53)  buf[1] = 00000011
 *
 * buf[0]:
 * 7  |6  |5  |4  |3  |2  |1  |0
 * ---|---|---|---|---|---|---|---
 * 27 |26 |25 |24 |23 |22 |21 |20
 * OFF|ON |OFF|ON |OFF|OFF|ON |ON
 * 0  |1  |0  |1  |0  |0  |1  |1
 *
 * buf[1]:
 * 7  |6  |5  |4  |3  |2  |1  |0
 * ---|---|---|---|---|---|---|----
 * -  |-  |-  |-  |-  |-  |29 |28
 * 0  |0  |0  |0  |0  |0  |ON |ON
 * 0  |0  |0  |0  |0  |0  |1  |1
 */

/**
 * \brief ��д��Ȧ������ɢ������д����Ĵ�����������Ĵ����ص�����ԭ��
 * \note  ��ʹ�ñ����ṩ�Ķ�д��Ȧ������ɢ������д����Ĵ�����
 *        ������Ĵ����ȹ����봦����ʱ����Ҫע��
 *
 * \par ʾ����
 * \code
 * aw_mb_exception_t my_coils_callback(aw_mb_slave_t slave,
 *                                     uint8_t   *buf,
 *                                     uint16_t   addr,
 *                                     uint16_t   num)
 * {
 *     //����Ȧ����
 * }
 *
 * //������slave֮ǰ�������²�����
 * aw_mb_slave_register_callback(slave,
 *                               AW_MB_FUNC_COILS_CALLBACK,
 *                               AW_MB_FUNC_CALLBACK_READ,
 *                               my_coils_callback);
 * \endcode
 *
 * \param[in]     slave  �ӻ����
 * \param[in,out] buf    ��д����������
 * \param[in]     addr   ��д������ʼ��ַ
 * \param[in]     num    ��д��������
 *
 * \return  ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 * \see aw_mb_slave_register_callback
 */
typedef aw_mb_exception_t (*aw_mb_slave_fn_code_callback_t)(
                            aw_mb_slave_t  slave,
                            uint8_t       *p_buf,
                            uint16_t       addr,
                            uint16_t       num);

/**
 * \brief ע���д��Ȧ������ɢ������д����Ĵ�����������Ĵ����ص�����.
 * \note  ��ʹ�ñ����ṩ�Ķ�д��Ȧ������ɢ������д����Ĵ�����������Ĵ���������
 *        ������ʱ, ����ʹ�øú������лص�����ע�ᣬ�����������÷Ƿ��������쳣
 *
 * \param[in] slave     �ӻ����
 * \param[in] type      ������ص���������
 * \param[in] op        ������ص�������������
 * \param[in] callback  �ص�����
 *
 * \retval AW_MB_ERR_NOERR   ע��ɹ�.
 * \retval AW_MB_ERR_EINVAL  �����������.
 */
aw_mb_err_t aw_mb_slave_register_callback (
            aw_mb_slave_t                  slave,
            enum aw_mb_func_cb_type        type,
            enum aw_mb_func_cb_op          op,
            aw_mb_slave_fn_code_callback_t callback);

/**
 * \brief �����봦����ԭ��.
 * \note  ���ӻ��յ�һ֡�����Ժ󣬽����ݹ�������ö�Ӧ�Ĵ�������
 *        ���������û�ͨ��\a aw_mb_slave_register_handler ��ע�ᡣ
 *        ͬʱ�Ѿ���������Ķ�д��Ȧ������ɢ������д����Ĵ�����������Ĵ���
 *        �ȴ�����������ο�\a aw_mb_regfunction.h
 *
 * \param[in]     slave     Modbus��վ���
 * \param[in,out] p_pdubuf  PDU����(����������)
 *                          - [in]  ����PDU����
 *                          - [out] Ӧ��PDU����
 * \param[in,out] p_pdulen  PDU���ݳ���
 *                          - [in]  ����PDU���ݳ���
 *                          - [out] Ӧ��PDU���ݳ���
 *
 * \return ���û���쳣������AW_MB_EXP_NONE�����򷵻ض�Ӧ���쳣��
 */
typedef aw_mb_exception_t (*aw_mb_fn_code_handler_t) (aw_mb_slave_t  slave,
                                                      uint8_t       *p_pdubuf,
                                                      uint16_t      *p_pdulen);

/******************************************************************************/
/**
 * \brief ��������ʼ��һ��Modbus��վ
 * \note  �ڵ������дӻ�����֮ǰ�������ȵ��øú���
 * \note  ����ͬ���紴�������վ
 *
 * \param[in] mode     ͨ��ģʽ
 * \param[in] p_param  ��ʼ������������MB_RTU/MB_ASCIIģʽʹ��
 *                     \a struct aw_mb_serial_param ��Ϊ����
 *                     ����AW_MB_TCPģʽ��param��Ϊ�˿ںŲ���ʹ��,ֱ�Ӵ����β�
 * \param[out] p_err   �����. ����Դ���Ų�����Ȥ���Դ���AW_MB_NULL������
 *                     ���ش���ֵ��
 *                     - AW_MB_ERR_NOERR           : ��ʼ���ɹ�.
 *                     - AW_MB_ERR_EINVAL          : �����������.
 *                     - AW_MB_ERR_ALLOC_FAIL      : �ڴ治�㣬�����վʧ��.
 *                     - AW_MB_ERR_MODE_NO_SUPPORT : ģʽ��֧��
 *                     - AW_MB_ERR_EPORTERR        : ����ģʽ-���䴮�ڻ�Ӳ����ʱ��ʧ��.
 *                     - AW_MB_ERR_EIO             : TCPģʽ-����˴���ʧ��.
 * \return Modbus��վ���������������󷵻�NULL
 *
 * \note 1.ʹ��RTUģʽʱĬ�ϵĴ�������λΪ8��ֹͣλΪ1��
 *       2.ʹ��ASCIIģʽʱĬ�ϵĴ�������λΪ7��ֹͣλΪ1��
 *         ������Ϊ����ASCII�����ֵΪ128��7λ���ӡ�
 */
aw_mb_slave_t aw_mb_slave_init (enum aw_mb_mode  mode,
                                void            *p_param,
                                aw_mb_err_t     *p_err);

/**
 * \brief �ر�Modbus��վ.
 * \note  �˳�ϵͳ���߲���Ҫ��ʹ�ôӻ�ʱ����
 *
 * \param[in] slave  Modbus��վ���
 *
 * \retval AW_MB_ERR_NOERR      �޴���.
 * \retval AW_MB_ERR_EINVAL     ��վ��������.
 * \retval AW_MB_ERR_EILLSTATE  Э��ջ�Ƿ�״̬.
 */
aw_mb_err_t aw_mb_slave_close (aw_mb_slave_t slave);

/**
 * \brief ����Modbus��վ.
 * \note  �ú��������ڵ���\a aw_mb_slave_init ��������ߵ���\a aw_mb_slave_stop �����󱻵���
 *
 * \param[in] slave  Modbus��վ���
 *
 * \retval AW_MB_ERR_NOERR      �޴���.
 * \retval AW_MB_ERR_EINVAL     ��վ��������.
 * \retval AW_MB_ERR_EILLSTATE  Э��ջ�Ƿ�״̬.
 */
aw_mb_err_t aw_mb_slave_start (aw_mb_slave_t slave);

/**
 * \brief ֹͣModbus��վ.
 *
 * \param[in] slave  Modbus��վ���
 *
 * \retval AW_MB_ERR_NOERR      �޴���.
 * \retval AW_MB_ERR_EINVAL     ��վ��������.
 * \retval AW_MB_ERR_EILLSTATE  Э��ջ�Ƿ�״̬.
 */
aw_mb_err_t aw_mb_slave_stop (aw_mb_slave_t slave);

/**
 * \brief ע�Ṧ���봦����.
 * \note  ��ʼ��Modbus��վ�Ժ󣬿��Ե��øú�����ע���Ӧ�Ĺ����봦������������չ������
 *
 * \note ����ע��Ĺ����봦����������\a aw_mb_config.h �� AW_MB_FUNC_CODE_HANDLER_MAX ���塣
 *       �����ø������޷���ע�ᡣͬһ��������ֻ����һ�������������ע������ͬ
 *       һ�������봦��������ֻʹ�����ע��Ĵ�������
 *
 * \param[in] slave    Modbus��վ���
 * \param[in] funcode  �������
 * \param[in] handler  �����봦����
 *
 * \retval AW_MB_ERR_NOERR   �޴���;
 * \retval AW_MB_ERR_EINVAL  ��������;
 * \retval AW_MB_ERR_ENORES  �޷���ע�Ṧ����.
 */
aw_mb_err_t aw_mb_slave_register_handler (aw_mb_slave_t            slave,
                                          uint8_t                  funcode,
                                          aw_mb_fn_code_handler_t  handler);

/**
 * \brief Modbus��վ��ѯ����.
 * \note  ��������֡�����������֡����ΪModbus��վЭ��ĺ��ġ��ú������뱻�����Եĵ��á�
 *        ��ʱ������������Ӧ�ú���վҪ���ʱ������
 * \note  �ýӿ�����RTU��ASCII��TCPģʽ.
 * \note  RTU/ASCII����ģʽ�£��ýӿ�����ʱ��ȡ����\a aw_mb_config.h Э��ջ���ýӿڡ�
 *        TCPģʽ�£��ýӿ�Ϊ��������
 *
 * \param[in] slave  Modbus��վ���
 *
 * \retval AW_MB_ERR_NOERR      �޴���;
 * \retval AW_MB_ERR_EINVAL     ��������;
 * \retval AW_MB_ERR_EILLSTATE  Э��ջ�Ƿ�״̬.
 * \retval AW_MB_ERR_ENORES     ����ģʽ����Ӧʱ�յ��µ�����.
 * \retval AW_MB_ERR_EIO        IO����. ����ģʽ������״̬����. TCPģʽ������ʧ��.
 */
aw_mb_err_t aw_mb_slave_poll (aw_mb_slave_t slave);

/** \brief ��վ�������� */
typedef int aw_mbs_cfg_type_t;
#define AW_MBS_TYPE_USER_FUNCODE       0x01   /**< \brief �û��Զ��幦����������  */
#define AW_MBS_TYPE_TCP_NUM            0x02   /**< \brief TCPģʽ���������������� */
#define AW_MBS_TYPE_ACCEPT_TCP_NUM_GET 0x03   /**< \brief ��ȡ��ǰ�ѽ��յ������� */
#define AW_MBS_TYPE_ALLOW_NEW_TCP      0x04   /**< \brief ���Ӵﵽ����ֵ������رվ����������������� */

/**
 * \brief ��վ��������
 *
 * \note  ���޵��øýӿڣ����������Ͱ���\a aw_mb_config.h �в�������.
 *
 * \param[in] slave  Modbus��վ���
 * \param[in] type   �������ͣ�
 *                   - AW_MBS_TYPE_USER_FUNCODE : �û��Զ��幦������������p_argΪ����������uint8_t��;
 *                   - AW_MBS_TYPE_TCP_NUM      : ������������������p_argΪ��������uint32_t��;
 *                   - AW_MBS_TYPE_ACCEPT_TCP_NUM_GET : ��ȡ��ǰ�ѽ���������������p_argΪ��ȡ����������uint32_t *��;
 *                   - AW_MBS_TYPE_ALLOW_NEW_TCP : �ﵽ�������޺�������������Ӷ��Ͽ�������,p_argΪ1ʱ����;
 * \param[in] p_arg  ���ò�����ȡ������������
 *
 * \return �����óɹ�������AW_MB_ERR_NOERR����֮��������Ӧ����ֵ��
 */
aw_mb_err_t aw_mb_slave_ioctl (aw_mb_slave_t     slave,
                               aw_mbs_cfg_type_t type,
                               void             *p_arg);

/**
 * \brief ��վģʽ��ȡ
 *
 * \param[in]      slave  Modbus��վ���
 * \param[in,out] p_mode  ��վ����ģʽ
 *
 * \retval AW_MB_ERR_EINVAL  �����������
 * \retval AW_MB_ERR_NOERR   �޴���
 */
aw_mb_err_t aw_mb_slave_mode_get (aw_mb_slave_t slave, enum aw_mb_mode *p_mode);

/**
 * \brief ����Modbus��վ��Ϣ��
 *        ��������Modbus��վ����˵������ǰ״̬������һЩ������Ϣ��
 *
 * \note ��ʵ�ֶ�����Modbus��վ��Ϣ��󳤶�Ϊ32���ֽڣ����У�
 *       - ջ����˵��ռһ���ֽ�
 *       - ��ǰ״̬ռһ���ֽ�
 *       - ������Ϣ���� <= 30����Ϣ \n
 *       ��ʹ����Ҫ�ر�ע�⡣
 * \note �ýӿ�֧�������ã�Ĭ�ϲ�֧��
 *
 * \param[in] slave         Modbus��վ���
 * \param[in] slave_id      ����˵��
 * \param[in] is_running    ��ǰ״̬
 * \param[in] p_additional  ����һЩ������Ϣ
 * \param[in] alen          ����һЩ������Ϣ����
 *
 * \retval AW_MB_ERR_NOERR   �޴���;
 * \retval AW_MB_ERR_EINVAL  ��������;
 * \retval AW_MB_ERR_ENORES  ��Դ����.
 */
aw_mb_err_t aw_mb_slave_set_id (aw_mb_slave_t  slave,
                                uint8_t        slave_id,
                                uint8_t        is_running,
                                const uint8_t *p_additional,
                                uint16_t       alen);

/**
 * \brief ����Modbus��վ��ַ.
 * \note  Ĭ��������ڵ���\a aw_mb_slave_init ���г�ʼ��ʱ����ʼ�����������Ѿ������ô�վ��ַ��
 *        ���������Ҫ������ʱ�޸Ĵ�վ��ַ������ʹ�øú�����
 *
 * \param[in] slave  Modbus��վ���
 * \param[in] addr   �µ�Modbus��վ��ַ
 *
 * \retval AW_MB_ERR_NOERR   �޴���;
 * \retval AW_MB_ERR_EINVAL  ��������; ��վ�����ڣ���ַ��������ַ�� �����õ�ַΪ�㲥��ַ
 */
aw_mb_err_t aw_mb_slave_set_addr (aw_mb_slave_t slave, uint8_t addr);

struct sockaddr; /**< \brief �׽��ֵ�ַ */

/** \brief ��������TCP��Ϣ */
struct aw_mb_slave_tcp_info {
    int              socket;      /**< \brief ����socket����������socket����  */
    struct sockaddr *p_sockaddr;  /**< \brief IP��ַ��Ϣ  */
};

/**
 * \brief ��վTCPģʽ���������ӹ��Ӻ���ԭ��
 * \note  ����˽������ӵ��øù��Ӻ���.
 *
 * \param[in] p_arg   �û��Զ������
 * \param[in] p_info  ���յ�������Ϣ
 *
 * \retval AW_TRUE   ������ո�����
 * \retval AW_FALSE  �ܾ����ո�����
 */
typedef aw_bool_t (*aw_mb_tcp_accept_hook_t) (void                        *p_arg,
                                              struct aw_mb_slave_tcp_info *p_info);

/**
 * \brief ��վTCPģʽ�����ӶϿ����Ӻ���ԭ��
 * \note  �ѽ��յ������쳣�Ͽ���
 *        �����Ӵﵽ����ֵ��������Ϊ�ﵽ����ֵ���������������ʱ�������Ͽ�ʱ���øù��Ӻ���
 *
 * \param[in] p_arg   �û��Զ������
 * \param[in] socket  �Ͽ����ӵ�socket
 *
 * \return ��
 */
typedef void (*aw_mb_tcp_close_hook_t) (void *p_arg, int socket);

/**
 * \brief TCPģʽsocket���ӹ��Ӻ���ע��
 *
 * \param[in] slave        Modbus��վ���
 * \param[in] accept_hook  ���������ӹ��Ӻ���
 * \param[in] close_hook   ���ӱ����Ͽ�����ﵽ�������޶Ͽ������Ӻ���
 * \param[in] p_arg        ���Ӻ�������
 *
 * \retval AW_MB_ERR_NOERR      ע��ɹ���
 * \retval AW_MB_ERR_EINVAL     ��������
 * \retval AW_MB_ERR_EILLSTATE  Э��ջ�Ƿ�״̬
 */
aw_mb_err_t aw_mb_slave_tcp_hook_register (aw_mb_slave_t            slave,
                                           aw_mb_tcp_accept_hook_t  accept_hook,
                                           aw_mb_tcp_close_hook_t   close_hook,
                                           void                    *p_arg);

#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_mb_slave */

#endif /* } __AW_MB_SLAVE_H */

/* end of file */
