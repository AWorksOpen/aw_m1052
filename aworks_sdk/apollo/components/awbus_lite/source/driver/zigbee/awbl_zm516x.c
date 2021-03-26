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
 * \brief zigbee 模块 ZM516X 驱动
 *
 * \internal
 * \par modification history
 * - 1.02 18-03-30  pea, optimize API
 * - 1.01 18-01-22  pea, update API to v1.03 manual
 * - 1.00 14-11-05  afm, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "aworks.h"
#include "aw_gpio.h"
#include "aw_ioctl.h"
#include "aw_delay.h"
#include "aw_errno.h"
#include "aw_common.h"
#include "aw_vdebug.h"
#include "aw_serial.h"
#include "aw_compiler.h"
#include "string.h"
#include "driver/zigbee/awbl_zm516x.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/** \brief 通过设备号获取设备实例 */
#define __ID_GET_ZM516X_DEV(id)   \
            (struct awbl_zm516x_dev *) \
            awbl_dev_find_by_name(AWBL_ZM516X_DRV_NAME, id)

/** \brief 通过设备实例获取设备信息 */
#define __ZM516X_DEVINFO_GET(p_dev)   \
            (struct awbl_zm516x_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __ZM516X_DEF_TAG    "\xAB\xBC\xCD"    /**< \brief 永久命令帧帧头 */
#define __ZM516X_CUR_TAG    "\xDE\xDF\xEF"    /**< \brief 临时命令帧帧头 */

#define __ZM516X_DEF_END    0xAA              /**< \brief 永久命令帧帧尾 */

#define __ZM516X_CMD_BUF_LEN       100   /**< \brief 命令缓存长度 */
#define __ZM516X_RSP_BUF_LEN       100   /**< \brief 应答缓存长度 */
#define __ZM516X_CMD_SPACE_TIME    5     /**< \brief 命令帧间隔时间，单位：ms */

/** \brief 永久参数配置命令 */
enum __ZM516X_DEF_CMD {
    ZM516X_DEF_CONFIG_GET       = 0xD1,    /**< \brief 读取本地配置 */
    ZM516X_DEF_CHAN_SET         = 0xD2,    /**< \brief 设置通道号 */
    ZM516X_DEF_DISCOVER         = 0xD4,    /**< \brief 搜索 */
    ZM516X_DEF_REMOTE_GET       = 0xD5,    /**< \brief 获取远程配置信息 */
    
    /** \brief 修改配置，设置成功需复位 */
    ZM516X_DEF_CONFIG_SET       = 0xD6,    
    ZM516X_DEF_RESET            = 0xD9,    /**< \brief 复位 */
    
    /** \brief 恢复出厂设置，设置成功需复位 */
    ZM516X_DEF_REDEFAULT        = 0xDA,    
    
    /** \brief 包头显示源地址，设置成功需复位 */
    ZM516X_DEF_SRC_DIS          = 0xDC,    
    
    /** \brief 设置/获取 GPIO 方向，设置成功需复位 */
    ZM516X_DEF_GPIO_DIR         = 0xE1,    
    
    /** \brief IO/AD 采集设置/获取，设置成功需复位 */
    ZM516X_DEF_IO_ADC_UPLOAD    = 0xE2,    
    ZM516X_DEF_GPIO_STATE       = 0xE3,    /**< \brief 设置/获取 GPIO 电平 */
    ZM516X_DEF_PWM              = 0xE4,    /**< \brief PWM 控制输出 */
    
    /** \brief 设置自组网功能，设置成功需复位 */
    ZM516X_DEF_NETWORK_SET      = 0xE5,    
    ZM516X_DEF_JOIN_ENABLE      = 0xE6,    /**< \brief 主机允许从机加入网络 */
    
    /** \brief 查询主机模块存储的从机信息 */
    ZM516X_DEF_SLAVE_GET        = 0xE7,    
    ZM516X_DEF_STATE_GET        = 0xE8,    /**< \brief 查询主从机状态 */
};

/** \brief 临时参数配置命令 */
enum __ZM516X_CUR_CMD {
    ZM516X_CUR_CHAN_SET         = 0xD1,    /**< \brief 修改通道号 */
    ZM516X_CUR_DEST_SET         = 0xD2,    /**< \brief 修改目的网络地址 */
    ZM516X_CUR_SRC_DIS          = 0xD3,    /**< \brief 包头显示源地址 */
    ZM516X_CUR_GPIO_DIR_SET     = 0xD4,    /**< \brief 设置 GPIO 方向 */
    ZM516X_CUR_GPIO_GET         = 0xD5,    /**< \brief 获取 GPIO 电平 */
    ZM516X_CUR_GPIO_SET         = 0xD6,    /**< \brief 设置 GPIO 电平 */
    ZM516X_CUR_ADC_GET          = 0xD7,    /**< \brief 获取 ADC 数值 */
    ZM516X_CUR_DEEP_SLEEP       = 0xD8,    /**< \brief 进入深度休眠模式 */
    ZM516X_CUR_SEND_MODE_SET    = 0xD9,    /**< \brief 设置发送模式 */
    ZM516X_CUR_CSQ_GET          = 0xDA,    /**< \brief 获取信号强度 */
};

/** \brief 永久参数配置命令应答 */
enum __ZM516X_DEF_ERR {
    ZM516X_DEF_OK              = 0x00,    /**< \brief 操作成功 */
    ZM516X_DEF_LENGTH_FAUSE    = 0x01,    /**< \brief 帧长度错误 */
    ZM516X_DEF_ADDRESS_FAUSE   = 0x02,    /**< \brief 地址错误 */
    ZM516X_DEF_CHECK_FAUSE     = 0x03,    /**< \brief 帧字节校验错误 */
    ZM516X_DEF_WRITE_FAUSE     = 0x04,    /**< \brief 写错误 */
    ZM516X_DEF_OTHER_FAUSE     = 0x05,    /**< \brief 其它错误 */
};

/** \brief 临时参数配置命令应答 */
enum __ZM516X_CUR_ERR {
    ZM516X_CUR_OK              = 0x00,    /**< \brief 操作成功 */
};

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/** \brief 第三阶段初始化 */
aw_local void __zm516x_inst_connect (struct awbl_dev *p_this);

/** \brief ACK 引脚中断服务函数 */
aw_local void __ack_pint_isr (void *p_arg);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_zm516x = {
    NULL,                    /* 第一阶段初始化 */
    NULL,                    /* 第二阶段初始化 */
    __zm516x_inst_connect    /* 第三阶段初始化 */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_zm516x = {
    AWBL_VER_1,                   /* AWBus 版本号 */
    AWBL_BUSID_PLB,               /* 总线 ID */
    AWBL_ZM516X_DRV_NAME,         /* 驱动名 */
    &__g_awbl_drvfuncs_zm516x,    /* 驱动入口点 */
    NULL,                         /* 驱动提供的方法 */
    NULL                          /* 驱动探测函数 */
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 第三阶段初始化 */
aw_local void __zm516x_inst_connect (struct awbl_dev *p_dev)
{
    struct awbl_zm516x_dev     *p_this    = (struct awbl_zm516x_dev *)p_dev;
    struct awbl_zm516x_devinfo *p_devinfo = __ZM516X_DEVINFO_GET(p_dev);

    if (p_devinfo->pfn_plfm_init != NULL) {
        (p_devinfo->pfn_plfm_init)();
    }

    aw_serial_ioctl(p_devinfo->com_id,
                    SIO_BAUD_SET,
                    (void *)p_devinfo->baudrate);
    aw_serial_ioctl(p_devinfo->com_id,
                    SIO_HW_OPTS_SET,
                    (void *)p_devinfo->serial_opts);
    aw_serial_ioctl(p_devinfo->com_id,
                    SIO_MODE_SET,
                    (void *)SIO_MODE_INT);
    aw_serial_ioctl(p_devinfo->com_id,
                    AW_TIOCRDTIMEOUT,
                    (void *)10);

    if (-1 != p_devinfo->rst_pin) {
        aw_gpio_pin_cfg(p_devinfo->rst_pin, AW_GPIO_OUTPUT_INIT_HIGH);
        aw_gpio_set(p_devinfo->rst_pin, 0);
        aw_mdelay(10);
        aw_gpio_set(p_devinfo->rst_pin, 1);
        aw_mdelay(10);
    }

    if (-1 != p_devinfo->ack_pin) {
        AW_SEMB_INIT(p_this->ack_wait, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

        aw_gpio_pin_cfg(p_devinfo->ack_pin, AW_GPIO_INPUT | AW_GPIO_PULL_DOWN);
        aw_gpio_trigger_connect(p_devinfo->ack_pin,
                                __ack_pint_isr,
                                (void *)p_dev);
        if (aw_gpio_trigger_cfg(p_devinfo->ack_pin,
                                AW_GPIO_TRIGGER_RISE) != AW_OK) {
            aw_gpio_trigger_cfg(p_devinfo->ack_pin, AW_GPIO_TRIGGER_FALL);
        }
        aw_gpio_trigger_on(p_devinfo->ack_pin);
    }
}

/** \brief ACK 引脚中断服务函数 */
aw_local void __ack_pint_isr (void *p_arg)
{
    struct awbl_zm516x_dev *p_this = (struct awbl_zm516x_dev *)p_arg;

    AW_SEMB_GIVE(p_this->ack_wait);
}

/** \brief 将串口接收到的数据转换为 awbl_zm516x_base_info_t 结构 */
aw_local void __bin_to_base_info (void *p_bin, awbl_zm516x_base_info_t *p_base)
{
    uint8_t *p_data = (uint8_t *)p_bin;

    if ((NULL == p_bin) || (NULL == p_base)) {
        return;
    }

    AW_BITS_SET(p_base->software, 8, 8, p_data[0]);
    AW_BITS_SET(p_base->software, 0, 8, p_data[1]);

    p_base->chan      = p_data[2];
    p_base->data_rate = p_data[3];

    AW_BITS_SET(p_base->panid, 8, 8, p_data[4]);
    AW_BITS_SET(p_base->panid, 0, 8, p_data[5]);

    AW_BITS_SET(p_base->addr, 8, 8, p_data[6]);
    AW_BITS_SET(p_base->addr, 0, 8, p_data[7]);

    p_base->state = p_data[7];
}

/** \brief 将串口接收到的数据转换为 awbl_zm516x_cfg_info_t 结构 */
aw_local void __bin_to_cfg_info (void *p_bin, awbl_zm516x_cfg_info_t *p_cfg)
{
    uint8_t *p_data = (uint8_t *)p_bin;

    if ((NULL == p_bin) || (NULL == p_cfg)) {
        return;
    }

    memcpy(p_cfg->dev_name, &p_data[0],  16);

    memcpy(p_cfg->dev_pwd, &p_data[16], 16);

    p_cfg->dev_mode = p_data[32];
    p_cfg->chan     = p_data[33];

    AW_BITS_SET(p_cfg->panid, 8, 8, p_data[34]);
    AW_BITS_SET(p_cfg->panid, 0, 8, p_data[35]);

    AW_BITS_SET(p_cfg->my_addr, 8, 8, p_data[36]);
    AW_BITS_SET(p_cfg->my_addr, 0, 8, p_data[37]);

    p_cfg->my_mac[7] = p_data[38];
    p_cfg->my_mac[6] = p_data[39];
    p_cfg->my_mac[5] = p_data[40];
    p_cfg->my_mac[4] = p_data[41];
    p_cfg->my_mac[3] = p_data[42];
    p_cfg->my_mac[2] = p_data[43];
    p_cfg->my_mac[1] = p_data[44];
    p_cfg->my_mac[0] = p_data[45];

    AW_BITS_SET(p_cfg->dst_addr, 8, 8, p_data[46]);
    AW_BITS_SET(p_cfg->dst_addr, 0, 8, p_data[47]);

    p_cfg->dst_mac[7] = p_data[48];
    p_cfg->dst_mac[6] = p_data[49];
    p_cfg->dst_mac[5] = p_data[50];
    p_cfg->dst_mac[4] = p_data[51];
    p_cfg->dst_mac[3] = p_data[52];
    p_cfg->dst_mac[2] = p_data[53];
    p_cfg->dst_mac[1] = p_data[54];
    p_cfg->dst_mac[0] = p_data[55];

    p_cfg->power_level   = p_data[57];
    p_cfg->retry_num     = p_data[58];
    p_cfg->tran_timeout  = p_data[59];
    p_cfg->serial_rate   = p_data[60];
    p_cfg->serial_data   = p_data[61];
    p_cfg->serial_stop   = p_data[62];
    p_cfg->serial_parity = p_data[63];
    p_cfg->send_mode     = p_data[64];
    p_cfg->state         = p_data[65];

    AW_BITS_SET(p_cfg->software, 8, 8, p_data[66]);
    AW_BITS_SET(p_cfg->software, 0, 8, p_data[67]);

    AW_BITS_SET(p_cfg->version, 8, 8, p_data[68]);
    AW_BITS_SET(p_cfg->version, 0, 8, p_data[69]);
}

/** \brief 将 awbl_zm516x_cfg_info_t 结构转换为二进制数据，方便串口发送 */
aw_local void __cfg_info_to_bin (awbl_zm516x_cfg_info_t *p_cfg, void *p_bin)
{
    uint8_t *p_data = (uint8_t *)p_bin;

    if ((NULL == p_bin) || (NULL == p_cfg)) {
        return;
    }

    memcpy(&p_data[0], p_cfg->dev_name, 16);

    memcpy(&p_data[16], p_cfg->dev_pwd, 16);

    p_data[32] = p_cfg->dev_mode;
    p_data[33] = p_cfg->chan;

    p_data[34] = AW_BITS_GET(p_cfg->panid, 8, 8);
    p_data[35] = AW_BITS_GET(p_cfg->panid, 0, 8);

    p_data[36] = AW_BITS_GET(p_cfg->my_addr, 8, 8);
    p_data[37] = AW_BITS_GET(p_cfg->my_addr, 0, 8);

    p_data[38] = p_cfg->my_mac[7];
    p_data[39] = p_cfg->my_mac[6];
    p_data[40] = p_cfg->my_mac[5];
    p_data[41] = p_cfg->my_mac[4];
    p_data[42] = p_cfg->my_mac[3];
    p_data[43] = p_cfg->my_mac[2];
    p_data[44] = p_cfg->my_mac[1];
    p_data[45] = p_cfg->my_mac[0];

    p_data[46] = AW_BITS_GET(p_cfg->dst_addr, 8, 8);
    p_data[47] = AW_BITS_GET(p_cfg->dst_addr, 0, 8);

    p_data[48] = p_cfg->dst_mac[7];
    p_data[49] = p_cfg->dst_mac[6];
    p_data[50] = p_cfg->dst_mac[5];
    p_data[51] = p_cfg->dst_mac[4];
    p_data[52] = p_cfg->dst_mac[3];
    p_data[53] = p_cfg->dst_mac[2];
    p_data[54] = p_cfg->dst_mac[1];
    p_data[55] = p_cfg->dst_mac[0];

    p_data[57] = p_cfg->power_level;
    p_data[58] = p_cfg->retry_num;
    p_data[59] = p_cfg->tran_timeout;
    p_data[60] = p_cfg->serial_rate;
    p_data[61] = p_cfg->serial_data;
    p_data[62] = p_cfg->serial_stop;
    p_data[63] = p_cfg->serial_parity;
    p_data[64] = p_cfg->send_mode;
    p_data[65] = p_cfg->state;

    p_data[66] = AW_BITS_GET(p_cfg->software, 8, 8);
    p_data[67] = AW_BITS_GET(p_cfg->software, 0, 8);

    p_data[68] = AW_BITS_GET(p_cfg->version, 8, 8);
    p_data[69] = AW_BITS_GET(p_cfg->version, 0, 8);
}

/** \brief 获取 COM ID */
aw_local aw_err_t __com_id_get (int id)
{
    struct awbl_zm516x_dev     *p_this    = __ID_GET_ZM516X_DEV(id);
    struct awbl_zm516x_devinfo *p_devinfo = NULL;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    p_devinfo = __ZM516X_DEVINFO_GET(&p_this->dev);
    if (NULL == p_devinfo) {
        return -AW_ENODEV;
    }

    return (aw_err_t)p_devinfo->com_id;
}

/** \brief 发送 ZM516X 命令 */
aw_local aw_bool_t __zm516x_send_cmd (int       com_id,
                                   uint8_t  *p_cmd,
                                   uint16_t  cmd_len,
                                   uint8_t  *p_rsp,
                                   uint16_t *p_rsp_len,
                                   uint16_t  timeout)
{
    aw_bool_t   flag  = AW_FALSE;
    uint16_t i     = 0;
    int32_t  len   = 0;
    int32_t  index = 0;

    aw_serial_write(com_id, (char *)p_cmd, cmd_len);

    if (NULL == p_rsp) {
        return AW_TRUE; /* 应答无返回值，直接返回 AW_TRUE */
    }

    aw_serial_ioctl(com_id, AW_FIORFLUSH, NULL);

    for (i = 0; i < timeout; i++) {
        len = aw_serial_read(com_id, (char *)p_rsp, __ZM516X_RSP_BUF_LEN);
        if (len > 0) {
            index = len;
            flag = AW_TRUE;
            break;
        }
    }
    if ( !flag ) {
        return AW_FALSE;
    }

    /* 应答帧间隔为 100ms，应答完成 */
    for (i = 0; i < 10; i++) {
        len = aw_serial_read(com_id,
                             (char *)&p_rsp[index],
                             __ZM516X_RSP_BUF_LEN - index);
        if (len > 0) {
            index += len;
            i = 0;
            continue;
        }
    }

    *p_rsp_len = index;

    return AW_TRUE;
}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 ZM516X 驱动注册到 AWBus 子系统中
 *
 * \note 本函数中禁止调用操作系统 API
 */
void awbl_zm516x_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_zm516x);
}

/*******************************************************************************
  配置命令
*******************************************************************************/

/** \brief 复位指定地址的 ZM516X 模块（RST 引脚或永久命令：D9） */
aw_err_t awbl_zm516x_reset (int id, uint16_t dst_addr)
{
    awbl_zm516x_cfg_info_t      cfg_info;
    uint8_t                     cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t                     rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t                    rsp_len   = 0;
    aw_err_t                    err       = AW_OK;
    struct awbl_zm516x_dev     *p_this    = __ID_GET_ZM516X_DEV(id);
    struct awbl_zm516x_devinfo *p_devinfo = NULL;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    p_devinfo = __ZM516X_DEVINFO_GET(&p_this->dev);
    if (NULL == p_devinfo) {
        return -AW_ENODEV;
    }

    /* 获取本地模块配置 */
    if ((err = awbl_zm516x_cfg_info_get(id, AW_TRUE, 0, &cfg_info)) != AW_OK) {
        return err;
    }

    /* 如果是复位本地模块，优先使用 RST 引脚复位 */
    if ((cfg_info.my_addr == dst_addr) && (-1 != p_devinfo->rst_pin)) {
        aw_gpio_set(p_devinfo->rst_pin, 0);
        aw_mdelay(10);
        aw_gpio_set(p_devinfo->rst_pin, 1);
        aw_mdelay(10);
        return AW_OK;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_RESET;
    if (cfg_info.my_addr == dst_addr) {
        cmd_buf[4] = AW_BITS_GET(cfg_info.my_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(cfg_info.my_addr, 0, 8);
    } else {
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    }
    cmd_buf[6] = 0x00;
    cmd_buf[7] = 0x01;
    cmd_buf[8] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(p_devinfo->com_id,
                          cmd_buf,
                          9,
                          NULL,
                         &rsp_len,
                          30) ) {
        err = AW_OK;
    } else {
        err = AW_ERROR;
    }
    aw_mdelay(10);

    return err;
}

/** \brief 恢复指定地址的 ZM516X 模块为出厂设置（永久命令：DA） */
aw_err_t awbl_zm516x_default_set (int id, uint16_t dst_addr)
{
    awbl_zm516x_cfg_info_t cfg_info;
    uint8_t                cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t                rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t               rsp_len = 0;
    aw_err_t               err     = AW_OK;
    int                    com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_REDEFAULT;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 0x00;
    cmd_buf[7] = 0x01;
    cmd_buf[8] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          9,
                          rsp_buf,
                         &rsp_len,
                          80) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[8] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        /* 复位生效 */
        return awbl_zm516x_reset(id, dst_addr);
    }
    return -AW_EPERM;
}

/** \brief 获取指定地址的 ZM516X 模块的配置信息（永久命令：D1 或 D5） */
aw_err_t awbl_zm516x_cfg_info_get (int                     id,
                                   aw_bool_t               is_local,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_cfg_info) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    if (is_local ) {
        cmd_buf[3] = ZM516X_DEF_CONFIG_GET;
        cmd_buf[4] = __ZM516X_DEF_END;
    } else {
        cmd_buf[3] = ZM516X_DEF_REMOTE_GET;
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
        cmd_buf[6] = __ZM516X_DEF_END;
    }

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          (is_local ) ? 5 : 7,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }

        /* 将串口接收到的数据转换为 awbl_zm516x_cfg_info_t 结构 */
        __bin_to_cfg_info(&rsp_buf[4], p_cfg_info);
        return AW_OK;
    }
    return -AW_EPERM;
}


