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
 * \brief AWBus GNSS interface implementation (lite)
 *
 * \internal
 * \par modification history:
 * - 1.00 16-12-28  vih, first implementation
 * \endinternal
 */

/*******************************************************************************
  includes
*******************************************************************************/
#include "apollo.h"                     /* 此头文件必须被首先包含 */
#include "awbl_gnss.h"
#include "aw_assert.h"
#include "aw_sem.h"
#include "aw_vdebug.h"
#include "aw_serial.h"

/******************************************************************************/
#if 0
#define __DEBUG_INFO_PRINTF
#endif

#ifdef  __DEBUG_INFO_PRINTF
#define __kprintf   aw_kprintf

#else
#define __kprintf(fmt, ...)
#endif



/******************************************************************************/
AWBL_METHOD_DEF(awbl_gnss_serv_get, "awbl_gnss_serv_get_name");
/*******************************************************************************
  locals
*******************************************************************************/

/** \brief pointer to first gnss service */
aw_local struct awbl_gnss_service *__gp_gnss_serv_head = NULL;

/*******************************************************************************
  implementation
*******************************************************************************/

aw_local uint32_t __cur_ms_time_get (void)
{
    return aw_ticks_to_ms(aw_sys_tick_get());
}


aw_local aw_err_t __gnss_dat_get (struct awbl_gnss_service   *p_this,
                                  aw_gnss_data_type_t         type,
                                  void                       *ptr,
                                  uint8_t                     len)
{
    if ((NULL == p_this) || (NULL == ptr)) {
        return -AW_EINVAL;
    }

    uint8_t                   i = 0;
    int                       ret = AW_ERROR;
    int                       cnt = 0;
    uint32_t                  use_tim = 0;
    uint32_t                  flag_sta = 0;
    aw_bool_t                    reflesh_req = 0;
    aw_bool_t                    is_timeover = AW_TRUE;

    if (!p_this->dev_is_enable) {
        return  -AW_EBUSY;
    }

    if ((p_this->func_flag & (type >> 16)) == 0) {
        return -AW_ENOTSUP;
    }

    /* 结束计时，并判断是否超过有效时间，是则清空data */
    use_tim = (__cur_ms_time_get() - p_this->begin_tim);

    is_timeover = (uint32_t)use_tim >= p_this->valid_tim ?
                   AW_TRUE : AW_FALSE;

    if (reflesh_req || is_timeover) {

        memset(&p_this->data, 0, sizeof(p_this->data));

        memset(p_this->p_com->buf, 0, p_this->p_com->size);
        for (i = 0; i < 3; i++) {
            cnt = cnt + aw_serial_read(p_this->p_com->id,
                                       p_this->p_com->buf + cnt,
                                       p_this->p_com->size - cnt);
            if (cnt >=  p_this->p_com->size - 1) {
                break;
            }
        }
    }

    /* flag的都decode一次 */
    if (cnt > 0) {
        __kprintf("%s\n", p_this->p_com->buf);

        ret = awbl_nmea_0183_dat_decode(&p_this->data,
                                         p_this->p_com->buf,
                                         p_this->sys_freq,
                                         p_this->func_flag,
                                         &flag_sta);
    }

    if (ret == AW_OK) {

        /* 获取到数据则重新计时 */
        p_this->begin_tim = __cur_ms_time_get();

    } else if (is_timeover) {

        return -AW_ETIME;
    }

    ret = awbl_nmea_0183_get_dat(&p_this->data, type, ptr, len);

    return ret;
}

void __com_rec_timout (void *p_arg)
{
    struct awbl_gnss_service *p_this = p_arg;

    AW_SEMB_GIVE(p_this->timout_sem);
}

/******************************************************************************/
/**
 * \brief allocate gnss service from instance (helper)
 *
 * \param[in]       p_dev   the current processing  instance
 * \param[in,out]   p_arg   argument
 *
 * \retval          AW_OK
 *
 * \see awbl_alldevs_method_run()
 */
