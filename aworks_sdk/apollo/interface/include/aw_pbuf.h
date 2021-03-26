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
 * \brief ͨ��PBUF����ӿ�.
 *
 * ��ģ���ṩ����ͨ���ж����ݻ�������Դ�Ĺ������ݻ������߱����ü������ƣ�
 * �����ڶ��ģ�����ͨ��������ͬ������Դʱ���Ի�������Դ�Ĺ���,
 * ��߻�������Դ��ʹ��Ч�ʣ������ڴ�Ŀ�����
 *
 * ʹ�ñ�ģ����Ҫ����ͷ�ļ� aw_pbuf.h
 *
 * \par ��ʾ��
 * \code
 *
 *   gp_test_pool_buf = aw_pbuf_alloc(&g_pbuf_pool,       // PBUF�ṹ�����ָ��
 *                                     AW_PBUF_TYPE_POOL, // AW_PBUF_TYPE_POOL����
 *                                     35,                // PBUF�����ݳ���
 *                                     NULL,              // ��
 *                                     NULL,              // �ص�����Ϊ��
 *                                     NULL);             // �ص���������Ϊ��
 *   //  ʹ����...........
 *
 *   aw_pbuf_free(gp_test_pool_buf);          // �ͷŸ�PBUF������
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-03-05 bob, first implementation.
 * \endinternal
 */

#ifndef __AW_PBUF_H /* { */
#define __AW_PBUF_H

/**
 * \addtogroup grp_aw_if_pbuf
 * \copydoc aw_pbuf.h
 * @{
 */

#include "aw_types.h"
#include "aw_common.h"
#include "aw_list.h"
#include "aw_pool.h"
#include "aw_sem.h"