/** \brief 修改指定地址的 ZM516X 模块的配置信息（永久命令：D6），设置成功需复位 */
aw_err_t awbl_zm516x_cfg_info_set (int                     id,
                                   uint16_t                dst_addr,
                                   awbl_zm516x_cfg_info_t *p_cfg_info)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_cfg_info) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_CONFIG_SET;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    __cfg_info_to_bin(p_cfg_info, &cmd_buf[6]);
    cmd_buf[6 + 65] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          7 + 65,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[6] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        /* 复位生效 */
        return awbl_zm516x_reset(id, dst_addr);
    }
    return -AW_EPERM;
}

/** \brief 设置 ZM516X 模块通道号（临时命令：D1 或者永久命令：D2） */
aw_err_t awbl_zm516x_chan_set (int id, uint8_t chan, aw_bool_t is_temp)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (is_temp) {
        memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
        cmd_buf[3] = ZM516X_CUR_CHAN_SET;
        cmd_buf[4] = chan;
    } else {
        memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
        cmd_buf[3] = ZM516X_DEF_CHAN_SET;
        cmd_buf[4] = chan;
        cmd_buf[5] = __ZM516X_DEF_END;
    }

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          is_temp ? 5 : 6,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if ((rsp_buf[4] != ZM516X_CUR_OK) &&
            (rsp_buf[4] != ZM516X_DEF_OK)) {
            return -AW_EPERM;
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/**
 * \brief 设置 ZM516X 模块接收的数据包包头是否显示源地址（临时命令：D3 
 * 或者永久命令：DC） 
 */
aw_err_t awbl_zm516x_display_head_set (int       id, 
                                       aw_bool_t is_show, 
                                       aw_bool_t is_temp)
{
    awbl_zm516x_cfg_info_t cfg_info;
    uint8_t                cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t                rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t               rsp_len = 0;
    aw_err_t               err     = AW_OK;
    int                    com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    /* 获取本地模块配置 */
    if ((err = awbl_zm516x_cfg_info_get(id, AW_TRUE, 0, &cfg_info)) != AW_OK) {
        return err;
    }

    if (is_temp) {
        memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
        cmd_buf[3] = ZM516X_CUR_SRC_DIS;
        cmd_buf[4] = is_show;
    } else {
        memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
        cmd_buf[3] = ZM516X_DEF_SRC_DIS;
        cmd_buf[4] = AW_BITS_GET(cfg_info.my_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(cfg_info.my_addr, 0, 8);
        cmd_buf[6] = is_show;
        cmd_buf[7] = __ZM516X_DEF_END;
    }

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          is_temp ? 5 : 8,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if ((rsp_buf[4] != ZM516X_CUR_OK) &&
            (rsp_buf[6] != ZM516X_DEF_OK)) {
            return -AW_EPERM;
        }
        if (!is_temp) {
            if ((rsp_buf[4] != AW_BITS_GET(cfg_info.my_addr, 8, 8)) ||
                (rsp_buf[5] != AW_BITS_GET(cfg_info.my_addr, 0, 8))) {
                return -AW_EPERM;
            }

            /* 复位生效 */
            return awbl_zm516x_reset(id, cfg_info.my_addr);
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 设置 ZM516X 模块进入深度休眠模式（临时命令：D8） */
aw_err_t awbl_zm516x_enter_deep_sleep (int id)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_DEEP_SLEEP;
    cmd_buf[4] = 0x01;

    __zm516x_send_cmd(com_id, cmd_buf, 5, NULL, &rsp_len, 10);

    return AW_OK;
}

/** \brief 设置 ZM516X 模块的发送模式（临时命令：D9） */
aw_err_t awbl_zm516x_send_mode_temp_set (int id, uint8_t send_mode)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_SEND_MODE_SET;
    cmd_buf[4] = send_mode;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          5,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[4] != ZM516X_CUR_OK) {
            return -AW_EPERM;
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 设置 ZM516X 模块目标地址（临时命令：D2） */
aw_err_t awbl_zm516x_dst_addr_temp_set (int id, uint16_t dst_addr)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_DEST_SET;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          6,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[4] != ZM516X_CUR_OK) {
            return -AW_EPERM;
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 读取指定地址的 ZM516X 模块的信号强度（临时命令：DA） */
aw_err_t awbl_zm516x_sigal_get (int id, uint16_t dst_addr, uint8_t *p_signal)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_signal) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_CSQ_GET;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          6,
                          rsp_buf,
                         &rsp_len,
                          100) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }

        *p_signal = rsp_buf[6];
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 搜索其它模块（永久命令：D4） */
aw_err_t awbl_zm516x_discover (int                      id,
                               uint8_t                  buf_size,
                               awbl_zm516x_base_info_t *p_base_info,
                               uint8_t                 *p_get_size)
{
    uint8_t  i                             = 0;
    uint8_t  num                           = 0;
    uint8_t  pack_size                     = 0;
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len                       = 0;
    int      com_id                        = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if ((NULL == p_base_info) || (NULL == p_get_size)) {
        return -AW_EINVAL;
    }

    pack_size = 9 + 4;

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_DISCOVER;
    cmd_buf[4] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          5,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        num = rsp_len / pack_size;

        if (0 == num) {
            return -AW_EPERM;
        }

        num = (num > buf_size) ? buf_size : num;

        *p_get_size = 0;

        for (i = 0; i < num; i++) {
            if (memcmp(rsp_buf + pack_size * i, __ZM516X_DEF_TAG, 3) != 0) {
                return -AW_EPERM;
            }
            if (rsp_buf[pack_size * i + 3] != ZM516X_DEF_DISCOVER) {
                return -AW_EPERM;
            }

            /* 将串口接收到的数据转换为 awbl_zm516x_base_info_t 结构 */
            __bin_to_base_info(&rsp_buf[pack_size * i + 4], p_base_info + i);
            (*p_get_size)++;
        }

        return AW_OK;
    }
    return -AW_EPERM;
}

