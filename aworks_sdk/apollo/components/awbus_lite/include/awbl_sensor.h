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
 * \brief 传感器设备管理
 *
 * \internal
 * \par modification history
 * - 1.00 18-05-15  tee, first implementation
 * \endinternal
 */
#ifndef __AWBL_SENSOR_H
#define __AWBL_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "apollo.h"
#include "awbus_lite.h"
#include "aw_sensor.h"
#include "aw_sem.h"

/**
 * \addtogroup grp_awbl_if_sensor
 * \copydoc awbl_sensor.h
 * @{
 */

/**
 * \brief 将传感器值设置为无效值（仅供器件驱动使用）
 *
 * 若某一通道存在问题，数据读取失败，则应使用该函数将对应缓存的值设置为无效值。
 *
 * \param[in] p_val : 指向要设置为无效值的数据
 * \param[in] errno : 标准错误码，
 *
 * \note 器件驱动使用该宏将某一传感器值设置为无效值时，错误码不能为  -AW_ENODEV，
 * 即通道不存在，该失败原因仅可在中间层使用。也就是说，器件驱动不应该判断通道是
 * 否存在，其仅操作该器件支持的几个通道即可，其操作的通道都应该是已经存在的。
 *
 * 若错误码为 -AW_ENODEV， 将强制转换为错误码：-AW_EIO。
 */
#define AWBL_SENSOR_VAL_SET_INVALID(p_val, errno)                      \
    do {                                                               \
        (p_val)->val  = ((errno == (-AW_ENODEV)) ? (-AW_EIO) : errno); \
        (p_val)->unit = AW_SENSOR_UNIT_INVALID;                        \
    } while(0)

/**
 * \brief 将传感器操作的状态存于 val 中（仅供器件驱动使用）
 *
 * 当使能或禁能多个通道时，传感器值用于存储当前通道操作的结果。
 *
 * \param[in] p_val  : 指向要设置为无效值的数据
 * \param[in] result : 本次操作的结果，典型的， AW_OK，成功； -AW_EIO，I/O错误。
 *
 *
 * \note 器件驱动使用该宏时，操作结果不能为  -AW_ENODEV，即通道不存在，
 * -AW_ENODEV 仅在中间层使用。也就是说，器件驱动不应该判断通道是否
 * 存在，其仅操作该器件支持的几个通道即可，其操作的通道都应该是已经存在的。
 *
 * 若错误码为 -AW_ENODEV， 将强制转换为错误码：-AW_EIO。
 */
#define AWBL_SENSOR_VAL_SET_RESULT(p_val, result)                        \
    do {                                                                 \
        (p_val)->val  = ((result == (-AW_ENODEV)) ? (-AW_EIO) : result); \
        (p_val)->unit = AW_SENSOR_UNIT_INVALID;                          \
    } while(0)


struct awbl_sensor_servfuncs {

    /**
     * \brief 从指定通道获取采样数据
     *
     * \param[in]  p_cookie : 驱动自定义参数
     * \param[in]  p_ids    ：传感器通道 id 列表， p_id[0]必然属于该设备
     * \param[in]  num      ：通道数目
     * \param[out] p_buf    : 传感器数据输出缓冲区，大小与 chan_num 一致
     *
     * \retval  AW_OK    成功，一个或多个通道数据读取成功
     * \retval    <0     部分或所有通道读取失败
     *
     * \attention 当通道读取失败时，器件驱动应使用 AWBL_SENSOR_VAL_SET_INVALID()
     * 宏该通道对应的传感器值标记为无效。仅可操作属于该传感器设备的通道，不可访
     * 问或操作其它通道对应的 传感器值。
     *
     * \note p_ids 列表可能包含多个通道，当中间层调用该函数时，表明至少有一个
     * 通道（p_ids[0]）是该传感器能够采集的。即该通道时该传感器提供的，但部分传
     * 感器可能支持多个通道，此时，驱动应检测后续其它所有通道，若存在其它通道需
     * 要采集，则传感器应该采集，并在p_buf的相应位置填充数据。例如，传感器可以
     * 采集列表的第0个通道和第3个通道，则应填充p_buf[0] 和  p_buf[3]。不属与该
     * 传感器的通道数据切勿尝试访问或改变。
     */
    aw_err_t (*pfn_data_get)(void                     *p_cookie,
                             const int                *p_ids,
                             int                       num,
                             aw_sensor_val_t          *p_buf);
                             
