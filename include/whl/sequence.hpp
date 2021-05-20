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

#ifndef WHEEL_WHL_SEQUENCE_HPP
#define WHEEL_WHL_SEQUENCE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <iterator>
#include <optional>
#include <type_traits>

namespace whl {

template<typename Iter>
struct sequence {
  using const_iterator = Iter;
  using iterator = const_iterator;
  using value_type = typename Iter::value_type;
  using pointer = typename Iter::pointer;
  using reference = typename Iter::reference;
  using const_reference = const reference;
  using const_pointer = const pointer;
  using difference_type = typename Iter::difference_type;

  protected:
  const iterator first, last;

  public:
  constexpr sequence(iterator first, iterator last) : first(first), last(last) {}

  constexpr iterator begin() const noexcept {
    return first;
  }

  constexpr iterator end() const noexcept {
    return last;
  }
};

template<typename Val>
struct range_iter {
  private:
  Val value;

  public:
  using value_type = Val;
  using pointer = value_type *;
  using reference = value_type &;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr range_iter(Val value) : value(value){};

  value_type operator*() {
    return value;
  }

  pointer operator->() {
    return std::addressof(value);
  }

  range_iter &operator++() {
    ++value;
    return *this;
  }

  range_iter operator++(int) {
    range_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const range_iter &it) {
    return !(*this == it);
  }

  bool operator==(const range_iter &it) {
    return value == it.value;
  }
};

template<typename Val>
constexpr inline auto range(Val begin, Val end) {
  return sequence(range_iter(begin), range_iter(end));
}

template<typename Val, typename Fn>
struct generator_iter {
  private:
  std::optional<Val> value;
  const Fn generate;

  public:
  using value_type = Val;
  using pointer = std::optional<Val>;
  using reference = value_type &;
  using difference_type = std::ptrdiff_t;
  using iterator_category = std::input_iterator_tag;

  public:
  constexpr generator_iter(Val value, Fn generate) : value(value), generate(generate){};

  constexpr generator_iter(Fn generate) : value(), generate(generate){};

  value_type operator*() {
    return *value;
  }

  pointer operator->() {
    return value;
  }

  generator_iter &operator++() {
    value = generate(*value);
    return *this;
  }

  generator_iter operator++(int) {
    generator_iter it = *this;
    ++*this;
    return it;
  }

  bool operator!=(const generator_iter &it) {
    return !(*this == it);
  }

  bool operator==(const generator_iter &it) {
    return value == it.value;
  }
};

template<typename Val, typename Fn>
constexpr inline auto generate(Val init, Fn generate) {
  return sequence(generator_iter(init, generate), generator_iter<Val, Fn>(generate));
}

} // namespace whl

#endif // WHEEL_WHL_SEQUENCE_HPP
