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
 * \brief sim800 GPRS模块驱动
 *
 * \par 使用示例
 * \code
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.07 17-12-09  vih, redefine the architecture
 * - 1.06 15-08-26  afm, support internal protocol for sim800.
 * - 1.05 15-08-07  afm, support hex sms for sim800.
 * - 1.04 15-05-13  afm, support chinese sms for sim800.
 * - 1.03 15-03-06  ops, add sms if for sim800.
 * - 1.02 15-01-23  ops, add signal strength if.
 * - 1.01 15-01-09  ops, redefine the architecture of sim800 driver.
 * - 1.00 14-12-15  afm, first implementation.
 * \endinternal
 */

#include "aworks.h"
#include "driver/gprs/awbl_gprs_sim800.h"
#include "driver/gprs/gb2312_unicode_table.h"
#include "aw_gpio.h"
#include "aw_serial.h"
#include "aw_ioctl.h"
#include "aw_delay.h"
#include "aw_vdebug.h"

#include "lwip/err.h"
#include "lwip/dns.h"
#include "lwip/inet.h"

#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "awbl_plb.h"

#define ON   "1"
#define OFF  "0"

#define __SIM800_IP_TASK_STK_SIZE    2048
#define __SIM800_IP_TASK_PRIO        7
AW_TASK_DECL_STATIC(__sim800_ip_task, __SIM800_IP_TASK_STK_SIZE);


#define __SIM800_DEV_DECL(p_this, p_dev) \
        struct awbl_gprs_sim800_dev *p_this = \
                AW_CONTAINER_OF(p_dev, struct awbl_gprs_sim800_dev, dev)

#define __SIM800_DEVINFO_DECL(p_devinfo, p_dev) \
        struct awbl_gprs_sim800_devinfo *p_devinfo = \
                ((struct awbl_gprs_sim800_devinfo *)AWBL_DEVINFO_GET(p_dev))

#define __SIM800_DECL_FROM_GPRS(p_this, p_gprs) \
        struct awbl_gprs_sim800_dev *p_this = \
                (struct awbl_gprs_sim800_dev *)p_gprs->p_gprs_dev

#define __ITERATE(EXP, COND, ntrys, ms_delay)   \
{\
    uint8_t __ntrys = ntrys;\
    while (__ntrys) {\
        EXP;\
        if (COND) {\
            break;\
        } else {\
            __ntrys--;\
            aw_mdelay(ms_delay);\
        }\
    }\
}


/** \brief GPRS模块操作结果  */
#define AW_GPRS_NO_SIM          (-2)    /**< \brief 找不到SIM卡  */
#define AW_GPRS_CSQ_LOW         (-3)    /**< \brief 信号强度弱  */
#define AW_GPRS_REG_FAIL        (-4)    /**< \brief 注册失败  */
#define AW_GPRS_NO_RSP          (-5)    /**< \brief 模块无响应  */
#define AW_GPRS_RSP_ERR         (-6)    /**< \brief 模块应答错误  */
#define AW_GPRS_PARA_ERR        (-7)    /**< \brief 模块参数设置错误  */


/** \brief 内部协议栈发送数据结构  */
struct awbl_gprs_snd_dat_struct
{
    uint8_t *p_data;
    size_t   len;
    uint32_t timeout;
};

/** \brief GPRS AT命令结构  */
struct awbl_gprs_at_cmd_struct
{
    char    *p_at;
    uint32_t timeout;
};

aw_local aw_err_t __gprs_sim800_init (struct awbl_gprs_sim800_dev  *p_sim);
aw_local aw_err_t __gprs_sim800_fn_init(struct awbl_gprs_dev *p_this);

#ifdef GPRS_INTERNAL_PROTOCOL
aw_local aw_err_t awbl_gprs_sim800_ip_close(struct awbl_gprs_dev *p_this, int hd);
#endif

aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc;


#ifdef GPRS_INTERNAL_PROTOCOL
/** \brief 处理GPRS内部协议栈接收数据  */
void awbl_gprs_process_rcv_data(struct awbl_gprs_sim800_dev *p_sim)
{
    uint16_t i;
    int32_t  len = 0;
    const char rvc_head[] = "+IPD,";
    char rcv_dat;
    uint8_t head_flag;
    const uint8_t mask[] = {0x01, 0x02, 0x04, 0x08, 0x10};
    char rcv_len[6] = {0};
    uint32_t read_len;
    uint32_t timeout;

    head_flag = 0;
    while (1) {
        read_len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, &rcv_dat, 1);
        if (read_len == 0) {
            break;
        }
        for (i=0; i<strlen(rvc_head); i++) {
            if (rcv_dat == rvc_head[i]) {
                head_flag |= mask[i];
                head_flag &= ~(0xFF << (i+1));
                break;
            }
        }
        if (head_flag == 0x1F) {
            break;
        }
    }
    if (head_flag == 0x1F) {
        head_flag = 0;
        memset(rcv_len, 0, sizeof(rcv_len));
        for (i=0; i<5; i++) {
            read_len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, &rcv_dat, 1);
            if (read_len == 0) {
                continue;
            }
            if (rcv_dat != ':') {
                rcv_len[i] = rcv_dat;
            } else {
                break;
            }
        }
        if (i >= 5) {
            return;
        }

        len = atoi(rcv_len);
        if (len > GPRS_RSP_BUF_LEN) {
            len = GPRS_RSP_BUF_LEN;
        }
        timeout = 0;
        while (len) {
            if ((read_len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, 
                                           p_sim->gprs_rcv_dat_buf, 
                                           len)) > 0) {
                timeout = 0;
                aw_rngbuf_put(&p_sim->gprs_rcv_rngbuf, 
                               p_sim->gprs_rcv_dat_buf, 
                               read_len);
                p_sim->gprs.ppp.sio_mgn.flow_statistic_download += len;
                len -= read_len;
            } else {
                if (++timeout >= 100) {
                    break;
                }
            }
        }
    }
}
#endif

/** \brief GPRS发送命令  */
aw_local aw_bool_t __gprs_sim800_cmd_send_low(
    struct awbl_gprs_sim800_dev *p_sim,
    const char                  *p_cmd,
    const char                  *p_rsp,
    uint16_t                     timeout)
{
    uint16_t i;
    int32_t  len = 0, index = 0;
    aw_bool_t   flag = AW_FALSE;
#ifdef GPRS_INTERNAL_PROTOCOL
    char    *p_char;
    const char rvc_head[] = "+IPD,";
#endif

#ifdef GPRS_INTERNAL_PROTOCOL
    while (1) {
        aw_serial_ioctl((int)p_sim->gprs.ppp.sio_mgn.fd, AW_FIONREAD, &len);
        if (len > 0) {
            awbl_gprs_process_rcv_data(p_sim);
        } else {
            break;
        }
    }
#endif

    aw_serial_ioctl((int)p_sim->gprs.ppp.sio_mgn.fd, AW_FIOFLUSH, NULL);

    if (p_cmd != NULL) {

        aw_serial_write((int)p_sim->gprs.ppp.sio_mgn.fd, p_cmd, strlen(p_cmd));
    }

    if (p_rsp == NULL) {            /* 应答无返回值   */
        return AW_TRUE;
    }

    for (i = 0; i < timeout; i++) {
        len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, 
                             (char *)p_rsp, GPRS_RSP_BUF_LEN - 1);
        if (len > 0) {
            index = len;
            flag = AW_TRUE;
            break;
        }
    }
    if ( !flag ) {
        return AW_FALSE;
    }

    for (i = 0; i < 10; i++) {            /* 应答帧间隔为100ms，应答完成   */
        len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, 
                             (char *)&p_rsp[index],
                             GPRS_RSP_BUF_LEN - 1 - index);
        if (len > 0) {
            index += len;
            i = 0;
            continue;
        }
    }

