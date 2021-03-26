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
 * \brief 抽象扫描器类实现
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 14-12-04  ops, first implementation.
 * \endinternal
 */

#include "awbl_scanner.h"


aw_local void __scanner_run (void *p_arg)
{
    struct awbl_scanner_dev       *p_scan = (struct awbl_scanner_dev *)p_arg;
    struct awbl_scan_cb_handler   *p_cur = NULL;
    struct aw_list_head           *p_pos = NULL;

    if (p_scan->pause_time == 0) {

        if (p_scan->scan_index >= p_scan->p_par->scan_cnt) {

            p_scan->scan_index = 0;
        }

        aw_list_for_each(p_pos, &p_scan->cb_list_head) {

            p_cur = aw_list_entry(p_pos, struct awbl_scan_cb_handler, list_elm);

            p_cur->pfn_scan_cb(p_arg);
        }

        ++p_scan->scan_index;

        aw_delayed_work_start(&p_scan->work, p_scan->p_par->scan_interval);

    }else {

        p_scan->pause_time = 0;
    }
}

aw_local void __scanner_start(struct awbl_scanner_dev *p_scan)
{
    aw_delayed_work_start(&p_scan->work, p_scan->p_par->scan_interval);
}

aw_local void __scanner_pause(struct awbl_scanner_dev *p_scan,
                              uint16_t                 pause_time)
{
    aw_delayed_work_stop(&p_scan->work);

    p_scan->pause_time = pause_time;

    aw_delayed_work_start(&p_scan->work, p_scan->pause_time);
}

aw_local void __scanner_stop(struct awbl_scanner_dev *p_scan)
{
    aw_delayed_work_stop(&p_scan->work);
}

void awbl_scanner_start(struct awbl_scanner_dev *p_scan)
{
    p_scan->pfn_scanner_start(p_scan);
}

void awbl_scanner_pause(struct awbl_scanner_dev *p_scan, uint16_t pause_time)
{
    p_scan->pfn_scanner_pause(p_scan, pause_time);
}

void awbl_scanner_stop(struct awbl_scanner_dev *p_scan)
{
    p_scan->pfn_scanner_stop(p_scan);
}

void awbl_scanner_dev_ctor(struct awbl_scanner_dev   *p_scan, 
                           struct awbl_scanner_param *p_par)
{

    p_scan->p_par          = p_par;
    p_scan->scan_index     = 0;
    p_scan->scan_enable    = AW_TRUE;

    p_scan->pause_time     = 0;

    p_scan->pfn_scanner_start = __scanner_start;
    p_scan->pfn_scanner_pause = __scanner_pause;
    p_scan->pfn_scanner_stop  = __scanner_stop;

    aw_list_head_init(&p_scan->cb_list_head);

    aw_delayed_work_init(&p_scan->work, __scanner_run, (void *)p_scan);
}

/* end of file */

