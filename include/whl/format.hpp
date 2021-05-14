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

#ifndef WHEEL_WHL_FORMAT_HPP
#define WHEEL_WHL_FORMAT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

#include "whl/type.hpp"

namespace whl {

template<typename T, typename = void>
struct formatter {

  template<typename Out, typename FmtIter>
  void operator()(const T &arg, Out &out, FmtIter first, FmtIter last) {
    out << arg;
  }
};

namespace detail {

template<typename Out, typename FmtIter, typename Arg, typename... Args>
inline void format_to(Out &out, FmtIter first, FmtIter last, const Arg &arg, const Args &...args) {
  if (first == last) return;
  auto &&begin = std::find(first, last, '{');
  while (begin != last && begin + 1 != last && *(begin + 1) == '{') {
    begin = std::find(begin + 2, last, '{');
  }
  if (begin == last) {
    std::for_each(first, last, [&out](auto &&it) { out << it; });
    return;
  }
  auto &&end = std::find(begin + 1, last, '}');
  while (end != last && end + 1 != last && *(end + 1) == '}') {
    begin = std::find(end + 2, last, '}');
  }
  if (end == last) {
    std::for_each(first, last, [&out](auto &&it) { out << it; });
    return;
  }
  std::for_each(first, begin, [&out](auto &&it) { out << it; });
  formatter<Arg>()(arg, out, begin + 1, end);
  if constexpr (sizeof...(args) != 0) {
    if (end + 1 != last) format_to(out, end + 1, last, args...);
  } else {
    std::for_each(end + 1, last, [&out](auto &&it) { out << it; });
  }
}

template<typename Out, typename FmtIter, typename... Args>
inline void format_to(Out &out, FmtIter first, FmtIter last, const Args &...args) {
  if constexpr (sizeof...(args) == 0) {
    std::for_each(first, last, [&out](auto &&it) { out << it; });
  } else {
    format_to(out, first, last, args...);
  }
}

template<typename FmtIter, typename... Args>
inline auto format(FmtIter first, FmtIter last, const Args &...args) {
  using char_type = remove_cr_t<decltype(*first)>;
  std::basic_ostringstream<char_type> out{};
  format_to(out, first, last, args...);
  return out.str();
}

template<typename Out, typename... Args>
inline void out_to(Out &out, const Args &...args) {
  (..., (formatter<Args>()(args, out, nullptr, nullptr)));
}

template<typename Tuple, size_t... I>
inline void out_tuple_to(std::basic_ostream<char> &out, const Tuple &tuple, std::index_sequence<I...>) {
  out << '(';
  (..., out_to(out, I ? ", " : "", std::get<I>(tuple)));
  out << ')';
}

template<typename Tuple, size_t... I>
inline void out_tuple_to(std::basic_ostream<wchar_t> &out, const Tuple &tuple, std::index_sequence<I...>) {
  out << L'(';
  (..., out_to(out, I ? L", " : L"", std::get<I>(tuple)));
  out << L')';
}

} // namespace detail

template<typename Fmt, typename... Args>
inline auto format(const Fmt &fmt, const Args &...args) {
  return detail::format(std::begin(fmt), std::end(fmt), args...);
}

template<typename CharT = char, typename... Args>
inline auto to_string(const Args &...args) {
  std::basic_ostringstream<CharT> out{};
  detail::out_to(out, args...);
  return out.str();
}

template<>
struct formatter<std::string> {

  template<typename FmtIter>
  void operator()(const std::string &arg, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    out << arg;
  }

  template<typename FmtIter>
  void operator()(const std::string &arg, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    std::wstring str(arg.length(), '\0');
    std::transform(arg.begin(), arg.end(), str.begin(), [](auto &&ch) { return static_cast<wchar_t>(ch); });
    out << str;
  }
};

template<>
struct formatter<std::wstring> {

  template<typename FmtIter>
  void operator()(const std::wstring &arg, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    std::string str(arg.length(), '\0');
    std::transform(arg.begin(), arg.end(), str.begin(), [](auto &&ch) { return static_cast<char>(ch); });
    out << str;
  }

  template<typename FmtIter>
  void operator()(const std::wstring &arg, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    out << arg;
  }
};

template<>
struct formatter<const char *> {

  template<typename FmtIter>
  void operator()(const char *arg, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    out << arg;
  }
};

template<>
struct formatter<const wchar_t *> {

  template<typename FmtIter>
  void operator()(const wchar_t *arg, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    out << arg;
  }
};

template<>
struct formatter<bool> {

  template<typename FmtIter>
  void operator()(const bool arg, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    out << (arg ? "true" : "false");
  }

  template<typename FmtIter>
  void operator()(const bool arg, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    out << (arg ? L"true" : L"false");
  }
};

template<typename T1, typename T2>
struct formatter<std::pair<T1, T2>> {

  template<typename FmtIter>
  void operator()(const std::pair<T1, T2> &pair, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    detail::out_to(out, '(', pair.first, ", ", pair.second, ')');
  }

  template<typename FmtIter>
  void operator()(const std::pair<T1, T2> &pair, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    detail::out_to(out, L'(', pair.first, L", ", pair.second, L')');
  }
};

template<typename... Args>
struct formatter<std::tuple<Args...>> {

  template<typename Out, typename FmtIter>
  void operator()(const std::tuple<Args...> &tuple, Out &out, FmtIter first, FmtIter last) {
    detail::out_tuple_to(out, tuple, std::make_index_sequence<sizeof...(Args)>());
  }
};

template<typename T>
struct formatter<T, decltype(std::begin(std::declval<T>()), void())> {

  template<typename FmtIter>
  void operator()(const T &arg, std::basic_ostream<char> &out, FmtIter first, FmtIter last) {
    auto &&it = std::begin(arg);
    if (it == std::end(arg)) {
      out << "[]";
      return;
    }
    detail::out_to(out, '[', *it);
    for (; it != std::end(arg); ++it) {
      detail::out_to(out, ", ", *it);
    }
    out << ']';
  }

  template<typename FmtIter>
  void operator()(const T &arg, std::basic_ostream<wchar_t> &out, FmtIter first, FmtIter last) {
    auto &&it = std::begin(arg);
    if (it == std::end(arg)) {
      out << L"[]";
      return;
    }
    detail::out_to(out, L'[', *it);
    for (; it != std::end(arg); ++it) {
      detail::out_to(out, L", ", *it);
    }
    out << L']';
  }
};

} // namespace whl

#endif // WHEEL_WHL_FORMAT_HPP
