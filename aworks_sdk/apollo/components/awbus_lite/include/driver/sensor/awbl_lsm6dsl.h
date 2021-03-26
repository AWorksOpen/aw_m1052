/*******************************************************************************
*                                  AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn
* e-mail:      support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief LSM6DSL (������ٶȺ����������Ǵ�����) ͷ�ļ�
 *
 * \internal
 * \par Modification History
 * - 1.00 18-12-3  wan, first implementation
 * \endinternal
 */

#ifndef __AWBL_LSM6DSL_H
#define __AWBL_LSM6DSL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aworks.h"
#include "awbus_lite.h"
#include "aw_spinlock.h"
#include "awbl_i2cbus.h"
#include "aw_sensor.h"
#include "awbl_sensor.h"
#include "aw_isr_defer.h"

#define AWBL_LSM6DSL_NAME      "awbl_lsm6dsl"

/**
 * \breif ��������������ֵѡ��
 *
 * ���磺��ѡ��156mg�����ʾ���ٶ�X��Y��Z��������ٶȶ��ڡ�156mg��Χʱ����ﵽ��
 * ����ֵ����
 */
typedef enum awbl_lsm6dsl_free_fall {
    LSM6DSL_FREE_FALL_THRESHOLD_156MG,  /**< \breif ������������ֵΪ��156mg */
    LSM6DSL_FREE_FALL_THRESHOLD_219MG,  /**< \breif ������������ֵΪ��219mg */
    LSM6DSL_FREE_FALL_THRESHOLD_250MG,  /**< \breif ������������ֵΪ��250mg */
    LSM6DSL_FREE_FALL_THRESHOLD_312MG,  /**< \breif ������������ֵΪ��312mg */
    LSM6DSL_FREE_FALL_THRESHOLD_344MG,  /**< \breif ������������ֵΪ��344mg */
    LSM6DSL_FREE_FALL_THRESHOLD_406MG,  /**< \breif ������������ֵΪ��406mg */
    LSM6DSL_FREE_FALL_THRESHOLD_469MG,  /**< \breif ������������ֵΪ��469mg */
    LSM6DSL_FREE_FALL_THRESHOLD_500MG   /**< \breif ������������ֵΪ��500mg */
} awbl_lsm6dsl_free_fall_t;

/**
 * \breif �������壬���ѣ�6D/4D����˫������Ծ�ͷǻ�Ծѡ��
 */
typedef enum awbl_lsm6dsl_special_func {
    LSM6DSL_ACTIVITY  = 0x80,           /**< \breif ��Ծ�ͷǻ�Ծ�¼� */
    LSM6DSL_TAP       = 0x48,           /**< \breif ������˫���¼� */
    LSM6DSL_WAKE_UP   = 0x20,           /**< \breif �����¼�*/
    LSM6DSL_FREE_FALL = 0x10,           /**< \breif ���������¼� */
    LSM6DSL_6D_4D     = 0x04,           /**< \breif 6D/4D�¼� */
} awbl_lsm6dsl_special_func_t;

/**
 * \breif �����¼�����ʱ��������Ļ�ȡ
 */
enum awbl_lsm6dsl_wake_up {
    LSM6DSL_WAKE_UP_X,                  /**< \breif �����¼�������X�� */
    LSM6DSL_WAKE_UP_Y,                  /**< \breif �����¼�������Y�� */
    LSM6DSL_WAKE_UP_Z,                  /**< \breif �����¼�������Z�� */
};

/**
 * \breif 6D/4D�¼�����ʱ����ǰ��λ��ȡ
 */
enum awbl_lsm6dsl_6d_4d {
    LSM6DSL_6D_4D_XH,                   /**< \breif ��ǰλ��X���������� */
    LSM6DSL_6D_4D_XL,                   /**< \breif ��ǰλ��X�Ḻ������ */
    LSM6DSL_6D_4D_YH,                   /**< \breif ��ǰλ��Y���������� */
    LSM6DSL_6D_4D_YL,                   /**< \breif ��ǰλ��Y�Ḻ������ */
    LSM6DSL_6D_4D_ZH,                   /**< \breif ��ǰλ��Z���������� */
    LSM6DSL_6D_4D_ZL                    /**< \breif ��ǰλ��Z�Ḻ������ */
};

/**
 * \breif ��˫���¼�����ʱ������״̬�Ļ�ȡ
 */
