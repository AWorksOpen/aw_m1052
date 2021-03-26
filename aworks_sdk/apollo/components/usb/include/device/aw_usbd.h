/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/


#ifndef __AW_USBD_H
#define __AW_USBD_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usbd
 * \copydoc aw_usbd.h
 * @{
 */
#include "aw_list.h"
#include "aw_usb_common.h"
#include "aw_usb_mem.h"
#include "aw_usb_os.h"
#include "device/awbl_usbd.h"


/** \brief 管道方向定义 */
#define AW_PIPE_DIR_IN      AW_USB_DIR_IN       /**< \brief IN : to host */
#define AW_PIPE_DIR_OUT     AW_USB_DIR_OUT      /**< \brief OUT: to device */


/** \brief 管道类型定义 */
#define AW_PIPE_TYPE_CTRL   AW_USB_EP_TYPE_CTRL     /**< \brief 控制管道 */
#define AW_PIPE_TYPE_ISO    AW_USB_EP_TYPE_ISO      /**< \brief 等时管道 */
#define AW_PIPE_TYPE_BULK   AW_USB_EP_TYPE_BULK     /**< \brief 批量管道 */
#define AW_PIPE_TYPE_INT    AW_USB_EP_TYPE_INT      /**< \brief 中断管道 */


/** \brief USBD传输结构定义 */
struct aw_usbd_trans {
    struct aw_usbd_pipe     *p_pipe;    /**< \brief 管道指针 */
    struct awbl_usbd_trans   tr;        /**< \brief 传输结构 */
};




/** \brief USBD管道信息结构定义 */
struct aw_usbd_pipe_info {
    uint8_t  num;       /**< \brief 端点地址    0为自动分配 */
    uint8_t  dir;       /**< \brief 端点方向    1:IN 0:OUT */
    uint8_t  attr;      /**< \brief 传输类型和补充信息 */
    uint8_t  inter;     /**< \brief 中断和等时端点的服务时距 */
    uint16_t mps;       /**< \brief 端点每个事务允许最大数据字节数 */
};




/** \brief USBD管道类型定义 */
struct aw_usbd_pipe {
    struct awbl_usbd_ep             *p_hw;      /**< \brief 端点指针 */
    const struct aw_usbd_pipe_info  *p_info;    /**< \brief 管道信息指针 */
    struct aw_usbd_fun              *p_fun;     /**< \brief 接口指针 */
    uint8_t                          addr;      /**< \brief 管道地址 */
    struct aw_list_head              node;      /**< \brief 节点 */
    aw_usb_sem_handle_t              sync;
};



/* 接口联合描述符*/
struct awbl_usbd_iad {
    uint8_t  first_if;      /* 第一个接口编号*/
    uint8_t  if_count;      /* 接口数量*/
    uint8_t  fun_class;     /* 接口类*/
    uint8_t  fun_subclass;  /* 接口子类*/
    uint8_t  fun_protocol;  /* 接口协议*/
    uint8_t  ifunc;
    aw_bool_t used;         /* 是否被使用*/
};


/** \brief USBD接口信息 */
struct aw_usbd_fun_info {
    uint8_t       clss;         /**< \brief 类代码 */
    uint8_t       sub_clss;     /**< \brief 子类代码 */
    uint8_t       protocol;     /**< \brief 协议代码 */

    /** \brief 替代设置回调 */
    int (*pfn_alt_set) (struct aw_usbd_fun  *p_fun,
                        aw_bool_t            set);

    /** \brief setup控制传输回调 */
    int (*pfn_ctrl_setup) (struct aw_usbd_fun    *p_fun,
                           struct aw_usb_ctrlreq *p_setup,
                           void                  *p_buf,
                           int                    buf_len);
};




/** \brief USBD接口结构定义 */
struct aw_usbd_fun {
    struct aw_usbd                *p_obj;      /**< \brief USBD设备指针 */
    const char                    *name;       /**< \brief 接口名称 */
    const struct aw_usbd_fun_info *p_info;     /**< \brief 接口 信息 */
    uint8_t                        num;        /**< \brief 接口编号 */
    uint8_t                        alt;        /**< \brief 替代设置编号 */
    uint8_t                        num_alt;    /**< \brief 只有设置0有效*/
    struct aw_usbd_fun            *p_next_alt; /**< \brief 最后一个为NULL*/
    aw_bool_t                      need_iad;
    uint8_t                       *p_spec_des; /**< \brief 类自定义描述符*/
    uint8_t                        spec_size;  /**< \brief 类自定义描述符大小*/
    aw_bool_t                      set;        /**< \brief 是否设置 */
    uint8_t                        neps;       /**< \brief 端点数量 */
    struct aw_list_head            pipes;      /**< \brief 管道链表 */
    struct aw_list_head            node;       /**< \brief 节点 */
    uint8_t                        strid;      /**< \brief 字符串索引 */
};


/* 接口联合描述符最大数量*/
#define AW_USBD_MAX_IAD_NUM     10
/** \brief USBD配置结构定义 */
struct aw_usbd_cfg {
    struct aw_usbd        *p_obj;                     /**< \brief USBD设备指针 */
    struct awbl_usbd_iad   iads[AW_USBD_MAX_IAD_NUM]; /**< \brief 接口联合描述符*/
    struct aw_list_head    itfs;                      /**< \brief 接口链表 */
    uint8_t                nitfs;                     /**< \brief 接口数量 */
    uint8_t                value;                     /**< \brief 配置编号 */
    struct aw_list_head    node;                      /**< \brief 节点 */
};




/** \brief USBD信息结构定义 */
struct aw_usbd_info {
    uint8_t          clss;          /**< \brief 类代码 */
    uint8_t          sub_clss;      /**< \brief 子类代码 */
    uint8_t          protocol;      /**< \brief 协议代码 */
    uint8_t          mps0;          /**< \brief 端点0包最大尺寸 */
    uint16_t         bcd;           /**< \brief 设备版本号 */
    uint16_t         vid;           /**< \brief 厂商ID */
    uint16_t         pid;           /**< \brief 产品ID */
    const char      *manufacturer;  /**< \brief 制造商字符串描述 */
    const char      *product;       /**< \brief 产品字符串描述 */
    const char      *serial;        /**< \brief 序列号字符串描述 */
};




/** \brief USBD对象结构定义 */
struct aw_usbd {
    struct awbl_usbd           *p_usbd;     /**< \brief USBD设备指针 */
    const struct aw_usbd_info  *p_info;     /**< \brief 设备信息 */
    struct aw_usbd_cfg         *p_cur_cfg;  /**< \brief 当前配置指针 */
    struct aw_usbd_cfg          def_cfg;    /**< \brief 默认配置 */
    struct aw_list_head         cfgs;       /**< \brief 配置链表 */
    uint8_t                     ncfgs;      /**< \brief 配置数量 */
    const struct aw_usbd_cb    *p_cb;       /**< \brief 设备类回调函数列表 */
    aw_usb_mutex_handle_t       lock;
    uint8_t                     status;     /**< \brief 设备状态 */
    uint8_t                     strid_mf;   /**< \brief 厂商字符串索引 */
    uint8_t                     strid_pd;   /**< \brief 产品字符串索引 */
    uint8_t                     strid_sn;   /**< \brief 序列号字符串索引 */
};




/** \brief USB设备类回调函数列表 */
struct aw_usbd_cb {

    /** \brief 控制器链接到设备 */
    int (*pfn_link) (struct aw_usbd *p_obj);

    /** \brief 控制器和设备断开链接 */
    int (*pfn_unlink) (struct aw_usbd *p_obj);

    /** \brief 接收到SETUP包 */
    int (*pfn_ctrl_setup) (struct aw_usbd        *p_obj,
                           struct aw_usb_ctrlreq *p_setup,
                           void                  *p_buf,
                           int                    buf_len);

    /** \brief 设置或清除配置 */
    int (*pfn_config_set) (struct aw_usbd      *p_obj,
                           struct aw_usbd_cfg  *p_cfg,
                           aw_bool_t            set);

    /** \brief 总线挂起 */
    void (*pfn_suspend) (struct aw_usbd *p_obj);

    /** \brief 总线恢复 */
    void (*pfn_resume) (struct aw_usbd  *p_obj);

    /** \brief 总线断开连接 */
    void (*pfn_disconnect) (struct aw_usbd *p_obj);
};




/**
 * \brief USB device对象初始化函数
 *
 * \param[in] p_obj      USBD对象指针
 * \param[in] p_info     USBD设备信息结构指针
 * \param[in] p_cb       USBD回调函数列表指针
 * \param[in] controller USB device控制器名称
 *
 * \retval  AW_OK       初始化成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_ENODEV  设备不存在
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_init (struct aw_usbd             *p_obj,
                  const struct aw_usbd_info  *p_info,
                  const struct aw_usbd_cb    *p_cb,
                  const char                 *controller);



/**
 * \brief USB device创建配置
 *
 * \param[in] p_obj      USBD对象指针
 *
 * \retval  AW_OK       创建成功
 * \retval  -AW_EINVAL  无效参数
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_ENOMEM  存储空间不足
 */
int aw_usbd_cfg_create (struct aw_usbd  *p_obj);




/**
 * \brief USB device初始化接口
 *
 * \param[in] p_fun      USBD接口指针
 * \param[in] name       接口名称
 * \param[in] p_info     接口信息指针
 *
 * \retval  AW_OK       初始化成功
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_fun_init (struct aw_usbd_fun            *p_fun,
                      const char                    *name,
                      const struct aw_usbd_fun_info *p_info);




/**
 * \brief 添加接口到指定配置
 *
 * \param[in] p_obj      USBD对象指针
 * \param[in] value      配置编号
 * \param[in] p_fun      USBD接口指针
 *
 * \retval  AW_OK       添加成功
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_ENOENT  对应配置不存在
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_fun_add (struct aw_usbd     *p_obj,
                     uint8_t             value,
                     struct aw_usbd_fun *p_fun);




/*
 * 需要在aw_usbd_fun_add后调用，因为某些spec_des需要使用接口号等信息
 */
int aw_usbd_fun_spec_desc_add (struct aw_usbd_fun *p_fun,
                               uint8_t            *p_desc,
                               uint8_t             size);

/**
 *  添加iad联合描述符
 *  需要在aw_usbd_fun_add后调用该函数
 */
int aw_usbd_fun_iad_create(struct aw_usbd_fun *p_fun,
                           uint8_t             cfg_value,
                           uint8_t             count);



/**
 * 为接口添加altsetting
 * 
 */
int aw_usbd_fun_altset_add(struct aw_usbd_fun *p_def_fun,
                           struct aw_usbd_fun *p_alt_fun,
                           uint8_t             cfg_value,
                           uint8_t             alt_value);


/**
 * \brief 从指定配置移除一个接口
 *
 * \param[in] p_obj      USBD对象指针
 * \param[in] value      配置编号
 * \param[in] p_fun      USBD接口指针
 *
 * \retval  AW_OK       移除成功
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_ENOENT  对应配置不存在
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_fun_del (struct aw_usbd     *p_obj,
                     uint8_t             value,
                     struct aw_usbd_fun *p_fun);




/**
 * \brief 创建并添加一个管道到指定接口
 *
 * \param[in] p_fun      USBD接口指针
 * \param[in] p_pipe     USBD管道指针
 * \param[in] p_info     USBD管道信息
 *
 * \retval  AW_OK       创建添加成功
 * \retval  -AW_EBUSY   设备忙
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_pipe_create (struct aw_usbd_fun              *p_fun,
                         struct aw_usbd_pipe             *p_pipe,
                         const struct aw_usbd_pipe_info  *p_info);



/**
 * \brief 启动USB设备
 *
 * \param[in] p_obj      USB设备对象指针
 *
 * \retval  AW_OK       启动成功
 * \retval  -AW_ENODEV  设备不存在
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_start (struct aw_usbd *p_obj);



/**
 * \brief 停止USB设备
 *
 * \param[in] p_obj      USB设备对象指针
 *
 * \retval  AW_OK       停止成功
 * \retval  -AW_ENODEV  设备不存在
 * \retval  -AW_EINVAL  无效参数
 */
int aw_usbd_stop (struct aw_usbd *p_obj);




/**
 * \brief 填充一个传输结构
 *
 * \param[in] p_trans   USB传输结构指针
 * \param[in] p_pipe    USB管道指针
 * \param[in] p_buf     传输内存首地址
 * \param[in] len       数据字节长度
 * \param[in] flag      flasg标志
 * \param[in] complete  数据传输完成回调
 * \param[in] p_arg     回调函数参数
 *
 * \return  无
 */
void aw_usbd_fill_trans (
            struct aw_usbd_trans  *p_trans,
            struct aw_usbd_pipe   *p_pipe,
            void                  *p_buf,
            size_t                 len,
            int                    flag,
            void                 (*complete) (void *p_arg),
            void                  *p_arg);




/**
 * \brief USB数据异步传输
 *
 * \param[in] p_obj      USB设备对象指针
 * \param[in] p_trans    USB传输结构指针
 *
 * \retval  AW_OK         传输提交完成
 * \retval  -AW_ENODEV    设备不存在
 * \retval  -AW_EINVAL    无效参数
 * \retval  -AW_ESHUTDOWN usb设备没有运行
 */
int aw_usbd_trans_async (struct aw_usbd        *p_obj,
                         struct aw_usbd_trans  *p_trans);




/**
 * \brief 取消USB数据传输
 *
 * \param[in] p_obj      USB设备对象指针
 * \param[in] p_trans    USB传输结构指针
 *
 * \retval  AW_OK         传输取消成功
 * \retval  -AW_ENODEV    设备不存在
 * \retval  -AW_EINVAL    无效参数
 * \retval  -AW_ESHUTDOWN usb设备没有运行
 */
int aw_usbd_trans_cancel (struct aw_usbd        *p_obj,
                          struct aw_usbd_trans  *p_trans);




/**
 * \brief USB数据同步传输
 *
 * \param[in] p_obj      USB设备对象指针
 * \param[in] p_pipe     USB管道指针
 * \param[in] p_buf      传输内存首地址
 * \param[in] len        数据字节长度
 * \param[in] flag       flasg标志
 * \param[in] timeout    传输超时时间（毫秒）
 *
 * \retval  -AW_ENODEV    设备不存在
 * \retval  -AW_EINVAL    无效参数
 * \retval  -AW_ESHUTDOWN usb设备没有运行
 * \retval  >= 0          已传输的字节数
 */
int aw_usbd_trans_sync (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe,
                        void                 *p_buf,
                        size_t                len,
                        int                   flag,
                        int                   timeout);




/**
 * \brief USB设置和清除STALL握手包
 *
 * \param[in] p_obj      USB设备对象指针
 * \param[in] p_pipe     USB管道指针
 * \param[in] set        是否设置
 *
 * \retval  AW_OK         配置成功
 * \retval  -AW_EINVAL    无效参数
 */
int aw_usbd_pipe_stall (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe,
                        aw_bool_t             set);




/**
 * \brief USB管道复位
 *
 * \param[in] p_obj      USB设备对象指针
 * \param[in] p_pipe     USB管道指针
 *
 * \retval  AW_OK         复位成功
 * \retval  -AW_EINVAL    无效参数
 */
int aw_usbd_pipe_reset (struct aw_usbd       *p_obj,
                        struct aw_usbd_pipe  *p_pipe);




/**
 * \brief 判断USB设备是否有效
 *
 * \param[in] p_obj      USB设备对象指针
 *
 * \retval  AW_TRUE      设备有效
 * \retval  AW_FALSE     设备无效
 */
aw_bool_t aw_usbd_valid (struct aw_usbd  *p_obj);




/**
 * \brief 判断USB接口功能是否有效
 *
 * \param[in] p_fun      USB接口对象指针
 *
 * \retval  AW_TRUE      接口功能有效
 * \retval  AW_FALSE     接口功能无效
 */
aw_bool_t aw_fun_valid (struct aw_usbd_fun  *p_fun);


/** @} grp_aw_if_usbd */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USBD_H */

/* end of file */
