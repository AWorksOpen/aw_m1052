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
*******************************************************************************/

/**
 * \file
 * \brief LSM6DSL (������ٶȺ����������Ǵ�����) ����
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-3  wan, first implementation
 * \endinternal
 */

#include "aworks.h"
#include "aw_gpio.h"
#include "aworks.h"
#include "driver/sensor/awbl_lsm6dsl.h"
#include "aw_vdebug.h"
#include "aw_isr_defer.h"
#include "aw_types.h"
#include "aw_delay.h"

/*******************************************************************************
  ���غ궨��
*******************************************************************************/
/** \brief ͨ�� AWBus �豸����ȡ LSM6DSL�豸 */
#define __MY_GET_DEV(p_dev) (awbl_lsm6dsl_dev_t*)(p_dev)

/** \brief ͨ�� AWBus �豸����ȡ LSM6DSL�豸��Ϣ */
#define __MY_DEVINFO_GET(p_dev) (awbl_lsm6dsl_devinfo_t*)AWBL_DEVINFO_GET(p_dev)

#define __LSM6DSL_FUNC_CFG_ACCESS  (0x01)   /**< \brief ����Ƕ��ʽ���ܼĴ��� */
#define __LSM6DSL_SYNC_TIME_FRAME  (0x04)   /**< \brief ������ͬ��ʱ��֡�Ĵ���*/
#define __LSM6DSL_SYNC_RES_RATIO   (0x05)   /**< \brief ������ͬ���ֱ��� */
#define __LSM6DSL_FIFO_CTRL1       (0x06)   /**< \brief FIFO���ƼĴ���1 */
#define __LSM6DSL_FIFO_CTRL2       (0x07)   /**< \brief FIFO���ƼĴ���2 */
#define __LSM6DSL_FIFO_CTRL3       (0x08)   /**< \brief FIFO���ƼĴ���3 */
#define __LSM6DSL_FIFO_CTRL4       (0x09)   /**< \brief FIFO���ƼĴ���4 */
#define __LSM6DSL_FIFO_CTRL5       (0x0A)   /**< \brief FIFO���ƼĴ���5 */
#define __LSM6DSL_DRDY_CFG_G       (0x0B)   /**< \brief ����׼���������üĴ���*/
#define __LSM6DSL_INT1_CTRL        (0x0D)   /**< \brief INT1���ƼĴ��� */
#define __LSM6DSL_INT2_CTRL        (0x0E)   /**< \brief INT2���ƼĴ��� */
#define __LSM6DSL_WHO_AM_I         (0x0F)   /**< \brief who am I */
#define __LSM6DSL_CTRL1_XL         (0x10)   /**< \brief ���Լ��ٶȴ��������ƼĴ���1 */
#define __LSM6DSL_CTRL2_G          (0x11)   /**< \brief ���ٶȴ��������ƼĴ���2 */
#define __LSM6DSL_CTRL3_C          (0x12)   /**< \brief ���ƼĴ���3 */
#define __LSM6DSL_CTRL4_C          (0x13)   /**< \brief ���ƼĴ���4 */
#define __LSM6DSL_CTRL5_C          (0x14)   /**< \brief ���ƼĴ���5 */
#define __LSM6DSL_CTRL6_C          (0x15)   /**< \brief ���ٶȴ��������ƼĴ���6 */
#define __LSM6DSL_CTRL7_G          (0x16)   /**< \brief ���ٶȴ��������ƼĴ���7 */
#define __LSM6DSL_CTRL8_XL         (0x17)   /**< \brief ���Լ��ٶȴ��������ƼĴ���8 */
#define __LSM6DSL_CTRL9_XL         (0x18)   /**< \brief ���Լ��ٶȴ��������ƼĴ���9 */
#define __LSM6DSL_CTRL10_C         (0x19)   /**< \brief ���ƼĴ���10 */
#define __LSM6DSL_MASTER_CONFIG    (0x1A)   /**< \brief �������üĴ��� */
#define __LSM6DSL_WAKE_UP_SRC      (0x1B)   /**< \brief �����ж�Դ�Ĵ��� */
#define __LSM6DSL_TAP_SRC          (0x1C)   /**< \brief TAPԴ�Ĵ��� */
#define __LSM6DSL_D6D_SRC          (0x1D)   /**< \brief 6D���ƼĴ��� */
#define __LSM6DSL_STATUS_REG       (0x1E)   /**< \brief ����״̬�Ĵ��� */
#define __LSM6DSL_OUT_TEMP_L       (0x20)   /**< \brief �¶����ݼĴ�����8λ */
#define __LSM6DSL_OUT_TEMP_H       (0x21)   /**< \brief �¶����ݼĴ�����8λ */
#define __LSM6DSL_OUTX_L_G         (0x22)   /**< \brief ���ٶȴ�����X�����ݵ�λ */
#define __LSM6DSL_OUTX_H_G         (0x23)   /**< \brief ���ٶȴ�����X�����ݸ�λ */
#define __LSM6DSL_OUTY_L_G         (0x24)   /**< \brief ���ٶȴ�����Y�����ݵ�λ */
#define __LSM6DSL_OUTY_H_G         (0x25)   /**< \brief ���ٶȴ�����Y�����ݸ�λ */
#define __LSM6DSL_OUTZ_L_G         (0x26)   /**< \brief ���ٶȴ�����Z�����ݵ�λ */
#define __LSM6DSL_OUTZ_H_G         (0x27)   /**< \brief ���ٶȴ�����Z�����ݸ�λ */
#define __LSM6DSL_OUTX_L_XL        (0x28)   /**< \brief ���ٶȴ�����X�����ݵ�λ */
#define __LSM6DSL_OUTX_H_XL        (0x29)   /**< \brief ���ٶȴ�����X�����ݸ�λ */
#define __LSM6DSL_OUTY_L_XL        (0x2A)   /**< \brief ���ٶȴ�����Y�����ݵ�λ */
#define __LSM6DSL_OUTY_H_XL        (0x2B)   /**< \brief ���ٶȴ�����Y�����ݸ�λ */
#define __LSM6DSL_OUTZ_L_XL        (0x2C)   /**< \brief ���ٶȴ�����Z�����ݵ�λ */
#define __LSM6DSL_OUTZ_H_XL        (0x2D)   /**< \brief ���ٶȴ�����Z�����ݸ�λ */
#define __LSM6DSL_SENSORHUB1_REG   (0x2E)   /**< \brief �ⲿ�����������ĵ�1���ֽ� */
#define __LSM6DSL_SENSORHUB2_REG   (0x2F)   /**< \brief �ⲿ�����������ĵ�2���ֽ� */
#define __LSM6DSL_SENSORHUB3_REG   (0x30)   /**< \brief �ⲿ�����������ĵ�3���ֽ� */
#define __LSM6DSL_SENSORHUB4_REG   (0x31)   /**< \brief �ⲿ�����������ĵ�4���ֽ� */
#define __LSM6DSL_SENSORHUB5_REG   (0x32)   /**< \brief �ⲿ�����������ĵ�5���ֽ� */
#define __LSM6DSL_SENSORHUB6_REG   (0x33)   /**< \brief �ⲿ�����������ĵ�6���ֽ� */
#define __LSM6DSL_SENSORHUB7_REG   (0x34)   /**< \brief �ⲿ�����������ĵ�7���ֽ� */
#define __LSM6DSL_SENSORHUB8_REG   (0x35)   /**< \brief �ⲿ�����������ĵ�8���ֽ� */
#define __LSM6DSL_SENSORHUB9_REG   (0x36)   /**< \brief �ⲿ�����������ĵ�9���ֽ� */
#define __LSM6DSL_SENSORHUB10_REG  (0x37)   /**< \brief �ⲿ�����������ĵ�10���ֽ� */
#define __LSM6DSL_SENSORHUB11_REG  (0x38)   /**< \brief �ⲿ�����������ĵ�11���ֽ� */
#define __LSM6DSL_SENSORHUB12_REG  (0x39)   /**< \brief �ⲿ�����������ĵ�12���ֽ� */
#define __LSM6DSL_FIFO_STATUS1     (0x3A)   /**< \brief FIFO״̬�Ĵ���1 */
#define __LSM6DSL_FIFO_STATUS2     (0x3B)   /**< \brief FIFO״̬�Ĵ���2 */
#define __LSM6DSL_FIFO_STATUS3     (0x3C)   /**< \brief FIFO״̬�Ĵ���3 */
#define __LSM6DSL_FIFO_STATUS4     (0x3D)   /**< \brief FIFO״̬�Ĵ���4 */
#define __LSM6DSL_FIFO_DATA_OUT_L  (0x3E)   /**< \brief FIFO���ݵ�8λ */
#define __LSM6DSL_FIFO_DATA_OUT_H  (0x3F)   /**< \brief FIFO���ݸ�8λ */
#define __LSM6DSL_TIMESTAMP0_REG   (0x40)   /**< \brief ʱ�����һ�ֽ���������Ĵ��� */
#define __LSM6DSL_TIMESTAMP1_REG   (0x41)   /**< \brief ʱ����ڶ��ֽ���������Ĵ��� */
#define __LSM6DSL_TIMESTAMP2_REG   (0x42)   /**< \brief ʱ��������ֽ���������Ĵ��� */
#define __LSM6DSL_STEP_TIMESTAMP_L (0x49)   /**< \brief ��������ʱ�����Ϣ�Ĵ�����8λ */
#define __LSM6DSL_STEP_TIMESTAMP_H (0x4A)   /**< \brief ��������ʱ�����Ϣ�Ĵ�����8λ */
#define __LSM6DSL_STEP_COUNTER_L   (0x4B)   /**< \brief ������������Ĵ�����8λ */
#define __LSM6DSL_STEP_COUNTER_H   (0x4C)   /**< \brief ������������Ĵ�����8λ */
#define __LSM6DSL_SENSORHUB13_REG  (0x4D)   /**< \brief �ⲿ�����������ĵ�13���ֽ� */
#define __LSM6DSL_SENSORHUB14_REG  (0x4E)   /**< \brief �ⲿ�����������ĵ�14���ֽ� */
#define __LSM6DSL_SENSORHUB15_REG  (0x4F)   /**< \brief �ⲿ�����������ĵ�15���ֽ� */
#define __LSM6DSL_SENSORHUB16_REG  (0x50)   /**< \brief �ⲿ�����������ĵ�16���ֽ� */
#define __LSM6DSL_SENSORHUB17_REG  (0x51)   /**< \brief �ⲿ�����������ĵ�17���ֽ� */
#define __LSM6DSL_SENSORHUB18_REG  (0x52)   /**< \brief �ⲿ�����������ĵ�18���ֽ� */
#define __LSM6DSL_FUNC_SRC1        (0x53)   /**< \brief 5���ж�Դ�Ĵ��� */
#define __LSM6DSL_FUNC_SRC2        (0x54)   /**< \brief ������б�ж�Դ�Ĵ��� */
#define __LSM6DSL_WRIST_TILT_IA    (0x55)   /**< \brief ������б�ж�Դ�Ĵ��� */
#define __LSM6DSL_TAP_CFG          (0x58)   /**< \brief �����жϣ�tap�ȹ��� */
#define __LSM6DSL_TAP_THS_6D       (0x59)   /**< \brief λ�ú�tap������ֵ�Ĵ��� */
#define __LSM6DSL_INT_DUR2         (0x5A)   /**< \brief tapʶ�������üĴ��� */
#define __LSM6DSL_WAKE_UP_THS      (0x5B)   /**< \brief ��˫tap�������޼Ĵ��� */
#define __LSM6DSL_WAKE_UP_DUR      (0x5C)   /**< \brief �������䣬���ѣ�ʱ�����˯��ģʽ���ܳ���ʱ�����üĴ��� */
#define __LSM6DSL_FREE_FALL        (0x5D)   /**< \brief �������幦�ܳ���ʱ�����üĴ��� */
#define __LSM6DSL_MD1_CFG          (0x5E)   /**< \brief INT1�ϵĺ���·�� */
#define __LSM6DSL_MD2_CFG          (0x5F)   /**< \brief INT2�ϵĺ���·�� */
#define __LSM6DSL_MASTER_CMD_CODE  (0x60)   /**< \brief ���ڴ�����ͬ����ѹ����������� */
#define __LSM6DSL_SPI_ERROR_CODE   (0x61)   /**< \brief ���ڴ�����ͬ���Ĵ������ */
#define __LSM6DSL_OUT_MAG_RAW_X_L  (0x66)   /**< \brief X���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_OUT_MAG_RAW_X_H  (0x67)   /**< \brief X���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_OUT_MAG_RAW_Y_L  (0x68)   /**< \brief Y���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_OUT_MAG_RAW_Y_H  (0x69)   /**< \brief Y���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_OUT_MAG_RAW_Z_L  (0x6A)   /**< \brief Z���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_OUT_MAG_RAW_Z_H  (0x6B)   /**< \brief Z���ⲿ��ǿ��ԭʼ���ݼĴ�����8λ */
#define __LSM6DSL_X_OFS_USR        (0x73)   /**< \brief ���ٶȼ�X���û�ƫ��У�� */
#define __LSM6DSL_Y_OFS_USR        (0x74)   /**< \brief ���ٶȼ�Y���û�ƫ��У�� */
#define __LSM6DSL_Z_OFS_USR        (0x75)   /**< \brief ���ٶȼ�Z���û�ƫ��У�� */

