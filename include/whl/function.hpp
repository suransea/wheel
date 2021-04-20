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

#ifndef WHEEL_WHL_FUNCTION_HPP
#define WHEEL_WHL_FUNCTION_HPP

#include <functional>

namespace whl::func {

template<typename T>
using consumer = std::function<void(T)>;

using action = std::function<void(void)>;

template<typename T, typename U>
using bi_consumer = std::function<void(T, U)>;

template<typename T>
using supplier = std::function<T(void)>;

template<typename T, typename R>
using function = std::function<R(T)>;

template<typename T, typename U, typename R>
using bi_function = std::function<R(T, U)>;

template<typename T>
using predicate = std::function<bool(T)>;

template<typename T, typename U>
using bi_predicate = std::function<bool(T, U)>;

constexpr inline auto identity = [](auto &&x) { return x; };

constexpr inline auto always_true = [](auto &&x) { return true; };

constexpr inline auto always_false = [](auto &&x) { return false; };

constexpr inline auto empty_consumer = [](auto &&x) {};

constexpr inline auto do_nothing = []() {};

template<typename Val>
constexpr inline auto supply(Val val) { return [val]() { return val; }; }

} // namespace whl

#endif // WHEEL_WHL_FUNCTION_HPP
