test-tuple_vector
=================

C++11 implementation of std::vector&lt;std::tuple> emulation internally storing tuple of vectors of Ts for better data locality

Intended usage:

    tuple_vector<std::tuple<T1, T2, T3, ...>> v;
    v.resize(count);

    for (int i = 0; i < count; ++i)
    {
        T1 t = std::get<0>(v[i]);
        std::get<0>(v[i]) = ...;
    }
    
Note that

    tuple_vector<>::operator[]

returns

    std::tuple<T1&, T2&, T3&, ...>
    
which is convertible to value_type.
