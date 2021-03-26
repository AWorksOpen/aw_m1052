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
 * \brief CRC(Cyclic Redundancy Check)循环冗余校验服务标准头文件
 *
 * 使用本服务需要包含头文件 aw_crc.h
 *
 * \par 使用示例
 * \code
 *  #include "aw_crc.h"
 *
 *  int main() 
 *  {
 *
 *      uint8_t  ret;
 *      uint8_t  ndata[4] = {0x30,0x31,0x32,0x33};
 *      uint32_t crc;
 *
 *      //定义了一个模型，该模型就是常用的CRC-16模型，如WinRAR中就是使用的该模型
 *      AW_CRC_DECL(crc16,         // crc模型 crc16
 *                  16,            // crc宽度 16
 *                  0x1021,        // 生成多项式
 *                  0x0000,        // 初始值
 *                  AW_TRUE,          // refin
 *                  AW_TRUE,          // refout
 *                  0x0000);       // xorout
 *
 *
 *       ret = AW_CRC_INIT(crc16,               // crc模型，由AW_CRC_DECL()定义的
 *                         crctable16_1021_ref, // crc表，系统的，不需要计算
 *                         AW_CRC_FLAG_AUTO)    // 自动模式，自动选择硬件或软件实现
 *      if (ret != AW_OK) {
 *          //初始化失败
 *      } else {
 *          AW_CRC_CAL(crc16,    // crc模型，由AW_CRC_DECL()定义的
 *                     ndata,   // 待进行CRC计算的数据
 *                     4);   // 此次计算的数据
 *
 *          //中间可以继续计算
 *          // ...
 *
 *
 *          //最后可以提取出最终结果
 *          crc = AW_CRC_FINAL(crc16);  // crc模型，由AW_CRC_DECL()定义的
 *
 *          aw_kprintf("计算结果为:%x\n",crc);
 *
 *          //如果后续还需计算，则需要重新调用AW_CRC_INIT()初始化后再计算。
 *      }
 *      return 0;
 *  }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.00 14-10-27  tee, first implementation
 * \endinternal
 */

