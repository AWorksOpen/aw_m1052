#include <stdio.h>
#include "py/mpconfig.h"
#include "py/runtime.h"
#include "py/mpthread.h"
#include "py/gc.h"
#include "mpthreadport.h"
#include "rtk_list.h"
#include "rtk_int.h"
#include "rtk_task.h"
#include "aw_mem.h"

#define MP_THREAD_MIN_STACK_SIZE                        (2 * 1024)
#define MP_THREAD_DEFAULT_STACK_SIZE                    (MP_THREAD_MIN_STACK_SIZE + 1024)
#define MP_THREAD_PRIORITY                              (2)


typedef struct {
    struct rtk_task * thread;
    /* whether the thread is ready and running */
    aw_bool_t ready;
    /* thread Python args, a GC root pointer */
    void *arg;
    /* pointer to the stack */
    void *stack;
    void *tcb;
    /* number of words in the stack */
    size_t stack_len;
    rtk_list_node_t list;
} mp_thread, *mp_thread_t;

STATIC mp_thread *main_thread;

STATIC mp_thread thread_entry0;

typedef struct {
    struct rtk_mutex * mutex;
    rtk_list_node_t list;
} mp_mutex, *mp_mutex_t;


STATIC mp_thread_mutex_t thread_mutex;

STATIC rtk_list_node_t thread_list,mutex_list;


void mp_thread_mutex_init(mp_thread_mutex_t *mutex){
    static uint8_t count = 0;
    char name[AW_NAME_MAX];
    uint32_t level;

    level = __rtk_interrupt_disable();

    mp_mutex *node = aw_mem_alloc(sizeof(mp_mutex));
    if (node == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create mutex list node"));
    }

    /* build name */
    snprintf(name, sizeof(name), "mp%02d", count ++);

    int ret = rtk_mutex_init(mutex);
    if(ret != 0){
        mp_raise_OSError(ret);
    }

    // add mutex to linked list of all mutexs
    node->mutex = mutex;
    rtk_list_add_tail(&(node->list), &mutex_list);

    __rtk_interrupt_enable(level);

}

int mp_thread_mutex_lock(mp_thread_mutex_t *mutex, int wait) {

    int ret = rtk_mutex_lock (mutex, wait);
    if(ret == AW_OK){
        return 1;
    }else if(ret == -AW_EAGAIN){
        return 0;
    }else{
        return ret;
    }
}

void mp_thread_mutex_unlock(mp_thread_mutex_t *mutex) {
    rtk_mutex_unlock(mutex);
}
STATIC void *(*ext_thread_entry)(void*) = NULL;



STATIC void thread_entry(void *arg) {
    if (ext_thread_entry) {
        ext_thread_entry(arg);
    }


    return ;
}
void mp_thread_create_ex(void *(*entry)(void*), void *arg, size_t *stack_size, int priority, char *name) {
    // store thread entry function into a global variable so we can access it
    ext_thread_entry = entry;//(void (*)(void *parameter))

    if (*stack_size == 0) {
        *stack_size = MP_THREAD_DEFAULT_STACK_SIZE; // default stack size
    } else if (*stack_size < MP_THREAD_MIN_STACK_SIZE) {
        *stack_size = MP_THREAD_MIN_STACK_SIZE; // minimum stack size
    }

    // allocate TCB, stack and linked-list node (must be outside thread_mutex lock)
    struct rtk_task * th = m_new_obj(struct rtk_task);
    if (th == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread TCB"));
    }
    uint8_t *stack = m_new(uint8_t, *stack_size);
    if (stack == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread stack"));
    }
    mp_thread *node = m_new_obj(mp_thread);
    if (node == NULL) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, "can't create thread list node"));
    }
    // adjust the stack_size to provide room to recover from hitting the limit
    *stack_size -= 1024;

    node->ready = AW_FALSE;
    node->arg = arg;
    node->stack = stack;
    node->stack_len = *stack_size / 4;

    mp_thread_mutex_lock(&thread_mutex, 1);

    rtk_task_init(th, name, priority + __HIGH_THAN_USR_PRIORITY_COUNT , 0, \
            (char *)stack, (char *)(stack + *stack_size), thread_entry, arg, (void *)0);

    // add thread to linked list of all threads
    {
        uint32_t level;

        level = __rtk_interrupt_disable();

        node->thread = th;
        rtk_list_add_tail(&(node->list), &thread_list);

        __rtk_interrupt_enable(level);
    }

    rtk_task_startup(th);

    mp_thread_mutex_unlock(&thread_mutex);
}

