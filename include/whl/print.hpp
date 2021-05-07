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

#ifndef WHEEL_WHL_PRINT_HPP
#define WHEEL_WHL_PRINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <iostream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace whl {

namespace detail {

template<typename T>
inline void print_impl(const T &arg) {
  std::cout << arg;
}

template<typename Tuple, std::size_t... I>
inline void print_tuple(const Tuple &tuple, std::index_sequence<I...>);

template<typename Iter>
inline void print_by_iter(Iter first, Iter last);

} // namespace detail

template<typename... Args>
inline void print(const Args &...args);

template<typename T>
inline auto print(const T &arg) -> decltype(std::cout << arg, void()) {
  detail::print_impl(arg);
}

template<typename T1, typename T2>
inline void print(const std::pair<T1, T2> &pair) {
  print('(', pair.first, ", ", pair.second, ')');
}

template<typename... Args>
inline void print(const std::tuple<Args...> &tuple) {
  detail::print_tuple(tuple, std::make_index_sequence<sizeof...(Args)>());
}

template<typename T>
inline auto print(const T &arg) -> decltype(std::begin(arg), void()) {
  detail::print_by_iter(std::begin(arg), std::end(arg));
}

inline void print(const std::string &str) {
  detail::print_impl(str);
}

inline void print(const char *str) {
  detail::print_impl(str);
}

template<typename... Args>
inline void print(const Args &...args) {
  (..., print(args));
}

template<typename... Args>
inline void println(const Args &...args) {
  print(args...);
  println();
}

template<>
inline void println() {
  print('\n');
}

template<typename Tuple, size_t... I>
inline void detail::print_tuple(const Tuple &tuple, std::index_sequence<I...>) {
  print_impl('(');
  (..., print(I ? ", " : "", std::get<I>(tuple)));
  print_impl(')');
}

template<typename Iter>
inline void detail::print_by_iter(Iter first, Iter last) {
  if (first == last) {
    print_impl("[]");
    return;
  }
  print('[', *first);
  for (auto it = ++first; it != last; ++it) {
    print(", ", *it);
  }
  print_impl(']');
}

} // namespace whl

#endif // WHEEL_WHL_PRINT_HPP
