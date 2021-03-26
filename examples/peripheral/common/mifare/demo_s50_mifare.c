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
 * \brief Mifare�����̣�IC��ʶ��
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
 *   3. �����߰�Ϳ���������Ƶ���A�࿨����������
 *
 * - ʵ������
 *   1. ��S50��S70��Mifare��Ƭ�������ߣ���ʶ��ɹ�������������һ������λ����ӡ����Ƭ��Ϣ
 *
 * - ��ע��
 *   1.Mifare���İ������������߽ӿڣ�һ��A�࿨�ӿڣ�IC������һ��B�࿨�ӿڣ�ID������
 *     ʹ��ʱ��ע�⣬����ɲ鿴�����ĵ�ʹ��˵��
 *
 * \par Դ����
 * \snippet demo_s50_mifare.c src_s50_mifare
 *
 * \internal
 * \par Modification History
 * - 1.00 17-12-7  mkr, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_s50_mifare
 * \copydoc demo_s50_mifare.c
 */

/** [src_s50_mifare] */
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

#define INTERFACES_TEST  (0)          /**< \brief normal interfaces test */
#define SIMPLE_DEMO      (1)          /**< \brief simple demo test */
#define DEMO_TYPE        SIMPLE_DEMO  /**< \brief demo type */

#define MYDEV_ID         (0)          /**< \brief deive ID */
#define KEY_NUM          (6)          /**< \brief cipher numbers */
#define DETECT_SENSITIVE (1)          /**< \brief %5 sensitive */

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

/******************************************************************************/
#if (DEMO_TYPE == INTERFACES_TEST)

/** \brief increment, decrement, restore test */
aw_local aw_err_t s50_mifare_idr_test (uint8_t dev_id, uint8_t blkaddr)
{
    uint8_t  i;
    uint8_t  blk_addr;
    uint8_t  dat_buf[16];
    uint8_t  inc_buf[4] = {1,1,1,1};
    aw_err_t ret = AW_OK;

    /* 1. Convert Data blocks into Value blocks */
    blk_addr = blkaddr;
    for (i = 0; i < 2; i++) {
        ret = awbl_fm175xx_mifare_valblk_construct(MYDEV_ID,
                                                   blk_addr + i,
                                                   0x06060606);
        if (AW_OK != ret) {
            DEMO_S50_ERR("valblk %d construct Fail, error code = %d!!\n",
                         blk_addr + i, ret);
            return ret;
        }
    }

    for (i = 0; i < 2; i++) {
        memset(dat_buf, 0, 16);
        ret = awbl_fm175xx_mifare_read(MYDEV_ID, blk_addr + i, dat_buf, 16);
        if (AW_OK == ret) {
            DEMO_S50_INFO("\nBefore increment, the value block %d data are as follow:\n",
                          blk_addr + i);
            for (i = 0; i < 16; i++) {
                DEMO_S50_INFO("%4d", dat_buf[i]);
            }
        } else {
            return ret;
        }
    }

    /* 2. increment test
     * data just in transfer buffer, do not in reality block.
     */
    ret = awbl_fm175xx_mifare_increment(MYDEV_ID, blk_addr, inc_buf, 4);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Increment Fail, error code = %d!!\n", blk_addr, ret);
        return ret;
    }

    memset(dat_buf, 0, 16);
    ret = awbl_fm175xx_mifare_read(MYDEV_ID, blk_addr, dat_buf, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Read Fail, error code = %d!!\n", blk_addr, ret);
        return ret;
    }
    DEMO_S50_INFO("\nAfter increment, the value block %d data are as follow:\n",
                  blk_addr);
    for (i = 0; i < 16; i++) {
        DEMO_S50_INFO("%4d", dat_buf[i]);
    }

    /* 3. decrement test */
    ret = awbl_fm175xx_mifare_decrement(MYDEV_ID, blk_addr + 1, inc_buf, 4);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Decrement Fail, error code = %d!!\n",
                     blk_addr + 1, ret);
        return ret;
    }

    memset(dat_buf, 0, 16);
    ret = awbl_fm175xx_mifare_read(MYDEV_ID, blk_addr + 1, dat_buf, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Read Fail, error code = %d!!\n", blk_addr + 1, ret);
        return ret;
    }
    DEMO_S50_INFO("\nAfter decrement, the value block %d data are as follow:\n",
                  blk_addr + 1);
    for (i = 0; i < 16; i++) {
        DEMO_S50_INFO("%4d", dat_buf[i]);
    }

    /* 4. restore the value block data into transfer buffer */
    ret = awbl_fm175xx_mifare_restore(MYDEV_ID, blk_addr);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Restore Fail, error code = %d!!\n", blk_addr, ret);
        return ret;
    }

    /* flush transfer buffer to reality block */
    ret = awbl_fm175xx_mifare_transfer(MYDEV_ID, blk_addr + 2);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Transfer Fail, error code = %d!!\n", ret);
        return ret;
    }

    memset(dat_buf, 0, 16);
    ret = awbl_fm175xx_mifare_read(MYDEV_ID, blk_addr + 2, dat_buf, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("block %d Read Fail, error code = %d!!\n", blk_addr + 1, ret);
        return ret;
    }
    DEMO_S50_INFO("\nRestore block %d to block %d, After restore, "
                  "the value block %d data are as follow:\n",
                  blk_addr, blk_addr + 2, blk_addr + 2);
    for (i = 0; i < 16; i++) {
        DEMO_S50_INFO("%4d", dat_buf[i]);
    }

    return ret;
}

