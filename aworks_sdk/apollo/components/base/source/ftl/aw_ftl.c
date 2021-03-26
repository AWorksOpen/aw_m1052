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
 * \brief FLASH device manage sub system
 *
 * \internal
 * \par modification history:
 * - 1.00 17-4-15  vih, first implementation.
 * \endinternal
 */

#include "aworks.h"

#include "string.h"
#include "aw_pool.h"

#include "ftl/aw_ftl_core.h"
#include "ftl/aw_ftl_utils.h"
#include "ftl/aw_ftl_flash.h"
#include "ftl/aw_ftl_map.h"
#include "ftl/aw_ftl_config.h"

/******************************************************************************/
#define PFX "FTL"
#define __MSG_MASK      AW_FTL_MSG_MASK_ITF

/*----------------------------------------------------------------------------*/
#if defined(__VIH_DEBUG)
#include "aw_shell.h"
extern int aw_shell_printf_with_color (aw_shell_user_t *p_user,
                                       const char *color,
                                       const char *func_name,
                                       const char *fmt,
                                       ...);
#define __LOG_BASE(mask, color, fmt, ...)   \
    if ((mask)) {\
        aw_shell_printf_with_color(AW_DBG_SHELL_IO, \
                                   color,\
                                   __func__,\
                                   fmt, \
                                   ##__VA_ARGS__);\
    }
#define __LOG_BASE_ERR   __LOG_BASE
#else
#include "aw_vdebug.h"
#define __LOG_BASE(mask, color, fmt, ...)
#define __LOG_BASE_ERR(mask, color, fmt, ...)  \
    if (mask) {AW_INFOF((fmt, ##__VA_ARGS__)); }
#endif

#define __ASSERT_DETAIL(cond, fmt, ...) \
    if (!(cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED,\
                   "[ ASSERT Failed ][%s:%d] " #cond "\n", \
                   __func__, __LINE__); \
        if (fmt != NULL) {\
            __LOG_BASE_ERR(1, NULL,"[ DETAIL ] " fmt "\n", \
                       ##__VA_ARGS__); \
        } \
        while(1); \
    }
#define __WARN_DETAIL_IF(cond, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                   "[ WARN ][%s:%d] " #cond "\n",\
                   __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"[ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
    }
#define __RETURN_DETAIL_IF(cond, ret_exp, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                   "[ ERROR:%d ][%s:%d] " #cond "\n", \
                   (int)(ret_exp), __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL, "[ DETAIL ] " fmt "\n", ##__VA_ARGS__); \
        }\
        return (ret_exp); \
    }

#define __EXIT_DETAIL_IF(cond, fmt, ...)  \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                   "[ ERROR ][%s:%d] " #cond "\n", __func__, __LINE__); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"[ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
        return; \
    }
#define __GOTO_EXIT_DETAIL_IF(cond, ret_val, fmt, ...) \
    if ((cond)) {\
        __LOG_BASE_ERR(1, AW_SHELL_FCOLOR_RED, \
                   "[ ERROR:%d ][%s:%d] " #cond "\n", \
                   (int)(ret_val), __func__, __LINE__); \
        ret = (ret_val); \
        if (strlen(fmt) != 0) {\
            __LOG_BASE_ERR(1, NULL,"[ DETAIL ] " fmt "\n", ##__VA_ARGS__);\
        }\
        goto exit; \
    }

#define __ASSERT(cond)                __ASSERT_DETAIL(cond, "")
#define __WARN_IF(cond)               __WARN_DETAIL_IF(cond, "")
#define __RETURN_IF(cond, ret_exp)    __RETURN_DETAIL_IF(cond, ret_exp, "")
#define __EXIT_IF(cond)               __EXIT_DETAIL_IF(cond, "");
#define __GOTO_EXIT_IF(cond, ret_val) __GOTO_EXIT_DETAIL_IF(cond, ret_val, "")

/*----------------------------------------------------------------------------*/
typedef enum {
   __FTL_DATA_WR_STATUS_HEAD = 0,
   __FTL_DATA_WR_STATUS_MIDDLE,
   __FTL_DATA_WR_STATUS_TAIL,
} __ftl_data_wr_status_t;
/******************************************************************************/
/** \brief �ع���ṹ�� */
aw_local struct aw_pool      __g_ftl_dev_pool;
/******************************************************************************/
aw_local int __ftl_dev_deinit (struct aw_ftl_dev *p_ftl);

static void __ftl_dev_unregister (struct aw_ftl_dev *p_ftl);

aw_local void *__ftl_dev_free (struct aw_ftl_dev *p_ftl);

static struct aw_ftl_dev *__ftl_dev_get (const char *ftl_path);
aw_local aw_err_t __ftl_dev_put (
    struct aw_ftl_dev *p_ftl,
    void             (*pfn_release) (struct aw_refcnt *p_ref));
aw_local int __ftl_mem_deinit (struct aw_ftl_dev *p_ftl);

void aw_ftl_markbad_list_process (struct aw_ftl_dev *p_ftl);
/******************************************************************************/
/** \brief FTL�������� */
aw_local struct aw_list_head __g_ftl_dev_list;
aw_local aw_bool_t           __g_ftl_init = AW_FALSE;
AW_MUTEX_DECL_STATIC(        __g_ftl_dev_list_mutex);
aw_local aw_bool_t           __g_ftl_dev_list_mutex_init = AW_FALSE;

static aw_err_t __ftl_dev_list_lock (void)
{
    if (!__g_ftl_dev_list_mutex_init) {
        if (AW_MUTEX_INIT(__g_ftl_dev_list_mutex, AW_SEM_Q_PRIORITY) == NULL) {
            return -AW_ENOMEM;
        }
        __g_ftl_dev_list_mutex_init = AW_TRUE;
    }

    return AW_MUTEX_LOCK(__g_ftl_dev_list_mutex, AW_SEM_WAIT_FOREVER);
}

static aw_err_t __ftl_dev_list_unlock (void)
{
    if (!__g_ftl_dev_list_mutex_init) {
        if (AW_MUTEX_INIT(__g_ftl_dev_list_mutex, AW_SEM_Q_PRIORITY) == NULL) {
            return -AW_ENOMEM;
        }
        __g_ftl_dev_list_mutex_init = AW_TRUE;
    }

    return AW_MUTEX_UNLOCK(__g_ftl_dev_list_mutex);
}
/*******************************************************************************
  FTL��¼�ṹ��ع���ӿ�
*******************************************************************************/
/**
 * \brief FTL�����ڴ����
 * \param ��
 * \retval FTL����ָ��
 */
static struct aw_ftl_dev *__ftl_dev_alloc (const char *p_ftl_path)
{
    struct aw_ftl_dev *p_ftl = NULL;

    p_ftl = __ftl_dev_get(p_ftl_path);
    if (NULL != p_ftl) {
        return p_ftl;
    }

    p_ftl = aw_pool_item_get(&__g_ftl_dev_pool);

    if (NULL != p_ftl) {
        (void)memset(p_ftl, 0, sizeof(*p_ftl));
        strncpy(p_ftl->name, p_ftl_path, __FTL_NAME_MAX);
    }
    return p_ftl;
}


/**
 * \brief FTL�����ڴ��ͷ�
 * \param[in] p_ftl : FTL����ָ��
 *
 * \retval NULL          : �ͷųɹ�
 * \retval ԭFTL����ָ�� : �ͷ�ʧ��
 */
aw_local void *__ftl_dev_free (struct aw_ftl_dev *p_ftl)
{
    int err;

    memset(p_ftl->name, 0, __FTL_NAME_MAX);

    err = aw_pool_item_return(&__g_ftl_dev_pool, p_ftl);

    return 0 == err ? NULL : p_ftl;
}


/*******************************************************************************
  FTL��������ά��
*******************************************************************************/


/**
 * \brief FTL����ע��
 * \param[in] p_ftl : FTL��¼�ṹ��
 * \return ��
 */
static void __ftl_dev_register (struct aw_ftl_dev *p_ftl)
{
    __ftl_dev_list_lock();
    aw_list_add(&p_ftl->node, &__g_ftl_dev_list);
    __ftl_dev_list_unlock();

    p_ftl->resource++;
}


/**
 * \brief FTL����ע��
 * \param[in] p_ftl : FTL��¼�ṹ��
 * \return ��
 */
static void __ftl_dev_unregister (struct aw_ftl_dev *p_ftl)
{
    __ftl_dev_list_lock();
    aw_list_del_init(&p_ftl->node);
    __ftl_dev_list_unlock();

    p_ftl->resource--;
}

static struct aw_ftl_dev *__ftl_dev_get (const char *ftl_path)
{
    aw_bool_t isfind = AW_FALSE;

    struct aw_ftl_dev *cur = NULL;
    struct aw_ftl_dev *tmp = NULL;

    __ftl_dev_list_lock();
    aw_list_for_each_entry_safe(cur, tmp, &__g_ftl_dev_list, aw_ftl_dev_t, node) {

        if (0 == strcmp(ftl_path, cur->name)) {
            isfind = AW_TRUE;
            break;
        }
    }
    __ftl_dev_list_unlock();

    if (isfind) {
        aw_refcnt_get(&cur->ref);
    }

    return (isfind ? cur : NULL);
}

aw_local aw_err_t __ftl_dev_put (struct aw_ftl_dev *p_ftl,
                             void             (*pfn_release) (struct aw_refcnt *p_ref))
{
    return aw_refcnt_put(&p_ftl->ref, pfn_release);
}

/******************************************************************************/

/**
 * \brief ɾ���߼���������
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] lsn : ������(����ҳ���� - sectors_per_blk_hdr)
 *
 * \retval AW_OK : �����ɹ�
 */
aw_local int __ftl_sector_dat_del (struct aw_ftl_dev *p_ftl,
                                   uint32_t           lsn)
{
    int      ret      = AW_OK;
    int      log_blk, sector;

    uint16_t lbn;
    uint16_t idx;

    __RETURN_IF(NULL == p_ftl, -AW_EINVAL);
    __RETURN_DETAIL_IF(lsn >= p_ftl->data_sector_total, -AW_EINVAL,
                       "sector(%d) number out of range", lsn);


    lbn  = lsn / p_ftl->sectors_per_blk;
    idx  = lsn % p_ftl->sectors_per_blk;

    if (aw_ftl_logic_sector_be_used(p_ftl, lbn, idx)) {

        /* �޸������ݣ�����Ҫ����ɵ�ϵͳ��Ϣ���� */
        aw_ftl_sys_info_clear(p_ftl);

        if (!aw_ftl_logic_sector_is_dirty(p_ftl, lbn, idx)) {

            ret = aw_ftl_logic_sector_dirty_tag_write(p_ftl, lbn, idx);

        } else {

            /* �ҳ���������ݵ���־������ */
            for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {

                if (!p_ftl->log_tbl[log_blk].p_pyh_blk) {
                    break;
                }

                for (sector = 0;
                        sector < p_ftl->log_tbl[log_blk].sector_use_cnt;
                        sector++) {

                    if (!aw_ftl_log_sector_is_dirty(p_ftl, log_blk, sector) &&
                        p_ftl->log_tbl[log_blk].map[sector].lbn == lbn &&
                        p_ftl->log_tbl[log_blk].map[sector].sn  == idx) {

                        ret = aw_ftl_log_sector_dirty_tag_write(p_ftl,
                                                               log_blk,
                                                               sector);
                    }
                }
            }
        }
    }


    return ret;
}

/**
 * \brief FTL������ڴ涯̬����
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 *
 * \retval AW_OK      : �����ɹ�
 * \retval AW_FTL_MEM_ERR : ����ʧ��
 */
aw_local int __ftl_mem_init (struct aw_ftl_dev *p_ftl)
{
    int i = 0;
    uint32_t size;

    if (NULL == p_ftl->pbt) {
        size = sizeof(aw_ftl_blk_info_t) * p_ftl->nphy_blocks;

        p_ftl->pbt = (aw_ftl_blk_info_t *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_ftl->pbt) {
            goto _failed;
        }

        for (i = 0; i < p_ftl->nphy_blocks; i++) {
            p_ftl->pbt[i].wear_leveling = 0;
            aw_list_head_init(&p_ftl->pbt[i].node);
        }
    }

    if (NULL == p_ftl->buf) {
        size = 1 << p_ftl->log2_sector_size;

        p_ftl->buf = (uint8_t *)aw_ftl_mem_alloc(p_ftl, size);
        if (NULL == p_ftl->buf) {
            goto _failed;
        }
    }

    return AW_OK;

_failed:
    __ftl_mem_deinit(p_ftl);

    return -AW_ENOMEM;
}



/**
 * \brief FTL����ڴ���ͷ�
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \return AW_OK : �������
 */
aw_local int __ftl_mem_deinit (struct aw_ftl_dev *p_ftl)
{
    aw_ftl_mem_free(p_ftl, (void **)&p_ftl->pbt);
    aw_ftl_mem_free(p_ftl, (void **)&p_ftl->buf);

    return AW_OK;
}


aw_local int __ftl_flash_info_init (struct aw_ftl_dev *p_ftl)
{
    int size;
    struct aw_mtd_info *mtd = p_ftl->mtd;

    for (size = 1, p_ftl->log2_blk_size = 0;
         size < mtd->erase_size;
         p_ftl->log2_blk_size++, size <<= 1);

    for (size = 1, p_ftl->log2_write_size = 0;
         size < mtd->write_size;
         p_ftl->log2_write_size++, size <<= 1);

    p_ftl->blk_size    = mtd->erase_size;
    p_ftl->write_size  = mtd->write_size;

    if (AW_MTD_TYPE_NAND_FLASH == mtd->type) {

        p_ftl->sector_size      = mtd->write_size;

        p_ftl->log2_sector_size = p_ftl->log2_write_size;

        p_ftl->sectors_per_blk_hdr = ((sizeof(aw_ftl_pb_hdr_t) - 1) >>
                                     p_ftl->log2_write_size) + 1;

        p_ftl->sectors_per_blk = (mtd->erase_size >> p_ftl->log2_sector_size) -
                                p_ftl->sectors_per_blk_hdr;

    } else if (AW_MTD_TYPE_NOR_FLASH == mtd->type) {

        for (size = 1, p_ftl->log2_sector_size = 0;
             size < AW_FTL_NOR_SECTOR_SIZE;
             p_ftl->log2_sector_size++, size <<= 1);

        p_ftl->sector_size = AW_FTL_NOR_SECTOR_SIZE;

        p_ftl->sectors_per_blk = 1 << (p_ftl->log2_blk_size - p_ftl->log2_sector_size);

        p_ftl->sectors_per_blk_hdr = ((sizeof(aw_ftl_pb_hdr_t) +
                                     sizeof(struct aw_ftl_sector_tag) * p_ftl->sectors_per_blk - 1) >>
                                     p_ftl->log2_sector_size) + 1;

        p_ftl->sectors_per_blk -= p_ftl->sectors_per_blk_hdr;

    } else {
        __RETURN_DETAIL_IF(AW_TRUE, -AW_ENODEV,
                           "mtd type isn't NOR-Flash or NAND-Flash");
    }

    p_ftl->first_pbn          = 0;
    p_ftl->nphy_blocks        = mtd->size >> p_ftl->log2_blk_size;

    if (AW_FTL_CONFIG_NLOGBLKS == -1) {
        p_ftl->nlog_blocks      = 128 / (mtd->erase_size / mtd->write_size) +
                                  (128 % (mtd->erase_size / mtd->write_size) ?
                                  1 : 0);
    } else {
        p_ftl->nlog_blocks      = AW_FTL_CONFIG_NLOGBLKS;
    }

    p_ftl->nrsblocks    = AW_FTL_CONFIG_NRSBLKS;
    p_ftl->ndata_blocks = p_ftl->nphy_blocks - p_ftl->nlog_blocks - p_ftl->nrsblocks;

    p_ftl->sector_total       = mtd->size >> p_ftl->log2_sector_size;
    p_ftl->data_sector_total  = p_ftl->ndata_blocks * p_ftl->sectors_per_blk;
    p_ftl->free_sector_total  = p_ftl->data_sector_total;

    aw_list_head_init(&p_ftl->bad_blk_list);
    aw_list_head_init(&p_ftl->free_blk_list);
    aw_list_head_init(&p_ftl->dirty_blk_list);

    p_ftl->buf = NULL;

    AW_MUTEX_INIT(p_ftl->op_mutex, AW_SEM_Q_FIFO);
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    return AW_OK;
}




aw_local uint32_t __list_len_calculate (struct aw_list_head *head)
{
    uint32_t len = 0;
    struct aw_list_head *cur = head->next;

    for (len = 0; cur != head; len++) {
        cur = cur->next;
    }
    return len;
}

/**
 * \brief �����װ��
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] pbn   : �������
 *
 * \retval AW_OK                 : �����װ�سɹ�
 * \retval AW_FTL_READ_FAIL      : mtd��ȡʧ��
 * \retval AW_FTL_BLK_HEADER_ERR : �����ͷ��Ϣ����
 * \retval AW_FTL_BAD_BLK_TAG    : ������ǩ����
 */
aw_local int __ftl_blk_quick_mount (struct aw_ftl_dev *p_ftl, uint16_t pbn)
{
    int                     ret = AW_OK;
    struct aw_ftl_blk_tag   blk_tag;
    struct aw_ftl_pb_hdr    hdr;
    struct aw_ftl_blk_info *cur_blk = aw_ftl_pbn_info_get(p_ftl, pbn);
    aw_ftl_sys_info_t      *p_sys_info = &p_ftl->sys_info;

    /* ��ȡ����� ��ǩ��Ϣ     */
    ret = aw_ftl_blk_hdr_read(p_ftl, pbn, &hdr, &blk_tag);
    if (AW_OK != ret) {
        /* �豸������Ϣ   */
        if (aw_ftl_blk_hdr_format_verify(&hdr)) {
            cur_blk->wear_leveling = hdr.wear_leveling_info;
        }

        /* �ӵ�bad������ */
        aw_ftl_bad_node_add(p_ftl, pbn);
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block(%d) header information read failed", pbn);
        return ret;
    }

    /* ��֤��ʽ     */
    if (aw_ftl_blk_hdr_format_verify(&hdr)) {

        /* ������֤�ɹ���������֤ */
        if (hdr.flags            != 0x00 ||
            hdr.log2_sector_size != p_ftl->log2_sector_size) {

            aw_ftl_bad_node_add(p_ftl, pbn);

            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block(%d) header format error", pbn);
            return -AW_EINVAL;
        }
    } else {
        /* û�п���Ϣ�Ŀ�Ҳ��Ҫ����Ƿ��ǻ���   */
        aw_ftl_bad_node_add(p_ftl, pbn);

        AW_FTL_ERR_MSG(0, "block(%d) header format error", pbn);
        return -AW_EINVAL;
    }

    /* �豸������Ϣ   */
    cur_blk->wear_leveling = hdr.wear_leveling_info;

    p_ftl->lowest_wear = p_ftl->lowest_wear < cur_blk->wear_leveling?
                         p_ftl->lowest_wear : cur_blk->wear_leveling;
    cur_blk->pbn = pbn;

    switch (blk_tag.flag) {

    /* data block */
    case AW_FTL_BLOCK_TAG_DATA:
        cur_blk->lbn = blk_tag.lbn;
        p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk = cur_blk;

        break;

    case AW_FTL_BLOCK_TAG_FREE:
        /* �ӵ����������� */
        cur_blk->lbn = 0xFFFF;
        aw_ftl_free_node_add(p_ftl, pbn);

        break;

    case AW_FTL_BLOCK_TAG_LOG:

        cur_blk->lbn = 0xFFFF;
        p_ftl->log_tbl[blk_tag.log].p_pyh_blk = cur_blk;

        break;

    case AW_FTL_BLOCK_TAG_SYS_INFO:

        if (blk_tag.log >= p_sys_info->nsys_blk) {

            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, 
                           "sys info blk(%d) out of range ", 
                           blk_tag.log);

            /* �ӵ��������� */
            aw_ftl_dirty_node_add(p_ftl, cur_blk);

            break;
        }

        if (!p_sys_info->blk_valid[blk_tag.log]) {
            p_sys_info->array[blk_tag.log]     = pbn;
            p_sys_info->blk_valid[blk_tag.log] = AW_TRUE;

        } else {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "sys info blk repeat: pbn:%d, pbn:%d",
                    p_sys_info->array[blk_tag.log], pbn);

            /* �ӵ��������� */
            aw_ftl_dirty_node_add(p_ftl, cur_blk);
        }

        break;

    case AW_FTL_BLOCK_TAG_DATA_IN_LOG:
        break;

    case AW_FTL_BLOCK_TAG_GARBAGE:
    case AW_FTL_BLOCK_TAG_DATA_IN_BLK:
        /* �ӵ��������� */
        aw_ftl_dirty_node_add(p_ftl, cur_blk);

        break;

    /* error */
    default:
        aw_ftl_bad_node_add(p_ftl, pbn);

        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block tag is bad");
        ret = -AW_EINVAL;

        break;
    }

    if (pbn == p_ftl->first_pbn + p_ftl->nphy_blocks - 1) {
        AW_FTL_MSG(__MSG_MASK, "free:%d, dirty:%d, bad:%d %s",
                __list_len_calculate(&p_ftl->free_blk_list),
                __list_len_calculate(&p_ftl->dirty_blk_list),
                __list_len_calculate(&p_ftl->bad_blk_list),
                (__list_len_calculate(&p_ftl->bad_blk_list) ? "[ WARN ]" : "  "));
    }

    return ret;
}