void mp_thread_create(void *(*entry)(void*), void *arg, size_t *stack_size){
    static uint8_t count = 0;
    char name[10];

    snprintf(name,sizeof(name),"mpthread02%d",count++);
    mp_thread_create_ex(entry, arg, stack_size, MP_THREAD_PRIORITY, name);
}

void mp_thread_start(void){
    rtk_list_node_t *list = &thread_list, *node = NULL;
    mp_thread_t cur_thread_node = NULL;

    mp_thread_mutex_lock(&thread_mutex, 1);

    for (node = list->next; node != list; node = node->next) {
        cur_thread_node = RTK_LIST_ENTRY(node, mp_thread, list);
        if (cur_thread_node->thread == rtk_task_self()) {
            cur_thread_node->ready = AW_TRUE;
            break;
        }
    }

    mp_thread_mutex_unlock(&thread_mutex);

}
struct _mp_state_thread_t *mp_thread_get_state(void) {
    return (mp_state_thread_t *)(rtk_task_self()->thread_local_storage);
}


void mp_thread_set_state(struct _mp_state_thread_t *state){
    rtk_task_self()->thread_local_storage = (void *)state;
}


void mp_thread_finish(void){
    rtk_list_node_t *list = &thread_list, *node = NULL;
    mp_thread_t cur_thread_node = NULL;

    mp_thread_mutex_lock(&thread_mutex, 1);

    for (node = list->next; node != list; node = node->next) {
        cur_thread_node = RTK_LIST_ENTRY(node, mp_thread, list);
        if (cur_thread_node->thread == rtk_task_self()) {
            cur_thread_node->ready = AW_FALSE;
            // explicitly release all its memory
            m_del((struct rtk_task), cur_thread_node->thread, 1);
            m_del(uint8_t, cur_thread_node->stack, cur_thread_node->stack_len);
//            m_del(mp_thread, cur_thread_node, 1);
            rtk_list_del(node);
            break;
        }
    }

    mp_thread_mutex_unlock(&thread_mutex);

//    rtk_thread_detach(rtk_task_self());
}

/**
 * thread port initialization
 *
 * @param stack MicroPython main thread stack
 * @param stack_len MicroPython main thread stack, unit: word
 */
void mp_thread_init(void *stack, uint32_t stack_len) {
    mp_thread_set_state(&mp_state_ctx.thread);

    main_thread = &thread_entry0;
    main_thread->thread = rtk_task_self();
    main_thread->ready = AW_TRUE;
    main_thread->arg = NULL;
    main_thread->stack = stack;
    main_thread->stack_len = stack_len;

    rtk_list_node_init(&thread_list);
    rtk_list_node_init(&mutex_list);

    rtk_list_add_tail(&(main_thread->list), &thread_list);

    mp_thread_mutex_init(&thread_mutex);
}

void mp_thread_deinit(void) {
    uint32_t level;

    level = __rtk_interrupt_disable();
    /* remove all thread node on list */
    {
        rtk_list_node_t *list = &thread_list, *node = NULL;
        mp_thread_t cur_thread_node = NULL;

        for (node = list->next; node != list; node = node->next) {
            cur_thread_node = RTK_LIST_ENTRY(node, mp_thread, list);
            if (cur_thread_node->thread != main_thread->thread) {
                rtk_task_terminate(cur_thread_node->thread);
            }
        }
    }

    /* remove all mutex node on list */
    {
        rtk_list_node_t *list = &mutex_list, *node = NULL;
        mp_mutex_t cur_mutex_node = NULL;

        for (node = list->next; node != list; node = node->next) {
            cur_mutex_node = RTK_LIST_ENTRY(node, mp_mutex, list);
            int ret = rtk_mutex_terminate(cur_mutex_node->mutex);
            if(ret != 0){
                mp_raise_OSError(ret);
            }
            aw_mem_free(cur_mutex_node);
        }
    }
    __rtk_interrupt_enable(level);
    // allow Thread to clean-up the threads
    rtk_task_delay(200);

}

void mp_thread_gc_others(void) {
    rtk_list_node_t *list = &thread_list, *node = NULL;
    mp_thread_t cur_thread_node = NULL;

    mp_thread_mutex_lock(&thread_mutex, 1);

    for (node = list->next; node != list; node = node->next) {
        cur_thread_node = RTK_LIST_ENTRY(node, mp_thread, list);
        gc_collect_root((void **)&cur_thread_node->thread, 1);
        /* probably not needed */
        gc_collect_root(&cur_thread_node->arg, 1);
        if (cur_thread_node->thread == rtk_task_self()) {
            continue;
        }
        if (!cur_thread_node->ready) {
            continue;
        }
        /* probably not needed */
        gc_collect_root(cur_thread_node->stack, cur_thread_node->stack_len);
    }

    mp_thread_mutex_unlock(&thread_mutex);
}
