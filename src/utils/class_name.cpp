#include "utils/class_name.h"
#include <map>
#include <typeindex>

#if defined(__GNUC__)

#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#include <stdexcept>
#include <string>
#include "utils/resource_ptr.h"

namespace utils
{

// demangle name in programs compiled using compilers with GNU based ABI
static std::string get_class_name_impl(const std::type_info& id)
{
    const char* name = id.name();
    int status = 0;
    utils::resource_ptr<void, ::std::free> demangled(abi::__cxa_demangle(name, nullptr, nullptr, &status));

    if (status == 0) {
        return static_cast<char*>(demangled.get());
    }

    throw std::runtime_error(std::string{"Couldn't demangle name "} + name);
}

}

#else

namespace utils
{

// else just return the raw name, will show the demangled name when compiled with VC++
static std::string get_class_name_impl(const std::type_info& id)
{
    return id.name();
}

}

#endif // defined(__GNUC__)

namespace utils
{

const std::string& get_class_name(const std::type_info& id)
{
    thread_local std::map<std::type_index, std::string> cache;

    auto it = cache.find(id);
    if (it == cache.end()) {
        auto res = cache.emplace(id, get_class_name_impl(id));
        it = res.first;
    }

    return it->second;
}

}
