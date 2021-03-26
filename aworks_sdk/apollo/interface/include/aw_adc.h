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
 * \brief 通用ADC接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include aw_adc.h
 * \endcode
 *
 * \par 范例1(简单同步读取)
 * \code
 * #include "aw_adc.h"
 *
 * #define N_SAMPLES    10        //定义采样次数为10次
 * #define  CHANNEL      2        //转换通道为通道2
 *
 * int main()
 * {
 *     uint16_t  adc_val[N_SAMPLES]; // 假定采样位数为12位，则数据类型为uint16_t
 *     aw_err_t  ret;
 *     int       i;
 *
 *     //同步读取，转换结束后返回
 *     ret = aw_adc_sync_read(CHANNEL, adc_val, N_SAMPLES, AW_FALSE);
 *
 *     if (ret != AW_OK) {
 *         aw_kprintf("转换出错\r\n");
 *     } else {
 *         aw_kprintf("转换完成\r\n");
 *         //打印采样值
 *         for (i = 0; i < N_SAMPLES; i++) {
 *         aw_kprintf("%d ", p_val[i]);
 *         }
 *     }
 *
 *     //打印采样值
 *     for (i = 0; i < N_SAMPLES; i++) {
 *         aw_kprintf("%d ", p_val[i]);
 *     }
 * }
 * \endcode
 *
 * \par 范例2(客户端读取)
 * \code
 * #include "aw_adc.h"
 *
 * void adc_callback (void *p_arg, int state)
 * {
 *      int num = (int)p_arg;
 *
 *      if (state != AW_OK ) {
 *          //表征数据转换失败，后续序列会自动取消，如需再次转换，需重新启动
 *          //这里属于中断级回调，处理时间应该尽可能短，再此可以使用信号量或
 *          //消息邮箱处理转换失败的工作在任务中完成
 *      } else {
 *          //数据转换成功，接下来处理数据
 *          //这里属于中断级回调，处理时间应该尽可能短，再此可以使用信号量或
 *          //消息邮箱处理数据的工作在任务中完成
 *      }
 * }
 *
 * int main()
 * {
 *     void                 adc_val1[100];
 *     uint16_t             adc_val2[100]
 *     aw_adc_buf_desc_t    desc[2];
 *     struct aw_adc_client client;
 *
 *     //制作缓冲区描述符0，回调函数参数为0,0号缓冲区转换完成
 *     aw_adc_mkbufdesc(&desc[0],hs_val1,cnt,adc_callback,(void *)0);
 *
 *     //制作缓冲区描述符1，回调函数参数为1,1号缓冲区转换完成
 *     aw_adc_mkbufdesc(&desc[1],hs_val2,cnt,adc_callback,(void *)1);
 *
 *     aw_adc_client_init(&client, IMX28_HSADC_LRADCPIN0, AW_FALSE);
 *
 *     //启动客户端，2个缓冲区循环依次填充数据，0参数表示无限循环填充
 *     //函数立即返回，相应缓冲区数据填充满后调用相应的回调函数
 *     aw_adc_client_start(&client, // 待启动的客户端
 *                          desc,   // 缓冲区描述
 *                          2,      // 缓冲区的个数
 *                          5);     // 各缓冲区循环填充5次，0则表示无限循环填充
 *
 *     while(1) {
 *     }
 * }
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 2.01 15-06-10 tee, add and modify some interface
 * - 2.00 14-09-25 ehf, modify some interface and implemention to support
 *                 read mulitiple samples once
 * - 1.00 12-11-01 zyr, first implementation
 * \endinternal
 */

