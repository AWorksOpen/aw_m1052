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
 * \brief ��׼�źŲɼ�ģ�� TPS08U ����
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-03  pea, first implementation
 * \endinternal
 */

/*******************************************************************************
  ͷ�ļ�����
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
  �궨��
*******************************************************************************/

/** \brief ͨ���豸�Ż�ȡ�豸ʵ�� */
#define __ID_GET_TPS08U_DEV(id)               \
            (struct awbl_tps08u_dev *) \
            awbl_dev_find_by_name(AWBL_TPS08U_NAME, id)

/** \brief ͨ���豸ʵ����ȡ�豸��Ϣ */
#define __TPS08U_DEVINFO_GET(p_dev) \
            (struct awbl_tps08u_devinfo *)AWBL_DEVINFO_GET(p_dev)

#define __SPI_ST_IDLE            0    /**< \brief ����״̬ */
#define __SPI_ST_CS_VALID        1    /**< \brief Ƭѡ��Ч */
#define __SPI_ST_CMD             2    /**< \brief ���� CMD */
#define __SPI_ST_DATA            3    /**< \brief �������� */
#define __SPI_ST_CS_INVALID      4    /**< \brief Ƭѡ��Ч */

#define __SPI_EVT_NONE           0    /**< \brief ���¼� */
#define __SPI_EVT_CS_VALID       1    /**< \brief Ƭѡ��Ч */
#define __SPI_EVT_SEND_CMD       2    /**< \brief ���� CMD */
#define __SPI_EVT_SEND_DATA      3    /**< \brief �������� */
#define __SPI_EVT_RECV_DATA      4    /**< \brief �������� */
#define __SPI_EVT_SPI_DONE       5    /**< \brief SPI ������� */

/** \brief ״̬���ڲ�״̬�л� */
#define __SPI_NEXT_STATE(s, e)   \
    do {                         \
        p_this->spi_state = (s); \
        new_event = (e);         \
    } while(0)

/** \brief TPS08U �� ID */
#define __TPS08U_ID    0x38535054

/** \brief ��Ҫд�븴λ�Ĵ�����ֵ */
#define __RESET_MAGIC  0x05fa50af

/** \brief ͨ������ */
#define __CHAN_NUM     8

/** \brief �ο���ѹ */
#define __VREF_MV     (2500 * 2)

/** \brief ת��λ�� */
#define __ADC_BITS     24

/** \brief Ƭѡ��Ч����һ��ʱ���ص�ʱ���� */
#define __T1_DELAY_US        70     

/** \brief ���������֮���ʱ���� */
#define __T2_DELAY_US        70     

/** \brief ���ݴ��������Ƭѡ��Ч��ʱ���� */
#define __T3_DELAY_US        20     

/** \brief ���Ĵ���ʱƬѡ��Ч��Ƭѡ��Ч��ʱ���� */
#define __T4_READ_DELAY_US   40     

/** \brief д�Ĵ���ʱƬѡ��Ч��Ƭѡ��Ч��ʱ���� */
#define __T4_WRITE_DELAY_US  12000  

#define __DATA_ADDR(ch)      (0x00 + (ch))  /**< \brief ���ݼĴ�����ַ */
#define __CHAN_ENABLE_ADDR    0x08          /**< \brief ͨ��ʹ�ܼĴ�����ַ */
#define __CHAN_MODE_ADDR      0x09          /**< \brief ͨ��ģʽ�Ĵ�����ַ */
#define __STATE_ADDR          0x0A          /**< \brief ϵͳ״̬�Ĵ�����ַ */
#define __RESET_ADDR          0x0B          /**< \brief ϵͳ��λ�Ĵ�����ַ */
#define __ID_ADDR             0x0C          /**< \brief ID �Ĵ�����ַ */

/** \brief ��ͨ������תΪ���� */
#define __CHAN_DATA_TO_INT(data) (AW_BITS_GET(data, 0, 17) * 10) + \
                                 (AW_BITS_GET(data, 17, 15) * 1000000)

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief �����׶γ�ʼ�� */
aw_local void __tps08u_inst_connect (struct awbl_dev *p_this);

