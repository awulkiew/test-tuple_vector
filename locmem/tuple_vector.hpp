// Copyright (c) 2014 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LOCMEM_TUPLE_VECTOR_HPP
#define LOCMEM_TUPLE_VECTOR_HPP

#include <vector>
#include <tuple>
#include "tuple_util.hpp"

namespace locmem {

namespace detail {

template <int I, int N>
struct for_each_container
{
    template <typename SizeType, typename Containers>
    static inline void resize(SizeType count, Containers & containers)
    {
        // TODO - revert on exception
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

} // namespace detail

template <typename Container, typename Value, typename Reference>
class indexing_iterator
{
public:
    typedef Value value_type;
    typedef Reference reference;
    typedef std::random_access_iterator_tag iterator_category;
    typedef std::ptrdiff_t difference_type; // change
    typedef int pointer;

    indexing_iterator()
    {}

    indexing_iterator(Container * c, typename Container::size_type i)
        : m_container(c), m_index(i)
    {}

    template <typename C, typename V, typename R>
    indexing_iterator(indexing_iterator<C, V, R> const& other)
        : m_container(other.m_container), m_index(other.m_index)
    {}

    reference operator*() const
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
        indexing_iterator tmp(*this);
        operator--();
        return tmp;
    }

    friend difference_type operator-(indexing_iterator const& l, indexing_iterator const& r)
    {
        return r.m_index >= l.m_index ? r.m_index - l.m_index : l.m_index - r.m_index;
    }

    friend indexing_iterator operator+(indexing_iterator const& l, difference_type r)
    {
        assert(r >= 0 || l.m_index >= static_cast<std::size_t>(-r));
        return indexing_iterator(l.m_container, l.m_index + r);
    }

    friend indexing_iterator operator-(indexing_iterator const& l, difference_type r)
    {
        assert(r >= 0 || l.m_index >= static_cast<std::size_t>(-r));
        return indexing_iterator(l.m_container, l.m_index - r);
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

private:
    Container * m_container;
    typename Container::size_type m_index;
};

template <typename Tuple, typename Allocator = std::allocator<Tuple> >
class tuple_vector {};

template <typename Allocator, typename... Ts>
class tuple_vector< std::tuple<Ts...>, Allocator >
{
public:
    typedef typename Allocator::size_type size_type;
    typedef tuple_ref<Ts&...> reference;
    typedef tuple_ref<const Ts&...> const_reference;
    typedef std::tuple<Ts...> value_type;
    typedef indexing_iterator<tuple_vector, value_type, reference> iterator;
    typedef indexing_iterator<tuple_vector const, value_type const, const_reference> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    iterator begin() { return iterator(this, 0); }
    iterator end() { return iterator(this, size()); }
    const_iterator begin() const { return iterator(this, 0); }
    const_iterator end() const { return iterator(this, size()); }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    reference operator[](size_type index)
    {
        return subscript_dispatch<reference>(
                    index,
                    typename make_integral_sequence
                        <
                            std::size_t, 0, sizeof...(Ts)
                        >::type());
    }

    const_reference operator[](size_type index) const
    {
        return subscript_dispatch<const_reference>(
                    index,
                    typename make_integral_sequence
                        <
                            std::size_t, 0, sizeof...(Ts)
                        >::type());
    }

    size_type size() const
    {
        // TODO: check other sizes
        return std::get<0>(m_containers).size();
    }

    void resize(size_type count)
    {
        detail::for_each_container<0, sizeof...(Ts)>::resize(count, m_containers);
    }

private:
    typedef std::tuple< std::vector<Ts, Allocator>... > containers_type;

    template <typename Ref, std::size_t ...Is>
    Ref subscript_dispatch(size_type index, integral_sequence<std::size_t, Is...>)
    {
        return Ref(std::get<Is>(m_containers)[index]...);
    }

    template <typename Ref, std::size_t ...Is>
    Ref subscript_dispatch(size_type index, integral_sequence<std::size_t, Is...>) const
    {
        return Ref(std::get<Is>(m_containers)[index]...);
    }

    containers_type m_containers;
};

} // namespace locmem

#endif // LOCMEM_TUPLE_VECTOR_HPP
