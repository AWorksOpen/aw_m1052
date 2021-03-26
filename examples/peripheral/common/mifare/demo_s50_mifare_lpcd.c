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
 * \brief Mifare�����̣�IC��ʶ�𣬵͹���ģʽ��
 *
 * - �������裺
 *   1. ��ΪFM175XXоƬʹ��SPI����������ʹ�÷���������ָʾ������Ҫ���������ļ� aw_prj_params.h
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
 *   3. �����߰�Ϳ���������ƵA�࿨�������������
 *
 * - ʵ������
 *   1. ��S50��S70��Mifare��Ƭ�������ߣ���ʶ��ɹ�������������һ������λ����ӡ����Ƭ��Ϣ
 *
 * - ��ע��
 *   1.Mifare���İ������������߽ӿڣ�һ��A�࿨�ӿڣ�IC������һ��B�࿨�ӿڣ�ID������
 *     ʹ��ʱ��ע�⣬����ɲ鿴�����ĵ�ʹ��˵��
 *
 * \par Դ����
 * \snippet demo_s50_mifare_lpcd.c src_s50_mifare_lpcd
 *
 * \internal
 * \par Modification History
 * - 1.00 17-12-7  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_s50_mifare_lpcd
 * \copydoc demo_s50_mifare_lpcd.c
 */

/** [src_s50_mifare_lpcd] */
#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_delay.h"
#include "aw_buzzer.h"
#include "driver/card_reader/awbl_fm175xx.h"

/******************************************************************************/
#define DEMO_S50_ERR(...)    AW_ERRF((__VA_ARGS__))
#define DEMO_S50_INFO(...)   AW_INFOF((__VA_ARGS__))
#define DEMO_S50_ASSERT(...)          \
    do {                              \
        if (AW_OK != ret) {           \
            AW_INFOF((__VA_ARGS__));  \
            return ret;               \
        }                             \
    } while(0)

#define MYDEV_ID         (0)          /**< \brief deive ID */
#define KEY_NUM          (6)          /**< \brief cipher numbers */
#define DETECT_SENSITIVE (5)          /**< \brief %5 sensitive */

#define APP_MIFARE_TASK_PRIO          5
#define APP_MIFARE_TASK_STACK_SIZE    4096

AW_TASK_DECL_STATIC(mifare_task, APP_MIFARE_TASK_STACK_SIZE);

/******************************************************************************/
/** \brief mifare ����Ϣ�ṹ�� */
typedef struct mifare_info {
    uint16_t atqa;         /**< \brief ATQA */
    uint8_t  uid[12];      /**< \brief UID */
    uint8_t  sak[3];       /**< \brief SAK */
    uint8_t  uid_len;      /**< \brief uid ���� */
    uint8_t  cascal_level; /**< \brief UID���� */
    uint8_t  keya[6];      /**< \brief KAYA ��Կ */
    uint8_t  keyb[6];      /**< \brief KAYB ��Կ */
    uint8_t  acb[3];       /**< \brief ����λ */
} mifare_info_t;

typedef struct lpcd_mifare {
    mifare_info_t       mifare;
    awbl_fm175xx_lpcd_t lpcd;
    aw_semb_id_t        semb_id;
    AW_SEMB_DECL(semb);
} lpcd_mifare_t;

/******************************************************************************/
aw_local void lpcd_callback (void *p_arg)
{
    aw_semb_give((aw_semb_id_t)p_arg);
}

aw_local aw_err_t mifare_lpcd_mode_enter (uint8_t dev_id, lpcd_mifare_t *p_mi)
{
    aw_err_t ret = AW_OK;

    p_mi->semb_id = AW_SEMB_INIT(p_mi->semb,
                                 AW_SEM_EMPTY,
                                 AW_PSP_SEM_Q_PRIORITY);
    if (NULL == p_mi->semb_id) {
        return AW_ERROR;
    }

    ret = awbl_fm175xx_lpcd_param_init(dev_id,
                                       &p_mi->lpcd,
                                       lpcd_callback,
                                       p_mi->semb_id);
    DEMO_S50_ASSERT("LPCD Param init Failed!\n");

    ret = awbl_fm175xx_lpcd_config(dev_id);
    DEMO_S50_ASSERT("LPCD Config Failed!\n");

    ret = awbl_fm175xx_lpcd_calibra_init(dev_id, DETECT_SENSITIVE);
    DEMO_S50_ASSERT("LPCD Calibra Init Failed!\n");

    return ret;
}

