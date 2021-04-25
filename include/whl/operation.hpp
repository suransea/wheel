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
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <string>
#include <tuple>
#include <utility>

#include "whl/container.hpp"
#include "whl/print.hpp"
#include "whl/string.hpp"
#include "whl/type.hpp"

namespace whl::op {

template<typename Fn>
struct operation : Fn {
  operation() = default;

  constexpr explicit operation(Fn fn) : Fn(fn) {}

  template<typename T>
  friend constexpr auto operator|(T val, operation<Fn> op) {
    return op(val);
  }
};

template<typename Fn>
constexpr inline auto tap(Fn fn) {
  return operation([fn](auto &&cont) {
    std::for_each(std::begin(cont), std::end(cont), fn);
    return cont;
  });
}

template<typename Fn>
constexpr inline auto tap_indexed(Fn fn) {
  return operation([fn](auto &&cont) {
    for_each_indexed(cont, fn);
    return cont;
  });
}

template<template<typename...> typename C = std::vector, typename Fn>
constexpr inline auto map(Fn fn) {
  return operation([fn](auto &&cont) {
    C<remove_cr_t<decltype(fn(*std::begin(cont)))>> result{};
    std::transform(std::begin(cont), std::end(cont), std::back_inserter(result), fn);
    return result;
  });
}

template<template<typename...> typename C = std::vector, typename Fn>
constexpr inline auto flat_map(Fn fn) {
  return operation([fn](auto &&cont) {
    C<remove_cr_t<decltype(*std::begin(fn(*std::begin(cont))))>> result{};
    std::for_each(std::begin(cont), std::end(cont), [fn, &result](auto &&it) {
      auto &&mapped = fn(it);
      std::copy(std::begin(mapped), std::end(mapped), std::back_inserter(result));
    });
    return result;
  });
}

template<template<typename...> typename C = std::vector>
constexpr inline auto flatten() {
  return flat_map<C>(func::identity);
}

template<template<typename...> typename C>
constexpr inline auto to() {
  return operation([](auto &&cont) {
    C<remove_cr_t<decltype(*std::begin(cont))>> result{};
    std::copy(std::begin(cont), std::end(cont), std::back_inserter(result));
    return result;
  });
}

template<template<typename...> typename R = std::vector, typename C, typename Fn>
constexpr inline auto zip(const C &other, Fn fn) {
  return operation([other, fn](auto &&cont) {
    R<remove_cr_t<decltype(fn(*std::begin(cont), *std::begin(other)))>> result{};
    auto &&x = std::begin(cont);
    auto &&y = std::begin(other);
    for (; x != std::end(cont) && y != std::end(other); ++x, ++y) {
      result.push_back(fn(*x, *y));
    }
    return result;
  });
}

template<template<typename...> typename R = std::vector, typename C>
constexpr inline auto zip(const C &other) {
  return zip<R>(other, [](auto &&x, auto &&y) { return std::make_pair(x, y); });
}

template<template<typename...> typename R1 = std::vector, template<typename...> typename R2 = R1, typename Fn>
constexpr inline auto unzip(Fn fn) {
  return operation([fn](auto &&cont) {
    R1<remove_cr_t<decltype(std::get<0>(fn(*std::begin(cont))))>> x{};
    R2<remove_cr_t<decltype(std::get<1>(fn(*std::begin(cont))))>> y{};
    for (auto &&it : cont) {
      auto &&mapped = fn(it);
      x.push_back(std::get<0>(mapped));
      y.push_back(std::get<1>(mapped));
    }
    return std::make_pair(x, y);
  });
}

template<template<typename...> typename R1 = std::vector, template<typename...> typename R2 = R1>
constexpr inline auto unzip() {
  return unzip<R1, R2>(func::identity);
}

template<typename Pred>
constexpr inline auto filter(Pred pred) {
  return operation([pred](auto &&cont) {
    remove_cr_t<decltype(cont)> result{cont};
    auto &&it = std::remove_if(std::begin(result), std::end(result), [pred](auto &&it) { return !pred(it); });
    result.erase(it, std::end(result));
    return result;
  });
}

template<typename Eq>
constexpr inline auto distinct(Eq eq) {
  return operation([eq](auto &&cont) {
    remove_cr_t<decltype(cont)> result{};
    std::set<remove_cr_t<decltype(*std::begin(cont))>> set{};
    for (auto &&it : cont) {
      if (set.insert(it).second) {
        result.push_back(it);
      }
    }
    return result;
  });
}

constexpr inline auto distinct() {
  return distinct(func::equal);
}

constexpr inline auto reverse() {
  return operation([](auto &&cont) {
    remove_cr_t<decltype(cont)> result{cont};
    std::reverse(std::begin(result), std::end(result));
    return result;
  });
}

constexpr inline auto sort() {
  return operation([](auto &&cont) {
    remove_cr_t<decltype(cont)> result{cont};
    std::sort(std::begin(result), std::end(result));
    return result;
  });
}

template<typename Comp>
constexpr inline auto sort(Comp comp) {
  return operation([comp](auto &&cont) {
    remove_cr_t<decltype(cont)> result{cont};
    std::sort(std::begin(result), std::end(result), comp);
    return result;
  });
}

constexpr inline auto shuffle() {
  return operation([](auto &&cont) {
    remove_cr_t<decltype(cont)> result{cont};
    auto &&time = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(result), std::end(result), std::default_random_engine(time));
    return result;
  });
}

