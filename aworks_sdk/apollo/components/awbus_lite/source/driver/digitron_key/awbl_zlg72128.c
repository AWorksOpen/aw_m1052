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
 * \brief ZLG72128 驱动
 *
 * \internal
 * \par Modification History
 * - 1.00 17-10-11  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/
#include "aworks.h"
#include "aw_int.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_bitops.h"
#include "aw_spinlock.h"
#include "awbl_digitron_if.h"
#include "driver/digitron_key/awbl_zlg72128.h"

/*******************************************************************************
  寄存器地址定义
*******************************************************************************/
#define __ZLG72128_REGADDR_SYSTEM        0x00        /**< \brief 系统寄存器 */
#define __ZLG72128_REGADDR_KEY           0x01        /**< \brief 键值寄存器 */
#define __ZLG72128_REGADDR_REPEATCNT     0x02        /**< \brief 连击计数器 */
#define __ZLG72128_REGADDR_FUNCTION_KEY  0x03        /**< \brief 功能键寄存器 */
#define __ZLG72128_REGADDR_CMDBUF0       0x07        /**< \brief 命令缓冲区 0 */
#define __ZLG72128_REGADDR_CMDBUF1       0x08        /**< \brief 命令缓冲区 1 */
#define __ZLG72128_REGADDR_FLASH_TIME    0x0B        /**< \brief 闪烁控制寄存器 */
#define __ZLG72128_REGADDR_DISP_CTRL0    0x0C        /**< \brief 消隐寄存器 0 */
#define __ZLG72128_REGADDR_DISP_CTRL1    0x0D        /**< \brief 消隐寄存器 1 */
#define __ZLG72128_REGADDR_FLASH_CTRL0   0x0E        /**< \brief 闪烁寄存器 0 */
#define __ZLG72128_REGADDR_FLASH_CTRL1   0x0F        /**< \brief 闪烁寄存器 1 */
#define __ZLG72128_REGADDR_DISPBUF(n)   (0x10 + (n)) /**< \brief 显示缓冲区 */

/*******************************************************************************
  CMD 定义
*******************************************************************************/

/** \brief 段关闭命令 */
#define __ZLG72128_DIGITRON_CMD_SEGOFF           0x10

/** \brief 段打开命令 */
#define __ZLG72128_DIGITRON_CMD_SEGON            0x11

/** \brief 下载数据并译码命令 */
#define __ZLG72128_DIGITRON_CMD_DOWNLOAD(pos)   (0x20 + ((pos) & 0xF))

/** \brief 复位命令 */
#define __ZLG72128_DIGITRON_CMD_RESET            0x30

/** \brief 测试命令 */
#define __ZLG72128_DIGITRON_CMD_TEST             0x40

/** \brief 移位命令 */
#define __ZLG72128_DIGITRON_CMD_SHIFT(is_left, is_cyclic, n) \
    (((is_left) ? 0x50 : 0x70) + ((is_cyclic) ? 0x10 : 0x00) + ((n) & 0x0F))

/** \brief 数码管扫描位数设置命令 */
#define __ZLG72128_DIGITRON_SCANNING             0x90

/*******************************************************************************
  宏定义
*******************************************************************************/

/** \brief ZLG72128 7 位 I2C 地址 */
#define __ZLG72128_I2C_ADDR                      0x30

/** \brief 最大数码管数量 */
#define __ZLG72128_DIGITRON_NUM_MAX              12

/** \brief ZLG72128 状态 */
#define __ZLG72128_STATE_IDLE                    0
#define __ZLG72128_STATE_WAIT_READ_COMPLETE      1

/** \brief 通过扫描器设备定义并初始化 ZLG72128 设备 */
#define __SCANNER_TO_ZLG72128_DEV_DECL(p_dev, p_scan) \
          awbl_zlg72128_dev_t *p_dev =                \
          AW_CONTAINER_OF(p_scan, awbl_zlg72128_dev_t, scan_dev)

/** \brief 通过数码管设备定义并初始化 ZLG72128 设备 */
#define __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_this, p_dev) \
          struct awbl_zlg72128_dev *p_this =               \
          AW_CONTAINER_OF(p_dev, struct awbl_zlg72128_dev, digitron_dev)