enum awbl_lsm6dsl_tap {
    LSM6DSL_SINGLE_TAP = (1ul << 0),    /**< \breif �����˵����¼� */
    LSM6DSL_DOUBLE_TAP = (1ul << 1),    /**< \breif ������˫���¼� */
    LSM6DSL_XH_TAP     = (1ul << 2),    /**< \breif X���ᷢ���˵�����˫���¼� */
    LSM6DSL_XL_TAP     = (1ul << 3),    /**< \breif X���ᷢ���˵�����˫���¼� */
    LSM6DSL_YH_TAP     = (1ul << 4),    /**< \breif Y���ᷢ���˵�����˫���¼� */
    LSM6DSL_YL_TAP     = (1ul << 5),    /**< \breif Y���ᷢ���˵�����˫���¼� */
    LSM6DSL_ZH_TAP     = (1ul << 6),    /**< \breif Z���ᷢ���˵�����˫���¼� */
    LSM6DSL_ZL_TAP     = (1ul << 7)     /**< \breif Z���ᷢ���˵�����˫���¼�*/
};

/**
 * \breif ��Ծ�ͷǻ�Ծ�¼�״̬��ȡ
 */
enum awbl_lsm6dsl_activity {
    LSM6DSL_ACTIVITING = (1ul << 0),    /**< \breif ��ǰ������Ծ�¼��������Ծ״̬ */
    LSM6DSL_INACTIVITY = (1ul << 1),    /**< \breif ����ǻ�Ծ״̬ */
};

/**
 * \brief LSM6DSL �豸��Ϣ�ṹ��
 */
typedef struct awbl_lsm6dsl_devinfo {

    /**
     * \brief �ô�����ͨ������ʼid
     *
     * LSM6DSL ����7·�ɼ�ͨ����������ٶȵ�X��Y��Z������������ǵ�X��Y��Z���һ
	 * ·�¶����ݲɼ�ͨ������start_id��ʼ(����start_id)�����߸�ID�������ģ���
	 * �߸�ͨ��������start_id ��Ӧģ��������ٶ�X���ID��start_id + 1 ��Ӧģ����
	 * ����ٶ�Y���ID���Դ����˱��
     * eg�� ��start_id����Ϊ0, ��ID = 0,��Ӧģ���ͨ��1��ID = 1��Ӧģ���ͨ��2.
     */
    int     start_id;

    int     int1_pin;  /*< \brief �������ⲿ��������1 */
    int     int2_pin;  /*< \brief �������ⲿ��������2 */
    uint8_t i2c_addr;  /*< \brief I2C 7λ �豸��ַ */

    /** \brief ƽ̨��ʼ���ص����� */
    void    (*pfunc_plfm_init)(void);

    /**
     * \brief ������ٶȸ�����ģʽ�Ƿ��(�򿪣�1���رգ�0)
     *
     * ���򿪣���������ٶ���12.5HZ�����ϵĿ�ѡƵ���²ɼ����ݾ�ʹ�ø�����ģʽ
	 * ������Ϊ150-160uA����
     * ���رգ���Ƶ����1.6HZ��12.5HZ��26HZ��52HZʱ�����õ͹���ģʽ
	 * ������Ϊ4.5uA-25uA����
     * ��Ƶ��Ϊ104HZ��208HZʱ����������ģʽ������Ϊ44uA-85uA������Ƶ�ʸ���416HZ
	 * ʱ���Զ����ø�����ģʽ������Ϊ150uA-160uA����
     */
    uint8_t xl_hm_mode;

    /**
     * \brief ���������Ǹ�����ģʽ�Ƿ��(�򿪣�1���رգ�0)
     *
     * ���򿪣������������������п�ѡƵ���²ɼ����ݾ�ʹ�ø�����ģʽ������Ϊ555uA����
     * ���رգ���Ƶ����12.5HZ��26HZ��52HZʱ�����õ͹���ģʽ������Ϊ232uA-270uA����
     * ��Ƶ��Ϊ104HZ��208HZʱ����������ģʽ������Ϊ325uA-430uA������Ƶ�ʸ���
	 * 416HZʱ���Զ����ø�����ģʽ������Ϊ555uA����
     */
    uint8_t g_hm_mode;

    /**
     * \brief ������ٶȲ�������ѡ��(����1��0)
     *
     * ������0�����ʾƫ�����ԵĲ���ֵ�ֱ���Ϊ(1/1024)g����󲹳���ΧΪ
	 * (-0.124023g~0.124023g)
     * ������1�����ʾƫ�����ԵĲ���ֵ�ֱ���Ϊ(1/64)g����󲹳���ΧΪ
	 * (-1.984375g~1.984375g)
     */
    uint8_t xl_compensate;

} awbl_lsm6dsl_devinfo_t;

