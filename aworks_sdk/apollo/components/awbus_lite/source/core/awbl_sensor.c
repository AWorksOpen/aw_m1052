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
 * \brief AWBus 传感器接口实现
 *
 * \internal
 * \par modification history
 * - 1.00 16-08-09  ebi, first implementation
 * \endinternal
 */
#include "apollo.h"
#include "awbus_lite.h"
#include "aw_assert.h"
#include "aw_list.h"
#include "awbl_sensor.h"
#include "aw_sem.h"
#include "aw_errno.h"
#include <string.h>

/*******************************************************************************
  local variables
*******************************************************************************/

/* Sensor 服务链表  */
aw_local awbl_sensor_service_t *__gp_sensor_service_head = NULL;

/*******************************************************************************
  local defines
*******************************************************************************/
 
const static char *sensor_name_string[] = {"voltage",    "current",        "temperature",
                                           "pressure",   "illuminance",    "elevation",
                                           "distance",   "PM-1",           "PM-2.5",
                                           "PM-10",      "humidity",       "proximity",
                                           "orient-rad", "acceleration",   "gyroscope",
                                           "magnetic",   "Rotation Vector" };

const static char *sensor_unit_string[] = {"V",     "A",  "'C",     "Pa",     "lux",
                                           "m",     "m",  "ug/m^3", "ug/m^3", "ug/m^3",
                                           "%rH",   "",   "rad",    "m/s^2",
                                           "rad/s", "Gs", "rad"     };

/**
 * \brief 判断缓存中的数据是否已经被驱动设置，以据此判定该通道是否已经操作完成
 *
 * 两种情况视为已经操作：
 * 1. 数据有效
 * 2. 数据无效，但val值不为 -AW_ENODEV
 */
#define __SENSOR_CHAN_DATA_IS_READ(p_val)                     \
        ((AW_SENSOR_VAL_IS_VALID(*(p_val))) ||                \
         ((!AW_SENSOR_VAL_IS_VALID(*(p_val))) &&              \
         ((p_val)->val != (-AW_ENODEV))))

/*******************************************************************************
  local functions : sensor service manage
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_sensorserv_get);

aw_local aw_err_t __sensor_serv_find_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t  pfn_serv_get = NULL;
    awbl_sensor_service_t *p_serv       = NULL;

    /* 获取 Method  */
    pfn_serv_get = awbl_dev_method_get(p_dev,
                                       AWBL_METHOD_CALL(awbl_sensorserv_get));

    if (pfn_serv_get != NULL) {

        int    i;
        size_t tot_num = 0;

        pfn_serv_get(p_dev, &p_serv); /* 获取 sensor 服务 */

        if ((p_serv                               == NULL) ||
            (p_serv->p_servfuncs                  == NULL) ||
            (p_serv->p_servinfo                   == NULL) ||
            (p_serv->p_servinfo->p_type_info_list == NULL) ||
            (p_serv->p_servinfo->list_size        == 0)    ||
            (p_serv->p_servinfo->total_num        == 0)    ||
            (p_serv->p_servfuncs->pfn_data_get    == NULL)) {

            return AW_OK;
        }

        for (i = 0; i < p_serv->p_servinfo->list_size; i++) {
            tot_num += p_serv->p_servinfo->p_type_info_list[i].num;
        }

        /* 数目不一致，存在问题，不将服务加入系统中  */
        if (tot_num != p_serv->p_servinfo->total_num) {
            return AW_OK;
        }

        /* 所有验证通过，将服务加入系统中  */
        p_serv->p_next           = __gp_sensor_service_head;
        __gp_sensor_service_head = p_serv;
    }

    return AW_OK;
}

