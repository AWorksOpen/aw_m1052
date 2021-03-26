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
 * \brief LIS3MDL ����Ŵ���������
 *
 * \internal
 * \par Modification History
 * - 1.00 18-10-25  wan, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aworks.h"
#include "driver/sensor/awbl_lis3mdl.h"
#include "aw_vdebug.h"
#include "aw_isr_defer.h"

/*******************************************************************************
  ���غ궨��
*******************************************************************************/
/** \brief ͨ�� AWBus �豸����ȡ LIS3MDL�豸 */
#define __MY_GET_DEV(p_dev) (awbl_lis3mdl_dev_t*)(p_dev)

/** \brief ͨ�� AWBus �豸����ȡ LIS3MDL�豸��Ϣ */
#define __MY_DEVINFO_GET(p_dev) (awbl_lis3mdl_devinfo_t*)AWBL_DEVINFO_GET(p_dev)

#define __LIS3MDL_WHO_IM_I       (0x0F)   /*< \brief �豸ID��ַ               */
#define __LIS3MDL_MY_ID          (0x3D)   /*< \brief �豸ID                   */
#define __LIS3MDL_CTRL_REG1      (0x20)   /*< \brief ���ƼĴ���1��ַ          */
#define __LIS3MDL_REG1_INIT_VAL  (0x70)   /*< \brief ����Ϊ80HZ��X��Y��UHP    */
#define __LIS3MDL_CTRL_REG2      (0x21)   /*< \brief ���ƼĴ���2��ַ          */
#define __LIS3MDL_CTRL_REG3      (0x22)   /*< \brief ���ƼĴ���3��ַ          */
#define __LIS3MDL_REG3_INIT_VAL  (0x00)   /*< \brief ����Ϊ��������ģʽ       */
#define __LIS3MDL_CTRL_REG4      (0x23)   /*< \brief ���ƼĴ���4��ַ          */
#define __LIS3MDL_REG4_INIT_VAL  (0x0C)   /*< \brief ����Z��UHP               */
#define __LIS3MDL_CTRL_REG5      (0x24)   /*< \brief ���ƼĴ���5��ַ          */
#define __LIS3MDL_STATUS_REG     (0x27)   /*< \brief ״̬�Ĵ�����ַ           */
#define __LIS3MDL_OUT_X_L        (0x28)   /*< \brief X������������ֽڵ�ַ    */
#define __LIS3MDL_OUT_X_H        (0x29)   /*< \brief X������������ֽڵ�ַ    */
#define __LIS3MDL_X_DATA_LEN     (0x2)    /*< \brief X��������ռ���ֽڳ���    */
#define __LIS3MDL_OUT_Y_L        (0x2A)   /*< \brief Y������������ֽڵ�ַ    */
#define __LIS3MDL_OUT_Y_H        (0x2B)   /*< \brief Y������������ֽڵ�ַ    */
#define __LIS3MDL_Y_DATA_LEN     (0x2)    /*< \brief Y��������ռ���ֽڳ���    */
#define __LIS3MDL_OUT_Z_L        (0x2C)   /*< \brief Z������������ֽڵ�ַ    */
#define __LIS3MDL_OUT_Z_H        (0x2D)   /*< \brief Z������������ֽڵ�ַ    */
#define __LIS3MDL_Z_DATA_LEN     (0x2)    /*< \brief Z��������ռ���ֽڳ���    */
#define __LIS3MDL_TEMP_OUT_L     (0x2E)   /*< \brief �¶�����������ֽڵ�ַ   */
#define __LIS3MDL_TEMP_OUT_H     (0X2F)   /*< \brief �¶�����������ֽڵ�ַ   */
#define __LIS3MDL_TEM_DATA_LEN   (0x2)    /*< \brief �¶�������ռ���ֽڳ���   */
#define __LIS3MDL_INT_CFG        (0x30)   /*< \brief �ж����üĴ�����ַ       */
#define __LIS3MDL_INT_SRC        (0x31)   /*< \brief �ж�״̬�Ĵ�����ַ       */
#define __LIS3MDL_INT_THS_L      (0x32)   /*< \brief �ж���ֵ���ݵ��ֽڵ�ַ   */
#define __LIS3MDL_INT_THS_H      (0x33)   /*< \brief �ж���ֵ���ݸ��ֽڵ�ַ   */
#define __LIS3MDL_INT_DATA_LEN   (0X2)    /*< \brief �ж���ֵ���ݳ���         */

#define __LIS3MDL_CONTINUE_OPT   (0x80)   /*< \brief �ӵ�ַ��������MSB����    */
#define __LIS3MDL_OPEN_TEM       (0x80)   /*< \brief ���¶ȴ�����           */
#define __LIS3MDL_DATA_REG_LEN   (0x2)    /*< \brief ������ݵļĴ�������     */
#define __LIS3MDL_RATE_CRL       (0x81)   /*< \brief ��������Ƶ�ʵ�bitλ      */

