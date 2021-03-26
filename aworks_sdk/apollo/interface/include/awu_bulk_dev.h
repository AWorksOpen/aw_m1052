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
 * \brief USB Bulk Device �ӿ�
 *
 * \internal
 * \par modification history:
 * - 1.00 13-07-09  zen, first implementation
 * \endinternal
 */

#ifndef __AWU_BULK_DEV_H
#define __AWU_BULK_DEV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/** \brief ���󷵻��� (��ֵ) */
#define AWU_OK              0
#define AWU_NOTSTARTED      1
#define AWU_INPROGRESS      2
#define AWU_PERM            3
#define AWU_NOENT           4
#define AWU_IO              5
#define AWU_NXIO            6
#define AWU_NOMEM           7
#define AWU_BUSY            8
#define AWU_NODEV           9
#define AWU_INVAL           10
#define AWU_NOTSUP          11
#define AWU_TIMEDOUT        12
#define AWU_SUSPENDED       13
#define AWU_UNKNOWN         14
#define AWU_TEST_FAILED     15
#define AWU_STATE           16
#define AWU_STALLED         17
#define AWU_PARAM           18
#define AWU_ABORTED         19
#define AWU_SHORT           20

/** \brief ���õȴ� */
#define AWU_WAIT_FOREVER    (-1)

/** \brief �豸��Ϣ */
typedef struct {
    uint16_t  vid;                    /**< \brief �豸�� Vendor ID */
    uint16_t  pid;                    /**< \brief �豸�� Product ID */
    uint16_t  release;                /**< \brief �豸�İ汾�� */
    char     *p_product_name;         /**< \brief �豸���� */
    char     *p_product_manufacturer; /**< \brief �豸���������� */
    char     *p_product_serial;       /**< \brief �豸���к� */
    int       self_powered;           /**< \brief �Ƿ��Թ����豸 1-�� 0-�� */
    uint32_t  max_power;              /**< \brief �豸��Ҫ�������� (mA) */
} awu_bulk_devinfo_t;


/** \brief ��Щ�ص������� Bulk �豸�ڶ�Ӧ�Ľ׶ε���, ÿ���ص����ǿ�ѡ�� */
typedef struct {
    int  (*init)(void *context);    /**< \brief �豸��ʼ��ʱ���� */
    int  (*enable)(void *context);  /**< \brief �豸����(���������ӳɹ�)ʱ���� */
    int  (*disable)(void *context); /**< \brief �豸�Ͽ�(�������Ͽ�)ʱ���� */
    int  (*suspend)(void *context); /**< \brief �豸����ʱ���� */
    int  (*resume)(void *context);  /**< \brief �豸�ָ�ʱ���� */
    int  (*uninit)(void *context);  /**< \brief �豸ж��ʱ���� */
} awu_bulk_app_cb_t;


/**
 * \brief ��ʼ�� Bulk �豸
 *
 * \param p_devinfo     Bulk �豸�ĳ�ʼ����Ϣ����ο� awu_bulk_devinfo_t
 * \param p_app_cb      Ӧ�ó����ṩ�Ļص���������Bulk �豸�Ĳ�ͬ״̬�±��ص�,
 *                      ��ο� awu_bulk_app_cb_t
 * \param p_app_ctx     Bulk Ӧ�ó���ص����������ģ�p_app_cb �лص������Ĳ���
 *
 * \retval AWB_OK       �ɹ�
 * \retval ��ֵ         ���󣬾��庬����ο������󷵻��롱
 */
int awu_bulk_dev_init (awu_bulk_devinfo_t *p_devinfo,
                       awu_bulk_app_cb_t  *p_app_cb,
                       void               *p_app_ctx);

/**
 * \brief Bulk �豸�Ƿ��ѳ�ʼ��������(�ѳɹ���������)
 *
 * \retval  1    �ѳ�ʼ��������
 * \retval  0   ��δ��ʼ��δ����
 */
int awu_bulk_dev_is_ready(void);

/**
 * \brief ���� Bulk �豸
 *
 * \attention ������Ӧ�����豸��ʼ���Լ�����(awu_bulk_dev_init())���֮����á�
 *            �������ʼ��Ӳ�����Ӳ����¶˵����á���USB�������ӵ��豸ʱ��
 *            �������Ὺʼ���豸��ö�١�
 *
 * \retval AWB_OK       �ɹ�
 * \retval ��ֵ         ���󣬾��庬����ο������󷵻��롱
 */
int awu_bulk_dev_run(void);

/**
 * \brief �� Bulk �ܵ�
 *
 * \param pipe    Bulk �ܵ��� (1, 2... ), �ܵ�������ο�ƽ̨˵��
 * \param p_buf   ��������
 * \param nbytes  ��ȡ���ݸ���
 * \param timeout ��ʱ (ms) AWU_WAIT_FOREVER(���õȴ�)
 *
 * \retval >=0    ʵ�ʶ�ȡ�������ݸ���
 * \retval ��ֵ   ���󣬾��庬����ο������󷵻��롱
 */
ssize_t awu_bulk_read(unsigned  pipe,
                      void     *p_buf,
                      size_t    nbytes,
                      int       timeout);

/**
 * \brief д Bulk �ܵ�
 *
 * \param pipe    Bulk �ܵ��� (1, 2... ), �ܵ�������ο�ƽ̨˵��
 * \param p_buf   д������
 * \param nbytes  д���ݸ���
 * \param timeout ��ʱ (ms) AWU_WAIT_FOREVER(���õȴ�)
 *
 * \retval >=0    ʵ��д������ݸ���
 * \retval ��ֵ   ���󣬾��庬����ο������󷵻��롱
 */
ssize_t awu_bulk_write(unsigned    pipe,
                       const void *p_buf,
                       size_t      nbytes,
                       int         timeout);

/**
 * \brief ȡ�� Bulk �ܵ��ϵĶ�����
 *
 * \param pipe    Bulk �ܵ��� (1, 2... ), �ܵ�������ο�ƽ̨˵��
 */
void awu_bulk_read_abort(unsigned pipe);

/**
 * \brief ȡ�� Bulk �ܵ��ϵ�д����
 *
 * \param pipe    Bulk �ܵ��� (1, 2... ), �ܵ�������ο�ƽ̨˵��
 */
void awu_bulk_write_abort(unsigned pipe);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWU_BULK_DEV_H */

/* end of file */