#define __LSM6DSL_MY_ID            (0x6A)   /**< \brief MY ID */

/** \brief ��USR_OFF_W������Ϊ2^(-10)ʱ���ܱ�ʾ����󲹳�ֵ */
#define __LSM6DSL_USR_OFF_W_10     ((int)((127*1*1000000)/(0x1 << 10)))

/** \brief ��USR_OFF_W������Ϊ2^(-6)ʱ���ܱ�ʾ����󲹳�ֵ */
#define __LSM6DSL_USR_OFF_W_6      ((int)((127*1*1000000)/(0x1 << 6)))

/** \brief ���ݼĴ���16bitֵ����������2g��k��ʱʵ�ʼ��ٶ�ֵ������10^6�� */
#define __LSM6DSL_XL_VALUE(k, data) ((int32_t)(((int64_t)(data)*1000000*(1*(k)))/(16396)))

/** \brief ���ݼĴ���16bitֵ����������250dps��k��ʱʵ��������ֵ������10^6�� */
#define __LSM6DSL_G_VALUE(k, data) ((int32_t)(((int64_t)(data)*1000000*(100*(k)))/(11428)))

/** \brief ����ʵ���¶Ȳ�����10^6�� */
#define __LSM6DSL_TEM_VAL(data)  ((int32_t)((1000000*(int64_t)((data)+6400))/256))

/**
 * \brief ���ٶȴ���������ѡ��
 */
enum lsm6dsl_xl_full_scale {
    LSM6DSL_FULL_SCALE_2  = 0x00,           /**< \brief ������Ϊ��2g           */
    LSM6DSL_FULL_SCALE_4  = 0x08,           /**< \brief ������Ϊ��4g           */
    LSM6DSL_FULL_SCALE_8  = 0x0c,           /**< \brief ������Ϊ��8g           */
    LSM6DSL_FULL_SCALE_16 = 0x04            /**< \brief ������Ϊ��16g          */
};

/**
 * \brief �����Ǵ���������ѡ��
 */
enum lsm6dsl_g_full_scale {
    LSM6DSL_FULL_SCALE_250  = 0x00,         /**< \brief ������Ϊ��250dps       */
    LSM6DSL_FULL_SCALE_500  = 0x04,         /**< \brief ������Ϊ��500dps       */
    LSM6DSL_FULL_SCALE_1000 = 0x08,         /**< \brief ������Ϊ��1000dps      */
    LSM6DSL_FULL_SCALE_2000 = 0x0c          /**< \brief ������Ϊ��2000dps      */
};

/**
 * \brief ������Ƶ������ѡ��
 */
typedef enum lsm6dsl_rate_select {
    LSM6DSL_RATE_1_6     = 0xB0,            /**< \brief ����Ƶ��Ϊ 1.6HZ      */
    LSM6DSL_RATE_12_5    = 0x10,            /**< \brief ����Ƶ��Ϊ 12.5HZ     */
    LSM6DSL_RATE_26      = 0x20,            /**< \brief ����Ƶ��Ϊ 26HZ       */
    LSM6DSL_RATE_52      = 0x30,            /**< \brief ����Ƶ��Ϊ 52HZ       */
    LSM6DSL_RATE_104     = 0x40,            /**< \brief ����Ƶ��Ϊ 104HZ      */
    LSM6DSL_RATE_208     = 0x50,            /**< \brief ����Ƶ��Ϊ 208HZ      */
    LSM6DSL_RATE_416     = 0x60,            /**< \brief ����Ƶ��Ϊ 416HZ      */
    LSM6DSL_RATE_833     = 0x70,            /**< \brief ����Ƶ��Ϊ 833HZ      */
    LSM6DSL_RATE_1660    = 0x80,            /**< \brief ����Ƶ��Ϊ 1600HZ     */
    LSM6DSL_RATE_3330    = 0x90,            /**< \brief ����Ƶ��Ϊ 3330HZ     */
    LSM6DSL_RATE_6660    = 0xA0,            /**< \brief ����Ƶ��Ϊ 6660HZ     */
} lsm6dsl_rate_select_t;

/*******************************************************************************
  ���غ�������
*******************************************************************************/
/**
 * \brief �����������ȡ����
 */
aw_local aw_err_t __lsm6dsl_sensorserv_get (struct awbl_dev *p_dev, void *p_arg);

/*
 * \brief �ڶ��׶γ�ʼ������
 */
aw_local void __lsm6dsl_inst_init2 (struct awbl_dev *p_dev);

/*
 * \brief �����׶γ�ʼ������
 */
aw_local void __lsm6dsl_inst_connect (struct awbl_dev *p_dev);

/**
 * \brief ��ָ��ͨ����ȡ��������
 */
aw_local aw_err_t __lsm6dsl_data_get (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf);

