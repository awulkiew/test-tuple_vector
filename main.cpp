// Copyright (c) 2014 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <algorithm>

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <cassert>

#include <memory>

#include "locmem/tuple_vector.hpp"

struct greater
{
    template <typename L, typename R>
    inline bool operator()(L const& l, R const& r) const
    {
        return std::get<0>(l) > std::get<0>(r);
    }
};

struct A
{
    float arr[4];
};

typedef std::tuple<float, A, double, int, std::string> T;

template <typename V>
inline void test(std::string const& name)
{
    std::ifstream file("asdas");
    std::chrono::time_point<std::chrono::system_clock> start, end;

    static const int count = 10000000;

    std::cout << "count: " << count << '\n';

    V v;
    v.resize(count);

    file >> std::get<0>(v[0]);

    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = std::get<0>(v[i - 1]) + 0.01f;
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one +): " << s.count() << "s\n";
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = std::get<0>(v[i - 1]) / 3;
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one /): " << s.count() << "s\n";
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = ::sqrt(std::get<0>(v[i - 1]));
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one sqrt): " << s.count() << "s\n";
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = ::sqrt(::sqrt(std::get<0>(v[i - 1])));
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one sqrt of sqrt): " << s.count() << "s\n";
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
        {
            std::get<0>(v[i]) = std::get<0>(v[i - 1]) + 0.01f;
            std::get<1>(v[i]).arr[0] = std::get<0>(v[i]);
            std::get<2>(v[i]) = std::get<1>(v[i]).arr[0];
            std::get<3>(v[i]) = int(std::get<2>(v[i]));
            std::get<4>(v[i]) = char('0' + std::get<3>(v[i]));
        }
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access all): " << s.count() << "s\n";
    }

    {
        start = std::chrono::system_clock::now();
        std::sort(v.begin(), v.end(), greater());
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " sort (access one, copy all): " << s.count() << "s\n";
    }
    std::cout << "dummy: " << std::get<0>(v[count - 1]) << std::get<1>(v[count - 1]).arr[0] << std::get<2>(v[count - 1]) << '\n';
}

int main()
{
//    typedef std::unique_ptr<int> P;
//    P a(new int(10));
//    std::cout << 1 << std::endl;
//    std::cout << a.get() << std::endl;

//    std::tuple<P> t(std::move(a));
//    std::cout << 2 << std::endl;
//    std::cout << a.get() << std::endl;
//    std::cout << std::get<0>(t).get() << std::endl;

//    locmem::tuple_ref<P&&> tt(std::move(t));
//    std::cout << 3 << std::endl;
//    std::cout << a.get() << std::endl;
//    std::cout << std::get<0>(t).get() << std::endl;
//    std::cout << std::get<0>(tt).get() << std::endl;

//    std::tuple<P> t2(std::move(tt));
//    std::cout << 4 << std::endl;
//    std::cout << a.get() << std::endl;
//    std::cout << std::get<0>(t).get() << std::endl;
//    std::cout << std::get<0>(tt).get() << std::endl;
//    std::cout << std::get<0>(t2).get() << std::endl;

    test< locmem::tuple_vector<T> >("tuple_vector<std::tuple<>>");
    test< std::vector<T> >("std::vector<std::tuple<>>");
}
