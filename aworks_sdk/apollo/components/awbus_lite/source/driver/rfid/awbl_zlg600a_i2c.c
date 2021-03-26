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
 * \brief AWBus ZLG600A I2C driver
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

#include "awbus_lite.h"
#include "aw_i2c.h"
#include "aw_gpio.h"
#include "aw_sem.h"
#include "awbl_i2cbus.h"

#include "driver/rfid/awbl_zlg600a_i2c.h"

/******************************************************************************/

/* declare pcf8563 device instance */
#define __ZLG600A_I2C_DEV_DECL(p_this, p_dev) \
    struct awbl_zlg600a_i2c_dev *p_this = \
        (struct awbl_zlg600a_i2c_dev *)(p_dev)

/* declare pcf8563 device infomation */
#define __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev) \
    struct awbl_zlg600a_i2c_devinfo *p_devinfo = \
        (struct awbl_zlg600a_i2c_devinfo *)AWBL_DEVINFO_GET(p_dev)

/*******************************************************************************
  forward declarations
*******************************************************************************/

/** \brief ����һ����֡���� */
aw_local aw_err_t __zlg600a_i2c_frame_old_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief ����һ���ɻ�Ӧ֡ */
aw_local aw_err_t __zlg600a_i2c_frame_old_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief ����һ��������֡ */
aw_local aw_err_t __zlg600a_i2c_frame_new_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/* ����һ���»�Ӧ֡ */
aw_local aw_err_t __zlg600a_i2c_frame_new_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief I2C�豸���ƺ��� */
aw_local aw_err_t __zlg600a_i2c_device_ioctl (void *p_cookie, 
                                              int   request, 
                                              void *p_arg);

/** \brief first level initialization routine */
aw_local void __zlg600a_i2c_inst_init1 (awbl_dev_t *p_dev);

/** \brief second level initialization routine */
aw_local void __zlg600a_i2c_inst_init2 (awbl_dev_t *p_dev);

/** \brief third level initialization routine */
aw_local void __zlg600a_i2c_inst_connect (awbl_dev_t *p_dev);

/** \brief method "awbl_zlg600a_uartserv_get" handler */
aw_local aw_err_t __zlg600a_i2c_zlg600aserv_get (struct awbl_dev *p_dev,
                                                 void            *p_arg);


/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_zlg600a_i2c_drvfuncs = {
    __zlg600a_i2c_inst_init1,
    __zlg600a_i2c_inst_init2,
    __zlg600a_i2c_inst_connect
};

/* pcf8563 service functions (must defined as aw_const) */
aw_local aw_const struct awbl_zlg600a_servopts __g_zlg600a_i2c_old_servopts = {
    __zlg600a_i2c_frame_old_cmd_send,
    __zlg600a_i2c_frame_old_respond_recv,
    __zlg600a_i2c_device_ioctl,
};

/* pcf8563 service functions (must defined as aw_const) */
aw_local aw_const struct awbl_zlg600a_servopts __g_zlg600a_i2c_new_servopts = {
    __zlg600a_i2c_frame_new_cmd_send,
    __zlg600a_i2c_frame_new_respond_recv,
    __zlg600a_i2c_device_ioctl,
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_zlg600aserv_get);

aw_local aw_const struct awbl_dev_method __g_zlg600a_i2c_dev_methods[] = {
    AWBL_METHOD(awbl_zlg600aserv_get, __zlg600a_i2c_zlg600aserv_get),
    AWBL_METHOD_END
};

/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_i2c_drvinfo_t __g_zlg600a_i2c_drv_registration = {
    {
        AWBL_VER_1,                       /* awb_ver */
        AWBL_BUSID_I2C,                   /* bus_id */
        AWBL_ZLG600A_I2C_NAME,            /* p_drvname */
        &__g_zlg600a_i2c_drvfuncs,        /* p_busfuncs */
        &__g_zlg600a_i2c_dev_methods[0],  /* p_methods */
        NULL                              /* pfunc_drv_probe */
    }
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief GPIO�ص�����
 * \param[in] p_arg : �ص�������ڲ���
 * \return ��
 */
aw_local void __zlg600a_pin_int_callback (void *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_arg);

    AW_SEMB_GIVE(p_this->semb);
}

/* I2C�ص����� */
aw_local void __zlg600a_i2c_trans_callback (void *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_arg);

    AW_SEMB_GIVE(p_this->semb);
}

