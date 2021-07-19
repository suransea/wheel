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

#ifndef WHEEL_WHL_CONS_HPP
#define WHEEL_WHL_CONS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <type_traits>
#include <utility>

namespace whl {

namespace tuple {

template<typename Car, typename Cdr>
constexpr inline auto cons(const Car &car, const Cdr &cdr) {
  return std::pair(car, cdr);
}

template<typename Pair>
constexpr inline auto car(const Pair &p) {
  return std::get<0>(p);
}

template<typename Pair>
constexpr inline auto cdr(const Pair &p) {
  return std::get<1>(p);
}

struct unit_t {};

constexpr inline unit_t unit = {};

constexpr inline unit_t tuple() { return unit; }

template<typename T, typename... Args>
constexpr inline auto tuple(const T &arg, const Args &...args) {
  return cons(arg, tuple(args...));
}

template<std::size_t N, typename Tuple>
constexpr inline auto nth(const Tuple &t) {
  if constexpr (N == 0) {
    return car(t);
  } else {
    return nth<N - 1>(cdr(t));
  }
}

template<std::size_t N, typename Tuple>
constexpr inline auto nthcdr(const Tuple &t) {
  if constexpr (N == 0) {
    return t;
  } else {
    return nthcdr<N - 1>(cdr(t));
  }
}

template<typename Tuple>
constexpr inline auto append(unit_t, const Tuple &t) { return t; }

template<typename T1, typename T2>
constexpr inline auto append(const T1 &t1, const T2 &t2) {
  return cons(car(t1), append(cdr(t1), t2));
}

constexpr inline auto length(unit_t) { return 0; }

template<typename Tuple>
constexpr inline auto length(const Tuple &t) {
  return 1 + length(cdr(t));
}

constexpr inline bool null(unit_t) { return true; }

template<typename Tuple>
constexpr inline bool null(Tuple) { return false; }

} // namespace tuple

namespace list {

template<typename T>
struct cons_t : std::pair<T, std::shared_ptr<cons_t<T>>> {

  template<typename Ptr>
  cons_t(const T &arg, Ptr ptr)
      : std::pair<T, std::shared_ptr<cons_t<T>>>(arg, ptr) {}
};

template<typename Car, typename Cdr>
inline auto cons(const Car &car, const Cdr &cdr) {
  return std::make_shared<cons_t<Car>>(car, cdr);
}

template<typename Pair>
inline auto car(const Pair &p) {
  return std::get<0>(*p);
}

template<typename Pair>
inline auto cdr(const Pair &p) {
  return std::get<1>(*p);
}

constexpr inline std::nullptr_t nil = nullptr;

constexpr inline std::nullptr_t list() { return nil; }

template<typename T, typename... Args>
constexpr inline auto list(const T &arg, const Args &...args) {
  return cons(arg, list(args...));
}

template<typename List>
constexpr inline auto nth(std::size_t n, const List &l) {
  if (n == 0) return car(l);
  return nth(--n, cdr(l));
}

template<typename List>
constexpr inline auto nthcdr(std::size_t n, const List &l) {
  if (n == 0) return l;
  return nthcdr(--n, cdr(l));
}

template<typename L1, typename L2>
constexpr inline auto append(const L1 &l1, const L2 &l2) {
  if (!l1) return l2;
  return cons(car(l1), append(cdr(l1), l2));
}

template<typename List>
constexpr inline std::size_t length(const List &l) {
  return l ? 1 + length(cdr(l)) : 0;
}

template<typename List>
constexpr inline bool null(const List &l) { return l == nil; }

} // namespace list

} // namespace whl

#endif // WHEEL_WHL_CONS_HPP
