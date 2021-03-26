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
 * \brief SPI��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_spi.h
 *
 * \par ��ȡFLASH ID
 * \code
 *
 *  uint32_t cmd_rdid (aw_spi_device_t *p_dev)
 *  {
 *      uint8_t  cmd = 0x9f;    // RDID ����
 *      uint32_t id;
 *
 *      // ���� RDID ����
 *      aw_spi_write_then_read(p_dev,
 *                             &cmd,
 *                             1,
 *                             (uint8_t *)&id,
 *                             3);
 *      return id;
 *  }
 * \endcode
 *
 * \par SPI ����
 * \code
 *  
 *  int spi_test (void)
 *  {
 *      int i;
 *      aw_spi_device_t spi_flash;
 *
 *      // ���� SPI FLASH �豸
 *      aw_spi_mkdev(&spi_flash,
 *                   SPI0,              // λ������SPI������
 *                   8,                 // �ִ�С
 *                   AW_SPI_MODE_0,     // SPI ģʽ
 *                   3000000,           // ֧�ֵ�����ٶ�
 *                   PIO0_2,            // Ƭѡ����
 *                   NULL);             // ���Զ����Ƭѡ���ƺ���
 *
 *      // ���� SPI FLASH �豸
 *      if (aw_spi_setup(&spi_flash) != AW_OK) {
 *          return AW_ERROR;            // ����ʧ��
 *      }
 *
 *      // ��ȡ JEDEC ID
 *      if (cmd_rdid(&spi_flash) != FLASH_JEDEC_ID) {
 *          return AW_ERROR;            // JEDEC ID ����ȷ
 *      }
 *
 *      return AW_OK
 *  }
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-11-16  zen, first implementation
 * \endinternal
 */

#ifndef __AW_SPI_H
#define __AW_SPI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "aw_list.h"

/*lint ++flb */

/**
 * \addtogroup grp_aw_if_spi
 * \copydoc aw_spi.h
 * @{
 */

/**
 * \name SPIģʽ��־
 * @{
 */
 
/** \brief ���ú��2��ʱ���ؽ��в���,�����ڵ�1��ʱ���ؽ��в��� */
#define AW_SPI_CPHA         0x01

/** \brief ���ú����ʱ���Ǹߵ�ƽ, ����Ϊ�͵�ƽ */
#define AW_SPI_CPOL         0x02  
#define AW_SPI_CS_HIGH      0x04  /**< \brief Ƭѡ����Ч, ����Ƭѡ����Ч  */
#define AW_SPI_LSB_FIRST    0x08  /**< \brief ����λ�ȳ��ķ�ʽ��������֡ */
#define AW_SPI_3WIRE        0x10  /**< \brief 3��ģʽ��SI/SO �ź��߹���*/
#define AW_SPI_LOOP         0x20  /**< \brief �ػ�ģʽ  */
#define AW_SPI_NO_CS        0x40  /**< \brief ���豸����, ��Ƭѡ */

/** \brief READY�ź�,�ӻ����ʹ��ź���ͣ���� */
#define AW_SPI_READY        0x80  
#define AW_SPI_AUTO_CS      0x100 /**< \brief Ӳ���Զ����� CS ��� */

/**
 *  \brief SPIģʽ0 (MicroWire) ����ʱ���ǵ͵�ƽ, ��1��ʱ���ز���
 */
#define AW_SPI_MODE_0   (0 | 0)

/**
 *  \brief SPIģʽ1 ����ʱ���ǵ͵�ƽ, ��2��ʱ���ز���
 */
#define AW_SPI_MODE_1   (0 | AW_SPI_CPHA)

/**
 *  \brief SPIģʽ2 ����ʱ���Ǹߵ�ƽ, ��1��ʱ���ز���
 */
#define AW_SPI_MODE_2   (AW_SPI_CPOL | 0)

/**
 *  \brief SPIģʽ3 ����ʱ���Ǹߵ�ƽ, ��2��ʱ���ز���
 */
#define AW_SPI_MODE_3   (AW_SPI_CPOL | AW_SPI_CPHA)

/** \brief SPI�ڶ�ȡ������MOSI��������ߵ�ƽ��Ĭ��Ϊ�͵�ƽ��*/
#define AW_SPI_READ_MOSI_HIGH    0x0200

