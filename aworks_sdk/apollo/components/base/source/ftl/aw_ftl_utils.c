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
 * \brief utilities
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15  vih, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "string.h"
#include "aw_mem.h"

#include "ftl/aw_ftl_core.h"
#include "ftl/aw_ftl_utils.h"
#include "ftl/aw_ftl_map.h"
#include "ftl/aw_ftl_flash.h"
#include "ftl/aw_ftl_config.h"

/******************************************************************************/
#define PFX "ftl-utils"
#define __MSG_MASK          AW_FTL_MSG_MASK_UTILS
#define __MASK_DBG          AW_FTL_MSG_MASK_UTILS
#define __ERR_MSG_MASK      AW_FTL_MSG_MASK_ERR
/******************************************************************************/
int aw_ftl_message_mask = {

#if 0
    AW_FTL_MSG_MASK_BLK         |
    AW_FTL_MSG_MASK_FLASH       |
    AW_FTL_MSG_MASK_UTILS       |
    AW_FTL_MSG_MASK_SYS         |
    AW_FTL_MSG_MASK_MAP         |
    AW_FTL_MSG_MASK_ITF         |
    AW_FTL_MSG_MASK_WD          |
    AW_FTL_MSG_MASK_RD          |
    AW_FTL_MSG_MASK_FORMAT      |

#endif
    AW_FTL_MSG_MASK_ERR         |
    1
};

/* 被标记为坏块的块都挂到该list上 */
static struct aw_list_head _g_markbad_list = {
    .next = &_g_markbad_list,
    .prev = &_g_markbad_list
};
/******************************************************************************/
/**
 * \brief 检测除页0外其它页是否是干净的
 */
aw_local int __flash_blk_check (struct aw_ftl_dev      *p_ftl, 
                                struct aw_ftl_blk_info *p_pyh_blk)
{
    int ret;
    int i, j;

    for (i = 0; i < p_ftl->sectors_per_blk; i++) {
        ret = aw_ftl_flash_page_read_with_tag(p_ftl, 
                                              p_pyh_blk->pbn, 
                                              i, 
                                              p_ftl->buf, 
                                              NULL,
                                              0);
        if (ret == AW_OK) {
            for (j = 0; j < p_ftl->sector_size; j++) {
                if (p_ftl->buf[j] != 0xFF) {
                    return -AW_EBADMSG;
                }
            }
        } else if (ret != AW_OK) {
            return -AW_EBADMSG;
        }
    }

    return AW_OK;
}


/******************************************************************************/
uint32_t aw_ftl_blk_id_get (struct aw_ftl_dev *p_ftl, uint32_t pbn)
{
    return pbn - (p_ftl->first_pbn);
}

struct aw_ftl_blk_info *aw_ftl_pbn_info_get (struct aw_ftl_dev *p_ftl,
                                             uint16_t           pbn)
{
    p_ftl->pbt[pbn - p_ftl->first_pbn].pbn = pbn;

    return &p_ftl->pbt[pbn - p_ftl->first_pbn];
}

/**
 * \brief FTL BLOCK格式验证
 *
 * \param[in] p_blk_hdr : 物理块头信息
 *
 * \retval AW_TRUE  : 格式正确
 * \retval AW_FALSE : 格式不正确
 */
aw_bool_t aw_ftl_blk_hdr_format_verify (aw_ftl_pb_hdr_t *p_blk_hdr)
{
    aw_local uint8_t __format_pattern[6] = "AWFTL";

    return ((0 == memcmp(__format_pattern,
                         p_blk_hdr->format_pattern,
                         6)) ? AW_TRUE : AW_FALSE);
}

/******************************************************************************/
void * aw_ftl_mem_alloc (struct aw_ftl_dev *p_ftl, int size)
{
    aw_ftl_sys_info_t   *p_sys_info = &p_ftl->sys_info;
    void                *p_mem;

    p_mem = (uint16_t *)aw_mem_alloc(size);
    if (NULL == p_mem) {
        return NULL;
    }
    p_ftl->malloc_cnt += size;
    p_ftl->resource++;
    memset(p_mem, 0, size);

    return p_mem;
}