/**
 * \brief �����װ��
 *
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \param[in] pbn   : �������
 *
 * \retval AW_OK                 : �����װ�سɹ�
 * \retval AW_FTL_READ_FAIL      : mtd��ȡʧ��
 * \retval AW_FTL_BLK_HEADER_ERR : �����ͷ��Ϣ����
 * \retval AW_FTL_BAD_BLK_TAG    : ������ǩ����
 */
aw_local int __ftl_blk_mount (struct aw_ftl_dev *p_ftl, uint16_t pbn)
{
    int                     ret = AW_OK;
    struct aw_ftl_blk_tag   blk_tag;
    struct aw_ftl_pb_hdr    hdr;
    struct aw_ftl_blk_info *cur_blk = aw_ftl_pbn_info_get(p_ftl, pbn);

    /* ��ȡ�����ͷ��Ϣ     */
    ret = aw_ftl_blk_hdr_read(p_ftl, pbn, &hdr, &blk_tag);
    if (AW_OK != ret) {
        /* �豸������Ϣ   */
        if (aw_ftl_blk_hdr_format_verify(&hdr)) {
            cur_blk->wear_leveling = hdr.wear_leveling_info;
        }

        /* �ӵ�bad������ */
        aw_ftl_bad_node_add(p_ftl, pbn);
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block(%d) header information read failed", pbn);
        return ret;
    }

    /* ��֤��ʽ     */
    if (aw_ftl_blk_hdr_format_verify(&hdr)) {

        /* ������֤�ɹ���������֤ */
        if (hdr.flags            != 0x00 ||
            hdr.log2_sector_size != p_ftl->log2_sector_size) {

            aw_ftl_bad_node_add(p_ftl, pbn);

            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block(%d) header format error", pbn);
            return -AW_EINVAL;
        }
    } else {
        /* û�п���Ϣ�Ŀ�Ҳ��Ҫ����Ƿ��ǻ���   */
        aw_ftl_bad_node_add(p_ftl, pbn);

        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "block(%d) header format error", pbn);
        return -AW_EINVAL;
    }

    /* �豸������Ϣ   */
    cur_blk->wear_leveling = hdr.wear_leveling_info;
    p_ftl->lowest_wear = p_ftl->lowest_wear < cur_blk->wear_leveling?
                         p_ftl->lowest_wear : cur_blk->wear_leveling;

    switch (blk_tag.flag) {

    /* data block */
    case AW_FTL_BLOCK_TAG_DATA:
        cur_blk->lbn = blk_tag.lbn;

        /*
         * ����Ѿ���ӳ����������ˣ����ʾ�ڵ�һ�׶�ʱ�����߼����Ѿ�д����ɣ���ǩ
         * д��Ϊ�ڶ��׶ε�IN_LOG����ȴ��δ���ļ���������д��������ǩ����ʱ��IN_LOG
         * �Ŀ� Ϊ��Ч�飬��TAG_DATA������뵽������
         */
        if (!p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk) {

            p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk = cur_blk;

            aw_ftl_logic_info_get(p_ftl, blk_tag.lbn);
        } else {
            aw_ftl_dirty_node_add(p_ftl, cur_blk);
        }

        break;

    case AW_FTL_BLOCK_TAG_DATA_IN_LOG:

        AW_FTL_MSG(__MSG_MASK,
                "system abnormal power off, begin recovery data. ");

        p_ftl->lbn_need_recovery = AW_TRUE;
        p_ftl->need_recovery_lbn = blk_tag.lbn;

        cur_blk->lbn = blk_tag.lbn;

        if (p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk) {
            aw_ftl_dirty_node_add(p_ftl,
                        p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk);
        }

        p_ftl->logic_tbl[blk_tag.lbn].p_pyh_blk = cur_blk;

        aw_ftl_logic_info_get(p_ftl, blk_tag.lbn);

        break;

    case AW_FTL_BLOCK_TAG_FREE:
        /* �ӵ����������� */
        cur_blk->lbn = 0xFFFF;
        aw_ftl_free_node_add(p_ftl, pbn);

        break;

    case AW_FTL_BLOCK_TAG_LOG:

        cur_blk->lbn = 0xFFFF;
        p_ftl->log_tbl[blk_tag.log].p_pyh_blk = cur_blk;

        break;

    case AW_FTL_BLOCK_TAG_SYS_INFO:

        AW_FTL_MSG(__MSG_MASK, "find invalid sys info blk, add to dirty list.");

        /* �ӵ��������� */
        aw_ftl_dirty_node_add(p_ftl, cur_blk);
        break;


    case AW_FTL_BLOCK_TAG_DATA_IN_BLK:
        AW_FTL_MSG(__MSG_MASK,
                "system abnormal power off, the blk data invalid. ");

        /* �ӵ��������� */
        aw_ftl_dirty_node_add(p_ftl, cur_blk);

        break;

    case AW_FTL_BLOCK_TAG_GARBAGE:
        /* �ӵ��������� */
        aw_ftl_dirty_node_add(p_ftl, cur_blk);

        break;

    /* error */
    default:
        aw_ftl_bad_node_add(p_ftl, pbn);
        return -AW_EINVAL;

        break;
    }

    if (pbn == p_ftl->first_pbn + p_ftl->nphy_blocks - 1) {
        AW_FTL_MSG(__MSG_MASK, "free:%d, dirty:%d, bad:%d",
                __list_len_calculate(&p_ftl->free_blk_list),
                __list_len_calculate(&p_ftl->dirty_blk_list),
                __list_len_calculate(&p_ftl->bad_blk_list));
    }

    return ret;
}

