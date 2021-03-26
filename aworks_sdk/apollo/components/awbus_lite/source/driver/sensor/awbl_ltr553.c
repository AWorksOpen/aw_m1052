/*******************************************************************************
 *                                  AWorks
 *                       ----------------------------
 *                       innovating embedded platform
 *
 * Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
 * All rights reserved.
 *
 * Contact information:
 * web site:    http://www.zlg.cn
 * e-mail:      support@zlg.cn
 ******************************************************************************/
 
 /**
  * \file
  * \brief LTR-553ALS-01����Դ��
  *
  * \internal
  * \par Modification History
  * - 1.00 16-03-25  cod, first implementation.
  * \endinternal
 */
 
#include "aworks.h"
#include "awbus_lite.h"
#include "awbl_i2cbus.h"
#include "awbl_input.h"
#include "aw_isr_defer.h"
#include "awbl_sensor.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "driver/sensor/awbl_ltr553.h"


/*******************************************************************************
 �궨��
*******************************************************************************/

/* ʹ��Isr_Defer�ڶ�ȡI2C���� */
#define __LTR553_ISR_DEFER
/* �����꣬ͨ�������豸ָ���þ����豸ָ�� */
#define __LTR553_DEV_DECL(p_this,p_dev) struct awbl_ltr553_dev *(p_this) = \
                                        (struct awbl_ltr553_dev *)(p_dev)

/* �����꣬ͨ�������豸ָ�����豸��Ϣָ�� */
#define __LTR553_DEVINFO_DECL(p_devinfo,p_dev) \
            struct awbl_ltr553_devinfo *(p_devinfo) = \
           (struct awbl_ltr553_devinfo *)AWBL_DEVINFO_GET(p_dev)

/** \brief �����꣬���ڽ��Ĵ���ֵת��Ϊ��Ӧch��ֵ */
#define __LTR553_REG_2_DATA1(x)         ((x)[0] + ((x)[1] << 8))
#define __LTR553_REG_2_DATA0(x)         ((x)[2] + ((x)[3] << 8))

/** \brief �����꣬����ratio��ֵ */
#define __LTR553_RATIO_CAL(data0,data1) (((data0) + (data1) == 0) ? 0 : \
                                        (100 * (data1) / ((data0) + (data1))))


/* LTR553�Ĵ������� */
#define __LTR553_REG_ALS_CONTR          0x80        /**< \brief ALS����ģʽ   */
#define __LTR553_VAL_ALS_MODE_ACTIVE    0x01        /**< \brief ALS ACTIVE    */
#define __LTR553_VAL_ALS_MODE_STANDBY   0x00        /**< \brief ALS standby   */
#define __LTR553_VAL_SW_RESET           0X02        /**< \brief ִ�������λ  */
#define __LTR553_VAL_ALS_GAIN_1X        0X00        /**< \brief 1    - 64k lx */
#define __LTR553_VAL_ALS_GAIN_2X        (0X01 << 2) /**< \brief 0.5  - 32k lx */
#define __LTR553_VAL_ALS_GAIN_4X        (0X02 << 2) /**< \brief 0.25 - 16k lx */
#define __LTR553_VAL_ALS_GAIN_8X        (0X03 << 2) /**< \brief 0.125- 8k  lx */
#define __LTR553_VAL_ALS_GAIN_48X       (0X06 << 2) /**< \brief 0.02 - 1.3k lx*/
#define __LTR553_VAL_ALS_GAIN_96X       (0X07 << 2) /**< \brief 0.01 - 600 lx */
/** \brief GAIN�Ĵ�������*/
#define __LTR553_VAL_GAIN_MASK          (~(0X07 << 2))

#define __LTR553_REG_PS_CONTR           0X81  /**< \brief PS����ģʽ����      */
#define __LTR553_VAL_PS_MODE_ACTIVE     0x03  /**< \brief PS ACTIVEģʽֵ     */
#define __LTR553_VAL_PS_MODE_STANDBY    0x00  /**< \brief PS standbyģʽֵ    */
#define __LTR553_VAL_PS_SATURATION_ON   0x20  /**< \brief PS ���Ͷ�ʹ��       */
#define __LTR553_VAL_PS_SATURATION_OFF  0x00  /**< \brief PS ���ͶȽ���       */

#define __LTR553_REG_PS_LED             0X82  /**< \brief PS LED����          */
#define __LTR553_REG_PS_N_PULSES        0X83  /**< \brief PS��������          */
#define __LTR553_REG_PS_MEAS_RATE       0X84  /**< \brief PS����Ƶ��          */

#define __LTR553_REG_ALS_MEAS_RATE      0X85  /**< \brief ALS����Ƶ��         */

#define __LTR553_REG_PART_ID            0X86  /**< \brief PART ID             */
#define __LTR553_REG_MANUFAC_ID         0X87  /**< \brief manufacturer id     */

#define __LTR553_REG_ALS_DATA_CH1_0     0X88  /**< \brief ALS CH1����ֵ���ֽ� */
#define __LTR553_REG_ALS_DATA_CH1_1     0X89  /**< \brief ALS CH1����ֵ���ֽ� */
#define __LTR553_REG_ALS_DATA_CH0_0     0X8A  /**< \brief ALS CH0����ֵ���ֽ� */
#define __LTR553_REG_ALS_DATA_CH0_1     0X8B  /**< \brief ALS CH0����ֵ���ֽ� */

#define __LTR553_REG_ALS_PS_STATUS      0X8C  /**< \brief ALS��PS����״̬     */
#define __LTR553_VAL_ALS_DATA_VALID     0X80  /**< \brief ALS������Ч         */
#define __LTR553_VAL_ALS_INTERRUPT      0X08  /**< \brief ALS���ޱ����ж�     */
#define __LTR553_VAL_ALS_DATA_READY     0X04  /**< \brief ALS����׼�����ж�   */
#define __LTR553_VAL_PS_DATA_READY      0X01  /**< \brief PS����׼�����ж�    */
#define __LTR553_VAL_PS_INTERRUPT       0X02  /**< \brief PS���ޱ����ж�      */


#define __LTR553_REG_PS_DATA_0          0X8D  /**< \brief PS CH0����ֵ���ֽ�  */
#define __LTR553_REG_PS_DATA_1          0X8E  /**< \brief PS CH0����ֵ���ֽ�  */