void aw_ftl_mem_free (struct aw_ftl_dev *p_ftl, void **pp_mem)
{
    if (pp_mem && *pp_mem) {
        aw_mem_free(*pp_mem);
        *pp_mem = NULL;
        p_ftl->resource--;
    }
}

/******************************************************************************/
static void __delayed_word_cb (void *p_arg)
{
    struct aw_defer_djob    *p_defer_job = p_arg;

    if (!aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, (void *)p_defer_job) ) {
        AW_FTL_ERR_MSG(__ERR_MSG_MASK, "defer job is not usable!");
    }

    if (aw_sys_defer_commit(AW_SYS_DEFER_TYPE_NOR, (void *)p_defer_job) != AW_OK) {
        AW_FTL_ERR_MSG(__ERR_MSG_MASK, "defer job commit failed!");
    }
}

int aw_ftl_defer_job_init (struct aw_ftl_dev *p_this,
                       void              (*pfunc) (void *))
{
    int ret = AW_OK;
    struct aw_defer_djob    *p_defer_job  = &p_this->defer_job;
    struct aw_delayed_work  *p_delay_work = &p_this->delay_work;

    aw_defer_job_init(p_defer_job, pfunc, p_this);
    if ( !aw_sys_defer_job_is_usable(AW_SYS_DEFER_TYPE_NOR, p_defer_job) ) {
        ret = -AW_EPERM;
    }

    aw_delayed_work_init(p_delay_work, __delayed_word_cb, (void *)p_defer_job);

    return ret;
}

void aw_ftl_defer_job_start (struct aw_ftl_dev *p_this, uint32_t ms_tim)
{
    if (ms_tim > 0) {
        aw_delayed_work_start(&p_this->delay_work, ms_tim);
    }
}

void aw_ftl_defer_job_stop (struct aw_ftl_dev *p_this)
{
    aw_sys_defer_abort(AW_SYS_DEFER_TYPE_NOR, &p_this->defer_job);
    aw_delayed_work_stop(&p_this->delay_work);
}

/******************************************************************************/

static aw_err_t __bad_blk_rw_chk (struct aw_ftl_dev *p_ftl, uint16_t pbn)
{
    aw_err_t    ret;
    uint32_t    offs;
    int         i, j;
    char        oob[256];

    ret = aw_ftl_mtd_block_erase(p_ftl, pbn);
    if (ret != AW_OK) {
        return -AW_EBADMSG;
    }

    for (i = 0; i < p_ftl->sectors_per_blk + p_ftl->sectors_per_blk_hdr; i++) {
        memset(p_ftl->buf, 0, p_ftl->write_size);
        memset(oob, 0, sizeof(oob));
        ret = aw_ftl_mtd_oob_write(p_ftl, 
                                   pbn,
                                   i, 
                                   p_ftl->buf, 
                                   oob, 
                                   p_ftl->mtd->oob_avail);
        if (ret != AW_OK) {
            return -AW_EBADMSG;
        }

        ret = aw_ftl_mtd_oob_read(p_ftl, 
                                  pbn,
                                  i,
                                  p_ftl->buf, 
                                  oob, 
                                  p_ftl->mtd->oob_avail);
        if (ret != AW_OK) {
            return -AW_EBADMSG;
        }

#if (AW_FTL_CONFIG_BAD_CHECK_LEVEL_HIGH)
        for (j = 0; j < p_ftl->write_size; j++) {
            if (p_ftl->buf[j] != 0) {
                return -AW_EBADMSG;
            }
        }
        for (j = 0; j < p_ftl->mtd->oob_avail; j++) {
            if (oob[j] != 0) {
                return -AW_EBADMSG;
            }
        }
#endif
    }

    return AW_OK;
}