/*******************************************************************************
  IO 命令
*******************************************************************************/

/** \brief 获取指定地址的 ZM516X 模块的 GPIO 输入输出方向（永久命令：E1） */
aw_err_t awbl_zm516x_gpio_dir_get (int id, uint16_t dst_addr, uint8_t *p_dir)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (NULL == p_dir) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_GPIO_DIR;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 0;
    cmd_buf[7] = 0;
    cmd_buf[8] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          9,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[7] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }
        *p_dir = rsp_buf[6];
        return AW_OK;
    }
    return -AW_EPERM;
}

/** 
 * \brief 设置指定地址的 ZM516X 模块的 GPIO 输入输出方向
 *（临时命令：D4 或者永久命令：E1） 
 */
aw_err_t awbl_zm516x_gpio_dir_set (int      id,
                                   uint16_t dst_addr,
                                   uint8_t  dir,
                                   aw_bool_t   is_temp)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (is_temp) {
        memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
        cmd_buf[3] = ZM516X_CUR_GPIO_DIR_SET;
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
        cmd_buf[6] = dir;
    } else {
        memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
        cmd_buf[3] = ZM516X_DEF_GPIO_DIR;
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
        cmd_buf[6] = 1;
        cmd_buf[7] = dir;
        cmd_buf[8] = __ZM516X_DEF_END;
    }

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          is_temp ? 7 : 9,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (is_temp) {
            if (rsp_buf[6] != ZM516X_CUR_OK) {
                return -AW_EPERM;
            }
        } else {
            if (rsp_buf[6] != dir) {
                return -AW_EPERM;
            }
            if (rsp_buf[7] != ZM516X_DEF_OK) {
                return -AW_EPERM;
            }

            /* 复位生效 */
            return awbl_zm516x_reset(id, dst_addr);
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 读取指定地址的 ZM516X 模块的 GPIO 输入值（临时命令：D5） */
aw_err_t awbl_zm516x_gpio_get (int id, uint16_t dst_addr, uint8_t *p_value)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_value) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_GPIO_GET;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          6,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }

        *p_value = rsp_buf[6];
        return AW_OK;
    }
    return -AW_EPERM;
}

