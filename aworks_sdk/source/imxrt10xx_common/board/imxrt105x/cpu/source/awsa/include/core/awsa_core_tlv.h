/*******************************************************************************
*                                 AWorks
*                       ----------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2016 Guangzhou ZHIYUAN Electronics Stock Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      apollo.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief TLV(Type-Length-Value)
 *
 *        uint32_t type      - see AWSA_CORE_CTL_TLVT_*
 *        uint32_t length
 *        data aligned to sizeof(uint32_t), use
 *        block_length = (length + (sizeof(unsigned int) - 1)) &
 *                       ~(sizeof(unsigned int) - 1)) ....
 *
 * 使用本模块需要包含以下头文件：
 * \code
 * \endcode
 *
 * \internal
 * \par modification history:
 * - 1.00 16-11-11  win, first implementation
 * \endinternal
 */

#ifndef __AWSA_CORE_TLV_H
#define __AWSA_CORE_TLV_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

/**
 * \brief db scale type
 */
enum awsa_core_tlv_type {

    /** \brief group of TLVs */
    AWSA_CORE_TLV_TYPE_CONTAINER       = 0,

    /** \brief dB scale */
    AWSA_CORE_TLV_TYPE_DB_SCALE,

    /** \brief linear volume */
    AWSA_CORE_TLV_TYPE_DB_LINEAR,

    /** \brief dB range container */
    AWSA_CORE_TLV_TYPE_DB_RANGE,

    /** \brief dB scale with min/max */
    AWSA_CORE_TLV_TYPE_DB_MINMAX,

    /** \brief dB scale with min/max with mute */
    AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE,
};

/**
 * \name DB SCALE Step Values
 * @{
 */

/** \brief maks */
#define AWSA_CORE_TLV_DB_SCALE_MASK       (0xffff)

/** \brief mute */
#define AWSA_CORE_TLV_DB_SCALE_MUTE       (0x10000)

/**
 * \brief dB Scale
 * \note type     AWSA_CORE_TLV_CTL_DB_SCALE
 *       length   2 * sizeof(uint32_t)
 *       value    xmin, mute | step
 */
#define AWSA_CORE_TLV_DB_SCALE_ITEM(xmin, xstep, xmute)     \
    AWSA_CORE_TLV_TYPE_DB_SCALE,                            \
    2 * sizeof(uint32_t),                                   \
    (uint32_t)(xmin),                                       \
    (uint32_t)((xstep) & AWSA_CORE_TLV_DB_SCALE_MASK)    |  \
    (uint32_t)((xmute) ? AWSA_CORE_TLV_DB_SCALE_MUTE : 0)

/**
 * \brief DB SCALE Decl
 */
#define AWSA_CORE_TLV_DB_SCALE_DECL(xname, xmin, xstep, xmute) \
    uint32_t xname[] = { AWSA_CORE_TLV_DB_SCALE_ITEM(xmin, xstep, xmute) }

/** @} */

/**
 * \name dB scale with min/max
 * @{
 */

/**
 * \brief dB scale specified with min/max values instead of step
 * \note type      AWSA_CORE_TLV_CTL_DB_MINMAX
 *       length    2 * sizeof(uint32_t)
 *       value     min_dB, max_dB
 */
#define AWSA_CORE_TLV_DB_MINMAX_ITEM(min_dB, max_dB)            \
    AWSA_CORE_TLV_TYPE_DB_MINMAX,                               \
    2 * sizeof(uint32_t),                                       \
    (uint32_t)(min_dB), (uint32_t)(max_dB)

/**
 * \brief dB scale specified with min/max values with mute
 * \note type      AWSA_CORE_TLV_CTL_DB_MINMAX_MUTE
 *       length    2 * sizeof(uint32_t)
 *       value     min_dB, max_dB
 */
#define AWSA_CORE_TLV_DB_MINMAX_MUTE_ITEM(min_dB, max_dB)       \
    AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE,                          \
    2 * sizeof(uint32_t),                                       \
    (uint32_t)(min_dB), (uint32_t)(max_dB)

/**
 * \brief dB scale with min/max declare
 */
#define AWSA_CORE_TLV_DB_MINMAX_DECL(name, min_dB, max_dB)      \
    uint32_t name[] = { AWSA_CORE_TLV_DB_MINMAX_ITEM(min_dB, max_dB) }

/**
 * \brief dB scale with min/max with mute declare
 */
