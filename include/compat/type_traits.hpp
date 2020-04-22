#ifndef RLRPG_COMPAT_TYPE_TRAITS_HPP
#define RLRPG_COMPAT_TYPE_TRAITS_HPP

#include<type_traits>

template<class T>
struct type_identity { using type = T; };

template<class T>
using type_identity_t = typename type_identity<T>::type;

#endif // RLRPG_COMPAT_TYPE_TRAITS_HPP