#define __LTR553_REG_INTERRUPT          0X8F  /**< \brief �ж�����            */
#define __LTR553_VAL_INTERRUPT_NONE     0X00  /**< \brief ALS��PS����ʹ��     */
#define __LTR553_VAL_INTERRUPT_PS       0X01  /**< \brief ֻ��PS�ж�ʹ��      */
#define __LTR553_VAL_INTERRUPT_ALS      0X02  /**< \brief ֻ��ALS�ж�ʹ��     */
#define __LTR553_VAL_INTERRUPT_BOTH     0X01  /**< \brief ALS��PS�жϾ�ʹ��   */

#define __LTR553_REG_PS_THRES_UP_0      0X90  /**< \brief PS �߱���ֵ���ֽ�   */
#define __LTR553_REG_PS_THRES_UP_1      0X91  /**< \brief PS �߱���ֵ���ֽ�   */
#define __LTR553_REG_PS_THRES_LOW_0     0X92  /**< \brief PS �ͱ���ֵ���ֽ�   */
#define __LTR553_REG_PS_THRES_LOW_1     0X93  /**< \brief PS �ͱ���ֵ���ֽ�   */

#define __LTR553_REG_PS_OFFSET_1        0X94  /**< \brief PS OFFSET���ֽ�     */
#define __LTR553_REG_PS_OFFSET_0        0X95  /**< \brief PS OFFSET���ֽ�     */

#define __LTR553_REG_ALS_THRES_UP_0     0X97  /**< \brief ALS �߱���ֵ���ֽ�  */
#define __LTR553_REG_ALS_THRES_UP_1     0X98  /**< \brief ALS �߱���ֵ���ֽ�  */
#define __LTR553_REG_ALS_THRES_LOW_0    0X99  /**< \brief ALS �ͱ���ֵ���ֽ�  */
#define __LTR553_REG_ALS_THRES_LOW_1    0X9A  /**< \brief ALS �ͱ���ֵ���ֽ�  */

#define __LTR553_REG_INTERRUPT_PERSIST  0X9E  /**< \brief �����жϱ�����      */

#define __LTR553_ALS_GAIN_SW_THRES      60000 /**< \brief �����л�����        */

#define __LTR553_ACTIVE_MODE_WAIT_TIME  10    /**< \brief �л�Active�ȴ�ʱ��  */
#define __LTR553_POWERUP_WAIT_TIME      100   /**< \brief �ϵ�ȴ�ʱ��        */

#define __LTR553_MODE_ALS_ACTIVE        0X01  /**< \brief als������           */
#define __LTR553_MODE_PS_ACTIVE         0X02  /**< \brief ps������            */
#define __LTR553_MODE_BOTH_ACTIVE       0X03  /**< \brief ps��als������       */
#define __LTR553_MODE_NONE_ACTIVE       0X00  /**< \brief ps��alsδ����       */

//#define __LTR553_AUTO_MATCH_GAIN            /**< \brief ʹ���Զ��������    */

/*******************************************************************************
  ���غ�������
*******************************************************************************/
aw_local void __ltr553_inst_connect (awbl_dev_t *p_dev);
aw_local aw_err_t __ltr553_sensorserv_get(struct awbl_dev *p_dev, void *p_arg);
aw_local aw_err_t __ltr553_enable(void            *p_cookie,
                                  const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t    *p_result);
aw_local aw_err_t __ltr553_disable(void              *p_cookie,
                                   const int         *p_ids,
                                   int                num,
                                   aw_sensor_val_t   *p_result);
aw_local aw_err_t __ltr553_data_get(void             *p_cookie,
                                    const int        *p_ids,
                                    int               num,
                                    aw_sensor_val_t  *p_buf);

aw_local aw_err_t __ltr553_attr_set(void                  *p_cookie,
                                    int                    id,
                                    int                    attr,
                                    const aw_sensor_val_t *p_val);

aw_local aw_err_t __ltr553_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val);
aw_local aw_err_t __ltr553_trigger_cfg (void                  *p_cookie,
                                        int                     id,
                                        uint32_t                flags,
                                        aw_sensor_trigger_cb_t pfn_cb,
                                        void                  *p_arg);

aw_local aw_err_t __ltr553_trigger_on (void *p_cookie, int id);
aw_local aw_err_t __ltr553_trigger_off(void *p_cookie, int id);
aw_local void __ltr553_alarm_callback(void *p_arg);
aw_local void __ltr553_isr_handle(void *p_arg);

aw_local aw_err_t __ltr553_reg_write(awbl_ltr553_dev_t *p_this,
                                     uint8_t            reg_addr,
                                     uint8_t           *p_val,
                                     int                len);
aw_local aw_err_t __ltr553_reg_read(awbl_ltr553_dev_t *p_this,
                                    uint8_t            start_reg_addr,
                                    uint8_t           *p_buf,
                                    int                len);
aw_local aw_err_t __ltr553_als_update_settings(awbl_ltr553_dev_t *p_this,
                                               uint8_t           *p_adc);

/*******************************************************************************
 ����ȫ�ֱ�������
*******************************************************************************/
/**
 *\brief LTR553�������������
 *���ǵ��͹������⣬��ϵͳ����ʱĬ���ô���������standbyģʽ
 */
aw_local aw_const struct awbl_drvfuncs __g_ltr553_drvfuncs = {
    NULL,
    NULL,
    __ltr553_inst_connect
};

/** \brief ������������Ϣ */
aw_local const awbl_sensor_type_info_t __g_ltr553_typeinfo[2] = {
    {AW_SENSOR_TYPE_LIGHT,      1},
    {AW_SENSOR_TYPE_PROXIMITY,  1}
};

/** \brief ����������Ϣ */
aw_local aw_const awbl_sensor_servinfo_t __g_ltr553_servinfo = {
    2,
    &__g_ltr553_typeinfo[0],
    AW_NELEMENTS(__g_ltr553_typeinfo),
};

/** \brief ������ */
aw_local const struct awbl_sensor_servfuncs __g_ltr553_servfuncs = {
    __ltr553_data_get,
    __ltr553_enable,
    __ltr553_disable,
    __ltr553_attr_set,
    __ltr553_attr_get,   
    __ltr553_trigger_cfg,
    __ltr553_trigger_on,
    __ltr553_trigger_off,
};

/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);
aw_local const struct awbl_dev_method __g_ltr553_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __ltr553_sensorserv_get},
    AWBL_METHOD_END
};

/** \brief ����ע����Ϣ */
aw_local struct awbl_drvinfo __g_ltr553_drvinfo = {
    AWBL_VER_1,
    AWBL_BUSID_I2C,
    AWBL_LTR553_NAME,
    &__g_ltr553_drvfuncs,
    __g_ltr553_methods,
    NULL
};

