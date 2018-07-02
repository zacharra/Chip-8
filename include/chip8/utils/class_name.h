#pragma once

#include <string>
#include <typeinfo>

namespace utils
{
    const std::string& get_class_name(const std::type_info& id);

    template <class T>
    const std::string& class_name_of()
    {
        return get_class_name(typeid(T));
    }

    template <class T>
    const std::string& class_name_of(const T& obj)
    {
        return get_class_name(typeid(obj));
    }
}