aw_local aw_err_t __gnss_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_gnss_serv  = NULL;
    struct awbl_gnss_service  *p_gnss_serv      = NULL;

    /* find handler */

    pfunc_gnss_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_gnss_serv_get));

    /* call method handler to allocate GNSS service */
    if (pfunc_gnss_serv != NULL) {

        pfunc_gnss_serv(p_dev, &p_gnss_serv);

        /* found a gnss service, insert it to the service list */

        if (p_gnss_serv != NULL) {
            struct awbl_gnss_service **pp_serv_cur = &__gp_gnss_serv_head;

            while (*pp_serv_cur != NULL) {
                pp_serv_cur = &(*pp_serv_cur)->p_next;
            }

            *pp_serv_cur = p_gnss_serv;
            p_gnss_serv->p_next = NULL;

            // 初始化互斥锁
            AW_MUTEX_INIT(p_gnss_serv->op_mutex, AW_SEM_Q_FIFO);

            // 初始化信号量
            AW_SEMB_INIT(p_gnss_serv->timout_sem,
                         AW_SEM_EMPTY,
                         AW_SEM_Q_PRIORITY);

            aw_delayed_work_init(&p_gnss_serv->timout_work,
                                 __com_rec_timout,
                                 p_gnss_serv); // 初始化work
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate gnss services from instance tree
 */
aw_local aw_err_t __gnss_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every gnss device will be find */
    __gp_gnss_serv_head = NULL;

    awbl_dev_iterate(__gnss_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/**
 * \brief find out a service who accept the id
 */
aw_local struct awbl_gnss_service * __gnss_id_to_serv (int id)
{
    struct awbl_gnss_service *p_serv_cur = __gp_gnss_serv_head;

    while ((p_serv_cur != NULL)) {

        if (id == p_serv_cur->p_servinfo->gnss_id) {

            return p_serv_cur;
        }

        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}


/******************************************************************************/
void awbl_gnss_init (void)
{
    /* allocate gnss services from instance tree */

    __gnss_serv_alloc();
}


aw_local aw_inline aw_bool_t __is_rec_data (uint8_t *p_buf, uint8_t *p_rec)
{
    char *p_addr = 0;

    if ((p_buf != NULL) && (p_rec != NULL)) {
        p_addr = strstr(p_buf, p_rec);
        if (p_addr != NULL) {
            if (strstr(p_addr, "\r") != NULL) {
                return  AW_TRUE;
            }
        }
    }

    return  AW_FALSE;

}

#ifdef __DEBUG_INFO_PRINTF
aw_local void __get_run_time (char opt, uint32_t *p_last_ms)
{
    aw_timespec_t timespec;      // 日历时间
    uint32_t      ms = 0;

    aw_timespec_get(&timespec);
    ms = timespec.tv_nsec / 1000000 + timespec.tv_sec * 1000;

    if (opt == 0) {
        __kprintf("\n[USE TIME] %d mS \n", ms - (*p_last_ms));
        __kprintf("--------------------------------------------------\n");
    }
    *p_last_ms = ms;    /* 保存毫秒 */
}
#endif

/* 发送数据并等待接收返回数据，超时单位mS  */
aw_err_t awbl_gnss_send_wait_res (
    struct awbl_gnss_service         *p_this,
    uint8_t                          *p_send,
    uint8_t                          *p_res,
    uint32_t                          timout)
{
    int      ret;
    int      cnt = 0;
    uint32_t read_offset = 0;

    (void)ret;
    if (p_this == 0 || p_this->p_com->id < 0) {
        return -AW_EINVAL;
    }

    /* 发送字符串 */
    if (p_send) {
        aw_serial_write(p_this->p_com->id, p_send, strlen((char *)p_send));
        __kprintf("[COM%d send %d]%s \n", p_this->p_com->id,
                                          strlen(p_send),
                                          p_send);
    }

    /* 接收字符串，进入接收并判断接收到的数据是否是制定的字符串 */
    if (p_res) {

        memset(p_this->p_com->buf, 0, p_this->p_com->size);

        /* timout毫秒后启动delayed_work */
        aw_delayed_work_start(&p_this->timout_work, timout);

#ifdef __DEBUG_INFO_PRINTF
        uint32_t run_time = 0;
        __get_run_time(1, &run_time);
#endif

        while (1) {
            cnt = aw_serial_read(p_this->p_com->id,
                                 p_this->p_com->buf  + read_offset,
                                 p_this->p_com->size - read_offset - 1);

            if (cnt > 0) {
                read_offset += cnt;

                __kprintf("--------------------------------------------------\n");
                __kprintf("[rec %d]%s(len %d) \n",
                           cnt,
                           p_this->p_com->buf,
                           strlen(p_this->p_com->buf));

                if (__is_rec_data(p_this->p_com->buf, p_res)) {

                    aw_delayed_work_stop(&p_this->timout_work);

                    #ifdef __DEBUG_INFO_PRINTF
                    __get_run_time(0, &run_time);
                    #endif
                    return AW_OK;
                }

                if (read_offset >= p_this->p_com->size - 1) {
                    read_offset = 0;
                    memset(p_this->p_com->buf, 0, p_this->p_com->size);
                }
            }

            if (AW_SEMB_TAKE(p_this->timout_sem, AW_SEM_NO_WAIT) == AW_OK) {
                break;
            }
        }

#ifdef __DEBUG_INFO_PRINTF
        __get_run_time(0, &run_time);
#endif
        return -AW_ETIME;
    }

    return AW_OK;
}
/******************************************************************************/
aw_err_t aw_gnss_enable (int id)
{
    aw_err_t                  ret;
    struct awbl_gnss_service *p_serv = __gnss_id_to_serv(id);

    if (p_serv == NULL || p_serv->init_stat == AWBL_GNSS_INIT_FAILED) {
        return -AW_ENODEV;  /* [id]/dev is invalid */
    }

    if (p_serv->dev_is_enable) {
        return AW_OK;
    }

    if (p_serv->p_servfunc->pfn_start != NULL) {

        AW_MUTEX_LOCK(p_serv->op_mutex, AW_SEM_WAIT_FOREVER);
        ret = p_serv->p_servfunc->pfn_start(p_serv->p_cookie);

        if (ret == AW_OK) {
            p_serv->dev_is_enable = AW_TRUE;
        }
        AW_MUTEX_UNLOCK(p_serv->op_mutex);

        return ret;
    }

    return -AW_ENOTSUP;
}

aw_err_t aw_gnss_disable (int id)
{
    aw_err_t                  ret = AW_ERROR;
    struct awbl_gnss_service *p_serv = __gnss_id_to_serv(id);

    if (p_serv == NULL || p_serv->init_stat == AWBL_GNSS_INIT_FAILED) {
        return -AW_EINVAL;  /* [id] is invalid */
    }

    if (!p_serv->dev_is_enable) {
        return AW_OK;
    }

    if (p_serv->p_servfunc->pfn_end != NULL) {

        AW_MUTEX_LOCK(p_serv->op_mutex, AW_SEM_WAIT_FOREVER);
        ret = p_serv->p_servfunc->pfn_end(p_serv->p_cookie);
        if (ret == AW_OK) {
            p_serv->dev_is_enable = AW_FALSE;
        }

        AW_MUTEX_UNLOCK(p_serv->op_mutex);
    }

    return ret;
}

aw_err_t aw_gnss_ioctl(int id, aw_gnss_ioctl_req_t req, void *arg)
{
    struct awbl_gnss_service *p_serv = __gnss_id_to_serv(id);
    aw_err_t                  ret = AW_ERROR;

    if (p_serv == NULL) {
        return -AW_EINVAL;  /* [id] is invalid */
    }

    if (req == AW_GNSS_DEV_STATUS) {
        if (p_serv->init_stat == AWBL_GNSS_INIT_OK) {
            *(aw_err_t *)arg = AW_OK;
        } else {
            *(aw_err_t *)arg = -AW_EBUSY;
        }
        return AW_OK;
    }

    if (p_serv->init_stat == AWBL_GNSS_INIT_FAILED) {
        return -AW_EBUSY;
    }

    switch (req) {
        case AW_GNSS_NMEA_SELECT:
            p_serv->func_flag = (uint32_t)arg;
            return  AW_OK;

        case AW_GNSS_MAX_VALID_TIM:
            p_serv->valid_tim = (uint32_t)arg;
            return  AW_OK;

        default:
            /* 将由驱动的 ioctl 处理 */
            break;
    }

    if (p_serv->p_servfunc->dev_ioctl != NULL ) {

        AW_MUTEX_LOCK(p_serv->op_mutex, AW_SEM_WAIT_FOREVER);
        ret = p_serv->p_servfunc->dev_ioctl(p_serv->p_cookie, req, arg);
        AW_MUTEX_UNLOCK(p_serv->op_mutex);
    }

    return ret;
}

aw_err_t aw_gnss_dat_get   (int                    id,
                            aw_gnss_data_type_t    type,
                            void                  *ptr,
                            uint8_t                len)
{
    struct awbl_gnss_service *p_serv = __gnss_id_to_serv(id);
    aw_err_t                  ret = AW_ERROR;

    if (p_serv == NULL) {
        return -AW_EINVAL;  /* [id] is invalid */
    }

    if (p_serv->init_stat == AWBL_GNSS_INIT_FAILED) {
        return -AW_EBUSY;
    }

    AW_MUTEX_LOCK(p_serv->op_mutex, AW_SEM_WAIT_FOREVER);
    if (p_serv->p_servfunc->dat_get != NULL) {

        ret = p_serv->p_servfunc->dat_get(p_serv->p_cookie, type, ptr, len);
    } else {

        ret = __gnss_dat_get(p_serv, type, ptr, len);
    }
    AW_MUTEX_UNLOCK(p_serv->op_mutex);

    return ret;
}


/******************************************************************************/

/* end of file */

