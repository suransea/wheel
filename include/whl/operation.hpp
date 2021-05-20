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
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <optional>
#include <random>
#include <string>
#include <tuple>
#include <utility>

#include "whl/container.hpp"
#include "whl/format.hpp"
#include "whl/print.hpp"
#include "whl/sequence.hpp"
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
constexpr inline auto foreach (Fn fn) {
  return operation([fn](auto &&cont) {
    std::for_each(std::begin(cont), std::end(cont), fn);
    return cont;
  });
}

template<typename Fn>
constexpr inline auto foreach_indexed(Fn fn) {
  return operation([fn](auto &&cont) {
    whl::foreach_indexed(cont, fn);
    return cont;
  });
}

template<typename Iter, typename Fn>
struct map_iter {
  private:
  Iter iter;
  const Fn fn;

  public:
  using value_type = remove_cr_t<decltype(fn(std::declval<typename Iter::value_type>()))>;
  using pointer = value_type *;
  using reference = value_type &;
  using difference_type = typename Iter::difference_type;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr map_iter(Iter iter, Fn fn) : iter(iter), fn(fn){};

  value_type operator*() {
    return fn(*iter);
  }

  std::optional<value_type> operator->() {
    return **this;
  }

  map_iter &operator++() {
    ++iter;
    return *this;
  }

  map_iter operator++(int) {
    map_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const map_iter &it) {
    return !(*this == it);
  }

  bool operator==(const map_iter &it) {
    return iter == it.iter;
  }
};

template<typename Fn>
constexpr inline auto map(Fn fn) {
  return operation([fn](auto &&cont) {
    return sequence(map_iter(std::begin(cont), fn), map_iter(std::end(cont), fn));
  });
}

template<typename Iter>
struct flatten_iter {
  private:
  Iter iter;
  using iter_value_type = typename Iter::value_type;
  using inner_iter_type = typename iter_value_type::iterator;
  std::optional<iter_value_type> value{};
  std::optional<inner_iter_type> inner_iter{};

  public:
  using difference_type = typename inner_iter_type::difference_type;
  using value_type = typename inner_iter_type::value_type;
  using pointer = std::optional<value_type>;
  using reference = typename inner_iter_type::reference;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr flatten_iter(Iter iter) : iter(iter){};

  value_type operator*() {
    if (inner_iter) {
      return **inner_iter;
    }
    value = *iter;
    inner_iter = std::begin(*value);
    return **inner_iter;
  }

  flatten_iter &operator++() {
    if (inner_iter) {
      ++*inner_iter;
      if (inner_iter == std::end(*value)) {
        ++iter;
        value.reset();
        inner_iter.reset();
      }
    } else {
      value = *iter;
      inner_iter = std::begin(*value);
      ++*this;
    }
    return *this;
  }

  pointer operator->() {
    return **this;
  }

  flatten_iter operator++(int) {
    flatten_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const flatten_iter &it) {
    return !(*this == it);
  }

  bool operator==(const flatten_iter &it) {
    return iter == it.iter && inner_iter == it.inner_iter;
  }
};

constexpr inline auto flatten() {
  return operation([](auto &&cont) {
    return sequence(flatten_iter(std::begin(cont)), flatten_iter(std::end(cont)));
  });
}

template<typename Fn>
constexpr inline auto flat_map(Fn fn) {
  return operation([fn](auto &&cont) {
    return cont | map(fn) | flatten();
  });
}

template<template<typename...> typename C>
constexpr inline auto to() {
  return operation([](auto &&cont) {
    using value_type = remove_cr_t<decltype(*std::begin(cont))>;
    return C<value_type>(std::begin(cont), std::end(cont));
  });
}

template<template<typename...> typename C = std::vector>
constexpr inline auto collect() {
  return to<C>();
}

template<typename Iter, typename OtherIter, typename Fn>
struct zip_iter {
  private:
  Iter iter, iter_end;
  OtherIter other_iter, other_iter_end;
  const Fn zipper;

  public:
  using difference_type = typename Iter::difference_type;
  using value_type = remove_cr_t<decltype(zipper(std::declval<typename Iter::value_type>(),
                                                 std::declval<typename OtherIter::value_type>()))>;
  using pointer = std::optional<value_type>;
  using reference = value_type &;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr zip_iter(Iter iter, Iter end, OtherIter other, OtherIter other_end, Fn zipper)
      : iter(iter), iter_end(end), other_iter(other), other_iter_end(other_end), zipper(zipper){};

  constexpr zip_iter(Iter end, OtherIter other_end, Fn zipper)
      : iter(end), iter_end(end), other_iter(other_end), other_iter_end(other_end), zipper(zipper){};

  constexpr value_type operator*() {
    return zipper(*iter, *other_iter);
  }

  zip_iter &operator++() {
    ++iter;
    ++other_iter;
    if (other_iter == other_iter_end) {
      iter = iter_end;
    }
    return *this;
  }

