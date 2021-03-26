/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2020 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

#define AW_USB_WAIT_FOREVER  -1
#define AW_USB_NO_WAIT        0



typedef void*  aw_usb_task_handle_t;
typedef void*  aw_usb_sem_handle_t;
typedef void*  aw_usb_mutex_handle_t;


aw_usb_task_handle_t aw_usb_task_create (const char  *name,
                                         int          prio,
                                         size_t       stk_s,
                                         void       (*pfnc) (void *p_arg),
                                         void        *p_arg);


aw_err_t aw_usb_task_delete (aw_usb_task_handle_t tsk);

aw_err_t aw_usb_task_startup (aw_usb_task_handle_t tsk);

aw_err_t aw_usb_task_suspend(aw_usb_task_handle_t tsk);

aw_err_t aw_usb_task_resume(aw_usb_task_handle_t tsk);

aw_usb_sem_handle_t aw_usb_sem_create (void);

void aw_usb_sem_delete (aw_usb_sem_handle_t sem);
aw_err_t aw_usb_sem_take (aw_usb_sem_handle_t sem,
                          int                 timeout);

aw_err_t aw_usb_sem_give (aw_usb_sem_handle_t sem);


aw_usb_mutex_handle_t aw_usb_mutex_create (void);

void aw_usb_mutex_delete (aw_usb_mutex_handle_t mutex);

aw_err_t aw_usb_mutex_lock (aw_usb_mutex_handle_t mutex,
                            int                   timeout);

aw_err_t aw_usb_mutex_unlock (aw_usb_mutex_handle_t mutex);