/**
 * \brief ʹ�ܴ�������ĳһͨ��
 */
aw_local aw_err_t __lsm6dsl_enable (void                  *p_cookie,
                                    const int             *p_ids,
                                    int                    num,
                                    aw_sensor_val_t       *p_result);

/**
 * \brief ���ܴ�����ͨ��
 */
aw_local aw_err_t __lsm6dsl_disable (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_result);

/*
 * \brief ���Ե�����
 */
aw_local aw_err_t __lsm6dsl_attr_set (void                  *p_cookie,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val);

/*
 * \brief ��ȡ����
 */
aw_local aw_err_t __lsm6dsl_attr_get (void            *p_cookie,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val);

/**
 * \brief ��������
 *
 * \note ֻ֧��һ·���������ٴε��ûḲ��֮ǰ���õ�
 */
aw_local aw_err_t __lsm6dsl_trigger_cfg (void                  *p_cookie,
                                         int                    id,
                                         uint32_t               flags,
                                         aw_sensor_trigger_cb_t pfn_cb,
                                         void                  *p_arg);

/*
 * \brief �򿪴���
 */
aw_local aw_err_t __lsm6dsl_trigger_on (void *p_cookie, int id);

/*
 * \brief �رմ���
 */
aw_local aw_err_t __lsm6dsl_trigger_off (void *p_cookie, int id);

/*******************************************************************************
  local functions
*******************************************************************************/
/*
 * \brief LSM6DSL д����
 */
aw_local aw_err_t __lsm6dsl_write (awbl_lsm6dsl_dev_t *p_this,
                                   uint32_t            subaddr,
                                   uint8_t            *p_buf,
                                   uint32_t            nbytes)
{
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_this);

    return awbl_i2c_write((struct awbl_i2c_device *)p_this,
                          AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                          p_devinfo->i2c_addr,
                          subaddr,
                          (const void*)p_buf,
                          nbytes);
}

/*
 * \brief LSM6DSL ������
 */
aw_local aw_err_t __lsm6dsl_read (awbl_lsm6dsl_dev_t *p_this,
                                  uint32_t            subaddr,
                                  uint8_t            *p_buf,
                                  uint32_t            nbytes)
{
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_this);

    return awbl_i2c_read((struct awbl_i2c_device *)p_this,
                         AW_I2C_ADDR_7BIT | AW_I2C_SUBADDR_1BYTE,
                         p_devinfo->i2c_addr,
                         subaddr,
                         (void*)p_buf,
                         nbytes);
}

/*
 * \brief ������ٶ����ݴ���
 */
