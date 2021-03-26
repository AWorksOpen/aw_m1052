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
 * \brief gpio i2c driver
 *
 * AWBus i2c bus controller simulated by GPIO
 *
 * \internal
 * \par modification history:
 * - 1.11 18-11-02  sls  update i2c arch
 * - 1.10 13-08-26  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "aw_gpio.h"
#include "aw_psp_delay.h"
#include "aw_common.h"
#include "awbus_lite.h"
#include "awbl_plb.h"
#include "awbl_i2cbus.h"
#include "driver/busctlr/awbl_gpio_i2c.h"


/*******************************************************************************
  macro operate
*******************************************************************************/

/* get gpio i2c device instance */
#define __GPI2C_GPI2C_DEV_DECL(p_gpi2c, p_dev) \
    struct awbl_gpio_i2c_dev *p_gpi2c = \
        (struct awbl_gpio_i2c_dev *)(p_dev)

/* get gpio i2c device infomation */
#define __GPI2C_GPI2C_DEVINFO_DECL(p_gpi2c_dvif, p_dev) \
    struct awbl_gpio_i2c_devinfo *p_gpi2c_dvif = \
        (struct awbl_gpio_i2c_devinfo *)AWBL_DEVINFO_GET(p_dev)

/* get i2c master instance */
#define __GPI2C_MASTER_DEV_DECL(p_master, p_dev) \
    struct awbl_i2c_master *p_master = (struct awbl_i2c_master *)(p_dev)

/* get i2c master infomation */
#define __GPI2C_MASTER_DEVINFO_DECL(p_mdvif, p_dev) \
    struct awbl_i2c_master_devinfo *p_mdvif = \
        (struct awbl_i2c_master_devinfo *)AWBL_DEVINFO_GET(p_dev)


#define __GPI2C_USER_DEALY(p_dev)   __gpi2c_bus_delay(p_dev);

#define __GPI2C_SCL_HIGH(p_devinfo)     aw_gpio_set(p_devinfo->pin_scl, 1)
#define __GPI2C_SCL_LOW(p_devinfo)      aw_gpio_set(p_devinfo->pin_scl, 0)

#define __GPI2C_SDA_HIGH(p_devinfo)     aw_gpio_set(p_devinfo->pin_sda, 1)
#define __GPI2C_SDA_LOW(p_devinfo)      aw_gpio_set(p_devinfo->pin_sda, 0)

#define __GPI2C_SDA_INPUT(p_devinfo)    \
    p_devinfo->pfunc_sda_set_input(p_devinfo)

#define __GPI2C_SDA_OUTPUT(p_devinfo)   \
    p_devinfo->pfunc_sda_set_output(p_devinfo)


#define __GPI2C_SDA_VAL_GET(p_devinfo)  aw_gpio_get(p_devinfo->pin_sda)


/*******************************************************************************
  forward declarations
*******************************************************************************/
aw_local void __gpi2c_inst_init1(awbl_dev_t *p_dev);
aw_local void __gpi2c_inst_init2(awbl_dev_t *p_dev);
aw_local void __gpi2c_inst_connect(awbl_dev_t *p_dev);

aw_local void __gpi2c_hard_init(struct awbl_gpio_i2c_dev  *p_gpi2c);

aw_err_t __gpi2c_write (struct awbl_i2c_master *p_master,
                        uint16_t                chip_addr,
                        struct awbl_trans_buf  *p_trans_buf,
                        uint32_t                trans_buf_num,
                        uint8_t                 stop_set,
                        uint16_t                dr_flag);

aw_err_t  __gpi2c_read (struct awbl_i2c_master *p_master,
                        uint16_t                chip_addr,
                        struct awbl_trans_buf  *p_trans_buf,
                        uint8_t                 start_ctl,
                        uint16_t                dr_flag);

aw_err_t  __gpi2c_set_cfg (struct awbl_i2c_master *p_master,
                           struct aw_i2c_config   *p_cfg);


aw_err_t __gpi2c_get_cfg (struct awbl_i2c_master *p_master,
                          struct aw_i2c_config   *p_cfg);

/*******************************************************************************
  locals
*******************************************************************************/

/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_gpi2c_drvfuncs = {
    __gpi2c_inst_init1,
    __gpi2c_inst_init2,
    __gpi2c_inst_connect
};


