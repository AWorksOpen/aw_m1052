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
 * \brief touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.01 16-08-18  anu, first implementation.
 * \endinternal
 */

#include "driver/input/touchscreen/awbl_ts2046.h"
#include "aw_gpio.h"
#include <string.h>
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_mem.h"

#define __TS2046_ADEV_2_DEV(p_adev)   \
    AW_CONTAINER_OF(p_adev, awbl_ts2046_dev_t, adev)
#define __TS2046_TS_SERV_2_DEV(p_ts_serv)   \
    AW_CONTAINER_OF(p_ts_serv, awbl_ts2046_dev_t, ts_serv)
#define __TS2046_DEVINFO_GET(p_adev)     \
    (awbl_ts2046_devinfo_t *)AWBL_DEVINFO_GET(p_adev)

#define __TS2046_SPI_CS_ENABLE   0
#define __TS2046_SPI_CS_DISABLE  1

#define __TS2046_SPI_MODE     AW_SPI_MODE_0   /* SPIģʽ0 */
#define __TS2046_SPI_BPW      8               /* SPI����λ����ȡֵֻ����4��8��16 */

#define __TS2046_READ_X       0xd1  /** \brief ��tsc2046���Ͳ���x������*/
#define __TS2046_READ_Y       0x91  /** \brief ��tsc2046���Ͳ���y������*/
#define __TS2046_READ_Z1      0xb1  /** \brief ��tsc2046���Ͳ���z1������*/
#define __TS2046_READ_Z2      0xc1  /** \brief ��tsc2046���Ͳ���z2������*/

#define __TS2046_LOCK         0x01  /** \brief ��ס������������*/
#define __TS2046_UNLOCK       0x00  /** \brief ����������������*/

#define __TS2046_RELASED      0X01
#define __TS2046_PRESSED      0X00

#define __SAMPLE_AXLE_NUM     4     /* ���������� x y z1 z2 */

#define __TS2046_PRESSURE_MAX_VAL   1200//100  /* ѹ��ֵС�ڸ�ֵ������� 750 */
#define __TS2046_PRESSURE_MIN_VAL   10    /* ѹ��ֵ���ڸ�ֵ������� */

#define __TS2046_MAX_12BIT   ((1<<12)-1)

#define __TS2046_TOUCH_OHM   400   /* �������迹 Ĭ�� 400�� */

static aw_err_t __ts2046_read (awbl_ts2046_dev_t *p_dev,
                               awbl_ts2046_val_t *p_recvbuf,
                               int val)
{
    awbl_ts2046_devinfo_t *p_info = __TS2046_DEVINFO_GET(&p_dev->adev);

    struct aw_spi_message   msg;
    /* 1������ 1������ ÿ������ִ��1�� + 1�ε��� */
    struct aw_spi_transfer  trans[2];
    struct aw_spi_transfer *p_trans;

    aw_err_t  ret;
    uint8_t   sendbuf[1];
    uint8_t   sendzero[2];
    uint16_t  *recvbuf;
    int       speed;

    aw_spi_msg_init(&msg, NULL, NULL);

    memset(sendzero, 0x00, sizeof(sendzero));

    speed = p_info->spi_info.spi_speed;
    p_trans = trans;

    sendbuf[0] = val;
    switch (val) {
        case __TS2046_READ_X:
            recvbuf = &p_recvbuf->x;
            break;
        case __TS2046_READ_Y:
            recvbuf = &p_recvbuf->y;
            break;
        case __TS2046_READ_Z1:
            recvbuf = &p_recvbuf->z1;
            break;
        case __TS2046_READ_Z2:
            recvbuf = &p_recvbuf->z2;
            break;
        default:
            aw_kprintf("please enter the correct parameters\n");
            break;
    }

    /* ����ָ�� */
    aw_spi_mktrans(p_trans, &sendbuf[0], NULL, 1, 0, 0, 0, speed, 0);
    aw_spi_trans_add_tail(&msg, p_trans);
    p_trans++;
    /* ��ȡ���� */
    aw_spi_mktrans(p_trans, sendzero, recvbuf, 2, 0, 0, 0, speed, 0);
    aw_spi_trans_add_tail(&msg, p_trans);

    /* end */
    aw_gpio_set(p_info->spi_info.spi_cs_pin, __TS2046_SPI_CS_ENABLE);
    ret =  aw_spi_sync(&p_dev->sdev, &msg);
    aw_gpio_set(p_info->spi_info.spi_cs_pin, __TS2046_SPI_CS_DISABLE);

    return ret;
}

static aw_err_t __ts2046_read_val (awbl_ts2046_dev_t *p_dev,
                                   awbl_ts2046_val_t *p_recvbuf,
                                   int val, int count)
{
    aw_err_t  ret;
    int i;

    for (i = 0; i < count; i++) {
        ret = __ts2046_read(p_dev, p_recvbuf, val);
        p_recvbuf++;
    }
    return ret;
}

