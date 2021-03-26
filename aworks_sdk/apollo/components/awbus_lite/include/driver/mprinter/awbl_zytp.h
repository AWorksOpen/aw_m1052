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
 * \brief ZYTP打印机通用驱动，依赖于UART，USB驱动
 *
 * ZYTP微型打印机是广州周立功单片机科技有限公司推出了多款热敏微型打印机产品，
 * 该产品功能完善，支持五十多条常用ESC/POS指令。
 *
 * \internal
 * \par modification history:
 * - 1.00 16-04-25  bob, first implementation
 * \endinternal
 */

/**
 * \addtogroup grp_awbl_drv_zytp
 * \copydetails awbl_zytp.h
 * @{
 */

/** @} grp_awbl_drv_zytp */

#ifndef __AWBL_ZYTP_H
#define __AWBL_ZYTP_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "aw_sem.h"
#include "aw_mprinter.h"
#include "io/aw_io_device.h"

#define AWBL_ZYTP_XX_NAME       "ZYTP"


/** \brief 打印并进纸 */
#define AWBL_ZYTP_POS_PRINT             ((uint64_t)1 << 0)

/** \brief 回车*/
#define AWBL_ZYTP_POS_ENTER             ((uint64_t)1 << 1)

/** \brief 打印并将标签/黑标进纸到打印起始位置 */
#define AWBL_ZYTP_POS_PAFSP             ((uint64_t)1 << 2)

/** \brief 将标签/黑标进纸到打印起始位置 */
#define AWBL_ZYTP_POS_FSP               ((uint64_t)1 << 3)

/** \brief 打印并进纸n点 */
#define AWBL_ZYTP_POS_PAFNP             ((uint64_t)1 << 4)

/** \brief 打印并退纸n点 */
#define AWBL_ZYTP_POS_PABNP             ((uint64_t)1 << 5)

/** \brief 打印并进纸n行 */
#define AWBL_ZYTP_POS_PAFNFL            ((uint64_t)1 << 6)

/** \brief 打印并退纸n行 */
#define AWBL_ZYTP_POS_PAFBFL            ((uint64_t)1 << 7)

/** \brief 设置打印位置为打印行起点 */
#define AWBL_ZYTP_POS_SPPTFLS           ((uint64_t)1 << 8)

/** \brief 设置行间距为n点 */
#define AWBL_ZYTP_POS_SLSNP             ((uint64_t)1 << 9)

/** \brief 设置行间距为默认值 */
#define AWBL_ZYTP_POS_SLSDF             ((uint64_t)1 << 10)

/** \brief 设置左边距 */
#define AWBL_ZYTP_POS_SLFM              ((uint64_t)1 << 11)

/** \brief 设置右边距 */
#define AWBL_ZYTP_POS_SRGM              ((uint64_t)1 << 12)

/** \brief 设定左侧空白量*/
#define AWBL_ZYTP_POS_SLFBLK            ((uint64_t)1 << 13)

/** \brief 设置右侧字符间距 */
#define AWBL_ZYTP_POS_SRGCSP            ((uint64_t)1 << 14)

/** \brief 设置打印区域宽度 */
#define AWBL_ZYTP_POS_SPSWD             ((uint64_t)1 << 15)

/** \brief 设置绝对打印位置 */
#define AWBL_ZYTP_POS_SABSPP            ((uint64_t)1 << 16)

/** \brief 设置相对打印位置 */
#define AWBL_ZYTP_POS_SRLTPP            ((uint64_t)1 << 17)

/** \brief 设置字符打印方式 */
#define AWBL_ZYTP_POS_SCPT              ((uint64_t)1 << 18)

/** \brief 设定/解除下划线 */
#define AWBL_ZYTP_POS_SRUNDL            ((uint64_t)1 << 19)

/** \brief 设定/解除粗体打印 */
#define AWBL_ZYTP_POS_SRBFP             ((uint64_t)1 << 20)

/** \brief 设置打印对齐方式*/
#define AWBL_ZYTP_POS_SPALIT            ((uint64_t)1 << 21)

/** \brief 设置/解除颠倒打印模式 */
#define AWBL_ZYTP_POS_SRRVSMD           ((uint64_t)1 << 22)

/** \brief 设定/解除反白打印模式 */
#define AWBL_ZYTP_POS_SRANTWMD          ((uint64_t)1 << 23)