/** \brief LTR553�ж϶�ȡ��ز������� */
#ifdef __LTR553_ISR_DEFER
aw_local struct aw_isr_defer_job __g_ltr553_isr_defer;
#endif

/** \brief ALS����ֵ��Ĵ���ֵ��ת����ϵ*/
aw_local aw_const uint8_t __ltr553_als_gain_table[] = {
    1, 2, 4, 8, 1, 1, 48, 96
};

/** \brief ALSת��ʱ����Ĵ���ֵ��Ӧ��ϵ ��С10��*/
aw_local aw_const uint8_t __ltr553_als_integration_time[] = {
    10, 5, 20, 40, 15, 25, 30, 35
};

/** \brief PS����������Ĵ���ֵ��Ӧ��ϵ*/
aw_local aw_const uint16_t __ltr553_als_meas_time[] = {
    50, 100, 200, 500, 1000, 2000, 2000, 2000
};

/** \brief ALS����������Ĵ���ֵ��Ӧ��ϵ ��С10��*/
aw_local aw_const uint16_t __ltr553_ps_meas_time[] = {
    50, 70, 100, 200, 500, 1000, 1000, 10
};

/* �����ֲᶨ���lux����ʹ�ñ��� */
struct als_coeff {
    int32_t data0_coeff_i;
    int32_t data1_coeff_i;
    int32_t data0_coeff_f;
    int32_t data1_coeff_f;
    int32_t win_fac;
    int32_t sign;
};
aw_local struct als_coeff eqtn_map[] = {
    {1, 1, 7743, 1059, 44, 1},
    {4, 1, 2785, 9548, 50, -1},
    {0, 0, 5926, 1185, 40, 1},
    {0, 0, 0,    0,    1,  1},
};
/******************************************************************************/
/**
 * \brief ת��sensorֵ
 */
aw_local int32_t __sersor_val_cal(const aw_sensor_val_t *p_val)
{
    aw_local const uint32_t pow10[] = {
        1UL,                  /* 0 */
        10UL,                 /* 1 */
        100UL,                /* 2 */
        1000UL,               /* 3 */
        10000UL,              /* 4 */
        100000UL,             /* 5 */
        1000000UL,            /* 6 */
        10000000UL,           /* 7 */
        100000000UL,          /* 8 */
        1000000000UL,         /* 9 */
    };
    int32_t unit = p_val->unit;

    if (unit < 0) {
        unit = -unit;
    }
    if ((unit >= 0) && (unit <= 9)) {
        return (int32_t)((int64_t)p_val->val * pow10[unit]);
    }

    return 0;
}

/******************************************************************************/
/**
 * \brief ����lux
 */
aw_local int32_t __ltr553_als_adc_2_lux (uint8_t *p_regval,
                                         int32_t  gain,
                                         int32_t  integration_time)
{
    int32_t ratio;
    int32_t data1 = (int32_t)__LTR553_REG_2_DATA1(p_regval);
    int32_t data0 = (int32_t)__LTR553_REG_2_DATA0(p_regval);
    int32_t lux;

    if ((data1 + data0) == 0) {
        return 0;
    }

#if 1
    int32_t lux_i;
    int32_t lux_f;
    struct als_coeff *eqtn;

    ratio = __LTR553_RATIO_CAL(data0,data1);
    if (ratio < 45){
        eqtn = &eqtn_map[0];
    } else if (ratio < 68){
        eqtn = &eqtn_map[1];
    } else if (ratio < 99){
        eqtn = &eqtn_map[2];
    } else {
        eqtn = &eqtn_map[3];
    }

    lux_i = (data0 * eqtn->data0_coeff_i + data1 * eqtn->data1_coeff_i) *
            eqtn->win_fac * 1000;

    lux_f = (data0 * eqtn->data0_coeff_f + data1 * eqtn->data1_coeff_f *
            eqtn->sign) / 100 * eqtn->win_fac * 1000;

    if (lux_f < 0) {
        lux_f = -lux_f;
    }

    lux = (lux_i + lux_f / 100) / ( __ltr553_als_gain_table[gain] *
                    __ltr553_als_integration_time[integration_time]);
#else
    data1 = (int32_t)__LTR553_REG_2_DATA0(p_regval);
    data0 = (int32_t)__LTR553_REG_2_DATA1(p_regval);
    ratio = 100 * data0 / (data0 + data1);
    if (ratio < 45) {
        //lux = (17743 * data0) + 11059 * data1;
        lux = (17743 * data0) + 11059 * data1;
    } else if (ratio < 64) {
        //lux = 37725 * data0 - 13363 * data1;
        lux = 42785 * data0 - 19548 * data1;
    } else if (ratio < 85) {
        //lux = 16900 * data0 - 169 * data1;
        lux = 5926 * data0 + 1185 * data1;
    } else {
        lux = 0;
    }

    if (lux != 0) {
        lux = lux_i / (100 * __ltr553_als_gain_table[gain] *
                __ltr553_als_integration_time[integration_time]);
    }

#endif
    return lux;
}

/******************************************************************************/
/**
 * \brief ����lux����adcֵ
 */
aw_local int32_t __ltr553_als_lux_2_adc (int32_t lux,
                                         int32_t ratio,
                                         int32_t gain,
                                         int32_t integration_time)
{
    struct als_coeff *eqtn;
    int32_t divisor_i;
    int32_t divisor_f;
    int32_t dividend;
    int32_t adc_val;

    if (ratio == 0) {
        return 0;
    }
    if (ratio < 45){
        eqtn = &eqtn_map[0];
    } else if (ratio < 68){
        eqtn = &eqtn_map[1];
    } else if (ratio < 99){
        eqtn = &eqtn_map[2];
    } else {
        eqtn = &eqtn_map[3];
    }

    dividend = lux * __ltr553_als_gain_table[gain] *
                __ltr553_als_integration_time[integration_time];

    divisor_i = ((100 - ratio) * eqtn->data0_coeff_i / ratio +
                eqtn->data1_coeff_i * eqtn->sign) * eqtn->win_fac;

    divisor_f = ((100 - ratio) * eqtn->data0_coeff_f / ratio +
                eqtn->data1_coeff_f * eqtn->sign) * eqtn->win_fac / 10000;

    if (divisor_f < 0) {
        divisor_f = -divisor_f;
    }

    if ((divisor_i + divisor_f) == 0) {
        return 0;
    }
    adc_val = dividend / (divisor_i + divisor_f);

    return (adc_val <= 0 ? 1 : adc_val);
}

/******************************************************************************/
/**
 * \brief ����adֵ����ps
 */
