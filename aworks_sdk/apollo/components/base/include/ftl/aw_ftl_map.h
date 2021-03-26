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
 * \brief ��ַӳ�书��
 *
 * \internal
 * \par modification history:
 * - 1.00 2017-4-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_MAP_H
#define __AW_FTL_MAP_H

/**
 * \addtogroup grp_aw_if_ftl_map
 * \copydoc aw_ftl_map.h
 * @{
 */

#include "aworks.h"


#ifdef __cplusplus
extern "C" {
#endif


/** \brief NOR FLASH����������С�̶�Ϊ512 */
#define AW_FTL_NOR_SECTOR_SIZE        512

/** \brief δ����������� */
#define AW_FTL_UNASSIGN_BLK_NO        0xFFFF

/** \brief δ����������� */
#define AW_FTL_UNASSIGN_SECTOR_NO     0xFFFFFFFF

/**
 * \brief FTL������ǩ
 * \anchor aw_ftl_block_tag
 * @ {
 */

/** \brief �����������ͱ�ǩ */
#define AW_FTL_BLOCK_TAG_FREE         0xFF

/** \brief ϵͳ��Ϣ�����ͱ�ǩ */
#define AW_FTL_BLOCK_TAG_SYS_INFO     0xAA

/** \brief ��־�����ͱ�ǩ */
#define AW_FTL_BLOCK_TAG_LOG          0x77

/** \brief ���ݿ����ͱ�ǩ */
#define AW_FTL_BLOCK_TAG_DATA         0x55

/** 
 * \brief ��һ�׶Σ����ݿ����ڴӾɿ�д�����ݣ�
 *        ��ɺ�д��AW_FTL_BLOCK_TAG_DATA_IN_LOG 
 */
#define AW_FTL_BLOCK_TAG_DATA_IN_BLK  0x5F

/** 
 * \brief �ڶ��׶Σ����ݿ����ڴ���־��д�����ݣ�
 *        ��ɺ�д��AW_FTL_BLOCK_TAG_DATA 
 */
#define AW_FTL_BLOCK_TAG_DATA_IN_LOG  0x57

/** \brief ��������Ҫ�����־ */
#define AW_FTL_BLOCK_TAG_GARBAGE      0x00

/**
 * @} aw_ftl_block_tag
 */


/**
 * \brief FTL������ǩ
 * \anchor aw_ftl_sector_tag
 * @ {
 */

/** \brief �������б�־ */
#define AW_FTL_SECTOR_TAG_FREE        0xFF

/** \brief ����������Ч��־ */
#define AW_FTL_SECTOR_TAG_GARBAGE     0x00

/** \brief �����洢���ݵı�־ */
#define AW_FTL_SECTOR_TAG_DATA        0x44

/**
 * @} aw_ftl_sector_tag
 */


/**
 * \brief FTL�߼�����Ϣ
 */
struct aw_ftl_logic_blk_info {

    /** \brief �������Ϣ */
    struct aw_ftl_blk_info *p_pyh_blk;

    /** \brief ������ʹ�ñ�־ */
    uint8_t *sector_be_used;

    /** \brief ���������־ */
    uint8_t *sector_is_dirty;

    /** \brief ��ʹ�õ��������� */
    uint8_t sector_use_cnt;

    /** \brief ������������ */
    uint8_t sector_dirty_cnt;
};

struct aw_ftl_log_map_info {
    uint16_t lbn;
    uint16_t sn;
};

/**
 * \brief FTL��־����Ϣ
 */
struct aw_ftl_log_blk_info {

    /** \brief ��־���ӳ����Ϣ */
    struct aw_ftl_log_map_info *map;

    /** \brief �������Ϣ */
    struct aw_ftl_blk_info *p_pyh_blk;

    /** \brief ����־��������ʹ�ñ�־ */
    uint8_t *sector_be_used;

    /** \brief ����־�����������־ */
    uint8_t *sector_is_dirty;

    /** \brief ����־�鱻ʹ�õ��������� */
    uint8_t sector_use_cnt;

    /** \brief ����־�������������� */
    uint8_t sector_dirty_cnt;
};

struct aw_ftl_dev;
struct aw_ftl_sector_tag;

aw_err_t aw_ftl_map_table_init (struct aw_ftl_dev *p_ftl);
void aw_ftl_map_table_deinit (struct aw_ftl_dev *p_ftl);

aw_bool_t aw_ftl_logic_blk_is_empty (struct aw_ftl_dev *p_ftl, uint16_t lbn);

aw_bool_t aw_ftl_logic_sector_be_used (struct aw_ftl_dev *p_ftl,
                                       uint16_t           lbn,
                                       uint8_t            sector);

aw_bool_t aw_ftl_logic_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                        uint16_t           lbn,
                                        uint8_t            sector);

