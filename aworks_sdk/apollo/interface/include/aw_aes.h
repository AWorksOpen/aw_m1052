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
 * \brief AES(Advanced Encryption Standard) algorithms standard header
 *        - aes-[128|192|256]-cbc  128/192/256 bit AES in CBC mode
 *        - aes-[128|192|256]      Alias for aes-[128|192|256]-cbc
 *        - aes-[128|192|256]-cfb  128/192/256 bit AES in 128 bit CFB mode
 *        - aes-[128|192|256]-cfb1 128/192/256 bit AES in 1 bit CFB mode
 *        - aes-[128|192|256]-cfb8 128/192/256 bit AES in 8 bit CFB mode
 *        - aes-[128|192|256]-ecb  128/192/256 bit AES in ECB mode
 *        - aes-[128|192|256]-ofb  128/192/256 bit AES in OFB mode
 *
 * ʹ�ñ�������Ҫ����ͷ�ļ� aw_aes.h
 *
 * \par ʹ��ʾ��
 * \code
 * #include "aw_aes.h"
 *
 * #define KEY_LEN AES_KEYSIZE_128
 * #define CODE_LEN   10
 *
 *  uint8_t key[KEY_LEN] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
 *                          0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
 *  uint8_t src[CODE_LEN]= {0x32, 0x43, 0xf6, 0xad, 0x88, 0x5a, 0x30, 0x8d, 
 *                          0x31, 0x31};
 *  uint8_t dst[CODE_LEN] = {0};
 *  uint8_t src_org[CODE_LEN] = {0};
 *  aes_key_t  aes_ctx;
 *
 *  aw_aes_set_key(aes_ctx, key, KEY_LEN);
 *  aw_aes_encrypt(aes_ctx, src, dst);
 *  aw_aes_decrtpt(aes_ctx, dst, src_org);
 *
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.01 14-11-05 fft, add hardware support and some notes
 * - 1.00 14-06-06 zen, first implementation
 * \endinternal
 */

#ifndef __AW_AES_H
#define __AW_AES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "aw_types.h"

#define AES_ENCRYPT 1
#define AES_DECRYPT 0

/** \brief Both sizes are in bytes */
#define AES_BLOCK_SIZE 16


#define AES_KEYSIZE_128      128
#define AES_KEYSIZE_192      192
#define AES_KEYSIZE_256      256

/** \brief �Զ�ģʽ���ɳ���ѡ��Ӳ���������ʵ�� */
#define CRC_FLAG_AUTO        1 

/** \brief ָ����Ӳ��ʵ��*/      
#define CRC_FLAG_HARDWARE    2 

/** \brief ָ�������ʵ��*/   
#define CRC_FLAG_SOFTWARE    4    

#define AES_MAX_KEYLENGTH       (15 * 16)
#define AES_MAX_KEYLENGTH_U32   (AES_MAX_KEYLENGTH / sizeof(uint32_t))

/**
 * \brief AES ������
 */
typedef struct aes_key {
    uint32_t key[AES_MAX_KEYLENGTH_U32 + 1];
    uint32_t key_length;
}aes_key_t;

/**
 * \brief ��ʼ������Key
 *
 * \param[in] p_key   ��Կ�ַ���
 * \param[in] key_len ��Կ���ȣ���bitΪ��λ
 * \param[in] key     ��Կ�����ı���
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  keyΪ��
 */
aw_err_t aw_aes_set_encrypt_key(const uint8_t *p_key, 
                                size_t         key_len,
                                aes_key_t     *key);

/**
 * \brief ��ʼ������Key
 *
 * \param[in] p_key   ��Կ�ַ���
 * \param[in] key_len ��Կ���ȣ���bitΪ��λ
 * \param[in] key     ��Կ�����ı���
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  keyΪ��
 */
aw_err_t aw_aes_set_decrypt_key(const uint8_t *p_key, 
                                size_t         key_len,
                                aes_key_t     *key);

/**
 * \brief aes���ܵ������ݿ�
 *
 * \param[in] in   �����ַ���
 * \param[in] out  ���ܺ������ַ���
 * \param[in] key  ��Կ�����ı���
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  keyΪ��
 */
aw_err_t aw_aes_encrypt(uint8_t *in, uint8_t *out, aes_key_t *key);

/**
 * \brief aes���ܵ������ݿ�
 *
 * \param[in] in   �����ַ���
 * \param[in] out  ���ܺ�������ַ���
 * \param[in] key  ��Կ�����ı���
 *
 * \retval  AW_OK      �ɹ�
 * \retval -AW_EINVAL  aesΪ��
 */
aw_err_t aw_aes_decrypt(uint8_t *in, uint8_t *out, aes_key_t aes);

/**
 * \brief aes����/���ܵ������ݿ� ECBģʽ
 *
 * \param[in] in   ��Ҫ����/���ܵ��ַ���
 * \param[in] out  ���������ַ���
 * \param[in] key  ��Կ�����ı���
 * \param[in] enc  ���ܽ��ܱ�־λ
 *                    - AES_ENCRYPT ����
 *                    - AES_ENCRYPT ����
 *
 */
