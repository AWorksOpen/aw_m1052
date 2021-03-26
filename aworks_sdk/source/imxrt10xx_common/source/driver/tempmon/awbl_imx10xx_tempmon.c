/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      aworks.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief iMX.RT10xx内部温度监控驱动文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-04-11  mex, first implementation.
 * \endinternal
 */

#include "aw_common.h"
#include "aw_int.h"
#include "awbus_lite.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "driver/tempmon/awbl_imx10xx_tempmon.h"
#include "aw_delay.h"

/*******************************************************************************
  local defines
*******************************************************************************/

/**
 * \brief imx10xx tempmon register block
 */
typedef struct __imx10xx_tempmon_regs {
    volatile uint32_t tempsense0;       /**< \brief tempsense0 */
    volatile uint32_t tempsense0_set;   /**< \brief tempsense0_set */
    volatile uint32_t tempsense0_clr;   /**< \brief tempsense0_clr */
    volatile uint32_t tempsense0_tog;   /**< \brief tempsense0_tog */
    volatile uint32_t tempsense1;       /**< \brief tempsense1 */
    volatile uint32_t tempsense1_set;   /**< \brief tempsense1_set */
    volatile uint32_t tempsense1_clr;   /**< \brief tempsense1_clr */
    volatile uint32_t tempsense1_tog;   /**< \brief tempsense1_tog */
    volatile uint32_t reserved0[60];    /**< \brief reserved */
    volatile uint32_t tempsense2;       /**< \brief tempsense2 */
    volatile uint32_t tempsense2_set;   /**< \brief tempsense2_set */
    volatile uint32_t tempsense2_clr;   /**< \brief tempsense2_clr */
    volatile uint32_t tempsense2_tog;   /**< \brief tempsense2_tog */
} __imx10xx_tempmon_regs_t;

/**
 * \brief tempmon reg_field
 * @{
 */

#define __TEMPMON_POWER                 0
#define __TEMPMON_MEASURE               1
#define __TEMPMON_FINISHED              2
#define __TEMPMON_TEMP_VALUE            8
#define __TEMPMON_TEMP_VALUE_LEN        12
#define __TEMPMON_HIGH_ALARM            20
#define __TEMPMON_HIGH_ALARM_LEN        12

#define __TEMPMON_MEASURE_FREQ          0
#define __TEMPMON_MEASURE_FREQ_LEN      16
#define __TEMPMON_MEASURE_FREQ_VAL      327

#define __TEMPMON_LOW_ALARM             0
#define __TEMPMON_LOW_ALARM_LEN         12
#define __TEMPMON_PANIC_ALARM           16
#define __TEMPMON_PANIC_ALARM_LEN       12

#define __MISC0_REFTOP_SELBIASOFF       3

#define __TEMPMON_IRQ_TEMP_PANIC        27
#define __TEMPMON_IRQ_TEMP_LOW          28
#define __TEMPMON_IRQ_TEMP_HIGH         29

#define __IMX10xx_OCOTP_ANA1_ADDR        0x401F44E0
#define __IMX10xx_XTALOSC24M_MISC0_ADDR  0x400D8150
#define __IMX10xx_CCM_ANALOG_MISC1_ADDR  0x400D8160

#define __REG_SET                       0x4
#define __REG_CLR                       0x8
#define __REG_TOG                       0xC

/** @} */

/**
 * It defines the temperature in millicelsius for passive trip point
 * that will trigger cooling action when crossed.
 */
#define __FACTOR0                           10000000
#define __FACTOR1                           15423
#define __FACTOR2                           4148468
#define __OFFSET                            3580661

typedef struct imx10xx_thermal_data {

    uint64_t c1;
    uint64_t c2;        /* See formula in __imx10xx_tempmon_data_calibrate() */

} imx10xx_thermal_data_t;

/** \brief Macro that covert p_serv to awbl_imx10xx_tempmon_dev */
#define __SERV_GET_THERM_DEV(p_serv)   \
        (AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_tempmon_dev, temp_serv))

/** \brief Macro that covert p_serv to awbl_dev */
#define __SERV_GET_DEV(p_serv)   \
        ((AW_CONTAINER_OF(p_serv, struct awbl_imx10xx_tempmon_dev, temp_serv))->dev)

/** \brief Macro that covert p_serv to awbl_imx10xx_tempmon_devinfo */
#define __SERV_GET_THERM_INFO(p_serv) \
        ((struct awbl_imx10xx_tempmon_devinfo *)AWBL_DEVINFO_GET(&__SERV_GET_DEV(p_serv)))

