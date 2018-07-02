#pragma once

#include <chrono>
#include <limits>
#include <random>

namespace utils
{

template <class Int = int>
class Random
{
public:
    Random()
        : m_gen(std::mt19937::result_type(std::chrono::system_clock::now().time_since_epoch().count())),
        m_dist(std::numeric_limits<Int>::min(), std::numeric_limits<Int>::max()) {}

    Int operator ()()
    {
        return m_dist(m_gen);
    }

private:
	std::mt19937 m_gen;
    std::uniform_int_distribution<Int> m_dist;
};

template <class Int = int>
Int random()
{
    thread_local Random<Int> r;
    return r();
}

}
