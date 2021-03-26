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
 * \brief ͨ�û��λ�����
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_rngbuf.h
 *
 * \par ��ʾ��
 * \code
 * #include "aworks.h"
 * #include "aw_rngbuf.h"
 *
 * AW_RNGBUF_DEFINE(char_buf, char, 64);    // 64��char�ռ�Ļ��λ�����
 * char data;
 *
 * aw_rngbuf_putchar(&char_buf, 'a');       // ���1���ַ���������
 * aw_rngbuf_getchar(&char_buf, &data);     // �ӻ�������ȡ1���ַ�
 *
 * \endcode
 *
 * // �������ݴ���ӡ�����
 *
 * \internal
 * \par modification history
 * - 1.02 12-12-08  orz, change type of size and nbytes to size_t.
 * - 1.01 12-10-22  orz, add prefix "aw_" to everything
 * - 1.00 12-10-15  orz, add implementation from VxWroks
 * @endinternal
 */

#ifndef __AW_RNGBUF_H
#define __AW_RNGBUF_H

/**
 * \addtogroup grp_aw_if_rngbuf
 * \copydoc aw_rngbuf.h
 * @{
 */
#include "aw_common.h"

/*lint ++flb */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief ���徲̬�Ļ��λ������洢�ռ�
 *
 * �����ᶨ��Ĵ洢�ռ�������Ŀռ��һ���ֽ�
 *
 * \param[in,out] rb            ��Ҫ���建��ռ�Ļ��λ�����
 * \param[in]     element_type  ���λ������洢������Ԫ������, 
 *                              ��char�� int��ṹ���
 * \param[in]     num_elements  ����Ԫ�ظ���
 *
 * \note ���λ������洢�ռ��СΪ��sizeof(element_type) * num_elements
 *
 * \par ʾ��
 * \code
 * typedef struct my_struct {
 *     char a;
 *     int  b;
 * } my_struct;
 *
 * AW_RNGBUF_SPACE_DEFINE(char_buf, char, 64);      // 64��char����ռ�
 * AW_RNGBUF_SPACE_DEFINE(int_buf, int, 64);        // 64��int����ռ�
 * AW_RNGBUF_SPACE_DEFINE(my_buf, my_struct, 16);   // 16��my_struct����ռ�
 * \endcode
 * \hideinitializer
 */
#define AW_RNGBUF_SPACE_DEFINE(rb, element_type, num_elements) \
    struct __rngbuf_space_##rb { \
        element_type space[num_elements]; \
        char         resv[1]; \
    }; \
    static struct __rngbuf_space_##rb __rngbuf_space_##rb##_data

/**
 * \brief ��ȡʹ�ú�AW_RNGBUF_SPACE_DEFINE()����Ļ���ռ��С
 *
 * \param[in] rb ���λ�����
 *
 * \return ʹ�ú�AW_RNGBUF_SPACE_DEFINE()����Ļ���ռ��С
 * \hideinitializer
 */