/** \brief Macro that covert struct awbl_dev to awbl_imx10xx_tempmon_dev */
#define __ADEV_GET_TEMPMON_DEV(p_dev) \
    (awbl_imx10xx_tempmon_dev_t *)(p_dev)

/** \brief Macro that covert struct awbl_dev to awbl_imx10xx_tempmon_devinfo */
#define __ADEV_GET_TEMPMON_INFO(p_dev) \
    (awbl_imx10xx_tempmon_dev_info_t *)AWBL_DEVINFO_GET(p_dev)

/* declare tempmon device instance */
#define __TEMPMON_DEV_DECL(p_this, p_dev) \
    struct awbl_imx10xx_tempmon_dev *p_this = \
        (struct awbl_imx10xx_tempmon_dev *)(p_dev)

/*******************************************************************************
  forward declarations
*******************************************************************************/

aw_local void __imx10xx_tempmon_inst_init2 (awbl_dev_t *p_dev);
aw_local void __imx10xx_tempmon_isr (void *p_cookie);
aw_local int  __imx10xx_tempmon_temp_get (void *p_cookie);
aw_local aw_err_t __imx10xx_tempmon_int_state_get (uint8_t *p_int_state);
aw_local aw_err_t __imx10xx_tempmon_data_calibrate (imx10xx_thermal_data_t *data);

/*******************************************************************************
  globals
*******************************************************************************/

static imx10xx_thermal_data_t      __g_imx10xx_thermal_data;
static awbl_imx10xx_tempmon_dev_t *__gp_imx10xx_tempmon_dev;

aw_const struct awbl_drvfuncs __g_imx10xx_tempmon_awbl_drvfuncs = {
    NULL,                               /* devInstanceInit */
    __imx10xx_tempmon_inst_init2,        /* devInstanceInit2 */
    NULL                                /* devConnect */
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * driver registration (must defined as aw_const)
 */
aw_local aw_const struct awbl_drvinfo __g_imx10xx_tempmon_drv_registration = {
    AWBL_VER_1,                         /* awb_ver */
    AWBL_BUSID_PLB,                     /* bus_id */
    AWBL_IMX10XX_TEMPMON_NAME,          /* p_drvname*/
    &__g_imx10xx_tempmon_awbl_drvfuncs, /* p_busfuncs */
    NULL,                               /* p_methods */
    NULL                                /* pfunc_drv_probe */
};

/**
 * \brief tempmon task
 */
aw_local void __tempmon_task_entry (void *p_arg)
{
    awbl_imx10xx_tempmon_dev_t      *p_dev     = __ADEV_GET_TEMPMON_DEV(p_arg);
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(p_dev);
    __imx10xx_tempmon_regs_t        *p_reg     = (__imx10xx_tempmon_regs_t *)(p_devinfo->regbase);

    uint8_t int_state;

    AW_FOREVER {

        AW_SEMB_TAKE(p_dev->tempmon_sem_sync, AW_SEM_WAIT_FOREVER);

        int_state = 0;

        AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_MEASURE));       /* stop */

        __imx10xx_tempmon_int_state_get(&int_state);

        /* clr corresponding int flag */
        AW_REG_WRITE32(__IMX10xx_CCM_ANALOG_MISC1_ADDR + __REG_CLR,
                       AW_BIT(__TEMPMON_IRQ_TEMP_PANIC) |
                       AW_BIT(__TEMPMON_IRQ_TEMP_LOW)   |
                       AW_BIT(__TEMPMON_IRQ_TEMP_HIGH));

        AW_REG_WRITE32(&p_reg->tempsense0_set, (1 << __TEMPMON_MEASURE));       /* start */

        /* callback is not null */
        if (p_dev->pfn_tempmon_callback != NULL) {
            p_dev->pfn_tempmon_callback(p_dev->p_arg, int_state);
        }
    }
}

/* adc service functions (must defined as aw_const) */
aw_local struct awbl_temp_servfuncs __g_temp_servfuncs = {
    __imx10xx_tempmon_temp_get,
};

/**
 * \brief tempmon step 2 init
 */