/** \brief 设置打印灰度 */
#define AWBL_ZYTP_POS_SPGYS             ((uint64_t)1 << 24)

/** \brief 设置打印速度 */
#define AWBL_ZYTP_POS_SPTATE            ((uint64_t)1 << 25)

/** \brief 设置打印字体 */
#define AWBL_ZYTP_POS_SPFONT            ((uint64_t)1 << 26)

/** \brief 设置/解除顺时针90°旋转 */
#define AWBL_ZYTP_POS_SRCLKWSRT         ((uint64_t)1 << 27)

/** \brief 设定字符大小 */
#define AWBL_ZYTP_POS_SFNTSZ            ((uint64_t)1 << 28)

/** \brief 选择汉字模式 */
#define AWBL_ZYTP_POS_SCHNMD            ((uint64_t)1 << 29)

/** \brief 取消汉字模式 */
#define AWBL_ZYTP_POS_RCHNMD            ((uint64_t)1 << 30)

/** \brief 选择国标字符集 */
#define AWBL_ZYTP_POS_SINTLFNT          ((uint64_t)1 << 31)

/** \brief 选择字符代码页 */
#define AWBL_ZYTP_POS_SFNTCDP           ((uint64_t)1 << 32)

/** \brief 选择打印纸传感器以停止打印 */
#define AWBL_ZYTP_POS_SSNRTSP           ((uint64_t)1 << 33)

/** \brief 激活/禁止面板按键*/
#define AWBL_ZYTP_POS_SRBLKKY           ((uint64_t)1 << 34)

/** \brief 图形垂直取模数据填充 */
#define AWBL_ZYTP_POS_GDMVP             ((uint64_t)1 << 35)

/** \brief 图片水平取模数据打印 */
#define AWBL_ZYTP_POS_GLMDP             ((uint64_t)1 << 36)

/** \brief 打印NV位图*/
#define AWBL_ZYTP_POS_PNVIMG            ((uint64_t)1 << 37)

/** \brief 下载NV位图 */
#define AWBL_ZYTP_POS_DLNVIMG           ((uint64_t)1 << 38)

/** \brief 水平制表 */
#define AWBL_ZYTP_POS_LVLCTTAB          ((uint64_t)1 << 39)

/** \brief 设置水平制表位置 */
#define AWBL_ZYTP_POS_LVLCTTABP         ((uint64_t)1 << 40)

/** \brief 打印90°表格 */
#define AWBL_ZYTP_POS_PRGHTANG          ((uint64_t)1 << 41)

/** \brief 设置一维条码可读字符（HRI）打印位置 */
#define AWBL_ZYTP_POS_SHRIPP            ((uint64_t)1 << 42)

/** \brief 设置一维条码高度 */
#define AWBL_ZYTP_POS_SODCH             ((uint64_t)1 << 43)

/** \brief 设置一维条码宽度*/
#define AWBL_ZYTP_POS_SODCW             ((uint64_t)1 << 44)

/** \brief 设置一维条码字体*/
#define AWBL_ZYTP_POS_SODCF             ((uint64_t)1 << 45)

/** \brief 打印一维条码 */
#define AWBL_ZYTP_POS_PODC              ((uint64_t)1 << 46)

/** \brief 设置/打印二维码*/
#define AWBL_ZYTP_POS_SPQRC             ((uint64_t)1 << 47)

/** \brief 标签纸/黑标纸指令*/
#define AWBL_ZYTP_POS_SLBABLKP          ((uint64_t)1 << 48)

/** \brief 设置调整值 */
#define AWBL_ZYTP_POS_SADJVAL           ((uint64_t)1 << 49)

/** \brief 查询打印机状态（实时）*/
#define AWBL_ZYTP_POS_RTQPS             ((uint64_t)1 << 50)

/** \brief 设置/取消打印机状态自动返回*/
#define AWBL_ZYTP_POS_SRPSAB            ((uint64_t)1 << 51)

/** \brief 传送状态（非实时） */
#define AWBL_ZYTP_POS_DLSTAT            ((uint64_t)1 << 52)

/** \brief 初始化打印机 */
#define AWBL_ZYTP_POS_PINIT             ((uint64_t)1 << 53)

/** \brief 清打印缓存（实时） */
#define AWBL_ZYTP_POS_RLCLPBUF          ((uint64_t)1 << 54)