#define AW_RNGBUF_SPACE_SIZE(rb) \
        (sizeof(__rngbuf_space_##rb##_data.space) + 1)

/**
 * \brief ��ȡʹ�ú�AW_RNGBUF_SPACE_DEFINE()����Ļ���ռ�ָ��
 *
 * \param[in] rb ���λ�����
 *
 * \return ʹ�ú�AW_RNGBUF_SPACE_DEFINE()����Ļ���ռ�ָ��
 * \hideinitializer
 */
#define AW_RNGBUF_SPACE_GET(rb) \
        (__rngbuf_space_##rb##_data.space)

/**
 * \brief ��ʼ��һ��ʹ�ú�AW_RNGBUF_SPACE_DEFINE()�����˴洢�ռ�Ļ��λ�����
 *
 * \param[in] rb ���λ�����
 *
 * \note ������ʹ�ú�AW_RNGBUF_SPACE_DEFINE()����ô洢�ռ䣬Ȼ�����ʹ�ñ���
 * \hideinitializer
 */
#define AW_RNGBUF_INIT(rb) \
    aw_rngbuf_init(&rb, \
                   (char *)AW_RNGBUF_SPACE_GET(rb), \
                   AW_RNGBUF_SPACE_SIZE(rb))

/**
 * \brief ����һ���Ѿ���ʼ�����Ļ��λ�����
 *
 * ʹ������궨��Ļ��λ�������������ʹ�ã��������ٵ���rngbuf_init()���г�ʼ��
 *
 * \param[in] rb            Ҫ����Ļ��λ�����
 * \param[in] element_type  ���λ������洢������Ԫ������, ��char�� int��ṹ���
 * \param[in] num_elements  ����Ԫ�ظ���
 *
 * \note - ���λ������洢�ռ��СΪ��sizeof(element_type) * num_elements
 *       - ��Ҫ�ٴ�ʹ��rngbuf_init()�Ի��λ��������г�ʼ��
 *
 * \par ʾ��
 * \code
 * typedef struct my_struct {
 *     char a;
 * } my_struct;
 *
 * int              data    = 1;
 * struct my_struct my_data = {0};
 *
 * AW_RNGBUF_DEFINE(char_buf, char, 64);    // 64��char�ռ�Ļ��λ�����
 * AW_RNGBUF_DEFINE(int_buf, int, 64);      // 64��int�ռ�Ļ��λ�����
 * AW_RNGBUF_DEFINE(my_buf, my_struct, 16); // 16��my_struct���͵Ļ��λ�����
 *
 * aw_rngbuf_putchar(&char_buf, 'a');
 * aw_rngbuf_put(&int_buf, (char *)&data, sizeof(data));
 * aw_rngbuf_put(&my_buf, (char *)&my_data, sizeof(my_data));
 * \endcode
 * \hideinitializer
 */
#define AW_RNGBUF_DEFINE(rb, element_type, num_elements) \
    AW_RNGBUF_SPACE_DEFINE(rb, element_type, num_elements); \
    struct aw_rngbuf rb = { \
        0, \
        0, \
        AW_RNGBUF_SPACE_SIZE(rb), \
        (char *)AW_RNGBUF_SPACE_GET(rb) \
    }

/**
 * \brief ���λ���������ṹ
 *
 * aw_rngbuf ����ʵ����һ�������ģ�һ���������һ������д�����ж�д��һ�������
 * �����İ�ȫ������Ҳ����˵������ж���aw_rngbuf_putchar����ͬһʱ�̽�����һ��
 * �������aw_rngbuf_getchar�����������Ϳ��Ա�֤aw_rngbuf���ݽṹ�����ݰ�ȫ
 *
 * ͬ���ģ��ж��е���aw_rngbuf_put��ͬһʱ�̽�����һ���������aw_rngbuf_get��
 * Ҳ�������Ϳ��Ա�֤aw_rngbuf���ݽṹ�����ݰ�ȫ
 * \note ��Ҫֱ�Ӳ������ṹ�ĳ�Ա
 */
struct aw_rngbuf {
    uint32_t    in;         /**< \brief ����������д��λ�� */
    uint32_t    out;        /**< \brief ���������ݶ���λ�� */
    int         size;       /**< \brief ��������С */
    char       *buf;        /**< \brief ������ */
};

/** \brief ���λ��������� */
typedef struct aw_rngbuf *aw_rngbuf_t;

/**
 * \brief ��ʼ�����λ���
 *
 * \param[in,out] p_rb    Ҫ��ʼ���Ļ��λ�����
 * \param[in]     p_buf   ������
 * \param[in]     size    ��������С��ϵͳ����һ���ֽڣ�ʵ�ʿ��ô�СΪ size - 1
 */
void aw_rngbuf_init (struct aw_rngbuf *p_rb, char *p_buf, size_t size);

/**
 * \brief ���һ���ֽڵ����λ�����
 *
 * \param[in] rb    Ҫ�����Ļ��λ���
 * \param[in] data  Ҫ��ŵ��������������ֽ�
 *
 * \retval 0 ���ݴ��ʧ�ܣ���������
 * \retval 1 ���ݳɹ����
 */
int aw_rngbuf_putchar (aw_rngbuf_t rb, const char data);

/**
 * \brief �ӻ��λ�����ȡ��һ���ֽ�����
 *
 * \param[in]  rb     Ҫ�����Ļ��λ���
 * \param[out] p_data ��������ֽڵ�ָ��
 *
 * \retval 0 ����ȡ��ʧ�ܣ���������
 * \retval 1 ���ݳɹ�ȡ��
 */
int aw_rngbuf_getchar (aw_rngbuf_t rb, char *p_data);

/**
 * \brief ��������ֽڵ����λ�����
 *
 * \param[in] rb     Ҫ�����Ļ��λ���
 * \param[in] p_buf  Ҫ��ŵ����λ����������ݻ���
 * \param[in] nbytes Ҫ��ŵ����λ����������ݸ���
 *
 * \return �ɹ���ŵ����ݸ���
 */
size_t aw_rngbuf_put (aw_rngbuf_t rb, const char *p_buf, size_t nbytes);

/**
 * \brief �ӻ��λ�������ȡ����
 *
 * \param[in]  rb     Ҫ�����Ļ��λ���
 * \param[out] p_buf  ��Ż�ȡ���ݵĻ���
 * \param[in ] nbytes Ҫ��ȡ�����ݸ���
 *
 * \return �ɹ���ȡ�����ݸ���
 */
size_t aw_rngbuf_get (aw_rngbuf_t rb, char *p_buf, size_t nbytes);

/**
 * \brief ��ջ��λ�����
 *
 * ���������ջ��λ�����������֮ǰ���ڻ����������ݻᶪʧ
 *
 * \param[in] rb Ҫ�����Ļ��λ���
 */
void aw_rngbuf_flush (aw_rngbuf_t rb);

/**
 * \brief ���Ի��λ������Ƿ�Ϊ��
 *
 * \param[in] rb Ҫ���ԵĻ��λ�����
 *
 * \return ���λ������շ���AW_TRUE, ���򷵻�AW_FALSE
 */
aw_bool_t aw_rngbuf_isempty (aw_rngbuf_t rb);

/**
 * \brief ���Ի��λ������Ƿ�����
 *
 * \param[in] rb Ҫ���ԵĻ��λ�����
 *
 * \return ���λ�����������AW_TRUE, ���򷵻�AW_FALSE
 */
aw_bool_t aw_rngbuf_isfull  (aw_rngbuf_t rb);

/**
 * \brief ��ȡ���λ��������пռ��С
 *
 * \param[in] rb Ҫ�жϵĻ��λ�����
 *
 * \return ���λ��������пռ��С
 */
size_t aw_rngbuf_freebytes (aw_rngbuf_t rb);

/**
 * \brief ��ȡ���λ������������������ֽڸ���
 *
 * \param[in] rb Ҫ�жϵĻ��λ�����
 *
 * \return ���λ�����������ֽڸ���
 */
size_t aw_rngbuf_nbytes (aw_rngbuf_t rb);

/**
 * \brief ���λ�����������һ���ֽڣ������ƶ�д������ָ��
 *
 * ����������λ�������д��һ���ֽڣ������ƶ�д������ָ�롣�������ֽڲ��ܱ�
 * ����aw_rngbuf_get()��ȡ������ֽڵ�д��λ��Ϊ���λ������еĵ�ǰд��λ��ƫ��
 *  offset ���ֽڡ����ƫ�� offset Ϊ0��Ϊ��ǰд��λ����һ���ֽڣ����������
 * ����aw_rngbuf_putchar()һ����ֻ��������ָ��û����ǰ�ƶ���
 *
 * ֻҪ��ȷ�ƶ�һ����������ָ�룬���������д�����λ��������������ݾͿ��Ա�����
 * �����ƶ���������ָ��ͨ������aw_rngbuf_move_ahead()��ɡ�
 *
 * \note �ڵ���aw_rngbuf_put_ahead()֮ǰ��Ҫȷ�����λ��������� offset + 1 ��
 *       �ֽڿ��пռ�
 *
 * \param[in] rb     Ҫ�����Ļ��λ�����
 * \param[in] byte   Ҫ���뻷�λ��������ֽ�����
 * \param[in] offset Ҫд���������Ի��е�ǰд��λ�õ�ƫ��λ�ã�����Ϊ����
 *
 * \par ʾ��
 * \code
 * aw_rngbuf_putchar(&rb, 'a');        // д������'a'
 *
 * aw_rngbuf_put_ahead(&rb, 'b', 0);   // д������'b'�������'b'������
 * aw_rngbuf_put_ahead(&rb, 'c', 1);   // д������'c'�����'c'Ҳ������
 * aw_rngbuf_move_ahead(&rb, 2);       // ��ǰ�ƶ�����ָ��2���ֽ�
 *
 * aw_rngbuf_get(&rb, buf, 3);         // ��ʱ���԰�"abc"��������
 * \endcode
 */
void aw_rngbuf_put_ahead  (aw_rngbuf_t rb, char byte, size_t offset);

/**
 * \brief �ѻ��λ������ĵ�ǰд��ָ����ǰ�ƶ� nbytes ���ֽ�
 *
 * ��������ѻ��λ������ĵ�ǰд��ָ����ǰ�ƶ� nbytes ���ֽڡ���ʹ��
 * aw_rngbuf_put_ahead()д���������ֽں�ʹ�����������ʹд�����Щ�ֽ��ܱ�����
 *
 * \param[in] rb     Ҫ�ƶ�����ָ��Ļ��λ�����
 * \param[in] nbytes ����ָ����ǰ�ƶ����ֽڸ���
 */
void aw_rngbuf_move_ahead (aw_rngbuf_t rb, size_t nbytes);

/** @}  grp_aw_if_rngbuf */

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AW_RNGBUF_H */

/* end of file */