/** @} */

/**
 * \brief SPI ����
 *
 * - SPI ����д�ĸ������ǵ��ڶ��ĸ�����Э������Ӧ�������ṩ \a rx_buf ��/��
 * \a tx_buf ����ĳЩ����£�����ͬ���������ṩ���ݴ����DMA��ַ�������Ļ�����
 * ����������ʹ��DMA���Խ������ݴ���Ĵ��ۡ�
 *
 * - ������仺����\a tx_buf ΪNULL���������\a rx_buf ʱ��0���ᱻ�Ƴ��������ϡ�
 * ������ջ����� \a rx_buf ΪNULL�������������������ݽ��ᱻ������ֻ�� \a len
 * ���ֽڻᱻ�Ƴ������롣�����Ƴ��������Ǵ���Ĳ��������磬�Ƴ�3���ֽڶ��ִ�С
 * �� 16 �� 20 bit��ǰ��ÿ����ʹ��2���ֽڣ�������ʹ��4���ֽڡ�
 *
 * - ���ڴ��У��������ǰ�CPU�ĵı����ֽ�˳���ţ������ֽ�˳���� (����ˣ�����
 * �豸ģʽ�����־�б������� AW_SPI_LSB_FIRST)�����磬��\a bits_per_word Ϊ
 * 16��������Ϊ 2N �ֽڳ� (\a len = 2N) ������CPU�ֽ�˳�㱣��N��16-bit�֡�
 *
 * - �� SPI ������ִ�С����2���ݴα�8-bitʱ����Щ�ڴ��е��ֽ����������bit����
 * ���У����������Ҷ���ģ���ˣ�δ���� (rx) ��δʹ�� (tx) ��λ���������Чλ��
 *
 * - ����SPI���俪ʼʱ��Ƭѡ�����Ч��ͨ����һֱ������Ч��ֱ����Ϣ�����һ������
 * ��ɡ���������ʹ�� \a cs_change ��Ӱ��Ƭѡ:
 *  -# ����ô��䲻����Ϣ�����һ������ñ�־����������Ϣ���м���Ƭѡ��Ϊ��Ч��
 *     �����ַ�ʽ����תƬѡ��������Ҫ��ֹһ��оƬ����õ��� aw_spi_message_t
 *     ִ������оƬ�����顣
 *  -# ����ô�������Ϣ�����һ������Ƭѡ������Чֱ����һ�����䡣�ڶ��豸SPI����
 *     �ϣ���û��Ҫ���͸������豸����Ϣ�������������������ܵ�һ�����ɣ�
 *     ���͸���һ���豸����Ϣ��ʼִ�к󣬱��ʹ��ǰ�豸Ƭѡ��Ч�����ǣ�������ĳ
 *     Щ����£��ñ�־���Ա�������֤��ȷ�ԡ�ĳЩ�豸��ҪЭ��������һϵ�е�
 *     aw_spi_message_t ��ɣ�����һ����Ϣ��������ǰһ����Ϣ�Ľ��������������
 *     ������Ƭѡ��Ϊ��Ч��������
 *
 *  -# �����һ��message��Ƭѡ������Ч�ģ���ʱ��һ��message��Ƭѡ���ź���һ����
 *     �Ų���ͬ��������Ϊ�ǲ�����ģ��᷵��һ����Ӧ�Ĵ���
 *
 * \attention
 * ��ײ��ύ aw_spi_message_t �Ĵ��븺��������Լ����ڴ档�������е��㲻��Ҫ��
 * ȷ���õ����Ƽ�ʹ�� aw_spi_msg_init() ����ʼ�� aw_spi_message_t ��
 * ���ύ��Ϣ�Լ����Ĵ���֮�󣬲��ܲ�����Щ�ڴ�,ֱ����Ϣ�Ĵ�����ɻص�����������
 * (ʹ���첽���� aw_spi_async())��������(ʹ��ͬ������ aw_spi_sync())��
 */
