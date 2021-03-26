/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/
#include "driver/sensor/awbl_fxos8700.h"
#include "awbus_lite.h"
#include "aw_vdebug.h"
#include "aw_i2c.h"
#include "aw_gpio.h"


/*******************************************************************************
  宏定义
*******************************************************************************/
/**< \brief 通过 AWBus设备  定义并初始化  BMA253 设备 */
#define FXOS8700_DEV_DECL(p_this, p_dev) \
        awbl_sensor_fxos8700_dev_t *p_this = ( awbl_sensor_fxos8700_dev_t *)(p_dev)

/**< \brief 通过 AWBus设备 定义并初始化  BMA253 设备信息 */
#define FXOS8700_DEVINFO_DECL(p_devinfo, p_dev)                    \
        awbl_sensor_fxos8700_devinfo_t *p_devinfo =                \
        (awbl_sensor_fxos8700_devinfo_t *)AWBL_DEVINFO_GET(p_dev)


/*******************************************************************************
  本地函数声明
*******************************************************************************/
aw_local void __fxos8700_inst_init1 (struct awbl_dev *p_dev);
aw_local void __fxos8700_inst_init2 (struct awbl_dev *p_dev);
aw_local void __fxos8700_inst_connect (struct awbl_dev *p_dev);

aw_local void __fxoc8700_sensor_serv_get (struct awbl_dev *p_dev, void *p_arg);

aw_local aw_err_t awbl_fxos8700_data_get(void                     *p_cookie,
                                         const int                *p_ids,
                                         int                       num,
                                         aw_sensor_val_t          *p_buf);

aw_local aw_err_t awbl_fxos8700_enable (void                  *p_cookie,
                                        const int             *p_ids,
                                        int                    num,
                                        aw_sensor_val_t       *p_result);

aw_local aw_err_t awbl_fxos8700_disable(void                  *p_cookie,
                                        const int             *p_ids,
                                        int                    num,
                                        aw_sensor_val_t       *p_result);

aw_local aw_err_t awbl_fxos8700_attr_set (void                    *p_cookie,
                                          int                      id,
                                          int                      attr,
                                          const aw_sensor_val_t   *p_val);

aw_local aw_err_t awbl_fxos8700_attr_get (void                    *p_cookie,
                                          int                      id,
                                          int                      attr,
                                          aw_sensor_val_t         *p_val);

aw_local aw_err_t awbl_fxos8700_trigger_cfg (void                     *p_cookie,
                                             int                       id,
                                             uint32_t                  flags,
                                             aw_sensor_trigger_cb_t    pfn_cb,
                                             void                     *p_arg);

aw_local aw_err_t awbl_fxos8700_trigger_on (void *p_cookie, int id);

aw_local aw_err_t awbl_fxos8700_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  本地全局变量定义
*******************************************************************************/
awbl_sensor_fxos8700_dev_t  __g_fxos8700_dev;


/*******************************************************************************
** \brief 驱动入口点
*******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_fxos8700_drvfuncs = {
        __fxos8700_inst_init1,
        __fxos8700_inst_init2,
        __fxos8700_inst_connect
};


/*******************************************************************************
\brief 驱动提供的方法
*******************************************************************************/
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_fxos8700_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __fxoc8700_sensor_serv_get),
        AWBL_METHOD_END
};


/*******************************************************************************
\brief 驱动注册信息
*******************************************************************************/
aw_local aw_const struct awbl_drvinfo __g_fxos8700_drv_registration = {
        AWBL_VER_1,                      /* AWBus 版本号 */
        AWBL_BUSID_I2C,                  /* 总线 ID */
        AWBL_FXOS8700_NAME,              /* 驱动名 */
        &__g_fxos8700_drvfuncs,          /* 驱动入口点 */
        __g_fxos8700_dev_methods,        /* 驱动提供的方法 */
        NULL                             /* 驱动探测函数 */
};


