/**
 * \file device-flags.h
 * Special device flags to deal with bugs in specific devices.
 *
 * Copyright (C) 2005-2007 Richard A. Low <richard@wentnet.com>
 * Copyright (C) 2005-2012 Linus Walleij <triad@df.lth.se>
 * Copyright (C) 2006-2007 Marcus Meissner
 * Copyright (C) 2007 Ted Bullock
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 *
 * This file is supposed to be included by both libmtp and libgphoto2.
 */

/**
 * 这些标记用于指示某些或其他设备是否需要特殊处理。这些应该可以使用逻辑或衔接起来，所以
 * 请为每个功能使用一个位，并祈祷我们不需要超过32位。
 */
#define DEVICE_FLAG_NONE 0x00000000
/**
 * 这意味着，当你请求对象0xFFFFFFFF的对象属性链表时，如果参数3同样设置为0xFFFFFFFF，则
 * PTP_OC_MTP_GetObjPropList不会返回设备上所有文件的格式正确的元数据。与
 * DEVICE_FLAG_BROKEN_MTPGETOBJECTPROPLIST相比，
 * DEVICE_FLAG_BROKEN_MTPGETOBJECTPROPLIST只表示它在获取单个对象的元数据时被破坏。
 * 实现可能被破坏的一个典型方式是，它可能无法返回对象的正确计数，并且有时候(例如在ZENs上)如果使用
 * 这个方法，则对象只是从列表中丢失。有时它被错误地用来掩盖代码中的错误(例如处理大小为-1(0xFFFFFFFFU))的
 * 数据事务，在这种情况下，请删除它，现在代码是固定的。有时使用这个命令是因为获取所有对象太慢，如果使用
 * 这个命令。USB事务将超时。
 */
#define DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST_ALL 0x00000001
/**
 * 这意味着在Linux下，另一个内核模块可能正在使用这个设备的USB接口，因此如果是这样，我们需要
 * 将其分离。通常这是在双模设备上，该设备将同时显示符合MTP接口和设备描述符和一个USB大容量存储
 * 接口。如果USB大容量存储接口在使用中，其他apps(例如我们用户空间的libmtp通过libusb访问路径)
 * 不能使用它。所以我们可以移除不合法的应用。通常这意味着你可能得以root用户运行这个程序。
 */
#define DEVICE_FLAG_UNLOAD_DRIVER 0x00000002
/**
 * 这意味着PTP_OC_MTP_GetObjPropList(9805)在某种程度上被破坏了，或者
 * 它根本不工作(对于android设备)，或者如果参数3设置为0xFFFFFFFFU，它将不能
 * 正确地返回所有对象属性。
 */
#define DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST 0x00000004
/**
 * 这意味着当传输边界是64字节的倍数(USB 1.1 端点大小)时，设备不会发送0数据包来指示传输结束。
 * 相反，如果大小是USB 1.1 端点大小(64字节)的整数倍，则在传输结束时只发送一个额外字节。
 *
 * 这种行为很可能是一种解决方法，因为设备找那个的硬件USB从机控制器根本不能处理0写操作，而且USB
 * 1.1 端点大小的使用是因为设备在USB 1.1集线器上“减速”，因为64字节是512字节的倍数，它可以
 * 在USB1.1 和USB2.0 上工作。
 */
#define DEVICE_FLAG_NO_ZERO_READS 0x00000008
/**
 * 这个标志表示设备容易忘记OGG容器文件类型，因此libmtp必须查看文件扩展名才能确定文件实际
 * 上是OGG。这是一个清除的当前固件的错误，虽然固件错误应该在固件中修复，但我们非常喜欢OGG，
 * 因此我们引入这个标志来支持它。这个错误只有在iriver设备上才能看到。打开这个标志不会有任
 * 影响，只要对“未知”文件进行文件扩展名检查即可。如果播放器甚至不知道(报告)它支持ogg，即使
 * 它支持，请使用最强大的OGG_IS_UNKNOWN标志，它将强制支持任何扩展名为.ogg的对象上的ogg。
 */
#define DEVICE_FLAG_IRIVER_OGG_ALZHEIMER 0x00000010
/**
 * 这个标志指明了设备可以接受的文件名的限制-它们必须是7位(所有字符<=127/0x7F)。
 * Philips Shoqbox上第一次发现这个问题，PTP标准要求任何unicode字符都可以用于
 * 文件名。我猜这是由于设备本质上使用7位文件系统导致的。
 */
