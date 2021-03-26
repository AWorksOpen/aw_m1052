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
 * - 1.00 17-04-15  vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "string.h"
#include "ftl/aw_ftl_core.h"
#include "ftl/aw_ftl_utils.h"
#include "ftl/aw_ftl_map.h"
#include "ftl/aw_ftl_flash.h"

#define PFX "FTL-map"
#define __MSG_MASK          AW_FTL_MSG_MASK_MAP
#define __ERR_MSG_MASK      AW_FTL_MSG_MASK_ERR
/******************************************************************************/
/** \brief ��־��ʹ�ñ�־��� */
aw_local void __ftl_log_use_flag_clear (struct aw_ftl_dev *p_ftl,
                                        uint16_t           log)
{
    uint8_t size = (p_ftl->sectors_per_blk + 7) >> 3;

    p_ftl->log_tbl[log].sector_use_cnt = 0;

    /* ���ʹ�ñ�־ */
    memset(p_ftl->log_tbl[log].sector_be_used, 0, size);
}

/** \brief ��־�����־��� */
aw_local void __ftl_log_dirty_flag_clear (struct aw_ftl_dev *p_ftl,
                                          uint16_t           log)
{
    uint8_t size = (p_ftl->sectors_per_blk + 7) >> 3;

    p_ftl->log_tbl[log].sector_dirty_cnt = 0;

    memset(p_ftl->log_tbl[log].sector_is_dirty, 0, size);
}

/** \brief ��־�����־��� */
aw_local void __ftl_log_map_info_clear (struct aw_ftl_dev *p_ftl,
                                        uint16_t           log)
{
    memset(p_ftl->log_tbl[log].map,
           0,
           sizeof(struct aw_ftl_log_map_info) * p_ftl->sectors_per_blk);
}
/******************************************************************************/

/** \brief ��־������ʹ�ñ�־���� */
aw_local void __ftl_log_sector_used_set (struct aw_ftl_dev *p_ftl,
                                       uint16_t           log,
                                       uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->log_tbl[log].sector_be_used[offs] |= (1 << (sector % 8));
}

/** \brief ��־������ʹ�ñ�־��� */
aw_local void __ftl_log_sector_used_clear (struct aw_ftl_dev *p_ftl,
                                        uint16_t           log,
                                        uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->log_tbl[log].sector_be_used[offs] &= ~(1 << (sector % 8));
}

/** \brief ��־������ʹ�ñ�־�ж� */
aw_local aw_bool_t __ftl_log_sector_be_used (struct aw_ftl_dev *p_ftl,
                                          uint16_t           log,
                                          uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    return (p_ftl->log_tbl[log].sector_be_used[offs] & 
            (1 << (sector % 8)) ? AW_TRUE : AW_FALSE);
}

/** \brief ��־���������ж� */
aw_local aw_bool_t __ftl_log_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                           uint16_t           log,
                                           uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    return (p_ftl->log_tbl[log].sector_is_dirty[offs] & 
            (1 << (sector % 8)) ? AW_TRUE : AW_FALSE);
}

/** \brief ��־������������ */
aw_local void __ftl_log_sector_dirty_set (struct aw_ftl_dev *p_ftl,
                                          uint16_t           log,
                                          uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->log_tbl[log].sector_is_dirty[offs] |= (1 << (sector % 8));
}

/** \brief ��־����������� */
aw_local void __ftl_log_sector_dirty_clear (struct aw_ftl_dev *p_ftl,
                                            uint16_t           log,
                                            uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->log_tbl[log].sector_is_dirty[offs] &= ~(1 << (sector % 8));
}
/******************************************************************************/
/** \brief �߼�������ʹ�ñ�־���� */
aw_local void __ftl_logic_sector_used_set (struct aw_ftl_dev *p_ftl,
                                           uint16_t           lbn,
                                           uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->logic_tbl[lbn].sector_be_used[offs] |= (1 << (sector % 8));
}

/** \brief �߼�������ʹ�ñ�־��� */
aw_local void __ftl_logic_sector_used_clear (struct aw_ftl_dev *p_ftl,
                                             uint16_t           lbn,
                                             uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->logic_tbl[lbn].sector_be_used[offs] &= ~(1 << (sector % 8));
}

/** \brief �߼�������ʹ�ñ�־�ж� */
aw_local aw_bool_t __ftl_logic_sector_be_used (struct aw_ftl_dev *p_ftl,
                                            uint16_t           lbn,
                                            uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    return (p_ftl->logic_tbl[lbn].sector_be_used[offs] & 
            (1 << (sector % 8)) ? AW_TRUE : AW_FALSE);
}