#ifdef GPRS_INTERNAL_PROTOCOL
    p_char = (char *)p_rsp;
    while (1) {
        p_char = strstr(p_char, rvc_head);
        if (p_char == NULL) {
            break;
        }
        p_char += 5;
        len = atoi(p_char);
        p_char = strchr(p_char, ':');
        if (p_char != NULL) {
            p_char += 1;
            if (index - (p_char-p_rsp) < len) {
                len = index - (p_char-p_rsp);
                aw_rngbuf_put(&p_sim->gprs_rcv_rngbuf, p_char, len);
                p_sim->gprs.ppp.sio_mgn.flow_statistic_download += len;
                break;
            } else {
                aw_rngbuf_put(&p_sim->gprs_rcv_rngbuf, p_char, len);
                p_sim->gprs.ppp.sio_mgn.flow_statistic_download += len;
                p_char += len;
            }
        }
    }
#endif

    return AW_TRUE;
}

/** \brief GPRS发送命令  */
aw_local aw_bool_t __gprs_sim800_cmd_send(
    struct awbl_gprs_sim800_dev *p_sim,
    const char                  *p_cmd,
    const char                  *p_rsp,
    uint16_t                     timeout)
{
    struct awbl_gprs_at_cmd_struct at_msg;
    uint32_t result;

    at_msg.p_at    = (char *)p_cmd;
    at_msg.timeout = timeout;
    AW_MAILBOX_SEND(p_sim->gprs_at_cmd_mailbox, 
                    (uint32_t )&at_msg, 
                    AW_SEM_WAIT_FOREVER,
                    0);
    if (AW_MAILBOX_RECV(p_sim->gprs_at_rsp_mailbox, 
                       (uint32_t *)&result, 
                       AW_SEM_WAIT_FOREVER) == AW_OK) {
        return result;
    }
    return AW_FALSE;
}

/** \brief 关回显   */
aw_local aw_err_t __gprs_sim800_echo(struct awbl_gprs_sim800_dev *p_sim,
                                     char                        *p_echo)
{
    uint8_t i = 0;

    sprintf(p_sim->at_cmd_buf, "ATE%s\r", p_echo);
    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   50) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
    }

    return AW_GPRS_NO_RSP;
}