#ifndef __AW_ADC_H
#define __AW_ADC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_adc
 * \copydoc aw_adc.h
 * @{
 */

#include "apollo.h"
#include "aw_psp_adc.h"
#include "aw_list.h"

/**
 * \brief ADC通道类型定义
 */
typedef aw_psp_adc_channel_t  aw_adc_channel_t;

/**
 * \brief ADC值类型定义
 */
typedef aw_psp_adc_val_t      aw_adc_val_t;

/**
 * \brief   转换完成回调函数类型
 *
 * \param[in]  p_cookie    用户初始化客户端传入的回调函数参数
 * \param[in]  state       转换完成的状态：
 *                             AW_OK        : 转换成功
 *                             AW_ERROR     : 转换出错
 *
 * \note 处理转换数据前应该确保转换结束的状态为AW_OK。如果不为AW_OK，则后续转换
 *       序列将自动取消，不会再转换。通常情况下，转换出错可能是由于缓冲区的buf过
 *       小，使得buf切换需要很频繁，数据处理时间稍长，数据就溢出了。可以增大缓冲
 *       区的buf大小以增加buf切换的时间间隔。
 */
typedef void (*pfn_adc_complete_t) (void *p_cookie, aw_err_t state);

/**
 * \brief ADC缓冲区描述
 */
typedef struct aw_adc_buf_desc {

    /**
     * \brief 用于存放ADC转换结果的数据缓冲区
     *
     * 具体类型与实际ADC转换位数相关，需要确保字长为2的幂次倍8-bit。
     * 常见的ADC转换位数（可通过aw_adc_bits_get()获取）和缓冲区类型的关系如下：
     *
     *      ADC bits    |    buffer type
     *    ------------- | -------------------
     *       1  ~  8    |  uint8_t/int8_t
     *       9  ~  16   |  uint16_t/int16_t
     *      17  ~  31   |  uint32_t/int32_t
     */
    void               *p_buf;

    /** \brief 数据缓冲区的长度 */
    uint32_t            length;

    /** \brief 该缓冲区数据填充满后的回调函数 */
    pfn_adc_complete_t  pfn_complete;

    /** \brief 回调函数的参数 */
    void               *p_arg;
} aw_adc_buf_desc_t;


/**
 * \brief ADC客户端结构(请不要直接使用该结构体的成员变量)
 *
 * 该结构用于静态分配内存，用户和驱动程序不允许直接使用其成员变量
 */
typedef struct aw_adc_client {

    struct aw_list_head      node;        /**< \brief 客户端服务链表节点     */
    struct awbl_adc_service *p_serv;      /**< \brief 与客户端关联ADC服务    */

    aw_adc_channel_t         channel;     /**< \brief 与该客户端关联的通道号 */
    aw_adc_buf_desc_t       *p_desc;      /**< \brief 缓冲区描述             */
    uint32_t                 desc_num;    /**< \brief 缓冲区个数             */
    uint32_t                 count;       /**< \brief 序列转换完成的次数     */
    int                      stat;        /**< \brief 客户端状态             */
    aw_bool_t                urgent;      /**< \brief 紧急性                 */
    uint8_t                  data_bits;   /**< \brief 缓冲区数据的位数       */

} aw_adc_client_t;

/**
 * \brief 获取一个ADC通道的位数
 *
 * \param[in] ch  ADC通道号
 *
 * \return    >0      该ADC通道的有效位数
 * \retval -AW_ENXIO  ADC通道号不存在
 */
int aw_adc_bits_get(aw_adc_channel_t ch);

/**
 * \brief 获取一个AD通道的参考电压（单位：mV）
 *
 * \param[in] ch  ADC通道号
 *
 * \return   >0       该ADC通道参考电压(mV)
 * \retval -AW_ENXIO  ADC通道号不存在
 */
int aw_adc_vref_get(aw_adc_channel_t ch);

/**
 * \brief 获取ADC当前的采样率(Samples Per Sencond,每秒钟采样的个数)
 *
 * \param[in]  ch      ADC通道号
 * \param[out] p_rate  用于读取当前的采样率
 *
 * \retval   AW_OK     读取成功
 * \retval -AW_ENXIO   ADC通道号不存在
 */
aw_err_t aw_adc_rate_get(aw_adc_channel_t ch, uint32_t *p_rate);

/**
 * \brief 设置ADC的采样率(Samples Per Sencond,每秒钟采样的个数)
 *
 * \param[in]  ch     ADC通道号
 * \param[in]  rate   设置的采样率
 *
 * \retval   AW_OK    设置成功
 * \retval -AW_ENXIO  ADC通道号不存在
 *
 * \note 实际的采样率可能与此处设置的采样率存在一定误差
 */
aw_err_t aw_adc_rate_set(aw_adc_channel_t ch, uint32_t rate);

/**
 * \brief 将一个通道的采样值值转换为电压值（单位：mV）
 *
 * \param[in]  ch     ADC通道号
 * \param[in]  cnt    ADC采样值的个数
 * \param[out] p_val  存放ADC采样值
 * \param[out] p_mv   存放转换结果的电压值(mV)
 *
 * \return   >0   该ADC通道的有效位数
 * \retval -ENXIO ADC通道号不存在
 */
aw_err_t aw_adc_val_to_mv(aw_adc_channel_t  ch,
                          void             *p_val,
                          uint32_t          cnt,
                          uint32_t         *p_mv);


/**
 * \brief 设置ADC数据缓冲区描述符的相关参数
 *
 * \param[in]  p_desc        ADC数据缓冲区描述符指针
 * \param[in]  length        数据缓冲区长度
 * \param[in]  pfn_complete  数据填充满后的回调函数
 * \param[in]  p_arg         回调函数参数
 * \param[out] p_buf         用于存放ADC转换结果的缓冲区,注意数据类型，数据宽度
 *                           为2的幂次倍8-bit,取满足ADC位数的最小值
 *
 * \return 无
 */
aw_local aw_inline void aw_adc_mkbufdesc (aw_adc_buf_desc_t     *p_desc,
                                          void                  *p_buf,
                                          uint32_t               length,
                                          pfn_adc_complete_t     pfn_complete,
                                          void                  *p_arg)
{
    p_desc->p_buf        = p_buf;
    p_desc->length       = length;
    p_desc->pfn_complete = pfn_complete;
    p_desc->p_arg        = p_arg;
}

/**
 * \brief 同步读取一个ADC通道的数字量值
 *
 * 使用该接口可以方便的获取一个AD通道的一个或多个采样值,
 * 如需更复杂的控制可使用ADC客户端
 *
 * \param[in]  ch      AD通道号
 * \param[out] p_val   用于存放采样值的数组地址，具体类型与ADC的数据宽度相关
 * \param[in]  samples 需要采样的个数
 * \param[in]  urgent  紧急转换任务，将优先于urgent为FALSE的转换序列转换
 *
 * \return >=0       该AD通道的数字量值
 * \retval -AW_ENXIO AD通道号不存在
 *
 *
 * \note 关于p_val值的类型
 *
 * 具体类型与实际ADC转换位数相关，需要确保字长为2的幂次倍8-bit。
 * 常见的ADC转换位数（可通过aw_adc_bits_get()获取）和缓冲区类型的关系如下：
 *
 *      ADC bits    |    buffer type
 *    ------------- | -------------------
 *       1  ~  8    |  uint8_t/int8_t
 *       9  ~  16   |  uint16_t/int16_t
 *      17  ~  31   |  uint32_t/int32_t
 */
aw_err_t aw_adc_sync_read(aw_adc_channel_t  ch,
                          void             *p_val,
                          uint32_t          samples,
                          aw_bool_t         urgent);

/**
 * \brief 初始化ADC客户端
 *
 * \param[in] p_client 由用户提供的ADC客户端内存空间
 * \param[in] ch       该ADC客户端对应的转换通道
 * \param[in] urgent   客户端是否需紧急转换（紧急客户端优先于普通客户被端转换）
 *
 * \retval  AW_OK     初始化成功
 * \retval -AW_EINVAL 参数错误，检查参数的合法性
 * \retval -AW_ENXIO  通道为无效通道
 *
 * \note 关于转换紧急性，紧急转换客户端优先于普通转换客户端，如果当前存在普通客
 *       户端，则会在当前普通客户端的一个序列结束后，强制打断普通客户端的转换，
 *       切换到紧急客户端。
 */
aw_err_t aw_adc_client_init(aw_adc_client_t    *p_client,
                            aw_adc_channel_t    ch,
                            aw_bool_t           urgent);

/**
 * \brief 启动ADC客户端开始转换
 *
 * 启动客户端转换前，确保ADC客户端已初始化，初始化一次后，如果不修改客户端对应的
 * 通道号和转换紧急性，后续启动均可以不再初始化。
 *
 * \param[in] p_client  所使用的ADC客户端
 * \param[in] p_desc    缓冲区描述,可以是单个（单个缓冲区描述的地址）或多个（缓
 *                      冲区描述数组的地址）
 * \param[in] desc_num  指定缓冲区描述的个数
 * \param[in] count     序列的转换次数（一个转换序列表示desc_num指定个数的的缓
 *                      冲区依次填充满），如果指定为0，则会持续不断的转换，即一
 *                      个序列结束，自动继续进行下一个序列
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_EALREADY 客户端已经开始，如需重新开始，请先取消客户端转换
 * \retval -AW_EPERM    客户端未初始化
 *
 * \note  关于缓冲区描述符，缓冲区描述符描述了存放ADC转换结果的缓冲区，可以是单个或
 *        多个，如果指定了多个缓冲区。则会在前一个缓冲区填充满后，自动连续的切换到
 *        下一个缓冲区。指定序列转换次数为多次时（包括持续转换(0)），缓冲区个数至少
 *        为2，如果为缓冲区个数为1，将返回参数错误（单个缓冲区无法循环填充）。
 */
aw_err_t aw_adc_client_start(aw_adc_client_t    *p_client,
                             aw_adc_buf_desc_t  *p_desc,
                             int                 desc_num,
                             uint32_t            count);

/**
 * \brief 取消ADC客户端的转换请求
 *
 * 该接口会把对应客户端请求从请求列表中删除
 *
 * \param[in] p_client 要取消的ADC客户端
 *
 * \retval  AW_OK       操作成功
 * \retval -AW_EINVAL   参数错误
 * \retval -AW_EPERM    客户端未在转换序列中，取消操作不允许
 */
aw_err_t aw_adc_client_cancel(aw_adc_client_t *p_client);


/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* __AW_ADC_H */

/* end of file */