/** \brief �߼����������ж� */
aw_local aw_bool_t __ftl_logic_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                             uint16_t           lbn,
                                             uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    return (p_ftl->logic_tbl[lbn].sector_is_dirty[offs] & 
            (1 << (sector % 8)) ? AW_TRUE : AW_FALSE);
}

/** \brief �߼������������� */
aw_local void __ftl_logic_sector_dirty_set (struct aw_ftl_dev *p_ftl,
                                            uint16_t           lbn,
                                            uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->logic_tbl[lbn].sector_is_dirty[offs] |= (1 << (sector % 8));
}

/** \brief �߼������������ */
aw_local void __ftl_logic_sector_dirty_clear (struct aw_ftl_dev *p_ftl,
                                              uint16_t           lbn,
                                              uint8_t            sector)
{
    uint8_t offs = sector >> 3;

    p_ftl->logic_tbl[lbn].sector_is_dirty[offs] &= ~(1 << (sector % 8));
}

/******************************************************************************/

/**
 * \brief ����һ�������������Ϊ��־��
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] lbn   : �߼�����
 *
 * \retval AW_OK              : �����ɹ�
 * \retval AW_FTL_NO_FREE_BLK : û�п��е������
 * \retval ELSE               : MTD����ʧ��
 */
aw_local int __ftl_get_new_log_blk (struct aw_ftl_dev *p_ftl,
                                    uint8_t            log_blk)
{
    uint16_t pbn;
    struct aw_ftl_blk_tag tag;
    int                  ret;

    p_ftl->log_tbl[log_blk].p_pyh_blk = aw_ftl_free_node_get(p_ftl);

    if (NULL == p_ftl->log_tbl[log_blk].p_pyh_blk) {
        return -AW_EINVAL;
    }

    p_ftl->log_tbl[log_blk].p_pyh_blk->lbn = log_blk;

    p_ftl->log_tbl[log_blk].sector_dirty_cnt = 0;
    p_ftl->log_tbl[log_blk].sector_use_cnt   = 0;

    __ftl_log_use_flag_clear(p_ftl, log_blk);
    __ftl_log_dirty_flag_clear(p_ftl, log_blk);
    __ftl_log_map_info_clear(p_ftl, log_blk);

    pbn = p_ftl->log_tbl[log_blk].p_pyh_blk->pbn;

    tag.flag = AW_FTL_BLOCK_TAG_LOG;
    tag.log  = log_blk;
    tag.lbn  = 0xffff;

    /* д����Ϣ��ǩ */
    ret = aw_ftl_blk_hdr_write(p_ftl, pbn, NULL, &tag);
    if (ret == AW_OK) {
        AW_FTL_MSG(__MSG_MASK, "write FTL format information to block(pbn:%d)", pbn);
    } else {
        return ret;
    }

    return AW_OK;
}


/**
 * \brief �߼������ݵ�ˢ�£����ɵ��߼������Ч���ݰ��Ƶ��¿���
 *
 * \param[in]  p_ftl  : FTL�豸�ṹ��
 * \param[in]  lbn  : �߼���
 *
 * \retval AW_OK : �����ɹ�
 * \retval ELSE      : MTD����ʧ��
 */