    /**
     * \brief 使能传感器的某一通道
     * 
     * 使能后，传感器通道被激活，传感器开始采集外部信号。若设置了传感器的采样频
     * （#AW_SENSOR_ATTR_SAMPLING_RATE），且值不为0，则系统将自动按照属性值指定
     * 的采率属性值样频率获取传感器中的采样值。若用户打开了数据准备就绪触发，则
     * 每次数据采样结束后，都将触发用户回调函数。特别地，若采样频率为0（默认），
     * 则不自动采样，每次调用 pfn_data_get()函数时才从传感器中获取最新数据。
     * 
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] p_ids    ：传感器通道 id 列表， p_id[0]必然属于该设备
     * \param[in] num      ：传感器通道数目
     * \param[in] p_result ：每个通道使能的结果存于val值中，与标准错误码含义一致
     *
     * \retval  AW_OK  所有通道使能成功
     * \retval   <0    部分或所有通道使能失败，每一个通道使能的结果存于 val 值中
     *
     * \attention p_result 指向的缓存用于存储每个通道使能的结果，器件驱动应使用
     * AWBL_SENSOR_VAL_SET_RESULT() 宏设置每个通道使能的结果。特别地，若p_result
     * 为NULL，则表明不关心每个通道使能的结果。
     *
     * \note 若该值为NULL，则表明通道无需使能，已经默认处于使能状态。
     */
    aw_err_t (*pfn_enable) (void                  *p_cookie,
                            const int             *p_ids,
                            int                    num,
                            aw_sensor_val_t       *p_result);
                                 
    /**
     * \brief 禁能传感器通道
     * 
     * 禁能后，传感器被关闭，传感器将停止采集外部信号，同时，系统也不再从传感器
     * 中获取采样值（无论采样频率是否为0）。
     *
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] p_ids    ：传感器通道 id 列表， p_id[0]必然属于该设备
     * \param[in] num      ：传感器通道数目
     * \param[in] p_result ：每个通道使能的结果存于val值中，与标准错误码含义一致
     *
     * \retval  AW_OK  所有通道使能成功
     * \retval   <0    标准错误码，部分或所有通道使能失败
     *
     * \attention p_result 指向的缓存用于存储每个通道禁能的结果，器件驱动应使用
     * AWBL_SENSOR_VAL_SET_RESULT() 宏设置每个通道禁能的结果。特别地，若p_result
     * 为NULL，则表明不关心每个通道禁能的结果。
     *
     * \note 若该值为NULL，则表明通道无法禁能，始终处于使能状态。
     */
    aw_err_t (*pfn_disable) (void                  *p_cookie,
                             const int             *p_ids,
                             int                    num,
                             aw_sensor_val_t       *p_result);
                                 
    /**
     * \brief 配置传感器通道属性
     *
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] id       ：传感器通道， 该通道必然属于该设备
     * \param[in] attr     ：属性，AW_SENSOR_ATTR_*(eg:
     *                       #AW_SENSOR_ATTR_SAMPLING_RATE)
     * \param[in] p_val    ：属性值
     *
     * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
     * 
     * \note 支持的属性与具体传感器相关，部分传感器可能不支持任何属性。此时，可
     * 将该函数设置为NULL。
     */
    aw_err_t (*pfn_attr_set)(void                    *p_cookie,
                             int                      id,
                             int                      attr,
                             const aw_sensor_val_t   *p_val);

    /**
     * \brief 获取传感器通道属性
     *
     * \param[in]  p_cookie : 驱动自定义参数
     * \param[in]  id       ：传感器通道 id， 该通道必然属于该设备
     * \param[in]  attr     : 属性，AW_SENSOR_ATTR_*(eg:
     *                        #AW_SENSOR_ATTR_SAMPLING_RATE)
     * \param[out] p_val    : 用于获取属性值
     *
     * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
     * 
     * \note 支持的属性与具体传感器相关，部分传感器可能不支持任何属性。此时，可
     * 将该函数设置为NULL。
     */
    aw_err_t (*pfn_attr_get) (void                   *p_cookie,
                              int                     id,
                              int                     attr,
                              aw_sensor_val_t        *p_val);

    /**
     * \brief 设置触发，一个通道仅能设置一个触发回调函数
     *
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] id       ：传感器通道 id， 该通道必然属于该设备
     * \param[in] flags    : 触发标志，指定触发的类型
     * \param[in] pfn_cb   : 触发时执行的回调函数的指针
     * \param[in] p_arg    : 回调函数用户参数
     *
     * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
     *
     * \note 支持的触发模式与具体传感器相关，部分传感器不支持任何触发模式。此时，
     * 可将该函数设置为NULL。
     */
    aw_err_t  (*pfn_trigger_cfg) (void                     *p_cookie,
                                  int                       id,
                                  uint32_t                  flags,
                                  aw_sensor_trigger_cb_t    pfn_cb,
                                  void                     *p_arg);
 