/******************************************************************************/
aw_local awbl_sensor_service_t *__sensor_id_to_serv (int id)
{
    awbl_sensor_service_t        *p_serv = __gp_sensor_service_head;
    const awbl_sensor_servinfo_t *p_info;

    int i;

    while (p_serv != NULL) {

        int start_id = p_serv->start_id;
        int end_id   = start_id + p_serv->p_servinfo->total_num - 1;

        /* 确保ID范围  */
        if ((id >= start_id) && (id <= end_id)) {
            return p_serv;
        }

        p_serv = p_serv->p_next;
    }

    return NULL;
}

/******************************************************************************/
aw_local uint32_t __pow10 (int n)
{
    /* 32位数最大表示： 4 294 967 296， 最多返回：10^9 ，即 ：1 000 000 000 */
    aw_local const uint32_t pow10[] = {
        1UL,                  /* 0 */
        10UL,                 /* 1 */
        100UL,                /* 2 */
        1000UL,               /* 3 */
        10000UL,              /* 4 */
        100000UL,             /* 5 */
        1000000UL,            /* 6 */
        10000000UL,           /* 7 */
        100000000UL,          /* 8 */
        1000000000UL,         /* 9 */
    };

    if ((n >= 0) && (n <= 9)) {
        return pow10[n];
    }

    return 0;
}

/* val值有效范围：-2147483648 ~ 2147483647，10位有效数。最大扩大 10 ^ 9*/
/******************************************************************************/
aw_local aw_err_t __sensor_val_unit_convert (aw_sensor_val_t *p_val,
                                             int32_t          to_unit)
{
    if (p_val->val != 0) {                 /* 值为0，单位设置为目标单位即可  */

        uint32_t mul;

        int unit_err = to_unit - p_val->unit;

        if (unit_err < 0) {                                   /* 单位缩小  */

            unit_err = (-1) * unit_err;

            if (unit_err > 9) {                               /* 超出范围   */

                return -AW_ERANGE;

            } else {

                mul = __pow10(unit_err);                     /* 扩大的倍数 */

                /* 只有当  val 值 不超过 max_val 时，才不会超过范围 */
                uint32_t max_val = 2147483647 / mul;

                if ((p_val->val > 0) && (p_val->val > max_val)) {
                    return -AW_ERANGE;
                }

                if ((p_val->val < 0) && (p_val->val < -max_val)) {
                    return -AW_ERANGE;
                }

                p_val->val *= mul;                            /* val值扩大 */
            }

        } else if (unit_err > 0) {                            /* 单位扩大    */

            if (unit_err > 9) {                               /* 超出范围   */
                p_val->val = 0;
            } else {

                mul = __pow10(unit_err);                     /* 缩小的倍数 */

                /* val 值缩小 ，保证四舍五入 */
                p_val->val = (p_val->val + (mul / 2)) / mul;
            }
        }
    }

    p_val->unit = to_unit;

    return AW_OK;
}

/******************************************************************************/
aw_local aw_err_t __sensor_group_data_get (const int       *p_ids,
                                           int              num,
                                           aw_sensor_val_t *p_buf)
{
    awbl_sensor_service_t  *p_serv = NULL;

    int  i       = 0;
    int  ret     = AW_OK;                           /* 本函数返回值           */
    int  ret_cur = AW_OK;                           /* 当前通道读取的返回值   */

    for (i = 0; i < num; i++) {
        p_buf[i].unit = AW_SENSOR_UNIT_INVALID;
        p_buf[i].val  = -AW_ENODEV;                 /* 初始标记通道不存在     */
    }

    i = 0;
    while (i < num) {

        p_serv = __sensor_id_to_serv(p_ids[i]);

        if (p_serv == NULL) {                       /* 该通道无对应传感器设备 */

            ret_cur = -AW_ENODEV;

        } else {

            /*
             * 数据读取时，将当前通道作为通道列表的第一个通道，因为其之前的
             * 所有通道已经读取
             */
            ret_cur = p_serv->p_servfuncs->pfn_data_get(p_serv->p_cookie,
                                                        p_ids + i,
                                                        num   - i,
                                                        p_buf + i);
        }

        /* 第一次遇到返回值不为 AW_OK 时，将该值作为函数的返回值   */
        if ((ret == AW_OK) && (ret_cur != AW_OK)) {
            ret = ret_cur;
        }

        i++;                    /* 当前通道读取完成 */

        /* 判断本次是否顺带读取了其它通道，若存在，则跳过 */
        while (i < num) {
            if (__SENSOR_CHAN_DATA_IS_READ(&p_buf[i])) {
                 i++;
            } else {
                break;
            }
        }
    }

    return ret;
}

