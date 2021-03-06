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
 * \brief 传感器操作接口
 *
 * 使用本服务需要包含头文件 aw_sensor.h
 *
 * 在一个系统中，可能存在多种类型的传感器，例如：温度、湿度、电流、压强等。同种
 * 类型的传感器也可能存在多个，例如，可能连接10个温度传感器以测试10个温度检测点
 * 的温度。特别地，部分传感器可以采集多路信号，例如，温湿度传感器SHT11可以采集
 * 温度和湿度。
 *
 * 为便于描述，使用传感器通道来表示一路信号的采集，对于只能采集单一信号的传感器，
 * 每个传感器只能为系统提供一路传感器通道；对于可以采集多路信号的传感器（比如：
 * SHT11），则每个传感器可以为系统提供多路传感器通道。也可能存在多个同种传感器，
 * 以便为系统提供更多的传感器通道。
 *
 * 为了便于区分各个传感器通道，在AWorks中，为每个传感器通道分配了唯一 id，后续
 * 即可使用id作为相关接口的参数，指定要操作的通道。
 *
 * 假设系统的传感器资源如下：
 *
 * - 10路温度传感器通道
 * - 3路（常见的x,y,z）加速度传感器通道
 * - 3路（常见的x,y,z）角速度传感器通道
 * - 8路湿度传感器通道
 * - 1路压强传感器通道
 * - 2路光照传感器通道
 *
 * 则系统 id 的分配可能为：
 *
 * - 0  ~  9 ： 10路温度传感器通道
 * - 10 ~ 12 ： 3路（常见的x,y,z）加速度传感器通道
 * - 13 ~ 15 ： 3路（常见的x,y,z）角速度传感器通道
 * - 16 ~ 23 ： 8路湿度传感器通道
 * -   24    ： 1路压强传感器通道
 * - 25 ~ 26 ： 2路光照传感器通道
 *
 * 实际中，id 与具体硬件平台相关，同类型传感器通道的 id 不一定连续，用户应查看
 * SDK中的用户手册了解系统所有的传感器通道资源，以正确使用各个传感器通道。
 *
 * \par 使用示例1（一次获取单个通道的数据）
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  current;               // 用于存储获取到的电流值
 *     
 *     aw_sensor_enable(0);                    // 使能通道0
 
 *     // 获取通道0的值，假定通道0为电流传感器通道
 *     int ret = aw_sensor_data_get(0, &current);
 *     
 *     if (ret == AW_OK) {
 *         // 数据读取成功，可以将电流值通过调试串口打印出来，
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              current.val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(current.unit));
 *
 *         // 若浮点运算对系统效率影响不大，可以直接使用公式计算以基准单位A为
 *         // 单位的电流值：
 *         // float c = current.val * pow(10, current.unit);  // 电流值，单位：A
 *
 *    } else {
 *         // 数据获取失败
 *    }
 * \endcode
 *
 * \par 使用示例2（一次获取多个通道的数据）
 * \code
 *     #include "aw_sensor.h"
 *
 *     const int        sensor[4] = {0, 2, 3, 5}; // 假定应用需要使用4个通道
 *     aw_sensor_val_t  buf[4];                   // 存储4通道传感器数据的缓冲区
 *     
 *     // 使能通道，buf用于存储每一个通道使能的结果，并非传感器数据
 *     aw_sensor_group_enable(sensor, 4, buf);
 * 
 *     // 获取数据
 *     int ret = aw_sensor_group_data_get(sensor, 4, buf);
 *     
 *     if (ret == AW_OK) {
 *         // 数据读取成功，均为有效值，无需使用AW_SENSOR_VAL_IS_VALID()宏进行
 *         // 逐一判断。可以将电流值通过调试串口打印出来，
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              buf[0].val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(buf[0].unit));
 *
 *         // 若浮点运算对系统效率影响不大，可以直接使用公式计算以基准单位A为
 *         // 单位的电流值：
 *         // float c = buf[0].val * pow(10, buf[0].unit);   // 电流值，单位：A
 *
 *    // 存在数据读取失败的通道
 *    } else {
 *        int i;
 *        for (i = 0; i < 4; i++) {
 *            if (AW_SENSOR_VAL_IS_VALID(buf[i])) {
 *                // 该通道数据有效，可以正常使用
 *            } else {
 *                // 该通道数据无效，数据获取失败，失败原因可通过buf[i].val判断
 *            }
 *        }
 *    }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 2.00 18-05-09  tee, modified some interface, sensor type and attributes.
 * - 1.00 16-08-05  ebi, first implementation.
 * \endinternal
 */