typedef struct aw_spi_transfer {

    /** \brief �������ݻ�����(DMA��ȫ) */
    const void *p_txbuf;

    /** \brief �������ݻ�����(DMA��ȫ) */
    void       *p_rxbuf;

    /** \brief ����/���ջ����������ݳ��� */
    size_t      nbytes;

    /** \brief �ִ�С����Ϊ0����ʹ�� aw_spi_device_t �����õ�ֵ */
    uint8_t     bits_per_word;

    /** \brief ���δ�����ɺ��Ƿ�Ӱ��Ƭѡ: 1-Ӱ�� 0-��Ӱ�� */
    uint8_t     cs_change;

    /**
     * \brief �ڱ��δ�֮���ڸı�Ƭѡ״̬֮ǰ��ʱ��΢������Ȼ��ʼ��һ�����䣬
     *        �������ǰ��Ϣ��
     */
    uint16_t    delay_usecs;

    /**
     * \brief Ϊ���δ���ѡ��һ���� aw_spi_device_t �е�Ĭ�����ò�ͬ���ٶȣ�
     *        0 ��ʾʹ��Ĭ��ֵ
     */
    uint32_t    speed_hz;

    /**
     * \brief ���δ�����̵�һЩ�������ã������ڶ�ȡ����MOSI���ŵ�״̬
     */
    uint32_t    flag;

    /** \brief ������ */
    struct aw_list_head trans_node;

} aw_spi_transfer_t;

/**
 * \brief SPI ��Ϣ
 *
 * 1��SPI��Ϣ������ִ��һ��ԭ�ӵ����ݴ������У�ÿ�������� aw_spi_transfer ��ʾ��
 * ��������ԭ�ӵģ���ζ���κ�����SPI��Ϣ������ʹ��SPI���ߣ�ֱ��������ִ����ɡ�
 * ������ϵͳ�У���Ϣ�Ǳ��Ŷӵģ����͸�ĳ�� aw_spi_device_t ����Ϣ������FIFO��
 * ˳�򱻴���
 *
 * \attention
 * ��ײ��ύ aw_spi_message_t �Ĵ��븺��������Լ����ڴ档�������е��㲻��Ҫ��
 * ȷ���õ����Ƽ�ʹ�� aw_spi_msg_init() ����ʼ�� aw_spi_message_t ��
 * ���ύ��Ϣ�Լ����Ĵ���֮�󣬲��ܲ�����Щ�ڴ�,ֱ����Ϣ�Ĵ�����ɻص�����������
 * (ʹ���첽���� aw_spi_async())��������(ʹ��ͬ������ aw_spi_sync())��
 */
typedef struct aw_spi_message {

    /** \brief ָ��ǰmessage�Ŀ����� */
    struct awbl_spi_master *p_master;

    /** \brief �����Ϣ�Ĵ���� */
    struct aw_list_head transfers;   

    /** \brief ������Ϣ��SPI���豸 */    
    struct aw_spi_device *p_spi_dev; 

    /** \brief ������ɻص����� */
    aw_pfuncvoid_t  pfunc_complete;  

    /** \brief ���ݸ� pfunc_complete �Ĳ��� */
    void           *p_arg;           

    /** \brief ���гɹ����͵������ֽ����� */
    size_t          actual_length;

    /** \brief ��Ϣ��״̬ */
    int             status;          

    /** \brief msg�ڵ� */
    struct aw_list_head msg;

} aw_spi_message_t;

