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
 * \brief ft5x06 touch screen controller
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-19  nwt, first implementation.
 * \endinternal
 */
#include "driver/input/touchscreen/awbl_ft5x06_ts.h"
#include "aw_adc.h"
#include "aw_delay.h"
#include "aw_ts.h"
#include "aw_input.h"
#include "aw_vdebug.h"
#include "aw_ts_lib.h"
#include <string.h>

/**
 * \name 部分设备子地址寄存器定义
 * @{
 */
#define FT_DEVIDE_MODE          0x00        /**< \brief FT5x06模式控制寄存器.  */
#define FT_REG_NUM_FINGER       0x02        /**< \brief 触摸状态寄存器.        */

#define FT_TP1_REG              0X03        /**< \brief 第一个触摸点数据起始地址. */
#define FT_TP2_REG              0X09        /**< \brief 第二个触摸点数据起始地址. */
#define FT_TP3_REG              0X0F        /**< \brief 第三个触摸点数据起始 地址. */
#define FT_TP4_REG              0X15        /**< \brief 第四个触摸点数据起始地址. */
#define FT_TP5_REG              0X1B        /**< \brief 第五个触摸点数据起始地址. */

#define FT_ID_G_THGROUP         0x80        /**< \brief 触摸有效值设置寄存器.     */
#define FT_ID_G_THPEAK          0x81        /**< \brief 触摸有效值(峰值)设置寄存器.*/
#define FT_ID_G_PERIODACTIVE    0x88        /**< \brief 激活状态周期设置寄存器.   */
#define FT_ID_G_LIB_VERSION     0xA6        /**< \brief 固件版本.  */
#define FT_ID_G_CIPHER          0xA3        /**< \brief 芯片ID(0x36). */
#define FT_ID_G_MODE            0xA4        /**< \brief 中断模式控制寄存器. */
#define FT_ID_G_FT5X06ID        0xA8        /**< \brief CTPM Vendor ID. */

#define FT_ID_G_AUTO_CLB_MODE   0xAh        /**< \brief 自动校准寄存器. */

/** @} */

/**
 * \name 触摸
 * @{
 */
#define TS_PUT_DOWN_EVENT         0x00
#define TS_PUT_UP_EVENT           0x40
#define TS_PUT_CONTACT_EVENT      0x80

/** @} */

#define  AWBL_DEV_TO_FT5X06_TS_DEV(p_dev, p_ts_dev) \
    awbl_ft5x06_ts_dev_t  *p_ts_dev = \
                    (awbl_ft5x06_ts_dev_t *)(AW_CONTAINER_OF(p_dev, \
                                            struct awbl_ft5x06_ts_dev, \
                                            super))

#define  TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv) \
    awbl_ft5x06_ts_dev_t  *p_dev = \
                    (awbl_ft5x06_ts_dev_t *)(AW_CONTAINER_OF(p_serv, \
                                            struct awbl_ft5x06_ts_dev, \
                                            ts_serv))

#define TO_FT5X06_TS_DEVINFO(pdev) (awbl_ft5x06_ts_devinfo_t *)AWBL_DEVINFO_GET(pdev)


static int __ft5x06_ts_reg_wr(awbl_ft5x06_ts_dev_t *p_dev, 
                              uint8_t               addr,
                              uint8_t              *buf,
                              uint32_t              length)
{
    int err = 0;

    if (p_dev == NULL || buf == NULL) {
        return -AW_EINVAL;
    }

   {

        err = aw_i2c_write(&p_dev->i2c_ft5x06,
                           addr,
                           buf,
                           length);
    }

    if (err != AW_OK) {

        AW_INFOF(("ft5x06 i2c write %d!\r\n", err));
    }

    return err;
}

static int __ft5x06_ts_rd_reg(awbl_ft5x06_ts_dev_t *p_dev,
                              uint8_t addr,
                              uint8_t *buf,
                              uint32_t length)
{

    int err = 0;

    if (p_dev == NULL || buf == NULL) {
        return -AW_EINVAL;
    }

    {
        err = aw_i2c_read(&p_dev->i2c_ft5x06,
                          addr,
                          buf,
                          length);

        if (err != AW_OK) {

            AW_INFOF(("ft5x06 i2c read err %d!\r\n", err));
        }
    }

    return err;
}


/* 获取触摸屏是否按下 */
static int __ft5x06_ts_is_touch (struct awbl_ts_service *p_serv)
{
    TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv);
    uint8_t sta = 0;

    __ft5x06_ts_rd_reg(p_dev, FT_REG_NUM_FINGER, &sta, 1);
    /* 触摸屏按下 */
    if (sta & 0x0f) {
        return  1;
    } else {
        return 0;
    }
}

