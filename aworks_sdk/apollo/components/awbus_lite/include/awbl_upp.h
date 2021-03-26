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
 * \brief AWBus 通用并行接口头文件
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * #include "awbl_upp.h"
 * \endcode
 * 本模块为 通用并行接口的精简版实现
 *
 * 更多内容待添加。。。
 *
 * \internal
 * \par modification history:
 * - 1.00 15-10-15  dcf, first implementation
 * \endinternal
 */

#ifndef __AWBL_UPP_H
#define __AWBL_UPP_H

#ifdef __cplusplus
extern "C" {
#endif	/* __cplusplus	*/

#include <stdarg.h>
#include "apollo.h"
#include "aw_msgq.h"
#include "awbus_lite.h"
#include "awbl_plb.h"


/**
 * \brief 通用并行接口
 *
 * 应用接口：所有成员不应该被应用直接使用
 *
 * 驱动接口：驱动需要管理除了“内部使用”外的所有其它成员。
 *      features:
 */
struct awbl_upp
{
    awbl_dev_t super;

/** \brief （驱动开发者使用）告诉标准层，驱动支持输入特性  */
#define AWBL_UPP_FT_IN          1   

/** \brief （驱动开发者使用）告诉标准层，驱动支持输出特性  */
#define AWBL_UPP_FT_OUT         2   

/** \brief （驱动开发者使用）告诉标准层，驱动支持内部DMA特性  */
#define AWBL_UPP_FT_BUILDIN_DMA 8   


    uint16_t    features;       /**< \brief 控制器的特性(支持的传输控制标志)  */

    uint16_t    align_size;     /**< \brief 设备对齐要求 */
    uint32_t    max_xfer_size;  /**< \brief 设备每次启动最大传输量 */

/** \brief （驱动开发者使用）告诉驱动打开设备 */
#define AWBL_UPP_CMD_OPEN       1   

/** \brief （驱动/应用开发者使用）告诉驱动开启传输 */
#define AWBL_UPP_CMD_XFER_START 2   

/** \brief （驱动/应用开发者使用）告诉驱动开启传输*/
#define AWBL_UPP_CMD_XFER_STOP  3   

/** \brief （驱动开发者使用）告诉驱动关闭设备 */
#define AWBL_UPP_CMD_CLOSE      4   

/** \brief （驱动/应用开发者使用）告诉驱动设置为输出 */
#define AWBL_UPP_CMD_SET_OUTPUT 5   

/** \brief （驱动/应用开发者使用）告诉驱动设置输入 */
#define AWBL_UPP_CMD_SET_INPUT  6   

/** \brief （驱动开发者使用）驱动设置一个初始化好的msgq给标准层使用 */
#define AWBL_UPP_CMD_SET_MSGQID 7   


#define AWBL_UPP_CMD_RESET      8   /**< \brief （驱动/应用开发者使用）重置 */
    aw_err_t (*ioctl)(struct awbl_upp *_this, int cmd, va_list va );

    /** \brief 内部使用,驱动开发和应用开发都不需要关注 */
    void (*xfer_complete_callback)( void *callback_arg );
    void           *callback_arg;
    aw_msgq_id_t    msgid;
    int             is_opened;
};

/**
 * \brief  API for application developers
 *
 *
 * \param[in] device_name       设备名
 * \param[in] unit_no           设备序号,配置中的unit
 *
 * \retval NULL  打开失败
 */
struct awbl_upp *awbl_upp_open( const char *device_name, int unit_no );
void awbl_upp_close( struct awbl_upp *thiz );
/**
 * \brief 设置设备属性
 *
 * \param[in] cmd 命令字，应用程序可取：
 *               AWBL_UPP_CMD_XFER_START:需要参数window_address,line_size_valid,
 *                                       line_size,line_count，
 *                                       等价使用awbl_upp_xfer_async()
 *               AWBL_UPP_CMD_XFER_STOP: 不需要额外参数
 *               AWBL_UPP_CMD_SET_OUTPUT:不需要额外参数
 *               AWBL_UPP_CMD_SET_INPUT: 不需要额外参数
 *
 * windows_address可以移动窗口水平和垂直位置，
 * 第四个参数可以控制总窗口大小(虚拟窗口宽度)，
 * 第三个参数可以控制有效窗口大小（宽度），
 * line_count可以控制高度
 * \retval AW_OK 命令操作成功
 * \retval AW_ERROR 命令操作失败
 */
aw_err_t awbl_upp_ioctl(struct awbl_upp *_this, int cmd, ... );
aw_err_t awbl_upp_xfer_async(
        struct awbl_upp *_this,
        char       *window_address,
        uint32_t    line_size_valid,
        uint32_t    line_size_virtual,
        uint32_t    line_count
                        );
/**
 * \brief 传输完成回调函数
 *
 * 此回调函数不能保证一定要中断上下文调用。
 */
void awbl_upp_set_callback(struct awbl_upp *_this,
        void (*xfer_complete_callback)( void * ),
        void *arg );
/**
 * \brief 传输完成同步
 *
 * 异步等待上一次传输完成。
 * \retval AW_OK 成功传输
 * \retval -ETIME 超时失败
 */
aw_err_t awbl_upp_wait_async( struct awbl_upp *_this, void **buffer, int timeout );

/**
 * \brief API for driver developers
 */
void awbl_upp_init(struct awbl_upp *_this );
void awbl_upp_destroy(struct awbl_upp *_this);

/** \brief need to set msgq_id */
aw_err_t awbl_upp_ioctl(struct awbl_upp *_this, int cmd, ... ); 
void awbl_upp_complete( struct awbl_upp *_this, void *buffer );


#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_I2CBUS_H */

/* end of file */