/**
 * \brief 测试物理块是否为坏块，如果是坏块则标记坏块，如果是好块则擦除该块。
 * 先判断该块是否已经被标记为坏块，如果不是则开始测试该块是否为坏块。
 * 测试步骤：擦除该块，整块写入0包括oob区，读出整块数据判断是否全为0，
 *
 * \param[in] p_ftl :
 * \param[in] pbn : 物理块编号
 * \param[in] force : 为 1 强制格式化，并进行读写测试
 *
 * \retval AW_OK    : 不是坏块
 * \retval -AW_EBADMSG : 是坏块
 * \retval -AW_EIO     : 有io错误
 */
static aw_err_t __bad_block_verify (struct aw_ftl_dev *p_ftl, uint16_t pbn, char force)
{
    aw_err_t    ret;
    uint32_t    offs;

    offs = (1 << p_ftl->log2_blk_size) * pbn;

    if (force == AW_FALSE) {
        ret = aw_mtd_block_is_bad(p_ftl->mtd, offs);
        if (ret > 0) {
            return -AW_EBADMSG;
        } else if (ret < 0) {
            return -AW_EIO;
        }
    }

    ret = __bad_blk_rw_chk(p_ftl, pbn);
    if (ret != AW_OK) {
        AW_FTL_ERR_MSG(__ERR_MSG_MASK,
                        "the block(%d) is bad, check again!!!", pbn);
        ret = __bad_blk_rw_chk(p_ftl, pbn);
        if (ret != AW_OK) {
            goto _badblk_process;
        }
    }

    AW_FTL_MSG(__MASK_DBG, "aw_ftl_blk_format [ begin ]");

    /* 正常块则进行ftl的格式化  */
    ret = aw_ftl_blk_format(p_ftl, pbn);
    if (ret != AW_OK) {
        goto _badblk_process;
    }
    AW_FTL_MSG(__MSG_MASK,
            "verify the block(%d, wear:%d) is good and format it!",
            pbn, p_ftl->pbt[pbn - p_ftl->first_pbn].wear_leveling);

    return AW_OK;

_badblk_process:
    AW_FTL_ERR_MSG(__ERR_MSG_MASK,
                    "verify the block(%d) is bad and mark it bad!", pbn);
    aw_mtd_block_mark_bad(p_ftl->mtd, offs);
    return -AW_EBADMSG;
}
/******************************************************************************/
/**
 * \brief 如果链表不为空的话，回收脏链表里的块
 *
 * \param[in] p_ftl : FTL设备结构体
 */
void aw_ftl_dirty_blk_recover (struct aw_ftl_dev *p_ftl)
{
    struct aw_ftl_blk_info *cur_blk = NULL;
    int ret = -1;

    if (!aw_list_empty(&p_ftl->dirty_blk_list)) {
        cur_blk = aw_list_entry(p_ftl->dirty_blk_list.next, aw_ftl_blk_info_t, node);

        if (&cur_blk->node == cur_blk->node.next) {
            AW_FTL_ERR_MSG(__ERR_MSG_MASK, "BUG!!! dirty list not empty, "
                       "but &cur_blk->node == cur_blk->node.next (&cur_blk->node:0x%08x)", 
                       &cur_blk->node);
            return;
        }

        if (p_ftl->need_verify_blk == cur_blk->pbn) {
            AW_FTL_MSG(__MSG_MASK,
                    "cann't recover the dirty block(pbn:%d), "
                    "need verify bad block.", cur_blk->pbn);

            aw_ftl_dirty_node_del(p_ftl, cur_blk);
            aw_ftl_bad_node_add(p_ftl, cur_blk->pbn);
            p_ftl->need_verify_blk = -1;
            return;
        }

        ret = aw_ftl_blk_format(p_ftl, cur_blk->pbn);
        if (ret == AW_OK) {
            AW_FTL_MSG(AW_FTL_MSG_MASK_FORMAT, 
                       "recover the dirty block(pbn:%d), "
                       "erase and format the block [ OK ]. ", cur_blk->pbn);
            aw_ftl_dirty_node_del(p_ftl, cur_blk);
            aw_ftl_free_node_add(p_ftl, cur_blk->pbn);
        } else if (ret == -AW_EPERM ) {
            AW_FTL_ERR_MSG(__ERR_MSG_MASK, "recover the dirty block(pbn:%d), "
                                   "erase and format the block ERROR(%d). ", 
                                   cur_blk->pbn, 
                                   ret);
            aw_ftl_dirty_node_del(p_ftl, cur_blk);
            aw_ftl_bad_node_add(p_ftl, cur_blk->pbn);
        }
    }
}

