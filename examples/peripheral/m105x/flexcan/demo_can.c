/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
/**
 * \file
 * \brief CAN��ʾ����
 *
 * - �������裺
 *   1. ����������aw_prj_params.hͷ�ļ���ʹ��
 *   - ��Ӧƽ̨��CAN�豸��(AW_DEV_XXXXX_CAN1)
 *   - AW_COM_CONSOLE
 *   - ��Ӧƽ̨�Ĵ����豸��(AW_DEV_XXXXX_UART1)
 *   2. ����CAN1��������ŵ�CAN�շ������ٽ�CAN�շ����ӿ���USBCAN-II�豸������
 *   3. ��CAN��λ�������ò�����Ϊ500k��
 *   4. �򿪴��ڵ����նˣ�������115200-8-N-1��
 *
 * - ʵ������
 *   1. ��λ���������ݺ󣬿������յ����ݣ����ط����ݸ���λ����
 *   2. ���ڴ�ӡ��ص�����Ϣ��
 *
 * - ��ע��
 *   1. ���� CAN ID �� ���������� ��Ҫ���ݾ���Ӳ��ƽ̨�޸ġ�
 *
 * \par Դ����
 * \snippet demo_can.c src_demo_can
 *
 * \internal
 * \par modification history:
 * - 17-12-15, mex, first implementation.
 * \endinternal
 */

/**
 * \page demo_if_can
 * \copydoc demo_can.c
 */

#include "aworks.h"
#include "aw_can.h"
#include "aw_delay.h"
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_sem.h"

#define  CAN_CHN              0     /*CAN ID�����ݾ���ƽ̨�޸�*/
#define  CAN_RCV_BUFF_LENTH   1024

/** \brief �����ʱ������ȡ���ھ���Ӳ��ƽ̨����Ҫ���ݾ���Ӳ��ƽ̨������Ӧ���޸�*/
/** \brief tseg1 tseg2 sjw smp brp*/
static aw_can_baud_param_t can_btr_1000k = {15,  4,   4,   0,   2  };   /* 1000k, sample point 80% */
static aw_can_baud_param_t can_btr_800k  = {6,   3,   3,   0,   5  };   /* 800k,  sample point 70% */
static aw_can_baud_param_t can_btr_500k  = {15,  4,   4,   0,   4  };   /* 500k,  sample point 80% */
static aw_can_baud_param_t can_btr_250k  = {15,  4,   4,   0,   8  };   /* 250k,  sample point 80% */
static aw_can_baud_param_t can_btr_125k  = {15,  4,   4,   0,   16 };   /* 125k,  sample point 80% */
static aw_can_baud_param_t can_btr_100k  = {15,  4,   4,   0,   20 };   /* 100k,  sample point 80% */
static aw_can_baud_param_t can_btr_50k   = {15,  4,   4,   0,   40 };   /* 50k,   sample point 80% */
static aw_can_baud_param_t can_btr_20k   = {15,  4,   4,   0,   100};   /* 20k,   sample point 80% */
static aw_can_baud_param_t can_btr_10k   = {15,  4,   4,   0,   200};   /* 10k,   sample point 80% */
static aw_can_baud_param_t can_btr_6_25k = {16,  8,   4,   0,   256};   /* ����С�����ʣ�6.25k, sample point 68% */


aw_can_std_msg_t    can_rcv_msg[CAN_RCV_BUFF_LENTH];
aw_can_bus_status_t can_bus_status = 0;

/**< \brief ���廥���ź���������ͬ�����ͺͽ��գ����ͺͽ��ղ���ͬʱ���У� */
AW_MUTEX_DECL_STATIC(mutex);

aw_can_std_msg_t can_send_test_msg = {
    {
        0x111,
        0x01,
        AW_CAN_MSG_FLAG_SEND_NORMAL,    /**< \brief �������� */
        {0, 0},
        CAN_CHN,                        /**< \brief chn ��������ͨ���� */
        8                               /**< \brief msglen ���ĳ��� */
    },

    {1,2,3,4,5,6,7,8}
};

