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
 * \brief 基于AES的密码认证实现
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-21  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_CMAC_H
#define __AM_LORA_CMAC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "am_lora_aes.h"

/**
 * @addtogroup am_if_lora_cmac
 * @copydoc am_lora_cmac.h
 * @{
 */

#define AES_CMAC_KEY_LENGTH     16
#define AES_CMAC_DIGEST_LENGTH  16
 
typedef struct am_lora_aes_cmac_ctx {
    am_lora_aes_context_t rijndael;
    uint8_t               x[16];
    uint8_t               m_last[16];
    uint32_t              m_n;
} am_lora_aes_cmac_ctx_t;
   
//#include <sys/cdefs.h>
    
/* __BEGIN_DECLS */
void am_lora_aes_cmac_init (am_lora_aes_cmac_ctx_t *p_ctx);
void am_lora_aes_cmac_setkey (am_lora_aes_cmac_ctx_t *p_ctx,
                              const uint8_t           key[AES_CMAC_KEY_LENGTH]);
void am_lora_aes_cmac_update (am_lora_aes_cmac_ctx_t *p_ctx,
                              const uint8_t          *p_data,
                              uint32_t                len);
            /*      __attribute__((__bounded__(__string__, 2, 3))); */
void am_lora_aes_cmac_final (
         am_lora_aes_cmac_ctx_t *p_ctx,
         uint8_t                 digest[AES_CMAC_DIGEST_LENGTH]);
/*     __attribute__((__bounded__(__minbytes__, 1, AES_CMAC_DIGEST_LENGTH))); */
/* __END_DECLS */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_CMAC_H */

/* end of file */
