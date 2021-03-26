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
  �궨��
*******************************************************************************/
/**< \brief ͨ�� AWBus�豸  ���岢��ʼ��  BMA253 �豸 */
#define FXOS8700_DEV_DECL(p_this, p_dev) \
        awbl_sensor_fxos8700_dev_t *p_this = ( awbl_sensor_fxos8700_dev_t *)(p_dev)

/**< \brief ͨ�� AWBus�豸 ���岢��ʼ��  BMA253 �豸��Ϣ */
#define FXOS8700_DEVINFO_DECL(p_devinfo, p_dev)                    \
        awbl_sensor_fxos8700_devinfo_t *p_devinfo =                \
        (awbl_sensor_fxos8700_devinfo_t *)AWBL_DEVINFO_GET(p_dev)


/*******************************************************************************
  ���غ�������
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
  ����ȫ�ֱ�������
*******************************************************************************/
awbl_sensor_fxos8700_dev_t  __g_fxos8700_dev;


/*******************************************************************************
** \brief ������ڵ�
*******************************************************************************/
aw_local aw_const struct awbl_drvfuncs __g_fxos8700_drvfuncs = {
        __fxos8700_inst_init1,
        __fxos8700_inst_init2,
        __fxos8700_inst_connect
};


/*******************************************************************************
\brief �����ṩ�ķ���
*******************************************************************************/
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_fxos8700_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __fxoc8700_sensor_serv_get),
        AWBL_METHOD_END
};


/*******************************************************************************
\brief ����ע����Ϣ
*******************************************************************************/
aw_local aw_const struct awbl_drvinfo __g_fxos8700_drv_registration = {
        AWBL_VER_1,                      /* AWBus �汾�� */
        AWBL_BUSID_I2C,                  /* ���� ID */
        AWBL_FXOS8700_NAME,              /* ������ */
        &__g_fxos8700_drvfuncs,          /* ������ڵ� */
        __g_fxos8700_dev_methods,        /* �����ṩ�ķ��� */
        NULL                             /* ����̽�⺯�� */
};


/*******************************************************************************
 \brief ������
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
 *��fxos8700�Ĵ���
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
*дfxos8700�Ĵ���
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
        return AW_ERROR;                    //��ͨ�����Ϸ�
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
    /* ��ʧ�ܿ������� */
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
    /* ע�ᴫ�����豸���жϻص����� */
    p_this->pfn_trigger_cb = pfn_cb;
    /* ע��ص������Ĳ��� */
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
 * \brief ����
*******************************************************************************/
aw_local void __fxoc8700_sensor_serv_get (struct awbl_dev *p_dev, void *p_arg)
{
    FXOS8700_DEV_DECL(p_this, p_dev);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    awbl_sensor_service_t *p_sensor_serv = NULL;

    /*
     *  �ô�������ͨ������
     */
    aw_local const awbl_sensor_type_info_t type_info[] = {
         {AW_SENSOR_TYPE_ACCELEROMETER, 3},    /* 3·���ٶ�  */
         {AW_SENSOR_TYPE_MAGNETIC, 3}          /* 3·�Ÿ�Ӧǿ�� */
    };

    aw_local const awbl_sensor_servinfo_t serv_info = {
         6,                                   /* ��֧��6��ͨ��  */
         type_info,
         AW_NELEMENTS(type_info)              /* �����б���Ŀ    */
    };

    p_sensor_serv                  = &p_this->sensor_serv;
    p_sensor_serv->p_cookie        = p_dev;
    p_sensor_serv->p_next          = NULL;
    p_sensor_serv->p_servfuncs     = &__g_fxos8700_servfuncs;
    p_sensor_serv->p_servinfo      = &serv_info;
    p_sensor_serv->start_id        = p_devinfo->start_chn_id;

    *((awbl_sensor_service_t **)p_arg) = p_sensor_serv;
}


/* �жϺ��������ͷ�һ���ź�����֪ͨ����ȥ���� */
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

        /* �����û����õ��жϻص�����֮ǰ���ȡ�ж�Դ�Ĵ������Ա��ȡ�ж�Դ
         * ���˱�������ص������������ж��ж����͡�
         * �ô��������ж�Դ�������Ĵ���������Ŀǰֻ��ȡһ���Ĵ�����״̬��
         * �û������ж�ȡ����һ���ж�״̬�Ĵ����� */
        __fxos8700_reg_read(p_this, INT_SOURCE_REG, &regval, 1);

        if (p_this->pfn_trigger_cb != NULL) {
            p_this->pfn_trigger_cb(p_this->cb_parg, regval);
        }
    }
    return NULL;
}





/*******************************************************************************
 * \brief ��һ�׶γ�ʼ��
*******************************************************************************/
aw_local void __fxos8700_inst_init1 (struct awbl_dev *p_dev)
{
    return;
}


/*******************************************************************************
 * \brief �ڶ��׶γ�ʼ��
*******************************************************************************/
aw_local void __fxos8700_inst_init2 (struct awbl_dev *p_dev)
{
    return;
}


/*******************************************************************************
 * \brief �����׶γ�ʼ��
*******************************************************************************/
aw_local void __fxos8700_inst_connect (struct awbl_dev *p_dev)
{
    FXOS8700_DEV_DECL(p_this, p_dev);
    FXOS8700_DEVINFO_DECL(p_devinfo, p_this);

    aw_err_t    ret    = -1;
    uint8_t     regval = 0;

    /* ƽ̨��ʼ�� */
    if (p_devinfo->pfunc_plfm_init) {
        p_devinfo->pfunc_plfm_init();
    }

    /* ��ʼ���жϻص�ָ����豸��ʼ����־ */
    p_this->pfn_trigger_cb = NULL;
    p_this->init_flag = AW_FALSE;

    /* �����ж����ţ������жϺ��� */
    if (aw_gpio_pin_request("fxos8700_pins", &p_devinfo->int_pin, 1) == AW_OK) {

        aw_gpio_pin_cfg(p_devinfo->int_pin, AW_GPIO_INPUT);
        aw_gpio_trigger_connect(p_devinfo->int_pin,
                                __isr_fxos8700,
                                (void*)p_this);
        aw_gpio_trigger_cfg(p_devinfo->int_pin, AW_GPIO_TRIGGER_FALL);
    }

    /* ��ʼ���ж��ź��� */
    AW_SEMB_INIT(p_this->isr_semb, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);

    /* �����жϷ����� */
    p_this->isr_tsk = aw_task_create("fxos8700_isr_tsk",
                                      6,
                                      2048,
                                      __task_fxos8700,
                                      (void *)p_this);
    if (p_this->isr_tsk) {
        aw_task_startup(p_this->isr_tsk);
    }

    /* Ϊfxos8700����IIC�豸 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                  p_dev->p_devhcf->bus_index,
                  p_devinfo->slave_addr,
                  (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));
    return;
}


/*******************************************************************************
 * \brief �� FXOS8700 ����ע�ᵽ AWBus ��ϵͳ��
*******************************************************************************/
void awbl_fxos8700_drv_register (void)
{
    awbl_drv_register(&__g_fxos8700_drv_registration);
}