/** \brief 模块退出命令模式, 返回数据模式   */
aw_local aw_err_t __gprs_sim800_dat_mode_enter(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "ATO\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "CONNECT") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 模块退出数据模式 , 返回命令模式  */
aw_local int __gprs_sim800_cmd_mode_enter(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;

    for (i = 0; i < 5; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "+++", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取CCID   */
aw_local int __gprs_sim800_ccid_get(struct awbl_gprs_sim800_dev *p_sim,
                                    char                        *p_ccid,
                                    uint8_t                      nbytes)
{
    uint8_t i = 0;
    char *p_start = NULL, *p_end = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CCID\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_start = strchr(p_sim->at_rsp_buf, '\r');
                if (p_start != NULL) {
                    p_start += 2;
                    p_end = p_start;
                    p_end = strchr(p_end, '\r');

                    if (p_end != NULL) {
                        *p_end = 0;

                        /* 模块有可能获取CCID失败, 只返回OK */
                        if (strstr(p_start, "OK") != NULL) {
                            return AW_GPRS_RSP_ERR;
                        }

                        strncpy(p_ccid, p_start, nbytes);
                        return AW_OK;
                    }
                }
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取模块版本号   */
aw_local int __gprs_sim800_revision_get(struct awbl_gprs_sim800_dev *p_sim,
                                        char                        *p_revision,
                                        uint8_t                      nbytes)
{
    uint8_t i = 0;
    char *p_start = NULL, *p_end = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+GMR\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_start = strchr(p_sim->at_rsp_buf, ':');
                if (p_start != NULL) {
                    p_start += 1;
                    p_end = p_start;
                    p_end = strchr(p_end, '\r');

                    if (p_end != NULL) {
                        *p_end = 0;
                        strncpy(p_revision, p_start, nbytes);
                        return AW_OK;
                    }
                }
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取模块IMEI号   */
aw_local int __gprs_sim800_imei_get(struct awbl_gprs_sim800_dev *p_sim,
                                    char                        *p_imei,
                                    uint8_t                      nbytes)
{
    uint8_t i = 0;
    char *p_start = NULL, *p_end = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+GSN\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_start = p_sim->at_rsp_buf + 2;
                p_end = p_start;
                p_end = strchr(p_end, '\r');
                if (p_end != NULL) {
                    *p_end = 0;
                    strncpy(p_imei, p_start, nbytes);
                    return AW_OK;
                }
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取模块IMSI号   */
aw_local int __gprs_sim800_imsi_get(struct awbl_gprs_sim800_dev *p_sim,
                                    char                        *p_imsi,
                                    uint8_t                      nbytes)
{
    uint8_t i = 0;
    char *p_start = NULL, *p_end = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CIMI\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_start = p_sim->at_rsp_buf + 2;
                p_end = p_start;
                p_end = strchr(p_end, '\r');
                if (p_end != NULL) {
                    *p_end = 0;
                    strncpy(p_imsi, p_start, nbytes);
                    return AW_OK;
                }
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取模块短信息中心号码   */
aw_local int __gprs_sim800_sca_get(struct awbl_gprs_sim800_dev *p_sim,
                                   char                        *p_sca,
                                   uint8_t                      nbytes)
{
    uint8_t i = 0;
    char *p_start = NULL, *p_end = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CSCA?\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_start = strchr(p_sim->at_rsp_buf, ':');
                if (p_start != NULL) {
                    p_start += 3;
                    p_end = p_start;
                    p_end = strchr(p_end, '"');
                    if (p_end != NULL) {
                        *p_end = 0;
                        strncpy(p_sca, p_start, nbytes);
                        return AW_OK;
                    }
                }
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 检查SIM卡   */
aw_local int __gprs_sim800_sim_card_check(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i;

    for (i = 0; i < 10; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CPIN?\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "READY") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(1000);
    }

    return AW_GPRS_NO_SIM;
}

/** \brief 设置不自动应答电话   */
aw_local int __gprs_sim800_not_auto_answer_set(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;

    for (i = 0; i < 30; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "ATS0=0\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 设置模块功能   */
aw_local int __gprs_sim800_fun_set(struct awbl_gprs_sim800_dev *p_sim, uint8_t fun)
{
    uint8_t i = 0;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CFUN=1\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 模块拨打数据业务电话   */
aw_local int __gprs_sim800_dial_up(struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i;

    for (i = 0; i < 10; i++) {
        sprintf(p_sim->at_cmd_buf, "ATD%s\r", p_sim->gprs.p_info->p_dial_num);
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   500) ) {
            if (strstr(p_sim->at_rsp_buf, "CONNECT") != NULL) {
                p_sim->online = AW_TRUE;
                return AW_OK;
            }
            if (strstr(p_sim->at_rsp_buf, "NO CARRIER") != NULL) {
                aw_mdelay(1000);
                continue;
            }
            break;
        }
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 模块挂断电话   */
aw_local int __gprs_sim800_dial_down(struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "ATH\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                p_sim->online = AW_FALSE;
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取信号强度   */
aw_local int __gprs_sim800_csq_get(struct awbl_gprs_sim800_dev *p_sim, 
                                   uint8_t                     *p_csq)
{
    uint8_t i = 0;
    char *p_char = NULL;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CSQ\r", p_sim->at_rsp_buf, 200) ) {
            p_char = strstr(p_sim->at_rsp_buf, "+CSQ:");
            if (p_char != NULL) {
                *p_csq = atoi(&p_char[6]);
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 获取注册信息   */
aw_local int __gprs_sim800_reg_get(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;
    char *p_char = NULL;
    uint8_t reg = 0;

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CREG?\r", p_sim->at_rsp_buf, 200) ) {
            p_char = strstr(p_sim->at_rsp_buf, "+CREG:");
            if (p_char != NULL) {
                reg = p_char[9] - '0';
                switch (reg) {
                case 1:                         /* 注册成功 */
                case 5:                         /* 处于漫游状态 */
                    return AW_OK;
                case 0:                         /* 注册失败 */
                case 2:                         /* 正在注册 */
                case 4:                         /* 未定义 */
                default:
                    return AW_GPRS_REG_FAIL;
                }
            }
        }
        aw_mdelay(200);
    }

    return AW_GPRS_RSP_ERR;
}

/** \brief 检查信号   */
aw_local aw_err_t __gprs_sim800_net_dect(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t csq = 0;
    uint8_t i = 0, j = 0;

    for (i = 0; i < 90; i++) {
        if (__gprs_sim800_csq_get(p_sim, &csq) != AW_OK) {
            aw_mdelay(1000);
            continue;
        }

        if ((csq < 3) || (csq == 99)) {
            aw_mdelay(1000);
            continue;
        }

        for (j = 0; j < 30; j++) {
            if (__gprs_sim800_reg_get(p_sim) != AW_OK) {
                aw_mdelay(1000);
                continue;
            } else {
                return AW_OK;
            }
        }
        return AW_GPRS_REG_FAIL;

    }
    return AW_GPRS_CSQ_LOW;
}

/** \brief 设置PDP */
aw_local aw_err_t __gprs_sim800_pdp_set(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;

    if ((p_sim->gprs.p_info->p_apn == NULL) ||
        (*p_sim->gprs.p_info->p_apn == '\0')) {

        return AW_OK;
    }

    sprintf(p_sim->at_cmd_buf, 
            "AT+CGDCONT=1,\"IP\",\"%s\"\r", 
            p_sim->gprs.p_info->p_apn);
    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }
    return AW_GPRS_PARA_ERR;
}

aw_local u32_t __serial_write (void* fd, u8_t *data, u32_t len)
{
    return aw_serial_write((int)fd, (char *)data, len);
}

aw_local u32_t __serial_read (void* fd, u8_t *data, u32_t len)
{
    int per_len;
    int cur_len;

    aw_serial_ioctl((int)fd, AW_FIONREAD, &per_len);
    if (per_len != 0) {
        aw_mdelay(5);
        aw_serial_ioctl((int)fd, AW_FIONREAD, &cur_len);

        if (cur_len == per_len) {
            cur_len = aw_serial_read((int)fd, (char *)data, len);
            return cur_len;
        }
    }

    return 0;
}

aw_local void __gprs_sim800_serial_init(struct awbl_gprs_sim800_dev *p_sim)
{
    uint32_t timeout;
    timeout = p_sim->p_devinfo->com_timeover;

    aw_serial_ioctl(p_sim->p_devinfo->com, 
                    SIO_BAUD_SET, 
                    (void *)p_sim->p_devinfo->baud);
    aw_serial_ioctl(p_sim->p_devinfo->com, 
                    SIO_HW_OPTS_SET, 
                    (void *)( CLOCAL | CREAD | CS8 ));
    /* 设置读超时*/
    aw_serial_ioctl(p_sim->p_devinfo->com, 
                    AW_TIOCRDTIMEOUT, 
                    (void *)timeout);

    p_sim->gprs.ppp.sio_mgn.fd = (void *)p_sim->p_devinfo->com;

    p_sim->gprs.ppp.sio_mgn.pfn_sio_read  = __serial_read;
    p_sim->gprs.ppp.sio_mgn.pfn_sio_write = __serial_write;
}

/** \brief 短信初始化 */
aw_local aw_err_t __gprs_sim800_sms_init(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i = 0;
    int32_t result = 0;

    if ((p_sim->gprs.p_info->p_center_num != NULL) &&
        (*p_sim->gprs.p_info->p_center_num != '\0')) {
        sprintf(p_sim->at_cmd_buf, 
                "AT+CSCA=\"%s\",145\r", 
                p_sim->gprs.p_info->p_center_num);
        result = -AW_ERROR;
        for (i = 0; i < 3; i++) {
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            if (__gprs_sim800_cmd_send(p_sim, 
                                       p_sim->at_cmd_buf, 
                                       p_sim->at_rsp_buf, 
                                       200) ) {
                if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                    result = AW_OK;
                    break;
                }
            }
            aw_mdelay(200);
        }
        if (result != AW_OK) {
            return result;
        }
    }

    result = -AW_ERROR;

    for (i = 0; i < 90; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CPMS=\"MT\",\"MT\",\"MT\"\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                result = AW_OK;
                break;
            }
        }
        aw_mdelay(1000);
    }

    return result;
}

aw_local aw_err_t __gprs_power_on (struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    aw_err_t ret;

    if (!p_sim->init_flag ) {
        ret = __gprs_sim800_init(p_sim);
        if (ret != AW_OK) {
            return ret;
        }

        ret = awbl_gprs_init_tail(&p_sim->gprs);

        p_sim->init_flag = AW_TRUE;
    }

    return __gprs_sim800_fn_init(p_this);
}

aw_local aw_err_t __gprs_power_off (struct awbl_gprs_dev *p_this)
{
    /* 在中间层调用了 pfn_power_off */
    return AW_OK;
}

aw_local aw_err_t __gprs_sim800_module_init(struct awbl_gprs_sim800_dev *p_sim)
{

    if (__gprs_sim800_echo(p_sim, OFF) != AW_OK) {
        return AW_GPRS_NO_RSP;
    }

    if (__gprs_sim800_fun_set(p_sim, 1) != AW_OK) {
        return AW_GPRS_RSP_ERR;
    }

    if (__gprs_sim800_sim_card_check(p_sim) != AW_OK) {
        return AW_GPRS_NO_SIM;
    }

    if (__gprs_sim800_not_auto_answer_set(p_sim) != AW_OK) {
        return AW_GPRS_RSP_ERR;
    }

    return AW_OK;
}

#if 0
/** \brief RI中断回调函数  */
static void aw_gprs_ri_int_callbak(void *p_arg)
{
    struct awbl_gprs_sim800_dev *p_sim = (struct awbl_gprs_sim800_dev *)p_arg;

    AW_SEMC_GIVE(p_sim->event_sem);
}
#endif

aw_local aw_err_t __sim800_at_test(struct awbl_gprs_sim800_dev *p_sim)
{
    memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
    if (__gprs_sim800_cmd_send(p_sim, "AT\r", p_sim->at_rsp_buf, 200) ) {
        if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
            return AW_OK;
        }
    }
    return -AW_EPERM;
}

aw_local aw_err_t __gprs_sim800_fn_init(struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    aw_err_t result = 0;
    char cimi[20];

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    __ITERATE(result = __sim800_at_test(p_sim), result == AW_OK, 5, 500);
    if (result == AW_OK) {
        AW_INFOF(("__gprs_sim800_fn_init： AT CMD test.\n"));
    } else {
        AW_INFOF(("__gprs_sim800_fn_init： AT CMD test failed \n"));
        return result;
    }

    result = __gprs_sim800_module_init(p_sim);
    if(result != AW_OK) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }
    AW_INFOF(("__gprs_sim800_fn_init: GPRS module init \n"));

    result = __gprs_sim800_net_dect(p_sim);
    if(result != AW_OK) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }
    AW_INFOF(("__gprs_sim800_fn_init: detect csq normal \n"));

    result = __gprs_sim800_imsi_get(p_sim, cimi, sizeof(cimi));
    if (strstr(cimi, "46000") != NULL ||
        strstr(cimi, "46002") != NULL ||
        strstr(cimi, "46007") != NULL) {
        p_sim->p_devinfo->p_gprs_info->p_apn = "CMNET";

    } else if (strstr(cimi, "46001") != NULL ||
                strstr(cimi, "46006") != NULL) {
        p_sim->p_devinfo->p_gprs_info->p_apn = "3GNET";

    } else if (strstr(cimi, "46003") != NULL ||
               strstr(cimi, "46005") != NULL ||
               strstr(cimi, "46011") != NULL) {
        p_sim->p_devinfo->p_gprs_info->p_apn = "CTNET";
    }
    if (p_sim->p_devinfo->p_gprs_info->p_apn != NULL) {
        AW_INFOF(("__gprs_sim800_fn_init：detect apn: %s \n", 
                  p_sim->p_devinfo->p_gprs_info->p_apn));
    }

    result = __gprs_sim800_pdp_set(p_sim);
    if(result != AW_OK) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }
    AW_INFOF(("__gprs_sim800_fn_init: GPRS set PDP \n"));

    result = __gprs_sim800_sms_init(p_sim);
    if (result != AW_OK) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }
    AW_INFOF(("__gprs_sim800_fn_init：init sms \n"));

    AW_SEMB_GIVE(p_sim->gprs_sem);

    return AW_OK;
}

aw_local aw_err_t awbl_gprs_sim800_csq_get(
    struct awbl_gprs_dev *p_this, uint8_t *p_csq)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {
        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {
            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_csq_get(p_sim, p_csq) != AW_OK) {
            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_csq_get(p_sim, p_csq) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }

    AW_SEMB_GIVE(p_sim->gprs_sem);
    return 0;
}

aw_local aw_err_t awbl_gprs_sim800_ccid_get(
    struct awbl_gprs_dev *p_this, char *p_buf, uint8_t nbytes)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {
        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {
            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_ccid_get(p_sim, p_buf, nbytes) != AW_OK) {
            __gprs_sim800_dat_mode_enter(p_sim);

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);

        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_ccid_get(p_sim, p_buf, nbytes) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }
}


/** \brief  删除指定位置的短信息   */
aw_local aw_err_t __gprs_sim800_sms_del(struct awbl_gprs_sim800_dev *p_sim, 
                                        uint16_t                     index)
{
    uint8_t i = 0;

    sprintf(p_sim->at_cmd_buf, "AT+CMGD=%d,0\r", index);
    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }
    return -AW_ERROR;
}