aw_local int __ftl_logic_blk_data_update (struct aw_ftl_dev *p_ftl, uint16_t lbn)
{
    struct aw_ftl_blk_info *new_phy_blk;
    uint16_t pbn, i;
    int      ret = AW_OK;
    struct aw_ftl_sector_tag sec_tag;
    struct aw_ftl_blk_tag    blk_tag;

    new_phy_blk      = aw_ftl_free_node_get(p_ftl);

    if (new_phy_blk == NULL || !p_ftl->logic_tbl[lbn].p_pyh_blk) {
        return -AW_EINVAL;
    }

    /* ��ȡ�¿�󣬽��Ӿɿ�д�����ݵı�ǩд�뵽��������� */
    pbn = new_phy_blk->pbn;

    blk_tag.flag = AW_FTL_BLOCK_TAG_DATA_IN_BLK;
    blk_tag.lbn  = lbn;
    blk_tag.log  = 0xff;

    ret = aw_ftl_blk_hdr_write(p_ftl, pbn, NULL, &blk_tag);
    if (ret == AW_OK) {
        AW_FTL_MSG(__MSG_MASK, "get a new block(%d) for logic update "
                  "and write blk tag parse 1", pbn);
    } else {
        return ret;
    }

    new_phy_blk->lbn = lbn;

    for (i = 0; i < p_ftl->sectors_per_blk; i++) {

        /* ����������в����������ת������ */
        if (__ftl_logic_sector_be_used(p_ftl, lbn, i) &&
            !__ftl_logic_sector_is_dirty(p_ftl, lbn, i)) {

            /* �����߼���������е����ݶ����¿��� */
            pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;
            ret = aw_ftl_flash_page_read(p_ftl, pbn, i, p_ftl->buf);

            pbn = new_phy_blk->pbn;

            sec_tag.flag = AW_FTL_SECTOR_TAG_DATA;
            sec_tag.lbn  = 0xffff;
            sec_tag.sn   = 0xff;
            ret = aw_ftl_flash_page_write_with_tag(p_ftl,
                                                   pbn,
                                                   i,
                                                   p_ftl->buf,
                                                   &sec_tag,
                                                   sizeof(sec_tag));

        } else if (__ftl_logic_sector_be_used(p_ftl, lbn, i) &&
                   __ftl_logic_sector_is_dirty(p_ftl, lbn, i)) {

            /* �������������������������� */
            __ftl_logic_sector_used_clear(p_ftl, lbn, i);
            __ftl_logic_sector_dirty_clear(p_ftl, lbn, i);

            if (p_ftl->logic_tbl[lbn].sector_use_cnt) {
                p_ftl->logic_tbl[lbn].sector_use_cnt--;
            }
        }
    }

    /* ��һ�׶ε��߼�������ת�Ƴɹ���д��ڶ��׶εı�ǩ */
    blk_tag.flag = AW_FTL_BLOCK_TAG_DATA_IN_LOG;
    if (lbn != blk_tag.lbn) {
        blk_tag.lbn  = 0xffff;
    }
    blk_tag.lbn  = 0xffff;
    blk_tag.log  = 0xff;
    pbn = new_phy_blk->pbn;

    ret = aw_ftl_blk_hdr_write(p_ftl, pbn, NULL, &blk_tag);
    if (ret == AW_OK) {
        AW_FTL_MSG(__MSG_MASK, "write block tag parse 2 for logic block update", pbn);
    } else {
        return ret;
    }

    aw_ftl_dirty_node_add(p_ftl, p_ftl->logic_tbl[lbn].p_pyh_blk);

    p_ftl->logic_tbl[lbn].p_pyh_blk      = new_phy_blk;
    p_ftl->logic_tbl[lbn].p_pyh_blk->lbn = lbn;
    p_ftl->logic_tbl[lbn].sector_dirty_cnt = 0;

    return ret;
}

/******************************************************************************/
aw_bool_t aw_ftl_logic_blk_is_empty (struct aw_ftl_dev *p_ftl, uint16_t lbn)
{
    return p_ftl->logic_tbl[lbn].p_pyh_blk == NULL ? AW_TRUE : AW_FALSE;
}

aw_bool_t aw_ftl_logic_sector_be_used (struct aw_ftl_dev *p_ftl,
                                    uint16_t           lbn,
                                    uint8_t            sector)
{
    return __ftl_logic_sector_be_used(p_ftl, lbn, sector);
}

aw_bool_t aw_ftl_logic_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                     uint16_t           lbn,
                                     uint8_t            sector)
{
    return __ftl_logic_sector_is_dirty(p_ftl, lbn, sector);
}

aw_bool_t aw_ftl_log_sector_is_dirty (struct aw_ftl_dev *p_ftl,
                                   uint16_t           log,
                                   uint8_t            sector)
{
    return __ftl_log_sector_is_dirty(p_ftl, log, sector);
}

int aw_ftl_new_logic_blk_get (struct aw_ftl_dev *p_ftl,
                              uint16_t           lbn)
{
    uint16_t              pbn;
    struct aw_ftl_blk_tag tag;
    uint8_t               size;
    int                   ret;

    p_ftl->logic_tbl[lbn].p_pyh_blk = aw_ftl_free_node_get(p_ftl);
    if (NULL == p_ftl->logic_tbl[lbn].p_pyh_blk) {
        return -AW_ENODEV;
    }

    p_ftl->logic_tbl[lbn].p_pyh_blk->lbn = lbn;

    p_ftl->logic_tbl[lbn].sector_dirty_cnt = 0;
    p_ftl->logic_tbl[lbn].sector_use_cnt   = 0;

    size = (p_ftl->sectors_per_blk + 7) >> 3;
    memset(p_ftl->logic_tbl[lbn].sector_be_used,  0, size);
    memset(p_ftl->logic_tbl[lbn].sector_is_dirty, 0, size);

    pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

    tag.flag = AW_FTL_BLOCK_TAG_DATA;
    tag.lbn  = lbn;
    tag.log  = 0xff;

    /* д����Ϣ��ǩ */
    ret = aw_ftl_blk_hdr_write(p_ftl, pbn, NULL, &tag);
    if (ret != AW_OK) {
        return ret;
    }

    AW_FTL_MSG(__MSG_MASK, 
               "get a new physical block(%d) to logic block(%d) [ OK ]. ", 
               pbn, lbn);
    return AW_OK;
}