/** \brief SPI���豸�����ṹ */
typedef struct aw_spi_device {

    /** \brief �豸���������ӿڱ�� */
    uint8_t     busid;

    /**
     * brief ���ݴ������1�������֣��Ƚϳ������ִ�СΪ8-bit��12-bit�����ڴ�
     * �У��ִ�СӦ����2���ݴη������磬20-bit�Դ�СӦʹ��32-bit�ڴ档�����ÿ���
     * ���豸�����ı䣬��������Ϊ0��ʾʹ��Ĭ�ϵġ��ִ�СΪ8-bit����
     * �����䡱�� aw_spi_transfer_t.bits_per_word �������ش����á�
     */
    uint8_t     bits_per_word;

    /** \brief ���豸��SPIģʽ��־����ο���SPIģʽ��־��*/
    uint16_t    mode;

    /**
     * \brief �豸֧�ֵ�����ٶ�
     *
     * �����䡱�� aw_spi_transfer.speed_hz �����ش����á�
     */
    uint32_t    max_speed_hz;

    /**
     * \brief Ƭѡ���ű��
     *
     * �� aw_spi_device.pfunc_cs ��ΪNULLʱ��ʹ�ô����á�Ĭ�ϵ͵�ƽ��Ч����
     * aw_spi_device.mode �� AW_SPI_CS_HIGH ��־�����ã���ߵ�ƽ��Ч��
     */
    int         cs_pin;

    /**
     * \brief Ƭѡ���ƺ���
     *
     * ʹ�ô˺��������豸��Ƭѡ: ���� \a state Ϊ1ʱƬѡ��Ч��Ϊ0ʱƬѡ��Ч��
     * ��������ΪNULLʱ����ʹ�� aw_spi_device.cs_pin ��ΪƬѡ���ţ�����������
     * ��������һ�����ʵ�Ƭѡ���ƺ�����
     *
     * \param[in] state  Ƭѡ����
     */
    void      (*pfunc_cs) (int state);

    /**
     * \brief ���������ж�
     *
     * ���������aw_spi_mkdev,��setuped����0x55555555��
     * ���������aw_spi_setup,��setuped���ڸ����豸��ַ��
     */
    uint32_t   setuped;

} aw_spi_device_t;

/**
 * \brief ����SPI�豸�ṹ�����
 *
 * \param[in] p_dev         SPI�ӻ��豸������ָ��
 * \param[in] busid         �豸���������ӿڱ��
 * \param[in] bits_per_word �ִ�С��Ϊ0ʱʹ��Ĭ�ϵġ�8-bit�ִ�С��
 * \param[in] mode          �豸ģʽ��־������SPIģʽ��־��
 * \param[in] max_speed_hz  �豸֧�ֵ�����ٶ�
 * \param[in] cs_pin        Ƭѡ���ű��(\a pfunc_cs ΪNULLʱ����������Ч)
 * \param[in] pfunc_cs      Ƭѡ���ƺ���(��������Ϊ NULL ʱ�� \a cs_pin ������Ч)
 *
 * \par ����
 * �� aw_spi_setup()
 */
aw_local aw_inline void aw_spi_mkdev (aw_spi_device_t *p_dev,
                                      uint8_t          busid,
                                      uint8_t          bits_per_word,
                                      uint16_t         mode,
                                      uint32_t         max_speed_hz,
                                      int              cs_pin,
                                      void (*pfunc_cs) (int state))
{
    p_dev->busid         = busid;
    p_dev->bits_per_word = bits_per_word;
    p_dev->mode          = mode;
    p_dev->max_speed_hz  = max_speed_hz;
    p_dev->cs_pin        = cs_pin;
    p_dev->pfunc_cs      = pfunc_cs;
    p_dev->setuped       = 0x55555555;
}

/**
 * \brief ����SPI����ṹ�����
 *
 * \param[in] p_trans       ����ṹ��������ָ��
 * \param[in] p_txbuf       �������ݻ�����
 * \param[in] p_rxbuf       �������ݻ�����
 * \param[in] nbytes        ����/���ջ����������ݳ���
 * \param[in] cs_change     �Ƿ�Ӱ��Ƭѡ
 * \param[in] bits_per_word �ִ�С
 * \param[in] delay_usecs   �ڸı�Ƭѡ״̬֮ǰ��ʱ��΢����
 * \param[in] speed_hz      �δ����ٶ�
 * \param[in] flag          ���δ�����̵�һЩ��������
 *
 * \par sa aw_spi_async()
 */
aw_local aw_inline void aw_spi_mktrans (aw_spi_transfer_t *p_trans,
                                        const void        *p_txbuf,
                                        void              *p_rxbuf,
                                        size_t             nbytes,
                                        uint8_t            cs_change,
                                        uint8_t            bits_per_word,
                                        uint16_t           delay_usecs,
                                        uint32_t           speed_hz,
                                        uint32_t           flag)
{
    p_trans->p_txbuf        = p_txbuf;
    p_trans->p_rxbuf        = p_rxbuf;
    p_trans->nbytes         = nbytes;
    p_trans->cs_change      = cs_change;
    p_trans->bits_per_word  = bits_per_word;
    p_trans->delay_usecs    = delay_usecs;
    p_trans->speed_hz       = speed_hz;
    p_trans->flag           = flag;
}

