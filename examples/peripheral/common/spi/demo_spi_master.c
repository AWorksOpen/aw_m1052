/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2015 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief SPI������ʾ���̣�����SPI�Ļ�����д���ܣ�
 *
 * - ��������:
 *   1. ������ SPI �� MOSI ���ź� MISO �����öŰ��ߺʹӻ��豸��MOSI��MISO������
 *   2. ����aw_prj_params.h��ʹ�ܣ�
 *      - ��Ӧƽ̨��SPI��
 *      - AW_COM_CONSOLE
 *   3. ��������ʹ�ø�SPI������裻
 *
 * - ʵ������:
 *   1. ����ͨ��MISO�������ݣ����������ݴ�MOSI���أ�
 *   2. ����������ӡ���͡����յ���������Ϣ��
 *
 * \par Դ����
 * \snippet demo_spi_master.c src_spi_master
 *
 * \internal
 * \par History
 * - 1.00 18-06-28  lqy, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_spi_master SPI(����)
 * \copydoc demo_spi_master.c
 */

/** [src_spi_master] */
#include "aworks.h"         /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_delay.h"       /* ��ʱ���� */
#include "aw_spi.h"
#include "aw_gpio.h"
#include "aw_vdebug.h"
#include "aw_demo_config.h"

#define HARD_CS_CFG          1
//#define SOFT_CS_CFG          1 /* ʹ�����CS���spi�����ļ����ر�Ӳ��CSģʽ*/

/** \brief SPI1������ */
#define SPI_DEV_BUSID        DE_SPI_DEV_BUSID       /**< \brief SPI���� */
#define SPI_DEV_MODE         AW_SPI_CPHA            /**< \brief SPIģʽ */
#define SPI_DEV_SPEED        2000000                /**< \brief SPI�ٶ� */
#define SPI_DEV_BPW          8                     /**< \brief SPI����λ����ȡֵֻ����8��16��32 */
#define SPI_TEST_LEN         16                     /**< \brief SPI�շ������� */
#define SPI_CS_PIN           DE_SPI_CS_PIN          /**< \brief SPIƬѡ���ţ����ݾ���ƽ̨�޸� */

struct aw_spi_message       spi_msg;
struct aw_spi_transfer      spi_trans;

#if HARD_CS_CFG
void pfunc_cs (int state){

    return ;
}
#endif

/*
 * \brief ����SPI����ģʽ�Ļ�����д����
 */
void demo_spi_master_entry (void)
{
    aw_kprintf("demo_spi_master test...\n");

    aw_spi_device_t     spi_dev;
    int                 ret;
    int                 i;
#if SPI_DEV_BPW == 8
    uint8_t             read_buf[SPI_TEST_LEN]  = {0};
    uint8_t             write_buf[SPI_TEST_LEN] = {0};
#elif SPI_DEV_BPW == 16
    uint16_t             read_buf[SPI_TEST_LEN]  = {0};
    uint16_t             write_buf[SPI_TEST_LEN] = {0};
#elif SPI_DEV_BPW == 32
    uint32_t             read_buf[SPI_TEST_LEN]  = {0};
    uint32_t             write_buf[SPI_TEST_LEN] = {0};
#endif

#if SOFT_CS_CFG
    int                 cs_pin = SPI_CS_PIN;
    /* ����SPI֮ǰ��������CS���� */
    aw_gpio_pin_request("spi_demo_cs", &cs_pin, 1);
#endif

    /* ���� SPI FLASH �豸 */
    aw_spi_mkdev(&spi_dev,
                  SPI_DEV_BUSID,   /* λ������SPI������ */
                  SPI_DEV_BPW,     /* �ִ�С */
                  SPI_DEV_MODE,    /* SPI ģʽ */
                  SPI_DEV_SPEED,   /* ֧�ֵ�����ٶ� */
#if HARD_CS_CFG
                  NULL,            /* Ƭѡ���� */
                  pfunc_cs);       /* ���Զ����Ƭѡ���ƺ��� */
#else
                  cs_pin,          /* Ƭѡ���� */
                  NULL);           /* ���Զ����Ƭѡ���ƺ��� */
#endif
    if (aw_spi_setup(&spi_dev) != AW_OK) {
        aw_kprintf("aw_spi_setup fail!\r\n");
        goto _exit ;
    }


    /* ���÷������� */
    for (i = 0; i < SPI_TEST_LEN; i++) {
        write_buf[i] = i;
    }

    memset(read_buf,0,SPI_TEST_LEN);

    while(1){
        /* �������� */
        ret = aw_spi_write_and_read (&spi_dev,
                                     write_buf,
                                     read_buf,
                                     sizeof(write_buf));

        if (ret != AW_OK) {
            aw_kprintf("__spi_trans fail!\r\n");
            goto _exit ;
        }
        aw_kprintf("write_buf:");
        for(i = 0; i < SPI_TEST_LEN;i++){
            aw_kprintf("%d ",write_buf[i]);
        }
        aw_kprintf("\r\n");

        aw_kprintf("read_buf:");
        for(i = 0; i < SPI_TEST_LEN;i++){
            aw_kprintf("%d ",read_buf[i]);
        }
        aw_kprintf("\r\n");

        aw_mdelay(500);
    }

_exit:
#if SOFT_CS_CFG
    /* ������ɺ��ͷ�CS���� */
    aw_gpio_pin_release(&cs_pin, 1);
#endif
    return ;
}

/** [src_spi_master] */

/*end of file */
