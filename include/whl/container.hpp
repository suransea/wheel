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

#ifndef WHEEL_WHL_CONTAINER_HPP
#define WHEEL_WHL_CONTAINER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cctype>
#include <iterator>
#include <type_traits>
#include <utility>

#include "whl/function.hpp"
#include "whl/literals.hpp"

namespace whl {

template<typename Index = std::size_t, typename Iter, typename Fn>
inline void foreach_indexed(Iter first, Iter last, Fn fn) {
  for (auto i = Index{}; first != last; ++first, ++i) {
    fn(i, *first);
  }
}

template<typename C, typename Fn>
inline void foreach_indexed(const C &cont, Fn fn) {
  foreach_indexed<decltype(std::size(cont))>(std::begin(cont), std::end(cont), fn);
}

template<typename C, typename Fn>
inline void foreach_indexed(C &cont, Fn fn) {
  foreach_indexed<decltype(std::size(cont))>(std::begin(cont), std::end(cont), fn);
}

template<typename C>
struct with_index {
  const C &cont;

  template<typename Iter>
  struct iter_indexed {
    decltype(std::size(cont)) index{};
    Iter it;

    constexpr explicit iter_indexed(Iter it) : it(it) {}

    constexpr auto operator*() const {
      return std::make_pair(index, *it);
    }

    iter_indexed &operator++() {
      ++index;
      ++it;
      return *this;
    }

    bool operator!=(const iter_indexed &iter) const {
      return it != iter.it;
    }
  };

  constexpr explicit with_index(const C &cont) : cont(cont) {}

  using iterator = iter_indexed<decltype(std::cbegin(cont))>;

  constexpr iterator begin() const {
    return iterator{std::cbegin(cont)};
  }

  constexpr iterator end() const {
    return iterator{std::cend(cont)};
  }

  constexpr iterator cbegin() const {
    return begin();
  }

  constexpr iterator cend() const {
    return end();
  }
};

template<typename T>
struct array {
  public:
  using value_type = T;
  using reference = T &;
  using const_reference = const T &;
  using pointer = T *;
  using const_pointer = const T *;
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  using iterator = pointer;
  using const_iterator = const iterator;

  private:
  pointer ptr;
  size_type size_;

  public:
  array() : ptr{}, size_{} {}

  array(pointer ptr, size_type size) : ptr{ptr}, size_{size} {}

  array(const array &arr) : ptr{new value_type[arr.size_]}, size_{arr.size_} {
    for (auto i = size_type{}; i < size_; ++i) {
      new (ptr + i) value_type(arr[i]);
    }
  }

  array(array &&arr) : ptr{arr.ptr}, size_{arr.size_} {
    arr.ptr = pointer{};
    arr.size_ = size_type{};
  }

  array(std::initializer_list<value_type> il) : ptr(new value_type[il.size()]), size_(il.size()) {
    foreach_indexed(il, [this](auto i, auto &&v) {
      new (ptr + i) value_type(std::move(v));
    });
  }

  ~array() {
    delete[] ptr;
  }

  array &operator=(const array &arr) {
    delete[] ptr;
    size_ = arr.size_;
    ptr = new value_type[size_];
    for (auto i = size_type{}; i < size_; ++i) {
      new (ptr + i) value_type(arr[i]);
    }
    return *this;
  }

  array &operator=(array &&arr) {
    delete[] ptr;
    ptr = arr.ptr;
    size_ = arr.size_;
    arr.ptr = pointer{};
    arr.size_ = size_type{};
    return *this;
  }

  reference operator[](size_type i) {
    return ptr[i];
  }

  size_type size() const noexcept {
    return size_;
  }

  iterator begin() noexcept {
    return ptr;
  }

  const_iterator begin() const noexcept {
    return ptr;
  }

  const_iterator cbegin() const noexcept {
    return begin();
  }

  iterator end() noexcept {
    return ptr + size_;
  }

  const_iterator end() const noexcept {
    return ptr + size_;
  }

  const_iterator cend() const noexcept {
    return end();
  }
};

} // namespace whl

#endif // WHEEL_WHL_CONTAINER_HPP