int aw_ftl_logic_sector_write_with_tag (  struct aw_ftl_dev        *p_ftl,
                                          uint16_t                  lbn,
                                          uint16_t                  idx,
                                          uint8_t                  *data,
                                          struct aw_ftl_sector_tag  *tag)

{
    uint16_t pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;
    int      ret = -1;
    uint32_t retlen, addr;
    struct aw_mtd_oob_ops ops;

    /* ����ǰд���ݵĿ��������Ϊ�Ѿ���ʹ�� */

    if (data && !__ftl_logic_sector_be_used(p_ftl, lbn, idx)) {

        p_ftl->logic_tbl[lbn].sector_use_cnt++;

        __ftl_logic_sector_used_set(p_ftl, lbn, idx);

        p_ftl->free_sector_total--;
    }

    ret = aw_ftl_flash_page_write_with_tag(p_ftl, pbn, idx, data, tag, sizeof(*tag));
    return ret;
}

int aw_ftl_logic_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                         uint16_t           lbn,
                                         uint16_t           idx)
{
    struct aw_ftl_sector_tag  tag;
    uint16_t                 pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

    if (!__ftl_logic_sector_is_dirty(p_ftl, lbn, idx)) {

        p_ftl->logic_tbl[lbn].sector_dirty_cnt++;
        __ftl_logic_sector_dirty_set(p_ftl, lbn, idx);
    }

    tag.flag = AW_FTL_SECTOR_TAG_GARBAGE;
    tag.lbn  = 0xffff;
    tag.sn   = 0xff;

    return aw_ftl_flash_page_tag_write(p_ftl, pbn, idx, &tag);
}

aw_err_t aw_ftl_logic_sector_read (struct aw_ftl_dev *p_ftl,
                                   uint16_t           lbn,
                                   uint16_t           idx,
                                   uint8_t           *data)
{
    uint16_t pbn;

    pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;
    return aw_ftl_flash_page_read(p_ftl, pbn, idx, data);
}

/** \brief ��ȡ�߼���ľ�����Ϣ */
int aw_ftl_logic_info_get (struct aw_ftl_dev *p_ftl, uint16_t lbn)
{
    int      ret;
    int      i = 0;
    uint16_t pbn;
    struct aw_ftl_sector_tag tag;

    if (p_ftl->logic_tbl[lbn].p_pyh_blk == NULL) {
        return AW_OK;
    }

    pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

    for (i = 0; i < p_ftl->sectors_per_blk; i++) {
        ret = aw_ftl_flash_page_tag_read(p_ftl, pbn, i, &tag);
        if (ret != AW_OK) {
            return ret;
        }

        if (tag.flag == AW_FTL_SECTOR_TAG_GARBAGE) {

            p_ftl->logic_tbl[lbn].sector_use_cnt++;
            __ftl_logic_sector_used_set(p_ftl, lbn, i);

            p_ftl->logic_tbl[lbn].sector_dirty_cnt++;
            __ftl_logic_sector_dirty_set(p_ftl, lbn, i);

        } else if (tag.flag == AW_FTL_SECTOR_TAG_DATA) {

            p_ftl->logic_tbl[lbn].sector_use_cnt++;

            __ftl_logic_sector_used_set(p_ftl, lbn, i);
            __ftl_logic_sector_dirty_clear(p_ftl, lbn, i);

        } else {
            __ftl_logic_sector_used_clear(p_ftl, lbn, i);
            __ftl_logic_sector_dirty_clear(p_ftl, lbn, i);
        }
    }

    return AW_OK;
}