/** \brief 进纸并切纸 */
#define AWBL_ZYTP_POS_FDCTPAP           ((uint64_t)1 << 55)

/** \brief 设置串口通讯参数 */
#define AWBL_ZYTP_POS_SCMMPAP           ((uint64_t)1 << 56)

/** \brief 选择打印控制模式 */
#define AWBL_ZYTP_POS_SPCTLMD           ((uint64_t)1 << 57)

/** \brief 选择打印头控制模式 */
#define AWBL_ZYTP_POS_SPHCTLMD          ((uint64_t)1 << 58)

/** \brief 传送打印机ID */
#define AWBL_ZYTP_POS_DLPRTID           ((uint64_t)1 << 59)

/** \brief 进入/退出低功耗模式（实时） */
#define AWBL_ZYTP_POS_SRLOWPMD          ((uint64_t)1 << 60)

/** \brief 设置/读写打印机配置 */
#define AWBL_ZYTP_POS_SRCFG             ((uint64_t)1 << 61)


/** \brief 型号AWBL_ZYTP58_XX4A  */
#define AWBL_ZYTP58_XX4A  (                        \
                           AWBL_ZYTP_POS_PRINT     | \
                           AWBL_ZYTP_POS_ENTER     | \
                           AWBL_ZYTP_POS_PAFNP     | \
                           AWBL_ZYTP_POS_PABNP     | \
                           AWBL_ZYTP_POS_PAFNFL    | \
                           AWBL_ZYTP_POS_PAFBFL    | \
                           AWBL_ZYTP_POS_SLSNP     | \
                           AWBL_ZYTP_POS_SLSDF     | \
                           AWBL_ZYTP_POS_SLFM      | \
                           AWBL_ZYTP_POS_SRGM      | \
                           AWBL_ZYTP_POS_SABSPP    | \
                           AWBL_ZYTP_POS_SPALIT    | \
                           AWBL_ZYTP_POS_SPGYS     | \
                           AWBL_ZYTP_POS_GDMVP     | \
                           AWBL_ZYTP_POS_GLMDP     | \
                           AWBL_ZYTP_POS_LVLCTTAB  | \
                           AWBL_ZYTP_POS_LVLCTTABP | \
                           AWBL_ZYTP_POS_RTQPS     | \
                           AWBL_ZYTP_POS_SRPSAB    | \
                           AWBL_ZYTP_POS_DLSTAT    | \
                           AWBL_ZYTP_POS_PINIT     | \
                           AWBL_ZYTP_POS_RLCLPBUF  | \
                           AWBL_ZYTP_POS_SCMMPAP   | \
                           AWBL_ZYTP_POS_SRLOWPMD  | \
                           AWBL_ZYTP_POS_SRCFG       \
                          )

/** \brief 型号AWBL_ZYTP58_XX4B  */
#define AWBL_ZYTP58_XX4B (                         \
                          AWBL_ZYTP_POS_PRINT      | \
                          AWBL_ZYTP_POS_ENTER      | \
                          AWBL_ZYTP_POS_PAFNP      | \
                          AWBL_ZYTP_POS_PABNP      | \
                          AWBL_ZYTP_POS_PAFNFL     | \
                          AWBL_ZYTP_POS_PAFBFL     | \
                          AWBL_ZYTP_POS_SLSNP      | \
                          AWBL_ZYTP_POS_SLSDF      | \
                          AWBL_ZYTP_POS_SLFM       | \
                          AWBL_ZYTP_POS_SRGM       | \
                          AWBL_ZYTP_POS_SABSPP     | \
                          AWBL_ZYTP_POS_SCPT       | \
                          AWBL_ZYTP_POS_SPALIT     | \
                          AWBL_ZYTP_POS_SPGYS      | \
                          AWBL_ZYTP_POS_GDMVP      | \
                          AWBL_ZYTP_POS_GLMDP      | \
                          AWBL_ZYTP_POS_LVLCTTAB   | \
                          AWBL_ZYTP_POS_LVLCTTABP  | \
                          AWBL_ZYTP_POS_SHRIPP     | \
                          AWBL_ZYTP_POS_SODCH      | \
                          AWBL_ZYTP_POS_SODCW      | \
                          AWBL_ZYTP_POS_SODCF      | \
                          AWBL_ZYTP_POS_PODC       | \
                          AWBL_ZYTP_POS_RTQPS      | \
                          AWBL_ZYTP_POS_SRPSAB     | \
                          AWBL_ZYTP_POS_DLSTAT     | \
                          AWBL_ZYTP_POS_PINIT      | \
                          AWBL_ZYTP_POS_RLCLPBUF   | \
                          AWBL_ZYTP_POS_SCMMPAP    | \
                          AWBL_ZYTP_POS_SRLOWPMD   | \
                          AWBL_ZYTP_POS_SRCFG        \
                         )