aw_local aw_err_t __lsm6dsl_xl_data_deal (awbl_lsm6dsl_dev_t *p_this,
                                          uint32_t            subaddr,
                                          uint8_t             len,
                                          aw_sensor_val_t    *p_buf)
{
    int16_t data_val = 0;

    uint8_t recv_data[2] = {0};
    aw_err_t ret = AW_OK;

    ret = __lsm6dsl_read(p_this, subaddr, &recv_data[0], len);
    if (ret != AW_OK) {
        return ret;
    }

    if (len == 1) {
        data_val = (int16_t)(recv_data[0] << 8);
    } else {
        data_val = (int16_t)((recv_data[1] << 8) | recv_data[0]);
    }

    switch (p_this->full_scale[0].val) {

    case 2:
        p_buf->val  = __LSM6DSL_XL_VALUE(1, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 4:
        p_buf->val  = __LSM6DSL_XL_VALUE(2, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 8:
        p_buf->val  = __LSM6DSL_XL_VALUE(4, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 16:
        p_buf->val  = __LSM6DSL_XL_VALUE(8, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    default:
        break;
    }

    return AW_OK;
}

/*
 * \brief �������������ݴ���
 */
aw_local aw_err_t __lsm6dsl_g_data_deal (awbl_lsm6dsl_dev_t *p_this,
                                         uint32_t            subaddr,
                                         uint8_t             len,
                                         aw_sensor_val_t    *p_buf)
{
    int16_t data_val = 0;

    uint8_t recv_data[2] = {0};
    aw_err_t ret = AW_OK;

    ret = __lsm6dsl_read(p_this, subaddr, &recv_data[0], len);
    if (ret != AW_OK) {
        return ret;
    }

    if (len == 1) {
        data_val = (int16_t)(recv_data[0] << 8);
    } else {
        data_val = (int16_t)((recv_data[1] << 8) | recv_data[0]);
    }

    switch (p_this->full_scale[1].val) {

    case 250:
        p_buf->val  = __LSM6DSL_G_VALUE(1, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 500:
        p_buf->val  = __LSM6DSL_G_VALUE(2, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 1000:
        p_buf->val  = __LSM6DSL_G_VALUE(4, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    case 2000:
        p_buf->val  = __LSM6DSL_G_VALUE(8, data_val);
        p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/
        break;
    default:
        break;
    }

    return AW_OK;
}

/*
 * \brief �¶����ݴ���
 */
aw_local aw_err_t __lsm6dsl_tem_data_deal (awbl_lsm6dsl_dev_t *p_this,
                                           uint32_t            subaddr,
                                           uint8_t             len,
                                           aw_sensor_val_t    *p_buf)
{
    int16_t data_val = 0;

    uint8_t recv_data[2] = {0};
    aw_err_t ret = AW_OK;

    ret = __lsm6dsl_read(p_this, subaddr, &recv_data[0], len);
    if (ret != AW_OK) {
        return ret;
    }

    if (len == 1) {
        data_val = (int16_t)(recv_data[0] << 8);
    } else {
        data_val = (int16_t)((recv_data[1] << 8) | recv_data[0]);
    }

    p_buf->val  = __LSM6DSL_TEM_VAL(data_val);/*< \brief �����¶� */
    p_buf->unit = AW_SENSOR_UNIT_MICRO;       /*< \brief ��λĬ��Ϊ-6:10^(-6)*/

    return AW_OK;
}

/*
 * \brief ���û���һ���Ĵ�����λ
 */
aw_local aw_err_t __lsm6dsl_set_reg (awbl_lsm6dsl_dev_t *p_this,
                                     uint8_t             addr,
                                     uint8_t             value,
                                     uint8_t             start,
                                     uint8_t             len,
                                     uint8_t             flag)
{
    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    uint8_t reg_value = 0;

    uint8_t or_value = ~(((uint8_t)((0x1 << len) - 1)) << start);

    ret = __lsm6dsl_read(p_this, addr, &reg_value, 1);
    if (ret != AW_OK) {
        cur_ret = ret;
    }

    /* ��λ */
    if (flag == 1) {
        reg_value = (reg_value & or_value) | value;
    }

    /* ���� */
    if (flag == 0) {
        reg_value &= or_value;
    }

    ret = __lsm6dsl_write(p_this, addr, &reg_value, 1);
    if (ret != AW_OK) {
        cur_ret = ret;
    }

    return cur_ret;
}

/*
 * \brief ���ü��ٶȺ������ǲ���Ƶ��
 */
aw_local aw_err_t __lsm6dsl_set_rate (awbl_lsm6dsl_dev_t *p_this,
                                      int                 rate,
                                      int                 cur_id)
{
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_this);

    aw_err_t ret = AW_OK;

    uint8_t cur_rate = 0;

    uint8_t set_reg = 0;

    if (0 < rate && rate <= 3) {
        cur_rate = (uint8_t)LSM6DSL_RATE_1_6;
    } else if (10 <= rate && rate <= 14) {
        cur_rate = (uint8_t)LSM6DSL_RATE_12_5;
    } else if (24 <= rate && rate <= 28) {
        cur_rate = (uint8_t)LSM6DSL_RATE_26;
    } else if (50 <= rate && rate <= 54) {
        cur_rate = (uint8_t)LSM6DSL_RATE_52;
    } else if (100 <= rate && rate <= 110) {
        cur_rate = (uint8_t)LSM6DSL_RATE_104;
    } else if (200 <= rate && rate <= 210) {
        cur_rate = (uint8_t)LSM6DSL_RATE_208;
    } else if (400 <= rate && rate <= 420) {
        cur_rate = (uint8_t)LSM6DSL_RATE_416;
    } else if (830 <= rate && rate <= 840) {
        cur_rate = (uint8_t)LSM6DSL_RATE_833;
    } else if (1600 <= rate && rate <= 1670) {
        cur_rate = (uint8_t)LSM6DSL_RATE_1660;
    } else if (3300 <= rate && rate <= 3340) {
        cur_rate = (uint8_t)LSM6DSL_RATE_3330;
    } else if (6600 <= rate && rate <= 6670) {
        cur_rate = (uint8_t)LSM6DSL_RATE_6660;
    } else {
        return -AW_ENOTSUP;
    }

    if (cur_rate == (uint8_t)LSM6DSL_RATE_1_6) {
        if (cur_id == 0 || cur_id == 1 || cur_id == 2) {
            if (p_devinfo->xl_hm_mode == 0) {
                set_reg = __LSM6DSL_CTRL1_XL;
            } else {
                return -AW_ENOTSUP;
            }
        } else {
            return -AW_ENOTSUP;
        }
    } else {
        if (cur_id == 0 || cur_id == 1 || cur_id == 2) {
            set_reg = __LSM6DSL_CTRL1_XL;
        } else {
            set_reg = __LSM6DSL_CTRL2_G;
        }
    }

    ret = __lsm6dsl_set_reg(p_this, set_reg, cur_rate, 4, 4, 1);
    if (ret != AW_OK) {
        return ret;
    }

    p_this->sampling_rate[cur_id/3].val = rate;
    p_this->sampling_rate[cur_id/3].unit = 0;

    /* ������������ܣ���Ϊ�ı���ٶ�Ƶ�ʣ���Ƶ��Ϊ12.5��26ʱ */
    if (p_devinfo->xl_hm_mode == 0 &&
            (cur_id == 0 || cur_id == 1 || cur_id == 2) &&
            (cur_rate == (uint8_t)LSM6DSL_RATE_12_5 ||
                    cur_rate == (uint8_t)LSM6DSL_RATE_26)) {
        if (cur_rate == (uint8_t)LSM6DSL_RATE_12_5) {
            p_this->sampling_rate[2].val  = 125;
            p_this->sampling_rate[2].unit = -1;
        } else if (cur_rate == (uint8_t)LSM6DSL_RATE_26) {
            p_this->sampling_rate[2].val  = 26;
            p_this->sampling_rate[2].unit = 0;
        }
    } else if (p_devinfo->xl_hm_mode == 1 &&
                (cur_id == 0 || cur_id == 1 || cur_id == 2)) {
        p_this->sampling_rate[2].val  = 52;
        p_this->sampling_rate[2].unit = 0;
    }

    return ret;
}

/*
 * \brief �޸�����ֵ
 */
aw_local aw_err_t __lsm6dsl_set_full_scale (awbl_lsm6dsl_dev_t *p_this,
                                            int                 value,
                                            int                 cur_id)
{
    aw_err_t ret = AW_OK;

    uint8_t set_reg = 0;
    uint8_t reg_addr = 0;

    /* �޸ļ��ٶ����� */
    if (cur_id == 0 || cur_id == 1 || cur_id == 2) {
        reg_addr = __LSM6DSL_CTRL1_XL;
        switch (value) {
        case 2:
            set_reg = LSM6DSL_FULL_SCALE_2;
            break;
        case 4:
            set_reg = LSM6DSL_FULL_SCALE_4;
            break;
        case 8:
            set_reg = LSM6DSL_FULL_SCALE_8;
            break;
        case 16:
            set_reg = LSM6DSL_FULL_SCALE_16;
            break;
        default:
            return -AW_ENOTSUP;
        }
    } else {
        /* �޸����������� */
        reg_addr = __LSM6DSL_CTRL2_G;
        switch (value) {
        case 250:
            set_reg = LSM6DSL_FULL_SCALE_250;
            break;
        case 500:
            set_reg = LSM6DSL_FULL_SCALE_500;
            break;
        case 1000:
            set_reg = LSM6DSL_FULL_SCALE_1000;
            break;
        case 2000:
            set_reg = LSM6DSL_FULL_SCALE_2000;
            break;
        default:
            return -AW_ENOTSUP;
        }
    }

    ret = __lsm6dsl_set_reg(p_this, reg_addr, set_reg, 2, 2, 1);
    if (ret != AW_OK) {
        return ret;
    }

    if (cur_id == 0 || cur_id == 1 || cur_id == 2) {
        p_this->full_scale[0].val  = value;
        p_this->full_scale[0].unit = 0;
    } else {
        p_this->full_scale[1].val  = value;
        p_this->full_scale[1].unit = 0;
    }

    return ret;
}

/*
 * \brief ���ü��ٶ�ƫ�ƼĴ�����ֵ
 */
aw_local aw_err_t __lsm6dsl_offset_reg (awbl_lsm6dsl_dev_t *p_this,
                                        int                 value,
                                        int                 cur_id)
{
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_this);

    aw_err_t ret = AW_OK;

    uint8_t reg_addr = 0;

    uint8_t off_set_value = 0;

    if (cur_id == 3 || cur_id == 4 || cur_id == 5) {
        return -AW_ENOTSUP;
    }

    if (p_devinfo->xl_compensate == 0) {
        if ((value >= (-__LSM6DSL_USR_OFF_W_10)) &&
                (value <= __LSM6DSL_USR_OFF_W_10)) {
            off_set_value = (int8_t)((value)/(__LSM6DSL_USR_OFF_W_10/127));
        } else {
            return -AW_EINVAL;
        }
    } else if (p_devinfo->xl_compensate == 1) {
        if ((value >= (-__LSM6DSL_USR_OFF_W_6)) &&
                (value <= __LSM6DSL_USR_OFF_W_6)) {
            off_set_value = (int8_t)((value)/(__LSM6DSL_USR_OFF_W_6/127));
        } else {
            return -AW_EINVAL;
        }
    } else {
        return -AW_ENOTSUP;
    }

    if (cur_id == 0) {
        reg_addr = __LSM6DSL_X_OFS_USR;
    } else if (cur_id == 1) {
        reg_addr = __LSM6DSL_Y_OFS_USR;
    } else {
        reg_addr = __LSM6DSL_Z_OFS_USR;
    }

    ret = __lsm6dsl_set_reg(p_this, reg_addr, off_set_value, 0, 8, 1);
    if (ret != AW_OK) {
        return ret;
    }

    p_this->xl_off_data[cur_id].val  = value;
    p_this->xl_off_data[cur_id].unit = AW_SENSOR_UNIT_MICRO;

    return ret;
}

/*
 * \brief �����¼����
 */
aw_local void __wake_up_event (awbl_lsm6dsl_dev_t *p_this, uint8_t wake_up_src)
{
    if (p_this->pfn_basic_fnc[1] && (wake_up_src & 0x08)) {
        if (wake_up_src & 0x4) {
            p_this->pfn_basic_fnc[1](p_this->p_basic[1], LSM6DSL_WAKE_UP_X);
        }
        if (wake_up_src & 0x2) {
            p_this->pfn_basic_fnc[1](p_this->p_basic[1], LSM6DSL_WAKE_UP_Y);
        }
        if (wake_up_src & 0x1) {
            p_this->pfn_basic_fnc[1](p_this->p_basic[1], LSM6DSL_WAKE_UP_Z);
        }
    }
}

/*
 * \brief 6D/4D�¼����
 */
aw_local void __6d_4d_event (awbl_lsm6dsl_dev_t *p_this)
{
    uint8_t d6d_src = 0;

    if (p_this->pfn_basic_fnc[2]) {
        __lsm6dsl_read(p_this, __LSM6DSL_D6D_SRC, &d6d_src, 1);
        if (d6d_src & 0x40) {
            if (d6d_src & 0x20) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_ZH);
            } else if (d6d_src & 0x10) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_ZL);
            } else if (d6d_src & 0x08) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_YH);
            } else if (d6d_src & 0x04) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_YL);
            } else if (d6d_src & 0x02) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_XH);
            } else if (d6d_src & 0x01) {
                p_this->pfn_basic_fnc[2](p_this->p_basic[2], LSM6DSL_6D_4D_XL);
            }
        }
    }
}

/*
 * \brief ��˫���¼����
 */
aw_local void __tap_event (awbl_lsm6dsl_dev_t *p_this)
{
    uint8_t tap_src = 0;

    if (p_this->pfn_basic_fnc[3]) {
        __lsm6dsl_read(p_this, __LSM6DSL_TAP_SRC, &tap_src, 1);

        /* �����¼� */
        if (tap_src & 0x20) {
            if ((tap_src & 0x08) == 0) {
                if (tap_src & 0x04) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_XH_TAP);
                } else if (tap_src & 0x02) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_YH_TAP);
                } else if (tap_src & 0x01) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_ZH_TAP);
                }
            } else if (tap_src & 0x08) {
                if (tap_src & 0x04) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_XL_TAP);
                } else if (tap_src & 0x02) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_YL_TAP);
                } else if (tap_src & 0x01) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_SINGLE_TAP | LSM6DSL_ZL_TAP);
                }
            }
        }

        /* ˫���¼� */
        if (tap_src & 0x10) {
            if ((tap_src & 0x08) == 0) {
                if (tap_src & 0x04) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_XH_TAP);
                } else if (tap_src & 0x02) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_YH_TAP);
                } else if (tap_src & 0x01) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_ZH_TAP);
                }
            } else if (tap_src & 0x08) {
                if (tap_src & 0x04) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_XL_TAP);
                } else if (tap_src & 0x02) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_YL_TAP);
                } else if (tap_src & 0x01) {
                    p_this->pfn_basic_fnc[3](p_this->p_basic[3], LSM6DSL_DOUBLE_TAP | LSM6DSL_ZL_TAP);
                }
            }
        }
    }
}

/*
 * \brief ��Ծ�ǻ�Ծ���
 */
