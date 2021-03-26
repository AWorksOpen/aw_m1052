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

#include "aworks.h"
#include "aw_delay.h"
#include "aw_ts.h"
#include "aw_input.h"
#include "aw_vdebug.h"
#include "aw_ts_lib.h"
#include "driver/input/touchscreen/awbl_gt911_ts.h"
#include <string.h>


#define  AWBL_DEV_TO_GT911_TS_DEV(p_dev, p_ts_dev) \
    awbl_gt911_ts_dev_t  *p_ts_dev = \
                    (awbl_gt911_ts_dev_t *)(AW_CONTAINER_OF(p_dev, \
                                            struct awbl_gt911_ts_dev, \
                                            super))

#define  TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv) \
    awbl_gt911_ts_dev_t  *p_dev = \
                (awbl_gt911_ts_dev_t *)(AW_CONTAINER_OF(p_serv, \
                                        struct awbl_gt911_ts_dev, \
                                        ts_serv))

#define TO_GT911_TS_DEVINFO(pdev) (awbl_gt911_ts_devinfo_t *)AWBL_DEVINFO_GET(pdev)

/*
 * GT911配置参数表
 * 从地址8047到地址80FE，184个寄存器的赋值
 * 必须保证新的版本号大于等于GT911内部
 * flash原有版本号,才会更新配置.
 */
uint8_t GT911_CFG_TBL[]=                 /*数组内容修改了*/
{
    0x76,0x00,0x04,0x58,0x02,0x05,0x3D,0x00,0x02,0x08,
    0x28,0x08,0x5A,0x46,0x03,0x05,0x00,0x00,0x00,0x00,
    0x00,0x00,0x04,0x04,0x04,0x04,0x03,0x88,0x29,0x0A,
    0x4B,0x4D,0x0C,0x08,0x00,0x00,0x00,0x21,0x02,0x1D,
    0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x46,0x64,0x94,0xD5,0x02,0x07,0x00,0x00,0x04,
    0x83,0x48,0x00,0x77,0x4D,0x00,0x6D,0x53,0x00,0x64,
    0x59,0x00,0x5A,0x60,0x00,0x5A,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,0x10,
    0x12,0x14,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x02,0x04,0x06,0x08,0x0F,0x10,0x12,
    0x16,0x18,0x1C,0x1D,0x1E,0x1F,0x20,0x21,0x22,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x7C,0x01
};


/* 五个触摸点触摸坐标起始地址*/
const uint16_t GT911_TPX_TBL[5]={GT911_TP1_REG,GT911_TP2_REG,GT911_TP3_REG,GT911_TP4_REG,GT911_TP5_REG};

/* gt911 i2c写寄存器数据*/
static int __gt911_ts_reg_wr(awbl_gt911_ts_dev_t *p_dev, 
                             uint16_t             addr, 
                             uint8_t             *buf,
                             uint32_t             length)
{
    int err = 0;
    if (p_dev == NULL || buf == NULL) {
        return -AW_EINVAL;
    }
    else {
        err = aw_i2c_write(&p_dev->i2c_gt911, addr, buf, length);
    }

    if (err != AW_OK) {
        AW_INFOF(("GT911 i2c write %d!\r\n", err));
    }

    return err;
}

/* gt911 i2c读取寄存器数据*/
static int __gt911_ts_rd_reg(awbl_gt911_ts_dev_t *p_dev, 
                             uint16_t             addr, 
                             uint8_t             *buf, 
                             uint32_t             length)
{

    int err = 0;
    /* 合法性判断*/
    if (p_dev == NULL || buf == NULL) {
        return -AW_EINVAL;
    }
    else {

        err = aw_i2c_read(&p_dev->i2c_gt911, addr, buf, length);

        if (err != AW_OK) {
            AW_INFOF(("GT911 i2c read err %d!\r\n", err));
        }
    }

    return err;
}