/**
 * \brief ��ʼ����Ϣ
 *
 * \param[in] p_msg           SPI��Ϣ������ָ��
 * \param[in] pfunc_complete ������ɻص�����(������ aw_i2c_async() ʱ�˲�����Ч,
 *                           ���� aw_i2c_sync()ʱ��������Ч)
 * \param[in] p_arg          ���ݸ��ص������Ĳ���
 *
 * \sa aw_spi_async()
 */
aw_local aw_inline void aw_spi_msg_init (aw_spi_message_t  *p_msg,
                                         aw_pfuncvoid_t     pfunc_complete,
                                         void              *p_arg)
{
    memset(p_msg, 0, sizeof(*p_msg));

    AW_INIT_LIST_HEAD(&p_msg->transfers);

    p_msg->pfunc_complete = pfunc_complete;
    p_msg->p_arg          = p_arg;
    p_msg->status         = -AW_ENOTCONN;
}

/**
 * \brief ��������ӵ���Ϣĩβ
 *
 * \param[in] p_msg    SPI��Ϣ������ָ��
 * \param[in] p_trans  SPI����������ָ��
 */
aw_local aw_inline void aw_spi_trans_add_tail (aw_spi_message_t  *p_msg,
                                               aw_spi_transfer_t *p_trans)
{
    aw_list_add_tail(&p_trans->trans_node, &p_msg->transfers);
}

/**
 * \brief ���������Ϣ��ɾ��
 *
 * \param[in] p_trans   SPI����������ָ��
 */
aw_local aw_inline void aw_spi_trans_del (aw_spi_transfer_t *p_trans)
{
    aw_list_del(&p_trans->trans_node);
}

/**
 * \brief ����SPI�ӻ��豸
 *
 * \attention ��SPI�ӻ��豸��ʹ��ǰ�������ȵ��ñ������������ã����籾��������
 *            ������һ��������ʹ�ô�SPI�ӻ�
 *
 * \param[in,out]   p_dev   SPI�ӻ��豸
 *
 * \retval   AW_OK      �ɹ�
 * \retval  -AW_ENXIO   δ�ҵ�ָ����SPI����
 * \retval  -AW_ENOTSUP ĳЩ���Բ�֧��
 *
 * \par ʾ��
 * \code
 * aw_spi_device_t spi_dev;     // �豸�����ṹ
 *
 * // ��ʼ���豸�����ṹ
 * aw_spi_mkdev(&spi_dev,
 *              SPI0,           // λ��SPI0������
 *              8��             // �ִ�СΪ8-bit
 *              AW_SPI_MODE_0,  // SPI ģʽ0
 *              500000,         // ֧�ֵ�����ٶ� 500000 Hz
 *              PIO0_3,         // Ƭѡ����Ϊ PIO0_3
 *              NULL);          // ���Զ����Ƭѡ���ƺ���
 *
 * // �����豸
 * aw_spi_setup(&spi_dev);
 *
 * \endcode
 */
aw_err_t aw_spi_setup(aw_spi_device_t *p_dev);

