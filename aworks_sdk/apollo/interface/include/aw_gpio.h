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
 * \brief AWorks GPIO ��׼�ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_gpio.h
 *
 * �������ṩ���������Ź���ģʽ���Լ���дGPIO�������������ƽ��������
 *
 * \par ��ʾ��
 * \code
 * #include "aw_gpio.h"
 *
 * // �������� PIO0_11 ΪGPIO���빦�ܣ���������ʹ��
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * val = aw_gpio_get(PIO0_11); //��ȡ���� PIO0_11 ������ֵ
 *
 * // �������� PIO0_11 ΪGPIO�������
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT);
 *
 * aw_gpio_set(PIO0_11, 0);    //�������� PIO0_11 �����ֵΪ0(�͵�ƽ)
 * val = aw_gpio_get(PIO0_11); //��ȡ���� PIO0_11 ��ǰ�����ֵ
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history:
 * - 1.10 13-04-23  zen, add api aw_gpio_toggle()
 * - 1.00 12-08-23  zen, first implementation
 * \endinternal
 */

#ifndef __AW_GPIO_H
#define __AW_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_gpio
 * \copydoc aw_gpio.h
 * @{
 */

#include "aw_psp_gpio.h"
#include "aw_gpio_util.h"

/**
 * \name ���Ź���
 * @{
 */

/** \brief GPIO���빦�� */
#define AW_GPIO_INPUT             AW_GPIO_FUNCBITS_CODE(AW_GPIO_INPUT_VAL)

/** \brief GPIO������� */
#define AW_GPIO_OUTPUT            AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_VAL)

/** \brief GPIO�����ʼΪ�� */
#define AW_GPIO_OUTPUT_INIT_HIGH  AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_INIT_HIGH_VAL)

/** \brief GPIO�����ʼΪ�� */
#define AW_GPIO_OUTPUT_INIT_LOW   AW_GPIO_FUNCBITS_CODE(AW_GPIO_OUTPUT_INIT_LOW_VAL)

/** @} */

/**
 * \name ����ģʽ
 * @{
 */

/** \brief ������������ʹ�� */
#define AW_GPIO_PULL_UP         AW_GPIO_MODEBITS_CODE(AW_GPIO_PULL_UP_VAL)

/** \brief ������������ʹ�� */
#define AW_GPIO_PULL_DOWN       AW_GPIO_MODEBITS_CODE(AW_GPIO_PULL_DOWN_VAL)

/** \brief ��������(�����������������) */
#define AW_GPIO_FLOAT           AW_GPIO_MODEBITS_CODE(AW_GPIO_FLOAT_VAL)

/** \brief ���ſ�© */
#define AW_GPIO_OPEN_DRAIN      AW_GPIO_MODEBITS_CODE(AW_GPIO_OPEN_DRAIN_VAL)

/** \brief ����������� */
#define AW_GPIO_PUSH_PULL       AW_GPIO_MODEBITS_CODE(AW_GPIO_PUSH_PULL_VAL)

/** @} */

/**
 * \name ���Ŵ�������
 * @{
 */

/** \brief �����ر� */
#define AW_GPIO_TRIGGER_OFF             0

/** \brief �ߵ�ƽ���� */
#define AW_GPIO_TRIGGER_HIGH            1

/** \brief �͵�ƽ���� */
#define AW_GPIO_TRIGGER_LOW             2

/** \brief �����ش��� */
#define AW_GPIO_TRIGGER_RISE            3

/** \brief �½��ش��� */
#define AW_GPIO_TRIGGER_FALL            4

/** \brief ˫���ش��� */
#define AW_GPIO_TRIGGER_BOTH_EDGES      5

/** @} */

/**
 * \brief �������ŵĹ��ܡ�ģʽ�Ȳ���
 *
 * ͨ������ \a flags ��ֵ���ﵽ�������Ź���ģʽ�Ȳ�����Ŀ�ģ�\a flags ��ʽΪ��
 * (<b>����|ģʽ|ƽ̨����</b>)
 *
 *  - �����ܡ�����:
 *      - #AW_GPIO_INPUT
 *      - #AW_GPIO_OUTPUT
 *  - ��ģʽ������:
 *      - #AW_GPIO_PULL_UP
 *      - #AW_GPIO_PULL_DOWN
 *      - #AW_GPIO_FLOAT
 *      - #AW_GPIO_OPEN_DRAIN
 *      - #AW_GPIO_PUSH_PULL
 *
 * �����ܡ��͡�ģʽ����־��ϳ�Ϊ��ͨ�ö��塱��־�򡣡�ͨ�ö��塱
 * ����ÿ����־���ǿ�ѡ�ģ�δ���õı�־���򱣳ֵ�ǰ���ò��䣬���磬
 * <code> aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT) </code>
 * ֻ�Ὣ���� PIO0_1 �ġ����ܡ�����Ϊ��GPIO���롱���ܣ����䡰ģʽ��
 * ���ֵ�ǰ�����ò��䡣
 *
 * �����ⲿ�жϹ��ܵ����ſ������Ӵ����ص������������ŵ������ƽ����
 * ���������������ҡ�����ʹ�ܡ�����ʱ������������ӵ������ϵĻص�������
 * �����ص�������������ο����� aw_gpio_trigger_connect()��
 *
 * ��ƽ̨���塱��־����ƽ̨���壬ʵ��ƽ̨�ض����������ã��硰ƽ̨���塱
 * ��־���롰ͨ�ö��塱��־�����ظ��Ķ��壬��ǰ�ߵ����ý�ȡ�����ߵ����ã�
 * ���磬<code> aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT| PIO0_1_OUTPUT) </code>
 * �Ὣ PIO0_1 ����Ϊ��GPIO������ܡ���
 *
 * \par ʵ����أ�
 *  - ƽ̨���Բ���ȫ֧�֡�ͨ�ö��塱��־��Ķ���
 *  - �Ƽ���ƽ̨���塱��־��ĸ�ʽΪ
 *    (<b>���Ź���|����ģʽ|���������������ò���</b>)
 *
 * \attention ���Ƽ�ͬʱʹ�á�ͨ�ö��塱��־��͡�ƽ̨���塱��־��
 *
 * \param[in] pin       ���ű��
 * \param[in] flags     ���ñ�־�����ʽ��ο��������ϸ����
 *
 * \retval AW_OK        �����ɹ�
 * \retval -AW_ENXIO    \a pin ������
 * \retval -AW_EIO      ��������
 *
 * \par ʾ�����������Ź��� (ʹ�á�ͨ�ö��塱��־��)
 * \code
 * #include "aw_gpio.h"
 *
 * // ��������PIO0_1ΪGPIO���빦��
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT);
 *
 * // ��������PIO0_1ΪGPIO���빦�ܣ���ʹ��������������
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * // ��������PIO0_1ΪGPIO������ܣ���ʹ��������������
 * aw_gpio_pin_cfg(PIO0_1, AW_GPIO_OUTPUT | AW_GPIO_PULL_UP);
 * \endcode
 */
aw_err_t aw_gpio_pin_cfg(int pin, uint32_t flags);


/**
 * \brief ����һ�����ű�ĳ����/ģ��ʹ��
 *
 * \param[in] p_name  ���ű�ʹ�õ�����/ģ����
 * \param[in] p_pins  ָ�������б�
 * \param[in] num     ������
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ���ű�ռ��
 * \retval -AW_ENXIO  \a pin ������
 *
 * \par ʾ��
 * \code
 *
 * static const uart0_pins_tabe[] = {
 *     PIO0_0,
 *     PIO0_1
 * };
 * if (aw_gpio_pin_request("UART0",
 *                          uart0_pins_tabe,
 *                          AW_NELEMENTS(uart0_pins_tabe)) == AW_OK) {
 *
 *     aw_gpio_pin_cfg(PIO0_0, PIO0_0_USART0_RXD | PIO0_0_PULLUP);
 *     aw_gpio_pin_cfg(PIO0_1, PIO0_1_USART0_TXD | PIO0_1_PULLUP);
 *
 *  }
 *
 * \endcode
 */
aw_err_t aw_gpio_pin_request(const char *p_name, const int *p_pins, int num);


/**
 * \brief �ͷ�һ�����ţ����ÿ�����ŵ�ʹ�ü�¼��
 *
 * \param[in] p_pins  ָ�������б�
 * \param[in] num     ������
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ERROR  ʧ��
 * \retval -AW_ENXIO  \a pin ������
 *
 * \par ʾ��
 * \code
 * static const uart0_pins_tabe[] = {
 *     PIO0_0,
 *     PIO0_1
 * };
 * aw_gpio_pin_release(uart0_pins_tabe, AW_NELEMENTS(uart0_pins_tabe));
 *
 * \endcode
 */
aw_err_t aw_gpio_pin_release(const int *p_pins, int num);

/**
 * \brief ��ȡGPIO���ŵ�����ֵ/���ֵ
 *
 * ��ȡ���� \a pin ��ǰ������ֵ(\a pin ������ΪGPIO���빦��)�����ֵ
 * (\a pin ������ΪGPIO�������)
 *
 * \par ʵ�����
 * �����ű�����ΪGPIO�������ʱ�� aw_gpio_get() ���صĿ��������õ����ֵ��
 * Ҳ�����������ϵ�ʵ�ʵ�ƽ
 *
 * \param[in] pin   ���ű��
 *
 * \retval  0         \a pin ������ֵ�����ֵΪ0(�͵�ƽ)
 * \retval  1         \a pin ������ֵ�����ֵΪ1(�ߵ�ƽ)
 * \retval -AW_ENXIO  \a pin ������
 *
 * \par ʾ��
 * \code
 * #include "aw_gpio.h"
 *
 * int val;
 *
 * // �������� PIO0_11 ΪGPIO���빦�ܣ���������ʹ��
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_INPUT | AW_GPIO_PULL_UP);
 *
 * val = aw_gpio_get(PIO0_11); //��ȡ���� PIO0_11 ������ֵ
 *
 * // �������� PIO0_11 ΪGPIO�������
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT);
 *
 * val = aw_gpio_get(PIO0_11); //��ȡ���� PIO0_11 ��ǰ�����ֵ
 * \endcode
 */
aw_err_t aw_gpio_get(int pin);

/**
 * \brief ����GPIO���ŵ����ֵ
 *
 * �������� \a pin �����ֵ (\a pin ��Ҫ������ΪGPIO�������)
 *
 * \param[in] pin   ���ű��
 * \param[in] value ���ŵ����ֵ��Ϊ0ʱ���������0(�͵�ƽ)��Ϊ������0ֵʱ��
 *                  �������1(�ߵ�ƽ)
 *
 * \retval AW_OK      �ɹ�
 * \retval -AW_ENXIO  \a pin ������
 * \retval -AW_EIO    �޷�����\a pin �����ֵ (���磬pin������ΪGPIO���빦��)
 *
 * \par ʾ��
 * \code
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT); // ���� PIO0_11 ΪGPIO�������
 * aw_gpio_set(PIO0_11, 0);                  // PIO0_11 ���0 (�͵�ƽ)
 * aw_gpio_set(PIO0_11, 1);                  // PIO0_11 ���1 (�ߵ�ƽ)
 * \endcode
 */
aw_err_t aw_gpio_set(int pin, int value);

/**
 * \brief ��תGPIO���ŵ����ֵ
 *
 * ��תGPIO���� \a pin �����ֵ (\a pin ��Ҫ������ΪGPIO�������):
 * �ߵ�ƽ��͵�ƽ; �͵�ƽ��ߵ�ƽ��
 *
 * \param[in] pin   ���ű��
 *
 * \retval  0         \a pin ����תΪ�͵�ƽ
 * \retval  1         \a pin ����תΪ�ߵ�ƽ
 * \retval -AW_ENXIO  \a pin ������
 * \retval -AW_EIO    �޷�����\a pin �����ֵ (���磬pin������ΪGPIO���빦��)
 *
 * \par ʾ��
 * \code
 * #include "aw_gpio.h"
 *
 * aw_gpio_pin_cfg(PIO0_11, AW_GPIO_OUTPUT); // ���� PIO0_11 ΪGPIO�������
 * aw_gpio_toggle(PIO0_11);                  // ��ת PIO0_11 �������ƽ
 * \endcode
 */
aw_err_t aw_gpio_toggle(int pin);

/**
 * \brief �������ŵĴ�������
 *
 * �����ⲿ�жϹ��ܵ����ſ������ӻص������������ŵ������ƽ/�������㡰����������
 * ���Ҵ�������ʱ������������ӵ������ϵĻص�������
 * ʹ�ñ����������������ŵġ��������������ص�������������ο�����
 * aw_gpio_trigger_connect()�������Ŀ����ر���ο�����
 * aw_gpio_trigger_on() �� aw_gpio_trigger_off()��
 *
 * \param[in] pin       ���ű��
 * \param[in] flags     ���ñ�־��֧�ֵı�־��:
 *                      - #AW_GPIO_TRIGGER_HIGH
 *                      - #AW_GPIO_TRIGGER_LOW
 *                      - #AW_GPIO_TRIGGER_RISE
 *                      - #AW_GPIO_TRIGGER_FALL
 *                      - #AW_GPIO_TRIGGER_BOTH_EDGES
 *
 * \retval AW_OK        �����ɹ�
 * \retval -AW_ENXIO    \a pin ������
 * \retval -AW_ENOTSUP  ���������Ų�֧�ֵ�����
 *
 * \par ʾ��
 * ��ο� aw_gpio_trigger_connect()
 */
aw_err_t aw_gpio_trigger_cfg(int pin, uint32_t flags);

/**
 * \brief ����һ���ص�����������
 *
 * �����ⲿ�жϹ��ܵ����ſ������ӻص������������ŵ������ƽ/�������㡰����������
 * ���Ҵ�������ʱ������������ӵ������ϵĻص�������ʹ�ñ��������ӻص�������
 * ʹ�ú��� aw_gpio_trigger_on() �� aw_gpio_trigger_off() ���ֱ����͹رմ�����
 * ʹ�ú��� aw_gpio_trigger_cfg() �豸�������͡�
 *
 * \attention �����ñ�����ʱ�����ű��Զ�����Ϊ�ⲿ�жϹ��ܡ�
 *
 * \par ʵ�����
 * ������û���ⲿ�жϹ��ܣ��������ӻص������������ش��� -ENOTSUP ��
 * ƽ̨����֧�����Ӷ���ص�������ͬһ�������ϣ���������������ʱ�����ᰴ�����ӵ�
 * �Ⱥ�˳����ø������ص�������(�����ݡ��ص�������)��
 * ƽ̨Ҳ����ֻ֧������һ���ص�������һ�������ϣ������Ѿ������ӵ����Ž��᷵��
 * ���� -ENOSPC �������ȵ��� aw_gpio_trigger_disconnect() �Ͽ���ǰ�����ӵĻص�
 * �������ſ��Գɹ������µĻص�������
 *
 * \param[in]   pin             ���ű��
 * \param[in]   pfunc_callback  �����ص�����
 * \param[in]   p_arg           ���ݸ��ص������Ĳ���
 *
 * \retval      AW_OK          �ɹ�
 * \retval     -AW_ENXIO       \a pin ������
 * \retval     -AW_EFAULT      \a pfunc_callback Ϊ NULL
 * \retval     -AW_ENOSPC      �ռ䲻��
 * \retval     -AW_ENOTSUP     ��֧��
 *
 * \par ʾ���������÷�
 * \code
 *  #include "aw_gpio.h"
 *
 *  void my_callback(void *p_arg)
 *  {
 *      int my_arg = (int)p_arg; //my_arg = 5
 *      //�жϴ�������
 *  }
 *
 *  // ���ӻص�����������
 *  aw_gpio_trigger_connect(PIO0_11, my_callback, (void *)5);
 *
 *  // �������Ŵ������ͣ��½��ش���
 *  aw_gpio_trigger_cfg(PIO_11, AW_GPIO_TRIGGER_FALL);
 *
 *  // ���������ϵĴ�����
 *  aw_gpio_trigger_on(PIO0_11);
 * \endcode
 *
 * \par ʾ�������»ص�����
 * ��ʵ��ֻ֧��һ����������һ���ص�����ʱ�������·�ʽ���»ص�����:
 * \code
 *  // �Ͽ��ɵĻص�����
 *  aw_gpio_trigger_disconnect(PIO0_11, my_callback, (void *)5);
 *
 *  // �����µĻص�����
 *  aw_gpio_trigger_connect(PIO0_11, new_callback, (void *)6);
 * \endcode
 *
 * \see aw_gpio_trigger_disconnect(), aw_gpio_trigger_on(), aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_connect(int             pin,
                                 aw_pfuncvoid_t  pfunc_callback,
                                 void           *p_arg);

/**
 * \brief �Ͽ����ŵ�һ���ص�����
 *
 * �����������ŶϿ�һ������ڵ�ַ��Ϊ \a pfunc_callback ������Ϊ�� \a p_arg
 * �Ļص�������
 *
 * \param[in]   pin             ���ű��
 * \param[in]   pfunc_callback  Ҫ�Ͽ��Ļص������ġ���ڵ�ַ��
 * \param[in]   p_arg           Ҫ�Ͽ��Ļص������ġ�������
 *
 * \retval     AW_OK        �ɹ�
 * \retval     -AW_ENXIO    \a pin ������
 * \retval     -AW_ENOENT   δ�ҵ�ָ���Ļص�����
 *
 * \par ʾ��
 * ��ο� aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_connect(), aw_gpio_trigger_on(), aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_disconnect(int             pin,
                                    aw_pfuncvoid_t  pfunc_callback,
                                    void           *p_arg);

/**
 * \brief �ر������ϵĴ�����
 *
 * �������ر��������ӵ����ŵĴ�����
 *
 * \param[in]   pin     ���ű��
 *
 * \retval      AW_OK     �ɹ�
 * \retval     -AW_ENXIO  \a pin ������
 *
 * \par ʾ��
 * ��ο� aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_on()
 */
aw_err_t aw_gpio_trigger_off(int pin);

/**
 * \brief �������ŵĴ���
 *
 * �������������ŵĴ���
 *
 * \param[in]   pin     ���ű��
 *
 * \retval     AW_OK      �ɹ�
 * \retval     -AW_ENXIO  \a pin ������
 *
 * \par ʾ��
 * ��ο� aw_gpio_trigger_connect()
 *
 * \see aw_gpio_trigger_off()
 */
aw_err_t aw_gpio_trigger_on(int pin);

/** @} grp_aw_if_gpio */

#ifdef __cplusplus
}
#endif

#endif /* __AW_GPIO_H */

/* end of file */