#define __LIS3MDL_FULL_SCALE_4   (0x00)   /*< \brief ���üĴ���2Ϊ��4��˹      */
#define __LIS3MDL_FULL_SCALE_8   (0x20)   /*< \brief ���üĴ���2Ϊ��8��˹      */
#define __LIS3MDL_FULL_SCALE_12  (0x40)   /*< \brief ���üĴ���2Ϊ��12��˹     */
#define __LIS3MDL_FULL_SCALE_16  (0x60)   /*< \brief ���üĴ���2Ϊ��16��˹     */

#define __LIS3MDL_4_LSB_G        (6842)   /*< \brief ������Ϊ��4��˹ʱ��б��   */
#define __LIS3MDL_8_LSB_G        (3421)   /*< \brief ������Ϊ��8��˹ʱ��б��   */
#define __LIS3MDL_12_LSB_G       (2281)   /*< \brief ������Ϊ��12��˹ʱ��б��  */
#define __LIS3MDL_16_LSB_G       (1711)   /*< \brief ������Ϊ��16��˹ʱ��б��  */

#define __LIS3MDL_INT_CFG_CRL    (0x04)   /*< \brief ����ж����üĴ���       */
#define __LIS3MDL_INT_CFG_X      (0x85)   /*< \brief �ж����üĴ�����X��    */
#define __LIS3MDL_INT_CFG_Y      (0x45)   /*< \brief �ж����üĴ�����Y��    */
#define __LIS3MDL_INT_CFG_Z      (0x25)   /*< \brief �ж����üĴ�����Z��    */

/** \brief ���ݼĴ���16bitֵ����������kʱ��ʵ�ʴŸ�Ӧǿ�Ȳ�����10^6�� */
#define __LIS3MDL_VALUE(k, data) ((int32_t)((1000000*(int64_t)(data))/(k)))

/** \brief ����ʵ�ʴŸ�Ӧǿ��*10^6,����������kʱ�ļĴ���16bitֵ */
#define __LIS3MDL_T_SET(k, data) (((uint16_t)(((int64_t)(data)*(k))/1000000)) \
                                 & (0x7FFF))

/** \brief ����ʵ���¶Ȳ�����10^6�� */
#define __LIS3MDL_TEM_VAL(data)  ((int32_t)((1000000*(int64_t)((data)+200))/8))

/**
 * \brief ����������ѡ��
 */
enum lis3mdl_full_scale {
    LIS3MDL_FULL_SCALE_4  = 4,            /*< \brief ������Ϊ��4��˹           */
    LIS3MDL_FULL_SCALE_8  = 8,            /*< \brief ������Ϊ��8��˹           */
    LIS3MDL_FULL_SCALE_12 = 12,           /*< \brief ������Ϊ��12��˹          */
    LIS3MDL_FULL_SCALE_16 = 16            /*< \brief ������Ϊ��16��˹          */
};

/**
 * \brief ������Ƶ������ѡ��
 */
enum lis3mdl_rate_select {
    LIS3MDL_RATE_0_625  = 0x60,           /*< \brief ����Ƶ��Ϊ 0.625HZ       */
    LIS3MDL_RATE_1_25   = 0x64,           /*< \brief ����Ƶ��Ϊ 1.25HZ        */
    LIS3MDL_RATE_2_5    = 0x68,           /*< \brief ����Ƶ��Ϊ 2.25HZ        */
    LIS3MDL_RATE_5      = 0x6C,           /*< \brief ����Ƶ��Ϊ 5HZ           */
    LIS3MDL_RATE_10     = 0x70,           /*< \brief ����Ƶ��Ϊ 10HZ          */
    LIS3MDL_RATE_20     = 0x74,           /*< \brief ����Ƶ��Ϊ 20HZ          */
    LIS3MDL_RATE_40     = 0x78,           /*< \brief ����Ƶ��Ϊ 40HZ          */
    LIS3MDL_RATE_80     = 0x7C,           /*< \brief ����Ƶ��Ϊ 80HZ          */
    LIS3MDL_RATE_1000   = 0x1E,           /*< \brief ����Ƶ��Ϊ 1000HZ        */
    LIS3MDL_RATE_560    = 0x3E,           /*< \brief ����Ƶ��Ϊ 560HZ         */
    LIS3MDL_RATE_300    = 0x5E,           /*< \brief ����Ƶ��Ϊ 300HZ         */
    LIS3MDL_RATE_155    = 0x7E            /*< \brief ����Ƶ��Ϊ 155HZ         */
};

/**
 * \brief ������Z��ģʽ����
 */
enum lis3mdl_omz_select {
    LIS3MDL_OMZ_LP   = 0x00,              /*< \brief ����ģʽΪ�͹���ģʽ     */
    LIS3MDL_OMZ_MP   = 0x04,              /*< \brief ����ģʽΪ�е�����ģʽ   */
    LIS3MDL_OMZ_HP   = 0x08,              /*< \brief ����ģʽΪ������ģʽ     */
    LIS3MDL_OMZ_UHP  = 0x0C               /*< \brief ����ģʽΪ��������ģʽ   */
};
/*******************************************************************************
  ���غ�������
*******************************************************************************/
/**
 * \brief �����������ȡ����
 */