/*******************************************************************************
 \brief 服务函数
*******************************************************************************/
aw_local aw_const struct awbl_sensor_servfuncs __g_fxos8700_servfuncs = {
        awbl_fxos8700_data_get,                 /* pfn_data_get */
        awbl_fxos8700_enable,                   /* pfn_enable */
        awbl_fxos8700_disable,                  /* pfn_disable */
        awbl_fxos8700_attr_set,                 /* pfn_attr_set */
        awbl_fxos8700_attr_get,                 /* pfn_attr_get */
        awbl_fxos8700_trigger_cfg,              /* pfn_trigger_cfg */
        NULL,                                   /* pfn_trigger_on */
        NULL,                                   /* pfn_trigger_off */
};


/*******************************************************************************
 *读fxos8700寄存器
*******************************************************************************/
aw_local aw_err_t __fxos8700_reg_read(struct awbl_sensor_fxos8700_dev *pdev,
                                      uint8_t                          reg_addr,
                                      uint8_t                         *pbuf,
                                      uint32_t                         len)
{
    return aw_i2c_read(&pdev->i2c_dev,
                        reg_addr,
                        pbuf,
                        len);
}


/*******************************************************************************
*写fxos8700寄存器
*******************************************************************************/
aw_local aw_err_t __fxos8700_reg_write(struct awbl_sensor_fxos8700_dev *pdev,
                                       uint8_t                          reg_addr,
                                       uint8_t                         *pbuf,
                                       uint32_t                         len)
{
    return aw_i2c_write(&pdev->i2c_dev,
                         reg_addr,
                         pbuf,
                         len);
}




aw_local aw_err_t awbl_fxos8700_init (awbl_sensor_fxos8700_dev_t  *p_this)
{
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);
    aw_err_t        ret = -1;
    uint8_t         regval = 0;

    ret = __fxos8700_reg_read(p_this, WHO_AM_I_REG, &regval, 1);
    if (regval != kFXOS_WHO_AM_I_Device_ID) {
        return AW_ERROR;
    }
    aw_kprintf("sensor fxos8700 device id:0x%X.\r\n", regval);

    /* setup auto sleep with FFMT trigger
     * go to standby */
    regval = 0;
    ret = __fxos8700_reg_read(p_this, CTRL_REG1, &regval, 1);
    regval &= (uint8_t)~ACTIVE_MASK;
    ret = __fxos8700_reg_write(p_this, CTRL_REG1, &regval, 1);
    /* Read again to make sure we are in standby mode. */
    regval = 0;
    ret = __fxos8700_reg_read(p_this, CTRL_REG1, &regval, 1);
    if ((regval & ACTIVE_MASK) == ACTIVE_MASK) {
        return AW_ERROR;
    }

    /* Disable the FIFO */
    regval = F_MODE_DISABLED;
    ret = __fxos8700_reg_write(p_this, F_SETUP_REG, &regval, 1);

#ifdef LPSLEEP_HIRES
    /* enable auto-sleep, low power in sleep, high res in wake */
    regval = SLPE_MASK | SMOD_LOW_POWER | MOD_HIGH_RES;
    ret = __fxos8700_reg_write(p_this, CTRL_REG2, &regval, 1);
#else
    /* enable auto-sleep, low power in sleep, high res in wake */
    regval = MOD_HIGH_RES;
    ret = __fxos8700_reg_write(p_this, CTRL_REG2, &regval, 1);
#endif

    /* set up Mag OSR and Hybrid mode using M_CTRL_REG1, use default for Acc */
    regval = (M_RST_MASK | M_OSR_MASK | M_HMS_MASK);
    ret = __fxos8700_reg_write(p_this, M_CTRL_REG1, &regval, 1);

    /* Enable hyrid mode auto increment using M_CTRL_REG2 */
    regval = M_HYB_AUTOINC_MASK;
    ret = __fxos8700_reg_write(p_this, M_CTRL_REG2, &regval, 1);

#ifdef EN_FFMT
    /* enable FFMT for motion detect for X and Y axes, latch enable */
    regval = XEFE_MASK | YEFE_MASK | ELE_MASK | OAE_MASK;
    ret = __fxos8700_reg_write(p_this, FF_MT_CFG_REG, &regval, 1);
#endif

