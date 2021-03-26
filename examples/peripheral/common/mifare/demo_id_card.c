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
 * \brief Mifare�����̣�ID����
 *
 * - �������裺
 *   1. ��ΪFM175XXоƬʹ��SPI3����������ʹ�÷���������ָʾ������Ҫ���������ļ� aw_prj_params.h
 *      �ļ��д������豸:
 *      - AW_DEV_XXX_LPSPI3
 *      - AW_DEV_FM175XX_SPI3_0
 *      - AW_DEV_PWM_BUZZER
 *      - ��������Ӧ��PWM��
 *      - AW_DEV_XXX_LPUART1
 *   2. ʹ��USBת���ڵȴ��ڹ������ӿ�����ĵ��Դ��ں�PC����PC�Ĵ��ڵ������֣����ڵ�����������Ϊ��
 *      - ������ ��115200
 *      - ����λ ��8λ
 *      - У��λ ����
 *      - ֹͣλ ��1λ
 *      - ������
 *   3. �����߰�Ϳ���������Ƶ���B�࿨����������
 *
 * - ʵ������
 *   1. �����֤�������ߣ���ʶ��ɹ�������������һ������λ����ӡ����Ƭ��Ϣ
 *
 * - ��ע��
 *   1.Mifare���İ������������߽ӿڣ�һ��A�࿨�ӿڣ�IC������һ��B�࿨�ӿڣ�ID������
 *     ʹ��ʱ��ע�⣬����ɲ鿴�����ĵ�ʹ��˵��
 *
 * \par Դ����
 * \snippet demo_id_card.c src_id_card
 *
 * \internal
 * \par Modification History
 * - 1.00 18-11-1 mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_id_card
 * \copydoc demo_id_card.c
 */

/** [src_id_card] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_buzzer.h"
#include "driver/card_reader/awbl_fm175xx.h"

/******************************************************************************/
#define MYDEV_ID                     (0)    /**< \brief deive ID */

#define APP_IDCARD_TASK_PRIO          6
#define APP_IDCARD_TASK_STACK_SIZE    8192

AW_TASK_DECL_STATIC(id_card_task, APP_IDCARD_TASK_STACK_SIZE);

/******************************************************************************/
/** \brief ���֤��Ϣ�ṹ�� */
typedef struct id_card_info {
    awbl_piccb_dev_t        piccb;
    awbl_piccb_attrib_rsp_t attrib_rsp;
} id_card_info_t;

/******************************************************************************/
aw_static_inline void id_card_info_print (awbl_piccb_dev_t *p_piccb)
{
    AW_INFOF(("\n\nThe Id Card info are as follow:\n"
              "ID = 0x%02x, 0x%02x, 0x%02x, 0x%02x.\n\n"
              "Application Data are as follow:\n"
              "afi           = 0x%02x.\n"
              "crcb_aid      = 0x%02x, 0x%02x.\n"
              "app_num       = 0x%02x.\n\n"
              "Protocol Info are as follow:\n"
              "bitrate_cap   = 0x%02x\n"
              "protocol_type = 0x%02x\n"
              "maxframe_size = 0x%02x\n"
              "fo            = 0x%02x\n"
              "adc           = 0x%02x\n"
              "fwi           = 0x%02x\n",
              p_piccb->pupi[0],
              p_piccb->pupi[1],
              p_piccb->pupi[2],
              p_piccb->pupi[3],
              p_piccb->app_dat.afi,
              p_piccb->app_dat.crcb_aid[0],
              p_piccb->app_dat.crcb_aid[1],
              p_piccb->app_dat.app_num,
              p_piccb->prtl_info.bitrate_cap,
              p_piccb->prtl_info.protocol_type,
              p_piccb->prtl_info.maxframe_size,
              p_piccb->prtl_info.fo,
              p_piccb->prtl_info.adc,
              p_piccb->prtl_info.fwi));
}