aw_local void __imx10xx_tempmon_inst_init2 (awbl_dev_t *p_dev)
{
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(p_dev);
    awbl_imx10xx_tempmon_dev_t      *p_this    = __ADEV_GET_TEMPMON_DEV(p_dev);
    __imx10xx_tempmon_regs_t        *p_reg     = (__imx10xx_tempmon_regs_t *)(p_devinfo->regbase);

    /* platform init (enable tempmon clock) */
    if (p_devinfo->pfn_plfm_init != NULL) {
        p_devinfo->pfn_plfm_init();
    }

    p_this->temp_serv.id          = AWBL_DEV_UNIT_GET(p_this);
    p_this->temp_serv.p_servfuncs = &__g_temp_servfuncs;

    /* add temp serv to list */
    awbl_temp_serv_add(&p_this->temp_serv);

    /* calibrate */
    __imx10xx_tempmon_data_calibrate(&__g_imx10xx_thermal_data);

    /* load local dev */
    __gp_imx10xx_tempmon_dev = p_this;

    /* set temp limit */
    awbl_imx10xx_tempmon_limit_temp_set(AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_LOW,
                        p_devinfo->low_alarm_temp);     /* set low limit */
    awbl_imx10xx_tempmon_limit_temp_set(AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_HIGH,
                        p_devinfo->high_alarm_temp);    /* set high limit */
    awbl_imx10xx_tempmon_limit_temp_set(AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_PANIC,
                        p_devinfo->panic_temp);         /* set reset limit */

    /* init some params */
    p_this->pfn_tempmon_callback = NULL;
    p_this->p_arg                = NULL;
    p_this->func_exist           = 0;

    /* open tempmon sensor */
    AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_POWER));         /* power on */
    AW_REG_WRITE32(__IMX10xx_XTALOSC24M_MISC0_ADDR + __REG_SET, AW_BIT(__MISC0_REFTOP_SELBIASOFF));

    AW_REG_BITS_SET32(&p_reg->tempsense1, __TEMPMON_MEASURE_FREQ,
                                          __TEMPMON_MEASURE_FREQ_LEN,
                                          __TEMPMON_MEASURE_FREQ_VAL);

    AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_MEASURE));       /* stop */
    AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_FINISHED));      /* invalid */
    AW_REG_WRITE32(&p_reg->tempsense0_set, (1 << __TEMPMON_MEASURE));       /* start */

    /* init device mutex */
    AW_MUTEX_INIT(p_this->mutex, AW_SEM_Q_PRIORITY);

    /* initialize the stack binary semaphore */
    AW_SEMB_INIT(p_this->tempmon_sem_sync, AW_SEM_EMPTY, AW_SEM_Q_FIFO);

    /* 初始化任务sim_trans_task */
    AW_TASK_INIT(p_this->tempmon_task_decl,         /* task entity */
                 "tempmon_task",                    /* task name */
                 p_devinfo->task_prio,              /* task prio */
                 __TEMPMON_TASK_STACK_SIZE,         /* task stack size */
                 __tempmon_task_entry,              /* task entry */
                 (void *)p_dev);                    /* task entry param */

    /* start tempmon_task */
    AW_TASK_STARTUP(p_this->tempmon_task_decl);
}

/**
 * \brief tempmon isr
 */
aw_local void __imx10xx_tempmon_isr (void *p_cookie)
{
    awbl_imx10xx_tempmon_dev_t      *p_dev     = __ADEV_GET_TEMPMON_DEV(p_cookie);
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(p_cookie);

    /* param invalid? */
    if (p_cookie == NULL) {
        return;
    }

    AW_SEMB_GIVE(p_dev->tempmon_sem_sync);

    /* disable interrupt */
    aw_int_disable(p_devinfo->inum);
}

/**
 * \brief tempmon get current temperature
 */
aw_local int __imx10xx_tempmon_temp_get (void *p_cookie)
{
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __SERV_GET_THERM_INFO(p_cookie);
    __imx10xx_tempmon_regs_t        *p_reg     = (__imx10xx_tempmon_regs_t *)(p_devinfo->regbase);
    imx10xx_thermal_data_t          *data      = &__g_imx10xx_thermal_data;

    uint32_t n_meas;

    /* param invalid? */
    if (p_cookie == NULL) {
        return -AW_EINVAL;
    }

    /* finish? */
    while (0 == AW_REG_BIT_ISSET32(&p_reg->tempsense0, __TEMPMON_FINISHED)) {
        /* task level delay 1ms */
        aw_mdelay(1);
    }

    n_meas = AW_REG_BITS_GET32(&p_reg->tempsense0, __TEMPMON_TEMP_VALUE, __TEMPMON_TEMP_VALUE_LEN);

    AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_FINISHED));

    /* 1000 times */
    return ((data->c2 - n_meas * data->c1 + __OFFSET) / 1000);
}

/**
 * \brief tempmon get interrupt state
 */
