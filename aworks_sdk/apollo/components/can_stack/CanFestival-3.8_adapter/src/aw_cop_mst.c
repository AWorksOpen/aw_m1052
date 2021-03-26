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
 * \brief CANfestival 适配 CANopen标准接口
 *
 * \par 源代码
 * \snippet demo_cop_mst.c src_cop_mst
 *
 * \internal
 * \par Modification History
 * - 1.00 18-11-07  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_cop_mst
 * \copydoc demo_cop_mst.c
 */

/** [src_cop_mst] */
#include "apollo.h"                   /* 此头文件必须被首先包含 */
#include "aw_can.h"
#include "aw_cop_mst.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_canfestival_adapter.h"
#include "aw_timer.h"
#include "aw_time.h"

static void __otimer_callback (void *p_arg)
{
    aw_bool_t *p_timeout = p_arg;

    *p_timeout = AW_TRUE;
}

static void __rpdo_update_callback (
    void *p_arg, uint16_t pdo_id, uint8_t *p_data, uint8_t len)
{
    aw_cop_pdo_msg_t msg;

    aw_cop_mst_pfn_pdo_recv_t pfn_pdo_recv_callback = 
        (aw_cop_mst_pfn_pdo_recv_t)p_arg;

    if (pfn_pdo_recv_callback != NULL) {
        memset(&msg, 0x00, sizeof(msg));
        msg.node_id = (pdo_id & 0x7F);
        msg.pdo_id = pdo_id;
        msg.pdo_len = len;
        memcpy(msg.pdo_data, p_data, len);
        pfn_pdo_recv_callback(&msg);
    }
}

aw_cop_err_t aw_cop_mst_init (uint8_t inst, aw_cop_mst_info_t *p_info)
{
    void         *p_inst_data = NULL;
    aw_can_err_t  ret;

    if (p_info == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    ret = aw_canfestival_inst_init(inst, p_info->baudrate);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_INITFAIL;
    }

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    setNodeId(p_inst_data, p_info->node_id);

    /* 该处由于canfestival 设置nodeid会重新初始化pdo的值，故需要全部设置为无效 */
    aw_canfestival_all_pdo_disable(inst, AW_TRUE);
    aw_canfestival_all_pdo_disable(inst, AW_FALSE);

    aw_canfestival_rpdo_update_callback_register(
        inst, __rpdo_update_callback, p_info->pfn_pdo_recv_callback);

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_deinit (uint8_t inst)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_deinit(inst);

    if (ret == AW_CAN_ERR_NONE) {
        return AW_COP_ERR_NO_ERROR;
    } else {
        return -AW_COP_ERR_NOTEXIST;
    }
}

