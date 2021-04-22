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
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>

#include "whl/collection.hpp"
#include "whl/type.hpp"

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
  return operation([fn](auto &&coll) {
    std::for_each(std::begin(coll), std::end(coll), fn);
    return coll;
  });
}

template<typename Fn>
constexpr inline auto tap_indexed(Fn fn) {
  return operation([fn](auto &&coll) {
    for_each_indexed(coll, fn);
    return coll;
  });
}

template<template<typename...> typename C = std::vector, typename Fn>
constexpr inline auto map(Fn fn) {
  return operation([fn](auto &&coll) {
    C<remove_cr_t<decltype(fn(*std::begin(coll)))>> result{};
    std::transform(std::begin(coll), std::end(coll), std::back_inserter(result), fn);
    return result;
  });
}

template<template<typename...> typename C = std::vector, typename Fn>
constexpr inline auto flat_map(Fn fn) {
  return operation([fn](auto &&coll) {
    C<remove_cr_t<decltype(*std::begin(fn(*std::begin(coll))))>> result{};
    std::for_each(std::begin(coll), std::end(coll), [fn, &result](auto &&it) {
      auto &&mapped = fn(it);
      std::copy(std::begin(mapped), std::end(mapped), std::back_inserter(result));
    });
    return result;
  });
}

template<template<typename...> typename C>
constexpr inline auto to() {
  return operation([](auto &&coll) {
    C<remove_cr_t<decltype(*std::begin(coll))>> result{};
    std::copy(std::begin(coll), std::end(coll), std::back_inserter(result));
    return result;
  });
}

template<template<typename...> typename R = std::vector, typename C, typename Fn>
constexpr inline auto zip(const C &other, Fn fn) {
  return operation([other, fn](auto &&coll) {
    R<remove_cr_t<decltype(fn(*std::begin(coll), *std::begin(other)))>> result{};
    auto &&x = std::begin(coll);
    auto &&y = std::begin(other);
    for (; x != std::end(coll) && y != std::end(other); ++x, ++y) {
      result.push_back(fn(*x, *y));
    }
    return result;
  });
}

template<template<typename...> typename R = std::vector, typename C>
constexpr inline auto zip(const C &other) {
  return zip<R>(other, [](auto &&x, auto &&y) { return std::make_pair(x, y); });
}

template<typename Pred>
constexpr inline auto filter(Pred pred) {
  return operation([pred](auto &&coll) {
    remove_cr_t<decltype(coll)> result{coll};
    auto &&it = std::remove_if(std::begin(result), std::end(result), [pred](auto &&it) { return !pred(it); });
    result.erase(it, std::end(result));
    return result;
  });
}

constexpr inline auto reverse() {
  return operation([](auto &&coll) {
    remove_cr_t<decltype(coll)> result{coll};
    std::reverse(std::begin(result), std::end(result));
    return result;
  });
}

constexpr inline auto sort() {
  return operation([](auto &&coll) {
    remove_cr_t<decltype(coll)> result{coll};
    std::sort(std::begin(result), std::end(result));
    return result;
  });
}

template<typename Comp>
constexpr inline auto sort(Comp comp) {
  return operation([comp](auto &&coll) {
    remove_cr_t<decltype(coll)> result{coll};
    std::sort(std::begin(result), std::end(result), comp);
    return result;
  });
}

template<typename Size>
constexpr inline auto take(Size n) {
  return operation([n](auto &&coll) {
    remove_cr_t<decltype(coll)> result{std::begin(coll), std::begin(coll) + n};
    return result;
  });
}

template<typename Size>
constexpr inline auto drop(Size n) {
  return operation([n](auto &&coll) {
    remove_cr_t<decltype(coll)> result{std::begin(coll) + n, std::end(coll)};
    return result;
  });
}

template<typename Val, typename BinOp>
constexpr inline auto fold(Val initial, BinOp op) {
  return operation([initial, op](auto &&coll) {
    return std::accumulate(std::begin(coll), std::end(coll), initial, op);
  });
}

template<typename BinOp>
constexpr inline auto reduce(BinOp op) {
  return operation([op](auto &&coll) {
    assert(std::size(coll) > 0);
    auto result = *std::begin(coll);
    return std::accumulate(++std::begin(coll), std::end(coll), result, op);
  });
}

template<typename T = std::int32_t>
constexpr inline auto sum() {
  T zero{};
  return fold(zero, func::plus);
}

template<typename F = std::double_t>
constexpr inline auto average() {
  return operation([](auto &&coll) {
    return sum<F>()(coll) / static_cast<F>(std::size(coll));
  });
}

constexpr inline auto min() {
  return operation([](auto &&coll) {
    return *std::min_element(std::begin(coll), std::end(coll));
  });
}

template<typename Comp>
constexpr inline auto min(Comp comp) {
  return operation([comp](auto &&coll) {
    return *std::min_element(std::begin(coll), std::end(coll), comp);
  });
}

constexpr inline auto max() {
  return operation([](auto &&coll) {
    return *std::max_element(std::begin(coll), std::end(coll));
  });
}

template<typename Comp>
constexpr inline auto max(Comp comp) {
  return operation([comp](auto &&coll) {
    return *std::max_element(std::begin(coll), std::end(coll), comp);
  });
}

constexpr inline auto count() {
  return operation([](auto &&coll) {
    return std::size(coll);
  });
}

template<typename Pred>
constexpr inline auto count(Pred pred) {
  return operation([pred](auto &&coll) {
    return std::count_if(std::begin(coll), std::end(coll), pred);
  });
}

template<template<typename...> typename R = std::vector, typename C>
constexpr inline auto concat(const C &other) {
  return operation([other](auto &&coll) {
    R<remove_cr_t<decltype(*std::begin(coll))>> result{coll};
    std::copy(std::begin(other), std::end(other), std::back_inserter(result));
    return result;
  });
}

template<template<typename...> typename R = std::vector, template<typename...> typename Blk = R, typename Size>
constexpr inline auto chunk(Size n) {
  assert(n > 0);
  return operation([n](auto &&coll) {
    R<Blk<remove_cr_t<decltype(*std::begin(coll))>>> result{};
    Blk<remove_cr_t<decltype(*std::begin(coll))>> block{};
    for (auto &&[i, v] : with_index(coll)) {
      block.push_back(v);
      if (i == std::size(coll) - 1) {
        result.push_back(block);
        return result;
      }
      if ((i + 1) % n == 0) {
        result.push_back(block);
        block = {};
      }
    }
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

constexpr inline auto println(std::ostream &out = std::cout) {
  return operation([&out](auto &&coll) {
    print(out)(coll);
    out << '\n';
    return coll;
  });
}

} // namespace whl::op

#endif // WHEEL_WHL_OPERATION_HPP