aw_local aw_err_t __imx10xx_tempmon_int_state_get (uint8_t *p_int_state)
{
    uint32_t int_flag;

    /* param invalid? */
    if (p_int_state == NULL) {
        return -AW_EINVAL;
    }

    /* get int flag */
    int_flag = AW_REG_READ32(__IMX10xx_CCM_ANALOG_MISC1_ADDR);

    if (AW_BIT(__TEMPMON_IRQ_TEMP_PANIC) & int_flag) {
        *p_int_state |= AWBL_IMX10xx_TEMPMON_STATE_TEMP_PANIC;
    }

    if (AW_BIT(__TEMPMON_IRQ_TEMP_LOW) & int_flag) {
        *p_int_state |= AWBL_IMX10xx_TEMPMON_STATE_TEMP_LOW;
    }

    if (AW_BIT(__TEMPMON_IRQ_TEMP_HIGH) & int_flag) {
        *p_int_state |= AWBL_IMX10xx_TEMPMON_STATE_TEMP_HIGH;
    }

    return AW_OK;
}

/**
 * \brief tempmon calibrate temp prama
 */
aw_local aw_err_t __imx10xx_tempmon_data_calibrate (imx10xx_thermal_data_t *data)
{
    int t1, n1;
    uint64_t temp64;

    /* param invalid? */
    if (data == NULL) {
        return -AW_EINVAL;
    }

    /*
     * Sensor data layout:
     *   [31:20] - sensor value @ 25C
     */
    n1 = AW_REG_READ32(__IMX10xx_OCOTP_ANA1_ADDR) >> 20;
    t1 = 25;            /* t1 always 25C */

    temp64 = __FACTOR0;
    temp64 *= 1000000;
    temp64 = temp64 / (__FACTOR1 * n1 - __FACTOR2);
    data->c1 = temp64;
    data->c2 = n1 * data->c1 + 1000000 * t1;

    return AW_OK;
}

/**
 * \brief AWBus imx10xx tempmon set limit temp
 */
