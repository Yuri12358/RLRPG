#ifndef RLRPG_COMPAT_FUNCTIONAL_HPP
#define RLRPG_COMPAT_FUNCTIONAL_HPP

#include<functional>
#define FORWARD(x) std::forward<decltype(x)>(x)  // NOLINT(cppcoreguidelines-macro-usage)

template<class Fn, class ...Args>
auto bind_front(Fn&& fn, Args&& ...boundArgs) noexcept {
    return [=] (auto&& ...rest) {
        return std::invoke(fn, boundArgs..., FORWARD(rest)...);
    };
}

#endif // RLRPG_COMPAT_FUNCTIONAL_HPP