#ifndef __AW_SENSOR_H
#define __AW_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sensor
 * \copydoc aw_sensor.h
 * @{
 */

/**
 * \name 传感器类型定义
 * @{
 */
#define AW_SENSOR_TYPE_VOLTAGE     (1)  /**< \brief 电压，单位: V             */
#define AW_SENSOR_TYPE_CURRENT     (2)  /**< \brief 电流，单位: A             */
#define AW_SENSOR_TYPE_TEMPERATURE (3)  /**< \brief 温度，单位: 摄氏度        */
#define AW_SENSOR_TYPE_PRESS       (4)  /**< \brief 压强，单位: 帕斯卡        */
#define AW_SENSOR_TYPE_LIGHT       (5)  /**< \brief 光照度，单位: 勒克斯      */
#define AW_SENSOR_TYPE_ALTITUDE    (6)  /**< \brief 海拔，单位: 米            */
#define AW_SENSOR_TYPE_DISTANCE    (7)  /**< \brief 距离，单位: 米            */

/** \brief 直径<=1um的微粒浓度，单位: ug/m^3 */
#define AW_SENSOR_TYPE_PM_1             (100)

/** \brief 直径<=2.5um的微粒浓度，单位: ug/m^3 */
#define AW_SENSOR_TYPE_PM_2_5           (101)

 /** \brief 直径<=10um的微粒浓度，单位: ug/m^3 */
#define AW_SENSOR_TYPE_PM_10            (102)

/** \brief 相对湿度，值为百分比，1表示 1% */
#define AW_SENSOR_TYPE_HUMIDITY         (103)

/** \brief 接近式传感器，二值信号，1 表示有接近物 */
#define AW_SENSOR_TYPE_PROXIMITY        (104)

/** \brief 方向，角度制 0/90/180/270 分别表示正北/正东/正南/正西 */
#define AW_SENSOR_TYPE_ORIENT           (105)

/** 
 * \brief 加速度，单位: m/s^2
 *
 * 部分传感器可以检测多个方向的加速度，例如：x,y,z 轴，这种情况下，将使用通道号
 * 区分不同方向的加速度，x，y，z 表示了不同的通道。
 */
#define AW_SENSOR_TYPE_ACCELEROMETER    (200)

/** 
 * \brief 角速度（陀螺仪测量），单位: rad/s
 *
 * 部分传感器可以检测多个方向的角速度，例如：x,y,z 轴，这种情况下，将使用通道号
 * 区分不同方向的角速度，x，y，z 表示了不同的通道。
 */
#define AW_SENSOR_TYPE_GYROSCOPE        (201)

/** 
 * \brief 磁感应强度，单位: 高斯
 *
 * 部分传感器可以检测多个方向的磁感应强度，例如：x,y,z 轴，这种情况下，将使用通
 * 道号区分不同方向的磁感应强度，x，y，z 表示了不同的通道。
 */
#define AW_SENSOR_TYPE_MAGNETIC         (202)

/** 
 * \brief 旋转矢量(Rotation Vector)，角度制。
 *
 * 旋转矢量通常分为相对于三个方向的旋转角度： x,y,z 轴，这种情况下，将使用通
 * 道号区分不同的方向。
 */
#define AW_SENSOR_TYPE_ROTATION_VECTOR  (203)
 
/** @} */

/**
 * \name 传感器触发模式定义
 * @{
 */
 
/** 
 * \brief 数据准备就绪触发
 */
#define AW_SENSOR_TRIGGER_DATA_READY   (1ul << 0)
 
/**
 * \brief 门限触发
 *
 * 当连续多次（次数达到 #AW_SENSOR_ATTR_DURATION_DATA 属性值）采样数据均高于
 * 上限（ #AW_SENSOR_ATTR_THRESHOLD_UPPER 属性值）或低于下限
 *（ #AW_SENSOR_ATTR_THRESHOLD_LOWER 属性值）时触发。
 */
#define AW_SENSOR_TRIGGER_THRESHOLD    (1ul << 1)

/** 
 * \brief 增量（斜率）触发
 *
 * 当连续多次（次数达到 #AW_SENSOR_ATTR_DURATION_SLOPE 属性值）采样数据相
 * 对于上一次采样数据的增量均超过增量门限（ #AW_SENSOR_ATTR_THRESHOLD_SLOPE ）时
 * 触发。
 */
#define AW_SENSOR_TRIGGER_SLOPE        (1ul << 2)

/** 
 * \brief 布尔类型的触发
 *
 * 如接近式传感器，被动红外传感器，这类传感器反馈的信号是二值信号，当值为真时
 *（非0）触发。
 */
#define AW_SENSOR_TRIGGER_BOOL         (1ul << 3)

/** @} */

/**
 * \name 传感器通道属性定义
 * @{
 */
 
/** \brief 采样频率，每秒采样的次数 */
#define AW_SENSOR_ATTR_SAMPLING_RATE          (1)

/** \brief 满量程值 */
#define AW_SENSOR_ATTR_FULL_SCALE             (2)

/** \brief 偏移值，用于静态校准传感器数据，传感器数据 = 实际采样值 + offset */
#define AW_SENSOR_ATTR_OFFSET                 (3)

/** \brief 下门限，用于门限触发模式，数据低于该值时触发 */
#define AW_SENSOR_ATTR_THRESHOLD_LOWER        (4)

/** \brief 上门限，用于门限触发模式，数据高于该值时触发 */
#define AW_SENSOR_ATTR_THRESHOLD_UPPER        (5)

/** \brief 增量门限，用于增量触发，数据变化过快（斜率过大），超过该值时触发 */
#define AW_SENSOR_ATTR_THRESHOLD_SLOPE        (6)

/**
 * \brief 采样数据持续次数，用于门限触发模式
 *
 * 当该值有效时，则表示连续多次（次数达到该属性值）采样数据均高于上限或低于下限
 * 时才触发。
 */
#define AW_SENSOR_ATTR_DURATION_DATA          (7)

/**
 * \brief 增量持续次数，用于增量触发模式
 *
 * 当该值有效时，则表示连续多次（次数达到该属性值）采样数据相对于上一次采样数据
 * 的增量均超过增量门限时才触发。
 */
#define AW_SENSOR_ATTR_DURATION_SLOPE         (8)

/** @} */


/**
 * \name 单位定义，用于传感器数据的 unit 参数
 *
 * \anchor table_unit
 *
 * \par 基本单位前缀表
 *
 * | 指数（10的幂） | 符号（Symbol） | 英文前缀 |  中文读音  |
 * | :------------: | :----------- : | :------: |:---------: |
 * |       24       |        Y       |   Yotta  |  尧（它）  |
 * |       21       |        Z       |   Zetta  |  泽（它）  |
 * |       18       |        E       |    Exa   | 艾（可萨） |
 * |       15       |        P       |   Peta   |  拍（它）  |
 * |       12       |        T       |   Tera   |  太（拉）  |
 * |        9       |        G       |   Giga   |  吉（咖）  |
 * |        6       |        M       |   Mega   |     兆     |
 * |        3       |        k       |   kilo   |     千     |
 * |        2       |        h       |   hecto  |     百     |
 * |        1       |        da      |   deca   |     十     |
 * |        0       |        -       |     -    |     -      |
 * |       -1       |        d       |   deci   |     分     |
 * |       -2       |        c       |   centi  |     厘     |
 * |       -3       |        m       |   milli  |     毫     |
 * |       -6       |        μ       |   micro  |     微     |
 * |       -9       |        n       |   nano   |  纳（诺）  |
 * |       -12      |        p       |   pico   |  皮（可）  |
 * |       -15      |        f       |   femto  | 飞（母托） |
 * |       -18      |        a       |   atto   |  阿（托）  |
 * |       -21      |        z       |   zepto  | 仄（普托） |
 * |       -24      |        y       |   yocto  | 幺（科托） |
 *
 * \note
 *  
 *  - 指数代表 10的幂，例如，24表示 10^24，指数通常以3为间隔，即：1000倍为间隔。
 *  - 符号Symbol，区分大小写。正指数的前缀符号中，多数符号为大写，唯代表 kilo 
 * 的符号 "k" 是小写，以区别于代表温度的符号：大写的 "K"。
 *  - 前缀代表在英文单词中，通常增加该前缀表示数量级，例如，长度单位米的英文单词
 * 为：meter，则千米对应的单词为：kilometer。
 *  - 中文读音项中，括号内的字在不致混淆的情况下可以省略。
 *  - 百（10^2）、十（10^1）、分（10 ^-1）、厘（10^-2）较少使用。
 * @{
 */

#define AW_SENSOR_UNIT_YOTTA      (24)       /**< \brief 尧（它），10^(24)    */
#define AW_SENSOR_UNIT_ZETTA      (21)       /**< \brief 泽（它），10^(21)    */
#define AW_SENSOR_UNIT_EXA        (18)       /**< \brief 艾（可萨），10^(18)  */
#define AW_SENSOR_UNIT_PETA       (15)       /**< \brief 拍（它），10^(15)    */
#define AW_SENSOR_UNIT_TERA       (12)       /**< \brief 太（拉），10^(12)    */
#define AW_SENSOR_UNIT_GIGA       (9)        /**< \brief 吉（咖），10^(9)     */
#define AW_SENSOR_UNIT_MEGA       (6)        /**< \brief 兆，10^(6)           */
#define AW_SENSOR_UNIT_KILO       (3)        /**< \brief 千，10^(3)           */
#define AW_SENSOR_UNIT_HECTO      (2)        /**< \brief 百，10^(2)           */
#define AW_SENSOR_UNIT_DECA       (1)        /**< \brief 十，10^(1)           */
#define AW_SENSOR_UNIT_BASE       (0)        /**< \brief 个，10^(0)，倍数为1  */
#define AW_SENSOR_UNIT_DECI       (-1)       /**< \brief 分，10^(-1)          */
#define AW_SENSOR_UNIT_CENTI      (-2)       /**< \brief 厘，10^(-2)          */
#define AW_SENSOR_UNIT_MILLI      (-3)       /**< \brief 毫，10^(-3)          */
#define AW_SENSOR_UNIT_MICRO      (-6)       /**< \brief 微，10^(-6)          */
#define AW_SENSOR_UNIT_NANO       (-9)       /**< \brief 纳（诺），10^(-9)    */
#define AW_SENSOR_UNIT_PICO       (-12)      /**< \brief 皮（可），10^(-12)   */
#define AW_SENSOR_UNIT_FEMTO      (-15)      /**< \brief 飞（母托），10^(-15) */
#define AW_SENSOR_UNIT_ATTO       (-18)      /**< \brief 阿（托），10^(-18)   */
#define AW_SENSOR_UNIT_ZEPTO      (-21)      /**< \brief 仄（普托），10^(-21) */
#define AW_SENSOR_UNIT_YOCTO      (-24)      /**< \brief 幺（科托），10^(-24) */

#define AW_SENSOR_UNIT_INVALID    (-32768)   /**< \brief 无效值特殊标记       */

/** @} */


/** \brief 判断传感器某一通道的数据是否有效，宏值为真时有效，否则，无效 */
#define AW_SENSOR_VAL_IS_VALID(data)    \
             ((data).unit != (int32_t)(AW_SENSOR_UNIT_INVALID))

/**
 * \brief 辅助函数，获取一个单位对应的前缀符号，如"M"（兆）、"k"（千）
 *
 * \param[in] unit : 单位，AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \return 符号字符串。若单位无效，则返回NULL；若unit为0(#AW_SENSOR_UNIT_BASE)
 * 则返回空字符串: ""。特别注意，函数返回的是一个字符串，而不是一个字符，因为
 * 部分单位的符号可能是两个字母，如："da"（十）。
 */
#define AW_SENSOR_UNIT_SYMBOL_GET(unit) aw_sensor_unit_symbol_get(unit)

/**
 * \brief 传感器数据类型
 *
 * 实际中，传感器数据可能为小数，为了避免影响系统性能，AWorks内核不直接使用浮点
 * 数。而是使用“整数” + “单位”来表示一个传感器数据。当实际传感器数据为小数时，只
 * 需使用更小的单位来表示数据，即可仅使用整数，完全避免小数。
 *
 * 例如，一个电压传感器采集到的电压为1.234V（电压的基本单位为V，具体每种传感器类
 * 型对应的基本单位在定义传感器类型宏时进行了说明），为了避免小数，将其单位缩小
 * 为 mV，即为：1234 mV。
 *
 * 这里使用了 val 和 unit 两个有符号数分别表示传感器数据的值的单位。
 *
 * - val  : 传感器数据值，有符号32位数。
 * - unit ：单位，如"M"（兆）、"k"（千）、"m"（毫）、"μ"（微）、 "p"（皮）等等。
 * 单位使用10的幂进行表示，如"M"表示10^6，则unit的值为6。通常情况下，unit为负数，
 * 以表示精确到小数点后几位。
 *
 * 若需将传感器数据统一为基本单位，如将电压数据全部统一为单位V，则可以通过如下公
 * 式完成：data = val * (10 ^ unit)。典型的几个例子如下表:
 *
 * |   data（真实值，电压: V） |    val    |   unit   |
 * | :-----------------------: | :-------: | :------: |
 * |           2.854           |  2854     |  -3(mV)  |
 * |           1.7             |  1700     |  -3(mV)  |
 * |           2.3433          |  2343300  |  -6(μV)  |
 * |           2888000         |  2888     |   3(kV)  |
 * |           0               |     0     |     0    |
 * |          -2888000         |  -2888    |   3(kV)  |
 * |          -2.3433          |  -2343300 |  -6(μV)  |
 * |          -2.854           |  -2854    |  -3(mV)  |
 * |          -1.7             |  -1700    |  -3(mV)  |
 *
 * \attention 当 unit 的值为 -32768 时，则表示该值为无效的传感器数据。在使用组
 * （group）相关接口操作多个传感器通道时，若返回值不为AW_OK，则表示某些通道操作
 * 失败，失败的通道对应的传感器值将被标记为无效值（使用 AW_SENSOR_VAL_IS_VALID() 
 * 宏可以快速判断值是否有效），此时，val成员为标准错误码，可以据此判定失败的原因。
 */
typedef struct aw_sensor_val {

    /** 
     * \brief 传感器数值，有效范围：-2147483648 ~ 2147483647
     *
     * 特别地，当 unit 的值为 (#AW_SENSOR_UNIT_INVALID) 时，表示该值为无效的传感
     * 器的值。此时，val成员的值表示标准错误码，以辅助用户判断操作失败的原因。
     * 例如，-AW_ENODEV 表示传感器通道id不存在（无对应传感器设备）
     */
    int32_t   val;
 
    /** 
     * \brief 单位
     *
     * 使用指数（10的幂）表示，例如：6表示10^6，即："M"（兆）；3表示10^3，
     * 即："k"（千）；-3表示10^-3，即："m"（毫）。
     *
     * 不建议直接使用数字，可以使用已经定义好的单位宏：AW_SENSOR_UNIT_* 
     * (eg: #AW_SENSOR_UNIT_MILLI)
     *
     * 特别注意，当值为 (#AW_SENSOR_UNIT_INVALID) 时，表示该值为无效的传感器的值。
     * 此时，val成员的值表示标准错误码，以辅助用户判断操作失败的原因。
     *
     * 通常情况下，单位从侧面反映了一个传感器的精度，因此，若一个传感器的量程不
     * 变，则该传感器数据的的单位一般也不会改变。
     */
    int32_t   unit;

} aw_sensor_val_t;

/** 
 * \brief 触发器回调函数类型
 *
 * \param[in] p_arg       : 用户设置回调函数时指定的用户参数
 * \param[in] trigger_src : 触发源，AW_SENSOR_TRIGGER_*，可能是多个宏的或值。
 *
 * \return 无
 */
typedef void (*aw_sensor_trigger_cb_t) (void *p_arg, uint32_t trigger_src);

/**
 * \brief 获取某一传感器通道的类型，例如：温度、湿度、压强等
 *
 * 传感器通道有类型（例如：温度、湿度、压强等）之分，不同通道可能具有不同的类型，
 * 使用该接口可以快速获取某一传感器通道的类型。
 *
 * \param[in] id : 传感器通道 id
 * 
 * \retval   >=0      传感器类型，AW_SENSOR_TYPE_*(eg: #AW_SENSOR_TYPE_VOLTAGE)
 * \retval -AW_ENODEV 通道不存在
 */
aw_err_t aw_sensor_type_get(int id);

/**
 * \brief 辅助函数，获取一个单位对应的前缀符号，如"M"（兆）、"k"（千）
 *
 * \param[in] unit : 单位，AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \return 符号字符串。若单位无效，则返回NULL；若unit为0(#AW_SENSOR_UNIT_BASE)
 * 则返回空字符串: ""。特别注意，函数返回的是一个字符串，而不是一个字符，因为
 * 部分单位的符号可能是两个字母，如："da"（十）。
 */
const char * aw_sensor_unit_symbol_get(int32_t unit);

/**
 * \brief 辅助函数，用于传感器数据单位的转换
 *
 * \param[in] p_val   : 传感器数据缓存
 * \param[in] num     : 传感器数据个数
 * \param[in] to_unit : 目标单位，AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \retval AW_OK      转换成功
 * \retval -AW_EINVAL 转换失败，参数错误
 * \retval -AW_ERANGE 某一数据转换失败，其转换结果会超出范围（仅在缩小单位时可能
 *                    出现）,转换失败的数据将保持原值和原单位不变，用户可通过单
 *                    位是否为目标单位来判断是否转换成功。
 *
 * \note 单位转换的原理
 *
 * - 若是扩大单位（增加unit的值），假定unit增加的值为n，则会将val的值整除10^n，
 * 由于存在整除，将会使原val值的精度减小。精度减小时，遵循四舍五入法则。例如：
 * 原数据为1860mV，若将单位转换为V，则转换的结果为2V；原数据为1245mV，若将单位
 * 转换为V，则转换的结果为1V。由于存在精度的损失，单位的扩大应该谨慎使用。
 *
 * - 若是缩小单位（减小unit的值），假定unit减小的值为n，则会将val的值乘以10^n，
 * 但应特别注意，val值的类型为32位有符号数，其表示的数据范围为：-2147483648 ~ 
 * 2147483647。不应使val值扩大10^n后的数据超过该范围。缩小单位不存在精度的损失，
 * 但应注意数据的溢出，不应将一个数据缩小至太小的单位。
 * 
 * - 特别地，若转换前后的单位没有发生变化，则整个传感器的值保持不变。
 */
aw_err_t aw_sensor_val_unit_convert (aw_sensor_val_t *p_buf, int num, int32_t to_unit);

/**
 * \brief 获取某一传感器通道的数据
 *
 * \param[in]  id    : 传感器通道 id 
 * \param[out] p_val : 传感器数据输出缓存
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENODEV  通道不存在
 * \retval -AW_EPERM   通道未使能，操作不允许
 *
 * \note
 *
 * - 若传感器使用默认采样频率（属性值： #AW_SENSOR_ATTR_SAMPLING_RATE ）即：0，
 * 不自动采样，则每次调用该接口读取传感器数据都将立即通过传感器通信接口（I2C、
 * SPI等）来完成，比较耗时，因此，这种情况下，该函数不能在中断环境中使用。
 *
 * - 若修改传感器采样频率为非零值，此时，通道使能后，系统将自动按照采样频率获取
 * 传感器中的采样值，并存于内部缓存中。若用户设置了数据准备就绪触发，则每次数据
 * 采样结束后，都将触发用户回调函数。这种情况下，调用该接口将直接从内部缓存中
 * 获取数据，效率很高，可以在中断中使用。但应避免耗时较长的处理，影响下一次自动
 * 采集。
 *
 * - 实际中，部分传感器可能可以提供多通道传感器数据，例如，温湿度传感器SHT11可以
 * 同时提供一路温度和一路湿度数据。为了提高获取数据的效率，当应用程序需要获取多
 * 通道的传感器数据时，建议使用  aw_sensor_group_data_get() 函数一次性获取一组（
 * 多个通道）的值。而不是多次调用本函数，一次获取一个通道的值。
 *
 * \par 使用示例
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  current;               // 用于存储获取到的电流值
 *     
 *     // 获取通道0的值，假定通道0为电流传感器通道
 *     int ret = aw_sensor_data_get(0, &current);
 *     
 *     if (ret == AW_OK) {
 *         // 数据读取成功，可以将电流值通过调试串口打印出来，
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              current.val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(current.unit));
 *
 *         // 若浮点运算对系统效率影响不大，可以直接使用公式计算以基准单位A为
 *         // 单位的电流值：
 *         // float c = current.val * pow(10, current.unit);  // 电流值，单位：A
 *
 *    } else {
 *         // 数据获取失败
 *    }
 * \endcode
 */
aw_err_t aw_sensor_data_get(int id, aw_sensor_val_t *p_val);

/**
 * \brief 获取一组传感器通道（多个通道）的数据
 *
 * 实际应用中，部分传感器可以提供多通道传感器数据，例如，温湿度传感器SHT11，其可
 * 以同时提供一路温度和一路湿度数据。基于此，为了提高获取数据的效率，当应用程序
 * 需要获取多通道的传感器数据时，建议使用本函数一次性获取多个通道的值。
 * 
 * \param[in]  p_ids ：传感器通道 id 列表
 * \param[in]  num   : 通道数目
 * \param[out] p_buf ：数据输出缓冲区，缓冲区大小应该与 num 一致
 *
 * \note 本函数将获取 start_id ~ (start_id + num - 1) 所有传感器通道的数据。 若 
 *       id 为 2, num 为10， 则获取数据的通道范围为： 2 ~ 11。
 *
 * \return  AW_OK  数据读取成功，获取的所有传感器数据均有效
 * \retval   <0    标准错误码，数据读取失败，部分或所有通道数据读取失败
 *
 * \attention 当返回值不为AW_OK时，表示某些通道数据读取失败，读取失败的通道对应
 *            的传感器数据将为无效值，可以使用 AW_SENSOR_VAL_IS_VALID()宏判断哪
 *            些通道的数据读取失败了，若某一通道对应的数据为无效值，则传感器数据
 *            中的 val 值表示了标准错误码，用户可以据此判断该通道失败的原因，
 *            失败原因与标准错误码的对应关系与使用 aw_sensor_data_get() 读取单通
 *            道数据的返回值含义一致。
 *
 * \par 使用示例
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  buf[4];                 // 存储4个通道传感器数据的缓冲区
 *     
 *     // 获取通道 0 ~ 通道3 的数据，起始通道号：0，通道数目4
 *     int ret = aw_sensor_data_get(0, 4, buf);
 *     
 *     if (ret == AW_OK) {
 *         // 数据读取成功，均为有效值，无需使用AW_SENSOR_VAL_IS_VALID()宏进行
 *         // 逐一判断。可以将电流值通过调试串口打印出来，
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              buf[0].val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(buf[0].unit));
 *
 *         // 若浮点运算对系统效率影响不大，可以直接使用公式计算以基准单位A为
 *         // 单位的电流值：
 *         // float c = buf[0].val * pow(10, buf[0].unit);   // 电流值，单位：A
 *
 *    // 存在数据读取失败的通道
 *    } else {
 *        int i;
 *        for (i = 0; i < 4; i++) {
 *            if (AW_SENSOR_VAL_IS_VALID(buf[i])) {
 *                // 该通道数据有效，可以正常使用
 *            } else {
 *                // 该通道数据无效，数据获取失败，失败原因可通过buf[i].val判断
 *            }
 *        }
 *    }
 * \endcode
 */
aw_err_t aw_sensor_group_data_get (const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_buf);

/**
 * \brief 使能单个传感器通道
 * 
 * 使能后，传感器被激活，传感器开始采集外部信号。若设置了传感器的采样频率属性值
 * （#AW_SENSOR_ATTR_SAMPLING_RATE），且值不为0，则系统将自动按照属性值指定的采
 * 样频率获取传感器中的采样值。若用户打开了数据准备就绪触发，则每次数据采样结束
 * 后，都将触发用户回调函数。特别地，若采样频率为0（默认），则不自动采样，每次调
 * 用aw_sensor_data_get()函数时才从传感器中获取最新数据。
 * 
 * \param[in]  id  : 传感器通道 id 
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENODEV  通道不存在
 */
aw_err_t aw_sensor_enable(int id);

/**
 * \brief 使能一组（多个通道）传感器通道
 * 
 * \param[in] p_ids    ：传感器通道 id 列表
 * \param[in] num      ：传感器通道数目
 * \param[in] p_result ：用于存储每一个通道使能的结果，结果存于 val 值中，
 *                       与标准错误码含义一致。
 *
 * \retval  AW_OK  所有通道使能成功
 * \retval   <0    部分或所有通道使能失败，每一个通道使能的结果存于 val 值中
 *
 * \attention p_result 指向的缓存用于存储每个通道使能的结果，它们表示状态，并非
 *            有效的传感器数据，使用 AW_SENSOR_VAL_IS_VALID()宏进行有效性判定时，
 *            都会返回假，因此，没有必要使用AW_SENSOR_VAL_IS_VALID()宏进行判断。
 *            仅需通过 val 值进行判断即可，val 值的含义与标准错误码的对应关系与
 *            使用aw_sensor_enable() 使能单通道的返回值含义一致。特别地，若
 *            p_result 为NULL，则表明不关心每个通道使能的结果。
 */
aw_err_t aw_sensor_group_enable (const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result);

/**
 * \brief 禁能单个传感器通道
 * 
 * 禁能后，传感器被关闭，传感器将停止采集外部信号，同时，系统也不再从传感器中获
 * 取采样值（无论采样频率是否为0）。
 *
 * \param[in]  id    : 传感器通道 id 
 * \param[out] p_val : 传感器数据输出缓存
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENODEV  通道不存在
 * \retval -AW_EPERM   通道尚未使能，操作不允许
 */
aw_err_t aw_sensor_disable(int id);

/**
 * \brief 禁能一组（多个通道）传感器通道
 * 
 * \param[in] p_ids    ：传感器通道 id 列表
 * \param[in] num      ：传感器通道数目
 * \param[in] p_result ：用于存储每一个通道禁能的结果，结果存于 val 值中，
 *                       与标准错误码含义一致。
 *
 * \return  AW_OK  所有通道禁能成功
 * \retval   < 0   标准错误码，部分或所有通道禁能失败
 *
 * \attention p_result 指向的缓存用于存储每个通道禁能的结果，它们表示状态，并非
 *            有效的传感器数据，使用 AW_SENSOR_VAL_IS_VALID()宏进行有效性判定时，
 *            都会返回假，因此，没有必要使用 AW_SENSOR_VAL_IS_VALID()宏进行判断。
 *            仅需通过 val 值进行判断即可，val 值的含义与标准错误码的对应关系与
 *            使用 aw_sensor_disable() 禁能单通道的返回值含义一致。特别地，若
 *            p_result 为NULL，则表明不关心每个通道禁能的结果。
 */
aw_err_t aw_sensor_group_disable (const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_result);

/**
 * \brief 配置传感器通道属性
 *
 * \param[in] id    : 传感器通道 id
 * \param[in] attr  : 属性，AW_SENSOR_ATTR_*(eg:#AW_SENSOR_ATTR_SAMPLING_RATE)
 * \param[in] p_val : 属性值
 *
 * \retval   AW_OK       成功
 * \retval  -AW_EINVAL   参数错误
 * \retval  -AW_ENOTSUP  不支持
 * \retval  -AW_ENODEV   通道不存在
 * 
 * \note 支持的属性与具体传感器相关，部分传感器可能不支持任何属性。该函数不
 *       能在中断环境中使用。
 */
aw_err_t aw_sensor_attr_set (int id, int attr, const aw_sensor_val_t *p_val);

/**
 * \brief 获取传感器通道属性
 *
 * \param[in]  id    : 传感器通道 id
 * \param[in]  attr  : 属性，AW_SENSOR_ATTR_*(eg:#AW_SENSOR_ATTR_SAMPLING_RATE)
 * \param[out] p_val : 用于获取属性值的缓存
 *
 * \retval   AW_OK       成功
 * \retval  -AW_EINVAL   参数错误
 * \retval  -AW_ENOTSUP  不支持
 * \retval  -AW_ENODEV   通道不存在
 * 
 * \note 支持的属性与具体传感器相关，部分传感器可能不支持任何属性。该函数不
 *       能在中断环境中使用。
 */
aw_err_t aw_sensor_attr_get (int id, int attr, aw_sensor_val_t *p_val);

/**
 * \brief 设置触发，一个通道仅能设置一个触发回调函数
 *
 * \param[in] id     : 传感器通道 id 
 * \param[in] flags  : 触发标志，指定触发的类型
 * \param[in] pfn_cb : 触发时执行的回调函数的指针
 * \param[in] p_arg  : 用户参数
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  参数错误
 * \retval -AW_ENOTSUP 不支持的触发模式
 * \retval -AW_ENODEV  通道不存在
 *
 * \note 支持的触发模式与具体传感器相关，部分传感器不支持任何触发模式。配置模式
 *       时往往需要通过I2C、SPI等通信接口与传感器通信，比较耗时，该函数不能在中
 *       断环境中使用。
 *
 * \par 使用示例
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_local void __sensor_trigger_cb (void *p_arg, uint32_t trigger_src)
 *     {
 *         if (trigger_src & AW_SENSOR_TRIGGER_THRESHOLD) {  // 数据超过门限
 *             aw_buzzer_on();                               // 蜂鸣器报警
 *         }
 *     }
 *
 *     // 配置通道0的触发模式
 *     int ret = aw_sensor_trigger_cfg(0, 
 *                                     AW_SENSOR_TRIGGER_THRESHOLD,
 *                                     __sensor_trigger_cb,
 *                                     NULL);
 *     if (ret < 0) {
 *         // 设置失败，可能不支持该触发模式
 *     } else {
 *         // 设置成功，打开触发
 *         aw_sensor_trigger_on(0); 
 *     }
 * \endcode
 */
aw_err_t  aw_sensor_trigger_cfg (int                       id,
                                 uint32_t                  flags,
                                 aw_sensor_trigger_cb_t    pfn_cb,
                                 void                     *p_arg);
 

/**
 * \brief 打开触发
 * \param[in] id : 传感器通道 id 
 * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
 */
aw_err_t aw_sensor_trigger_on (int id);

/**
 * \brief 关闭触发
 * \param[in] id : 传感器通道 id 
 * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
 */
aw_err_t aw_sensor_trigger_off (int id);


/**
 * \brief 获取传感器类型字符串和单位字符串
 */
aw_err_t aw_sensor_type_name_get (int id,  const char **name,  const char **unit);

/** @} grp_aw_if_sensor */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SENSOR_H */

/* end of file */