int aw_ftl_log_sector_write (struct aw_ftl_dev *p_ftl,
                             uint16_t           lbn,
                             uint16_t           idx,
                             uint8_t           *data)
{
    int                     log_blk, sector;
    int                     ret;
    struct aw_ftl_sector_tag tag;
    uint32_t                dirty_sec_cnt = 0;

    for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {

        /* �����־�黹δ��������п飬����� */
        if (!p_ftl->log_tbl[log_blk].p_pyh_blk) {
            ret = __ftl_get_new_log_blk(p_ftl, log_blk);
            if (ret == AW_OK) {
                AW_FTL_MSG(__MSG_MASK, "allocate a new log(%d) blk(pbn:%d) ok.",
                            log_blk, p_ftl->log_tbl[log_blk].p_pyh_blk->pbn);
            } else {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "allocate a new log block failed");
                return ret;
            }
        }

        if (p_ftl->log_tbl[log_blk].sector_use_cnt < p_ftl->sectors_per_blk) {

            for (sector = p_ftl->log_tbl[log_blk].sector_use_cnt;
                 sector < p_ftl->sectors_per_blk;
                 sector++) {

                if (__ftl_log_sector_be_used(p_ftl, log_blk, sector)) {
                    continue;
                }

                tag.flag = AW_FTL_SECTOR_TAG_DATA;
                tag.lbn  = lbn;
                tag.sn   = idx;

                ret = aw_ftl_flash_page_write_with_tag(p_ftl,
                                         p_ftl->log_tbl[log_blk].p_pyh_blk->pbn,
                                         sector,
                                         data,
                                         &tag,
                                         sizeof(tag));

                AW_FTL_MSG(AW_FTL_MSG_MASK_WD,
                        "write log sector(log:%d-%d, logic:%d-%d(%d), pbn:%d).",
                         log_blk,
                         sector,
                         lbn,
                         idx,
                         lbn * p_ftl->sectors_per_blk + idx,
                         p_ftl->log_tbl[log_blk].p_pyh_blk->pbn);

                /* д��ɹ� */
                if (ret == AW_OK) {

                    /* ������д������־�飬�򽫾��߼������ݱ��Ϊ��  */
                    if ( !aw_ftl_logic_sector_is_dirty(p_ftl, lbn, idx) ) {
                        aw_ftl_logic_sector_dirty_tag_write(p_ftl, lbn, idx);
                    }

                    p_ftl->log_tbl[log_blk].map[sector].lbn = lbn;
                    p_ftl->log_tbl[log_blk].map[sector].sn  = idx;

                    p_ftl->log_tbl[log_blk].sector_use_cnt++;
                    __ftl_log_sector_used_set(p_ftl, log_blk, sector);

                    /* �жϿ���ʹ����־�������Ѿ�û���е��ˣ���ǿ�������ϲ����� */
                    if (log_blk == p_ftl->nlog_blocks - 1 &&
                        sector  == p_ftl->sectors_per_blk - 1) {

                        AW_FTL_MSG(__MSG_MASK, "log is full, begin merge.");

                        aw_ftl_log_blk_merge_all(p_ftl);
                    }

                    return ret;

                /* д��ʧ���򽫸�ҳ��Ϊdirty������ҳ��д */
                } else {
                    aw_ftl_log_sector_dirty_tag_write(p_ftl, log_blk, sector);
                }
            }
        }
    }

    return -AW_EIO;
}

int aw_ftl_log_sector_rewrite (struct aw_ftl_dev   *p_ftl,
                               uint16_t             lbn,
                               uint16_t             idx,
                               uint8_t             *data)
{
    int log_blk, sector, ret = -1;
    int log_blk_bc = 0, sector_bc = 0;
    aw_bool_t find_old = AW_FALSE;

    /* �������е�log, �ҳ������ݵ�����ҳ  */
    for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {

        /* �����־�黹δ��������п飬�˳�  */
        if (!p_ftl->log_tbl[log_blk].p_pyh_blk) {
            break;
        }

        for (sector = 0; sector < p_ftl->log_tbl[log_blk].sector_use_cnt; sector++) {

            /* �ҳ�һ����ͬlbn��sn��logҳ */
            if (!__ftl_log_sector_is_dirty(p_ftl, log_blk, sector) &&
                p_ftl->log_tbl[log_blk].map[sector].lbn == lbn &&
                p_ftl->log_tbl[log_blk].map[sector].sn  == idx) {

                /* ����Ѿ��ҵ���һ���ɵ�logҳ����ɾ����һ���ҳ���logҳ  */
                if (find_old) {
                    ret = aw_ftl_log_sector_dirty_tag_write(p_ftl,
                                                            log_blk_bc,
                                                            sector_bc);
                    if (ret == AW_OK) {
                        AW_FTL_MSG(AW_FTL_MSG_MASK_WD, "delete old log data(%d-%d, pbn:%d)",
                                log_blk_bc, sector_bc, p_ftl->log_tbl[log_blk_bc].p_pyh_blk->pbn);
                    }
                } else {
                    find_old = AW_TRUE;
                }

                log_blk_bc = log_blk;
                sector_bc  = sector;
            }
        }
    }


    /* ��д�������� */
    ret = aw_ftl_log_sector_write(p_ftl, lbn, idx, data);

    /* ��������һ�������� */
    if (find_old) {
        if (p_ftl->log_tbl[log_blk_bc].p_pyh_blk) {
            ret = aw_ftl_log_sector_dirty_tag_write(p_ftl, 
                                                    log_blk_bc, 
                                                    sector_bc);
            if (ret == AW_OK) {
                AW_FTL_MSG(AW_FTL_MSG_MASK_WD, 
                           "delete old log data(%d-%d, pbn:%d)",
                           log_blk_bc, sector_bc, 
                           p_ftl->log_tbl[log_blk_bc].p_pyh_blk->pbn);
            }
        }
    }
    return ret;
}