/* driver registration (must defined as aw_const) */
aw_local aw_const awbl_plb_drvinfo_t __g_gpi2c_drv_registration = {
    {
        AWBL_VER_1,                             /* awb_ver */
        AWBL_BUSID_PLB | AWBL_DEVID_BUSCTRL,    /* bus_id */
        AWBL_GPIO_I2C_NAME,                     /* p_drvname */
        &__g_gpi2c_drvfuncs,                    /* p_busfuncs */
        NULL,                                   /* p_methods */
        NULL                                    /* pfunc_drv_probe */
    }
};


/*  device information submited by driver (must defined as aw_const) */
aw_local aw_const struct awbl_i2c_master_devinfo2 __g_gpi2c_devinfo2 = {
    AWBL_FEATURE_I2C_7BIT |
    AWBL_FEATURE_I2C_IGNORE_NAK,

    __gpi2c_write,
    __gpi2c_read,
    __gpi2c_set_cfg,
    __gpi2c_get_cfg
};


/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief bus delay
 *
 * \param[in] p_devinfo     device information
 */
aw_local void __gpi2c_bus_delay (struct awbl_gpio_i2c_dev *p_dev)
{
    uint32_t u_delay = 0;

    /* 设置GPIO理论最大时钟是500K */
    if (p_dev->speed >= 500000) {
        u_delay = 1;
    } else {
        u_delay = 500000 / p_dev->speed;
    }

    aw_udelay(u_delay);
}


/**
 * \brief Generate START Condition
 *
 * \param[in] p_devinfo     device information
 */
aw_local void __gpi2c_start (struct awbl_gpio_i2c_dev *p_dev)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);

    __GPI2C_SDA_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SCL_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SDA_LOW(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SCL_LOW(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    return;
}


/**
 * \brief Generate STOP Condition
 *
 * \param[in] p_devinfo     device information
 */
aw_local void __gpi2c_stop (struct awbl_gpio_i2c_dev *p_dev)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);
    __GPI2C_SDA_LOW(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SCL_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SDA_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    return;
}


/**
 * \brief ACK get
 *
 * \param[in] p_devinfo     device information
 *
 * \retval AW_TRUE   ACKed
 * \retval AW_FALSE  not ACK
 */
aw_local aw_bool_t __gpi2c_ack_get (struct awbl_gpio_i2c_dev *p_dev)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);
    aw_bool_t acked = AW_FALSE;

    __GPI2C_SDA_HIGH(p_devinfo);
    __GPI2C_SDA_INPUT(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SCL_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    acked = __GPI2C_SDA_VAL_GET(p_devinfo) ? AW_FALSE : AW_TRUE;

    __GPI2C_SCL_LOW(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);
    __GPI2C_SDA_OUTPUT(p_devinfo);

    return acked;
}


/**
 * \brief ACK get
 *
 * \param[in] p_devinfo     device information
 * \param[in] ack           AW_TRUE - ACK AW_FALSE - no ACK
 */
aw_local void __gpi2c_ack_put (struct awbl_gpio_i2c_dev *p_dev,
                               aw_bool_t                    ack)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);
    __GPI2C_SDA_OUTPUT(p_devinfo);

    ack ?  __GPI2C_SDA_LOW(p_devinfo)
                :  __GPI2C_SDA_HIGH(p_devinfo);

    /* __GPI2C_USER_DEALY(p_dev); */

    __GPI2C_SCL_HIGH(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

    __GPI2C_SCL_LOW(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);

#if 0
    __GPI2C_SDA_HIGH(p_devinfo);
    __GPI2C_SDA_INPUT(p_devinfo);
    __GPI2C_USER_DEALY(p_dev);
#endif
}


/**
 * \brief Write Byte
 *
 * \param[in] p_devinfo     device information
 *
 * \retval AW_TRUE     ACKed
 * \retval AW_FALSE    no ACK
 */
aw_local aw_bool_t __gpi2c_write_byte (struct awbl_gpio_i2c_dev *p_dev,
                                       uint8_t                   data)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);
    int i;

    for (i = 7; i >= 0; i--) {
        AW_BIT_ISSET(data, i) ? __GPI2C_SDA_HIGH(p_devinfo)
                              : __GPI2C_SDA_LOW(p_devinfo);
        __GPI2C_USER_DEALY(p_dev);

        __GPI2C_SCL_HIGH(p_devinfo);
        __GPI2C_USER_DEALY(p_dev);

        __GPI2C_SCL_LOW(p_devinfo);
        __GPI2C_USER_DEALY(p_dev);
    }

    return __gpi2c_ack_get(p_dev);
}