  pointer operator->() {
    return **this;
  }

  zip_iter operator++(int) {
    zip_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const zip_iter &it) {
    return !(*this == it);
  }

  bool operator==(const zip_iter &it) {
    return iter == it.iter;
  }
};

template<typename Iter, typename C, typename Fn>
struct zip_sequence {
  public:
  using const_iterator = zip_iter<Iter, typename C::const_iterator, Fn>;
  using iterator = const_iterator;
  using value_type = typename iterator::value_type;
  using pointer = typename iterator::pointer;
  using reference = typename iterator::reference;
  using const_reference = const reference;
  using const_pointer = const pointer;
  using difference_type = typename iterator::difference_type;

  private:
  C other;
  Iter first, last;
  const Fn zipper;

  public:
  zip_sequence(Iter first, Iter last, const C &other, Fn zipper)
      : first(first), last(last), other(other), zipper(zipper) {}

  iterator begin() const {
    return zip_iter(first, last, std::begin(other), std::end(other), zipper);
  }

  iterator end() const {
    return zip_iter(last, std::end(other), zipper);
  }
};

template<typename C, typename Fn>
constexpr inline auto zip(const C &other, Fn fn) {
  return operation([other, fn](auto &&cont) {
    return zip_sequence(std::begin(cont), std::end(cont), other, fn);
  });
}