/** \brief �����������ȡ���� */
aw_local aw_err_t __sensor_server_get (struct awbl_dev *p_dev, void *p_arg);

/**
 * \brief SPI ����״̬��
 */
aw_local aw_err_t __spi_sm_event (struct awbl_tps08u_dev *p_this,
                                  uint32_t                event);

/**
 * \brief SPI ������ɻص�����
 */
aw_local void __spi_callback (void *p_arg);

/**
 * \brief �ӳ���ҵ�ص�����
 */
aw_local void __work_callback (void *p_arg);

/**
 * \brief ��ٵ�Ƭѡ���ƺ���
 */
aw_local void __dummy_cs (int state);

/**
 * \brief ����Ƭѡ״̬
 */
aw_local void __chip_select (aw_spi_device_t *p_dev, int state);

/**
 * \brief ��ȡ�Ĵ���
 */
aw_local aw_err_t __reg_read (struct awbl_dev *p_dev, uint8_t addr, void *p_buf);

/**
 * \brief д�Ĵ���
 */
aw_local aw_err_t __reg_write (struct awbl_dev *p_dev, 
                               uint8_t          addr, 
                               void            *p_buf);

/**
 * \brief ��ָ��ͨ����ȡ��������
 */
aw_local aw_err_t __sensor_data_get (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_buf);

/**
 * \brief ʹ�ܴ�������ĳһͨ��
 */
aw_local aw_err_t __sensor_enable (void            *p_cookie,
                                   const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_result);

/**
 * \brief ���ܴ�����ͨ��
 */
aw_local aw_err_t __sensor_disable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result);

/**
 * \brief ���ô�����ͨ������
 */
aw_local aw_err_t __sensor_attr_set (void                  *p_cookie,
                                     int                    id,
                                     int                    attr,
                                     const aw_sensor_val_t *p_val);

/**
 * \brief ��ȡ������ͨ������
 */
aw_local aw_err_t __sensor_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val);

/**
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
 */
aw_local aw_err_t  __sensor_trigger_cfg (void                   *p_cookie,
                                         int                     id,
                                         uint32_t                flags,
                                         aw_sensor_trigger_cb_t  pfn_cb,
                                         void                   *p_arg);

/**
 * \brief �򿪴���
 */
aw_local aw_err_t __sensor_trigger_on (void *p_cookie, int id);

/**
 * \brief �رմ���
 */
aw_local aw_err_t __sensor_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/

/** \brief ���䴫�����ӿ���Ҫ�ṩ�ķ��� */
aw_local aw_const struct awbl_sensor_servfuncs __g_sensor_funcs = {
    __sensor_data_get,    /* ��ָ��ͨ����ȡ�������� */
    __sensor_enable,      /* ʹ�ܴ�������ĳһͨ�� */
    __sensor_disable,     /* ���ܴ�����ͨ�� */
    __sensor_attr_set,    /* ���ô�����ͨ������ */
    __sensor_attr_get,    /* ��ȡ������ͨ������ */
    __sensor_trigger_cfg, /* ���ô�����һ��ͨ����������һ�������ص����� */
    __sensor_trigger_on,  /* �򿪴��� */
    __sensor_trigger_off  /* �رմ��� */
};

/** \brief �������� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

/** \brief �����ṩ�ķ��� */
aw_local struct awbl_dev_method __g_sensor_methods[] = {
    {AWBL_METHOD_CALL(awbl_sensorserv_get), __sensor_server_get},
    AWBL_METHOD_END
};