/**
 * \brief Read Byte
 *
 * \param[in] p_devinfo     device information
 * \param[in] ack           AW_TRUE-ACK  AW_FALSE- no ACK
 *
 * \retval data
 */
aw_local uint8_t __gpi2c_read_byte (struct awbl_gpio_i2c_dev *p_dev,
                                    aw_bool_t                 ack)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_devinfo, p_dev);
    int i;
    uint8_t data;

    /* __GPI2C_SDA_HIGH(p_devinfo); */
    __GPI2C_SDA_INPUT(p_devinfo);

    data = 0;
    for (i = 7; i >= 0; i--) {

        __GPI2C_SCL_HIGH(p_devinfo);
        __GPI2C_USER_DEALY(p_dev);

        if (__GPI2C_SDA_VAL_GET(p_devinfo)) {
            data |= 1 << i;
        };

        __GPI2C_SCL_LOW(p_devinfo);
        __GPI2C_USER_DEALY(p_dev);
    }

    __gpi2c_ack_put(p_dev, ack);

    /* __GPI2C_SDA_OUTPUT(p_devinfo); */

    return data;
}


aw_err_t __gpi2c_write (struct awbl_i2c_master *p_master,
                        uint16_t                chip_addr,
                        struct awbl_trans_buf  *p_trans_buf,
                        uint32_t                trans_buf_num,
                        uint8_t                 stop_ctl,
                        uint16_t                dr_flag)
{
    __GPI2C_GPI2C_DEV_DECL(p_this, p_master);
    uint8_t   *p_tem_buf = NULL;
    aw_bool_t acked = AW_FALSE;
    uint32_t i   = 0, j = 0;
    uint32_t late_tick = 0;
    uint32_t temp_tick = 0;
    uint32_t nbytes    = 0;
    aw_bool_t   timeout_check = AW_FALSE;

    /* todo 发送了开始没有检测总线是否是处理忙的状态 */
    __gpi2c_start(p_this);

    /* wirte */
    acked = __gpi2c_write_byte(p_this, (chip_addr << 1));

    /* not ack */
    if (!acked) {
        return -AW_ENODEV;
    }

    if ((p_this->timeout != -1) || (p_this->timeout == 0)) {
        timeout_check = AW_TRUE;
        late_tick = aw_sys_tick_get();
    }

    for (j = 0; j < trans_buf_num; j++) {

        nbytes = p_trans_buf->buf_size;
        p_tem_buf = p_trans_buf->p_buf;
        /* 发送数据  */
        for (i = 0; i < nbytes; i++) {

            acked = __gpi2c_write_byte(p_this, p_tem_buf[i]);

            /* 数据无应答 */
            if (!acked) {
                return -AW_ENODEV;
            }

            /* 判断码间超时 */
            if (timeout_check) {
                temp_tick = aw_sys_tick_get() - late_tick;

                if (aw_ticks_to_ms(temp_tick) > p_this->timeout) {
                    /* 给个停止信号，并返回错误 */
                    return -AW_ENODEV;
                }
            }
        }

        p_trans_buf++;
    }

    /* 总线空闲时间 */
    if (stop_ctl == AWBL_I2C_WRITE_STOP) {
        __gpi2c_stop(p_this);

        if (p_this->bus_idle_time) {
            aw_udelay(p_this->bus_idle_time);
        }
    }

    return AW_OK;
}