aw_local int32_t __ltr553_ps_cal (uint8_t *p_regval)
{
    int32_t temp = (int32_t)(p_regval[0] + (p_regval[1] << 8));

    if (temp & 0x8000) {
        return 0;
    } else {
        return temp;
    }
}

/*
 * \brief LTR553�����жϺ���ʱ������
 */
aw_local void __ltr553_isr_handle (void *p_arg)
{
    __LTR553_DEV_DECL(p_this, p_arg);
    uint8_t reg_val;
#ifdef __LTR553_AUTO_MATCH_GAIN
    uint8_t als_adc[4];
#endif
    
    if (__ltr553_reg_read(p_this, __LTR553_REG_ALS_PS_STATUS, &reg_val, 1) !=
            AW_OK) {
        return ;
    }

    if ((reg_val & __LTR553_VAL_ALS_INTERRUPT) && p_this->pfn_als_trigger_cb) {
        (p_this->pfn_als_trigger_cb)(p_this->p_als_arg,
                                     AW_SENSOR_TRIGGER_THRESHOLD);
#ifdef __LTR553_AUTO_MATCH_GAIN
        if(__ltr553_reg_read(p_this,
                            __LTR553_REG_ALS_DATA_CH1_0,
                            als_adc,
                            4) != AW_OK) {
            return;
        }
        __ltr553_als_update_settings(p_this, als_adc);
#endif
    }
    if ((reg_val & __LTR553_VAL_PS_INTERRUPT) && p_this->pfn_ps_trigger_cb) {
        (p_this->pfn_ps_trigger_cb)(p_this->p_ps_arg,
                                    AW_SENSOR_TRIGGER_THRESHOLD);
    }
}

/*
 * \brief ���������ж�
 */
aw_local void __ltr553_alarm_callback (void *p_arg)
{
    #ifdef __LTR553_ISR_DEFER
    aw_isr_defer_job_add(&__g_ltr553_isr_defer);
    #else
    __LTR553_DEV_DECL(p_this,p_arg);
    __LTR553_DEVINFO_DECL(p_devinfo,p_arg);
    #endif
}

/**
 * \brief �����׶γ�ʼ��
 */
aw_local void __ltr553_inst_connect (awbl_dev_t *p_dev)
{
    __LTR553_DEV_DECL(p_this, p_dev);
    __LTR553_DEVINFO_DECL(p_devinfo, p_dev);
    uint8_t reg_val;

    /* ������ʼ�� */
    p_this->pfn_als_trigger_cb      = NULL;
    p_this->pfn_ps_trigger_cb       = NULL;
    p_this->p_als_arg               = NULL;
    p_this->p_ps_arg                = NULL;
    p_this->alert_chan              = __LTR553_VAL_INTERRUPT_NONE;
    p_this->als_thres_low           = -1;
    p_this->als_thres_up            = -1;
    p_this->ps_thres_low            = -1;
    p_this->ps_thres_up             = -1;
    p_this->als_ps_mode             = __LTR553_MODE_NONE_ACTIVE;
    p_this->als_meas_time           = 500;

    /* als ps������standby״̬ */
    aw_mdelay(__LTR553_POWERUP_WAIT_TIME);
    reg_val = 0;
    __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    reg_val = 0;
    __ltr553_reg_write(p_this, __LTR553_REG_PS_CONTR, &reg_val, 1);

    /* ����als */
    if ((p_devinfo->als_integration_time < LTR553_INTEGRATION_TIME_100MS) &&
        (p_devinfo->als_integration_time > LTR553_INTEGRATION_TIME_350MS)) {
        p_devinfo->als_integration_time = LTR553_INTEGRATION_TIME_100MS;
    }
    reg_val = (p_devinfo->als_integration_time << 3) | 0x03;
    __ltr553_reg_write(p_this, __LTR553_REG_ALS_MEAS_RATE, &reg_val, 1);

    if (((p_devinfo->als_gain < LTR553_GAIN_1X) &&
        (p_devinfo->als_gain > LTR553_GAIN_96X)) ||
        (p_devinfo->als_gain == 4) ||
        (p_devinfo->als_gain == 5)) {
        p_devinfo->als_gain = LTR553_GAIN_1X;
    }

    __ltr553_reg_read(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    reg_val = (reg_val & __LTR553_VAL_GAIN_MASK) | (p_devinfo->als_gain << 2);
    __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);

    reg_val |= 0x01 << 1;
    __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);

    /* �ж��������� */
    if (p_devinfo->alert_pin != -1) {
        aw_gpio_trigger_cfg(p_devinfo->alert_pin, AW_GPIO_TRIGGER_FALL);
        aw_gpio_trigger_connect(p_devinfo->alert_pin,
                                __ltr553_alarm_callback,
                                p_dev);
    }

    #ifdef __LTR553_ISR_DEFER
    aw_isr_defer_job_init(&__g_ltr553_isr_defer, __ltr553_isr_handle, p_dev);
    #endif
}

/******************************************************************************/
/**
 * \brief LTR553����I2Cд�Ĵ���
 */
aw_local aw_err_t __ltr553_reg_write (awbl_ltr553_dev_t *p_this,
                                      uint8_t            start_reg_addr,
                                      uint8_t           *p_val,
                                      int                len)
{
    __LTR553_DEVINFO_DECL(p_devinfo, p_this);
    aw_err_t ret;
    int i;

    for (i = 0; i < len; i++) {
        ret = awbl_i2c_write((struct awbl_i2c_device *)p_this,
                             AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                             p_devinfo->i2c_addr ,
                             (uint32_t)start_reg_addr,
                             &p_val[i],
                             1);
        if (ret != AW_OK) {
            return ret;
        }
        start_reg_addr ++;
    }

    return AW_OK;
}

/**
 * \brief LTR553�������Ĵ���
 */
aw_local aw_err_t __ltr553_reg_read (awbl_ltr553_dev_t *p_this,
                                     uint8_t            start_reg_addr,
                                     uint8_t           *p_buf,
                                     int                len)
{
    __LTR553_DEVINFO_DECL(p_devinfo, p_this);
    aw_err_t ret;
    int i;

    for (i = 0; i < len; i++) {
        ret  = awbl_i2c_read((struct awbl_i2c_device *)p_this,
                AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                p_devinfo->i2c_addr,
                (uint32_t)start_reg_addr,
                &p_buf[i],
                1);
        if (ret != AW_OK) {
            return ret;
        }
        start_reg_addr ++;
    }

    return AW_OK;
}

/**
 * \brief   LTR553 ALS ���õ����ޱ���ֵ
 */