/** \brief 准备Y轴电压值测量
 *        (1) 接通X的测量电压, 准备对Y测量.
 *        (2) 切断Y的电压,禁用对X的测量.
 */
static void __ft5x06_ts_active_x(struct awbl_ts_service *p_serv)
{

}
/** \brief 准备X轴电压值测量
 *        (1) 接通Y的测量电压, 准备对X测量.
 *        (2) 切断X的电压,禁用对Y的测量.
 */
static void __ft5x06_ts_active_y(struct awbl_ts_service *p_serv)
{

}

static void __ft5x06_ts_measure_x(
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{
    TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv);
    awbl_ft5x06_ts_devinfo_t *pinfo = TO_FT5X06_TS_DEVINFO(&p_dev->super);

     int i = 0, j = 0, ret = 0;

     uint8_t sta = 0;

     uint8_t mt_cnt = 0;

     uint8_t mt_cnt_tmp = 0;

     uint8_t buf[4] = {0};

     uint16_t *p_i2c_xsample  = NULL;

     /* 防止抖动点击 */
     if (aw_gpio_get(pinfo->int_pin) == 1) {
        p_dev->is_debounce = 0;
        return ;
     } else {

         if (p_dev->is_debounce == 0) {
             /* 第一次刚按下时延时消抖 */
             aw_mdelay(p_serv->p_service_info->debounce_ms);
             p_dev->is_debounce = 1;
         }

         /* 抖动 */
         if (aw_gpio_get(pinfo->int_pin) == 1) {
             p_dev->is_debounce = 0;
             return ;
         }
     }

     __ft5x06_ts_rd_reg(p_dev, FT_REG_NUM_FINGER, &sta,1);  /* 读取触摸点的状态 */

     mt_cnt_tmp = sta & 0x0f;

     if (mt_cnt_tmp != 0) {

         for (i = 0; i < mt_cnt_tmp; i++) {

             /* 被按下则读取对应触摸点坐标数据 */
             /* 读取XY坐标值 */
             ret = __ft5x06_ts_rd_reg(p_dev, FT_TP1_REG + i * 6, buf, 4); 

             if (ret != AW_OK) {
                 return;
             }

             /* 为contact事件才代表按下 */
             if((buf[0] & 0xC0) == TS_PUT_CONTACT_EVENT) {

                 if (mt_cnt <= max_slots - 1) {

                    p_i2c_xsample = (uint16_t *)p_sample[mt_cnt];
                    mt_cnt++;

                    for (j = 0; j < count; j++) {
                       p_i2c_xsample[j] = ((uint16_t)(buf[0] & 0X0F) << 8) | (uint16_t)buf[1];
                    }
                 }

             }
         }

     }
}

static void __ft5x06_ts_measure_y (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{
    TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv);

    int i = 0, j = 0, ret = 0;

    uint8_t sta = 0;

    uint8_t mt_cnt = 0;

    uint8_t mt_cnt_tmp = 0;

    uint8_t buf[4] = {0};

    uint16_t *p_i2c_ysample  = NULL;

    __ft5x06_ts_rd_reg(p_dev, FT_REG_NUM_FINGER, &sta,1); /* 读取触摸点的状态 */

    mt_cnt_tmp = sta & 0x0f;

    if (mt_cnt_tmp != 0) {

        for (i = 0; i < mt_cnt_tmp; i++) {

            /* 被按下则读取对应触摸点坐标数据 */
            /* 读取XY坐标值 */
            ret = __ft5x06_ts_rd_reg(p_dev, FT_TP1_REG + i * 6, buf, 4); 

            if (ret != AW_OK) {
                return;
            }

            /* 为contact事件才代表按下 */
            if((buf[0] & 0xC0) == TS_PUT_CONTACT_EVENT) {

                if (mt_cnt <= max_slots - 1) {

                   p_i2c_ysample = (uint16_t *)p_sample[mt_cnt];
                   mt_cnt++;

                   for (j = 0; j < count; j++) {
                      p_i2c_ysample[j] = ((uint16_t)(buf[2] & 0X0F) << 8) | (uint16_t)buf[3];
                   }
                }

            }
        }

    }
}

aw_local void __ft5x06_ts_lock(struct awbl_ts_service *p_serv, aw_bool_t lock)
{
    TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv);

    uint8_t reg_value = 0;
    int err = 0;

    if (lock) {
        reg_value = 1;
        err = __ft5x06_ts_reg_wr(p_dev, FT_DEVIDE_MODE, &reg_value, 1);  /* 进入系统设置模式. */

        if (err != AW_OK) {
            AW_INFOF(("ft5x06 lock set err!\r\n"));
        }

    } else {
        err = __ft5x06_ts_reg_wr(p_dev, FT_DEVIDE_MODE, &reg_value, 1);  /* 进入正常操作模式. */

        if (err != AW_OK) {
            AW_INFOF(("ft5x06 unlock set err!\r\n"));
        }

    }
}


