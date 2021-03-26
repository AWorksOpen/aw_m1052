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
 * \brief SDcard 演示例程（裸测）
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - AW_COM_SDCARD
 *      - 对应平台的SD宏（如：AW_DEV_xxx_USDHCx）
 *      - AW_COM_CONSOLE
 *   2. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *   3. 在开发板上插入SD卡.
 *
 * - 实验现象：
 *   1. 串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_sdcard.c src_sdcard
 *
 * \internal
 * \par Modification History
 * - 1.00 17-08-31  xdn, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_sdcard SDCARD(裸操作)
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
 * \brief SD卡裸写 demo
 * \return 无
 */
void demo_sdcard_entry (void)
{
    aw_err_t ret;
    int      i;
    int      rty              = 10;
    uint8_t  buf[512]         = {0};
    aw_sdcard_dev_t *p_sdcard = NULL;

    /* 检测SD卡是否插入 */
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

    /* 打开SDcard设备 */
    p_sdcard = aw_sdcard_open(SD_DEV_NAME);
    if (p_sdcard == NULL) {
        AW_ERRF(("sdcard open error.\n"));
        return;
    }

    /*
     * SD卡读写测试
     * 注意：该接口为SD卡裸操作接口，直接操作物理块地址，写操作将会损坏SD卡数据和文件系统
     * 操作前确保备份SD卡数据，操作后需使用特定文件系统格式化后挂载使用文件系统操作
     */
    /* 写数据 */
      memset(buf, 'A', sizeof(buf));
      ret = aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_FALSE);
      if (ret != AW_OK) {
          AW_ERRF(("sdcard write error.\n"));
          goto __close;
      }

      /* 读数据 */
      memset(buf, 0x0, sizeof(buf));
      ret = aw_sdcard_rw_sector(p_sdcard, 0, buf, 1, AW_TRUE);
      if (ret != AW_OK) {
          AW_ERRF(("sdcard read error.\n"));
      }

      /* 数据校验 */
      for (i = 0; i < sizeof(buf); i++) {
          if (buf[i] != 'A') {
              AW_ERRF(("sdcard data verify failed at %d.\n", i));
              goto __close;
          }
      }

      AW_INFOF(("sdcard test successfully.\n"));

__close:

    /* 关闭 SD卡设备 */
    aw_sdcard_close(SD_DEV_NAME);

    return ;
}

/** [src_sdcard] */
 
/* end of file */