/** \brief 通过 AWBus 设备定义并初始化 ZLG72128 设备 */
#define __DEV_TO_ZLG72128_DEV_DECL(p_this, p_dev)          \
          struct awbl_zlg72128_dev *p_this =               \
          AW_CONTAINER_OF(p_dev, struct awbl_zlg72128_dev, dev)

/** \brief 通过 AWBus 设备定义并初始化 ZLG72128 设备信息 */
#define __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, p_dev)  \
          struct awbl_zlg72128_devinfo *p_devinfo =        \
          (struct awbl_zlg72128_devinfo *)AWBL_DEVINFO_GET(p_dev)

/** \brief 通过设备号定义并初始化 ZLG72128 设备 */
#define __ID_GET_ZLG72128_DEV_DECL(p_dev, id) \
          struct awbl_zlg72128_dev *p_dev =   \
          (struct awbl_zlg72128_dev *)        \
          awbl_dev_find_by_name(AWBL_ZLG72128_NAME, id)

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/**
 * \brief 第三阶段初始化
 */
aw_local void __zlg72128_inst_connect (awbl_dev_t *p_dev);

/**
 * \brief I2C 传输完成回调函数
 */
aw_local void __zlg72128_trans_complete (void *p_arg);

/**
 * \brief 按键中断或扫描器回调函数
 */
aw_local void __zlg72128_int_callback (void *p_arg);

/**
 * \brief 设置 ZLG72128 传输结构体参数
 */
aw_local void __zlg72128_mkmsg (awbl_zlg72128_dev_t *p_dev,
                                awbl_zlg72128_msg_t *p_msg,
                                uint8_t              regaddr,
                                uint8_t             *p_buf,
                                size_t               len);

/**
 * \brief 数码管组注册函数, 将该数码管组注册进数码管子系统
 */