aw_local aw_err_t __lis3mdl_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);

/*
 * \brief �ڶ��׶γ�ʼ������
 */
aw_local void __lis3mdl_inst_init2 (struct awbl_dev *p_dev);

/*
 * \brief �����׶γ�ʼ������
 */
aw_local void __lis3mdl_inst_connect (struct awbl_dev *p_dev);

/**
 * \brief ��ָ��ͨ����ȡ��������
 */
aw_local aw_err_t __lis3mdl_data_get (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf);

/**
 * \brief ʹ�ܴ�������ĳһͨ��
 */
aw_local aw_err_t __lis3mdl_enable (void                  *p_cookie,
                                    const int             *p_ids,
                                    int                    num,
                                    aw_sensor_val_t       *p_result);

/**
 * \brief ���ܴ�����ͨ��
 */
aw_local aw_err_t __lis3mdl_disable (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_result);

/*
 * \brief ���Ե�����
 */
aw_local aw_err_t __lis3mdl_attr_set (void                  *p_cookie,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val);

/*
 * \brief ��ȡ����
 */
aw_local aw_err_t __lis3mdl_attr_get (void            *p_cookie,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val);

/**
 * \brief ��������
 *
 * \note ֻ֧��һ·���������ٴε��ûḲ��֮ǰ���õ�
 */
aw_local aw_err_t __lis3mdl_trigger_cfg (void                  *p_cookie,
                                         int                    id,
                                         uint32_t               flags,
                                         aw_sensor_trigger_cb_t pfn_cb,
                                         void                  *p_arg);

/*
 * \brief �򿪴���
 */
aw_local aw_err_t __lis3mdl_trigger_on (void *p_cookie, int id);

/*
 * \brief �رմ���
 */
aw_local aw_err_t __lis3mdl_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  local functions
*******************************************************************************/
/*
 * \brief LIS3MDL д����
 */
aw_local aw_err_t __lis3mdl_write (awbl_lis3mdl_dev_t *p_this,
                                   uint32_t            subaddr,
                                   uint8_t            *p_buf,
                                   uint32_t            nbytes)
{
    return aw_i2c_write(&p_this->i2c_dev,
                        subaddr | __LIS3MDL_CONTINUE_OPT,
                        p_buf,
                        nbytes);
}

/*
 * \brief LIS3MDL ������
 */
aw_local aw_err_t __lis3mdl_read (awbl_lis3mdl_dev_t *p_this,
                                  uint32_t            subaddr,
                                  uint8_t            *p_buf,
                                  uint32_t            nbytes)
{
    return aw_i2c_read(&p_this->i2c_dev,
                       subaddr | __LIS3MDL_CONTINUE_OPT,
                       p_buf,
                       nbytes);
}

/*
 * \brief ���ݴ���
 */