/** \brief  短信内容判断   */
aw_local aw_bool_t sms_check_type(uint8_t *psms)
{
    while (*psms) {
        if (*psms >= 0x80) {        /*  中文短信 */
            return AW_TRUE;
        }
        psms++;
    }
    return AW_FALSE;
}

/** \brief 计算中文字符长度  */
aw_local uint16_t compute_chinese_len(uint8_t *pstr)
{
    uint16_t len = 0;

    while (*pstr) {
        if (*pstr >= 0x80) {
            pstr++;
        }
        pstr++;
        len++;
    }
    return len;
}

/** \brief 中文短信初始化 */
aw_local int __gprs_sim800_chinese_sms_init(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i;
    int32_t result;

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CMGF=0\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                result = AW_OK;
                break;
            }
        }
        aw_mdelay(200);
    }

    return result;
}

/** \brief 发送中文短信  */
aw_local int __gprs_sim800_chinese_sms_send(struct awbl_gprs_dev *p_this,
                                            char                 *p_sms,
                                            char                 *p_num)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i;
    int32_t result;
    char message[370]={0};
    uint8_t len;
    aw_const char GPRS_MSG_END[]={0x1A,0x00};   /*  短信结束 */
    aw_const char GPRS_MSG_EXIT[]={0x1B,0x00};  /*  短信退出 */

    if (p_sms == NULL || *p_sms == '\0') {
        return -AW_ERROR;
    }

    if (p_num == NULL || *p_num == '\0') {
        return -AW_ERROR;
    }

    if (compute_chinese_len((uint8_t *)p_sms) > 70) {    /* 一条中文短信最长为70个中文字符 */
        return -AW_ERROR;
    }

    if (p_sim->online ) {
        p_sim->cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {
            p_sim->cmd_state = AW_FALSE;
            return -AW_ERROR;
        }
    }

    result = __gprs_sim800_chinese_sms_init(p_sim);
    if (result != AW_OK) {
        if (p_sim->online ) {

            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
        }
        return result;
    }

    len = strlen(p_sms);
    result = awbl_gprs_pdu_pack(message, 
                                sizeof(message), 
                                NULL, 
                                (uint8_t *)p_sms,
                                len, 
                                p_num, 
                                AW_GPRS_SMS_CODE_UTF8);
    if (result != AW_OK) {
        return result;
    }

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        sprintf(p_sim->at_cmd_buf, "AT+CMGS=%d\r\n", strlen(message)/2-1);
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "\r\n>") == NULL) {
                strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
                memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
                __gprs_sim800_cmd_send(p_sim, 
                                       p_sim->at_cmd_buf, 
                                       p_sim->at_rsp_buf, 
                                       200);
                continue;
            }

            strcat(message, GPRS_MSG_END);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            if (__gprs_sim800_cmd_send(p_sim, message, p_sim->at_rsp_buf, 6000) ) {
                if (strstr(p_sim->at_rsp_buf, "OK") != NULL) {
                    result = AW_OK;
                    break;
                }
                if (strstr(p_sim->at_rsp_buf, "ERROR") != NULL) {
                    result = -AW_ERROR;
                    break;
                }
            }
        } else {
            strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            __gprs_sim800_cmd_send(p_sim, p_sim->at_cmd_buf, p_sim->at_rsp_buf, 2000);
        }
    }

    if (p_sim->online ) {

        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
    }
    return result;
}

/** \brief 发送HEX短信  */
aw_local int __gprs_sim800_hex_sms_send(struct awbl_gprs_dev *p_this,
                                        uint8_t              *p_sms,
                                        uint8_t               len,
                                        char                 *p_num)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i;
    int32_t result;

    char message[370]={0};

    aw_const char GPRS_MSG_END[]={0x1A,0x00};      /*  短信结束 */
    aw_const char GPRS_MSG_EXIT[]={0x1B,0x00};     /*  短信退出 */

    if (p_sms == NULL) {
        return -AW_ERROR;
    }

    if (p_num == NULL || *p_num == '\0') {
        return -AW_ERROR;
    }

    if (len > 140) {            /*  一条HEX短信最长为140个字节  */
        return -AW_ERROR;
    }

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            return -AW_ERROR;
        }
    }

    result = __gprs_sim800_chinese_sms_init(p_sim);
    if (result != AW_OK) {
        if (p_sim->online ) {

            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
        }
        return result;
    }

    result = awbl_gprs_pdu_pack(message, 
                                sizeof(message), 
                                NULL, 
                                p_sms, 
                                len, 
                                p_num, 
                                AW_GPRS_SMS_CODE_8BIT);
    if (result) {
        return result;
    }
    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        sprintf(p_sim->at_cmd_buf, "AT+CMGS=%d\r\n", strlen(message)/2-1);
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "\r\n>") == NULL) {
                strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
                memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
                __gprs_sim800_cmd_send(p_sim, 
                                       p_sim->at_cmd_buf, 
                                       p_sim->at_rsp_buf, 
                                       200);
                continue;
            }

            strcat(message, GPRS_MSG_END);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            if (__gprs_sim800_cmd_send(p_sim, message, p_sim->at_rsp_buf, 6000) ) {
                if (strstr(p_sim->at_rsp_buf, "OK") != NULL) {
                    result = AW_OK;
                    break;
                }
                if (strstr(p_sim->at_rsp_buf, "ERROR") != NULL) {
                    result = -AW_ERROR;
                    break;
                }
            }
        } else {
            strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            __gprs_sim800_cmd_send(p_sim, p_sim->at_cmd_buf, p_sim->at_rsp_buf, 2000);
        }
    }

    if (p_sim->online ) {

        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
    }
    return result;
}

/** \brief 英文短信初始化 */
aw_local int __gprs_sim800_english_sms_init(struct awbl_gprs_sim800_dev *p_sim)
{
    uint8_t i;
    int32_t result;

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CMGF=1\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                result = AW_OK;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (result != AW_OK) {
        return result;
    }

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CSCS=\"GSM\"\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                result = AW_OK;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (result != AW_OK) {
        return result;
    }

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CSMP=17,168,0,0\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                result = AW_OK;
                break;
            }
        }
        aw_mdelay(200);
    }

    return result;
}

