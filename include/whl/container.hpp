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
#include <utility>

#include "whl/function.hpp"
#include "whl/literals.hpp"

namespace whl {

template<typename Size = std::size_t, typename Iter, typename Fn>
inline void for_each_indexed(Iter first, Iter last, Fn fn) {
  for (Size i{}; first != last; ++first, ++i) {
    fn(i, *first);
  }
}

template<typename C, typename Fn>
inline void for_each_indexed(const C &cont, Fn fn) {
  for_each_indexed<decltype(std::size(cont))>(std::begin(cont), std::end(cont), fn);
}

template<typename C>
struct with_index {
  const C &cont;

  template<typename Iter>
  struct iter_indexed {
    Iter it;
    decltype(std::size(cont)) index{};

    constexpr explicit iter_indexed(Iter it) : it(it) {}

    constexpr auto operator*() const {
      return std::make_pair(index, *it);
    }

    iter_indexed &operator++() {
      ++it;
      ++index;
      return *this;
    }

    bool operator!=(const iter_indexed &iter) const {
      return it != iter.it;
    }
  };

  constexpr explicit with_index(const C &cont) : cont(cont) {}

  constexpr auto begin() {
    return iter_indexed<decltype(std::begin(cont))>(std::begin(cont));
  }

  constexpr auto end() {
    return iter_indexed<decltype(std::end(cont))>(std::end(cont));
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
  array() : ptr(), size_() {}

  array(pointer ptr, size_type size) : ptr(ptr), size_(size) {}

  array(size_type size) : ptr(new value_type[size]()), size_(size) {}

  array(const array &arr) : array(arr.size_) {
    std::memcpy(ptr, arr.ptr, size_ * sizeof(value_type));
  }

  array(array &&arr) : ptr(arr.ptr), size_(arr.size_) {
    arr.ptr = pointer();
    arr.size_ = size_type();
  }

  array(std::initializer_list<value_type> il) : ptr(new value_type[il.size()]), size_(il.size()) {
    for_each_indexed(il, [this](auto &&i, auto &&v) {
      ptr[i] = v;
    });
  }

  ~array() {
    delete[] ptr;
  }

  array &operator=(const array &arr) {
    if (size_ < arr.size_) {
      delete[] ptr;
      ptr = new value_type[arr.size_];
    }
    size_ = arr.size_;
    std::memcpy(ptr, arr.ptr, size_ * sizeof(value_type));
    return *this;
  }

  array &operator=(array &&arr) {
    delete[] ptr;
    ptr = arr.ptr;
    size_ = arr.size_;
    arr.ptr = pointer();
    arr.size_ = size_type();
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

  iterator end() noexcept {
    return ptr + size_;
  }

  const_iterator end() const noexcept {
    return ptr + size_;
  }
};

} // namespace whl

#endif // WHEEL_WHL_CONTAINER_HPP
