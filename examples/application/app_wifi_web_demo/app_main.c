
/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
*******************************************************************************/
/**
 * \file
 * \brief  Web Server��ʾ����(goahead��ʽʵ��)
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - ��Ӧƽ̨��wifi�����
 *      - ������
 *      - LED
 *   2. ���������ط���LED�Ƶ�ʹ��
 *   3. �ֻ����뿪���忪����AP���˺ţ�AWWIFI�� ���룺meiyoumima
 *   4. ��shell�������� ip addrָ��鿴ap ip��ַ
 *   5. ���뿪�����ȵ���ֻ�������������俪����wifi ap��IP��ַ
 *
 * - ʵ������
 *   1. �����������վҳ�棬��ʾ��demo��ҳ��
 *
 * \par Դ����
 * \snippet demo_webserver_goahead.c src_webserver_goahead
 *
 * \internal
 * \par modification history:
 * - 1.00 17-09-07, sdy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_webserver_goahead
 * \copydoc demo_webserver_goahead.c
 */

/** [src_std_webserver] */
#include "aworks.h"

extern void app_wifi_ap_entry(void);
extern void app_webserver_goahead_entry(void);

void app_webserver_main(void){

    app_wifi_ap_entry();

    app_webserver_goahead_entry();

    return ;
}