/* �ȴ����Խ������� */
aw_local aw_err_t __zlg600a_wait_can_recv (awbl_zlg600a_i2c_dev_t *p_dev)
{
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    /* �Ѿ��ǵ͵�ƽ */
    if (aw_gpio_get(p_devinfo->pin) == 0) {
        return AW_OK;
    }

    aw_gpio_trigger_on(p_devinfo->pin);

    return AW_SEMB_TAKE(p_dev->semb,
                        aw_ms_to_ticks(p_devinfo->timeout));
}

/**
 * \brief ����һ����֡����
 */
aw_local aw_err_t __zlg600a_i2c_frame_old_cmd_send (void                      *p_cookie,
                                                    awbl_zlg600a_frame_base_t *p_frame)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_cookie);
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_cookie);

    aw_i2c_message_t      msg;
    aw_i2c_transfer_t     trans[9];

    int       i, j;
    uint8_t  *p_buf;
    uint8_t   info_len;
    uint8_t   temp[6];
    uint8_t   bcc = 0;

    temp[0] = p_frame->info_length + 0x6;
    bcc ^= temp[0];

    temp[1] = p_frame->cmd_class | (p_frame->frame_seq << 4);
    bcc ^= temp[1];

    temp[2] = p_frame->cmd_status;
    bcc ^= temp[2];

    temp[3] = p_frame->info_length;
    bcc ^= temp[3];

    aw_i2c_mktrans(&trans[0],
                   p_this->addr,
                   AW_I2C_M_7BIT | AW_I2C_M_WR,
                   (uint8_t *)temp,
                   4);

    /* ���͸�����Ϣ�������е���Ϣ */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;
        if (info_len != 0) {                        /* ������Ϣ������ */

            /* �м�����ݷ��;����������ź� */
            aw_i2c_mktrans(&trans[1 + i],
                           p_this->addr,
                           AW_I2C_M_7BIT | AW_I2C_M_WR | AW_I2C_M_NOSTART,
                           p_buf,
                           info_len);

            for (j = 0; j < info_len; j++) {
                bcc ^= p_buf[j];
            }
        }
    }

    bcc = ~bcc;  /* �����ֵȡ������ΪУ��� */

    /* ���ڷ���У��ͺ�֡������ */
    temp[4] = bcc;
    temp[5] = 0x03;

    /* ���һ�η�����Ҫֹͣ */
    aw_i2c_mktrans(&trans[1 + i],
                   p_this->addr,
                   AW_I2C_M_7BIT | AW_I2C_M_WR | AW_I2C_M_NOSTART,
                   (uint8_t *)&temp[4],
                   2);

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2 + i,
                 __zlg600a_i2c_trans_callback,
                 p_this);

    aw_i2c_async(AWBL_I2C_PARENT_BUSID_GET(p_this), &msg);

    AW_SEMB_TAKE(p_this->semb, aw_ms_to_ticks(p_devinfo->timeout));

    return msg.status;
}

/* ���һ��I2C���ݴ��� */
aw_local aw_err_t __zlg600a_i2c_old_trans_data (awbl_zlg600a_i2c_dev_t *p_dev,
                                                uint16_t                flags,
                                                uint8_t                *p_buf,
                                                uint32_t                nbytes)
{
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    aw_i2c_transfer_t trans;
    aw_i2c_message_t  msg;

    aw_i2c_mktrans(&trans,
                   p_dev->addr,
                   flags,
                   p_buf,
                   nbytes);

    aw_i2c_mkmsg(&msg,
                 &trans,
                 1,
                 __zlg600a_i2c_trans_callback,
                 p_dev);

    aw_i2c_async(AWBL_I2C_PARENT_BUSID_GET(p_dev), &msg);

    AW_SEMB_TAKE(p_dev->semb, aw_ms_to_ticks(p_devinfo->timeout));

    return (msg.status);
}


/**
 * \brief ����һ���ɻ�Ӧ֡
 */
aw_local aw_err_t __zlg600a_i2c_frame_old_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_cookie);

    int      i,j;
    uint8_t  info_len;
    uint8_t *p_buf;

    aw_err_t ret;
    uint8_t  temp[6];
    uint8_t  bcc = 0;
    uint8_t  remain_len;
    uint8_t  discard_data;

    /* �ȴ����Խ��� */
    ret= __zlg600a_wait_can_recv(p_this);

    /* ����֡���ͣ�������ջ�Ӧ֡ */
    if (p_frame->cmd_status == 'K' && p_frame->cmd_class == 0x01) {
        return AW_OK;
    }

    if (ret != AW_OK) {
        return ret;
    }

    /* ����֡�� + �������� + ״̬ + ��Ϣ���� ������4���ֽڣ�*/
    ret = __zlg600a_i2c_old_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       (uint8_t *)temp,
                                       4);
    if (ret != AW_OK) {
        return ret;
    }
