#ifndef INC_CPPAGENT_H_
#define INC_CPPAGENT_H_
#include <cstdint>
#include <new>

//--- Memory allocation
//----------------------------------------------------------------
void* operator new(std::size_t size, const char* file, int line);
void* operator new(std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept;
void* operator new(std::size_t size);
void* operator new(std::size_t size, const std::nothrow_t&) noexcept;

void* operator new(std::size_t size, std::align_val_t alignment);
void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
void* operator new(std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void* operator new[](std::size_t size, const char* file, int line);
void* operator new[](std::size_t size, const std::nothrow_t&, const char* file, int line) noexcept;
void* operator new[](std::size_t size);
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept;

void* operator new[](std::size_t size, std::align_val_t alignment);
void* operator new[](std::size_t size, const std::nothrow_t&) noexcept;
void* operator new[](std::size_t size, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void operator delete(void* ptr) noexcept;
void operator delete(void* ptr, std::size_t size) noexcept; 
void operator delete(void* ptr, std::size_t alignment) noexcept;
void operator delete(void* ptr, std::size_t size, std::align_val_t alignment) noexcept;
void operator delete(void* ptr, const std::nothrow_t&) noexcept; 
void operator delete(void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void operator delete[](void* ptr) noexcept;
void operator delete[](void* ptr, std::size_t size) noexcept; 
void operator delete[](void* ptr, std::size_t alignment) noexcept;
void operator delete[](void* ptr, std::size_t size, std::align_val_t alignment) noexcept;
void operator delete[](void* ptr, const std::nothrow_t&) noexcept; 
void operator delete[](void* ptr, std::align_val_t alignment, const std::nothrow_t&) noexcept;

void* cppa_malloc(std::size_t size);
void* cppa_malloc(std::size_t size, std::size_t alignment);

void cppa_free(void* ptr);
void cppa_free_size(void* ptr, std::size_t size);
void cppa_free_alignment(void* ptr, std::size_t alignment);
void cppa_free_size_alignment(void* ptr, std::size_t size, std::size_t alignment);

void* cppa_malloc(std::size_t size, const char* file, int line);
void* cppa_malloc(std::size_t size, std::size_t alignment, const char* file, int line);
void* cppa_realloc(void* ptr, size_t size);
void* cppa_realloc(void* ptr, size_t size, const char* file, int line);

#define CPPA_NULL nullptr

#ifdef _DEBUG
#define CPPA_MALLOC(size) cppa_malloc(size,__FILE__,__LINE__)
#define CPPA_MALLOC_ALIGNED(size,alignment) cppa_malloc(size,alignment,__FILE__,__LINE__)
#define CPPA_REALLOC(ptr,size) cppa_realloc(ptr,size,__FILE__,__LINE__)
#define CPPA_FREE(ptr) cppa_free(ptr);(ptr)=CPPA_NULL
#define CPPA_FREE_ALIGNED(ptr,alignment) cppa_free(ptr,alignment);(ptr)=CPPA_NULL
#define CPPA_NEW new(__FILE__,__LINE__)
#define CPPA_DELETE(ptr) delete(ptr);(ptr)=CPPA_NULL
#define CPPA_DELETE_ARRAY(ptr) delete[](ptr);(ptr)=CPPA_NULL
#else
#define CPPA_MALLOC(size) cppa_malloc(size)
#define CPPA_MALLOC_ALIGNED(size,alignment) cppa_malloc(size,alignment)
#define CPPA_REALLOC(ptr,size) cppa_realloc(ptr,size)
#define CPPA_FREE(ptr) cppa_free(ptr);(ptr)=CPPA_NULL
#define CPPA_FREE_ALIGNED(ptr,alignment) cppa_free(ptr,alignment);(ptr)=CPPA_NULL
#define CPPA_NEW new
#define CPPA_DELETE(ptr) delete(ptr);(ptr)=CPPA_NULL
#define CPPA_DELETE_ARRAY(ptr) delete[](ptr);(ptr)=CPPA_NULL
#endif

namespace cppagent
{
}
#endif //INC_CPPAGENT_H_
