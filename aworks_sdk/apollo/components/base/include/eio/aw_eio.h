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
 * \brief expand I/O system.
 *
 * \internal
 * \par modification history:
 * - 16-01-07 deo, first implementation.
 * \endinternal
 */

#ifndef __AW_EIO_H
#define __AW_EIO_H


#ifdef __cplusplus
extern "C" {
#endif

#include "aw_list.h"
#include "aw_sem.h"
#include "aw_pool.h"
#include "aw_spinlock.h"
#include "aw_pbuf.h"

/**
 * \brief EIO �豸�ṹ��
 */
struct aw_eio_dev {
    struct aw_list_head        node;     /**< \brief �豸�ڵ� */

    const char                *name;     /**< \brief �豸�� */

    const struct aw_eio_funs  *p_funs;   /**< \brief �豸������EIO�����ṩ */

    aw_spinlock_isr_t          lock;     /**< \brief �豸������ */

    struct aw_list_head        pipes;    /**< \brief ���豸��ͨ������ͷ */

    uint32_t                   np;       /**< \brief ���豸����ͨ���� */
};

/**
 * \brief EIO ͨ���ṹ��
 */
struct aw_eio_pipe {
    uint32_t                id;          /**< \brief ͨ��ID, ��������ͨ������ */
#define AW_EIO_IN        0x80000000      /**< \brief �ɶ�ͨ���������� */
#define AW_EIO_OUT       0x40000000      /**< \brief ��дͨ���������� */
#define AW_EIO_INOUT     0xC0000000      /**< \brief �ɶ�дͨ������˫���� */

    struct aw_list_head     node;        /**< \brief ͨ���ڵ� */

    struct aw_list_head     work_list;   /**< \brief �ȴ���������ݰ��� */

    struct aw_list_head     done_list;   /**< \brief ����ɴ�������ݰ��� */

    AW_SEMC_DECL(           done_sem);   /**< \brief ͨ���ź���,����ͬ�����Ĵ��� */

    aw_spinlock_isr_t       lock;        /**< \brief ͨ�������� */

    struct aw_eio_obj      *p_obj;       /**< \brief ͨ������EIO���� */

    void                   *p_priv;      /**< \brief ��������������ʹ�� */
};

/**
 * \brief EIO ����ṹ��
 */
struct aw_eio_obj {
    struct aw_eio_dev   *p_dev;         /**< \brief EIO������豸 */
    void                *p_mem;         /**< \brief EIO������ڴ�ص��ڴ� */
    struct aw_pool       pkt_pool;      /**< \brief EIO������ڴ�� */
};

/**
 * \brief EIO ���ݰ��ṹ��
 */
struct aw_eio_pkt {
    struct aw_list_head   node;        /**< \brief ���ݰ��ڵ� */

    struct aw_eio_obj    *p_obj;       /**< \brief ���ݰ������Ķ��� */

    void                 *p_buf;       /**< \brief �������ݻ����� */
    size_t                len;         /**< \brief ���ݻ��������� */

    void                 *p_cur_buf;   /**< \brief ��ǰ���ݰ����ڴ����λ�� */

    int                   opt;         /**< \brief ���ݰ��������� */
#define AW_EIO_READ        0           /**< \brief ������ */
#define AW_EIO_WRITE       1           /**< \brief д���� */

    int                   status;      /**< \brief ���ݰ�����״̬ */

    int                   ret_len;     /**< \brief ��ɴ������ݳ��� */

    uint8_t               flags;       /**< \brief ���ݰ���־ */
#define AW_EIO_FLAGS_POOL       0x01   /**< \brief �����ݰ���ͷ���ڴ���ṩ */
#define AW_EIO_FLAGS_PBUF       0x02   /**< \brief �����ݰ�������ΪPBUF��ʽ */

    /** \brief ���ݰ�������ɻص����� */
    aw_bool_t              (*pfn_complete) (void *arg);

    /** \brief �ص��������� */
    void                 *p_arg;

    void                 *p_priv;        /**< \brief ��������������ʹ�� */
};

/**
 * \brief EIO �豸����ṹ��
 */
struct aw_eio_funs {

    /** \brief �������ݴ��䣬����Ӧע���ظ���������� */
    aw_err_t (*pfn_start) (struct aw_eio_dev  *p_dev,
                           struct aw_eio_pipe *p_pipe);

    /** \brief ȡ�����ڴ�����������ݰ��Ĵ������ */
    void (*pfn_abort) (struct aw_eio_dev  *p_dev,
                       struct aw_eio_pipe *p_pipe);

    /** \brief ȡ��һ�����ݰ����ȴ���������ڴ��䣩�Ĵ��� */
    void (*pfn_cancel) (struct aw_eio_dev  *p_dev,
                        struct aw_eio_pipe *p_pipe,
                        struct aw_eio_pkt  *p_pkt);

    /** \brief �豸���� */
    aw_err_t (*pfn_control) (struct aw_eio_dev *p_dev,
                             int                cmd,
                             void              *p_arg);
};


/**
 * \brief EIO �����ʼ��
 *
 * \retval ��
 */
void aw_eio_init (void);

/**
 * \brief EIO �����ʼ��
 *
 * \param[in]       p_obj   EIO ����
 * \param[in]       name    EIO �豸��
 * \param[in]       p_mem   ���ڴ������ݰ��ص��ڴ�
 * \param[in]       size    �ڴ�Ĵ�С
 *
 * \retval AW_OK          ��ʼ���ɹ�
 * \retval -AW_EINVAL     ��������
 * \retval -AW_ENODEV     û�з�����Ϊname���豸
 * \retval -AW_ENOMEM     ���ݰ���ʼ��ʧ��
 */
aw_err_t aw_eio_obj_init (struct aw_eio_obj  *p_obj,
                          const char         *name,
                          void               *p_mem,
                          size_t              size);

/**
 * \brief EIO ����ȥ��ʼ��
 *
 * \param[in]       p_obj   EIO ����
 *
 * \retval AW_OK       ȥ��ʼ���ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval AW_ERROR    ȥ��ʼ��ʧ��
 */
aw_err_t aw_eio_obj_uninit (struct aw_eio_obj  *p_obj);

/**
 * \brief ��һ�� EIO ͨ��
 *
 * \param[in]       p_obj   EIO ����
 * \param[in]       pipe_id ͨ��ID
 *
 * \retval ��NULL   ָ��򿪵�ͨ��
 * \retval NULL     ��ʧ��
 */
struct aw_eio_pipe *aw_eio_pipe_open (struct aw_eio_obj *p_obj,
                                      uint32_t           pipe_id);

/**
 * \brief �ر�һ�� EIO ͨ��
 *
 * \param[in]    p_pipe   EIO ͨ��
 *
 * \retval ��
 */
void aw_eio_pipe_close (struct aw_eio_pipe *p_pipe);

/**
 * \brief EIO ͬ����
 *
 * \param[in]       p_pipe   EIO ͨ��
 * \param[in]       p_buf    EIO ���ݻ�����
 * \param[in]       len      ��ȡ�ĳ���
 * \param[in]       timeout  ��ʱ��ms��
 *
 * \retval len         ��ȡ�ɹ�
 * \retval 0 ~ len     ��ȡ��ʱ�������Ѿ�������ɵĳ���
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENOMEM  ��ʼ���ź���ʧ��
 *
 * \note ����ӿڴ�EIOͨ����ȡlen���ֽ����ݣ�
 *       ��timeoutʱ����δ��ɴ˲���ʱֹͣ��
 *       ���������Ѿ���ȡ�����ֽ�����
 */
int aw_eio_read (struct aw_eio_pipe *p_pipe,
                 void               *p_buf,
                 size_t              len,
                 int                 timeout);

/**
 * \brief EIO ͬ��д
 *
 * \param[in]       p_pipe   EIO ͨ��
 * \param[in]       p_buf    EIO ���ݻ�����
 * \param[in]       len      д��ĳ���
 * \param[in]       timeout  ��ʱ��ms��
 *
 * \retval len         ��ȡ�ɹ�
 * \retval 0 ~ len     ��ȡ��ʱ�������Ѿ�������ɵĳ���
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENOMEM  ��ʼ���ź���ʧ��
 *
 * \note ����ӿ���EIOͨ��д��len���ֽ����ݣ�
 *       ��timeout��ʱ��δ��ɴ˲���ʱֹͣ��
 *       ���������Ѿ�д����ֽ�����
 */
int aw_eio_write (struct aw_eio_pipe *p_pipe,
                  void               *p_buf,
                  size_t              len,
                  int                 timeout);

/**
 * \brief ISSUE/RECLAIMģʽ������һ���첽�������
 *
 * \param[in]       p_pipe       EIO ͨ��
 * \param[in]       opt          �첽����Ĳ���
 * \param[in]       p_buf        ���ݻ�����
 * \param[in]       len          ��������ݳ���
 * \param[in]       pfn_complete ��ɴ���ص�����
 * \param[in]       p_arg        �ص���������
 *
 * \retval AW_OK       ����ɹ�
 * \retval -AW_ENOMEM  EIO���ڴ�ؿ�
 * \retval -AW_EINVAL  ��������
 *
 * \note ����ӿ���ͨ������һ���첽���䣬���pfn_complete��Ϊ�գ��˴������
 *       ������pfn_complete�ص����������pfn_completeΪ�գ��˴�����ɺ��
 *       ������������ݺͽ������ڸ�ͨ���ڣ��û�ʹ��aw_eio_reclaim���Դ�ͨ
 *       �����ոô��������
 */
int aw_eio_issue (struct aw_eio_pipe *p_pipe,
                  int                 opt,
                  void               *p_buf,
                  size_t              len,
                  aw_bool_t          (*pfn_complete) (void *arg),
                  void               *p_arg);

/**
 * \brief ISSUE/RECLAIMģʽ�»���һ���첽�������
 *
 * \param[in]        p_pipe     ͨ��
 * \param[out]       p_opt      ����Ĳ���
 * \param[out]       pp_buf     ��������ݻ�������ָ��
 * \param[out]       p_ret_len  ������ɵĳ���
 * \param[out]       p_status   ��������״̬
 * \param[in]        timeout    ��ʱ��ms��
 * \param[in]        abort      ȡ����־
 *
 * \retval AW_OK      ���ճɹ�
 * \retval -AW_ETIME  ���ճ�ʱ
 * \retval -AW_EINVAL ��������
 *
 * \note ����ӿ���aw_eio_issue���ʹ�ã����ڻ���һ���첽����������첽�����
 *       ���������������ѭFIFOԭ��������Ĳ����ȱ����պ�����Ĳ����󱻻��ա�
 *       aw_eio_reclaim�ӿڷ��غ�pp_bufָ����յ��Ļ������׵�ַ��������������
 *       �ֱ���p_opt��p_ret_len��p_status�С����ô˽ӿں�timeoutʱ�������û��
 *       ���յ��������ʱ�����abortΪTRUEʱ��ǿ����ֹ���ڴ�������ݲ��������
 *       �иò��������ظò�����abortΪFALSE��һֱ�ȵ��ò������ʱ�ŷ��ء�
 */
int aw_eio_reclaim(struct aw_eio_pipe *p_pipe,
                   int                *p_opt,
                   void              **pp_buf,
                   size_t             *p_ret_len,
                   int                *p_status,
                   int                 timeout,
                   aw_bool_t           abort);

/**
 * \brief ISSUE/RECLAIMģʽ������һ���첽�������
 *
 * \param[in]       p_pipe       EIO ͨ��
 * \param[in]       opt          �첽����Ĳ���
 * \param[in]       p_pbuf       ���ݻ�����
 * \param[in]       len          ��������ݳ���
 * \param[in]       pfn_complete ��ɴ���ص�����
 * \param[in]       p_arg        �ص���������
 *
 * \retval AW_OK       ����ɹ�
 * \retval -AW_ENOMEM  EIO���ڴ�ؿ�
 * \retval -AW_EINVAL  ��������
 *
 * \note ����ӿ���ͨ������һ���첽���䣬���pfn_complete��Ϊ�գ��˴������
 *       ������pfn_complete�ص����������pfn_completeΪ�գ��˴�����ɺ��
 *       ������������ݺͽ������ڸ�ͨ���ڣ��û�ʹ��aw_eio_pbuf_reclaim����
 *       ��ͨ�����ոô��������
 */
int aw_eio_pbuf_issue (struct aw_eio_pipe *p_pipe,
                       int                 opt,
                       struct aw_pbuf     *p_pbuf,
                       size_t              len,
                       aw_bool_t          (*pfn_complete) (void *arg),
                       void               *p_arg);

/**
 * \brief ISSUE/RECLAIMģʽ�»���һ���첽�������
 *
 * \param[in]        p_pipe     ͨ��
 * \param[out]       p_opt      ����Ĳ���
 * \param[out]       pp_pbuf    ��������ݻ�������ָ��
 * \param[out]       p_ret_len  ������ɵĳ���
 * \param[out]       p_status   ��������״̬
 * \param[in]        timeout    ��ʱ��ms��
 * \param[in]        abort      ȡ����־
 *
 * \retval AW_OK      ���ճɹ�
 * \retval -AW_ETIME  ���ճ�ʱ
 * \retval -AW_EINVAL ��������
 *
 * \note ����ӿ���aw_eio_pbuf_issue���ʹ�ã����ڻ���һ���첽����������첽��
 *       ������������������ѭFIFOԭ��������Ĳ����ȱ����պ�����Ĳ����󱻻�
 *       �ա�����ӿڷ��غ�pp_bufָ����յ��Ļ������׵�ַ�������������ݷֱ���
 *       p_opt��p_ret_len��p_status�С����ô˽ӿں�timeoutʱ�������û�л��յ���
 *       �����ʱ�����abortΪTRUEʱ��ǿ����ֹ���ڴ�������ݲ���������иò�����
 *       ���ظò�����abortΪFALSE��һֱ�ȵ��ò������ʱ�ŷ��ء�
 */
int aw_eio_pbuf_reclaim(struct aw_eio_pipe  *p_pipe,
                        int                 *p_opt,
                        struct aw_pbuf     **pp_pbuf,
                        size_t              *p_ret_len,
                        int                 *p_status,
                        int                  timeout,
                        aw_bool_t            abort);

/**
 * \brief EIO �豸����
 *
 * \param[in]       p_obj   EIO ����
 * \param[in]       cmd     ��������
 * \param[in,out]   p_arg   ���Ʋ���
 *
 * \retval -AW_EINVAL    ��������
 * \retval -AW_ENOTSUP   ����û��ʵ�ָ÷���
 * \retval AW_OK         ȡ���ɹ�
 *
 * \note ����ӿڿ���EIO�豸��������ã�������ͨ�����ʵ�
 */
aw_err_t aw_eio_ctrl (struct aw_eio_obj *p_obj,
                      int                cmd,
                      void              *p_arg);

/**
 * \brief ȡ��ͨ�����������ݰ��Ĵ���
 *
 * \param[in]         p_pipe  EIO ͨ��
 *
 * \retval -AW_EINVAL    ��������
 * \retval -AW_ENOTSUP   ����û��ʵ�ָ÷���
 * \retval AW_OK         ȡ���ɹ�
 */
aw_err_t aw_eio_abort (struct aw_eio_pipe *p_pipe);

/**
 * \brief ����һ�� EIO �豸����EIO�豸������ʼ����ʱ���������豸
 *
 * \param[in]       p_dev   EIO �豸ָ��
 * \param[in]       name    EIO �豸��
 * \param[in]       p_funs  �豸����ָ��
 *
 * \retval -AW_EINVAL     ��������
 * \retval -AW_EEXIST     ���豸�Ѿ�ע��
 * \retval AW_OK          ע��ɹ�
 *
 * \note �ȴ����豸�������ͨ��������ӿ���aw_eio_dev_pipe_addǰ����
 */
aw_err_t aw_eio_dev_create (struct aw_eio_dev  *p_dev,
                            const char         *name,
                            struct aw_eio_funs *p_funs);

/**
 * \brief ɾ�� EIO �豸
 *
 * \param[in]   p_dev   EIO �豸
 *
 * \retval -AW_EINVAL  ��������
 * \retval AW_OK       �����ɹ�
 */
aw_err_t aw_eio_dev_delete (struct aw_eio_dev *p_dev);

/**
 * \brief �� EIO �豸�����ͨ��
 *
 * \param[in]       p_dev   EIO �豸
 * \param[in]       p_pipe  EIO ͨ��ָ��
 * \param[in]       id      ͨ��ID
 * \param[in]       p_priv  ͨ����������������ò�����
 *
 * \retval ��
 *
 * \note ��EIO�豸������ʼ����ʱ���������豸ͨ�����ȴ����豸��
 *       �����ͨ��������ӿ���aw_eio_dev_create�����
 */
void aw_eio_dev_pipe_add (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe,
                          uint32_t            id,
                          void               *p_priv);

/**
 * \brief ��EIO�豸��ɾ��ͨ��
 *
 * \param[in]    p_dev     EIO �豸
 * \param[in]    p_pipe    EIO ͨ��
 *
 * \retval ��
 *
 * \note ��EIO�豸����ȥ��ʼ����ʱ�����ɾ���豸ͨ��
 */
void aw_eio_dev_pipe_del (struct aw_eio_dev  *p_dev,
                          struct aw_eio_pipe *p_pipe);

/**
 * \brief ��ͨ����ȡһ�����ݰ�
 *
 * \param[in]       p_pipe   EIOͨ��
 *
 * \retval ��NULL   ��ȡ��һ�����ݰ�
 * \retval NULL     ͨ��Ϊ��
 *
 * \note ����ӿ�ΪEIO���½ӿڣ���EIO����start��������ͨ������֮��
 *       ������Ӹ�ͨ�����ϻ�ȡ���ݰ����д��䣬������Ὣ��ɵ�����
 *       ���ύ��EIO,ֱ��ͨ��Ϊ��ֹͣ����
 */
struct aw_eio_pkt *aw_eio_dev_pkt_get (struct aw_eio_pipe *p_pipe);

/**
 * \brief ��ȡ���ݰ���һ�����ݻ�����
 *
 * \param[in]       p_pkt     ָ�����ڴ�������ݰ�
 * \param[out]      pp_buf    ָ�򷵻ص����ݻ�����
 * \param[out]      p_len     ���ص����ݻ������ĳ���
 *
 * \retval AW_OK        ��ȡ�ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_ENOMEM   ���ݰ��ѿ�
 *
 * \note ����ӿ�ΪEIO���½ӿڣ���ȡָ�����ݰ���һ�����ݻ�������һ�����ݰ�
 *       p_pkt�Ļ����������Ƕ���ҷ������ģ�ʹ������ӿڲ��ϻ�ȡ���ݰ�����
 *       �ݻ������ֿ鲢���䣬ֱ�����ݰ����Ѿ�û�л�����ʱ��ʾ��һ�������Ѿ�
 *       ��ɴ�����������Ե���aw_eio_dev_pkt_done�ύ��һ������
 */
aw_err_t aw_eio_pkt_buf_get (struct aw_eio_pkt *p_pkt,
                             void             **pp_buf,
                             size_t            *p_len);

/**
 * \brief �ύһ������ɵ����ݰ�
 *
 * \param[in]       p_pipe   EIO ͨ��
 * \param[in]       p_pkt    �Ѿ�������ɵ����ݰ�
 *
 * \retval ��
 *
 * \note ����ӿ�ΪEIO���½ӿڣ�EIO�豸�������һ�����Ĵ�����������ӿ�
 *       �ύ���ݰ���EIO
 */
void aw_eio_dev_pkt_done (struct aw_eio_pipe *p_pipe,
                          struct aw_eio_pkt  *p_pkt);

#ifdef __cplusplus
}
#endif


#endif /* __AW_EIO_H */
