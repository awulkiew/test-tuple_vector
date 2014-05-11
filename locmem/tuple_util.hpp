// Copyright (c) 2014 Adam Wulkiewicz, Lodz, Poland.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef LOCMEM_TUPLE_UTIL_HPP
#define LOCMEM_TUPLE_UTIL_HPP

#include <tuple>
#include <type_traits>

namespace locmem {

template <typename T, T... Is>
struct integral_sequence {};

template <typename T, T First, T Last, T... Is>
struct make_integral_sequence
    : make_integral_sequence<T, First, Last - 1, Last - 1, Is...>
{
    static_assert(First < Last, "Invalid Range");
};

template <typename T, T First, T... Is>
struct make_integral_sequence<T, First, First, Is...>
{
    typedef integral_sequence<T, Is...> type;
};

template <typename... Refs>
struct tuple_ref
{
    template <typename... Args>
    tuple_ref(Args&&... t) : refs(std::forward<Args>(t)...) {}

    template <typename... Us>
    tuple_ref(tuple_ref<Us...> & t) : refs(t.refs) {}
    template <typename... Us>
    tuple_ref(tuple_ref<Us...> const& t) : refs(t.refs) {}
    template <typename... Us>
    tuple_ref(tuple_ref<Us...> && t) : refs(std::move(t.refs)) {}

    template <typename... Us>
    tuple_ref & operator=(tuple_ref<Us...> & t) { refs = t.refs; return *this; }
    template <typename... Us>
    tuple_ref & operator=(tuple_ref<Us...> const& t) { refs = t.refs; return *this; }
    template <typename... Us>
    tuple_ref & operator=(tuple_ref<Us...> && t) { refs = std::move(t.refs); return *this; }

    template <typename... Us>
    tuple_ref(std::tuple<Us...> & t) : refs(t) {}
    template <typename... Us>
    tuple_ref(std::tuple<Us...> const& t) : refs(t) {}
    template <typename... Us>
    tuple_ref(std::tuple<Us...> && t) : refs(std::move(t)) {}

    template <typename... Us>
    tuple_ref & operator=(std::tuple<Us...> & t) { refs = t; return *this; }
    template <typename... Us>
    tuple_ref & operator=(std::tuple<Us...> const& t) { refs = t; return *this; }
    template <typename... Us>
    tuple_ref & operator=(std::tuple<Us...> && t) { refs = std::move(t); return *this; }

    template <typename... Us>
    operator std::tuple<Us...>() const
    {
        typedef typename
            locmem::make_integral_sequence
                <
                    std::size_t, 0, sizeof...(Refs)
                >::type iseq;

        return cast_impl< std::tuple<Us...> >(iseq());
    }

    template <typename Ret, std::size_t... Is>
    inline
    Ret cast_impl(integral_sequence<std::size_t, Is...>) const
    {
        return Ret(
            std::forward
                <
                    typename std::tuple_element
                        <
                            Is, std::tuple<Refs...>
                        >::type
                >(std::get<Is>(refs))...);
    }

    std::tuple<Refs...> refs;
};

} // namespace locmem

namespace std {

template <typename... Refs>
class tuple_size< locmem::tuple_ref<Refs...> >
    : public std::integral_constant<std::size_t, sizeof...(Refs)>
{};

template <std::size_t I, typename... Refs>
class tuple_element< I, locmem::tuple_ref<Refs...> >
{
public:
    typedef typename
        std::tuple_element< I, std::tuple<Refs...> >::type type;
};

} // namespace std

namespace std {

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<Ts&...> >::type &
//get(locmem::tuple_ref<Ts&...> & t)
//{
//    return std::get<I>(t.refs);
//}

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<const Ts&...> >::type const&
//get(locmem::tuple_ref<Ts const&...> & t)
//{
//    return std::get<I>(t.refs);
//}

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<Ts&&...> >::type &&
//get(locmem::tuple_ref<Ts&&...> & t)
//{
//    return std::get<I>(std::move(t.refs));
//}

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<Ts&...> >::type &
//get(locmem::tuple_ref<Ts&...> && t)
//{
//    return std::get<I>(t.refs);
//}

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<const Ts&...> >::type const&
//get(locmem::tuple_ref<Ts const&...> && t)
//{
//    return std::get<I>(t.refs);
//}

//template <std::size_t I, typename... Ts>
//inline
//typename std::tuple_element<I, locmem::tuple_ref<Ts&&...> >::type &&
//get(locmem::tuple_ref<Ts&&...> && t)
//{
//    return std::get<I>(std::move(t.refs));
//}

template <std::size_t I, typename... Refs>
inline
typename std::tuple_element<I, locmem::tuple_ref<Refs...> >::type &
get(locmem::tuple_ref<Refs...> & t)
{
    return std::get<I>(t.refs);
}

template <std::size_t I, typename... Refs>
inline
typename std::tuple_element<I, locmem::tuple_ref<Refs...> >::type const&
get(locmem::tuple_ref<Refs...> const& t)
{
    return std::get<I>(t.refs);
}

template <std::size_t I, typename... Refs>
inline
typename std::tuple_element<I, locmem::tuple_ref<Refs...> >::type &&
get(locmem::tuple_ref<Refs...> && t)
{
    return std::get<I>(std::move(t.refs));
}

} // namesapce std

namespace locmem { namespace detail {

template <typename Ret, typename Ref, std::size_t... Is>
inline
Ret move_impl(Ref ref, integral_sequence<std::size_t, Is...>)
{
    return Ret(std::move(std::get<Is>(ref))...);
}

}} // namespace locmem::detail

namespace std {

template <typename... Ts>
inline
locmem::tuple_ref<Ts&&...> move(locmem::tuple_ref<Ts&...> t)
{
    typedef locmem::tuple_ref<Ts&&...> ret_t;
    typedef typename
        locmem::make_integral_sequence
            <
                std::size_t, 0, sizeof...(Ts)
            >::type iseq;

    return locmem::detail::move_impl<ret_t>(t, iseq());
}

} // namespace std

namespace locmem { namespace detail {

template <int I, int N>
struct swap_ref_n
{
    template <typename TRef>
    static inline void apply(TRef l, TRef r)
    {
        std::swap(std::get<I>(l), std::get<I>(r));
        swap_ref_n<I+1,N>::apply(l, r);
    }
};

template <int N>
struct swap_ref_n<N, N>
{
    template <typename TRef>
    static inline void apply(TRef, TRef) {}
};

}} // namespace locmem::detail

namespace std {

template <typename... Ts>
inline
void swap(locmem::tuple_ref<Ts&...> l, locmem::tuple_ref<Ts&...> r)
{
    locmem::detail::swap_ref_n<0, sizeof...(Ts)>::apply(l, r);
}

} // namespace std

#endif // LOCMEM_TUPLE_UTIL_HPP
