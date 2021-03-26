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
 * \brief AWBus non-volatile RAM to non-volatile memory routine mapping
 *
 * This library provides the external API to handle non-volatile
 * RAM manipulation in a AWBus environment.
 *
 * \internal
 * \par modification history:
 * - 1.00 12-10-30  zen, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/

#include "apollo.h"
#include "awbus_lite.h"

#include "awbl_nvram.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_nvram_get);
AWBL_METHOD_IMPORT(awbl_nvram_set);

/*******************************************************************************
  type defines
*******************************************************************************/

struct __nvram_info {
    char         *p_name;    /* nvram segment's name */
    int           unit;      /* nvram segment's unit number */
    char         *p_buf;     /* where to copy non-volatile RAM */
    int           offset;    /* offset from start of allocation unit */
    int           len;       /* maximum number of bytes to copy */
    aw_bool_t     done;      /* read/write done ? */
};

/*******************************************************************************
  implementation
*******************************************************************************/

/**
 * \brief Non Volatile RAM library initialization
 */

void awbl_nvram_init()
{
    return;
}

/**
 * \brief Helper function to read from NVRam
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __nvram_get_helper (struct awbl_dev *p_dev, void *p_arg)
{
    struct __nvram_info   *p_info = (struct __nvram_info *)p_arg;
    awbl_method_handler_t  pfunc_nvram_get;
    aw_err_t               ret;

    pfunc_nvram_get = awbl_dev_method_get(p_dev, 
                                          AWBL_METHOD_CALL(awbl_nvram_get));
    if (pfunc_nvram_get == NULL) {
        return AW_OK;       /* iterating continue */
    }

    ret = ((pfunc_awbl_nvram_get_t)pfunc_nvram_get)(p_dev,
                                                    p_info->p_name,
                                                    p_info->unit,
                                                    p_info->p_buf,
                                                    p_info->offset,
                                                    p_info->len);

    if (ret == AW_OK) {
        p_info->done = AW_TRUE;
        return -AW_EINTR;      /* iterating stop */
    }

    return AW_OK;           /* iterating continue */
}


/**
 * \brief Helper function to write to NVRam
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __nvram_set_helper (struct awbl_dev *p_dev, void *p_arg)
{
    struct __nvram_info   *p_info = (struct __nvram_info *)p_arg;
    awbl_method_handler_t  pfunc_nvram_set;
    aw_err_t               ret;

    pfunc_nvram_set = awbl_dev_method_get(p_dev, 
                                          AWBL_METHOD_CALL(awbl_nvram_set));
    if (pfunc_nvram_set == NULL) {
        return AW_OK;       /* iterating continue */
    }

    ret = ((pfunc_awbl_nvram_get_t)pfunc_nvram_set)(p_dev,
                                                    p_info->p_name,
                                                    p_info->unit,
                                                    p_info->p_buf,
                                                    p_info->offset,
                                                    p_info->len);

    if (ret == AW_OK) {
        p_info->done = AW_TRUE;
        return -AW_EINTR;      /* iterating stop */
    }

    return AW_OK;           /* iterating continue */
}

/******************************************************************************/
aw_err_t aw_nvram_get (char *p_name, int unit, char *p_buf, int offset, int len)
{
    struct __nvram_info info;

    if ((p_name == NULL) ||
        (p_buf  == NULL)) {

        return -AW_EFAULT;
    }

    info.p_name = p_name;
    info.unit   = unit;
    info.p_buf  = p_buf;
    info.offset = offset;
    info.len    = len;
    info.done   = AW_FALSE;

    awbl_dev_iterate(__nvram_get_helper,
                     (void *)&info,
                     AWBL_ITERATE_INSTANCES | AWBL_ITERATE_INTRABLE);

    if (info.done ) {
        return AW_OK;
    }

    return -AW_ENXIO;
}

/******************************************************************************/
aw_err_t aw_nvram_set (char *p_name, int unit, char *p_buf, int offset, int len)
{
    struct __nvram_info info;

    if ((p_name == NULL) ||
        (p_buf  == NULL)) {

        return -AW_EFAULT;
    }

    info.p_name = p_name;
    info.unit   = unit;
    info.p_buf  = p_buf;
    info.offset = offset;
    info.len    = len;
    info.done   = AW_FALSE;

    awbl_dev_iterate(__nvram_set_helper,
                     (void *)&info,
                     AWBL_ITERATE_INSTANCES | AWBL_ITERATE_INTRABLE);

    if (info.done ) {
        return AW_OK;
    }

    return -AW_ENXIO;
}

/* end of file */
