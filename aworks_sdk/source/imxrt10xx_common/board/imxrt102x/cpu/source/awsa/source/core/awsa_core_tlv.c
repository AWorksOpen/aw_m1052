/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded systems
*
* Copyright (c) 2001-2018 Guangzhou ZHIYUAN Electronics Stock Co.,  Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
* e-mail:      ametal.support@zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief 
 *
 * \internal
 * \par Modification history
 * - 1.00 18-06-14  win, first implementation.
 * \endinternal
 */

#include "core/awsa_core_internal.h"

/**
 * \brief Parse TLV stream and retrieve dB information
 */
int awsa_core_tlv_db_info_parse (const uint32_t  *p_tlv,
                                 const uint32_t **pp_db_tlvp)
{
    uint32_t type;
    uint32_t size, len, min_size;
    aw_err_t err;

    *pp_db_tlvp = NULL;
    type        = p_tlv[0];
    size        = p_tlv[1];

    switch (type) {

    case AWSA_CORE_TLV_TYPE_CONTAINER:
        size   = AWSA_CORE_TLV_INT_INDEX(size) * sizeof(uint32_t);
        p_tlv += 2;
        while (size > 0) {
            err = awsa_core_tlv_db_info_parse(p_tlv, pp_db_tlvp);
            if (err < 0) {
                return err; /* error */
            }

            if (err > 0) {
                return err; /* found */
            }

            len    = AWSA_CORE_TLV_INT_INDEX(p_tlv[1]) + 2;
            size  -= len * sizeof(uint32_t);
            p_tlv += len;
        }
        break;

    case AWSA_CORE_TLV_TYPE_DB_SCALE:
    case AWSA_CORE_TLV_TYPE_DB_MINMAX:
    case AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE:
    case AWSA_CORE_TLV_TYPE_DB_LINEAR:
    case AWSA_CORE_TLV_TYPE_DB_RANGE: {
        if (type == AWSA_CORE_TLV_TYPE_DB_RANGE) {
            min_size = 4 * sizeof(uint32_t);
        } else {
            min_size = 2 * sizeof(uint32_t);
        }

        if (size < min_size) {
            AWSA_CORE_DEBUG_ERROR("ctl invalid dB scale tlv size");
            return -AW_EINVAL;
        }

        if (size > AWSA_CORE_TLV_RANGE_SIZE_MAX) {
            AWSA_CORE_DEBUG_ERROR("ctl too big dB scale tlv size: %d", size);
            return -AW_EINVAL;
        }
        *pp_db_tlvp = p_tlv;
        return size + sizeof(uint32_t) * 2;
    }

    default:
        break;
    }

    return -AW_EINVAL; /* not found */
}

/**
 * \brief Get the dB min/max values
 */
aw_err_t awsa_core_tlv_db_range_get (const uint32_t *p_tlv,
                                     int32_t         range_min,
                                     int32_t         range_max,
                                     int32_t        *p_min,
                                     int32_t        *p_max)
{
    aw_err_t err;
    uint32_t pos, len;

    int32_t  rmin = 0, rmax = 0;
    int32_t  sub_min,  sub_max;
    int32_t  step;

    switch (p_tlv[0]) {

    case AWSA_CORE_TLV_TYPE_DB_RANGE: {

        len = AWSA_CORE_TLV_INT_INDEX(p_tlv[1]);
        if (len > AWSA_CORE_TLV_RANGE_SIZE_MAX) {
            return -AW_EINVAL;
        }
        pos = 2;
        while (pos + 4 <= len) {

            sub_min = (int32_t)p_tlv[pos];
            sub_max = (int32_t)p_tlv[pos + 1];
            if (range_max < sub_max) {
                sub_max = range_max;
            }

            err = awsa_core_tlv_db_range_get(p_tlv + pos + 2,
                                             sub_min,
                                             sub_max,
                                             &rmin,
                                             &rmax);
            if (err != AW_OK) {
                return err;
            }

            if (pos > 2) {
                if (rmin < *p_min) {
                    *p_min = rmin;
                }

                if (rmax > *p_max) {
                    *p_max = rmax;
                }
            } else {
                *p_min = rmin;
                *p_max = rmax;
            }

            if (range_max == sub_max) {
                return 0;
            }

            pos += AWSA_CORE_TLV_INT_INDEX(p_tlv[pos + 3]) + 4;
        }
        return AW_OK;
    }

    case AWSA_CORE_TLV_TYPE_DB_SCALE: {

        if (p_tlv[3] & 0x10000) {
            *p_min = AWSA_CORE_TLV_DB_GAIN_MUTE;
        } else {
            *p_min = (int32_t)p_tlv[2];
        }

        step   = (p_tlv[3] & 0xffff);
        *p_max = (int32_t)p_tlv[2] + step * (range_max - range_min);
        return AW_OK;
    }

    case AWSA_CORE_TLV_TYPE_DB_MINMAX:
    case AWSA_CORE_TLV_TYPE_DB_LINEAR:
        *p_min = (int32_t)p_tlv[2];
        *p_max = (int32_t)p_tlv[3];
        return AW_OK;

    case AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE:
        *p_min = AWSA_CORE_TLV_DB_GAIN_MUTE;
        *p_max = (int32_t)p_tlv[3];
        return AW_OK;

    default:
        break;

    }

    return -AW_EINVAL;
}