/**
 * \brief 设置指定地址的 ZM516X 模块的 GPIO 输出值
 *（临时命令：D6 或者永久命令：E3） 
 */
aw_err_t awbl_zm516x_gpio_set (int         id,
                               uint16_t    dst_addr,
                               uint8_t     value,
                               aw_bool_t   is_temp)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (is_temp) {
        memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
        cmd_buf[3] = ZM516X_CUR_GPIO_SET;
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
        cmd_buf[6] = value;
    } else {
        memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
        cmd_buf[3] = ZM516X_DEF_GPIO_STATE;
        cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
        cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
        cmd_buf[6] = 1;
        cmd_buf[7] = value;
        cmd_buf[8] = __ZM516X_DEF_END;
    }

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          is_temp ? 7 : 9,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (is_temp) {
            if (rsp_buf[6] != ZM516X_CUR_OK) {
                return -AW_EPERM;
            }
        } else {
            if ((rsp_buf[4] != AW_BITS_GET(dst_addr, 8, 8)) ||
                (rsp_buf[5] != AW_BITS_GET(dst_addr, 0, 8))) {
                return -AW_EPERM;
            }
            if (rsp_buf[6] != value) {
                return -AW_EPERM;
            }
            if (rsp_buf[7] != ZM516X_DEF_OK) {
                return -AW_EPERM;
            }
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 读取指定地址的 ZM516X 模块的 AD 转换值（临时命令：D7） */
aw_err_t awbl_zm516x_adc_get (int       id,
                              uint16_t  dst_addr,
                              uint8_t   chan,
                              uint16_t *p_value)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_value) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_CUR_TAG, 3);
    cmd_buf[3] = ZM516X_CUR_ADC_GET;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = chan;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          7,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }

        *p_value = rsp_buf[6] << 8 |
                   rsp_buf[7];
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 获取指定地址的 ZM516X 模块的 IO/AD 采集配置（永久命令：E2） */
aw_err_t awbl_zm516x_io_adc_upload_get (int        id,
                                        uint16_t   dst_addr,
                                        uint8_t   *p_trigger,
                                        uint16_t  *p_period_10ms,
                                        aw_bool_t *p_is_dormant)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_IO_ADC_UPLOAD;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 0;
    memset(&cmd_buf[7], 0, 4);
    cmd_buf[11] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          12,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[10] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        if (NULL != p_trigger) {
            *p_trigger = rsp_buf[6];
        }
        if (NULL != p_period_10ms) {
            *p_period_10ms = (rsp_buf[7] << 8) | rsp_buf[8];
        }
        if (NULL != p_is_dormant) {
            *p_is_dormant = rsp_buf[9];
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/**
 * \brief 设置指定地址的 ZM516X 模块的 IO/AD 采集配置（永久命令：E2），
 * 设置成功需复位
 */
aw_err_t awbl_zm516x_io_adc_upload_set (int         id,
                                        uint16_t    dst_addr,
                                        uint8_t     trigger,
                                        uint16_t    period_10ms,
                                        aw_bool_t   is_dormant)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_IO_ADC_UPLOAD;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 1;
    cmd_buf[7] = trigger;
    cmd_buf[8] = AW_BITS_GET(period_10ms, 8, 8);
    cmd_buf[9] = AW_BITS_GET(period_10ms, 0, 8);
    cmd_buf[10] = (is_dormant ) ? 1 : 0;
    cmd_buf[11] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          12,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[6] != trigger) {
            return -AW_EPERM;
        }
        if ((rsp_buf[7] != AW_BITS_GET(period_10ms, 8, 8)) ||
            (rsp_buf[8] != AW_BITS_GET(period_10ms, 0, 8))){
            return -AW_EPERM;
        }
        if (rsp_buf[9] != ((is_dormant ) ? 1 : 0)) {
            return -AW_EPERM;
        }
        if (rsp_buf[10] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        /* 复位生效 */
        return awbl_zm516x_reset(id, dst_addr);
    }
    return -AW_EPERM;
}

