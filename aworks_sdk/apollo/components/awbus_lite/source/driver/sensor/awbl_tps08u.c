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
 * \brief 标准信号采集模块 TPS08U 驱动
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-03  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  头文件包含
*******************************************************************************/

#include "aworks.h"
#include "aw_int.h"
#include "aw_spi.h"
#include "aw_gpio.h"
#include "aw_delay.h"
#include "aw_errno.h"
#include "aw_common.h"
#include "aw_vdebug.h"
#include "aw_compiler.h"
#include "aw_delayed_work.h"
#include "awbl_sensor.h"
#include "string.h"
#include "driver/sensor/awbl_tps08u.h"

/*******************************************************************************
  宏定义
*******************************************************************************/

/** \brief 通过设备号获取设备实例 */
#define __ID_GET_TPS08U_DEV(id)               \
            (struct awbl_tps08u_dev *) \
            awbl_dev_find_by_name(AWBL_TPS08U_NAME, id)

/** \brief 通过设备实例获取设备信息 */
#define __TPS08U_DEVINFO_GET(p_dev) \
            (struct awbl_tps08u_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __SPI_ST_IDLE            0    /**< \brief 空闲状态 */
#define __SPI_ST_CS_VALID        1    /**< \brief 片选有效 */
#define __SPI_ST_CMD             2    /**< \brief 传输 CMD */
#define __SPI_ST_DATA            3    /**< \brief 传输数据 */
#define __SPI_ST_CS_INVALID      4    /**< \brief 片选无效 */

#define __SPI_EVT_NONE           0    /**< \brief 无事件 */
#define __SPI_EVT_CS_VALID       1    /**< \brief 片选有效 */
#define __SPI_EVT_SEND_CMD       2    /**< \brief 发送 CMD */
#define __SPI_EVT_SEND_DATA      3    /**< \brief 发送数据 */
#define __SPI_EVT_RECV_DATA      4    /**< \brief 接收数据 */
#define __SPI_EVT_SPI_DONE       5    /**< \brief SPI 传输完成 */

/** \brief 状态机内部状态切换 */
#define __SPI_NEXT_STATE(s, e)   \
    do {                         \
        p_this->spi_state = (s); \
        new_event = (e);         \
    } while(0)

/** \brief TPS08U 的 ID */
#define __TPS08U_ID    0x38535054

/** \brief 需要写入复位寄存器的值 */
#define __RESET_MAGIC  0x05fa50af

/** \brief 通道数量 */
#define __CHAN_NUM     8

/** \brief 参考电压 */
#define __VREF_MV     (2500 * 2)

/** \brief 转换位数 */
#define __ADC_BITS     24

/** \brief 片选有效至第一个时钟沿的时间间隔 */
#define __T1_DELAY_US        70     

/** \brief 命令和数据之间的时间间隔 */
#define __T2_DELAY_US        70     

/** \brief 数据传输完成至片选无效的时间间隔 */
#define __T3_DELAY_US        20     

/** \brief 读寄存器时片选无效至片选有效的时间间隔 */
#define __T4_READ_DELAY_US   40     

/** \brief 写寄存器时片选无效至片选有效的时间间隔 */
#define __T4_WRITE_DELAY_US  12000  

#define __DATA_ADDR(ch)      (0x00 + (ch))  /**< \brief 数据寄存器地址 */
#define __CHAN_ENABLE_ADDR    0x08          /**< \brief 通道使能寄存器地址 */
#define __CHAN_MODE_ADDR      0x09          /**< \brief 通道模式寄存器地址 */
#define __STATE_ADDR          0x0A          /**< \brief 系统状态寄存器地址 */
#define __RESET_ADDR          0x0B          /**< \brief 系统复位寄存器地址 */
#define __ID_ADDR             0x0C          /**< \brief ID 寄存器地址 */

/** \brief 将通道数据转为整形 */
#define __CHAN_DATA_TO_INT(data) (AW_BITS_GET(data, 0, 17) * 10) + \
                                 (AW_BITS_GET(data, 17, 15) * 1000000)

/*******************************************************************************
  本地函数声明
*******************************************************************************/

/** \brief 第三阶段初始化 */
aw_local void __tps08u_inst_connect (struct awbl_dev *p_this);

/** \brief 传感器服务获取方法 */
aw_local aw_err_t __sensor_server_get (struct awbl_dev *p_dev, void *p_arg);

/**
 * \brief SPI 传输状态机
 */
aw_local aw_err_t __spi_sm_event (struct awbl_tps08u_dev *p_this,
                                  uint32_t                event);

/**
 * \brief SPI 传输完成回调函数
 */
aw_local void __spi_callback (void *p_arg);

/**
 * \brief 延迟作业回调函数
 */
aw_local void __work_callback (void *p_arg);

/**
 * \brief 虚假的片选控制函数
 */
aw_local void __dummy_cs (int state);

/**
 * \brief 控制片选状态
 */
aw_local void __chip_select (aw_spi_device_t *p_dev, int state);

/**
 * \brief 读取寄存器
 */
aw_local aw_err_t __reg_read (struct awbl_dev *p_dev, uint8_t addr, void *p_buf);

/**
 * \brief 写寄存器
 */
aw_local aw_err_t __reg_write (struct awbl_dev *p_dev, 
                               uint8_t          addr, 
                               void            *p_buf);

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __sensor_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf);