/******************************************************************************/
aw_local aw_err_t __sensor_group_cmd (const int          *p_ids,
                                      int                 num,
                                      aw_sensor_val_t    *p_result,
                                      aw_bool_t           is_enable)
{
    aw_err_t             (*pfn_group_cmd) (void *,
                                           const int *,
                                           int ,
                                           aw_sensor_val_t *);

    awbl_sensor_service_t *p_serv = NULL;

    int  i       = 0;
    int  ret     = AW_OK;                          /* 本函数返回值            */
    int  ret_cur = AW_OK;                          /* 当前通道读取的返回值    */
    
    if (p_result != NULL) {
        for (i = 0; i < num; i++) {
            p_result[i].unit = AW_SENSOR_UNIT_INVALID;
            p_result[i].val  = -AW_ENODEV;         /* 初始标记通道不存在      */
        }
    }

    i = 0;
    while (i < num) {

        p_serv = __sensor_id_to_serv(p_ids[i]);

        if (p_serv == NULL) {                      /* 该通道无对应传感器设备  */

            ret_cur = -AW_ENODEV;
            
        } else {

            if (is_enable) {
                pfn_group_cmd = p_serv->p_servfuncs->pfn_enable;
            } else {
                pfn_group_cmd = p_serv->p_servfuncs->pfn_disable;
            }

            if (pfn_group_cmd == NULL) {

                if (is_enable) {              /* 通道无需使能，直接返回 AW_OK */
                    ret_cur = AW_OK;
                } else {                      /* 通道无法禁能，返回不支持     */
                    ret_cur = -AW_ENOTSUP;
                }
                if (p_result != NULL) {
                     AWBL_SENSOR_VAL_SET_RESULT(&p_result[i], ret_cur);
                }
                
            } else {
                ret_cur = pfn_group_cmd(p_serv->p_cookie,
                                        p_ids    + i,
                                        num      - i,
                                        (p_result == NULL) ? NULL : p_result + i);
            }
        }
           
        /* 第一次遇到返回值不为 AW_OK 时，将该值作为函数的返回值   */
        if ((ret == AW_OK) && (ret_cur != AW_OK)) {
            ret = ret_cur;
        }

        i++;                             /* 当前通道操作完成 */

        /* 判断本次是否顺带读取了其它通道，若存在，则跳过 */
        if (p_result != NULL) {
            while (i < num) {
                if (__SENSOR_CHAN_DATA_IS_READ(&p_result[i])) {
                    i++;
                } else {
                    break;
                }
            }
        }
    }
    return ret;
}

/*******************************************************************************
  Public functions : sensor lib manage
*******************************************************************************/
void awbl_sensor_init(void)
{
    __gp_sensor_service_head = NULL;

    /* 遍历所有设备，寻找 传感器服务  */
    awbl_dev_iterate(__sensor_serv_find_helper, NULL, AWBL_ITERATE_INSTANCES);
}

/*******************************************************************************
  Standard interface implement
*******************************************************************************/
aw_err_t aw_sensor_type_get(int id)
{
    awbl_sensor_service_t  *p_serv = NULL;

    int  offset;                           /* 相对于当前类型的偏移            */
    int  cur_end;                          /* 当前类型对应ID的结束边界（不含）*/
    int  i;

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                  /* 该通道无对应传感器设备          */

        return -AW_ENODEV;

    } else {

        /* 0 ~ tot_num - 1 */
        offset = id - p_serv->start_id;

        for (i = 0; i < p_serv->p_servinfo->list_size; i++) {

            cur_end = p_serv->p_servinfo->p_type_info_list[i].num;

            if (offset < cur_end) {
                return p_serv->p_servinfo->p_type_info_list[i].type;
            } else {
                offset -= cur_end;
            }
        }
    }

    return -AW_ENODEV;
}

