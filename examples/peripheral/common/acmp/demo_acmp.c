/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/

/**
 * \file
 * \brief 模拟比较器
 *
 * - 实验步骤：
 *   1. 需要在 aw_prj_params.h 中打开
 *      - AW_DEV_XXXX_ACMP1
 *      - AW_DEV_GPIO_LED
 *   2. 关闭其他文件中对LED灯的使用
 *   3. 使用ACMP_ID_GPIO引脚接触VCC和GND
 *   4. 屏蔽其他使用到LED灯的地方
 *
 *
 * - 实验现象：
 *    1. 当ACMP_ID_GPIO引脚接VCC，LED亮
 *    2. 当ACMP_ID_GPIO引脚接GND，LED灭
 *
 * \par 源代码
 * \snippet demo_acmp.c src_acmp
 *
 * \internal
 * \par modification history:
 * - 1.00 18-04-12  mex, first implementation
 * \endinternal
 */

/**
 * \addtogroup demo_if_acmp 模拟比较器
 * \copydoc demo_acmp.c
 */

/** [src_acmp] */
#include "aworks.h"                     /* 此头文件必须被首先包含 */
#include "aw_vdebug.h"
#include "aw_prj_params.h"
#include "aw_led.h"
#include "driver/acmp/awbl_acmp.h"
#include "aw_demo_config.h"

/** \brief ACMP的ID */
#define  ACMP_ID                  DE_ACMPID
#define  ACMP_ID_GPIO             DE_ACMPID_GPIO

/** \brief 参考电压输入通道选择 */
#define  ACMP_VREF_SRC            VREF_SOURCE_VIN2

/** \brief 参考电压的输入电压 */
#define  ACMP_VREF_IN_VOLT        33000

/** \brief 参考电压的输出电压(经过内部的64级T型电阻网络转换) */
#define  ACMP_VREF_DAC_OUT_VOLT   15000

/** \brief 同相输入端通道选择(相关引脚的初始化在配置头文件中) */
#define  ACMP_POSITIVE_CAH         1

/** \brief 反相输入端通道选择 */
#define  ACMP_NEGATIVE_CAH         7


/* ACMP中断回调函数 */
aw_local void __acmp_isr_func (void *p_arg)
{
    cmp_status_flags_t *flag = (cmp_status_flags_t *)p_arg;

    if (*flag == kCMP_OutputRisingEventFlag) {
        aw_led_on(0);
    } else if (*flag == kCMP_OutputFallingEventFlag) {
        aw_led_off(0);
    }
}

/******************************************************************************/
void demo_acmp_entry (void)
{
    /* 初始化并注册中断回调函数 */
    awbl_acmp_init(ACMP_ID, __acmp_isr_func);

    /* 设置参考电压 */
    awbl_acmp_config_dac(ACMP_ID,
                         ACMP_VREF_SRC,
                         ACMP_VREF_IN_VOLT,
                         ACMP_VREF_DAC_OUT_VOLT);

    /* 设置比较器的输入通道 */
    awbl_acmp_set_input_chan(ACMP_ID,
                             ACMP_POSITIVE_CAH,
                             ACMP_NEGATIVE_CAH);

    return ;
}

/** [src_acmp] */

/* end of file */