/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __sensor_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result);

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __sensor_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result);

/**
 * \brief 配置传感器通道属性
 */
aw_local aw_err_t __sensor_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val);

/**
 * \brief 获取传感器通道属性
 */
aw_local aw_err_t __sensor_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val);

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t  __sensor_trigger_cfg (void                   *p_cookie,
                                         int                     id,
                                         uint32_t                flags,
                                         aw_sensor_trigger_cb_t  pfn_cb,
                                         void                   *p_arg);

/**
 * \brief 打开触发
 */
aw_local aw_err_t __sensor_trigger_on (void *p_cookie, int id);

/**
 * \brief 关闭触发
 */
aw_local aw_err_t __sensor_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/

/** \brief 适配传感器接口需要提供的方法 */
aw_local aw_const struct awbl_sensor_servfuncs __g_sensor_funcs = {
    __sensor_data_get,    /* 从指定通道获取采样数据 */
    __sensor_enable,      /* 使能传感器的某一通道 */
    __sensor_disable,     /* 禁能传感器通道 */
    __sensor_attr_set,    /* 配置传感器通道属性 */
    __sensor_attr_get,    /* 获取传感器通道属性 */
    __sensor_trigger_cfg, /* 设置触发，一个通道仅能设置一个触发回调函数 */
    __sensor_trigger_on,  /* 打开触发 */
    __sensor_trigger_off  /* 关闭触发 */
};

/** \brief 声明方法 */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

/** \brief 驱动提供的方法 */
aw_local struct awbl_dev_method __g_sensor_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __sensor_server_get},
    AWBL_METHOD_END
};

/** \brief 驱动入口点 */
aw_local aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_tps08u = {
    NULL,                    /* 第一阶段初始化 */
    NULL,                    /* 第二阶段初始化 */
    __tps08u_inst_connect    /* 第三阶段初始化 */
};

/** \brief 驱动注册信息 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_tps08u = {
    AWBL_VER_1,                   /* AWBus 版本号 */
    AWBL_BUSID_SPI,               /* 总线 ID */
    AWBL_TPS08U_NAME,      /* 驱动名 */
    &__g_awbl_drvfuncs_tps08u,    /* 驱动入口点 */
    &__g_sensor_methods[0],       /* 驱动提供的方法 */
    NULL                          /* 驱动探测函数 */
};

/*******************************************************************************
  本地函数定义
*******************************************************************************/

/** \brief 第三阶段初始化 */
aw_local void __tps08u_inst_connect (struct awbl_dev *p_dev)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);

    if (-1 == p_devinfo->cs_pin) {
        AW_ERRF(("TPS08U: cs pin is -1!\n"));
        return;
    }

    /* 初始化片选引脚 todo imx28x 暂未适配 aw_gpio_pin_request() */
#if 0 
    if (aw_gpio_pin_request(AWBL_TPS08U_NAME, &p_devinfo->cs_pin, 1) != AW_OK) {
        AW_ERRF(("TPS08U: cs pin %d request failed!\n", p_devinfo->cs_pin));
        return;
    }
#endif
    aw_gpio_pin_cfg(p_devinfo->cs_pin, AW_GPIO_OUTPUT_INIT_HIGH);

    /* 初始化设备描述结构 */
    aw_spi_mkdev(&p_this->spi_dev,
                  p_dev->p_devhcf->bus_index,
                  8,                    /* 字大小为 8-bit */
                  AW_SPI_MODE_1,        /* SPI 模式 1 */
                  p_devinfo->sclk_freq, /* SPI 速度 */
                  p_devinfo->cs_pin,    /* 片选引脚 */
                  __dummy_cs);          /* 无自定义的片选控制函数 */

    /* 设置 SPI 设备 */
    if (aw_spi_setup(&p_this->spi_dev) != AW_OK) {
        AW_ERRF(("TPS08U: spi setup failed!\n"));
        return;
    }

    /* 初始化传感器采样延迟作业 */
    aw_delayed_work_init(&p_this->sampling_work, __work_callback, p_dev);
}

