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
 * @file
 * @brief telnet shell impl
 *
 * @internal
 * @par History
 * - 150514, rnk, first implementation.
 * @endinternal
 */

#ifndef __AW_TELNET_SHELL_H
#define __AW_TELNET_SHELL_H

#include "aw_shell_core.h"
#include "aw_task.h"
#include "aw_rngbuf.h"
#include "telnet/aw_telnet_io.h"

#ifdef __cplusplus
extern "C" {
#endif

struct aw_telnet_shell_dev {
    struct aw_shell_context   sc;
    struct aw_shell_user      sh;
    struct aw_shell_user     *p_old_sh;
    struct aw_shell_opt      *p_sc_opt;
    struct aw_rngbuf          in;
    char                      in_buf[32];
    struct aw_telnet_session *ts;
    struct aw_telnet_srv_dev *tsd;
    aw_task_id_t              tsk_id;
    aw_task_context_t         tsk_cont;
};

aw_err_t aw_telnet_shell_run(int arg);

/**
 * \brief ��ʼ��telnetʵ��
 * \param srv       telnet����������ʹ�ýṹ��
 * \param ts        telnet����ʵ���ṹ������
 * \param sd        shellʵ���ṹ������
 * \param cnt       telnet֧�ֿ����ӵ�����������ts��sd�����鳤��
 * \param listen_port telnet�����˿ںţ�Ĭ��Ӧ����Ϊ23
 * \param usr       telnet�����û���������û���Ϊ�գ����ʾ����Ҫ����
 * \param psw       telnet��������
 * \retval AW_OK    ���гɹ�
 */
aw_err_t aw_telnet_shell_init(struct aw_telnet_srv_dev   *srv,
                              struct aw_telnet_session   *ts,
                              struct aw_telnet_shell_dev *sd,
                              int                         cnt,
                              int                         listen_port,
                              const char                 *usr,
                              const char                 *psw,
                              struct aw_shell_opt        *p_shell_opt);

#ifdef __cplusplus
}
#endif

#endif /* __AW_TELNET_SHELL_H */

/* end of file */