/** \brief 发送英文短信  */
aw_local int __gprs_sim800_english_sms_send(struct awbl_gprs_dev *p_this,
                                            char                 *p_sms,
                                            char                 *p_num)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    const char GPRS_MSG_END[] = {0x1A, 0x00};
    const char GPRS_MSG_EXIT[] = {0x1B, 0x00};
    uint8_t i;
    int32_t result;

    if (p_sms == NULL || *p_sms == '\0') {
        return -AW_ERROR;
    }

    if (p_num == NULL || *p_num == '\0') {
        return -AW_ERROR;
    }

    if (strlen(p_sms) > 160) {        /* 一条英文短信最长为160个英文字符  */
        return -AW_ERROR;
    }

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            return -AW_ERROR;
        }
    }

    result = __gprs_sim800_english_sms_init(p_sim);
    if (result != AW_OK) {
        if (p_sim->online ) {

            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
        }
        return result;
    }

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        sprintf(p_sim->at_cmd_buf, "AT+CMGS=\"%s\"\r", p_num);
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "\r\n>") == NULL) {
                strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
                memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
                __gprs_sim800_cmd_send(p_sim, 
                                       p_sim->at_cmd_buf, 
                                       p_sim->at_rsp_buf, 
                                       200);
                continue;
            }

            memset(p_sim->at_cmd_buf, 0, GPRS_CMD_BUF_LEN);
            strncpy(p_sim->at_cmd_buf, p_sms, 160);
            strcat(p_sim->at_cmd_buf, GPRS_MSG_END);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            if (__gprs_sim800_cmd_send(p_sim, 
                                       p_sim->at_cmd_buf, 
                                       p_sim->at_rsp_buf, 
                                       6000) ) {
                if (strstr(p_sim->at_rsp_buf, "OK") != NULL) {
                    result = AW_OK;
                    break;
                }
                if (strstr(p_sim->at_rsp_buf, "ERROR") != NULL) {
                    result = -AW_ERROR;
                    break;
                }
            }
        } else {
            strcpy(p_sim->at_cmd_buf, GPRS_MSG_EXIT);
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            __gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   2000);
        }
    }

    if (p_sim->online ) {

        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
    }
    return result;
}

/** \brief 删除全部短信 */
aw_local aw_err_t awbl_gprs_sim800_all_sms_del(struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {
        p_sim->cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {
            p_sim->cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }
    }

    for (i = 0; i < 3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim,
                                   "AT+CMGD=1,4\r", 
                                   p_sim->at_rsp_buf, 
                                   3000) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {

                AW_SEMB_GIVE(p_sim->gprs_sem);
                return AW_OK;
            }
        }

        aw_mdelay(200);
    }

    if (p_sim->online ) {
        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
    }

    AW_SEMB_GIVE(p_sim->gprs_sem);
    return -AW_ERROR;
}

/** \brief 发送短信  */
aw_local aw_err_t awbl_gprs_sim800_sms_send(struct awbl_gprs_dev *p_this,
                                            uint8_t              *p_sms,
                                            uint8_t               len,
                                            char                 *p_num)
{
    int result;

    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    if (sms_check_type(p_sms) ) {
        AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);
        result = __gprs_sim800_chinese_sms_send(p_this, (char *)p_sms, p_num);;
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    } else {
        AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);
        result = __gprs_sim800_english_sms_send(p_this, (char *)p_sms, p_num);
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }
}

/** \brief 发送HEX短信  */
aw_local aw_err_t awbl_gprs_sim800_sms_hex_send(struct awbl_gprs_dev *p_this,
                                                uint8_t              *p_sms,
                                                uint8_t               len,
                                                char                 *p_num)
{
    int result;

    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);
    result = __gprs_sim800_hex_sms_send(p_this, p_sms, len, p_num);
    AW_SEMB_GIVE(p_sim->gprs_sem);

    return result;
}

/** \brief 接收短信 */
aw_local aw_err_t awbl_gprs_sim800_sms_recv(struct awbl_gprs_dev      *p_this,
                                            struct aw_gprs_sms_buf    *p_sms,
                                            uint32_t                   timeout)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i;
    int32_t result;
    char *pret;
    uint16_t index;
    int quantity;
    aw_err_t ret;
    uint8_t del_flag = AW_FALSE;

    ret = AW_SEMC_TAKE(p_sim->sms_sem, timeout);
    if (ret != AW_OK) {
        return -1;
    }

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {
        p_sim->cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {
            p_sim->cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }
    }

    result = __gprs_sim800_chinese_sms_init(p_sim);
    if (result != AW_OK) {
        if (p_sim->online ) {
            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
        }
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return result;
    }

    result = -AW_ERROR;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CMGL=4\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "+CMGL:") != NULL) {
                result = AW_OK;
                break;
            }
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                break;
            }
        }
        aw_mdelay(100);
    }
    if (result != AW_OK) {
        if (p_sim->online ) {
            __gprs_sim800_dat_mode_enter(p_sim);
            p_sim->cmd_state = AW_FALSE;
        }
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -1;
    }

    result = -AW_ERROR;
    pret = strstr(p_sim->at_rsp_buf, "+CMGL:");
    if (pret != NULL) {
        pret += 7;
        index = atoi(pret);                  /*  获取<index> */
        del_flag = AW_TRUE;
        result = AW_OK;
    }

    if (result == AW_OK) {
        result = -AW_ERROR;
        if (pret != NULL) {
            if ((pret = strstr(pret, "\r\n")) != NULL) {
                pret += 2;
                result = AW_OK;
            }
        }
    }

    result = awbl_gprs_pdu_unpack(pret, p_sms);
    if (result == AW_OK) {
        result = -AW_ERROR;
        for (i = 0; i < 3; i++) {
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            if (__gprs_sim800_cmd_send(p_sim, 
                                       "AT+CPMS?\r", 
                                       p_sim->at_rsp_buf, 
                                       200) ) {
                if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                    result = AW_OK;
                    break;
                }
            }
            aw_mdelay(200);
        }
        quantity = 0;
        if (result == AW_OK) {
            pret = strchr(p_sim->at_rsp_buf, ',');
            if (pret != NULL) {              /*  寻找短信条数位置 */
                pret++;
                 quantity = atoi(pret);      /*  获取当前条数 */
            }
        }
    } else {
        quantity = -1;
    }

    if (del_flag ) {
        if (__gprs_sim800_sms_del(p_sim, index) != AW_OK) {/*  删除短信 */
            for (i = 0; i < 3; i++) {
                memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
                if (__gprs_sim800_cmd_send(p_sim, 
                                           "AT+CMGD=1,4\r", 
                                           p_sim->at_rsp_buf, 
                                           3000) ) {
                    if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                        break;
                    }
                }
                aw_mdelay(200);
            }
        }
    }

    if (p_sim->online ) {

        __gprs_sim800_dat_mode_enter(p_sim);
        p_sim->cmd_state = AW_FALSE;
    }

    AW_SEMB_GIVE(p_sim->gprs_sem);
    return quantity;
}

/** \brief 获取GPRS模块的版本号 */
aw_local aw_err_t awbl_gprs_sim800_revision_get(struct awbl_gprs_dev *p_this, 
                                                char                 *p_buf, 
                                                uint8_t               nbytes)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_revision_get(p_sim, p_buf, nbytes) != AW_OK) {
            __gprs_sim800_dat_mode_enter(p_sim);

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);

        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_revision_get(p_sim, p_buf, nbytes) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }
}

/** \brief 获取GPRS模块的IMEI号 */
aw_local aw_err_t awbl_gprs_sim800_imei_get(struct awbl_gprs_dev *p_this, 
                                            char                 *p_buf, 
                                            uint8_t               nbytes)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_imei_get(p_sim, p_buf, nbytes) != AW_OK) {

            __gprs_sim800_dat_mode_enter(p_sim);

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);

        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_imei_get(p_sim, p_buf, nbytes) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }
}

/** \brief 获取GPRS模块的IMSI号 */
aw_local aw_err_t awbl_gprs_sim800_imsi_get(struct awbl_gprs_dev *p_this, 
                                            char                  *p_buf, 
                                            uint8_t               nbytes)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_imsi_get(p_sim, p_buf, nbytes) != AW_OK) {

            __gprs_sim800_dat_mode_enter(p_sim);

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);

        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_imsi_get(p_sim, p_buf, nbytes) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }
}