aw_bool_t aw_ftl_log_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                      uint16_t           log,
                                      uint8_t            sector);


/**
 * \brief ��ȡһ���µ��߼���
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] lbn   : �߼�����
 *
 * \retval AW_OK    : �����ɹ�
 * \retval -ENODEV  : û�п��е������
 * \retval ELSE     : MTD����ʧ��
 */
int aw_ftl_new_logic_blk_get (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief ���߼���д��һ���������ݺͱ�ǩ
 *
 * \param[in] p_ftl  : FTL�豸�ṹ��
 * \param[in] lbn    : �߼����
 * \param[in] idx    : ��������λ������
 * \param[in] data   : Ҫд�������
 * \param[in] tag    : Ҫд��ı�ǩ��Ϊ����д��ǩ
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_logic_sector_write_with_tag (struct aw_ftl_dev         *p_ftl,
                                        uint16_t                   lbn,
                                        uint16_t                   idx,
                                        uint8_t                   *data,
                                        struct aw_ftl_sector_tag  *tag);

/**
 * \brief �����ǩд������
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] lbn   : �߼�����
 * \param[in] idx   : ������(����ҳ���� - sectors_per_blk_hdr)
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_logic_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                         uint16_t           lbn,
                                         uint16_t           idx);

aw_err_t aw_ftl_logic_sector_read (struct aw_ftl_dev *p_ftl,
                                   uint16_t           lbn,
                                   uint16_t           idx,
                                   uint8_t           *data);

/**
 * \brief ���߼�������д����־����,ͬʱҲ��дtag
 *
 * \param[in] p_ftl: FTL�豸�ṹ��
 * \param[in] lbn  : �߼�����
 * \param[in] idx  : ��������λ������
 * \param[in] data : ����
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_sector_write (struct aw_ftl_dev *p_ftl,
                             uint16_t           lbn,
                             uint16_t           idx,
                             uint8_t           *data);

/**
 * \brief ����־����дָ���߼�����������,ͬʱҲ��дtag���Ὣ�����ݱ��Ϊdirty
 *
 * \param[in] p_ftl  : FTL�豸�ṹ��
 * \param[in] lbn    : �߼�����
 * \param[in] idx    : ��������λ������
 * \param[in] data   : ����
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_sector_rewrite (struct aw_ftl_dev   *p_ftl,
                               uint16_t             lbn,
                               uint16_t             idx,
                               uint8_t             *data);

/**
 * \brief ��־������ǩ��Ϣд����Ϣ
 *
 * \param[in] p_ftl   : FTL�豸�ṹ��
 * \param[in] log_blk : ��־���
 * \param[in] sector  : ������(����ҳ���� - sectors_per_blk_hdr)
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                       uint16_t           log_blk,
                                       uint16_t           sector);

/**
 * \brief ����־���ȡ�ƶ��߼�����������
 *
 * \param[in] p_ftl  : FTL�豸�ṹ��
 * \param[in] lbn    : �߼�����
 * \param[in] idx    : ��������λ������
 * \param[in] data   : ��ȡ����������
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_sector_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           lbn,
                            uint16_t           idx,
                            uint8_t           *data);

/**
 * \brief �ϲ�ָ�����߼�����־������
 *
 * \param[in]  p_ftl  : FTL�豸�ṹ��
 * \param[in]  lbn    : �߼���
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_blk_merge (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief �ϲ�������־������
 *
 * \param[in]  p_ftl  : FTL�豸�ṹ��
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_blk_merge_all (struct aw_ftl_dev *p_ftl);

/**
 * \brief ��ȡ�߼���ľ�����Ϣ 
 *
 * \param[in]   p_ftl  : FTL�豸�ṹ��
 * \param[out]  lbn    : �߼���
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_logic_info_get (struct aw_ftl_dev *p_ftl, uint16_t lbn);

/**
 * \brief ��ȡ��־��ľ�����Ϣ 
 *
 * \param[in]  p_ftl  : FTL�豸�ṹ��
 * \param[in]  log    : ��־��
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE  : MTD����ʧ��
 */
int aw_ftl_log_info_get (struct aw_ftl_dev *p_ftl, uint16_t log);


/** @}  grp_aw_if_ftl_map */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_MAP_H */



/* end of file */
