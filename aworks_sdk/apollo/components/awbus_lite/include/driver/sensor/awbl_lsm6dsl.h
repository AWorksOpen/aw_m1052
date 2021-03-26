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
 * \brief LSM6DSL (三轴加速度和三轴陀螺仪传感器) 头文件
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-3  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_LSM6DSL_H
#define __AWBL_LSM6DSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"

#define AWBL_LSM6DSL_NAME      "awbl_lsm6dsl"

/**
 * \breif 检测自由落体的阈值选择
 *
 * 例如：若选择156mg，则表示加速度X，Y，Z轴三轴加速度都在±156mg范围时，则达到触
 * 发阈值条件
 */
typedef enum awbl_lsm6dsl_free_fall {
    LSM6DSL_FREE_FALL_THRESHOLD_156MG,  /**< \breif 自由落体检测阈值为±156mg */
    LSM6DSL_FREE_FALL_THRESHOLD_219MG,  /**< \breif 自由落体检测阈值为±219mg */
    LSM6DSL_FREE_FALL_THRESHOLD_250MG,  /**< \breif 自由落体检测阈值为±250mg */
    LSM6DSL_FREE_FALL_THRESHOLD_312MG,  /**< \breif 自由落体检测阈值为±312mg */
    LSM6DSL_FREE_FALL_THRESHOLD_344MG,  /**< \breif 自由落体检测阈值为±344mg */
    LSM6DSL_FREE_FALL_THRESHOLD_406MG,  /**< \breif 自由落体检测阈值为±406mg */
    LSM6DSL_FREE_FALL_THRESHOLD_469MG,  /**< \breif 自由落体检测阈值为±469mg */
    LSM6DSL_FREE_FALL_THRESHOLD_500MG   /**< \breif 自由落体检测阈值为±500mg */
} awbl_lsm6dsl_free_fall_t;

/**
 * \breif 自由落体，唤醒，6D/4D，单双击，活跃和非活跃选择
 */
typedef enum awbl_lsm6dsl_special_func {
    LSM6DSL_ACTIVITY  = 0x80,           /**< \breif 活跃和非活跃事件 */
    LSM6DSL_TAP       = 0x48,           /**< \breif 单击或双击事件 */
    LSM6DSL_WAKE_UP   = 0x20,           /**< \breif 唤醒事件*/
    LSM6DSL_FREE_FALL = 0x10,           /**< \breif 自由落体事件 */
    LSM6DSL_6D_4D     = 0x04,           /**< \breif 6D/4D事件 */
} awbl_lsm6dsl_special_func_t;

/**
 * \breif 唤醒事件触发时，触发轴的获取
 */
enum awbl_lsm6dsl_wake_up {
    LSM6DSL_WAKE_UP_X,                  /**< \breif 唤醒事件发生在X轴 */
    LSM6DSL_WAKE_UP_Y,                  /**< \breif 唤醒事件发生在Y轴 */
    LSM6DSL_WAKE_UP_Z,                  /**< \breif 唤醒事件发生在Z轴 */
};

/**
 * \breif 6D/4D事件触发时，当前方位获取
 */
enum awbl_lsm6dsl_6d_4d {
    LSM6DSL_6D_4D_XH,                   /**< \breif 当前位置X轴正方向朝上 */
    LSM6DSL_6D_4D_XL,                   /**< \breif 当前位置X轴负方向朝上 */
    LSM6DSL_6D_4D_YH,                   /**< \breif 当前位置Y轴正方向朝上 */
    LSM6DSL_6D_4D_YL,                   /**< \breif 当前位置Y轴负方向朝上 */
    LSM6DSL_6D_4D_ZH,                   /**< \breif 当前位置Z轴正方向朝上 */
    LSM6DSL_6D_4D_ZL                    /**< \breif 当前位置Z轴负方向朝上 */
};

/**
 * \breif 单双击事件触发时，触发状态的获取
 */
enum awbl_lsm6dsl_tap {
    LSM6DSL_SINGLE_TAP = (1ul << 0),    /**< \breif 发生了单击事件 */
    LSM6DSL_DOUBLE_TAP = (1ul << 1),    /**< \breif 发生了双击事件 */
    LSM6DSL_XH_TAP     = (1ul << 2),    /**< \breif X正轴发生了单击或双击事件 */
    LSM6DSL_XL_TAP     = (1ul << 3),    /**< \breif X负轴发生了单击或双击事件 */
    LSM6DSL_YH_TAP     = (1ul << 4),    /**< \breif Y正轴发生了单击或双击事件 */
    LSM6DSL_YL_TAP     = (1ul << 5),    /**< \breif Y负轴发生了单击或双击事件 */
    LSM6DSL_ZH_TAP     = (1ul << 6),    /**< \breif Z正轴发生了单击或双击事件 */
    LSM6DSL_ZL_TAP     = (1ul << 7)     /**< \breif Z负轴发生了单击或双击事件*/
};

