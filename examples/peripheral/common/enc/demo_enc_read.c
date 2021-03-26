/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief ����������
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��AW_DEV_XXX_ENC
 *
 * - ʵ������
 *   1. ������ÿתһȦ���ص���λʱ�������һ���ߵ�ƽ���壬��ʱ�ᴥ��Index�жϡ�
 *   2. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_enc_read.c src_enc_read
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_enc_read ������(��)
 * \copydoc demo_enc_read.c
 */

/** [src_enc_read] */
#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_delay.h"                   /* ��ʱ���� */
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "driver/enc/awbl_enc.h"



/* Index�жϴ������� */
aw_local uint32_t __g_index_conter = 0;

/* ʹ�õı�����������ͨ�� */
#define ENC_CHAN   1

/* ������������ */
#define ENC_LINE   1000


/* Index�жϻص����� */
aw_local void __index_int_func (void *p_arg)
{
    /*
     * ������ÿתһȦ���ص���λʱ��
     * �����һ���ߵ�ƽ���壬��ʱ�ᴥ��Index�жϡ�
     */
    __g_index_conter++;
}

/*
 * /brief �������
 */
void demo_enc_read_entry (void)
{

    aw_err_t ret  = 0;
    int read_cont = 20;
    int position  = 0;
    int16_t hold_position_difference = 0;
    int16_t hold_revolution = 0;

    /* enc��ʼ����ע���жϻص����� */
    ret = awbl_enc_init(ENC_CHAN, __index_int_func, NULL);
    if (AW_OK != ret) {
        aw_kprintf("awbl_enc_init fail!\r\n");
        return ;
    }

    while (read_cont--) {

        ret = awbl_enc_get_position(ENC_CHAN, &position);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_position error!\r\n");
            continue ;
        }
        aw_kprintf("enc position is: %d\r\n", position);

        ret = awbl_enc_get_position_difference(ENC_CHAN, &hold_position_difference);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_position_difference error!\r\n");
            continue ;
        }
        aw_kprintf("enc position difference is: %d\r\n", hold_position_difference);

        ret = awbl_enc_get_revolution(ENC_CHAN, &hold_revolution);
        if (AW_OK != ret) {
            aw_kprintf("awbl_enc_get_revolution error!\r\n");
            continue ;
        }
        aw_kprintf("enc revolution is: %d\r\n", hold_revolution);

        aw_mdelay(200);
    }
}


/** [src_enc_read] */

/* end of file */