aw_err_t awbl_imx10xx_tempmon_limit_temp_set (uint8_t flags, int limit_temp)
{
    if (__gp_imx10xx_tempmon_dev == NULL) {
        return -AW_ENXIO;
    }

    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(__gp_imx10xx_tempmon_dev);
    __imx10xx_tempmon_regs_t        *p_reg     = (__imx10xx_tempmon_regs_t *)(p_devinfo->regbase);
    imx10xx_thermal_data_t          *data      = &__g_imx10xx_thermal_data;

    if ((limit_temp < -30000) || (limit_temp > 115000)) {
        return -AW_EINVAL;
    }

    AW_REG_WRITE32(&p_reg->tempsense0_clr, (1 << __TEMPMON_MEASURE));       /* stop */

    /* clr corresponding int flag */
    AW_REG_WRITE32(__IMX10xx_CCM_ANALOG_MISC1_ADDR + __REG_CLR,
                   AW_BIT(__TEMPMON_IRQ_TEMP_PANIC) |
                   AW_BIT(__TEMPMON_IRQ_TEMP_LOW)   |
                   AW_BIT(__TEMPMON_IRQ_TEMP_HIGH));

    int temp_value = (data->c2 + __OFFSET - limit_temp * 1000) / data->c1;

    switch (flags) {

    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_LOW:
        AW_REG_BITS_SET32(&p_reg->tempsense2, __TEMPMON_LOW_ALARM,
                                              __TEMPMON_LOW_ALARM_LEN,
                                              temp_value);
        break;

    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_HIGH:
        AW_REG_BITS_SET32(&p_reg->tempsense0, __TEMPMON_HIGH_ALARM,
                                              __TEMPMON_HIGH_ALARM_LEN,
                                              temp_value);
        break;

    /* reset limit(SRC WARM default is enable) */
    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_PANIC:
        AW_REG_BITS_SET32(&p_reg->tempsense2, __TEMPMON_PANIC_ALARM,
                                              __TEMPMON_PANIC_ALARM_LEN,
                                              temp_value);
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    AW_REG_WRITE32(&p_reg->tempsense0_set, (1 << __TEMPMON_MEASURE));       /* start */

    return AW_OK;
}

/**
 * \brief AWBus imx10xx tempmon set limit temp
 */
aw_err_t awbl_imx10xx_tempmon_limit_temp_get (uint8_t flags, int *p_limit_temp)
{
    if (__gp_imx10xx_tempmon_dev == NULL) {
        return -AW_ENXIO;
    }

    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(__gp_imx10xx_tempmon_dev);
    __imx10xx_tempmon_regs_t        *p_reg     = (__imx10xx_tempmon_regs_t *)(p_devinfo->regbase);
    imx10xx_thermal_data_t          *data      = &__g_imx10xx_thermal_data;

    int temp_value;

    if (NULL == p_limit_temp) {
        return -AW_EINVAL;
    }

    switch (flags) {

    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_LOW:
        temp_value = AW_REG_BITS_GET32(&p_reg->tempsense2, __TEMPMON_LOW_ALARM,
                                                           __TEMPMON_LOW_ALARM_LEN);
        break;

    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_HIGH:
        temp_value = AW_REG_BITS_GET32(&p_reg->tempsense0, __TEMPMON_HIGH_ALARM,
                                                           __TEMPMON_HIGH_ALARM_LEN);
        break;

    case AWBL_IMX10xx_TEMPMON_LIMIT_TEMP_PANIC:
        temp_value = AW_REG_BITS_GET32(&p_reg->tempsense2, __TEMPMON_PANIC_ALARM,
                                                           __TEMPMON_PANIC_ALARM_LEN);
        break;

    default:
        return -AW_EINVAL;
        break;
    }

    *p_limit_temp = (data->c2 - temp_value * data->c1 + __OFFSET) / 1000;

    return AW_OK;
}

/**
 * \brief AWBus imx10xx tempmon interrupt enable
 */
aw_err_t awbl_imx10xx_tempmon_int_enable (void)
{
    if (__gp_imx10xx_tempmon_dev == NULL) {
        return -AW_ENXIO;
    }

    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(__gp_imx10xx_tempmon_dev);

    /* enable interrupt */
    return (aw_int_enable(p_devinfo->inum));
}

/**
 * \brief AWBus imx10xx tempmon interrupt disable
 */
aw_err_t awbl_imx10xx_tempmon_int_disable (void)
{
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(__gp_imx10xx_tempmon_dev);

    /* disable interrupt */
    return (aw_int_disable(p_devinfo->inum));
}

/**
 * \brief AWBus imx10xx tempmon int connect
 */
aw_err_t awbl_imx10xx_tempmon_int_connect (aw_pfunc_tempmon_callback_t pfn_callback, void *p_arg)
{
    if (__gp_imx10xx_tempmon_dev == NULL) {
        return -AW_ENXIO;
    }

    awbl_imx10xx_tempmon_dev_t      *p_dev     = __gp_imx10xx_tempmon_dev;
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(p_dev);

    /* param invalid? */
    if (pfn_callback == NULL) {
        return -AW_EINVAL;
    }

    /* lcok */
    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);

    /* judge if callback func exist */
    if (p_dev->func_exist == 1) {
        AW_MUTEX_UNLOCK(p_dev->mutex);      /* unlock */
        return -AW_EPERM;
    }

    /* load callback func */
    if (p_dev->func_exist == 0) {

        p_dev->pfn_tempmon_callback = pfn_callback;
        p_dev->p_arg                = p_arg;
        p_dev->func_exist           = 1;

        /* connect interrupt */
        aw_int_connect(p_devinfo->inum, __imx10xx_tempmon_isr, p_dev);
    }

    /* unlock */
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return AW_OK;
}

/**
 * \brief AWBus imx10xx tempmon int disconnect
 */
aw_err_t awbl_imx10xx_tempmon_int_disconnect (void)
{
    if (__gp_imx10xx_tempmon_dev == NULL) {
        return -AW_ENXIO;
    }

    awbl_imx10xx_tempmon_dev_t      *p_dev     = __gp_imx10xx_tempmon_dev;
    awbl_imx10xx_tempmon_dev_info_t *p_devinfo = __ADEV_GET_TEMPMON_INFO(p_dev);

    /* lcok */
    AW_MUTEX_LOCK(p_dev->mutex, AW_SEM_WAIT_FOREVER);

    p_dev->pfn_tempmon_callback = NULL;
    p_dev->p_arg                = NULL;
    p_dev->func_exist           = 0;

    /* connect interrupt */
    aw_int_disconnect(p_devinfo->inum, __imx10xx_tempmon_isr, p_dev);

    /* unlock */
    AW_MUTEX_UNLOCK(p_dev->mutex);

    return AW_OK;
}

/******************************************************************************/
/**
 * \brief TEMPMON driver register
 */
void awbl_imx10xx_tempmon_drv_register (void)
{
    awbl_drv_register(&__g_imx10xx_tempmon_drv_registration);
}

/* end of file */