/** \brief 型号AWBL_ZYTP58_XX5B  */
#define AWBL_ZYTP58_XX5B  (                         \
                           AWBL_ZYTP_POS_PRINT      | \
                           AWBL_ZYTP_POS_ENTER      | \
                           AWBL_ZYTP_POS_PAFNP      | \
                           AWBL_ZYTP_POS_PABNP      | \
                           AWBL_ZYTP_POS_PAFNFL     | \
                           AWBL_ZYTP_POS_PAFBFL     | \
                           AWBL_ZYTP_POS_SLSNP      | \
                           AWBL_ZYTP_POS_SLSDF      | \
                           AWBL_ZYTP_POS_SLFM       | \
                           AWBL_ZYTP_POS_SRGM       | \
                           AWBL_ZYTP_POS_SABSPP     | \
                           AWBL_ZYTP_POS_SCPT       | \
                           AWBL_ZYTP_POS_SPALIT     | \
                           AWBL_ZYTP_POS_SPGYS      | \
                           AWBL_ZYTP_POS_SPTATE     | \
                           AWBL_ZYTP_POS_SPFONT     | \
                           AWBL_ZYTP_POS_GDMVP      | \
                           AWBL_ZYTP_POS_GLMDP      | \
                           AWBL_ZYTP_POS_LVLCTTAB   | \
                           AWBL_ZYTP_POS_LVLCTTABP  | \
                           AWBL_ZYTP_POS_SHRIPP     | \
                           AWBL_ZYTP_POS_SODCH      | \
                           AWBL_ZYTP_POS_SODCW      | \
                           AWBL_ZYTP_POS_SODCF      | \
                           AWBL_ZYTP_POS_PODC       | \
                           AWBL_ZYTP_POS_RTQPS      | \
                           AWBL_ZYTP_POS_SRPSAB     | \
                           AWBL_ZYTP_POS_DLSTAT     | \
                           AWBL_ZYTP_POS_PINIT      | \
                           AWBL_ZYTP_POS_RLCLPBUF   | \
                           AWBL_ZYTP_POS_SCMMPAP    | \
                           AWBL_ZYTP_POS_SRLOWPMD   | \
                           AWBL_ZYTP_POS_SRCFG        \
                          )

/** \brief 型号AWBL_ZYTP58_XX6B  */
#define AWBL_ZYTP58_XX6B  (                        \
                           AWBL_ZYTP_POS_PRINT     | \
                           AWBL_ZYTP_POS_ENTER     | \
                           AWBL_ZYTP_POS_PAFNP     | \
                           AWBL_ZYTP_POS_PABNP     | \
                           AWBL_ZYTP_POS_PAFNFL    | \
                           AWBL_ZYTP_POS_PAFBFL    | \
                           AWBL_ZYTP_POS_SLSNP     | \
                           AWBL_ZYTP_POS_SLSDF     | \
                           AWBL_ZYTP_POS_SLFM      | \
                           AWBL_ZYTP_POS_SRGM      | \
                           AWBL_ZYTP_POS_SABSPP    | \
                           AWBL_ZYTP_POS_SCPT      | \
                           AWBL_ZYTP_POS_SPALIT    | \
                           AWBL_ZYTP_POS_SPGYS     | \
                           AWBL_ZYTP_POS_SPTATE    | \
                           AWBL_ZYTP_POS_SCHNMD    | \
                           AWBL_ZYTP_POS_RCHNMD    | \
                           AWBL_ZYTP_POS_SINTLFNT  | \
                           AWBL_ZYTP_POS_SFNTCDP   | \
                           AWBL_ZYTP_POS_GDMVP     | \
                           AWBL_ZYTP_POS_GLMDP     | \
                           AWBL_ZYTP_POS_PNVIMG    | \
                           AWBL_ZYTP_POS_DLNVIMG   | \
                           AWBL_ZYTP_POS_LVLCTTAB  | \
                           AWBL_ZYTP_POS_LVLCTTABP | \
                           AWBL_ZYTP_POS_SHRIPP    | \
                           AWBL_ZYTP_POS_SODCH     | \
                           AWBL_ZYTP_POS_SODCW     | \
                           AWBL_ZYTP_POS_SODCF     | \
                           AWBL_ZYTP_POS_PODC      | \
                           AWBL_ZYTP_POS_RTQPS     | \
                           AWBL_ZYTP_POS_SRPSAB    | \
                           AWBL_ZYTP_POS_DLSTAT    | \
                           AWBL_ZYTP_POS_PINIT     | \
                           AWBL_ZYTP_POS_RLCLPBUF  | \
                           AWBL_ZYTP_POS_SCMMPAP   | \
                           AWBL_ZYTP_POS_DLPRTID   | \
                           AWBL_ZYTP_POS_SRLOWPMD  | \
                           AWBL_ZYTP_POS_SRCFG       \
                          )

