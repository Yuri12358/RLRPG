#ifndef RLRPG_UTILS_FUNCTIONAL_HPP
#define RLRPG_UTILS_FUNCTIONAL_HPP

#include"compat/functional.hpp"

template<class Arg>
auto add(Arg&& arg) noexcept {
    return bind_front(std::plus<>{}, FORWARD(arg));
}

#endif // RLRPG_UTILS_FUNCTIONAL_HPP