/** \brief 获取GPRS模块的短信息中心号码 */
aw_local aw_err_t awbl_gprs_sim800_sca_get(struct awbl_gprs_dev *p_this, 
                                           char                 *p_buf, 
                                           uint8_t               nbytes)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    if (p_sim->online ) {

        p_sim->cmd_state = AW_TRUE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_TRUE;

        if (__gprs_sim800_cmd_mode_enter(p_sim) != AW_OK) {

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        if (__gprs_sim800_sca_get(p_sim, p_buf, nbytes) != AW_OK) {
            __gprs_sim800_dat_mode_enter(p_sim);

            p_sim->cmd_state = AW_FALSE;
            p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        __gprs_sim800_dat_mode_enter(p_sim);

        p_sim->cmd_state = AW_FALSE;
        p_sim->gprs.ppp.sio_mgn.serial_cmd_state = AW_FALSE;

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    } else {
        if (__gprs_sim800_sca_get(p_sim, p_buf, nbytes) != AW_OK) {

            AW_SEMB_GIVE(p_sim->gprs_sem);
            return -AW_ERROR;
        }

        AW_SEMB_GIVE(p_sim->gprs_sem);
        return AW_OK;
    }
}

#ifdef GPRS_INTERNAL_PROTOCOL
/** \brief 关闭GPRS连接 */
aw_local aw_err_t awbl_gprs_sim800_ip_close(struct awbl_gprs_dev *p_this, int hd)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPCLOSE=0\r", 
                                   p_sim->at_rsp_buf, 
                                   1000) ) {
            if (strstr(p_sim->at_rsp_buf, "CLOSE OK") != NULL) {
                break;
            }
        }
        aw_mdelay(200);
    }

    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPSHUT\r", 
                                   p_sim->at_rsp_buf, 
                                   1000) ) {
            if (strstr(p_sim->at_rsp_buf, "SHUT OK") != NULL) {
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }
    AW_SEMB_GIVE(p_sim->gprs_sem);
    return -AW_ERROR;
}

/** \brief 建立GPRS连接 */
aw_local int awbl_gprs_sim800_ip_start(struct awbl_gprs_dev *p_this, 
                                       uint8_t               mode,
                                       char                 *p_ip, 
                                       uint16_t              port,
                                       uint32_t              timeout)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;
    uint8_t err = 0;
    char *p_char;
    int ip[4];

    if ((p_sim->gprs.p_info->p_apn == NULL) ||
        (*p_sim->gprs.p_info->p_apn == '\0')) {
        return AW_GPRS_PARA_ERR;
    }

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPSHUT\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "SHUT OK") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<timeout; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CGATT?\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "+CGATT: 1") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(1000);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPMUX=0\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPMODE=0\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPHEAD=1\r", 
                                   p_sim->at_rsp_buf, 
                                   200)) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    sprintf(p_sim->at_cmd_buf, 
            "AT+CSTT=\"%s\",\"\",\"\"\r", 
            p_sim->gprs.p_info->p_apn);
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   p_sim->at_cmd_buf, 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CIICR\r", p_sim->at_rsp_buf, 1000) ) {
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        awbl_gprs_sim800_ip_close(p_this, 0);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CIFSR\r", p_sim->at_rsp_buf, 200) ) {
            p_char = strstr(p_sim->at_rsp_buf, "\r\n");
            if (p_char != NULL) {
                p_char += 2;
                if (sscanf(p_char, "%d.%d.%d.%d", &ip[0], &ip[1], &ip[2], &ip[3]) == 4) {
                    p_sim->gprs.ppp.p_pppaddr->our_ipaddr.addr = \
                        ((uint8_t)ip[3] << 24) | ((uint8_t)ip[2] << 16) | \
                        ((uint8_t)ip[1] << 8) | (uint8_t)ip[0];
                    err = 0;
                    break;
                }
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        p_sim->gprs.ppp.p_pppaddr->our_ipaddr.addr = 0;
    }

    if (mode == 1) {
        sprintf(p_sim->at_cmd_buf, "AT+CIPSTART=\"UDP\",\"%s\",\"%d\"\r", p_ip, port);
    } else {
        sprintf(p_sim->at_cmd_buf, "AT+CIPSTART=\"TCP\",\"%s\",\"%d\"\r", p_ip, port);
    }
    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, p_sim->at_cmd_buf, p_sim->at_rsp_buf, 500) ) {
            if (strstr(p_sim->at_rsp_buf, "CONNECT OK") != NULL) {
                err = 0;
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return AW_OK;
            }
            if (strstr(p_sim->at_rsp_buf, "CONNECT FAIL") != NULL) {
                err = 0;
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return -AW_ERROR;
            }
            if (strstr(p_sim->at_rsp_buf, "OK\r\n") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        awbl_gprs_sim800_ip_close(p_this, 0);
        return -AW_ERROR;
    }

    err = 1;
    for (i=0; i<timeout; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, NULL, p_sim->at_rsp_buf, 100) ) {
            if (strstr(p_sim->at_rsp_buf, "CONNECT OK") != NULL) {
                err = 0;
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return AW_OK;
            }
            if (strstr(p_sim->at_rsp_buf, "CONNECT FAIL") != NULL) {
                err = 0;
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return -AW_ERROR;
            }
        }
    }
    AW_SEMB_GIVE(p_sim->gprs_sem);
    return -AW_ERROR;
}

/** \brief 发送GPRS数据 */
aw_local ssize_t awbl_gprs_sim800_ip_send(struct awbl_gprs_dev *p_this,
                                          int                   hd,
                                          void                    *p_data,
                                          size_t                 len,
                                          uint32_t                 timeout)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;
    uint8_t err = 0;
    struct awbl_gprs_snd_dat_struct gprs_snd_msg;
    ssize_t snd_len;

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    err = 1;
    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, 
                                   "AT+CIPSTATUS\r", 
                                   p_sim->at_rsp_buf, 
                                   200) ) {
            if (strstr(p_sim->at_rsp_buf, "STATE: CONNECT OK") != NULL) {
                err = 0;
                break;
            }
        }
        aw_mdelay(200);
    }
    if (err == 1) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return -1;
    }

    gprs_snd_msg.p_data  = p_data;
    gprs_snd_msg.len     = len;
    gprs_snd_msg.timeout = timeout;
    AW_MAILBOX_SEND(p_sim->gprs_snd_dat_mailbox, 
                    (uint32_t )&gprs_snd_msg, 
                    AW_SEM_WAIT_FOREVER,
                    0);
    if (AW_MAILBOX_RECV(p_sim->gprs_snd_dat_rsp_mailbox, 
                        (uint32_t *)&snd_len, 
                        AW_SEM_WAIT_FOREVER) == AW_OK) {
        AW_SEMB_GIVE(p_sim->gprs_sem);
        return snd_len;
    }
    AW_SEMB_GIVE(p_sim->gprs_sem);
    return 0;
}

/** \brief 接收GPRS数据 */
aw_local ssize_t awbl_gprs_sim800_ip_receive(struct awbl_gprs_dev   *p_this,
                                             int                     hd,
                                             void                   *p_buf,
                                             size_t                  len)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);

    return aw_rngbuf_get(&p_sim->gprs_rcv_rngbuf, p_buf, len);
}

/** \brief 获取GPRS连接状态 */
aw_local aw_err_t awbl_gprs_sim800_ip_get_status(struct awbl_gprs_dev *p_this)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    uint8_t i = 0;

    AW_SEMB_TAKE(p_sim->gprs_sem, AW_SEM_WAIT_FOREVER);

    for (i=0; i<3; i++) {
        memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send(p_sim, "AT+CIPSTATUS\r", p_sim->at_rsp_buf, 200) ) {
            if (strstr(p_sim->at_rsp_buf, "STATE: CONNECT OK") != NULL) {
                AW_SEMB_GIVE(p_sim->gprs_sem);
                return AW_OK;
            }
        }
        aw_mdelay(200);
    }
    AW_SEMB_GIVE(p_sim->gprs_sem);
    return AW_ERROR;
}

/** \brief 获取GPRS发送数据ACK长度 */
aw_local aw_err_t awbl_gprs_sim800_ip_get_dat_ack(
    struct awbl_gprs_sim800_dev *p_sim, uint32_t *ack, uint32_t *nack)
{
    uint8_t i = 0;
    char *p_char;
    int tx_len, ack_len, nack_len;

    for (i=0; i<3; i++) {
        memset(p_sim->gprs_rcv_dat_buf, 0, GPRS_RSP_BUF_LEN);
        if (__gprs_sim800_cmd_send_low(p_sim, 
                                       "AT+CIPACK\r", 
                                       p_sim->gprs_rcv_dat_buf, 
                                       200) ) {
            p_char = strstr(p_sim->gprs_rcv_dat_buf, "+CIPACK:");
            if (p_char != NULL) {
                p_char += 8;
                if (sscanf(p_char, "%d,%d,%d", &tx_len, &ack_len, &nack_len) == 3) {
                    *ack  = ack_len;
                    *nack = nack_len;
                    return AW_OK;
                }
            }
        }
        aw_mdelay(200);
    }
    return AW_ERROR;
}
#endif

