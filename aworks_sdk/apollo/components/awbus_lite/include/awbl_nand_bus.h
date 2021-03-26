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
 * \brief NandBus head file
 *
 * define NandBus data structure and functions
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-16  deo, first implementation
 * - 2.00 17-3-23   vih, modify
 * \endinternal
 */
#ifndef __AWBL_NAND_BUS_H
#define __AWBL_NAND_BUS_H


#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_spinlock.h"
#include "awbus_lite.h"
#include "string.h"


/** \brief 定义一个awbl_nand_controller类型的指针变量 */
#define AWBL_NANDBUS_DECL(p_dev, p_awdev) \
            struct awbl_nandbus *p_dev = \
                (struct awbl_nandbus *)AW_CONTAINER_OF( \
                    AW_CONTAINER_OF(p_awdev, struct awbl_busctlr, super), \
                    struct awbl_nandbus, \
                    awbus)

struct awbl_nandbus;

/** \brief 控制器操作 */
typedef struct awbl_nandbus_opt {

    /** \brief 操作类型 */
    uint8_t            type;
#define NAND_CMD       0x01   /**< \brief 命令 */
#define NAND_ADDR      0x02   /**< \brief 地址 */
#define NAND_RD_DATA   0x03   /**< \brief 读取数据 */
#define NAND_WR_DATA   0x04   /**< \brief 写入数据 */

    /** \brief CE是否锁住 */
    uint8_t            lock_ce;

    /** \brief 指向操作所需数据 */
    const void        *ptr;

    /** \brief 操作所需数据的长度 */
    uint32_t           len;

} awbl_nandbus_opt_t;





/** \brief 控制器驱动信息 */
struct awbl_nandbus_serv {

    /** \brief 片选 */
    void (*pfn_select)  (struct awbl_nandbus        *p_bus,
                         void                       *p_info);

    /** \brief 操作 */
    aw_err_t (*pfn_opt) (struct awbl_nandbus        *p_bus,
                         struct awbl_nandbus_opt    *p_opt);

    /** \brief Nand芯片就绪检查 */
    aw_bool_t (*pfn_ready) (struct awbl_nandbus     *p_bus,
                            void                    *priv);

    /** \brief 控制器复位 */
    void (*pfn_reset)   (struct awbl_nandbus        *p_bus);
};


struct awbl_nandbus {

    /** \brief 总线控制器 */
    struct awbl_busctlr          awbus;

    /** \brief 总线ID */
    uint8_t                      bus_index;

    struct awbl_nandbus_serv     serv;

    /** \brief 设备自旋锁 */
    aw_spinlock_isr_t            dev_lock;

    /** \brief 设备互斥锁 */
    AW_MUTEX_DECL(               dev_mux);

    /** \brief 当前控制器是否上锁 */
    aw_bool_t                       lock;

    /** \brief ECC强度支持 */
    uint8_t                      ecc_support;
#define AWBL_NAND_ECC_NONE       0x00   /**< \brief 控制器不支持ECC功能 */
#define AWBL_NAND_ECC_1BITS      0x01   /**< \brief 控制器支持1位ECC纠错 */
#define AWBL_NAND_ECC_4BITS      0x08   /**< \brief 控制器支持4位ECC纠错 */
#define AWBL_NAND_ECC_8BITS      0x80   /**< \brief 控制器支持8位ECC纠错 */

    void                        *priv;
};


/***************************Nand控制器通用接口**********************************/

/**
 * \brief NandBus初始化
 */
void awbl_nand_bus_init (void);


/**
 * \brief 创建控制器
 * \param[in] p_controller  总线控制器
 * \param[in] ecc_support   ECC强度支持
 *
 * \return AW_OK表示创建成功，其他表示失败。
 */
aw_err_t awbl_nandbus_create (struct awbl_nandbus *p_bus,
                              uint8_t              bus_index);


/**
 * \brief 总线控制器加锁
 * \param[in] p_controller  总线控制器
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_nandbus_lock (struct awbl_nandbus *p_bus);


/**
 * \brief 总线控制器去锁
 * \param[in] p_controller : 总线控制器
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_nandbus_unlock (struct awbl_nandbus *p_bus);


/**
 * \brief 总线控制器复位
 * \param[in] p_controller : 总线控制器
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_nandbus_reset (struct awbl_nandbus *p_bus);


/**
 * \brief 片选
 * \param[in] p_bus   bus
 * \param[in] p_chip  Nand芯片
 *
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_nandbus_select (struct awbl_nandbus *p_bus,
                              void                *p_arg);


/**
 * \brief 对Nand芯片进行操作
 * \param[in] p_chip  Nand芯片
 * \param[in] p_opt   操作结构体
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_err_t awbl_nandbus_opt (struct awbl_nandbus      *p_bus,
                           struct awbl_nandbus_opt  *p_opt);


/**
 * \brief 检查Nand芯片是否准备就绪
 * \param[in] p_chip  Nand芯片
 * \return AW_OK表示操作成功，其他表示失败。
 */
aw_bool_t awbl_nandbus_is_ready (struct awbl_nandbus *p_bus);


#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif /* __AWBL_NAND_BUS_H */

/* end of file */