/* 0x5D地址选择时序初始化 */
static void __gt911_address1_init (awbl_gt911_ts_devinfo_t *pinfo)
{
    /* 初始化触摸屏的复位引脚和中断引脚 */
    if(pinfo->rst_pin != -1)
    {
            aw_gpio_pin_cfg(pinfo->rst_pin, AW_GPIO_OUTPUT);
    	    aw_gpio_set(pinfo->rst_pin, 0);
    } else {
        AW_INFOF(("GT911  GPIO RST pin Fail !\r\n"));
    }

    if(pinfo->int_pin != -1)
    {
        aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT);
    	aw_gpio_set(pinfo->int_pin, 0);
    } else {
        AW_INFOF(("GT911  GPIO INT pin Fail !\r\n"));
    }

    aw_mdelay(300);
    aw_gpio_set(pinfo->rst_pin, 1);
    aw_mdelay(10);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT);
    aw_mdelay(100);
}

/* 0x14地址选择时序初始化 */
static void __gt911_address2_init (awbl_gt911_ts_devinfo_t *pinfo)
{
    /* 初始化触摸屏的复位引脚和中断引脚*/
    if(pinfo->int_pin != -1)
    {
            aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
    } else {
        AW_INFOF(("GT911  GPIO INT pin Fail !\r\n"));
    }

    if(pinfo->rst_pin != -1)
    {
            aw_gpio_pin_cfg(pinfo->rst_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PUSH_PULL);
    } else {
        AW_INFOF(("GT911  GPIO RST pin Fail !\r\n"));
    }

    aw_gpio_set(pinfo->rst_pin, 0);
    aw_mdelay(10);
    aw_gpio_set(pinfo->rst_pin, 1);
    aw_mdelay(10);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_FLOAT);
    aw_mdelay(100);

    /* 同步中断脚信号*/
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PULL_DOWN);
    aw_mdelay(20);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_FLOAT);
}


/* 复位GT911时序*/
void __gt911_reset(awbl_gt911_ts_devinfo_t *pinfo)
{
    AW_INFOF(("GT911 Reset!\r\n"));

    aw_gpio_pin_cfg(pinfo->rst_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_PUSH_PULL);
    aw_mdelay(20);
    if (pinfo->dev_addr == GT911_ADDRESS1) {
        aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_FLOAT);
    }
    if (pinfo->dev_addr == GT911_ADDRESS2) {
        aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_FLOAT);
    }
    aw_mdelay(2);
    aw_gpio_set(pinfo->rst_pin, 1);
    aw_mdelay(10);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_FLOAT);
    aw_mdelay(1);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_FLOAT);
    aw_mdelay(50);
    aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_FLOAT);
}



/*
 * 发送GT911配置参数
 * mode:0,参数不保存到flash
 *      1,参数保存到flash  
 */
uint8_t GT911_Send_Cfg(awbl_gt911_ts_dev_t *p_dev, uint8_t mode, const uint8_t *config)
{
    uint8_t buf[2];
    uint8_t i=0;
    buf[0]=0;
    buf[1]=mode;               /* 是否写入到GT911 FLASH?  即是否掉电保存*/

    /*计算校验和 */
    for(i=0;i< 184;i++) {

        buf[0]+=config[i];
    }
    buf[0]=(~buf[0])+1;

    /*发送寄存器配置*/
    __gt911_ts_reg_wr(p_dev, GT911_CONFIGVERSION_REG,(uint8_t*)config, 184);

    /*写入校验和,和配置更新标记*/
    __gt911_ts_reg_wr(p_dev, GT911_CHECKSUM_REG,buf,2);

    return 0;
}

/** 获取触摸屏是否按下 */
static int __gt911_ts_is_touch (struct awbl_ts_service *p_serv)
{
    TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv);
    uint8_t sta = 0;
    uint8_t temp;

    __gt911_ts_rd_reg(p_dev, GT911_GSTID_REG, &sta, 1);

#ifdef GT911_DEBUG
    AW_INFOF(("reg %x \r\n",sta));
#endif

    /** 数据准备好了，清0标志位*/
    if((sta >> 7) == 1) {
        temp = 0;
        __gt911_ts_reg_wr(p_dev, GT911_GSTID_REG, &temp, 1);
    }

    /** 触摸屏按下 */
    if ((sta & 0x0f) && ((sta & 0x0f) < 6)) {
        return  1;
    } else {
        return 0;
    }
}

