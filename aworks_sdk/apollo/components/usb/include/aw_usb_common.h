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


#ifndef __AW_USB_COMMON_H
#define __AW_USB_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usb_common
 * \copydoc aw_usb_common.h
 * @{
 */


/** \brief USB传输方向 */
#define AW_USB_DIR_OUT                 0            /**< \brief OUT: to device */
#define AW_USB_DIR_IN                  0x80         /**< \brief IN : to host */


/** \brief USB请求类型 */
#define AW_USB_REQ_TYPE_MASK           (0x03 << 5)
#define AW_USB_REQ_TYPE_STANDARD       (0x00 << 5)  /**< \brief USB标准请求 */
#define AW_USB_REQ_TYPE_CLASS          (0x01 << 5)  /**< \brief USB类请求 */
#define AW_USB_REQ_TYPE_VENDOR         (0x02 << 5)  /**< \brief USB自定义请求 */
#define AW_USB_REQ_TYPE_RESERVED       (0x03 << 5)  /**< \brief 保留 */


/** \brief USB请求目标 */
#define AW_USB_REQ_TAG_MASK               0x1f
#define AW_USB_REQ_TAG_DEVICE             0x00      /**< \brief 设备 */
#define AW_USB_REQ_TAG_INTERFACE          0x01      /**< \brief 接口 */
#define AW_USB_REQ_TAG_ENDPOINT           0x02      /**< \brief 端点 */
#define AW_USB_REQ_TAG_OTHER              0x03      /**< \brief 其他 */
#define AW_USB_REQ_TAG_PORT               0x04      /**< \brief 端口 */
#define AW_USB_REQ_TAG_RPIPE              0x05      /**< \brief 管道 */


/** \brief USB请求 */
#define AW_USB_REQ_GET_STATUS             0x00      /**< \brief 获取状态 */
#define AW_USB_REQ_CLEAR_FEATURE          0x01      /**< \brief 清除特性 */
#define AW_USB_REQ_SET_FEATURE            0x03      /**< \brief 设置特性 */
#define AW_USB_REQ_SET_ADDRESS            0x05      /**< \brief 设置地址 */
#define AW_USB_REQ_GET_DESCRIPTOR         0x06      /**< \brief 获取描述符 */
#define AW_USB_REQ_SET_DESCRIPTOR         0x07      /**< \brief 设置描述符 */
#define AW_USB_REQ_GET_CONFIGURATION      0x08      /**< \brief 获取配置 */
#define AW_USB_REQ_SET_CONFIGURATION      0x09      /**< \brief 设置配置 */
#define AW_USB_REQ_GET_INTERFACE          0x0A      /**< \brief 获取接口信息 */
#define AW_USB_REQ_SET_INTERFACE          0x0B      /**< \brief 设置接口 */
#define AW_USB_REQ_SYNCH_FRAME            0x0C      /**< \brief 同步帧 */

#define AW_USB_REQ_SET_ENCRYPTION         0x0D      /**< \brief 设置加密 */
#define AW_USB_REQ_GET_ENCRYPTION         0x0E      /**< \brief 获取加密 */
#define AW_USB_REQ_RPIPE_ABORT            0x0E      /**< \brief RPipe中止 */
#define AW_USB_REQ_SET_HANDSHAKE          0x0F      /**< \brief 设置握手 */
#define AW_USB_REQ_RPIPE_RESET            0x0F      /**< \brief RPipe复位 */
#define AW_USB_REQ_GET_HANDSHAKE          0x10      /**< \brief 获取握手 */
#define AW_USB_REQ_SET_CONNECTION         0x11      /**< \brief 设置连接 */
#define AW_USB_REQ_SET_SECURITY_DATA      0x12      /**< \brief 设置加密数据 */
#define AW_USB_REQ_GET_SECURITY_DATA      0x13      /**< \brief 获取加密数据 */
#define AW_USB_REQ_SET_WUSB_DATA          0x14      /**< \brief 配置无线USB数据 */
#define AW_USB_REQ_LOOPBACK_DATA_WRITE    0x15      /**< \brief 回环数据写 */
#define AW_USB_REQ_LOOPBACK_DATA_READ     0x16      /**< \brief 回环数据读 */
#define AW_USB_REQ_SET_INTERFACE_DS       0x17      /**< \brief SET_INTERFACE_DS请求 */
#define AW_USB_REQ_SET_SEL                0x30      /**< \brief SET_SEL请求 */
#define AW_USB_REQ_SET_ISOCH_DELAY        0x31      /**< \brief SET_ISOCH_DELAY请求 */


/** \brief USB设备速度定义 */
#define AW_USB_SPEED_UNKNOWN           0
#define AW_USB_SPEED_LOW               1       /**< \brief 低速(usb 1.1) */
#define AW_USB_SPEED_FULL              2       /**< \brief 全速(usb 1.1) */
#define AW_USB_SPEED_HIGH              3       /**< \brief 高速(usb 2.0) */
#define AW_USB_SPEED_WIRELESS          4       /**< \brief 无线USB(usb 2.5) */
#define AW_USB_SPEED_SUPER             5       /**< \brief 超高速(usb 3.0) */


/** \brief USB设备类定义 */
#define AW_USB_CLASS_PER_INTERFACE        0x00  /**< \brief 接口描述中定义 */
#define AW_USB_CLASS_AUDIO                0x01  /**< \brief 音频设备类 */
#define AW_USB_CLASS_COMM                 0x02  /**< \brief 通讯设备类 */
#define AW_USB_CLASS_HID                  0x03  /**< \brief 人机交互设备类 */
#define AW_USB_CLASS_PHYSICAL             0x05  /**< \brief 物理设备类 */
#define AW_USB_CLASS_STILL_IMAGE          0x06  /**< \brief 图像设备类 */
#define AW_USB_CLASS_PRINTER              0x07  /**< \brief 打印机设备类 */
#define AW_USB_CLASS_MASS_STORAGE         0x08  /**< \brief 大容量存储类 */
#define AW_USB_CLASS_HUB                  0x09  /**< \brief 集线器类 */
#define AW_USB_CLASS_CDC_DATA             0x0a  /**< \brief CDC-Date通信设备类 */
#define AW_USB_CLASS_CSCID                0x0b  /**< \brief 智能卡设备类 */
#define AW_USB_CLASS_CONTENT_SEC          0x0d  /**< \brief 信息安全设备类 */
#define AW_USB_CLASS_VIDEO                0x0e  /**< \brief 视频设备类 */
#define AW_USB_CLASS_WIRELESS_CONTROLLER  0xe0  /**< \brief 无线控制器类 */
#define AW_USB_CLASS_MISC                 0xef  /**< \brief 杂项类 */
#define AW_USB_CLASS_APP_SPEC             0xfe  /**< \brief 应用专有规范 */
#define AW_USB_CLASS_VENDOR_SPEC          0xff  /**< \brief 厂商自定义类 */


/** \brief USB端点类型 */
#define AW_USB_EP_TYPE_MASK            0x03
#define AW_USB_EP_TYPE_CTRL            0x00     /**< \brief 控制端点 */
#define AW_USB_EP_TYPE_ISO             0x01     /**< \brief 等时端点 */
#define AW_USB_EP_TYPE_BULK            0x02     /**< \brief 批量端点 */
#define AW_USB_EP_TYPE_INT             0x03     /**< \brief 中断端点 */


/** \brief USB描述符类型定义 */
#define AW_USB_DT_DEVICE                  0x01  /**< \brief 设备描述符 */
#define AW_USB_DT_CONFIG                  0x02  /**< \brief 配置描述符 */
#define AW_USB_DT_STRING                  0x03  /**< \brief 字符串描述符 */
#define AW_USB_DT_INTERFACE               0x04  /**< \brief 接口描述符 */
#define AW_USB_DT_ENDPOINT                0x05  /**< \brief 端点描述符(0端点外)*/
#define AW_USB_DT_DEVICE_QUALIFIER        0x06  /**< \brief 设备限定描述符 */
#define AW_USB_DT_OTHER_SPEED_CONFIG      0x07  /**< \brief 其他速率配置描述符 */
#define AW_USB_DT_INTERFACE_POWER         0x08  /**< \brief 接口功耗描述符 */
#define AW_USB_DT_OTG                     0x09  /**< \brief OTG描述符 */
#define AW_USB_DT_DEBUG                   0x0a  /**< \brief 调试描述符 */
#define AW_USB_DT_INTERFACE_ASSOCIATION   0x0b  /**< \brief 接口联合描述符 */
#define AW_USB_DT_SECURITY                0x0c  /**< \brief 安全性描述符 */
#define AW_USB_DT_KEY                     0x0d  /**< \brief 密码钥匙描述符 */
#define AW_USB_DT_ENCRYPTION_TYPE         0x0e  /**< \brief 加密类型描述符 */
#define AW_USB_DT_BOS                     0x0f  /**< \brief 二进制设备目标存储 */
#define AW_USB_DT_DEVICE_CAPABILITY       0x10  /**< \brief 设备性能描述符 */
#define AW_USB_DT_WIRELESS_ENDPOINT_COMP  0x11  /**< \brief 无线端点伙伴 */
#define AW_USB_DT_WIRE_ADAPTER            0x21  /**< \brief DWA描述符 */
#define AW_USB_DT_RPIPE                   0x22  /**< \brief RPipe描述符 */
#define AW_USB_DT_CS_RADIO_CONTROL        0x23  /**< \brief Radio Control描述符 */
#define AW_USB_DT_PIPE_USAGE              0x24  /**< \brief Pipe Usage描述符 */
#define AW_USB_DT_SS_ENDPOINT_COMP        0x30  /**< \brief 超高速端点伙伴 */


/** \brief USB设备特性定义 */
#define AW_USB_DEV_SELF_POWERED        0   /**< \brief 自供电 */
#define AW_USB_DEV_REMOTE_WAKEUP       1   /**< \brief 支持远程唤醒 */


/** \brief USB集线器特性设置 */
#define AW_USB_HUB_C_LOCAL_POWER      0
#define AW_USB_HUB_C_OVER_CURRENT     1
#define AW_USB_PORT_CONNECTION        0
#define AW_USB_PORT_ENABLE            1
#define AW_USB_PORT_SUSPEND           2
#define AW_USB_PORT_OVER_CURRENT      3
#define AW_USB_PORT_RESET             4
#define AW_USB_PORT_POWER             8
#define AW_USB_PORT_LOW_SPEED         9
#define AW_USB_PORT_C_CONNECTION      16
#define AW_USB_PORT_C_ENABLE          17
#define AW_USB_PORT_C_SUSPEND         18
#define AW_USB_PORT_C_OVER_CURRENT    19
#define AW_USB_PORT_C_RESET           20
#define AW_USB_PORT_C_TEST            21
#define AW_USB_PORT_C_INDICATOR       22

/* USB人体接口设备接口子类和协议代码*/
#define AW_USB_INTERFACE_SUBCLASS_BOOT     1
#define AW_USB_INTERFACE_PROTOCOL_KEYBOARD 1
#define AW_USB_INTERFACE_PROTOCOL_MOUSE    2

/**
 * \brief USB字节序转换
 */
#if (AW_CPU_ENDIAN == AW_BIG_ENDIAN)
#define AW_USB_CPU_TO_LE32(x) \
            x = ((((x) & 0xff000000) >> 24) | \
             (((x) & 0x00ff0000) >> 8) | \
             (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24))

#define AW_USB_CPU_TO_LE16(x) \
            x =(uint16_t) ((((x) & 0xff00) >> 8) | (((x) & 0x00ff) << 8))
#else
#define AW_USB_CPU_TO_LE32(x) (x)
#define AW_USB_CPU_TO_LE16(x) (x)
#endif

#define AW_USB_LE_WRITE32(val, addr)    writel(AW_USB_CPU_TO_LE32(val), \
                                                (volatile void __iomem *)(addr))