/******************************************************************************/
const char * aw_sensor_unit_symbol_get (int32_t unit)
{
    /* 单位从小到大的顺序排列 */
    static const char *symbol[] = {
        "y",  "z", "a", "f", "p", "n", "μ", "m",     /* 间隔 3, offset = 0 */
        "c",  "d",                                   /* 间隔 1, offset = 8 */
        "",                                          /* 0, offset = 10 */
        "da", "h",                                   /* 间隔 1, offset = 11 */
        "k",  "M", "G", "T", "P", "E", "Z", "Y",     /* 间隔 3, offset = 13 */
    };

    /* 当前单位有效范围 */
    if ((unit >= AW_SENSOR_UNIT_YOCTO) && (unit <= AW_SENSOR_UNIT_YOTTA)) {

        /* 厘  ~ 百 */
        if ((unit >= AW_SENSOR_UNIT_CENTI) && (unit <= AW_SENSOR_UNIT_HECTO)) {
            return symbol[10 + unit];
        } else if ((unit % 3) == 0) {
            unit = unit / 3;
            if (unit < 0) {
                return symbol[10 + AW_SENSOR_UNIT_CENTI + unit];
            } else {
                return symbol[10 + AW_SENSOR_UNIT_HECTO + unit];
            }
        }
    }
    return NULL;
}

/******************************************************************************/
aw_err_t aw_sensor_val_unit_convert (aw_sensor_val_t *p_buf,
                                     int              num, 
                                     int32_t          to_unit)
{
    int      i;
    aw_err_t ret = AW_OK;
    aw_err_t ret_cur;

    if ((p_buf == NULL) || (num <= 0)) {
        return -AW_EINVAL;
    }

    for (i = 0; i < num; i++) {

        if (AW_SENSOR_VAL_IS_VALID(p_buf[i])) {

            ret_cur = __sensor_val_unit_convert(&p_buf[i], to_unit);

            if ((ret == AW_OK) && (ret_cur != AW_OK)) {
                 ret = ret_cur;
            }
        }
    }

    return ret;
}

/******************************************************************************/
aw_err_t aw_sensor_data_get (int id, aw_sensor_val_t *p_val)
{
    if (p_val == NULL) {
        return -AW_EINVAL;
    }

    return __sensor_group_data_get(&id, 1, p_val);
}

/******************************************************************************/
aw_err_t aw_sensor_group_data_get (const int       *p_ids,
                                   int              num,
                                   aw_sensor_val_t *p_buf)
{
    if ((p_ids == NULL) || (num <= 0) || (p_buf == NULL)) {
        return -AW_EINVAL;
    }

    return __sensor_group_data_get(p_ids, num, p_buf);
}

/******************************************************************************/
aw_err_t aw_sensor_enable (int id)
{
    return __sensor_group_cmd(&id, 1, NULL, AW_TRUE);
}

/******************************************************************************/
aw_err_t aw_sensor_group_enable (const int       *p_ids,
                                 int              num,
                                 aw_sensor_val_t *p_result)
{
    if ((p_ids == NULL) || (num <= 0)) {
        return -AW_EINVAL;
    }

    return __sensor_group_cmd(p_ids, num, p_result, AW_TRUE);
}

/******************************************************************************/
aw_err_t aw_sensor_disable (int id)
{
    return __sensor_group_cmd(&id, 1, NULL, AW_FALSE);
}

/******************************************************************************/
aw_err_t aw_sensor_group_disable (const int       *p_ids,
                                  int              num,
                                  aw_sensor_val_t *p_result)
{
    if ((p_ids == NULL) || (num <= 0)) {
        return -AW_EINVAL;
    }

    return __sensor_group_cmd(p_ids, num, p_result, AW_FALSE);
}