#ifdef __cplusplus
extern "C" {
#endif


/**
 * \brief PBUF ����ö��
 *
 *        ����һ��PBUFʱ��Ҫָ��PBUF�����͡�
 *
 *        AW_PBUF_TYPE_RAM ����PBUF�� ���ݻ�������PBUFͷ���ڴ���в���������䣬
 *        �ص����������ݻ�������������һ��洢�� ����������ͷ����������
 *
 *        AW_PBUF_TYPE_POOL ���͵�PBUF���ݻ�������PBUFͷ���ڴ���ṩ�����ڴ�ص�
 *        �ڴ�ͳطֿ��С�ɶ����ʼ��ʱָ�������ڳطֿ��С�ǹ̶��ģ����Ե��û�
 *        Ҫ�����һ���ϴ�PBUFʱ������������ڴ�طֿ鲢�����������������׸�
 *        PBUF��ָ�룬��������ͷ���������𣬸����͵���ҪӦ������������
 *
 *        AW_PBUF_TYPE_ROM/REF ����PBUF��PBUFͷ���ڴ���ṩ�����ڴ�ص��ڴ��ڶ�
 *        ���ʼ����ʱ��ָ������PBUF�����ݻ��������û��ṩ����ʹ��aw_pbuf_alloc
 *        �ӿ�ʱ����ָ��PBUF���ݻ��������������������͵�PBUF��PBUFͷ���������ͷ�
 *        ������������ݻ������������ͷ����û����𣬵�����aw_pbuf_free��������
 *        ���͵�PBUF�����ü���Ϊ0ʱ��������ͷ�PBUFͷ������������Ψһ��ͬ������
 *        �������洢����,AW_PBUF_TYPE_ROM���ͻ�������ROM�С�
 */
typedef enum {

    /** \brief ���ݻ�������PBUFͷ�����ڴ���з��䣨����ṩ����
     *         ������Ӧ�ó�����Э��ջ
     */
    AW_PBUF_TYPE_RAM = 0,

    /** \brief ���ݻ��������û��ṩ��ROM����PBUFͷ������ṩ���ڴ�أ���
     *������Ӧ�ó��� 
     */
    AW_PBUF_TYPE_ROM,

    /** \brief ���ݻ��������û��ṩ��PBUFͷ������ṩ���ڴ�أ���
     *         ������Ӧ�ó���
     */
    AW_PBUF_TYPE_REF,

    /** \brief ���ݻ�������PBUFͷ���ڴ���з��䣬������ṩ���������������� */
    AW_PBUF_TYPE_POOL
} aw_pbuf_type_t;

/**
 * \brief PBUF ���ͽṹ��
 */
struct aw_pbuf {

    struct aw_pbuf      *p_next;      /**< \brief PBUF�ڵ�ָ�� */

    void                *p_payload;   /**< \brief ָ��PBUF�����ݻ�����������*/

    /** \brief ��ǰPBUF��֮����������PBUF��ͬ��һ���������ݻ��������Ⱥͣ�
     *         ��ͨ�Ź����У�һ�����ݰ������ɶ��PBUF���Ӷ��ɣ����������¼��ǰ
     *         PBUF���������ݰ����ж������ݴ洢�ռ䡣
     *         ����ret_len(p) = ret_len(p->next) + len(p->next)
     */
    size_t               tot_len;

    size_t               len;         /**< \brief PBUF���ݻ��������� */

    aw_pbuf_type_t       type;        /**< \brief PBUF���� */

    uint8_t              flags;       /**< \brief PBUF��־ */
#define AW_PBUF_FLAGS_CUSTOM (1 << 0) /**< \brief �û��Զ����PBUF */

    size_t               ref;         /**< \brief ���ü���ֵ */

    AW_MUTEX_DECL(       mutex);      /**< \brief ������,�������� */

    struct aw_pbuf_pool *p_pbuf_pool; /**< \brief PBUF�����ڵ��ڴ�� */
};

/**
 * \brief CUSTOM����PBUF�ص��������Ͷ���
 */
typedef void (*aw_pbuf_custom_callback_t)(struct aw_pbuf *p, void *p_arg);

/**
 * \brief pbuf_custom���ͣ������û��Զ��壬һ���û������̳�PBUF
 */
struct aw_pbuf_custom {

    /** \brief PBUF�ṹ�����  */
    struct aw_pbuf                       pbuf;

    /** \brief PBUF�ص������������ڻ����û������ڴ棨������ */
    aw_pbuf_custom_callback_t            pfn_app_free;

    /** \brief �ص��������� */
    void                                *p_arg;
};

/**
 * \brief pbuf�������ṹ��
 */
struct aw_pbuf_pool {

    /** \brief �ڴ�أ����ڷ���AW_PBUF_TYPE_POOL���͵�PBUF�ڵ��
     *         ���ݻ�������Ҫ���ڴ� 
     */
    aw_pool_t            pool_pbuf_pool;

    /** \brief AW_PBUF_TYPE_POOL���͵�PBUF���ݻ��������� */
    size_t               pool_pbuf_item_size;

    /** \brief PBUF���ݻ������ڴ���볤�� */
    size_t               pbuf_align_size;

    /** \brief �ڴ�أ����ڷ���AW_PBUF_TYPE_ROM/REF���͵�PBUFͷ��Ҫ���ڴ� */
    aw_pool_t            rom_ref_pbuf_pool;

    /** \brief PBUF POOL����ڵ� */
    struct aw_list_head  node;
};

/**
 * \brief ͨ��PBUF�����ʼ��
 *
 * \retval ��
 */
void aw_pbuf_init(void);

/**
 * \brief PBUF�����ʼ�����û����ṩ������ڴ�أ������û���Ӧ����ȷ��������
 *
 * \param[in] p_pbuf_pool      ָ��PBUF�����ָ��
 * \param[in] p_pool_mem       AW_PBUF_TYPE_POOL����PBUF�ڴ���׵�ַ
 * \param[in] pool_mem_size    AW_PBUF_TYPE_POOL����PBUF�ڴ�ش�С
 *
 * \param[in] p_romref_mem     AW_PBUF_TYPE_ROM/REF����PBUF�ڴ���׵�ַ
 * \param[in] romref_mem_size  AW_PBUF_TYPE_ROM/REF����PBUF�ڴ�ش�С
 *
 * \param[in] data_buf_size    POOL����PBUF�����ݻ�������С
 * \param[in] align_size       �ڴ���볤��
 *
 * \retval  AW_OK      ��ʼ�����
 * \retval -AW_EINVAL  ��������
 * \retval -AW_EBUSY   �ڴ���Ѿ���ʹ��
 */
aw_err_t aw_pbuf_pool_init (struct aw_pbuf_pool *p_pbuf_pool,

                            void                *p_pool_mem,
                            size_t               pool_mem_size,

                            void                *p_romref_mem,
                            size_t               romref_mem_size,

                            size_t               data_buf_size,
                            size_t               align_size);

/**
 * \brief ����ȥ��ʼ��
 *
 * \param[in] p_pbuf_pool  ָ��PBUF�����ָ��
 *
 * \retval ��
 */
void aw_pbuf_pool_uninit (struct aw_pbuf_pool *p_pbuf_pool);

/**
 * \brief ����һ��PBUF
 *
 * \param[in] p_pbuf_pool  ָ��PBUF��ָ��
 * \param[in] type         PBUF������
 * \param[in] size         PBUF�����ݳ���
 * \param[in] p_mem        PBUF��payload���ݻ�����,��typeΪAW_PBUF_TYPE_ROM/REF
 *                         ʱ���û��ṩ����������Ϊ�ա������Ϊ�գ�������ע��
 *                         ���ݻ������ڴ��������
 *
 * \retval ����ɹ�������ΪPBUFָ��
 * \retval NULL   ����ʧ��,ʧ��ԭ��ɲ鿴errno
 */
struct aw_pbuf *aw_pbuf_alloc (struct aw_pbuf_pool *p_pbuf_pool,
                               aw_pbuf_type_t       type,
                               size_t               size,
                               void                *p_mem);

/**
 * \brief �û��Է����PBUF���壨����PBUF��չ��
 *
 * \param[in] p_pbuf        ָ��PBUF��ָ��
 * \param[in] p_mem         �Զ���PBUF���ڴ�
 * \param[in] size          PBUF�����ݳ���
 * \param[in] pfn_app_free  PBUF�ͷŻص���������PBUF���ü���Ϊ0ʱ����ã�
 * \param[in] p_arg         �ص���������
 *
 * \retval ����ɹ�������ΪPBUFָ��
 * \retval NULL,����ʧ�ܣ�ʧ��ԭ��ɲ鿴errno
 */
struct aw_pbuf *aw_pbuf_alloced_custom (struct aw_pbuf_custom    *p_pbuf,
                                        void                     *p_mem,
                                        size_t                    size,

                                        aw_pbuf_custom_callback_t pfn_app_free,
                                        void                     *p_arg);

/**
 * \brief ���µ���pbuf���Ĵ�С��ֻ֧����С��֧�����䣩
 *
 * \param[in] p_pbuf  ָ��PBUF��ָ��
 * \param[in] size    PBUF����֮��ĳ���
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_ENOTSUP  ��֧����չP_PBUF���ݳ���
 */
aw_err_t aw_pbuf_realloc (struct aw_pbuf *p_pbuf, size_t size);


/**
 * \brief �������׸�PBUF�Ĵ�С
 *
 * \param[in] p_pbuf  ָ��PBUF��ָ��
 * \param[in] size    ���ӵĴ�С�������ɸ���
 *
 * \retval  AW_OK      �豸��Ϣ�������
 * \retval -AW_EINVAL  ��������
 * \retval  AW_ERROR   size̫���̫С����Ϊ���������ݻ�����������PBUFͷ�����
 *
 * \note ������Ŀռ�ɴ洢������Ϣ��������Χ�����ݻ������ռ�֮��,����ӿڿ�����
 *       �ü�����֡��ͷ��
 */
aw_err_t aw_pbuf_header (struct aw_pbuf *p_pbuf, ssize_t size);

/**
 * \brief ����PBUF�����ü���ֵ
 *
 * \param[in] p_pbuf  ָ��PBUF��ָ��
 *
 * \retval AW_OK       �ɹ�
 * \retval -AW_EINVAL  ��������
 *
 * \note ��ʹ��һ�����Լ����������PBUFʱ����Ҫ���ô˽ӿڱ�ʾ���������PBUF
 */
aw_err_t aw_pbuf_ref(struct aw_pbuf *p_pbuf);

/**
 * \brief �ͷ�p_pbuf������
 *
 * \param[in] p_pbuf  ָ��PBUF��ָ��
 *
 * \retval �ͷŵ���PBUF����
 *
 * \note ������ü���Ϊ�գ�ϵͳ����ո�PBUF��Դ
 */
size_t aw_pbuf_free (struct aw_pbuf *p_pbuf);

/**
 * \brief ͳ��PUBF���ĳ���
 *
 * \param[in] p_pbuf ָ��PBUF��ָ��
 *
 * \retval ���е�PBUF����
 */
size_t aw_pbuf_cnt_get (struct aw_pbuf *p_pbuf);

/**
 * \brief ��p_tail��p_head����һ����
 *
 * \param[in] p_head  �����׸�PBUFָ�루p_head����Ϊһ����������һ�������У�
 * \param[in] p_tail  �������PBUF��
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_ENOTSUP  p_tail�Ѿ�������p_head�У�����������
 *
 * \note ���ô˽ӿں�p_tail������Ч������ӿڽ�p_tail����p_head���С�ע��p_head
 *       ��p_tail֮�䲻���ǽ���ģ�Ҳ����˵���p_head��p_tail������ͬ��PBUF,
 *       ��ôp_head��p_tail����ʹ������ӿڡ�
 */
aw_err_t aw_pbuf_pkt_expand (struct aw_pbuf *p_head, struct aw_pbuf *p_tail);

/**
 * \brief ��p_tail��p_head������
 *
 * \param[in] p_pkt_fifo  ָ�������(p_head����Ϊ������ͷָ��)
 * \param[in] p_pkt       ָ������İ�����һ������PBUF��ɣ�
 *
 * \retval  AW_OK       �����ɹ�
 * \retval -AW_EINVAL   ��������
 * \retval -AW_ENOTSUP  p_tail�Ѿ�������p_head�У�����������
 *
 * \note p_pkt_fifo��һ�������е�ͷ��p_pkt�����������p_pkt_fifo�У����ô˽ӿں�
         p_pkt������Ч������ӿ�������aw_pbuf_pkt_expand��ͬ��һ�����ݰ�������һ
         ��PBUF��ɵģ�Ҳ�������ж��PBUF��ɵģ�aw_pbuf_pkt_expand�ӿ��ǽ�p_pkt
         �������p_pkt_fifo�У���aw_pbuf_pkt_chain�ǽ�������������С�ע��
         p_pkt_fifo��p_pkt֮�䲻���ǽ���ģ�Ҳ����˵���p_pkt_fifo��p_pkt����
         ��ͬ��PBUF,��ôp_pkt_fifo��p_pkt����ʹ������ӿڡ�
 */
aw_err_t aw_pbuf_pkt_chain (struct aw_pbuf *p_pkt_fifo, struct aw_pbuf *p_pkt);

/**
 * \brief ��p_pbufת��Ϊtype���͵�PBUF
 *
 * \param[in] p_pbuf  ָ����Ҫת����PBUF
 * \param[in] type    ת��֮������ͣ�֧��AW_PBUF_TYPE_RAM��AW_PBUF_TYPE_POOL��
 *
 * \retval ת���ɹ�,ָ��p_buf��ָ��
 * \retval NULL,ת��ʧ�ܣ�ʧ��ԭ��ɲ鿴errno
 */
struct aw_pbuf* aw_pbuf_type_change (struct aw_pbuf *p_pbuf,
                                     aw_pbuf_type_t  type);

/**
 * \brief PBUF�ڵ����ݿ�����PBUF֮�䣩
 *
 * \param[in] p_to    Ŀ��PBUF
 * \param[in] p_from  ԴPBUF
 *
 * \retval  AW_OK      �����ɹ�
 * \retval -AW_EINVAL  ��������
 * \retval -AW_ENOMEM  p_to���Ȳ���С��p_from����
 */
aw_err_t aw_pbuf_copy (struct aw_pbuf *p_to, struct aw_pbuf *p_from);

/**
 * \brief PBUF���ݿ�����PBUF��������������
 *
 * \param[in] p_pbuf  ԴPBUF
 * \param[in] p_data  ָ�����ݻ������׵�ַ
 * \param[in] len     ���������ݳ���
 * \param[in] offset  PBUF����������ƫ��
 *
 * \retval �����ɹ�����������С��0Ϊ����ʧ��,ʧ��ԭ��ɲ鿴errno
 */
ssize_t aw_pbuf_data_read (struct aw_pbuf *p_pbuf,
                           void           *p_data,
                           size_t          len,
                           size_t          offset);

/**
 * \brief PBUF���ݿ�����������������PBUF��
 *
 * \param[in] p_pbuf  Ŀ��PBUF
 * \param[in] p_data  ָ�����ݻ������׵�ַ
 * \param[in] len     ���������ݳ���
 * \param[in] offset  PBUF����������ƫ��
 *
 * \retval �����ɹ�����������С��0Ϊ����ʧ��,ʧ��ԭ��ɲ鿴errno
 */
ssize_t aw_pbuf_data_write (struct aw_pbuf *p_pbuf,
                            const void     *p_data,
                            size_t          len,
                            size_t          offset);

/**
 * \brief ��ȡPBUF��ָ��λ�õ����ݣ�һ���ֽڣ�
 *
 * \param[in] p_pbuf  ָ�������PBUF
 * \param[in] offset  ���ֽ���PBUF�е�ƫ��
 *
 * \retval ��0 ��ȡ���ַ�
 * \retval 0  ��ȡʧ��(p_pbuf����������С��offset���������)
 */
uint8_t aw_pbuf_char_get (struct aw_pbuf* p_pbuf, size_t offset);

/**
 * \brief �Ƚ�PBUF�е��ַ���
 *
 * \param[in] p_pbuf  �Ƚϵ�PBUF
 * \param[in] offset  PBUF������ƫ��
 * \param[in] p_str   �Ƚϵ��ַ���
 * \param[in] len     �Ƚϵ��ַ�������
 *
 * \retval 0            �Ƚϳɹ�
 * \retval ssize_t < 0  �Ƚϴ���,ʧ��ԭ��ɲ鿴errno
 * \retval ssize_t > 0  ʧ��(���ַ���p_str��ssize_t�ֽڴ��Ƚ�ʧ��)
 */
ssize_t aw_pbuf_memcmp (struct aw_pbuf *p_pbuf,
                        size_t          offset,
                        const void     *p_str,
                        size_t          len);
/**
 * \brief ƥ��PBUF�е��ַ�������PBUF��start_offsetλ�ÿ�ʼƥ���ַ�����
 *
 * \param[in] p_pbuf        ָ����Ҫƥ���PBUF
 * \param[in] p_str         ��ƥ����ַ���
 * \param[in] len           �ַ�������
 * \param[in] start_offset  PBUF��ƫ��
 *
 * \return ƥ�䵽�ַ����ĵ�ַ,С��0Ϊƥ��ʧ��,ʧ��ԭ��ɲ鿴errno
 */
ssize_t aw_pbuf_memfind (struct aw_pbuf *p_pbuf,
                         const void     *p_str,
                         size_t          len,
                         size_t          start_offset);

/**
 * \brief PBUF�ַ���ƥ�䣨��PBUF��ƥ��һ���ַ��������ظ��ַ����ĵ�ַ��
 *
 * \param[in] p_pbuf    ָ����Ҫƥ���PBUF
 * \param[in] p_substr  ��ƥ����ַ���
 *
 * \return ƥ�䵽�ַ����ĵ�ַ,С��0Ϊƥ��ʧ��,ʧ��ԭ��ɲ鿴errno
 */
ssize_t aw_pbuf_strstr (struct aw_pbuf *p_pbuf, const char *p_substr);


#ifdef __cplusplus
}
#endif

/** @} grp_aw_if_pbuf */

#endif /* } __AW_PBUF_H */

/* end of file */