#define AW_USB_LE_READ32(addr)          AW_USB_CPU_TO_LE32( \
                                            readl((volatile void __iomem *)(addr)))


#pragma pack (push, 1)


/**
 * \brief SETUP包结构定义
 */
struct aw_usb_ctrlreq {
    uint8_t     bRequestType;   /**< \brief 数据流方向、请求类型、接收端等 */
    uint8_t     bRequest;       /**< \brief 确认请求 */
    uint16_t    wValue;         /**< \brief 请求专属信息 */
    uint16_t    wIndex;         /**< \brief 请求专属信息 */
    uint16_t    wLength;        /**< \brief 数据阶段数据长度 */
};



/**
 * \brief USB描述符头部
 */
struct aw_usb_desc_head {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 描述符类型 */
};




/**
 * \brief USB设备描述符定义
 */
struct aw_usb_device_desc {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 常（01h） */

    uint16_t    bcdUSB;             /**< \brief USB规范版本号 */
    uint8_t     bDeviceClass;       /**< \brief 类代码 */
    uint8_t     bDeviceSubClass;    /**< \brief 子类代码 */
    uint8_t     bDeviceProtocol;    /**< \brief 协议代码 */
    uint8_t     bMaxPacketSize0;    /**< \brief 端点0包最大尺寸 */
    uint16_t    idVendor;           /**< \brief 厂商ID */
    uint16_t    idProduct;          /**< \brief 产品ID */
    uint16_t    bcdDevice;          /**< \brief 设备版本号 */
    uint8_t     iManufacturer;      /**< \brief 制造商字符串描述符索引 */
    uint8_t     iProduct;           /**< \brief 产品字符串描述符索引 */
    uint8_t     iSerialNumber;      /**< \brief 序列号字符串描述符索引 */
    uint8_t     bNumConfigurations; /**< \brief 配置数目 */
};