/** 
 * \brief 准备Y轴电压值测量
 *        (1) 接通X的测量电压, 准备对Y测量.
 *        (2) 切断Y的电压,禁用对X的测量.
 */
static void __gt911_ts_active_x(struct awbl_ts_service *p_serv)
{
}
/** 
 * \brief 准备X轴电压值测量
 *        (1) 接通Y的测量电压, 准备对X测量.
 *        (2) 切断X的电压,禁用对Y的测量.
 */
static void __gt911_ts_active_y(struct awbl_ts_service *p_serv)
{
}


/* 测量X轴采样值  */
static void __gt911_ts_measure_x(
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{
    int i, j, ret = 0;
    uint8_t val;
    uint8_t temp;
    uint8_t mt_cnt = 0;
    uint8_t mt_cnt_tmp = 0;
    uint8_t buf[4] = {0};
    uint16_t *p_i2c_xsample  = NULL;

    TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv);

    /* 判断用户要求读的点数是否合法*/
    if(max_slots > 5){
        AW_INFOF(("GT911 error: max slots > 5\r\n"));
        return;
    }

    /* 读取触摸状态*/
    __gt911_ts_rd_reg(p_dev, GT911_GSTID_REG, &val, 1);

    /* 数据准备好了，清0标志位*/
    if((val >> 7) == 1) {
        temp = 0;
        __gt911_ts_reg_wr(p_dev, GT911_GSTID_REG, &temp, 1);
    }

    /* 获取触摸点数*/
    mt_cnt_tmp = val & 0x0f;

#ifdef GT911_GEBUG
    AW_INFOF(("GT911 0x814E register val: %x\r\n",val));
#endif

    /* 只需读取用户请求的触摸点个数即可 */
    if (mt_cnt_tmp > max_slots) {
        mt_cnt_tmp = max_slots;
    }

    /* 触摸点数在合理范围内，则读出坐标*/
    if((mt_cnt_tmp > 0) && (mt_cnt_tmp < 6)) {

        /* 消抖*/
        aw_mdelay(p_serv->p_service_info->debounce_ms);

        for (i = 0; i < mt_cnt_tmp; i++) {

            ret = __gt911_ts_rd_reg(p_dev, GT911_TPX_TBL[i], buf, 4);

            if(ret != AW_OK) {
                return;
            }

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

/* 测量Y轴采样值  */
static void __gt911_ts_measure_y (
        struct awbl_ts_service *p_serv,
        void                   *p_sample[],
        int                     max_slots,
        int                     count)
{
    int i, j, ret = 0;
    uint8_t val;
    uint8_t temp;
    uint8_t mt_cnt = 0;
    uint8_t mt_cnt_tmp = 0;
    uint8_t buf[4] = {0};
    uint16_t *p_i2c_ysample  = NULL;

    TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv);

    /* 判断用户要求读的点数是否合法*/
    if(max_slots > 5){
        AW_INFOF(("GT911 error: max slots > 5\r\n"));
        return;
    }

    /* 读取触摸状态*/
    __gt911_ts_rd_reg(p_dev, GT911_GSTID_REG, &val, 1);

    /* 数据准备好了，清0标志位*/
    if((val >> 7) == 1) {
        temp = 0;
        __gt911_ts_reg_wr(p_dev, GT911_GSTID_REG, &temp, 1);
    }

    /* 获取触摸点数*/
    mt_cnt_tmp = val & 0x0f;

#ifdef GT911_GEBUG
    AW_INFOF(("GT911 0x814E register val: %x\r\n",val));
#endif

    /* 只需读取用户请求的触摸点个数即可 */
    if (mt_cnt_tmp > max_slots) {
        mt_cnt_tmp = max_slots;
    }

    /* 触摸点数在合理范围内，则读出坐标*/
    if((mt_cnt_tmp > 0) && (mt_cnt_tmp < 6)) {

        /* 消抖*/
        aw_mdelay(p_serv->p_service_info->debounce_ms);

        for (i = 0; i < mt_cnt_tmp; i++) {
            ret = __gt911_ts_rd_reg(p_dev, GT911_TPX_TBL[i], buf, 4);

            if(ret != AW_OK) {
                return;
            }

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

aw_local void __gt911_ts_lock(struct awbl_ts_service *p_serv, aw_bool_t lock)
{
    uint8_t val,i;
    uint8_t ret;
    TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv);
    awbl_gt911_ts_devinfo_t *pinfo = TO_GT911_TS_DEVINFO(&p_dev->super);

    if(lock){

        /* 进入睡眠模式：关屏*/
        aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_LOW | AW_GPIO_FLOAT);
        val = 0x05;
        __gt911_ts_reg_wr(p_dev, GT911_COMMAND_REG, &val, 1);
        AW_INFOF(("GT911 Touchscreen enter sleep mode!\r\n"));

        /* 进入睡眠模式至少要58ms才能进行唤醒*/
        aw_mdelay(60);
    }
    else {
        /* 唤醒触摸屏*/
        for(i = 0;i < 10;i++){

            /* 中断线拉高2-5ms唤醒*/
            aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_OUTPUT_INIT_HIGH | AW_GPIO_FLOAT);
            aw_mdelay(5);

            /* 检查I2C通信是否恢复正常*/
            ret = __gt911_ts_rd_reg(p_dev,GT911_CONFIGVERSION_REG, &val, 1);

            if(ret == AW_OK){

                AW_INFOF(("GT911 Touchscreen wake up sleep!\r\n"));
                aw_gpio_set(pinfo->int_pin, 0);
                aw_mdelay(25);
                aw_gpio_pin_cfg(pinfo->int_pin, AW_GPIO_INPUT | AW_GPIO_FLOAT);
                return;
            }

        }

        /* 唤醒失败则重新复位时序*/
        AW_INFOF(("GT911 Touchscreen wake up sleep failed!\r\n"));
        __gt911_reset(pinfo);
    }
}


/* 获取X Y 轴坐标，并且返回按下的点数 */
static int __gt911_ts_xy_get (
        struct awbl_ts_service *p_serv,
        void                   *p_x_sample[],
        void                   *p_y_sample[],
        int                     max_slots,
        int                     count)
{
    int i,j,ret = 0;
    uint8_t val;
    uint8_t temp;
    uint8_t mt_cnt = 0;
    uint8_t mt_cnt_tmp = 0;
    uint8_t buf[4] = {0};
    uint16_t *p_i2c_xsample  = NULL;
    uint16_t *p_i2c_ysample  = NULL;

    TS_SERV_TO_GT911_TS_DEV(p_dev, p_serv);

    /* 判断用户要求读的点数是否合法*/
    if(max_slots > 5){
        AW_INFOF(("GT911 error: max slots > 5\r\n"));
        return -1;
    }
    /* 读取触摸状态*/
    __gt911_ts_rd_reg(p_dev, GT911_GSTID_REG, &val, 1);

    /* 数据准备好了，清0标志位*/
    if((val >> 7) == 1) {
        temp = 0;
        __gt911_ts_reg_wr(p_dev, GT911_GSTID_REG, &temp, 1);
    }

    /* 获取触摸点数*/
    mt_cnt_tmp = val & 0x0f;
#ifdef GT911_GEBUG
    AW_INFOF(("GT911 0x814E register val: %x\r\n",val));
#endif

    /* 只需读取用户请求的触摸点个数即可 */
    if (mt_cnt_tmp > max_slots) {
        mt_cnt_tmp = max_slots;
    }

    /* 触摸点数在合理范围内，则读出坐标*/
	if((mt_cnt_tmp > 0) && (mt_cnt_tmp < 6)){

        /* 消抖*/
        aw_mdelay(p_serv->p_service_info->debounce_ms);

        for (i = 0; i < mt_cnt_tmp; i++) {
            p_i2c_xsample = (uint16_t *)p_x_sample[mt_cnt];
            p_i2c_ysample = (uint16_t *)p_y_sample[mt_cnt];

            for(j = 0;j < count; j++){
                ret = __gt911_ts_rd_reg(p_dev, GT911_TPX_TBL[i], buf, 4);

                if(ret != AW_OK) {
                    return -1;
                }

                p_i2c_xsample[j]=((uint16_t)(buf[1] & 0X0F) << 8) + buf[0];
                p_i2c_ysample[j]=((uint16_t)(buf[3] & 0X0F) << 8) + buf[2];
            }

            mt_cnt++;
        }
    } else {
        return 0;
    }

    return mt_cnt_tmp;
}

/* 触摸屏初始化 */
static void __gt911_ts_init(struct awbl_dev *pdev)
{
    uint8_t chip_type[11] = {0};
    uint8_t reg_value[6] = {0};
    int  err = 0;
#ifdef GT911_DEBUG
    int i;
#endif

    /* 获取gt911设备类型*/
    AWBL_DEV_TO_GT911_TS_DEV(pdev, p_ts_dev);

    /* 获取gt911设备信息*/
    awbl_gt911_ts_devinfo_t *pinfo = TO_GT911_TS_DEVINFO(pdev);

    /* 生成从机设备(gt911)描述结构,7位设备地址,2个字节子地址 */
    aw_i2c_mkdev(&(p_ts_dev->i2c_gt911), 
                  pdev->p_devhcf->bus_index, 
                  pinfo->dev_addr, 
                  AW_I2C_M_7BIT | AW_I2C_SUBADDR_2BYTE);

    if (pinfo->dev_addr == GT911_ADDRESS1) {
        __gt911_address1_init(pinfo);
    } else if (pinfo->dev_addr == GT911_ADDRESS2){
        __gt911_address2_init(pinfo);
    } else {
        AW_INFOF(("GT911 Address Fail ! choose 0x5D or 0x14 !\r\n"));
    }

    /* 软复位 */
    reg_value[0] = 0x02;
    err = __gt911_ts_reg_wr(p_ts_dev, GT911_COMMAND_REG, reg_value, 1);

    /* 读取固件的版本 */
    __gt911_ts_rd_reg(p_ts_dev, GT911_CHIPTYPE_REG, chip_type, 10);
    chip_type[10] = '\0';
     __gt911_ts_rd_reg(p_ts_dev, GT911_ID_VERSION_REG, reg_value, 6);
     if (reg_value[3] == 0x00){
         AW_INFOF(("\r\nChipID：%s,Product ID: %c%c%c,Firmware version: 0x%x%x\r\n",
                 chip_type, reg_value[0], reg_value[1], reg_value[2],reg_value[5], reg_value[4]));
     }
     else{
         AW_INFOF(("\r\nChipID：%s,Product ID: %c%c%c%c,Firmware version: 0x%x%x\r\n",
                 chip_type, reg_value[0], reg_value[1], reg_value[2], reg_value[3],reg_value[5], reg_value[4]));
     }

     /* 检查固件*/
     __gt911_ts_rd_reg(p_ts_dev, 0x41E4, reg_value, 1);
     if(reg_value[0] != 0xBE)
     {
         AW_INFOF(("GT911 Firmware Error！\r\n"));
     }

     /* 读取模组信息*/
     __gt911_ts_rd_reg(p_ts_dev, GT911_VENDORID_REG, reg_value, 1);
     AW_INFOF(("GT911 Vendor ID: 0x%x\r\n",reg_value[0]));

     /* 读取配置文件版本号*/
     __gt911_ts_rd_reg(p_ts_dev, GT911_CONFIGVERSION_REG, reg_value, 1);
     AW_INFOF(("GT911 Config Version: 0x%x\r\n",reg_value[0]));

     /** 配置用户定义的坐标大小*/
     GT911_CFG_TBL[1] = (uint8_t)(pinfo->x_usr_output_max & 0xFF);
     GT911_CFG_TBL[2] = (uint8_t)((pinfo->x_usr_output_max >> 8) & 0xFF);
     GT911_CFG_TBL[3] = (uint8_t)(pinfo->y_usr_output_max & 0xFF);
     GT911_CFG_TBL[4] = (uint8_t)((pinfo->y_usr_output_max >> 8) & 0xFF);

     /* 更新并保存配置 */
     GT911_Send_Cfg(p_ts_dev, CFG_SAVE, GT911_CFG_TBL);

     /* 正常工作模式*/
     reg_value[0] = 0x00;
     err = __gt911_ts_reg_wr(p_ts_dev, GT911_COMMAND_REG, reg_value, 1);
     if (err != AW_OK) {
         AW_INFOF(("GT911 initialize failed!\r\n"));
     }
     else
         AW_INFOF(("GT911 initialize finished!\r\n"));


     aw_mdelay(10);
#ifdef GT911_DEBUG
     /* 读出配置表(用于调试) */
     for(i=0;i<186;i++)
     {
         __gt911_ts_rd_reg(p_ts_dev, GT911_CONFIGVERSION_REG+i, reg_value, 1);
         AW_INFOF(("TEMP[%x]:%x ",(0X8047+i),reg_value[0]));
     }
     AW_INFOF(("\r\n"));
#endif
}

/* 初始化第一阶段 */
static void __gt911_ts_inst_init1 (struct awbl_dev *p_dev)
{
}
/* 初始化第二阶段 */
static void __gt911_ts_inst_init2 (struct awbl_dev *p_dev){
    /* 获取gt911设备类型*/
    AWBL_DEV_TO_GT911_TS_DEV(p_dev, p_ts_dev);
    /* 获取gt911设备信息*/
    awbl_gt911_ts_devinfo_t *pinfo = TO_GT911_TS_DEVINFO(p_dev);
    /* 初始化触摸屏服务*/
    awbl_ts_service_init(&p_ts_dev->ts_serv, &pinfo->ts_serv_info);
    /* 初始化触摸服务函数集*/
    /* 获取X Y轴坐标，并且返回有多少个点按下 */
    p_ts_dev->ts_serv.drv_fun.pfn_xy_get    = __gt911_ts_xy_get;  
    /* 测量Y轴采样值  */
    p_ts_dev->ts_serv.drv_fun.pfn_measure_y = __gt911_ts_measure_y;  
    p_ts_dev->ts_serv.drv_fun.pfn_measure_x = __gt911_ts_measure_x;  /* 测量X轴采样值  */
    p_ts_dev->ts_serv.drv_fun.pfn_lock_set = __gt911_ts_lock;        /* 设置是否锁屏 */
    p_ts_dev->ts_serv.drv_fun.pfn_is_touch = __gt911_ts_is_touch;    /* 是否按下 */
    p_ts_dev->ts_serv.drv_fun.pfn_active_y = __gt911_ts_active_y;    /* 准备测量Y轴坐标  */
    p_ts_dev->ts_serv.drv_fun.pfn_active_x = __gt911_ts_active_x;    /* 准备测量X轴坐标  */
    /* 得到的坐标值，不需要校准  */
    p_ts_dev->ts_serv.ts_cal_att  = 0;
    /* 注册TS设备 */
    awbl_ts_service_register(&p_ts_dev->ts_serv);
}
/** 初始化第三阶段 */
static void __gt911_ts_inst_connect (struct awbl_dev *p_dev)
{
    __gt911_ts_init(p_dev);
}


/** \brief driver functions (must defined as aw_const) */
/* 驱动入口点函数 */
aw_const struct awbl_drvfuncs __g_awbl_drvfuncs_gt911_ts = {
    __gt911_ts_inst_init1,      /* pfunc_dev_init1 */
    __gt911_ts_inst_init2,      /* pfunc_dev_init2 */
    __gt911_ts_inst_connect,    /* pfunc_dev_connect */
};


/** driver registration (must defined as aw_const) */
/* 驱动注册结构体 */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_gt911_ts = {
    AWBL_VER_1,                            /* AWBus-lite版本号 */
    AWBL_BUSID_I2C,                        /* 总线ID */
    AWBL_GT911_TOUCHSCREEN_NAME,           /* 驱动名字 */
    &__g_awbl_drvfuncs_gt911_ts,           /* 驱动入口点 */
    NULL,                                  /* Methods 对象列表 */
    NULL                                   /* 探测函数 */
};

/* 注册触摸屏驱动*/
void awbl_gt911_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_gt911_ts);
}

/* end of file*/