aw_local void __zlg72128_disp_register (struct awbl_digitron_dev        *p_dd,
                                        struct awbl_digitron_disp_param *p_par,
                                        int                              dd_id);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_zlg72128 = {
    NULL,                   /* 第一阶段初始化 */
    NULL,                   /* 第二阶段初始化 */
    __zlg72128_inst_connect /* 第三阶段初始化 */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_zlg72128 = {
    AWBL_VER_1,                  /* AWBus 版本号 */
    AWBL_BUSID_I2C,              /* 总线 ID */
    AWBL_ZLG72128_NAME,          /* 驱动名 */
    &__g_awbl_drvfuncs_zlg72128, /* 驱动入口点 */
    NULL,                        /* 驱动提供的方法 */
    NULL                         /* 驱动探测函数 */
};

/** \brief 数码管索引表 */
aw_local aw_const uint8_t __g_zlg72128_disp_idx_tbl[12] = {
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/**
 * \brief 第三阶段初始化
 */
aw_local void __zlg72128_inst_connect (struct awbl_dev *p_dev)
{
    __DEV_TO_ZLG72128_DEV_DECL(p_this, p_dev);
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, p_dev);
    aw_local aw_const uint8_t reg_addr = 0x00;    /* 始终为 0 */
    uint8_t                   i;

    if ((NULL == p_this) || (NULL == p_devinfo)) {
        return;
    }

    p_this->disp_param.num_segment    = 8;
    p_this->disp_param.num_rows       = 1;
    p_this->disp_param.num_cols       = p_devinfo->digitron_num;
    p_this->disp_param.seg_active_low = AW_FALSE;
    p_this->disp_param.com_active_low = AW_FALSE;
    p_this->disp_param.blink_cnt_max  = 50;
    p_this->disp_param.p_disp_buf     = NULL;
    p_this->disp_param.p_disp_idx_tbl = __g_zlg72128_disp_idx_tbl;
    p_this->f_key                     = 0xFF;
    p_this->num_cols                  = 0;
    p_this->num_rows                  = 0;
    p_this->state                     = __ZLG72128_STATE_IDLE;
    p_this->pfn_key_cb                = NULL;
    p_this->pfn_key_arg               = NULL;

    /* 计算实际使用的按键行数与列数 */
    for (i = 0; i < 8; i++) {
        if (p_devinfo->key_use_col_flags & (1 << i)) {
            p_this->num_cols++;
        }
        if (p_devinfo->key_use_row_flags & (1 << i)) {
            p_this->num_rows++;
        }
    }

    /* 为异步 I2C 读写函数分配一块内存池 */
    p_this->pool_id = aw_pool_init(&p_this->pool,
                                    p_this->bulk,
                                    sizeof(p_this->bulk),
                                    sizeof(p_this->bulk[0]));

    /* 设置I2C设备结构体参数 */
    aw_i2c_mkdev(&(p_this->i2c_dev),
                  p_this->dev.p_devhcf->bus_index,
                  __ZLG72128_I2C_ADDR,
                  (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    /* 传输寄存器地址 */
    aw_i2c_mktrans(&(p_this->trans[0]),
                     __ZLG72128_I2C_ADDR,
                     AW_I2C_M_7BIT | AW_I2C_M_WR,
                     (uint8_t *)&reg_addr,
                     1);

    /* 读取连续 3 个键值寄存器的值 */
    aw_i2c_mktrans(&(p_this->trans[1]),
                     __ZLG72128_I2C_ADDR,
                     AW_I2C_M_7BIT | AW_I2C_M_RD,
                     p_this->key_buf,
                     4);

    /* 制作好 msg，后续直接传输即可，避免重复设置 msg */
    aw_i2c_mkmsg(&(p_this->msg),
                 &(p_this->trans[0]),
                 2,
                 __zlg72128_trans_complete,
                 &p_this->dev);

    /* 初始化复位引脚 */
    if (p_devinfo->rst_pin != -1) {
        aw_gpio_pin_cfg(p_devinfo->rst_pin, AW_GPIO_OUTPUT_INIT_LOW);
        aw_mdelay(1);
        aw_gpio_set(p_devinfo->rst_pin, 1);
        aw_mdelay(5);
    }

    if (p_devinfo->int_pin != -1) {
        aw_gpio_pin_cfg(p_devinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);

        /* 连接 GPIO 引脚中断 */
        if (aw_gpio_trigger_connect(p_devinfo->int_pin,
                                    __zlg72128_int_callback,
                                   &p_this->scan_dev) != AW_OK) {
            return;
        }

        /* 下降沿触发中断 */
        if (aw_gpio_trigger_cfg(p_devinfo->int_pin,
                                AW_GPIO_TRIGGER_FALL) != AW_OK) {
            return;
        }

        aw_gpio_trigger_on(p_devinfo->int_pin);

    } else {

        /* 构造扫描器 */
        p_this->scan_param.scan_cnt = 1;
        p_this->scan_param.scan_interval = p_devinfo->interval_ms;
        awbl_scanner_dev_ctor(&p_this->scan_dev, &p_this->scan_param);

        /* 向扫描器添加回调函数, 注意回调函数的注册顺序, 先注册先执行 */
        p_this->scan_cb_handler.pfn_scan_cb = __zlg72128_int_callback;
        awbl_scan_cb_list_add(&p_this->scan_dev, &p_this->scan_cb_handler);

        /* 启动扫描器 */
        awbl_scanner_start(&p_this->scan_dev);
    }

    /* 熄灭所有数码管 */
    awbl_zlg72128_digitron_disp_reset(p_this->dev.p_devhcf->unit);

    /* 初始化数码管闪烁时间 */
    awbl_zlg72128_digitron_flash_time_cfg(p_this->dev.p_devhcf->unit,
                                          p_devinfo->blink_on_time,
                                          p_devinfo->blink_off_time);

    /* 配置数码管的扫描位数 */
    awbl_zlg72128_digitron_scan_ctrl(p_this->dev.p_devhcf->unit,
                                     p_devinfo->digitron_num);

    /* 将 ZLG72128 的数码管组注册进数码管子系统 */
    __zlg72128_disp_register(&p_this->digitron_dev,
                             &p_this->disp_param,
                              p_devinfo->digitron_disp_id);
}

/**
 * \brief ZLG72128 读寄存器
 */
aw_local aw_err_t __zlg72128_data_read (awbl_zlg72128_dev_t *p_dev,
                                        uint8_t              regaddr,
                                        uint8_t             *p_buf,
                                        size_t               len)
{
    return aw_i2c_read(&(p_dev->i2c_dev),
                        regaddr,
                        p_buf,
                        len);
}

/**
 * \brief ZLG72128 写寄存器
 */
aw_local aw_err_t __zlg72128_data_write (awbl_zlg72128_dev_t *p_dev,
                                         uint8_t              regaddr,
                                         uint8_t             *p_buf,
                                         size_t               len)
{
    awbl_zlg72128_msg_t *p_msg  = NULL;

    if (__ZLG72128_STATE_IDLE == p_dev->state) {
        return aw_i2c_write(&(p_dev->i2c_dev),
                             regaddr,
                             p_buf,
                             len);
    } else {
        p_msg= (awbl_zlg72128_msg_t *)aw_pool_item_get(p_dev->pool_id);

        if (NULL == p_msg) {
            return -AW_EPERM;
        }

        __zlg72128_mkmsg(p_dev,
                         p_msg,
                         regaddr,
                         p_buf,
                         len);

        return aw_i2c_async(p_dev->i2c_dev.busid, &(p_msg->i2c_msg));
    }
}

/**
 * \brief 向 input 子系统上报按键事件
 *
 * \param[in] p_arg     自定义参数
 * \param[in] row       行
 * \param[in] col       列
 * \param[in] key_state 按键状态
 *
 * \retval AW_OK 成功
 */
aw_local int __zlg72128_key_code_report (void *p_arg,
                                         int   row,
                                         int   col,
                                         int   key_state)
{
    __DEV_TO_ZLG72128_DEV_DECL(p_dev, p_arg);
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, &p_dev->dev);
    int     i;
    uint8_t flags;
    uint8_t actual_row = row;
    uint8_t actual_col = col;

    if ((p_devinfo->key_use_row_flags & (1 << row)) &&
        (p_devinfo->key_use_col_flags & (1 << col))){

        if (p_devinfo->key_use_row_flags != 0x0F) {
            flags = p_devinfo->key_use_row_flags;
            for (i = 0; i < row; i++) {
                if ((flags & (1 << i)) == 0) {      /* skip unused rows */
                    actual_row--;
                }
            }
        }

        if (p_devinfo->key_use_col_flags != 0xFF) {
            flags = p_devinfo->key_use_col_flags;
            for (i = 0; i < col; i++) {
                if ((flags & (1 << i)) == 0) {      /* skip unused cols */
                    actual_col--;
                }
            }
        }

        /* 向 input 子系统上报按键事件 */
        aw_input_report_key(AW_INPUT_EV_KEY,
                            p_devinfo->p_key_codes\
                            [actual_row * p_dev->num_cols + actual_col],
                            key_state);
    }

    return AW_OK;
}

/**
 * \brief 按键处理函数
 */
aw_local void __zlg72128_key_process (awbl_zlg72128_dev_t *p_dev)
{
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, &p_dev->dev);
    int      i          = 0;
    int      row        = 0;
    int      col        = 0;
    uint8_t  change     = 0;
    uint8_t *p_buf      = p_dev->key_buf;
    uint8_t  key_val    = p_buf[1];
    uint8_t  repeat_cnt = p_buf[2];
    uint8_t  funkey_val = p_buf[3];

    /* 如果不存在按键按下，直接返回 */
    if ((p_buf[0] & 0x01) == 0) {
        return;
    }

    /* 调用用户回调函数 */
    if (p_dev->pfn_key_cb) {
        p_dev->pfn_key_cb(p_dev->pfn_key_arg, p_buf[1], p_buf[2], p_buf[3]);
    }

    if (key_val != 0x00) {
        key_val--;
        row = key_val / 8;
        col = key_val % 8;
        __zlg72128_key_code_report(p_dev, row, col, 1);
    }

    /* 判断是否有特殊功能按键按下 */
    if (!(p_devinfo->key_use_row_flags & AWBL_ZLG72128_KEY_ROW_3)) {
        return;
    }
    if (p_dev->f_key == funkey_val) {
        return;
    }
    change = p_dev->f_key ^ funkey_val;
    for (i = 0; i < 8; i++) {
        if (change & (1 << i)) {
            __zlg72128_key_code_report(p_dev,
                                       3,
                                       i,
                                       (funkey_val & (1 << i)) ? 0 : 1);
        }
    }
    p_dev->f_key = funkey_val;
}

/**
 * \brief I2C 传输完成回调函数
 */
aw_local void __zlg72128_trans_complete (void *p_arg)
{
    __DEV_TO_ZLG72128_DEV_DECL(p_dev, p_arg);
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, p_arg);

    /* 调用按键处理函数 */
    if (p_dev->msg.status == AW_OK) {
        __zlg72128_key_process(p_dev);
    }

    p_dev->state = __ZLG72128_STATE_IDLE;

    /* 使能 GPIO 中断 */
    if (p_devinfo->int_pin != -1) {
        aw_gpio_trigger_on(p_devinfo->int_pin);
    }
}