/******************************************************************************/
/** \brief ��ȡ���֤���� */
aw_local aw_err_t id_card_attribute_read (uint8_t dev_id, id_card_info_t *p_id)
{
    aw_err_t            ret;
    awbl_piccb_attrib_t attrib;

    memset(&attrib, 0, sizeof(awbl_piccb_attrib_t));
    memset(&p_id->attrib_rsp, 0, sizeof(awbl_piccb_attrib_rsp_t));
    attrib.tr0              = AWBL_PICCB_ATTRIB_MIN_TR0_DEFAULT;
    attrib.tr1              = AWBL_PICCB_ATTRIB_MIN_TR1_DEFAULT;
    attrib.is_eof           = AW_TRUE;
    attrib.is_sof           = AW_TRUE;
    attrib.picc2pcd_bitrate = AWBL_PICCB_ATTRIB_PICC2PCD_BITRATE_106;
    attrib.pcd2picc_bitrate = AWBL_PICCB_ATTRIB_PCD2PICC_BITRATE_106;
    attrib.max_frame_size   = AWBL_PICCB_ATTRIB_MAX_FRAME_SIZE_256;
    attrib.cid              = 0x08;
    attrib.hinfo_len        = 0;
    attrib.p_hinfo          = NULL;

    ret = awbl_fm175xx_piccb_attrib_get(dev_id,
                                        &p_id->piccb,
                                        &attrib,
                                        &p_id->attrib_rsp);
    if (AW_OK == ret) {
        AW_INFOF(("\n\nThe ID Card Attrib Response are as follow:\n"
                  "mbli = 0x%02x, cid = 0x%02x\n",
                  p_id->attrib_rsp.mbli,
                  p_id->attrib_rsp.cid));
    } else {
        AW_INFOF(("\n\nID Card get attribute failed!\n"));
    }

    return ret;
}

/** \brief mifare ��������ʼ�� */
aw_local aw_err_t mifare_card_reader_init (uint8_t dev_id)
{
    aw_err_t                 ret;
    awbl_fm175xx_cfg_param_t param = AWBL_FM175XX_CFG_TYPEB_1;

    ret = awbl_fm175xx_config(dev_id, &param);
    if (AW_OK != ret) {
        AW_ERRF(("FM175xx Config Fail, error code = %d!!\n", ret));
    }

    return ret;
}

/* ��ȡ���֤UID */
aw_local aw_err_t id_card_uid_read (uint32_t devid, id_card_info_t *p_id)
{
    uint8_t            i;
    uint8_t            outbuf[5];
    uint8_t            inbuf[10];
    aw_err_t           ret;
    awbl_fm175xx_cmd_t cmd;

    outbuf[0] = 0x00;
    outbuf[1] = 0x36;
    outbuf[2] = 0x00;
    outbuf[3] = 0x00;
    outbuf[4] = p_id->attrib_rsp.cid;
    awbl_fm175xx_mkcmd(&cmd,
                       AWBL_FM175XX_CMD_TRANSCEIVE,
                       outbuf,
                       5,
                       0,
                       inbuf,
                       10);

    ret = awbl_fm175xx_crc_set(devid, AW_TRUE,  AW_TRUE);
    if (AW_OK != ret) {
        AW_ERRF(("TX CRC Setting failed, errno = %d", ret));
    }

    ret = awbl_fm175xx_crc_set(devid, AW_FALSE, AW_TRUE);
    if (AW_OK != ret) {
        AW_ERRF(("RX CRC Setting failed, errno = %d", ret));
    }

    ret = awbl_fm175xx_timeout_set(devid, 20);
    if (AW_OK != ret) {
        AW_ERRF(("Timeout Setting failed, errno = %d", ret));
    }

    ret = awbl_fm175xx_cmd_execute(devid, &cmd);
    if (AW_OK != ret) {
        AW_ERRF(("CMD EXECUTE failed, errno = %d", ret));
    } else {
        AW_INFOF(("\n\nUID = "));
        for (i = 0; i < 10; i++) {
            AW_INFOF(("0x%02x, ", inbuf[i]));
        }
        AW_INFOF(("\n"));
    }

    return ret;
}