/** \brief 型号AWBL_ZYTP58_XX4C  */
#define AWBL_ZYTP58_XX4C   (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_PRGHTANG  | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD  | \
                            AWBL_ZYTP_POS_SRCFG       \
                           )

/** \brief 型号AWBL_ZYTP58_XX4BC  */
#define AWBL_ZYTP58_XX4BC  (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD  | \
                            AWBL_ZYTP_POS_SRCFG       \
                           )

/** \brief 型号AWBL_ZYTP80_XX4EC  */
#define AWBL_ZYTP80_XX4EC  (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFSP     | \
                            AWBL_ZYTP_POS_FSP       | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_SPPTFLS   | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SLFBLK    | \
                            AWBL_ZYTP_POS_SRGCSP    | \
                            AWBL_ZYTP_POS_SPSWD     | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SRLTPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SRUNDL    | \
                            AWBL_ZYTP_POS_SRBFP     | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SRRVSMD   | \
                            AWBL_ZYTP_POS_SRANTWMD  | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_SPTATE    | \
                            AWBL_ZYTP_POS_SPFONT    | \
                            AWBL_ZYTP_POS_SRCLKWSRT | \
                            AWBL_ZYTP_POS_SFNTSZ    | \
                            AWBL_ZYTP_POS_SFNTCDP   | \
                            AWBL_ZYTP_POS_SSNRTSP   | \
                            AWBL_ZYTP_POS_SRBLKKY   | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_PNVIMG    | \
                            AWBL_ZYTP_POS_DLNVIMG   | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_SPQRC     | \
                            AWBL_ZYTP_POS_SLBABLKP  | \
                            AWBL_ZYTP_POS_SADJVAL   | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SPCTLMD   | \
                            AWBL_ZYTP_POS_SPHCTLMD  | \
                            AWBL_ZYTP_POS_DLPRTID     \
                           )

