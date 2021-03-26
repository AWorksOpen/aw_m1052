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
 * \brief �����������ӿ�
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_sensor.h
 *
 * ��һ��ϵͳ�У����ܴ��ڶ������͵Ĵ����������磺�¶ȡ�ʪ�ȡ�������ѹǿ�ȡ�ͬ��
 * ���͵Ĵ�����Ҳ���ܴ��ڶ�������磬��������10���¶ȴ������Բ���10���¶ȼ���
 * ���¶ȡ��ر�أ����ִ��������Բɼ���·�źţ����磬��ʪ�ȴ�����SHT11���Բɼ�
 * �¶Ⱥ�ʪ�ȡ�
 *
 * Ϊ����������ʹ�ô�����ͨ������ʾһ·�źŵĲɼ�������ֻ�ܲɼ���һ�źŵĴ�������
 * ÿ��������ֻ��Ϊϵͳ�ṩһ·������ͨ�������ڿ��Բɼ���·�źŵĴ����������磺
 * SHT11������ÿ������������Ϊϵͳ�ṩ��·������ͨ����Ҳ���ܴ��ڶ��ͬ�ִ�������
 * �Ա�Ϊϵͳ�ṩ����Ĵ�����ͨ����
 *
 * Ϊ�˱������ָ���������ͨ������AWorks�У�Ϊÿ��������ͨ��������Ψһ id������
 * ����ʹ��id��Ϊ��ؽӿڵĲ�����ָ��Ҫ������ͨ����
 *
 * ����ϵͳ�Ĵ�������Դ���£�
 *
 * - 10·�¶ȴ�����ͨ��
 * - 3·��������x,y,z�����ٶȴ�����ͨ��
 * - 3·��������x,y,z�����ٶȴ�����ͨ��
 * - 8·ʪ�ȴ�����ͨ��
 * - 1·ѹǿ������ͨ��
 * - 2·���մ�����ͨ��
 *
 * ��ϵͳ id �ķ������Ϊ��
 *
 * - 0  ~  9 �� 10·�¶ȴ�����ͨ��
 * - 10 ~ 12 �� 3·��������x,y,z�����ٶȴ�����ͨ��
 * - 13 ~ 15 �� 3·��������x,y,z�����ٶȴ�����ͨ��
 * - 16 ~ 23 �� 8·ʪ�ȴ�����ͨ��
 * -   24    �� 1·ѹǿ������ͨ��
 * - 25 ~ 26 �� 2·���մ�����ͨ��
 *
 * ʵ���У�id �����Ӳ��ƽ̨��أ�ͬ���ʹ�����ͨ���� id ��һ���������û�Ӧ�鿴
 * SDK�е��û��ֲ��˽�ϵͳ���еĴ�����ͨ����Դ������ȷʹ�ø���������ͨ����
 *
 * \par ʹ��ʾ��1��һ�λ�ȡ����ͨ�������ݣ�
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  current;               // ���ڴ洢��ȡ���ĵ���ֵ
 *     
 *     aw_sensor_enable(0);                    // ʹ��ͨ��0
 
 *     // ��ȡͨ��0��ֵ���ٶ�ͨ��0Ϊ����������ͨ��
 *     int ret = aw_sensor_data_get(0, &current);
 *     
 *     if (ret == AW_OK) {
 *         // ���ݶ�ȡ�ɹ������Խ�����ֵͨ�����Դ��ڴ�ӡ������
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              current.val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(current.unit));
 *
 *         // �����������ϵͳЧ��Ӱ�첻�󣬿���ֱ��ʹ�ù�ʽ�����Ի�׼��λAΪ
 *         // ��λ�ĵ���ֵ��
 *         // float c = current.val * pow(10, current.unit);  // ����ֵ����λ��A
 *
 *    } else {
 *         // ���ݻ�ȡʧ��
 *    }
 * \endcode
 *
 * \par ʹ��ʾ��2��һ�λ�ȡ���ͨ�������ݣ�
 * \code
 *     #include "aw_sensor.h"
 *
 *     const int        sensor[4] = {0, 2, 3, 5}; // �ٶ�Ӧ����Ҫʹ��4��ͨ��
 *     aw_sensor_val_t  buf[4];                   // �洢4ͨ�����������ݵĻ�����
 *     
 *     // ʹ��ͨ����buf���ڴ洢ÿһ��ͨ��ʹ�ܵĽ�������Ǵ���������
 *     aw_sensor_group_enable(sensor, 4, buf);
 * 
 *     // ��ȡ����
 *     int ret = aw_sensor_group_data_get(sensor, 4, buf);
 *     
 *     if (ret == AW_OK) {
 *         // ���ݶ�ȡ�ɹ�����Ϊ��Чֵ������ʹ��AW_SENSOR_VAL_IS_VALID()�����
 *         // ��һ�жϡ����Խ�����ֵͨ�����Դ��ڴ�ӡ������
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              buf[0].val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(buf[0].unit));
 *
 *         // �����������ϵͳЧ��Ӱ�첻�󣬿���ֱ��ʹ�ù�ʽ�����Ի�׼��λAΪ
 *         // ��λ�ĵ���ֵ��
 *         // float c = buf[0].val * pow(10, buf[0].unit);   // ����ֵ����λ��A
 *
 *    // �������ݶ�ȡʧ�ܵ�ͨ��
 *    } else {
 *        int i;
 *        for (i = 0; i < 4; i++) {
 *            if (AW_SENSOR_VAL_IS_VALID(buf[i])) {
 *                // ��ͨ��������Ч����������ʹ��
 *            } else {
 *                // ��ͨ��������Ч�����ݻ�ȡʧ�ܣ�ʧ��ԭ���ͨ��buf[i].val�ж�
 *            }
 *        }
 *    }
 * \endcode
 *
 * \internal
 * \par modification history
 * - 2.00 18-05-09  tee, modified some interface, sensor type and attributes.
 * - 1.00 16-08-05  ebi, first implementation.
 * \endinternal
 */

