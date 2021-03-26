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
 * \brief 数学辅助函数实现
 *
 * \internal
 * \par Modification history
 * - 1.0.0 17-09-21  sky, first implementation.
 * \endinternal
 */

#ifndef __AM_LORA_DATA_H
#define __AM_LORA_DATA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <time.h>

/**
 * @addtogroup am_if_lora_data
 * @copydoc am_lora_data.h
 * @{
 */

/**
 * \brief Initializes the pseudo random generator initial value
 * \param [in] seed : Pseudo random generator initial value
 */
void am_lora_srand1 (uint32_t seed);

/**
 * \brief Generate a 32bits random
 * \retval random 32bits random value
 */
int32_t am_lora_rand1 (void);

/**
 * \brief Computes a random number between min and max
 * \param [in] min : range minimum value
 * \param [in] max : range maximum value
 * \retval random random value in range min..max
 */
int32_t am_lora_randr (int32_t min, int32_t max);

/**
 * \brief Copies size elements of src array to dst array
 *
 * \remark avoid standard memset function only works on pointers that are aligned
 *
 * \param [out] dst  :  Destination array
 * \param [in]  src  : Source array
 * \param [in]  size : Number of bytes to be copied
 */
void am_lora_memcpy1 (uint8_t *p_dst, const uint8_t *p_src, uint32_t size);

/**
 * \brief Copies size elements of src array to dst array reversing the byte order
 *
 * \remark avoid standard memset function only works on pointers that are aligned
 *
 * \param [out] dst  : Destination array
 * \param [in]  src  : Source array
 * \param [in]  size : Number of bytes to be copied
 */
void am_lora_memcpyr (uint8_t *p_dst, const uint8_t *p_src, uint32_t size);

/**
 * \brief Set size elements of dst array with value
 *
 * \remark avoid standard memset function only works on pointers that are aligned
 *
 * \param [out] dst   : Destination array
 * \param [in]  value : Default value
 * \param [in]  size  : Number of bytes to be copied
 */
void am_lora_memset1 (uint8_t *p_dst, uint8_t value, uint32_t size);

/**
 * \brief Converts a nibble to an hexadecimal character
 * \param [in] a : Nibble to be converted
 * \retval hexChar Converted hexadecimal character
 */
int8_t am_lora_nibble_to_hex_char (uint8_t a);

/**
 * \brief calc the ones in a 16 bits array
 *
 * \param [in] max_nb  : how many numbers in the array
 * \param [in] p_input : point to the array
 *
 * \retval number of ones
 */
uint8_t am_lora_calc_ones_of_16bits_array (uint8_t max_nb, uint16_t *p_input);

/**
 * \brief reverse a 8 bits number
 * \param [in] num : number which is wait for reverse
 * \retval the reverse result
 */
uint8_t am_lora_reverse_8bits (uint8_t num);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __AM_LORA_DATA_H */

/* end of file */