/** \brief 获取指定地址的 ZM516X 模块的 PWM 输出值配置（永久命令：E4） */
aw_err_t awbl_zm516x_pwm_get (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    uint16_t i       = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }
    if (NULL == p_pwm) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_PWM;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 0;
    memset(&cmd_buf[7], 0, 20);
    cmd_buf[27] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          28,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[26] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        for (i = 0; i < 4; i++) {
            memcpy(&p_pwm->freq[i], rsp_buf + 5 * i + 7, sizeof(uint32_t));
            p_pwm->freq[i] = AW_LONGSWAP(p_pwm->freq[i]);
            p_pwm->duty_cycle[i] = rsp_buf[5 * i + 11];
        }

        return AW_OK;
    }
    return -AW_EPERM;
}
/** \brief 设置指定地址的 ZM516X 模块的 PWM 输出值（永久命令：E4） */
aw_err_t awbl_zm516x_pwm_set (int                id,
                              uint16_t           dst_addr,
                              awbl_zm516x_pwm_t *p_pwm)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    uint16_t i       = 0;
    uint32_t temp    = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (NULL == p_pwm) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_PWM;
    cmd_buf[4] = AW_BITS_GET(dst_addr, 8, 8);
    cmd_buf[5] = AW_BITS_GET(dst_addr, 0, 8);
    cmd_buf[6] = 1;

    for (i = 0; i < 4; i++) {
        memcpy(&temp, &p_pwm->freq[i], sizeof(uint32_t));
        temp = AW_LONGSWAP(temp);
        memcpy(cmd_buf + 5 * i + 7, &temp, 4);
        cmd_buf[5 * i + 11] = p_pwm->duty_cycle[i];
    }

    cmd_buf[27] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          28,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 6) != 0) {
            return -AW_EPERM;
        }

        for (i = 0; i < 4; i++) {
            memcpy(&temp, &p_pwm->freq[i], sizeof(uint32_t));
            temp = AW_LONGSWAP(temp);
            if ((memcmp(rsp_buf + 5 * i + 6, &temp, 4) != 0) ||
                (rsp_buf[5 * i + 10] != p_pwm->duty_cycle[i])) {
                return -AW_EPERM;
            }
        }

        if (rsp_buf[26] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/*******************************************************************************
  自组网命令
*******************************************************************************/

/** \brief 设置自组网功能（永久命令：E5），设置成功需复位 */
aw_err_t awbl_zm516x_network_config (int id, uint8_t type, aw_bool_t is_enable)
{
    awbl_zm516x_cfg_info_t cfg_info;
    uint8_t                cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t                rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t               rsp_len = 0;
    aw_err_t               err     = AW_OK;
    int                    com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    /* 获取本地模块配置 */
    if ((err = awbl_zm516x_cfg_info_get(id, AW_TRUE, 0, &cfg_info)) != AW_OK) {
        return err;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_NETWORK_SET;
    cmd_buf[4] = (is_enable ) ? 1 : 0;
    cmd_buf[5] = type;
    cmd_buf[6] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          7,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, __ZM516X_DEF_TAG, 3) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[3] != ZM516X_DEF_NETWORK_SET) {
            return -AW_EPERM;
        }
        if (rsp_buf[4] != ((is_enable ) ? 1 : 0)) {
            return -AW_EPERM;
        }
        if (rsp_buf[5] != type) {
            return -AW_EPERM;
        }
        if (rsp_buf[6] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }

        /* 复位生效 */
        return awbl_zm516x_reset(id, cfg_info.my_addr);
    }
    return -AW_EPERM;
}

