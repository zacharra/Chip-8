#pragma once

#include <exception>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/format.h>
#include "utils/class_name.h"

class Exception
    : public std::exception
{
public:
    explicit Exception(std::string str)
        : m_msg(std::move(str))
    {
    }

    template <class... Args>
    explicit Exception(std::string_view fmt, Args&&... args)
        : m_msg(fmt::format(fmt, std::forward<Args>(args)...))
    {
    }

    const char* what() const noexcept override
    {
        return utils::class_name_of(*this).c_str();
    }

    virtual std::string_view message() const noexcept
    {
        return m_msg;
    }

private:
    std::string m_msg;
};

class IOException
    : public Exception
{
public:
    using Exception::Exception;
};

class FileNotFoundException
    : public IOException
{
public:
    explicit FileNotFoundException(std::string filename_)
        : IOException("File \"{}\" not found", filename_),
          m_filename(std::move(filename_))
    {}

    std::string_view filename() const noexcept
    {
        return m_filename;
    }

private:
    std::string m_filename;
};
