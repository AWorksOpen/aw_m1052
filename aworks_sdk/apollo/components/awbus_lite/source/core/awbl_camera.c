/*******************************************************************************
*                                 AWorks
*                       ---------------------------
*                       innovating embedded platform
*
* Copyright (c) 2001-2012 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    http://www.zlg.cn/
*******************************************************************************/
#include "AWorks.h"
#include "awbus_lite.h"
#include "aw_int.h"
#include "string.h"
#include "aw_camera.h"
#include "awbl_camera.h"
#include "aw_sem.h"
#include "aw_mem.h"
#include "aw_vdebug.h"

/*******************************************************************************
  import
*******************************************************************************/

AWBL_METHOD_IMPORT(awbl_cameraserv_get);

/*******************************************************************************
  locals
*******************************************************************************/

/* pointer to first camera service */
aw_local struct awbl_camera_service *__gp_camera_serv_head = NULL;

AW_MUTEX_DECL_STATIC(__g_camera_serv_lock);

/*******************************************************************************
  implementation
*******************************************************************************/


aw_local aw_err_t __camera_serv_alloc_helper (struct awbl_dev *p_dev, void *p_arg)
{
    awbl_method_handler_t      pfunc_camera_serv = NULL;
    struct awbl_camera_service  *p_camera_serv     = NULL;

    /* find handler */

    pfunc_camera_serv = awbl_dev_method_get(p_dev,
                                          AWBL_METHOD_CALL(awbl_cameraserv_get));

    /* call method handler to allocate camera service */

    if (pfunc_camera_serv != NULL) {

        pfunc_camera_serv(p_dev, &p_camera_serv);

        /* insert a camera service to the head of service list */

        if (p_camera_serv != NULL) {
            p_camera_serv->p_next = __gp_camera_serv_head;
            __gp_camera_serv_head = p_camera_serv;
        }
    }

    return AW_OK;   /* iterating continue */
}

/**
 * \brief allocate camera services from instance tree
 */
aw_err_t __camera_serv_alloc (void)
{
    /* todo: count the number of service */

    /* Empty the list head, as every camera device will be find */
    __gp_camera_serv_head = NULL;

    awbl_dev_iterate(__camera_serv_alloc_helper, NULL, AWBL_ITERATE_INSTANCES);

    return AW_OK;
}

/****************************CAMERA SERVER OPTION*****************************/


aw_local struct awbl_camera_service* __camera_serv_create(char *p_name);
aw_local aw_err_t __camera_serv_insert(struct awbl_camera_service *p_serv);
aw_local struct awbl_camera_service* __camera_serv_find(char *p_name);
aw_local struct awbl_camera_service* __camera_serv_remove(char *p_name);
aw_local aw_err_t __camera_serv_delete(struct awbl_camera_service *p_serv);

/**
 * \brief 创建一个摄像头服务对象
 */
aw_local struct awbl_camera_service* __camera_serv_create(char *p_name)
{
    struct awbl_camera_service *p_serv_new;

    p_serv_new = (struct awbl_camera_service *)aw_mem_alloc(sizeof(struct awbl_camera_service));
    if (p_serv_new == NULL) {
        AW_MUTEX_UNLOCK(__g_camera_serv_lock);
        return NULL;
    }
    memset(p_serv_new, 0, sizeof(struct awbl_camera_service));

    /* 如果设备在创建之前未被打开（创建），则为其分配内存资源 */
    p_serv_new->p_name = (char *)aw_mem_alloc(32);
    if (p_serv_new->p_name == NULL) {
        AW_MUTEX_UNLOCK(__g_camera_serv_lock);
        aw_mem_free((void *)p_serv_new);
        return NULL;
    }
    strcpy(p_serv_new->p_name, p_name);

    /* 初始化相关变量  */
    p_serv_new->flag |= AW_CAMERA_FLAG_DYNAMIC;
    AW_SEMC_INIT(p_serv_new->sem, 0, AW_SEM_Q_PRIORITY);
    return p_serv_new;
}

/**
 * \brief 将摄像头服务对象加入引索中
 */
aw_local aw_err_t __camera_serv_insert(struct awbl_camera_service *p_serv)
{
    p_serv->p_next = __gp_camera_serv_head;
    __gp_camera_serv_head = p_serv;
    return AW_OK;
}

/**
 * \brief 从引索中查找摄像头对象
 *
 * \return 摄像头服务对象
 */
aw_local struct awbl_camera_service* __camera_serv_find(char *p_name)
{
    struct awbl_camera_service *p_serv_cur;

    p_serv_cur = __gp_camera_serv_head;

    while ((p_serv_cur != NULL)) {
        if (strcmp(p_serv_cur->p_name, p_name) == 0) {
            return p_serv_cur;
        }
        p_serv_cur = p_serv_cur->p_next;
    }

    return NULL;
}