/**
 * \brief 在 I2C 传输完成回调函数中再次执行 I2C 操作的传输完成回调函数
 */
aw_local void __zlg72128_trans_reentry_complete (void *p_arg)
{
    awbl_zlg72128_msg_t *p_msg = (awbl_zlg72128_msg_t *)p_arg;

    aw_pool_item_return(p_msg->pool_id, p_msg);
}

/**
 * \brief 按键中断或扫描器回调函数
 */
aw_local void __zlg72128_int_callback (void *p_arg)
{
    __SCANNER_TO_ZLG72128_DEV_DECL(p_dev, p_arg);
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, &p_dev->dev);

    if (p_devinfo->int_pin != -1) {

        /* 失能 GPIO 中断 */
        aw_gpio_trigger_off(p_devinfo->int_pin);
    }

    if (p_dev->state == __ZLG72128_STATE_IDLE) {
        p_dev->state = __ZLG72128_STATE_WAIT_READ_COMPLETE;
        aw_i2c_async(p_dev->i2c_dev.busid, &(p_dev->msg));
    }
}

/**
 * \brief 设置 ZLG72128 传输结构体参数
 */
aw_local void __zlg72128_mkmsg (awbl_zlg72128_dev_t *p_dev,
                                awbl_zlg72128_msg_t *p_msg,
                                uint8_t              regaddr,
                                uint8_t             *p_buf,
                                size_t               len)
{
    __DEV_GET_ZLG72128_DEVINFO_DECL(p_devinfo, p_dev);
    int i = 0;

    p_msg->pool_id = p_dev->pool_id;

    p_msg->buf[0] = regaddr;
    for (i = 0; i < len; i++) {
        p_msg->buf[i + 1] = p_buf[i];
    }

    aw_i2c_mktrans(&(p_msg->trans[0]),
                    __ZLG72128_I2C_ADDR,
                    AW_I2C_M_7BIT | AW_I2C_M_WR,
                    p_msg->buf,
                    len + 1);

    aw_i2c_mkmsg(&(p_msg->i2c_msg),
                 &(p_msg->trans[0]),
                 1,
                 __zlg72128_trans_reentry_complete,
                 p_msg);
}