int aw_ftl_log_sector_dirty_tag_write (struct aw_ftl_dev *p_ftl,
                                       uint16_t           log_blk,
                                       uint16_t           sector)
{
    struct aw_ftl_sector_tag sec_tag;

    if (log_blk >= p_ftl->nlog_blocks ||
        sector >= p_ftl->sectors_per_blk ||
        !p_ftl) {

        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "parameter invalid");
        return -AW_EINVAL;
    }

    if (!p_ftl->log_tbl[log_blk].p_pyh_blk) {
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "p_ftl->log_tbl[log_blk].p_pyh_blk = NULL");
        return -AW_EINVAL;
    }

    if (!__ftl_log_sector_is_dirty(p_ftl, log_blk, sector)) {

        p_ftl->log_tbl[log_blk].sector_dirty_cnt++;
        __ftl_log_sector_dirty_set(p_ftl, log_blk, sector);

        sec_tag.flag = AW_FTL_SECTOR_TAG_GARBAGE;
        sec_tag.lbn  = 0xffff;
        sec_tag.sn   = 0xff;

        return aw_ftl_flash_page_tag_write(p_ftl,
                                           p_ftl->log_tbl[log_blk].p_pyh_blk->pbn,
                                           sector,
                                           &sec_tag);
    }

    return AW_OK;
}

int aw_ftl_log_sector_read (struct aw_ftl_dev *p_ftl,
                            uint16_t           lbn,
                            uint16_t           idx,
                            uint8_t           *data)
{
    int log_blk, sector;
    int ret;

    /* ������� */
    for (log_blk = p_ftl->nlog_blocks - 1; log_blk >= 0; log_blk--) {

        for (sector = p_ftl->log_tbl[log_blk].sector_use_cnt - 1;
             sector >= 0; sector--) {

            if (p_ftl->log_tbl[log_blk].p_pyh_blk != NULL &&
                !__ftl_log_sector_is_dirty(p_ftl, log_blk, sector) &&
                p_ftl->log_tbl[log_blk].map[sector].lbn == lbn &&
                p_ftl->log_tbl[log_blk].map[sector].sn  == idx) {

                AW_FTL_MSG(AW_FTL_MSG_MASK_RD, "read log(log:%d-%d)( lbn:%d-%d(%d) ). ",
                         log_blk, sector, lbn, idx, lbn * p_ftl->sectors_per_blk + idx);

                ret = aw_ftl_flash_page_read(p_ftl,
                                         p_ftl->log_tbl[log_blk].p_pyh_blk->pbn,
                                         sector,
                                         data);
                if (ret != AW_OK) {
                    /* ��ҳ��ʧ�ܣ��������д������ʱδд��Ͼ͵��磬��ʱ������������ */

                    return aw_ftl_log_sector_dirty_tag_write(p_ftl,
                                                      log_blk,
                                                      sector);
                } else {
                    return ret;
                }
            }
        }
    }

    return -AW_ENODEV;
}