/** \brief 传感器服务获取方法 */
aw_local aw_err_t __sensor_server_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_tps08u_dev           *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo       *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);
    aw_local awbl_sensor_type_info_t  type_info[__CHAN_NUM / 2]; /* 通道类型*/
    uint32_t                          i;

    /* 传感器服务常量信息*/
    aw_local const awbl_sensor_servinfo_t serv_info = {
        __CHAN_NUM,             /* 支持的通道数量 */
        type_info,              /* 传感器类型列表 */
        AW_NELEMENTS(type_info) /* 类型列表数目 */
    };

    struct awbl_sensor_service *p_serv = &p_this->sensor_service;

    /* 根据配置信息初始化通道类型 */
    for (i = 0; i < __CHAN_NUM / 2; i++) {
        type_info[i].num = 2;
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, i)) {
            type_info[i].type = AW_SENSOR_TYPE_VOLTAGE;
        } else {
            type_info[i].type = AW_SENSOR_TYPE_CURRENT;
        }
    }

    p_serv->p_servinfo  = &serv_info;
    p_serv->start_id    = p_devinfo->start_id;
    p_serv->p_next      = NULL;
    p_serv->p_cookie    = p_dev;
    p_serv->p_servfuncs = &__g_sensor_funcs;

    *(struct awbl_sensor_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief 获取指定地址寄存器的大小，单位为字节
 */
aw_local uint8_t __reg_size_get (uint8_t addr)
{
    uint8_t size = 0;

    if ((addr == __CHAN_ENABLE_ADDR) ||
        (addr == __CHAN_MODE_ADDR) ||
        (addr == __STATE_ADDR)){
        size = 1;
    } else if ((addr >= __DATA_ADDR(0)) &&
               (addr < __DATA_ADDR(__CHAN_NUM))){
        size = 4;
    } else if ((addr == __RESET_ADDR) ||
               (addr == __ID_ADDR)){
        size = 4;
    } else {
        size = 0;
    }

    return size;
}

/**
 * \brief SPI 传输状态机
 */
aw_local aw_err_t __spi_sm_event (struct awbl_tps08u_dev *p_this,
                                  uint32_t                event)
{
    uint32_t new_event = __SPI_EVT_NONE;

    while (1) {
        if (new_event != __SPI_EVT_NONE) {     /* 检查新事件是否来自内部 */
            event = new_event;
            new_event = __SPI_EVT_NONE;
        }

        switch (p_this->spi_state) {

        /* 空闲状态 */
        case __SPI_ST_IDLE:

            /* 如果为传输就绪事件，则跳转到片选有效状态 */
            if (__SPI_EVT_CS_VALID == event) {
                __SPI_NEXT_STATE(__SPI_ST_CS_VALID, __SPI_EVT_SEND_CMD);

                p_this->cmd = (0x80 | p_this->reg_addr);

                /* 第一个传输，发送 CMD */
                aw_spi_mktrans(&p_this->trans[0],  /* 传输描述结构 */
                               &p_this->cmd,       /* 发送缓冲区 */
                                NULL,              /* 接收缓冲区 */
                                1,                 /* 传输数量 */
                                0,                 /* 不影响片选 */
                                0,                 /* bpw 使用默认配置 */
                                0,                 /* udelay 无延时 */
                                0,                 /* speed 使用默认值 */
                                0);                /* 无 flag */

                /* 第二个传输，读、写数据 */
                aw_spi_mktrans(&p_this->trans[1],
                                p_this->is_read ? NULL : p_this->p_buf,
                                p_this->is_read ? p_this->p_buf : NULL,
                                __reg_size_get(p_this->reg_addr),
                                0,
                                0,
                                0,
                                0,
                                0);

                /* 初始化消息 */
                aw_spi_msg_init(&p_this->msg[0], __spi_callback, p_this);
                aw_spi_msg_init(&p_this->msg[1], __spi_callback, p_this);
                aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]);
                aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]);

                /* 片选有效 */
                __chip_select(&p_this->spi_dev, 0);
            } else {
                return -AW_EINVAL;
            }
            break;

        /* 片选有效 */
        case __SPI_ST_CS_VALID:
            if (__SPI_EVT_SEND_CMD == event) {
                __SPI_NEXT_STATE(__SPI_ST_CMD, __SPI_EVT_NONE);

                aw_udelay(__T1_DELAY_US);

                /* 发送 CMD */
                if (aw_spi_async(&p_this->spi_dev, &p_this->msg[0]) != AW_OK) {
                    return -AW_EIO;
                }
            }
            break;

        /* 传输 CMD */
        case __SPI_ST_CMD:
            if (__SPI_EVT_SPI_DONE == event) {
                __SPI_NEXT_STATE(__SPI_ST_DATA, __SPI_EVT_NONE);

                aw_udelay(__T2_DELAY_US);

                /* 发送数据 */
                if (aw_spi_async(&p_this->spi_dev, &p_this->msg[1]) != AW_OK) {
                    return -AW_EIO;
                }
            }
            break;

        /* 传输数据 */
        case __SPI_ST_DATA:
            if (__SPI_EVT_SPI_DONE == event) {
                __SPI_NEXT_STATE(__SPI_ST_IDLE, __SPI_EVT_NONE);

                aw_udelay(__T3_DELAY_US);

                /* 片选无效 */
                __chip_select(&p_this->spi_dev, 1);
                if (p_this->is_read) {
                    aw_udelay(__T4_READ_DELAY_US);
                } else {
                    aw_udelay(__T4_WRITE_DELAY_US);
                }
            }
            break;

        default:
            break;
        }

        /* 没有来自内部的事件，跳出 */
        if (new_event == __SPI_EVT_NONE) {
            break;
        }
    }

    return AW_OK;
}