/**
 * \brief LSM6DSL �豸�ṹ��
 */
typedef struct awbl_lsm6dsl_dev {

    /** \brief �̳��� IIC �豸 */
    struct awbl_i2c_device               dev;

    /** \brief sensor ����  */
    awbl_sensor_service_t                sensor_serv;

    /** \brief �ж���ʱ�������� */
    struct aw_isr_defer_job              g_myjob[2];

    /** \brief ������ٶȺ������Ǻ��¶Ȳ���Ƶ�ʣ�ÿ������Ĵ��� */
    aw_sensor_val_t                      sampling_rate[3];

    /** \brief ������ٶȺ������ǵ�ǰ������ */
    aw_sensor_val_t                      full_scale[2];

    /** \brief �򿪺͹ر�ͨ���Ķ�Ӧ��־λ */
    uint8_t                              en_or_dis;

    /** \brief ����ʹ��λ��ÿλ��Ӧһ��ͨ�� */
    uint8_t                              on_or_off;

    /** \brief ͨ�������ص����� */
    aw_sensor_trigger_cb_t               pfn_trigger_fnc[7];

    /** \brief ͨ�������ص��������� */
    void                                *p_argc[7];

    /** \brief ͨ��������־*/
    uint32_t                             flags[7];

    /** \brief ���ٶ�ƫ��������ֵ */
    aw_sensor_val_t                      xl_off_data[3];

    /** \brief ���⹦���¼��ص����� */
    aw_sensor_trigger_cb_t               pfn_basic_fnc[5];

    /** \brief ���⹦���¼��ص��������� */
    void                                *p_basic[5];

} awbl_lsm6dsl_dev_t;

/**
 * \brief ע�� LSM6DSL ����
 *
 * �ú��������û�����
 */
void awbl_lsm6dsl_drv_register (void);

/**
 * \brief �����������幦�ܼ�⺯��
 *
 * �����ٶ�X��Y��Z��������ٶȶ�����ֵ��Χ��Χ�ڣ��Ҵﵽ����ʱ��ʱ���򴥷�������
 * ���¼�
 *
 * \param pfn_fnc      �������������¼�ʱ��ִ�е��û��ص�����
 * \param p_argc       �������������¼�ʱ��ִ�е��û��ص���������
 * \param ff_threshold ������������ֵѡ��
 * \param ff_time      ������ֵ1~63���������峬����ֵ�ӳټ��ʱ��
 *                      ���������ü��ٶ�Ƶ��Ϊ12.5HZ�����ʱ��Ϊ(m_time/12.5)s
 *                      ���������ü��ٶ�Ƶ��Ϊ26HZ�����ʱ��Ϊ(m_time/26)s
 *                      ...
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval  < 0        ʧ�ܣ���Ӧ��׼������
 */
aw_err_t awbl_lsm6dsl_free_fall_on (aw_sensor_trigger_cb_t    pfn_fnc,
                                    void                     *p_argc,
                                    awbl_lsm6dsl_free_fall_t  ff_threshold,
                                    uint8_t                   ff_time);

/**
 * \brief ���û����¼����ܼ�⺯��
 *
 * �����ٶȵı�����������һ������������һ�볬������ֵ����ڸ���ֵ�����Ҵﵽ����
 * ����ʱ��ʱ���򴥷������¼�
 *
 * \param pfn_fnc      ���������¼�ʱ��ִ�е��û��ص�����
 * \param p_argc       ���������¼�ʱ��ִ�е��û��ص���������
 * \param wu_threshold ������ֵ1~63�������¼������ֵѡ��
 *                      ���������ٶȵ�����Ϊ��2g�������ֵΪ��(ff_threshold*(2/64))g
 *                      ���������ٶȵ�����Ϊ��4g�������ֵΪ��(ff_threshold*(4/64))g
 *                      ...
 * \param wu_time      ������ֵ0~3�������¼�������ֵ�ӳټ��ʱ��
 *                       ���������ü��ٶ�Ƶ��Ϊ12.5HZ�����ʱ��Ϊ(wu_time/12.5)s
 *                       ���������ü��ٶ�Ƶ��Ϊ26HZ�����ʱ��Ϊ(wu_time/26)s
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval  < 0        ʧ�ܣ���Ӧ��׼������
 */