int aw_ftl_log_blk_merge (struct aw_ftl_dev *p_ftl, uint16_t lbn)
{
    int                            ret = AW_OK;
    struct aw_ftl_sector_tag       sec_tag;
    struct aw_ftl_blk_tag          blk_tag;
    uint_t                         i;
    uint16_t                       pbn;
    uint16_t                       idx;
    int                            log_blk, sector;

    aw_ftl_sys_info_clear(p_ftl);

    AW_FTL_MSG(__MSG_MASK, "begin update lbn from old(lbn:%d, pbn:%d).",
             lbn,
             p_ftl->logic_tbl[lbn].p_pyh_blk->pbn);

    /*
     * ����û�������ݣ�Ҳ��Ҫ������־�����ݽ��кϲ���
     * ��Ϊ������ڵڶ��׶ζϵ磬����־�黹�иÿ�����
     */
    if (p_ftl->logic_tbl[lbn].sector_dirty_cnt) {

        /* �����߼������ݸ��µ����߼����� */
        ret = __ftl_logic_blk_data_update(p_ftl, lbn);
        if (ret == AW_OK) {
            AW_FTL_MSG(__MSG_MASK, "logic(%d) block update [ OK ]", lbn);
        } else {
            AW_FTL_ERR_MSG(__ERR_MSG_MASK, "logic(%d) block update ERROR", lbn);
            return ret;
        }
    }

    /* ����־���е�ָ�����߼������ݸ��µ����߼����У��ٽ���־����Ӧ���������Ϊdirty */
    for (log_blk = p_ftl->nlog_blocks - 1; log_blk >= 0; log_blk--) {

        /* ������������dirty�������ô��� */
        if (!p_ftl->log_tbl[log_blk].p_pyh_blk ||
            (p_ftl->log_tbl[log_blk].sector_use_cnt ==
             p_ftl->log_tbl[log_blk].sector_dirty_cnt)) {
            continue;
        }

        for (sector = p_ftl->log_tbl[log_blk].sector_use_cnt - 1;
             sector >= 0 ;
             sector--) {

            if (!__ftl_log_sector_is_dirty(p_ftl, log_blk, sector) &&
                 __ftl_log_sector_be_used(p_ftl, log_blk, sector)) {

                /* �������־������ָ����߼����ָ���߼��������������һ����־���� */
                if (lbn != p_ftl->log_tbl[log_blk].map[sector].lbn) {
                    continue;
                }

                idx = p_ftl->log_tbl[log_blk].map[sector].sn;

                /*
                 * ��������߼����и��������Ѿ���д�������ݣ�
                 * ����־���и�����������ֱ����Ϊdirty
                 */
                if (__ftl_logic_sector_be_used(p_ftl, lbn, idx)) {
                    ret = aw_ftl_log_sector_dirty_tag_write(p_ftl,
                                                     log_blk,
                                                     sector);
                    continue;
                }

                /* ����־����������д���µ��߼����� */
                ret = aw_ftl_flash_page_read(p_ftl,
                                        p_ftl->log_tbl[log_blk].p_pyh_blk->pbn,
                                        sector,
                                        p_ftl->buf);

                sec_tag.flag = AW_FTL_SECTOR_TAG_DATA;
                sec_tag.sn   = 0xff;
                sec_tag.lbn  = 0xffff;

                ret = aw_ftl_logic_sector_write_with_tag(p_ftl, lbn, idx, p_ftl->buf, &sec_tag);
                if (ret == AW_OK) {
                    ret = aw_ftl_log_sector_dirty_tag_write(p_ftl, log_blk, sector);
                }
            }
        }
    }

    if (ret == AW_OK) {

        /* �ڶ��׶ε���־������ת�Ƴɹ���д��ɹ���ǩ */
        blk_tag.flag = AW_FTL_BLOCK_TAG_DATA;
        blk_tag.lbn  = 0xffff;
        blk_tag.log  = 0xff;
        pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

        ret = aw_ftl_blk_hdr_write(p_ftl, pbn, NULL, &blk_tag);
        if (ret == AW_OK) {
            AW_FTL_MSG(__MSG_MASK, "write block tag parse 3", lbn);
        } else {
            return ret;
        }

        AW_FTL_MSG(__MSG_MASK, "update to new lbn finish(lbn:%d, pbn:%d).",
                 lbn,
                 p_ftl->logic_tbl[lbn].p_pyh_blk->pbn);
    }

    return ret;
}

int aw_ftl_log_blk_merge_all (struct aw_ftl_dev *p_ftl)
{
    uint_t    i;
    int       ret = AW_OK;
    uint16_t  log_blk;

    for (i = 0; i < p_ftl->ndata_blocks; i++) {

        /* ��������������ݣ���˵����������д������־�� */
        if (p_ftl->logic_tbl[i].sector_dirty_cnt) {
            ret = aw_ftl_log_blk_merge(p_ftl, i);
        }
    }

    /* ��־��ı�Ǻ�ӳ�������� */
    for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {

        if (NULL != p_ftl->log_tbl[log_blk].p_pyh_blk) {
            aw_ftl_dirty_node_add(p_ftl, p_ftl->log_tbl[log_blk].p_pyh_blk);

            p_ftl->log_tbl[log_blk].p_pyh_blk = NULL;

            __ftl_log_use_flag_clear(p_ftl, log_blk);
            __ftl_log_dirty_flag_clear(p_ftl, log_blk);
            __ftl_log_map_info_clear(p_ftl, log_blk);
        }
    }

    return ret;
}