/** \brief �˲����ʽ
 * ��1�����˲���ʽѡ��,ͨ��ѡ��ȫID+��־�˲� ������0
 *
 * ��2�μ�֮����ȫID�˲��ĸ�ʽ,����
 * --------------------------------
 * | rmt(31) | ext(30) | id(29~1) |
 * --------------------------------
 * rmt Զ��֡��־.����λΪ1,����ն�ӦID��Զ��֡,�����������֡
 * ext ��չ֡��־.����λΪ1,����ն�ӦID����չ֡,������ձ�׼֡
 * id  �ٲ�ID.����(29~19)Ϊ��׼֡ID��,(29~1)Ϊ��չ֡ID��
 *
 */
#define __ID_ARB(id, ext, rmt)  (rmt << 31) | (ext << 30) | (id << (ext ? 1 : 19))
#define __ID_MSK_STD(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 19))  /* ��׼֡���� */
#define __ID_MSK_EXT(id, ext, rmt)  ((rmt << 31) | (ext << 30) | (id << 1))   /* ��չ֡���� */

/** \brief �����˲��� */
uint32_t filter_mr[] = {
    __ID_ARB(0x1, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x2, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x3, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x4, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x5, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x6, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x7, 0, 0),                       /* �ٲ�λ(��׼����֡) */
    __ID_ARB(0x8, 1, 0),                       /* �ٲ�λ(��չ����֡) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_STD(AW_CAN_STD_IDS_MASK, 1, 1),   /* ����λ(ȫ��׼֡�˲�) */
    __ID_MSK_EXT(AW_CAN_EXT_IDS_MASK, 1, 1)    /* ����λ(ȫ��չ֡�˲�) */
};

/******************************************************************************/
uint32_t __can_timestamp_get_cb (uint32_t p_arg);
aw_bool_t __can_err_sta_cb (int chn, aw_can_bus_err_t err, aw_can_bus_status_t status);

aw_can_app_callbacks_t can_app_callback_fun = {__can_timestamp_get_cb,
                                               NULL,
                                               NULL,
                                               __can_err_sta_cb,
                                               NULL
                                               };

/******************************************************************************/
/* �û��ص�������� */
uint32_t __can_timestamp_get_cb (uint32_t p_arg)
{
    return 0;
}

/******************************************************************************/
aw_bool_t __can_err_sta_cb (int                 chn,
                            aw_can_bus_err_t    err,
                            aw_can_bus_status_t status)
{
    aw_can_err_cnt_t  can_err_reg;

    if (err & AW_CAN_BUS_ERR_BIT) {    /**< \brief λ���� */
        aw_kprintf(("AW_CAN_BUS_ERR_BIT\n"));
    }
    if (err &AW_CAN_BUS_ERR_ACK) {     /**< \brief Ӧ����� */
        aw_kprintf(("AW_CAN_BUS_ERR_ACK\n"));
    }
    if (err &AW_CAN_BUS_ERR_CRC) {     /**< \brief CRC���� */
        aw_kprintf(("AW_CAN_BUS_ERR_CRC\n"));
    }
    if (err &AW_CAN_BUS_ERR_FORM) {    /**< \brief ��ʽ���� */
        aw_kprintf(("AW_CAN_BUS_ERR_FORM\n"));
    }
    if (err &AW_CAN_BUS_ERR_STUFF) {   /**< \brief ������ */
        aw_kprintf(("AW_CAN_BUS_ERR_STUFF\n"));
    }
    aw_can_err_cnt_get (chn, &can_err_reg);
    if (can_err_reg.tx_err_cnt > 127) {
        aw_can_err_cnt_clr(chn);
    }

    switch (status){
    case AW_CAN_BUS_STATUS_OK:
        break;

    case AW_CAN_BUS_STATUS_WARN:
        break;

    case AW_CAN_BUS_STATUS_OFF:
        //AW_INFOF(("\r\nCAN: Buss-Off! \r\n"));
        break;

    default:
        break;
    }
    can_bus_status = status;
    return AW_TRUE;
}

