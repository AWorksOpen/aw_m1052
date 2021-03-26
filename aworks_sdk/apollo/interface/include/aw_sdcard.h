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
 * \brief SDCard标准接口
 *
 * 使用本服务需要包含头文件
 * \code
 * #include "aw_sdcard.h"
 * \endcode
 *
 * \par 范例(简单读写操作)
 * \code
 * #include "aw_sdcard.h"
 *
 * int main()
 * {
 *      aw_sdcard_dev_t *p_sdcard;
 *
 *      //等待SD卡插入
 *      AW_FOREVER {
 *          if (!aw_sdcard_is_insert("/dev/sd0")) {
 *              aw_mdelay(200);
 *          } else {
 *              break;
 *          }
 *      }
 *
 *      //打开SD卡设备
 *      p_sdcard = aw_sdcard_open("/dev/sd0");
 *      if (p_sdcard == NULL) {
 *          //todo SD卡打开失败
 *      }
 *
 *      //SD卡数据读写
 *      uint8_t buf[512] = {0};
 *      memset(buf, 'A', sizeof(buf));
 *      aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_FALSE);
 *      memset(buf, 0x00, sizeof(buf));
 *      aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_TRUE);
 *
 *      //关闭SD卡设备
 *      awbl_sdcard_close("/dev/sd0");
 * }
 * \endcode
 *
 *
 * \internal
 * \par Modification History
 * - 1.00 17-08-30  xdn, first implementation
 * \endinternal
 */



#ifndef __AW_SDCARD_H
#define __AW_SDCARD_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */


/**
 * \addtogroup grp_aw_if_sdcard
 * \copydoc aw_sdcard.h
 * @{
 */

#include "driver/sdiocard/awbl_sdcard.h"


/**
 * \brief SDCard设备结构
 */
typedef awbl_sdcard_dev_t       aw_sdcard_dev_t;


/**
 * \brief 打开指定的SDCard
 *
 * \param[in] p_name  SDCard设备名称
 *
 * \retval    NULL    SDCard设备打开失败
 * \retval    !=NULL  SDCard设备指针
 */
aw_local aw_inline aw_sdcard_dev_t *aw_sdcard_open (const char *p_name)
{
    return awbl_sdcard_open(p_name);
}


/**
 * \brief 关闭指定的SDCard
 *
 * \param[in] p_name  SDCard设备名称
 *
 * \retval    AW_OK   SDCard设备关闭成功
 * \retval    <0      SDCard设备关闭失败
 */
aw_local aw_inline aw_err_t aw_sdcard_close (const char *p_name)
{
    return awbl_sdcard_close(p_name);
}


/**
 * \brief SDCard块读写函数
 *
 * \param[in]       p_sd        SDCard设备指针
 * \param[in]       sect_no     读/写物理块号码
 * \param[in,out]   p_data      输入/输出数据缓存区
 * \param[in]       sect_cnt    读/写块数量
 * \param[in]       read        是否为读操作，TRUE为读，FALSE为写
 *
 * \retval    AW_OK         读写成功
 * \retval    -AW_ENODEV    SDCard设备不存在
 * \retval    -AW_ETIME     SDCard设备读写超时
 * \retval    <0            其它错误
 *
 * \note SDCard读写操作需要以块为单位进行读写
 * 该接口使用时请特别注意：该接口为SD卡裸操作接口，直接操作SD卡物理块地址，
 * 不经过文件系统层使用该接口进行写操作以后将导致文件系统数据结构被破坏，
 * 直接导致数据丢失或者文件系统损坏。使用该接口操作前确保备份SD卡数据，
 * 写操作后需使用特定文件系统格式化后挂载使用文件系统操作
 */
aw_local aw_inline aw_err_t aw_sdcard_rw_sector (aw_sdcard_dev_t  *p_sd,
                                                 uint32_t          sect_no,
                                                 uint8_t          *p_data,
                                                 uint32_t          sect_cnt,
                                                 aw_bool_t         read)
{
    return awbl_sdcard_rw_sector(p_sd, 
                                 sect_no, 
                                 p_data, 
                                 sect_cnt, 
                                 read);
}


/**
 * \brief 检查指定SDCard设备是否插入
 *
 * \param[in] name    SDCard设备名称
 *
 * \retval    AW_TRUE    设备已插入
 * \retval    AW_FALSE   设备未插入
 */
aw_local aw_inline aw_bool_t aw_sdcard_is_insert (const char *name)
{
    return awbl_sdcard_is_insert(name);
}


/** @} grp_aw_if_sdcard */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif  /* __AW_SDCARD_H */

/* end of file */