/**
 * \brief SPI 传输完成回调函数
 */
aw_local void __spi_callback (void *p_arg)
{
    struct awbl_tps08u_dev *p_this = (struct awbl_tps08u_dev *)p_arg;

    __spi_sm_event(p_this, __SPI_EVT_SPI_DONE);
}

/**
 * \brief 延迟作业回调函数
 */
aw_local void __work_callback (void *p_arg)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_arg;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_arg);
    int32_t                     slope;
    uint32_t                    i;
    uint8_t                     id;

    if (!p_this->is_busy) {
        return;
    }

    /* 重新启动延迟作业 */
    if (0 != p_this->sampling_rate) {
        aw_delayed_work_start(&p_this->sampling_work, 
                              1000 / p_this->sampling_rate);
    }

    /* 只有 SPI 状态为空闲时才进行操作 */
    if (__SPI_ST_IDLE != p_this->spi_state) {
        return;
    }

    if (0 == p_this->read_state) { /* 读取状态寄存器 */

        /* 判断 ADC 数据是否更新 */
        if (p_this->reg_state & AWBL_TPS08U_STATE_DATA_UPDATE) {
            if (p_this->reg_state == AWBL_TPS08U_STATE_DATA_UPDATE) {
                p_this->read_state++;
            }
            p_this->reg_state = 0;
        } else {
            p_this->is_read = AW_TRUE;
            p_this->reg_addr = __STATE_ADDR;
            p_this->p_buf = &p_this->reg_state;
            __spi_sm_event(p_this, __SPI_EVT_CS_VALID);
        }
    }

    if (1 == p_this->read_state) { /* 读取通道数据寄存器 */
        for (i = p_this->current_chan; i < __CHAN_NUM; i++) {
            if (AW_BIT_ISSET(p_this->reg_chan_enable, i)) {
                p_this->current_chan = i;
                break;
            }
        }
        if (__CHAN_NUM <= i) {
            p_this->current_chan = 0;
            p_this->read_state++;
        } else {
            p_this->is_read = AW_TRUE;
            p_this->reg_addr = __DATA_ADDR(p_this->current_chan);
            p_this->p_buf = &p_this->reg_data[p_this->current_chan];
            __spi_sm_event(p_this, __SPI_EVT_CS_VALID);
            p_this->current_chan++;
        }
    }

    if (2 == p_this->read_state) { /* 填充缓冲区 */
        p_this->read_state = 0;

        for (i = 0; i < __CHAN_NUM; i++) {

            /* 判断通道是否使能以及触发是否打开 */
            if ((!AW_BIT_ISSET(p_this->reg_chan_enable, i)) ||
                (!AW_BIT_ISSET(p_this->trigger, i))) {
                continue;
            }

            /* 数据准备就绪触发 */
            if (AW_SENSOR_TRIGGER_DATA_READY & p_this->flags[i]) {

                /* 调用触发器回调函数 */
                if (NULL != p_this->pfn_cb[i]) {
                    p_this->pfn_cb[i](p_this->p_arg[i],
                                      AW_SENSOR_TRIGGER_DATA_READY);
                }
            }

            /* 门限触发 */
            if (AW_SENSOR_TRIGGER_THRESHOLD & p_this->flags[i]) {

                /* 判断是否小于下门限 */
                if (__CHAN_DATA_TO_INT(p_this->reg_data[i]) + 
                    p_this->offset[i] < p_this->threshold_lower[i]) {
                    if (p_this->threshold_type[i] != 
                        AW_SENSOR_ATTR_THRESHOLD_LOWER) {
                        p_this->threshold_type[i] = 
                            AW_SENSOR_ATTR_THRESHOLD_LOWER;
                        p_this->threshold_cnt[i] = 1;
                    } else {
                        p_this->threshold_cnt[i]++;
                    }

                /* 判断是否大于上门限 */
                } else if (__CHAN_DATA_TO_INT(p_this->reg_data[i]) + 
                           p_this->offset[i] > p_this->threshold_upper[i]) {
                    if (p_this->threshold_type[i] != 
                        AW_SENSOR_ATTR_THRESHOLD_UPPER) {
                        p_this->threshold_type[i] = 
                            AW_SENSOR_ATTR_THRESHOLD_UPPER;
                        p_this->threshold_cnt[i] = 1;
                    } else {
                        p_this->threshold_cnt[i]++;
                    }
                } else {
                    p_this->threshold_cnt[i] = 0;
                }

                /* 判断是否达到门限次数 */
                if (p_this->threshold_cnt[i] >= p_this->duration_data[i]) {
                    p_this->threshold_cnt[i] = 0;

                    /* 调用触发器回调函数 */
                    if (NULL != p_this->pfn_cb[i]) {
                        p_this->pfn_cb[i](p_this->p_arg[i],
                                          AW_SENSOR_TRIGGER_THRESHOLD);
                    }
                }
            }

            /* 增量（斜率）触发 */
            if (AW_SENSOR_TRIGGER_SLOPE & p_this->flags[i]) {

                /* 计算增量 */
                slope = ((int32_t)__CHAN_DATA_TO_INT(p_this->reg_data[i])) -
                        ((int32_t)__CHAN_DATA_TO_INT(p_this->reg_data_last[i]));

                if (((0 < p_this->threshold_slope[i]) &&
                    (slope > p_this->threshold_slope[i])) ||
                    ((0 >= p_this->threshold_slope[i]) &&
                    (slope <= p_this->threshold_slope[i]))) {
                    p_this->slope_cnt[i]++;
                } else {
                    p_this->slope_cnt[i] = 0;
                }

                /* 判断是否达到门限次数 */
                if (p_this->slope_cnt[i] >= p_this->duration_slope[i]) {
                    p_this->slope_cnt[i] = 0;

                    /* 调用触发器回调函数 */
                    if (NULL != p_this->pfn_cb[i]) {
                        p_this->pfn_cb[i](p_this->p_arg[i],
                                          AW_SENSOR_TRIGGER_SLOPE);
                    }
                }

                /* 保存历史通道数据 */
                p_this->reg_data_last[i] = p_this->reg_data[i];
            }
        }
    }
}