/**
 * \brief ������Ϣ���첽ģʽ
 *
 * ���첽�ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status��
 * ��Ϣ�������(�ɹ������)ʱ���������\a p_msg->pfunc_complete �����ݲ���
 * \a p_msg->p_arg��
 *
 * \param[in]       p_dev   SPI�豸����
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval  AW_OK       ��Ϣ�Ŷӳɹ����ȴ�����
 * \retval  -AW_EINVAL  ��������
 *
 * �ɹ�����/���յ����ݸ�����ӳ��\a p_msg->actual_length
 *
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status ��
 *
 *      \li  -AW_ENOTCONN    ��Ϣ��δ�Ŷ�
 *      \li  -AW_EISCONN     ��Ϣ�����Ŷ�
 *      \li  -AW_EINPROGRESS ��Ϣ���ڱ�����
 * ����Ϊ��Ϣ������ϵĽ��
 *      \li  AW_OK           ���д���ɹ�����
 *      \li  -AW_ENOTSUP     ��Ϣ��ĳ����������ò�֧��(���磬�ִ�С���ٶȵ�)
 *      \li  -AW_ECANCELED   �����������������������Ϣ��ȡ�������Ժ�����
 *
 * \par ����
 * \code
 *  aw_spi_device_t   spi_dev;
 *  aw_spi_message_t  msg;
 *  aw_spi_transfer_t trans[3];
 *  uint8_t           txbuf[16];
 *  uint8_t           rxbuf[16];
 *
 *  // ��ʼ���豸�����ṹ
 *  aw_spi_mkdev(&spi_dev,
 *               SPI0,           // λ��SPI0������
 *               8��             // �ִ�СΪ8-bit
 *               AW_SPI_MODE_0,  // SPI ģʽ0
 *               500000,         // ֧�ֵ�����ٶ� 500000 Hz
 *               PIO0_3,         // Ƭѡ����Ϊ PIO0_3
 *               NULL);          // ���Զ����Ƭѡ���ƺ���
 *
 *  // �����豸
 *  if (aw_spi_setup(&spi_dev) != AW_OK) {
 *      // �����豸ʧ��
 *  }
 *
 *  //  ��һ������,����16���ֽ�
 *  aw_spi_mktrans(&trans[0],       // ���������ṹ 0
 *                 &txbuf[0],       // ���ͻ�����
 *                 NULL,            // ���ջ���
 *                 16,              // ���͸���
 *                 0,               // cs_change �޸ı�
 *                 0,               // bpw ʹ��Ĭ������
 *                 0,               // udelay ����ʱ
 *                 0);              // speed ʹ��Ĭ��ֵ
 *
 *  //  �ڶ�������,����16���ֽ�
 *  aw_spi_mktrans(&trans[1],       // ���������ṹ 1
 *                 NULL,            // ���ͻ�����
 *                 &rxbuf[0],       // ���ջ���
 *                 16,              // ���ո���
 *                 0,               // cs_change �޸ı�
 *                 0,               // bpw ʹ��Ĭ������
 *                 0,               // udelay ����ʱ
 *                 0);              // speed ʹ��Ĭ��ֵ
 *
 *  //  ����������,�����ҽ���16���ֽ�
 *  aw_spi_mktrans(&trans[2],       // ���������ṹ 2
 *                 &txbuf[0],       // ���ͻ�����
 *                 &rxbuf[0],       // ���ջ���
 *                 16,              // ���ո���
 *                 0,               // cs_change �޸ı�
 *                 0,               // bpw ʹ��Ĭ������
 *                 0,               // udelay ����ʱ
 *                 0);              // speed ʹ��Ĭ��ֵ
 *
 *  // �����������Ϣ
 *  aw_spi_msg_init(&msg, my_callback, my_arg); // ��ʼ����Ϣ
 *  aw_spi_trans_add_tail(&msg, &trans[0]);     // ��ӵ�1������
 *  aw_spi_trans_add_tail(&msg, &trans[1]);     // ��ӵ�2������
 *  aw_spi_trans_add_tail(&msg, &trans[2]);     // ��ӵ�3������
 *
 *  // �첽���ʹ���Ϣ�������������أ�������ɺ����ú��� my_callback (my_arg)
 *  aw_spi_async(&spi_dev, &msg);
 *
 *  // ͬ�����ʹ���Ϣ��������ɺ����ŷ���
 *  aw_spi_sync(&spi_dev, &msg);
 *
 * \endcode
 *
 * \sa aw_spi_sync()
 */
aw_err_t aw_spi_async(aw_spi_device_t  *p_dev,
                      aw_spi_message_t *p_msg);

