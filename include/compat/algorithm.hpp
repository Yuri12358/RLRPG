#ifndef RLRPG_COMPAT_ALGORITHM_HPP
#define RLRPG_COMPAT_ALGORITHM_HPP

#include<algorithm>
#include<utility>

namespace compat {
    using std::begin;
    using std::end;

    #define TRAILING_DECLTYPE_AND_BODY(...) \
    -> decltype(__VA_ARGS__) { \
        return __VA_ARGS__; \
    }

    template<class Cont, class Comparator>
    auto sort(Cont& cont, Comparator&& cmp)
    TRAILING_DECLTYPE_AND_BODY(std::sort(begin(cont), end(cont), std::forward<Comparator>(cmp)))

    template<class Cont, class Predicate>
    auto find_if(Cont& cont, Predicate&& pred)
    TRAILING_DECLTYPE_AND_BODY(std::find_if(begin(cont), end(cont), std::forward<Predicate>(pred)))

    template<class Cont, class Predicate>
    auto any_of(Cont const& cont, Predicate&& pred)
    TRAILING_DECLTYPE_AND_BODY(std::any_of(begin(cont), end(cont), std::forward<Predicate>(pred)))

    template<class Cont, class OutIt, class Predicate>
    auto copy_if(Cont const& cont, OutIt out, Predicate&& pred)
    TRAILING_DECLTYPE_AND_BODY(std::copy_if(begin(cont), end(cont), out, std::forward<Predicate>(pred)))

    #undef TRAILING_DECLTYPE_AND_BODY
}

#endif // RLRPG_COMPAT_ALGORITHM_HPP