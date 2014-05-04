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

The results of this test/benchmark gathered on VS2013 /O2 Win7 Intel i7 870 (Lynnfield):

    count: 10000000
    
    tuple_vector<std::tuple<>> iteration (access one +): 0.0300017
    tuple_vector<std::tuple<>> iteration (access one /): 0.0410023
    tuple_vector<std::tuple<>> iteration (access one sqrt): 0.0640037
    tuple_vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.101006
    tuple_vector<std::tuple<>> iteration (access all): 0.162009
    tuple_vector<std::tuple<>> sort (access one, copy all): 1.06806
    
    std::vector<std::tuple<>> iteration (access one +): 0.158009
    std::vector<std::tuple<>> iteration (access one /): 0.160009
    std::vector<std::tuple<>> iteration (access one sqrt): 0.159009
    std::vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.160009
    std::vector<std::tuple<>> iteration (access all): 0.162009
    std::vector<std::tuple<>> sort (access one, copy all): 1.60809
    
The data locality and access pattern is extremely important nowadays. Note that in the case of std::vector<std::tuple> it's not important how many operations are performed (to some extent) because the CPU is mainly waiting for the next chunk of data.
