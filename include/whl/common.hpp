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

#ifndef WHEEL_WHL_COMMON_HPP
#define WHEEL_WHL_COMMON_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <iostream>

namespace whl {

template<typename T>
inline void print(T arg) {
  std::cout << arg;
}

template<typename T, typename... Args>
inline void print(T arg, Args... args) {
  std::cout << arg;
  print(args...);
}

inline void println() {
  std::cout << '\n';
}

template<typename T, typename... Args>
inline void println(T arg, Args... args) {
  std::cout << arg;
  println(args...);
}

} // namespace whl

#endif // WHEEL_WHL_COMMON_HPP