/**
 * \brief 虚假的片选控制函数
 */
aw_local void __dummy_cs ( int state)
{
    ; /* VOID */
}

/**
 * \brief 控制片选状态
 */
aw_local void __chip_select (aw_spi_device_t *p_dev, int state)
{
    aw_gpio_set(p_dev->cs_pin, state);
}

/**
 * \brief 读取寄存器
 */
aw_local aw_err_t __reg_read (struct awbl_dev *p_dev, uint8_t addr, void *p_buf)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);
    aw_err_t                    err       = AW_OK;
    uint8_t                     cmd       = (0x80 | addr);

    if (p_this->is_busy) {
        return -AW_EBUSY;
    }

    /* 第一个传输，发送 CMD */
    aw_spi_mktrans(&p_this->trans[0],  /* 传输描述结构 */
                   &cmd,               /* 发送缓冲区 */
                    NULL,              /* 接收缓冲区 */
                    1,                 /* 传输数量 */
                    0,                 /* 不影响片选 */
                    0,                 /* bpw 使用默认配置 */
                    0,                 /* udelay 无延时 */
                    0,                 /* speed 使用默认值 */
                    0);                /* 无 flag */

    /* 第二个传输，读取数据 */
    aw_spi_mktrans(&p_this->trans[1],     /* 传输描述结构 */
                    NULL,                 /* 发送缓冲区 */
                    p_buf,                /* 接收缓冲区 */
                    __reg_size_get(addr), /* 传输数量 */
                    0,                    /* 不影响片选 */
                    0,                    /* bpw 使用默认配置 */
                    0,                    /* udelay 无延时 */
                    0,                    /* speed 使用默认值 */
                    0);                   /* 无 flag */

    aw_spi_msg_init(&p_this->msg[0], NULL, NULL);              /* 初始化消息 */
    aw_spi_msg_init(&p_this->msg[1], NULL, NULL);              /* 初始化消息 */
    
    /* 添加第 1 个传输 */
    aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]); 
    
    /* 添加第 2 个传输 */
    aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]); 

    /* 片选有效 */
    __chip_select(&p_this->spi_dev, 0);
    aw_udelay(__T1_DELAY_US);

    /* 传输 CMD */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[0]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T2_DELAY_US);

    /* 传输数据 */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[1]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T3_DELAY_US);

    /* 片选无效 */
    __chip_select(&p_this->spi_dev, 1);
    aw_udelay(__T4_READ_DELAY_US);

    return AW_OK;
}

/**
 * \brief 写寄存器
 */