/**
 * \brief   ����������������
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \return  ���ؿ�������������
 *
 */
aw_local uint32_t __ftl_free_sector_calculate (struct aw_ftl_dev *p_ftl)
{
    uint16_t lgc_blk;
    uint32_t sector_use_cnt = 0;

    for (lgc_blk = 0; lgc_blk < p_ftl->ndata_blocks; lgc_blk++) {
        sector_use_cnt += p_ftl->logic_tbl[lgc_blk].sector_use_cnt;
    }

    return p_ftl->data_sector_total - sector_use_cnt;
}

/**
 * \brief ��������
 * \param[in] p_ftl : FTL�豸�ṹ��
 * \return ��
 *
 */
static void __ftl_garbage_collect (struct aw_ftl_dev *p_ftl)
{
    uint16_t lgc_blk;
    uint16_t log_blk;
    uint16_t log_use_cnt = 0;
    float    log_usage = 0, logic_usage;

    __EXIT_IF(NULL == p_ftl);

    p_ftl->free_sector_total = __ftl_free_sector_calculate(p_ftl);

    /* �����������򻵿������գ���ֱ������  */
    aw_ftl_dirty_blk_recover(p_ftl);
    aw_ftl_bad_list_process(p_ftl);

    /* ����log������ʹ��  */
    for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {

        log_use_cnt += p_ftl->log_tbl[log_blk].sector_use_cnt;
    }

    log_usage = (float)log_use_cnt / (float)(p_ftl->nlog_blocks * p_ftl->sectors_per_blk);

    /* �����־���ʹ���ʴﵽ�˷�ֵ����ϲ���־�����ݣ�������־����뵽��������  */
    if (p_ftl->log_clear_threshold <= 1 &&
        log_usage >= p_ftl->log_clear_threshold) {

        AW_FTL_MSG(__MSG_MASK, "log block be used %d%%, begin merge log block data. ",
                (log_use_cnt * 100) / (p_ftl->nlog_blocks * p_ftl->sectors_per_blk));

        aw_ftl_log_blk_merge_all(p_ftl);
    }

    if (p_ftl->logic_clear_threshold <= 1) {

        /* ���������ı��شﵽ��ֵʱ�Ը��߼�����и��� */
        for (lgc_blk = 0; lgc_blk < p_ftl->ndata_blocks; lgc_blk++) {

            logic_usage = (float)p_ftl->logic_tbl[lgc_blk].sector_dirty_cnt /
                          (float)p_ftl->sectors_per_blk;

            if (logic_usage >= p_ftl->logic_clear_threshold) {

                AW_FTL_MSG(__MSG_MASK, "begin merge logic block(lbn:%d). ", lgc_blk);

                /* ���Ƚ�����־���ݵĺϲ����ϲ����������߼�����û��dirty������ */
                aw_ftl_log_blk_merge(p_ftl, lgc_blk);
            }
        }
    }
}

