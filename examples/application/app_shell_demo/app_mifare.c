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
 * \brief ͨ��shell������ƶ�mifare���Ķ�д��IC����
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��
 *      - FM175XX�豸��
 *      - FM175XX�豸��Ӧ��SPI��
 *      - �������豸��
 *      - ��������Ӧ��PWM�豸��
 *   2. ��app_config.h�н�APP_BOARD��ֵ�޸�Ϊ__BOARD_MIFARE
 *   3. ��shell�������".test"��Ŀ¼������"mifare"
 *   4. ��mifare���������߸�Ӧ��
 *
 * - ʵ������
 *   1. �����ϴ�ӡ����Ƭ��Ϣ��
 *   2. ֻҪ������"mifare"ָ��ͻ�һֱ���������ǰ���"ctrl+C"��
 *
 * - ��ע��
 *   1.Mifare���İ������������߽ӿڣ�һ��A�࿨�ӿڣ�IC������һ��B�࿨�ӿڣ�ID������
 *     ʹ��ʱ��ע�⣬����ɲ鿴�����ĵ�ʹ��˵��
 *
 * \par Դ����
 * \snippet app_mifare.c app_mifare
 *
 * \internal
 * \par Modification History
 * - 1.00 18-07-11  sdq, first implementation.
 * \endinternal
 */

/** [src_app_mifare] */
#include "app_config.h"

#ifdef APP_MIFARE

#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_shell.h"
#include "aw_delay.h"
#include "aw_serial.h"
#include "aw_buzzer.h"
#include "aw_vdebug.h"

#include "driver/card_reader/awbl_fm175xx.h"


/** \brief mifare ����Ϣ�ṹ�� */
typedef struct mifare_info {
    uint16_t atqa;         /**< \brief ATQA */
    uint8_t  uid[12];      /**< \brief UID */
    uint8_t  sak[3];       /**< \brief SAK */
    uint8_t  uid_len;      /**< \brief uid ���� */
    uint8_t  cascal_level; /**< \brief UID ���� */
    uint8_t  keya[6];      /**< \brief KAYA ��Կ */
    uint8_t  keyb[6];      /**< \brief KAYB ��Կ */
    uint8_t  acb[3];       /**< \brief ����λ */
} mifare_info_t;

#define MYDEV_ID         (0)          /**< \brief deive ID */
#define KEY_NUM          (6)          /**< \brief cipher numbers */


/******************************************************************************/
aw_local aw_bool_t  __ctlc_is_press (void)
{
    char  ch = 0;

    uint32_t    size;

    size = aw_serial_read(0, (void *)&ch, 1);

    if ((size == 1) && (ch == 0x03)) {
        return  AW_TRUE;
    }

    return  AW_FALSE;
}




/******************************************************************************/
aw_local aw_err_t __mifare_card_init (uint8_t dev_id)
{
    aw_err_t                 ret;
#if 0
    awbl_fm175xx_cfg_param_t param;

    param.modwidth          = 0x26; /* ���ƿ�� */
    param.rx_gain           = 0x04; /* �������� */
    param.cwgsn             = 0x0F; /* �޵���ʱ��N��������絼ֵ */
    param.modgsn            = 0x08; /* ����ʱ��N��������絼ֵ */
    param.cwgsp             = 0x3F; /* �޵���ʱ��P��������絼ֵ */
    param.modgsp            = 0x20; /* ����ʱ��P��������絼ֵ */
    param.rx_min_level      = 0x05; /* �������ܹ����յ���С�ź�ǿ�� */
    param.rx_coll_min_level = 0x05; /* ������������ͻλ��С�ź�ǿ�� */
    param.prtl_type         = AWBL_ISO_IEC_14443_A;          /* ��Ƭ���� */
    param.comm_speed        = AWBL_FM175XX_COMM_SPEECH_106K; /* ͨѶ���� */
#endif
    awbl_fm175xx_cfg_param_t param = AWBL_FM175XX_CFG_TYPEA_1;
    ret = awbl_fm175xx_config(dev_id, &param);
    if (AW_OK != ret) {
        aw_kprintf("FM175xx Config Fail, error code = %d!!\n", ret);
    }

    return ret;
}




/******************************************************************************/
aw_local aw_err_t __mifare_card_activate (uint8_t dev_id, mifare_info_t *p_mi)
{
    uint8_t  cl;
    aw_err_t ret;

    for (cl = 0; cl < p_mi->cascal_level; cl++) {
        ret = awbl_fm175xx_picca_anticoll(dev_id, cl, p_mi->uid);
        if (ret != AW_OK) {
            aw_kprintf("Mifare Card Anticollision Fail, error code = %d!!\n", ret);
            break;
        } else {
            aw_kprintf("Mifare Card Anticollision level %d!\n", cl);
        }

        p_mi->uid_len += 5;

        aw_mdelay(1);

        ret = awbl_fm175xx_picca_select(dev_id,
                                        cl,
                                        &p_mi->uid[5 * cl],
                                        &p_mi->sak[cl]);
        if (ret != AW_OK) {
            aw_kprintf("Mifare Card Selection Fail, error code = %d!!\n", ret);
            break;
        } else {
            aw_kprintf("Mifare Card Selection %d!\n", cl);
        }
    }

    return ret;

}

