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

#ifndef WHEEL_WHL_POINTER_HPP
#define WHEEL_WHL_POINTER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstddef>
#include <type_traits>

#include "whl/container.hpp"

namespace whl {

template<typename T>
struct unique_arr {
  public:
  using value_type = T;
  using element_type = value_type;
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
  unique_arr() : ptr(), size_() {}

  unique_arr(pointer ptr, size_type size) : ptr(ptr), size_(size) {}

  unique_arr(size_type size) : ptr(new value_type[size]()), size_(size) {}

  unique_arr(unique_arr &&arr) : ptr(arr.ptr), size_(arr.size_) {
    arr.ptr = pointer();
    arr.size_ = size_type();
  }

  unique_arr(std::initializer_list<value_type> il) : ptr(new value_type[il.size()]), size_(il.size()) {
    for_each_indexed(il, [this](auto &&i, auto &&v) {
      ptr[i] = v;
    });
  }

  ~unique_arr() {
    delete[] ptr;
  }

  unique_arr &operator=(unique_arr &&arr) {
    auto size = arr.size_;
    reset(arr.release(), size);
    return *this;
  }

  unique_arr &operator=(std::nullptr_t) {
    reset();
    return *this;
  }

  reference operator[](size_type i) {
    return ptr[i];
  }

  explicit operator bool() const noexcept {
    return ptr != nullptr;
  }

  pointer get() const noexcept {
    return ptr;
  }

  pointer release() noexcept {
    pointer tmp = ptr;
    ptr = pointer();
    size_ = size_type();
    return tmp;
  }

  void reset(pointer p, size_type size) noexcept {
    delete[] ptr;
    ptr = p;
    size_ = size;
  }

  void reset(std::nullptr_t = nullptr) noexcept {
    delete[] ptr;
    ptr = nullptr;
    size_ = size_type();
  }

  void swap(unique_arr &arr) noexcept {
    std::swap(ptr, arr.ptr);
    std::swap(size_, arr.size_);
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

#endif // WHEEL_WHL_POINTER_HPP