aw_err_t  __gpi2c_read (struct awbl_i2c_master *p_master,
                        uint16_t                chip_addr,
                        struct awbl_trans_buf  *p_trans_buf,
                        uint8_t                 start_ctl,
                        uint16_t                dr_flag)
{
    __GPI2C_GPI2C_DEV_DECL(p_this, p_master);
    uint8_t   *p_tem_buf     = (uint8_t *)p_trans_buf->p_buf;
    aw_bool_t     acked         = AW_FALSE;
    uint32_t   i             = 0;
    uint32_t   late_tick     = 0;
    uint32_t   temp_tick     = 0;
    aw_bool_t     timeout_check = AW_FALSE;

    /* 发送起始信号  */
    __gpi2c_start(p_this);

    /* wirte */
    acked = __gpi2c_write_byte(p_this, (chip_addr << 1) | 1);

    /* not ack */
    if (!acked) {
        return -AW_ENODEV;
    }

    /* gpio的码间超时要考滤一下 ,码间只有出现在SCL嵌位的情况 */
    if ((p_this->timeout != (uint32_t)-1) || (p_this->timeout == 0)) {
        timeout_check = AW_TRUE;
        late_tick = aw_sys_tick_get();
    }

    for (i = 0; i < p_trans_buf->buf_size - 1; i++) {
        p_tem_buf[i] = __gpi2c_read_byte(p_this, AW_TRUE);

        /* 判断码间超时 */
        if (timeout_check) {
            temp_tick = aw_sys_tick_get() - late_tick;

            if (aw_ticks_to_ms(temp_tick) > p_this->timeout) {
                /* 给个停止信号，并返回错误 */
                return -AW_ENODEV;
            }
        }
    }

    /* 最后一个字节发送无应答 */
    p_tem_buf[p_trans_buf->buf_size - 1] = __gpi2c_read_byte(p_this, AW_FALSE);

    __gpi2c_stop(p_this);

    if (p_this->bus_idle_time) {
        aw_udelay(p_this->bus_idle_time);
    }

    return AW_OK;
}


aw_err_t  __gpi2c_set_cfg (struct awbl_i2c_master *p_master,
                           struct aw_i2c_config   *p_cfg)
{
    __GPI2C_GPI2C_DEV_DECL(p_this, p_master);

    p_this->bus_idle_time = p_cfg->bus_idle_time;
    p_this->timeout       = p_cfg->timeout;
    p_this->speed         = p_cfg->speed;
    return AW_OK;
}


aw_err_t __gpi2c_get_cfg (struct awbl_i2c_master *p_master,
                          struct aw_i2c_config   *p_cfg)
{
    __GPI2C_GPI2C_DEV_DECL(p_this, p_master);
    p_cfg->bus_idle_time = p_this->bus_idle_time;
    p_cfg->timeout       = p_this->timeout;
    p_cfg->speed         = p_this->speed;

    return AW_OK;
}


/**
 * \brief first level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gpi2c_inst_init1(awbl_dev_t *p_dev)
{
    return;
}

/**
 * \brief second level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gpi2c_inst_init2(awbl_dev_t *p_dev)
{
    __GPI2C_MASTER_DEV_DECL(p_master, p_dev);
    __GPI2C_GPI2C_DEV_DECL(p_gpi2c, p_dev);
    __GPI2C_GPI2C_DEVINFO_DECL(p_gpi2c_dvif, p_dev);

    /* platform initialization */
    if (p_gpi2c_dvif->pfunc_plfm_init != NULL) {
        p_gpi2c_dvif->pfunc_plfm_init();
    }

    /* AWBus i2c master general init2 */
    p_master->p_devinfo2 = &__g_gpi2c_devinfo2;

    /* init controller hardware */
    __gpi2c_hard_init(p_gpi2c);

    /* create i2c bus */
    awbl_i2cbus_create((struct awbl_i2c_master *)p_dev);

    p_gpi2c->timeout = p_gpi2c_dvif->i2c_master_devinfo.timeout;
    p_gpi2c->bus_idle_time = 0;
    p_gpi2c->speed  = p_gpi2c_dvif->speed_exp;

    return;
}


/**
 * \brief third level initialization routine
 *
 * \param[in] p_dev     device instance
 */
aw_local void __gpi2c_inst_connect(awbl_dev_t *p_dev)
{
    return;
}


/**
 * \brief I2C controller hardware initialize
 */
aw_local void __gpi2c_hard_init (struct awbl_gpio_i2c_dev *p_gpi2c)
{
    __GPI2C_GPI2C_DEVINFO_DECL(p_gpi2c_dvif, p_gpi2c);

    /* TODO: set bus speed ? how? */

    /* set controller to default master mode */
    aw_gpio_set(p_gpi2c_dvif->pin_sda, 1);
    aw_gpio_set(p_gpi2c_dvif->pin_scl, 1);
}


/******************************************************************************/
void awbl_gpio_i2c_drv_register (void)
{
    awbl_drv_register((struct awbl_drvinfo *)&__g_gpi2c_drv_registration);
}

/* end of file */