/** \brief 主机允许从机加入网络（永久命令：E6） */
aw_err_t awbl_zm516x_master_join_enable (int id, uint16_t time_second)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_JOIN_ENABLE;
    cmd_buf[4] = AW_BITS_GET(time_second, 8, 8);
    cmd_buf[5] = AW_BITS_GET(time_second, 0, 8);
    cmd_buf[6] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          7,
                          rsp_buf,
                         &rsp_len,
                          30) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if ((rsp_buf[4] != AW_BITS_GET(time_second, 8, 8)) ||
            (rsp_buf[5] != AW_BITS_GET(time_second, 0, 8))){
            return -AW_EPERM;
        }
        if (rsp_buf[6] != ZM516X_DEF_OK) {
            return -AW_EPERM;
        }
        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 查询主机模块存储的从机信息（永久命令：E7） */
aw_err_t awbl_zm516x_master_slave_info_get (
    int                       id,
    uint8_t                   buf_size,
    awbl_zm516x_slave_info_t *p_slave_info,
    uint8_t                  *p_get_size)
{
    int32_t  i;
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 1;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if ((NULL == p_slave_info) || (NULL == p_get_size)) {
        return -AW_EINVAL;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_SLAVE_GET;
    cmd_buf[4] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          5,
                          rsp_buf,
                         &rsp_len,
                          100) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }
        if (rsp_buf[rsp_len - 1] != __ZM516X_DEF_END) {
            return -AW_EPERM;
        }

        *p_get_size = (buf_size > rsp_buf[4]) ? rsp_buf[4] : buf_size;

        for (i = 0; i < *p_get_size; i++) {
            p_slave_info[i].mac[7] = rsp_buf[10 * i + 5 + 0];
            p_slave_info[i].mac[6] = rsp_buf[10 * i + 5 + 1];
            p_slave_info[i].mac[5] = rsp_buf[10 * i + 5 + 2];
            p_slave_info[i].mac[4] = rsp_buf[10 * i + 5 + 3];
            p_slave_info[i].mac[3] = rsp_buf[10 * i + 5 + 4];
            p_slave_info[i].mac[2] = rsp_buf[10 * i + 5 + 5];
            p_slave_info[i].mac[1] = rsp_buf[10 * i + 5 + 6];
            p_slave_info[i].mac[0] = rsp_buf[10 * i + 5 + 7];

            AW_BITS_SET(p_slave_info[i].addr, 8, 8, rsp_buf[10 * i + 13 + 0]);
            AW_BITS_SET(p_slave_info[i].addr, 0, 8, rsp_buf[10 * i + 13 + 1]);
        }

        return AW_OK;
    }
    return -AW_EPERM;
}