/**
 * \breif 活跃和非活跃事件状态获取
 */
enum awbl_lsm6dsl_activity {
    LSM6DSL_ACTIVITING = (1ul << 0),    /**< \breif 当前发生活跃事件，进入活跃状态 */
    LSM6DSL_INACTIVITY = (1ul << 1),    /**< \breif 进入非活跃状态 */
};

/**
 * \brief LSM6DSL 设备信息结构体
 */
typedef struct awbl_lsm6dsl_devinfo {

    /**
     * \brief 该传感器通道的起始id
     *
     * LSM6DSL 共有7路采集通道，三轴加速度的X，Y，Z轴和三轴陀螺仪的X，Y，Z轴和一
	 * 路温度数据采集通道，从start_id开始(包括start_id)连续七个ID分配给该模块的
	 * 七个通道，其中start_id 对应模块三轴加速度X轴的ID，start_id + 1 对应模块三
	 * 轴加速度Y轴的ID，以此依此编号
     * eg： 将start_id配置为0, 则ID = 0,对应模块的通道1，ID = 1对应模块的通道2.
     */
    int     start_id;

    int     int1_pin;  /*< \brief 传感器外部触发引脚1 */
    int     int2_pin;  /*< \brief 传感器外部触发引脚2 */
    uint8_t i2c_addr;  /*< \brief I2C 7位 设备地址 */

    /** \brief 平台初始化回调函数 */
    void    (*pfunc_plfm_init)(void);

    /**
     * \brief 三轴加速度高性能模式是否打开(打开：1，关闭：0)
     *
     * 若打开，则三轴加速度在12.5HZ及以上的可选频率下采集数据均使用高性能模式
	 * （功耗为150-160uA）。
     * 若关闭，则当频率在1.6HZ，12.5HZ，26HZ，52HZ时，采用低功耗模式
	 * （功耗为4.5uA-25uA）；
     * 当频率为104HZ和208HZ时，采用正常模式（功耗为44uA-85uA）；当频率高于416HZ
	 * 时，自动启用高性能模式（功耗为150uA-160uA）。
     */
    uint8_t xl_hm_mode;

    /**
     * \brief 三轴陀螺仪高性能模式是否打开(打开：1，关闭：0)
     *
     * 若打开，则三轴陀螺仪在所有可选频率下采集数据均使用高性能模式（功耗为555uA）。
     * 若关闭，则当频率在12.5HZ，26HZ，52HZ时，采用低功耗模式（功耗为232uA-270uA）；
     * 当频率为104HZ和208HZ时，采用正常模式（功耗为325uA-430uA）；当频率高于
	 * 416HZ时，自动启用高性能模式（功耗为555uA）。
     */
    uint8_t g_hm_mode;

    /**
     * \brief 三轴加速度补偿因子选择(传入1或0)
     *
     * 若传入0，则表示偏移属性的补偿值分辨率为(1/1024)g，最大补偿范围为
	 * (-0.124023g~0.124023g)
     * 若传入1，则表示偏移属性的补偿值分辨率为(1/64)g，最大补偿范围为
	 * (-1.984375g~1.984375g)
     */
    uint8_t xl_compensate;

} awbl_lsm6dsl_devinfo_t;

/**
 * \brief LSM6DSL 设备结构体
 */
typedef struct awbl_lsm6dsl_dev {

    /** \brief 继承自 IIC 设备 */
    struct awbl_i2c_device               dev;

    /** \brief sensor 服务  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief 中断延时处理任务 */
    struct aw_isr_defer_job              g_myjob[2];

    /** \brief 三轴加速度和陀螺仪和温度采样频率，每秒采样的次数 */
    aw_sensor_val_t                      sampling_rate[3];

    /** \brief 三轴加速度和陀螺仪当前的量程 */
    aw_sensor_val_t                      full_scale[2];

    /** \brief 打开和关闭通道的对应标志位 */
    uint8_t                              en_or_dis;

    /** \brief 触发使能位，每位对应一个通道 */
    uint8_t                              on_or_off;

    /** \brief 通道触发回调函数 */
    aw_sensor_trigger_cb_t               pfn_trigger_fnc[7];

    /** \brief 通道触发回调函数参数 */
    void                                *p_argc[7];

    /** \brief 通道触发标志*/
    uint32_t                             flags[7];

    /** \brief 加速度偏移量保存值 */
    aw_sensor_val_t                      xl_off_data[3];

    /** \brief 特殊功能事件回调函数 */
    aw_sensor_trigger_cb_t               pfn_basic_fnc[5];

    /** \brief 特殊功能事件回调函数参数 */
    void                                *p_basic[5];

} awbl_lsm6dsl_dev_t;