/** \brief GPRS接收任务  */
void aw_sim800_ip_task(void *arg)
{
    __SIM800_DECL_FROM_GPRS(p_sim, ((struct awbl_gprs_dev *)arg));
    aw_bool_t result;
    aw_err_t ret;
    struct awbl_gprs_at_cmd_struct  *p_gprs_at_cmd_msg;
#ifdef GPRS_INTERNAL_PROTOCOL
    uint16_t i;
    int32_t  len = 0, index = 0;
    char *p_rsp, *p_char;
    struct awbl_gprs_snd_dat_struct *p_gprs_snd_dat_msg;
    uint8_t err = 0;
    uint32_t ack_len_pre = 0;
    uint32_t ack_len, nack_len;
    const char rvc_head[] = "+IPD,";
#endif

    while (1) {
        /*
         *  处理AT命令
         */
        if (AW_MAILBOX_RECV(p_sim->gprs_at_cmd_mailbox, (uint32_t *)&p_gprs_at_cmd_msg, 10) == AW_OK) {
            memset(p_sim->at_rsp_buf, 0, GPRS_RSP_BUF_LEN);
            result = __gprs_sim800_cmd_send_low(p_sim, 
                                                p_gprs_at_cmd_msg->p_at, 
                                                p_sim->at_rsp_buf, 
                                                p_gprs_at_cmd_msg->timeout);
            AW_MAILBOX_SEND(p_sim->gprs_at_rsp_mailbox, 
                            (uint32_t )result, 
                            AW_SEM_WAIT_FOREVER,
                            0);
        }

        /*
         * 处理RI中断事件
         */
        ret = AW_SEMC_TAKE(p_sim->event_sem, 10);
        if (ret == AW_OK) {
            aw_mdelay(200);
            AW_SEMC_GIVE(p_sim->sms_sem);
        }

#ifdef GPRS_INTERNAL_PROTOCOL
        /*
         *  处理内部协议栈发送数据
         */
        if (AW_MAILBOX_RECV(p_sim->gprs_snd_dat_mailbox, 
                            (uint32_t *)&p_gprs_snd_dat_msg, 
                            10) == AW_OK) {
            if (awbl_gprs_sim800_ip_get_dat_ack(p_sim, &ack_len, &nack_len) == AW_OK) {
                ack_len_pre = ack_len;
            }

            sprintf(p_sim->at_cmd_buf, "AT+CIPSEND=%d\r", p_gprs_snd_dat_msg->len);
            err = 1;
            for (i=0; i<3; i++) {
                memset(p_sim->gprs_rcv_dat_buf, 0, GPRS_RSP_BUF_LEN);
                if (__gprs_sim800_cmd_send_low(p_sim, p_sim->at_cmd_buf, 
                                               p_sim->gprs_rcv_dat_buf, 
                                               500) ) {
                    if (strstr(p_sim->gprs_rcv_dat_buf, ">") != NULL) {
                        aw_serial_write((int)p_sim->gprs.ppp.sio_mgn.fd, 
                                        (char *)p_gprs_snd_dat_msg->p_data, 
                                        p_gprs_snd_dat_msg->len);
                        p_sim->gprs.ppp.sio_mgn.flow_statistic_upload += \
                                            p_gprs_snd_dat_msg->len;
                        err = 0;
                        break;
                    }
                }
                aw_mdelay(200);
            }
            if (err == 1) {
                AW_MAILBOX_SEND(p_sim->gprs_snd_dat_rsp_mailbox, 
                                (uint32_t )0, 
                                AW_SEM_WAIT_FOREVER,
                                0);
                continue;
            }

            memset(p_sim->gprs_rcv_dat_buf, 0, GPRS_RSP_BUF_LEN);
            err = 1;
            p_rsp = p_sim->gprs_rcv_dat_buf;
            for (i = 0; i < p_gprs_snd_dat_msg->timeout*100; i++) {
                len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, 
                                     (char *)p_rsp, 
                                     GPRS_RSP_BUF_LEN);
                if (len > 0) {
                    index = len;
                    err = 0;
                    break;
                }
            }
            if (err == 1) {
                AW_MAILBOX_SEND(p_sim->gprs_snd_dat_rsp_mailbox, 
                                (uint32_t )0, 
                                AW_SEM_WAIT_FOREVER,
                                0);
                continue;
            }
            for (i = 0; i < 10; i++) {    /* 应答帧间隔为100ms，应答完成 */
                len = aw_serial_read((int)p_sim->gprs.ppp.sio_mgn.fd, 
                                     (char *)&p_rsp[index], 
                                     GPRS_RSP_BUF_LEN - index);
                if (len > 0) {
                    index += len;
                    i = 0;
                    continue;
                }
            }

            p_char = (char *)p_rsp;
            while (1) {
                p_char = strstr(p_char, rvc_head);
                if (p_char == NULL) {
                    break;
                }
                p_char += 5;
                len = atoi(p_char);
                p_char = strchr(p_char, ':');
                if (p_char != NULL) {
                    p_char += 1;
                    if (index - (p_char-p_rsp) < len) {
                        len = index - (p_char-p_rsp);
                        aw_rngbuf_put(&p_sim->gprs_rcv_rngbuf, p_char, len);
                        p_sim->gprs.ppp.sio_mgn.flow_statistic_download += len;
                        break;
                    } else {
                        aw_rngbuf_put(&p_sim->gprs_rcv_rngbuf, p_char, len);
                        p_sim->gprs.ppp.sio_mgn.flow_statistic_download += len;
                        p_char += len;
                    }
                }
            }

            if (strstr(p_rsp, "SEND OK") != NULL) {
                AW_MAILBOX_SEND(p_sim->gprs_snd_dat_rsp_mailbox,
                               (uint32_t )p_gprs_snd_dat_msg->len,
                               AW_SEM_WAIT_FOREVER,
                               0);
            } else {
                err = 1;
                for (i = 0; i < p_gprs_snd_dat_msg->timeout; i++) {
                    if (awbl_gprs_sim800_ip_get_dat_ack(p_sim, &ack_len, &nack_len) == AW_OK) {
                        if ((ack_len-ack_len_pre) == p_gprs_snd_dat_msg->len) {
                            err = 0;
                            break;
                        }
                    }
                    aw_mdelay(1000);
                }
                if (err == 0) {
                    AW_MAILBOX_SEND(p_sim->gprs_snd_dat_rsp_mailbox, 
                                    (uint32_t )p_gprs_snd_dat_msg->len, 
                                    AW_SEM_WAIT_FOREVER,
                                    0);
                } else {
                    AW_MAILBOX_SEND(p_sim->gprs_snd_dat_rsp_mailbox, 
                                    (uint32_t )0, 
                                    AW_SEM_WAIT_FOREVER,
                                    0);
                }
            }
        }

        /*
         *  处理接收数据
         */
        while (1) {
            aw_serial_ioctl((int)p_sim->gprs.ppp.sio_mgn.fd, AW_FIONREAD, &len);
            if (len > 0) {
                awbl_gprs_process_rcv_data(p_sim);
            } else {
                break;
            }
        }
#endif
    }
}

/* GPRS_RSP_BUF_LEN*/
aw_local ssize_t  __at_cmd_send (struct awbl_gprs_dev *p_this, char *p_buf)
{
    __SIM800_DECL_FROM_GPRS(p_sim, p_this);
    if(__gprs_sim800_cmd_send(p_sim, p_buf, NULL, 20) ){
        if (strlen(p_sim->at_rsp_buf) > 0) {
            AW_INFOF(("__at_cmd_send: %s\n", p_sim->at_rsp_buf));
        }
    }
    return AW_OK;
}


