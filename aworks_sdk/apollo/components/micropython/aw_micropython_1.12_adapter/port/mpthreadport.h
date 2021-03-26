#include "stdint.h"
#include "stddef.h"

typedef struct rtk_mutex mp_thread_mutex_t;

void mp_thread_init(void *stack, uint32_t stack_len);
void mp_thread_deinit(void);
void mp_thread_gc_others(void);
