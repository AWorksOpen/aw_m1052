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
 * \brief AWBus �������ӿ�ʵ��
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

/* Sensor ��������  */
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
 * \brief �жϻ����е������Ƿ��Ѿ����������ã��Ծݴ��ж���ͨ���Ƿ��Ѿ��������
 *
 * ���������Ϊ�Ѿ�������
 * 1. ������Ч
 * 2. ������Ч����valֵ��Ϊ -AW_ENODEV
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

    /* ��ȡ Method  */
    pfn_serv_get = awbl_dev_method_get(p_dev,
                                       AWBL_METHOD_CALL(awbl_sensorserv_get));

    if (pfn_serv_get != NULL) {

        int    i;
        size_t tot_num = 0;

        pfn_serv_get(p_dev, &p_serv); /* ��ȡ sensor ���� */

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

        /* ��Ŀ��һ�£��������⣬�����������ϵͳ��  */
        if (tot_num != p_serv->p_servinfo->total_num) {
            return AW_OK;
        }

        /* ������֤ͨ�������������ϵͳ��  */
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

        /* ȷ��ID��Χ  */
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
    /* 32λ������ʾ�� 4 294 967 296�� ��෵�أ�10^9 ���� ��1 000 000 000 */
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

/* valֵ��Ч��Χ��-2147483648 ~ 2147483647��10λ��Ч����������� 10 ^ 9*/
/******************************************************************************/
aw_local aw_err_t __sensor_val_unit_convert (aw_sensor_val_t *p_val,
                                             int32_t          to_unit)
{
    if (p_val->val != 0) {                 /* ֵΪ0����λ����ΪĿ�굥λ����  */

        uint32_t mul;

        int unit_err = to_unit - p_val->unit;

        if (unit_err < 0) {                                   /* ��λ��С  */

            unit_err = (-1) * unit_err;

            if (unit_err > 9) {                               /* ������Χ   */

                return -AW_ERANGE;

            } else {

                mul = __pow10(unit_err);                     /* ����ı��� */

                /* ֻ�е�  val ֵ ������ max_val ʱ���Ų��ᳬ����Χ */
                uint32_t max_val = 2147483647 / mul;

                if ((p_val->val > 0) && (p_val->val > max_val)) {
                    return -AW_ERANGE;
                }

                if ((p_val->val < 0) && (p_val->val < -max_val)) {
                    return -AW_ERANGE;
                }

                p_val->val *= mul;                            /* valֵ���� */
            }

        } else if (unit_err > 0) {                            /* ��λ����    */

            if (unit_err > 9) {                               /* ������Χ   */
                p_val->val = 0;
            } else {

                mul = __pow10(unit_err);                     /* ��С�ı��� */

                /* val ֵ��С ����֤�������� */
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
    int  ret     = AW_OK;                           /* ����������ֵ           */
    int  ret_cur = AW_OK;                           /* ��ǰͨ����ȡ�ķ���ֵ   */

    for (i = 0; i < num; i++) {
        p_buf[i].unit = AW_SENSOR_UNIT_INVALID;
        p_buf[i].val  = -AW_ENODEV;                 /* ��ʼ���ͨ��������     */
    }

    i = 0;
    while (i < num) {

        p_serv = __sensor_id_to_serv(p_ids[i]);

        if (p_serv == NULL) {                       /* ��ͨ���޶�Ӧ�������豸 */

            ret_cur = -AW_ENODEV;

        } else {

            /*
             * ���ݶ�ȡʱ������ǰͨ����Ϊͨ���б�ĵ�һ��ͨ������Ϊ��֮ǰ��
             * ����ͨ���Ѿ���ȡ
             */
            ret_cur = p_serv->p_servfuncs->pfn_data_get(p_serv->p_cookie,
                                                        p_ids + i,
                                                        num   - i,
                                                        p_buf + i);
        }

        /* ��һ����������ֵ��Ϊ AW_OK ʱ������ֵ��Ϊ�����ķ���ֵ   */
        if ((ret == AW_OK) && (ret_cur != AW_OK)) {
            ret = ret_cur;
        }

        i++;                    /* ��ǰͨ����ȡ��� */

        /* �жϱ����Ƿ�˳����ȡ������ͨ���������ڣ������� */
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
    int  ret     = AW_OK;                          /* ����������ֵ            */
    int  ret_cur = AW_OK;                          /* ��ǰͨ����ȡ�ķ���ֵ    */
    
    if (p_result != NULL) {
        for (i = 0; i < num; i++) {
            p_result[i].unit = AW_SENSOR_UNIT_INVALID;
            p_result[i].val  = -AW_ENODEV;         /* ��ʼ���ͨ��������      */
        }
    }

    i = 0;
    while (i < num) {

        p_serv = __sensor_id_to_serv(p_ids[i]);

        if (p_serv == NULL) {                      /* ��ͨ���޶�Ӧ�������豸  */

            ret_cur = -AW_ENODEV;
            
        } else {

            if (is_enable) {
                pfn_group_cmd = p_serv->p_servfuncs->pfn_enable;
            } else {
                pfn_group_cmd = p_serv->p_servfuncs->pfn_disable;
            }

            if (pfn_group_cmd == NULL) {

                if (is_enable) {              /* ͨ������ʹ�ܣ�ֱ�ӷ��� AW_OK */
                    ret_cur = AW_OK;
                } else {                      /* ͨ���޷����ܣ����ز�֧��     */
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
           
        /* ��һ����������ֵ��Ϊ AW_OK ʱ������ֵ��Ϊ�����ķ���ֵ   */
        if ((ret == AW_OK) && (ret_cur != AW_OK)) {
            ret = ret_cur;
        }

        i++;                             /* ��ǰͨ��������� */

        /* �жϱ����Ƿ�˳����ȡ������ͨ���������ڣ������� */
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

    /* ���������豸��Ѱ�� ����������  */
    awbl_dev_iterate(__sensor_serv_find_helper, NULL, AWBL_ITERATE_INSTANCES);
}

/*******************************************************************************
  Standard interface implement
*******************************************************************************/
aw_err_t aw_sensor_type_get(int id)
{
    awbl_sensor_service_t  *p_serv = NULL;

    int  offset;                           /* ����ڵ�ǰ���͵�ƫ��            */
    int  cur_end;                          /* ��ǰ���Ͷ�ӦID�Ľ����߽磨������*/
    int  i;

    p_serv = __sensor_id_to_serv(id);

    if (p_serv == NULL) {                  /* ��ͨ���޶�Ӧ�������豸          */

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
    /* ��λ��С�����˳������ */
    static const char *symbol[] = {
        "y",  "z", "a", "f", "p", "n", "��", "m",     /* ��� 3, offset = 0 */
        "c",  "d",                                   /* ��� 1, offset = 8 */
        "",                                          /* 0, offset = 10 */
        "da", "h",                                   /* ��� 1, offset = 11 */
        "k",  "M", "G", "T", "P", "E", "Z", "Y",     /* ��� 3, offset = 13 */
    };

    /* ��ǰ��λ��Ч��Χ */
    if ((unit >= AW_SENSOR_UNIT_YOCTO) && (unit <= AW_SENSOR_UNIT_YOTTA)) {

        /* ��  ~ �� */
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

    if (p_serv == NULL) {                        /* ��ͨ���޶�Ӧ�������豸  */

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

    if (p_serv == NULL) {                        /* ��ͨ���޶�Ӧ�������豸  */

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

    if (p_serv == NULL) {                        /* ��ͨ���޶�Ӧ�������豸  */

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

    if (p_serv == NULL) {                        /* ��ͨ���޶�Ӧ�������豸  */

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

    if (p_serv == NULL) {                        /* ��ͨ���޶�Ӧ�������豸  */

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