aw_local aw_err_t __lis3mdl_data_deal (awbl_lis3mdl_dev_t *p_this,
                                       uint32_t            subaddr,
                                       uint8_t             len,
                                       aw_sensor_val_t    *p_buf)
{
    int16_t data_val = 0;

    uint8_t recv_data[2] = {0};
    aw_err_t ret = AW_OK;

    ret = __lis3mdl_read(p_this, subaddr, &recv_data[0], len);
    if (ret != AW_OK) {
        return ret;
    }

    if (len == 1) {
        data_val = (int16_t)(recv_data[0] << 8);
    } else {
        data_val = (int16_t)((recv_data[1] << 8) | recv_data[0]);
    }

    if (subaddr == __LIS3MDL_TEMP_OUT_L) {
        p_buf->val  = __LIS3MDL_TEM_VAL(data_val);/* �����¶� */
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /* ��λĬ��Ϊ-6:10^(-6)*/
        return AW_OK;
    }

    switch (p_this->full_scale) {

    case LIS3MDL_FULL_SCALE_4:
        p_buf->val  = __LIS3MDL_VALUE(__LIS3MDL_4_LSB_G, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /* ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case LIS3MDL_FULL_SCALE_8:
        p_buf->val  = __LIS3MDL_VALUE(__LIS3MDL_8_LSB_G, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /* ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case LIS3MDL_FULL_SCALE_12:
        p_buf->val  = __LIS3MDL_VALUE(__LIS3MDL_12_LSB_G, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /* ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case LIS3MDL_FULL_SCALE_16:
        p_buf->val  = __LIS3MDL_VALUE(__LIS3MDL_16_LSB_G, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /* ��λĬ��Ϊ-6:10^(-6)*/
        break;
    default:
        break;
    }

    return AW_OK;
}

/*
 * \brief ���ò���Ƶ�ʼĴ���
 */
aw_local aw_err_t __lis3mdl_set_rate_reg (awbl_lis3mdl_dev_t *p_this,
                                          const uint8_t       set_reg1,
                                          const uint8_t       set_reg4)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t ctrl_reg1 = 0;
    uint8_t ctrl_reg4 = 0;

    ret = __lis3mdl_read(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
    if (ret != AW_OK) {
        cur_ret = ret;
    }

    /* ���ñ��ε�Ƶ�� */
    ctrl_reg1 = (ctrl_reg1 & __LIS3MDL_RATE_CRL) | set_reg1;

    ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
    if (ret != AW_OK) {
        cur_ret = ret;
    }

    if (set_reg4 != 0) {

        /* ����Z����X��Y��ģʽͬ�� */
        ctrl_reg4 = set_reg4;

        ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG4, &ctrl_reg4, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    if (cur_ret == AW_OK) {
        switch (set_reg1) {
        case LIS3MDL_RATE_0_625:
            p_this->sampling_rate.val  = 625;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_MILLI;
            break;
        case LIS3MDL_RATE_1_25:
            p_this->sampling_rate.val  = 125;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_CENTI;
            break;
        case LIS3MDL_RATE_2_5:
            p_this->sampling_rate.val  = 25;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_DECI;
            break;
        case LIS3MDL_RATE_5:
            p_this->sampling_rate.val  = 5;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_10:
            p_this->sampling_rate.val  = 10;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_20:
            p_this->sampling_rate.val  = 20;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_40:
            p_this->sampling_rate.val  = 40;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_80:
            p_this->sampling_rate.val  = 80;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_1000:
            p_this->sampling_rate.val  = 1000;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_560:
            p_this->sampling_rate.val  = 560;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_300:
            p_this->sampling_rate.val  = 300;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        case LIS3MDL_RATE_155:
            p_this->sampling_rate.val  = 155;
            p_this->sampling_rate.unit = AW_SENSOR_UNIT_BASE;
            break;
        default:
            break;
        }
    }

    return cur_ret;
}

/*
 * \brief ���ò���Ƶ��
 */
aw_local aw_err_t __lis3mdl_set_rate (awbl_lis3mdl_dev_t *p_this, int rate)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    if (0 <= rate && rate < 1) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_0_625, 0);
    } else if (1 <= rate && rate < 2) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_1_25, 0);
    } else if (2 <= rate && rate < 4) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_2_5, 0);
    } else if (4 <= rate && rate < 7) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_5, 0);
    } else if (7 <= rate && rate < 15) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_10, 0);
    } else if (15 <= rate && rate < 30) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_20, 0);
    } else if (30 <= rate && rate < 60) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_40, 0);
    } else if (60 <= rate && rate < 110) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_80, 0);
    } else if (110 <= rate && rate < 220) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_155, LIS3MDL_OMZ_UHP);
    } else if (220 <= rate && rate < 430) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_300, LIS3MDL_OMZ_HP);
    } else if (430 <= rate && rate < 780) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_560, LIS3MDL_OMZ_MP);
    } else if (780 <= rate && rate < 1100) {
        ret = __lis3mdl_set_rate_reg(p_this, LIS3MDL_RATE_1000, LIS3MDL_OMZ_LP);
    } else {
        ret = -AW_ENOTSUP;
    }

    if (ret != AW_OK) {
        cur_ret = ret;
    }

    return cur_ret;
}

/*
 * \brief �޸�����ֵ�����Ĵ���
 */
aw_local aw_err_t __lis3mdl_set_full_reg (awbl_lis3mdl_dev_t *p_this,
                                          uint8_t             ctrl_reg,
                                          uint8_t             value)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t ctrl_reg2 = 0;

    ctrl_reg2 = ctrl_reg;

    ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG2, &ctrl_reg2, 1);
    if (ret != AW_OK) {
        cur_ret = AW_OK;
    } else {
        p_this->full_scale = value;
    }

    return cur_ret;
}

/*
 * \brief �޸�����ֵ
 */
aw_local aw_err_t __lis3mdl_set_full_scale (awbl_lis3mdl_dev_t *p_this,
                                            int                 value)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    if (0 <= value && value <= 4) {
        ret = __lis3mdl_set_full_reg(p_this,
                                     __LIS3MDL_FULL_SCALE_4,
                                     LIS3MDL_FULL_SCALE_4);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    } else if (4 < value && value <= 8) {
        ret = __lis3mdl_set_full_reg(p_this,
                                     __LIS3MDL_FULL_SCALE_8,
                                     LIS3MDL_FULL_SCALE_8);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    } else if (8 < value && value <= 12) {
        ret = __lis3mdl_set_full_reg(p_this,
                                     __LIS3MDL_FULL_SCALE_12,
                                     LIS3MDL_FULL_SCALE_12);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    } else if (12 < value && value <= 16) {
        ret = __lis3mdl_set_full_reg(p_this,
                                     __LIS3MDL_FULL_SCALE_16,
                                     LIS3MDL_FULL_SCALE_16);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    } else {
        cur_ret = -AW_ENOTSUP;
    }

    return cur_ret;
}

/*
 * \brief ������ֵ
 */