/******************************************************************************/
aw_err_t aw_sensor_attr_set (int id, int attr, const aw_sensor_val_t *p_val)
{
    awbl_sensor_service_t  *p_serv = NULL;

    if (p_val == NULL) {
        return -AW_EINVAL;
    }

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                        /* 该通道无对应传感器设备  */

        return -AW_ENODEV;

    } else {

         if (p_serv->p_servfuncs->pfn_attr_set == NULL) {
             return -AW_ENOTSUP;
         }

         return p_serv->p_servfuncs->pfn_attr_set(p_serv->p_cookie,
                                                  id,
                                                  attr,
                                                  p_val);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sensor_attr_get (int id, int attr, aw_sensor_val_t *p_val)
{
    awbl_sensor_service_t  *p_serv = NULL;

    if (p_val == NULL) {
        return -AW_EINVAL;
    }

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                        /* 该通道无对应传感器设备  */

        return -AW_ENODEV;

    } else {

         if (p_serv->p_servfuncs->pfn_attr_get == NULL) {
             return -AW_ENOTSUP;
         }

         return p_serv->p_servfuncs->pfn_attr_get(p_serv->p_cookie,
                                                  id,
                                                  attr,
                                                  p_val);
    }

    return AW_OK;
}

/******************************************************************************/
aw_err_t  aw_sensor_trigger_cfg (int                       id,
                                 uint32_t                  flags,
                                 aw_sensor_trigger_cb_t    pfn_cb,
                                 void                     *p_arg)
{
    awbl_sensor_service_t  *p_serv = NULL;

    if (pfn_cb == NULL) {
        return -AW_EINVAL;
    }

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                        /* 该通道无对应传感器设备  */

        return -AW_ENODEV;

    } else {

         if (p_serv->p_servfuncs->pfn_trigger_cfg == NULL) {
             return -AW_ENOTSUP;
         }

         return p_serv->p_servfuncs->pfn_trigger_cfg(p_serv->p_cookie,
                                                     id,
                                                     flags,
                                                     pfn_cb,
                                                     p_arg);
    }
    return AW_OK;
}


/******************************************************************************/
aw_err_t aw_sensor_trigger_on (int id)
{
    awbl_sensor_service_t  *p_serv = NULL;

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                        /* 该通道无对应传感器设备  */

        return -AW_ENODEV;

    } else {

         if (p_serv->p_servfuncs->pfn_trigger_on == NULL) {
             return -AW_ENOTSUP;
         }

         return p_serv->p_servfuncs->pfn_trigger_on(p_serv->p_cookie, id);
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sensor_trigger_off (int id)
{
    awbl_sensor_service_t  *p_serv = NULL;

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                        /* 该通道无对应传感器设备  */

        return -AW_ENODEV;

    } else {

         if (p_serv->p_servfuncs->pfn_trigger_off == NULL) {
             return -AW_ENOTSUP;
         }

         return p_serv->p_servfuncs->pfn_trigger_off(p_serv->p_cookie, id);
    }
    return AW_OK;
}

/******************************************************************************/
aw_err_t aw_sensor_type_name_get (int id,  const char **name,  const char **unit)
{
    aw_err_t    ret = 0;
    uint8_t     typeid = 0;
    ret = aw_sensor_type_get(id);
    if (ret < 0) {
        return ret;
    }

    if ((ret >= 1) && (ret <= 7)) {
        typeid = ret - 1;
    } else if ((ret >= 100) && (ret <= 105)) {
        typeid = 7 + (ret - 100);
    } else if ((ret >= 200) && (ret <= 203)) {
        typeid = 13 + (ret - 200);
    } else {
        return AW_ERROR;
    }

    *name = sensor_name_string[typeid];

    *unit = sensor_unit_string[typeid];

    return AW_OK;
}



/* end of file */