aw_cop_err_t aw_cop_mst_start (uint8_t inst)
{
    void         *p_inst_data = NULL;
    aw_can_err_t  ret;
    int8_t        node_cnt, i;
    uint32_t      exp_size;
    uint32_t      local_time;
    uint16_t      time;
    uint8_t       cnt;

    aw_cop_mst_node_info_t node_info;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    ret = aw_canfestival_inst_start(inst);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_STARTDEV;
    }

    /* init */
    setState(p_inst_data, Initialisation);

    /* Put the master in operational mode */
    setState(p_inst_data, Operational);

    aw_mdelay(500);

    masterSendNMTstateChange(p_inst_data, 0, NMT_Start_Node);

    node_cnt = aw_canfestival_inst_node_cnt(inst);
    for (i = 0; i < node_cnt; i++) {
        ret = aw_canfestival_inst_node_info_get(inst,
                                                i,
                                                &node_info.node_id,
                                                &node_info.check_mode,
                                                &node_info.check_period,
                                                &node_info.retry_factor);

        if (ret != AW_CAN_ERR_NONE) {
            continue;
        }

        if (node_info.node_id == 0) {
            continue;
        }

        if (node_info.check_mode == AW_COP_MST_NODE_CHECK_WAY_GUARDING) {
            time = node_info.check_period;
            cnt = node_info.retry_factor;

            exp_size = sizeof(time);
            writeLocalDict(p_inst_data,
                           0x100c,
                           0,
                           &time,
                           &exp_size,
                           RW);

            exp_size = sizeof(cnt);
            writeLocalDict(p_inst_data,
                           0x100d,
                           0,
                           &cnt,
                           &exp_size,
                           RW);

            time = node_info.check_period + 100;
            cnt = node_info.retry_factor;

            aw_cop_mst_sdo_dwonload(inst,
                node_info.node_id,
                0x100c,
                0,
                (uint8_t *)&time,
                sizeof(uint16_t),
                1000);

            aw_cop_mst_sdo_dwonload(inst,
                node_info.node_id,
                0x100d,
                0,
                &cnt,
                sizeof(uint8_t),
                1000);

        } else if (
            node_info.check_mode == AW_COP_MST_NODE_CHECK_WAY_HEARTBEAT) {
            time = node_info.check_period * 9 / 10;
            local_time =
                node_info.check_period | ((uint32_t)node_info.node_id << 16);

            aw_cop_mst_sdo_dwonload(inst,
                node_info.node_id,
                0x1017,
                0,
                (uint8_t *)&time,
                sizeof(uint16_t),
                1000);

            exp_size = sizeof(local_time);
            writeLocalDict(p_inst_data,
                           0x1016,
                           i + 1,
                           &local_time,
                           &exp_size,
                           RW);
        }
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_stop (uint8_t inst)
{
    aw_can_err_t ret;
    void         *p_inst_data = NULL;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    setState(p_inst_data, Stopped);

    ret = aw_canfestival_inst_stop(inst);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_NOTSTARTED;
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_add_node (uint8_t inst, aw_cop_mst_node_info_t *p_info)
{
    aw_can_err_t  ret;

    if (p_info == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    ret = aw_canfestival_inst_node_add(inst,
                                       p_info->node_id,
                                       p_info->check_mode,
                                       p_info->check_period,
                                       p_info->retry_factor);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_SLAVETABFUL;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_remove_node (uint8_t inst, uint8_t node_id)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_node_remove(inst, node_id);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_SLAVENOTEXIST;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_input_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_pdo_add(inst, pdo_id, AW_TRUE);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_PDOTABFULL;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_input_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_pdo_remove(inst, pdo_id, AW_TRUE);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_PDONOTREGISTER;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_output_pdo_install (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_pdo_add(inst, pdo_id, AW_FALSE);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_PDOTABFULL;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_output_pdo_remove (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    aw_can_err_t  ret;

    ret = aw_canfestival_inst_pdo_remove(inst, pdo_id, AW_FALSE);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_PDONOTREGISTER;
    } else {
        return AW_COP_ERR_NO_ERROR;
    }
}

aw_cop_err_t aw_cop_mst_sdo_upload (uint8_t   inst,
                                    uint8_t   node_id,
                                    uint16_t  index,
                                    uint8_t   subindex,
                                    uint8_t  *p_data,
                                    uint32_t *p_length,
                                    uint32_t  wait_time)
{
    void               *p_inst_data = NULL;
    aw_cop_err_t        ret;
    uint32_t            abort_code = 0;
    aw_timer_t          otimer;
    volatile aw_bool_t  timeout = AW_FALSE;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    ret = readNetworkDict(p_inst_data,
                          node_id,
                          index,
                          subindex,
                          0,
                          0);
    if (ret != 0) {
        return -AW_COP_ERR_SENDFAILED;
    }

    aw_timer_init(&otimer, __otimer_callback, (void *)&timeout);

    aw_timer_start(&otimer, aw_ms_to_ticks(wait_time));

    AW_FOREVER {
        ret = getReadResultNetworkDict(p_inst_data,
                                       node_id,
                                       p_data,
                                       p_length,
                                       &abort_code);
        if (ret != SDO_UPLOAD_IN_PROGRESS) {
            if (ret == SDO_FINISHED) {
                ret = AW_COP_ERR_NO_ERROR;
            } else {
                ret = -AW_COP_ERR_SDOABORT;
            }
            break;
        }

        if (timeout ) {
            ret = -AW_COP_ERR_TIMEOUT;
            break;
        }

        aw_mdelay(1);
    }

    aw_timer_stop(&otimer);
    closeSDOtransfer(p_inst_data, node_id, SDO_CLIENT);

    return ret;
}

aw_cop_err_t aw_cop_mst_sdo_dwonload (uint8_t    inst,
                                      uint8_t    node_id,
                                      uint16_t   index,
                                      uint8_t    subindex,
                                      uint8_t   *p_data,
                                      uint32_t   length,
                                      uint32_t   wait_time)
{
    void               *p_inst_data = NULL;
    aw_cop_err_t        ret;
    uint32_t            abort_code;
    aw_timer_t          otimer;
    volatile aw_bool_t  timeout = AW_FALSE;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    ret = writeNetworkDict(p_inst_data,
                           node_id,
                           index,
                           subindex,
                           length,
                           uint8,
                           p_data,
                           0);

    if (ret != 0) {
        return -AW_COP_ERR_SENDFAILED;
    }

    aw_timer_init(&otimer, __otimer_callback, (void *)&timeout);

    aw_timer_start(&otimer, aw_ms_to_ticks(wait_time));

    AW_FOREVER {
        ret = getWriteResultNetworkDict(p_inst_data, node_id, &abort_code);
        if (ret != SDO_DOWNLOAD_IN_PROGRESS) {
            if (ret == SDO_FINISHED) {
                ret = AW_COP_ERR_NO_ERROR;
            } else {
                ret = -AW_COP_ERR_SDOABORT;
            }
            break;
        }

        if (timeout ) {
            ret = -AW_COP_ERR_TIMEOUT;
            break;
        }

        aw_mdelay(1);
    }

    aw_timer_stop(&otimer);
    closeSDOtransfer(p_inst_data, node_id, SDO_CLIENT);

    return ret;
}

aw_cop_err_t aw_cop_mst_input_pdo_get (uint8_t   inst,
                                       uint8_t   node_id,
                                       uint32_t  pdo_id,
                                       uint8_t  *p_data,
                                       uint32_t *p_length,
                                       uint32_t  wait_time)
{
    aw_cop_err_t ret;
    int          pdo_num;

    if (p_data == NULL || p_length == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    ret = aw_canfestival_pdo_num_get(inst, pdo_id, AW_TRUE, &pdo_num);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_NOTEXIST;
    }

    ret = aw_canfestival_rpdo_rngbuf_data_get(
        inst, pdo_num, p_data, p_length, wait_time);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_BUFFERTOOSMALL;
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_output_pdo_set (uint8_t   inst,
                                        uint8_t   node_id,
                                        uint32_t  pdo_id,
                                        uint8_t  *p_data,
                                        uint8_t   length)
{
    aw_cop_err_t ret;
    int          pdo_num;

    if (p_data == NULL) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    ret = aw_canfestival_pdo_num_get(inst, pdo_id, AW_FALSE, &pdo_num);
    if (ret != AW_CAN_ERR_NONE) {
        return -AW_COP_ERR_NOTEXIST;
    }

    ret = aw_canfestival_pdo_data_set(inst, pdo_num, AW_FALSE, p_data, length);
    if (ret != AW_CAN_ERR_NONE) {

        if (ret == -AW_CAN_ERR_ABORT_TRANSMITTED) {
            return -AW_COP_ERR_SENDFAILED;
        }
        return -AW_COP_ERR_NOTEXIST;
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_remote_pdo_send (
    uint8_t inst, uint8_t node_id, uint32_t pdo_id)
{
    aw_cop_err_t  ret;
    int           pdo_num;
    CO_Data      *p_cop_data = NULL;

    p_cop_data = aw_canfestival_inst_data_get(inst);

    if (p_cop_data == NULL) {
        ret = -AW_CAN_ERR_NO_DEV;
        goto exit;
    }

    ret = aw_canfestival_pdo_num_get(inst, pdo_id, AW_TRUE, &pdo_num);
    if (ret != AW_CAN_ERR_NONE) {
        ret = -AW_COP_ERR_NOTEXIST;
        goto exit;
    }

    pdo_num += 0x1400;

    ret = sendPDOrequest(p_cop_data, pdo_num);
    if (ret == 0) {
        ret = AW_COP_ERR_NO_ERROR;
    } else {
        ret = -AW_COP_ERR_SENDFAILED;
    }

exit:
    return ret;
}

aw_cop_err_t aw_cop_mst_node_status_set (
    uint8_t inst, uint8_t node_id, aw_cop_mst_node_status_cmd_t status)
{
    void *p_inst_data = NULL;
    int   ret;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    ret = masterSendNMTstateChange(p_inst_data, node_id, status);
    if (ret != 0) {
        return -AW_COP_ERR_SENDFAILED;
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_node_status_get (
    uint8_t inst, uint8_t node_id, aw_cop_mst_node_status_t *p_status)
{
    void *p_inst_data = NULL;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    *p_status = getNodeState(p_inst_data, node_id);

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_sync_prodcer_cfg (
    uint8_t inst, uint32_t sync_id, uint16_t cyc_time)
{
    void     *p_inst_data = NULL;
    uint32_t  ret;
    uint16_t  sync_cob_id_index = 0x1005;
    uint16_t  sync_period_index = 0x1006;
    uint32_t  exp_size;
    uint32_t  period_us;

    p_inst_data = aw_canfestival_inst_data_get(inst);
    if (p_inst_data == NULL) {
        return -AW_COP_ERR_INVALIDHANDLE;
    }

    if (AW_COP_ID_SYNC_CLEAR == sync_id) {
        cyc_time = 0;
    }

    sync_id |= 0x40000000ul;
    exp_size = sizeof(sync_id);
    ret = writeLocalDict(p_inst_data,
                         sync_cob_id_index,
                         0,
                         &sync_id,
                         &exp_size,
                         RW);
    if (ret != OD_SUCCESSFUL) {
        return -AW_COP_ERR_INITFAIL;
    }

    period_us = cyc_time * 1000;
    exp_size = sizeof(period_us);
    ret = writeLocalDict(p_inst_data,
                         sync_period_index,
                         0,
                         &period_us,
                         &exp_size,
                         RW);
    if (ret != OD_SUCCESSFUL) {
        return -AW_COP_ERR_INITFAIL;
    }

    return AW_COP_ERR_NO_ERROR;
}

aw_cop_err_t aw_cop_mst_timestamp_send (uint8_t inst, aw_cop_time_t *p_time)
{
    aw_tm_t   since_tm;
    aw_time_t since_time;

    aw_tm_t   tm;
    aw_time_t time;

    aw_can_std_msg_t msg;
    aw_can_err_t     ret;

    uint32_t time_div;
    uint16_t day_div;

    if((p_time->year        < 1984) ||
       (p_time->hour        > 23)   ||
       (p_time->minute      > 59)   ||
       (p_time->second      > 59)   ||
       (p_time->millisecond > 999)) {
        return -AW_COP_ERR_INVALIDPARAM;
    }

    /* since_time 1984-1-1 */
    memset(&since_tm, 0x00, sizeof(since_tm));
    since_tm.tm_year = 1984 - 1900;
    since_tm.tm_mon = 1 - 1;
    since_tm.tm_mday = 1;
    aw_tm_to_time(&since_tm, &since_time);

    /* now time */
    memset(&tm, 0x00, sizeof(tm));
    tm.tm_year = p_time->year - 1900;
    tm.tm_mon = p_time->month - 1;
    tm.tm_mday = p_time->day;
    aw_tm_to_time(&tm, &time);

    memset(&msg, 0x00, sizeof(msg));
    time_div = p_time->hour * 3600000 +
               p_time->minute * 60000 +
               p_time->second * 1000 +
               p_time->millisecond;
    time_div &= 0x0FFFFFFF;
    memcpy(&msg.msgbuff[0], &time_div, sizeof(time_div));

    day_div = (time - since_time) / 86400;
    memcpy(&msg.msgbuff[sizeof(uint32_t)], &day_div, sizeof(day_div));

    msg.can_msg.id = AW_COP_ID_TIME_STAMP;
    msg.can_msg.length = 6;

    ret = aw_canfestival_msg_send(inst, &msg);
    if (ret == AW_CAN_ERR_NONE) {
        return AW_COP_ERR_NO_ERROR;
    } else {
        return -AW_COP_ERR_SENDFAILED;
    }
}

void aw_cop_mst_process (uint8_t inst)
{
    aw_mdelay(1000);
}

/** [src_cop_mst] */

/* end of file */
