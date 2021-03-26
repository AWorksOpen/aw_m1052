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

#ifndef __AW_MUTEX_H
#define __AW_MUTEX_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \addtogroup grp_aw_if_sem
 * \copydoc aw_sem.h
 * @{
 */

/**
 * \addtogroup grp_aw_if_mutex �����ź���
 * @{
 */

typedef aw_psp_mutex_id_t   aw_mutex_id_t;     /**< \brief �����ź���ID */

/**
 *  \brief ���廥���ź���
 *
 * ���궨��һ�������ź���, �ڱ���ʱ����ź��������ڴ�ķ���; ������ʱ, ��Ҫ����
 * AW_MUTEX_INIT() ����ź����ĳ�ʼ�������� AW_MUTEX_TERMINATE()��ֹ
 * �ź������ź�������ʼ��Ϊ��״̬��
 *
 * ����\a sem ֻ����Ϊ�� AW_MUTEX_INIT(), AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK() ��
 * AW_MUTEX_TERMINATE()�Ĳ�����
 *
 * AW_MUTEX_INIT() �����ź�����\a sem_id, \a sem_id ��Ϊ���� aw_mutex_lock() ,
 * aw_mutex_unlock() �� aw_mutex_terminate()�Ĳ���; ��Щ�����Ĺ������Ӧ�ĺ��
 * ������ͬ, ��������, ����ʹ��\a sem_id ��Ϊ����,\a sem_id ������Ϊ�ź����ľ��
 * �ں�������֮�䴫�ݡ�
 *
 * ����ʹ�ñ��꽫�ź���ʵ��Ƕ�뵽��һ�����ݽṹ��,�����Ļ�,�����Ǹ�������ݶ���
 * ��ʱ����Զ�������ź���ʵ����ڴ����, ������ķ�����ʾ��
 *
 * \attention ���ź���û�п�ģ��(�ļ�)ʹ�õ�����,�Ƽ�ʹ�� AW_MUTEX_DECL_STATIC()
 *            ���汾�꣬�����ɱ���ģ��֮���ź���������Ⱦ��
 *
 * \attention �������÷��ⲻ, ��Ī�ں�����ʹ�ô˺��������ź�������Ϊ�ں����ڴ˺�
 *            ������ڴ�������ջ�ϣ�һ���������أ�������ڴ汻�Զ����ա�
 *            �ں����ڲ�ʹ�� AW_MUTEX_DECL_STATIC()�������ź�����
 * \attention �����ź������������ж�ISR��
 *
 * \param[in] sem �ź���ʵ��, �� AW_MUTEX_INIT(), AW_MUTEX_LOCK(),
 *                AW_MUTEX_UNLOCK() �� AW_MUTEX_TERMINATE() һ��ʹ�á�
 *
 * \par �����������÷�, ������Դ�ķ���
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_TASK_DECL_STATIC(task_a, 512);
 *  AW_TASK_DECL_STATIC(task_b, 512);
 *
 *  AW_SEMC_DECL_STATIC(mutex);
 *
 *  void public_service (void)
 *  {
 *      AW_MUTEX_LOCK(mutex);
 *      //... Ҫ�����ĳ������
 *      AW_MUTEX_UNLOCK(mutex);
 *  }
 *  void func_proccess (void *arg)
 *  {
 *      while (1) {
 *          public_service();           // ���ʹ�������
 *          aw_mdelay(50);              // ��ʱ50ms
 *      }
 *  }
 *
 *  int main() {
 *
 *      // ��ʼ���ź���
 *      AW_MUTEX_INIT(sem, AW_SEM_Q_PRIORITY);
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(task_a,            // ����ʵ��
 *                   "task_a",          // ��������
 *                   5,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_proccess,     // ������ں���
 *                   NULL);             // ������ڲ���
 *
 *      AW_TASK_INIT(task_b,            // ����ʵ��
 *                   "task_b",          // ��������
 *                   6,                 // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_proccess,     // ������ں���
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
 *  #include "aw_sem.h"
 *
 *  struct my_struct {
 *      int my_data1;
 *      AW_TASK_DECL(task_a, 512);      // ����ʵ��task_a, ��ջ512
 *      AW_TASK_DECL(task_b, 512);      // ����ʵ��task_b, ��ջ512
 *      AW_MUTEX_DECL(mutex);           // �ź���ʵ��
 *      aw_mutex_id_t mutex_id;         // �ź���ID
 *  };
 *  struct my_struct   my_object;       // ���������ݶ���
 *
 *  void public_service (struct my_struct *p_myobj)
 *  {
 *      aw_mutex_lock(p_myobj->mutex_id);
 *      //... Ҫ�����ĳ������
 *      aw_mutex_unlock(p_myobj->mutex_id);
 *  }
 *
 *  void func_proccess (void *arg)
 *  {
 *      struct my_struct *p_myobj = arg;
 *      while (1) {
 *          public_service(p_myobj);        // ���ʹ�������
 *          aw_mdelay(50);                  // ��ʱ50ms
 *      }
 *  }
 *
 *  int main() {
 *
 *      // ��ʼ���ź���
 *      my_object.mutex_id = AW_MUTEX_INIT(my_object.mutex, AW_SEM_Q_PRIORITY);
 *      if (my_object.mutex_id == NULL) {
 *          return -1;                      // �ź�����ʼ��ʧ��
 *      }
 *
 *      // ��ʼ������
 *      AW_TASK_INIT(my_object.task_a,      // ����ʵ��
 *                   "task_a",              // ��������
 *                   5,                     // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_proccess,         // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      AW_TASK_INIT(my_object.task_b,      // ����ʵ��
 *                   "task_b",              // ��������
 *                   6,                     // �������ȼ�
 *                   512,               // �����ջ��С
 *                   func_proccess,         // ������ں���
 *                   (void *)&my_object);   // ������ڲ���
 *
 *      // ��������
 *      AW_TASK_STARTUP(my_object.task_a);
 *      AW_TASK_STARTUP(my_object.task_b);
 *
 *      aw_mdelay(20 * 1000);   //��ʱ20��
 *
 *      // AW_SEMC_TERMINATE(my_object.mutex);    // ��ֹ�ź���
 *      // aw_semc_terminate(my_object.mutex_id); // ��ֹ�ź���(ʹ���ź���ID)
 *
 *      return 0;
 *  }
 * \endcode
 *
 * \sa AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(), AW_MUTEX_LOCK(),
 *     AW_MUTEX_UNLOCK, AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_DECL(sem)       AW_PSP_MUTEX_DECL(sem)

/**
 * \brief ���廥���ź���(��̬)
 *
 * ������ AW_MUTEX_DECL() �Ĺ�����ͬ, ��������, AW_MUTEX_DECL_STATIC() �ڶ���
 * �ź��������ڴ�ʱ, ʹ�ùؼ���\b static ,���һ��, ����Խ��ź���ʵ�����������
 * ����ģ����(�ļ���), �Ӷ�����ģ��֮����ź���������ͻ���������ں�����ʹ�ñ���
 * �����ź�����
 *
 * \param[in] sem �ź���ʵ��, �� AW_MUTEX_INIT(), AW_MUTEX_LOCK(),
 *                AW_MUTEX_UNLOCK() �� AW_MUTEX_TERMINATE() һ��ʹ��
 *
 * \par ����
 * �� AW_MUTEX_DECL()
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_INIT(), AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK,
 *     AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_DECL_STATIC(sem)    AW_PSP_MUTEX_DECL_STATIC(sem)

/**
 * \brief ���û����ź���
 *
 * ��������һ�������ź���, �൱��ʹ�ùؼ� \b extern ����һ���ⲿ���š�
 * ���Ҫʹ����һ��ģ��(�ļ�)�ж�����ź���, ����ʹ�ñ���������ź���, Ȼ���
 * ���Բ������ź�������������ź���Ӧ����ʹ�� AW_MUTEX_DECL()����ġ�
 *
 * \param[in] sem ����ʵ��
 *
 * \par ����
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_MUTEX_IMPORT(sem_x);     // �����ź��� sem_x
 *
 *  int func () {
 *      AW_MUTEX_LOCK(sem_x);   // �ȴ��ź��� sem_x
 *  }
 * \endcode
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_IMPORT(sem)     AW_PSP_MUTEX_IMPORT(sem)

/**
 * \brief �����ź�����ʼ��
 *
 * ��ʼ���� AW_MUTEX_DECL() �� AW_MUTEX_DECL_STATIC()��̬����Ļ����ź���,
 * ��Ƕ�뵽��һ�������ݶ����еĶ������ź�����
 *
 * �����ź�����ѡ������������ڴ��ź�����������Ŷӷ�ʽ, ���԰����������ȼ�
 * ���Ƚ��ȳ��ķ�ʽ�Ŷ�, ���Ƿֱ��Ӧ #AW_SEM_Q_PRIORITY �� #AW_SEM_Q_FIFO ��
 *
 * ѡ�� #AW_SEM_DELETE_SAFE �����˵�ǰӵ�д��ź�������������������ɾ������ѡ��
 * �����Ĳ�����Ϊÿ�� aw_mutex_lock() ���һ�� AW_TASK_SAFE() ����, Ϊÿ��
 * aw_mutex_unlock() ���һ�� AW_TASK_UNSAFE() ���á�
 *
 * ѡ�� #AW_SEM_INVERSION_SAFE ������ϵͳ�������ȼ���ת, ��ѡ������
 * #AW_SEM_Q_PRIORITY �Ŷӷ�ʽһ��ʹ�á�
 *
 * \param[in] sem     �ź���ʵ��,�� AW_MUTEX_DECL()�� AW_MUTEX_DECL_STATIC()����
 * \param[in] options ��ʼ��ѡ���־("()"��ʾ��ѡ):
 *                          - #AW_SEM_Q_FIFO | (#AW_SEM_DELETE_SAFE)
 *                          - #AW_SEM_Q_PRIORITY | (#AW_SEM_DELETE_SAFE) |
                              (#AW_SEM_INVERSION_SAFE)
 *
 * \return �ɹ����ػ����ź�����ID, ʧ���򷵻�NULL
 *
 * \par ʾ��
 *  ��ο� AW_MUTEX_DECL()
 *
 * \sa AW_SEM_Q_FIFO, AW_SEM_Q_PRIORITY
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_LOCK(),
 *     AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_INIT(sem, options) \
            AW_PSP_MUTEX_INIT(sem, options)

/**
 * \brief ��ȡ�����ź���
 *
 * ��ȡ�ź���������ź���Ϊ��, �����񽫻ᱻ����ֱ���ź�����ÿ���(�����������
 * �� AW_MUTEX_UNLOCK()�� aw_mutex_unlock())������ź����Ѿ��ǿ��õ�, �򱾵���
 * ������ո��ź���, �����������ܻ�ȡ�����ź���, ֱ�����õ������ͷŴ��ź�����
 *
 * ���ɾ����ȫѡ��(#AW_SEM_DELETE_SAFE)��ʹ��, �򱾺�������һ��������
 * AW_TASK_SAFE()���á�
 *
 *  ���� \a timeout ���Ƶ��õ�����:
 *   - Ϊ #AW_SEM_WAIT_FOREVER ʱ, ���ý��ᱻ����, ֱ�����ź������ͷŻ�ɾ��,
 *     ����ͨ������ֵ���жϵ��ý��:AW_OK -�ɹ���ȡ���ź�����-AW_ENXIO -�ź����Ѿ�
 *     ���ݻ�
 *
 *   - Ϊ #AW_SEM_NO_WAIT ʱ,���ò��ᱻ����, ��������, ͨ������ֵ���жϵ��ý��;
 *     AW_OK -�ɹ���ȡ���ź���; -AW_EAGAIN -δ��ȡ���ź���,�Ժ�����;
 *     -AW_ENXIO -�ź����Ѿ�����ֹ
 *
 *   - ����Ϊ��ʱʱ��, ��ʱ�����г�ʱ������, ���趨�ĳ�ʱʱ�䵽��ʱ, ����δ��
 *     ȡ���ź���, ����÷���, ����ͨ������ֵ���жϵ��ý���� AW_OK -�ɹ���ȡ��
 *     �ź���; -AW_ETIME -��ʱ,δ��ȡ���ź���,�Ժ�����; -AW_ENXIO -�ź����Ѿ�����ֹ
 *
 *   - ��������ֵΪ�Ƿ�����
 *
 * \param[in] sem      �ź���ʵ��,�� AW_MUTEX_DECL()�� AW_MUTEX_DECL_STATIC() ����
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
 * \retval  -AW_EAGAIN ��ȡ�ź���ʧ��, �Ժ����� (\a timeout = #AW_SEM_NO_WAIT)
 *
 * \par ����
 * ���෶����ο� AW_MUTEX_DECL()
 * \code
 *  #include "aw_sem.h"
 *
 *  AW_MUTEX_LOCK(sem, AW_SEM_WAIT_FOREVER);        // ���õȴ�
 *
 *  aw_err_t ret;
 *  ret = AW_MUTEX_LOCK(sem, aw_ms_to_ticks(500));  //��ʱ�ȴ�,��ʱʱ��500ms
 *  if (ret == -AW_ETIME) {                         // �ȴ��ź�����ʱ
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = AW_MUTEX_LOCK(sem, AW_SEM_NO_WAIT);       // �޵ȴ�
 *  if (ret == -AW_EAGAIN) {                        // δ��ȡ���ź���
 *      //...
 *  }
 * \endcode
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_LOCK(sem, timeout)  AW_PSP_MUTEX_LOCK(sem, timeout)

/**
 * \brief �ͷŻ����ź���
 *
 * �ͷ��ź���������и������ȼ��������ȡ�˸��ź���, �������׼����������, ����
 * ��������ռ���� AW_MUTEX_UNLOCK() ���������ź���������(���ź������ͷ�,
 * ����û�������ȡ), ��û���κβ�����
 *
 * ���ɾ����ȫѡ��(#AW_SEM_DELETE_SAFE)��ʹ��, �򱾺�������һ��������
 * AW_TASK_UNSAFE()���á�
 *
 * \param[in] sem   �ź���ʵ��,�� AW_MUTEX_DECL() �� AW_MUTEX_DECL_STATIC() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MUTEX_DECL()
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_UNLOCK(sem)   AW_PSP_MUTEX_UNLOCK(sem)

/**
 * \brief ��ֹ�����ź���
 *
 * ��ֹ�ź������κεȴ����ź��������񽫻������, ������ -AW_ENXIO���˺���������
 * ���ź�����ص��κ��ڴ档
 *
 * \param[in] sem   �ź���ʵ��,�� AW_MUTEX_DECL() �� AW_MUTEX_DECL_STATIC() ����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MUTEX_DECL()
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK()
 * \sa aw_mutex_lock(), aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
#define AW_MUTEX_TERMINATE(sem)  AW_PSP_MUTEX_TERMINATE(sem)


/**
 * \brief ��ȡ�����ź���
 *
 * �������Ĺ����� AW_MUTEX_LOCK() ��ͬ, �������, ������ʹ��\a sem_id ��Ϊ������
 * \a sem_id �� AW_MUTEX_INIT() ���ء�
 *
 * \param[in] sem_id   �ź���ID,�� AW_MUTEX_INIT()����
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
 * \retval  -AW_EAGAIN ��ȡ�ź���ʧ��, �Ժ����� (\a timeout = #AW_SEM_NO_WAIT)
 *
 * \par ����
 * ���෶����ο� AW_MUTEX_DECL()
 * \code
 *  AW_MUTEX_DECL_STATIC(sem);                  // �����ź���ʵ��
 *  aw_mutex_id_t  sem_id;                      // �ź���ID
 *
 *  sem_id = AW_MUTEX_INI(sem);                 // ��ʼ���ź���, �õ��ź���ID
 *
 *  aw_mutex_lock(sem_id, AW_SEM_WAIT_FOREVER);         // ���õȴ�
 *
 *  aw_err_t ret;
 *  ret = aw_mutex_lock(sem_id, aw_ms_to_ticks(500));   // ��ʱʱ�� 500ms
 *  if (ret == -AW_ETIME) {                             // �ȴ��ź�����ʱ
 *      //...
 *  }
 *
 *  aw_err_t ret;
 *  ret = aw_mutex_lock(sem, AW_SEM_NO_WAIT);           // �޵ȴ�
 *  if (ret == -AW_EAGAIN) {                            // δ��ȡ���ź���
 *      //...
 *  }
 * \endcode
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_unlock(), aw_mutex_terminate()
 * \hideinitializer
 */
aw_err_t aw_mutex_lock (aw_mutex_id_t sem_id, unsigned int timeout);

/**
 * \brief �ͷŻ����ź���
 *
 * �������Ĺ����� AW_MUTEX_UNLOCK() ��ͬ, �������, ������ʹ��\a sem_id ��Ϊ����
 * \a sem_id �� AW_MUTEX_INIT() ���ء�
 *
 * \param[in] sem_id  �ź���ID,�� AW_MUTEX_INIT()����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MUTEX_DECL()
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_terminate()
 * \hideinitializer
 */
aw_err_t aw_mutex_unlock (aw_mutex_id_t sem_id);

/**
 * \brief ��ֹ�����ź���
 *
 * ��ֹ�ź������κεȴ����ź��������񽫻������, ������ -AW_ENXIO���˺��������ͷ�
 * �ź�����ص��κ��ڴ档
 *
 * \param[in] sem_id �ź���ʵ��,�� AW_MUTEX_DECL() �� AW_MUTEX_DECL_STATIC()����
 *
 * \retval   AW_OK     �ɹ�
 * \retval  -AW_EINVAL ������Ч
 *
 * \par ����
 * ��ο� AW_MUTEX_DECL()
 *
 * \sa AW_MUTEX_DECL(), AW_MUTEX_DECL_STATIC(), AW_MUTEX_INIT(),
 *     AW_MUTEX_LOCK(), AW_MUTEX_UNLOCK(), AW_MUTEX_TERMINATE()
 * \sa aw_mutex_lock(), aw_mutex_unlock()
 * \hideinitializer
 */
aw_err_t aw_mutex_terminate (aw_mutex_id_t sem_id);

/** @} grp_aw_if_mutex */
/** @} grp_aw_if_sem */

#ifdef __cplusplus
}
#endif

#endif /* __AW_MUTEX_H */

/* end of file */