aw_local aw_err_t __ltr553_als_set_thres_low (awbl_ltr553_dev_t *p_this,
                                              int32_t            lux,
                                              int32_t            ratio)
{
    __LTR553_DEVINFO_DECL(p_devinfo, p_this);
    int32_t adc_thres;
    uint8_t reg_val[2];

    adc_thres = __ltr553_als_lux_2_adc(lux,
                                    ratio,
                                    p_devinfo->als_gain,
                                    p_devinfo->als_integration_time);
    reg_val[0] = (uint8_t)(adc_thres);
    reg_val[1] = (uint8_t)(adc_thres >> 8);
    return __ltr553_reg_write(p_this, __LTR553_REG_ALS_THRES_LOW_0, reg_val, 2);
}

/**
 * \brief   LTR553 ALS ���ø����ޱ���ֵ
 */
aw_local aw_err_t __ltr553_als_set_thres_up (awbl_ltr553_dev_t *p_this,
                                             int32_t            lux,
                                             int32_t            ratio)
{
    __LTR553_DEVINFO_DECL(p_devinfo, p_this);
    int32_t adc_thres;
    uint8_t reg_val[2];

    adc_thres = __ltr553_als_lux_2_adc(lux,
                                       ratio,
                                       p_devinfo->als_gain,
                                       p_devinfo->als_integration_time);
    if (adc_thres == 0) {
        adc_thres = 0xffff;
    }
    reg_val[0] = (uint8_t)(adc_thres);
    reg_val[1] = (uint8_t)(adc_thres >> 8);
    return __ltr553_reg_write(p_this, __LTR553_REG_ALS_THRES_UP_0, reg_val, 2);
}

/**
 * \brief   LTR553 ALS ��������
 */
aw_local aw_err_t __ltr553_gain_set (awbl_ltr553_dev_t *p_this, int32_t gain)
{
    uint8_t reg_val;
    aw_err_t ret;

    ret = __ltr553_reg_read(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }
    reg_val = (uint8_t)( (reg_val & 0xd3) | (gain << 2) |
                            __LTR553_VAL_ALS_MODE_ACTIVE);
    return __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
}

/******************************************************************************/
/**
 * \brief   LTR553 ALS ����als gain��thres
 * \note    �����������ҪĿ���ǻ����Ѳ��������
 *          ����thres��Ҫ����Ϊgain��������������thres��Ӧ��ADCֵ���б仯
 */
