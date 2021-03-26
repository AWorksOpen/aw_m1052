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
 * 使用本服务需要包含头文件 aw_aes.h
 *
 * \par 使用示例
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

/** \brief 自动模式，由程序选择硬件或者软件实现 */
#define CRC_FLAG_AUTO        1 

/** \brief 指定由硬件实现*/      
#define CRC_FLAG_HARDWARE    2 

/** \brief 指定由软件实现*/   
#define CRC_FLAG_SOFTWARE    4    

#define AES_MAX_KEYLENGTH       (15 * 16)
#define AES_MAX_KEYLENGTH_U32   (AES_MAX_KEYLENGTH / sizeof(uint32_t))

/**
 * \brief AES 上下文
 */
typedef struct aes_key {
    uint32_t key[AES_MAX_KEYLENGTH_U32 + 1];
    uint32_t key_length;
}aes_key_t;

/**
 * \brief 初始化加密Key
 *
 * \param[in] p_key   密钥字符串
 * \param[in] key_len 密钥长度，以bit为单位
 * \param[in] key     密钥上下文变量
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  key为空
 */
aw_err_t aw_aes_set_encrypt_key(const uint8_t *p_key, 
                                size_t         key_len,
                                aes_key_t     *key);

/**
 * \brief 初始化解密Key
 *
 * \param[in] p_key   密钥字符串
 * \param[in] key_len 密钥长度，以bit为单位
 * \param[in] key     密钥上下文变量
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  key为空
 */
aw_err_t aw_aes_set_decrypt_key(const uint8_t *p_key, 
                                size_t         key_len,
                                aes_key_t     *key);

/**
 * \brief aes加密单个数据块
 *
 * \param[in] in   明文字符串
 * \param[in] out  加密后密文字符串
 * \param[in] key  密钥上下文变量
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  key为空
 */
aw_err_t aw_aes_encrypt(uint8_t *in, uint8_t *out, aes_key_t *key);

/**
 * \brief aes解密单个数据块
 *
 * \param[in] in   密文字符串
 * \param[in] out  解密后的明文字符串
 * \param[in] key  密钥上下文变量
 *
 * \retval  AW_OK      成功
 * \retval -AW_EINVAL  aes为空
 */
aw_err_t aw_aes_decrypt(uint8_t *in, uint8_t *out, aes_key_t aes);

/**
 * \brief aes加密/解密单个数据块 ECB模式
 *
 * \param[in] in   需要加密/解密的字符串
 * \param[in] out  计算后输出字符串
 * \param[in] key  密钥上下文变量
 * \param[in] enc  加密解密标志位
 *                    - AES_ENCRYPT 加密
 *                    - AES_ENCRYPT 解密
 *
 */
aw_err_t aw_aes_ecb_encrypt(const uint8_t    *in, 
                            uint8_t          *out,
                            aes_key_t        *key,
                            const int         enc);

/**
 * \brief aes加密/解密单个数据块 CBC模式
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
 *
 */
aw_err_t aw_aes_cbc_encrypt(const uint8_t   *in, 
                            uint8_t         *out,
                            size_t           length,
                            const aes_key_t *key,
                            uint8_t         *ivec,
                            const int        enc);

/**
 * \brief aes CFB128位加密/解密，输入输出数据区可以重叠
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param num    输出参数，计算状态，多少个CFB数据块
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
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
 * \brief aes CFB8位加密/解密，输入输出数据区可以重叠
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param num    输出参数，计算状态，多少个CFB数据块
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
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
 * \brief aes CFB1位加密/解密，输入输出数据区可以重叠
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param num    输出参数，计算状态，多少个CFB数据块
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
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
 * \brief aes OFB128位加密/解密，输入输出数据区可以重叠
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param num    输出参数，计算状态，多少个OFB数据块
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
 *
 */
aw_err_t AES_ofb128_encrypt(const uint8_t   *in, 
                            uint8_t         *out,
                            size_t           length,
                            const aes_key_t *key,
                            uint8_t         *ivec,
                            int             *num);

/**
 * \brief aes CTR128位加密/解密，输入输出数据区可以重叠
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param ecount_buf 输出参数，加密的次数，在第一次调用此函数时，需要初始化为0
 * \param num    输出参数，计算状态，多少个CTR数据块，在第一次调用此函数时，需要
 *               初始化为0
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
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
 * \brief aes加密/解密，输入输出数据区可以重叠，初始化向量是加密数据块的2倍，
 *        加密前用前半部分做一次异或加密，加密后用后半部分做一次异或
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
 *
 */
aw_err_t AES_ige_encrypt(const uint8_t     *in, 
                         uint8_t           *out,
                         size_t             length,
                         const aes_key_t   *key,
                         uint8_t           *ivec,
                         const int          enc);

/**
 * \brief aes加密/解密，输入输出数据区可以重叠，初始化向量是加密数据块的4倍，
 *        加密前用第一部分做一次异或加密，加密后用第二部分做一次异或；
 *        最后一个加密数据块，加密前用第三部分异或，加密后用第四部分异或
 *
 * \param in     需要加密/解密的字符串
 * \param out    计算后输出字符串
 * \param length 数据长度
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param enc    加密解密标志位
 *               - AES_ENCRYPT 加密
 *               - AES_ENCRYPT 解密
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
 * \brief aes算法对名为key数据加密
 *
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param out    加密后的密钥数据
 * \param in     密钥数据
 * \param inlen  密钥数据长度
 *
 * \retval AW_OK 成功
 */
aw_err_t AES_wrap_key(aes_key_t       *key, 
                      const uint8_t   *ivec,
                      uint8_t         *out,
                      const uint8_t   *in,
                      uint32_t         inlen);

/**
 * \brief aes算法对名为key数据解密
 *
 * \param key    密钥上下文变量
 * \param ivec   初始化向量
 * \param out    加密后的密钥数据
 * \param in     密钥数据
 * \param inlen  密钥数据长度
 *
 * \retval AW_OK 成功
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