#if 0 
    bcc ^= p_frame->frame_len;
    bcc ^= p_frame->cmd_type;
    bcc ^= p_frame->cmd_status;
    bcc ^= p_frame->info_len;
#endif 
    bcc ^= temp[0];
    bcc ^= temp[1];
    bcc ^= temp[2];
    bcc ^= temp[3];

    p_frame->cmd_status  = temp[2];
    p_frame->info_length = temp[3];

    remain_len = temp[3];      /* ʣ���������Ϣ���ַ����� */

    /* ���ո�����Ϣ�������е����� */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* ��ȡ���ֽ������ܴ���ʣ���ֽ� */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;

        if (info_len != 0) {                       /* ������Ϣ������� */

            if (p_buf == NULL) {                   /* ���յ����ݶ���   */

                while (info_len != 0) {

                    ret = __zlg600a_i2c_old_trans_data(p_this,
                                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                                       &discard_data,
                                                       1);

                    if (ret != AW_OK) {
                        return ret;
                    }

                    info_len--;
                    bcc ^= discard_data;
                }
            } else {

                ret = __zlg600a_i2c_old_trans_data(p_this,
                                                   AW_I2C_M_7BIT | AW_I2C_M_RD,
                                                   p_buf,
                                                   info_len);

                if (ret != AW_OK) {
                    return ret;
                }

                for (j = 0; j < info_len; j++) {
                    bcc ^= p_buf[j];
                }
            }
        }
    }

    /* δ���յ���Ϣ���� */
    while (remain_len != 0) {

        ret = __zlg600a_i2c_old_trans_data(p_this,
                                           AW_I2C_M_7BIT | AW_I2C_M_RD,
                                           &discard_data,
                                           1);

        if (ret != AW_OK) {
            return ret;
        }
        remain_len--;
        bcc ^= discard_data;
    }

    /* ���һ�ζ�ȡ��Ҫֹͣ */
    ret = __zlg600a_i2c_old_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       &temp[4],
                                       2);

    if (ret != AW_OK) {
        return ret;
    }

    bcc = ~bcc;

    if (bcc != temp[4]) {
        return -AW_EIO;
    }

    if (temp[1] != (p_frame->cmd_class | (p_frame->frame_seq << 4))) {
        return -AW_EIO;
    }

    if (temp[2] != 0x00) {
        return -AW_EIO;
    }

    return AW_OK;
}

/******************************************************************************/

/* ���һ��I2C���ݴ��� */
aw_local aw_err_t __zlg600a_i2c_new_trans_data (
                      awbl_zlg600a_i2c_dev_t *p_dev,
                      uint16_t                flags,
                      uint16_t                start_addr,
                      uint8_t                *p_buf,
                      uint32_t                nbytes)
{
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    aw_i2c_transfer_t trans[2];
    aw_i2c_message_t  msg;
    uint8_t           subaddr[2];

    subaddr[0] = (uint8_t)(start_addr >> 8);
    subaddr[1] = (uint8_t)(start_addr & 0xFF);

    aw_i2c_mktrans(&trans[0],
                   p_dev->addr,
                   AW_I2C_M_7BIT | AW_I2C_M_WR,
                   subaddr,
                   2);

    aw_i2c_mktrans(&trans[1],
                   p_dev->addr,
                   flags,
                   p_buf,
                   nbytes);

    aw_i2c_mkmsg(&msg,
                 &trans[0],
                 2,
                 __zlg600a_i2c_trans_callback,
                 p_dev);

    aw_i2c_async(AWBL_I2C_PARENT_BUSID_GET(p_dev), &msg);

    AW_SEMB_TAKE(p_dev->semb, aw_ms_to_ticks(p_devinfo->timeout));

    return (msg.status);
}

/**
 * \brief ����һ��������֡
 */
