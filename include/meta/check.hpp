#ifndef RLRPG_META_CHECK_HPP
#define RLRPG_META_CHECK_HPP

#include<type_traits>

namespace meta {
    template<bool Cond>
    using Check = std::enable_if_t<Cond, int>;
};

#define REQUIRES(...) meta::Check<__VA_ARGS__> = 0

#endif // RLRPG_META_CHECK_HPP
