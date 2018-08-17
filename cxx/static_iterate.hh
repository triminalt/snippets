//
// author : triminalt AT gmail DOT com
// date   : 2018-05-04
// desc   : static_iterate
//


#ifndef ZBB_UTIL_STATIC_ITERATE_HH
#define ZBB_UTIL_STATIC_ITERATE_HH


#include <utility>
#include "./ns.hh"


ZBB_UTIL_DETAIL_BEGIN

template<typename T, typename P, size_t I = std::tuple_size<T>::value>
struct static_iterator_impl {
    static bool iterate(T& t, P predicate) {
        static auto const index = std::tuple_size<T>::value - I;
        if (!predicate. template apply<index>(std::get<index>(t))) {
            return false;
        }
        return static_iterator_impl<T, P, I - 1>::iterate(t, predicate);
    };
    static bool iterate(T const& t, P predicate) {
        static auto const index = std::tuple_size<T>::value - I;
        if (!predicate. template apply<index>(std::get<index>(t))) {
            return false;
        }
        return static_iterator_impl<T, P, I - 1>::iterate(t, predicate);
    };
};

template<typename T, typename P>
struct static_iterator_impl<T, P, 1> {
    static bool iterate(T& t, P predicate) {
        static auto const index = std::tuple_size<T>::value - 1;
        return predicate. template apply<index>(std::get<index>(t));
    }
    static bool iterate(T const& t, P predicate) {
        static auto const index = std::tuple_size<T>::value - 1;
        return predicate. template apply<index>(std::get<index>(t));
    }
};

ZBB_UTIL_DETAIL_END

ZBB_UTIL_BEGIN

template<typename T, typename P>
static bool static_iterate(T& t, P predicate) {
    return detail::static_iterator_impl<T, P>::iterate(t, predicate);
};

template<typename T, typename P>
static bool static_iterate(T const& t, P const predicate) {
    return detail::static_iterator_impl<T, P>::iterate(t, predicate);
};

ZBB_UTIL_END


#endif // ZBB_UTIL_STATIC_ITERATE_HH
