#include <vector>
#include <tuple>
#include <algorithm>

#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <cassert>

template <int ...>
struct seq {};

template <int N, int ...S>
struct gens : gens<N - 1, N - 1, S...> { };

template <int ...S>
struct gens<0, S...> {
    typedef seq<S...> type;
};

template <int I, int N>
struct for_each_container
{
    template <typename SizeType, typename Containers>
    static inline void resize(SizeType count, Containers & containers)
    {
        std::get<I>(containers).resize(count);
        for_each_container<I + 1, N>::resize(count, containers);
    }
};

template <int N>
struct for_each_container<N, N>
{
    template <typename SizeType, typename Containers>
    static inline void resize(SizeType , Containers & ) {}
};

template <typename Allocator, typename Tuple>
struct tuple_vector_members
{};

template <typename Allocator, typename... Ts>
struct tuple_vector_members<Allocator, std::tuple<Ts...> >
{
    typedef typename Allocator::size_type size_type;
    typedef std::tuple<Ts&...> reference;
    typedef std::tuple<Ts...> value_type;

    reference operator[](size_type index)
    {
        return subscript_dispatch(index, typename gens<sizeof...(Ts)>::type());
    }

    void resize(size_type count)
    {
        // TODO - revert on exception
        for_each_container<0, sizeof...(Ts)>::resize(count, m_containers);
    }

private:
    typedef std::tuple< std::vector<Ts, Allocator>... > containers_type;

    template <int ...S>
    reference subscript_dispatch(size_type index, seq<S...>)
    {
        return reference(std::get<S>(m_containers)[index]...);
    }

    containers_type m_containers;
};

template <typename Tuple, typename Allocator = std::allocator<Tuple> >
class tuple_vector
    : private tuple_vector_members<Allocator, Tuple>
{
    typedef tuple_vector_members<Allocator, Tuple> base_t;

public:
    typedef typename base_t::size_type size_type;
    typedef typename base_t::reference reference;
    typedef typename base_t::value_type value_type;
    
    reference operator[](size_type index)
    {
        return base_t::operator[](index);
    }

    void resize(size_type count)
    {
        return base_t::resize(count);
    }
};

template <typename Container>
class indexing_iterator
{
public:
    typedef typename Container::value_type value_type;
    typedef typename Container::reference reference;
    typedef std::random_access_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type; // change
    typedef int pointer;

    indexing_iterator(Container & c, typename Container::size_type i)
        : m_container(&c), m_index(i)
    {}

    reference operator*()
    {
        return m_container->operator[](m_index);
    }

    indexing_iterator & operator++()
    {
        ++m_index;
        return *this;
    }

    indexing_iterator operator++(int)
    {
        indexing_iterator tmp(*this);
        operator++();
        return tmp;
    }

    indexing_iterator & operator--()
    {
        --m_index;
        return *this;
    }

    indexing_iterator operator--(int)
    {
        tuple_vector_iterator tmp(*this);
        operator--();
        return tmp;
    }

    difference_type operator-(indexing_iterator const& r) const
    {
        return r.m_index >= m_index ? r.m_index - m_index : m_index - r.m_index;
    }

    indexing_iterator operator+(difference_type d) const
    {
        return indexing_iterator(*m_container, m_index + d);
    }

    indexing_iterator operator-(difference_type d) const
    {
        return indexing_iterator(*m_container, m_index - d);
    }

    friend bool operator==(indexing_iterator const& l, indexing_iterator const& r)
    {
        assert(l.m_container == r.m_container);
        return l.m_index == r.m_index;
    }

    friend bool operator!=(indexing_iterator const& l, indexing_iterator const& r)
    {
        assert(l.m_container == r.m_container);
        return l.m_index != r.m_index;
    }

    friend bool operator<(indexing_iterator const& l, indexing_iterator const& r)
    {
        assert(l.m_container == r.m_container);
        return l.m_index < r.m_index;
    }

    Container * m_container;
    typename Container::size_type m_index;
};

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

        std::cout << name << " iteration (access one +): " << s.count() << '\n';
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = std::get<0>(v[i - 1]) / 3;
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one /): " << s.count() << '\n';
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = ::sqrt(std::get<0>(v[i - 1]));
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one sqrt): " << s.count() << '\n';
    }
    {
        start = std::chrono::system_clock::now();
        for (int i = 1; i < count; ++i)
            std::get<0>(v[i]) = ::sqrt(::sqrt(std::get<0>(v[i - 1])));
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " iteration (access one sqrt of sqrt): " << s.count() << '\n';
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

        std::cout << name << " iteration (access all): " << s.count() << '\n';
    }
    {
        start = std::chrono::system_clock::now();
        typedef indexing_iterator<V> I;
        std::sort(I(v, 0), I(v, count), greater());
        end = std::chrono::system_clock::now();

        std::chrono::duration<double> s = end - start;

        std::cout << name << " sort (access one, copy all): " << s.count() << '\n';
    }
    std::cout << "dummy: " << std::get<0>(v[count - 1]) << std::get<1>(v[count - 1]).arr[0] << std::get<2>(v[count - 1]) << '\n';
}

int main()
{
    test< tuple_vector<T> >("tuple_vector<std::tuple<>>");
    test< std::vector<T> >("std::vector<std::tuple<>>");
}