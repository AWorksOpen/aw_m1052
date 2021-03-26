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

#include "aworks.h"
#include "aw_vdebug.h"
#include "aw_led.h"
#include "aw_delay.h"
#include "aw_fb.h"

#define LED 0

#define TK_STACK_SIZE 64 * 1024

extern void* aworks_lcd_init(void);
extern int gui_app_start(int lcd_w, int lcd_h) ;
extern void ts_init (void* p_fb);


aw_local void __task_entry(void *p_arg)
{
    void* awtk_fb = aworks_lcd_init();

    aw_fb_var_info_t var_info;
    aw_fb_ioctl(awtk_fb, AW_FB_CMD_GET_VINFO, &var_info);

    ts_init(awtk_fb);

    gui_app_start(var_info.res.x_res, var_info.res.y_res);

}




void app_awtk_demo (void)
{
  /* 定义任务实体  */
  AW_TASK_DECL_STATIC(lcd_task, TK_STACK_SIZE);

  AW_TASK_INIT(lcd_task,      /* 任务实体 */
               "awtk_task",   /* 任务名字 */
               5,             /* 任务优先级 */
               TK_STACK_SIZE, /* 任务堆栈大小 */
               __task_entry,  /* 任务入口函数 */
               NULL);         /* 任务入口参数 */

  AW_TASK_STARTUP(lcd_task); /* 启动任务 */
}

/* end of file*/
