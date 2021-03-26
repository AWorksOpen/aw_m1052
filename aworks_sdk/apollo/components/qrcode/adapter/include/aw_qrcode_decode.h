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
 * \file   aw_qrcode_decode.h
 * \brief  qrcode
 *
 * \internal
 * \par modification history:
 * - 1.00 15-11-10  dcf, first implementation.
 * \endinternal
 */
 
#ifndef __AW_QRCODE_DECODE_H__
#define __AW_QRCODE_DECODE_H__
/**
* \brief  QR码解码函数
*
* \param[in]    data            图片数据
* \param[in]    width           图片的宽
* \param[in]    height          图片的高
* \param[out]   msg             解码信息
* \return                       返回解码标识符:-4~0.
* \note:
* 1.输入的图片数据data为四通道格式数据，在解码过程中，函数将取第一个
*   通道的数据进行解码，即：decode_data[k++] = data[i+=4]；
* 2.解码标识符的信息分别表示：
*     0：表解码成功；
*    -1：表解码失败，未检测到二维码；
*    -2：表解码失败，图片的长与宽均要大于0；
*    -3：表解码失败，解码出错；
*    -4：表解码失败，解码出错。
*/
#ifdef __cplusplus
extern "C" {
#endif

//int aw_qrcode_decode(const unsigned char *data, int width, int height, std::string *msg);
int aw_qrcode_decode(const unsigned char *data, int width, int height, char *p_buf, int size);

#ifdef __cplusplus
}
#endif
#endif
