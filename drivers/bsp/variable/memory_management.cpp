#include "FreeRTOS.h"
#include "malloc.h"
#include "task.h"
#include <new>

void *operator new(size_t size)
{
    return mymalloc(size);
}

void operator delete(void *ptr) noexcept
{
    myfree(ptr);
}

void *operator new[](size_t size)
{
    return mymalloc(size);
}

void operator delete[](void *ptr) noexcept
{
    myfree(ptr);
}
