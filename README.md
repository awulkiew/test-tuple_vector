test-tuple_vector
=================

C++11 implementation of a container emulating std::vector&lt;std::tuple>, internally storing tuple of vectors of Ts for better locality of data of the same type.

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

The data locality and access pattern is extremely important nowadays. In the case of std::vector&lt;std::tuple> it's not important how many operations are performed (to some extent) because the CPU is mainly waiting for the next chunk of data.

The example results of this test/benchmark for:

    count: 10000000

Win7 Intel i7-870 VS2013 /O2:
    
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
    
Linux Mint 15 Intel i5-3320M clang-3.2.1

    tuple_vector<std::tuple<>> iteration (access one +): 0.00959725
    tuple_vector<std::tuple<>> iteration (access one /): 0.0312704
    tuple_vector<std::tuple<>> iteration (access one sqrt): 0.0315015
    tuple_vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.0817646
    tuple_vector<std::tuple<>> iteration (access all): 0.485079
    tuple_vector<std::tuple<>> sort (access one, copy all): 5.84472

    std::vector<std::tuple<>> iteration (access one +): 0.0907764
    std::vector<std::tuple<>> iteration (access one /): 0.0938311
    std::vector<std::tuple<>> iteration (access one sqrt): 0.0937849
    std::vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.0955459
    std::vector<std::tuple<>> iteration (access all): 0.480064
    std::vector<std::tuple<>> sort (access one, copy all): 4.35685

Linux Mint 15 Intel i5-3320M gcc-4.7.3

    tuple_vector<std::tuple<>> iteration (access one +): 0.0277802
    tuple_vector<std::tuple<>> iteration (access one /): 0.0505362
    tuple_vector<std::tuple<>> iteration (access one sqrt): 0.0501095
    tuple_vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.101964
    tuple_vector<std::tuple<>> iteration (access all): 0.486031
    tuple_vector<std::tuple<>> sort (access one, copy all): 4.85451
    
    std::vector<std::tuple<>> iteration (access one +): 0.090579
    std::vector<std::tuple<>> iteration (access one /): 0.0940711
    std::vector<std::tuple<>> iteration (access one sqrt): 0.0938656
    std::vector<std::tuple<>> iteration (access one sqrt of sqrt): 0.107976
    std::vector<std::tuple<>> iteration (access all): 0.488348
    std::vector<std::tuple<>> sort (access one, copy all): 4.25615