/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_tps08u = {
    NULL,                    /* ��һ�׶γ�ʼ�� */
    NULL,                    /* �ڶ��׶γ�ʼ�� */
    __tps08u_inst_connect    /* �����׶γ�ʼ�� */
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_tps08u = {
    AWBL_VER_1,                   /* AWBus �汾�� */
    AWBL_BUSID_SPI,               /* ���� ID */
    AWBL_TPS08U_NAME,      /* ������ */
    &__g_awbl_drvfuncs_tps08u,    /* ������ڵ� */
    &__g_sensor_methods[0],       /* �����ṩ�ķ��� */
    NULL                          /* ����̽�⺯�� */
};

/*******************************************************************************
  ���غ�������
*******************************************************************************/

/** \brief �����׶γ�ʼ�� */
aw_local void __tps08u_inst_connect (struct awbl_dev *p_dev)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);

    if (-1 == p_devinfo->cs_pin) {
        AW_ERRF(("TPS08U: cs pin is -1!\n"));
        return;
    }

    /* ��ʼ��Ƭѡ���� todo imx28x ��δ���� aw_gpio_pin_request() */
#if 0 
    if (aw_gpio_pin_request(AWBL_TPS08U_NAME, &p_devinfo->cs_pin, 1) != AW_OK) {
        AW_ERRF(("TPS08U: cs pin %d request failed!\n", p_devinfo->cs_pin));
        return;
    }
#endif
    aw_gpio_pin_cfg(p_devinfo->cs_pin, AW_GPIO_OUTPUT_INIT_HIGH);

    /* ��ʼ���豸�����ṹ */
    aw_spi_mkdev(&p_this->spi_dev,
                  p_dev->p_devhcf->bus_index,
                  8,                    /* �ִ�СΪ 8-bit */
                  AW_SPI_MODE_1,        /* SPI ģʽ 1 */
                  p_devinfo->sclk_freq, /* SPI �ٶ� */
                  p_devinfo->cs_pin,    /* Ƭѡ���� */
                  __dummy_cs);          /* ���Զ����Ƭѡ���ƺ��� */

    /* ���� SPI �豸 */
    if (aw_spi_setup(&p_this->spi_dev) != AW_OK) {
        AW_ERRF(("TPS08U: spi setup failed!\n"));
        return;
    }

    /* ��ʼ�������������ӳ���ҵ */
    aw_delayed_work_init(&p_this->sampling_work, __work_callback, p_dev);
}