template<typename Size>
constexpr inline auto take(Size n) {
  return operation([n](auto &&cont) {
    remove_cr_t<decltype(cont)> result{std::begin(cont), std::begin(cont) + n};
    return result;
  });
}

template<typename Size>
constexpr inline auto drop(Size n) {
  return operation([n](auto &&cont) {
    remove_cr_t<decltype(cont)> result{std::begin(cont) + n, std::end(cont)};
    return result;
  });
}

template<typename Val, typename BinOp>
constexpr inline auto fold(Val initial, BinOp op) {
  return operation([initial, op](auto &&cont) {
    return std::accumulate(std::begin(cont), std::end(cont), initial, op);
  });
}

template<typename BinOp>
constexpr inline auto reduce(BinOp op) {
  return operation([op](auto &&cont) {
    assert(std::size(cont) > 0);
    auto result = *std::begin(cont);
    return std::accumulate(++std::begin(cont), std::end(cont), result, op);
  });
}

template<typename R = std::string, typename Dlm>
constexpr inline auto join(const Dlm &delimiter) {
  return operation([delimiter](auto &&cont) {
    return str::join<R>(cont, delimiter);
  });
}

template<typename T = std::int32_t>
constexpr inline auto sum() {
  T zero{};
  return fold(zero, func::plus);
}

template<typename F = std::double_t>
constexpr inline auto average() {
  return operation([](auto &&cont) {
    return sum<F>()(cont) / static_cast<F>(std::size(cont));
  });
}

constexpr inline auto min() {
  return operation([](auto &&cont) {
    return *std::min_element(std::begin(cont), std::end(cont));
  });
}

template<typename Comp>
constexpr inline auto min(Comp comp) {
  return operation([comp](auto &&cont) {
    return *std::min_element(std::begin(cont), std::end(cont), comp);
  });
}

constexpr inline auto max() {
  return operation([](auto &&cont) {
    return *std::max_element(std::begin(cont), std::end(cont));
  });
}

template<typename Comp>
constexpr inline auto max(Comp comp) {
  return operation([comp](auto &&cont) {
    return *std::max_element(std::begin(cont), std::end(cont), comp);
  });
}

constexpr inline auto count() {
  return operation([](auto &&cont) {
    return std::size(cont);
  });
}

template<typename Pred>
constexpr inline auto count(Pred pred) {
  return operation([pred](auto &&cont) {
    return std::count_if(std::begin(cont), std::end(cont), pred);
  });
}

template<typename Pred>
constexpr inline auto all(Pred pred) {
  return operation([pred](auto &&cont) {
    return std::all_of(std::begin(cont), std::end(cont), pred);
  });
}

template<typename Pred>
constexpr inline auto any(Pred pred) {
  return operation([pred](auto &&cont) {
    return std::any_of(std::begin(cont), std::end(cont), pred);
  });
}

template<typename Pred>
constexpr inline auto none(Pred pred) {
  return operation([pred](auto &&cont) {
    return std::none_of(std::begin(cont), std::end(cont), pred);
  });
}

template<template<typename...> typename R = std::vector, typename C>
constexpr inline auto concat(const C &other) {
  return operation([other](auto &&cont) {
    R<remove_cr_t<decltype(*std::begin(cont))>> result{cont};
    std::copy(std::begin(other), std::end(other), std::back_inserter(result));
    return result;
  });
}

template<template<typename...> typename R = std::vector, template<typename...> typename Blk = R, typename Size>
constexpr inline auto chunk(Size n) {
  assert(n > 0);
  return operation([n](auto &&cont) {
    R<Blk<remove_cr_t<decltype(*std::begin(cont))>>> result{};
    Blk<remove_cr_t<decltype(*std::begin(cont))>> block{};
    for (auto &&[i, v] : with_index(cont)) {
      block.push_back(v);
      if (i == std::size(cont) - 1) {
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

constexpr inline auto print() {
  return operation([](auto &&val) {
    whl::print(val);
    return val;
  });
}

constexpr inline auto println() {
  return operation([](auto &&val) {
    whl::println(val);
    return val;
  });
}

} // namespace whl::op

#endif // WHEEL_WHL_OPERATION_HPP