/* ��ȡ��־�������ľ�����Ϣ */
int aw_ftl_log_info_get (struct aw_ftl_dev *p_ftl, uint16_t log)
{
    int                     ret;
    int                     i = 0;
    struct aw_ftl_sector_tag tag;

    if (p_ftl->log_tbl[log].p_pyh_blk == NULL) {
        return AW_OK;
    }

    for (i = 0; i < p_ftl->sectors_per_blk; i++) {

        ret = aw_ftl_flash_page_tag_read(p_ftl,
                                         p_ftl->log_tbl[log].p_pyh_blk->pbn,
                                         i,
                                         &tag);

        if (tag.flag == AW_FTL_SECTOR_TAG_GARBAGE) {

            p_ftl->log_tbl[log].sector_use_cnt++;
            __ftl_log_sector_used_set(p_ftl, log, i);

            p_ftl->log_tbl[log].sector_dirty_cnt++;
            __ftl_log_sector_dirty_set(p_ftl, log, i);

        } else if (tag.flag == AW_FTL_SECTOR_TAG_DATA) {

            p_ftl->log_tbl[log].map[i].lbn = tag.lbn;
            p_ftl->log_tbl[log].map[i].sn  = tag.sn;

            p_ftl->log_tbl[log].sector_use_cnt++;

            __ftl_log_sector_used_set(p_ftl, log, i);
            __ftl_log_sector_dirty_clear(p_ftl, log, i);

        } else {
            __ftl_log_sector_used_clear(p_ftl, log, i);
            __ftl_log_sector_dirty_clear(p_ftl, log, i);
        }
    }

    return AW_OK;
}


aw_err_t aw_ftl_map_table_init (struct aw_ftl_dev *p_ftl)
{
    uint16_t             size  = 0;
    uint16_t             size2 = 0;
    uint16_t             mem_offs = 0;
    uint16_t             mem_size = 0;
    uint8_t             *p_mem;
    int                  ret;
    int                  i;

    if (NULL == p_ftl->logic_tbl) {
        size = sizeof(struct aw_ftl_logic_blk_info) * p_ftl->ndata_blocks;

        p_ftl->logic_tbl = (struct aw_ftl_logic_blk_info *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_ftl->logic_tbl) {
            goto _failed;
        }

        size     = (p_ftl->sectors_per_blk + 7) >> 3;
        mem_size = size * 2 * p_ftl->ndata_blocks;
        p_mem = (uint8_t *)aw_ftl_mem_alloc(p_ftl, mem_size);
        if (NULL == p_mem) {
            goto _failed;
        }
        mem_offs = 0;
        for (i = 0; i < p_ftl->ndata_blocks; i++) {
            p_ftl->logic_tbl[i].sector_use_cnt   = 0;
            p_ftl->logic_tbl[i].sector_be_used   = p_mem + mem_offs;
            mem_offs += size;

            p_ftl->logic_tbl[i].sector_dirty_cnt = 0;
            p_ftl->logic_tbl[i].sector_is_dirty  = p_mem + mem_offs;
            mem_offs += size;
        }
    }

    if (NULL == p_ftl->log_tbl) {
        size = sizeof(struct aw_ftl_log_blk_info) * p_ftl->nlog_blocks;

        p_ftl->log_tbl = (struct aw_ftl_log_blk_info *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_ftl->log_tbl) {
            goto _failed;
        }

        /* 1�� sector_be_used �Ĵ�С   */
        size     = (p_ftl->sectors_per_blk + 7) >> 3;
        /* 1��log��� map �Ĵ�С  */
        size2    = sizeof(struct aw_ftl_log_map_info) * p_ftl->sectors_per_blk;

        mem_size = size * 2 * p_ftl->nlog_blocks + size2 * p_ftl->nlog_blocks;
        p_mem = (uint8_t *)aw_ftl_mem_alloc(p_ftl, mem_size);
        if (NULL == p_mem) {
            goto _failed;
        }
        mem_offs = 0;
        for (i = 0; i < p_ftl->nlog_blocks; i++) {
            p_ftl->log_tbl[i].sector_use_cnt   = 0;
            p_ftl->log_tbl[i].sector_be_used   = p_mem + mem_offs;
            mem_offs += size;

            p_ftl->log_tbl[i].sector_dirty_cnt = 0;
            p_ftl->log_tbl[i].sector_is_dirty  = p_mem + mem_offs;
            mem_offs += size;

            p_ftl->log_tbl[i].map = (struct aw_ftl_log_map_info *)(p_mem + mem_offs);
            mem_offs += size2;
        }
    }

    return AW_OK;

_failed:
    aw_ftl_map_table_deinit(p_ftl);
    return -AW_ENOMEM;
}

void aw_ftl_map_table_deinit (struct aw_ftl_dev *p_ftl)
{
    if (p_ftl->logic_tbl != NULL) {
        if (p_ftl->logic_tbl[0].sector_be_used != NULL) {
            aw_ftl_mem_free(p_ftl, (void **)&p_ftl->logic_tbl[0].sector_be_used);
        }
        aw_ftl_mem_free(p_ftl, (void **)&p_ftl->logic_tbl);
    }

    if (p_ftl->log_tbl != NULL) {
        if (p_ftl->log_tbl[0].sector_be_used != NULL) {
            aw_ftl_mem_free(p_ftl, (void **)&p_ftl->log_tbl[0].sector_be_used);
        }
        aw_ftl_mem_free(p_ftl, (void **)&p_ftl->log_tbl);
    }
}




