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

#ifndef WHEEL_WHL_STRING_HPP
#define WHEEL_WHL_STRING_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <iterator>
#include <numeric>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

#include "whl/operation.hpp"
#include "whl/sequence.hpp"

namespace whl::str {

template<typename Iter>
struct split_sequence {
  public:
  using const_iterator = std::regex_token_iterator<Iter>;
  using iterator = const_iterator;
  using value_type = typename iterator::value_type;
  using pointer = typename iterator::pointer;
  using reference = typename iterator::reference;
  using const_reference = const reference;
  using const_pointer = const pointer;
  using difference_type = typename iterator::difference_type;

  private:
  Iter first, last;
  using regex_type = std::basic_regex<typename std::iterator_traits<Iter>::value_type>;
  const regex_type regex;

  public:
  split_sequence(Iter first, Iter last, regex_type regex)
      : first{first}, last{last}, regex(std::move(regex)) {}

  const_iterator begin() const {
    return {first, last, regex, -1};
  }

  const_iterator end() const {
    return {};
  }

  const_iterator cbegin() const {
    return begin();
  }

  const_iterator cend() const {
    return end();
  }
};

template<typename Iter, typename Regex>
constexpr inline auto split(Iter first, Iter last, const Regex &regex) {
  return split_sequence{first, last, std::basic_regex(regex)};
}

template<typename Str, typename Regex>
constexpr inline auto split(const Str &str, const Regex &regex) {
  auto sv = std::basic_string_view(str);
  return split(std::begin(sv), std::end(sv), regex);
}

template<typename CharT = char, typename Iter, typename Dlm>
constexpr inline auto join(Iter first, Iter last, const Dlm &delimiter) {
  return sequence{first, last} | op::join<CharT>(delimiter);
}

template<typename CharT = char, typename C, typename Dlm>
constexpr inline auto join(const C &cont, const Dlm &delimiter) {
  return cont | op::join<CharT>(delimiter);
}

template<typename Iter>
inline void toupper_inplace(Iter first, Iter last) {
  std::transform(
      first, last, first, [](auto ch) -> auto{ return std::toupper(ch); });
}

template<typename Str>
inline void toupper_inplace(Str &str) {
  toupper_inplace(std::begin(str), std::end(str));
}

template<typename Iter>
inline void tolower_inplace(Iter first, Iter last) {
  std::transform(
      first, last, first, [](auto ch) -> auto{ return std::tolower(ch); });
}

template<typename Str>
inline void tolower_inplace(Str &str) {
  tolower_inplace(std::begin(str), std::end(str));
}

template<typename Str>
constexpr inline auto toupper(const Str &str) {
  auto result = std::basic_string(str);
  toupper_inplace(result);
  return result;
}

template<typename Iter>
constexpr inline auto toupper(Iter first, Iter last) {
  auto result = std::basic_string(first, last);
  toupper_inplace(result);
  return result;
}

template<typename Str>
constexpr inline auto tolower(const Str &str) {
  auto result = std::basic_string(str);
  tolower_inplace(result);
  return result;
}

template<typename Iter>
constexpr inline auto tolower(Iter first, Iter last) {
  auto result = std::basic_string(first, last);
  tolower_inplace(result);
  return result;
}

} // namespace whl::str

#endif // WHEEL_WHL_STRING_HPP
