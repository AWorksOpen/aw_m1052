/**
 *  @file          cpu_usage.h
 *  @brief         
 *  @version       1.0.0
 *  @author        Or2, 2014-10-29 15:47:43
 */


#ifndef __CPU_USAGE_H__
#define __CPU_USAGE_H__

#include "rtk.h"



#ifdef __cplusplus
extern "C" {
#endif                                                                  /* __cplusplus                  */


struct cpu_usage_info
{
    struct rtk_task *task;
    unsigned int     tick_count;
};

struct cpu_usage
{
    unsigned int          tick_count_total;
    struct  rtk_tick      tick;
    
    unsigned int           count;
    struct cpu_usage_info *info;
};

#define CPU_USAGE_DEF(name, max) \
    static struct cpu_usage_info    name##_info[max]; \
    static struct cpu_usage         name##_mem = {0, {{0}}, max, name##_info}; \
    static struct cpu_usage        *name = &name##_mem;

void cpu_usage_init( struct cpu_usage *thiz );
void cpu_usage_start( struct cpu_usage *thiz );
void cpu_usage_clear( struct cpu_usage *thiz );
void cpu_usage_stop( struct cpu_usage *thiz );

/*
 *  
 * int ( *func )( void *arg, struct rtk_task *task, int percent );
 *      arg:  arg
 *      task: task pointor
 *      percent:        0~100
 */
void cpu_usage_for_each( struct cpu_usage *thiz,
                         int ( *func )( void*, struct rtk_task *, int ),
                         void *arg );
void cpu_usage_destroy( struct cpu_usage *thiz );



/**
 *  @code
 *
 *  demo
 *
 *  #include "cpu_usage.h"
 *  
 *  #define N_TASK  10
 *  
 *  CPU_USAGE_DEF( help_me, N_TASK );
 *   
 *  static int print_percent( void *arg, struct rtk_task *task, int percent )
 *  {
 *      int *index = arg;
 *      if ( *index == 0 ) {
 *          kprintf("**********************************************************************\r\n");  
 *          kprintf("tid\tname\tpercent\ttick\r\n");  
 *      }
 *      ++*index;
 *      kprintf("%d\t%s\t%d\t%d\r\n", task->tid, task->name, percent, help_me->info[task->tid-1].tick_count ); 
 *      return 0;
 *  }
 *   
 *  static void print_cpu_usage_task( void )
 *  {
 *      int index=0;
 *      task_delay(1000);
 *      cpu_usage_init( help_me );
 *      cpu_usage_start(help_me);
 *      while (9) {
 *          task_delay(100);
 *          index=0;
 *          kprintf("hello count=%d tick=%d\r\n", help_me->count, help_me->tick_count_total );
 *          cpu_usage_for_each( help_me, print_percent, &index );
 *      }
 *  }
 *
 *
 *  @endcode
 *  
 */


#ifdef __cplusplus
}
#endif                                                                  /*  __cplusplus                 */

#endif                                                                  /*  __CPU_USAGE_H__             */