/******************************************************************************/

/**
 * \brief 设置数码管显示闪烁属性
 */
aw_local void __digitron_disp_blink_set (struct awbl_digitron_dev *p_dd,
                                         int                       index,
                                         aw_bool_t                    blink)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    AW_BIT_MODIFY(p_dd->blink_flags, index, blink);

    awbl_zlg72128_digitron_flash_ctrl(p_dev->dev.p_devhcf->unit,
                                      p_dd->blink_flags);
}

/**
 * \brief 直接向数码管写入数据显示数据
 */
aw_local void __digitron_disp_at (struct awbl_digitron_dev *p_dd,
                                  int                       index,
                                  uint16_t                  seg)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    awbl_zlg72128_digitron_dispbuf_set(p_dev->dev.p_devhcf->unit,
                                       index,
                                       (uint8_t *)&seg,
                                       1);
}

/**
 * \brief 在指定编号的数码管上显示 ASCII 字符
 */
aw_local void __digitron_disp_char_at (struct awbl_digitron_dev *p_dd,
                                       int                       index,
                                       const char                ch)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);
    uint8_t seg = 0x00;
    aw_bool_t  is_blink;

    if (p_dev == NULL) {
        return;
    }

    is_blink = AW_BIT_GET(p_dd->blink_flags, index);

    if (awbl_zlg72128_digitron_disp_char(p_dev->dev.p_devhcf->unit,
                                         index,
                                         ch,
                                         AW_FALSE,
                                         is_blink) == -AW_ENOTSUP) {
        if ('.' == ch) {
            awbl_zlg72128_digitron_seg_ctrl(p_dev->dev.p_devhcf->unit,
                                            index,
                                            AWBL_ZLG72128_DIGITRON_SEG_DP,
                                            AW_TRUE);
        } else {
            if (p_dev->digitron_dev.digitron_ops.pfn_digitron_disp_decode) {
                seg = p_dev->digitron_dev.digitron_ops.pfn_digitron_disp_decode(ch);
                awbl_zlg72128_digitron_dispbuf_set(p_dev->dev.p_devhcf->unit,
                                                   index,
                                                  &seg,
                                                   1);
            }
        }
    }
}