/**
 * \brief 注册 LSM6DSL 驱动
 *
 * 该函数无需用户调用
 */
void awbl_lsm6dsl_drv_register (void);

/**
 * \brief 启用自由落体功能检测函数
 *
 * 当加速度X，Y，Z轴三轴加速度都在阈值范围范围内，且达到持续时间时，则触发自由落
 * 体事件
 *
 * \param pfn_fnc      发生自由落体事件时，执行的用户回调函数
 * \param p_argc       发生自由落体事件时，执行的用户回调函数参数
 * \param ff_threshold 自由落体检测阈值选择
 * \param ff_time      填入数值1~63，自由落体超过阈值延迟检测时间
 *                      ——若设置加速度频率为12.5HZ，则该时间为(m_time/12.5)s
 *                      ——若设置加速度频率为26HZ，则该时间为(m_time/26)s
 *                      ...
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval  < 0        失败，对应标准错误码
 */
aw_err_t awbl_lsm6dsl_free_fall_on (aw_sensor_trigger_cb_t    pfn_fnc,
                                    void                     *p_argc,
                                    awbl_lsm6dsl_free_fall_t  ff_threshold,
                                    uint8_t                   ff_time);

/**
 * \brief 启用唤醒事件功能检测函数
 *
 * 当加速度的本次数据与上一次数据增量的一半超过正阈值或低于负阈值，并且达到唤醒
 * 持续时间时，则触发唤醒事件
 *
 * \param pfn_fnc      发生唤醒事件时，执行的用户回调函数
 * \param p_argc       发生唤醒事件时，执行的用户回调函数参数
 * \param wu_threshold 填入数值1~63，唤醒事件检测阈值选择
 *                      ——若加速度的量程为±2g，则该阈值为±(ff_threshold*(2/64))g
 *                      ——若加速度的量程为±4g，则该阈值为±(ff_threshold*(4/64))g
 *                      ...
 * \param wu_time      填入数值0~3，唤醒事件超过阈值延迟检测时间
 *                       ——若设置加速度频率为12.5HZ，则该时间为(wu_time/12.5)s
 *                       ——若设置加速度频率为26HZ，则该时间为(wu_time/26)s
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval  < 0        失败，对应标准错误码
 */
aw_err_t awbl_lsm6dsl_wake_up_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                  void                   *p_argc,
                                  uint8_t                 wu_threshold,
                                  uint8_t                 wu_time);

/**
 * \brief 启用6D/4D方位检测功能函数
 *
 * 当传感器发生方位变化，且方位变化的角度超过阈值时，则触发回调函数，获得传感器
 * 的方位信息
 *
 * \param pfn_fnc      发生方位变化事件时，执行的用户回调函数
 * \param p_argc       发生方位变化事件时，执行的用户回调函数参数
 * \param d_threshold  填入数值0~3，方位变化角度阈值选择
 *                      ——若为0，则表示方位变化角度阈值为80度
 *                      ——若为1，则表示方位变化角度阈值为70度
 *                      ——若为2，则表示方位变化角度阈值为60度
 *                      ——若为3，则表示方位变化角度阈值为50度
 * \param d6_or_d4     填入数值0或1，6D方位检测或者4D方位检测选择
 *                      ——若为0，则表示采用6D方位检测
 *                      ——若为1，则表示采用4D方位检测
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval  < 0        失败，对应标准错误码
 *
 * \note 4D方向检测是6D方向检测的子集。在4D方位检测中，Z轴的位置检测(ZH和ZL)将被
 *       禁用
 */
aw_err_t awbl_lsm6dsl_6d_4d_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                void                   *p_argc,
                                uint8_t                 d_threshold,
                                uint8_t                 d6_or_d4);