/* tag:init */

/**
 * ftl�豸��ʼ�����Ὣû��ftl��Ϣ�Ŀ���и�ʽ��
 */
aw_local int __ftl_dev_init (struct aw_ftl_dev *p_ftl, struct aw_mtd_info *p_mtd)
{
    int ret, i;

    uint8_t  log_blk;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;

    __RETURN_IF(p_mtd == NULL || !p_ftl, -AW_EINVAL);

    (void)p_sys_info;

    p_ftl->mtd = p_mtd;

    /* ��ʼ��FTL  */
    ret = __ftl_flash_info_init(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    ret = __ftl_mem_init(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    if (p_ftl->nphy_blocks + p_ftl->first_pbn <= p_ftl->first_pbn) {
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "ftl format error");
        return -AW_EINVAL;
    }

    p_ftl->log_clear_threshold      = 0.85;
    p_ftl->logic_clear_threshold    = 1;
    p_ftl->need_verify_blk          = -1;

    p_ftl->lowest_wear = (~0);

    ret = aw_ftl_sys_info_init(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    ret = aw_ftl_map_table_init(p_ftl);
    if (ret != AW_OK) {
        return ret;
    }

    if (p_ftl->mtd->type == AW_MTD_TYPE_NAND_FLASH ||
        p_ftl->mtd->type == AW_MTD_TYPE_NOR_FLASH) {

        /** ����װ�����������     */
        for (i = p_ftl->first_pbn; i < p_ftl->nphy_blocks + p_ftl->first_pbn; i++) {

            __ftl_blk_quick_mount(p_ftl, i);
        }

        ret = aw_ftl_sys_info_download(p_ftl);
        if (ret == AW_OK) {
            p_ftl->sys_info.pbn_valid   = AW_TRUE;
            p_ftl->sys_info.need_update = AW_FALSE;

            AW_FTL_MSG(__MSG_MASK | AW_FTL_MSG_NO_PFX,
                    "%s: find and download the newest system information(", PFX);
            for (i = 0; i < p_ftl->sys_info.nsys_blk; i++) {
                AW_FTL_MSG(__MSG_MASK | AW_FTL_MSG_NO_PFX, "%d ", p_ftl->sys_info.array[i]);
            }
            AW_FTL_MSG(__MSG_MASK | AW_FTL_MSG_NO_PFX, ").\n");
        } else {
            p_ftl->sys_info.pbn_valid   = AW_FALSE;
            p_ftl->sys_info.need_update = AW_TRUE;
        }
    }

    /** ����װ�������ʧ�ܣ������ȫ��ɨ��     */
    if (!p_ftl->sys_info.pbn_valid) {
        struct aw_ftl_blk_info *p_pos;
        aw_bool_t find_in_list;

        AW_FTL_MSG(__MSG_MASK, "not find the system information, begin to scan all physical block. ");

        for (i = 0; i < p_ftl->ndata_blocks; i++) {
            p_ftl->logic_tbl[i].p_pyh_blk = NULL;
        }

        for (i = p_ftl->first_pbn; i < p_ftl->nphy_blocks + p_ftl->first_pbn; i++) {
            find_in_list = AW_FALSE;

            /* �Ѿ��� free ��bad��dirty �����ֱ������ */
            aw_list_for_each_entry(p_pos, &p_ftl->free_blk_list, struct aw_ftl_blk_info, node) {
                if (p_pos->pbn == i) {
                    find_in_list = AW_TRUE;
                    break;
                }
            }

            aw_list_for_each_entry(p_pos, &p_ftl->bad_blk_list, struct aw_ftl_blk_info, node) {
                if (p_pos->pbn == i) {
                    find_in_list = AW_TRUE;
                    break;
                }
            }

            aw_list_for_each_entry(p_pos, &p_ftl->dirty_blk_list, struct aw_ftl_blk_info, node) {
                if (p_pos->pbn == i) {
                    find_in_list = AW_TRUE;
                    break;
                }
            }

            if (find_in_list) {
                continue;
            }

            __ftl_blk_mount(p_ftl, i);
        }

        /* ������־���������Ϣ����Ҫ��װ���������� */
        for (log_blk = 0; log_blk < p_ftl->nlog_blocks; log_blk++) {
            aw_ftl_log_info_get(p_ftl, log_blk);
        }
    }

    p_ftl->free_sector_total = __ftl_free_sector_calculate(p_ftl);

    if (p_ftl->lbn_need_recovery) {

        /*
         * �����������־�����ݺϲ�ʱ���������������£�����־�������ݣ�����
         * ��Ӧ���߼��������ȴ��free�Ļ������в�������ݣ����߼������ݺϲ�
         */
        aw_ftl_log_blk_merge(p_ftl, p_ftl->need_recovery_lbn);
    }

    if (p_ftl->sys_info.update_time) {
        aw_ftl_defer_job_start(p_ftl, p_ftl->sys_info.update_time);
    }
    p_ftl->resource += 2;

    if (p_ftl->malloc_cnt < 1024) {
        AW_FTL_MSG(__MSG_MASK,
                "dynamic memory allocated size : %d KByte.", p_ftl->malloc_cnt);
    } else {
        AW_FTL_MSG(__MSG_MASK, "dynamic memory allocated size : %4.3f KByte.",
                (double)p_ftl->malloc_cnt / 1024);
    }

    aw_refcnt_init(&p_ftl->ref);

    return AW_OK;
}

/**
 * \brief ж��FTL
 */
aw_local int __ftl_dev_deinit (struct aw_ftl_dev *p_ftl)
{
    int ret;

    __RETURN_IF(NULL == p_ftl, -AW_EINVAL);

    ret = aw_ftl_sys_info_update(p_ftl);
    __WARN_DETAIL_IF(ret != AW_OK, "system information update failed(%d)???");

    aw_ftl_sys_info_deinit(p_ftl);
    aw_ftl_map_table_deinit(p_ftl);
    ret = __ftl_mem_deinit(p_ftl);

    aw_ftl_defer_job_stop(p_ftl);
    p_ftl->resource -= 2;

    return AW_OK;
}

aw_local void __ftl_dev_destroy (struct aw_refcnt  *p_ref)
{
    struct aw_ftl_dev *p_ftl =
            (struct aw_ftl_dev *)AW_CONTAINER_OF(p_ref, struct aw_ftl_dev, ref);

    __EXIT_IF(NULL == p_ftl);

    if (!aw_list_empty(&p_ftl->node)) {
        __ftl_dev_unregister(p_ftl);
    }

    __ftl_dev_deinit(p_ftl);

    p_ftl->resource--;
    __ftl_dev_free(p_ftl);

    __WARN_DETAIL_IF(p_ftl->resource != 0,
                     "ftl dev destory but resource remain : %d!!!", p_ftl->resource);
}
/*******************************����ӿ�***************************************/
void aw_ftl_garbage_collect (void)
{
    struct aw_ftl_dev *p_ftl = NULL;
    struct aw_ftl_dev *p_tmp = NULL;
    int                  ret;

    __ftl_dev_list_lock();
    aw_list_for_each_entry_safe(p_ftl, 
                                p_tmp, 
                                &__g_ftl_dev_list, 
                                struct aw_ftl_dev, 
                                node) {
        ret = AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_NO_WAIT);
        if (ret != AW_OK) {
            continue;
        }

        __ftl_garbage_collect(p_ftl);
        AW_MUTEX_UNLOCK(p_ftl->op_mutex);
    }
    __ftl_dev_list_unlock();
}


/**
 * \brief FTL����س�ʼ��
 *
 * \param[in] pool : ָ����ڴ�
 * \param[in] size : �ش�С
 *
 * \retval AW_OK      : ��ʼ���ɹ�
 * \retval -AW_EINVAL : ��������
 *
 * \note �ú�������aw_ftl_cfg.c�б�aw_ftl_lib_init����������
 */
aw_err_t aw_ftl_pool_init (struct aw_ftl_dev *pool, unsigned int size)
{
    __RETURN_IF(!pool || !size, -AW_EINVAL);

    if (__g_ftl_init) {
        return AW_OK;
    }

    aw_list_head_init(&__g_ftl_dev_list);

    aw_pool_init(&__g_ftl_dev_pool,
                 pool,
                 size,
                 sizeof(*pool));

    __g_ftl_init = AW_TRUE;

    return AW_OK;
}

/**
 * \brief ����һ��FTL�豸
 */
aw_err_t aw_ftl_dev_create (const char *p_ftl_path,
                            const char *p_mtd_path)
{
    int                  ret = AW_ERROR;
    struct aw_ftl_dev   *p_ftl;
    struct aw_mtd_info  *mtd;

    __RETURN_IF(!__g_ftl_init, -AW_ESRCH);
    __RETURN_IF(!p_ftl_path || !p_mtd_path, -AW_EINVAL);

    /* ����Ѿ��������򷵻�OK */
    p_ftl = __ftl_dev_get(p_ftl_path);
    if (p_ftl) {
        __ftl_dev_put(p_ftl, NULL);
        return AW_OK;
    }
    __RETURN_DETAIL_IF((p_ftl = __ftl_dev_alloc(p_ftl_path)) == NULL, -AW_ENOMEM,
                       "alloc ftl(%s) record pool failed", p_ftl_path);

    p_ftl->resource = 1;

    mtd = aw_mtd_dev_find(p_mtd_path);
    __RETURN_DETAIL_IF(!mtd, -AW_ENODEV, 
                       "fail to find the \"%s\" device.", p_mtd_path);

    ret = __ftl_dev_init(p_ftl, mtd);
    __WARN_DETAIL_IF(AW_OK != ret, "__ftl_dev_init failed(%d)", ret);
    if (AW_OK != ret) {
        __ftl_dev_free(p_ftl);
        return -AW_ENODEV;
    }

    __ftl_dev_register(p_ftl);

    AW_FTL_MSG(__MSG_MASK, "create the \"%s\" device.", p_ftl_path);

    return AW_OK;
}

aw_err_t aw_ftl_dev_destroy (const char *p_ftl_path)
{
    int    ret = AW_OK;
    struct aw_ftl_dev   *p_ftl;

    __RETURN_IF(!p_ftl_path, -AW_EINVAL);

    p_ftl = __ftl_dev_get(p_ftl_path);
    __RETURN_DETAIL_IF(!p_ftl, -AW_ENODEV, "not find the dev \"%s\"", p_ftl_path);

    /* ����ftl �豸��������ɾ��  */
    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
    __ftl_dev_unregister(p_ftl);
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    __ftl_dev_put(p_ftl, NULL);

    ret = __ftl_dev_put(p_ftl, __ftl_dev_destroy);

    return ret;
}

/**
 * \brief getһ��FTL�豸
 *
 * \param[in] p_ftl      : FTL�豸�ṹ��
 * \param[in] p_ftl_name : Ҫ������FTL�豸����
 * \param[in] p_mtd_name : Ҫʹ�õ�MTD�豸����
 *
 * \retval ��ʼ���ɹ�:���� ftl�豸ָ��
 */
void * aw_ftl_dev_get (const char *p_ftl_path)
{
    struct aw_ftl_dev   *p_ftl;

    __RETURN_IF(!p_ftl_path, NULL);

    p_ftl = __ftl_dev_get(p_ftl_path);
    __RETURN_DETAIL_IF(!p_ftl, NULL, "not find the dev \"%s\"", p_ftl_path);

    return (void *)p_ftl;
}


aw_err_t aw_ftl_dev_put (void *p_hd)
{
    struct aw_ftl_dev *p_ftl = (struct aw_ftl_dev *)p_hd;
    int ret = AW_OK;

    __RETURN_IF(NULL == p_ftl, -AW_EINVAL);

    ret = __ftl_dev_put(p_ftl, __ftl_dev_destroy);

    return ret;
}

/**
* tag:read
*/

/**
 * \brief �߼�������ȡ
 */
aw_err_t aw_ftl_sector_read (void              *p_hd,
                             uint32_t           lsn,
                             uint8_t           *data)
{
    struct aw_ftl_dev *p_ftl = p_hd;
    int ret = -AW_ERROR;
    uint16_t pbn, idx, lbn;

    __RETURN_IF(NULL == p_ftl || NULL == data, -AW_EINVAL);
    __RETURN_DETAIL_IF(lsn >= p_ftl->data_sector_total, -AW_ENOMEM,
                       "sector number(%d) out of range", lsn);

    if (lsn >= p_ftl->data_sector_total) {
        AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "sector number(%d) out of range", lsn);
        return -AW_ENOMEM;
    }

    lbn  = lsn / p_ftl->sectors_per_blk;
    idx  = lsn % p_ftl->sectors_per_blk;

    if (aw_ftl_logic_blk_is_empty(p_ftl, lbn) ||
        (!aw_ftl_logic_sector_be_used(p_ftl, lbn, idx)) ) {
        return -AW_ENODATA;
    }

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);

    pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

    if (!aw_ftl_logic_sector_is_dirty(p_ftl, lbn, idx)) {
        AW_FTL_MSG(AW_FTL_MSG_MASK_RD,
                "read  logic(lbn:%d-%d(%d), pbn:%d). ", lbn, idx, lsn, pbn);

        ret = aw_ftl_logic_sector_read(p_ftl, lbn, idx, data);
        if (ret != AW_OK) {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "ECC error or IO error(pbn:%d sn%d)", pbn, idx);

            /* ��ҳ��ʧ�ܣ��������д������ʱδд��Ͼ͵��磬��ʱ������������ */
            aw_ftl_logic_sector_dirty_tag_write(p_ftl, lbn, idx);
        }
    } else {
        /* �߼������������  �����־�������     */
        ret = aw_ftl_log_sector_read(p_ftl, lbn, idx, data);
        if (ret != AW_OK) {
            AW_FTL_MSG(AW_FTL_MSG_MASK_RD, "in log block not find the sector(%d) data", lsn);
        }
    }

    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    if (ret != AW_OK && ret != -AW_EIO) {
        ret = -AW_ENODATA;
    }
    return ret;
}


