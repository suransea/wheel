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
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <iterator>
#include <string>
#include <regex>
#include <numeric>
#include <vector>

namespace whl::str {

template<typename C=std::vector<std::string>, typename Iter, typename Regex>
constexpr inline C split(Iter first, Iter last, const Regex &regex) {
  std::basic_regex basic_regex(regex);
  return C{std::regex_token_iterator(first, last, basic_regex, -1),
           std::regex_token_iterator<decltype(first)>()};
}

template<typename C=std::vector<std::string>, typename Str, typename Regex>
constexpr inline C split(const Str &str, const Regex &regex) {
  return split<C>(std::begin(str), std::end(str), regex);
}

template<typename Str=std::string, typename Iter, typename Dlm>
constexpr inline Str join(Iter first, Iter last, const Dlm &delimiter) {
  Str result = *first;
  return std::accumulate(++first, last, result, [&delimiter](auto &&x, auto &&y) {
    return x + delimiter + y;
  });
}

template<typename Str=std::string, typename C, typename Dlm>
constexpr inline Str join(const C &coll, const Dlm &delimiter) {
  return join<Str>(std::begin(coll), std::end(coll), delimiter);
}

template<typename Iter>
inline void toupper_inplace(Iter first, Iter last) {
  std::transform(first, last, first, [](auto ch) -> auto { return std::toupper(ch); });
}

template<typename Str>
inline void toupper_inplace(Str &str) {
  toupper_inplace(std::begin(str), std::end(str));
}

template<typename Iter>
inline void tolower_inplace(Iter first, Iter last) {
  std::transform(first, last, first, [](auto ch) -> auto { return std::tolower(ch); });
}

template<typename Str>
inline void tolower_inplace(Str &str) {
  tolower_inplace(std::begin(str), std::end(str));
}

template<typename Str_R=std::string, typename Str_In>
constexpr inline Str_R toupper(const Str_In &str) {
  Str_R result{str};
  toupper_inplace(result);
  return result;
}

template<typename Str=std::string, typename Iter>
constexpr inline Str toupper(Iter first, Iter last) {
  Str result{first, last};
  toupper_inplace(result);
  return result;
}

template<typename Str_R=std::string, typename Str_In>
constexpr inline Str_R tolower(const Str_In &str) {
  Str_R result{str};
  tolower_inplace(result);
  return result;
}

template<typename Str=std::string, typename Iter>
constexpr inline Str tolower(Iter first, Iter last) {
  Str result{first, last};
  tolower_inplace(result);
  return result;
}

} // namespace whl::str

#endif // WHEEL_WHL_STRING_HPP