aw_err_t awbl_lsm6dsl_wake_up_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                  void                   *p_argc,
                                  uint8_t                 wu_threshold,
                                  uint8_t                 wu_time);

/**
 * \brief ����6D/4D��λ��⹦�ܺ���
 *
 * ��������������λ�仯���ҷ�λ�仯�ĽǶȳ�����ֵʱ���򴥷��ص���������ô�����
 * �ķ�λ��Ϣ
 *
 * \param pfn_fnc      ������λ�仯�¼�ʱ��ִ�е��û��ص�����
 * \param p_argc       ������λ�仯�¼�ʱ��ִ�е��û��ص���������
 * \param d_threshold  ������ֵ0~3����λ�仯�Ƕ���ֵѡ��
 *                      ������Ϊ0�����ʾ��λ�仯�Ƕ���ֵΪ80��
 *                      ������Ϊ1�����ʾ��λ�仯�Ƕ���ֵΪ70��
 *                      ������Ϊ2�����ʾ��λ�仯�Ƕ���ֵΪ60��
 *                      ������Ϊ3�����ʾ��λ�仯�Ƕ���ֵΪ50��
 * \param d6_or_d4     ������ֵ0��1��6D��λ������4D��λ���ѡ��
 *                      ������Ϊ0�����ʾ����6D��λ���
 *                      ������Ϊ1�����ʾ����4D��λ���
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval  < 0        ʧ�ܣ���Ӧ��׼������
 *
 * \note 4D��������6D��������Ӽ�����4D��λ����У�Z���λ�ü��(ZH��ZL)����
 *       ����
 */
aw_err_t awbl_lsm6dsl_6d_4d_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                void                   *p_argc,
                                uint8_t                 d_threshold,
                                uint8_t                 d6_or_d4);

/**
 * \brief ���õ�˫���¼���⹦�ܺ���
 *
 * \param pfn_fnc        ������˫���¼�ʱ��ִ�е��û��ص�����
 * \param p_argc         ������˫���¼�ʱ��ִ�е��û��ص���������
 * \param tap_threshold  ������ֵ1~31����˫���¼�����ֵѡ��
 *                        ���������ٶȵ�����Ϊ��2g�������ֵΪ��(tap_threshold*(2/32))g
 *                        ���������ٶȵ�����Ϊ��4g�������ֵΪ��(tap_threshold*(4/32))g
 *                        ...
 * \param shock_time     ������ֵ1~3����˫���¼��ĸ����𶯳���ʱ��
 *                        ���������ٶ�Ƶ��Ϊ416HZ�����ʱ��Ϊ(shock_time*8/416)s
 *                        ���������ٶ�Ƶ��Ϊ833HZ�����ʱ��Ϊ(shock_time*8/833)s
 * \param quiet_time     ������ֵ1~3��˫���¼��İ���ʱ��
 *                        ���������ٶ�Ƶ��Ϊ416HZ�����ʱ��Ϊ(quiet_time*4/416)s
 *                        ���������ٶ�Ƶ��Ϊ833HZ�����ʱ��Ϊ(quiet_time*4/833)s
 * \param dur_time       ������ֵ1~15��˫���¼��ĳ������ʱ��
 *                        ���������ٶ�Ƶ��Ϊ416HZ�����ʱ��Ϊ(dur_time*8/416)s
 *                        ���������ٶ�Ƶ��Ϊ833HZ�����ʱ��Ϊ(dur_time*8/833)s
 * \param one_or_two     ������ֵ0~2�������¼����û�˫���¼�����
 *                        ������Ϊ0�����ʾֻ���õ����¼�
 *                        ������Ϊ1�����ʾֻ˫���¼�
 *                        ������Ϊ2�����ʾͬʱ���õ�˫���¼�
 *
 * \retval  AW_OK        �ɹ�
 * \retval -AW_EINVAL    ��������
 * \retval -AW_EPERM     ����������
 * \retval  < 0          ʧ�ܣ���Ӧ��׼������
 *
 * \note 1. �ù���ֻ���ڼ��ٶ�Ƶ��Ϊ416HZ��833HZʱʹ�ã�������ʹ�øù���ǰ���޸�
 *          ���ٶ�Ƶ��
 *       2. ���ù����뻽���¼�����ͬʱʹ�ã���ù������õĵ�˫����ֵ(mg)�������
 *          ���ѹ��ܵ���ֵ(mg)
 */