template<typename C>
constexpr inline auto zip(const C &other) {
  return zip(other, [](auto &&x, auto &&y) { return std::make_pair(x, y); });
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

template<typename Iter, typename Pred>
struct filter_iter {
  public:
  using difference_type = typename Iter::difference_type;
  using value_type = typename Iter::value_type;
  using pointer = typename Iter::pointer;
  using reference = typename Iter::reference;
  using iterator_category = std::input_iterator_tag;

  private:
  Iter iter, iter_end;
  const Pred pred;
  std::optional<value_type> value{};

  void eval_value() {
    if (iter == iter_end) return;
    if (value) return;
    value = *iter;
    if (!pred(*value)) {
      ++iter;
      value.reset();
      eval_value();
    }
  }

  public:
  constexpr filter_iter(Iter iter, Iter end, Pred pred)
      : iter(iter), iter_end(end), pred(pred){};

  constexpr filter_iter(Iter end, Pred pred)
      : iter(end), iter_end(end), pred(pred){};

  value_type operator*() {
    eval_value();
    return *value;
  }

  filter_iter &operator++() {
    ++iter;
    value.reset();
    eval_value();
    return *this;
  }

  pointer operator->() {
    eval_value();
    return std::addressof(*value);
  }

  filter_iter operator++(int) {
    filter_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const filter_iter &it) {
    return !(*this == it);
  }

  bool operator==(const filter_iter &it) {
    eval_value();
    return iter == it.iter;
  }
};

template<typename Pred>
constexpr inline auto filter(Pred pred) {
  return operation([pred](auto &&cont) {
    return sequence(filter_iter(std::begin(cont), std::end(cont), pred), filter_iter(std::end(cont), pred));
  });
}

template<template<typename...> typename C = std::vector, typename Eq>
constexpr inline auto distinct(Eq eq) {
  return operation([eq](auto &&cont) {
    using value_type = remove_cr_t<decltype(*std::begin(cont))>;
    auto result = C<value_type>();
    std::unique_copy(std::begin(cont), std::end(cont), std::back_inserter(result), eq);
    return result;
  });
}

template<template<typename...> typename C = std::vector>
constexpr inline auto distinct() {
  return distinct<C>(func::equal);
}

constexpr inline auto reverse() {
  return operation([](auto &&cont) {
    return sequence(std::rbegin(cont), std::rend(cont));
  });
}

template<template<typename...> typename C = std::vector, typename Comp>
constexpr inline auto sort(Comp comp) {
  return operation([comp](auto &&cont) {
    using value_type = remove_cr_t<decltype(*std::begin(cont))>;
    auto result = C<value_type>(std::begin(cont), std::end(cont));
    std::sort(std::begin(result), std::end(result), comp);
    return result;
  });
}

template<template<typename...> typename C = std::vector>
constexpr inline auto sort() {
  return sort<C>(func::less);
}

template<template<typename...> typename C = std::vector>
constexpr inline auto shuffle() {
  return operation([](auto &&cont) {
    using value_type = remove_cr_t<decltype(*std::begin(cont))>;
    auto result = C<value_type>(std::begin(cont), std::end(cont));
    auto &&time = std::chrono::system_clock::now().time_since_epoch().count();
    std::shuffle(std::begin(result), std::end(result), std::default_random_engine(time));
    return result;
  });
}

template<typename Size>
constexpr inline auto take(Size n) {
  return operation([n](auto &&cont) {
    auto &&begin = std::begin(cont);
    std::advance(begin, n);
    return sequence(std::begin(cont), begin);
  });
}

template<typename Size>
constexpr inline auto drop(Size n) {
  return operation([n](auto &&cont) {
    auto &&begin = std::begin(cont);
    std::advance(begin, n);
    return sequence(begin, std::end(cont));
  });
}

template<typename Val, typename BinOp>
constexpr inline auto fold(Val init, BinOp op) {
  return operation([init, op](auto &&cont) {
    return std::accumulate(std::begin(cont), std::end(cont), init, op);
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

template<typename CharT = char, typename Dlm>
constexpr inline auto join(const Dlm &delimiter) {
  return operation([delimiter](auto &&cont) {
    std::basic_ostringstream<CharT> out;
    auto &&it = std::begin(cont);
    if (it == std::end(cont)) {
      return out.str();
    }
    detail::out_to(out, *it++);
    for (; it != std::end(cont); ++it) {
      detail::out_to(out, delimiter, *it);
    }
    return out.str();
  });
}

template<typename Val = std::int32_t>
constexpr inline auto sum() {
  return fold(Val{}, func::plus);
}

template<typename Val = std::double_t>
constexpr inline auto average() {
  return operation([](auto &&cont) {
    return sum<Val>()(cont) / static_cast<Val>(std::distance(std::begin(cont), std::end(cont)));
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
    return std::distance(std::begin(cont), std::end(cont));
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

template<typename Iter, typename OtherIter>
struct concat_iter {
  private:
  Iter iter, iter_end;
  OtherIter other_iter;

  public:
  using difference_type = typename Iter::difference_type;
  using value_type = typename Iter::value_type;
  using pointer = std::optional<value_type>;
  using reference = typename Iter::reference;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr concat_iter(Iter iter, Iter end, OtherIter other)
      : iter(iter), iter_end(end), other_iter(other){};

  constexpr concat_iter(Iter end, OtherIter other_end)
      : iter(end), iter_end(end), other_iter(other_end){};

  constexpr value_type operator*() {
    return iter != iter_end ? *iter : *other_iter;
  }

  concat_iter &operator++() {
    if (iter != iter_end) {
      ++iter;
    } else {
      ++other_iter;
    }
    return *this;
  }

  pointer operator->() {
    return **this;
  }

  concat_iter operator++(int) {
    concat_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const concat_iter &it) {
    return !(*this == it);
  }

  bool operator==(const concat_iter &it) {
    return iter == it.iter && other_iter == it.other_iter;
  }
};

template<typename Iter, typename C>
struct concat_sequence {
  public:
  using const_iterator = concat_iter<Iter, typename C::const_iterator>;
  using iterator = const_iterator;
  using value_type = typename iterator::value_type;
  using pointer = typename iterator::pointer;
  using reference = typename iterator::reference;
  using const_reference = const reference;
  using const_pointer = const pointer;
  using difference_type = typename iterator::difference_type;

  private:
  C other;
  Iter first, last;

  public:
  concat_sequence(Iter first, Iter last, const C &other)
      : first(first), last(last), other(other) {}

  iterator begin() const {
    return concat_iter(first, last, std::begin(other));
  }

  iterator end() const {
    return concat_iter(last, std::end(other));
  }
};

template<typename C>
constexpr inline auto concat(const C &other) {
  return operation([other](auto &&cont) {
    return concat_sequence(std::begin(cont), std::end(cont), other);
  });
}

template<typename Iter>
struct chunk_iter {
  public:
  using difference_type = std::ptrdiff_t;
  using value_type = sequence<Iter>;
  using pointer = std::optional<value_type>;
  using reference = value_type &;
  using iterator_category = std::input_iterator_tag;

  private:
  Iter iter, iter_end;
  difference_type n;

  void advance(Iter &it) {
    for (difference_type i = n; i > 0; --i) {
      if (it == iter_end) break;
      ++it;
    }
  }

  public:
  constexpr chunk_iter(Iter iter, Iter end, difference_type n)
      : iter(iter), iter_end(end), n(n){};

  explicit constexpr chunk_iter(Iter end)
      : iter(end), iter_end(end), n(){};

  constexpr value_type operator*() {
    Iter end = iter;
    advance(end);
    return sequence(iter, end);
  }

  chunk_iter &operator++() {
    advance(iter);
    return *this;
  }

  pointer operator->() {
    return **this;
  }

  chunk_iter operator++(int) {
    chunk_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const chunk_iter &it) {
    return !(*this == it);
  }

  bool operator==(const chunk_iter &it) {
    return iter == it.iter;
  }
};

template<typename Size>
constexpr inline auto chunk(Size n) {
  assert(n > 0);
  return operation([n](auto &&cont) {
    return sequence(chunk_iter(std::begin(cont), std::end(cont), n), chunk_iter(std::end(cont)));
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