#ifndef __AW_CRC_H
#define __AW_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_crc 
 * \copydoc aw_crc.h
 * @{
 */

#include "aworks.h"
#include "stdint.h"

/**
 * \name 8位及8位以下crc table
 *
 * 当前已经支持的模型，表格已经建立好，无需重新建立，直接使用即可。
 * 表的命名规则：crctableX_Y(_ref)[256] X、Y代表一个数字。
 *   - 紧跟crctable的数字X表示crc宽度，如crc5则对应crctable5
 *   - 接着下划线后的数字Y表示poly,生成多项式。
 *      - 8位及8位以下：poly使用8位表示，  如09,不省略0
 *      - 9至16位     ：poly使用16位表示，如8005,不省略0
 *      - 17 至32位   ：poly使用32位表示，如04C11DB7,不省略0.
 *   - 后缀带ref的表示该表为反序表，当crc模型中refin == TRUE时，
 *     应该传入后缀带ref的表。
 *
 * 因此，根据使用的crc模型，即可知道该使用哪个表，如crc模型为：
 *     width = 5，poly = 0x09,refin = AW_TRUE.
 * 则直接使用 crctable5_09_ref 表即可。
 *
 * \note 表的命名规则同样适合于其他位数的CRC table表
 * @{
 */

/** \brief crc4  poly = 0x03, refin = AW_TRUE  */
aw_import aw_const uint8_t crctable4_03_ref[256];

/** \brief crc5  poly = 0x09, refin = AW_FALSE */
aw_import aw_const uint8_t crctable5_09[256];

/** \brief crc5  poly = 0x15, refin = AW_TRUE */
aw_import aw_const uint8_t crctable5_15_ref[256];

/** \brief crc5  poly = 0x05, refin = AW_TRUE */
aw_import aw_const uint8_t crctable5_05_ref[256];

/** \brief crc6  poly = 0x03, refin = AW_TRUE */
aw_import aw_const uint8_t crctable6_03_ref[256];

/** \brief crc7  poly = 0x09, refin = AW_FALSE */
aw_import aw_const uint8_t crctable7_09[256];

/** \brief crc8  poly = 0x07, refin = AW_FALSE */
aw_import aw_const uint8_t crctable8_07[256];

/** \brief crc8  poly = 0x07, refin = AW_TRUE */
aw_import aw_const uint8_t crctable8_07_ref[256];

/** \brief crc8  poly = 0x31, refin = AW_TRUE */
aw_import aw_const uint8_t crctable8_31_ref[256];

/** @} */

/**
 * \name 9位至16位crc table
 * @{
 */

/** \brief crc16  poly = 0x8005, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_8005_ref[256];

/** \brief crc16  poly = 0x1021, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_1021_ref[256];

/** \brief crc16  poly = 0x1021, refin = AW_FALSE */
aw_import aw_const uint16_t crctable16_1021[256];

/** \brief crc16  poly = 0x3D65, refin = AW_TRUE */
aw_import aw_const uint16_t crctable16_3d65_ref[256];

/** @} */

/**
 * \name 17位至32位crc table
 * @{
 */

/** \brief crc32  poly = 0x04c11db7, refin = AW_TRUE */
aw_import aw_const uint32_t crctable32_04c11db7_ref[256];

/** \brief crc32  poly = 0x04c11db7, refin = AW_FALSE */
aw_import aw_const uint32_t crctable32_04c11db7[256];

/** @} */

/**
 * \name CRC计算标志，用于AW_CRC_INIT（）宏的flags参数
 * @{
 */
/** \brief 自动模式，由程序选择硬件或者软件实现 */
#define AW_CRC_FLAG_AUTO           1     

/** \brief 指定由硬件实现 */
#define AW_CRC_FLAG_HARDWARE       2    

/** \brief 指定由软件实现 */ 
#define AW_CRC_FLAG_SOFTWARE       4 

/** \brief 需要创建table表 */
#define AW_CRC_FLAG_CREATETAB      8     
/** @} */

/**
 * \brief CRC模型结构体定义
 */
typedef struct aw_crc_pattern {
    uint8_t    width;           /**< \brief CRC宽度 */
    uint32_t   poly;            /**< \brief CRC生成多项式 */
    uint32_t   initvalue;       /**< \brief CRC初始值 */
    aw_bool_t  refin;           /**< \brief 输入字节bit反序 */
    aw_bool_t  refout;          /**< \brief 输出CRC按bit反序 */
    uint32_t   xorout;          /**< \brief 输出异或值 */
} aw_crc_pattern_t;


/**
 * \brief CRC客户端结构体定义，用户不应使用,而应该直接使用AW_CRC_DECL()进行crc模型的定义
 */
typedef struct aw_crc_client {
    aw_crc_pattern_t pattern;     /**< \brief CRC模型            */
    uint32_t         crcvalue;    /**< \brief CRC计算值          */
    uint32_t        *p_table;     /**< \brief CRC table表        */
    aw_bool_t        isinit;      /**< \brief 是否正确初始化标志 */
    aw_bool_t        ishardware;  /**< \brief 是否是硬件计算     */
    void            *p_servfuncs; /**< \brief 服务函数           */
    void            *p_arg;       /**< \brief 服务函数参数       */
} aw_crc_client_t;


/**
 * \brief CRC初始化函数，用户无需直接使用该函数，应该使用宏AW_CRC_INIT()初始化
 *
 * \param[in] p_crc_client  指向CRC客户端结构体的指针
 * \param[in] crc_table     指向CRC表的指针
 * \param[in] flags         CRC标志
 *
 * \retval   AW_OK       初始化成功
 * \retval   AW_ERROR    初始化失败，通常，是由于表格传入错误引起的失败，请检查
 *                       表格是否传入正确,如果使能了硬件模式，则是因为没有搜索到
 *                       相应CRC计算的硬件设备。
 */
aw_err_t aw_crc_init(struct aw_crc_client *p_crc_client,
                     void                 *crc_table,
                     uint32_t              flags);

/**
 * \brief CRC计算， 用户无需直接使用该函数，应该使用宏AW_CRC_CAL()进行计算
 *
 * \param[in] p_crc_client  CRC模型实体，由 AW_CRC_DECL() 或 AW_CRC_DECL_STATIC()
 *                          定义的
 * \param[in] p_data        指向待进行CRC计算的数据
 * \param[in] nbytes        本次计算的个数
 *
 * \return 无返回值
 *
 * \note 该宏不会返回值，crc计算结束时，调用AW_CRC_FINAL()获取最终结果值
 */
void aw_crc_cal(struct aw_crc_client *p_crc_client,
                uint8_t              *p_data,
                uint32_t              nbytes);

/**
 * \brief 获取CRC计算结果， 用户无需直接使用该函数，应该使用宏AW_CRC_FINAL()得到
 *        最终的CRC计算结果
 *
 * \param[in] p_crc_client  CRC模型实体，由 AW_CRC_DECL() 或 AW_CRC_DECL_STATIC() 
 *                          定义的
 *
 * \return crc值
 */
uint32_t aw_crc_final(struct aw_crc_client *p_crc_client);


/**
 * \brief 定义一个CRC模型
 *
 * 本宏定义一个CRC对象，定义之后，需要调用AW_CRC_INIT()初始化CRC对象。
 * 初始化完成后，可以使用AW_CRC_CAL()计算CRC值。
 *
 * \param[in] crc        CRC对象，是后续作为宏 AW_CRC_INIT()、 AW_CRC_CAL()的第
 *                       一个参数
 * \param[in] width      CRC的宽度，如 crc16 则 width 为 16.当前支持的最大宽度为32.
 * \param[in] poly       CRC生成多项式
 * \param[in] initvalue  CRC计算的初始值
 * \param[in] refin      输入字节bit反序（具体参见CRC模型）
 * \param[in] refout     输出CRC按bit反序（具体含义参见CRC模型）
 * \param[in] xorout     输出异或值(具体含义参见CRC模型)
 *
 * CRC模型可以参见：<http://reveng.sourceforge.net/crc-catalogue/>
 */
#define AW_CRC_DECL(crc,width,poly,initvalue,refin,refout,xorout)   \
                                struct aw_crc_client crc = \
                          {{width,poly,initvalue,refin,refout,xorout}}

/**
 * \brief 定义CRC模型(静态)
 *
 * \param[in] crc        CRC对象，是后续作为宏 AW_CRC_INIT()、 AW_CRC_CAL()的第
 *                       一个参数
 * \param[in] width      CRC的宽度，如 crc16 则 width 为 16.当前支持的最大宽度为32.
 * \param[in] poly       CRC生成多项式
 * \param[in] initvalue  CRC计算的初始值
 * \param[in] refin      输入字节bit反序（具体参见CRC模型）
 * \param[in] refout     输出CRC按bit反序（具体含义参见CRC模型）
 * \param[in] xorout     输出异或值(具体含义参见CRC模型)
 *
 * 本宏与 AW_CRC_DECL() 的功能相同，区别在于，AW_CRC_DECL_STATIC() 在定义
 * 任务所需内存时，使用关键字 \b static ，如此一来，便可以将任务实体的作用域限制在
 * 模块内(文件内)从而避免模块之间的任务命名冲突;还可以在函数内使用本宏定义任务。
 */
#define AW_CRC_DECL_STATIC(crc,width,poly,initvalue,refin,refout,xorout)   \
                               static struct aw_crc_client crc = \
                          {{width,poly,initvalue,refin,refout,xorout}}

/**
 * \brief 初始化CRC模型
 *
 * \param[in] crc        CRC模型实体，由 AW_CRC_DECL() 或 AW_CRC_DECL_STATIC() 
 *                       定义的
 * \param[in] crctable   crc计算所用的table表，当前系统支持的CRC模型均提供了
 *                       table表，用户直接使用对应的表即可。需要注意的是，传
 *                       入table表时，如果模型中refin为TRUE,则需要传入后缀带
 *                       ref的表。
 * \param[in] flags      CRC标志
                         - AW_CRC_FLAG_AUTO       自动模式，由程序选择硬件或者
 *                                                软件实现
 *                       - AW_CRC_FLAG_HARDWARE   指定由硬件计算,则table可以为NULL
 *                       - AW_CRC_FLAG_SOFTWARE   指定由软件实现
 *                       - AW_CRC_FLAG_CREATETAB  指定需要创建crc表，当系统不支
 *                                                持指定CRC模型时， 传入此标志，
 *                                                将会自动将crctable填充正确的值。
 * \retval   AW_OK       初始化成功
 * \retval   AW_ERROR    初始化失败，通常，是由于表格传入错误引起的失败，请检查
 *                       表格是否传入正确如果使能了硬件模式，则是因为没有搜索到
 *                       相应CRC计算的硬件设备。
 *
 * \note 特别地：如果当前系统不支持用户所使用的模型，
 *       则可以自定义一个空数组，大小为256，数据元素宽度与CRC数据
 *       宽度(是CRC的数据宽度，如CRC5，则需要一个8位的数据来保存，该值则为8)
 *       一致。如:
 *        - crc5,  则传入  一个 uint8_t  table[256]类型的数组.
 *        - crc16, 则传入  一个 uint16_t table[256]类型的数组,
 *        - crc32, 则传入  一个 uint32_t table[256]类型的数组。
 *       在后续的flags标志中， 传入标志AW_CRC_FLAG_CREATETAB即可。
 */
#define AW_CRC_INIT(crc,crctable,flags)   \
                    aw_crc_init(&crc,(void *)crctable,flags)


/**
 * \brief  CRC计算
 *
 * \param[in] crc     CRC模型实体，由 AW_CRC_DECL() 或 AW_CRC_DECL_STATIC() 定义的
 * \param[in] pdata   指向待进行CRC计算的数据
 * \param[in] nbytes  本次计算的个数
 *
 * \return 无返回值
 *
 * \note 该宏不会返回值，crc计算结束时，调用AW_CRC_FINAL()获取最终结果值
 *
 */
#define AW_CRC_CAL(crc,pdata,nbytes)    aw_crc_cal(&crc,pdata,nbytes)

/**
 * \brief  获取CRC计算结果
 *
 * \param[in] crc  CRC模型实体，由 AW_CRC_DECL() 或 AW_CRC_DECL_STATIC() 定义的
 *
 * \return crc值
 */
#define AW_CRC_FINAL(crc)     aw_crc_final(&crc)

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* __AW_CRC_H */

/* end of file */