#define AWSA_CORE_TLV_DB_MINMAX_MUTE_DECL(name, min_dB, max_dB) \
    uint32_t name[] = { AWSA_CORE_TLV_DB_MINMAX_MUTE_ITEM(min_dB, max_dB) }

/** @} */

/**
 * \name linear volume
 * @{
 */

/**
 * \brief linear volume between min_dB and max_dB (.01dB unit).
 * \note type    AWSA_CORE_TLV_CTL_DB_LINEAR
 *       length  2 * sizeof(uint32_t)
 *       value   min_dB, max_dB
 */
#define AWSA_CORE_TLV_DB_LINEAR_ITEM(min_dB, max_dB)            \
    AWSA_CORE_TLV_TYPE_DB_LINEAR,                               \
    2 * sizeof(uint32_t),                                       \
    (uint32_t)(min_dB), (uint32_t)(max_dB)

/**
 * \brief linear volume declare
 */
#define AWSA_CORE_TLV_DB_LINEAR_DECL(name, min_dB, max_dB)      \
    uint32_t name[] = { AWSA_CORE_TLV_DB_LINEAR_ITEM(min_dB, max_dB) }

/** @} */

/**
 * \name dB range container
 * @{
 */

/**
 * \brief dB range container head
 * \note type    AWSA_CORE_TLV_CTL_DB_RANGE
 *       length  6 * (num) * sizeof(uint32_t)
 *       value   Each item is: <min> <max> <TLV>, each item TLV is 4
 *               words(uint32_t)
 *
 * \code
 * uint32_t db_range[] = {
 *     AWSA_CORE_TLV_DB_RANGE_HEAD(1),
 *     0, 10, AWSA_CORE_TLV_DB_SCALE_ITEM(-12000, 50, 0),
 * }
 * \endcode
 */
#define AWSA_CORE_TLV_DB_RANGE_HEAD(num)          \
    AWSA_CORE_TLV_TYPE_DB_RANGE, 6 * (num) * sizeof(uint32_t)

/** @} */

/** \brief db scale gain with mute */
#define AWSA_CORE_TLV_DB_GAIN_MUTE    (-9999999)

/** \brief convert to index of integer array */
#define AWSA_CORE_TLV_INT_INDEX(size)          \
    (((size) + sizeof(uint32_t) - 1) / sizeof(uint32_t))

/**
 * \brief max size of a TLV entry for dB information (including compound one)
 */
#define AWSA_CORE_TLV_RANGE_SIZE_MAX   256


/**
 * \brief Parse TLV stream and retrieve dB information
 */
int awsa_core_tlv_db_info_parse (const uint32_t  *p_tlv,
                                 const uint32_t **pp_db_tlvp);

/**
 * \brief Get the dB min values and max values
 */
aw_err_t awsa_core_tlv_db_range_get (const uint32_t *p_tlv,
                                     int32_t         range_min,
                                     int32_t         range_max,
                                     int32_t        *p_min,
                                     int32_t        *p_max);

/**
 * \brief Convert the given raw volume value to a dB gain
 *
 * \param[in] p_tlv     : the TLV source
 * \param[in] range_min : the minimum value of the raw volume
 * \param[in] range_max : the maximum value of the raw volume
 * \param[in] volume    : the raw volume value to convert
 * \param[in] p_db_gain : the dB gain (in 0.01dB unit)
 *
 * \return AW_OK if successful, or a negative error code
 */
aw_err_t awsa_core_tlv_convert_to_db (const uint32_t *p_tlv,
                                      int32_t         range_min,
                                      int32_t         range_max,
                                      int32_t         volume,
                                      int32_t        *p_db_gain);

/**
 * \brief Convert from dB gain to the corresponding raw value
 *
 * \param[in] p_tlv     : the TLV source
 * \param[in] range_min : the minimum value of the raw volume
 * \param[in] range_max : the maximum value of the raw volume
 * \param[in] db_gain   : the dB gain to convert (in 0.01dB unit)
 * \param[in] p_volume  : the pointer to store the converted raw volume value
 * \param[in] dir       : the direction for round-up. The value is round up
 *                        when this is positive.
 *
 * \return AW_OK if successful, or a negative error code
 */
aw_err_t awsa_core_tlv_convert_from_db (const uint32_t  *p_tlv,
                                        int32_t          range_min,
                                        int32_t          range_max,
                                        int32_t          db_gain,
                                        int32_t         *p_volume,
                                        int              dir);

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_TLV_H */

/* end of file */