    /**
     * \brief 打开触发
     *
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] id       ：传感器通道 id， 该通道必然属于该设备
     
     * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
     */
    aw_err_t (*pfn_trigger_on) (void *p_cookie, int id);

    /**
     * \brief 关闭触发
     *
     * \param[in] p_cookie : 驱动自定义参数
     * \param[in] id       ：传感器通道 id， 该通道必然属于该设备
     *
     * \return AW_OK，成功；其它值，失败，详见标准错误号定义。
     */
    aw_err_t (*pfn_trigger_off) (void *p_cookie, int id);
};

/**
 * \brief 传感器所支持的类型信息（描述一组通道的类型）
 *
 * 该信息通常由设备自身确定，驱动即可完成定义，无需用户额外指定信息。
 *
 * 一个传感器设备的ID从小到大排列，ID分配方法是，用户仅需指定一个起始编号，实际
 * ID范围即为： start_id ~ (start_id + total_num - 1)）。在该范围内的ID排列顺序
 * 是由驱动自行定义的。
 *
 * 例如，某传感器支持8路电压采集，2路电流采集。若首先是 8路电压通道，然后是2路
 * 电流通道 。则类型信息可以定义如下：
 *
 * // 信息类型定义
 * const awbl_sensor_type_info_t info[2] = {
 *     {AW_SENSOR_TYPE_VOLTAGE, 8},        // 首先是8路连续电压通道
 *     {AW_SENSOR_TYPE_CURRENT, 2},        // 然后是2路连续电流通道
 * };
 *
 * 若ID排列的顺序是：首先是 2路电压通道，然后是2路电流通道，最后又是6路电压通道。
 * 则类型信息可以定义如下：
 *
 * // 信息类型定义
 * const awbl_sensor_type_info_t info[3] = {
 *     {AW_SENSOR_TYPE_VOLTAGE, 2},        // 首先是2路连续电压通道
 *     {AW_SENSOR_TYPE_CURRENT, 2},        // 然后是2路连续电流通道
 *     {AW_SENSOR_TYPE_VOLTAGE, 6},        // 最后是6路连续电压通道
 * };
 *
 * \attention ID分配顺序由驱动决定，因此，类型信息表由驱动完成定义，无需用户参与
 */
typedef struct awbl_sensor_type_info {
    int        type;              /**< \brief 类型 */
    int        num;               /**< \brief 标记有连续几个通道为该类型 */
} awbl_sensor_type_info_t;

/**
 * \brief 传感器服务常量信息
 */
typedef struct awbl_sensor_servinfo {

    /** \brief 支持的传感器总数目 */
    size_t                                 total_num;

    /** 
     * \brief 各通道对应的类型列表
     *
     * 用于描述传感器通道 ID 列表中各个通道的类型，若存在几个连续的通道类型相同
     * 则可以设置 num 的值表示连续几个相同类型的通道。
     * 
     * 列表中所有项的 num 之和即应该与传感器支持的通道数目（total_num）一致，否
     * 则该服务将无效，不会被系统使用。
     */
    const awbl_sensor_type_info_t         *p_type_info_list;
    
    /** \brief 列表大小 */
    size_t                                 list_size;

} awbl_sensor_servinfo_t;
 
/** \brief 传感器服务 */
typedef struct awbl_sensor_service {
 
    /** \brief 服务相关信息，如通道数目，通道类型等 */
    aw_const awbl_sensor_servinfo_t    *p_servinfo;
    
    /**
     * \brief 本服务所有传感器通道的起始编号（该值通常由用户指定）
     *
     * 实际占用的通道即为： start_id ~ (start_id + total_num - 1)
     *
     * 例如，传感器设备支持8个通道，若起始编号为： 10，则其占用的通道编号为：
     * 10 ~ 17。必须确保通道 id 唯一， 范围不能有相互交叉覆盖。
     */
    int                                start_id;
  
    /** \brief 驱动函数 */
    const struct awbl_sensor_servfuncs *p_servfuncs;
    
    /** \brief 驱动函数参数 */
    void                               *p_cookie;
    
    /** \brief 用于指向下一个传感器服务，便于以链表的形式组织多个传感器设备 */
    struct awbl_sensor_service         *p_next;

} awbl_sensor_service_t;
 
/** 
 * \brief 传感器组件初始化，第二阶段初始化之后调用
 * \return 无
 */
void awbl_sensor_init(void);

/** @} grp_awbl_if_sensor */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_SENSOR_H */

/* end of file */