typedef union __big_data {
    uint16_t word;
    struct {
        uint8_t byteh;
        uint8_t bytel;
    };
} __big_data_t;

/* �����Ƿ��� ���Ҳ���xy��  ����Ϊtrue  �ſ�Ϊfalse */
static aw_bool_t __ts2046_measure_xy (awbl_ts2046_dev_t *p_dev)
{
    awbl_ts2046_val_t *p_recvbuf;

    __big_data_t *p_val;

    volatile uint32_t x, z1, z2, res;
    volatile uint16_t val;

    aw_bool_t   is_pressed;
    aw_err_t ret;
    int      i;
    int      sample_cnt = p_dev->p_info->ts_serv_info.sample_cnt;
    int      bufcnt     = sample_cnt * __SAMPLE_AXLE_NUM;

    p_recvbuf = p_dev->p_sample_buf;

    /* ����z1 */
    ret = __ts2046_read_val(p_dev, p_recvbuf, __TS2046_READ_Z1, sample_cnt);
    if (ret != AW_OK) {
        goto relased;
    }
    aw_mdelay(3);

    /* ����z2 */
    ret = __ts2046_read_val(p_dev, p_recvbuf, __TS2046_READ_Z2, sample_cnt);
    if (ret != AW_OK) {
        goto relased;
    }
    aw_mdelay(3);

    /* ����x */
    ret = __ts2046_read_val(p_dev, p_recvbuf, __TS2046_READ_X, sample_cnt);
    if (ret != AW_OK) {
        goto relased;
    }
    aw_mdelay(3);

    /* ����y */
    ret = __ts2046_read_val(p_dev, p_recvbuf, __TS2046_READ_Y, sample_cnt);
    if (ret != AW_OK) {
        goto relased;
    }
    aw_mdelay(1);

    p_val = (__big_data_t *)p_dev->p_sample_buf;

    /* ȡ���� */
    for (i = 0; i < bufcnt; i++) {

        val = (((uint16_t)p_val->byteh << 8) | p_val->bytel) >> 3;
        p_val->word = (val & 0xfff);
        p_val++;
    }

    /* ��ѹ��ֵ�����ж��Ƿ��ں���Χ��*/
    for (i = 0; i < sample_cnt; i++, p_recvbuf++) {

        x = p_recvbuf->x;
        z1 = p_recvbuf->z1;
        z2 = p_recvbuf->z2;

        if (x == __TS2046_MAX_12BIT) {
            goto relased;
        }

        /* ���ü��㹫ʽ3���м���ѹ��ֵ */
        if (z1 || z2) {
            res = z2;
            res -= z1;
            res *= x;
            res *= p_dev->p_info->touch_ohm?
                       p_dev->p_info->touch_ohm : __TS2046_TOUCH_OHM;
            res /= z1;
            res = (res + 2047) >> 12;
        } else {
            goto relased;
        }
        if ((res > __TS2046_PRESSURE_MAX_VAL) ||
            (res < __TS2046_PRESSURE_MIN_VAL)) {
            goto relased;
        }
    }

    p_dev->is_pressed = AW_TRUE;
    is_pressed = AW_TRUE;

    goto exit;

relased :
    p_dev->is_pressed = AW_FALSE;
    is_pressed = AW_FALSE;
    memset(p_dev->p_sample_buf,
           0xFF,
           bufcnt * sizeof(uint16_t));
exit:

    return is_pressed;
}

static int __ts2046_is_touch  (struct awbl_ts_service *p_serv)
{
    awbl_ts2046_dev_t *p_dev = __TS2046_TS_SERV_2_DEV(p_serv);
    int ret;
    ret = aw_gpio_get(p_dev->p_info->detect_pin);

    return !ret;
}

static int __ts2046_xy_get (
        struct awbl_ts_service *p_serv,
        void                   *p_x_sample[],
        void                   *p_y_sample[],
        int                     max_slots,
        int                     count)
{
    awbl_ts2046_dev_t *p_dev = __TS2046_TS_SERV_2_DEV(p_serv);

    aw_bool_t is_pressed;

    int i;

    uint16_t *p_spi_xsample  = (uint16_t *)p_x_sample[0];
    uint16_t *p_spi_ysample  = (uint16_t *)p_y_sample[0];



    if (!__ts2046_is_touch(p_serv)) {
        p_dev->is_debounce = 0;
        p_dev->is_pressed = AW_FALSE;
        is_pressed = AW_FALSE;
        return 0;
    }

    if (p_dev->is_debounce == 0) {
        /* ��һ�θհ���ʱ��ʱ���� */
        aw_mdelay(p_serv->p_service_info->debounce_ms);
        p_dev->is_debounce = 1;
    }
    if (__ts2046_is_touch(p_serv)) {
        is_pressed = __ts2046_measure_xy(p_dev);
        if (is_pressed) {
            is_pressed = AW_TRUE;
        } else {
            is_pressed = AW_FALSE;
            return 0;
        }
        /* TS2046ֻ�е���ģʽ */
        for (i = 0; i < count; i++) {
            p_spi_xsample[i] = p_dev->p_sample_buf[i].x;
            p_spi_ysample[i] = p_dev->p_sample_buf[i].y;
        }
    } else {
        aw_mdelay(p_serv->p_service_info->debounce_ms);
        is_pressed = AW_FALSE;
        p_dev->is_debounce = 0;
    }

    p_dev->is_pressed = is_pressed;

    return is_pressed;
}