/** \brief ��Ƭ���� */
aw_local aw_err_t mifare_card_activate (uint8_t dev_id, mifare_info_t *p_mi)
{
    uint8_t  cl;
    aw_err_t ret;

    for (cl = 0; cl < p_mi->cascal_level; cl++) {
        ret = awbl_fm175xx_picca_anticoll(dev_id, cl, p_mi->uid);
        if (ret != AW_OK) {
            DEMO_S50_ERR("Mifare Card Anticollision Fail, error code = %d!!\n", ret);
            break;
        } else {
            DEMO_S50_ERR("Mifare Card Anticollision level %d!\n", cl);
        }

        p_mi->uid_len += 5;

        aw_mdelay(1);

        ret = awbl_fm175xx_picca_select(dev_id,
                                        cl,
                                        &p_mi->uid[5 * cl],
                                        &p_mi->sak[cl]);
        if (ret != AW_OK) {
            DEMO_S50_ERR("Mifare Card Selection Fail, error code = %d!!\n", ret);
            break;
        } else {
            DEMO_S50_ERR("Mifare Card Selection %d!\n", cl);
        }
    }

    return ret;
}

/** \brief mifare ��������ʼ�� */
aw_local aw_err_t mifare_card_reader_init (uint8_t dev_id)
{
    aw_err_t                 ret;
    awbl_fm175xx_cfg_param_t param = AWBL_FM175XX_CFG_TYPEA_1;

    ret = awbl_fm175xx_config(dev_id, &param);
    if (AW_OK != ret) {
        DEMO_S50_ERR("FM175xx Config Fail, error code = %d!!\n", ret);
    }

    return ret;
}