aw_err_t aw_aes_ecb_encrypt(const uint8_t    *in, 
                            uint8_t          *out,
                            aes_key_t        *key,
                            const int         enc);

/**
 * \brief aes����/���ܵ������ݿ� CBCģʽ
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t aw_aes_cbc_encrypt(const uint8_t   *in, 
                            uint8_t         *out,
                            size_t           length,
                            const aes_key_t *key,
                            uint8_t         *ivec,
                            const int        enc);

/**
 * \brief aes CFB128λ����/���ܣ�������������������ص�
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param num    �������������״̬�����ٸ�CFB���ݿ�
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_cfb128_encrypt(const uint8_t   *in, 
                            uint8_t         *out,
                            size_t           length,
                            const aes_key_t *key,
                            uint8_t         *ivec,
                            int             *num,
                            const int        enc);

/**
 * \brief aes CFB8λ����/���ܣ�������������������ص�
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param num    �������������״̬�����ٸ�CFB���ݿ�
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_cfb8_encrypt(const uint8_t   *in, 
                          uint8_t         *out,
                          size_t           length,
                          const aes_key_t *key,
                          uint8_t         *ivec,
                          int             *num,
                          const int        enc);

/**
 * \brief aes CFB1λ����/���ܣ�������������������ص�
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param num    �������������״̬�����ٸ�CFB���ݿ�
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_cfb1_encrypt(const uint8_t   *in, 
                          uint8_t         *out,
                          size_t           length,
                          const aes_key_t *key,
                          uint8_t         *ivec,
                          int             *num,
                          const int        enc);

/**
 * \brief aes OFB128λ����/���ܣ�������������������ص�
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param num    �������������״̬�����ٸ�OFB���ݿ�
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_ofb128_encrypt(const uint8_t   *in, 
                            uint8_t         *out,
                            size_t           length,
                            const aes_key_t *key,
                            uint8_t         *ivec,
                            int             *num);

/**
 * \brief aes CTR128λ����/���ܣ�������������������ص�
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param ecount_buf ������������ܵĴ������ڵ�һ�ε��ô˺���ʱ����Ҫ��ʼ��Ϊ0
 * \param num    �������������״̬�����ٸ�CTR���ݿ飬�ڵ�һ�ε��ô˺���ʱ����Ҫ
 *               ��ʼ��Ϊ0
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_ctr128_encrypt(const uint8_t     *in, 
                            uint8_t           *out,
                            size_t             length,
                            const aes_key_t   *key,
                            uint8_t            ivec[AES_BLOCK_SIZE],
                            uint8_t            ecount_buf[AES_BLOCK_SIZE],
                            uint32_t          *num);

/**
 * \brief aes����/���ܣ�������������������ص�����ʼ�������Ǽ������ݿ��2����
 *        ����ǰ��ǰ�벿����һ�������ܣ����ܺ��ú�벿����һ�����
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_ige_encrypt(const uint8_t     *in, 
                         uint8_t           *out,
                         size_t             length,
                         const aes_key_t   *key,
                         uint8_t           *ivec,
                         const int          enc);

/**
 * \brief aes����/���ܣ�������������������ص�����ʼ�������Ǽ������ݿ��4����
 *        ����ǰ�õ�һ������һ�������ܣ����ܺ��õڶ�������һ�����
 *        ���һ���������ݿ飬����ǰ�õ���������򣬼��ܺ��õ��Ĳ������
 *
 * \param in     ��Ҫ����/���ܵ��ַ���
 * \param out    ���������ַ���
 * \param length ���ݳ���
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param enc    ���ܽ��ܱ�־λ
 *               - AES_ENCRYPT ����
 *               - AES_ENCRYPT ����
 *
 */
aw_err_t AES_bi_ige_encrypt(const uint8_t    *in, 
                            uint8_t          *out,
                            size_t            length,
                            const aes_key_t  *key,
                            const aes_key_t  *key2,
                            const uint8_t    *ivec,
                            const int         enc);

/**
 * \brief aes�㷨����Ϊkey���ݼ���
 *
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param out    ���ܺ����Կ����
 * \param in     ��Կ����
 * \param inlen  ��Կ���ݳ���
 *
 * \retval AW_OK �ɹ�
 */
aw_err_t AES_wrap_key(aes_key_t       *key, 
                      const uint8_t   *ivec,
                      uint8_t         *out,
                      const uint8_t   *in,
                      uint32_t         inlen);

/**
 * \brief aes�㷨����Ϊkey���ݽ���
 *
 * \param key    ��Կ�����ı���
 * \param ivec   ��ʼ������
 * \param out    ���ܺ����Կ����
 * \param in     ��Կ����
 * \param inlen  ��Կ���ݳ���
 *
 * \retval AW_OK �ɹ�
 */
aw_err_t AES_unwrap_key(aes_key_t       *key, 
                        const uint8_t   *iv,
                        uint8_t         *out,
                        const uint8_t   *in,
                        uint32_t         inlen);


#ifdef __cplusplus
}
#endif

#endif  /* __AW_AES_H */

/* end of file */