aw_local void __activity_event (awbl_lsm6dsl_dev_t *p_this, uint8_t wake_up_src)
{
    if (p_this->pfn_basic_fnc[4]) {
        if (wake_up_src & 0x10) {
            p_this->pfn_basic_fnc[4](p_this->p_basic[4], LSM6DSL_INACTIVITY);
        } else if ((wake_up_src & 0x10) == 0) {
            p_this->pfn_basic_fnc[4](p_this->p_basic[4], LSM6DSL_ACTIVITING);
        }
    }
}

/*
 * \brief INT1�����ж�
 */
aw_local void __lsm6dsl_int1_callback (void *p_arg)
{
    awbl_lsm6dsl_dev_t *p_this = __MY_GET_DEV(p_arg);

    aw_isr_defer_job_add(&p_this->g_myjob[0]); /*< \brief ����ж��ӳٴ������� */
}

/*
 * \brief INT2�����ж�
 */
aw_local void __lsm6dsl_int2_callback (void *p_arg)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

    aw_gpio_trigger_off(p_devinfo->int2_pin);

    aw_isr_defer_job_add(&p_this->g_myjob[1]); /*< \brief ����ж��ӳٴ������� */
}

/*
 * \brief INT1�ӳ��жϴ�����
 */
aw_local void __lsm6dsl_int1 (void *p_arg)
{
    awbl_lsm6dsl_dev_t *p_this = __MY_GET_DEV(p_arg);

    uint8_t wake_up_src = 0;

    if (p_this->pfn_basic_fnc[0] ||
            p_this->pfn_basic_fnc[1] ||
            p_this->pfn_basic_fnc[4]) {
        __lsm6dsl_read(p_this, __LSM6DSL_WAKE_UP_SRC, &wake_up_src, 1);
    }

    /* ���������¼���� */
    if (p_this->pfn_basic_fnc[0] && (wake_up_src & 0x20)) {
        p_this->pfn_basic_fnc[0](p_this->p_basic[0], -1);
    }

    /* �����¼���� */
    __wake_up_event(p_this, wake_up_src);

    /* 6D/4D�¼���� */
    __6d_4d_event(p_this);

    /* ��˫���¼���� */
    __tap_event(p_this);

    /* ��Ծ�ͷǻ�Ծ��� */
    __activity_event(p_this, wake_up_src);
}

/*
 * \brief INT2�ӳ��жϴ�����
 */
