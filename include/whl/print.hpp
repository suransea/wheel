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

#include <iostream>
#include <type_traits>

#include "whl/format.hpp"
#include "whl/type.hpp"

namespace whl {

template<typename T, typename... Args>
inline void print(const T &arg, const Args &...args) {
  detail::out_to(std::cout, arg, args...);
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

template<typename Fmt, typename... Args>
inline void printf(const Fmt &fmt, const Args &...args) {
  detail::format_to(std::cout, std::begin(fmt), std::end(fmt), args...);
}

} // namespace whl

#endif // WHEEL_WHL_PRINT_HPP