/** \brief 查询节点是主机还是从机（永久命令：E8） */
aw_err_t awbl_zm516x_network_state_get (int      id,
                                        uint8_t *p_type,
                                        uint8_t *p_response_state)
{
    uint8_t  cmd_buf[__ZM516X_CMD_BUF_LEN];
    uint8_t  rsp_buf[__ZM516X_RSP_BUF_LEN];
    uint16_t rsp_len = 0;
    int      com_id  = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    memcpy(cmd_buf, __ZM516X_DEF_TAG, 3);
    cmd_buf[3] = ZM516X_DEF_STATE_GET;
    cmd_buf[4] = __ZM516X_DEF_END;

    if (__zm516x_send_cmd(com_id,
                          cmd_buf,
                          5,
                          rsp_buf,
                         &rsp_len,
                          100) ) {
        if (memcmp(rsp_buf, cmd_buf, 4) != 0) {
            return -AW_EPERM;
        }

        if (NULL != p_type) {
            *p_type = rsp_buf[4];
        }
        if (NULL != p_response_state) {
            *p_response_state = rsp_buf[5];
        }

        return AW_OK;
    }
    return -AW_EPERM;
}

/*******************************************************************************
  数据传输命令
*******************************************************************************/

/** \brief 向 ZM516X 模块发送 nbytes 个字节数据 */
ssize_t awbl_zm516x_send (int id, const void *p_buf, size_t nbytes)
{
    int com_id = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_serial_write(com_id, (char *)p_buf, nbytes);
}