aw_local aw_err_t __ltr553_als_update_settings(awbl_ltr553_dev_t *p_this,
                                               uint8_t           *p_adc)
{
#ifdef __LTR553_AUTO_MATCH_GAIN
    __LTR553_DEVINFO_DECL(p_devinfo, p_this);
    int32_t i;
    int32_t new_gain;
#endif
    int32_t ratio;
    int32_t data1 = (int32_t)__LTR553_REG_2_DATA1(p_adc);
    int32_t data0 = (int32_t)__LTR553_REG_2_DATA0(p_adc);
    aw_err_t ret;

#ifdef __LTR553_AUTO_MATCH_GAIN
    /* ���㵱ǰ���������������� */
    for (i = AW_NELEMENTS(__ltr553_als_gain_table) - 1; i >= 0; i--) {
        if ((i == 4) || (i == 5)) {
            continue;
        }

        if (((data1 + data0) * __ltr553_als_gain_table[i] / p_devinfo->als_gain) <
                __LTR553_ALS_GAIN_SW_THRES) {
            break;
        }
    }
    new_gain = i < 0 ? 0 : i;

    if (new_gain != p_devinfo->als_gain) {
        /* ��ǰ���治������Ѳ������������� */
        /* �Ƚ���standbyģʽ */
        ret = __ltr553_gain_set(p_this, new_gain);
        if (ret != AW_OK) {
            return ret;
        }
        p_devinfo->als_gain = new_gain;
    }
#endif/* __LTR553_AUTO_MATCH_GAIN */
    /* ���������ޱ��������¸�ֵ��Ӧ�Ĵ��� */
    if (p_this->als_thres_low != -1) {
        ratio = __LTR553_RATIO_CAL(data0,data1);
        ret = __ltr553_als_set_thres_low(p_this,
                                        p_this->als_thres_low,
                                        ratio);
        if (ret != AW_OK) {
            return ret;
        }
    }
    if (p_this->als_thres_up != -1) {
        ratio = __LTR553_RATIO_CAL(data0,data1);
        ret = __ltr553_als_set_thres_up(p_this,
                                        p_this->als_thres_up,
                                        ratio);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief LTR553 ALS ����standbyģʽ
 */
aw_local aw_err_t __ltr553_als_enter_standby_mode (awbl_ltr553_dev_t *p_this)
{
    uint8_t reg_val;
    aw_err_t ret;

    /* ������standbyģʽ��ֱ�ӷ��� */
    if ((p_this->als_ps_mode & __LTR553_MODE_ALS_ACTIVE) == 0) {
        return AW_OK;
    }

    ret = __ltr553_reg_read(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    if ((reg_val & __LTR553_VAL_ALS_MODE_ACTIVE) == 0) {
        return AW_OK;
    }
    reg_val &= ~__LTR553_VAL_ALS_MODE_ACTIVE;
    return __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
}

/**
 * \brief LTR553 ALS ����activeģʽ
 */
aw_local aw_err_t __ltr553_als_enter_active_mode (awbl_ltr553_dev_t *p_this)
{
    uint8_t reg_val;
    aw_err_t ret;

    /* ������activeģʽ��ֱ�ӷ��� */
    if (p_this->als_ps_mode & __LTR553_MODE_ALS_ACTIVE) {
        return AW_OK;
    }

    ret = __ltr553_reg_read(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    if (reg_val & __LTR553_VAL_ALS_MODE_ACTIVE) {
        return AW_OK;
    }
    reg_val |= __LTR553_VAL_ALS_MODE_ACTIVE;
    ret = __ltr553_reg_write(p_this, __LTR553_REG_ALS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }
    p_this->als_ps_mode |= __LTR553_MODE_ALS_ACTIVE;

    return AW_OK;
}

/**
 * \brief LTR553 PS ����standbyģʽ
 */
aw_local aw_err_t __ltr553_ps_enter_standby_mode (awbl_ltr553_dev_t *p_this)
{
    uint8_t reg_val;
    aw_err_t ret;

    /* ������standbyģʽ��ֱ�ӷ��� */
    if ((p_this->als_ps_mode & __LTR553_MODE_PS_ACTIVE) == 0) {
        return AW_OK;
    }

    ret = __ltr553_reg_read(p_this, __LTR553_REG_PS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    if ((reg_val & __LTR553_VAL_PS_MODE_ACTIVE) == 0) {
        return AW_OK;
    }
    reg_val &= ~__LTR553_VAL_PS_MODE_ACTIVE;
    return __ltr553_reg_write(p_this, __LTR553_REG_PS_CONTR, &reg_val, 1);
}

/**
 * \brief LTR553 PS ����activeģʽ
 */
aw_local aw_err_t __ltr553_ps_enter_active_mode (awbl_ltr553_dev_t *p_this)
{
    uint8_t reg_val;
    aw_err_t ret;

    /* ������activeģʽ��ֱ�ӷ��� */
    if (p_this->als_ps_mode & __LTR553_MODE_PS_ACTIVE) {
        return AW_OK;
    }

    ret = __ltr553_reg_read(p_this, __LTR553_REG_PS_CONTR, &reg_val, 1);
    if (ret != AW_OK) {
        return ret;
    }

    if (reg_val & __LTR553_VAL_PS_MODE_ACTIVE) {
        return AW_OK;
    }
    reg_val |= __LTR553_VAL_PS_MODE_ACTIVE;
    return __ltr553_reg_write(p_this, __LTR553_REG_PS_CONTR, &reg_val, 1);
}

/**
 * \brief ��ȡLTR553����������
 */
aw_local aw_err_t __ltr553_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);

    int32_t i;
    int32_t idx;
    uint8_t reg_val[6];    /* �����I2C�ж�ȡ���ݵ���ʱ���� */
    uint8_t status;
    uint8_t meas_rate;

    aw_err_t ret;

    ret = __ltr553_reg_read(p_this, __LTR553_REG_ALS_PS_STATUS, &status, 1);
    if (ret != AW_OK) {
        return ret;
    }

    for (i = 0; i < num; i++) {
        idx = p_ids[i] - p_devinfo->start_id;
        
        if (idx == 0){
            if ((status & __LTR553_VAL_ALS_DATA_VALID) != 0) {
                /* ����δ׼���� */
                AWBL_SENSOR_VAL_SET_INVALID(&p_buf[i], -AW_EBUSY);
                continue ;
            }
            /* оƬ���壬ͨ��0��ͨ��1����һ���ȡ���ұ����ȶ�ȡ���ֽڣ�Ȼ���ȡ���ֽ� */
            ret = __ltr553_reg_read(p_this,
                                __LTR553_REG_ALS_DATA_CH1_0,
                                &reg_val[0],
                                4);

            p_buf[i].val = __ltr553_als_adc_2_lux(reg_val,
                                            p_devinfo->als_gain,
                                            p_devinfo->als_integration_time);
            p_buf[i].unit = AW_SENSOR_UNIT_MILLI;
            /* �������threshold����Ϊ��standbyģʽ������thresʱδд��Ĵ���  */
            __ltr553_als_update_settings(p_this, reg_val);
        } else if (idx == 1){
            ret = __ltr553_reg_read(p_this,
                                __LTR553_REG_PS_DATA_0,
                                &reg_val[4],
                                2);

            p_buf[i].val = __ltr553_ps_cal(&reg_val[4]);
            p_buf[i].unit = AW_SENSOR_UNIT_BASE;
        }
    }
    
    return AW_OK;
}

/**
 * \brief ʹ��LTR553��������һ������ͨ��
 */
aw_local aw_err_t __ltr553_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    int32_t i;
    int32_t idx;
    aw_err_t ret;
    uint8_t reg_val[4];

    for (i = 0; i < num; i++) {
        idx = p_ids[i] - p_devinfo->start_id;

        if (idx == 0) {
            ret = __ltr553_als_enter_active_mode(p_this);
            if (ret != AW_OK) {
                return ret;
            }

            /* �ȴ�һ�β������ */
            aw_mdelay(__LTR553_ACTIVE_MODE_WAIT_TIME + p_this->als_meas_time);

            /* ���²���ֵ */
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_ALS_DATA_CH1_0,
                                    reg_val,
                                    4);
            if (ret != AW_OK) {
                return ret;
            }

            return __ltr553_als_update_settings(p_this, reg_val);

        } else if ( idx == 1 ) {
            ret = __ltr553_ps_enter_active_mode(p_this);
            if (ret != AW_OK) {
                return ret;
            }
        }
    }
    
    return AW_OK;
}

/**
 * \brief ����LTR553��������һ������ͨ��
 */
aw_local aw_err_t __ltr553_disable(void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    int32_t i;
    int32_t idx;
    aw_err_t ret;

    for (i = 0; i < num; i++) {
        idx = p_ids[i] - p_devinfo->start_id;

        if (idx == 0) {
            ret = __ltr553_als_enter_standby_mode(p_this);
            if (ret != AW_OK) {
                return ret;
            }
        } else if ( idx == 1 ){
            ret = __ltr553_ps_enter_standby_mode(p_this);
            if (ret != AW_OK) {
                return ret;
            }
        }
    }

    return AW_OK;
}

/**
 * \brief als���ݲ����ʼ���Ĵ���ֵ
 */
aw_local uint8_t __ltr553_als_sample_rate_2_reg(int32_t sample_rate)
{
    int32_t i;

    for (i = 0; i < AW_NELEMENTS(__ltr553_als_meas_time); i++) {
        if (sample_rate <= __ltr553_als_meas_time[i]) {
            return i;
        }
    }
    return 0;
}

/**
 * \brief als���ݲ����ʼ���Ĵ���ֵ
 */
aw_local uint8_t __ltr553_ps_sample_rate_2_reg(int32_t sample_rate)
{
    int32_t i;

    if (sample_rate <= 10) {
        return 0x08;
    }
    for (i = 0; i < AW_NELEMENTS(__ltr553_ps_meas_time) - 1; i++) {
        if (sample_rate <= __ltr553_ps_meas_time[i]) {
            return i;
        }
    }
    return 0x02;/* Ĭ��ֵ */
}

/******************************************************************************/
/**
 * \brief ����LTR553��һ������
 */
aw_local aw_err_t __ltr553_attr_set (void                   *p_cookie,
                                     int                     id,
                                     int                     attr,
                                     const aw_sensor_val_t  *p_val)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t reg_val[4];
    uint8_t idx = id - p_devinfo->start_id;
    aw_err_t ret;
    int32_t actual_val;
    int32_t ratio;
    int32_t data0;
    int32_t data1;
    
    if ((idx != 0) && (idx != 1)) {
        return -AW_ENOTSUP;
    }

    switch ( attr ){
    /* ��������ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:

        actual_val = __sersor_val_cal(p_val);
        //actual_val = aw_sensor_val_unit_convert(p_val,1,AW_SENSOR_UNIT_BASE);
        if (idx == 0){/* als */
            p_this->als_thres_low = actual_val;
            /* ����standbyģʽ��ֻ��������,�������������ݸ��½�ȥ */
            if ((p_this->als_ps_mode & __LTR553_MODE_ALS_ACTIVE) == 0) {
                return AW_OK;
            } else {
                ret = __ltr553_reg_read(p_this,
                                        __LTR553_REG_ALS_DATA_CH1_0,
                                        reg_val,
                                        4);
                if (ret != AW_OK) {
                    return ret;
                }
                data0 = (int32_t)__LTR553_REG_2_DATA0(reg_val);
                data1 = (int32_t)__LTR553_REG_2_DATA1(reg_val);
                ratio = __LTR553_RATIO_CAL(data0,data1);
                return __ltr553_als_set_thres_low(p_this, actual_val, ratio);
            }
        } else {/* ps */
            p_this->ps_thres_low = actual_val;
            reg_val[0]  = (uint8_t)(actual_val);
            reg_val[1]  = (uint8_t)(actual_val >> 8);
            return __ltr553_reg_write(p_this,
                                      __LTR553_REG_PS_THRES_LOW_0,
                                      reg_val,
                                      2);
        }

    /* ��������ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:

        actual_val = __sersor_val_cal(p_val);
        if (idx == 0){/* als */
            p_this->als_thres_up = actual_val;
            /* ����standbyģʽ��ֻ��������,��enable��д�룬�������������ݸ��½�ȥ */
            if ((p_this->als_ps_mode & __LTR553_MODE_ALS_ACTIVE) == 0) {
                return AW_OK;
            } else {
                ret = __ltr553_reg_read(p_this,
                                        __LTR553_REG_ALS_DATA_CH1_0,
                                        reg_val,
                                        2);
                if (ret != AW_OK) {
                    return ret;
                }
                data0 = (int32_t)__LTR553_REG_2_DATA0(reg_val);
                data1 = (int32_t)__LTR553_REG_2_DATA1(reg_val);
                ratio = __LTR553_RATIO_CAL(data0,data1);
                return __ltr553_als_set_thres_up(p_this, actual_val, ratio);
            }
        } else {/* ps */
            p_this->ps_thres_up = actual_val;
            reg_val[0]          = (uint8_t)(actual_val);
            reg_val[1]          = (uint8_t)(actual_val >> 8);
            return __ltr553_reg_write(p_this,
                                      __LTR553_REG_PS_THRES_UP_0,
                                      reg_val,
                                      2);
        }

    /* ����ƫ��ֵ  */
    case AW_SENSOR_ATTR_OFFSET:

        if (idx == 0) {/* als��֧�� */
            return -AW_ENOTSUP;
        } else {
            actual_val  = __sersor_val_cal(p_val);
            reg_val[0]  = (uint8_t)((actual_val >> 8) & 0x03);
            reg_val[1]  = (uint8_t)actual_val;
            return __ltr553_reg_write(p_this,
                                      __LTR553_REG_PS_OFFSET_1,
                                      reg_val,
                                      2);
        }
        break;

    /* ���ò����� */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        actual_val = __sersor_val_cal(p_val);
        if (idx == 0) {/* als */
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_ALS_MEAS_RATE,
                                    reg_val,
                                    1);
            if (ret != AW_OK) {
                return ret;
            }
            reg_val[0] |= (~0x07) & __ltr553_als_sample_rate_2_reg(actual_val);
            return __ltr553_reg_write(p_this,
                                      __LTR553_REG_ALS_MEAS_RATE,
                                      reg_val,
                                      1);
        } else {/* ps */
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_PS_MEAS_RATE,
                                    reg_val,
                                    1);
            if (ret != AW_OK) {
                return ret;
            }
            reg_val[0] |= (~0x07) & __ltr553_ps_sample_rate_2_reg(actual_val);
            return __ltr553_reg_write(p_this,
                                      __LTR553_REG_PS_MEAS_RATE,
                                      reg_val,
                                      1);
        }
        break;

    /* �����жϴ����������� */
    case AW_SENSOR_ATTR_DURATION_DATA:
        actual_val = __sersor_val_cal(p_val);
        if ((actual_val < 1) || (actual_val > 16)) {
            return -AW_ENOTSUP;
        }

        if (idx == 0) {
            /* �Ĵ���ֵ��0��ʼ */
            reg_val[0] = (uint8_t)((actual_val - 1) & 0x0f);
        } else {
            reg_val[0] = (uint8_t)(((actual_val - 1) << 4) & 0xf0);
        }
        return __ltr553_reg_write(p_this,
                                  __LTR553_REG_INTERRUPT_PERSIST,
                                  reg_val,
                                  1);

    default:
        return -AW_ENOTSUP;
    }
    
    return AW_OK;
}

