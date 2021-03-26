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
 * \brief utf16�����ַ���תutf8�����ַ���
 *
 * \param[in]  pwcs   utf16����Դ�ַ���
 * \param[in]  inlen  �ַ�������
 * \param[out] str    ת����utf8�����ַ���Ŀ�껺��
 * \param[in]  maxout ���ת������
 *
 * \return ʵ��ת����ɳ���
 */
int aw_usb_utf16s_to_utf8s (const uint16_t  *pwcs,
                            int              inlen,
                            uint8_t         *str,
                            int              maxout);


/**
 * \brief utf8�����ַ���תutf16�����ַ���
 *
 * \param[in]  str  utf8����Դ�ַ���
 * \param[in]  len  �ַ����ֽڳ���
 * \param[out] pwcs ת����utf16�����ַ���Ŀ�껺��
 *
 * \return ʵ��ת�����ֽڳ���  ��  -AW_EINVAL(��Ч����)
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