/**
 * \brief ������Ϣ��ͬ��ģʽ
 *
 * ��ͬ���ķ�ʽ������Ϣ����Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status��
 *
 * \param[in]       p_dev   SPI�豸����
 * \param[in,out]   p_msg   Ҫ�������Ϣ
 *
 * �����ķ���ֵ���£�
 * \retval AW_OK          ��Ϣ����ɹ�
 * \retval -AW_EINVAL     ��������\a p_msg ��ĳЩ��Ա������Ч������
 * \retval -AW_ENOTSUP    ��Ϣ��ĳ����������ò�֧��(���磬�ִ�С���ٶȵ�)
 * \retval -AW_ECANCELED  �����������������������Ϣ��ȡ�������Ժ�����
 *
 * �ɹ�����/���յ����ݸ�����ӳ��\a p_msg->actual_length��
 * ��Ϣ�Ĵ���״̬�ͽ����ӳ��\a p_msg->status:
 *      \li  -AW_ENOTCONN       ��Ϣ��δ�Ŷ�
 *      \li  -AW_EISCONN        ��Ϣ�����Ŷ�
 *      \li  -AW_EINPROGRESS    ��Ϣ���ڱ�����
 *
 * ����Ϊ��Ϣ������ϵĽ��
 *      \li  AW_OK              ���д���ɹ�����
 *      \li  -AW_ENOTSUP        ��Ϣ��ĳ����������ò�֧��(���磬�ִ�С���ٶȵ�)
 *      \li  -AW_ECANCELED      �����������������������Ϣ��ȡ�������Ժ�����
 *
 * \sa aw_spi_async()
 */
aw_err_t aw_spi_sync(aw_spi_device_t  *p_dev,
                     aw_spi_message_t *p_msg);

/**
 * \brief ��д���
 *
 * �������Ƚ���SPIд�������ٽ��ж�������
 * ��ʵ��Ӧ���У�д����������Ϊ��ַ����������ΪҪ�Ӹõ�ַ��ȡ�����ݡ�
 *
 * \param[in]      p_dev   SPI�豸����
 * \param[in]      p_txbuf ���ݷ��ͻ�����
 * \param[in]      n_tx    Ҫ���͵������ֽڸ���
 * \param[out]     p_rxbuf ���ݽ��ջ�����
 * \param[in]      n_rx    Ҫ���յ������ֽڸ���
 *
 * \retval AW_OK        ��Ϣ����ɹ�
 * \retval -AW_EINVAL   ��������\a p_msg ��ĳЩ��Ա������Ч������
 * \retval -AW_ENOTSUP  ��Ϣ��ĳЩ�������õ����Բ�֧��
 */
aw_err_t aw_spi_write_then_read(aw_spi_device_t *p_dev,
                                const uint8_t   *p_txbuf,
                                size_t           n_tx,
                                uint8_t         *p_rxbuf,
                                size_t           n_rx);

/**
 * \brief ִ������д����
 *
 * ����������ִ������д���������η������ݻ�����0�ͻ�����1�е����ݡ�
 * ��ʵ��Ӧ���У�������0����Ϊ��ַ��������1ΪҪд��õ�ַ�����ݡ�
 *
 * \param[in]   p_dev    SPI�豸����
 * \param[in]   p_txbuf0 ���ݷ��ͻ�����0
 * \param[in]   n_tx0    ������0���ݸ���
 * \param[in]   p_txbuf1 ���ݷ��ͻ�����1
 * \param[in]   n_tx1    ������1���ݸ���
 *
 * \retval AW_OK        ��Ϣ����ɹ�
 * \retval -AW_EINVAL   ��������\a p_msg ��ĳЩ��Ա������Ч������
 * \retval -AW_ENOTSUP  ��Ϣ��ĳЩ�������õ����Բ�֧��
 */
aw_err_t aw_spi_write_then_write(aw_spi_device_t *p_dev,
                                 const uint8_t   *p_txbuf0,
                                 size_t           n_tx0,
                                 const uint8_t   *p_txbuf1,
                                 size_t           n_tx1);


/**
 * \brief д���Ҷ�����
 *
 *
 * \param[in]   p_dev    SPI�豸����
 * \param[in]   p_txbuf ���ݷ��ͻ�����
 * \param[out]  p_rxbuf ���ݷ��ͻ�����
 * \param[in]   n_tx1    ������1���ݸ���
 *
 * \retval AW_OK        ��Ϣ����ɹ�
 * \retval -AW_EINVAL   ��������\a p_msg ��ĳЩ��Ա������Ч������
 * \retval -AW_ENOTSUP  ��Ϣ��ĳЩ�������õ����Բ�֧��
 */
aw_err_t aw_spi_write_and_read (struct aw_spi_device   *p_dev,
                                const void             *p_txbuf,
                                void                   *p_rxbuf,
                                size_t                  nbytes);

/** @} grp_aw_if_spi */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SPI_H */

/* end of file */