aw_local aw_err_t __zlg600a_i2c_frame_new_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_cookie);

    int i, j;
    int ret;

    uint16_t  start_addr = 0x0102;
    uint16_t  sum = 0;

    uint8_t   *p_buf;
    uint16_t   info_len;
    uint8_t    stat = 0x8D;
    uint8_t    temp[8];

    /* �����������֡���ܳ��� */
    info_len = p_frame->info_length + 10;

    /* д������֡���� */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&info_len,
                                       2);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr += 2;

    temp[0] = p_this->addr << 1;
    temp[1] = 0x00;
    temp[2] = p_frame->frame_seq;
    temp[3] = p_frame->cmd_class;

    /* ���͵�ַ(1) + ��������(1) + ��ȫ����/����(1) + �������� ������4���ֽڣ�*/
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)temp,
                                       4);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr += 4;

    temp[4] = p_frame->cmd_status & 0xFF;
    temp[5] = (p_frame->cmd_status >> 8) & 0xFF;
    temp[6] = p_frame->info_length & 0xFF;
    temp[7] = (p_frame->info_length >> 8) & 0xFF;

    /* ��������/״̬��(2) + ��Ϣ����(2)������4���ֽڣ�                        */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&temp[4],
                                       4);
    if (ret != AW_OK) {
        return ret;
    }

    for (i = 0; i < 8; i++) {
        sum += temp[i];
    }

    start_addr += 4;

    /* ���͸��������е���Ϣ */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;
        if (info_len != 0) {

            ret = __zlg600a_i2c_new_trans_data(p_this,
                                               AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                               start_addr,
                                               p_buf,
                                               info_len);
            if (ret != AW_OK) {
                return ret;
            }

            for (j = 0; j < info_len; j++) {
                sum += p_buf[j];
            }
        }

        start_addr += info_len;
    }

    sum = ~sum;  /* ���õ��ĸ������ݵĺ�ֵȡ�� */

    /* ����У��ͣ�2�ֽ� */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&sum,
                                       2);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr = 0x0101;

    /* �����������ƣ�1�ֽ� */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&stat,
                                       1);

    return ret;
}

/* ����һ���»�Ӧ֡ */
aw_local aw_err_t __zlg600a_i2c_frame_new_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_cookie);

    int       i,j;
    int       ret;
    uint8_t  *p_buf;
    uint16_t  info_len;
    uint16_t  remain_len;
    uint16_t  start_addr   = 0x0104;
    uint8_t   discard_data;
    uint16_t  sum = 0, frame_sum; /* ����ʱ�����ܴ������ݶ�����ֱ�ӱ߶���У�� */

    uint8_t temp[4];

    /* �ȴ����Խ��� */
    ret= __zlg600a_wait_can_recv(p_this);

    /* ����֡���ͣ�������ջ�Ӧ֡ */
    if (p_frame->cmd_status == 'K' && p_frame->cmd_class == 0x01) {
        return AW_OK;
    }

    if (ret != AW_OK) {
        return ret;
    }

    /* ���յ�ַ(1) + ��������(1) + ��ȫ����/����(1) + �������� ������4���ֽڣ�*/
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       start_addr,
                                       (uint8_t *)temp,
                                       4);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr += 4;

    sum += temp[0];
    sum += temp[1];
    sum += temp[2];
    sum += temp[3];

    /* ��������/״̬��(2) + ��Ϣ����(2)������4���ֽڣ� */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       start_addr,
                                       (uint8_t *)(&p_frame->cmd_status),
                                       4);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr += 4;

    sum += p_frame->cmd_status & 0xFF;            /* ���ֽ� */
    sum += p_frame->cmd_status >> 8;              /* ���ֽ� */

    sum += p_frame->info_length & 0xFF;           /* ���ֽ� */
    sum += p_frame->info_length >> 8;             /* ���ֽ� */

    remain_len = p_frame->info_length;            /* ʣ���������Ϣ���ַ����� */

    /* ���ո�����Ϣ�������е����� */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* ��ȡ���ֽ������ܴ���ʣ���ֽ� */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;

        if (info_len != 0) {                       /* ������Ϣ������� */

            if (p_buf == NULL) {

                while (info_len != 0) {
                    ret = __zlg600a_i2c_new_trans_data(p_this,
                                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                                       start_addr,
                                                       &discard_data,
                                                       1);
                    if (ret != AW_OK) {
                        return ret;
                    }
                    info_len--;
                    start_addr++;
                    sum += discard_data;
                }
            } else {

                ret = __zlg600a_i2c_new_trans_data(p_this,
                                                   AW_I2C_M_7BIT | AW_I2C_M_RD,
                                                   start_addr,
                                                   p_buf,
                                                   info_len);
                if (ret != AW_OK) {
                    return ret;
                }

                for (j = 0; j < info_len; j++) {
                    sum += p_buf[j];
                }

                start_addr += info_len;
            }
        }
    }

    /* δ���յ���Ϣ���� */
    while (remain_len != 0) {

        ret = __zlg600a_i2c_new_trans_data(p_this,
                                           AW_I2C_M_7BIT | AW_I2C_M_RD,
                                           start_addr,
                                           &discard_data,
                                           1);

        if (ret != AW_OK) {
            return ret;
        }
        remain_len--;
        start_addr++;
        sum += discard_data;
    }

    sum = ~sum;

    /* ����У��� */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       start_addr,
                                       (uint8_t *)&frame_sum,
                                       2);
    if (ret != AW_OK) {
        return ret;
    }

    if (sum != frame_sum) {
        return -AW_EIO;
    }

    /* ״̬��Ϊ0x00����ʾ����ִ��ʧ��               */
    if (p_frame->cmd_status != 0x00) {
        return -AW_EIO;
    }

    /* ��Ӧ֡������֡�� �������� ��ͬ����ʾ���ִ��� */
    if (p_frame->cmd_class != temp[3]) {
        return -AW_EIO;
    }

    /* ��Ӧ֡������֡�� ���� ��ͬ����ʾ���ִ���     */
    if (p_frame->frame_seq != temp[2]) {
        return -AW_EIO;
    }

    return AW_OK;
}