/**
 * \brief Convert the given raw volume value to a dB gain
 */
aw_err_t awsa_core_tlv_convert_to_db (const uint32_t *p_tlv,
                                      int32_t         range_min,
                                      int32_t         range_max,
                                      int32_t         volume,
                                      int32_t        *p_db_gain)
{
    switch (p_tlv[0]) {

    case AWSA_CORE_TLV_TYPE_DB_RANGE: {
        uint32_t pos, len;

        len = AWSA_CORE_TLV_INT_INDEX(p_tlv[1]);
        if (len > AWSA_CORE_TLV_RANGE_SIZE_MAX) {
            return -AW_EINVAL;
        }
        pos = 2;
        while (pos + 4 <= len) {
            range_min = (int32_t)p_tlv[pos];
            range_max = (int32_t)p_tlv[pos + 1];
            if (volume >= range_min && volume <= range_max) {
                return awsa_core_tlv_convert_to_db(p_tlv + pos + 2,
                                                   range_min,
                                                   range_max,
                                                   volume,
                                                   p_db_gain);
            }
            pos += AWSA_CORE_TLV_INT_INDEX(p_tlv[pos + 3]) + 4;
        }
        return -AW_EINVAL;
    }

    case AWSA_CORE_TLV_TYPE_DB_SCALE: {
        int32_t min, step, mute;

        min  =  p_tlv[2];
        step = (p_tlv[3]  & 0xffff);
        mute = (p_tlv[3] >> 16) & 1;
        if (mute && volume <= range_min) {
            *p_db_gain = AWSA_CORE_TLV_DB_GAIN_MUTE;
        } else {
            *p_db_gain = (volume - range_min) * step + min;
        }
        return 0;
    }

    case AWSA_CORE_TLV_TYPE_DB_MINMAX:
    case AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE: {
        int32_t min_db, max_db;

        min_db = p_tlv[2];
        max_db = p_tlv[3];
        if (volume <= range_min || range_max <= range_min) {
            if (p_tlv[0] == AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE) {
                *p_db_gain = AWSA_CORE_TLV_DB_GAIN_MUTE;
            } else {
                *p_db_gain = min_db;
            }
        } else if (volume >= range_max) {
            *p_db_gain = max_db;
        } else {
            *p_db_gain = (max_db    - min_db)    *
                         (volume    - range_min) /
                         (range_max - range_min) +
                          min_db;
        }
        return AW_OK;
    }

    case AWSA_CORE_TLV_TYPE_DB_LINEAR: {
        int32_t min_db = p_tlv[2];
        int32_t max_db = p_tlv[3];

        if (volume <= range_min || range_max <= range_min) {
            *p_db_gain = min_db;
        } else if (volume >= range_max) {
            *p_db_gain = max_db;
        } else {
            double val = (double)(volume    - range_min) /
                         (double)(range_max - range_min);
            if (min_db <= AWSA_CORE_TLV_DB_GAIN_MUTE) {
                *p_db_gain = (long)(100.0 * 20.0 * log10(val)) + max_db;
            } else {

                /* FIXME: precalculate and cache these values */
                double lmin = pow(10.0, min_db/2000.0);
                double lmax = pow(10.0, max_db/2000.0);

                val         = (lmax - lmin) * val + lmin;
                *p_db_gain  = (int32_t)(100.0 * 20.0 * log10(val));
            }
        }
        return AW_OK;
    }

    default:
        break;
    }

    return -AW_EINVAL;
}

/**
 * \brief Convert from dB gain to the corresponding raw value
 */
