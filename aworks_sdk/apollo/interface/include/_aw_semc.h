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

#ifndef __AW_SEMC_H
#define __AW_SEMC_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sem
 * \copydoc aw_sem.h
 * @{
 */

/**
 * \addtogroup grp_aw_if_semc �����ź���
 * @{
 */

typedef aw_psp_semc_id_t    aw_semc_id_t;      /**< \brief �����ź���ID*/

/**
 *  \brief ��������ź���
 *
 * ���궨��һ�������ź���, �ڱ���ʱ����ź��������ڴ�ķ���; ������ʱ, ��Ҫ����
 * AW_SEMC_INIT() ����ź����ĳ�ʼ�������� AW_SEMC_TERMINATE()��ֹ�ź�����
 *
 * ����\a sem ֻ����Ϊ�� AW_SEMC_INIT(), AW_SEMC_TAKE(), AW_SEMC_GIVE()
 * �� AW_SEMC_TERMINATE()�Ĳ�����
 *
 * AW_SEMC_INIT() �����ź�����\a sem_id, \a sem_id ��Ϊ���� aw_semc_take() ,
 * aw_semc_give() �� aw_semc_terminate()�Ĳ���; ��Щ�����Ĺ������Ӧ�ĺ�Ĺ���
 * ��ͬ, ��������, ����ʹ��\a sem_id ��Ϊ����,\a sem_id ������Ϊ�ź����ľ����
 * ��������֮�䴫�ݡ�
 *
 * ����ʹ�ñ��꽫�ź���ʵ��Ƕ�뵽��һ�����ݽṹ��,�����Ļ�,�����Ǹ�������ݶ���
 * ��ʱ����Զ�������ź���ʵ����ڴ����, ������ķ�����ʾ��
 *
 * \attention ���ź���û�п�ģ��(�ļ�)ʹ�õ������Ƽ�ʹ�� AW_SEMC_DECL_STATIC()
 *            ���汾�꣬�����ɱ���ģ��֮���ź���������Ⱦ��
 *
 * \attention �������÷��ⲻ, ��Ī�ں�����ʹ�ô˺��������ź�������Ϊ�ں����ڴ˺�
 *            ������ڴ�������ջ�ϣ�һ���������أ�������ڴ汻�Զ����ա�
 *            �ں����ڲ�ʹ�� AW_SEMC_DECL_STATIC()�������ź�����
 *
 * \param[in] sem �ź���ʵ��, �� AW_SEMC_INIT(), AW_SEMC_TAKE(), AW_SEMC_GIVE()
 *                �� AW_SEMC_TERMINATE() һ��ʹ�á�
 *
 * \par �����������÷�
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_SEMC_DECL_STATIC(sem);
 *
 *  void func_a (void *arg)
 *  {
 *      while (1) {
 *          //...
 *          AW_SEMC_GIVE(sem);                      //�ͷ��ź���
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      while (1) {
 *          AW_SEMC_TAKE(sem, AW_SEM_WAIT_FOREVER); //�ȴ��ź���
 *          //...
 *      }
 *  }
 *
 *  int main() {
 *
 *      // ��ʼ���ź���
 *      AW_SEMC_INIT(sem, AW_SEM_EMPTY, AW_SEM_Q_PRIORITY);
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(task_a,            // ����ʵ��
 *                   "task_a",          // ��������
 *                   5,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_a,            // ������ں���
 *                   NULL);             // ������ڲ���
 *
 *      AW_TASK_INIT(task_b,            // ����ʵ��
 *                   "task_b",          // ��������
 *                   6,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_b,            // ������ں���
 *                   NULL);             // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(task_a);
 *      AW_TASK_STARTUP(task_b);
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \par ���������ź���ʵ��Ƕ����һ�����ݽṹ
 * \code
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);      // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(task_b, 512);      // ����ʵ��task_b, ��ջ512
 *      AW_SEMC_DECL(sem);              // �ź���ʵ��
 *      aw_semb_id_t sem_id;            // �ź���ID
 *  };
 *  struct my_struct   my_object;       // ���������ݶ���
 *
 *  void func_a (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          //...
 *          AW_SEMC_GIVE(p_myobj->sem);  //�ͷ��ź���(ʹ���ź���ʵ��)
 *      }
 *  }
 *
 *  void func_b (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          // �ȴ��ź���(ʹ���ź���ID)
 *          aw_semc_take(p_myobj->sem_id, AW_SEM_WAIT_FOREVER);
 *          //...
 *      }
 *  }
 *
 *  int main() {
 *
 *      // ��ʼ���ź���
 *      my_object.sem_id = AW_SEMC_INIT(my_object.sem,
 *                                      AW_SEM_EMPTY,
 *                                      AW_SEM_Q_PRIORITY);
 *      if (my_object.sem_id == NULL) {
 *          return -1;                      // �ź�����ʼ��ʧ��
 *      }
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(my_object.task_a,      // ����ʵ��
 *                   "task_a",              // ��������
 *                   5,                     // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_a,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      AW_TASK_INIT(my_object.task_b,      // ����ʵ��
 *                   "task_b",              // ��������
 *                   6,                     // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_b,                // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //��ʱ20��
 *
 *      // AW_SEMC_TERMINATE(my_object.sem);    // ��ֹ�ź���
 *      // aw_semc_terminate(my_object.sem_id); // ��ֹ�ź���(ʹ���ź���ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(), AW_SEMC_GIVE,
 *     AW_SEMB_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_DECL(sem)       AW_PSP_SEMC_DECL(sem)

/**
 * \brief ��������ź���(��̬)
 *
 * ������ AW_SEMC_DECL() �Ĺ�����ͬ, ��������, AW_SEMC_DECL_STATIC() �ڶ���
 * �ź��������ڴ�ʱ, ʹ�ùؼ���\b static , ���һ��, ����Խ��ź���ʵ�����������
 * ����ģ����(�ļ���)�Ӷ�����ģ��֮����ź���������ͻ���������ں�����ʹ�ñ��궨
 * ���ź�����
 *
 * \param[in] sem �ź���ʵ��, �� AW_SEMC_INIT(), AW_SEMC_TAKE(), AW_SEMC_GIVE()
 *                �� AW_SEMC_TERMINATE() һ��ʹ�á�
 *
 * \par ����
 * �� AW_SEMC_DECL()
 *
 * \sa AW_SEMC_DECL(), AW_SEMC_INIT(), AW_SEMC_TAKE(), AW_SEMC_GIVE,
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_DECL_STATIC(sem)    AW_PSP_SEMC_DECL_STATIC(sem)

/**
 * \brief ���ü����ź���
 *
 * ��������һ�������ź���, �൱��ʹ�ùؼ�\b extern ����һ���ⲿ���š�
 * ���Ҫʹ����һ��ģ��(�ļ�)�ж�����ź���, ����ʹ�ñ���������ź���, Ȼ���
 * ���Բ������ź�������������ź���Ӧ����ʹ�� AW_SEMC_DECL()����ġ�
 *
 * \param[in] sem �ź���ʵ��
 *
 * \par ����
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_SEMC_IMPORT(sem_x);      // �����ź��� sem_x
 *
 *  int func () {
 *      AW_SEMC_TAKE(sem_x);    // �ȴ��ź��� sem_x
 *  }
 * \endcode
 *
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE(), AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_IMPORT(sem)     AW_PSP_SEMC_IMPORT(sem)

/**
 * \brief �����ź�����ʼ��
 *
 * ��ʼ���� AW_SEMC_DECL() �� AW_SEMC_DECL_STATIC()��̬����ļ����ź���,
 * ��Ƕ�뵽��һ�������ݶ����еĶ������ź�����
 *
 * �����ź�����ѡ������������ڴ��ź�����������Ŷӷ�ʽ, ���԰����������ȼ�
 * ���Ƚ��ȳ��ķ�ʽ�Ŷ�, ���Ƿֱ��Ӧ #AW_SEM_Q_PRIORITY �� #AW_SEM_Q_FIFO ��
 *
 * \param[in]  sem            �ź���ʵ��,�� AW_SEMC_DECL()��
 *                            AW_SEMC_DECL_STATIC()����
 * \param[in]  initial_count  ��ʼ����ֵ, �Ǹ���
 * \param[in]  options        ��ʼ��ѡ���־:
 *                                - #AW_SEM_Q_FIFO
 *                                - #AW_SEM_Q_PRIORITY
 *
 * \return �ɹ����ؼ����ź�����ID, ʧ���򷵻�NULL
 *
 * \par ʾ��
 *  ��ο� AW_SEMC_DECL()
 *
 * \sa AW_SEM_Q_FIFO, AW_SEM_Q_PRIORITY
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_TAKE(), AW_SEMC_GIVE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_INIT(sem, initial_count, options) \
            AW_PSP_SEMC_INIT(sem, initial_count, options)

/**
 * \brief ��ȡ�����ź���
 *
 * ��ȡ�ź���������ź�������ֵΪ0, �����񽫻ᱻ����ֱ���ź�����ÿ���(��������
 * ������ AW_SEMC_GIVE()�� aw_semc_give())������ź����Ѿ��ǿ��õ�(����ֵ > 0),
 * �򱾵��ý���ʹ����ֵ�� 1��
 *
 *  ���� \a timeout ���Ƶ��õ�����:
 *   - Ϊ #AW_SEM_WAIT_FOREVER ʱ, ���ý��ᱻ����, ֱ�����ź������ͷŻ�ɾ��,
 *     ����ͨ������ֵ���жϵ��ý��:AW_OK -�ɹ���ȡ���ź�����-AW_ENXIO -�ź�����
 *     �����ݻ�
 *
 *   - Ϊ #AW_SEM_NO_WAIT ʱ,���ò��ᱻ����, ��������, ͨ������ֵ���жϵ��ý��;
 *     AW_OK -�ɹ���ȡ���ź���; -AW_EAGAIN -δ��ȡ���ź���,�Ժ�����;
 *     -AW_ENXIO -�ź����Ѿ�����ֹ
 *
 *   - ����ֵΪ��ʱʱ��, ��ʱ�����г�ʱ������, ���趨�ĳ�ʱʱ�䵽��ʱ, ����δ��
 *     ȡ���ź���, ����÷���, ����ͨ������ֵ���жϵ��ý���� AW_OK -�ɹ���ȡ��
 *     �ź���; -AW_ETIME -��ʱ,δ��ȡ���ź���,�Ժ�����; -AW_ENXIO -�ź����Ѿ���
 *     ��ֹ
 *
 *   - ��������ֵΪ�Ƿ�����
 *
 * \param[in] sem      �ź���ʵ��,�� AW_SEMC_DECL() �� AW_SEMC_DECL_STATIC() ����
 * \param[in] timeout  ���ȴ�ʱ��(��ʱʱ��), ��λ��ϵͳ����, ϵͳʱ�ӽ���Ƶ��
 *                     �ɺ��� aw_sys_clkrate_get()���,  ��ʹ�� aw_ms_to_ticks()
 *                     ������ת��Ϊ������; #AW_SEM_NO_WAIT Ϊ������;
 *                     #AW_SEM_WAIT_FOREVER Ϊһֱ�ȴ�, �޳�ʱ
 *
 * \retval   AW_OK      �ɹ�
 * \retval  -AW_EINVAL  ������Ч
 * \retval  -AW_EPERM   ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME   ��ȡ�ź�����ʱ
 * \retval  -AW_ENXIO   �ź����Ѿ�����ֹ
 * \retval  -AW_EAGAIN  ��ȡ�ź���ʧ��, �Ժ����� (\a timeout = #AW_SEM_NO_WAIT)
 *
 * \par ����
 * ���෶����ο� AW_SEMC_DECL()
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_SEMC_TAKE(sem, AW_SEM_WAIT_FOREVER);         // ���õȴ�
 *
 *  aw_err_t ret;
 *  ret = AW_SEMC_TAKE(sem, aw_ms_to_ticks(500));   //��ʱ�ȴ�,��ʱʱ�� 500ms
 *  if (ret == -AW_ETIME) {                         // �ȴ��ź�����ʱ
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = AW_SEMC_TAKE(sem, AW_SEM_NO_WAIT);        // �޵ȴ�
 *  if (ret == -AW_EAGAIN) {                        // δ��ȡ���ź���
 *      //...
 *  }
 * \endcode
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_GIVE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_TAKE(sem, timeout)  AW_PSP_SEMC_TAKE(sem, timeout)

/**
 * \brief �ͷż����ź���
 *
 * �ͷ��ź���������и������ȼ��������ȡ�˸��ź���, �������׼����������, ����
 * ��������ռ���� aw_semc_give() ���������ź���������(���ź������ͷ�, ����û
 * �������ȡ), ��û���κβ�������������ʹ�ź����ļ���ֵ��1��
 *
 * \param[in] sem   �ź���ʵ��,�� AW_SEMC_DECL() �� AW_SEMC_DECL_STATIC() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_GIVE(sem)   AW_PSP_SEMC_GIVE(sem)

/**
 * \brief ���ü����ź���
 *
 * \param[in] sem         �����ź���ʵ��,�� AW_SEMC_DECL() ��
 *                        AW_SEMC_DECL_STATIC() ����
 * \param[in] reset_value �����ź���������ֵ
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_RESET(sem, reset_value )   AW_PSP_SEMC_RESET(sem, reset_value)

/**
 * \brief ��ֹ�����ź���
 *
 * ��ֹ�ź������κεȴ����ź��������񽫻������, ������ -AW_ENXIO ���˺��������ͷ�
 * �ź�����ص��κ��ڴ档
 *
 * \param[in]  sem  �ź���ʵ��,�� AW_SEMC_DECL() �� AW_SEMC_DECL_STATIC() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE()
 * \sa aw_semc_take(), aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
#define AW_SEMC_TERMINATE(sem)  AW_PSP_SEMC_TERMINATE(sem)

/**
 * \brief ��ȡ�����ź���
 *
 * �������Ĺ����� AW_SEMC_TAKE() ��ͬ, �������, ������ʹ��\a sem_id ��Ϊ������
 * \a sem_id �� AW_SEMC_INIT() ���ء�
 *
 * \param[in] sem_id   �ź���ID, �� AW_SEMC_INIT()����
 * \param[in] timeout  ���ȴ�ʱ��(��ʱʱ��), ��λ��ϵͳ����, ϵͳʱ�ӽ���Ƶ��
 *                     �ɺ��� aw_sys_clkrate_get()���, ��ʹ�� aw_ms_to_ticks()
 *                     ������ת��Ϊ������; #AW_SEM_NO_WAIT Ϊ������;
 *                     #AW_SEM_WAIT_FOREVER Ϊһֱ�ȴ�, �޳�ʱ
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 * \retval  -AW_EPERM  ������Ĳ���,���ж��е����˱�����
 * \retval  -AW_ETIME  ��ȡ�ź�����ʱ
 * \retval  -AW_ENXIO  �ź����Ѿ�����ֹ
 * \retval  -AW_EAGAIN ��ȡ�ź���ʧ��, �Ժ����� (timeout = AW_SEM_NO_WAIT)
 *
 * \par ����
 * ���෶����ο� #AW_SEMC_DECL() �� #AW_SEMC_DECL_STATIC()
 * \code
 *  AW_SEMC_DECL_STATIC(sem);                       // �����ź���ʵ��
 *  aw_semc_id_t  sem_id;                           // �ź���ID
 *
 *  sem_id = AW_SEMC_INI(sem);                      // ��ʼ���ź���,�õ��ź���ID
 *
 *  aw_semc_take(sem_id, AW_SEM_WAIT_FOREVER);      // ���õȴ�
 *
 *  aw_err_t ret;
 *  ret = aw_semc_take(sem_id, aw_ms_to_ticks(500));// ��ʱʱ�� 500ms
 *  if (ret == -AW_ETIME) {                         // �ȴ��ź�����ʱ
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = aw_semc_take(sem, AW_SEM_NO_WAIT);        // �޵ȴ�
 *  if (ret == -AW_EAGAIN) {                        // δ��ȡ���ź���
 *      //...
 *  }
 * \endcode
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE(), AW_SEMC_TERMINATE()
 * \sa aw_semc_give(), aw_semc_terminate()
 * \hideinitializer
 */
aw_err_t aw_semc_take (aw_semc_id_t sem_id, unsigned int timeout);

/**
 * \brief �ͷż����ź���
 *
 * �������Ĺ����� AW_SEMC_GIVE() ��ͬ, �������, ������ʹ��\a sem_id ��Ϊ������
 * \a sem_id �� AW_SEMC_INIT() ���ء�
 *
 * \param[in] sem_id �ź���ID, �� AW_SEMC_INIT()����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 *
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE(), AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_terminate()
 * \hideinitializer
 */
aw_err_t aw_semc_give (aw_semc_id_t sem_id);

/**
 * \brief ���ü����ź���
 *
 * �������Ĺ����� AW_SEMC_RESET() ��ͬ, �������, ������ʹ��\a sem_id ��Ϊ������
 * \a sem_id �� AW_SEMC_INIT() ���ء�
 *
 * \param[in] sem_id      �����ź���ID, �� AW_SEMC_INIT()����
 * \param[in] reset_value �����ź���������ֵ
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 *
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE(), AW_SEMC_TERMINATE()
 * \sa aw_semc_take(), aw_semc_terminate()
 * \hideinitializer
 */
aw_err_t aw_semc_reset (aw_semc_id_t sem_id, unsigned int reset_value );

/**
 * \brief ��ֹ�����ź���
 *
 * ��ֹ�ź������κεȴ����ź��������񽫻������, ������ -AW_ENXIO ���˺��������ͷ�
 * �ź�����ص��κ��ڴ档
 *
 * \param[in] sem_id �ź���ID, �� AW_SEMC_INIT()����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_SEMC_DECL()
 *
 * \sa AW_SEMC_DECL(), AW_SEMC_DECL_STATIC(), AW_SEMC_INIT(), AW_SEMC_TAKE(),
 *     AW_SEMC_GIVE(), AW_SEMB_TERMINATE()
 * \sa aw_semc_take(), aw_semc_give()
 * \hideinitializer
 */
aw_err_t aw_semc_terminate (aw_semc_id_t sem_id);

/** @} grp_aw_if_semc */
/** @} grp_aw_if_sem */

#ifdef __cplusplus
}
#endif

#endif /* __AW_SEMC_H */

/* end of file */
