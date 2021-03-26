/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2017 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SDcard ��ʾ���̣���⣩
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *      - AW_COM_SDCARD
 *      - ��Ӧƽ̨��SD�꣨�磺AW_DEV_xxx_USDHCx��
 *      - AW_COM_CONSOLE
 *   2. �������е�DURX��DUTXͨ��USBת���ں�PC���ӣ����ڵ����������ã�
 *      ������Ϊ115200��8λ���ݳ��ȣ�1��ֹͣλ��������
 *   3. �ڿ������ϲ���SD��.
 *
 * - ʵ������
 *   1. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_sdcard.c src_sdcard
 *
 * \internal
 * \par Modification History
 * - 1.00 17-08-31  xdn, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdcard SDCARD(�����)
 * \copydoc demo_sdcard.c
 */

/** [src_sdcard] */
#include "aworks.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_task.h"
#include "aw_sdcard.h"
#include "aw_demo_config.h"

#define  SD_DEV_NAME       DE_SD_DEV_NAME

/**
 * \brief SD����д demo
 * \return ��
 */
void demo_sdcard_entry (void)
{
    aw_err_t ret;
    int      i;
    int      rty              = 10;
    uint8_t  buf[512]         = {0};
    aw_sdcard_dev_t *p_sdcard = NULL;

    /* ���SD���Ƿ���� */
    AW_INFOF(("wait for sdcard insert.\n"));
    while(rty-- > 0) {
        if (!aw_sdcard_is_insert(SD_DEV_NAME)) {
            aw_mdelay(1000);
        } else {
            break;
        }
    }
    if (rty <= 0 ) {
        AW_ERRF(("no sdcard found.\n"));
        return;
    }

    /* ��SDcard�豸 */
    p_sdcard = aw_sdcard_open(SD_DEV_NAME);
    if (p_sdcard == NULL) {
        AW_ERRF(("sdcard open error.\n"));
        return;
    }

    /*
     * SD����д����
     * ע�⣺�ýӿ�ΪSD��������ӿڣ�ֱ�Ӳ���������ַ��д����������SD�����ݺ��ļ�ϵͳ
     * ����ǰȷ������SD�����ݣ���������ʹ���ض��ļ�ϵͳ��ʽ�������ʹ���ļ�ϵͳ����
     */
    /* д���� */
      memset(buf, 'A', sizeof(buf));
      ret = aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_FALSE);
      if (ret != AW_OK) {
          AW_ERRF(("sdcard write error.\n"));
          goto __close;
      }

      /* ������ */
      memset(buf, 0x0, sizeof(buf));
      ret = aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_TRUE);
      if (ret != AW_OK) {
          AW_ERRF(("sdcard read error.\n"));
      }

      /* ����У�� */
      for (i = 0; i < sizeof(buf); i++) {
          if (buf[i] != 'A') {
              AW_ERRF(("sdcard data verify failed at %d.\n", i));
              goto __close;
          }
      }

      AW_INFOF(("sdcard test successfully.\n"));

__close:

    /* �ر� SD���豸 */
    aw_sdcard_close(SD_DEV_NAME);

    return ;
}

/** [src_sdcard] */
 
/* end of file */