#ifdef SET_THRESHOLD
    /* set threshold to about 0.25g */
    regval = 0x04;
    ret = __fxos8700_reg_write(p_this, FT_MT_THS_REG, &regval, 1);
#endif

#ifdef SET_DEBOUNCE
    /* set debounce to zero */
    regval = 0x00;
    ret = __fxos8700_reg_write(p_this, FF_MT_COUNT_REG, &regval, 1);
#endif

#ifdef EN_AUTO_SLEEP
    /* set auto-sleep wait period to 5s (=5/0.64=~8) */
    regval = 8;
    ret = __fxos8700_reg_write(p_this, ASLP_COUNT_REG, &regval, 1);
#endif

    /* default set to 4g mode */
    regval = p_devinfo->sample_range;
//    regval = FULL_SCALE_4G;
    ret = __fxos8700_reg_write(p_this, XYZ_DATA_CFG_REG, &regval, 1);

#ifdef EN_INTERRUPTS
    /* enable data-ready, auto-sleep and motion detection interrupts */
    /* FXOS1_WriteRegister(CTRL_REG4, INT_EN_DRDY_MASK | INT_EN_ASLP_MASK | INT_EN_FF_MT_MASK); */
    regval = 0x0;
    ret = __fxos8700_reg_write(p_this, CTRL_REG4, &regval, 1);

    /* route data-ready interrupts to INT1, others INT2 (default) */
    regval = INT_CFG_DRDY_MASK;
    ret = __fxos8700_reg_write(p_this, CTRL_REG5, &regval, 1);

    /* enable ffmt as a wake-up source */
    regval = WAKE_FF_MT_MASK;
    ret = __fxos8700_reg_write(p_this, CTRL_REG3, &regval, 1);

    /* finally activate accel_device with ASLP ODR=0.8Hz, ODR=100Hz, FSR=2g */
    regval = HYB_ASLP_RATE_0_8HZ | HYB_DATA_RATE_100HZ | ACTIVE_MASK;
    ret = __fxos8700_reg_write(p_this, CTRL_REG1, &regval, 1);
#else
    /* Setup the ODR for 200 Hz and activate the accelerometer */
    regval = p_devinfo->sample_rate | ACTIVE_MASK;
//    regval = HYB_DATA_RATE_200HZ | ACTIVE_MASK;
    ret = __fxos8700_reg_write(p_this, CTRL_REG1, &regval, 1);
#endif

    /* Read Control register again to ensure we are in active mode */
    regval = 0;
    ret = __fxos8700_reg_read(p_this, CTRL_REG1, &regval, 1);
    if ((regval & ACTIVE_MASK) != ACTIVE_MASK) {
        return AW_ERROR;
    }

    return AW_OK;
}



/*******************************************************************************
aw_err_t (*pfn_data_get)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_data_get(void                     *p_cookie,
                                         const int                *p_ids,
                                         int                       num,
                                         aw_sensor_val_t          *p_buf)
{
    FXOS8700_DEV_DECL(p_this, p_cookie);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t        ret = -1;
    uint8_t         i = 0;;
    uint8_t         regaddr = 0;
    uint8_t         regval[2] = {0};
    uint8_t         chn = *p_ids - p_devinfo->start_chn_id;

    if (chn == 0) {
        regaddr = OUT_X_MSB_REG;
    } else if (chn == 1) {
        regaddr = OUT_Y_MSB_REG;
    } else if (chn == 2) {
        regaddr = OUT_Z_MSB_REG;
    } else if (chn == 3) {
        regaddr = M_OUT_X_MSB_REG;
    } else if (chn == 4) {
        regaddr = M_OUT_Y_MSB_REG;
    } else if (chn == 5) {
        regaddr = M_OUT_Z_MSB_REG;
    } else {
        return AW_ERROR;                    //此通道不合法
    }

    for (i = 0; i < 2; i++) {
        ret = __fxos8700_reg_read(p_this, regaddr, &regval[i], 1);
        regaddr++;
        if (ret != AW_OK) {
            return AW_ERROR;
        }
    }

    p_buf->val = (regval[0] << 8) | regval[1];
    p_buf->unit = 0;
}


/*******************************************************************************
aw_err_t (*pfn_enable)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_enable (void                  *p_cookie,
                                        const int             *p_ids,
                                        int                    num,
                                        aw_sensor_val_t       *p_result)
{
    FXOS8700_DEV_DECL(p_this, p_cookie);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t        ret = -1;
    if (p_this->init_flag == AW_FALSE) {
        ret = awbl_fxos8700_init (p_this);
        p_this->init_flag = AW_TRUE;
    } else {
        return AW_OK;
    }

    return ret;
}


/*******************************************************************************
aw_err_t (*pfn_disable)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_disable(void                  *p_cookie,
                                        const int             *p_ids,
                                        int                    num,
                                        aw_sensor_val_t       *p_result)
{
    /* 无失能控制引脚 */
    return AW_OK;
}