aw_local aw_err_t __lis3mdl_set_threshold (awbl_lis3mdl_dev_t *p_this,
                                           int                 value)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint16_t threshold_data = 0;

    uint8_t set_reg[2] = {0};

    if (value > ((int)p_this->full_scale * 1000000)) {
        return -AW_EINVAL;
    }

    switch (p_this->full_scale) {

    case LIS3MDL_FULL_SCALE_4:
        threshold_data = __LIS3MDL_T_SET(__LIS3MDL_4_LSB_G, value);
        break;
    case LIS3MDL_FULL_SCALE_8:
        threshold_data = __LIS3MDL_T_SET(__LIS3MDL_8_LSB_G, value);
        break;
    case LIS3MDL_FULL_SCALE_12:
        threshold_data = __LIS3MDL_T_SET(__LIS3MDL_12_LSB_G, value);
        break;
    case LIS3MDL_FULL_SCALE_16:
        threshold_data = __LIS3MDL_T_SET(__LIS3MDL_16_LSB_G, value);
        break;
    default:
        break;
    }

    set_reg[0] = threshold_data;
    set_reg[1] = threshold_data >> 8;

    ret = __lis3mdl_write(p_this,
                          __LIS3MDL_INT_THS_L,
                          set_reg,
                          __LIS3MDL_INT_DATA_LEN);
    if (ret != AW_OK) {
        cur_ret = ret;
    } else {
        p_this->threshold_data.val  = value;
        p_this->threshold_data.unit = AW_SENSOR_UNIT_MICRO;
    }

    return cur_ret;
}

/*
 * \brief ����׼�����������ж�
 */
aw_local void __lis3mdl_drdy_alarm_callback (void *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    aw_gpio_trigger_off(p_devinfo->drdy_pin);

    aw_isr_defer_job_add(&p_this->g_myjob[0]); /* ����ж��ӳٴ������� */
}

/*
 * \brief ��ֵ�ж�
 */
aw_local void __lis3mdl_int_alarm_callback (void *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    aw_gpio_trigger_off(p_devinfo->int_pin);

    aw_isr_defer_job_add(&p_this->g_myjob[1]); /* ����ж��ӳٴ������� */

}

/*
 * \brief ����׼�������ӳ��жϴ�����
 */