/**
 * \brief 启用单双击事件检测功能函数
 *
 * \param pfn_fnc        产生单双击事件时，执行的用户回调函数
 * \param p_argc         产生单双击事件时，执行的用户回调函数参数
 * \param tap_threshold  填入数值1~31，单双击事件的阈值选择
 *                        ——若加速度的量程为±2g，则该阈值为±(tap_threshold*(2/32))g
 *                        ——若加速度的量程为±4g，则该阈值为±(tap_threshold*(4/32))g
 *                        ...
 * \param shock_time     填入数值1~3，单双击事件的感受震动持续时间
 *                        ——若加速度频率为416HZ，则该时间为(shock_time*8/416)s
 *                        ——若加速度频率为833HZ，则该时间为(shock_time*8/833)s
 * \param quiet_time     填入数值1~3，双击事件的安静时间
 *                        ——若加速度频率为416HZ，则该时间为(quiet_time*4/416)s
 *                        ——若加速度频率为833HZ，则该时间为(quiet_time*4/833)s
 * \param dur_time       填入数值1~15，双击事件的持续检测时间
 *                        ——若加速度频率为416HZ，则该时间为(dur_time*8/416)s
 *                        ——若加速度频率为833HZ，则该时间为(dur_time*8/833)s
 * \param one_or_two     填入数值0~2，单击事件启用或双击事件启用
 *                        ——若为0，则表示只启用单击事件
 *                        ——若为1，则表示只双击事件
 *                        ——若为2，则表示同时启用单双击事件
 *
 * \retval  AW_OK        成功
 * \retval -AW_EINVAL    参数错误
 * \retval -AW_EPERM     操作不允许
 * \retval  < 0          失败，对应标准错误码
 *
 * \note 1. 该功能只能在加速度频率为416HZ或833HZ时使用，所以在使用该功能前需修改
 *          加速度频率
 *       2. 若该功能与唤醒事件功能同时使用，则该功能设置的单双击阈值(mg)必须大于
 *          唤醒功能的阈值(mg)
 */
aw_err_t awbl_lsm6dsl_tap_on (aw_sensor_trigger_cb_t  pfn_fnc,
                              void                   *p_argc,
                              uint8_t                 tap_threshold,
                              uint8_t                 shock_time,
                              uint8_t                 quiet_time,
                              uint8_t                 dur_time,
                              uint8_t                 one_or_two);

/**
 * \brief 启用活跃和非活跃检测功能函数
 *
 * 该功能允许降低系统功耗，当进入非活跃状态时，加速度采样频率自动设置为12.5HZ，
 * 陀螺仪可以选择工作在
 * 三种方式下：1.该功能打开后陀螺仪的工作方式不发生变化
 *         2.该功能打开后陀螺仪进入睡眠模式
 *         3.该功能打开后陀螺仪进入低功耗模式
 * 当活跃状态断言时，加速度和陀螺仪恢复到原有的工作状态
 *
 * \param pfn_fnc        发生活跃和非活跃事件时，执行的用户回调函数
 * \param p_argc         发生活跃和非活跃事件时，执行的用户回调函数参数
 * \param act_threshold  填入数值1~63，活跃事件检测阈值选择(该阈值与唤醒事件的阈值相同)
 *                        ——若加速度的量程为±2g，则该阈值为±(ff_threshold*(2/64))g
 *                        ——若加速度的量程为±4g，则该阈值为±(ff_threshold*(4/64))g
 *                        ...
 * \param sleep_time     填入数值1~15，进入非活跃状态前的活跃状态检测持续时间
 *                        ——在该检测之前，若加速度频率为104HZ，则该时间为(sleep_time*512/104)s
 *                        ——在该检测之前，若加速度频率为208HZ，则该时间为(sleep_time*512/208)s
 *                        ...
 * \param gyro_mode      填入数值0~2，进入非活跃状态时的陀螺仪工作状态
 *                        ——若为0，则表示进入非活跃状态后，陀螺仪的工作方式不发生变化
 *                        ——若为1，则表示进入非活跃状态后，陀螺仪进入睡眠模式
 *                        ——若为2，则表示进入非活跃状态后，陀螺仪进入低功耗模式
 *
 * \retval  AW_OK        成功
 * \retval -AW_EINVAL    参数错误
 * \retval -AW_EPERM     操作不允许
 * \retval  < 0          失败，对应标准错误码
 */
aw_err_t awbl_lsm6dsl_activity_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                   void                   *p_argc,
                                   uint8_t                 act_threshold,
                                   uint8_t                 sleep_time,
                                   uint8_t                 gyro_mode);

/**
 * \brief 关闭自由落体，唤醒，6D/4D，单双击，活跃和非活跃检测
 *
 * \param   who_off  选择关闭的事件
 *
 * \retval  AW_OK     成功
 * \retval -AW_EINVAL 参数错误
 * \retval  < 0       失败，对应标准错误码
 */
aw_err_t awbl_lsm6dsl_special_func_off (awbl_lsm6dsl_special_func_t who_off);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LSM6DSL_H */

/* end of file */