/**
 * \brief USB配置描述符定义
 */
struct aw_usb_config_desc {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 常（02h） */

    uint16_t    wTotalLength;       /**< \brief 配置描述符及其全部附属描述符字节数 */
    uint8_t     bNumInterfaces;     /**< \brief 配置中接口数量 */
    uint8_t     bConfigurationValue;/**< \brief 配置编号 */
    uint8_t     iConfiguration;     /**< \brief 配置字符串描述符索引 */
    uint8_t     bmAttributes;       /**< \brief 附加特性（如：自供电、远程唤醒等） */
    uint8_t     bMaxPower;          /**< \brief 2mA单位的总线功耗 */
};




/**
 * \brief USB 设备限定描述符定义
 */
struct aw_usb_qualifier_desc {
    uint8_t  bLength;               /**< \brief 描述符字节长度 */
    uint8_t  bDescriptorType;       /**< \brief 常（06h） */

    uint16_t bcdUSB;                /**< \brief USB规范版本号 */
    uint8_t  bDeviceClass;          /**< \brief 类代码 */
    uint8_t  bDeviceSubClass;       /**< \brief 子类代码 */
    uint8_t  bDeviceProtocol;       /**< \brief 协议代码 */
    uint8_t  bMaxPacketSize0;       /**< \brief 端点0包最大尺寸 */
    uint8_t  bNumConfigurations;    /**< \brief 配置数目 */
    uint8_t  bRESERVED;             /**< \brief 保留 */
};