/******************************************************************************/
int app_mifare (int argc, char **argv, struct aw_shell_user *p_user)
{
    uint8_t             i;
    uint8_t             tx_dat[16];
    uint8_t             rx_dat[16];
    aw_err_t            ret;
    mifare_info_t       mifare;

    awbl_fm175xx_lock(MYDEV_ID);
    aw_kprintf("\nmifare test enter\n");

    ret = awbl_fm175xx_reset(MYDEV_ID);
    if (AW_OK != ret) {
        aw_kprintf("FM175xx Init Fail, error code = %d!!\n", ret);
    }

    for (i = 0; i < 16; i++) {
        tx_dat[i] = i;
        rx_dat[i] = 0;
    }

    AW_FOREVER {
        /* ��Ƭ��Ϣ��λ */
        memset(&mifare, 0, sizeof(mifare_info_t));
        memset(&mifare.keya, 0xFF, 6);
        memset(rx_dat, 0, 16);

        /* ������Ƶ */
        awbl_fm175xx_rf_on(MYDEV_ID);

        /* ��������ʼ�� */
        __mifare_card_init(MYDEV_ID);

        /* ���ѿ�Ƭ */
        ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                         AWBL_FM175XX_PICC_REQ_IDLE,
                                         &mifare.atqa);
        if (AW_OK == ret) {
            /* �ر���Ƶ */
            awbl_fm175xx_rf_off(MYDEV_ID);

            /* ��������ʼ�� */
            __mifare_card_init(MYDEV_ID);

            /* ������Ƶ */
            awbl_fm175xx_rf_on(MYDEV_ID);

            if ((mifare.atqa & 0xC0) == 0x00) {
                /* 1��UID */
                mifare.cascal_level = 1;
            } else if ((mifare.atqa & 0xC0) == 0x40) {
                /* 2��UID */
                mifare.cascal_level = 2;
            } else if ((mifare.atqa & 0xC0) == 0x80) {
                /* 3��UID */
                mifare.cascal_level = 3;
            }

            aw_kprintf("\n\nCard Info:\n");
            aw_kprintf("ATQA = 0x%x\nUID = ", mifare.atqa);

            /* ���ѿ�Ƭ */
            ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                             AWBL_FM175XX_PICC_REQ_IDLE,
                                             &mifare.atqa);
            if (ret != AW_OK) {
                aw_kprintf("Wake up Failed2, error code = %d!!\n", ret);
                continue;
            }

            /* ���Ƭ */
            ret = __mifare_card_activate(MYDEV_ID, &mifare);
            if (AW_OK != ret) {
                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_picca_halt(MYDEV_ID);
                if (AW_OK != ret) {
                    aw_kprintf("Halt Fail, error code = %d!!\n", ret);
                }
                continue;
            }

            for (i = 0; i < mifare.uid_len; i++) {
                aw_kprintf("0x%x ", mifare.uid[i]);
            }

            aw_kprintf(("\nSAK = "));
            for (i = 0; i < mifare.cascal_level; i++) {
                aw_kprintf("0x%x ", mifare.sak[i]);
            }

            /* ��Կ��֤ */
            ret = awbl_fm175xx_mifare_auth(MYDEV_ID,
                                           AWBL_FM175XX_MIFARE_KEYA,
                                           8,
                                           mifare.keya,
                                           mifare.uid,
                                           KEY_NUM);
            if (AW_OK != ret) {
                aw_kprintf("\nAuthentication Fail, error code = %d!!\n", ret);

                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_picca_halt(MYDEV_ID);
                if (AW_OK != ret) {
                    aw_kprintf("Halt Fail, error code = %d!!\n", ret);
                }

                continue;
            }

            aw_mdelay(5);

            /* д���� */
            ret = awbl_fm175xx_mifare_write(MYDEV_ID, 9, tx_dat, 16);
            if (AW_OK != ret) {
                aw_kprintf("\nMifare Card Write Fail, error code = %d!!\n", ret);
                continue;
            }

            /* ������ */
            memset(rx_dat, 0, 16);
            aw_mdelay(5);
            ret = awbl_fm175xx_mifare_read(MYDEV_ID, 9, rx_dat, 16);
            if (AW_OK != ret) {
                aw_kprintf("\nMifare Card Read Fail, error code = %d!!\n", ret);
                continue;
            }

            aw_mdelay(5);

            /* ֹͣ��Ƭ  */
            ret = awbl_fm175xx_picca_halt(MYDEV_ID);
            if (AW_OK != ret) {
                aw_kprintf("Halt Fail, error code = %d!!\n", ret);
                continue;
            }

            /* ����У�� */
            for (i = 0; i < 16; i++) {
                if (rx_dat[i] != i) {
                    aw_kprintf("\nData Checking Fail, error code = %d!!\n", ret);
                    break;
                }
            }
            aw_kprintf("\nSuccessful!!\n");

            /* ���������� 300 ms */
            aw_buzzer_beep(300);

        }
        /* �ر���Ƶ��� */
        awbl_fm175xx_rf_off(MYDEV_ID);

        aw_mdelay(500);

        if (__ctlc_is_press()) {
            break;
        }
    }

    aw_kprintf("mifare test exit\r\n");

    awbl_fm175xx_lock(MYDEV_ID);
    return AW_OK;
}

/** [src_app_mifare] */
#endif /* APP_MIFARE */