/******************************************************************************/
/**
 * \brief ��ȡLTR553��һ��ͨ������
 */
aw_local aw_err_t __ltr553_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    uint8_t reg_val[2];
    uint8_t idx = id - p_devinfo->start_id;
    aw_err_t ret;

    if ((idx != 0) && (idx != 1)) {
        return -AW_ENOTSUP;
    }

    switch ( attr ){
    
    /* ��ȡ����ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        if (idx == 0){
            if (p_this->als_thres_low == -1) {
                p_val->val = 0;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            } else {
                p_val->val = p_this->als_thres_low;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            }
        } else {
            if (p_this->ps_thres_low == -1) {
                p_val->val = 0;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            } else {
                p_val->val = p_this->ps_thres_low;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            }
        }
        break;

    /* ��ȡ����ֵ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        if (idx == 0){
            if (p_this->als_thres_up == -1) {
                p_val->val = 0xffff;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            } else {
                p_val->val = p_this->als_thres_up;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            }
        } else {
            if (p_this->ps_thres_up == -1) {
                p_val->val = 0xffff;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            } else {
                p_val->val = p_this->ps_thres_up;
                p_val->unit = AW_SENSOR_UNIT_BASE;
            }
        }
        break;

    /* ��ȡƫ��ֵ  */
    case AW_SENSOR_ATTR_OFFSET:
        if (idx == 0) {
            return -AW_ENOTSUP;
        } else {
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_PS_OFFSET_1,
                                    reg_val,
                                    2);
            if (ret != AW_OK) {
                return ret;
            }
            p_val->val = (int32_t)( ((reg_val[0] & 0x03) << 8) + reg_val[1]);
            p_val->unit = AW_SENSOR_UNIT_BASE;
        }
        break;

    case AW_SENSOR_ATTR_SAMPLING_RATE:
        if (idx == 0) {
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_ALS_MEAS_RATE,
                                    reg_val,
                                    1);
            if (ret != AW_OK) {
                return ret;
            }
            p_val->val = (int32_t)(__ltr553_als_meas_time[reg_val[0] & 0x07]);
            p_val->unit = AW_SENSOR_UNIT_BASE;
        } else {
            ret = __ltr553_reg_read(p_this,
                                    __LTR553_REG_PS_MEAS_RATE,
                                    reg_val,
                                    1);
            if (ret != AW_OK) {
                return ret;
            }
            if (reg_val[0] >= 0x08) {
                reg_val[0] = 0x08;
            }
            p_val->val = (int32_t)(__ltr553_ps_meas_time[reg_val[0] & 0x0f]);
            p_val->unit = AW_SENSOR_UNIT_BASE;
        }
        break;

    /* �жϴ����������� */
    case AW_SENSOR_ATTR_DURATION_DATA:

        ret = __ltr553_reg_read(p_this,
                                __LTR553_REG_INTERRUPT_PERSIST,
                                reg_val,
                                1);
        if (ret != AW_OK) {
            return ret;
        }
        if (idx == 0) {
            p_val->val = (int32_t)((reg_val[0] & 0x0f) + 1);
        } else {
            p_val->val = (int32_t)(((reg_val[0] >> 4) & 0x0f) + 1);
        }
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
 * \note ֧�ֵĴ���ģʽ����崫������أ����ִ�������֧���κδ���ģʽ����ʱ��
 * �ɽ��ú�������ΪNULL��
 */