/**
 * \brief 在数码管上显示 ASCII 字符串
 */
aw_local void __digitron_disp_str (struct awbl_digitron_dev *p_dd,
                                   int                       index,
                                   int                       len,
                                   const char               *p_str)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if ((p_dev == NULL) || (p_str == NULL)) {
        return;
    }

    awbl_zlg72128_digitron_disp_str(p_dev->dev.p_devhcf->unit,
                                    index,
                                    p_str,
                                    p_dd->blink_flags >> index);
}

/**
 * \brief 清除所有数码管显示
 */
aw_local void __digitron_disp_clr (struct awbl_digitron_dev *p_dd)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    p_dd->blink_flags = 0;

    awbl_zlg72128_digitron_disp_reset(p_dev->dev.p_devhcf->unit);
}

/**
 * \brief 使能数码管显示
 */
aw_local void __digitron_disp_enable (struct awbl_digitron_dev *p_dd)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    awbl_zlg72128_digitron_disp_ctrl(p_dev->dev.p_devhcf->unit, 0x0000);
}

/**
 * \brief 禁能数码管显示
 */
aw_local void __digitron_disp_disable (struct awbl_digitron_dev *p_dd)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    awbl_zlg72128_digitron_disp_ctrl(p_dev->dev.p_devhcf->unit, 0xFFFF);
}

/**
 * \brief 熄灭所有数码管
 */
aw_local void __digitron_disp_set_seg_deactive (struct awbl_digitron_dev *p_dd)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dev == NULL) {
        return;
    }

    awbl_zlg72128_digitron_disp_reset(p_dev->dev.p_devhcf->unit);
}

/**
 * \brief 数码管组注册函数, 将该数码管组注册进数码管子系统
 *
 * \param[in] p_dd  通用数码管设备
 * \param[in] p_par 数码管矩阵配置信息
 * \param[in] dd_id 数码管 ID
 *
 * \return 无
 */
aw_local void __zlg72128_disp_register (struct awbl_digitron_dev        *p_dd,
                                        struct awbl_digitron_disp_param *p_par,
                                        int                              dd_id)
{
    __DIGITRON_DEV_TO_ZLG72128_DEV_DECL(p_dev, p_dd);

    if (p_dd == NULL || p_par == NULL) {
        return;
    }

    p_dd->blink_cnt_max = p_par->blink_cnt_max;
    p_dd->blink_flags   = 0;
    p_dd->blink_cnt     = 0;
    p_dd->blinking      = AW_FALSE;

    p_dd->pdd_param   = p_par;
    p_dd->disp_dev_id = dd_id;
    p_dd->disp_enable = AW_TRUE;

    p_dd->digitron_ops.pfn_digitron_disp_blink_set        = __digitron_disp_blink_set;
    p_dd->digitron_ops.pfn_digitron_disp_at               = __digitron_disp_at;
    p_dd->digitron_ops.pfn_digitron_disp_char_at          = __digitron_disp_char_at;
    p_dd->digitron_ops.pfn_digitron_disp_str              = __digitron_disp_str;
    p_dd->digitron_ops.pfn_digitron_disp_clr              = __digitron_disp_clr;
    p_dd->digitron_ops.pfn_digitron_disp_seg_deactive_set = __digitron_disp_set_seg_deactive;
    p_dd->digitron_ops.pfn_digitron_disp_enable           = __digitron_disp_enable;
    p_dd->digitron_ops.pfn_digitron_disp_disable          = __digitron_disp_disable;
    p_dd->digitron_ops.pfn_digitron_disp_decode           = NULL;

    awbl_digitron_dev_list_add(p_dd);
}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 按键回调函数注册
 */
aw_err_t awbl_zlg72128_key_cb_set (int                     id,
                                   awbl_zlg72128_key_cb_t  pfn_key_cb,
                                   void                   *p_arg)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint32_t key;

    if (pfn_key_cb != NULL && p_dev != NULL) {

        AW_INT_CPU_LOCK(key);

        p_dev->pfn_key_cb  = pfn_key_cb;
        p_dev->pfn_key_arg = p_arg;

        AW_INT_CPU_UNLOCK(key);

        return AW_OK;
    }

    return -AW_EINVAL;
}

