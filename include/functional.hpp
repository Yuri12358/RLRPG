#ifndef RLRPG_FUNCTIONAL_HPP
#define RLRPG_FUNCTIONAL_HPP

#include<utility>
#include<type_traits>

template<class Fn1, class Fn2>
auto compose(Fn1&& fn1, Fn2&& fn2) {
    return [fn1 = std::forward<Fn1>(fn1), fn2 = std::forward<Fn2>(fn2)] (auto&&... xs) -> decltype(auto) {
        static_assert(std::is_invocable_v<Fn2, decltype(xs)&&...>, "right of the composed functions is not appliable to the args");
        static_assert(std::is_invocable_v<Fn1, decltype(std::invoke(fn2, std::forward<decltype(xs)>(xs)...))>, "left of the composed functions is not appliable to the result of the right one");
        return std::invoke(fn1, std::invoke(fn2, std::forward<decltype(xs)>(xs)...));
    };
}

inline auto const deref = [] (auto&& ptr) -> decltype(*ptr) {
    return *ptr;
};

template<class Func, class Proj>
auto applyProjection(Func&& func, Proj&& proj) {
    return [func = std::forward<Func>(func), proj = std::forward<Proj>(proj)] (auto&&... xs) -> decltype(auto) {
        return std::invoke(func, std::invoke(proj, std::forward<decltype(xs)>(xs))...);
    };
}

template<class T>
auto alwaysReturn(T&& t) {
    return [t = std::forward<T>(t)] (auto&&...) {
        return t;
    };
}

#endif // RLRPG_FUNCTIONAL_HPP