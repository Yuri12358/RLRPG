#ifndef RLRPG_COMPAT_FUNCTIONAL
#define RLRPG_COMPAT_FUNCTIONAL

#include<functional>

auto bind_front = [] (auto&& fn, auto&&... args) {
    return [=] (auto&&... rest) {
        return std::invoke(fn, args..., std::forward<decltype(rest)>(rest)...);
    };
};

#endif // RLRPG_COMPAT_FUNCTIONAL