/** \brief 型号AWBL_ZY_TP01  */
#define AWBL_ZY_TP01      (                         \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SRLTPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_FDCTPAP   | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief 型号AWBL_ZY_TP11  */
#define AWBL_ZY_TP11      (                         \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief 型号AWBL_ZY_TP12  */
#define AWBL_ZY_TP12       (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_RLCLPBUF  | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/** \brief 型号AWBL_ZY_TP21  */
#define AWBL_ZY_TP21       (                        \
                            AWBL_ZYTP_POS_PRINT     | \
                            AWBL_ZYTP_POS_ENTER     | \
                            AWBL_ZYTP_POS_PAFNP     | \
                            AWBL_ZYTP_POS_PABNP     | \
                            AWBL_ZYTP_POS_PAFNFL    | \
                            AWBL_ZYTP_POS_PAFBFL    | \
                            AWBL_ZYTP_POS_SLSNP     | \
                            AWBL_ZYTP_POS_SLSDF     | \
                            AWBL_ZYTP_POS_SLFM      | \
                            AWBL_ZYTP_POS_SRGM      | \
                            AWBL_ZYTP_POS_SABSPP    | \
                            AWBL_ZYTP_POS_SCPT      | \
                            AWBL_ZYTP_POS_SPALIT    | \
                            AWBL_ZYTP_POS_SPGYS     | \
                            AWBL_ZYTP_POS_SPTATE    | \
                            AWBL_ZYTP_POS_SCHNMD    | \
                            AWBL_ZYTP_POS_RCHNMD    | \
                            AWBL_ZYTP_POS_SINTLFNT  | \
                            AWBL_ZYTP_POS_SFNTCDP   | \
                            AWBL_ZYTP_POS_GDMVP     | \
                            AWBL_ZYTP_POS_GLMDP     | \
                            AWBL_ZYTP_POS_PNVIMG    | \
                            AWBL_ZYTP_POS_DLNVIMG   | \
                            AWBL_ZYTP_POS_LVLCTTAB  | \
                            AWBL_ZYTP_POS_LVLCTTABP | \
                            AWBL_ZYTP_POS_PRGHTANG  | \
                            AWBL_ZYTP_POS_SHRIPP    | \
                            AWBL_ZYTP_POS_SODCH     | \
                            AWBL_ZYTP_POS_SODCW     | \
                            AWBL_ZYTP_POS_SODCF     | \
                            AWBL_ZYTP_POS_PODC      | \
                            AWBL_ZYTP_POS_SLBABLKP  | \
                            AWBL_ZYTP_POS_RTQPS     | \
                            AWBL_ZYTP_POS_SRPSAB    | \
                            AWBL_ZYTP_POS_DLSTAT    | \
                            AWBL_ZYTP_POS_PINIT     | \
                            AWBL_ZYTP_POS_SCMMPAP   | \
                            AWBL_ZYTP_POS_SRLOWPMD    \
                           )

/**
 * \brief 状态返回回调函数
 */
typedef int (*aw_pfn_zytp_rsp_isr_t)(void *p_arg);

/**
 * \brief ZYTP 设备信息
 */
struct awbl_zytp_devinfo {
    /** \brief 打印机名 */
    const char            *p_name;

    /** \brief 打印机设备型号 */
    uint64_t               type;
};

/**
 * \brief ZYTP 设备实例
 */
struct awbl_zytp_dev {
    struct awbl_dev  dev;          /**< \brief 继承 awbl_dev 设备 */
    struct aw_iodev  io_dev;       /**< \brief io设备 */

    /** \brief 过压回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_over_vltg;
    /** \brief 过压回调函数参数           */
    void                 *p_over_vltg_arg;

    /** \brief 过压恢复回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_over_vltg_resume;
    /** \brief 过压恢复回调函数参数           */
    void                 *p_over_vltg_resume_arg;

    /** \brief 压轴回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_axo_open;
    /** \brief 压轴回调函数参数           */
    void                 *p_axo_open_arg;

    /** \brief 压轴恢复回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_axo_close;
    /** \brief 压轴恢复回调函数参数           */
    void                 *p_axo_close_arg;

    /** \brief 过热回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_over_heat;
    /** \brief 过热回调函数参数           */
    void                 *p_over_heat_arg;

    /** \brief 过热恢复回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_over_heat_resume;
    /** \brief 过热恢复回调函数参数           */
    void                 *p_over_heat_resume_arg;

    /** \brief 缺纸回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_of_paper;
    /** \brief 缺纸回调函数参数           */
    void                 *p_of_paper_arg;

    /** \brief 缺纸恢复回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_of_paper_resume;
    /** \brief 缺纸恢复回调函数参数           */
    void                 *p_of_paper_resume_arg;


    /** \brief 切刀移出回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_cutter_on;
    /** \brief 切刀移出回调函数参数           */
    void                 *p_cutter_on_arg;

    /** \brief 切刀关闭恢复回调函数               */
    aw_pfn_zytp_rsp_isr_t pfn_cutter_off;
    /** \brief 切刀关闭恢复回调函数参数           */
    void                 *p_cutter_off_arg;

    /** \brief 记录微型打印机的实时状态           */
    uint8_t               zytp_stat;

    /** \brief 自动状态返回标志，为TRUE时若打印机状态有任何变化都会返回       */
    uint8_t               auto_stat_flag;

    /** \brief 指向微型打印机正在处理的指令参数           */
    aw_mprinter_params_t *p_mp_param;
};

/**
 * \brief zytp driver register
 */
void awbl_zytp_drv_register(void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AWBL_ZYTP_H */

/* end of file */