/**
 * \brief 从引索中移除摄像头对象（并不销毁）
 */
aw_local struct awbl_camera_service* __camera_serv_remove(char *p_name)
{
    struct awbl_camera_service *p_serv_cur;
    struct awbl_camera_service **p_serv_prev;

    p_serv_cur = __gp_camera_serv_head;
    p_serv_prev = &__gp_camera_serv_head;

    /* 遍历链表搜索对应的服务  */
    while ((p_serv_cur != NULL)) {
        if (strcmp(p_serv_cur->p_name, p_name) == 0) {
            *p_serv_prev = p_serv_cur->p_next;
            break;
        }
        *p_serv_prev = p_serv_cur;
         p_serv_cur = p_serv_cur->p_next;
    }

    return p_serv_cur;
}

/**
 * \brief 销毁摄像头对象（请确保摄像头已经从引索中移除）
 */
aw_local aw_err_t __camera_serv_delete(struct awbl_camera_service *p_serv)
{
    if (p_serv->flag & AW_CAMERA_FLAG_DYNAMIC) {
        aw_mem_free(p_serv->p_name);
        aw_mem_free(p_serv);
    }
    return AW_OK;
}

/*****************************************************************************/


/********************************AWBL INTERFACE*******************************/

void aw_camera_serve_init (void)
{
    AW_MUTEX_INIT(__g_camera_serv_lock, AW_SEM_Q_PRIORITY);
    __camera_serv_alloc();
}




aw_err_t aw_camera_serve_create(char *p_name, void *p_cookie, struct awbl_camera_servfuncs *serv_fun)
{
    struct awbl_camera_service *p_serv;

    AW_MUTEX_LOCK(__g_camera_serv_lock, AW_SEM_WAIT_FOREVER);
    p_serv =__camera_serv_find(p_name);

    if (p_serv == NULL) {
        p_serv = __camera_serv_create(p_name);
        __camera_serv_insert(p_serv);
    }
    else {
        /* 避免重复创建  */
        if (p_serv->flag & AW_CAMERA_FLAG_ALIVE) {
            AW_MUTEX_UNLOCK(__g_camera_serv_lock);
            return -AW_EEXIST;
        }
    }

    p_serv->p_cookie = p_cookie;
    p_serv->used_count++;
    p_serv->p_servfuncs = serv_fun;
    p_serv->flag |= AW_CAMERA_FLAG_ALIVE;

    AW_SEMC_GIVE(p_serv->sem);

    AW_MUTEX_UNLOCK(__g_camera_serv_lock);

    return AW_OK;
}

aw_err_t aw_camera_serve_delete(char *p_name)
{
    struct awbl_camera_service *p_serv_cur;
    int take_flag = 0;
    aw_err_t ret;

    AW_MUTEX_LOCK(__g_camera_serv_lock, AW_SEM_WAIT_FOREVER);

    p_serv_cur = __camera_serv_remove(p_name);

    p_serv_cur->flag &= (~AW_CAMERA_FLAG_ALIVE);

    if (--p_serv_cur->used_count == 0) {
        __camera_serv_delete(p_serv_cur);
    } else {
        /* 当前设备仍被使用，需要取走信号量以避免被再次打开  */
        take_flag = 1;
    }

    AW_MUTEX_UNLOCK(__g_camera_serv_lock);

    if (take_flag) {
        ret = AW_SEMC_TAKE(p_serv_cur->sem, AW_SEM_WAIT_FOREVER);
        if (ret != AW_OK) {
            return ret;
        }
    }

    return AW_OK;
}

/**
 * \brief   打开摄像头
 *
 * \param       p_name  摄像头名
 * \param[in]   timeout 最大等待时间(超时时间), 单位是系统节拍, 系统时钟节拍频率
 *                      由函数 aw_sys_clkrate_get()获得, 可使用 aw_ms_to_ticks()
 *                      将毫秒转换为节拍数; #AW_SEM_NO_WAIT 为无阻塞;
 *                      #AW_SEM_WAIT_FOREVER 为一直等待, 无超时
 * \return  成功返回摄像头服务的指针, 否则返回NULL
 */