/**
 * \brief CAN demo
 *
 * \return ��
 */
void demo_can_entry (void)
{
    uint32_t rcv_num_real  = 0;
    uint32_t send_done_num = 0;
    uint32_t send_done_all = 0;

    uint32_t rcv_fifo_size = 0;

    size_t   filter_number = 0;

    //uint32_t timeout = 0;
    aw_can_err_t ret;

    /**< \brief ��ʼ�������ź��� */
    AW_MUTEX_INIT(mutex, AW_SEM_Q_FIFO);

    /** \brief ��ʼ��CAN */
    ret = aw_can_init(CAN_CHN,                 /** \brief CANͨ����*/
                      AW_CAN_WORK_MODE_NORMAL, /** \brief ����������ģʽ*/
                      &can_btr_500k,           /** \brief ��ǰ������*/
                      &can_app_callback_fun);  /** \brief �û��ص�����*/

    if (ret == AW_CAN_ERR_NONE) {
        AW_INFOF(("\r\nCAN: controller initialize ok. \r\n"));
    } else {
        AW_INFOF(("\r\nCAN: controller initialize error! \r\n"));
    }

    /** \brief ��ȡ����FIFO��С */
    rcv_fifo_size = aw_can_rcv_fifo_size (CAN_CHN);

    AW_INFOF(("CAN: receive fifo size %d messages.\r\n", rcv_fifo_size));

    /** \brief �����˲��� */
    aw_can_filter_table_set (CAN_CHN, (uint8_t*)&filter_mr, sizeof(filter_mr));
    aw_can_filter_table_get (CAN_CHN, (uint8_t*)&filter_mr, &filter_number);

    AW_INFOF(("CAN: filter table set.\r\n"));

    /** \brief ����CAN */
    aw_can_start(CAN_CHN);
    AW_INFOF(("CAN: controller start!\r\n"));

    AW_FOREVER {
        AW_MUTEX_LOCK(mutex, AW_SEM_WAIT_FOREVER);
#if 0
        aw_can_std_msgs_send(CAN_CHN,
                             &can_send_test_msg,
                             1,
                             &send_done_num,
                             500);
        if (1 != send_done_num) {
            AW_INFOF(("\r\nSend error(���ʹ���)!\r\n"));
        }
#else
        /** \brief ��ȡCAN����fifo�б��ĸ��� */
        rcv_num_real = aw_can_rcv_msgs_num (CAN_CHN);
        if (rcv_num_real) {
            aw_can_std_msgs_rcv(CAN_CHN,
                                can_rcv_msg,
                                CAN_RCV_BUFF_LENTH / 2,
                                &rcv_num_real,
                                0);

            if (rcv_num_real) {
                aw_can_std_msgs_send(CAN_CHN,
                                     can_rcv_msg,
                                     rcv_num_real,
                                     &send_done_num,
                                     3000);
                if (rcv_num_real != send_done_num) {
                    AW_INFOF(("\r\nSend error!\r\n"));
                }
                AW_INFOF(("\r\ndone:%d  all:%d \r\n",
                          send_done_num,
                          send_done_all += send_done_num));
                rcv_num_real = 0;
            }

        } else {
            aw_mdelay(10); /* ��ʱ10ms */
            if (can_bus_status == AW_CAN_BUS_STATUS_OFF) {
                aw_can_stop(CAN_CHN);
                aw_can_start(CAN_CHN);
            }
        }
        AW_MUTEX_UNLOCK(mutex);
        aw_mdelay(10);
#endif
    }
}

/** [src_can] */

/* end of file */
