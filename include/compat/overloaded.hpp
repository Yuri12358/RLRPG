#ifndef RLRPG_UTILS_OVERLOADED_HPP
#define RLRPG_UTILS_OVERLOADED_HPP

template<class... Ts>
struct overloaded: Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

#endif // RLRPG_UTILS_OVERLOADED_HPP