/*******************************************************************************
aw_err_t (*pfn_attr_set)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_attr_set (void                    *p_cookie,
                                          int                      id,
                                          int                      attr,
                                          const aw_sensor_val_t   *p_val)
{
    FXOS8700_DEV_DECL(p_this, p_cookie);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t        ret = -1;
    uint8_t         regaddr = attr & 0xFF;
    uint8_t         regval = p_val->val & 0xFF;

    ret = __fxos8700_reg_write(p_this, regaddr, &regval, 1);
    if (ret != AW_OK) {
        return AW_ERROR;
    }

    return AW_OK;
}


/*******************************************************************************
aw_err_t (*pfn_attr_get)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_attr_get (void                    *p_cookie,
                                          int                      id,
                                          int                      attr,
                                          aw_sensor_val_t         *p_val)
{
    FXOS8700_DEV_DECL(p_this, p_cookie);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t        ret = -1;
    uint8_t         regaddr = attr & 0xFF;
    uint8_t         regval = 0;

    ret = __fxos8700_reg_read(p_this, regaddr, &regval, 1);
    if (ret == AW_OK) {
        p_val->val = regval;
    } else {
        p_val->val = 0;
        return AW_ERROR;
    }

    return AW_OK;
}



/*******************************************************************************
aw_err_t (*pfn_trigger_cfg)
*******************************************************************************/
aw_local aw_err_t awbl_fxos8700_trigger_cfg (void                     *p_cookie,
                                             int                       id,
                                             uint32_t                  flags,
                                             aw_sensor_trigger_cb_t    pfn_cb,
                                             void                     *p_arg)
{
    FXOS8700_DEV_DECL(p_this, p_cookie);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    uint8_t         chn = id - p_devinfo->start_chn_id;

    if (chn > 5) {
        return AW_ERROR;
    }
    /* 注册传感器设备的中断回调函数 */
    p_this->pfn_trigger_cb = pfn_cb;
    /* 注册回调函数的参数 */
    p_this->cb_parg = p_arg;
    return AW_OK;
}


///*******************************************************************************
//aw_err_t (*pfn_trigger_on)
//*******************************************************************************/
//aw_local aw_err_t awbl_fxos8700_trigger_on (void *p_cookie, int id)
//{
//    FXOS8700_DEV_DECL(p_this, p_cookie);
//    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);
//
//    uint8_t         chn = id - p_devinfo->start_chn_id;
//
//    return AW_OK;
//}
//
//
///*******************************************************************************
//aw_err_t (*pfn_trigger_off)
//*******************************************************************************/
//aw_local aw_err_t awbl_fxos8700_trigger_off (void *p_cookie, int id)
//{
//    FXOS8700_DEV_DECL(p_this, p_cookie);
//    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);
//
//    uint8_t         chn = id - p_devinfo->start_chn_id;
//
//
//    return AW_OK;
//}


