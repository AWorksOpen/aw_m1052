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


#ifndef __AW_USB_UNICODE_H
#define __AW_USB_UNICODE_H


#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \addtogroup grp_aw_if_usb_unicode
 * \copydoc aw_usb_unicode.h
 * @{
 */


/**
 * \brief utf16编码字符串转utf8编码字符串
 *
 * \param[in]  pwcs   utf16编码源字符串
 * \param[in]  inlen  字符串长度
 * \param[out] str    转化的utf8编码字符串目标缓存
 * \param[in]  maxout 最大转换长度
 *
 * \return 实际转换完成长度
 */
int aw_usb_utf16s_to_utf8s (const uint16_t  *pwcs,
                            int              inlen,
                            uint8_t         *str,
                            int              maxout);


/**
 * \brief utf8编码字符串转utf16编码字符串
 *
 * \param[in]  str  utf8编码源字符串
 * \param[in]  len  字符串字节长度
 * \param[out] pwcs 转化的utf16编码字符串目标缓存
 *
 * \return 实际转换的字节长度  或  -AW_EINVAL(无效参数)
 */
int aw_usb_utf8s_to_utf16s(const uint8_t   *str,
                           int              len,
                           uint16_t        *pwcs);

/** @} grp_aw_if_usb_unicode */

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AW_USB_UNICODE_H */

/* end of file */