/**
 * tag:read
 */
aw_err_t aw_ftl_data_read  (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize)
{
    struct aw_ftl_dev           *p_ftl = p_hd;
    int                          ret = AW_OK;
    uint32_t                     lsn;
    uint8_t                     *p_lsn_data;
    uint32_t                     lsn_offs;
    uint32_t                     addr_offset;
    uint32_t                     data_offset;
    uint32_t                     remain;
    uint32_t                     len;
    uint32_t                     storage_size;
    __ftl_data_wr_status_t       status = __FTL_DATA_WR_STATUS_HEAD;

    __RETURN_IF(p_ftl == NULL, -AW_EINVAL);
    __RETURN_IF(data  == NULL, -AW_EINVAL);
    __RETURN_IF(dsize == 0, -AW_EINVAL);

    ret = aw_ftl_dev_ioctl(p_hd, AW_FTL_GET_STORAGE_SIZE, (void *)&storage_size);
    __RETURN_IF(ret != AW_OK, ret);
    __RETURN_IF(storage_size < start_addr + dsize, -AW_ENOMEM);

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);

    ret    = AW_OK;
    while (remain != 0) {
        switch (status) {
            case __FTL_DATA_WR_STATUS_HEAD:
                __ASSERT((start_addr >> p_ftl->log2_sector_size) == (start_addr / p_ftl->sector_size));

                addr_offset = start_addr;
                data_offset = 0;
                remain      = dsize;

                lsn         = start_addr >> p_ftl->log2_sector_size;
                lsn_offs    = start_addr % p_ftl->sector_size;

                if (lsn_offs != 0) {
                    ret = aw_ftl_sector_read(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_IF(ret != AW_OK, ret);

                    len = min(p_ftl->sector_size - lsn_offs, remain);
                    __WARN_IF(len > p_ftl->sector_size);

                    memcpy(data, p_ftl->buf + lsn_offs, len);

                    addr_offset += len;
                    data_offset += len;
                    remain      -= len;
                    __WARN_IF((addr_offset % p_ftl->sector_size) != 0);
                }
                status = __FTL_DATA_WR_STATUS_MIDDLE;
                break;

            case __FTL_DATA_WR_STATUS_MIDDLE:
                if (remain >= p_ftl->sector_size) {
                    lsn        = addr_offset >> p_ftl->log2_sector_size;
                    p_lsn_data = data + data_offset;

                    ret = aw_ftl_sector_read(p_hd, lsn, p_lsn_data);
                    __GOTO_EXIT_IF(ret != AW_OK, ret);

                    addr_offset += p_ftl->sector_size;
                    data_offset += p_ftl->sector_size;
                    remain      -= p_ftl->sector_size;
                } else {
                    status = __FTL_DATA_WR_STATUS_TAIL;
                }
                break;

            case __FTL_DATA_WR_STATUS_TAIL:
                if (remain != 0) {
                    lsn        = addr_offset >> p_ftl->log2_sector_size;
                    ret = aw_ftl_sector_read(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_IF(ret != AW_OK, ret);

                    len = remain;
                    memcpy(data + data_offset, p_ftl->buf, len);

                    addr_offset += len;
                    data_offset += len;
                    remain      -= len;
                    __WARN_IF(remain != 0);
                }
                break;

            default:
                __WARN_IF(status > __FTL_DATA_WR_STATUS_TAIL);
                break;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    if (ret != AW_OK && ret != -AW_ENODATA) {
        return ret;
    }
    return (dsize - remain);   /* ���ض����������� */
}

/* tag:write */
aw_err_t aw_ftl_data_write (void                *p_hd,
                            uint32_t             start_addr,
                            uint8_t             *data,
                            size_t               dsize)
{
    struct aw_ftl_dev           *p_ftl = p_hd;
    int                          ret = AW_OK;
    uint32_t                     lsn;
    uint8_t                     *p_lsn_data;
    uint32_t                     lsn_offs;
    uint32_t                     addr_offset;
    uint32_t                     data_offset;
    uint32_t                     remain;
    uint32_t                     len;
    uint32_t                     storage_size;
    __ftl_data_wr_status_t       status = __FTL_DATA_WR_STATUS_HEAD;

    __RETURN_IF(p_ftl == NULL, -AW_EINVAL);
    __RETURN_IF(data  == NULL, -AW_EINVAL);
    __RETURN_IF(dsize == 0, -AW_EINVAL);

    ret = aw_ftl_dev_ioctl(p_hd, AW_FTL_GET_STORAGE_SIZE, (void *)&storage_size);
    __RETURN_IF(ret != AW_OK, ret);
    __RETURN_IF(storage_size < start_addr + dsize, -AW_ENOMEM);

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);

    ret    = AW_OK;
    while (remain != 0) {
        switch (status) {
            case __FTL_DATA_WR_STATUS_HEAD:
                __ASSERT((start_addr >> p_ftl->log2_sector_size) == (start_addr / p_ftl->sector_size));

                addr_offset = start_addr;
                data_offset = 0;
                remain      = dsize;

                lsn         = start_addr >> p_ftl->log2_sector_size;
                lsn_offs    = start_addr % p_ftl->sector_size;

                if (lsn_offs != 0) {
                    ret = aw_ftl_sector_read(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_IF(ret != AW_OK && ret != -AW_ENODATA, ret);

                    len = min(p_ftl->sector_size - lsn_offs, remain);
                    __WARN_DETAIL_IF(len > p_ftl->sector_size,
                                     "len:%d p_ftl->sector_size:%d", len, p_ftl->sector_size);
                    memcpy(p_ftl->buf + lsn_offs, data, len);

                    ret = aw_ftl_sector_write(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_DETAIL_IF(ret != AW_OK, ret,
                                          "aw_ftl_sector_write(sector:%d)", lsn);

                    addr_offset += len;
                    data_offset += len;
                    remain      -= len;
                    __WARN_IF((addr_offset % p_ftl->sector_size) != 0);
                }
                status = __FTL_DATA_WR_STATUS_MIDDLE;
                break;

            case __FTL_DATA_WR_STATUS_MIDDLE:
                if (remain >= p_ftl->sector_size) {
                    lsn        = addr_offset >> p_ftl->log2_sector_size;
                    p_lsn_data = data + data_offset;

                    ret = aw_ftl_sector_write(p_hd, lsn, p_lsn_data);
                    __GOTO_EXIT_DETAIL_IF(ret != AW_OK, ret,
                                          "aw_ftl_sector_write(sector:%d)", lsn);

                    addr_offset += p_ftl->sector_size;
                    data_offset += p_ftl->sector_size;
                    remain      -= p_ftl->sector_size;
                } else {
                    status = __FTL_DATA_WR_STATUS_TAIL;
                }
                break;

            case __FTL_DATA_WR_STATUS_TAIL:
                if (remain != 0) {
                    lsn        = addr_offset >> p_ftl->log2_sector_size;
                    ret = aw_ftl_sector_read(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_IF(ret != AW_OK && ret != -AW_ENOMEM, ret);

                    len = remain;
                    memcpy(p_ftl->buf, data + data_offset, len);

                    ret = aw_ftl_sector_write(p_hd, lsn, p_ftl->buf);
                    __GOTO_EXIT_DETAIL_IF(ret != AW_OK, ret,
                                          "aw_ftl_sector_write(sector:%d)", lsn);

                    addr_offset += len;
                    data_offset += len;
                    remain      -= len;
                    __WARN_IF(remain != 0);
                }
                break;

            default:
                __WARN_IF(status > __FTL_DATA_WR_STATUS_TAIL);
                break;
        }
    }

exit:
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    if (ret != AW_OK) {
        return ret;
    }
    return (dsize - remain);  /* ����д��������� */
}

/* tag:write */

/**
 * \brief �߼�����д��
 */
aw_err_t aw_ftl_sector_write (void              *p_hd,
                              uint32_t           lsn,
                              uint8_t           *data)
{
    struct aw_ftl_dev   *p_ftl = p_hd;
    int                 ret = AW_OK;
    uint16_t            lbn, pbn, idx;
    struct aw_ftl_sector_tag  sec_tag;

    __RETURN_IF(NULL == p_ftl || NULL == data, -AW_EINVAL);
    __RETURN_DETAIL_IF(lsn >= p_ftl->data_sector_total, -AW_ENOMEM,
                       "sector number(%d) out of range", lsn);

    AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);

    lbn  = lsn / p_ftl->sectors_per_blk;
    idx  = lsn % p_ftl->sectors_per_blk;

    /* д���������ݣ�����Ҫ����ɵ�ϵͳ��Ϣ���� */
    aw_ftl_sys_info_clear(p_ftl);

__again:

    if (aw_ftl_logic_blk_is_empty(p_ftl, lbn)) {

        /* ����һ���������Ϊ���ݿ� */
        ret = aw_ftl_new_logic_blk_get(p_ftl, lbn);
        if (ret != AW_OK) {
            goto __exit;
        }

        pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

        /* ֱ�ӽ�����д������� */
        sec_tag.flag = AW_FTL_SECTOR_TAG_DATA;
        sec_tag.lbn  = 0xffff;
        sec_tag.sn   = 0xff;

        AW_FTL_MSG(AW_FTL_MSG_MASK_WD, "write logic(lbn:%d-%d(%d), pbn:%d). ", lbn, idx, lsn, pbn);

        ret = aw_ftl_logic_sector_write_with_tag(p_ftl, lbn, idx, data, &sec_tag);

        if (AW_OK != ret) {

            /* ��ҳΪ����д��ʧ�ܣ��������д������ʱδд��Ͼ͵��磬��ʱ������������ */
            aw_ftl_logic_sector_dirty_tag_write(p_ftl, lbn, idx);

            goto __again;
        }
    } else {

        pbn = p_ftl->logic_tbl[lbn].p_pyh_blk->pbn;

        if (aw_ftl_logic_sector_be_used(p_ftl, lbn, idx)) {
            /*
             * ���߼������Ѿ������� ��������������д�뵽log��������
             */
            ret = aw_ftl_log_sector_rewrite(p_ftl, lbn, idx, data);

        } else {
            /* ����������free�ģ�ֱ��д�����ݿ������� */
            sec_tag.flag = AW_FTL_SECTOR_TAG_DATA;
            sec_tag.lbn  = 0xffff;
            sec_tag.sn   = 0xff;

            AW_FTL_MSG(AW_FTL_MSG_MASK_WD,   "write logic(lbn:%d-%d(%d), pbn:%d). ",
                        lbn, idx, lsn, pbn);
            ret = aw_ftl_logic_sector_write_with_tag(p_ftl,
                                                     lbn,
                                                     idx,
                                                     data,
                                                     &sec_tag);
            if (AW_OK != ret) {
                AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "write logic failed( lbn:%d-%d(%d) ),"
                         " write new sector again. ",  lbn, idx, lsn);

                /* ��ҳΪ����д��ʧ�ܣ��������д������ʱδд��Ͼ͵��磬��ʱ������������ */
                aw_ftl_logic_sector_dirty_tag_write(p_ftl, lbn, idx);

                goto __again;
            }
        }
    }

__exit:
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

#if (AW_FTL_CONFIG_WRITE_VERIFY == 1)
    if (ret == AW_OK) {
        memset(p_ftl->buf, 0, p_ftl->sector_size);
        ret = aw_ftl_sector_read(p_ftl, lsn, p_ftl->buf);
        if (ret == AW_OK) {
            if (memcmp(p_ftl->buf, data, p_ftl->write_size) != 0) {
                ret = -AW_EBADMSG;
            }
        }

        if (ret != AW_OK) {
            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "write verify failed");
            aw_ftl_bad_node_add(p_ftl, p_ftl->logic_tbl[lbn].p_pyh_blk->pbn);
            p_ftl->logic_tbl[lbn].p_pyh_blk = NULL;
        }
    }
#endif

    if (ret != AW_OK) {
        ret = -AW_EAGAIN;
    }

    return ret;
}

/* tag:ioctl */

aw_err_t aw_ftl_dev_ioctl (void                *p_hd,
                           aw_ftl_ioctl_req_t   req,
                           void                *p_arg)
{
    struct aw_ftl_dev *p_ftl = p_hd;
    int               ret         = AW_OK;
    aw_ftl_sys_info_t *p_sys_info = &p_ftl->sys_info;
    int               lbn;
    int               blk;
    int               sector;
    aw_ftl_blk_info_t *p_pos;
    aw_ftl_blk_info_t *p_tmp;

    __RETURN_IF(!p_ftl, -AW_EINVAL);

    ret = AW_MUTEX_LOCK(p_ftl->op_mutex, AW_SEM_WAIT_FOREVER);
    switch (req) {

        case AW_FTL_SYS_INFO_UPDATE_TIME :

            p_sys_info->update_time = (uint32_t)p_arg;
            if (p_sys_info->need_update) {
                aw_ftl_defer_job_start(p_ftl, p_sys_info->update_time);
            }
            break;

        case AW_FTL_SYS_INFO_TARENEW :

            p_sys_info->time_auto_renew = (aw_bool_t)p_arg;

            break;

        case AW_FTL_GET_DEV_NAME :

            strncpy((char *)p_arg, p_ftl->name, __FTL_NAME_MAX);

            break;

        case AW_FTL_GET_SECTOR_COUNT :

            *(uint32_t *)p_arg = p_ftl->sectors_per_blk *
                                 p_ftl->ndata_blocks;

            break;

        case AW_FTL_GET_SECTOR_SIZE :

            *(uint32_t *)p_arg = p_ftl->sector_size;

            break;

        case AW_FTL_GET_STORAGE_SIZE :

            *(uint32_t *)p_arg = (p_ftl->sectors_per_blk * 
                                  p_ftl->ndata_blocks) *
                                  p_ftl->sector_size;

            break;

        case AW_FTL_LOGIC_THRESHOLD :

            p_ftl->logic_clear_threshold = (float)((uint32_t)p_arg) / 100;

            break;

        case AW_FTL_LOG_THRESHOLD :

            p_ftl->log_clear_threshold = (float)((uint32_t)p_arg) / 100;

            break;

        case AW_FTL_SECTOR_DAT_DEL :

            ret = __ftl_sector_dat_del(p_ftl, (uint32_t)p_arg);

            break;

        case AW_FTL_DEV_FORMAT :

            ret = 0;

            /* �������ݵ��߼�����ɾ��*/
            for (sector = 0; sector < p_ftl->data_sector_total; sector++) {
                __ftl_sector_dat_del(p_ftl, sector);
            }

            for (lbn = 0; lbn < p_ftl->ndata_blocks; lbn++) {
                if (p_ftl->logic_tbl[lbn].p_pyh_blk != NULL) {
                    if (!aw_list_empty(&p_ftl->logic_tbl[lbn].p_pyh_blk->node)) {
                        aw_list_del_init(&p_ftl->logic_tbl[lbn].p_pyh_blk->node);
                    }
                    aw_list_add(&p_ftl->logic_tbl[lbn].p_pyh_blk->node, &p_ftl->dirty_blk_list);
                    p_ftl->logic_tbl[lbn].p_pyh_blk = NULL;
                    ret++;
                }
            }

            for (blk = 0; blk < p_ftl->nlog_blocks; blk++) {
                /* �����־�黹δ��������п飬����� */
                if (p_ftl->log_tbl[blk].p_pyh_blk != NULL) {
                    if (!aw_list_empty(&p_ftl->log_tbl[blk].p_pyh_blk->node)) {
                        aw_list_del_init(&p_ftl->log_tbl[blk].p_pyh_blk->node);
                    }
                    aw_list_add(&p_ftl->log_tbl[blk].p_pyh_blk->node, &p_ftl->dirty_blk_list);
                    p_ftl->log_tbl[blk].p_pyh_blk = NULL;
                    ret++;
                }
            }

            p_sys_info = &p_ftl->sys_info;
            for (blk = 0; blk < p_sys_info->nsys_blk; blk++) {
                if (p_sys_info->blk_valid[blk]) {
                    p_pos = aw_ftl_pbn_info_get(p_ftl, p_sys_info->array[blk]);
                    if (!aw_list_empty(&p_pos->node)) {
                        aw_list_del_init(&p_pos->node);
                    }
                    aw_list_add(&p_pos->node, &p_ftl->dirty_blk_list);

                    p_sys_info->blk_valid[blk] = AW_FALSE;
                    p_sys_info->array[blk]     = 0xFFFF;
                    ret++;
                }
            }
            p_sys_info->need_update    = AW_TRUE;
            p_sys_info->pbn_valid      = AW_FALSE;
            p_sys_info->sector_use_cnt = 0;

            aw_list_for_each_entry_safe(p_pos,
                                        p_tmp,
                                        &p_ftl->free_blk_list,
                                        aw_ftl_blk_info_t,
                                        node) {
                aw_list_del_init(&p_pos->node);
                aw_list_add(&p_pos->node, &p_ftl->dirty_blk_list);
                ret++;
            }

            AW_FTL_MSG(__MSG_MASK, "all block wait format." 
                      "(ftl-name:%s, blk-num:%d)",
                      p_ftl->name , ret);

            if ((int)p_arg == 1) {
                while (aw_list_empty(&p_ftl->dirty_blk_list) == AW_FALSE) {
                    aw_ftl_dirty_blk_recover(p_ftl);
                }
                while (aw_list_empty(&p_ftl->bad_blk_list) == AW_FALSE) {
                    aw_ftl_bad_list_process(p_ftl);
                }
            }

            aw_ftl_markbad_list_process(p_ftl);

            ret = AW_OK;
            break;

        default :

            AW_FTL_ERR_MSG(AW_FTL_MSG_MASK_ERR, "the ioctl cmd not support");
            ret = -AW_ENOTSUP;
            break;
    }
    AW_MUTEX_UNLOCK(p_ftl->op_mutex);

    return ret;
}

/******************************************************************************/


/* end of file */
