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

  unique_arr(std::nullptr_t) : unique_arr() {}

  unique_arr(pointer ptr, size_type size) : ptr(ptr), size_(size) {}

  unique_arr(size_type size) : ptr(new value_type[size]()), size_(size) {}

  unique_arr(unique_arr &&arr) : ptr(arr.ptr), size_(arr.size_) {
    arr.ptr = pointer();
    arr.size_ = size_type();
  }

  unique_arr(std::initializer_list<value_type> il) : ptr(new value_type[il.size()]), size_(il.size()) {
    foreach_indexed(il, [this](auto &&i, auto &&v) {
      new (ptr + i) value_type(std::move(v));
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
    ptr = pointer();
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

template<typename>
struct weak_arr;

template<typename T>
struct shared_arr {
  template<typename>
  friend struct weak_arr;

  public:
  using value_type = T;
  using element_type = value_type;
  using weak_type = weak_arr<value_type>;
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
  long *ref_count;

  public:
  shared_arr() : ptr(), size_(), ref_count() {}

  shared_arr(std::nullptr_t) : shared_arr() {}

  shared_arr(pointer ptr, size_type size) : ptr(ptr), size_(size), ref_count(new long(1)) {}

  shared_arr(size_type size) : ptr(new value_type[size]()), size_(size), ref_count(new long(1)) {}

  shared_arr(const shared_arr &arr) : ptr(arr.ptr), size_(arr.size_), ref_count(arr.ref_count) {
    if (ref_count) ++(*ref_count);
  }

  shared_arr(const weak_type &arr);

  shared_arr(shared_arr &&arr) : ptr(arr.ptr), size_(arr.size_), ref_count(arr.ref_count) {
    arr.ptr = pointer();
    arr.size_ = size_type();
    arr.ref_count = nullptr;
  }

  shared_arr(unique_arr<value_type> &&arr) : ptr(arr.get()), size_(arr.size()) {
    ref_count = arr ? new long(1) : nullptr;
    arr.release();
  }

  shared_arr(std::initializer_list<value_type> il) : ptr(new value_type[il.size()]), size_(il.size()), ref_count(new long(1)) {
    foreach_indexed(il, [this](auto &&i, auto &&v) {
      new (ptr + i) value_type(std::move(v));
    });
  }

  ~shared_arr() {
    if (!ref_count) {
      return;
    }
    --(*ref_count);
    if (*ref_count == 0) {
      delete[] ptr;
      delete ref_count;
    }
  }

  shared_arr &operator=(const shared_arr &arr) {
    shared_arr(arr).swap(*this);
    return *this;
  }

  shared_arr &operator=(shared_arr &&arr) {
    shared_arr(std::move(arr)).swap(*this);
    return *this;
  }

  shared_arr &operator=(unique_arr<value_type> &&arr) {
    shared_arr(std::move(arr)).swap(*this);
    return *this;
  }

  shared_arr &operator=(std::nullptr_t) {
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

  void reset(pointer p, size_type size) noexcept {
    shared_arr(p, size).swap(*this);
  }

  void reset(std::nullptr_t = nullptr) noexcept {
    shared_arr().swap(*this);
  }

  void swap(shared_arr &arr) noexcept {
    std::swap(ptr, arr.ptr);
    std::swap(size_, arr.size_);
    std::swap(ref_count, arr.ref_count);
  }

  long use_count() const noexcept {
    return ref_count ? *ref_count : 0;
  }

  long unique() const noexcept {
    return use_count() == 1;
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

template<typename T>
struct weak_arr {
  template<typename>
  friend struct shared_arr;

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
  long *ref_count;

  public:
  weak_arr() : ptr(), size_(), ref_count() {}

  weak_arr(std::nullptr_t) : weak_arr() {}

  weak_arr(pointer ptr, size_type size) : ptr(ptr), size_(size), ref_count() {}

  weak_arr(const weak_arr &arr) : ptr(arr.ptr), size_(arr.size_), ref_count(arr.ref_count) {}

  weak_arr(weak_arr &&arr) : ptr(arr.ptr), size_(arr.size_), ref_count(arr.ref_count) {
    arr.ptr = pointer();
    arr.size_ = size_type();
    arr.ref_count = nullptr;
  }

  weak_arr(const shared_arr<value_type> &arr) : ptr(arr.get()), size_(arr.size()), ref_count(arr.ref_count) {}

  ~weak_arr() = default;

  weak_arr &operator=(const weak_arr &arr) {
    weak_arr(arr).swap(*this);
    return *this;
  }

  weak_arr &operator=(weak_arr &&arr) {
    weak_arr(std::move(arr)).swap(*this);
    return *this;
  }

  weak_arr &operator=(const shared_arr<value_type> &arr) {
    weak_arr(arr).swap(*this);
    return *this;
  }

  weak_arr &operator=(std::nullptr_t) {
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

  void reset(pointer p, size_type size) noexcept {
    weak_arr(p, size).swap(*this);
  }

  void reset(std::nullptr_t = nullptr) noexcept {
    weak_arr().swap(*this);
  }

  void swap(weak_arr &arr) noexcept {
    std::swap(ptr, arr.ptr);
    std::swap(size_, arr.size_);
    std::swap(ref_count, arr.ref_count);
  }

  long use_count() const noexcept {
    return ref_count ? *ref_count : 0;
  }

  bool expired() const noexcept {
    return use_count() == 0;
  }

  shared_arr<value_type> lock() const noexcept {
    return shared_arr<value_type>(*this);
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

template<typename T>
shared_arr<T>::shared_arr(const shared_arr<T>::weak_type &arr) : ptr(arr.ptr), size_(arr.size_), ref_count(arr.ref_count) {
  if (ref_count) ++(*ref_count);
}

} // namespace whl

#endif // WHEEL_WHL_POINTER_HPP