static void __ts2046_lock_set (struct awbl_ts_service *p_serv, aw_bool_t lock)
{

}

static void __ts2046_active_x (struct awbl_ts_service *p_serv)
{

}

static void __ts2046_active_y (struct awbl_ts_service *p_serv)
{

}

static void __ts2046_measure_x (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{

}

static void __ts2046_measure_y (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{

}


static void __ts2046_func_cs (int state)
{
}

/** \brief touch screen init */
static int __ts2046_init (struct awbl_dev *p_adev)
{
    awbl_ts2046_dev_t     *p_dev  = __TS2046_ADEV_2_DEV(p_adev);
    awbl_ts2046_devinfo_t *p_info = __TS2046_DEVINFO_GET(p_adev);

    int sample_size;

    aw_gpio_pin_cfg(p_info->spi_info.spi_cs_pin, AW_GPIO_OUTPUT);
    aw_gpio_set(p_info->spi_info.spi_cs_pin, __TS2046_SPI_CS_DISABLE); /* CS */

    aw_spi_mkdev(&p_dev->sdev,
                 p_info->spi_info.spi_busid,       // λ������SPI������
                 __TS2046_SPI_BPW,                 // �ִ�С
                 __TS2046_SPI_MODE,                // SPI ģʽ
                 p_info->spi_info.spi_speed,       // ֧�ֵ�����ٶ�
                 0,                                // Ƭѡ����
                 __ts2046_func_cs);                // ���Զ����Ƭѡ���ƺ���

    if (aw_spi_setup(&p_dev->sdev) != AW_OK) {
        AW_INFOF(("ts2046 init err!\r\n"));
        return -AW_ENODEV;
    }

    if (p_info->pfn_plfm_init != NULL) {
        p_info->pfn_plfm_init();
    }

    sample_size = (sizeof(awbl_ts2046_val_t)) *
            p_dev->p_info->ts_serv_info.sample_cnt;

    p_dev->p_sample_buf = (awbl_ts2046_val_t *)aw_mem_alloc(sample_size);

    if (p_dev->p_sample_buf == NULL) {
        return -AW_ENOSPC;
    }

    memset(p_dev->p_sample_buf, 0x00, sample_size);

    return AW_OK;
}

/** instance initializing stage 1 */
static void __ts2046_inst_init1 (struct awbl_dev *p_adev)
{
    awbl_ts2046_dev_t     *p_dev  = __TS2046_ADEV_2_DEV(p_adev);
    awbl_ts2046_devinfo_t *p_info = __TS2046_DEVINFO_GET(p_adev);

    p_dev->p_info = p_info;

    /* ��ʼ��TS���� */
    awbl_ts_service_init(&p_dev->ts_serv, &p_dev->p_info->ts_serv_info);

    /* ������Ҫ�õ������� */
    p_dev->ts_serv.drv_fun.pfn_xy_get    = __ts2046_xy_get;
    p_dev->ts_serv.drv_fun.pfn_lock_set  = __ts2046_lock_set;
    p_dev->ts_serv.drv_fun.pfn_active_x  = __ts2046_active_x;
    p_dev->ts_serv.drv_fun.pfn_active_y  = __ts2046_active_y;
    p_dev->ts_serv.drv_fun.pfn_measure_x = __ts2046_measure_x;
    p_dev->ts_serv.drv_fun.pfn_measure_y = __ts2046_measure_y;
    p_dev->ts_serv.drv_fun.pfn_is_touch  = __ts2046_is_touch;

    /* �õ����ǲ���ֵ����ҪУ׼  */
    p_dev->ts_serv.ts_cal_att = 1;

    /* ע��TS���� */
    (void)awbl_ts_service_register(&p_dev->ts_serv);
}

/* instance initializing stage 2 */
static void __ts2046_inst_init2 (struct awbl_dev *p_dev)
{
}

/* instance initializing stage 3 */
static void __ts2046_inst_connect (struct awbl_dev *p_dev)
{
    __ts2046_init(p_dev);
}


/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_ts2046 = {
    __ts2046_inst_init1,      /* pfunc_dev_init1 */
    __ts2046_inst_init2,      /* pfunc_dev_init2 */
    __ts2046_inst_connect     /* pfunc_dev_connect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_ts2046 = {

    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_SPI,                 /* bus_id */
    AWBL_TS2046_NAME,               /* p_drvname */
    &__g_awbl_drvfuncs_ts2046,      /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/** \brief register touch screen driver */
void awbl_touch_screen_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_ts2046);
}

/* end of file*/
