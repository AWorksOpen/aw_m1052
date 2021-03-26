/*******************************************************************************
*                                 Apollo
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-10  win, first implementation
 * \endinternal
 */

#ifndef __AWBL_UDA1380_H
#define __AWBL_UDA1380_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "awbus_lite.h"
#include "awbl_plb.h"
#include "aw_i2c.h"
#include "awbl_i2cbus.h"

#include "soc/awsa_soc_internal.h"

#define AWBL_UDA1380_NAME "uda1380_codec"

#define AWBL_UDA1380_DAC_CLK_SYSCLK 0
#define AWBL_UDA1380_DAC_CLK_WSPLL  1

struct awbl_uda1380_devinfo {
    int    gpio_power;
    int    gpio_reset;
    int    dac_clk;

    /** \brief power, reset use gpio ? */
    aw_bool_t gpio_is_valid;

    /** \brief �豸�ӻ���ַ */
    uint8_t addr;

    /** \brief soc codec name */
    const char *p_codec_name;

    /** \brief �����ǲ���+¼����ֻ�в��ţ�ֻ��¼�� */
    const char *dais_name[3];
};

/**
 * \brief UDA1380 �豸ʵ��
 */
struct awbl_uda1380_dev {

    /** \brief �̳��� AWBus I2C device �豸 */
    struct awbl_i2c_device       super;

    struct awsa_soc_codec        codec;
    struct awsa_soc_dai          dai[3];

    struct awsa_core_ctl_elem    ctl_elem[19];
    struct awsa_soc_dapm_widget  widget[18];
    struct awsa_core_ctl_elem    widget_ctl_elem[3];

    struct awsa_soc_dapm_path    path[20];

    /** \brief �Ĵ����޸ı�־ */
    uint32_t                     cache_dirty;

    /** \brief ��д�� */
    aw_spinlock_isr_t            lock;

    /** \brief �Ĵ������� */
    uint16_t                     reg_cache[0x24];
};

/**
 * \brief ����ע��
 */
void awbl_uda1380_drv_register (void);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWBL_UDA1380_H */

/* end of file */
