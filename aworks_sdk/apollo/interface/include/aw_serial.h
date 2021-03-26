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
 * \brief ����ͨѶ��׼�ӿ�
 *
 * ��ģ���ṩ���ڶ�д�����Ƶȷ���
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_serial.h
 *
 * \par ��ʾ��
 * \code
 * aw_serial_write(COM0, "Hello!", 7);  // COM0���"Hello!"
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 12-10-29  orz, first implementation.
 * - 1.00 18-10-24  hsg, add serial dcb.
 * \endinternal
 */

#ifndef __AW_SERIAL_H
#define __AW_SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_serial
 * \copydoc aw_serial.h
 * @{
 */

#include "aw_common.h"
#include "aw_sio_common.h"

/**
 * \name ��������
 * \brief Ŀǰ�ṩ���10�����ڣ���0��ʼ���
 * @{
 */
#define COM0    0
#define COM1    1
#define COM2    2
#define COM3    3
#define COM4    4
#define COM5    5
#define COM6    6
#define COM7    7
#define COM8    8
#define COM9    9
/** @} */


/**
 * \name ����У�鷽��ȡֵ
 *  @{
 */
#define AW_SERIAL_EVENPARITY     (0)    /** \breif żЧ��  */
#define AW_SERIAL_ODDPARITY      (1)    /** \breif ��Ч��  */


/**
 * \name ����ʹ�õ�ֹͣλ��ȡֵ
 *  @{
 */
#define AW_SERIAL_ONESTOPBIT     (0)    /** \brief 1λֹͣλ  */
#define AW_SERIAL_ONE5STOPTS     (1)    /** \brief 1.5 λֹͣλ  */
#define AW_SERIAL_TWOSTOPBITS    (2)    /** \brief 2   λֹͣλ  */
#define AW_SERIAL_STOP_INVALID   (0x3)  /** \brief 3����Чֹͣλ  */


/**
 * \name �����豸RTS(request-to-send)����ȡֵ
 *  @{
 */
#define AW_SERIAL_RTS_HANDSHAKE  (0)    /** \brief Ӳ������  */
#define AW_SERIAL_RTS_DISABLE    (1)    /** \brief �������RTS��Ч  */
#define AW_SERIAL_RTS_ENABLE     (2)    /** \brief �������RTS��Ч  */
#define AW_SERIAL_RTS_INVALID    (0x3)  /** \brief ��ЧRTS  */


/**
 * \name �����豸DTR(data-terminal-ready)����ȡֵ
 *  @{
 */
#define AW_SERIAL_DTR_HANDSHAKE  (0)
#define AW_SERIAL_DTR_DISABLE    (1)
#define AW_SERIAL_DTR_ENABLE     (2)
#define AW_SERIAL_DTR_INVALID    (0x3)


#pragma pack(push)
#pragma pack(1)

/** @} */
/**
 * \brief  �������ýṹ����
 */
struct aw_serial_dcb {
	
    /**
     * \brief ָ����ǰ�˿�ʹ�õ�����λ
     *
     * ��Χ��5 ~ 8
     */
    uint32_t                byte_size:4;

    /**
     * \brief �Ƿ�������żУ��,Ϊ0������У�飬Ϊ1��У�鷽ʽ��parity��ֵ
     */
    uint32_t                f_parity:1;

    /**
     * \brief ָ���˿����ݴ����У�鷽��
     */
    uint32_t                parity:1;

    /**
     * \brief ָ���˿ڵ�ǰʹ�õ�ֹͣλ��
     */
    uint32_t                stop_bits:2;

    /**
     * \brief �Ƿ���CTS(clear-to-send)�ź�����������ء�
     *
     * ������Ϊ1ʱ��CTS��Чʱ���ݷ��ͱ�����ֱ��CTS��Ч��
     */
    uint32_t                f_ctsflow:1;

    /**
     * \brief ����RTS(request-to-send)���ء�
     */
    uint32_t                f_rtsctrl:2;

    /**
     * \brief ����dsr_sensitivity, ���Ϊ1,���DSR�ź�����,����DSR�ź���Ч��
     *        ���򽫺������н��յ��ֽ�
     */
    uint32_t                f_dsrsensitivity:1;

    /**
     * \brief �Ƿ���DSR(data-set-ready�ź������������)��
     *
     * ������Ϊ1ʱ����DSR��Чʱ���ݷ��ͱ�����ֱ��DSR��Ч��
     */
    uint32_t                f_dsrflow:1;

    /**
     * \brief ����DTR(data-termial-ready)���ء�
     */
    uint32_t                f_dtrctrl:2;

    /**
     * \brief XON/XOFF���������ڷ���ʱ�Ƿ���á�
     *
     * ������Ϊ1ʱ����xoffֵ���յ�ʱ������ֹͣ����xonֵ���յ�ʱ�����ͼ�����
     */
    uint32_t                f_outx:1;

    /**
     * \brief XON/XOFF���������ڽ���ʱ�Ƿ���á�
     *
     * ������Ϊ1ʱ�������ջ������п�������С��xoff_lim�ֽ�ʱ������xoff�ַ���
     *              �����ջ�����������xon_lim�ֽڵĿ�������ʱ������xon�ַ���
     */
    uint32_t                f_inx:1;

    uint32_t                f_dummy:15;

    /**
     * \brief ������
     */
    uint32_t                baud_rate;

    /**
     * \brief ��XON�ַ�����ǰ���ջ������ڿ�����������С�ֽ���
     */
    uint32_t                xon_lim;
    /**
     * \brief ��XOFF�ַ�����ǰ���ջ������ڿ�������������ֽ���
     */
    uint32_t                xoff_lim;

    /**
     * \brief ָ��XON�ַ�
     */
    char                    xon_char;

    /**
     * \brief ָ��XOFF�ַ�
     */
    char                    xoff_char;

    /**
     * \brief ������Ӧ����ģʽ
     *
     * ������Ϊ1ʱ�����ڽ��յ�һ�����ݺ󽫻����������жϣ�������һЩ
     *              ���ڴ�����Ӧ�ٶ�Ҫ��ϸߵ����������Modbus��RTUģʽ��
     *              ��Ҫע����ǣ�ʹ�ܸñ�־�󣬴��ڵĽ����жϽ���
     *              Ƶ���Ĵ���������CPU�ĸ��ɡ�
     */
    uint8_t                 f_fast_reaction:1;
};
#pragma pack(pop)


/**
 *  \brief ���ڳ�ʱ����
 *
 *  \note: ���磺����rd_timeoutΪ100ms(��ʾ�ȴ����յ�1�����ݵ�ʱ��Ϊ100ms)��
 *             ����rd_interval_timeoutΪ10ms(��ʾ�ڽ��յ���1�����ݺ�
 *                                         �Ժ�ÿ2�ֽ�����֮��ĳ�ʱʱ��Ϊ10ms)��
 *             ����aw_serial_read()��������Ҫ��ȡ20�ֽ����ݡ�
 *
 *             ���1����1���ֽ����ڵ�101msʱ���٣����ڽ�����0�ֽ����ݲ�����0��ʾ��ʱ��
 *             ���2����1���ֽ����ڵ�99msʱ���ٵģ���103msʱ���˵�2�ֽ����ݣ��Ժ�ÿ4ms��1�ֽ����ݣ�һ��
 *                  ����20�ֽ����ݣ��򴮿ڽ�����20�ֽ����ݣ��������ض�ȡ��20�ֽ����ݡ�
 *             ���3����1���ֽ����ڵ�99msʱ���ٵģ���114msʱ���˵�2�ֽ����ݣ��Ժ��15ms��1�ֽ����ݣ�һ��
 *                  ����20�ֽ����ݣ��򴮿ڽ�����1�ֽ����ݣ�������1��ʾ������1�ֽ����ݡ�
 */
struct aw_serial_timeout {

    /**
     * \brief ����ʱʱ�䣬��λΪtick�����ʱ���ǵȴ����յ���1���ֽ����ݵĳ�ʱʱ�䡣
     *        ��������ʱ����û�ܽ��յ�1�����ݾͳ�ʱ���أ�
     *        ��������ʱ���ڽ��յ���1�����ݣ�������ÿ���ж��Ƿ���䳬ʱ��
     */
    uint32_t         rd_timeout;

    /**
     * \brief ��䳬ʱ����λΪtick�������ʱ����ÿ2���ֽ�֮��ĳ�ʱ�ȴ���
     *        �����n���ֽ����n+1���ֽ�֮���ʱ�䳬�������ʱ��ͳ�ʱ���ء�
     *        ��������ȴ��������ݣ�ÿ�εȴ��ĳ�ʱʱ��Ϊ�����ʱʱ�䡣
     */
    uint32_t         rd_interval_timeout;
};


/**
 * \brief �����豸����
 *
 * - ��������������еĴ����豸ioctl����
 * - FIOBAUDRATEӳ�䵽��sio_ioct��SIO_BAUD_SET����������е�ioctl���ֱ��
 *   ������sio_ioctl����
 *
 * \param[in] com     ���ڱ��
 *
 * \param[in] request iotcl�������󣬲μ� aw_ioctl.h �� aw_sio_common.h ��
 *              ���õ����󼰲������£�
 *              @arg AW_FIONREAD���ɶ����ݸ���������Ϊint��ָ��
 *              @arg AW_FIOFLUSH����ն����壬�޲���
 *              @arg AW_FIOSYNC��ͬFIOFLUSH
 *              @arg AW_FIORFLUSH��ͬFIOFLUSH
 *              @arg AW_TIOCRDTIMEOUT�����ö��ȴ�ȫ�ֳ�ʱʱ�䣬��λ���룬����Ϊ
 *                   int����
 *              @arg AW_TIOISATTY������豸�Ƿ�TTY���ã��޲�����TTY�豸����
 *                   trueֵ
 *              @arg SIO_BAUD_SET�����ô����豸�Ĳ����ʣ�����Ϊ������ֵ����9600
 *              @arg SIO_BAUD_GET����ȡ�����豸�Ĳ����ʣ�����Ϊint��ָ��
 *              @arg SIO_MODE_SET�����ô����豸��ģʽ������Ϊ��
 *                  - SIO_MODE_INT���ж�ģʽ
 *              @arg SIO_MODE_GET����ȡ�����豸�ĵ�ǰģʽ������Ϊintָ�룬ֵΪ
 *                   SIO_MODE_INT
 *              @arg SIO_AVAIL_MODES_GET����ȡ�����豸֧�ֵ�ģʽ������Ϊintָ�룬
 *                   SIO_MODE_INT �������ࡰ�򡱺��ֵ
 *              @arg SIO_HW_OPTS_SET�����ô����豸��ͨѶ������
 *                  ֧�ֵĲ�������Щ���������໥���򡱣���
 *                  - CLOCAL�� ����Modem��״̬��ʹ��3��UARTʱ����ʹ���������
 *                  - CREAD��ʹ�ܽ������ݣ�Ĭ���Ѿ�ʹ��
 *                  - ����λ����
 *                      - CS5�� 5λ����λ
 *                      - CS6�� 6λ����λ
 *                      - CS7�� 7λ����λ
 *                      - CS8�� 8λ����λ
 *                  - STOPB������ֹͣλ�����������þ�Ĭ��Ϊ1λ��������Ϊ2λ
 *                  - PARENB����żУ��ʹ�����ã���������ر���żУ�飬��������
 *                      - PARENB��ʹ��żУ��
 *                      - (PARENB | PARODD)��ʹ����У��
 *              @arg SIO_HW_OPTS_GET����ȡ�����豸��ͨѶ����������Ϊintָ�룬ֵ
 *                  ��ο� SIO_HW_OPTS_SET �����õ�ֵ
 *
 * \param[in,out] p_arg   ioctl��������������������ͺ�ȡֵȡ���ڶ�Ӧ����������
 *
 * \return ����ֵΪ���Ĵ�����룬�� aw_errno.h ������ȡ��������������ͷ���ֵ��
 *  - \b  AW_OK       ����ɹ�
 *  - \b -AW_EINVAL   ��������
 *  - \b -AW_ENODEV   com��Ӧ�Ĵ����豸û���ҵ���ϵͳ�ڲ�����
 *  - \b -AW_ENOTSUP  ���豸��֧�ִ˲������߲�֧�ִ˹���
 *
 * \par ����
 * \code
 * int baud;
 * aw_serial_ioctl(COM0, SIO_BAUD_SET, (void *)115200); // ���ò�����Ϊ115200bps
 * aw_serial_ioctl(COM0, SIO_BAUD_GET, (void *)&baud);  // ��ȡ��ǰ������ֵ
 * aw_serial_ioctl(COM0, AW_TIOCRDTIMEOUT, (void *)20); // ���ö���ʱΪ20����
 * // ���ô��ڲ���������modem��״̬��8������λ��1��ֹͣλ��ʹ��żУ��
 * aw_serial_ioctl(COM0, SIO_HW_OPTS_SET, (void *)(CLOCAL | CS8 | PARENB));
 * \endcode
 */
aw_err_t aw_serial_ioctl (uint32_t com, int request, void *p_arg);

/**
 * \brief �򴮿��豸д��nbytes���ֽ�����
 *
 * ����ӿ��򴮿��豸д��nbytes���ֽ����ݡ����豸���ڲ����岻�����������´���
 *
 * \param[in] com       ���ڱ��
 * \param[in] p_buffer  Ҫд���豸�����ݻ�����ָ��
 * \param[in] nbytes    Ҫд����ֽڸ��������ܳ���int�Ĵ�С
 *
 * \return �ɹ�д������ݸ�����С��0Ϊʧ�ܣ�ʧ��ԭ��Ϊ���Ĵ����룬�ɲ鿴
 *         aw_errno.h
 *
 * \note ��д����Ҫд����ֽ�����,��һ��д�������,����ӿ�ֻ�ǽ�Ҫд�����ݷ���
 *       ���ͻ������С�����ʲôʱ���������δ֪��, ��֮�Ὣ�������е����ݷ���
 *       ���Ϊֹ��
 */
int aw_serial_write (uint32_t com, aw_const void *p_buffer, size_t nbytes);


/**
 * \brief �Ӵ����豸��ȡmaxbytes���ֽ�����
 *
 * - ����ӿڴӴ����豸�������maxbytes���ֽ�����
 * - ���豸�ɶ����ݸ���Ϊ0���Ҷ���ʱ����Ϊ���ã��������´���
 *   - ���ûᱻ����ֱ������maxbytes������
 *
 * \param[in]  com      ���ڱ��
 * \param[out] p_buffer ��Ŷ�ȡ���ݵĻ�����ָ��
 * \param[in]  maxbytes ���Ҫ��ȡ���ֽڸ���
 *
 * \return �ɹ���ȡ�����ݸ�����С��0Ϊʧ�ܣ�ʧ��ԭ��Ϊ���Ĵ����룬�ɲ鿴
 *         aw_errno.h
 */
int aw_serial_read (uint32_t com, void *p_buffer, size_t maxbytes);


/**
 * \brief ��ѯ��ʽ�򴮿��豸д��nbytes���ֽ�����
 *
 * ����ӿ�����ѯ��ʽ�򴮿��豸д��nbytes���ֽ�����
 *
 * \param[in] com       ���ڱ��
 * \param[in] p_buffer  Ҫд���豸�����ݻ�����ָ��
 * \param[in] nbytes    Ҫд����ֽڸ���
 *
 * \return �ɹ�д������ݸ�����С��0Ϊʧ�ܣ�ʧ��ԭ��Ϊ���Ĵ����룬�ɲ鿴
 *         aw_errno.h
 *
 * \note �˽ӿ�ֻ�����쳣����(data_abort, prefetch_abort, undef_instruction)�м�
 *       �������е���,�ڷ��쳣�е����޷�Ӧ,����0
 */
int aw_serial_poll_write (uint32_t com, const void *p_buffer, size_t nbytes);


/**
 * \brief ���ô����豸����
 *
 * \param[in]  com      ���ڱ��
 * \param[in]  dcb      Ҫд�봮�����ýṹ��ָ��
 *
 *
 * \return ����ֵΪ���Ĵ�����룬�� aw_errno.h�����ͷ���ֵ��
 *  - \b  AW_OK         ���óɹ���û�д���
 *  - \b -AW_EINVAL     ��������
 *  - \b -AW_ENODEV     com��Ӧ�Ĵ����豸û���ҵ���ϵͳ�ڲ�����
 *  - \b -AW_ENOTSUP    ���ж��������е��ã�����ĳЩ���ܲ�֧��
 *
 * \note �ڽ�������ǰ����Ҫ�ȵ���aw_serial_dcb_get������ȡ��ǰ���ã�Ȼ���޸���Ҫ
 *       ���õ����ʹ��aw_serial_dcb_set������������
 *
 * \par ����
 * \code
 * struct aw_serial_dcb dcb;
 * //��ȡ��ǰ����
 * aw_serial_dcb_get(COM0, &dcb);
 * //�޸���Ҫ���õ���粨����
 * dcb.baud_rate = 9600;
 * //���ô���
 * aw_serial_dcb_set(COM0, &dcb);
 * \endcode
 */
aw_err_t aw_serial_dcb_set (uint32_t com, const struct aw_serial_dcb *p_dcb);


/**
 * \brief ��ȡ�����豸���ò���
 *
 * \param[in]  com      ���ڱ��
 * \param[out] p_dcb    ��Ŵ������ò�����ָ��
 *
 * \return ����ֵΪ���Ĵ�����룬�� aw_errno.h�����ͷ���ֵ��
 *  - \b  AW_OK       ��ȡ�ɹ���û�д���
 *  - \b -AW_EINVAL   com��������comֵ����ϵͳ���ô�������
 *  - \b -AW_ENODEV   com��Ӧ�Ĵ����豸û���ҵ���ϵͳ�ڲ�����
 */
aw_err_t aw_serial_dcb_get (uint32_t com, struct aw_serial_dcb *p_dcb);


/**
 * \brief ���ó�ʱ����
 *
 * \param[in]  com      ���ڱ��
 * \param[out] p_dcb    ��Ŵ��ڳ�ʱ������ָ��
 *
 * \return ����ֵΪ���Ĵ�����룬�� aw_errno.h�����ͷ���ֵ��
 *  - \b  AW_OK      ��ȡ�ɹ���û�д���
 *  - \b -AW_EINVAL  com��������comֵ����ϵͳ���ô�������, ������䳬ʱ������
 *                   ��ʱ
 *  - \b -AW_ENODEV  com��Ӧ�Ĵ����豸û���ҵ���ϵͳ�ڲ�����
 *  - \b -AW_ENOTSUP ��֧�֣����ж��������е���
 */
aw_err_t aw_serial_timeout_set(uint32_t com, struct aw_serial_timeout *p_cfg);


/**
 * \brief ��ȡ��ʱ����
 *
 * \param[in]  com      ���ڱ��
 * \param[out] p_cfg    ��Ŵ��ڳ�ʱ������ָ��
 *
 * \return ����ֵΪ���Ĵ�����룬�� aw_errno.h�����ͷ���ֵ��
 *  - \b  AW_OK       ��ȡ�ɹ���û�д���
 *  - \b -AW_EINVAL   com��������comֵ����ϵͳ���ô�������
 *  - \b -AW_ENODEV   com��Ӧ�Ĵ����豸û���ҵ���ϵͳ�ڲ�����
 */
aw_err_t aw_serial_timeout_get(uint32_t com, struct aw_serial_timeout *p_cfg);

/** @} grp_aw_if_serial */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SERIAL_H */

/* end of file */
