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
 * \brief ftl flash interface
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_FLASH_H
#define __AW_FTL_FLASH_H

/**
 * \addtogroup grp_aw_if_ftl_flash
 * \copydoc aw_ftl_flash.h
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

struct aw_ftl_blk_tag {
    uint8_t  flag;
    uint8_t  log;
    uint16_t lbn;
};

struct aw_ftl_sector_tag {
    uint8_t  flag;
    uint8_t  sn;
    uint16_t lbn;
};

/**
 * \brief �����ͷ��Ϣ�ͱ�ǩ��ȡ
 *
 * \param[in] p_ftl     : FTL�豸�ṹ��
 * \param[in] pbn       : �������
 * \param[in] p_blk_hdr : ָ�������ͷ��Ϣ�ṹ�壬��ΪNULL
 * \param[in] p_tag     : ҳ0�ı�ǩ����ΪNULL
 *
 * \retval AW_OK : ��ȡ�ɹ�
 * \retval ELSE      : MTD��ȡʧ��
 */
int aw_ftl_blk_hdr_read (struct aw_ftl_dev     *p_ftl,
                         uint16_t               pbn,
                         struct aw_ftl_pb_hdr  *p_blk_hdr,
                         struct aw_ftl_blk_tag *p_tag);

/**
 * \brief �����ͷ��Ϣ�ͱ�ǩд��
 *
 * \param[in] p_ftl     : FTL�豸�ṹ��
 * \param[in] pbn       : �������
 * \param[in] p_blk_hdr : ָ�������ͷ��Ϣ�ṹ�壬��ΪNULL
 * \param[in] p_tag     : ҳ0�ı�ǩ����ΪNULL
 *
 * \retval AW_OK : д��ɹ�
 * \retval ELSE  : MTDд��ʧ��
 */
int aw_ftl_blk_hdr_write (struct aw_ftl_dev     *p_ftl,
                          uint16_t               pbn,
                          struct aw_ftl_pb_hdr  *p_blk_hdr,
                          struct aw_ftl_blk_tag *p_tag);

/**
 * \brief ����ҳ���ݶ�ȡ
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] pbn   : �������
 * \param[in] idx   : ����ҳ����
 * \param[in] tag   : ��ȡ�����ı�ǩֵ
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_flash_page_read_with_tag (struct aw_ftl_dev *p_ftl,
                                     uint16_t           pbn,
                                     uint16_t           idx,
                                     uint8_t           *data,
                                     void              *tag,
                                     uint8_t            tag_size);

/**
 * \brief ������ҳд����ҳ��С������
 *
 * \param[in] p_ftl    : FTL�豸�ṹ��
 * \param[in] pbn      : �������
 * \param[in] idx      : ��������λ������
 * \param[in] data     : Ҫд�������
 * \param[in] tag      : Ҫд���ǩ
 * \param[in] tag_size : ��ǩ�Ĵ�С
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_flash_page_write_with_tag (struct aw_ftl_dev *p_ftl,
                                      uint16_t           pbn,
                                      uint16_t           idx,
                                      uint8_t           *data,
                                      void              *tag,
                                      uint8_t            tag_size);

/**
 * \brief ��������ȡһ����������
 *
 * \param[in] p_ftl  : FTL�豸�ṹ��
 * \param[in] pbn    : �������
 * \param[in] idx    : ��������λ������
 * \param[in] data   : ��ȡ����������
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_flash_page_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           pbn,
                            uint16_t           idx,
                            uint8_t           *data);

/**
 * \brief ������ǩ��Ϣ��ȡ
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] pbn   : �������
 * \param[in] idx   : ������(����ҳ���� - sectors_per_blk_hdr)
 * \param[in] tag   : ��ȡ�����ı�ǩֵ
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_flash_page_tag_read (struct aw_ftl_dev        *p_ftl,
                                uint16_t                  pbn,
                                uint16_t                  idx,
                                struct aw_ftl_sector_tag *tag);

/**
 * \brief ������ǩ��Ϣд��
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] pbn   : �������
 * \param[in] idx   : ������(����ҳ���� - sectors_per_blk_hdr)
 * \param[in] tag   : д��ı�ǩ
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_flash_page_tag_write (struct aw_ftl_dev        *p_ftl,
                                 uint16_t                  pbn,
                                 uint16_t                  idx,
                                 struct aw_ftl_sector_tag *tag);

/**
 * \brief ������ʽ��
 *
 * \param[in] p_ftl     : FTL�豸�ṹ��
 * \param[in] pbn       : �������
 *
 * \retval AW_OK  : �����ɹ�
 * \retval -EPERM : ���ֻ���
 * \retval -EBUSY : ����ʧ��
 * \retval -EIO   : MTD����ʧ��
 */
int aw_ftl_blk_format (struct aw_ftl_dev *p_ftl, uint16_t  pbn);

/**
 * \brief ��������
 *
 * \param[in] p_ftl  : FTL�豸�ṹ��
 * \param[in] pbn    : �������
 *
 * \retval AW_OK     : �����ɹ�
 * \retval -AW_EPERM : ���ֻ���
 * \retval -AW_EIO   : IO����
 */
aw_err_t aw_ftl_mtd_block_erase (struct aw_ftl_dev *p_ftl, uint16_t  pbn);

aw_err_t aw_ftl_mtd_oob_read (struct aw_ftl_dev *p_ftl,
                              uint16_t           pbn,
                              uint16_t           page,
                              uint8_t           *data,
                              void              *tag,
                              uint8_t            tag_size);

aw_err_t aw_ftl_mtd_oob_write ( struct aw_ftl_dev *p_ftl,
                                uint16_t           pbn,
                                uint16_t           page,
                                uint8_t           *data,
                                void              *tag,
                                uint8_t            tag_size);

/** @}  grp_aw_if_ftl_flash */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_FLASH_H */



/* end of file */
