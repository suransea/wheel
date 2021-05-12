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

template<typename T, typename = void>
struct printer {
  void operator()(const T &arg) {
    std::cout << arg;
  }
};

template<typename T>
inline void print(const T &arg) {
  printer<T>()(arg);
}

template<typename T, typename... Args>
inline void print(const T &arg, const Args &...args) {
  print(arg);
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

namespace detail {

template<typename Tuple, size_t... I>
inline void print_tuple(const Tuple &tuple, std::index_sequence<I...>) {
  print('(');
  (..., print(I ? ", " : "", std::get<I>(tuple)));
  print(')');
}

template<typename Iter>
inline void print_by_iter(Iter first, Iter last) {
  if (first == last) {
    print("[]");
    return;
  }
  print('[', *first);
  for (auto it = ++first; it != last; ++it) {
    print(", ", *it);
  }
  print(']');
}

} // namespace whl::detail

template<typename T1, typename T2>
struct printer<std::pair<T1, T2>> {
  void operator()(const std::pair<T1, T2> &pair) {
    print('(', pair.first, ", ", pair.second, ')');
  }
};

template<typename... Args>
struct printer<std::tuple<Args...>> {
  void operator()(const std::tuple<Args...> &tuple) {
    detail::print_tuple(tuple, std::make_index_sequence<sizeof...(Args)>());
  }
};

template<typename T>
struct printer<T, decltype(std::begin(std::declval<T>()), void())> {
  void operator()(const T &arg) {
    detail::print_by_iter(std::begin(arg), std::end(arg));
  }
};

template<>
struct printer<std::string> {
  void operator()(const std::string &str) {
    std::cout << str;
  }
};

template<>
struct printer<const char *> {
  void operator()(const char *str) {
    std::cout << str;
  }
};

} // namespace whl

#endif // WHEEL_WHL_PRINT_HPP
