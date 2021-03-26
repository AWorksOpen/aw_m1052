/*******************************************************************************
*                                 AWorks
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
 * \brief Aworks Flash转换层例程
 *
 * - 操作步骤：
 *   1. 本例程需在aw_prj_params.h头文件里使能
 *      - NANDFLASH外设宏
 *      - AW_COM_FTL
 *      - AW_COM_CONSOLE
 *   2. 根据实际情况，修改MTD设备名，即本例__MTD_NAME的名字。
 *   3. 将板子中的DURX、DUTX通过USB转串口和PC连接，串口调试助手设置：
 *      波特率为115200，8位数据长度，1个停止位，无流控
 *
 * - 实验现象:
 *   1. 进行FTL设备读写，串口打印调试信息。
 *
 * \par 源代码
 * \snippet demo_ftl.c src_ftl
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-05  sdq, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_ftl FTL读写
 * \copydoc demo_ftl.c
 */

/** [src_ftl] */

#include "aworks.h"
#include "aw_ftl.h"
#include "aw_vdebug.h"
#include "string.h"
#include "aw_delay.h"
#include "aw_mem.h"

/* mtd的名字在nandflash的硬件配置(awbl_hwconf_xxx_nandflash.h)里查找 */
#define __MTD_NAME  "/dev/mtd0"

/* 需要测试的逻辑块扇区编号 */
#define __TEST_SECTOR_NUM   150

/**
 * \brief 任务入口
 * \param[in] p_arg : 任务参数
 * \return 无
 */
void demo_ftl_entry (void)
{
    struct aw_ftl_dev *cur;
    uint32_t           arg = 0;
    char               name[100];
    uint8_t           *buf = NULL;
    aw_err_t           ret;
    uint32_t           sector_size;     /* 设备扇区大小 */

    /* 将一个mtd设备创建成ftl设备，创建模式为自动格式化 */
    ret = aw_ftl_dev_create ("/dev/ftl", __MTD_NAME);
    if (ret != AW_OK) {
        AW_INFOF(("ftl dev create failed\r\n"));
        return;
    }

    /* 获取刚刚创建的ftl设备 */
    cur = aw_ftl_dev_get("/dev/ftl");

    /* 设置系统信息的更新时间为60s */
    aw_ftl_dev_ioctl(cur, AW_FTL_SYS_INFO_UPDATE_TIME, (void *)60000);

    /* 获取设备的名字 */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_DEV_NAME, (void *)name)) {
        AW_INFOF(("Device name : %s\r\n", name));
    }

    /* 获取设备的扇区大小 */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_SECTOR_SIZE, (void *)&sector_size)) {
        AW_INFOF(("Sector size : %d\r\n", sector_size));
    }

    /* 获取设备的扇区数量 */
    if (!aw_ftl_dev_ioctl(cur, AW_FTL_GET_SECTOR_COUNT, (void *)&arg)) {
        AW_INFOF(("Sector counts : %d\r\n", arg));
    }

    /* 设置设备的逻辑块的清理阀值为100%时启动清理 */
    aw_ftl_dev_ioctl(cur, AW_FTL_LOGIC_THRESHOLD, (void *)100);

    /* 设置设备的日志块的清理阀值为87%时启动清理 */
    aw_ftl_dev_ioctl(cur, AW_FTL_LOG_THRESHOLD, (void *)87);

    buf = aw_mem_alloc(sector_size);
    if (buf == NULL) {
        AW_INFOF(("mem alloc failed\r\n"));
        return;
    }

    memset((char *)buf, 'h', sector_size);

    /* 写buf中的数据到扇区
     * 注意，读写扇区时buf空间大小要大于或等于ftl设备的扇区大小，
     * ftl设备的扇区大小可以通过ioctl获得
      */
    if (!aw_ftl_sector_write(cur, __TEST_SECTOR_NUM, buf)) {
        AW_INFOF(("Sector write success\r\n"));
    }

    memset((char *)buf, 0, sector_size);

    /* 读扇区的数据到buf中 */
    for(int i = 0 ; i < 20 ;i++) {
        ret = aw_ftl_sector_read(cur, __TEST_SECTOR_NUM, buf);
        if (ret == AW_OK) {
            AW_INFOF(("Sector read success\r\n"));
        }
    }

    /* 打印buf中的字符串 */
    AW_INFOF(("Sector %d data: %s\r\n", __TEST_SECTOR_NUM, buf));

    /* 当使用完ftl后，释放ftl设备 */
    if (!aw_ftl_dev_put(cur)) {
        AW_INFOF(("Put ftl device\r\n"));
    }

    /* 销毁ftl设备 */
    if (!aw_ftl_dev_destroy("/dev/ftl")) {
        AW_INFOF(("Destory ftl device\r\n"));
    }
}

/** [src_ftl] */
 
/* end of file */
