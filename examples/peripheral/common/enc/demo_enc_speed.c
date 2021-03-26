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
 * \brief ��������������
 *
 * - �������裺
 *   1. ��Ҫ��aw_prj_params.hͷ�ļ���ʹ��AW_DEV_XXXX_ENC
 *
 * - ʵ������
 *   1. ������ÿתһȦ���ص���λʱ�������һ���ߵ�ƽ���壬��ʱ�ᴥ��Index�жϡ�
 *   2. ���ڴ�ӡ������Ϣ��
 *
 * \par Դ����
 * \snippet demo_enc_speed.c src_enc_speed
 *
 * \internal
 * \par modification history:
 * - 1.00 17-11-16, mex, first implementation.
 * \endinternal
 */

/**
 * \addtogroup demo_if_enc_speed ������(����)
 * \copydoc demo_enc_speed.c
 */

/** [src_enc_speed] */


#include "aworks.h"                     /* ��ͷ�ļ����뱻���Ȱ��� */
#include "aw_delay.h"                   /* ��ʱ���� */
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_sem.h"
#include "aw_hwtimer.h"
#include "driver/enc/awbl_enc.h"
#include "aw_demo_config.h"


#define  __TEST_HWTIMER_DRV_NAME   DE_HWTIMER_NAME        /* ��ʱ������ */
#define  __TEST_HWTIMER_BUSID      DE_GPTID               /* ��ʱ�����ڵ�����ID */

/* Index�жϴ������� */
aw_local uint32_t __g_index_conter = 0;

/* ʹ�õı�����������ͨ�� */
#define ENC_CHAN   1

/* ������������ */
#define ENC_LINE   1000


/** \brief �����ź��� */
AW_SEMB_DECL_STATIC(__g_semb_intr);

/** \brief �жϷ����� */
aw_local void gpt_isr (void *p_arg)
{
    AW_SEMB_GIVE(__g_semb_intr);
}

/* Index�жϻص����� */
aw_local void __index_int_func (void *p_arg)
{
    /*
     * ������ÿתһȦ���ص���λʱ��
     * �����һ���ߵ�ƽ���壬��ʱ�ᴥ��Index�жϡ�
     */
    __g_index_conter++;
}

void demo_enc_speed_entry (void)
{
    int  position = 0;
    float speed   = 0.0;

    aw_hwtimer_handle_t timer;

    /* ������Ϊ __TEST_HWTIMER_DRV_NAME �Ķ�ʱ�� */
    timer = aw_hwtimer_alloc_byname(__TEST_HWTIMER_DRV_NAME,
                                    __TEST_HWTIMER_BUSID,
                                    0,
                                    gpt_isr,
                                    NULL);

    AW_SEMB_INIT(__g_semb_intr, AW_SEM_EMPTY,AW_SEM_FULL);

    /* enc��ʼ����ע���жϻص����� */
    awbl_enc_init(ENC_CHAN, __index_int_func, NULL);

    /* ʹ�ܶ�ʱ��,��������Ϊ1s */
    aw_hwtimer_enable(timer, 1);

    while(1) {

        /* �ȴ���ʱʱ�䵽 */
        AW_SEMB_TAKE(__g_semb_intr, AW_SEM_WAIT_FOREVER);

        /* ��ȡλ�� */
        awbl_enc_get_position(ENC_CHAN, &position);

        /* ����ÿ����ת��Ȧ�� */
        speed = __g_index_conter + position / (float)(4 * ENC_LINE);
        speed *= 60;
        __g_index_conter = 0;

        aw_kprintf("Motor speed is %f rpm\r\n", speed);
    }
}


/** [src_enc_speed] */

/* end of file */