/**
 * \brief 将物理块加入到脏块链当中
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn   : 物理块编号
 * \return 无
 */
int aw_ftl_dirty_node_add (struct aw_ftl_dev      *p_ftl,
                           struct aw_ftl_blk_info *blk_info)
{
    struct aw_ftl_blk_tag    blk_tag;
    int                     ret;

    if (blk_info == NULL) {
        AW_FTL_ERR_MSG(__ERR_MSG_MASK, "log_blk point is NULL");
        return -AW_EINVAL;
    }

    if (!aw_list_empty(&blk_info->node) && blk_info->node.next != NULL) {
        aw_list_del_init(&blk_info->node);
    }

    blk_tag.flag = AW_FTL_BLOCK_TAG_GARBAGE;

    AW_FTL_MSG(__MSG_MASK, "add dirty blk list.(pbn:%d)", blk_info->pbn);

    /* 写块信息标签 */
    ret = aw_ftl_blk_hdr_write(p_ftl, blk_info->pbn, NULL, &blk_tag);
    if (ret != AW_OK) {
        return ret;
    }

    aw_list_add(&blk_info->node, &p_ftl->dirty_blk_list);

    return AW_OK;
}

/**
 * \brief 将物理块从脏块链当中删除
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn : 物理块编号
 * \return 无
 */
void aw_ftl_dirty_node_del (struct aw_ftl_dev      *p_ftl,
                            struct aw_ftl_blk_info *blk_info)
{
    if (!aw_list_empty(&blk_info->node) && blk_info->node.next != NULL) {
        aw_list_del_init(&blk_info->node);
    }
}


/**
 * \brief 从空闲链表当中获取到一个空闲物理块
 * \param[in] p_ftl : FTL设备结构体
 * \return 获取到的物理块编号,AW_FTL_UNASSIGN_BLK_NO表示当前没有空闲物理块
 */
struct aw_ftl_blk_info * aw_ftl_free_node_get (struct aw_ftl_dev *p_ftl)
{
    aw_ftl_blk_info_t *p_blk_info = NULL;
    int ret;

again:
    /* 获取新块前先清理一次脏块和坏块，避免有脏块，却又没有空闲块的情况 */
    if (aw_list_empty(&p_ftl->free_blk_list)) {
        aw_ftl_dirty_blk_recover(p_ftl);
        aw_ftl_bad_list_process(p_ftl);
    }

    if (aw_list_empty(&p_ftl->free_blk_list)) {

        AW_FTL_ERR_MSG(__ERR_MSG_MASK, "cann't get a free block node");
        return NULL;
    } else {
        p_blk_info = AW_CONTAINER_OF(p_ftl->free_blk_list.next,
                                     aw_ftl_blk_info_t,
                                     node);
        aw_list_del_init(p_ftl->free_blk_list.next);

        /* 物理块信息的lbn为0xffff则表示该物理块未被使用过，需要检查该块是否真的干净  */
        if (p_blk_info->lbn == 0xFFFF) {
            ret = __flash_blk_check(p_ftl, p_blk_info);
            if (ret == AW_OK) {
                AW_FTL_MSG(__MSG_MASK, 
                           "first check physical block(%d) OK. ", 
                           p_blk_info->pbn);
            } else {
                aw_ftl_bad_node_add(p_ftl, p_blk_info->pbn);
                AW_FTL_ERR_MSG(__MSG_MASK, "first check physical block(%d) FAILED,"
                        " add to bad list. ", p_blk_info->pbn);

                goto again;
            }
        }

        return p_blk_info;
    }
}


/**
 * \brief 将物理块加入到空闲链当中
 * \param[in] p_ftl : FTL设备结构体
 * \param[in] pbn : 物理块编号
 * \return 无
 */
