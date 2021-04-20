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

#ifndef WHEEL_WHL_OPERATION_HPP
#define WHEEL_WHL_OPERATION_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <functional>
#include <algorithm>
#include <iostream>

#include "whl/collection.hpp"

namespace whl::op {

template<typename Fn>
struct operation : Fn {
  operation() = default;

  constexpr explicit operation(Fn fn) : Fn(fn) {}

  template<typename C>
  friend constexpr auto operator|(C coll, operation<Fn> op) {
    return op(coll);
  }
};

template<typename Fn>
constexpr inline auto tap(Fn fn) {
  return operation([&fn](auto &&coll) {
    std::for_each(std::begin(coll), std::end(coll), fn);
    return coll;
  });
}

template<typename Fn>
constexpr inline auto tap_indexed(Fn fn) {
  return operation([&fn](auto &&coll) {
    for_each_indexed(coll, fn);
    return coll;
  });
}

template<typename C, typename Fn>
constexpr inline auto map(Fn fn) {
  return operation([&fn](auto &&coll) {
    C result{};
    std::transform(std::begin(coll), std::end(coll), std::back_inserter(result), fn);
    return result;
  });
}

template<typename Pred>
constexpr inline auto filter(Pred pred) {
  return operation([&pred](auto &&coll) {
    std::remove_reference_t<decltype(coll)> result{coll};
    auto &&it = std::remove_if(std::begin(result), std::end(result), [&pred](auto &&it) { return !pred(it); });
    result.erase(it, std::end(result));
    return result;
  });
}

constexpr inline auto print(std::ostream &out = std::cout) {
  return operation([&out](auto &&coll) {
    if (std::size(coll) == 0) {
      out << "[]";
      return coll;
    }
    out << '[' << *std::begin(coll);
    for (auto it = ++std::begin(coll); it != std::end(coll); ++it) {
      out << ", " << *it;
    }
    out << ']';
    return coll;
  });
}

} // namespace whl::op

#endif // WHEEL_WHL_OPERATION_HPP