aw_local aw_err_t __reg_write (struct awbl_dev *p_dev, uint8_t addr, void *p_buf)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);
    aw_err_t                    err       = AW_OK;
    uint8_t                     cmd       = addr;

    if (p_this->is_busy) {
        return -AW_EBUSY;
    }

    /* 第一个传输，发送 CMD */
    aw_spi_mktrans(&p_this->trans[0],  /* 传输描述结构 */
                   &cmd,               /* 发送缓冲区 */
                    NULL,              /* 接收缓冲区 */
                    1,                 /* 传输数量 */
                    0,                 /* 不影响片选 */
                    0,                 /* bpw 使用默认配置 */
                    0,                 /* udelay 无延时 */
                    0,                 /* speed 使用默认值 */
                    0);                /* 无 flag */

    /* 第二个传输，写数据 */
    aw_spi_mktrans(&p_this->trans[1],     /* 传输描述结构 */
                    p_buf,                /* 发送缓冲区 */
                    NULL,                 /* 接收缓冲区 */
                    __reg_size_get(addr), /* 传输数量 */
                    0,                    /* 不影响片选 */
                    0,                    /* bpw 使用默认配置 */
                    0,                    /* udelay 无延时 */
                    0,                    /* speed 使用默认值 */
                    0);                   /* 无 flag */

    aw_spi_msg_init(&p_this->msg[0], NULL, NULL);              /* 初始化消息 */
    aw_spi_msg_init(&p_this->msg[1], NULL, NULL);              /* 初始化消息 */
    
    /* 添加第 1 个传输 */
    aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]); 
    
    /* 添加第 2 个传输 */
    aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]); 

    /* 片选有效 */
    __chip_select(&p_this->spi_dev, 0);
    aw_udelay(__T1_DELAY_US);

    /* 传输 CMD */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[0]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T2_DELAY_US);

    /* 传输数据 */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[1]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T3_DELAY_US);

    /* 片选无效 */
    __chip_select(&p_this->spi_dev, 1);
    aw_udelay(__T4_WRITE_DELAY_US);

    return AW_OK;
}

/**
 * \brief 从指定通道获取采样数据
 */
aw_local aw_err_t __sensor_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         i;
    int                         chan;
    uint32_t                    reg;
    aw_err_t                    err;

    for (i = 0; i < num; i++) {

        /* 获取实际通道号，并判断该通道是否属于当前设备 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* 如果设备忙，证明正在自动采样，直接读取数据即可 */
        if (p_this->is_busy) {
            p_buf[i].val = __CHAN_DATA_TO_INT(p_this->reg_data[chan]);
            if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* 电压通道 */
                p_buf[i].unit = AW_SENSOR_UNIT_MICRO;
            } else { /* 电流通道 */
                p_buf[i].unit = AW_SENSOR_UNIT_NANO;
            }
            continue;
        }

        /* 如果设备空闲，证明没有自动采样，读取寄存器 */
        err = __reg_read(&p_this->dev, __DATA_ADDR(chan), &reg);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_INVALID(&p_buf[i], err);
            return err;
        }
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* 电压通道 */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_buf[i].unit = AW_SENSOR_UNIT_NANO;
        }
        p_buf[i].val  = __CHAN_DATA_TO_INT(reg);
    }

    return AW_OK;
}

/**
 * \brief 使能传感器的某一通道
 */
aw_local aw_err_t __sensor_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         i;
    int                         chan;
    aw_err_t                    err;

    for (i = 0; i < num; i++) {

        /* 获取实际通道号，并判断该通道是否属于当前设备 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* 使能相应通道 */
        err = awbl_tps08u_chan_enable_set(p_this->dev.p_devhcf->unit,
                                                 chan + 1,
                                                 1);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* 电压通道 */

            /* 配置通道模式 */
            err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                                   chan + 1,
                                                   0);
            if (AW_OK != err) {
                AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
                return err;
            }
        } else { /* 电流通道 */

            /* 配置通道模式 */
            err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                                   chan + 1,
                                                   1);
            if (AW_OK != err) {
                AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
                return err;
            }
        }

        /* 设置返回值 */
        AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], AW_OK);
    }

    return AW_OK;
}

/**
 * \brief 禁能传感器通道
 */
aw_local aw_err_t __sensor_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         i;
    int                         chan;
    aw_err_t                    err;

    for (i = 0; i < num; i++) {

        /* 获取实际通道号，并判断该通道是否属于当前设备 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* 失能相应通道 */
        err = awbl_tps08u_chan_enable_set(p_this->dev.p_devhcf->unit,
                                                 chan + 1,
                                                 0);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        /* 通道配置为电压通道 */
        err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                               chan + 1,
                                               0);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        /* 设置返回值 */
        AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], AW_OK);
    }

    return AW_OK;
}

/**
 * \brief 配置传感器通道属性
 */