/** \brief �����������ȡ���� */
aw_local aw_err_t __sensor_server_get (struct awbl_dev *p_dev, void *p_arg)
{
    struct awbl_tps08u_dev           *p_this    = (struct awbl_tps08u_dev *)p_dev;
    struct awbl_tps08u_devinfo       *p_devinfo = __TPS08U_DEVINFO_GET(p_dev);
    aw_local awbl_sensor_type_info_t  type_info[__CHAN_NUM / 2]; /* ͨ������*/
    uint32_t                          i;

    /* ��������������Ϣ*/
    aw_local const awbl_sensor_servinfo_t serv_info = {
        __CHAN_NUM,             /* ֧�ֵ�ͨ������ */
        type_info,              /* �����������б� */
        AW_NELEMENTS(type_info) /* �����б���Ŀ */
    };

    struct awbl_sensor_service *p_serv = &p_this->sensor_service;

    /* ����������Ϣ��ʼ��ͨ������ */
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
 * \brief ��ȡָ����ַ�Ĵ����Ĵ�С����λΪ�ֽ�
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
 * \brief SPI ����״̬��
 */
aw_local aw_err_t __spi_sm_event (struct awbl_tps08u_dev *p_this,
                                  uint32_t                event)
{
    uint32_t new_event = __SPI_EVT_NONE;

    while (1) {
        if (new_event != __SPI_EVT_NONE) {     /* ������¼��Ƿ������ڲ� */
            event = new_event;
            new_event = __SPI_EVT_NONE;
        }

        switch (p_this->spi_state) {

        /* ����״̬ */
        case __SPI_ST_IDLE:

            /* ���Ϊ��������¼�������ת��Ƭѡ��Ч״̬ */
            if (__SPI_EVT_CS_VALID == event) {
                __SPI_NEXT_STATE(__SPI_ST_CS_VALID, __SPI_EVT_SEND_CMD);

                p_this->cmd = (0x80 | p_this->reg_addr);

                /* ��һ�����䣬���� CMD */
                aw_spi_mktrans(&p_this->trans[0],  /* ���������ṹ */
                               &p_this->cmd,       /* ���ͻ����� */
                                NULL,              /* ���ջ����� */
                                1,                 /* �������� */
                                0,                 /* ��Ӱ��Ƭѡ */
                                0,                 /* bpw ʹ��Ĭ������ */
                                0,                 /* udelay ����ʱ */
                                0,                 /* speed ʹ��Ĭ��ֵ */
                                0);                /* �� flag */

                /* �ڶ������䣬����д���� */
                aw_spi_mktrans(&p_this->trans[1],
                                p_this->is_read ? NULL : p_this->p_buf,
                                p_this->is_read ? p_this->p_buf : NULL,
                                __reg_size_get(p_this->reg_addr),
                                0,
                                0,
                                0,
                                0,
                                0);

                /* ��ʼ����Ϣ */
                aw_spi_msg_init(&p_this->msg[0], __spi_callback, p_this);
                aw_spi_msg_init(&p_this->msg[1], __spi_callback, p_this);
                aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]);
                aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]);

                /* Ƭѡ��Ч */
                __chip_select(&p_this->spi_dev, 0);
            } else {
                return -AW_EINVAL;
            }
            break;

        /* Ƭѡ��Ч */
        case __SPI_ST_CS_VALID:
            if (__SPI_EVT_SEND_CMD == event) {
                __SPI_NEXT_STATE(__SPI_ST_CMD, __SPI_EVT_NONE);

                aw_udelay(__T1_DELAY_US);

                /* ���� CMD */
                if (aw_spi_async(&p_this->spi_dev, &p_this->msg[0]) != AW_OK) {
                    return -AW_EIO;
                }
            }
            break;

        /* ���� CMD */
        case __SPI_ST_CMD:
            if (__SPI_EVT_SPI_DONE == event) {
                __SPI_NEXT_STATE(__SPI_ST_DATA, __SPI_EVT_NONE);

                aw_udelay(__T2_DELAY_US);

                /* �������� */
                if (aw_spi_async(&p_this->spi_dev, &p_this->msg[1]) != AW_OK) {
                    return -AW_EIO;
                }
            }
            break;

        /* �������� */
        case __SPI_ST_DATA:
            if (__SPI_EVT_SPI_DONE == event) {
                __SPI_NEXT_STATE(__SPI_ST_IDLE, __SPI_EVT_NONE);

                aw_udelay(__T3_DELAY_US);

                /* Ƭѡ��Ч */
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

        /* û�������ڲ����¼������� */
        if (new_event == __SPI_EVT_NONE) {
            break;
        }
    }

    return AW_OK;
}

/**
 * \brief SPI ������ɻص�����
 */
aw_local void __spi_callback (void *p_arg)
{
    struct awbl_tps08u_dev *p_this = (struct awbl_tps08u_dev *)p_arg;

    __spi_sm_event(p_this, __SPI_EVT_SPI_DONE);
}