/** \brief mifare card task */
aw_local void mifare_card_lpcd_task (void *p_arg)
{
    uint8_t             i;
    uint8_t             event;
    uint8_t             tx_dat[16];
    uint8_t             rx_dat[16];
    char               *p_name = (char *)p_arg;
    aw_err_t            ret;
    lpcd_mifare_t       mifare;

    awbl_fm175xx_lock(MYDEV_ID);
    DEMO_S50_INFO("\n%s enter\n", p_name);

    ret = awbl_fm175xx_reset(MYDEV_ID);
    if (AW_OK != ret) {
        DEMO_S50_ERR("FM175xx Init Fail, error code = %d!!\n", ret);
    }

    for (i = 0; i < 16; i++) {
        tx_dat[i] = i;
        rx_dat[i] = 0;
    }

    if (AW_OK != mifare_lpcd_mode_enter(MYDEV_ID, &mifare)) {
        goto err;
    }

    AW_FOREVER {

        /* ����LPCDģʽ */
        ret = awbl_fm175xx_lpcd_ctrl(MYDEV_ID, AW_TRUE);
        if (AW_OK != ret) {
            DEMO_S50_ERR("Re-enter LPCD mode Failed!\n");
        }

        aw_semb_take(mifare.semb_id, AW_SEM_WAIT_FOREVER);

        /* Ԥ��һС��ʱ�����ڵȴ������ȶ� */
        aw_mdelay(20);

        awbl_fm175xx_lpcd_event_pre_handle(MYDEV_ID, &event);
        if (AWBL_FM175XX_LPCD_CARDDET_EVENT !=
            (AWBL_FM175XX_LPCD_CARDDET_EVENT & event)) {

            /* �ǿ�Ƭ����¼������ؼ����ȴ� */
            DEMO_S50_INFO("Auto Wakeup Event!\n");
            continue;
        }

        /* ��Ƭ��Ϣ��λ */
        memset(&mifare.mifare, 0, sizeof(mifare_info_t));
        memset(&mifare.mifare.keya[0], 0xFF, 6);
        memset(rx_dat, 0, 16);

        /* ������Ƶ */
        awbl_fm175xx_rf_on(MYDEV_ID);

        /* ��������ʼ�� */
        mifare_card_reader_init(MYDEV_ID);

        /* ���ѿ�Ƭ */
        ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                         AWBL_FM175XX_PICC_REQ_ALL,
                                         &mifare.mifare.atqa);
        if (AW_OK == ret) {

            /* �ر���Ƶ */
            awbl_fm175xx_rf_off(MYDEV_ID);

            /* ��������ʼ�� */
            mifare_card_reader_init(MYDEV_ID);

            /* ������Ƶ */
            awbl_fm175xx_rf_on(MYDEV_ID);

            if ((mifare.mifare.atqa & 0xC0) == 0x00) {
                mifare.mifare.cascal_level = 1;         /* 1��UID */
            } else if ((mifare.mifare.atqa & 0xC0) == 0x40) {
                mifare.mifare.cascal_level = 2;         /* 2��UID */
            } else if ((mifare.mifare.atqa & 0xC0) == 0x80) {
                mifare.mifare.cascal_level = 3;         /* 3��UID */
            }

            DEMO_S50_INFO("\n\nCard Info:\n");
            DEMO_S50_INFO("ATQA = 0x%x\nUID = ", mifare.mifare.atqa);

            /* ���ѿ�Ƭ */
            ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                             AWBL_FM175XX_PICC_REQ_IDLE,
                                             &mifare.mifare.atqa);
            if (ret != AW_OK) {
                DEMO_S50_ERR("Wake up Failed2, error code = %d!!\n", ret);
                continue;
            }

            /* ���Ƭ */
            ret = mifare_card_activate(MYDEV_ID, &mifare.mifare);
            if (AW_OK != ret) {

                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_picca_halt(MYDEV_ID);
                if (AW_OK != ret) {
                    DEMO_S50_ERR("Halt Fail, error code = %d!!\n", ret);
                }
                continue;
            }

            for (i = 0; i < mifare.mifare.uid_len; i++) {
                DEMO_S50_INFO("0x%x ", mifare.mifare.uid[i]);
            }

            DEMO_S50_INFO(("\nSAK = "));
            for (i = 0; i < mifare.mifare.cascal_level; i++) {
                DEMO_S50_INFO("0x%x ", mifare.mifare.sak[i]);
            }

            /* ��Կ��֤ */
            ret = awbl_fm175xx_mifare_auth(MYDEV_ID,
                                           AWBL_FM175XX_MIFARE_KEYA,
                                           8,
                                           mifare.mifare.keya,
                                           mifare.mifare.uid,
                                           KEY_NUM);
            if (AW_OK != ret) {
                DEMO_S50_ERR("\nAuthentication Fail, error code = %d!!\n", ret);

                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_picca_halt(MYDEV_ID);
                if (AW_OK != ret) {
                    DEMO_S50_ERR("Halt Fail, error code = %d!!\n", ret);
                }

                continue;
            }

            aw_mdelay(5);

            /* д���� */
            ret = awbl_fm175xx_mifare_write(MYDEV_ID, 9, tx_dat, 16);
            if (AW_OK != ret) {
                DEMO_S50_ERR("\nMifare Card Write Fail, error code = %d!!\n", ret);
                continue;
            }

            /* ������ */
            memset(rx_dat, 0, 16);
            aw_mdelay(5);
            ret = awbl_fm175xx_mifare_read(MYDEV_ID, 9, rx_dat, 16);
            if (AW_OK != ret) {
                DEMO_S50_ERR("\nMifare Card Read Fail, error code = %d!!\n", ret);
                continue;
            }

            aw_mdelay(5);

            /* ֹͣ��Ƭ  */
            ret = awbl_fm175xx_picca_halt(MYDEV_ID);
            if (AW_OK != ret) {
                DEMO_S50_ERR("Halt Fail, error code = %d!!\n", ret);
                continue;
            }

            /* ����У�� */
            for (i = 0; i < 16; i++) {
                if (rx_dat[i] != i) {
                    DEMO_S50_ERR("\nData Checking Fail, error code = %d!!\n", ret);
                    break;
                }
            }
            DEMO_S50_INFO("\nSuccessful!!\n");

            /* ���������� 300 ms */
            aw_buzzer_beep(300);
        }

        /* �ر���Ƶ��� */
        awbl_fm175xx_rf_off(MYDEV_ID);

        aw_mdelay(500);
    }

err:

    DEMO_S50_INFO("\n%s exist\n", p_name);
    awbl_fm175xx_unlock(MYDEV_ID);
}

/******************************************************************************/
/**
 * \brief mifare lpcd card demo
 *
 * \return N/A
 */
void demo_s50_mifare_lpcd_entry (void)
{
    DEMO_S50_INFO("\nMifare Card(LPCD mode) Start...(10M_SPI)\n");

    /* ��ʼ������task */
    AW_TASK_INIT(mifare_task,                 /* ����ʵ�� */
                 "mifare_lpcd_task",          /* �������� */
                 APP_MIFARE_TASK_PRIO,        /* �������ȼ� */
                 APP_MIFARE_TASK_STACK_SIZE,  /* �����ջ��С */
                 mifare_card_lpcd_task,
                 (void *)"lpcd_task");        /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(mifare_task);
}

/** [src_s50_mifare_lpcd] */

/* end of file */
