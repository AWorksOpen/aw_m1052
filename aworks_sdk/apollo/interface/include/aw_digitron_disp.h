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
 * \brief 支持多实例的段式数码管驱动接口
 *
 * 使用本服务需要包含头文件 aw_digitron_disp.h
 *
 * \par 使用示例
 * \code
 * #include "aw_digitron_disp.h"
 *
 * // 初始化，设置8段ASCII解码
 * aw_digitron_disp_set_decode(0, aw_digitron_seg8_ascii_decode);
 *
 * aw_digitron_disp_set_blink(0, 0, TURE); // 设置第0个数码管闪烁
 * aw_digitron_disp_at(0, 0, 0x54);        // 第0个数码管显示自定义形状0x54
 * aw_digitron_disp_char_at(0, 1, 'A');    // 第2个数码管显示字符'A'
 * aw_digitron_disp_clr();                 // 清除所有数码管显示（黑屏）
 * aw_digitron_disp_str(0, 0, 4, "0123");  // 数码管显示字符串"0123"
 * \endcode
 *
 * \internal
 * \par modification history
 * - 1.10 17-09-07  tee, Change the return type from void to aw_err_t
 * - 1.03 15-03-17  ops, redefine the aw_digitron_disp_str.
 * - 1.02 14-11-24  ops, refactor by using list for abstract digitron device management.
 * - 1.01 14-11-06  ops, redefine the interface by using the concept of digitron matrix.
 * - 1.00 13-02-25  orz, first implementation.
 * \endinternal
 */

#ifndef __AW_DIGITRON_DISP_H
#define __AW_DIGITRON_DISP_H

#include "aw_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_digitron_disp
 * \copydoc aw_digitron_disp.h
 * @{
 */

/**
 * \brief 解码ASCII字符为8段数码管的段输出码
 *
 * \param ascii_char 要解码的ASCII字符
 *
 * \return ASCII字符对应的数码管字形数据, 如果无法解码，返回空白码。
 *
 * \note
 * 这个函数一般作为 aw_digitron_disp_set_decode() 的参数， 支持的ASCII字符有：
 *  |  A  |  S  |  C  |  I  |  I  |
 *  |-----|-----|-----|-----|-----|
 *  | '0' | '1' | '2' | '3' | '4' |
 *  | '5' | '6' | '7' | '8' | '9' |
 *  | 'A' | 'B' | 'C' | 'D' | 'E' |
 *  | 'F' | 'H' | 'I' | 'J' | 'L' |
 *  | 'N' | 'O' | 'P' | 'R' | 'S' |
 *  | 'a' | 'b' | 'c' | 'd' | 'e' |
 *  | 'f' | 'h' | 'i' | 'j' | 'l' |
 *  | 'n' | 'o' | 'p' | 'r' | 's' |
 *  | '-' | '.' | ' ' |  -  |  -  |
 */
uint16_t aw_digitron_seg8_ascii_decode (uint16_t ascii_char);

/**
 * \brief 设置数码管显示驱动的段码解码表
 *
 * \param id           显示器编号
 *
 * \param pfn_decode   自定义解码函数
 */
aw_err_t aw_digitron_disp_decode_set (int id, uint16_t (*pfn_decode)(uint16_t code));

/**
 * \brief 设置数码管显示闪烁属性
 *
 * \param id     显示器编号
 * \param index  数码管索引号
 * \param blink  闪烁属性，TURE：闪烁，FALSE：禁止闪烁。
 */
aw_err_t aw_digitron_disp_blink_set (int id, int index, aw_bool_t blink);

/**
 * \brief 直接向数码管写入数据显示数据。本函数一般用于显示自定义的特殊符号或形状
 *
 * \param id     显示器编号
 * \param index  数码管显存索引.
 * \param seg    写入到显存的数据(数码管的“段”码，高电平有效).
 */
aw_err_t aw_digitron_disp_at (int id, int index, uint16_t seg);

/**
 * \brief 在指定编号的数码管上显示ASCII字符
 *
 * \param id     显示器编号
 * \param index  数码管索引号
 * \param ch     要显示的ASCII字符
 *
 * \note 能显示的字符由解码函数确定
 */
aw_err_t aw_digitron_disp_char_at (int id, int index, const char ch);

/**
 * \brief 在数码管上显示ASCII字符串
 *
 * \param id    显示器编号
 * \param index 起始数码管索引号
 * \param len   显示长度
 * \param p_str 要显示的ASCII字符串
 *
 * \note 能显示的字符由解码函数确定
 */
aw_err_t aw_digitron_disp_str (int id, int index, int len, const char *p_str);


/**
 * \brief 清除所有数码管显示
 *
 * \param id    显示器编号
 */
aw_err_t aw_digitron_disp_clr (int id);

/**
 * \brief 使能数码管显示，开启扫描（默认状态是开启的）
 *
 * \param id    显示器编号
 */
aw_err_t aw_digitron_disp_enable (int id);

/**
 * \brief 禁能数码管显示,停止扫描
 *
 * \param id   显示器编号
 */
aw_err_t aw_digitron_disp_disable (int id);

/** @} grp_aw_if_digitron_disp */

#ifdef __cplusplus
}
#endif

#endif /* __AW_DIGITRON_DISP_H */

/* end of file */
