#ifndef INC_CPPAGENT_STRINGBUILDER_H_
#define INC_CPPAGENT_STRINGBUILDER_H_
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <string>
#include <type_traits>

namespace cppagent
{
template<class T = char>
class StringBuilder
{
public:
    static_assert(std::is_trivially_copyable<T>::value, "StringBuilder<T> requires T to be trivially copyable.");
    inline static constexpr uint32_t Expand = 256;
    inline static constexpr uint32_t Mask = 255;

    explicit StringBuilder(uint32_t capacity = 0);
    StringBuilder(StringBuilder<T>&& other);
    StringBuilder<T>& operator=(StringBuilder<T>&& other);
    ~StringBuilder();

    void clear();
    std::string toString() const;
    StringBuilder<T>& append(const T c);
    StringBuilder<T>& append(const T* str);
    StringBuilder<T>& append(uint32_t size, const T* str);
    StringBuilder<T>& append(const std::string& str);
    StringBuilder<T>& append(bool boolean);
    StringBuilder<T>& print(const T* fmt, ...);

    void print_debug();
private:
    StringBuilder(const StringBuilder<T>&) = delete;
    StringBuilder<T>& operator=(const StringBuilder<T>&) = delete;
    void reserve(uint32_t capacity);
    uint32_t capacity_;
    uint32_t size_;
    T* buffer_;
};

template<class T>
StringBuilder<T>::StringBuilder(uint32_t capacity)
    : capacity_(capacity)
    , size_(0)
    , buffer_(nullptr)
{
    reserve(capacity_);
}

template<class T>
StringBuilder<T>::StringBuilder(StringBuilder<T>&& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , buffer_(other.buffer_)
{
    other.capacity_ = 0;
    other.size_ = 0;
    other.buffer_ = nullptr;
}

template<class T>
StringBuilder<T>& StringBuilder<T>::operator=(StringBuilder<T>&& other)
{
    if(this != &other) {
        capacity_ = other.capacity_;
        size_ = other.size_;
        buffer_ = other.buffer_;
        other.capacity_ = 0;
        other.size_ = 0;
        other.buffer_ = nullptr;
    }
    return *this;
}

template<class T>
StringBuilder<T>::~StringBuilder()
{
    capacity_ = 0;
    size_ = 0;
    delete[] buffer_;
    buffer_ = nullptr;
}

template<class T>
void StringBuilder<T>::clear()
{
    size_ = 0;
}

template<class T>
std::string StringBuilder<T>::toString() const
{
    return std::basic_string<T>(buffer_, size_);
}

template<class T>
StringBuilder<T>& StringBuilder<T>::append(const T c)
{
    return append(1, &c);
}

template<class T>
StringBuilder<T>& StringBuilder<T>::append(const T* str)
{
    return append(::strlen(str), str);
}

template<class T>
StringBuilder<T>& StringBuilder<T>::append(uint32_t size, const T* str)
{
    uint32_t capacity = capacity_ + size;
    if(capacity_<capacity){
        reserve((capacity+Expand)&~Mask);
    }
    ::memcpy(buffer_ + size_, str, size);
    size_ += size;
    assert(size_ <= capacity_);
    return *this;
}

template<class T>
StringBuilder<T>& StringBuilder<T>::append(const std::string& str)
{
    return append(static_cast<uint32_t>(str.size()), str.c_str());
}

template<class T>
StringBuilder<T>& StringBuilder<T>::append(bool boolean)
{
    if(boolean){
        return append(static_cast<uint32_t>(::strlen("true")), "true");
	} else {
		return append(static_cast<uint32_t>(::strlen("false")), "false");
    }
}

template<class T>
StringBuilder<T>& StringBuilder<T>::print(const T* fmt, ...)
{
    assert(nullptr != fmt);
    va_list args;
    va_start(args, fmt);
#ifdef _MSC_VER
    int32_t len = _vscprintf(fmt, args);
#else
    int32_t len = vsnprintf(nullptr, 0, fmt, args);
#endif
    if(len < 0) {
        va_end(args);
        return *this;
    }
    static constexpr int32_t Size = 255;
    T buffer[Size + 1];
    if(len <= Size) {
        len = vsnprintf(buffer, len + 1, fmt, args);
        if(0 <= len) {
            append(len, buffer);
        }
    } else {
        T* buff = (T*)cppa_malloc(sizeof(T) * (len + 1));
        len = vsnprintf(buff, len + 1, fmt, args);
        if(0 <= len) {
            append(len, buffer);
        }
        cppa_free(buff);
    }
    va_end(args);
    return *this;
}

template<class T>
void StringBuilder<T>::reserve(uint32_t capacity)
{
    assert(capacity_ <= capacity);
    assert(size_ <= capacity);
    T* buffer = new T[capacity];
    ::memcpy(buffer, buffer_, sizeof(T) * size_);
    delete[] buffer_;
    buffer_ = buffer;
    capacity_ = capacity;
}

} // namespace cppagent
#endif // INC_CPPAGENT_STRINGBUILDER_H_
