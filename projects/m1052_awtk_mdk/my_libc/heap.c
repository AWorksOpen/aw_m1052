#include <stdio.h>
#include "aw_mem.h"


void *malloc (size_t size)
{
    return aw_mem_alloc(size);
}



void *calloc (size_t nelem, size_t size)
{
    return aw_mem_calloc(nelem, size);
}    

/*C:\Users\GUOJIA~1\AppData\Local\Temp\p1e50-2: Error: L6915E: Library reports error: __use_no_heap was requested, but realloc was referenced*/
void *realloc (void* ptr, size_t new_size)
{
    return aw_mem_realloc(ptr, new_size);

}    

void free(void *ptr)
{
    aw_mem_free(ptr);
}