/* 获取X Y 轴坐标，并且返回按下的点数 */
static int __ft5x06_ts_xy_get (
        struct awbl_ts_service *p_serv,
        void                   *p_x_sample[],
        void                   *p_y_sample[],
        int                     max_slots,
        int                     count)
{
    TS_SERV_TO_FT5X06_TS_DEV(p_dev, p_serv);

    awbl_ft5x06_ts_devinfo_t *pinfo = TO_FT5X06_TS_DEVINFO(&p_dev->super);


    int i = 0, j = 0, ret = 0;

    uint8_t sta = 0;

    uint8_t mt_cnt = 0;

    uint8_t mt_cnt_tmp = 0;

    uint8_t buf[4] = {0};

    uint16_t *p_i2c_xsample  = NULL;
    uint16_t *p_i2c_ysample  = NULL;

    __ft5x06_ts_rd_reg(p_dev, FT_REG_NUM_FINGER, &sta,1);     /* 读取触摸点的状态 */

    mt_cnt_tmp = sta & 0x0f;

    /* 只需读取用户请求的触摸点个数即可 */
    if (mt_cnt_tmp > max_slots) {
        mt_cnt_tmp = max_slots;
    }

    if (mt_cnt_tmp != 0) {

        for (i = 0; i < mt_cnt_tmp; i++) {

            p_i2c_xsample = (uint16_t *)p_x_sample[mt_cnt];
            p_i2c_ysample = (uint16_t *)p_y_sample[mt_cnt];

            for (j = 0; j < count; j++) {

                /* 被按下则读取对应触摸点坐标数据 */
                ret = __ft5x06_ts_rd_reg(p_dev, FT_TP1_REG + i * 6, buf, 2); /* 读取XY坐标值 */

                if (ret != AW_OK) {
                    return 0;
                }

                /* 被按下则读取对应触摸点坐标数据 */
                ret = __ft5x06_ts_rd_reg(p_dev, FT_TP1_REG + 2 + i * 6, &buf[2], 2); /* 读取XY坐标值 */

                if (ret != AW_OK) {
                    return 0;
                }

                /* 为contact事件才代表按下 */
                if (((buf[0] & 0xC0) == TS_PUT_CONTACT_EVENT)) {
                    p_i2c_xsample[j] = ((uint16_t)(buf[0] & 0X0F) << 8) | (uint16_t)buf[1];
                    p_i2c_ysample[j] = ((uint16_t)(buf[2] & 0X0F) << 8) | (uint16_t)buf[3];

                } else {
                    /* 中涂若发生判断TS_PUT_CONTACT_EVENT无效，退出  */
                    break;
                }
            }

            /* 每次都能准确判断TS_PUT_CONTACT_EVENT事件才认为触摸是有效的 */
            if (j == count) {
                mt_cnt++;
            }
        }
    }

    return mt_cnt;
}