#define DEVICE_FLAG_ONLY_7BIT_FILENAMES 0x00000020
/**
 * 这个标志表示如果在关闭设备时尝试获取端点的状态和/或释放接口，则设备将锁定。这特别解决了
 * SanDisk Sansa设备的问题。这可能是Windows从不释放接口的行为的副作用。
 */
#define DEVICE_FLAG_NO_RELEASE_INTERFACE 0x00000040
/**
 * 这个标志是随着 ZEN 8GB的出现而引入的。设备有时会返回一个坏的PTP头部，像这样：
 * < 1502 0000 0200 01d1 02d1 01d2 >后6个字节(表示“代码”和“事务ID”)包含
 * 垃圾。这违反了PTP/MTP规范，但仍然使用于Windows，可能是因为Windows没有检查这些
 * 字节是否有效。要与这样的设备进行操作，我们需要这个标志来模拟Windows错误。在Aricent
 * MTP协议栈里也发现了坏的头。
 */
#define DEVICE_FLAG_IGNORE_HEADER_ERRORS 0x00000080
/**
 * 摩托罗拉RAZR2 V8(其他？)已断开设置对象属性列表，导致元数据设置失败。(设
 * 置对象属性以设置单个属性的设置在该设备上有效，但元数据在追踪上会被忽略，尽管可以设
 * 置播放列表名称)
 */
#define DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST 0x00000100
/**
 * Samsung YP-T10认为Ogg文件应该以“unknown”(PTP_OFC_Undefined)文件
 * 类型发送，这会产生一个副作用，即iRiver Ogg Alzheimer问题(必须在文件扩展名上
 * 识别Ogg文件)和需要报告Ogg支持(设备本身并不正确声明支持它)以及在存储Ogg文件时需
 * 要将文件类型设备为未知，即使它们实际上并不是未知。后来iRivers似乎需要这个标志，因
 * 为它们不报告支持Ogg，即使它们真的支持Ogg。通常，设备在USB大容量存储模式下支持Ogg，
 * 那么固件只是没有正确声明元数据支持Ogg。
 */
#define DEVICE_FLAG_OGG_IS_UNKNOWN 0x00000200
/**
 * Creative Zen在libmtp中是相当不稳定的，但是在更高的固件版本中似乎更难实现。
 * 但是，在设置相册艺术尺寸时，它仍然经常崩溃。此标志禁止设置尺寸(这似乎与图形的显示方式无关)。
 */
#define DEVICE_FLAG_BROKEN_SET_SAMPLE_DIMENSIONS 0x00000400
/**
 * 一些设备，特别是SanDisk Sansas，需要始终检测其“OS描述符”才能正常工作
 */
#define DEVICE_FLAG_ALWAYS_PROBE_DESCRIPTOR 0x00000800
/**
 * Samsung已经实现了自己的播放列表格式，作为存储在普通文件系统中的.spl文件，而不是正确的
 * mtp播放列表。.spl格式有多个版本，由文件名：版本 X.XX
 * 版本1.00只是一个简单的播放列表
 */
#define DEVICE_FLAG_PLAYLIST_SPL_V1 0x00001000
/**
 * Samsung已经实现了自己的播放列表格式，作为存储在普通文件系统中的.spl文件，而不是正确的
 * mtp播放列表。.spl格式有多个版本，由文件名：版本 X.XX
 * 版本2.00是一个播放列表，但允许每个播放列表存储DNSe声音设置
 */
#define DEVICE_FLAG_PLAYLIST_SPL_V2 0x00002000
/**
 * Sansa E250已知存在这个问题，实际上是设备声称属性PTP_OPC_DateModified是可读/可写
 * 但仍然无法更新它。只有在第一次发送文件时才能正确设置。
 */
#define DEVICE_FLAG_CANNOT_HANDLE_DATEMODIFIED 0x00004000
/**
 * 这样可以避免使用发送对象属性列表，它在创建新对象(不仅仅是更新)时使
 * 用，DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST是相关
 * 的，但只涉及发送对象属性列表来更新现有对象的情况。
 * 例如Toshiba Gigabeat MEU202就有这个问题。
 */
#define DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST 0x00008000
/**
 * 设置不支持读电量
 */
