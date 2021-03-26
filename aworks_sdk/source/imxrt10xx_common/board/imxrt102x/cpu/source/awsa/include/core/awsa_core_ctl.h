/*******************************************************************************
*                                 Apollo
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
 * \brief
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

#ifndef __AWSA_CORE_CTL_H
#define __AWSA_CORE_CTL_H

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus  */

#include "core/awsa_core_internal.h"

/**
 * \brief ACPI Power Management State
 * \note for further details see the ACPI and PCI power management
 *       specification
 */
typedef enum awsa_core_ctl_power {

    /** \brief full On */
    AWSA_CORE_CTL_POWER_D0     = 0x0000,

    /** \brief partial On */
    AWSA_CORE_CTL_POWER_D1     = 0x0100,

    /** \brief partial On */
    AWSA_CORE_CTL_POWER_D2     = 0x0200,

    /** \brief Off */
    AWSA_CORE_CTL_POWER_D3     = 0x0300,

    /** \brief Off, with power */
    AWSA_CORE_CTL_POWER_D3hot  = (AWSA_CORE_CTL_POWER_D3 | 0x0000),

    /** \brief Off, without power */
    AWSA_CORE_CTL_POWER_D3cold = (AWSA_CORE_CTL_POWER_D3 | 0x0001),
} awsa_core_ctl_power_t;

/**
 * \brief ctl interface
 */
typedef enum {

    /** \brief soundcard device ctl */
    AWSA_CORE_CTL_ELEM_IF_CARD       = 0,

    /** \brief hardware dependent device ctl */
    AWSA_CORE_CTL_ELEM_IF_HWDEP,

    /** \brief mixer device ctl */
    AWSA_CORE_CTL_ELEM_IF_MIXER,

    /** \brief pcm device ctl */
    AWSA_CORE_CTL_ELEM_IF_PCM,

    /** \brief rawmidi device ctl */
    AWSA_CORE_CTL_ELEM_IF_RAWMIDI,

    /** \brief timer device ctl */
    AWSA_CORE_CTL_ELEM_IF_TIMER,

    /** \brief sequencer device ctl */
    AWSA_CORE_CTL_ELEM_IF_SEQUENCER,

    /** \brief the last */
    AWSA_CORE_CTL_ELEM_IF_LAST       = AWSA_CORE_CTL_ELEM_IF_SEQUENCER
} awsa_core_ctl_elem_if_t;

/**
 * \brief ctl device value type
 */
typedef enum {

    /** \brief invalid */
    AWSA_CORE_CTL_ELEM_TYPE_NONE        = 0,

    /** \brief boolean type */
    AWSA_CORE_CTL_ELEM_TYPE_BOOLEAN,

    /** \brief integer type */
    AWSA_CORE_CTL_ELEM_TYPE_INTEGER,

    /** \brief enumerated type */
    AWSA_CORE_CTL_ELEM_TYPE_ENUMERATED,

    /** \brief byte array */
    AWSA_CORE_CTL_ELEM_TYPE_BYTES,

    /** \brief IEC958 (S/PDIF) setup */
    AWSA_CORE_CTL_ELEM_TYPE_IEC958,

    /** \brief 64-bit integer type */
    AWSA_CORE_CTL_ELEM_TYPE_INTEGER64,

    /** \brief the last */
    AWSA_CORE_CTL_ELEM_TYPE_LAST        = AWSA_CORE_CTL_ELEM_TYPE_INTEGER64,
} awsa_core_ctl_elem_type_t;

/** \brief declare ctl device */
struct awsa_core_ctl;

/** \brief declare ctl element */
struct awsa_core_ctl_elem;

/** \brief declare ctl information */
struct awsa_core_ctl_elem_info;

/** \brief declare ctl value */
struct awsa_core_ctl_elem_value;

/**
 * \brief get ctl device information
 *
 * \param[in] p_ctl   : ctl device
 * \param[in] p_info  : ctl device information
 *
 * \retval AW_OK : success
 * \retval < 0   : fail
 */
