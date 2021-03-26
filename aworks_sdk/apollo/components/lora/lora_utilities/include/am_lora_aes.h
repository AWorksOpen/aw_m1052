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
 * \brief This is an AES implementation that uses only 8-bit byte operations on
 *  the cipher state.
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-21  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_AES_H
#define __AM_LORA_AES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup am_if_lora_aes
 * @copydoc am_lora_aes.h
 * @{
 */

#if 1
/** \brief AES encryption with a precomputed key schedule */
#define AM_LORA_AES_ENC_PREKEYED
#endif
#if 1
/** \brief AES decryption with a precomputed key schedule */
#define AM_LORA_AES_DEC_PREKEYED
#endif
#if 0
/** \brief AES encryption with 'on the fly' 128 bit keying */
#define AM_LORA_AES_ENC_128_OTFK
#endif
#if 0
/** \brief AES decryption with 'on the fly' 128 bit keying */
#define AM_LORA_AES_DEC_128_OTFK
#endif
#if 0
/** \brief AES encryption with 'on the fly' 256 bit keying */
#define AM_LORA_AES_ENC_256_OTFK
#endif
#if 0
/** \brief AES decryption with 'on the fly' 256 bit keying */
#define AM_LORA_AES_DEC_256_OTFK
#endif

#define AM_LORA_AES_N_ROW           4
#define AM_LORA_AES_N_COL           4
#define AM_LORA_AES_N_BLOCK         (AM_LORA_AES_N_ROW * AM_LORA_AES_N_COL)
#define AM_LORA_AES_N_MAX_ROUNDS    14

typedef uint8_t return_type;

/**
 * Warning: The key length for 256 bit keys overflows a byte (see comment below)
 */
typedef uint8_t length_type;

typedef struct {
    uint8_t ksch[(AM_LORA_AES_N_MAX_ROUNDS + 1) * AM_LORA_AES_N_BLOCK];
    uint8_t rnd;
} am_lora_aes_context_t;

/**
 * The following calls are for a precomputed key schedule
 *
 * NOTE: If the length_type used for the key length is an
 * unsigned 8-bit character, a key length of 256 bits must
 * be entered as a length in bytes (valid inputs are hence
 * 128, 192, 16, 24 and 32).
 */

#if defined(AM_LORA_AES_ENC_PREKEYED) || defined(AM_LORA_AES_DEC_PREKEYED)

return_type am_lora_aes_set_key (const uint8_t         key[],
                                 length_type           key_len,
                                 am_lora_aes_context_t ctx[1]);
#endif

#if defined(AM_LORA_AES_ENC_PREKEYED)

return_type am_lora_aes_encrypt (
                const uint8_t               in[AM_LORA_AES_N_BLOCK],
                uint8_t                     out[AM_LORA_AES_N_BLOCK],
                const am_lora_aes_context_t ctx[1]);

return_type am_lora_aes_cbc_encrypt (
                const uint8_t              *p_in,
                uint8_t                    *p_out,
                int32_t                     n_block,
                uint8_t                     iv[AM_LORA_AES_N_BLOCK],
                const am_lora_aes_context_t ctx[1]);
#endif

#if defined(AM_LORA_AES_DEC_PREKEYED)

return_type am_lora_aes_decrypt (
                const uint8_t               in[AM_LORA_AES_N_BLOCK],
                uint8_t                     out[AM_LORA_AES_N_BLOCK],
                const am_lora_aes_context_t ctx[1]);

return_type am_lora_aes_cbc_decrypt (
                const uint8_t              *p_in,
                uint8_t                    *p_out,
                int32_t                     n_block,
                uint8_t                     iv[AM_LORA_AES_N_BLOCK],
                const am_lora_aes_context_t ctx[1]);
#endif

/**
 * The following calls are for 'on the fly' keying.  In this case the
 * encryption and decryption keys are different.
 *
 * The encryption subroutines take a key in an array of bytes in
 * key[L] where L is 16, 24 or 32 bytes for key lengths of 128,
 * 192, and 256 bits respectively.  They then encrypts the input
 * data, in[] with this key and put the reult in the output array
 * out[].  In addition, the second key array, o_key[L], is used
 * to output the key that is needed by the decryption subroutine
 * to reverse the encryption operation.  The two key arrays can
 * be the same array but in this case the original key will be
 * overwritten.
 *
 * In the same way, the decryption subroutines output keys that
 * can be used to reverse their effect when used for encryption.
 *
 * Only 128 and 256 bit keys are supported in these 'on the fly'
 * modes.
 */

#if defined(AM_LORA_AES_ENC_128_OTFK)
void am_lora_aes_encrypt_128 (const uint8_t in[AM_LORA_AES_N_BLOCK],
                              uint8_t       out[AM_LORA_AES_N_BLOCK],
                              const uint8_t key[AM_LORA_AES_N_BLOCK],
                              uint8_t       o_key[AM_LORA_AES_N_BLOCK]);
#endif

#if defined(AM_LORA_AES_DEC_128_OTFK)
void am_lora_aes_decrypt_128 (const uint8_t in[AM_LORA_AES_N_BLOCK],
                              uint8_t       out[AM_LORA_AES_N_BLOCK],
                              const uint8_t key[AM_LORA_AES_N_BLOCK],
                              uint8_t       o_key[AM_LORA_AES_N_BLOCK]);
#endif

#if defined(AM_LORA_AES_ENC_256_OTFK)
void am_lora_aes_encrypt_256 (const uint8_t in[AM_LORA_AES_N_BLOCK],
                              uint8_t       out[AM_LORA_AES_N_BLOCK],
                              const uint8_t key[2 * AM_LORA_AES_N_BLOCK],
                              uint8_t       o_key[2 * AM_LORA_AES_N_BLOCK]);
#endif

#if defined(AM_LORA_AES_DEC_256_OTFK)
void am_lora_aes_decrypt_256 (const uint8_t in[AM_LORA_AES_N_BLOCK],
                              uint8_t out[AM_LORA_AES_N_BLOCK],
                              const uint8_t key[2 * AM_LORA_AES_N_BLOCK],
                              uint8_t o_key[2 * AM_LORA_AES_N_BLOCK]);
#endif

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_AES_H */

/* end of file */
