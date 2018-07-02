#pragma once

#include <memory>

namespace utils
{

template <class T, void(*D)(T*)>
struct Deleter
{
    void operator()(T* val) const noexcept
    {
        D(val);
    }
};

template <class T, void(*D)(T*)> 
using resource_ptr = std::unique_ptr<T, Deleter<T, D>>;

}