aw_local aw_err_t __gprs_sim800_init (struct awbl_gprs_sim800_dev  *p_sim)
{

    __SIM800_DEVINFO_DECL(p_devinfo, &p_sim->dev);
    struct awbl_gprs_dev *p_gprs = NULL;

    if (p_sim == NULL || p_devinfo == NULL) {
        return -AW_EINVAL;
    }

    p_sim->p_devinfo = p_devinfo;
    p_gprs           = &p_sim->gprs;

    /* 对gprs设备进行部分初始化  */
    p_gprs->dev_id      = p_devinfo->id;
    p_gprs->p_info      = p_devinfo->p_gprs_info;
    p_gprs->p_sms       = p_devinfo->p_sms_info;
    p_gprs->p_serv      = &__g_gprs_servfunc;
    p_gprs->p_gprs_dev  = (void *)p_sim;

    /* 暂不支持 eth 模式 */
    aw_assert(p_gprs->ll_type == AWBL_GPRS_LL_TYPE_PPP);

    p_gprs->ppp.pfn_dial_up     = __gprs_sim800_dial_up;
    p_gprs->ppp.pfn_dial_down   = __gprs_sim800_dial_down;

    AW_SEMC_INIT(p_sim->sms_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    AW_SEMC_INIT(p_sim->event_sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
    AW_SEMB_INIT(p_sim->gprs_sem, AW_SEM_FULL, AW_SEM_Q_PRIORITY);

    __gprs_sim800_serial_init(p_sim);

    AW_MAILBOX_INIT(p_sim->gprs_at_cmd_mailbox, 1, AW_MAILBOX_Q_PRIORITY);
    AW_MAILBOX_INIT(p_sim->gprs_at_rsp_mailbox, 1, AW_MAILBOX_Q_PRIORITY);

#ifdef GPRS_INTERNAL_PROTOCOL
    aw_rngbuf_init(&p_sim->gprs_rcv_rngbuf, 
                    (char *)p_sim->gprs_rcv_rngbuf_tmp, 
                    GPRS_IP_RCV_BUF_LEN);

    AW_MAILBOX_INIT(p_sim->gprs_snd_dat_mailbox, 1, AW_MAILBOX_Q_PRIORITY);
    AW_MAILBOX_INIT(p_sim->gprs_snd_dat_rsp_mailbox, 1, AW_MAILBOX_Q_PRIORITY);

    p_sim->gprs.ppp.p_pppaddr = &p_sim->pppaddr;
#endif

    AW_TASK_INIT(__sim800_ip_task,
                 "sim800 ip task",
                 AW_TASK_SYS_PRIORITY(__SIM800_IP_TASK_PRIO),
                 __SIM800_IP_TASK_STK_SIZE,
                 aw_sim800_ip_task,
                 (void *)&p_sim->gprs);

    AW_TASK_STARTUP(__sim800_ip_task);


    return AW_OK;
}



aw_local aw_err_t __gprs_ioctl (struct awbl_gprs_dev *p_gprs,
                                aw_gprs_ioctl_req_t   req,
                                aw_gprs_ioctl_args_t *p_arg)
{
    aw_err_t ret = -AW_ENOTSUP;
    uint8_t  u8_tmp;

    switch (req) {
    /** \brief 设备复位，参数给0则为软件复位，给1则为硬件复位    */
    case AW_GPRS_REQ_RESET:
        p_gprs->p_serv->pfn_gprs_power_off(p_gprs);
        p_gprs->p_serv->pfn_gprs_power_on(p_gprs);
        break;
    case AW_GPRS_REQ_CSQ_GET:
        ret = awbl_gprs_sim800_csq_get(p_gprs, &u8_tmp);
        if (ret == AW_OK) {
            p_arg->csq = (int)u8_tmp;
        }

        break;
    case AW_GPRS_REQ_CCID_GET:
        ret = awbl_gprs_sim800_ccid_get(p_gprs, p_arg->ccid, sizeof(p_arg->ccid));
        break;

    case AW_GPRS_REQ_VERSION_GET:
        ret = awbl_gprs_sim800_revision_get(p_gprs, p_arg->version, sizeof(p_arg->version));
        break;
    case AW_GPRS_REQ_IMEI_GET:
        ret = awbl_gprs_sim800_imei_get(p_gprs, p_arg->imei, sizeof(p_arg->imei));
        break;

    case AW_GPRS_REQ_IMSI_GET:
        ret = awbl_gprs_sim800_imsi_get(p_gprs, p_arg->imsi, sizeof(p_arg->imsi));
        break;

    case AW_GPRS_REQ_SCA_GET:
        ret = awbl_gprs_sim800_sca_get(p_gprs, p_arg->sca, sizeof(p_arg->sca));
        break;

    case AW_GPRS_REQ_NET_MODE_GET:
    case AW_GPRS_REQ_BS_GET:
    default:
        return -AW_ENOTSUP;
    }

    return ret;
}

/******************************************************************************/
/*aw_local char position__g_gprs_servfunc;*/
/* gprs service functions (must defined as aw_const) */
aw_local aw_const struct awbl_gprs_service __g_gprs_servfunc = {
    .pfn_gprs_power_on      = __gprs_power_on,
    .pfn_gprs_power_off     = __gprs_power_off,
    .pfn_gprs_ioctl         = __gprs_ioctl,
    .pfn_gprs_sms_send      = awbl_gprs_sim800_sms_send,
    .pfn_gprs_sms_hex_send  = awbl_gprs_sim800_sms_hex_send,
    .pfn_gprs_sms_recv      = awbl_gprs_sim800_sms_recv,
    .pfn_gprs_all_sms_del   = awbl_gprs_sim800_all_sms_del,
#ifdef GPRS_INTERNAL_PROTOCOL
    .pfn_gprs_start         = awbl_gprs_sim800_ip_start,
    .pfn_gprs_close         = awbl_gprs_sim800_ip_close,
    .pfn_gprs_send          = awbl_gprs_sim800_ip_send,
    .pfn_gprs_recv          = awbl_gprs_sim800_ip_receive,
#endif

    .pfn_at_cmd_send        = __at_cmd_send,
    .pfn_at_cmd_recv        = NULL,
};
/******************************************************************************/
/**
 * \brief method "__gnss_serv_get" handler
 */
aw_local aw_err_t __sim800_dev_get (struct awbl_dev *p_dev, void *p_arg)
{
    __SIM800_DEV_DECL(p_this, p_dev);
    __SIM800_DEVINFO_DECL(p_devinfo, p_dev);

    struct awbl_gprs_dev *p_gprs = NULL;

    if (!p_dev) {
        return -AW_EINVAL;
    }

    p_gprs = &p_this->gprs;

    AW_MUTEX_INIT(p_gprs->mutex, AW_SEM_Q_PRIORITY);

    p_gprs->p_serv      = &__g_gprs_servfunc;
    p_gprs->p_gprs_dev  = (void *)p_this;
    p_gprs->dev_id      = p_devinfo->id;
    p_this->p_devinfo   = p_devinfo;

    *(struct awbl_gprs_dev **)p_arg = p_gprs;

    return AW_OK;
}
/******************************************************************************/
/* driver functions (must defined as aw_const) */
aw_local aw_const struct awbl_drvfuncs __g_drvfuncs = {
    NULL,
    NULL,
    NULL
};

/* driver methods (must defined as aw_const) */
AWBL_METHOD_IMPORT(awbl_gprs_dev_get);

aw_local aw_const struct awbl_dev_method __g_gprs_sim800_methods[] = {
    AWBL_METHOD(awbl_gprs_dev_get, __sim800_dev_get),
    AWBL_METHOD_END
};

aw_local aw_const awbl_plb_drvinfo_t __g_sim800_drv_registration = {
    {
        AWBL_VER_1,                         /* awb_ver */
        AWBL_BUSID_PLB,                     /* bus_id */
        AWBL_GPRS_SIM800_NAME,              /* p_drvname */
        &__g_drvfuncs,                      /* p_busfuncs */
        &__g_gprs_sim800_methods[0],        /* p_methods */
        NULL                                /* pfunc_drv_probe */
    }
};

void awbl_gprs_sim800_drv_register (void)
{
    int ret;
    ret = awbl_drv_register((struct awbl_drvinfo *)&__g_sim800_drv_registration);
    if (ret != AW_OK) {
        return;
    }
}

/* end of file*/
