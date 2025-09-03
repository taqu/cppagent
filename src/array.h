#ifndef INC_CPPAGENT_ARRAY_H_
#define INC_CPPAGENT_ARRAY_H_
#include <cassert>
#include <cstdarg>
#include <cstdint>
#include <cstring>
#include <type_traits>
#include "cppagent.h"

namespace cppagent
{
template<class T>
class Array
{
public:
    inline static constexpr uint32_t Expand = 8;

    Array();
    Array(const Array& other);
    Array(Array&& other);
    Array& operator=(const Array& other);
    Array& operator=(Array&& other);
    ~Array();

    inline uint32_t capacity() const;
    inline uint32_t size() const;

    void clear();
    void add(const T& item);
    void add(T&& item);
    template<class ...Args>
    void emplace(Args&&... args);

    void removeAt(uint32_t index);

    inline const T& operator[](uint32_t index) const;
    inline T& operator[](uint32_t index);

    inline const T* begin() const;
    inline T* begin();
    inline const T* end() const;
    inline T* end();

private:
    void expand();

    uint32_t capacity_;
    uint32_t size_;
    T* items_;
};

template<class T>
Array<T>::Array()
    : capacity_(0)
    , size_(0)
    , items_(nullptr)
{
}

template<class T>
Array<T>::Array(const Array& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , items_(nullptr)
{
    items_ = reinterpret_cast<T*>(CPPA_MALLOC(sizeof(T) * capacity_));
    for(uint32_t i = 0; i < size_; ++i) {
        new(&items_[i]) T(other.items_[i]);
    }
}

template<class T>
Array<T>::Array(Array&& other)
    : capacity_(other.capacity_)
    , size_(other.size_)
    , items_(other.items_)
{
    other.capacity_ = 0;
    other.size_ = 0;
    other.items_ = nullptr;
}

template<class T>
Array<T>& Array<T>::operator=(const Array& other)
{
    if(this == &other) {
        return *this;
    }
    this->~Array();
    capacity_ = other.capacity_;
	size_ = other.size_;
    items_ = reinterpret_cast<T*>(CPPA_MALLOC(sizeof(T) * other.capacity_));
    for(uint32_t i = 0; i < other.size_; ++i) {
		new(&items_[i]) T(other.items_[i]);
	}
    return *this;
}

template<class T>
Array<T>& Array<T>::operator=(Array&& other)
{
    if(this == &other) {
        return *this;
    }
    this->~Array();
    capacity_ = other.capacity_;
	size_ = other.size_;
    items_ = other.items_;
    other.capacity_ = 0;
    other.size_ = 0;
    other.items_ = nullptr;
    return *this;
}

template<class T>
Array<T>::~Array()
{
    clear();
    CPPA_FREE(items_);
    capacity_ = 0;
}

template<class T>
inline uint32_t Array<T>::capacity() const
{
    return capacity_;
}
template<class T>
inline uint32_t Array<T>::size() const
{
    return size_;
}

template<class T>
void Array<T>::clear()
{
    for(uint32_t i = 0; i < size_; ++i) {
        items_[i].~T();
    }
    size_ = 0;
}

template<class T>
void Array<T>::add(const T& item)
{
    if(capacity_<=size_){
        expand();
    }
    items_[size_] = item;
	++size_;
}

template<class T>
void Array<T>::add(T&& item)
{
    if(capacity_<=size_){
        expand();
    }
    new(&items_[size_]) T(std::move(item));
	++size_;
}

template<class T>
template<class... Args>
void Array<T>::emplace(Args&&... args)
{
    if(capacity_ <= size_) {
        expand();
    }
    new(&items_[size_]) T(std::forward<Args>(args)...);
    ++size_;
}

template<class T>
void Array<T>::removeAt(uint32_t index)
{
    assert(index<size_);
    items_[index].~T();
    for(uint32_t i = index; i < size_-1; ++i) {
        items_[i] = std::move(items_[i + 1]);
    }
    --size_;
}

template<class T>
inline const T& Array<T>::operator[](uint32_t index) const
{
    assert(index<size_);
    return items_[index];
}

template<class T>
inline T& Array<T>::operator[](uint32_t index)
{
    assert(index<size_);
    return items_[index];
}

template<class T>
inline const T* Array<T>::begin() const
{
    return items_;
}

template<class T>
inline T* Array<T>::begin()
{
    return items_;
}

template<class T>
inline const T* Array<T>::end() const
{
    return items_ + size_;
}

template<class T>
inline T* Array<T>::end()
{
    return items_ + size_;
}

template<class T>
void Array<T>::expand()
{
    capacity_ += Expand;
    assert(size_<capacity_);
    T* items = reinterpret_cast<T*>(CPPA_MALLOC(sizeof(T) * capacity_));
    for(uint32_t i = 0; i < size_; ++i) {
		new(&items[i]) T(std::move(items_[i]));
	}
	CPPA_FREE(items_);
	items_ = items;
}
} // namespace cppagent
#endif // INC_CPPAGENT_ARRAY_H_

