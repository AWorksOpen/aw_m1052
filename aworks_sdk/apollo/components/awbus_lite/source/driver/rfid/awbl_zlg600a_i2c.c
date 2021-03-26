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
 * 使用本模块需要包含以下头文件：
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

/** \brief 发送一个旧帧命令 */
aw_local aw_err_t __zlg600a_i2c_frame_old_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 接收一个旧回应帧 */
aw_local aw_err_t __zlg600a_i2c_frame_old_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief 发送一个新命令帧 */
aw_local aw_err_t __zlg600a_i2c_frame_new_cmd_send (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/* 接收一个新回应帧 */
aw_local aw_err_t __zlg600a_i2c_frame_new_respond_recv (
                      void                      *p_cookie,
                      awbl_zlg600a_frame_base_t *p_frame);

/** \brief I2C设备控制函数 */
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
 * \brief GPIO回调函数
 * \param[in] p_arg : 回调函数入口参数
 * \return 无
 */
aw_local void __zlg600a_pin_int_callback (void *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_arg);

    AW_SEMB_GIVE(p_this->semb);
}

/* I2C回调函数 */
aw_local void __zlg600a_i2c_trans_callback (void *p_arg)
{
    __ZLG600A_I2C_DEV_DECL(p_this, p_arg);

    AW_SEMB_GIVE(p_this->semb);
}

/* 等待可以接收数据 */
aw_local aw_err_t __zlg600a_wait_can_recv (awbl_zlg600a_i2c_dev_t *p_dev)
{
    __ZLG600A_I2C_DEVINFO_DECL(p_devinfo, p_dev);

    /* 已经是低电平 */
    if (aw_gpio_get(p_devinfo->pin) == 0) {
        return AW_OK;
    }

    aw_gpio_trigger_on(p_devinfo->pin);

    return AW_SEMB_TAKE(p_dev->semb,
                        aw_ms_to_ticks(p_devinfo->timeout));
}

/**
 * \brief 发送一个旧帧命令
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

    /* 发送各个信息缓冲区中的信息 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;
        if (info_len != 0) {                        /* 存在信息，则发送 */

            /* 中间的数据发送均无需启动信号 */
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

    bcc = ~bcc;  /* 将异或值取反后作为校验和 */

    /* 便于发送校验和和帧结束符 */
    temp[4] = bcc;
    temp[5] = 0x03;

    /* 最后一次发送需要停止 */
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

/* 完成一次I2C数据传输 */
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
 * \brief 接收一个旧回应帧
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

    /* 等待可以接收 */
    ret= __zlg600a_wait_can_recv(p_this);

    /* 设置帧类型，无需接收回应帧 */
    if (p_frame->cmd_status == 'K' && p_frame->cmd_class == 0x01) {
        return AW_OK;
    }

    if (ret != AW_OK) {
        return ret;
    }

    /* 接收帧长 + 命令类型 + 状态 + 信息长度 （共计4个字节）*/
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

    remain_len = temp[3];      /* 剩余待接收信息的字符总数 */

    /* 接收各个信息缓冲区中的数据 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* 读取的字节数不能大于剩余字节 */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;

        if (info_len != 0) {                       /* 存在信息，则接收 */

            if (p_buf == NULL) {                   /* 接收的数据丢弃   */

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

    /* 未接收的信息丢弃 */
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

    /* 最后一次读取需要停止 */
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

/* 完成一次I2C数据传输 */
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
 * \brief 发送一个新命令帧
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

    /* 首先求得命令帧的总长度 */
    info_len = p_frame->info_length + 10;

    /* 写入命令帧长度 */
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

    /* 发送地址(1) + 卡槽索引(1) + 安全报文/包号(1) + 命令类型 （共计4个字节）*/
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

    /* 发送命令/状态码(2) + 信息长度(2)（共计4个字节）                        */
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

    /* 发送各缓冲区中的信息 */
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

    sum = ~sum;  /* 将得到的各个数据的和值取反 */

    /* 发送校验和，2字节 */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&sum,
                                       2);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr = 0x0101;

    /* 发送主机控制，1字节 */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_NOSTART,
                                       start_addr,
                                       (uint8_t *)&stat,
                                       1);

    return ret;
}

/* 接收一个新回应帧 */
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
    uint16_t  sum = 0, frame_sum; /* 接收时，可能存在数据丢弃，直接边读边校验 */

    uint8_t temp[4];

    /* 等待可以接收 */
    ret= __zlg600a_wait_can_recv(p_this);

    /* 设置帧类型，无需接收回应帧 */
    if (p_frame->cmd_status == 'K' && p_frame->cmd_class == 0x01) {
        return AW_OK;
    }

    if (ret != AW_OK) {
        return ret;
    }

    /* 接收地址(1) + 卡槽索引(1) + 安全报文/包号(1) + 命令类型 （共计4个字节）*/
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

    /* 接收命令/状态码(2) + 信息长度(2)（共计4个字节） */
    ret = __zlg600a_i2c_new_trans_data(p_this,
                                       AW_I2C_M_7BIT | AW_I2C_M_RD,
                                       start_addr,
                                       (uint8_t *)(&p_frame->cmd_status),
                                       4);
    if (ret != AW_OK) {
        return ret;
    }

    start_addr += 4;

    sum += p_frame->cmd_status & 0xFF;            /* 低字节 */
    sum += p_frame->cmd_status >> 8;              /* 高字节 */

    sum += p_frame->info_length & 0xFF;           /* 低字节 */
    sum += p_frame->info_length >> 8;             /* 低字节 */

    remain_len = p_frame->info_length;            /* 剩余待接收信息的字符总数 */

    /* 接收各个信息缓冲区中的数据 */
    for (i = 0; i < p_frame->buf_num; i++) {
        p_buf     = p_frame->p_info_desc[i].p_buf;
        info_len  = p_frame->p_info_desc[i].len;

        /* 读取的字节数不能大于剩余字节 */
        info_len = info_len > remain_len ? remain_len : info_len;

        remain_len -= info_len;

        p_frame->p_info_desc[i].len = info_len;

        if (info_len != 0) {                       /* 存在信息，则接收 */

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

    /* 未接收的信息丢弃 */
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

    /* 接收校验和 */
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

    /* 状态不为0x00，表示命令执行失败               */
    if (p_frame->cmd_status != 0x00) {
        return -AW_EIO;
    }

    /* 回应帧和命令帧的 命令类型 不同，表示出现错误 */
    if (p_frame->cmd_class != temp[3]) {
        return -AW_EIO;
    }

    /* 回应帧和命令帧的 包号 不同，表示出现错误     */
    if (p_frame->frame_seq != temp[2]) {
        return -AW_EIO;
    }

    return AW_OK;
}

/** \brief I2C设备控制函数 */
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

    /* 初始化信号量 */
    AW_SEMB_INIT(p_this->semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 配置引脚上拉使能 */
    aw_gpio_pin_cfg(p_devinfo->pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);

    /* 连接GPIO引脚中断 */
    aw_gpio_trigger_connect(p_devinfo->pin,
                            __zlg600a_pin_int_callback,
                            p_dev);

    /* 下降沿触发 */
    aw_gpio_trigger_cfg(p_devinfo->pin, AW_GPIO_TRIGGER_FALL);

    /* 关闭触发 */
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

