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
 * \brief FLASH translation layer
 *
 * \internal
 * \par modification history:
 * - 1.00 17-04-15   vih, first implementation.
 * \endinternal
 */

#ifndef __AW_FTL_CORE_H
#define __AW_FTL_CORE_H

/**
 * \addtogroup grp_aw_if_ftl_core
 * \copydoc aw_ftl_core.h
 * @{
 */

#include "fs/aw_blk_dev.h"
#include "mtd/aw_mtd.h"
#include "aw_list.h"
#include "aw_sem.h"
#include "aw_delayed_work.h"
#include "aw_sys_defer.h"
#include "aw_refcnt.h"

#include "aw_ftl.h"
#include "aw_ftl_sys.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief �������Ϣͷ
 */
typedef struct aw_ftl_pb_hdr {

    /** \brief AWFTL */
    uint8_t  format_pattern[6];

    /** \brief ��־��0x0000��ʾ������������ṹ����Ч */
    uint16_t  flags;

    /** \brief ������������ */
    uint32_t wear_leveling_info;

    /** \brief �����Ѹ�ʽ���Ĵ�С */
    uint32_t volumn_size;

    /** \brief log2(SECTOR_SIZE)��SECTOR_SIZE����һ�������Ĵ�С */
    uint8_t  log2_sector_size;

    /** \brief log2(UNIT_SIZE)��UNIT_SIZE����һ��������Ԫ�Ĵ�С */
    uint8_t  log2_blk_size;

    /** \brief ��һ��������� */
    uint16_t first_pbn;

    /** \brief ��ʽ���������� */
    uint16_t nphy_blocks;

    /** \brief ���ݿ鵥Ԫ���� */
    uint16_t ndata_blocks;

    /** \brief ��־����� */
    uint16_t nlog_blocks;

    /** \brief ������������ */
    uint16_t nrsblocks;

} aw_ftl_pb_hdr_t;

/**
 * \brief �������Ϣ
 */
typedef struct aw_ftl_blk_info {

    /** \brief ����ڵ� */
    struct aw_list_head node;

    /** \brief ������Ϣ */
    uint32_t wear_leveling;

    /** \brief �߼����� */
    uint16_t lbn;

    /** \brief ������� */
    uint16_t pbn;

} aw_ftl_blk_info_t;


/** \brief ǰ������ */
struct aw_ftl_logic_blk_info;
struct aw_ftl_log_blk_info;

/**
 * \brief FTL�豸�ṹ��
 */
typedef struct aw_ftl_dev {

    /** \brief BLK�豸 */
    struct aw_blk_dev        bdev;

    /** \brief MTD�豸 */
    struct aw_mtd_info      *mtd;

    struct aw_ftl_sys_info   sys_info;

    struct aw_refcnt         ref;

    /** \brief ������ */
    struct aw_list_head      bad_blk_list;

    /** \brief ���п��� */
    struct aw_list_head      free_blk_list;

    /** \brief ����� */
    struct aw_list_head      dirty_blk_list;

    /** \brief ����ڵ� */
    struct aw_list_head      node;

    /** \brief ��ʱ�������(data)����  */
    struct aw_delayed_work   delay_work;

    struct aw_defer_djob     defer_job;

    /** \brief ���ڶ���д���������յĻ����� */
    AW_MUTEX_DECL(op_mutex);

#define __FTL_NAME_MAX     32
    char               name[__FTL_NAME_MAX];

    /** \brief ������������ʹ�� */
    aw_ftl_blk_info_t            *pbt;

    /** \brief �߼�����Ϣ���� */
    struct aw_ftl_logic_blk_info *logic_tbl;

    /** \brief ��־����Ϣ���� */
    struct aw_ftl_log_blk_info   *log_tbl;

    /** \brief ��ʱ���棬��СΪҳ��С */
    uint8_t *buf;

    /** \brief ��͵�ĥ����⣬������Ϣ��ʧʱʹ�ø�ĥ�����   */
    uint32_t lowest_wear;

    /** \brief ��ǰFTL�������ĸ��� */
    uint32_t sector_total;

    /** \brief �������������� */
    uint32_t data_sector_total;

    /** \brief ���ÿ������������� */
    uint32_t free_sector_total;

    /** \brief һ�������Ĵ�С */
    uint32_t sector_size;

    /** \brief ������Ԫ�Ĵ�С */
    uint32_t blk_size;

    /** \brief ��д��Ԫ�Ĵ�С */
    uint32_t write_size;

    uint32_t malloc_cnt;

    /** \brief �߼�������ķ�ֵ�����߼����dirty���������ﵽ�÷�ֵʱ����ϲ��ÿ����� */
    float    logic_clear_threshold;

    /** \brief ��־������ķ�ֵ����������־�������ʹ�������ﵽ�÷�ֵʱ����ϲ�������־������ */
    float    log_clear_threshold;

    /** \brief ��һ������鵥Ԫ��� */
    uint16_t first_pbn;

    /** \brief �߼����п��õ��������� */
    uint16_t sectors_per_blk;

    /** \brief ����鵥Ԫ���� */
    uint16_t nphy_blocks;

    /** \brief ���ݿ鵥Ԫ���� */
    uint16_t ndata_blocks;

    /** \brief ��־�鵥Ԫ���� */
    uint16_t nlog_blocks;

    /** \brief ������������ */
    uint16_t nrsblocks;

    /** \brief �����˲�����������������Ҫ�ָ����߼��� */
    uint16_t need_recovery_lbn;

    /** \brief ռ����Դ����   */
    int16_t  resource;

    /** \brief ������ecc���󣬵����ܾ����Ŀ飬��Ҫ���л�����   */
    int16_t  need_verify_blk;

    /** \brief physical block headerռ�õ��������� */
    uint8_t  sectors_per_blk_hdr;

    /** \brief log2(sector_size), sector_size����һ�������Ĵ�С */
    uint8_t  log2_sector_size;

    /** \brief log2(blk_size)��blk_size����һ��������Ԫ�Ĵ�С */
    uint8_t  log2_blk_size;

    /** \brief log2(write_size)��wirte_size����һ����д��Ԫ�Ĵ�С */
    uint8_t  log2_write_size;

    /** \brief �߼�����Ҫ�ָ���־��Ϊtrue��ʾ�����˲������������� */
    aw_bool_t   lbn_need_recovery;

    /** \brief blk ע���� */
    aw_bool_t   blk_reg;

} aw_ftl_dev_t;


/**
 * \brief FTL����س�ʼ��
 *
 * \param[in] pool : ָ����ڴ�
 * \param[in] size : �ش�С
 *
 * \return �����ɹ�����AW_OK, ���򷵻�aw_err_t�Ĵ�������
 *
 * \note �ú�������aw_ftl_cfg.c�б�aw_ftl_lib_init����������
 */
aw_err_t aw_ftl_pool_init (struct aw_ftl_dev *pool, unsigned int size);


/** @}  grp_aw_if_ftl_core */

#ifdef __cplusplus
}
#endif

#endif /* __AW_FTL_CORE_H */



/* end of file */