/** \brief 向 ZM516X 模块发送 nbytes 个字节数据，并等待 ACK */
ssize_t awbl_zm516x_send_with_ack (int id, const void *p_buf, size_t nbytes)
{
    struct awbl_zm516x_dev     *p_this    = __ID_GET_ZM516X_DEV(id);
    struct awbl_zm516x_devinfo *p_devinfo = NULL;
    ssize_t                     num       = 0;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    p_devinfo = __ZM516X_DEVINFO_GET(&p_this->dev);
    if (NULL == p_devinfo) {
        return -AW_ENODEV;
    }

    if (-1 == p_devinfo->ack_pin) {
        return -AW_ENOTSUP;
    }

    AW_SEMB_RESET(p_this->ack_wait, 0);

    num = aw_serial_write(p_devinfo->com_id, (char *)p_buf, nbytes);

    if (AW_SEMB_TAKE(p_this->ack_wait,
                     aw_ms_to_ticks(p_devinfo->ack_timeout)) != AW_OK) {
        return -AW_ETIME;
    } else {
        return num;
    }
}

/** \brief 从 ZM516X 模块接收 maxbytes 个字节数据 */
aw_err_t awbl_zm516x_receive (int id, void *p_buf, size_t maxbytes)
{
    int com_id = __com_id_get(id);

    if (com_id < 0) {
        return com_id;
    }

    if (NULL == p_buf) {
        return -AW_EINVAL;
    }

    return aw_serial_read(com_id, (char *)p_buf, maxbytes);
}

/* end of file */