aw_local void power_save_test (uint8_t id)
{
    /* �͹���ģʽ���� */
    awbl_fm175xx_low_power_enter(id, AWBL_FM175XX_DEEP_PWRDOWN);

    aw_mdelay(2000);

    /* �˳��͹���ģʽ */
    awbl_fm175xx_low_power_exit(id, AWBL_FM175XX_DEEP_PWRDOWN);
}

/** \brief ������ԣ�������д�����������桢�͹��� */
aw_local void s50_mifare_interfaces_test (void)
{
    uint8_t                  i;
    uint8_t                  cl;
    uint8_t                  casade;
    uint8_t                  sak;
    uint8_t                  uid[5];
    uint8_t                  blk_data[16];
    uint8_t                  key[KEY_NUM] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    uint16_t                 atqa;
    aw_err_t                 ret;
    awbl_fm175xx_cfg_param_t param = AWBL_FM175XX_CFG_TYPEA_1;

    ret = awbl_fm175xx_reset(MYDEV_ID);
    if (AW_OK != ret) {
        DEMO_S50_ERR("FM175xx Init Fail, error code = %d!!\n", ret);
        goto err;
    }

next:

    ret = awbl_fm175xx_config(MYDEV_ID, &param);
    if (AW_OK != ret) {
        DEMO_S50_ERR("FM175xx Config Fail, error code = %d!!\n", ret);
        goto err;
    }

    /* ���������� */
    awbl_fm175xx_rf_on(MYDEV_ID);

    /* ����mifare�� */
    ret = awbl_fm175xx_picca_request(MYDEV_ID, AWBL_FM175XX_PICC_REQ_ALL, &atqa);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Mifare Card Wake up Fail, error code = %d!!\n", ret);
        goto err;
    }
    DEMO_S50_INFO("\nATQA = 0x%x\n", atqa);

    if ((atqa & 0xC0) == 0x00) {
        casade = 1;     /* 1��UID */
    } else if ((atqa & 0xC0) == 0x40) {
        casade = 2;     /* 2��UID */
    } else if ((atqa & 0xC0) == 0x80) {
        casade = 3;     /* 3��UID */
    }

    for (cl = 0; cl < casade; cl++) {
        ret = awbl_fm175xx_picca_anticoll(MYDEV_ID, cl, uid);
        if (ret != AW_OK) {
            DEMO_S50_ERR("Mifare Card Anticollision Fail, error code = %d!!\n", ret);
            goto err;
        }

        DEMO_S50_INFO(("UID = "));
        for (i = 0; i < 5; i++) {
            DEMO_S50_INFO("0x%x ", uid[i]);
        }

        ret = awbl_fm175xx_picca_select(MYDEV_ID, cl, uid, &sak);
        if (ret != AW_OK) {
            DEMO_S50_ERR("Mifare Card Selection Fail, error code = %d!!\n", ret);
            goto err;
        }
        DEMO_S50_INFO("SAK = 0x%x", sak);
    }

    ret = awbl_fm175xx_mifare_auth(MYDEV_ID,
                                   AWBL_FM175XX_MIFARE_KEYA,
                                   16,
                                   key,
                                   uid,
                                   KEY_NUM);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Mifare Card Authentication Fail, error code = %d!!\n", ret);
        goto err;
    }

    for (uint8_t i = 0; i < 16; i++) {
        blk_data[i] = i;
    }
    ret = awbl_fm175xx_mifare_write(MYDEV_ID, 16, blk_data, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Mifare Card Write Fail, error code = %d!!\n", ret);
        goto err;
    }

    memset(blk_data, 0, 16);
    ret = awbl_fm175xx_mifare_read(MYDEV_ID, 16, blk_data, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Mifare Card Read Fail, error code = %d!!\n", ret);
        goto err;
    }

    for (i = 0; i < 16; i++) {
        if (blk_data[i] != i) {
            DEMO_S50_ERR("Data Checking Fail, error code = %d!!\n", ret);
            goto err;
        }
    }

    ret = s50_mifare_idr_test(MYDEV_ID, 16);
    if (AW_OK != ret) {
        DEMO_S50_ERR("increment, decrement Checking Fail, error code = %d!!\n", ret);
        goto err;
    }

    for (uint8_t blkaddr = 16; blkaddr < 20; blkaddr++) {
        ret = awbl_fm175xx_mifare_read(MYDEV_ID, blkaddr, blk_data, 16);
        if (AW_OK != ret) {
            DEMO_S50_ERR("\nblock %d Read Fail, error code = %d!!\n", blkaddr, ret);
            goto err;
        } else {
            DEMO_S50_INFO("\nblock 0x%x data are as follow:\n", blkaddr);
            for (i = 0; i < 16; i++) {
                DEMO_S50_INFO("0x%x ", blk_data[i]);
            }
        }
    }

    ret = awbl_fm175xx_picca_halt(MYDEV_ID);
    if (AW_OK != ret) {
        DEMO_S50_ERR("Halt Fail, error code = %d!!\n", ret);
        goto err;
    }

    /* �ر���Ƶ */
    awbl_fm175xx_rf_off(MYDEV_ID);

    power_save_test(MYDEV_ID);
    goto next;

    DEMO_S50_INFO("Successful!!\n");