/**
 * \brief �ӳ���ҵ�ص�����
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

    /* ���������ӳ���ҵ */
    if (0 != p_this->sampling_rate) {
        aw_delayed_work_start(&p_this->sampling_work, 
                              1000 / p_this->sampling_rate);
    }

    /* ֻ�� SPI ״̬Ϊ����ʱ�Ž��в��� */
    if (__SPI_ST_IDLE != p_this->spi_state) {
        return;
    }

    if (0 == p_this->read_state) { /* ��ȡ״̬�Ĵ��� */

        /* �ж� ADC �����Ƿ���� */
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

    if (1 == p_this->read_state) { /* ��ȡͨ�����ݼĴ��� */
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

    if (2 == p_this->read_state) { /* ��仺���� */
        p_this->read_state = 0;

        for (i = 0; i < __CHAN_NUM; i++) {

            /* �ж�ͨ���Ƿ�ʹ���Լ������Ƿ�� */
            if ((!AW_BIT_ISSET(p_this->reg_chan_enable, i)) ||
                (!AW_BIT_ISSET(p_this->trigger, i))) {
                continue;
            }

            /* ����׼���������� */
            if (AW_SENSOR_TRIGGER_DATA_READY & p_this->flags[i]) {

                /* ���ô������ص����� */
                if (NULL != p_this->pfn_cb[i]) {
                    p_this->pfn_cb[i](p_this->p_arg[i],
                                      AW_SENSOR_TRIGGER_DATA_READY);
                }
            }

            /* ���޴��� */
            if (AW_SENSOR_TRIGGER_THRESHOLD & p_this->flags[i]) {

                /* �ж��Ƿ�С�������� */
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

                /* �ж��Ƿ���������� */
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

                /* �ж��Ƿ�ﵽ���޴��� */
                if (p_this->threshold_cnt[i] >= p_this->duration_data[i]) {
                    p_this->threshold_cnt[i] = 0;

                    /* ���ô������ص����� */
                    if (NULL != p_this->pfn_cb[i]) {
                        p_this->pfn_cb[i](p_this->p_arg[i],
                                          AW_SENSOR_TRIGGER_THRESHOLD);
                    }
                }
            }

            /* ������б�ʣ����� */
            if (AW_SENSOR_TRIGGER_SLOPE & p_this->flags[i]) {

                /* �������� */
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

                /* �ж��Ƿ�ﵽ���޴��� */
                if (p_this->slope_cnt[i] >= p_this->duration_slope[i]) {
                    p_this->slope_cnt[i] = 0;

                    /* ���ô������ص����� */
                    if (NULL != p_this->pfn_cb[i]) {
                        p_this->pfn_cb[i](p_this->p_arg[i],
                                          AW_SENSOR_TRIGGER_SLOPE);
                    }
                }

                /* ������ʷͨ������ */
                p_this->reg_data_last[i] = p_this->reg_data[i];
            }
        }
    }
}

/**
 * \brief ��ٵ�Ƭѡ���ƺ���
 */
aw_local void __dummy_cs ( int state)
{
    ; /* VOID */
}

/**
 * \brief ����Ƭѡ״̬
 */
aw_local void __chip_select (aw_spi_device_t *p_dev, int state)
{
    aw_gpio_set(p_dev->cs_pin, state);
}

/**
 * \brief ��ȡ�Ĵ���
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

    /* ��һ�����䣬���� CMD */
    aw_spi_mktrans(&p_this->trans[0],  /* ���������ṹ */
                   &cmd,               /* ���ͻ����� */
                    NULL,              /* ���ջ����� */
                    1,                 /* �������� */
                    0,                 /* ��Ӱ��Ƭѡ */
                    0,                 /* bpw ʹ��Ĭ������ */
                    0,                 /* udelay ����ʱ */
                    0,                 /* speed ʹ��Ĭ��ֵ */
                    0);                /* �� flag */

    /* �ڶ������䣬��ȡ���� */
    aw_spi_mktrans(&p_this->trans[1],     /* ���������ṹ */
                    NULL,                 /* ���ͻ����� */
                    p_buf,                /* ���ջ����� */
                    __reg_size_get(addr), /* �������� */
                    0,                    /* ��Ӱ��Ƭѡ */
                    0,                    /* bpw ʹ��Ĭ������ */
                    0,                    /* udelay ����ʱ */
                    0,                    /* speed ʹ��Ĭ��ֵ */
                    0);                   /* �� flag */

    aw_spi_msg_init(&p_this->msg[0], NULL, NULL);              /* ��ʼ����Ϣ */
    aw_spi_msg_init(&p_this->msg[1], NULL, NULL);              /* ��ʼ����Ϣ */
    
    /* ��ӵ� 1 ������ */
    aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]); 
    
    /* ��ӵ� 2 ������ */
    aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]); 

    /* Ƭѡ��Ч */
    __chip_select(&p_this->spi_dev, 0);
    aw_udelay(__T1_DELAY_US);

    /* ���� CMD */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[0]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T2_DELAY_US);

    /* �������� */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[1]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T3_DELAY_US);

    /* Ƭѡ��Ч */
    __chip_select(&p_this->spi_dev, 1);
    aw_udelay(__T4_READ_DELAY_US);

    return AW_OK;
}