aw_local aw_err_t __sensor_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    aw_err_t                    err;
    aw_sensor_val_t             val = *p_val;

    /* 获取实际通道号 */
    id = id - p_devinfo->start_id;

    switch (attr) {

    /* 采样频率，每秒采样的次数 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:

        /* 转换单位 */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);

        if (0 == val.val) {
            aw_delayed_work_stop(&p_this->sampling_work);
            p_this->is_busy = AW_FALSE;
        } else {

            /* 获取通道使能配置 */
            err = awbl_tps08u_chan_enable_get(p_this->dev.p_devhcf->unit,
                                                     0,
                                                    &p_this->reg_chan_enable);
            if ((AW_OK != err) || (0 == p_this->reg_chan_enable)) {
                return err;
            }

            /* 获取通道模式配置 */
            err = awbl_tps08u_chan_mode_get(p_this->dev.p_devhcf->unit,
                                                   0,
                                                  &p_this->reg_chan_mode);
            if (AW_OK != err) {
                return err;
            }

            p_this->current_chan = 0;
            p_this->is_busy = AW_TRUE;
            aw_delayed_work_start(&p_this->sampling_work, 1000 / val.val);
        }

        p_this->sampling_rate = val.val;
        break;

    /* 满量程值 */
    case AW_SENSOR_ATTR_FULL_SCALE:
        return -AW_EINVAL;

    /* 偏移值，用于静态校准传感器数据，传感器数据 = 实际采样值 + offset */
    case AW_SENSOR_ATTR_OFFSET:

        /* 转换单位 */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* 电流通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->offset[id] = val.val;
        break;

    /* 下门限，用于门限触发模式，数据低于该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:

        /* 转换单位 */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* 电流通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_lower[id] = val.val;
        break;

    /* 上门限，用于门限触发模式，数据高于该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:

        /* 转换单位 */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* 电流通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_upper[id] = val.val;
        break;

    /* 增量门限，用于增量触发，数据变化过快（斜率过大），超过该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_SLOPE:

        /* 转换单位 */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* 电流通道 */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_slope[id] = val.val;
        break;

    /* 采样数据持续次数，用于门限触发模式 */
    case AW_SENSOR_ATTR_DURATION_DATA:

        /* 转换单位 */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);
        p_this->duration_data[id] = val.val;
        break;

    /* 增量持续次数，用于增量触发模式 */
    case AW_SENSOR_ATTR_DURATION_SLOPE:

        /* 转换单位 */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);
        p_this->duration_slope[id] = val.val;
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief 获取传感器通道属性
 */
aw_local aw_err_t __sensor_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);

    /* 获取实际通道号 */
    id = id - p_devinfo->start_id;

    switch (attr) {

    /* 采样频率，每秒采样的次数 */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val = p_this->sampling_rate;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    /* 满量程值 */
    case AW_SENSOR_ATTR_FULL_SCALE:
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            p_val->val = 5000000;
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_val->val = 20000000;
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* 偏移值，用于静态校准传感器数据，传感器数据 = 实际采样值 + offset */
    case AW_SENSOR_ATTR_OFFSET:
        p_val->val = p_this->offset[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* 下门限，用于门限触发模式，数据低于该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val = p_this->threshold_lower[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* 上门限，用于门限触发模式，数据高于该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val = p_this->threshold_upper[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* 增量门限，用于增量触发，数据变化过快（斜率过大），超过该值时触发 */
    case AW_SENSOR_ATTR_THRESHOLD_SLOPE:
        p_val->val = p_this->threshold_slope[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* 电压通道 */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* 电流通道 */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* 采样数据持续次数，用于门限触发模式 */
    case AW_SENSOR_ATTR_DURATION_DATA:
        p_val->val = p_this->duration_data[id];
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    /* 增量持续次数，用于增量触发模式 */
    case AW_SENSOR_ATTR_DURATION_SLOPE:
        p_val->val = p_this->duration_slope[id];
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 */
aw_local aw_err_t  __sensor_trigger_cfg (void                   *p_cookie,
                                         int                     id,
                                         uint32_t                flags,
                                         aw_sensor_trigger_cb_t  pfn_cb,
                                         void                   *p_arg)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         key;

    /* 获取实际通道号 */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    p_this->flags[id] = flags;
    p_this->pfn_cb[id] = pfn_cb;
    p_this->p_arg[id] = p_arg;

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/**
 * \brief 打开触发
 */
aw_local aw_err_t __sensor_trigger_on (void *p_cookie, int id)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         key;

    /* 获取实际通道号 */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    AW_BIT_SET(p_this->trigger, id);

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/**
 * \brief 关闭触发
 */
aw_local aw_err_t __sensor_trigger_off (void *p_cookie, int id)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         key;

    /* 获取实际通道号 */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    AW_BIT_CLR(p_this->trigger, id);

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/*******************************************************************************
  外部函数定义
*******************************************************************************/

/**
 * \brief 将 TPS08U 驱动注册到 AWBus 子系统中
 *
 * \note 本函数中禁止调用操作系统 API
 */
void awbl_tps08u_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_tps08u);
}

/**
 * \brief 检测与 TPS08U 的通信是否正常
 */
aw_err_t awbl_tps08u_detect (int id)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint32_t                tps08u_id;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    /* 读 ID 寄存器 */
    err = __reg_read(&p_this->dev, __ID_ADDR, &tps08u_id);
    if (err != AW_OK) {
        return err;
    }

    /* 判断 ID 是否正确 */
    if (tps08u_id != __TPS08U_ID) {
        return -AW_ENODEV;
    }

    return AW_OK;
}

/**
 * \brief 获取 TPS08U 使能的通道
 */
aw_err_t awbl_tps08u_chan_enable_get (int      id,
                                      uint8_t  chan,
                                      uint8_t *p_chan_enable)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint8_t                 reg;

    if ((NULL == p_chan_enable) || (chan > __CHAN_NUM)) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (awbl_tps08u_detect(id) != AW_OK) {
        return -AW_ENODEV;
    }

    err = __reg_read(&p_this->dev, __CHAN_ENABLE_ADDR, &reg);
    if (err != AW_OK) {
        return err;
    }

    if (0 == chan) {
        *p_chan_enable = reg;
    } else {
        *p_chan_enable = AW_BIT_GET(reg, chan - 1);
    }

    return AW_OK;
}

/**
 * \brief 配置 TPS08U 使能的通道
 */
aw_err_t awbl_tps08u_chan_enable_set (int id, uint8_t chan, uint8_t chan_enable)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint8_t                 reg    = chan_enable;

    if (chan > __CHAN_NUM) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (chan != 0) {
        err = awbl_tps08u_chan_enable_get(id, 0, &reg);
        if (err != AW_OK) {
            return err;
        }
        AW_BIT_MODIFY(reg, chan - 1, chan_enable);
    } else {
        if (awbl_tps08u_detect(id) != AW_OK) {
            return -AW_ENODEV;
        }
    }

    err = __reg_write(&p_this->dev, __CHAN_ENABLE_ADDR, &reg);
    if (err != AW_OK) {
        return err;
    }

    return AW_OK;
}