aw_local void __lsm6dsl_int2 (void *p_arg)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_arg);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_arg);

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

    if (p_this->pfn_trigger_fnc[3] &&
            (p_this->flags[3] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[3](p_this->p_argc[3],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_fnc[4] &&
            (p_this->flags[4] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[4](p_this->p_argc[4],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_fnc[5] &&
            (p_this->flags[5] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[5](p_this->p_argc[5],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    if (p_this->pfn_trigger_fnc[6] &&
            (p_this->flags[6] & AW_SENSOR_TRIGGER_DATA_READY)) {
        p_this->pfn_trigger_fnc[6](p_this->p_argc[6],
                                   AW_SENSOR_TRIGGER_DATA_READY);
    }

    aw_gpio_trigger_on(p_devinfo->int2_pin);
}

/*******************************************************************************
  ����ȫ�ֱ�������
*******************************************************************************/
/** \brief �����ṩ�ķ��� */
AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_const struct awbl_dev_method __g_lsm6dsl_dev_methods[] = {
        AWBL_METHOD(awbl_sensorserv_get, __lsm6dsl_sensorserv_get),
        AWBL_METHOD_END
};

/** \brief ������ڵ� */
aw_local aw_const struct awbl_drvfuncs __g_lsm6dsl_drvfuncs = {
        NULL,                     /*< \brief ��һ�׶γ�ʼ��                   */
        __lsm6dsl_inst_init2,     /*< \brief �ڶ��׶γ�ʼ��                   */
        __lsm6dsl_inst_connect    /*< \brief �����׶γ�ʼ��                   */
};

/** \brief ����ע����Ϣ */
aw_local aw_const struct awbl_drvinfo __g_drvinfo_lsm6dsl = {
    AWBL_VER_1,                   /*< \brief AWBus �汾��                     */
    AWBL_BUSID_I2C,               /*< \brief ���� ID                          */
    AWBL_LSM6DSL_NAME,            /*< \brief ������                           */
    &__g_lsm6dsl_drvfuncs,        /*< \brief ������ڵ�                       */
    &__g_lsm6dsl_dev_methods[0],  /*< \brief �����ṩ�ķ���                   */
    NULL                          /*< \brief ����̽�⺯��                     */
};

/** \brief ������ */
aw_local aw_const struct awbl_sensor_servfuncs __g_lsm6dsl_servfuncs = {
        __lsm6dsl_data_get,
        __lsm6dsl_enable,
        __lsm6dsl_disable,
        __lsm6dsl_attr_set,
        __lsm6dsl_attr_get,
        __lsm6dsl_trigger_cfg,
        __lsm6dsl_trigger_on,
        __lsm6dsl_trigger_off
};

/*******************************************************************************
    ���غ�������
*******************************************************************************/
/** \brief �����������ȡ���� */
aw_local aw_err_t __lsm6dsl_sensorserv_get (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    awbl_sensor_service_t *p_sensor_serv = &p_this->sensor_serv;

    /** \brief �ô�������ͨ������ */
    aw_local aw_const awbl_sensor_type_info_t __g_senter_type[3] = {
            {AW_SENSOR_TYPE_ACCELEROMETER, 3},
            {AW_SENSOR_TYPE_GYROSCOPE, 3},
            {AW_SENSOR_TYPE_TEMPERATURE, 1}
    };

    /** \brief ��������������Ϣ */
    aw_local aw_const awbl_sensor_servinfo_t  __g_senser_info = {
            7,
            __g_senter_type,
            AW_NELEMENTS(__g_senter_type)
    };

    p_sensor_serv->p_servinfo  = &__g_senser_info;
    p_sensor_serv->start_id    = p_devinfo->start_id;
    p_sensor_serv->p_servfuncs = &__g_lsm6dsl_servfuncs;
    p_sensor_serv->p_cookie    = (awbl_lsm6dsl_dev_t*)p_this;
    p_sensor_serv->p_next      = NULL;

    *((awbl_sensor_service_t**)p_arg) = p_sensor_serv;

    return AW_OK;
}

/******************************************************************************/
/** \brief �ڶ��γ�ʼ�������������ȡ���� */
aw_local void __lsm6dsl_inst_init2 (struct awbl_dev *p_dev)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    int i = 0;

    /* Ƶ�� */
    p_this->sampling_rate[0].val    = 0;
    p_this->sampling_rate[0].unit   = AW_SENSOR_UNIT_BASE;
    p_this->sampling_rate[1].val    = 0;
    p_this->sampling_rate[1].unit   = AW_SENSOR_UNIT_BASE;
    p_this->sampling_rate[2].val    = 0;
    p_this->sampling_rate[2].unit   = AW_SENSOR_UNIT_BASE;

    /* ���� */
    p_this->full_scale[0].val       = 2;
    p_this->full_scale[0].unit      = AW_SENSOR_UNIT_BASE;
    p_this->full_scale[1].val       = 250;
    p_this->full_scale[1].unit      = AW_SENSOR_UNIT_BASE;

    /* ��־λ */
    p_this->en_or_dis               = 0;
    p_this->on_or_off               = 0;

    /* ���ٶ�ƫ���� */
    p_this->xl_off_data[0].val      = 0;
    p_this->xl_off_data[0].unit     = AW_SENSOR_UNIT_BASE;
    p_this->xl_off_data[1].val      = 0;
    p_this->xl_off_data[1].unit     = AW_SENSOR_UNIT_BASE;
    p_this->xl_off_data[2].val      = 0;
    p_this->xl_off_data[2].unit     = AW_SENSOR_UNIT_BASE;

    /* �ص����� */
    for (i = 0; i < 7; i++) {
        p_this->pfn_trigger_fnc[i]  = NULL;
        p_this->p_argc[i]           = NULL;
        p_this->flags[i]            = 0;
    }

    /* ���⹦�ܻص����� */
    for (i = 0; i < 5; i++) {
        p_this->pfn_basic_fnc[i]    = NULL;
        p_this->p_basic[i]          = NULL;
    }

    if (p_devinfo->pfunc_plfm_init != NULL) {
        p_devinfo->pfunc_plfm_init();
    }

    aw_isr_defer_job_init(&p_this->g_myjob[0], __lsm6dsl_int1, p_this);
    aw_isr_defer_job_init(&p_this->g_myjob[1], __lsm6dsl_int2, p_this);

    if (p_devinfo->int1_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->int1_pin, AW_GPIO_TRIGGER_RISE);
        aw_gpio_trigger_connect(p_devinfo->int1_pin,
                                __lsm6dsl_int1_callback,
                                p_this);
    }
    if (p_devinfo->int2_pin != -1) {

        aw_gpio_trigger_cfg(p_devinfo->int2_pin, AW_GPIO_TRIGGER_RISE);
        aw_gpio_trigger_connect(p_devinfo->int2_pin,
                                __lsm6dsl_int2_callback,
                                p_this);
    }
}

/******************************************************************************/
/** \brief �����׶γ�ʼ������ */
aw_local void __lsm6dsl_inst_connect (struct awbl_dev *p_dev)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    uint8_t lsm6dsl_id = 0;

    uint8_t lsm6dsl_mode;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    /* reset */
    lsm6dsl_mode = 0x05;
    __lsm6dsl_write(p_this, __LSM6DSL_CTRL3_C, &lsm6dsl_mode, 1);
    aw_udelay(60);

    ret = __lsm6dsl_read(p_this, __LSM6DSL_WHO_AM_I, &lsm6dsl_id, 1);
    if (ret != AW_OK || lsm6dsl_id != __LSM6DSL_MY_ID) {
        cur_ret = -AW_ENXIO;
    } else {

        /* ���ٶȸ�����ѡ�� */
        if (p_devinfo->xl_hm_mode == 0) {
            lsm6dsl_mode = 0x10;
            ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL6_C, lsm6dsl_mode, 4, 1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

        /* �����Ǹ�����ѡ�� */
        if (p_devinfo->g_hm_mode == 0) {
            lsm6dsl_mode = 0x00;
            ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL7_G, lsm6dsl_mode, 7, 1, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }
        }

        /* ������ٶȲ�������ѡ�� */
        if (p_devinfo->xl_compensate == 0) {
            lsm6dsl_mode = 0x00;
        } else {
            lsm6dsl_mode = 0x08;
        }
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL6_C, lsm6dsl_mode, 3, 1, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /* �޸�Ϊ�ж�������ʽ */
        lsm6dsl_mode = 0x80;
        ret = __lsm6dsl_write(p_this, __LSM6DSL_DRDY_CFG_G, &lsm6dsl_mode, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }

        /* ���ݸ���BDU����Ϊ1 */
        lsm6dsl_mode = 0x44;
        ret = __lsm6dsl_write(p_this, __LSM6DSL_CTRL3_C, &lsm6dsl_mode, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    if (cur_ret != AW_OK) {
        aw_kprintf("\r\nSensor LSM6DSL Init is ERROR! \r\n");
    }
}

/******************************************************************************/
/** \brief �ϵ�����*/
aw_local void __lsm6dsl_reconnect (struct awbl_dev *p_dev)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);
	
    int cur_id = 0;
    uint8_t i = 0;
	
    __lsm6dsl_inst_init2((awbl_dev_t *)p_dev);
    __lsm6dsl_inst_connect ((awbl_dev_t *)p_dev);

    for(i = 0; i < 7; i++){
        cur_id = p_devinfo->start_id+i;
        __lsm6dsl_enable(p_this, &cur_id, 1, NULL);
    }
}

/******************************************************************************/
/** \brief ��ָ��ͨ����ȡ�������� */
aw_local aw_err_t __lsm6dsl_data_get (void            *p_cookie,
                                      const int       *p_ids,
                                      int              num,
                                      aw_sensor_val_t *p_buf)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;

    uint8_t status_val = 0;
    uint8_t timeout = 0;

    int cur_id = p_ids[0] - p_devinfo->start_id;
    int i = 0;

    if (!AW_BIT_GET(p_this->en_or_dis, 7)) {
        return -AW_EPERM;
    }

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        /* ��ȡ���ٶ����� */
        if (cur_id == 0 || cur_id == 1 || cur_id == 2) {

            if (((status_val & 0x1) != 0x1) &&
                    !(p_this->flags[cur_id] & AW_SENSOR_TRIGGER_DATA_READY)) {
                do {
                    ret = __lsm6dsl_read(p_this, __LSM6DSL_STATUS_REG, &status_val, 1);
                    if (ret != AW_OK) {
                        return ret;
                    }
                    if(++timeout > 100){
                        __lsm6dsl_reconnect((awbl_dev_t *)p_cookie);
                        return -1;
                    }
                } while ((status_val & 0x1) != 0x1);
            }

            switch (cur_id) {
            case 0:
                 ret = __lsm6dsl_xl_data_deal(p_this, __LSM6DSL_OUTX_L_XL, 2, &p_buf[i]);
                 if (ret != AW_OK) {
                     return ret;
                 }
                break;
            case 1:
                ret = __lsm6dsl_xl_data_deal(p_this, __LSM6DSL_OUTY_L_XL, 2, &p_buf[i]);
                if (ret != AW_OK) {
                    return ret;
                }
                break;
            case 2:
                ret = __lsm6dsl_xl_data_deal(p_this, __LSM6DSL_OUTZ_L_XL, 2, &p_buf[i]);
                if (ret != AW_OK) {
                    return ret;
                }
                break;
            default:
                break;
            }
        }

        /* ��ȡ���������� */
        if (cur_id == 3 || cur_id == 4 || cur_id == 5) {

            if (((status_val & 0x2) != 0x2) &&
                    !(p_this->flags[cur_id] & AW_SENSOR_TRIGGER_DATA_READY)) {
                do {
                    ret = __lsm6dsl_read(p_this, __LSM6DSL_STATUS_REG, &status_val, 1);
                    if (ret != AW_OK) {
                        return ret;
                    }
                    if(++timeout > 100){
                        __lsm6dsl_reconnect((awbl_dev_t *)p_cookie);
                        return -1;
                    }
                } while ((status_val & 0x2) != 0x2);
            }

            switch (cur_id) {
            case 3:
                 ret = __lsm6dsl_g_data_deal(p_this, __LSM6DSL_OUTX_L_G, 2, &p_buf[i]);
                 if (ret != AW_OK) {
                     return ret;
                 }
                break;
            case 4:
                ret = __lsm6dsl_g_data_deal(p_this, __LSM6DSL_OUTY_L_G, 2, &p_buf[i]);
                if (ret != AW_OK) {
                    return ret;
                }
                break;
            case 5:
                ret = __lsm6dsl_g_data_deal(p_this, __LSM6DSL_OUTZ_L_G, 2, &p_buf[i]);
                if (ret != AW_OK) {
                    return ret;
                }
                break;
            default:
                break;
            }
        }

        /* ��ȡ�¶����� */
        if (cur_id == 6) {

            if (((status_val & 0x4) != 0x4) &&
                    !(p_this->flags[cur_id] & AW_SENSOR_TRIGGER_DATA_READY)) {
                do {
                    ret = __lsm6dsl_read(p_this, __LSM6DSL_STATUS_REG, &status_val, 1);
                    if (ret != AW_OK) {
                        return ret;
                    }
                    if(++timeout > 100){
                        __lsm6dsl_reconnect((awbl_dev_t *)p_cookie);
                        return -1;
                    }
                } while ((status_val & 0x4) != 0x4);
            }

            ret = __lsm6dsl_tem_data_deal(p_this, __LSM6DSL_OUT_TEMP_L, 2, &p_buf[i]);
            if (ret != AW_OK) {
                return ret;
            }
        }

        /* �˳� */
        if (cur_id > 6) {
            break;   /*< \brief �����ڸ��豸��ͨ����ֱ���˳� */
        }

    }

    return AW_OK;
}

/******************************************************************************/
/** \brief ʹ�ܸô�������һ������ͨ�� */
aw_local aw_err_t __lsm6dsl_enable (void            *p_cookie,
                                    const int       *p_ids,
                                    int              num,
                                    aw_sensor_val_t *p_result)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    uint8_t ctrl_reg = 0;

    aw_err_t ret     = AW_OK;
    aw_err_t cur_ret = AW_OK;

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if ((cur_id == 0 || cur_id == 1 || cur_id == 2 || cur_id == 6) &&
                p_this->sampling_rate[0].val == 0) {
            ctrl_reg = 0x10;       /*< \brief ����Ƶ��12.5HZ */
            ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL1_XL, ctrl_reg, 4, 4, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            } else {
                p_this->sampling_rate[0].val  = 125;
                p_this->sampling_rate[0].unit = -1;
                if (p_devinfo->xl_hm_mode == 0) {
                    p_this->sampling_rate[2].val  = 125;
                    p_this->sampling_rate[2].unit = -1;
                } else {
                    p_this->sampling_rate[2].val  = 52;
                    p_this->sampling_rate[2].unit = 0;
                }
            }
        }

        if ((cur_id == 3 || cur_id == 4 || cur_id == 5) &&
                p_this->sampling_rate[1].val == 0) {
            ctrl_reg = 0x10;       /*< \brief ����Ƶ��12.5HZ */
            ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL2_G, ctrl_reg, 4, 4, 1);
            if (ret != AW_OK) {
                cur_ret = ret;
            } else {
                p_this->sampling_rate[1].val  = 125;
                p_this->sampling_rate[1].unit = -1;
            }
        }

        if (cur_id > 6) {
            break;   /*< \brief �����ڸ��豸��ͨ����ֱ���˳� */
        }

        /* ���÷���ֵ */
        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], cur_ret);
        }

        /* ��λ��־λ */
        if (cur_ret == AW_OK) {
            AW_BIT_SET(p_this->en_or_dis, 7);
            AW_BIT_SET(p_this->en_or_dis, cur_id);
        }
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief ���ܴ�����ͨ�� */
aw_local aw_err_t __lsm6dsl_disable (void            *p_cookie,
                                     const int       *p_ids,
                                     int              num,
                                     aw_sensor_val_t *p_result)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    int i = 0;
    int cur_id = 0;

    uint8_t ctrl_reg = 0;

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    for (i = 0; i < num; i++) {

        cur_id = p_ids[i] - p_devinfo->start_id;

        if (cur_id > 6) {
            break;     /*< \brief ���˴�ͨ�������ڸô����������ж�����ͨ�� */
        }

        AW_BIT_CLR(p_this->en_or_dis, cur_id);

        if (p_result != NULL) {
            AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], cur_ret);
        }
    }

    /* �رռ��ٶȺ��¶� */
    if ((p_this->en_or_dis & 0x47) == 0) {
        ctrl_reg = 0x00;       /*< \brief ����Ϊpower down */
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL1_XL, ctrl_reg, 4, 4, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        } else {
            p_this->sampling_rate[0].val  = 0;
            p_this->sampling_rate[0].unit = 0;
            p_this->sampling_rate[2].val  = 0;
            p_this->sampling_rate[2].unit = 0;
            p_this->full_scale[0].val     = 2;
            p_this->full_scale[0].unit    = 0;
        }
    }

    /* �ر������� */
    if ((p_this->en_or_dis & 0x38) == 0) {
        ctrl_reg = 0x00;       /*< \brief ����Ϊpower down */
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL2_G, ctrl_reg, 4, 4, 1);
        if (ret != AW_OK) {
            cur_ret = ret;
        } else {
            p_this->sampling_rate[1].val  = 0;
            p_this->sampling_rate[1].unit = 0;
            p_this->full_scale[1].val     = 250;
            p_this->full_scale[1].unit    = 0;
        }
    }

    if ((p_this->en_or_dis & 0x7f) == 0) {
        AW_BIT_CLR(p_this->en_or_dis, 7);
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief ���Ե����� */
aw_local aw_err_t __lsm6dsl_attr_set (void                  *p_cookie,
                                      int                    id,
                                      int                    attr,
                                      const aw_sensor_val_t *p_val)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    aw_sensor_val_t sensor_val = {0, 0};

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 6) {
        return -AW_ENODEV;
    }
    if (cur_id == 6) {
        return -AW_ENOTSUP;
    }

    switch (attr) {

    /** \brief ����Ƶ������ */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        sensor_val.val  = p_val->val;
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_BASE);
        ret = __lsm6dsl_set_rate(p_this, sensor_val.val, cur_id);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
        break;

    /** \brief ������ֵ���� */
    case AW_SENSOR_ATTR_FULL_SCALE:
        sensor_val.val  = p_val->val;
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_BASE);
        ret = __lsm6dsl_set_full_scale(p_this, sensor_val.val, cur_id);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
        break;

    /** \brief ƫ��ֵ�����ڴ�����У׼ */
    case AW_SENSOR_ATTR_OFFSET:
        sensor_val.val  = p_val->val;
        sensor_val.unit = p_val->unit;
        aw_sensor_val_unit_convert(&sensor_val, 1, AW_SENSOR_UNIT_MICRO);
        ret = __lsm6dsl_offset_reg(p_this, sensor_val.val, cur_id);
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
aw_local aw_err_t __lsm6dsl_attr_get (void            *p_cookie,
                                      int              id,
                                      int              attr,
                                      aw_sensor_val_t *p_val)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 6) {
        return -AW_ENODEV;
    }

    switch(attr) {

    /** \brief ����Ƶ�ʻ�ȡ */
    case AW_SENSOR_ATTR_SAMPLING_RATE:
        p_val->val  = p_this->sampling_rate[cur_id/3].val;
        p_val->unit = p_this->sampling_rate[cur_id/3].unit;
        break;

    /** \brief ������ֵ��ȡ */
    case AW_SENSOR_ATTR_FULL_SCALE:
        if (cur_id == 6) {
            return -AW_ENOTSUP;
        }
        p_val->val  = p_this->full_scale[cur_id/3].val;
        p_val->unit = p_this->full_scale[cur_id/3].unit;
        break;

    /** \brief ƫ��ֵ�����ڴ�����У׼ */
    case AW_SENSOR_ATTR_OFFSET:
        if (cur_id >= 3) {
            return -AW_ENOTSUP;
        }
        p_val->val = p_this->xl_off_data[cur_id].val;
        p_val->unit = p_this->xl_off_data[cur_id].unit;
        break;

    default:
        return -AW_ENOTSUP;
    }

    return ret;
}