void *aw_camera_wait_open(char *p_name, int time_out)
{
    struct awbl_camera_service *p_serv;
    int ret = 0;

    AW_MUTEX_LOCK(__g_camera_serv_lock, AW_SEM_WAIT_FOREVER);

    p_serv =__camera_serv_find(p_name);
    if (p_serv == NULL ) {
        p_serv = __camera_serv_create(p_name);
        __camera_serv_insert(p_serv);

        if (ret != AW_OK) {
            __camera_serv_remove(p_name);
            __camera_serv_delete(p_serv);
            return NULL;
        }
    }

    AW_MUTEX_UNLOCK(__g_camera_serv_lock);

    ret = AW_SEMC_TAKE(p_serv->sem, time_out);
    if (ret != AW_OK) {
        aw_kprintf("wait for camera failed:%d\n\r", ret);
        return NULL;
    }

    AW_SEMC_GIVE(p_serv->sem);
    ret = p_serv->p_servfuncs->pfunc_camera_open(p_serv->p_cookie);
    if (ret == AW_OK) {
        ++p_serv->used_count;
        return (void*)p_serv;
    }

    return NULL;
}

/*****************************************************************************/


/******************************APPLICATION INTERFACE**************************/

/**
 * \brief   打开摄像头
 *
 * \param   p_name  摄像头名
 * \return  成功返回摄像头服务的指针, 否则返回NULL
 */
void* aw_camera_open (char *p_name)
{
    struct awbl_camera_service *p_serv;
    int ret = 0;

    AW_MUTEX_LOCK(__g_camera_serv_lock, AW_SEM_WAIT_FOREVER);
    p_serv =__camera_serv_find(p_name);
    AW_MUTEX_UNLOCK(__g_camera_serv_lock);

    if (p_serv == NULL) {
        return NULL;
    }

    ret = p_serv->p_servfuncs->pfunc_camera_open(p_serv->p_cookie);
    if (ret == AW_OK) {
        ++p_serv->used_count;
        return (void*)p_serv;
    }

    return NULL;
}

/**
 * \brief 关闭摄像头
 *
 * \param   p_serv  摄像头的服务指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_close (void *p_serv)
{

    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;
    aw_err_t ret;
    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    ret = p_server->p_servfuncs->pfunc_camera_close(p_server->p_cookie);

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        if (--p_server->used_count == 0) {
            __camera_serv_delete(p_server);
        }
    }

    return ret;
}


/**
 * \brief  摄像头配制接口
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_cfg   配制内容的指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_config_set (void *p_serv, struct aw_camera_cfg *p_cfg, uint32_t *option)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return AW_ENODEV;
    }

    return p_server->p_servfuncs->pfunc_camera_config_set(p_server->p_cookie, p_cfg, option);
}


/**
 * \brief  摄像头配制接口
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_cfg   配制内容的指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_config_get (void *p_serv, struct aw_camera_cfg *p_cfg, uint32_t *option)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return AW_ENODEV;
    }

    return p_server->p_servfuncs->pfunc_camera_config_get(p_server->p_cookie, p_cfg, option);
}


/**
 * \brief 释放摄像头的一帧拍照地址
 *
 * \param   p_serv  摄像头的服务指针
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_release_photo_buf(void *p_serv, void *buf)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return AW_ENODEV;
    }

    return p_server->p_servfuncs-> \
            pfunc_camera_release_photo_buf(p_server->p_cookie, (uint32_t)buf);
}


/**
 * \brief 释放摄像头的一帧预览地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   addr    待释放的地址
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_release_video_buf(void *p_serv, void *buf)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return AW_ENODEV;
    }

    return p_server->p_servfuncs->\
            pfunc_camera_release_video_buf( p_server->p_cookie, (uint32_t)buf);
}


/**
 * \brief 获取摄像头的一帧拍照地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_buf   获取的地址存储在p_buf中
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_get_photo_buf (void *p_serv, void **p_buf)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return AW_ENODEV;
    }

    return  p_server->p_servfuncs->\
            pfunc_camera_get_photo_buf(p_server->p_cookie, (uint32_t **)p_buf);
}


/**
 * \brief 获取摄像头的一帧预览地址
 *
 * \param   p_serv  摄像头的服务指针
 * \param   p_buf   获取的地址存储在p_buf中
 * \return  成功返回AW_OK, 否则返回相应的错误代码
 */
aw_err_t aw_camera_get_video_buf (void *p_serv, void **p_buf)
{
    struct awbl_camera_service *p_server = (struct awbl_camera_service *)p_serv;

    if (p_serv == NULL) {
        return -AW_EFAULT;
    }

    if (!(p_server->flag & AW_CAMERA_FLAG_ALIVE)) {
        return -AW_ENODEV;
    }

    return p_server->p_servfuncs->\
            pfunc_camera_get_video_buf(p_server->p_cookie, (uint32_t **)p_buf);
}

/*****************************************************************************/


/* 判断是否在中断上下文中调用接口,如果在中断上下文中则返回NULL  */
//if (AW_INT_CONTEXT ()) {
//    return NULL;
//}
