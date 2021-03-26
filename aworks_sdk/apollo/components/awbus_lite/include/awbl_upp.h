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
 * \brief AWBus ͨ�ò��нӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_upp.h"
 * \endcode
 * ��ģ��Ϊ ͨ�ò��нӿڵľ����ʵ��
 *
 * �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.00 15-10-15  dcf, first implementation
 * \endinternal
 */

#ifndef __AWBL_UPP_H
#define __AWBL_UPP_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include <stdarg.h>
#include "apollo.h"
#include "aw_msgq.h"
#include "awbus_lite.h"
#include "awbl_plb.h"


/**
 * \brief ͨ�ò��нӿ�
 *
 * Ӧ�ýӿڣ����г�Ա��Ӧ�ñ�Ӧ��ֱ��ʹ��
 *
 * �����ӿڣ�������Ҫ������ˡ��ڲ�ʹ�á��������������Ա��
 *      features:
 */
struct awbl_upp
{
    awbl_dev_t super;

/** \brief ������������ʹ�ã����߱�׼�㣬����֧����������  */
#define AWBL_UPP_FT_IN          1   

/** \brief ������������ʹ�ã����߱�׼�㣬����֧���������  */
#define AWBL_UPP_FT_OUT         2   

/** \brief ������������ʹ�ã����߱�׼�㣬����֧���ڲ�DMA����  */
#define AWBL_UPP_FT_BUILDIN_DMA 8   


    uint16_t    features;       /**< \brief ������������(֧�ֵĴ�����Ʊ�־)  */

    uint16_t    align_size;     /**< \brief �豸����Ҫ�� */
    uint32_t    max_xfer_size;  /**< \brief �豸ÿ������������� */

/** \brief ������������ʹ�ã������������豸 */
#define AWBL_UPP_CMD_OPEN       1   

/** \brief ������/Ӧ�ÿ�����ʹ�ã����������������� */
#define AWBL_UPP_CMD_XFER_START 2   

/** \brief ������/Ӧ�ÿ�����ʹ�ã�����������������*/
#define AWBL_UPP_CMD_XFER_STOP  3   

/** \brief ������������ʹ�ã����������ر��豸 */
#define AWBL_UPP_CMD_CLOSE      4   

/** \brief ������/Ӧ�ÿ�����ʹ�ã�������������Ϊ��� */
#define AWBL_UPP_CMD_SET_OUTPUT 5   

/** \brief ������/Ӧ�ÿ�����ʹ�ã����������������� */
#define AWBL_UPP_CMD_SET_INPUT  6   

/** \brief ������������ʹ�ã���������һ����ʼ���õ�msgq����׼��ʹ�� */
#define AWBL_UPP_CMD_SET_MSGQID 7   


#define AWBL_UPP_CMD_RESET      8   /**< \brief ������/Ӧ�ÿ�����ʹ�ã����� */
    aw_err_t (*ioctl)(struct awbl_upp *_this, int cmd, va_list va );

    /** \brief �ڲ�ʹ��,����������Ӧ�ÿ���������Ҫ��ע */
    void (*xfer_complete_callback)( void *callback_arg );
    void           *callback_arg;
    aw_msgq_id_t    msgid;
    int             is_opened;
};

/**
 * \brief  API for application developers
 *
 *
 * \param[in] device_name       �豸��
 * \param[in] unit_no           �豸���,�����е�unit
 *
 * \retval NULL  ��ʧ��
 */
struct awbl_upp *awbl_upp_open( const char *device_name, int unit_no );
void awbl_upp_close( struct awbl_upp *thiz );
/**
 * \brief �����豸����
 *
 * \param[in] cmd �����֣�Ӧ�ó����ȡ��
 *               AWBL_UPP_CMD_XFER_START:��Ҫ����window_address,line_size_valid,
 *                                       line_size,line_count��
 *                                       �ȼ�ʹ��awbl_upp_xfer_async()
 *               AWBL_UPP_CMD_XFER_STOP: ����Ҫ�������
 *               AWBL_UPP_CMD_SET_OUTPUT:����Ҫ�������
 *               AWBL_UPP_CMD_SET_INPUT: ����Ҫ�������
 *
 * windows_address�����ƶ�����ˮƽ�ʹ�ֱλ�ã�
 * ���ĸ��������Կ����ܴ��ڴ�С(���ⴰ�ڿ��)��
 * �������������Կ�����Ч���ڴ�С����ȣ���
 * line_count���Կ��Ƹ߶�
 * \retval AW_OK ��������ɹ�
 * \retval AW_ERROR �������ʧ��
 */
aw_err_t awbl_upp_ioctl(struct awbl_upp *_this, int cmd, ... );
aw_err_t awbl_upp_xfer_async(
        struct awbl_upp *_this,
        char       *window_address,
        uint32_t    line_size_valid,
        uint32_t    line_size_virtual,
        uint32_t    line_count
                        );
/**
 * \brief ������ɻص�����
 *
 * �˻ص��������ܱ�֤һ��Ҫ�ж������ĵ��á�
 */
void awbl_upp_set_callback(struct awbl_upp *_this,
        void (*xfer_complete_callback)( void * ),
        void *arg );
/**
 * \brief �������ͬ��
 *
 * �첽�ȴ���һ�δ�����ɡ�
 * \retval AW_OK �ɹ�����
 * \retval -ETIME ��ʱʧ��
 */
aw_err_t awbl_upp_wait_async( struct awbl_upp *_this, void **buffer, int timeout );

/**
 * \brief API for driver developers
 */
void awbl_upp_init(struct awbl_upp *_this );
void awbl_upp_destroy(struct awbl_upp *_this);

/** \brief need to set msgq_id */
aw_err_t awbl_upp_ioctl(struct awbl_upp *_this, int cmd, ... ); 
void awbl_upp_complete( struct awbl_upp *_this, void *buffer );


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_I2CBUS_H */

/* end of file */
