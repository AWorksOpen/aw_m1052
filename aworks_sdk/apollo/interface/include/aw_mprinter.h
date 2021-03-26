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
 * \brief 微型打印机通用标准接口，参考EPSON手册对微型打印机大多数命令进行封装。
 *        用户可根据应用扩展特殊的指令
 * \internal
 * \par Modification History
 * - 1.00 16-04-25  bob, first implementation.
 * \endinternal
 */

#ifndef __AW_MPRINTER_H
#define __AW_MPRINTER_H

#ifdef __cplpl
extern "C" {
#endif

#include "apollo.h"
#include "aw_sem.h"


/**
 * \addtogroup am_if_mprinter
 * \copydoc aw_mprinter.h
 * @{
 */

/**
 * \name 字符打印方式（aw_mprinter_font_type_set函数传入，可多选）
 * \anchor grp_aw_mprinter_font_type
 * @{
 */

/** \brief 普通           */
#define AW_MPRINTER_FONT_TYPE_NORMAL         0x01

/** \brief 斜体           */
#define AW_MPRINTER_FONT_TYPE_ITALIC         0x02

/** \brief 边框           */
#define AW_MPRINTER_FONT_TYPE_BORDER         0x04

/** \brief 加粗           */
#define AW_MPRINTER_FONT_TYPE_EMPRINTHASIZE  0x08

/** \brief 倍高           */
#define AW_MPRINTER_FONT_TYPE_DOUBLE_HEIGHT  0x10

/** \brief 倍宽           */
#define AW_MPRINTER_FONT_TYPE_DOUBLE_WIDTH   0x20

/** \brief 反白           */
#define AW_MPRINTER_FONT_TYPE_REVERSE_VEDIO  0x40

/** \brief 下划线         */
#define AW_MPRINTER_FONT_TYPE_UNDERLINE      0x80

/** @} */

/**
 * \name 打印机状态标志位（aw_mprinter_printer_state_rt_get函数获取）
 * \anchor grp_aw_mprinter_stat_flag
 * @{
 */

/** \brief 过压                    */
#define AW_MPRINTER_STAT_FLAG_OVERVOLATGE    0x01

/** \brief 轴打开                  */
#define AW_MPRINTER_STAT_FLAG_AXOOPEN        0x02

/** \brief 缺纸，打印机没有打印纸  */
#define AW_MPRINTER_STAT_FLAG_PAPER_NONE     0x04

/** \brief 过热                    */
#define AW_MPRINTER_STAT_FLAG_OVERHEAT       0x08

/** \brief 卡纸                    */
#define AW_MPRINTER_STAT_FLAG_PAPER_STUCK    0x10

/** \brief 切刀移出                */
#define AW_MPRINTER_STAT_FLAG_CUTTERON       0x20

/** \brief 纸将用尽                */
#define AW_MPRINTER_STAT_FLAG_PAPER_LACK     0x40

/** \brief 用户未取纸              */
#define AW_MPRINTER_STAT_FLAG_PAPER_OUT      0x80

/** @} */

/**
 * \name 打印机类型ID（aw_mprinter_id_get函数获取,）
 * \anchor grp_aw_mprinter_type_id
 * @{
 */

/** \brief 支持双字节编码                 */
#define AW_MPRINTER_TYPE_ID_DOUBLE_CODE      0x01

/** \brief 支持切纸功能                   */
#define AW_MPRINTER_TYPE_ID_CUT_PAPER        0x02

/** \brief 允许BM传感器                   */
#define AW_MPRINTER_TYPE_ID_BM_SENSOR        0x04

/** @} */

/**
 * \name kanji char mode（aw_mprinter_kanjichar_pmode_select函数传入，可多选）
 * \anchor grp_aw_mprinter_kanjichar_mode
 * @{
 */

/** \brief 倍宽           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_WDT         0x04

/** \brief 倍高           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_HGT         0x08

/** \brief 下划线           */
#define AW_MPRINTER_KANJICHAR_MODE_DBL_UNDERLINE   0x80

/** @} */

/**
 * \name 打印机纸张状态（aw_mprinter_paper_stat_get函数获取）
 * \anchor grp_aw_mprinter_paper_stat
 * @{
 */

/** \brief 纸张将用完               */
#define AW_MPRINTER_PAPER_STAT_LACK       0x03

/** \brief 缺纸（打印机没有纸张）   */
#define AW_MPRINTER_PAPER_STAT_NONE       0x0c


/** @} */


/**
 * \brief 打印机回调函数类型
 */
typedef enum aw_mprinter_cbk_type {

    /** \brief 过压               */
    AW_MPRINTER_CBK_OVER_VOLTAGE = 0,

    /** \brief 过压恢复           */
    AW_MPRINTER_CBK_OVER_VOLTAGE_RESUME,

    /** \brief 压轴               */
    AW_MPRINTER_CBK_AXO_OPEN,

    /** \brief 压轴恢复           */
    AW_MPRINTER_CBK_AXO_CLOSE,

    /** \brief 过热               */
    AW_MPRINTER_CBK_OVER_HEAT,

    /** \brief 过热恢复           */
    AW_MPRINTER_CBK_OVER_HEAT_RESUME,

    /** \brief 缺纸               */
    AW_MPRINTER_CBK_OF_PAPER,

    /** \brief 缺纸恢复           */
    AW_MPRINTER_CBK_OF_PAPER_RESUME,

    /** \brief 切刀  移出         */
    AW_MPRINTER_CBK_CUTTER_ON,

    /** \brief 切刀关闭           */
    AW_MPRINTER_CBK_CUTTER_OFF,
} aw_mprinter_cbk_type_t;

/**
 * \brief 打印对齐方式枚举
 */
typedef enum aw_mprinter_align {

    AW_MPRINTER_ALIGN_LEFT  = 48, /**< \brief 居左    */
    AW_MPRINTER_ALIGN_CENTER,     /**< \brief 居中    */
    AW_MPRINTER_ALIGN_RIGHT,      /**< \brief 居右    */

} aw_mprinter_align_t;

/**
 * \brief 打印控制模式
 */
typedef enum aw_mprinter_ctrl_md {

    AW_MPRINTER_PRINT_CTRL_MODE  = 48, /**< \brief 打印机模式  */
    AW_MPRINTER_PRINT_CTRL_DENSITY,    /**< \brief 打印机密度  */
    AW_MPRINTER_PRINT_CTRL_SPEED,      /**< \brief 打印速度    */

} aw_mprinter_ctrl_md_t;

/**
 * \brief 选择国际字符集
 */
typedef enum aw_mprinter_inter_char {

    AW_MPRINTER_INTER_CHAR_USA = 0, /**< \brief 美国                 */
    AW_MPRINTER_INTER_CHAR_FRA,     /**< \brief 法国                 */
    AW_MPRINTER_INTER_CHAR_GM,      /**< \brief 德国                 */
    AW_MPRINTER_INTER_CHAR_UK,      /**< \brief 英国                 */
    AW_MPRINTER_INTER_CHAR_DK1,     /**< \brief 丹麦Ⅰ               */
    AW_MPRINTER_INTER_CHAR_SE,      /**< \brief 瑞典                 */
    AW_MPRINTER_INTER_CHAR_IT,      /**< \brief 意大利               */
    AW_MPRINTER_INTER_CHAR_ES1,     /**< \brief 西班牙               */
    AW_MPRINTER_INTER_CHAR_JP,      /**< \brief 日本                 */
    AW_MPRINTER_INTER_CHAR_NO,      /**< \brief 挪威                 */
    AW_MPRINTER_INTER_CHAR_DK2,     /**< \brief 丹麦Ⅱ               */
    AW_MPRINTER_INTER_CHAR_ES2,     /**< \brief 西班牙Ⅱ             */
    AW_MPRINTER_INTER_CHAR_LA,      /**< \brief 拉丁美洲             */
    AW_MPRINTER_INTER_CHAR_KP,      /**< \brief 韩国                 */
    AW_MPRINTER_INTER_CHAR_SVN,     /**< \brief 斯洛文尼亚/克罗地亚  */
    AW_MPRINTER_INTER_CHAR_CHI,     /**< \brief 中国                 */

} aw_mprinter_inter_char_t;

/**
 * \brief 字符代码页
 */
typedef enum aw_mprinter_code_page {

    AW_MPRINTER_CODE_PAGE_PC437    = 0, /**< \brief 美国，欧洲标准    */
    AW_MPRINTER_CODE_PAGE_KATAKANA,     /**< \brief 片假名            */
    AW_MPRINTER_CODE_PAGE_PC850,        /**< \brief 多国语言          */
    AW_MPRINTER_CODE_PAGE_PC860,        /**< \brief 葡萄牙语          */
    AW_MPRINTER_CODE_PAGE_PC863,        /**< \brief 加拿大-法语       */
    AW_MPRINTER_CODE_PAGE_PC865,        /**< \brief 北欧              */
    AW_MPRINTER_CHAR_PAGE_WPC1252 = 16, /**< \brief WPC1252           */
    AW_MPRINTER_CODE_PAGE_PC866,        /**< \brief 南斯拉夫2         */
    AW_MPRINTER_CODE_PAGE_PC852,        /**< \brief 拉丁语2           */
    AW_MPRINTER_CODE_PAGE_PC858,        /**< \brief 欧洲标准          */
    AW_MPRINTER_CODE_PAGE_SPACE = 255,  /**< \brief 空白              */

} aw_mprinter_char_page_t;


/**
 * \brief 图形垂直填充点图格式
 */
typedef enum aw_mprinter_dot_type {

    /** \brief 8点单精度    */
    AW_MPRINTER_DOT_8_SINGLE_DENSITY  = 0,

    /** \brief 8点双精度    */
    AW_MPRINTER_DOT_8_DOUBLE_DENSITY,

    /** \brief 24点单精度   */
    AW_MPRINTER_DOT_24_SINGLE_DENSITY = 32,

    /** \brief 24点双精度   */
    AW_MPRINTER_DOT_24_DOUBLE_DENSITY,

} aw_mprinter_dot_type_t;

/**
 * \brief 图形水平打印点图格式
 */
typedef enum aw_mprinter_picture_type {

    /** \brief 正常模式    */
    AW_MPRINTER_PICTURE_TYPE_NORMAL  = 48,

    /** \brief 倍宽        */
    AW_MPRINTER_PICTURE_TYPE_DOUBLE_WIDTH,

    /** \brief 倍高        */
    AW_MPRINTER_PICTURE_TYPE_DOUBLE_HEIGHT,

    /** \brief 倍宽倍高    */
    AW_MPRINTER_PICTURE_TYPE_QUADRUPLE,

} aw_mprinter_picture_type_t;


/**
 * \brief 打印机通信参数配置类型
 */
typedef enum aw_mprinter_cmm_cfg_type {

    /** \brief 波特率设置       */
    AW_MPRINTER_CMM_CFG_BAUDRATE_SET  = 1,

    /** \brief 校验位设置   */
    AW_MPRINTER_CMM_CFG_PARITY_SET,

    /** \brief 流控制设置   */
    AW_MPRINTER_CMM_CFG_FLOWCTRL_SET,

    /** \brief 数据长度设置   */
    AW_MPRINTER_CMM_CFG_DATLEN_SET
} aw_mprinter_cmm_cfg_type_t;

/**
 * \brief 打印机串口流控制
 */
enum aw_mprinter_flowctrl_type {

    /** \brief 硬件流控制   */
    AW_MPRINTER_FLOWCTRL_HARDWARE = 48,

    /** \brief 软件流控制   */
    AW_MPRINTER_FLOWCTRL_SOFTWARE,
};


/**
 * \brief 打印机串口校验位设置
 */
enum aw_mprinter_parity_type {

    /** \brief 无校验       */
    AW_MPRINTER_PARITY_NONE  = 48,

    /** \brief 奇校验   */
    AW_MPRINTER_PARITY_ODD,

    /** \brief 偶校验   */
    AW_MPRINTER_PARITY_EVEN,
};

/**
 * \brief 数据位
 */
enum aw_mprinter_datlen_type {

    /** \brief 7位       */
    AW_MPRINTER_DATLEN_NONE_7BIT = 55,

    /** \brief 8位   */
    AW_MPRINTER_DATLEN_NONE_8BIT,
};

/**
 * \brief 返回状态类型（非实时）
 */
typedef enum aw_mprinter_trans_stat_type {

    /** \brief 纸张传感器状态 */
    AW_MPRINTER_TRANS_STAT_PAPEER_SENSOR  = 1,

    /** \brief   */
    AW_MPRINTER_TRANS_STAT_DRAW_KICKOUT,

    /** \brief 笔墨状态 */
    AW_MPRINTER_TRANS_STAT_INK = 4,
} aw_mprinter_trans_stat_type_t;

/**
 * \brief 打印机ID类型
 */
typedef enum aw_mprinter_id {

    /** \brief 打印型号ID  */
    AW_MPRINTER_ID_PRINT_MODE  = 49,

    /** \brief 类型ID      */
    AW_MPRINTER_ID_TYPE,

    /** \brief 固件版本ID  */
    AW_MPRINTER_ID_FIRMWARE_VER,
} aw_mprinter_id_t;

/**
 * \brief 下划线线粗
 */
typedef enum aw_mprinter_underline_crude {

    /** \brief 下划线一点粗 */
    AW_MPRINTER_UNDERLINE_CRUDE_1  = 49,

    /** \brief 下划线两点粗 */
    AW_MPRINTER_UNDERLINE_CRUDE_2,

} aw_mprinter_underline_crude_t;

/**
 * \brief 打印字体类型
 */
typedef enum aw_mprinter_print_font_type {

    /** \brief 中文:24×24，外文:12×24 */
    AW_MPRINTER_PRINT_FONT_TYPE_1  = 0,

    /** \brief 中文:16×16，外文:8×16  */
    AW_MPRINTER_PRINT_FONT_TYPE_2,

    /** \brief 中文:12×12，外文:6×12  */
    AW_MPRINTER_PRINT_FONT_TYPE_3,

} aw_mprinter_print_font_type_t;

/**
 * \brief 切纸方式
 */
typedef enum aw_mprinter_cut_paper_type {

    /** \brief 半切 */
    AW_MPRINTER_CUT_PAPER_TYPE_HALF  = 0,

    /** \brief 全切 */
    AW_MPRINTER_CUT_PAPER_TYPE_ALL,

} aw_mprinter_cut_paper_type_t;

/**
 * \brief 打印机控制模式
 */
typedef enum aw_mprinter_ctrl_mode {

    /** \brief 打印模式控制   */
    AW_MPRINTER_CTRL_MODE_PRINT  = 0,

    /** \brief  设置打印密度  */
    AW_MPRINTER_CTRL_MODE_PRINT_DENSITY,

} aw_mprinter_ctrl_mode_t;

/**
 * \brief 打印机控制模式（打印速度）
 */
typedef enum aw_mprinter_print_head_ctrl_mode {

    /** \brief 最大速度150mm/s  */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_150  = 0,

    /** \brief 最大速度100mm/s  */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_100 = 8,

    /** \brief 最大速度50mm/s   */
    AW_MPRINTER_PRINT_HEAD_CTRL_MODE_50 = 16,

} aw_mprinter_print_head_ctrl_mode_t;


/**
 * \brief 下划线模式
 */
typedef enum aw_mprinter_underline_mode {

    /** \brief 解除下划线模式  */
    AW_MPRINTER_UNDERLINE_OFF,

    /** \brief 设定下划线模式（1点粗）  */
    AW_MPRINTER_UNDERLINE_1_DOT,

    /** \brief 设定下划线模式（2点粗）  */
    AW_MPRINTER_UNDERLINE_2_DOT,

} aw_mprinter_underline_mode_t;

/**
 * \brief 打印字体
 */
typedef enum aw_mprinter_char_font {

    /** \brief 打印字体A  */
    AW_MPRINTER_CHAR_FONT_A,

    /** \brief 打印字体B  */
    AW_MPRINTER_CHAR_FONT_B,

    /** \brief 打印字体C  */
    AW_MPRINTER_CHAR_FONT_C,

    /** \brief Extended font  */
    AW_MPRINTER_CHAR_FONT_EXTENDED = 97,
} aw_mprinter_char_font_t;

/**
 * \brief 设置字体大小
 */
typedef enum aw_mprinter_char_size {

    /** \brief 普通模式  */
    AW_MPRINTER_CHAR_SIZE_NORMAL = 0,

    /** \brief 倍高  */
    AW_MPRINTER_CHAR_SIZE_DBL_HGT,

    /** \brief 倍宽  */
    AW_MPRINTER_CHAR_SIZE_DBL_WDT = 16,

    /** \brief 倍高倍宽  */
    AW_MPRINTER_CHAR_SIZE_DBL_HGT_DBL_WDT,
} aw_mprinter_char_size_t;


/**
 * \brief 位图打印模式
 */
typedef enum aw_mprinter_bmg_ptype {

    /** \brief 普通模式  */
    AW_MPRINTER_BMG_PTYPE_NORMAL = 0x01,

    /** \brief 倍宽  */
    AW_MPRINTER_BMG_PTYPE_DBL_WDT,

    /** \brief 倍高  */
    AW_MPRINTER_BMG_PTYPE_DBL_HGT,

    /** \brief 倍高倍宽  */
    AW_MPRINTER_BMG_PTYPE_DBL_HGT_DBL_WDT,
} aw_mprinter_bmg_ptype_t;

/**
 * \brief 打印位置设置为打印行起点
 */
typedef enum aw_mprinter_ppos_beg {

    /** \brief 删除打印缓冲区中的所有数据后，设置打印位置为打印起始点  */
    AW_MPRINTER_PPOS_BEG_CANCEL,

    /** \brief 将打印缓冲区中的所有数据打印后，设置打印位置为打印行起始点  */
    AW_MPRINTER_PPOS_BEG_PRINT,

} aw_mprinter_ppos_beg_t;

/**
 * \brief 设置字符效果
 */
typedef enum aw_mprinter_effect_type {

    /** \brief 设置字符颜色  */
    AW_MPRINTER_EFFECT_CHAR_COLOR,

    /** \brief 设置背景色  */
    AW_MPRINTER_EFFECT_BACKGROUND_COLOR,

    /** \brief 阴影模式开  */
    AW_MPRINTER_EFFECT_SHADING_ON,

    /** \brief 阴影模式关  */
    AW_MPRINTER_EFFECT_SHADING_OFF,

} aw_mprinter_char_effect_type_t;

/**
 * \brief 页模式下打印方向设置
 */
typedef enum aw_mprinter_pdir_type {

    /** \brief 从左到右打印 */
    AW_MPRINTER_PDIR_LEFT_TO_RIGHT,

    /** \brief 从下往上打印  */
    AW_MPRINTER_PDIR_BOTTOM_TO_TOP,

    /** \brief 从右到左打印  */
    AW_MPRINTER_PDIR_RIGHT_TO_LEFT,

    /** \brief 从上往下打印  */
    AW_MPRINTER_PDIR_TOP_TO_BOTTOM,

} aw_mprinter_pdir_type_t;

/**
 * \brief 切纸类型
 */
typedef enum aw_mprinter_cut_pape_type {

    /** \brief Executes a full cut (cuts the paper comprintletely). */
    AW_MPRINTER_CUT_PAPE_A,

    /** \brief Executes a partial cut (one point left uncut). */
    AW_MPRINTER_CUT_PAPE_A_HALF,

    /** \brief Feeds paper to and executes a full cut.*/
    AW_MPRINTER_CUT_PAPE_B = 65,

    /** \brief Feeds paper to and executes a partial cut. */
    AW_MPRINTER_CUT_PAPE_B_HALF,

    /** \brief Specifies a paper cutting range to and executes a full cut. */
    AW_MPRINTER_CUT_PAPE_C = 97,

    /** \brief Specifies a paper cutting range to and executes a partial cut. */
    AW_MPRINTER_CUT_PAPE_C_HALF,

    /** \brief Feeds paper to and executes a full cut, then feeds paper to the
     *         print start position. */
    AW_MPRINTER_CUT_PAPE_D = 103,

    /** \brief Feeds paper to and executes a partial cut, then feeds paper to
     *         the print start position. */
    AW_MPRINTER_CUT_PAPE_D_HALF,

} aw_mprinter_cut_pape_type_t;

/**
 * \brief 打印机控制模式,设置条码可读字符（HRI）打印位置
 */
typedef enum aw_mprinter_hri_site {

    /** \brief 不打印       */
    AW_MPRINTER_HRI_SITE_NONE  = 48,

    /** \brief 上方         */
    AW_MPRINTER_HRI_SITE_TOP,

    /** \brief 下方         */
    AW_MPRINTER_HRI_SITE_BELOW ,

    /** \brief 上方和下方   */
    AW_MPRINTER_HRI_SITE_TOP_BELOW,

} aw_mprinter_hri_site_t;

/**
 * \brief 一维条码字体
 */
typedef enum aw_mprinter_hri_font_type {

    /** \brief 12 × 24  */
    AW_MPRINTER_HRI_FONT_TYPE_12_24  = 0,

    /** \brief 8 × 16   */
    AW_MPRINTER_HRI_FONT_TYPE_8_16,

    /** \brief 6 × 12   */
    AW_MPRINTER_HRI_FONT_TYPE_6_12 ,

} aw_mprinter_hri_font_type_t;


/**
 * \brief 一维条码编码方式
 */
typedef enum aw_mprinter_barcode_sys {

    /** \brief 编码系统为UPC-A                    */
    AW_MPRINTER_BARCODE_SYS_UPCA = 65,

    /** \brief 编码系统为UPC-E                    */
    AW_MPRINTER_BARCODE_SYS_UPCE,

    /** \brief 编码系统为JAN13(EAN13)             */
    AW_MPRINTER_BARCODE_SYS_EAN13,

    /** \brief 编码系统为JAN8(EAN8)               */
    AW_MPRINTER_BARCODE_SYS_EAN8,

    /** \brief 编码系统为CODE39                   */
    AW_MPRINTER_BARCODE_SYS_CODE39,

    /** \brief 编码系统为ITF (Interleaved 2 of 5) */
    AW_MPRINTER_BARCODE_SYS_ITF25,

    /** \brief 编码系统为CODABAR (NW-7)           */
    AW_MPRINTER_BARCODE_SYS_CODABAR,

    /** \brief 编码系统为CODE93                   */
    AW_MPRINTER_BARCODE_SYS_CODE93,

    /** \brief 编码系统为CODE128                  */
    AW_MPRINTER_BARCODE_SYS_CODE128,

    /** \brief 编码系统为C/EAN128                 */
    AW_MPRINTER_BARCODE_SYS_EAN128,

} aw_mprinter_barcode_sys_t;


/**
 * \brief 二维码编码方式
 */
typedef enum aw_mprinter_symbol_type {

    /** \brief PDF417编码  */
    AW_MPRINTER_SYMBOL_TYPE_PDF417  = 48,

    /** \brief QR编码   */
    AW_MPRINTER_SYMBOL_TYPE_QR_CODE,

} aw_mprinter_symbol_type_t;

/**
 * \brief 状态返回回调函数
 */
typedef int (*aw_pfn_printer_stat_isr_t)(void *p_arg);

/**
 * \brief 状态回调类型
 */
typedef struct aw_mprinter_stat_cbk {

    /** \brief 回调函数类型 */
    aw_mprinter_cbk_type_t     stat_cbk_type;

    /** \brief 回调函数    */
    aw_pfn_printer_stat_isr_t  pfn_callback;

    /** \brief 回调函数参数    */
    void                      *p_callback_arg;
} aw_mprinter_stat_cbk_t;


/**
 * \brief 打印机数据参数类型
 */
typedef struct aw_mprinter_params {

    /** \brief 数据buf1, 常传递子功能码或命令码 */
    uint8_t     *p_buf1;

    /** \brief 数据buf1长度 */
    size_t       buf1_len;

    /** \brief 数据buf2，常传递数据，表格或图像数据 */
    uint8_t     *p_buf2;

    /** \brief 数据buf2长度 */
    size_t       buf2_len;


    /** \brief 回应数据缓冲区，少数指令要求微打回应数据*/
    uint8_t     *p_rsp_buf;

    /** \brief 请求回应长度 */
    size_t       rsp_len;

    /** \brief 收到回应长度 */
    size_t       ret_len;

    /** \brief 回应超时时间 */
    int          timeout;

    /** \brief 信号量ID */
    aw_semb_id_t sem_id;
} aw_mprinter_params_t;


/**
 * \brief 打印并进纸
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 将打印缓存里的内容打印，之后根据当前的行间距设置进纸一行，并调整打印位
 *       置至下一行的起始位置(可参考EPSON微打指令LF Print and line feed)
 */
aw_err_t aw_mprinter_print_and_feed (int fd);

/**
 * \brief 页模式回归标准模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note (可参考EPSON微打指令FF (in page mode) Print and return to standard mode
 *       (in page mode))
 */
aw_err_t aw_mprinter_page_stdmd_enter (int fd);

/**
 * \brief 回车（打印位置调整至本行起始位置，不进纸）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令CR Print and carriage return
 */
aw_err_t aw_mprinter_print_and_enter (int fd);

/**
 * \brief 页模式下打印数据
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note    ESC FF Print data in page mode
 */
aw_err_t aw_mprinter_page_print (int fd);

/**
 * \brief 打印并进纸n点（将打印缓存里的内容打印并进纸n点）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] ndotlines      : 进纸ndotlines点（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC J Print and feed paper
 */
aw_err_t aw_mprinter_print_and_ndotlines_feed (int fd, uint8_t ndotlines);

/**
 * \brief 打印并退纸n点（将打印缓存的内容打印并退纸n点）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] ndotlines      : 退纸ndotlines点（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC K Print and reverse feed
 */
aw_err_t aw_mprinter_print_and_ndotlines_back (int fd, uint8_t ndotlines);

/**
 * \brief 打印并进纸n行（将打印缓存的内容打印并进纸n行）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] nfontlines     : 进纸nfontlines行（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC d Print and feed n lines
 */
aw_err_t aw_mprinter_print_and_nfontlines_feed (int fd, uint8_t nfontlines);

/**
 * \brief 打印并退纸n行
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] nfontlines     : 退纸nfontlines行（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC e Print and reverse feed n lines
 */
aw_err_t aw_mprinter_print_and_nfontlines_back (int fd, uint8_t nfontlines);

/**
 * \brief 设置行间距为默认值（33点）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC 2 Select default line spacing
 */
aw_err_t aw_mprinter_print_line_spaceing_default_set (int fd);

/**
 * \brief 设置行间距为n点
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] ndotlines      : 设置行间距ndotlines点（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC 3 Set line spacing
 */
aw_err_t aw_mprinter_print_line_spaceing_set (int fd, uint8_t ndotlines);

/**
 * \brief 页模式下删除打印数据
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令CAN Cancel print data in page mode
 *
 */
aw_err_t aw_mprinter_page_data_cancel (int fd);

/**
 * \brief 设置右侧字符间距
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] space          : 字符间距
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC SP Set right-side character spacing
 */
aw_err_t aw_mprinter_right_spacing_set (int fd, uint8_t space);

/**
 * \brief 设置字符打印方式
 *
 * \param[in] fd          : 指向zytp58_xx6b设备结构体的指针
 * \param[in] font_type      : 字符打印方式，AW_MPRINTER58_XX6B_FONT_TYPE_*，
 *                             可多个宏组合使用，参见
 *                             \ref grp_aw_mprinter_font_type
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC ! Select print mode(s)
 */
aw_err_t aw_mprinter_font_type_set (int fd, uint8_t font_type);

/**
 * \brief 选择用户自定义字符
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC % Select/cancel user-defined character set
 *
 */
aw_err_t aw_mprinter_user_defch_select (int fd);

/**
 * \brief 取消用户自定义字符
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note
 */
aw_err_t aw_mprinter_user_defch_cancel (int fd);

/**
 * \brief 定义用户自定义字符
 *
 * \param[in] fd                        : 微打设备描述符
 * \param[in] ver_bytes                 : 垂直方向字节数
 * \param[in] char_code_beg             : 字符起始编号
 * \param[in] char_code_end             : 字符结束编号
 * \param[in] p_code_buf                : 自定义字符数据时
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC & Define user-defined characters
 */
aw_err_t aw_mprinter_user_char_def (int      fd,
                                    uint8_t  ver_bytes,
                                    uint8_t  char_code_beg,
                                    uint8_t  char_code_end,
                                    uint8_t *p_code_buf);

/**
 * \brief 下划线模式开
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC – Turn underline mode on/off
 */
aw_err_t aw_mprinter_underline_on (int fd);

/**
 * \brief  下划线模式关
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC – Turn underline mode on/off
 */
aw_err_t aw_mprinter_underline_off (int fd);

/**
 * \brief 取消用户自定义字符
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] char_code      : 字符编号
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC ? Cancel user-defined characters
 */
aw_err_t aw_mprinter_user_char_canceldef (int fd, uint8_t char_code) ;

/**
 * \brief 开启粗体打印的开
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC E Turn emprinthasized mode on/off
 */
aw_err_t aw_mprinter_emprinthasized_on (int fd);

/**
 * \brief 开启粗体打印的关
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC E Turn emprinthasized mode on/off
 */
aw_err_t aw_mprinter_emprinthasized_off (int fd);

/**
 * \brief 双重打印模式的开
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC G Turn double-strike mode on/off
 */
aw_err_t aw_mprinter_dbl_strike_on (int fd);

/**
 * \brief 双重打印模式的关
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC G Turn double-strike mode on/off
 */
aw_err_t aw_mprinter_dbl_strike_off (int fd);

/**
 * \brief 选择打印字体
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] font           : 字体类型
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC M Select character font
 */
aw_err_t aw_mprinter_character_font_set (int fd, aw_mprinter_char_font_t font);

/**
 * \brief 选择国标字符集
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] char_type      : 国标字符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC R Select an international character set
 */
aw_err_t aw_mprinter_gb_char_type_set (int                      fd,
                                       aw_mprinter_inter_char_t char_type);

/**
 * \brief 设置/解除顺时针90°旋转
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC V Turn 90° clockwise rotation mode on/off
 */
aw_err_t aw_mprinter_clockwise90_rotat_set (int fd);

/**
 * \brief 选择字符代码页
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] code_page      : 字符代码页
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC t Select character code table
 */
aw_err_t aw_mprinter_char_page_set (int fd, aw_mprinter_char_page_t code_page);

/**
 * \brief 设置颠倒打印模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC { Turn upside-down print mode on/off
 */
aw_err_t aw_mprinter_upside_down_on (int fd);

/**
 * \brief 解除颠倒打印模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note  可参考EPSON微打指令ESC { Turn upside-down print mode on/off
 */
aw_err_t aw_mprinter_upside_down_off (int fd);

/**
 * \brief 选择字符效果
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 字符效果类型
 * \param[in] color          : 颜色类型
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( N Select character effects
 */
aw_err_t aw_mprinter_char_effects_select (int                            fd,
                                          aw_mprinter_char_effect_type_t type,
                                          uint8_t                        color);

/**
 * \brief  选择字符大小
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] size           : 字符大小
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note  可参考EPSON微打指令GS ! Select character size
 */
aw_err_t aw_mprinter_char_size_set (int fd, aw_mprinter_char_size_t size);

/**
 * \brief 设定反白打印模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS B Turn white/black reverse print mode on/off
 */
aw_err_t aw_mprinter_whi_blk_reverse_on (int fd);

/**
 * \brief 解除反白打印模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS B Turn white/black reverse print mode on/off
 */
aw_err_t aw_mprinter_whi_blk_reverse_off (int fd);

/**
 * \brief 设定平滑模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS b Turn smoothing mode on/off
 */
aw_err_t aw_mprinter_smoothing_mode_on (int fd);

/**
 * \brief 解除平滑模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS b Turn smoothing mode on/off
 */
aw_err_t aw_mprinter_smoothing_mode_off (int fd);

/**
 * \brief 使能面板按钮
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC c 5 Enable/disable panel buttons
 */
aw_err_t aw_mprinter_panel_buttons_enable (int fd);

/**
 * \brief 禁能面板按钮
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC c 5 Enable/disable panel buttons
 */
aw_err_t aw_mprinter_panel_buttons_disable (int fd);

/**
 * \brief 开纸张传感器停止打印
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC c 4 Select paper sensor(s) to stop printing
 */
aw_err_t aw_mprinter_paper_sensors_on (int fd);

/**
 * \brief 关纸张传感器停止打印
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC c 4 Select paper sensor(s) to stop printing
 */
aw_err_t aw_mprinter_paper_sensors_off (int fd);

/**
 * \brief 水平制表
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏

 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令HT Horizontal tab
 */
aw_err_t aw_mprinter_horizontal_tab (int fd);

/**
 * \brief 设置绝对打印位置（此指令只对本行有效，
 *        换行后打印位置复位为打印起始位置）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] xnum           : 打印位置（以点为单位，范围0~65535）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏

 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC $ Set absolute print position
 */
aw_err_t aw_mprinter_absolute_print_set (int fd, uint16_t xnum);

/**
 * \brief 设置水平制表位置
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] p_buf          : 水平制表位置数据，以8点单位
 * \param[in] data_num       : 数据个数（0~16）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC D Set horizontal tab positions
 */
aw_err_t aw_mprinter_horizontaltab_set (int            fd,
                                        uint8_t       *p_buf,
                                        size_t         data_num);

/**
 * \brief 页面模式下选择打印方向
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 方向
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC T Select print direction in page mode
 */
aw_err_t aw_mprinter_page_print_dir_set (int fd, aw_mprinter_pdir_type_t type);

/**
 * \brief 页面模式下选择打印区域
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] origin_x       : 打印区原点水平坐标
 * \param[in] origin_y       : 打印区原点垂直坐标
 * \param[in] dx             : 打印区水平宽度
 * \param[in] dy             : 打印区垂直高度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC W Set print area in page mode
 */
aw_err_t aw_mprinter_page_print_area_set (int       fd,
                                          uint16_t  origin_x,
                                          uint16_t  origin_y,
                                          uint16_t  dx,
                                          uint16_t  dy);

/**
 * \brief 设置相对打印位置
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] post           : 相对位置
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note  可参考EPSON微打指令ESC \ Set relative print position
 */
aw_err_t aw_mprinter_relative_ppos_set (int fd, uint16_t post);

/**
 * \brief 设置打印对齐方式
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] align          : 打印对齐方式
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏

 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC a Select justification
 */
aw_err_t aw_mprinter_print_align_type_set (int fd, aw_mprinter_align_t align);

/**
 * \brief 页模式下下设置纵向绝对位置
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] post           : 绝对位置
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS $ Set absolute vertical 
 *       print position in page mode
 */
aw_err_t aw_mprinter_page_absver_ppost_set (int fd, uint8_t post);

/**
 * \brief 设定左侧空白量
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] space          : 空白量
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS L Set left margin
 */
aw_err_t aw_mprinter_left_spacing_set (int fd, uint16_t space);

/**
 * \brief 设置打印位置为打印行起点
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 配置类型
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS T Set print position to the 
 *       beginning of print line
 */
aw_err_t aw_mprinter_ppos_begp_set (int fd, aw_mprinter_ppos_beg_t type);

/**
 * \brief 设置打印区域宽度
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] width          : 宽度信息
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS W Set print area width
 */
aw_err_t aw_mprinter_area_width_set (int fd, uint16_t width);

/**
 * \brief 页模式下设置垂直相对位置
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] post           : 相对位置
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS \ Set relative vertical print 
 *       position in page mode
 */
aw_err_t aw_mprinter_page_vrtcl_post_set (int fd, uint16_t post);

/**
 * \brief 图形垂直取模数据填充
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 点图方式
 * \param[in] hsize          : 水平方向点数（1 ≤ HSize ≤ 384）
 * \param[in] p_dataBuf      : 图片数据缓冲区
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC * Select bit-image mode
 */
aw_err_t aw_mprinter_picture_printv (int                     fd,
                                     aw_mprinter_dot_type_t  type,
                                     uint16_t                hsize,
                                     uint8_t                *p_dataBuf);

/**
 * \brief 打印NV位图
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \param[in] picture_id     : 指定位图
 * \param[in] picture_type   : 位图方式
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS p Print NV bit image
 */
aw_err_t aw_mprinter_nvpicture_print (int                        fd,
                                      uint8_t                    picture_id,
                                      aw_mprinter_picture_type_t picture_type);

/**
 * \brief 下载NV位图
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] picture_num    : 下载的图片数量
 * \param[in] p_picture_buf  : 下载图片的缓冲区
 * \param[in] len            : 图片数据长度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS q Define NV bit image
 */
aw_err_t aw_mprinter_nvpicture_download (int              fd,
                                         uint8_t          picture_num,
                                         uint8_t         *p_picture_buf,
                                         size_t           len);

/**
 * \brief 图片水平取模数据打印
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 位图方式，可以为以下任意值
 * \param[in] xsize          : 水平方向字节数（1 ≤ xsize ≤ 48）
 * \param[in] ysize          : 垂直方向点数（0 ≤ ysize ≤ 65535）
 * \param[in] p_data_buf     : 图片数据缓存首址
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS v 0 Print raster bit image
 */
aw_err_t aw_mprinter_picture_printh (int                         fd,
                                     aw_mprinter_picture_type_t  type,
                                     uint16_t                    xsize,
                                     uint16_t                    ysize,
                                     uint8_t                    *p_data_buf);

/**
 * \brief 定义下载位图
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] x_bytes        : 水平方向字节数
 * \param[in] y_bytes        : 垂直方向字节数
 * \param[in] p_image_buf    : 位图信息
 *
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS * Define downloaded bit image
 */
aw_err_t aw_mprinter_download_bmg_def (int        fd,
                                       uint8_t    x_bytes,
                                       uint8_t    y_bytes,
                                       uint8_t   *p_image_buf);

/**
 * \brief 打印下载位图
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 位图类型
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令Print downloaded bit image
 */
aw_err_t aw_mprinter_download_bmg_print (int fd, aw_mprinter_bmg_ptype_t type);

/**
 * \brief 查询打印机状态（实时）
 *
 * \param[in]  fd            : 微打设备描述符
 * \param[out] p_state       : 打印机返回状态值，各个数据位独立，可表示多个状态
 *                             参见\ref grp_aw_mprinter_stat_flag
 * \param[in]  ms_time_out   : 应答超时（ms为单位）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ETIME         : 传输超时
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令DLE EOT Transmit real-time status
 */
aw_err_t aw_mprinter_printer_state_rt_get (int             fd,
                                           uint8_t        *p_state,
                                           uint16_t        ms_time_out);

/**
 * \brief 传输外围设备状态
 *
 * \param[in]  fd            : 微打设备描述符
 * \param[out] p_stat        : 状态
 * \param[in] time_out       : 超时时间
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC u Transmit peripheral device status
 */
aw_err_t aw_mprinter_prph_dev_stat_trans (int          fd,
                                          uint8_t     *p_stat,
                                          uint16_t     time_out);

/**
 * \brief 传输纸张传感器状态
 *
 * \param[in]  fd            : 微打设备描述符
 * \param[out] p_stat        : 状态
 * \param[out] time_out      : 超时时间
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC v Transmit paper sensor status
 */
aw_err_t aw_mprinter_paper_sensor_stat_trans (int         fd,
                                              uint8_t    *p_stat,
                                              uint16_t    time_out);

/**
 * \brief 设置/取消打印机状态自动返回
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] key            : 选项开关，多个选项可以相或
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS a Enable/disable Automatic Status Back
 */
aw_err_t aw_mprinter_printer_state_auto_set (int fd, uint8_t key);

/**
 * \brief 传送状态（非实时）
 *
 * \param[in]  fd            : 微打设备描述符
 * \param[in] type          : 传送状态类型
 * \param[out] p_state       : 状态标志
 * \param[in]  time_out      : 超时，单位为ms
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ETIME         : 传输超时
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS r Transmit status
 */
aw_err_t aw_mprinter_paper_stat_get (int                           fd,
                                     aw_mprinter_trans_stat_type_t type,
                                     uint8_t                      *p_state,
                                     uint16_t                      time_out);

/**
 * \brief 设置一维条码可读字符（HRI）打印位置
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] hri_pos        : 条码HRI显示位置
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS H Select print position of HRI characters
 */
aw_err_t aw_mprinter_barcode_hri_pos_set (int                    fd,
                                          aw_mprinter_hri_site_t hri_pos);

/**
 * \brief 设置一维条码字体
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] hri_font       : 条码文字字体
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS f Select font for HRI characters
 */
aw_err_t aw_mprinter_barcode_hri_font_set (int                         fd,
                                           aw_mprinter_hri_font_type_t hri_font);

/**
 * \brief 设置一维条码高度
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] height         : 条码高度，以点为单位（0~255）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS h Set bar code height
 */
aw_err_t aw_mprinter_barcode_height_set (int fd, uint8_t height);

/**
 * \brief 打印一维条码
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \param[in] barcode_sys    : 编码方式
 * \param[in] p_code_buf     : 条码数据首地址
 * \param[in] code_len       : 条码数据长度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS k Print bar code
 */
aw_err_t aw_mprinter_barcode_print (int                       fd,
                                    aw_mprinter_barcode_sys_t barcode_sys,
                                    uint8_t                  *p_code_buf,
                                    size_t                    code_len);

/**
 * \brief 设置一维条码宽度
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] width          : 条码黑白条宽度，以点为单位，范围（1~6）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS w Set bar code width
 */
aw_err_t aw_mprinter_barcode_width_set (int fd, uint8_t width);

/**
 * \brief 开始宏定义
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS : Start/end macro definition
 */
aw_err_t aw_mprinter_macro_def_start (int fd);

/**
 * \brief 部分切割（三点之外）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC m Partial cut (three points left uncut)
 */
aw_err_t aw_mprinter_threepoint_cut (int fd);

/**
 * \brief 选择切割模式和切割纸张
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 切纸类型
 * \param[in] n              : 切纸进纸n点
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS V Select cut mode and cut paper
 */
aw_err_t aw_mprinter_print_and_cut (int                         fd,
                                    aw_mprinter_cut_pape_type_t type,
                                    uint8_t                     n);

/**
 * \brief 发送实时请求给打印机
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 请求类型
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令DLE ENQ Send real-time request to printer
 */
aw_err_t aw_mprinter_real_time_req_send (int fd, uint8_t type);

/**
 * \brief 进入低功耗模式（实时）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] ms_time_out    : 超时时间（单位为毫秒）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ETIME         : 超时
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令DLE DC4 (fn = 2) Execute power-off sequence
 */
aw_err_t aw_mprinter_printer_low_power_mode_enter (int fd, uint16_t ms_time_out);

/**
 * \brief 退出低功耗模式（实时）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] ms_time_out    : 超时时间（单位为毫秒）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏

 * \retval -AW_ETIME         : 超时
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note  可参考EPSON微打指令DLE DC4 (fn = 2) Execute power-off sequence
 */
aw_err_t aw_mprinter_printer_low_power_mode_exit (int fd, uint16_t ms_time_out);

/**
 * \brief 清打印缓存（实时）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令DLE DC4 (fn = 8) Clear buffer (s)
 */
aw_err_t aw_mprinter_printer_buffer_clear (int fd);

/**
 * \brief 使能外围设备
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC = Select peripheral device
 */
aw_err_t aw_mprinter_prph_dev_enable (int fd);

/**
 * \brief 禁能外围设备
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC = Select peripheral device
 */
aw_err_t aw_mprinter_prph_dev_disable (int fd);

/**
 * \brief 初始化打印机
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC @ Initialize printer
 */
aw_err_t aw_mprinter_init (int fd);

/**
 * \brief 选择页模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC L Select page mode
 */
aw_err_t aw_mprinter_page_mode_select (int fd);

/**
 * \brief 选择标准模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令ESC S Select standard mode
 */
aw_err_t aw_mprinter_stand_mode_select (int fd);

/**
 * \brief 启动实时命令，使能指定实时（DLE DC4）命令
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] cmd_id         : 指令ID(DLE DC4实时指令编号)
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( D Enable/disable real-time command
 */
aw_err_t aw_mprinter_real_time_cmd_enable (int fd, uint8_t cmd_id);
/**
 * \brief 禁用实时命令
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] cmd_id         : 指令ID
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( D Enable/disable real-time command
 */
aw_err_t aw_mprinter_real_time_cmd_disable (int fd, uint8_t cmd_id);

/**
 * \brief 选择打印控制模式
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] md             : 控制模式
 * \param[in] dat            : 控制数据
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( K Select print control method(s)
 */
aw_err_t aw_mprinter_print_ctrl_meth_select (int                   fd,
                                             aw_mprinter_ctrl_md_t md,
                                             uint8_t               dat);

/**
 * \brief 传送打印机ID
 *
 * \param[in]  fd            : 微打设备描述符
 * \param[in]  id_type       : id类型
 * \param[out] p_id_val      : id值，id类型为AW_MPRINTER_ID_TYPE时id_val结果
 *                             参见\ref grp_aw_mprinter_type_id
 * \param[in]  ms_time_out   : 超时时间（单位为毫秒）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ETIME         : 传输超时
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS I Transmit printer ID
 */
aw_err_t aw_mprinter_id_get (int              fd,
                             aw_mprinter_id_t id_type,
                             uint8_t         *p_id_val,
                             uint16_t         ms_time_out);

/**
 * \brief 设置水平或垂直运动单元
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] x              : 水平单元（速度）
 * \param[in] y              : 垂直单元（速度）
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS P Set horizontal and vertical motion units
 */
aw_err_t aw_mprinter_hv_motion_unit_set (int fd, uint8_t  x, uint8_t  y);

/**
 * \brief 选择汉字打印模式
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 打印模式， 参见
 *                             \ref grp_aw_mprinter_kanjichar_mode
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS ! Select print mode(s) for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_pmode_select (int fd, uint8_t type);

/**
 * \brief 选择汉字模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS & Select Kanji character mode
 */
aw_err_t aw_mprinter_chinese_mode_enter (int fd);

/**
 * \brief 设定汉字模式下划线
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] type           : 下划线模式
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS – Turn underline mode on/off for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_underline_on (int                           fd,
                                             aw_mprinter_underline_crude_t type);

/**
 * \brief 解除汉字模式下划线
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS – Turn underline mode on/off for Kanji characters
 */
aw_err_t aw_mprinter_kanjichar_underline_off (int fd);

/**
 * \brief 取消汉字模式
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS . Cancel Kanji character mode
 */
aw_err_t aw_mprinter_chinese_mode_exit (int fd);

/**
 * \brief 设置PDF417二维码数据区域列数
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] vrows          : 数据区域列数
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_areavrow_set (int fd, uint8_t vrows);

/**
 * \brief 设置PDF417二维码行数
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] hrows          : 行数
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_areahrow_set (int fd, uint8_t hrows);

/**
 * \brief 设置二维码单元宽度大小
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] symbol         : 编码方式
 * \param[in] wdt            : 宽度大小
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_width_set (int                       fd,
                                       aw_mprinter_symbol_type_t symbol,
                                       uint8_t                   wdt);

/**
 * \brief 设置PDF417二维码行高
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] high           : 行高
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_rowhigh_set (int fd, uint8_t high);

/**
 * \brief 设置二维码纠正等级
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] symbol         : 编码方式
 * \param[in] lvl            : 纠错等级
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_errcorlvl_set (int                       fd,
                                           aw_mprinter_symbol_type_t symbol,
                                           uint8_t                   lvl);

/**
 * \brief 设置PDF417二维码为标准模式（二维码）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_stdmd_set (int fd);

/**
 * \brief 设置PDF417二维码为截断模式（二维码）
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_PDF417_tncatemd_set (int fd);

/**
 * \brief 传输数据到缓存（二维码）
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] symbol         : 编码方式
 * \param[in] p_code         : 二维码数据
 * \param[in] code_len       : 二维码数据长度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_data_write (int                       fd,
                                      aw_mprinter_symbol_type_t   symbol,
                                      uint8_t                    *p_code,
                                      size_t                      code_len);

/**
 * \brief 打印编码缓存中的二维条码
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] symbol         : 编码方式
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( k Set up and print the symbol
 */
aw_err_t aw_mprinter_symbol_data_print (int                       fd,
                                        aw_mprinter_symbol_type_t symbol);

/**
 * \brief 向用户NV内存写入
 *
 * \param[in] fd               : 微打设备描述符
 * \param[in] addr             : 微打的nv内存的地址
 * \param[in] p_nv_buf         : 缓冲区
 * \param[in] len              : 数据长度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS g 1 Write to NV user memory
 */
aw_err_t aw_mprinter_nv_usr_mem_write (int       fd,
                                     uint32_t    addr,
                                     uint8_t    *p_nv_buf,
                                     size_t      len);

/**
 * \brief 从用户NV内存读取
 *
 * \param[in] fd              : 微打设备描述符
 * \param[in] addr            : 微打的nv内存的地址
 * \param[in] p_nv_buf        : 缓冲区
 * \param[in] len             : 数据长度
 * \param[in] time_out        : 超时时间
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS g 2 Read from NV user memory
 */
aw_err_t aw_mprinter_nv_usr_mem_nv_read (int       fd,
                                       uint32_t    addr,
                                       uint8_t    *p_nv_buf,
                                       size_t      len,
                                       uint16_t    time_out);

/**
 * \brief 设置用户设置命令
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] cfg_type       : 设置类型
 * \param[in] param          : 设置数据
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令GS ( E Set user setup commands
 */
aw_err_t aw_mprinter_cmm_cfg_set (int                          fd,
                                aw_mprinter_cmm_cfg_type_t     cfg_type,
                                uint32_t                       param);

/**
 * \brief 标签纸/黑标纸指令
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] p_code         : 子功能码或数据
 * \param[in] code_len       : 数据长度
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持此功能
 *
 * \note 可参考EPSON微打指令FS ( L Select label and black mark control 
 *       function(s)
 */
aw_err_t aw_mprinter_abel_and_black_mark_set (int       fd,
                                              uint8_t  *p_code,
                                              size_t    code_len);

/**
 * \brief 设置打印机状态回调函数
 *
 * \param[in] fd              : 微打设备描述符
 *
 * \param[in] callback_type   : 回调函数类型
 * \param[in] pfn_callback    : 回调函数指针
 * \param[in] p_callback_arg  : 回调函数参数,回调函数被调用时传递该参数
 *
 * \retval AW_OK              : 成功
 * \retval -AW_EBADF          : 设备描述符损坏
 * \retval -AW_EINVAL         : 无效参数
 */
aw_err_t aw_mprinter_stat_callback_set (int                       fd,
                                        aw_mprinter_cbk_type_t    callback_type,
                                        aw_pfn_printer_stat_isr_t pfn_callback,
                                        void                     *p_callback_arg);

/**
 * \brief 配置微打参数
 *
 * \param[in] p_cmd           : 微打命令（包含打印的数据）
 * \param[in] p_buf1          : 指令数据1
 * \param[in] buf1_len        : 指令数据1长度
 * \param[in] p_buf2          : 指令数据2
 * \param[in] buf2_len        : 指令数据2长度
 * \param[in] p_rspbuf        : 回应数据缓冲
 * \param[in] rspbuf_len      : 需要回应的数据量
 * \param[in] time_out        : 回应超时
 *
 * \retval 无
 *
 * \note 驱动收到p_cmd命令后首先会检验命令的有效性，比如设备是否支持该指令，
 *       通过校验后会依次将buf1，buf2里面的数据写入微打设备，如果这条命令需要微
 *       打回应数据时p_rspbuf配置回应数据的缓冲区，rspbuf_len为回应数据的长度，
 *       若time_out时间内没有收到rspbuf_len长度的回应数据会产生超时，可通过
 *       p_cmd->ret_len判断收到回应的数据量，如果该命令不需要回应数据则p_rspbuf
 *       置为空。之所以指令数据分为buf1，buf2，是因为子功能码与打印数据不在一个
 *       连续的空间，分开方便操作，减少数据搬运操作。
 */
aw_local aw_inline
void aw_mprinter_cfg_cmd (aw_mprinter_params_t     *p_cmd,
                        uint8_t                    *p_buf1,
                        size_t                      buf1_len,
                        uint8_t                    *p_buf2,
                        size_t                      buf2_len,
                        uint8_t                    *p_rspbuf,
                        size_t                      rspbuf_len,
                        int                         time_out)
{
    p_cmd->p_buf1    = p_buf1;
    p_cmd->buf1_len  = buf1_len;
    p_cmd->p_buf2    = p_buf2;
    p_cmd->buf2_len  = buf2_len;
    p_cmd->p_rsp_buf = p_rspbuf;
    p_cmd->rsp_len   = rspbuf_len;
    p_cmd->timeout   = time_out;
}

/**
 * \brief 扩展指令接口
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] p_cmd          : 扩展参数
 *
 * \retval AW_OK             : 成功
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持这种指令
 */
aw_err_t aw_mprinter_features_expand (int fd, aw_mprinter_params_t *p_cmd);

/**
 * \brief 微打设备写
 *
 * \param[in] fd             : 微打设备描述符
 * \param[in] p_buf          : 数据缓冲区
 * \param[in] nbytes         : 数据长度
 *
 * \retval 大于0             : 写入的数据长度
 * \retval -AW_EINVAL        : 无效参数
 * \retval -AW_EBADF         : 设备描述符损坏
 * \retval -AW_ENOSYS        : 该类型微型打印机不支持这种指令
 */
ssize_t aw_mprinter_write(int fd, const void *p_buf, size_t nbytes);

/**
 * \brief 打开微打设备
 *
 * \param[in] p_name         : 打印机名，可查看配置文件中设备名
 *
 * \retval 大于0             : 设备描述符
 * \retval 小于0             : 错误
 */
int aw_mprinter_dev_open (const char *p_name);

/**
 * \brief 关闭微打设备
 *
 * \param[in] fd             : 微打设备描述符
 *
 * \return 无
 */
void aw_mprinter_dev_close (int fd);

/** @} */

#ifdef __cplplus
}
#endif

#endif /* __AW_MPRINTER_H */

/* end of file */