/**
 * \brief д�Ĵ���
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

    /* ��һ�����䣬���� CMD */
    aw_spi_mktrans(&p_this->trans[0],  /* ���������ṹ */
                   &cmd,               /* ���ͻ����� */
                    NULL,              /* ���ջ����� */
                    1,                 /* �������� */
                    0,                 /* ��Ӱ��Ƭѡ */
                    0,                 /* bpw ʹ��Ĭ������ */
                    0,                 /* udelay ����ʱ */
                    0,                 /* speed ʹ��Ĭ��ֵ */
                    0);                /* �� flag */

    /* �ڶ������䣬д���� */
    aw_spi_mktrans(&p_this->trans[1],     /* ���������ṹ */
                    p_buf,                /* ���ͻ����� */
                    NULL,                 /* ���ջ����� */
                    __reg_size_get(addr), /* �������� */
                    0,                    /* ��Ӱ��Ƭѡ */
                    0,                    /* bpw ʹ��Ĭ������ */
                    0,                    /* udelay ����ʱ */
                    0,                    /* speed ʹ��Ĭ��ֵ */
                    0);                   /* �� flag */

    aw_spi_msg_init(&p_this->msg[0], NULL, NULL);              /* ��ʼ����Ϣ */
    aw_spi_msg_init(&p_this->msg[1], NULL, NULL);              /* ��ʼ����Ϣ */
    
    /* ��ӵ� 1 ������ */
    aw_spi_trans_add_tail(&p_this->msg[0], &p_this->trans[0]); 
    
    /* ��ӵ� 2 ������ */
    aw_spi_trans_add_tail(&p_this->msg[1], &p_this->trans[1]); 

    /* Ƭѡ��Ч */
    __chip_select(&p_this->spi_dev, 0);
    aw_udelay(__T1_DELAY_US);

    /* ���� CMD */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[0]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T2_DELAY_US);

    /* �������� */
    err = aw_spi_sync(&p_this->spi_dev, &p_this->msg[1]);
    if (AW_OK != err) {
        return err;
    }
    aw_udelay(__T3_DELAY_US);

    /* Ƭѡ��Ч */
    __chip_select(&p_this->spi_dev, 1);
    aw_udelay(__T4_WRITE_DELAY_US);

    return AW_OK;
}

/**
 * \brief ��ָ��ͨ����ȡ��������
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

        /* ��ȡʵ��ͨ���ţ����жϸ�ͨ���Ƿ����ڵ�ǰ�豸 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* ����豸æ��֤�������Զ�������ֱ�Ӷ�ȡ���ݼ��� */
        if (p_this->is_busy) {
            p_buf[i].val = __CHAN_DATA_TO_INT(p_this->reg_data[chan]);
            if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* ��ѹͨ�� */
                p_buf[i].unit = AW_SENSOR_UNIT_MICRO;
            } else { /* ����ͨ�� */
                p_buf[i].unit = AW_SENSOR_UNIT_NANO;
            }
            continue;
        }

        /* ����豸���У�֤��û���Զ���������ȡ�Ĵ��� */
        err = __reg_read(&p_this->dev, __DATA_ADDR(chan), &reg);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_INVALID(&p_buf[i], err);
            return err;
        }
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* ��ѹͨ�� */
            p_buf[i].unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_buf[i].unit = AW_SENSOR_UNIT_NANO;
        }
        p_buf[i].val  = __CHAN_DATA_TO_INT(reg);
    }

    return AW_OK;
}

/**
 * \brief ʹ�ܴ�������ĳһͨ��
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

        /* ��ȡʵ��ͨ���ţ����жϸ�ͨ���Ƿ����ڵ�ǰ�豸 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* ʹ����Ӧͨ�� */
        err = awbl_tps08u_chan_enable_set(p_this->dev.p_devhcf->unit,
                                                 chan + 1,
                                                 1);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        if (!AW_BIT_ISSET(p_devinfo->chan_mode, chan / 2)) { /* ��ѹͨ�� */

            /* ����ͨ��ģʽ */
            err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                                   chan + 1,
                                                   0);
            if (AW_OK != err) {
                AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
                return err;
            }
        } else { /* ����ͨ�� */

            /* ����ͨ��ģʽ */
            err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                                   chan + 1,
                                                   1);
            if (AW_OK != err) {
                AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
                return err;
            }
        }

        /* ���÷���ֵ */
        AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], AW_OK);
    }

    return AW_OK;
}

