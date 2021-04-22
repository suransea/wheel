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

#ifndef WHEEL_WHL_TYPE_HPP
#define WHEEL_WHL_TYPE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <type_traits>

namespace whl {

template<typename...>
struct replace_first {};

template<typename To, template<typename...> typename C, typename Orig, typename... Args>
struct replace_first<To, C<Orig, Args...>> {
  using type = C<To, Args...>;
};

template<typename... Args>
using replace_first_t = typename replace_first<Args...>::type;

template<typename T>
using remove_cr_t = std::remove_const_t<std::remove_reference_t<T>>;

} // namespace whl

#endif // WHEEL_WHL_TYPE_HPP