aw_err_t awbl_lsm6dsl_tap_on (aw_sensor_trigger_cb_t  pfn_fnc,
                              void                   *p_argc,
                              uint8_t                 tap_threshold,
                              uint8_t                 shock_time,
                              uint8_t                 quiet_time,
                              uint8_t                 dur_time,
                              uint8_t                 one_or_two);

/**
 * \brief ���û�Ծ�ͷǻ�Ծ��⹦�ܺ���
 *
 * �ù���������ϵͳ���ģ�������ǻ�Ծ״̬ʱ�����ٶȲ���Ƶ���Զ�����Ϊ12.5HZ��
 * �����ǿ���ѡ������
 * ���ַ�ʽ�£�1.�ù��ܴ򿪺������ǵĹ�����ʽ�������仯
 *         2.�ù��ܴ򿪺������ǽ���˯��ģʽ
 *         3.�ù��ܴ򿪺������ǽ���͹���ģʽ
 * ����Ծ״̬����ʱ�����ٶȺ������ǻָ���ԭ�еĹ���״̬
 *
 * \param pfn_fnc        ������Ծ�ͷǻ�Ծ�¼�ʱ��ִ�е��û��ص�����
 * \param p_argc         ������Ծ�ͷǻ�Ծ�¼�ʱ��ִ�е��û��ص���������
 * \param act_threshold  ������ֵ1~63����Ծ�¼������ֵѡ��(����ֵ�뻽���¼�����ֵ��ͬ)
 *                        ���������ٶȵ�����Ϊ��2g�������ֵΪ��(ff_threshold*(2/64))g
 *                        ���������ٶȵ�����Ϊ��4g�������ֵΪ��(ff_threshold*(4/64))g
 *                        ...
 * \param sleep_time     ������ֵ1~15������ǻ�Ծ״̬ǰ�Ļ�Ծ״̬������ʱ��
 *                        �����ڸü��֮ǰ�������ٶ�Ƶ��Ϊ104HZ�����ʱ��Ϊ(sleep_time*512/104)s
 *                        �����ڸü��֮ǰ�������ٶ�Ƶ��Ϊ208HZ�����ʱ��Ϊ(sleep_time*512/208)s
 *                        ...
 * \param gyro_mode      ������ֵ0~2������ǻ�Ծ״̬ʱ�������ǹ���״̬
 *                        ������Ϊ0�����ʾ����ǻ�Ծ״̬�������ǵĹ�����ʽ�������仯
 *                        ������Ϊ1�����ʾ����ǻ�Ծ״̬�������ǽ���˯��ģʽ
 *                        ������Ϊ2�����ʾ����ǻ�Ծ״̬�������ǽ���͹���ģʽ
 *
 * \retval  AW_OK        �ɹ�
 * \retval -AW_EINVAL    ��������
 * \retval -AW_EPERM     ����������
 * \retval  < 0          ʧ�ܣ���Ӧ��׼������
 */
aw_err_t awbl_lsm6dsl_activity_on (aw_sensor_trigger_cb_t  pfn_fnc,
                                   void                   *p_argc,
                                   uint8_t                 act_threshold,
                                   uint8_t                 sleep_time,
                                   uint8_t                 gyro_mode);

/**
 * \brief �ر��������壬���ѣ�6D/4D����˫������Ծ�ͷǻ�Ծ���
 *
 * \param   who_off  ѡ��رյ��¼�
 *
 * \retval  AW_OK     �ɹ�
 * \retval -AW_EINVAL ��������
 * \retval  < 0       ʧ�ܣ���Ӧ��׼������
 */
aw_err_t awbl_lsm6dsl_special_func_off (awbl_lsm6dsl_special_func_t who_off);

#ifdef __cplusplus
}
#endif

#endif /* __AWBL_LSM6DSL_H */

/* end of file */