/**
 * \brief 获取 TPS08U 通道模式
 */
aw_err_t awbl_tps08u_chan_mode_get (int id, uint8_t  chan, uint8_t *p_chan_mode)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint8_t                 reg;

    if ((NULL == p_chan_mode) || (chan > __CHAN_NUM)) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (awbl_tps08u_detect(id) != AW_OK) {
        return -AW_ENODEV;
    }

    err = __reg_read(&p_this->dev, __CHAN_MODE_ADDR, &reg);
    if (err != AW_OK) {
        return err;
    }

    if (0 == chan) {
        *p_chan_mode = reg;
    } else {
        *p_chan_mode = AW_BIT_GET(reg, (chan - 1) / 2);
    }

    return AW_OK;
}

/**
 * \brief 配置 TPS08U 通道模式
 */
aw_err_t awbl_tps08u_chan_mode_set (int id, uint8_t chan, uint8_t chan_mode)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint8_t                 reg    = chan_mode;

    if (chan > __CHAN_NUM) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (chan != 0) {
        err = awbl_tps08u_chan_mode_get(id, 0, &reg);
        if (err != AW_OK) {
            return err;
        }
        AW_BIT_MODIFY(reg, (chan - 1) / 2, chan_mode);
    } else {
        if (awbl_tps08u_detect(id) != AW_OK) {
            return -AW_ENODEV;
        }
    }

    err = __reg_write(&p_this->dev, __CHAN_MODE_ADDR, &reg);
    if (err != AW_OK) {
        return err;
    }

    return AW_OK;
}

/**
 * \brief 获取 TPS08U 通道数据
 */
aw_err_t awbl_tps08u_chan_data_get (int id, uint8_t chan, uint32_t *p_chan_data)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint32_t                reg;
    int32_t                 i;

    if ((NULL == p_chan_data) || (chan > __CHAN_NUM)) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (awbl_tps08u_detect(id) != AW_OK) {
        return -AW_ENODEV;
    }

    if (0 == chan) {
        for (i = 0; i < __CHAN_NUM; i++) {
            err = __reg_read(&p_this->dev, __DATA_ADDR(i), &reg);
            if (AW_OK == err) {
                *p_chan_data++ = reg;
            }
        }
    } else {
        err = __reg_read(&p_this->dev, __DATA_ADDR(chan - 1), &reg);
        if (AW_OK == err) {
            *p_chan_data = reg;
        }
    }

    return AW_OK;
}

/**
 * \brief 获取 TPS08U 状态
 */
aw_err_t awbl_tps08u_state_get (int id, uint8_t *p_state)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);

    if (NULL == p_state) {
        return -AW_EINVAL;
    }

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (awbl_tps08u_detect(id) != AW_OK) {
        return -AW_ENODEV;
    }

    return __reg_read(&p_this->dev, __STATE_ADDR, p_state);
}

/**
 * \brief 复位 TPS08U
 */
aw_err_t awbl_tps08u_reset (int id)
{
    struct awbl_tps08u_dev *p_this      = __ID_GET_TPS08U_DEV(id);
    uint32_t                reset_magic = __RESET_MAGIC;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    if (awbl_tps08u_detect(id) != AW_OK) {
        return -AW_ENODEV;
    }

    return __reg_write(&p_this->dev, __RESET_ADDR, &reset_magic);
}

/* end of file */