/**
 * \brief ���ܴ�����ͨ��
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

        /* ��ȡʵ��ͨ���ţ����жϸ�ͨ���Ƿ����ڵ�ǰ�豸 */
        chan = p_ids[i] - p_devinfo->start_id;
        if (__CHAN_NUM <= chan) {
            continue;
        }

        /* ʧ����Ӧͨ�� */
        err = awbl_tps08u_chan_enable_set(p_this->dev.p_devhcf->unit,
                                                 chan + 1,
                                                 0);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        /* ͨ������Ϊ��ѹͨ�� */
        err = awbl_tps08u_chan_mode_set(p_this->dev.p_devhcf->unit,
                                               chan + 1,
                                               0);
        if (AW_OK != err) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], err);
            return err;
        }

        /* ���÷���ֵ */
        AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], AW_OK);
    }

    return AW_OK;
}

/**
 * \brief ���ô�����ͨ������
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

    /* ��ȡʵ��ͨ���� */
    id = id - p_devinfo->start_id;

    switch (attr) {

    /* ����Ƶ�ʣ�ÿ������Ĵ��� */
    case AW_SENSOR_ATTR_SAMPLING_RATE:

        /* ת����λ */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);

        if (0 == val.val) {
            aw_delayed_work_stop(&p_this->sampling_work);
            p_this->is_busy = AW_FALSE;
        } else {

            /* ��ȡͨ��ʹ������ */
            err = awbl_tps08u_chan_enable_get(p_this->dev.p_devhcf->unit,
                                                     0,
                                                    &p_this->reg_chan_enable);
            if ((AW_OK != err) || (0 == p_this->reg_chan_enable)) {
                return err;
            }

            /* ��ȡͨ��ģʽ���� */
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

    /* ������ֵ */
    case AW_SENSOR_ATTR_FULL_SCALE:
        return -AW_EINVAL;

    /* ƫ��ֵ�����ھ�̬У׼���������ݣ����������� = ʵ�ʲ���ֵ + offset */
    case AW_SENSOR_ATTR_OFFSET:

        /* ת����λ */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* ����ͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->offset[id] = val.val;
        break;

    /* �����ޣ��������޴���ģʽ�����ݵ��ڸ�ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:

        /* ת����λ */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* ����ͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_lower[id] = val.val;
        break;

    /* �����ޣ��������޴���ģʽ�����ݸ��ڸ�ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:

        /* ת����λ */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* ����ͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_upper[id] = val.val;
        break;

    /* �������ޣ������������������ݱ仯���죨б�ʹ��󣩣�������ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_SLOPE:

        /* ת����λ */
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_MICRO);
        } else { /* ����ͨ�� */
            aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_NANO);
        }
        p_this->threshold_slope[id] = val.val;
        break;

    /* �������ݳ����������������޴���ģʽ */
    case AW_SENSOR_ATTR_DURATION_DATA:

        /* ת����λ */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);
        p_this->duration_data[id] = val.val;
        break;

    /* ��������������������������ģʽ */
    case AW_SENSOR_ATTR_DURATION_SLOPE:

        /* ת����λ */
        aw_sensor_val_unit_convert(&val, 1, AW_SENSOR_UNIT_BASE);
        p_this->duration_slope[id] = val.val;
        break;

    default:
        return -AW_ENOTSUP;
    }

    return AW_OK;
}

/**
 * \brief ��ȡ������ͨ������
 */