/**
 * \brief 设置数码管闪烁时间，即当设定某位闪烁时点亮持续的时间和熄灭持续的时间
 */
aw_err_t awbl_zlg72128_digitron_flash_time_cfg (int      id,
                                                uint16_t on_ms,
                                                uint16_t off_ms)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t on_n;
    uint8_t off_n;
    uint8_t ctrl;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (on_ms <= 150) {
        on_n = 0;
    } else {
        on_n = ((on_ms - 150) + (50 - 1)) / 50;
    }

    if (off_ms <= 150) {
        off_n = 0;
    } else {
        off_n = ((off_ms - 150) + (50 - 1)) / 50;
    }

    on_n  &= 0xF;
    off_n &= 0xF;

    ctrl = (on_n << 4) | off_n;

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_FLASH_TIME,
                                &ctrl,
                                 1);
}

/**
 * \brief 控制（打开或关闭）数码管闪烁，默认均不闪烁
 */
aw_err_t awbl_zlg72128_digitron_flash_ctrl (int id, uint16_t ctrl_val)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t ctrl[2];

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    /* 低字节控制的高 8 位，高字节控制的低 8 位 */
    ctrl[0] = (ctrl_val >> 8) & 0xF;      /* 高 8 位后发送 */
    ctrl[1] =  ctrl_val & 0xFF;           /* 低 8 位后发送 */

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_FLASH_CTRL0,
                                 ctrl,
                                 2);
}

/**
 * \brief 控制数码管的显示属性（打开显示或关闭显示）
 */
aw_err_t awbl_zlg72128_digitron_disp_ctrl (int id, uint16_t ctrl_val)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t ctrl[2];

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    /* 低字节控制的高 8 位，高字节控制的低 8 位 */
    ctrl[0] = (ctrl_val >> 8) & 0xF;      /* 高 8 位后发送 */
    ctrl[1] =  ctrl_val & 0xFF;           /* 低 8 位后发送 */

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_DISP_CTRL0,
                                 ctrl,
                                 2);
}

/**
 * \brief 控制数码管的扫描位数
 */
aw_err_t awbl_zlg72128_digitron_scan_ctrl (int id, uint8_t ctrl_val)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t ctrl;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    ctrl = (ctrl_val & 0xF) | __ZLG72128_DIGITRON_SCANNING;

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_DISP_CTRL0,
                                &ctrl,
                                 1);
}

/**
 * \brief 直接设置数码管显示的内容（段码）
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_set (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (start_pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    if ((start_pos + num) > 12) {
        num = (start_pos + num) - 12;
    }

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_DISPBUF(start_pos),
                                 p_buf,
                                 num);
}

/**
 * \brief 获取数码管显示的内容（段码）
 */
aw_err_t awbl_zlg72128_digitron_dispbuf_get (int      id,
                                             uint8_t  start_pos,
                                             uint8_t *p_buf,
                                             uint8_t  num)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (start_pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    if ((start_pos + num) > 12) {
        num = (start_pos + num) - 12;
    }

    return __zlg72128_data_read(p_dev,
                                __ZLG72128_REGADDR_DISPBUF(start_pos),
                                p_buf,
                                num);
}

/**
 * \brief 设置数码管显示的字符
 */
aw_err_t awbl_zlg72128_digitron_disp_char (int     id,
                                           uint8_t pos,
                                           char    ch,
                                           aw_bool_t  is_dp_disp,
                                           aw_bool_t  is_flash)

{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t                i;
    uint8_t                cmd[2];
    aw_local aw_const char code[32] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'b', 'C', 'd', 'E', 'F',
        'G', 'H', 'i', 'J', 'L', 'o', 'p', 'q',
        'r', 't', 'U', 'y', 'c', 'h', 'T', ' ',
    };


    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    for (i = 0; i < sizeof(code); i++) {
        if (code[i] == ch) {
            break;
        }
    }

    if (i == sizeof(code)) {
        return -AW_ENOTSUP;
    }

    cmd[0] = __ZLG72128_DIGITRON_CMD_DOWNLOAD(pos);
    cmd[1] = i;

    if (is_dp_disp ) {
        cmd[1] |= (1 << 7);
    }

    if (is_flash ) {
        cmd[1] |= (1 << 6);
    }

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                 cmd,
                                 2);
}

/**
 * \brief 从指定位置开始显示一个字符串
 */