/** \brief touch screen init */
static void __ft5x06_ts_init(struct awbl_dev *pdev)
{

    uint8_t reg_value[2] = {0};

    int  err = 0;

    AWBL_DEV_TO_FT5X06_TS_DEV(pdev, p_ts_dev);
    awbl_ft5x06_ts_devinfo_t *pinfo = TO_FT5X06_TS_DEVINFO(pdev);

    int i2c_touch_gpios[] = {0, 0};

    i2c_touch_gpios[0] = pinfo->int_pin;
    if (pinfo->rst_pin != -1) {
        i2c_touch_gpios[1] = pinfo->rst_pin;
    }

   if (aw_gpio_pin_request("i2c_touch_gpios", i2c_touch_gpios,
                     AW_NELEMENTS(i2c_touch_gpios)) == AW_OK) {

       aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
       if (pinfo->rst_pin != -1) {
          aw_gpio_pin_cfg(pinfo->rst_pin, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_PULL_UP);
       }
   }

    /* 生成从机设备(ft5x06)描述结构 */
    aw_i2c_mkdev(&(p_ts_dev->i2c_ft5x06),
                 pdev->p_devhcf->bus_index,
                 pinfo->dev_addr,
                 AW_I2C_M_7BIT | AW_I2C_SUBADDR_1BYTE);
    /* 复位 */
    if (pinfo->rst_pin != -1) {
      aw_gpio_set(pinfo->rst_pin, 0);
      aw_mdelay(20);
      aw_gpio_set(pinfo->rst_pin, 1);
      aw_mdelay(50);
    }

   /* 读取固件的版本 */
    __ft5x06_ts_rd_reg(p_ts_dev, FT_ID_G_LIB_VERSION, reg_value, 1);
    AW_INFOF(("ft5x06 Firmware version is 0x%x\r\n", reg_value[0]));

    /* 读取固件的版本 */
     __ft5x06_ts_rd_reg(p_ts_dev, FT_ID_G_CIPHER, reg_value, 1);
     AW_INFOF(("ft5x06 chip id is 0x%x\r\n", reg_value[0]));

    /* 读取报告速率 */
    __ft5x06_ts_rd_reg(p_ts_dev, FT_ID_G_PERIODACTIVE, reg_value, 1);
    AW_INFOF(("ft5x06 report rate is 0x%x\r\n", reg_value[0]));

    /* 触摸有效值 */
    __ft5x06_ts_rd_reg(p_ts_dev, FT_ID_G_THGROUP, reg_value, 1);
    AW_INFOF(("ft5x06 touch threshold is 0x%x\r\n", reg_value[0]));

    reg_value[0]  = 0;
    /* 进入正常操作模式.           */
    err = __ft5x06_ts_reg_wr(p_ts_dev, FT_DEVIDE_MODE, reg_value, 1);  
    /* 为1时，FT5x06使能中断模式.  */    
    err = __ft5x06_ts_reg_wr(p_ts_dev, FT_ID_G_MODE,   reg_value,  1);   

    /* 触摸有效值(为4的倍数, 从0开始算起),越小越灵敏.       */
    reg_value[0] = 0x17;
    err = __ft5x06_ts_reg_wr(p_ts_dev, FT_ID_G_THGROUP, reg_value, 1);

    /* 触摸有效值 */
    __ft5x06_ts_rd_reg(p_ts_dev, FT_ID_G_THGROUP, reg_value, 1);
    AW_INFOF(("ft5x06 touch threshold reload is 0x%x\r\n", reg_value[0]));

    /* 激活周期，不能小于12，最大14  */
    reg_value[0] = 12;
    err =__ft5x06_ts_reg_wr(p_ts_dev, FT_ID_G_PERIODACTIVE, reg_value, 1);

    if (err != AW_OK) {
        AW_INFOF(("ft5x06 init failed\r\n"));
    }

}

/* instance initializing stage 1 */
static void __ft5x06_ts_inst_init1 (struct awbl_dev *p_dev)
{

}
/* instance initializing stage 2 */
static void __ft5x06_ts_inst_init2 (struct awbl_dev *p_dev)
{
    AWBL_DEV_TO_FT5X06_TS_DEV(p_dev, p_ts_dev);
    awbl_ft5x06_ts_devinfo_t *pinfo     = TO_FT5X06_TS_DEVINFO(p_dev);
    awbl_ts_service_init(&p_ts_dev->ts_serv, &pinfo->ts_serv_info);
    p_ts_dev->ts_serv.drv_fun.pfn_xy_get    = __ft5x06_ts_xy_get;
    p_ts_dev->ts_serv.drv_fun.pfn_measure_y = __ft5x06_ts_measure_y;
    p_ts_dev->ts_serv.drv_fun.pfn_measure_x = __ft5x06_ts_measure_x;
    p_ts_dev->ts_serv.drv_fun.pfn_lock_set = __ft5x06_ts_lock;
    p_ts_dev->ts_serv.drv_fun.pfn_is_touch = __ft5x06_ts_is_touch;
    p_ts_dev->ts_serv.drv_fun.pfn_active_y = __ft5x06_ts_active_y;
    p_ts_dev->ts_serv.drv_fun.pfn_active_x = __ft5x06_ts_active_x;

    /* 得到的坐标值，不需要校准  */
    p_ts_dev->ts_serv.ts_cal_att  = 0;

    /* 注册TS设备 */
    awbl_ts_service_register(&p_ts_dev->ts_serv);

}

/** instance initializing stage 1 */
static void __ft5x06_ts_inst_connect (struct awbl_dev *p_dev)
{
    __ft5x06_ts_init(p_dev);
}

/** \brief driver functions (must defined as aw_const) */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_ft5x06_ts = {
    __ft5x06_ts_inst_init1,      /* pfunc_dev_init1 */
    __ft5x06_ts_inst_init2,      /* pfunc_dev_init2 */
    __ft5x06_ts_inst_connect,    /* pfunc_dev_connect */
};

/** driver registration (must defined as aw_const) */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_ft5x06_ts = {
    AWBL_VER_1,                     /* awb_ver */
    AWBL_BUSID_I2C,                 /* bus_id */
    AWBL_FT5X06_TOUCHSCREEN_NAME,   /* p_drvname */
    &__g_awbl_drvfuncs_ft5x06_ts,   /* p_busfuncs */
    NULL,                           /* p_methods */
    NULL                            /* pfunc_drv_probe */
};

/** \brief register touch screen driver */
void awbl_ft5x06_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_ft5x06_ts);
}

/* end of file*/