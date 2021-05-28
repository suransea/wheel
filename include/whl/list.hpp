//
// Copyright 2021 sea
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef WHEEL_WHL_LIST_HPP
#define WHEEL_WHL_LIST_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <type_traits>

#include "whl/cons.hpp"

namespace whl::list {

struct nil_t {};

constexpr inline auto nil = nil_t{};

template<typename T, typename... Args>
constexpr inline auto list(const T &arg, const Args &...args) {
  if constexpr (sizeof...(args) == 0) {
    return cons(arg, nil);
  } else {
    return cons(arg, list(args...));
  }
}

template<typename List>
constexpr inline auto first(const List &l) {
  return car(l);
}

template<typename List>
constexpr inline auto rest(const List &l) {
  return cdr(l);
}

template<std::size_t N, typename Cons>
constexpr inline auto nth(const Cons &c) {
  if constexpr (N == 0) {
    return car(c);
  } else {
    return nth<N - 1>(cdr(c));
  }
}

template<std::size_t N, typename Cons>
constexpr inline auto nthcdr(const Cons &c) {
  if constexpr (N == 0) {
    return c;
  } else {
    return nthcdr<N - 1>(cdr(c));
  }
}

namespace detail {

template<typename T>
constexpr inline auto is_nil = std::is_same_v<T, nil_t>;

}

template<typename L1, typename L2>
constexpr inline auto append(const L1 &l1, const L2 &l2) {
  if constexpr (detail::is_nil<L1>) {
    return l2;
  } else {
    return cons(car(l1), append(cdr(l1), l2));
  }
}

template<typename List>
constexpr inline auto length(const List &l) {
  if constexpr (detail::is_nil<List>) {
    return 0;
  } else {
    return 1 + length(cdr(l));
  }
}

template<typename List, typename Fn>
inline void foreach(const List &l, Fn fn) {
  if constexpr (!detail::is_nil<List>) {
    fn(car(l));
    foreach(cdr(l), fn);
  }
}

} // namespace whl::list

#endif // WHEEL_WHL_LIST_HPP
