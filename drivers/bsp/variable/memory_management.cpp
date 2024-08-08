#include "FreeRTOS.h"
#include "task.h"
#include <new>

// 重载 new 操作符
void* operator new(size_t size) noexcept
{
    void* p = pvPortMalloc(size);
    return p;
}

// 重载 new[] 操作符
void* operator new[](size_t size) noexcept
{
    void* p = pvPortMalloc(size);
    return p;
}

// 重载 delete 操作符
void operator delete(void* p) noexcept
{
    vPortFree(p);
}

// 重载 delete[] 操作符
void operator delete[](void* p) noexcept
{
    vPortFree(p);
}

// C++14 要求的带 size 参数的 delete 重载
void operator delete(void* p, size_t size) noexcept
{
    (void)size;
    vPortFree(p);
}

// C++14 要求的带 size 参数的 delete[] 重载
void operator delete[](void* p, size_t size) noexcept
{
    (void)size;
    vPortFree(p);
}

// 可选：处理内存分配失败的情况
void* operator new(size_t size, const std::nothrow_t&) noexcept
{
    void* p = pvPortMalloc(size);
    return p;
}

void* operator new[](size_t size, const std::nothrow_t&) noexcept
{
    void* p = pvPortMalloc(size);
    return p;
}

