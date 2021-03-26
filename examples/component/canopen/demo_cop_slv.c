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
 * \brief CANopen 从站演示例程(该例程需要使用字典文件aw_cop_slv_od_inst.c，配合CANopen核心板使用)
 *
 * - 操作步骤：
 *   1.在aw_prj_params.h中打开如下宏定义：
 *     - AW_COM_CANOPEN
 *     - CAN设备
 *     - GPT定时器
 *   2.接上USBCAN-E-P主站板卡，主站从站波特率均设置为500kbps
 *   3.打开CANmanager for CANopen上位机软件，点击启动选择对应的主站板卡及波特率
 *   4.选择COP-AW.eds文件
 *   5.点击搜索，搜索后点击关闭，即会进行从站分析（demo中从站地址为2）
 *   6.在上位机软件输出RPDO 设置自动加1及自动发送
 *
 * - 实验现象：
 *   1.调试串口会每隔3秒输出从站当前的状态，写入对象词典的状态以及所读取的对象词典值
 *   2.上位机工具上也可以看到demo中写入对象词典的数据
 *
 * \par 源代码
 * \snippet demo_cop_slv.c src_cop_slv
 *
 * \internal
 * \par Modification History
 * - 1.00 19-12-23  anu, first implementation.
 * \endinternal
 */

/**
 * \addtogroup grp_demo_if_cop_slv
 * \copydoc demo_cop_slv.c
 */

/** [src_cop_slv] */
#include "aworks.h"
#include "aw_cop_slv.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "string.h"

/************************************ 申明  *************************************/
extern const uint8_t *aw_cop_slv_od_seg_cont_get (void);
extern uint32_t aw_cop_slv_od_seg_cont_size_get (void);
extern uint8_t *aw_cop_slv_od_process_image_get (void);
extern uint32_t aw_cop_slv_od_process_image_size_get (void);
extern uint8_t aw_cop_slv_od_node_id_get (void);

/******************************************************************************/
aw_err_t demo_cop_slv (void)
{
    aw_err_t ret;
    uint8_t  inst = 0;
    uint32_t process_var_time = 0;

    aw_cop_slv_info_t info;

    aw_cop_slv_status_t status;

    uint8_t rdata[16], sdata[16];

    int i;

    info.baudrate = AW_COP_BAUD_500K;
    info.node_id = aw_cop_slv_od_node_id_get();
    info.p_seg_cont = aw_cop_slv_od_seg_cont_get();
    info.seg_cont_size = aw_cop_slv_od_seg_cont_size_get();
    info.p_process_image = aw_cop_slv_od_process_image_get();
    info.process_image_size = aw_cop_slv_od_process_image_size_get();

    ret = aw_cop_slv_init(inst, &info);       /* 初始化协议栈 */
    if (ret != AW_COP_ERR_NO_ERROR) {
        return ret;
    }

    ret = aw_cop_slv_connect_net(inst);       /* 连接到网络 */
    if (ret != AW_COP_ERR_NO_ERROR) {
        goto exit;
    }

    while (1) {

        if (process_var_time++ >= 3000) {

            ret = aw_cop_slv_status_get(inst, &status);
            AW_INFOF(("ret:%d, status:%d\n", ret, status));

            for (i = 0; i < sizeof(sdata); i++) {
                sdata[i]++;
            }

            ret = aw_cop_slv_tpdo_send(inst,
                                       0x00,
                                       sdata,
                                       sizeof(sdata));

            AW_INFOF(("write ret:%d\n", ret));

            ret = aw_cop_slv_rpdo_read(inst, 0x00, rdata, sizeof(rdata));
            AW_INFOF(("read ret:%d data:", ret));

            for (i = 0; i < sizeof(rdata); i++) {
                AW_INFOF(("%d ", rdata[i]));
            }

            AW_INFOF(("\n"));

            process_var_time = 0;
        }

        aw_cop_slv_process(inst);
        aw_mdelay(1);
    }

exit:
    aw_cop_slv_deinit(inst);
    return ret;
}

/** [src_cop_slv] */

/* end of file */