aw_err_t awbl_zlg72128_digitron_disp_str (int         id,
                                          uint8_t     start_pos,
                                          const char *p_str,
                                          uint8_t     flash_flags)
{
    int    i   = 0;
    int    ret = AW_OK;
    aw_bool_t is_flash;

    if (p_str == NULL) {
        return -AW_EINVAL;
    }

    if (start_pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    while (*p_str != '\0' && start_pos < __ZLG72128_DIGITRON_NUM_MAX) {

        is_flash = AW_BIT_GET(flash_flags, i++);

        /* '.' 单独处理 */
        if (*p_str == '.') {

            /* 显示 '.' */
            ret = awbl_zlg72128_digitron_disp_char(id,
                                                   start_pos,
                                                   ' ',
                                                   AW_TRUE,
                                                   is_flash);
        } else {

            if (*(p_str + 1) == '.') {

                ret = awbl_zlg72128_digitron_disp_char(id,
                                                       start_pos,
                                                      *p_str,
                                                       AW_TRUE,
                                                       is_flash);

                p_str++;
            } else {
                ret = awbl_zlg72128_digitron_disp_char(id,
                                                       start_pos,
                                                      *p_str,
                                                       AW_FALSE,
                                                       is_flash);
            }
        }

        if (ret != AW_OK) {
            return ret;
        }

        start_pos++;
        p_str++;
    }

    return ret;
}

/**
 * \brief 设置数码管显示的数字（0 ~ 9）
 */
aw_err_t awbl_zlg72128_digitron_disp_num (int     id,
                                          uint8_t pos,
                                          uint8_t num,
                                          aw_bool_t  is_dp_disp,
                                          aw_bool_t  is_flash)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t cmd[2];

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (num >= 10 || pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    cmd[0] = __ZLG72128_DIGITRON_CMD_DOWNLOAD(pos);
    cmd[1] = num;

    if (is_dp_disp ) {
        cmd[1] |= (1 << 7);
    }

    if (is_flash ) {
        cmd[1] |= (1 << 6);
    }

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                 cmd,
                                 2);
}

/**
 * \brief 直接控制段的点亮或熄灭
 */
aw_err_t awbl_zlg72128_digitron_seg_ctrl (int     id,
                                          uint8_t pos,
                                          char    seg,
                                          aw_bool_t  is_on)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t cmd[2];

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (pos >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    if (is_on ) {
        cmd[0] = __ZLG72128_DIGITRON_CMD_SEGON;
    } else {
        cmd[0] = __ZLG72128_DIGITRON_CMD_SEGOFF;
    }

    cmd[1] = ((pos & 0x0F) << 4) | (seg);

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                 cmd,
                                 2);
}

/**
 * \brief 显示移位命令
 */
aw_err_t awbl_zlg72128_digitron_shift (int     id,
                                       uint8_t dir,
                                       aw_bool_t  is_cyclic,
                                       uint8_t num)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t cmd;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    if (num >= __ZLG72128_DIGITRON_NUM_MAX) {
        return -AW_EINVAL;
    }

    /* 低4位为移位的位数, dir (0-right，1-left) */
    cmd = __ZLG72128_DIGITRON_CMD_SHIFT(dir, is_cyclic, num);

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                &cmd,
                                 1);

}

/**
 * \brief 复位命令，将数码管的所有 LED 段熄灭
 */
aw_err_t awbl_zlg72128_digitron_disp_reset (int id)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t cmd;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    cmd = __ZLG72128_DIGITRON_CMD_RESET;

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                &cmd,
                                 1);
}

/**
 * \brief 测试命令，所有 LED 段以 0.5S 的速率闪烁，用于测试数码管显示是否正常
 */
aw_err_t awbl_zlg72128_digitron_disp_test (int id)
{
    __ID_GET_ZLG72128_DEV_DECL(p_dev, id);
    uint8_t cmd;

    if (p_dev == NULL) {
        return -AW_EINVAL;
    }

    cmd = __ZLG72128_DIGITRON_CMD_TEST;

    return __zlg72128_data_write(p_dev,
                                 __ZLG72128_REGADDR_CMDBUF0,
                                &cmd,
                                 1);
}

/**
 * \brief 将 ZLG72128 驱动注册到 AWBus 子系统中
 *
 * \note 本函数中禁止调用操作系统 API
 */
void awbl_zlg72128_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_zlg72128);
}

/* end of file */