/**
 * \brief USB接口联合描述符定义
 */
struct aw_usb_interface_assoc_desc {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 常（0Bh） */

    uint8_t     bFirstInterface;    /**< \brief 功能联合的第一个接口的编号 */
    uint8_t     bInterfaceCount;    /**< \brief 功能联合的接口的数量 */
    uint8_t     bFunctionClass;     /**< \brief 类代码 */
    uint8_t     bFunctionSubClass;  /**< \brief 子类代码 */
    uint8_t     bFunctionProtocol;  /**< \brief 协议代码 */
    uint8_t     iFunction;          /**< \brief 功能字符串描述符索引 */
};




/**
 * \brief USB接口描述符定义
 */
struct aw_usb_interface_desc {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 常（04h） */

    uint8_t     bInterfaceNumber;   /**< \brief 接口编号 */
    uint8_t     bAlternateSetting;  /**< \brief 替代设置编号 */
    uint8_t     bNumEndpoints;      /**< \brief 所支持的端点数目（除端点0外） */
    uint8_t     bInterfaceClass;    /**< \brief 类代码 */
    uint8_t     bInterfaceSubClass; /**< \brief 子类代码 */
    uint8_t     bInterfaceProtocol; /**< \brief 协议代码 */
    uint8_t     iInterface;         /**< \brief 接口字符串描述符索引 */
};




/**
 * \brief USB端点描述符定义
 */
struct aw_usb_endpoint_desc {
    uint8_t     bLength;            /**< \brief 描述符字节长度 */
    uint8_t     bDescriptorType;    /**< \brief 常（05h） */

    uint8_t     bEndpointAddress;   /**< \brief 端点地址和方向 */
    uint8_t     bmAttributes;       /**< \brief 补充信息（如传输类型） */
    uint16_t    wMaxPacketSize;     /**< \brief 所支持的最大包尺寸
                                                                                                                                                第12，11位表明了每微帧的事务数量
                                                00(每微帧1个事务)
                                                01(每微帧2个事务)
                                                10(每微帧3个事务)
                                                11(保留)
                                                                                                                                                低11位指明了每个单独事务的有效数据字节数*/
    uint8_t     bInterval;          /**< \brief 轮询端点的时间间隔
                                                                                                                              根据设备运行速度以帧(全/低速)或微帧(高速)表示单位(1ms或125us)
                                                                                                                              全/高速同步端点，这个值必须在1~16，轮询端点的时间为2^(binterval-1)微帧
                                                                                                                              全/低速中断端点，这个值必须在1~255个帧
                                                                                                                              高速中断端点，这个值必须在1——16，轮询端点的时间为2^(binterval-1)微帧
                                                                                                                              高速批量/控制输出端点，binterval指定端点的最大NAK率，0表示没有NAKs
                                                                                                                              其他值表示每binterval微帧里最多一个NAK，这个值必选0~255*/
};



#pragma pack (pop)


/** @} grp_aw_if_usb_common */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USB_COMMON_H */

/* end of file */