aw_local aw_err_t __sensor_attr_get (void            *p_cookie,
                                     int              id,
                                     int              attr,
                                     aw_sensor_val_t *p_val)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);

    /* ��ȡʵ��ͨ���� */
    id = id - p_devinfo->start_id;

    switch (attr) {

    /* ����Ƶ�ʣ�ÿ������Ĵ��� */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val = p_this->sampling_rate;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    /* ������ֵ */
    case AW_SENSOR_ATTR_FULL_SCALE:
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            p_val->val = 5000000;
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_val->val = 20000000;
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* ƫ��ֵ�����ھ�̬У׼���������ݣ����������� = ʵ�ʲ���ֵ + offset */
    case AW_SENSOR_ATTR_OFFSET:
        p_val->val = p_this->offset[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* �����ޣ��������޴���ģʽ�����ݵ��ڸ�ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val = p_this->threshold_lower[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* �����ޣ��������޴���ģʽ�����ݸ��ڸ�ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val = p_this->threshold_upper[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* �������ޣ������������������ݱ仯���죨б�ʹ��󣩣�������ֵʱ���� */
    case AW_SENSOR_ATTR_THRESHOLD_SLOPE:
        p_val->val = p_this->threshold_slope[id];
        if (!AW_BIT_ISSET(p_devinfo->chan_mode, id / 2)) { /* ��ѹͨ�� */
            p_val->unit = AW_SENSOR_UNIT_MICRO;
        } else { /* ����ͨ�� */
            p_val->unit = AW_SENSOR_UNIT_NANO;
        }
        break;

    /* �������ݳ����������������޴���ģʽ */
    case AW_SENSOR_ATTR_DURATION_DATA:
        p_val->val = p_this->duration_data[id];
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    /* ��������������������������ģʽ */
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
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
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

    /* ��ȡʵ��ͨ���� */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    p_this->flags[id] = flags;
    p_this->pfn_cb[id] = pfn_cb;
    p_this->p_arg[id] = p_arg;

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/**
 * \brief �򿪴���
 */
aw_local aw_err_t __sensor_trigger_on (void *p_cookie, int id)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         key;

    /* ��ȡʵ��ͨ���� */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    AW_BIT_SET(p_this->trigger, id);

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/**
 * \brief �رմ���
 */
aw_local aw_err_t __sensor_trigger_off (void *p_cookie, int id)
{
    struct awbl_tps08u_dev     *p_this    = (struct awbl_tps08u_dev *)p_cookie;
    struct awbl_tps08u_devinfo *p_devinfo = __TPS08U_DEVINFO_GET(p_cookie);
    int                         key;

    /* ��ȡʵ��ͨ���� */
    id = id - p_devinfo->start_id;

    AW_INT_CPU_LOCK(key);

    AW_BIT_CLR(p_this->trigger, id);

    AW_INT_CPU_UNLOCK(key);

    return AW_OK;
}

/*******************************************************************************
  �ⲿ��������
*******************************************************************************/

/**
 * \brief �� TPS08U ����ע�ᵽ AWBus ��ϵͳ��
 *
 * \note �������н�ֹ���ò���ϵͳ API
 */
void awbl_tps08u_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_tps08u);
}

/**
 * \brief ����� TPS08U ��ͨ���Ƿ�����
 */
aw_err_t awbl_tps08u_detect (int id)
{
    struct awbl_tps08u_dev *p_this = __ID_GET_TPS08U_DEV(id);
    aw_err_t                err;
    uint32_t                tps08u_id;

    if (NULL == p_this) {
        return -AW_ENODEV;
    }

    /* �� ID �Ĵ��� */
    err = __reg_read(&p_this->dev, __ID_ADDR, &tps08u_id);
    if (err != AW_OK) {
        return err;
    }

    /* �ж� ID �Ƿ���ȷ */
    if (tps08u_id != __TPS08U_ID) {
        return -AW_ENODEV;
    }

    return AW_OK;
}

/**
 * \brief ��ȡ TPS08U ʹ�ܵ�ͨ��
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
 * \brief ���� TPS08U ʹ�ܵ�ͨ��
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
 * \brief ��ȡ TPS08U ͨ��ģʽ
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
 * \brief ���� TPS08U ͨ��ģʽ
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
 * \brief ��ȡ TPS08U ͨ������
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
 * \brief ��ȡ TPS08U ״̬
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
 * \brief ��λ TPS08U
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