aw_local aw_err_t __ltr553_trigger_cfg (void                  *p_cookie,
                                        int                    id,
                                        uint32_t               flags,
                                        aw_sensor_trigger_cb_t pfn_cb,
                                        void                  *p_arg)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    
    int32_t idx = id - p_devinfo->start_id;

    if (((idx != 0) && (idx != 1)) || (flags != AW_SENSOR_TRIGGER_THRESHOLD)) {
        return -AW_ENOTSUP;
    }

    if (idx == 0) {
        p_this->pfn_als_trigger_cb = pfn_cb;
        p_this->p_als_arg = p_arg;
    } else if (idx == 1) {
        p_this->pfn_ps_trigger_cb = pfn_cb;
        p_this->p_ps_arg = p_arg;
    }

    return AW_OK;

}

/**
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
 */
aw_local aw_err_t __ltr553_trigger_on (void *p_cookie, int id)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    int32_t idx;
    uint8_t reg_val;/* �Ĵ�������ֵ */
    aw_err_t ret;

    if (p_devinfo->alert_pin == -1) {
        return -AW_ENOTSUP;
    }

    ret = __ltr553_reg_read(p_this,
                            __LTR553_REG_INTERRUPT,
                            &reg_val,
                            1);

    idx = id - p_devinfo->start_id;
    if (idx == 0) {
        if ((reg_val & __LTR553_VAL_INTERRUPT_ALS) != 0) {
            /* ������ */
            return AW_OK;
        }
        if (p_this->pfn_als_trigger_cb == NULL) {/* ���лص����� */
            return -AW_ENOTSUP;
        }
        p_this->alert_chan |= __LTR553_VAL_INTERRUPT_ALS;
    } else if (idx == 1) {
        if ((reg_val & __LTR553_VAL_INTERRUPT_PS) != 0) {
            return AW_OK;
        }
        if (p_this->pfn_ps_trigger_cb == NULL) {/* ���лص����� */
            return -AW_ENOTSUP;
        }
        p_this->alert_chan |= __LTR553_VAL_INTERRUPT_PS;
    } else {
        return -AW_ENOTSUP;
    }

    ret = aw_gpio_trigger_on(p_devinfo->alert_pin);
    if (ret != AW_OK) {
        return ret;
    }
    return __ltr553_reg_write(p_this,
                              __LTR553_REG_INTERRUPT,
                              &p_this->alert_chan,
                              1);
}

/**
 * \brief �رմ�����һ��ͨ����������һ�������ص�����
 */
aw_local aw_err_t __ltr553_trigger_off (void *p_cookie, int id)
{
    __LTR553_DEV_DECL(p_this, p_cookie);
    __LTR553_DEVINFO_DECL(p_devinfo, p_cookie);
    int32_t idx;
    uint8_t reg_val;/* �Ĵ�������ֵ */
    aw_err_t ret;

    if (p_devinfo->alert_pin == -1) {
        return -AW_ENOTSUP;
    }

    ret = __ltr553_reg_read(p_this, __LTR553_REG_INTERRUPT, &reg_val, 1);

    idx = id - p_devinfo->start_id;
    if (idx == 0) {
        if ((reg_val & __LTR553_VAL_INTERRUPT_ALS) == 0) {
            /* ������ */
            return AW_OK;
        }
        p_this->alert_chan &= ~__LTR553_VAL_INTERRUPT_ALS;
    } else if (idx == 1) {
        if ((reg_val & __LTR553_VAL_INTERRUPT_PS) == 0) {
            return AW_OK;
        }
        p_this->alert_chan &= ~__LTR553_VAL_INTERRUPT_PS;
    } else {
        return -AW_ENOTSUP;
    }

    if (p_this->alert_chan == __LTR553_VAL_INTERRUPT_NONE) {
        ret = aw_gpio_trigger_off(p_devinfo->alert_pin);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return __ltr553_reg_write(p_this, __LTR553_REG_INTERRUPT, &reg_val, 1);
}

/**
 * \brief LTR553 ��׼����ӿڻ�ȡ
 */
aw_local aw_err_t __ltr553_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    __LTR553_DEV_DECL(p_this, p_dev);
    __LTR553_DEVINFO_DECL(p_devinfo, p_dev);
    struct awbl_sensor_service *p_serv = &p_this->sensor_serv;

    p_serv->p_next      = NULL;
    p_serv->p_cookie    = (void *)p_dev;
    p_serv->start_id    = p_devinfo->start_id;/* ���մ�����ͨ����ʼ */
    p_serv->p_servinfo  = &__g_ltr553_servinfo;
    p_serv->p_servfuncs = &__g_ltr553_servfuncs;

    *(struct awbl_sensor_service **)p_arg = p_serv;

    return AW_OK;
}

/**
 * \brief �� LTR553 ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_ltr553_drv_register (void)
{
    awbl_drv_register(&__g_ltr553_drvinfo);
}

/* end of file */
 
