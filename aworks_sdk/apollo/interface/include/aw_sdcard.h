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
 * \brief SDCard��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ�
 * \code
 * #include "aw_sdcard.h"
 * \endcode
 *
 * \par ����(�򵥶�д����)
 * \code
 * #include "aw_sdcard.h"
 *
 * int main()
 * {
 *      aw_sdcard_dev_t *p_sdcard;
 *
 *      //�ȴ�SD������
 *      AW_FOREVER {
 *          if (!aw_sdcard_is_insert("/dev/sd0")) {
 *              aw_mdelay(200);
 *          } else {
 *              break;
 *          }
 *      }
 *
 *      //��SD���豸
 *      p_sdcard = aw_sdcard_open("/dev/sd0");
 *      if (p_sdcard == NULL) {
 *          //todo SD����ʧ��
 *      }
 *
 *      //SD�����ݶ�д
 *      uint8_t buf[512] = {0};
 *      memset(buf, 'A', sizeof(buf));
 *      aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_FALSE);
 *      memset(buf, 0x00, sizeof(buf));
 *      aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_TRUE);
 *
 *      //�ر�SD���豸
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
 * \brief SDCard�豸�ṹ
 */
typedef awbl_sdcard_dev_t       aw_sdcard_dev_t;


/**
 * \brief ��ָ����SDCard
 *
 * \param[in] p_name  SDCard�豸����
 *
 * \retval    NULL    SDCard�豸��ʧ��
 * \retval    !=NULL  SDCard�豸ָ��
 */
aw_local aw_inline aw_sdcard_dev_t *aw_sdcard_open (const char *p_name)
{
    return awbl_sdcard_open(p_name);
}


/**
 * \brief �ر�ָ����SDCard
 *
 * \param[in] p_name  SDCard�豸����
 *
 * \retval    AW_OK   SDCard�豸�رճɹ�
 * \retval    <0      SDCard�豸�ر�ʧ��
 */
aw_local aw_inline aw_err_t aw_sdcard_close (const char *p_name)
{
    return awbl_sdcard_close(p_name);
}


/**
 * \brief SDCard���д����
 *
 * \param[in]       p_sd        SDCard�豸ָ��
 * \param[in]       sect_no     ��/д��������
 * \param[in,out]   p_data      ����/������ݻ�����
 * \param[in]       sect_cnt    ��/д������
 * \param[in]       read        �Ƿ�Ϊ��������TRUEΪ����FALSEΪд
 *
 * \retval    AW_OK         ��д�ɹ�
 * \retval    -AW_ENODEV    SDCard�豸������
 * \retval    -AW_ETIME     SDCard�豸��д��ʱ
 * \retval    <0            ��������
 *
 * \note SDCard��д������Ҫ�Կ�Ϊ��λ���ж�д
 * �ýӿ�ʹ��ʱ���ر�ע�⣺�ýӿ�ΪSD��������ӿڣ�ֱ�Ӳ���SD��������ַ��
 * �������ļ�ϵͳ��ʹ�øýӿڽ���д�����Ժ󽫵����ļ�ϵͳ���ݽṹ���ƻ���
 * ֱ�ӵ������ݶ�ʧ�����ļ�ϵͳ�𻵡�ʹ�øýӿڲ���ǰȷ������SD�����ݣ�
 * д��������ʹ���ض��ļ�ϵͳ��ʽ�������ʹ���ļ�ϵͳ����
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
 * \brief ���ָ��SDCard�豸�Ƿ����
 *
 * \param[in] name    SDCard�豸����
 *
 * \retval    AW_TRUE    �豸�Ѳ���
 * \retval    AW_FALSE   �豸δ����
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