aw_local void __lis3mdl_drdy_delay_int (void *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    /* ��ȡ���Զ����жϱ�־ */
    __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_X_H, 1, &p_this->current_data[0]);
    __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_Y_H, 1, &p_this->current_data[1]);
    __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_Z_H, 1, &p_this->current_data[2]);

    if (p_this->pfn_trigger_fnc[0] &&
            (p_this->flags[0] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[0](p_this->p_argc[0],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_fnc[1] &&
            (p_this->flags[1] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[1](p_this->p_argc[1],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_fnc[2] &&
            (p_this->flags[2] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[2](p_this->p_argc[2],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    aw_gpio_trigger_on(p_devinfo->drdy_pin);
}


/*
 * \brief ��ֵ�ӳ��жϴ�����
 */
aw_local void __lis3mdl_int_delay_int (void *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    uint8_t int_src = 0;

    __lis3mdl_read(p_this, __LIS3MDL_INT_SRC, &int_src, 1);

    /* X����������� */
    if ((int_src >> 7) == 0x1) {
        if (p_this->pfn_trigger_fnc[0] &&
                (p_this->flags[0] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[0](p_this->p_argc[0],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    /* X����ڸ����� */
    if ((int_src & 0x10) > 0) {
        if (p_this->pfn_trigger_fnc[0] &&
                (p_this->flags[0] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[0](p_this->p_argc[0],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    /* Y����������� */
    if ((int_src & 0x40) > 0) {
        if (p_this->pfn_trigger_fnc[1] &&
                (p_this->flags[1] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[1](p_this->p_argc[1],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    /* Y����ڸ����� */
    if ((int_src & 0x08) > 0) {
        if (p_this->pfn_trigger_fnc[1] &&
                (p_this->flags[1] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[1](p_this->p_argc[1],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    /* Z����������� */
    if ((int_src & 0x20) > 0) {
        if (p_this->pfn_trigger_fnc[2] &&
                (p_this->flags[2] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[2](p_this->p_argc[2],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    /* Z����ڸ����� */
    if ((int_src & 0x04) > 0) {
        if (p_this->pfn_trigger_fnc[2] &&
                (p_this->flags[2] & AW_SENSOR_TRIGGER_THRESHOLD)) {
            p_this->pfn_trigger_fnc[2](p_this->p_argc[2],
                                       AW_SENSOR_TRIGGER_THRESHOLD);
        }
    }

    aw_gpio_trigger_on(p_devinfo->int_pin);
}

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/
/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_lis3mdl_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __lis3mdl_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_lis3dml_drvfuncs = {
        NULL,                     /*< \brief ��һ�׶γ�ʼ��                   */
        __lis3mdl_inst_init2,     /*< \brief �ڶ��׶γ�ʼ��                   */
        __lis3mdl_inst_connect    /*< \brief �����׶γ�ʼ��                   */
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_lis3mdl = {
    AWBL_VER_1,                   /*< \brief AWBus �汾��                     */
    AWBL_BUSID_I2C,               /*< \brief ���� ID                          */
    AWBL_LIS3MDL_NAME,            /*< \brief ������                           */
    &__g_lis3dml_drvfuncs,        /*< \brief ������ڵ�                       */
    &__g_lis3mdl_dev_methods[0],  /*< \brief �����ṩ�ķ���                   */
    NULL                          /*< \brief ����̽�⺯��                     */
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_lis3mdl_servfuncs = {
        __lis3mdl_data_get,
        __lis3mdl_enable,
        __lis3mdl_disable,
        __lis3mdl_attr_set,
        __lis3mdl_attr_get,
        __lis3mdl_trigger_cfg,
        __lis3mdl_trigger_on,
        __lis3mdl_trigger_off
};

/*******************************************************************************
    ���غ�������
*******************************************************************************/
/**
 * \brief �����������ȡ����
 */
aw_local aw_err_t __lis3mdl_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    awbl_sensor_service_t *p_sensor_serv = &p_this->sensor_serv;

    /** \brief �ô�������ͨ������ */
    aw_local aw_const awbl_sensor_type_info_t __g_senter_type[2] = {
            {AW_SENSOR_TYPE_MAGNETIC, 3},
            {AW_SENSOR_TYPE_TEMPERATURE, 1},
    };

    /** \brief ��������������Ϣ */
    aw_local aw_const awbl_sensor_servinfo_t  __g_senser_info = {
            4,
            __g_senter_type,
            AW_NELEMENTS(__g_senter_type)
    };

    p_sensor_serv->p_servinfo  = &__g_senser_info;
    p_sensor_serv->start_id    = p_devinfo->start_id;
    p_sensor_serv->p_servfuncs = &__g_lis3mdl_servfuncs;
    p_sensor_serv->p_cookie    = (awbl_lis3mdl_dev_t*)p_this;
    p_sensor_serv->p_next      = NULL;

    *((awbl_sensor_service_t**)p_arg) = p_sensor_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief �ڶ��γ�ʼ�������������ȡ���� */
aw_local void __lis3mdl_inst_init2 (struct awbl_dev *p_dev)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    /** \brief ����IIC�豸 */
    aw_i2c_mkdev(&p_this->i2c_dev,
                 p_dev->p_devhcf->bus_index,
                 p_devinfo->i2c_addr,
                 (AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE));

    p_this->sampling_rate.val    = 10;       /*< \brief Ĭ��Ϊ10HZ */
    p_this->sampling_rate.unit   = AW_SENSOR_UNIT_BASE;
    p_this->full_scale           = 4;        /*< \brief Ĭ������Ϊ��4��˹ */
    p_this->threshold_data.val   = 0;
    p_this->threshold_data.unit  = AW_SENSOR_UNIT_BASE;

    p_this->pfn_trigger_fnc[0]   = NULL;
    p_this->pfn_trigger_fnc[1]   = NULL;
    p_this->pfn_trigger_fnc[2]   = NULL;
    p_this->p_argc[0]            = NULL;
    p_this->p_argc[1]            = NULL;
    p_this->p_argc[2]            = NULL;
    p_this->flags[0]             = 0;
    p_this->flags[1]             = 0;
    p_this->flags[2]             = 0;

    p_this->current_data[0].val  = 0;
    p_this->current_data[0].unit = 0;
    p_this->current_data[1].val  = 0;
    p_this->current_data[1].unit = 0;
    p_this->current_data[2].val  = 0;
    p_this->current_data[2].unit = 0;

    p_this->trigger              = 0;

    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    aw_isr_defer_job_init(&p_this->g_myjob[0], __lis3mdl_drdy_delay_int, p_this);
    aw_isr_defer_job_init(&p_this->g_myjob[1], __lis3mdl_int_delay_int, p_this);

    if (p_devinfo->drdy_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->drdy_pin, AW_GPIO_TRIGGER_HIGH);
        aw_gpio_trigger_connect(p_devinfo->drdy_pin,
                                __lis3mdl_drdy_alarm_callback,
                                p_this);
    }
    if (p_devinfo->int_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->int_pin, AW_GPIO_TRIGGER_HIGH);
        aw_gpio_trigger_connect(p_devinfo->int_pin,
                                __lis3mdl_int_alarm_callback,
                                p_this);
    }
}

/******************************************************************************/
/** \brief �����׶γ�ʼ������ */
aw_local void __lis3mdl_inst_connect (struct awbl_dev *p_dev)
{
    awbl_lis3mdl_dev_t *p_this = __MY_GET_DEV(p_dev);

    uint8_t lis3mdl_id = 0;
    uint8_t reg_init_val = __LIS3MDL_REG1_INIT_VAL;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    ret = __lis3mdl_read(p_this, __LIS3MDL_WHO_IM_I, &lis3mdl_id, 1);
    if (ret != AW_OK || lis3mdl_id != __LIS3MDL_MY_ID) {
        cur_ret = ret;
    } else {

        /**
         * \brief ����x��y�������ģʽ
         */
        ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG1, &reg_init_val, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /**
         * \brief ����z�������ģʽ
         */
        reg_init_val = __LIS3MDL_REG4_INIT_VAL;
        ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG4, &reg_init_val, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /**
         * \brief ����Ϊ��������ģʽ
         */
        reg_init_val = __LIS3MDL_REG3_INIT_VAL;
        ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG3, &reg_init_val, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /**
         * \brief ����ж�����λ
         */
        reg_init_val = __LIS3MDL_INT_CFG_CRL;
        ret = __lis3mdl_write(p_this, __LIS3MDL_INT_CFG, &reg_init_val, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

    }

    if (cur_ret != AW_OK) {
        aw_kprintf("\r\nSensor LIS3MDL Init is ERROR! \r\n");
    }
}

/******************************************************************************/
/** \brief ��ָ��ͨ����ȡ�������� */
aw_local aw_err_t __lis3mdl_data_get (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;

    uint8_t status_val = 0;

    int cur_id = p_ids[0] - p_devinfo->start_id;
    int i = 0;

    /* ��������׼������������ʽ����ֱ�Ӹ�ֵ */
    if ((AW_BIT_GET(p_this->trigger, 3)) && (cur_id != 3)) {
        p_buf->val = p_this->current_data[cur_id].val;
        p_buf->unit= p_this->current_data[cur_id].unit;
        return AW_OK;
    }

    do {
        ret = __lis3mdl_read(p_this, __LIS3MDL_STATUS_REG, &status_val, 1);
        if (ret != AW_OK) {
            return ret;
        }
    } while (((status_val >> 3) & 0x1) != 0x1);

    for ( i = 0; i < num; i++ ) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        switch (cur_id) {
        case 0:
             ret = __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_X_L, 2, &p_buf[i]);
             if (ret != AW_OK) {
                 return ret;
             }
            break;
        case 1:
            ret = __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_Y_L, 2, &p_buf[i]);
            if (ret != AW_OK) {
                return ret;
            }
            break;
        case 2:
            ret = __lis3mdl_data_deal(p_this, __LIS3MDL_OUT_Z_L, 2, &p_buf[i]);
            if (ret != AW_OK) {
                return ret;
            }
            break;
        case 3:
            ret = __lis3mdl_data_deal(p_this, 
                                      __LIS3MDL_TEMP_OUT_L, 
                                      2, 
                                     &p_buf[i]);
            if (ret != AW_OK) {
                return ret;
            }
            break;
        default:
            break;
        }

        if (cur_id > 3) {
            break;   /*< \brief �����ڸ��豸��ͨ����ֱ���˳� */
        }
    }

    return AW_OK;;
}

/******************************************************************************/
/** \brief ʹ�ܸô�������һ������ͨ�� */
aw_local aw_err_t __lis3mdl_enable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    uint8_t ctrl_reg1 = 0;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 3) {

            ret = __lis3mdl_read(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
            ctrl_reg1 |= __LIS3MDL_OPEN_TEM;
            ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }
        if (cur_id > 3) {
            break;   /*< \brief �����ڸ��豸��ͨ����ֱ���˳� */
        }

        /** \brief ���÷���ֵ */
        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], cur_ret);
        }
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief ���ܴ�����ͨ�� */
aw_local aw_err_t __lis3mdl_disable (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_result)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t ctrl_reg1 = 0;

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id == 3) {

            ret = __lis3mdl_read(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
            ctrl_reg1 &= ~__LIS3MDL_OPEN_TEM;
            ret = __lis3mdl_write(p_this, __LIS3MDL_CTRL_REG1, &ctrl_reg1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

        if (cur_id > 3) {
            break;     /*< \brief ���˴�ͨ�������ڸô����������ж�����ͨ�� */
        }

        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], cur_ret);
        }
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief ���Ե����� */
aw_local aw_err_t __lis3mdl_attr_set (void                  *p_cookie,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    aw_sensor_val_t sensor_val = {0, 0};

    if (id < p_devinfo->start_id || id > (p_devinfo->start_id + 3)) {
        return -AW_ENODEV;
    }
    if (id == p_devinfo->start_id + 3) {
        return -AW_ENOTSUP;
    }

    switch (attr) {

    /** \brief ����Ƶ������ */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        sensor_val.val  = p_val->val;
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_BASE);
        ret = __lis3mdl_set_rate(p_this, sensor_val.val);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
        break;

    /** \brief ������ֵ���� */
    case AW_SENSOR_ATTR_FULL_SCALE:
        sensor_val.val  = p_val->val;
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_BASE);
        ret = __lis3mdl_set_full_scale(p_this, sensor_val.val);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
        break;

    /** \brief ������ֵ����,������ֵ��Ϊ�෴��,����������,X,Y,Z����ֵ��ͬ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        sensor_val.val  = p_val->val;
        if (sensor_val.val < 0) {
            sensor_val.val = -sensor_val.val;
        }
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_MICRO);
        ret = __lis3mdl_set_threshold(p_this, sensor_val.val);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
        break;

    default:
        cur_ret = -AW_ENOTSUP;
        break;
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief ��ȡ���� */
aw_local aw_err_t __lis3mdl_attr_get (void            *p_cookie,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t cur_ret = AW_OK;

    if (id < p_devinfo->start_id || id > (p_devinfo->start_id + 3)) {
        return -AW_ENODEV;
    }
    if (id == p_devinfo->start_id + 3) {
        return -AW_ENOTSUP;
    }

    switch(attr) {

    /** \brief ����Ƶ�ʻ�ȡ */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val = p_this->sampling_rate.val;
        p_val->unit = p_this->sampling_rate.unit;
        break;

    /** \brief ������ֵ��ȡ */
    case AW_SENSOR_ATTR_FULL_SCALE:
        p_val->val = (int)p_this->full_scale;
        p_val->unit = AW_SENSOR_UNIT_BASE;
        break;

    /** \brief ����ֵ��ȡ��X��Y��Z����ֵ��ͬ */
    case AW_SENSOR_ATTR_THRESHOLD_UPPER:
        p_val->val = p_this->threshold_data.val;
        p_val->unit = p_this->threshold_data.unit;
        break;

    /** \brief ����ֵ��ȡ��X��Y��Z����ֵ��ͬ */
    case AW_SENSOR_ATTR_THRESHOLD_LOWER:
        p_val->val = -(p_this->threshold_data.val);
        p_val->unit = p_this->threshold_data.unit;
        break;
    default:
        cur_ret = -AW_ENOTSUP;
        break;
    }

    return cur_ret;
}

/******************************************************************************/
/**
 * \brief ��������
 *
 * \note ֻ֧��һ·���������ٴε��ûḲ��֮ǰ���õ�
 */
aw_local aw_err_t __lis3mdl_trigger_cfg (void                  *p_cookie,
                                         int                    id,
                                         uint32_t               flags,
                                         aw_sensor_trigger_cb_t pfn_cb,
                                         void                  *p_arg)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t cur_ret = AW_OK;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 3) {
        return -AW_ENODEV;
    }

    if (cur_id == 3) {
        return -AW_ENOTSUP;
    }

    switch (flags) {
    case AW_SENSOR_TRIGGER_DATA_READY:
        break;
    case AW_SENSOR_TRIGGER_THRESHOLD:
        break;
    case (AW_SENSOR_TRIGGER_DATA_READY | AW_SENSOR_TRIGGER_THRESHOLD):
        break;
    default:
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_fnc[cur_id] = pfn_cb;
    p_this->p_argc[cur_id]          = p_arg;
    p_this->flags[cur_id]           = flags;

    return cur_ret;
}

/******************************************************************************/
/** \brief �򿪴��� */
aw_local aw_err_t __lis3mdl_trigger_on (void *p_cookie, int id)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t int_cfg_reg = 0;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id >= 3) {
        return -AW_ENOTSUP;
    }

    ret = __lis3mdl_read(p_this, __LIS3MDL_INT_CFG, &int_cfg_reg, 1);
    if (ret != AW_OK) {
        return -AW_ENOTSUP;
    }

    switch (cur_id) {

    case 0:
        int_cfg_reg |= __LIS3MDL_INT_CFG_X;
        break;

    case 1:
        int_cfg_reg |= __LIS3MDL_INT_CFG_Y;
        break;

    case 2:
        int_cfg_reg |= __LIS3MDL_INT_CFG_Z;
        break;

    default:
        break;
    }

    ret = __lis3mdl_write(p_this, __LIS3MDL_INT_CFG, &int_cfg_reg, 1);
    if (ret != AW_OK) {
        return -AW_ENOTSUP;
    }

    if ((p_devinfo->drdy_pin != -1)
            && (p_this->flags[cur_id] & AW_SENSOR_TRIGGER_DATA_READY)) {
        ret = aw_gpio_trigger_on(p_devinfo->drdy_pin);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        if (cur_ret == AW_OK) {
            AW_BIT_SET(p_this->trigger, 3);
        }
    }

    if ((p_devinfo->int_pin != -1)
            && (p_this->flags[cur_id] & AW_SENSOR_TRIGGER_THRESHOLD)) {
        ret = aw_gpio_trigger_on(p_devinfo->int_pin);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    if (cur_ret == AW_OK) {
        AW_BIT_SET(p_this->trigger, cur_id);
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief �رմ��� */
aw_local aw_err_t __lis3mdl_trigger_off (void *p_cookie, int id)
{
    awbl_lis3mdl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lis3mdl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t reg_data = 0;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id >= 3) {
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_fnc[cur_id] = NULL;
    p_this->p_argc[cur_id]          = NULL;
    p_this->flags[cur_id]           = 0;

    AW_BIT_CLR(p_this->trigger, cur_id);

    if ((p_this->trigger && 0x7) == 0) {

        reg_data = __LIS3MDL_INT_CFG_CRL;
        ret = __lis3mdl_write(p_this, __LIS3MDL_INT_CFG, &reg_data, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        if (p_devinfo->drdy_pin != -1) {
            ret = aw_gpio_trigger_off(p_devinfo->drdy_pin);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

        if (p_devinfo->int_pin != -1) {
            ret = aw_gpio_trigger_off(p_devinfo->int_pin);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

    }

    return cur_ret;
}

/*******************************************************************************
  extern functions
*******************************************************************************/
/**
 * \brief �� LIS3MDL ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_lis3mdl_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_lis3mdl);
}


/* end of file */

