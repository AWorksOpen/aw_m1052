/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief AWBus GPIO �ӿ�ͷ�ļ�
 *
 * ʹ�ñ�ģ����Ҫ��������ͷ�ļ���
 * \code
 * #include "awbl_gpio.h"
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 12-09-29  zen, first implementation
 * \endinternal
 */

#ifndef __AWBL_GPIO_H
#define __AWBL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif    /* __cplusplus    */

/**
 * \brief AWBus GPIO ����ʵ���������Ϣ
 */
struct awbl_gpio_servinfo {

    /** \brief ֧�ֵ���С���ű�� */
    int pin_min;

    /** \brief ֧�ֵ�������ű�� */
    int pin_max;

    /**
     *\brief ��¼����״̬���ڴ�(ÿ������1��bit)
     *
     * ������ʵ���˷����� pfunc_gpio_toggle()�򲻱��ṩ���ڴ棬��д NULL
     */
    uint8_t *p_pin_state;
};

/**
 * \brief AWBus GPIO ������
 */
struct awbl_gpio_servfuncs {

    /** \brief �������Ź��� */
    aw_err_t (*pfunc_gpio_pin_cfg)(void *p_cookie, int pin, uint32_t flags);

    /** \brief ��ȡ���ŵ�ǰ������/���ֵ */
    aw_err_t (*pfunc_gpio_get)(void *p_cookie, int pin);

    /** \brief �������ŵ����ֵ */
    aw_err_t (*pfunc_gpio_set)(void *p_cookie, int pin, int value);

    /** \brief ��ת���ŵ����ֵ */
    aw_err_t (*pfunc_gpio_toggle)(void *p_cookie, int pin);

    /** \brief �������Ŵ������� */
    aw_err_t (*pfunc_gpio_trigger_cfg)(void     *p_cookie,
                                       int       pin,
                                       uint32_t  flags);

    /** \brief ���ӻص����������� */
    aw_err_t (*pfunc_gpio_trigger_connect)(void           *p_cookie,
                                           int             pin,
                                           aw_pfuncvoid_t  pfunc_callback,
                                           void           *p_arg);

    /** \brief �����ŶϿ��ص����� */
    aw_err_t (*pfunc_gpio_trigger_disconnect)(void           *p_cookie,
                                              int             pin,
                                              aw_pfuncvoid_t  pfunc_callback,
                                              void           *p_arg);
    /** \brief �������ŵĴ����� */
    aw_err_t (*pfunc_gpio_trigger_on)(void *p_cookie, int pin);

    /** \brief �ر����ŵĴ����� */
    aw_err_t (*pfunc_gpio_trigger_off)(void *p_cookie, int pin);

    /** \brie ����һ������ */
    aw_err_t (*pfunc_gpio_pin_request)(void       *p_cookie, 
                                       const char *p_name,
                                       int         pin);

    /** \brie �ͷ�һ������ */
    aw_err_t (*pfunc_gpio_pin_release)(void *p_cookie, int pin);
};

/**
 * \brief AWBus GPIO ����ʵ��
 */
struct awbl_gpio_service {

    /** \brief ָ����һ�� GPIO ���� */
    struct awbl_gpio_service *p_next;

    /** \brief GPIO ����������Ϣ */
    const struct awbl_gpio_servinfo *p_servinfo;

    /** \brief GPIO �������غ��� */
    const struct awbl_gpio_servfuncs *p_servfuncs;

    /**
     * \brief GPIO �����Զ��������
     *
     * �˲�����GPIO�ӿ�ģ����������ʱ���������ã��ڵ��÷�����ʱ��Ϊ��һ��������
     */
    void *p_cookie;
};

/**
 * \brief AWBus GPIO �ӿ�ģ���ʼ��
 *
 * \attention ������Ӧ���� awbl_dev_init1() ֮���� awbl_dev_init2() ֮ǰ����
 */
void awbl_gpio_init(void);

#ifdef __cplusplus
}
#endif    /* __cplusplus     */

#endif    /* __AWBL_GPIO_H */

/* end of file */