void aw_ftl_free_node_add (struct aw_ftl_dev *p_ftl,
                           uint16_t           pbn)
{
    struct aw_ftl_blk_info *cur_pos = NULL;
    struct aw_ftl_blk_info *blk_info = aw_ftl_pbn_info_get(p_ftl, pbn);

    if (!aw_list_empty(&blk_info->node) && blk_info->node.next != NULL) {
        aw_list_del_init(&blk_info->node);
    }

    /* 如果空闲链表为空，直接加到链表的最后 */
    if (aw_list_empty(&p_ftl->free_blk_list)) {
        aw_list_add(&blk_info->node, &p_ftl->free_blk_list);
        return;
    }

    aw_list_for_each_entry(cur_pos,
                           &p_ftl->free_blk_list,
                           aw_ftl_blk_info_t,
                           node) {

        if (blk_info->wear_leveling > cur_pos->wear_leveling) {
            continue;
        } else {
            aw_list_add_tail(&blk_info->node, &cur_pos->node);
            return;
        }
    }

    aw_list_add_tail(&blk_info->node, &p_ftl->free_blk_list);
}

void aw_ftl_bad_node_add (struct aw_ftl_dev *p_ftl,
                          uint16_t           pbn)
{
    struct aw_ftl_blk_info *blk_info = aw_ftl_pbn_info_get(p_ftl, pbn);

    if (!aw_list_empty(&blk_info->node) && blk_info->node.next != NULL) {
        aw_list_del_init(&blk_info->node);
    }
    aw_list_add(&blk_info->node, &p_ftl->bad_blk_list);
}


void aw_ftl_bad_list_process (struct aw_ftl_dev *p_ftl)
{
    struct aw_ftl_blk_info *blk_info;
    struct aw_ftl_blk_info *tmp;
    aw_err_t                ret;

    if (aw_list_empty(&p_ftl->bad_blk_list)) {
        return;
    }

    aw_list_for_each_entry_safe(blk_info,
                                tmp,
                                &p_ftl->bad_blk_list,
                                struct aw_ftl_blk_info,
                                node) {

        aw_list_del_init(&blk_info->node);

        if (p_ftl->mtd->type == AW_MTD_TYPE_NAND_FLASH) {
            ret = __bad_block_verify(p_ftl, blk_info->pbn, AW_FALSE);
        } else if (p_ftl->mtd->type == AW_MTD_TYPE_NOR_FLASH) {
            ret = aw_ftl_blk_format(p_ftl, blk_info->pbn);
        } else {
            ret = -AW_EINVAL;
        }

        if (ret == AW_OK) {
            aw_ftl_free_node_add(p_ftl, blk_info->pbn);
        } else if (ret == -AW_EBADMSG) {
            aw_list_add(&blk_info->node, &_g_markbad_list);
            AW_FTL_MSG(__MSG_MASK, 
                       "pbn:%d is bad, add to bad_list", blk_info->pbn);
        } else {
            return;
        }

        break;
    }
}

void aw_ftl_markbad_list_process (struct aw_ftl_dev *p_ftl)
{
    struct aw_ftl_blk_info *blk_info;
    struct aw_ftl_blk_info *tmp;
    aw_err_t                ret;

    if (aw_list_empty(&_g_markbad_list)) {
        return;
    }

    aw_list_for_each_entry_safe(blk_info,
                                tmp,
                                &_g_markbad_list,
                                struct aw_ftl_blk_info,
                                node) {

        if (p_ftl->mtd->type == AW_MTD_TYPE_NAND_FLASH) {
            AW_FTL_MSG(__MSG_MASK, "force verify pbn:%d", blk_info->pbn);
            ret = __bad_block_verify(p_ftl, blk_info->pbn, AW_TRUE);

            if (ret == AW_OK) {
                aw_list_del_init(&blk_info->node);

                aw_ftl_free_node_add(p_ftl, blk_info->pbn);
            } else if (ret == -AW_EBADMSG) {
                AW_FTL_MSG(__MSG_MASK, "pbn:%d is bad", blk_info->pbn);
            }
        }
    }
}