/*******************************************************************************
 * \brief 方法
*******************************************************************************/
aw_local void __fxoc8700_sensor_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    FXOS8700_DEV_DECL(p_this, p_dev);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    awbl_sensor_service_t *p_sensor_serv = NULL;

    /*
     *  该传感器的通道分配
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_ACCELEROMETER, 3},    /* 3路加速度  */
         {AW_SENSOR_TYPE_MAGNETIC, 3}          /* 3路磁感应强度 */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         6,                                   /* 共支持6个通道  */
         type_info,
         AW_NELEMENTS(type_info)              /* 类型列表数目    */
    };

    p_sensor_serv                  = &p_this->sensor_serv;
    p_sensor_serv->p_cookie        = p_dev;
    p_sensor_serv->p_next          = NULL;
    p_sensor_serv->p_servfuncs     = &__g_fxos8700_servfuncs;
    p_sensor_serv->p_servinfo      = &serv_info;
    p_sensor_serv->start_id        = p_devinfo->start_chn_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}


/* 中断函数仅仅释放一个信号量，通知任务去处理 */
void __isr_fxos8700 (void *p_arg)
{
    FXOS8700_DEV_DECL(p_this, p_arg);
    AW_SEMB_GIVE(p_this->isr_semb);
    return;
}


void *__task_fxos8700 (void *p_arg)
{
    FXOS8700_DEV_DECL(p_this, p_arg);

    uint8_t     regval = 0;

    AW_FOREVER {

        AW_SEMB_TAKE(p_this->isr_semb, AW_WAIT_FOREVER);

        /* 调用用户设置的中断回调函数之前需读取中断源寄存器，以便获取中断源
         * 将此变量传入回调函数，用于判断中断类型。
         * 该传感器的中断源有两个寄存器，这里目前只读取一个寄存器的状态，
         * 用户可自行读取另外一个中断状态寄存器。 */
        __fxos8700_reg_read(p_this, INT_SOURCE_REG, &regval, 1);

        if (p_this->pfn_trigger_cb != NULL) {
            p_this->pfn_trigger_cb(p_this->cb_parg, regval);
        }
    }
    return NULL;
}





/*******************************************************************************
 * \brief 第一阶段初始化
*******************************************************************************/
aw_local void __fxos8700_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}


/*******************************************************************************
 * \brief 第二阶段初始化
*******************************************************************************/
aw_local void __fxos8700_inst_init2 (struct awbl_dev *p_dev)
{
    return;
}


/*******************************************************************************
 * \brief 第三阶段初始化
*******************************************************************************/
aw_local void __fxos8700_inst_connect (struct awbl_dev *p_dev)
{
    FXOS8700_DEV_DECL(p_this, p_dev);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t    ret    = -1;
    uint8_t     regval = 0;

    /* 平台初始化 */
    if (p_devinfo->pfunc_plfm_init) {
        p_devinfo->pfunc_plfm_init();
    }

    /* 初始化中断回调指针和设备初始化标志 */
    p_this->pfn_trigger_cb = NULL;
    p_this->init_flag = AW_FALSE;

    /* 申请中断引脚，连接中断函数 */
    if (aw_gpio_pin_request("fxos8700_pins", &p_devinfo->int_pin, 1) == AW_OK) {

        aw_gpio_pin_cfg(p_devinfo->int_pin, AW_GPIO_INPUT);
        aw_gpio_trigger_connect(p_devinfo->int_pin,
                                __isr_fxos8700,
                                (void*)p_this);
        aw_gpio_trigger_cfg(p_devinfo->int_pin, AW_GPIO_TRIGGER_FALL);
    }

    /* 初始化中断信号量 */
    AW_SEMB_INIT(p_this->isr_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* 创建中断服务函数 */
    p_this->isr_tsk = aw_task_create("fxos8700_isr_tsk",
                                      6,
                                      2048,
                                      __task_fxos8700,
                                      (void *)p_this);
    if (p_this->isr_tsk) {
        aw_task_startup(p_this->isr_tsk);
    }

    /* 为fxos8700创建IIC设备 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                  p_dev->p_devhcf->bus_index,
                  p_devinfo->slave_addr,
                  (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));
    return;
}


/*******************************************************************************
 * \brief 将 FXOS8700 驱动注册到 AWBus 子系统中
*******************************************************************************/
void awbl_fxos8700_drv_register (void)
{
    awbl_drv_register(&__g_fxos8700_drv_registration);
}