err:

    while (1) {
        aw_mdelay(10);
    }

}

#else
/******************************************************************************/
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

#if 1
    awbl_fm175xx_cfg_param_t param = AWBL_FM175XX_CFG_TYPEA_1;

#else
    awbl_fm175xx_cfg_param_t param;

    param.rf_tx_pin         = 0;    /* ��Ƶ����ܽ�ΪTX1 */
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
    ret = awbl_fm175xx_config(dev_id, &param);
    if (AW_OK != ret) {
        DEMO_S50_ERR("FM175xx Config Fail, error code = %d!!\n", ret);
    }

    return ret;
}

/** \brief mifare card task */
aw_local void mifare_card_task (void *p_arg)
{
    uint8_t             i;
    uint8_t             tx_dat[16];
    uint8_t             rx_dat[16];
    uint8_t            *p_name = (uint8_t *)p_arg;
    aw_err_t            ret;
    mifare_info_t       mifare;

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

    AW_FOREVER {

        /* ��Ƭ��Ϣ��λ */
        memset(&mifare, 0, sizeof(mifare_info_t));
        memset(&mifare.keya, 0xFF, 6);
        memset(rx_dat, 0, 16);

        /* ������Ƶ */
        awbl_fm175xx_rf_on(MYDEV_ID);

        /* ��������ʼ�� */
        mifare_card_reader_init(MYDEV_ID);

        /* ���ѿ�Ƭ */
        ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                         AWBL_FM175XX_PICC_REQ_ALL,
                                         &mifare.atqa);
        if (AW_OK == ret) {

            /* �ر���Ƶ */
            awbl_fm175xx_rf_off(MYDEV_ID);

            /* ��������ʼ�� */
            mifare_card_reader_init(MYDEV_ID);

            /* ������Ƶ */
            awbl_fm175xx_rf_on(MYDEV_ID);

            if ((mifare.atqa & 0xC0) == 0x00) {
                mifare.cascal_level = 1;        /* 1��UID */
            } else if ((mifare.atqa & 0xC0) == 0x40) {
                mifare.cascal_level = 2;        /* 2��UID */
            } else if ((mifare.atqa & 0xC0) == 0x80) {
                mifare.cascal_level = 3;        /* 3��UID */
            }

            DEMO_S50_INFO("\n\nCard Info:\n");
            DEMO_S50_INFO("ATQA = 0x%x\nUID = ", mifare.atqa);

            /* ���ѿ�Ƭ */
            ret = awbl_fm175xx_picca_request(MYDEV_ID,
                                             AWBL_FM175XX_PICC_REQ_IDLE,
                                             &mifare.atqa);
            if (ret != AW_OK) {
                DEMO_S50_ERR("Wake up Failed2, error code = %d!!\n", ret);
                continue;
            }

            /* ���Ƭ */
            ret = mifare_card_activate(MYDEV_ID, &mifare);
            if (AW_OK != ret) {

                /* ֹͣ��Ƭ  */
                ret = awbl_fm175xx_picca_halt(MYDEV_ID);
                if (AW_OK != ret) {
                    DEMO_S50_ERR("Halt Fail, error code = %d!!\n", ret);
                }
                continue;
            }

            for (i = 0; i < mifare.uid_len; i++) {
                DEMO_S50_INFO("0x%x ", mifare.uid[i]);
            }

            DEMO_S50_INFO(("\nSAK = "));
            for (i = 0; i < mifare.cascal_level; i++) {
                DEMO_S50_INFO("0x%x ", mifare.sak[i]);
            }

            /* ��Կ��֤ */
            ret = awbl_fm175xx_mifare_auth(MYDEV_ID,
                                           AWBL_FM175XX_MIFARE_KEYA,
                                           8,
                                           mifare.keya,
                                           mifare.uid,
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

    DEMO_S50_INFO("\n%s exist\n", p_name);
    awbl_fm175xx_lock(MYDEV_ID);
}
#endif

/******************************************************************************/
/**
 * \brief mifare card demo
 *
 * \return N/A
 */
void demo_s50_mifare_entry (void)
{
    DEMO_S50_INFO("\nMifare Card Start...(10M_SPI)\n");
    const char *task_name = "mifare_task";

    /* ��ʼ������ */
    AW_TASK_INIT(mifare_task,                 /* ����ʵ�� */
                 "mifare_task",               /* �������� */
                 APP_MIFARE_TASK_PRIO,        /* �������ȼ� */
                 APP_MIFARE_TASK_STACK_SIZE,  /* �����ջ��С */
#if (DEMO_TYPE == INTERFACES_TEST)
                 s50_mifare_interfaces_test,  /* ������ں��� */
#else
                 mifare_card_task,
#endif
                 (void *)task_name);      /* ������ڲ��� */

    /* �������� */
    AW_TASK_STARTUP(mifare_task);
}

/** [src_s50_mifare] */

/* end of file */
