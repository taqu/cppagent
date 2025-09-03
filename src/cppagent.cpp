#include "cppagent.h"
#include <string>
#include <iostream>
#include <mimalloc/mimalloc.h>

void* cppa_malloc(std::size_t size)
{
    return mi_malloc(size);
}

void* cppa_malloc(std::size_t size, std::size_t alignment)
{
    return mi_malloc_aligned(size, alignment);
}

void cppa_free(void* ptr)
{
    return mi_free(ptr);
}

void cppa_free_size(void* ptr, std::size_t size)
{
    return mi_free_size(ptr, size);
}

void cppa_free_alignment(void* ptr, std::size_t alignment)
{
    return mi_free_aligned(ptr, alignment);
}

void cppa_free_size_alignment(void* ptr, std::size_t size, std::size_t alignment)
{
    return mi_free_size_aligned(ptr, size, alignment);
}

void* cppa_malloc(std::size_t size, const char* file, int line)
{
    (void)file;
    (void)line;
    return mi_malloc(size);
}

void* cppa_malloc(std::size_t size, std::size_t alignment, const char* file, int line)
{
    (void)file;
    (void)line;
    return mi_malloc_aligned(size, alignment);
}

void* cppa_realloc(void* ptr, size_t size)
{
    return mi_realloc(ptr, size);
}

void* cppa_realloc(void* ptr, size_t size, const char* file, int line)
{
    (void)file;
    (void)line;
    return mi_realloc(ptr, size);
}

void* operator new(std::size_t size, const char* file, int line)
{
    return cppa_malloc(size,file,line);
}

void* operator new(std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept
{
    return cppa_malloc(size,file,line);
}

void* operator new(std::size_t size)
{
    return cppa_malloc(size);
}

void* operator new(std::size_t size, const std::nothrow_t&) noexcept
{
    return cppa_malloc(size);
}

void* operator new(std::size_t size, std::align_val_t alignment)
{
    return cppa_malloc(size, (std::size_t)alignment);
}

void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return cppa_malloc(size, (std::size_t)alignment);
}

void* operator new[](std::size_t size, const char* file, int line)
{
    return cppa_malloc(size, file, line);
}

void* operator new[](std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept
{
    return cppa_malloc(size, file, line);
}

void* operator new[](std::size_t size)
{
    return cppa_malloc(size);
}

void* operator new[](std::size_t size, const std::nothrow_t&) noexcept
{
    return cppa_malloc(size);
}

void* operator new[](std::size_t size, std::align_val_t alignment)
{
    return cppa_malloc(size, (std::size_t)alignment);
}

void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    return cppa_malloc(size, (std::size_t)alignment);
}

void operator delete(void* ptr) noexcept
{
    cppa_free(ptr);
}

void operator delete(void* ptr, std::size_t size) noexcept
{
    cppa_free_size(ptr, size);
}

void operator delete(void* ptr, std::align_val_t alignment) noexcept
{
    cppa_free_alignment(ptr, (std::size_t)alignment);
}

void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept
{
    cppa_free_size_alignment(ptr, size, (std::size_t)alignment);
}

void operator delete(void* ptr, const std::nothrow_t&) noexcept
{
    cppa_free(ptr);
}

void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    cppa_free_alignment(ptr, (std::size_t)alignment);
}

void operator delete[](void* ptr) noexcept
{
    cppa_free(ptr);
}

void operator delete[](void* ptr, std::size_t size) noexcept
{
    cppa_free_size(ptr, size);
}

void operator delete[](void* ptr, std::align_val_t alignment) noexcept
{
    cppa_free_alignment(ptr, (std::size_t)alignment);
}

void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept
{
    cppa_free_size_alignment(ptr, size, (std::size_t)alignment);
}

void operator delete[](void* ptr, const std::nothrow_t&) noexcept
{
    cppa_free(ptr);
}

void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept
{
    cppa_free_alignment(ptr, (std::size_t)alignment);
}