/** \brief ID card task */
aw_local void id_card_entry (void *p_arg)
{
    uint8_t         afi;
    uint8_t         slots;
    char           *p_name = (char *)p_arg;
    aw_err_t        ret;
    id_card_info_t  id_card;

    awbl_fm175xx_lock(MYDEV_ID);
    AW_INFOF(("\n%s enter\n", p_name));

    ret = awbl_fm175xx_reset(MYDEV_ID);
    if (AW_OK != ret) {
        AW_ERRF(("FM175xx Init Fail, error code = %d!!\n", ret));
    }

    slots = 0;
    afi   = awbl_piccb_afi(AWBL_PICCB_AFI_MAIN_ALL, AWBL_PICCB_AFI_SUB_ALL);

    AW_FOREVER {

        /* ��Ƭ��Ϣ��λ */
        memset(&id_card, 0, sizeof(id_card_info_t));

        /* ��������ʼ�� */
        mifare_card_reader_init(MYDEV_ID);

        /* ������Ƶ */
        awbl_fm175xx_rf_on(MYDEV_ID);

        /* ������ʱһС��ʱ�䣬����һ���ֿ�Ƭ����ֻ��ѳ�ʱ */
        aw_mdelay(10);

        /* ���ѿ�Ƭ */
        ret = awbl_fm175xx_piccb_request(MYDEV_ID,
                                         AWBL_FM175XX_PICC_REQ_IDLE,
                                         afi,
                                         slots,
                                         &id_card.piccb);
        if (AW_OK == ret) {
            id_card_info_print(&id_card.piccb);

            /* ���ѿ�Ƭ */
            ret = awbl_fm175xx_piccb_request(MYDEV_ID,
                                             AWBL_FM175XX_PICC_REQ_IDLE,
                                             afi,
                                             slots,
                                             &id_card.piccb);
            if (ret != AW_OK) {
                AW_ERRF(("Request Failed, error code = %d!!\n", ret));
                continue;
            }

            /* ��ȡ��Ƭ���� */
            ret = id_card_attribute_read(MYDEV_ID, &id_card);
            if (AW_OK != ret) {

                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_piccb_halt(MYDEV_ID, &id_card.piccb);
                if (AW_OK != ret) {
                    AW_ERRF(("Halt Fail, error code = %d!!\n", ret));
                }
                continue;
            }

            id_card_uid_read(MYDEV_ID, &id_card);

            /* ֹͣ��Ƭ  */
            ret = awbl_fm175xx_piccb_halt(MYDEV_ID, &id_card.piccb);
            if (AW_OK != ret) {
                AW_ERRF(("Halt Fail, error code = %d!!\n", ret));
                continue;
            }

            AW_INFOF(("\nSuccessful!!\n"));

            /* ���������� 300 ms */
            aw_buzzer_beep(300);
        }

        /* �ر���Ƶ��� */
        awbl_fm175xx_rf_off(MYDEV_ID);
        aw_mdelay(500);
    }

    awbl_fm175xx_unlock(MYDEV_ID);
    AW_INFOF(("\n%s exist\n", p_name));
}

/******************************************************************************/
/**
 * \brief ID card demo
 *
 * \return N/A
 */
void demo_id_card_entry (void)
{
    AW_INFOF(("\nMifare Card Reader Start...V1.03(10M_SPI)\n"));

    /* ��ʼ������ */
    AW_TASK_INIT(id_card_task,                /* ����ʵ�� */
                 "mifare_id_task",            /* �������� */
                 APP_IDCARD_TASK_PRIO,        /* �������ȼ� */
                 APP_MIFARE_TASK_STACK_SIZE,  /* �����ջ��С */
                 id_card_entry,
                 (void *)"mifare_id_task");   /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(id_card_task);
}


/** [src_id_card] */

/* end of file */