aw_err_t awsa_core_tlv_convert_from_db (const uint32_t  *p_tlv,
                                        int32_t          range_min,
                                        int32_t          range_max,
                                        int32_t          db_gain,
                                        int32_t         *p_volume,
                                        int              dir)
{
    switch (p_tlv[0]) {
    case AWSA_CORE_TLV_TYPE_DB_RANGE: {
        int32_t  db_min, db_max, prev_sub_max;
        uint32_t pos, len;

        len = AWSA_CORE_TLV_INT_INDEX(p_tlv[1]);
        if (len < 6 || len > AWSA_CORE_TLV_RANGE_SIZE_MAX) {
            return -AW_EINVAL;
        }
        pos          = 2;
        prev_sub_max = 0;
        while (pos + 4 <= len) {
            int32_t sub_min, sub_max;

            sub_min = (int32_t)p_tlv[pos];
            sub_max = (int32_t)p_tlv[pos + 1];
            if (range_max < sub_max) {
                sub_max = range_max;
            }
            if (!awsa_core_tlv_db_range_get(p_tlv + pos + 2,
                                            sub_min,
                                            sub_max,
                                            &db_min,
                                            &db_max) &&
                db_gain >= db_min && db_gain <= db_max) {
                return awsa_core_tlv_convert_from_db(p_tlv + pos + 2,
                                                     sub_min,
                                                     sub_max,
                                                     db_gain,
                                                     p_volume,
                                                     dir);
            } else if (db_gain < db_min) {
                *p_volume = dir > 0 || pos == 2 ? sub_min : prev_sub_max;
                return AW_OK;
            }
            prev_sub_max = sub_max;
            if (range_max == sub_max) {
                break;
            }
            pos += AWSA_CORE_TLV_INT_INDEX(p_tlv[pos + 3]) + 4;
        }
        *p_volume = prev_sub_max;
        return AW_OK;
    }

    case AWSA_CORE_TLV_TYPE_DB_SCALE: {
        int32_t min, step, max;

        min  =  p_tlv[2];
        step = (p_tlv[3] & 0xffff);
        max  = min + (int32_t)(step * (range_max - range_min));
        if (db_gain <= min) {
            if (db_gain > AWSA_CORE_TLV_DB_GAIN_MUTE &&
                dir     > 0                          &&
               (p_tlv[3] & 0x10000)) {
                *p_volume = range_min + 1;
            } else {
                *p_volume = range_min;
            }
        } else if (db_gain >= max) {
            *p_volume = range_max;
        } else {
            int32_t v = (db_gain - min) * (range_max - range_min);
            if (dir > 0) {
                v += (max - min) - 1;
            }
            v         = v / (max - min) + range_min;
            *p_volume = v;
        }

        return AW_OK;
    }
    case AWSA_CORE_TLV_TYPE_DB_MINMAX:
    case AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE: {
        int32_t min, max;

        min = p_tlv[2];
        max = p_tlv[3];
        if (db_gain <= min) {
            if (db_gain   > AWSA_CORE_TLV_DB_GAIN_MUTE         &&
                dir       > 0                                  &&
                p_tlv[0] == AWSA_CORE_TLV_TYPE_DB_MINMAX_MUTE) {
                *p_volume = range_min + 1;
            } else {
                *p_volume = range_min;
            }
        } else if (db_gain >= max) {
            *p_volume = range_max;
        } else {
            int32_t v = (db_gain - min) * (range_max - range_min);
            if (dir > 0) {
                v += (max - min) - 1;
            }
            v         = v / (max - min) + range_min;
            *p_volume = v;
        }
        return 0;
    }

    case AWSA_CORE_TLV_TYPE_DB_LINEAR: {
        int32_t min, max;

        min = p_tlv[2];
        max = p_tlv[3];
        if (db_gain <= min) {
            *p_volume = range_min;
        } else if (db_gain >= max) {
            *p_volume = range_max;
        } else {

            /* FIXME: precalculate and cache vmin and vmax */
            double vmin, vmax, v;

            vmin = (min <= AWSA_CORE_TLV_DB_GAIN_MUTE) ? 0.0 :
                    pow(10.0,  (double)min / 2000.0);
            vmax = !max ? 1.0 : pow(10.0,  (double)max / 2000.0);
            v    = pow(10.0, (double)db_gain / 2000.0);
            v    = (v - vmin) * (range_max - range_min) / (vmax - vmin);
            if (dir > 0) {
                v = ceil(v);
            }

            *p_volume = (int32_t)v + range_min;
        }

        return AW_OK;
    }

    default:
        break;
    }

    return -AW_EINVAL;
}

/* end of file */