/******************************************************************************/
/**
 * \brief ��������
 *
 * \note ֻ֧��һ·���������ٴε��ûḲ��֮ǰ���õ�
 */
aw_local aw_err_t __lsm6dsl_trigger_cfg (void                   *p_cookie,
                                         int                     id,
                                         uint32_t                flags,
                                         aw_sensor_trigger_cb_t  pfn_cb,
                                         void                   *p_arg)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 6) {
        return -AW_ENODEV;
    }

    switch (flags) {
    case AW_SENSOR_TRIGGER_DATA_READY:
        break;
    default:
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_fnc[cur_id] = pfn_cb;
    p_this->p_argc[cur_id]          = p_arg;
    p_this->flags[cur_id]           = flags;

    return ret;
}

/******************************************************************************/
/** \brief �򿪴��� */
aw_local aw_err_t __lsm6dsl_trigger_on (void *p_cookie, int id)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 6) {
        return -AW_ENOTSUP;
    }

    switch (p_this->flags[cur_id]) {

    case AW_SENSOR_TRIGGER_DATA_READY:
            ret = __lsm6dsl_set_reg(p_this,
                                    __LSM6DSL_INT2_CTRL,
                                    (uint8_t)(0x1 << (cur_id/3)),
                                    0,
                                    1,
                                    1);
            if (ret != AW_OK) {
                cur_ret = ret;
            }

            if (cur_ret == AW_OK) {
                AW_BIT_SET(p_this->on_or_off, cur_id);
            }

        break;

    default:
        return -AW_EPERM;

    }

    if ((p_devinfo->int2_pin != -1) && cur_ret == AW_OK) {
        ret = aw_gpio_trigger_on(p_devinfo->int2_pin);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    return cur_ret;
}