/** \brief I2C�豸���ƺ��� */
aw_local aw_err_t __zlg600a_i2c_device_ioctl (void *p_cookie, 
                                              int   request, 
                                              void *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_cookie);

    aw_err_t ret;

    switch (request) {

    case AWBL_ZLG600A_DEV_IOCTL_BAUD_SET:
        ret = AW_OK;
        break;

    case AWBL_ZLG600A_DEV_IOCTL_ADDR_SET:
        p_this->addr = ((int)p_arg) >> 1;
        ret = AW_OK;
        break;

    case AWBL_ZLG600A_DEV_IOCTL_FRAME_FMT_SET:
        if ((int)p_arg == AWBL_ZLG600A_FRAME_FMT_NEW) {
            p_this->serv.p_servopts = &__g_zlg600a_i2c_new_servopts;
        }

        if ((int)p_arg == AWBL_ZLG600A_FRAME_FMT_OLD) {
            p_this->serv.p_servopts = &__g_zlg600a_i2c_old_servopts;
        }
        ret = AW_OK;
        break;

    default :
        ret = -AW_ENOTSUP;
        break;
    }

    return ret;
}

/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_i2c_inst_init1 (awbl_dev_t *p_dev)
{
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_i2c_inst_init2 (awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __zlg600a_i2c_inst_connect (awbl_dev_t *p_dev)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_dev);
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    /* platform initialization */
    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    p_this->serv.frame_seq = 0;
    p_this->addr           = p_devinfo->addr;

    /* ��ʼ���ź��� */
    AW_SEMB_INIT(p_this->semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* ������������ʹ�� */
    aw_gpio_pin_cfg(p_devinfo->pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);

    /* ����GPIO�����ж� */
    aw_gpio_trigger_connect(p_devinfo->pin,
                            __zlg600a_pin_int_callback,
                            p_dev);

    /* �½��ش��� */
    aw_gpio_trigger_cfg(p_devinfo->pin, AW_GPIO_TRIGGER_FALL);

    /* �رմ��� */
    aw_gpio_trigger_off(p_devinfo->pin);

    return;
}

/**
 * \brief method "awbl_zlg600a_i2cserv_get" handler
 */
aw_local aw_err_t __zlg600a_i2c_zlg600aserv_get (struct awbl_dev *p_dev, 
                                                 void            *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_dev);
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_zlg600a_service *p_serv = NULL;

    /* get intcltr service instance */
    p_serv = &p_this->serv;

    /* initialize the ZLG600A service instance */
    p_serv->p_next     = NULL;
    p_serv->p_servinfo = &p_devinfo->zlg600a_servinfo;

    if (p_devinfo->frame_fmt == AWBL_ZLG600A_FRAME_FMT_NEW) {
        p_serv->p_servopts = &__g_zlg600a_i2c_new_servopts;
    } else {
        p_serv->p_servopts = &__g_zlg600a_i2c_old_servopts;
    }

    p_serv->p_cookie   = (void *)p_dev;

    /* send back the service instance */
    *(struct awbl_zlg600a_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief ZLG600A I2C driver register
 */
void awbl_zlg600a_i2c_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_zlg600a_i2c_drv_registration);
}

/* end of file */