typedef aw_err_t (*pfn_core_ctl_elem_info_t) (
    struct awsa_core_ctl_elem      *p_elem,
    struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief get ctl device element value
 *
 * \param[in]  p_ctl   : ctl device
 * \param[out] p_value : element value
 *
 * \retval AW_OK : success
 */
typedef aw_err_t (*pfn_core_ctl_elem_get_t) (
    struct awsa_core_ctl_elem       *p_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief put ctl device element value
 *
 * \param[in]  p_ctl   : ctl device
 * \param[out] p_value : element value
 *
 * \retval AW_OK : success
 */
typedef aw_err_t (*pfn_core_ctl_elem_set_t) (
    struct awsa_core_ctl_elem       *p_ctl,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief ctl device information
 */
struct awsa_core_ctl_elem_info {

    /** \brief elem type */
    awsa_core_ctl_elem_type_t type;

    /** \brief count of value */
    uint32_t                  count;

    union __core_ctl_elem_info {
        struct __core_ctl_elem_info_integer {
            int32_t min;           /**< \brief minimum value */
            int32_t max;           /**< \brief maximum value */
            int32_t step;          /**< \brief step (0 variable) */
        } integer;

        struct __core_ctl_elem_info_integer64 {
            int64_t min;           /**< \brief minimum value */
            int64_t max;           /**< \brief maximum value */
            int64_t step;          /**< \brief step (0 variable) */
        } integer64;

        struct __core_ctl_elem_info_enumerated {
            uint32_t items;        /**< \brief number of items */
            uint32_t item;         /**< \brief item */
        } enumerated;
    } info;
};

/**
 * \brief ctl device value
 */
struct awsa_core_ctl_elem_value {

    union __core_ctl_elem_value {

        struct __ctl_value_interger {
            int32_t *p_value;    /**< \brief 32bit integer array */
        } integer;

        struct __ctl_value_integer64 {
            int64_t *p_value;    /**< \brief 64bit integer array */
        } integer64;

        struct __ctl_value_enumerated {
            uint32_t *p_item;    /**< \brief 32bit unisgned integer array */
        } enumerated;

        struct __bytes {
            uint8_t  *p_data;    /**< \brief bytes array */
        } bytes;

    } value;
};

/**
 * \brief declare ctl element
 */
#define AWSA_CORE_CTL_ELEM_DECL(                        \
    name, iface, pfn_info, pfn_get, pfn_set, p_tlv, p)  \
{                                                       \
    name, iface, pfn_info, pfn_get, pfn_set, p_tlv, p,  \
}

/**
 * \brief ctl element driver
 */
struct awsa_core_ctl_elem_driver {

    /** \brief name of elem */
    const char                   *p_name;

    /** \brief interface */
    awsa_core_ctl_elem_if_t       iface;

    /** \brief get information */
    pfn_core_ctl_elem_info_t      pfn_info_get;

    /** \brief get value */
    pfn_core_ctl_elem_get_t       pfn_value_get;

    /** \brief set value */
    pfn_core_ctl_elem_set_t       pfn_value_set;

    /** \brief tlv array */
    const uint32_t               *p_tlv;

    /** \brief clt private value */
    uint32_t                      private_value;
};

/**
 * \brief ctl device identifier
 */
struct awsa_core_ctl_elem {

    /** \brief ctl element driver */
    const struct awsa_core_ctl_elem_driver *p_driver;

    /** \brief elem type */
    awsa_core_ctl_elem_type_t               type;

    /** \brief index, zero = invalid */
    //uint32_t                                index;

    /** \brief count of value */
    //uint32_t                                count;

    /** \brief list of elems */
    struct aw_list_head                     list;

    /** \brief belong to which ctl  */
    struct awsa_core_ctl                   *p_ctl;

    /** \brief soundcard set */
    uint32_t                                num_id;

    /** \brief private data */
    void                                   *p_private_data;
};

/**
 * \brief ctl structure
 */
struct awsa_core_ctl {

    /** \brief ctl device */
    struct awsa_core_dev  dev;

    /** \brief list of controls */
    struct aw_list_head   list;

    /** \brief count of elements */
    uint32_t              elem_count;

    /** \brief last num_id */
    uint32_t              last_num_id;

    /** \brief lock */
    AW_MUTEX_DECL(mutex);
};

/**
 * \brief ctl device create
 * \param[in] p_card  : the sound card pointer
 * \return AW_OK if successful, or a negative error code
 */
aw_err_t awsa_core_ctl_init (struct awsa_core_ctl  *p_ctl,
                             struct awsa_core_card *p_card);

/**
 * \brief find ctl device
 * \param[in] p_card  : the sound card pointer
 * \return ctl device
 */
struct awsa_core_ctl *awsa_core_ctl_get (struct awsa_core_card *p_card);

/**
 * \brief add the ctl instance to the card
 *
 * \param[in] p_card    : the card instance
 * \param[in] p_control : the ctl instance to add
 *
 * \retval AW_OK      : success
 *         -AW_EINVAL : the parameter is invalid
 *         -AW_EBUSY  : device is busy
 *         -AW_ENOMEM : no enough ram
 *
 * \note Adds the ctl instance created via awsa_core_ctl_new() or
 *       awsa_core_ctl_new() to the given card. Assigns also an unique
 *       num_id used for fast search.
 */
aw_err_t awsa_core_ctl_elem_add (
    struct awsa_core_ctl_elem              *p_elem,
    const struct awsa_core_ctl_elem_driver *p_driver,
    void                                   *p_private_data,
    struct awsa_core_ctl                   *p_ctl);

/**
 * \brief find ctl element
 */
struct awsa_core_ctl_elem *awsa_core_ctl_elem_find (
    struct awsa_core_ctl    *p_ctl,
    const char              *p_name,
    awsa_core_ctl_elem_if_t  iface,
    uint32_t                 num_id);

/**
 * \brief read control device value
 */
aw_err_t awsa_core_ctl_elem_value_read (
    struct awsa_core_ctl_elem       *p_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief write control device value
 */
aw_err_t awsa_core_ctl_elem_value_write (
    struct awsa_core_ctl_elem       *p_elem,
    struct awsa_core_ctl_elem_value *p_value);

/**
 * \brief get ctl device information
 *
 * \param[in] p_card : the sound card pointer
 * \param[in] p_info : the ctl information
 *
 * \return AW_OK if successful, or a negative error code
 */
aw_err_t awsa_core_ctl_elem_info_get (struct awsa_core_ctl_elem      *p_elem,
                                      struct awsa_core_ctl_elem_info *p_info);

/**
 * \brief tlv read
 */
static inline
const uint32_t *awsa_core_ctl_elem_tlv_get (struct awsa_core_ctl_elem *p_elem)
{
    return p_elem->p_driver->p_tlv;
}

#ifdef __cplusplus
}
#endif  /* __cplusplus  */

#endif /* __AWSA_CORE_CTL_H */

/* end of file */