/******************************************************************************/
/** \brief �رմ��� */
aw_local aw_err_t __lsm6dsl_trigger_off (void *p_cookie, int id)
{
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_cookie);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_cookie);

    aw_err_t ret = AW_OK;
    aw_err_t cur_ret = AW_OK;

    int cur_id = id - p_devinfo->start_id;

    if (cur_id < 0 || cur_id > 6) {
        return -AW_ENOTSUP;
    }

    p_this->pfn_trigger_fnc[cur_id] = NULL;
    p_this->p_argc[cur_id]          = NULL;
    p_this->flags[cur_id]           = 0;

    AW_BIT_CLR(p_this->on_or_off, cur_id);

    /* �رռ��ٶ����ݴ��� */
    if ((p_this->on_or_off & 0x3) == 0) {
        ret = __lsm6dsl_set_reg(p_this,
                                __LSM6DSL_INT2_CTRL,
                                (uint8_t)0x1,
                                0,
                                1,
                                0);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    /* �ر����������ݴ��� */
    if ((p_this->on_or_off & 0x38) == 0) {
        ret = __lsm6dsl_set_reg(p_this,
                                __LSM6DSL_INT2_CTRL,
                                (uint8_t)0x2,
                                1,
                                1,
                                0);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    /* �ر��¶����ݴ��� */
    if ((p_this->on_or_off & 0x40) == 0) {
        ret = __lsm6dsl_set_reg(p_this,
                                __LSM6DSL_INT2_CTRL,
                                (uint8_t)0x4,
                                2,
                                1,
                                0);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    /* �ر����Ŵ��� */
    if ((p_this->on_or_off == 0) && (p_devinfo->int2_pin != -1)) {
        ret = aw_gpio_trigger_off(p_devinfo->int2_pin);
        if (ret != AW_OK) {
            cur_ret = ret;
        }
    }

    return cur_ret;
}

/*******************************************************************************
  extern functions
*******************************************************************************/
/**
 * \brief �� LSM6DSL ����ע�ᵽ AWBus ��ϵͳ��
 */
void awbl_lsm6dsl_drv_register (void)
{
    awbl_drv_register(&__g_drvinfo_lsm6dsl);
}

/**
 * \brief �������幦�ܼ�⺯��
 */
aw_err_t awbl_lsm6dsl_free_fall_on (aw_sensor_trigger_cb_t    pfn_fnc,
                                    void                     *p_argc,
                                    awbl_lsm6dsl_free_fall_t  ff_threshold,
                                    uint8_t                   ff_time)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    aw_err_t ret = AW_OK;

    uint8_t reg1 = 0;
    uint8_t reg2 = 0;

    if (p_this == NULL || pfn_fnc == NULL) {
        return -AW_EINVAL;
    }
    if (ff_time < 1 || ff_time > 63) {
        return -AW_EINVAL;
    }

    p_this->pfn_basic_fnc[0] = pfn_fnc;
    p_this->p_basic[0]       = p_argc;

    /* �����ж� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x80, 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* latched */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x01, 0, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ʱ������ */
    reg1 = ff_time >> 5;
    reg2 = ff_time & 0x1f;
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_DUR, (reg1 << 7), 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_FREE_FALL, (reg2 << 3), 3, 5, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ������ֵ */
    reg2 = (uint8_t)ff_threshold;
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_FREE_FALL, reg2, 0, 3, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ·�ɵ�INT1�� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x10, 4, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_gpio_trigger_on(p_devinfo->int1_pin);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/**
 * \brief �����¼����ܼ�⺯��
 */
aw_err_t awbl_lsm6dsl_wake_up_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                  void                   *p_argc,
                                  uint8_t                 wu_threshold,
                                  uint8_t                 wu_time)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    aw_sensor_val_t cur_rate;

    aw_err_t ret = AW_OK;

    if (p_this == NULL || pfn_fnc == NULL) {
        return -AW_EINVAL;
    }

    if (wu_threshold < 1 || wu_threshold > 63 || wu_time < 0 || wu_time > 3) {
        return -AW_EINVAL;
    }

    p_this->pfn_basic_fnc[1] = pfn_fnc;
    p_this->p_basic[1]       = p_argc;

    /* ���ó���ʱ�� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_DUR, (wu_time << 5), 5, 2, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ������ֵ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_THS, wu_threshold, 0, 6, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* �����ж� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x80, 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* latched */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x01, 0, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��ֹ�״����еı�Ҫ���� */
    cur_rate.val = p_this->sampling_rate[0].val;
    cur_rate.unit = p_this->sampling_rate[0].unit;
    ret = __lsm6dsl_set_rate(p_this, 833, 0);
    if (ret != AW_OK) {
        return ret;
    }
    aw_mdelay(10);
    aw_sensor_val_unit_convert(&cur_rate, 1, AW_SENSOR_UNIT_BASE);
    ret = __lsm6dsl_set_rate(p_this, cur_rate.val, 0);
    if (ret != AW_OK) {
        return ret;
    }

    /* ·�ɵ�INT1������ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x20, 5, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_gpio_trigger_on(p_devinfo->int1_pin);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/**
 * \brief ����6D/4D��λ��⹦�ܺ���
 */
aw_err_t awbl_lsm6dsl_6d_4d_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                void                   *p_argc,
                                uint8_t                 d_threshold,
                                uint8_t                 d6_or_d4)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    aw_err_t ret = AW_OK;

    if (p_this == NULL || pfn_fnc == NULL) {
        return -AW_EINVAL;
    }

    if (d_threshold < 0 || d_threshold > 3 || d6_or_d4 < 0 || d6_or_d4 > 1) {
        return -AW_EINVAL;
    }

    p_this->pfn_basic_fnc[2] = pfn_fnc;
    p_this->p_basic[2]       = p_argc;

    /* �����ж� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x80, 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* latched */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x01, 0, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* �Ƕ���ֵ���� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_THS_6D, (d_threshold << 5), 5, 2, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* 4Dѡ�� */
    if (d6_or_d4) {
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_THS_6D, 0x80, 7, 1, 1);
        if (ret != AW_OK) {
            return ret;
        }
    /* 6Dѡ�� */
    } else {
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_THS_6D, 0x80, 7, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /* ��LPF2 */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_CTRL8_XL, 0x01, 0, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ·�ɵ�INT1������ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x04, 2, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_gpio_trigger_on(p_devinfo->int1_pin);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/**
 * \brief ���õ����¼��������õ�����˫���¼���⹦�ܺ���
 */
aw_err_t awbl_lsm6dsl_tap_on (aw_sensor_trigger_cb_t  pfn_fnc,
                              void                   *p_argc,
                              uint8_t                 tap_threshold,
                              uint8_t                 shock_time,
                              uint8_t                 quiet_time,
                              uint8_t                 dur_time,
                              uint8_t                 one_or_two)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    aw_err_t ret = AW_OK;

    uint8_t int_dur2 = 0;

    if (p_this == NULL || pfn_fnc == NULL) {
        return -AW_EINVAL;
    }

    if (tap_threshold < 1 || tap_threshold > 31 ||
            shock_time < 1 || shock_time > 3 ||
            quiet_time < 1 || quiet_time > 3 ||
            dur_time < 1 || dur_time > 15 ||
            one_or_two < 0 || one_or_two > 2) {
        return -AW_EINVAL;
    }

    if (p_this->sampling_rate[0].val < 400 || p_this->sampling_rate[0].val > 840) {
        return -AW_EPERM;
    }

    p_this->pfn_basic_fnc[3] = pfn_fnc;
    p_this->p_basic[3]       = p_argc;

    /* �����ж� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x80, 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��X,Y,Z���tap�ж� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, 0x0E, 1, 3, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ����tap��ֵ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_THS_6D, tap_threshold, 1, 5, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��������ʱ�� */
    int_dur2 = (dur_time << 4) | (quiet_time << 2) | shock_time;
    ret = __lsm6dsl_write(p_this, __LSM6DSL_INT_DUR2, &int_dur2, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ��˫��ѡ�� */
    if (one_or_two) {
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_THS, 0x80, 7, 1, 1);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /* �����жϴ򿪵�INT1�� */
    if (one_or_two == 0 || one_or_two == 2) {
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x40, 6, 1, 1);
        if (ret != AW_OK) {
            return ret;
        }
    }

    /* ��˫���жϵ�INT1�� */
    if (one_or_two == 1 || one_or_two == 2) {
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x08, 3, 1, 1);
        if (ret != AW_OK) {
            return ret;
        }
    }

    ret = aw_gpio_trigger_on(p_devinfo->int1_pin);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/**
 * \brief ���û�Ծ�ͷǻ�Ծ��⹦�ܺ���
 */
aw_err_t awbl_lsm6dsl_activity_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                   void                   *p_argc,
                                   uint8_t                 act_threshold,
                                   uint8_t                 sleep_time,
                                   uint8_t                 gyro_mode)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);
    awbl_lsm6dsl_devinfo_t *p_devinfo = __MY_DEVINFO_GET(p_dev);

    aw_err_t ret = AW_OK;

    uint8_t tap_cfg = 0;

    if (p_this == NULL || pfn_fnc == NULL) {
        return -AW_EINVAL;
    }

    if (act_threshold < 1 || act_threshold > 63 ||
            sleep_time < 1 || sleep_time > 15 ||
            gyro_mode < 0 || gyro_mode > 2) {
        return -AW_EINVAL;
    }

    p_this->pfn_basic_fnc[4] = pfn_fnc;
    p_this->p_basic[4]       = p_argc;

    /* ����˯��ʱ�� */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_DUR, sleep_time, 0, 4, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ������ֵ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_WAKE_UP_THS, act_threshold, 0, 6, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* �����жϺ����������� */
    if (gyro_mode == 0) {
        tap_cfg = 0xA0;
    } else if (gyro_mode == 1) {
        tap_cfg = 0xC0;
    } else if (gyro_mode == 2) {
        tap_cfg = 0xE0;
    }
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_TAP_CFG, tap_cfg, 5, 3, 1);
    if (ret != AW_OK) {
        return ret;
    }

    /* ·�ɵ�INT1������ */
    ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x80, 7, 1, 1);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_gpio_trigger_on(p_devinfo->int1_pin);
    if (ret != AW_OK) {
        return ret;
    }

    return ret;
}

/**
 * \brief �ر��������壬���ѣ�6D/4D����˫������Ծ�ͷǻ�Ծ���
 */
aw_err_t awbl_lsm6dsl_special_func_off (awbl_lsm6dsl_special_func_t who_off)
{
    awbl_dev_t *p_dev = awbl_dev_find_by_name(AWBL_LSM6DSL_NAME, 0);
    awbl_lsm6dsl_dev_t     *p_this    = __MY_GET_DEV(p_dev);

    aw_err_t ret = AW_OK;

    if (p_this == NULL) {
        return -AW_EINVAL;
    }

    switch (who_off) {
    case LSM6DSL_FREE_FALL:
        p_this->pfn_basic_fnc[0] = NULL;
        p_this->p_basic[0]       = NULL;
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, (uint8_t)who_off, 4, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        break;
    case LSM6DSL_WAKE_UP:
        p_this->pfn_basic_fnc[1] = NULL;
        p_this->p_basic[1]       = NULL;
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, (uint8_t)who_off, 5, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        break;
    case LSM6DSL_6D_4D:
        p_this->pfn_basic_fnc[2] = NULL;
        p_this->p_basic[2]       = NULL;
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, (uint8_t)who_off, 2, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        break;
    case LSM6DSL_TAP:
        p_this->pfn_basic_fnc[3] = NULL;
        p_this->p_basic[3]       = NULL;
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x40, 6, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, 0x08, 3, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        break;
    case LSM6DSL_ACTIVITY:
        p_this->pfn_basic_fnc[4] = NULL;
        p_this->p_basic[4]       = NULL;
        ret = __lsm6dsl_set_reg(p_this, __LSM6DSL_MD1_CFG, (uint8_t)who_off, 7, 1, 0);
        if (ret != AW_OK) {
            return ret;
        }
        break;
    default:
        return -AW_EINVAL;
    }

    return ret;
}


/* end of file */

