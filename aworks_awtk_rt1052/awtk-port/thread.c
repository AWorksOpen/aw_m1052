#include "aworks.h"
#include "aw_task.h"
#include "tkc/mem.h"
#include "tkc/thread.h"
struct _tk_thread_t {
    void*             args;
    tk_thread_entry_t entry;
    aw_task_id_t id;
    bool_t       running;
    const char* name;
    uint32_t    stack_size;
    uint32_t    priority;
    void*       status;
};
tk_thread_t* tk_thread_create(tk_thread_entry_t entry, void* args)
{
    return_value_if_fail(entry != NULL, NULL);
    tk_thread_t* thread = (tk_thread_t*)TKMEM_ZALLOC(tk_thread_t);
    return_value_if_fail(thread != NULL, NULL);
    thread->args       = args;
    thread->entry      = entry;
    thread->name       = "tk_thread";
    thread->stack_size = 1024;
    thread->priority   = aw_task_lowest_priority();
    return thread;
}
ret_t tk_thread_set_name(tk_thread_t* thread, const char* name)
{
    return_value_if_fail(thread != NULL && name != NULL, RET_BAD_PARAMS);
    thread->name = name;
    return RET_OK;
}
ret_t tk_thread_set_stack_size(tk_thread_t* thread, uint32_t stack_size)
{
    return_value_if_fail(thread != NULL, RET_BAD_PARAMS);
    thread->stack_size = stack_size;
    return RET_OK;
}
ret_t tk_thread_set_priority(tk_thread_t* thread, uint32_t priority)
{
    return_value_if_fail(thread != NULL, RET_BAD_PARAMS);
    thread->priority = priority;
    return RET_OK;
}
ret_t tk_thread_start(tk_thread_t* thread)
{
    return_value_if_fail(thread != NULL && !thread->running, RET_BAD_PARAMS);
    thread->id = aw_task_create(thread->name, thread->priority, thread->stack_size, thread->entry, thread->args);
    if (thread->id == AW_TASK_ID_INVALID) {
        return RET_FAIL;
    }
    if (aw_task_startup(thread->id) == AW_OK) {
        thread->running = TRUE;
    } else {
        return RET_FAIL;
    }
    return RET_OK;
}
ret_t tk_thread_join(tk_thread_t* thread)
{
    return_value_if_fail(thread != NULL, RET_BAD_PARAMS);
    if (aw_task_join(thread->id, &thread->status) == AW_OK) {
        return RET_OK;
    } else {
        return RET_FAIL;
    }
}
void* tk_thread_get_args(tk_thread_t* thread)
{
    return_value_if_fail(thread != NULL, NULL);
    return thread->args;
}
ret_t tk_thread_destroy(tk_thread_t* thread)
{
    return_value_if_fail(thread != NULL && thread->id != AW_TASK_ID_INVALID, RET_BAD_PARAMS);
    return_value_if_fail(aw_task_terminate(thread->id) == AW_OK, RET_FAIL);
    memset(thread, 0x00, sizeof(tk_thread_t));
    TKMEM_FREE(thread);
    return RET_OK;
}
uint64_t tk_thread_self(void) {
  return (uint64_t)aw_task_id_self();
}