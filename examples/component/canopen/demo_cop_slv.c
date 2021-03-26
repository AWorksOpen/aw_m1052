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
 * \brief CANopen ��վ��ʾ����(��������Ҫʹ���ֵ��ļ�aw_cop_slv_od_inst.c�����CANopen���İ�ʹ��)
 *
 * - �������裺
 *   1.��aw_prj_params.h�д����º궨�壺
 *     - AW_COM_CANOPEN
 *     - CAN�豸
 *     - GPT��ʱ��
 *   2.����USBCAN-E-P��վ�忨����վ��վ�����ʾ�����Ϊ500kbps
 *   3.��CANmanager for CANopen��λ��������������ѡ���Ӧ����վ�忨��������
 *   4.ѡ��COP-AW.eds�ļ�
 *   5.������������������رգ�������д�վ������demo�д�վ��ַΪ2��
 *   6.����λ��������RPDO �����Զ���1���Զ�����
 *
 * - ʵ������
 *   1.���Դ��ڻ�ÿ��3�������վ��ǰ��״̬��д�����ʵ��״̬�Լ�����ȡ�Ķ���ʵ�ֵ
 *   2.��λ��������Ҳ���Կ���demo��д�����ʵ������
 *
 * \par Դ����
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

/************************************ ����  *************************************/
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

    ret = aw_cop_slv_init(inst, &info);       /* ��ʼ��Э��ջ */
    if (ret != AW_COP_ERR_NO_ERROR) {
        return ret;
    }

    ret = aw_cop_slv_connect_net(inst);       /* ���ӵ����� */
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