#ifndef __AW_SENSOR_H
#define __AW_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sensor
 * \copydoc aw_sensor.h
 * @{
 */

/**
 * \name ���������Ͷ���
 * @{
 */
#define AW_SENSOR_TYPE_VOLTAGE     (1)  /**< \brief ��ѹ����λ: V             */
#define AW_SENSOR_TYPE_CURRENT     (2)  /**< \brief ��������λ: A             */
#define AW_SENSOR_TYPE_TEMPERATURE (3)  /**< \brief �¶ȣ���λ: ���϶�        */
#define AW_SENSOR_TYPE_PRESS       (4)  /**< \brief ѹǿ����λ: ��˹��        */
#define AW_SENSOR_TYPE_LIGHT       (5)  /**< \brief ���նȣ���λ: �տ�˹      */
#define AW_SENSOR_TYPE_ALTITUDE    (6)  /**< \brief ���Σ���λ: ��            */
#define AW_SENSOR_TYPE_DISTANCE    (7)  /**< \brief ���룬��λ: ��            */

/** \brief ֱ��<=1um��΢��Ũ�ȣ���λ: ug/m^3 */
#define AW_SENSOR_TYPE_PM_1             (100)

/** \brief ֱ��<=2.5um��΢��Ũ�ȣ���λ: ug/m^3 */
#define AW_SENSOR_TYPE_PM_2_5           (101)

 /** \brief ֱ��<=10um��΢��Ũ�ȣ���λ: ug/m^3 */
#define AW_SENSOR_TYPE_PM_10            (102)

/** \brief ���ʪ�ȣ�ֵΪ�ٷֱȣ�1��ʾ 1% */
#define AW_SENSOR_TYPE_HUMIDITY         (103)

/** \brief �ӽ�ʽ����������ֵ�źţ�1 ��ʾ�нӽ��� */
#define AW_SENSOR_TYPE_PROXIMITY        (104)

/** \brief ���򣬽Ƕ��� 0/90/180/270 �ֱ��ʾ����/����/����/���� */
#define AW_SENSOR_TYPE_ORIENT           (105)

/** 
 * \brief ���ٶȣ���λ: m/s^2
 *
 * ���ִ��������Լ��������ļ��ٶȣ����磺x,y,z �ᣬ��������£���ʹ��ͨ����
 * ���ֲ�ͬ����ļ��ٶȣ�x��y��z ��ʾ�˲�ͬ��ͨ����
 */
#define AW_SENSOR_TYPE_ACCELEROMETER    (200)

/** 
 * \brief ���ٶȣ������ǲ���������λ: rad/s
 *
 * ���ִ��������Լ��������Ľ��ٶȣ����磺x,y,z �ᣬ��������£���ʹ��ͨ����
 * ���ֲ�ͬ����Ľ��ٶȣ�x��y��z ��ʾ�˲�ͬ��ͨ����
 */
#define AW_SENSOR_TYPE_GYROSCOPE        (201)

/** 
 * \brief �Ÿ�Ӧǿ�ȣ���λ: ��˹
 *
 * ���ִ��������Լ��������ĴŸ�Ӧǿ�ȣ����磺x,y,z �ᣬ��������£���ʹ��ͨ
 * �������ֲ�ͬ����ĴŸ�Ӧǿ�ȣ�x��y��z ��ʾ�˲�ͬ��ͨ����
 */
#define AW_SENSOR_TYPE_MAGNETIC         (202)

/** 
 * \brief ��תʸ��(Rotation Vector)���Ƕ��ơ�
 *
 * ��תʸ��ͨ����Ϊ����������������ת�Ƕȣ� x,y,z �ᣬ��������£���ʹ��ͨ
 * �������ֲ�ͬ�ķ���
 */
#define AW_SENSOR_TYPE_ROTATION_VECTOR  (203)
 
/** @} */

/**
 * \name ����������ģʽ����
 * @{
 */
 
/** 
 * \brief ����׼����������
 */
#define AW_SENSOR_TRIGGER_DATA_READY   (1ul << 0)
 
/**
 * \brief ���޴���
 *
 * ��������Σ������ﵽ #AW_SENSOR_ATTR_DURATION_DATA ����ֵ���������ݾ�����
 * ���ޣ� #AW_SENSOR_ATTR_THRESHOLD_UPPER ����ֵ�����������
 *�� #AW_SENSOR_ATTR_THRESHOLD_LOWER ����ֵ��ʱ������
 */
#define AW_SENSOR_TRIGGER_THRESHOLD    (1ul << 1)

/** 
 * \brief ������б�ʣ�����
 *
 * ��������Σ������ﵽ #AW_SENSOR_ATTR_DURATION_SLOPE ����ֵ������������
 * ������һ�β������ݵ������������������ޣ� #AW_SENSOR_ATTR_THRESHOLD_SLOPE ��ʱ
 * ������
 */
#define AW_SENSOR_TRIGGER_SLOPE        (1ul << 2)

/** 
 * \brief �������͵Ĵ���
 *
 * ��ӽ�ʽ���������������⴫���������ഫ�����������ź��Ƕ�ֵ�źţ���ֵΪ��ʱ
 *����0��������
 */
#define AW_SENSOR_TRIGGER_BOOL         (1ul << 3)

/** @} */

/**
 * \name ������ͨ�����Զ���
 * @{
 */
 
/** \brief ����Ƶ�ʣ�ÿ������Ĵ��� */
#define AW_SENSOR_ATTR_SAMPLING_RATE          (1)

/** \brief ������ֵ */
#define AW_SENSOR_ATTR_FULL_SCALE             (2)

/** \brief ƫ��ֵ�����ھ�̬У׼���������ݣ����������� = ʵ�ʲ���ֵ + offset */
#define AW_SENSOR_ATTR_OFFSET                 (3)

/** \brief �����ޣ��������޴���ģʽ�����ݵ��ڸ�ֵʱ���� */
#define AW_SENSOR_ATTR_THRESHOLD_LOWER        (4)

/** \brief �����ޣ��������޴���ģʽ�����ݸ��ڸ�ֵʱ���� */
#define AW_SENSOR_ATTR_THRESHOLD_UPPER        (5)

/** \brief �������ޣ������������������ݱ仯���죨б�ʹ��󣩣�������ֵʱ���� */
#define AW_SENSOR_ATTR_THRESHOLD_SLOPE        (6)

/**
 * \brief �������ݳ����������������޴���ģʽ
 *
 * ����ֵ��Чʱ�����ʾ������Σ������ﵽ������ֵ���������ݾ��������޻��������
 * ʱ�Ŵ�����
 */
#define AW_SENSOR_ATTR_DURATION_DATA          (7)

/**
 * \brief ��������������������������ģʽ
 *
 * ����ֵ��Чʱ�����ʾ������Σ������ﵽ������ֵ�����������������һ�β�������
 * ��������������������ʱ�Ŵ�����
 */
#define AW_SENSOR_ATTR_DURATION_SLOPE         (8)

/** @} */


/**
 * \name ��λ���壬���ڴ��������ݵ� unit ����
 *
 * \anchor table_unit
 *
 * \par ������λǰ׺��
 *
 * | ָ����10���ݣ� | ���ţ�Symbol�� | Ӣ��ǰ׺ |  ���Ķ���  |
 * | :------------: | :----------- : | :------: |:---------: |
 * |       24       |        Y       |   Yotta  |  Ң������  |
 * |       21       |        Z       |   Zetta  |  ������  |
 * |       18       |        E       |    Exa   | ���������� |
 * |       15       |        P       |   Peta   |  �ģ�����  |
 * |       12       |        T       |   Tera   |  ̫������  |
 * |        9       |        G       |   Giga   |  ��������  |
 * |        6       |        M       |   Mega   |     ��     |
 * |        3       |        k       |   kilo   |     ǧ     |
 * |        2       |        h       |   hecto  |     ��     |
 * |        1       |        da      |   deca   |     ʮ     |
 * |        0       |        -       |     -    |     -      |
 * |       -1       |        d       |   deci   |     ��     |
 * |       -2       |        c       |   centi  |     ��     |
 * |       -3       |        m       |   milli  |     ��     |
 * |       -6       |        ��       |   micro  |     ΢     |
 * |       -9       |        n       |   nano   |  �ɣ�ŵ��  |
 * |       -12      |        p       |   pico   |  Ƥ���ɣ�  |
 * |       -15      |        f       |   femto  | �ɣ�ĸ�У� |
 * |       -18      |        a       |   atto   |  �����У�  |
 * |       -21      |        z       |   zepto  | �ƣ����У� |
 * |       -24      |        y       |   yocto  | �ۣ����У� |
 *
 * \note
 *  
 *  - ָ������ 10���ݣ����磬24��ʾ 10^24��ָ��ͨ����3Ϊ���������1000��Ϊ�����
 *  - ����Symbol�����ִ�Сд����ָ����ǰ׺�����У���������Ϊ��д��Ψ���� kilo 
 * �ķ��� "k" ��Сд���������ڴ����¶ȵķ��ţ���д�� "K"��
 *  - ǰ׺������Ӣ�ĵ����У�ͨ�����Ӹ�ǰ׺��ʾ�����������磬���ȵ�λ�׵�Ӣ�ĵ���
 * Ϊ��meter����ǧ�׶�Ӧ�ĵ���Ϊ��kilometer��
 *  - ���Ķ������У������ڵ����ڲ��»���������¿���ʡ�ԡ�
 *  - �٣�10^2����ʮ��10^1�����֣�10 ^-1�����壨10^-2������ʹ�á�
 * @{
 */

#define AW_SENSOR_UNIT_YOTTA      (24)       /**< \brief Ң��������10^(24)    */
#define AW_SENSOR_UNIT_ZETTA      (21)       /**< \brief ��������10^(21)    */
#define AW_SENSOR_UNIT_EXA        (18)       /**< \brief ������������10^(18)  */
#define AW_SENSOR_UNIT_PETA       (15)       /**< \brief �ģ�������10^(15)    */
#define AW_SENSOR_UNIT_TERA       (12)       /**< \brief ̫��������10^(12)    */
#define AW_SENSOR_UNIT_GIGA       (9)        /**< \brief ����������10^(9)     */
#define AW_SENSOR_UNIT_MEGA       (6)        /**< \brief �ף�10^(6)           */
#define AW_SENSOR_UNIT_KILO       (3)        /**< \brief ǧ��10^(3)           */
#define AW_SENSOR_UNIT_HECTO      (2)        /**< \brief �٣�10^(2)           */
#define AW_SENSOR_UNIT_DECA       (1)        /**< \brief ʮ��10^(1)           */
#define AW_SENSOR_UNIT_BASE       (0)        /**< \brief ����10^(0)������Ϊ1  */
#define AW_SENSOR_UNIT_DECI       (-1)       /**< \brief �֣�10^(-1)          */
#define AW_SENSOR_UNIT_CENTI      (-2)       /**< \brief �壬10^(-2)          */
#define AW_SENSOR_UNIT_MILLI      (-3)       /**< \brief ����10^(-3)          */
#define AW_SENSOR_UNIT_MICRO      (-6)       /**< \brief ΢��10^(-6)          */
#define AW_SENSOR_UNIT_NANO       (-9)       /**< \brief �ɣ�ŵ����10^(-9)    */
#define AW_SENSOR_UNIT_PICO       (-12)      /**< \brief Ƥ���ɣ���10^(-12)   */
#define AW_SENSOR_UNIT_FEMTO      (-15)      /**< \brief �ɣ�ĸ�У���10^(-15) */
#define AW_SENSOR_UNIT_ATTO       (-18)      /**< \brief �����У���10^(-18)   */
#define AW_SENSOR_UNIT_ZEPTO      (-21)      /**< \brief �ƣ����У���10^(-21) */
#define AW_SENSOR_UNIT_YOCTO      (-24)      /**< \brief �ۣ����У���10^(-24) */

#define AW_SENSOR_UNIT_INVALID    (-32768)   /**< \brief ��Чֵ������       */

/** @} */


/** \brief �жϴ�����ĳһͨ���������Ƿ���Ч����ֵΪ��ʱ��Ч��������Ч */
#define AW_SENSOR_VAL_IS_VALID(data)    \
             ((data).unit != (int32_t)(AW_SENSOR_UNIT_INVALID))

/**
 * \brief ������������ȡһ����λ��Ӧ��ǰ׺���ţ���"M"���ף���"k"��ǧ��
 *
 * \param[in] unit : ��λ��AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \return �����ַ���������λ��Ч���򷵻�NULL����unitΪ0(#AW_SENSOR_UNIT_BASE)
 * �򷵻ؿ��ַ���: ""���ر�ע�⣬�������ص���һ���ַ�����������һ���ַ�����Ϊ
 * ���ֵ�λ�ķ��ſ�����������ĸ���磺"da"��ʮ����
 */
#define AW_SENSOR_UNIT_SYMBOL_GET(unit) aw_sensor_unit_symbol_get(unit)

/**
 * \brief ��������������
 *
 * ʵ���У����������ݿ���ΪС����Ϊ�˱���Ӱ��ϵͳ���ܣ�AWorks�ں˲�ֱ��ʹ�ø���
 * ��������ʹ�á������� + ����λ������ʾһ�����������ݡ���ʵ�ʴ���������ΪС��ʱ��ֻ
 * ��ʹ�ø�С�ĵ�λ����ʾ���ݣ����ɽ�ʹ����������ȫ����С����
 *
 * ���磬һ����ѹ�������ɼ����ĵ�ѹΪ1.234V����ѹ�Ļ�����λΪV������ÿ�ִ�������
 * �Ͷ�Ӧ�Ļ�����λ�ڶ��崫�������ͺ�ʱ������˵������Ϊ�˱���С�������䵥λ��С
 * Ϊ mV����Ϊ��1234 mV��
 *
 * ����ʹ���� val �� unit �����з������ֱ��ʾ���������ݵ�ֵ�ĵ�λ��
 *
 * - val  : ����������ֵ���з���32λ����
 * - unit ����λ����"M"���ף���"k"��ǧ����"m"��������"��"��΢���� "p"��Ƥ���ȵȡ�
 * ��λʹ��10���ݽ��б�ʾ����"M"��ʾ10^6����unit��ֵΪ6��ͨ������£�unitΪ������
 * �Ա�ʾ��ȷ��С�����λ��
 *
 * ���轫����������ͳһΪ������λ���罫��ѹ����ȫ��ͳһΪ��λV�������ͨ�����¹�
 * ʽ��ɣ�data = val * (10 ^ unit)�����͵ļ����������±�:
 *
 * |   data����ʵֵ����ѹ: V�� |    val    |   unit   |
 * | :-----------------------: | :-------: | :------: |
 * |           2.854           |  2854     |  -3(mV)  |
 * |           1.7             |  1700     |  -3(mV)  |
 * |           2.3433          |  2343300  |  -6(��V)  |
 * |           2888000         |  2888     |   3(kV)  |
 * |           0               |     0     |     0    |
 * |          -2888000         |  -2888    |   3(kV)  |
 * |          -2.3433          |  -2343300 |  -6(��V)  |
 * |          -2.854           |  -2854    |  -3(mV)  |
 * |          -1.7             |  -1700    |  -3(mV)  |
 *
 * \attention �� unit ��ֵΪ -32768 ʱ�����ʾ��ֵΪ��Ч�Ĵ��������ݡ���ʹ����
 * ��group����ؽӿڲ������������ͨ��ʱ��������ֵ��ΪAW_OK�����ʾĳЩͨ������
 * ʧ�ܣ�ʧ�ܵ�ͨ����Ӧ�Ĵ�����ֵ�������Ϊ��Чֵ��ʹ�� AW_SENSOR_VAL_IS_VALID() 
 * ����Կ����ж�ֵ�Ƿ���Ч������ʱ��val��ԱΪ��׼�����룬���Ծݴ��ж�ʧ�ܵ�ԭ��
 */
typedef struct aw_sensor_val {

    /** 
     * \brief ��������ֵ����Ч��Χ��-2147483648 ~ 2147483647
     *
     * �ر�أ��� unit ��ֵΪ (#AW_SENSOR_UNIT_INVALID) ʱ����ʾ��ֵΪ��Ч�Ĵ���
     * ����ֵ����ʱ��val��Ա��ֵ��ʾ��׼�����룬�Ը����û��жϲ���ʧ�ܵ�ԭ��
     * ���磬-AW_ENODEV ��ʾ������ͨ��id�����ڣ��޶�Ӧ�������豸��
     */
    int32_t   val;
 
    /** 
     * \brief ��λ
     *
     * ʹ��ָ����10���ݣ���ʾ�����磺6��ʾ10^6������"M"���ף���3��ʾ10^3��
     * ����"k"��ǧ����-3��ʾ10^-3������"m"��������
     *
     * ������ֱ��ʹ�����֣�����ʹ���Ѿ�����õĵ�λ�꣺AW_SENSOR_UNIT_* 
     * (eg: #AW_SENSOR_UNIT_MILLI)
     *
     * �ر�ע�⣬��ֵΪ (#AW_SENSOR_UNIT_INVALID) ʱ����ʾ��ֵΪ��Ч�Ĵ�������ֵ��
     * ��ʱ��val��Ա��ֵ��ʾ��׼�����룬�Ը����û��жϲ���ʧ�ܵ�ԭ��
     *
     * ͨ������£���λ�Ӳ��淴ӳ��һ���������ľ��ȣ���ˣ���һ�������������̲�
     * �䣬��ô��������ݵĵĵ�λһ��Ҳ����ı䡣
     */
    int32_t   unit;

} aw_sensor_val_t;

/** 
 * \brief �������ص���������
 *
 * \param[in] p_arg       : �û����ûص�����ʱָ�����û�����
 * \param[in] trigger_src : ����Դ��AW_SENSOR_TRIGGER_*�������Ƕ����Ļ�ֵ��
 *
 * \return ��
 */
typedef void (*aw_sensor_trigger_cb_t) (void *p_arg, uint32_t trigger_src);

/**
 * \brief ��ȡĳһ������ͨ�������ͣ����磺�¶ȡ�ʪ�ȡ�ѹǿ��
 *
 * ������ͨ�������ͣ����磺�¶ȡ�ʪ�ȡ�ѹǿ�ȣ�֮�֣���ͬͨ�����ܾ��в�ͬ�����ͣ�
 * ʹ�øýӿڿ��Կ��ٻ�ȡĳһ������ͨ�������͡�
 *
 * \param[in] id : ������ͨ�� id
 * 
 * \retval   >=0      ���������ͣ�AW_SENSOR_TYPE_*(eg: #AW_SENSOR_TYPE_VOLTAGE)
 * \retval -AW_ENODEV ͨ��������
 */
aw_err_t aw_sensor_type_get(int id);

/**
 * \brief ������������ȡһ����λ��Ӧ��ǰ׺���ţ���"M"���ף���"k"��ǧ��
 *
 * \param[in] unit : ��λ��AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \return �����ַ���������λ��Ч���򷵻�NULL����unitΪ0(#AW_SENSOR_UNIT_BASE)
 * �򷵻ؿ��ַ���: ""���ر�ע�⣬�������ص���һ���ַ�����������һ���ַ�����Ϊ
 * ���ֵ�λ�ķ��ſ�����������ĸ���磺"da"��ʮ����
 */
const char * aw_sensor_unit_symbol_get(int32_t unit);

/**
 * \brief �������������ڴ��������ݵ�λ��ת��
 *
 * \param[in] p_val   : ���������ݻ���
 * \param[in] num     : ���������ݸ���
 * \param[in] to_unit : Ŀ�굥λ��AW_SENSOR_UNIT_* (eg: #AW_SENSOR_UNIT_MILLI)
 *
 * \retval AW_OK      ת���ɹ�
 * \retval -AW_EINVAL ת��ʧ�ܣ���������
 * \retval -AW_ERANGE ĳһ����ת��ʧ�ܣ���ת������ᳬ����Χ��������С��λʱ����
 *                    ���֣�,ת��ʧ�ܵ����ݽ�����ԭֵ��ԭ��λ���䣬�û���ͨ����
 *                    λ�Ƿ�ΪĿ�굥λ���ж��Ƿ�ת���ɹ���
 *
 * \note ��λת����ԭ��
 *
 * - ��������λ������unit��ֵ�����ٶ�unit���ӵ�ֵΪn����Ὣval��ֵ����10^n��
 * ���ڴ�������������ʹԭvalֵ�ľ��ȼ�С�����ȼ�Сʱ����ѭ�������뷨�����磺
 * ԭ����Ϊ1860mV��������λת��ΪV����ת���Ľ��Ϊ2V��ԭ����Ϊ1245mV��������λ
 * ת��ΪV����ת���Ľ��Ϊ1V�����ڴ��ھ��ȵ���ʧ����λ������Ӧ�ý���ʹ�á�
 *
 * - ������С��λ����Сunit��ֵ�����ٶ�unit��С��ֵΪn����Ὣval��ֵ����10^n��
 * ��Ӧ�ر�ע�⣬valֵ������Ϊ32λ�з����������ʾ�����ݷ�ΧΪ��-2147483648 ~ 
 * 2147483647����Ӧʹvalֵ����10^n������ݳ����÷�Χ����С��λ�����ھ��ȵ���ʧ��
 * ��Ӧע�����ݵ��������Ӧ��һ��������С��̫С�ĵ�λ��
 * 
 * - �ر�أ���ת��ǰ��ĵ�λû�з����仯����������������ֵ���ֲ��䡣
 */
aw_err_t aw_sensor_val_unit_convert (aw_sensor_val_t *p_buf, int num, int32_t to_unit);

/**
 * \brief ��ȡĳһ������ͨ��������
 *
 * \param[in]  id    : ������ͨ�� id 
 * \param[out] p_val : �����������������
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENODEV  ͨ��������
 * \retval -AW_EPERM   ͨ��δʹ�ܣ�����������
 *
 * \note
 *
 * - ��������ʹ��Ĭ�ϲ���Ƶ�ʣ�����ֵ�� #AW_SENSOR_ATTR_SAMPLING_RATE ������0��
 * ���Զ���������ÿ�ε��øýӿڶ�ȡ���������ݶ�������ͨ��������ͨ�Žӿڣ�I2C��
 * SPI�ȣ�����ɣ��ȽϺ�ʱ����ˣ���������£��ú����������жϻ�����ʹ�á�
 *
 * - ���޸Ĵ���������Ƶ��Ϊ����ֵ����ʱ��ͨ��ʹ�ܺ�ϵͳ���Զ����ղ���Ƶ�ʻ�ȡ
 * �������еĲ���ֵ���������ڲ������С����û�����������׼��������������ÿ������
 * ���������󣬶��������û��ص���������������£����øýӿڽ�ֱ�Ӵ��ڲ�������
 * ��ȡ���ݣ�Ч�ʺܸߣ��������ж���ʹ�á���Ӧ�����ʱ�ϳ��Ĵ���Ӱ����һ���Զ�
 * �ɼ���
 *
 * - ʵ���У����ִ��������ܿ����ṩ��ͨ�����������ݣ����磬��ʪ�ȴ�����SHT11����
 * ͬʱ�ṩһ·�¶Ⱥ�һ·ʪ�����ݡ�Ϊ����߻�ȡ���ݵ�Ч�ʣ���Ӧ�ó�����Ҫ��ȡ��
 * ͨ���Ĵ���������ʱ������ʹ��  aw_sensor_group_data_get() ����һ���Ի�ȡһ�飨
 * ���ͨ������ֵ�������Ƕ�ε��ñ�������һ�λ�ȡһ��ͨ����ֵ��
 *
 * \par ʹ��ʾ��
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  current;               // ���ڴ洢��ȡ���ĵ���ֵ
 *     
 *     // ��ȡͨ��0��ֵ���ٶ�ͨ��0Ϊ����������ͨ��
 *     int ret = aw_sensor_data_get(0, &current);
 *     
 *     if (ret == AW_OK) {
 *         // ���ݶ�ȡ�ɹ������Խ�����ֵͨ�����Դ��ڴ�ӡ������
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              current.val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(current.unit));
 *
 *         // �����������ϵͳЧ��Ӱ�첻�󣬿���ֱ��ʹ�ù�ʽ�����Ի�׼��λAΪ
 *         // ��λ�ĵ���ֵ��
 *         // float c = current.val * pow(10, current.unit);  // ����ֵ����λ��A
 *
 *    } else {
 *         // ���ݻ�ȡʧ��
 *    }
 * \endcode
 */
aw_err_t aw_sensor_data_get(int id, aw_sensor_val_t *p_val);

/**
 * \brief ��ȡһ�鴫����ͨ�������ͨ����������
 *
 * ʵ��Ӧ���У����ִ����������ṩ��ͨ�����������ݣ����磬��ʪ�ȴ�����SHT11�����
 * ��ͬʱ�ṩһ·�¶Ⱥ�һ·ʪ�����ݡ����ڴˣ�Ϊ����߻�ȡ���ݵ�Ч�ʣ���Ӧ�ó���
 * ��Ҫ��ȡ��ͨ���Ĵ���������ʱ������ʹ�ñ�����һ���Ի�ȡ���ͨ����ֵ��
 * 
 * \param[in]  p_ids ��������ͨ�� id �б�
 * \param[in]  num   : ͨ����Ŀ
 * \param[out] p_buf �������������������������СӦ���� num һ��
 *
 * \note ����������ȡ start_id ~ (start_id + num - 1) ���д�����ͨ�������ݡ� �� 
 *       id Ϊ 2, num Ϊ10�� ���ȡ���ݵ�ͨ����ΧΪ�� 2 ~ 11��
 *
 * \return  AW_OK  ���ݶ�ȡ�ɹ�����ȡ�����д��������ݾ���Ч
 * \retval   <0    ��׼�����룬���ݶ�ȡʧ�ܣ����ֻ�����ͨ�����ݶ�ȡʧ��
 *
 * \attention ������ֵ��ΪAW_OKʱ����ʾĳЩͨ�����ݶ�ȡʧ�ܣ���ȡʧ�ܵ�ͨ����Ӧ
 *            �Ĵ��������ݽ�Ϊ��Чֵ������ʹ�� AW_SENSOR_VAL_IS_VALID()���ж���
 *            Щͨ�������ݶ�ȡʧ���ˣ���ĳһͨ����Ӧ������Ϊ��Чֵ���򴫸�������
 *            �е� val ֵ��ʾ�˱�׼�����룬�û����Ծݴ��жϸ�ͨ��ʧ�ܵ�ԭ��
 *            ʧ��ԭ�����׼������Ķ�Ӧ��ϵ��ʹ�� aw_sensor_data_get() ��ȡ��ͨ
 *            �����ݵķ���ֵ����һ�¡�
 *
 * \par ʹ��ʾ��
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_sensor_val_t  buf[4];                 // �洢4��ͨ�����������ݵĻ�����
 *     
 *     // ��ȡͨ�� 0 ~ ͨ��3 �����ݣ���ʼͨ���ţ�0��ͨ����Ŀ4
 *     int ret = aw_sensor_data_get(0, 4, buf);
 *     
 *     if (ret == AW_OK) {
 *         // ���ݶ�ȡ�ɹ�����Ϊ��Чֵ������ʹ��AW_SENSOR_VAL_IS_VALID()�����
 *         // ��һ�жϡ����Խ�����ֵͨ�����Դ��ڴ�ӡ������
 *         aw_kprintf("current chan 0 is : %d %sA!\r\n",
 *              buf[0].val,
 *              AW_SENSOR_UNIT_SYMBOL_GET(buf[0].unit));
 *
 *         // �����������ϵͳЧ��Ӱ�첻�󣬿���ֱ��ʹ�ù�ʽ�����Ի�׼��λAΪ
 *         // ��λ�ĵ���ֵ��
 *         // float c = buf[0].val * pow(10, buf[0].unit);   // ����ֵ����λ��A
 *
 *    // �������ݶ�ȡʧ�ܵ�ͨ��
 *    } else {
 *        int i;
 *        for (i = 0; i < 4; i++) {
 *            if (AW_SENSOR_VAL_IS_VALID(buf[i])) {
 *                // ��ͨ��������Ч����������ʹ��
 *            } else {
 *                // ��ͨ��������Ч�����ݻ�ȡʧ�ܣ�ʧ��ԭ���ͨ��buf[i].val�ж�
 *            }
 *        }
 *    }
 * \endcode
 */
aw_err_t aw_sensor_group_data_get (const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_buf);

/**
 * \brief ʹ�ܵ���������ͨ��
 * 
 * ʹ�ܺ󣬴������������������ʼ�ɼ��ⲿ�źš��������˴������Ĳ���Ƶ������ֵ
 * ��#AW_SENSOR_ATTR_SAMPLING_RATE������ֵ��Ϊ0����ϵͳ���Զ���������ֵָ���Ĳ�
 * ��Ƶ�ʻ�ȡ�������еĲ���ֵ�����û���������׼��������������ÿ�����ݲ�������
 * �󣬶��������û��ص��������ر�أ�������Ƶ��Ϊ0��Ĭ�ϣ������Զ�������ÿ�ε�
 * ��aw_sensor_data_get()����ʱ�ŴӴ������л�ȡ�������ݡ�
 * 
 * \param[in]  id  : ������ͨ�� id 
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENODEV  ͨ��������
 */
aw_err_t aw_sensor_enable(int id);

/**
 * \brief ʹ��һ�飨���ͨ����������ͨ��
 * 
 * \param[in] p_ids    ��������ͨ�� id �б�
 * \param[in] num      ��������ͨ����Ŀ
 * \param[in] p_result �����ڴ洢ÿһ��ͨ��ʹ�ܵĽ����������� val ֵ�У�
 *                       ���׼�����뺬��һ�¡�
 *
 * \retval  AW_OK  ����ͨ��ʹ�ܳɹ�
 * \retval   <0    ���ֻ�����ͨ��ʹ��ʧ�ܣ�ÿһ��ͨ��ʹ�ܵĽ������ val ֵ��
 *
 * \attention p_result ָ��Ļ������ڴ洢ÿ��ͨ��ʹ�ܵĽ�������Ǳ�ʾ״̬������
 *            ��Ч�Ĵ��������ݣ�ʹ�� AW_SENSOR_VAL_IS_VALID()�������Ч���ж�ʱ��
 *            ���᷵�ؼ٣���ˣ�û�б�Ҫʹ��AW_SENSOR_VAL_IS_VALID()������жϡ�
 *            ����ͨ�� val ֵ�����жϼ��ɣ�val ֵ�ĺ������׼������Ķ�Ӧ��ϵ��
 *            ʹ��aw_sensor_enable() ʹ�ܵ�ͨ���ķ���ֵ����һ�¡��ر�أ���
 *            p_result ΪNULL�������������ÿ��ͨ��ʹ�ܵĽ����
 */
aw_err_t aw_sensor_group_enable (const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result);

/**
 * \brief ���ܵ���������ͨ��
 * 
 * ���ܺ󣬴��������رգ���������ֹͣ�ɼ��ⲿ�źţ�ͬʱ��ϵͳҲ���ٴӴ������л�
 * ȡ����ֵ�����۲���Ƶ���Ƿ�Ϊ0����
 *
 * \param[in]  id    : ������ͨ�� id 
 * \param[out] p_val : �����������������
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENODEV  ͨ��������
 * \retval -AW_EPERM   ͨ����δʹ�ܣ�����������
 */
aw_err_t aw_sensor_disable(int id);

/**
 * \brief ����һ�飨���ͨ����������ͨ��
 * 
 * \param[in] p_ids    ��������ͨ�� id �б�
 * \param[in] num      ��������ͨ����Ŀ
 * \param[in] p_result �����ڴ洢ÿһ��ͨ�����ܵĽ����������� val ֵ�У�
 *                       ���׼�����뺬��һ�¡�
 *
 * \return  AW_OK  ����ͨ�����ܳɹ�
 * \retval   < 0   ��׼�����룬���ֻ�����ͨ������ʧ��
 *
 * \attention p_result ָ��Ļ������ڴ洢ÿ��ͨ�����ܵĽ�������Ǳ�ʾ״̬������
 *            ��Ч�Ĵ��������ݣ�ʹ�� AW_SENSOR_VAL_IS_VALID()�������Ч���ж�ʱ��
 *            ���᷵�ؼ٣���ˣ�û�б�Ҫʹ�� AW_SENSOR_VAL_IS_VALID()������жϡ�
 *            ����ͨ�� val ֵ�����жϼ��ɣ�val ֵ�ĺ������׼������Ķ�Ӧ��ϵ��
 *            ʹ�� aw_sensor_disable() ���ܵ�ͨ���ķ���ֵ����һ�¡��ر�أ���
 *            p_result ΪNULL�������������ÿ��ͨ�����ܵĽ����
 */
aw_err_t aw_sensor_group_disable (const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_result);

/**
 * \brief ���ô�����ͨ������
 *
 * \param[in] id    : ������ͨ�� id
 * \param[in] attr  : ���ԣ�AW_SENSOR_ATTR_*(eg:#AW_SENSOR_ATTR_SAMPLING_RATE)
 * \param[in] p_val : ����ֵ
 *
 * \retval   AW_OK       �ɹ�
 * \retval  -AW_EINVAL   ��������
 * \retval  -AW_ENOTSUP  ��֧��
 * \retval  -AW_ENODEV   ͨ��������
 * 
 * \note ֧�ֵ���������崫������أ����ִ��������ܲ�֧���κ����ԡ��ú�����
 *       �����жϻ�����ʹ�á�
 */
aw_err_t aw_sensor_attr_set (int id, int attr, const aw_sensor_val_t *p_val);

/**
 * \brief ��ȡ������ͨ������
 *
 * \param[in]  id    : ������ͨ�� id
 * \param[in]  attr  : ���ԣ�AW_SENSOR_ATTR_*(eg:#AW_SENSOR_ATTR_SAMPLING_RATE)
 * \param[out] p_val : ���ڻ�ȡ����ֵ�Ļ���
 *
 * \retval   AW_OK       �ɹ�
 * \retval  -AW_EINVAL   ��������
 * \retval  -AW_ENOTSUP  ��֧��
 * \retval  -AW_ENODEV   ͨ��������
 * 
 * \note ֧�ֵ���������崫������أ����ִ��������ܲ�֧���κ����ԡ��ú�����
 *       �����жϻ�����ʹ�á�
 */
aw_err_t aw_sensor_attr_get (int id, int attr, aw_sensor_val_t *p_val);

/**
 * \brief ���ô�����һ��ͨ����������һ�������ص�����
 *
 * \param[in] id     : ������ͨ�� id 
 * \param[in] flags  : ������־��ָ������������
 * \param[in] pfn_cb : ����ʱִ�еĻص�������ָ��
 * \param[in] p_arg  : �û�����
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENOTSUP ��֧�ֵĴ���ģʽ
 * \retval -AW_ENODEV  ͨ��������
 *
 * \note ֧�ֵĴ���ģʽ����崫������أ����ִ�������֧���κδ���ģʽ������ģʽ
 *       ʱ������Ҫͨ��I2C��SPI��ͨ�Žӿ��봫����ͨ�ţ��ȽϺ�ʱ���ú�����������
 *       �ϻ�����ʹ�á�
 *
 * \par ʹ��ʾ��
 * \code
 *     #include "aw_sensor.h"
 *
 *     aw_local void __sensor_trigger_cb (void *p_arg, uint32_t trigger_src)
 *     {
 *         if (trigger_src & AW_SENSOR_TRIGGER_THRESHOLD) {  // ���ݳ�������
 *             aw_buzzer_on();                               // ����������
 *         }
 *     }
 *
 *     // ����ͨ��0�Ĵ���ģʽ
 *     int ret = aw_sensor_trigger_cfg(0, 
 *                                     AW_SENSOR_TRIGGER_THRESHOLD,
 *                                     __sensor_trigger_cb,
 *                                     NULL);
 *     if (ret < 0) {
 *         // ����ʧ�ܣ����ܲ�֧�ָô���ģʽ
 *     } else {
 *         // ���óɹ����򿪴���
 *         aw_sensor_trigger_on(0); 
 *     }
 * \endcode
 */
aw_err_t  aw_sensor_trigger_cfg (int                       id,
                                 uint32_t                  flags,
                                 aw_sensor_trigger_cb_t    pfn_cb,
                                 void                     *p_arg);
 

/**
 * \brief �򿪴���
 * \param[in] id : ������ͨ�� id 
 * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
 */
aw_err_t aw_sensor_trigger_on (int id);

/**
 * \brief �رմ���
 * \param[in] id : ������ͨ�� id 
 * \return AW_OK���ɹ�������ֵ��ʧ�ܣ������׼����Ŷ��塣
 */
aw_err_t aw_sensor_trigger_off (int id);


/**
 * \brief ��ȡ�����������ַ����͵�λ�ַ���
 */
aw_err_t aw_sensor_type_name_get (int id,  const char **name,  const char **unit);

/** @} grp_aw_if_sensor */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SENSOR_H */

/* end of file */