#define DEVICE_FLAG_BROKEN_BATTERY_LEVEL 0x00010000
/**
 * 需要直接PTP匹配(libgphoto2)
 */
#define DEVICE_FLAG_OLYMPUS_XML_WRAPPED     0x00800000
/**
 * 这个标志与DEVICE_FLAG_OGG_IS_UNKNOWN类似，但用于FLAC文件。对于
 * FLAC文件使用未知文件类型
 */
#define DEVICE_FLAG_FLAC_IS_UNKNOWN     0x01000000
/**
 * 设备需要独一无二的文件名，没有两个文件可以命名相同的字符串。
 */
#define DEVICE_FLAG_UNIQUE_FILENAMES        0x02000000
/**
 * 这个标志在BlackBerry设备上执行一些magic，从USB大容量存储切换到我们认为的MTP模式
 */
#define DEVICE_FLAG_SWITCH_MODE_BLACKBERRY  0x04000000
/**
 * 这个标志指明了设备在一些操作上需要一个额外的长时间超时
 */
#define DEVICE_FLAG_LONG_TIMEOUT        0x08000000
/**
 * 这个标志表示设备在每次连接后都需要显示的USB复位。某些设备不喜欢这样，因此
 * 默认情况下不会这样做
 */
#define DEVICE_FLAG_FORCE_RESET_ON_CLOSE    0x10000000
/**
 * 似乎有些设备在使用GetObjectInfo操作时返回了错误的数据。所以在这种情况下，我们
 * 更喜欢用MTP property list覆盖PTP-compatible object infos。
 *
 * 例如一些Samsung Galaxy S设备包含一个MTP协议栈，该协议用64位而不是32位表
 * 示ObjectInfo
 */
#define DEVICE_FLAG_PROPLIST_OVERRIDES_OI   0x40000000
/**
 * Samsung Galaxy设备的MTP协议栈中在GetPartialObject有一个神秘的错误。当GetPartialObject被
 * 调用来读取文件的最后字节并且要读取的数据量使得应答中发送的最后一个USB数据包与USB2.0数据包大小完全匹配
 * 时，Samsung Galaxy设备将挂起，从而导致超时错误。
 */
#define DEVICE_FLAG_SAMSUNG_OFFSET_BUG      0x80000000
/**
 * 所有这些bug标志需要在SONY NWZ Walkman被设置，和会在检测到自定义扩展描
 * 述符"sony.net"的未知设备上被自动检测
 */
#define DEVICE_FLAGS_SONY_NWZ_BUGS \
  (DEVICE_FLAG_UNLOAD_DRIVER | \
   DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST | \
   DEVICE_FLAG_UNIQUE_FILENAMES | \
   DEVICE_FLAG_FORCE_RESET_ON_CLOSE)
/**
 * 所有这些bug标志都需要在android设备上设置，它们声称支持但实际上它们无法处理的
 * MTP操作，特别是9805(获取对象属性列表)。它们自动分配给在设备扩展描述符中报
 * 告“android.com”的设备。
 */
#define DEVICE_FLAGS_ANDROID_BUGS \
  (DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST | \
   DEVICE_FLAG_BROKEN_SET_OBJECT_PROPLIST | \
   DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST | \
   DEVICE_FLAG_UNLOAD_DRIVER | \
   DEVICE_FLAG_LONG_TIMEOUT | \
   DEVICE_FLAG_FORCE_RESET_ON_CLOSE)
/**
 * 所有这些bug 标志都出现在一些SonyEricsson设备上，包括没有使用Android 4.0+ MTP
 * 协议栈的Android设备。高度怀疑这些bug来自Aricent的MTP的实现，因此它被称为Aricent bug
 * 标志。对于这个协议栈来说，尤其是需要忽略的头错误是典型操作
 *
 * 经过一些猜测之后，我们自动将这些错误标志分配给提出"microsoft.com/WPDNA"和"sonyericsson.com/SE"，
 * 但不是"android.com"描述符的设备*/
#define DEVICE_FLAGS_ARICENT_BUGS \
  (DEVICE_FLAG_IGNORE_HEADER_ERRORS | \
   DEVICE_FLAG_BROKEN_SEND_OBJECT_PROPLIST | \
   DEVICE_FLAG_BROKEN_MTPGETOBJPROPLIST)
